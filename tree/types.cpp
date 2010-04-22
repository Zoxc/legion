#include "types.hpp"
#include "../compiler.hpp"
#include "../document.hpp"
#include "symbols.hpp"

namespace Legion
{
	Type *lookup_type(ValidationArgs &args, String *name, Range *range)
	{
		bool found;

		Symbol *symbol = args.scope->lookup(name, &args.document, Symbol::TYPE, found);

		if(found)
		{
			if(symbol)
				return symbol->node->get_type(args);
			else
				return 0;
		}

		new WrongSymbolError(args.document, *range, symbol, name, Symbol::TYPE);

		return 0;
	}

	Type *TypeNode::validate(ValidationArgs &args)
	{
		Type *result;
		
		if(name)
			result = lookup_type(args, name, range);
		else
			result = 0;

		if(result)
		{
			for(TypeModifierList::Iterator i = modifiers.begin(); i; i++)
			{
				switch((*i)->node_type())
				{
					case TYPE_POINTER_NODE:
						result = result->get_indirect(args);
						break;

					case TYPE_ARRAY_NODE:
						result = result->get_array(args, 0); //TODO: Get the real size
						break;

					default:
						break;
				}
			}

			return result;
		}
		else
			return 0;
	}

	Type *Type::resolve(Type *type)
	{
		while(type && type->kind == Type::TYPEDEF_TYPE)
			type = ((TypedefType *)type)->base;

		return type;
	}

	Type *Type::get_indirect(ValidationArgs &args)
	{
		if(!indirect)
			indirect = new (args.memory_pool) PointerType(this); 

		return indirect;
	}
	
	Type *Type::get_array(ValidationArgs &args, size_t size)
	{
		for(auto i = arrays.begin(); i; i++)
			if(i().size == size)
				return *i;

		ArrayType *array_type = new (args.memory_pool) ArrayType(this, size); 

		arrays.append(array_type);

		return array_type;
	}

	void Type::compatible(ValidationArgs &args, Type *type, ExpressionNode *node)
	{
		if(!this || !type)
			return;

		if(!compatible(args, type, WEAK))
			new IncompatableTypesError(args.document, node->get_range(args.memory_pool), type, this);
	}

	void Type::compatible(ValidationArgs &args, ExpressionNode *node)
	{
		compatible(args, node->validate(args), node);
	}

	Type *CompositeType::get_member(ValidationArgs &args, String *name, Range &range)
	{
		for(auto i = fields.begin(); i; i++)
		{
			if(i().name == name)
				return i().type;
		}

		args.document.report(range, "Unable to find member '" + name->string() + "' in type '" +  string() + "'");
		
		return 0;
	}

	bool PointerType::compatible(ValidationArgs &args, Type *other, Compatibility compatibility)
	{
		if(!base)
			return true;

		if(compatibility == EXACT)
			return this == other;

		Type *type = resolve(other);

		if(this == type || type == args.types.type_null.type)
			return true;
		
		if(type->kind != POINTER_TYPE)
			return false;
		
		return base->compatible(args, ((PointerType *)type)->base, TYPEDEF);
	}

	void Types::declare(TypeNativeNode *base, const char *name, TypeNativeNode &type, bool declare)
	{
		TypeSymbol *result = new (compiler.memory_pool) TypeSymbol;
		result->range = 0;
		result->name = compiler.string_pool.get(name);
		result->node = &type;

		type.type = new NativeType(compiler.memory_pool, base ? base->type : 0);
		type.type->name = result->name;

		if(declare)
			compiler.scope.declare_symbol(result);
	}

	void Types::declare_operators(TypeNativeNode &type, bool arithmetic, bool bitwise)
	{
		if(arithmetic)
		{
			type.type->unary_operators.add(Lexeme::ADD, type.type);
			type.type->unary_operators.add(Lexeme::SUB, type.type);

			type.type->binary_operators.add(Lexeme::ADD, type.type);
			type.type->binary_operators.add(Lexeme::SUB, type.type);
			type.type->binary_operators.add(Lexeme::MUL, type.type);
			type.type->binary_operators.add(Lexeme::DIV, type.type);
			type.type->binary_operators.add(Lexeme::MOD, type.type);

			type.type->binary_operators.add(Lexeme::GREATER, type_bool.type);
			type.type->binary_operators.add(Lexeme::GREATER_OR_EQUAL, type_bool.type);
			type.type->binary_operators.add(Lexeme::LESS, type_bool.type);
			type.type->binary_operators.add(Lexeme::LESS_OR_EQUAL, type_bool.type);
		}

		if(bitwise)
		{
			type.type->unary_operators.add(Lexeme::BITWISE_NOT, type.type);

			type.type->binary_operators.add(Lexeme::BITWISE_AND, type.type);
			type.type->binary_operators.add(Lexeme::BITWISE_OR, type.type);
			type.type->binary_operators.add(Lexeme::BITWISE_XOR, type.type);
			type.type->binary_operators.add(Lexeme::LEFT_SHIFT, type.type);
			type.type->binary_operators.add(Lexeme::RIGHT_SHIFT, type.type);
		}
	}

	Types::Types(Compiler &compiler) : compiler(compiler)
	{
		declare(0, "null", type_null, false);
		declare(0, "void", type_void, false);

		declare(0, "nullable", type_nullable, false);
		type_nullable.type->add_conversion(type_null.type, true);

		declare(&type_nullable, "handle", type_handle, true);
		declare(&type_handle, "string", type_string, true);
		declare(&type_handle, "text", type_text, true);
		declare(0, "bool", type_bool, true);
		declare(0, "byte", type_byte, true);
		declare(0, "char", type_char, true);
		declare(0, "fixed", type_fixed, true);
		declare(0, "int", type_int, true);
		
		type_bool.type->unary_operators.add(Lexeme::LOGICAL_NOT, type_bool.type);

		type_bool.type->binary_operators.add(Lexeme::LOGICAL_AND, type_bool.type);
		type_bool.type->binary_operators.add(Lexeme::LOGICAL_OR, type_bool.type);
		
		declare_operators(type_int);
		declare_operators(type_bool);
		declare_operators(type_fixed, true, false);
		
		type_fixed.type->add_conversion(type_int.type, false);
		
		type_byte.type->add_conversion(type_int.type, false);
		type_int.type->add_conversion(type_byte.type, false);

		type_string.type->binary_operators.add(Lexeme::ADD, type_string.type);

		type_text.type->binary_operators.add(Lexeme::ADD, type_text.type);
		
		declare(0, "color", type_color, true);

		declare(&type_nullable, "doodad", type_doodad, true);
		declare(&type_nullable, "revealer", type_revealer, true);
		declare(&type_nullable, "sound", type_sound, true);
		declare(&type_nullable, "trigger", type_trigger, true);
		declare(&type_nullable, "unit", type_unit, true);
		declare(&type_nullable, "wave", type_wave, true);

		declare(&type_handle, "abilcmd", type_abilcmd, true);
		declare(&type_handle, "actor", type_actor, true);
		declare(&type_handle, "actorscope", type_actorscope, true);
		declare(&type_handle, "aifilter", type_aifilter, true);
		declare(&type_handle, "bank", type_bank, true);
		declare(&type_handle, "camerainfo", type_camerainfo, true);
		declare(&type_handle, "marker", type_marker, true);
		declare(&type_handle, "order", type_order, true);
		declare(&type_handle, "playergroup", type_playergroup, true);
		declare(&type_handle, "point", type_point, true);
		declare(&type_handle, "region", type_region, true);
		declare(&type_handle, "soundlink", type_soundlink, true);
		declare(&type_handle, "timer", type_timer, true);
		declare(&type_handle, "transmissionsource", type_transmissionsource, true);
		declare(&type_handle, "unitfilter", type_unitfilter, true);
		declare(&type_handle, "unitgroup", type_unitgroup, true);
		declare(&type_handle, "unitref", type_unitref, true);
		declare(&type_handle, "waveinfo", type_waveinfo, true);
		declare(&type_handle, "wavetarget", type_wavetarget, true);
	}
};