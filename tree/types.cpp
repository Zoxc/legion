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
		while(type->kind == Type::TYPEDEF_TYPE)
			type = ((TypedefType *)type)->base;

		return type;
	}

	Type *Type::get_indirect(ValidationArgs &args)
	{
		if(!indirect)
			indirect = new (args.memory_pool) PointerType(this); 

		return indirect;
	}

	void Type::compatible(ValidationArgs &args, Type *type, ExpressionNode *node)
	{
		if(!this || !type)
			return;

		if(!compatible(args, type, true))
			new IncompatableTypesError(args.document, node->get_range(args.memory_pool), type, this);
	}

	void Type::compatible(ValidationArgs &args, ExpressionNode *node)
	{
		compatible(args, node->validate(args), node);
	}

	bool PointerType::compatible(ValidationArgs &args, Type *other, bool weak)
	{
		if(!base)
			return true;

		return this == other || other == args.types.type_null.type || base->compatible(args, other, weak);
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

	Types::Types(Compiler &compiler) : compiler(compiler)
	{
		declare(0, "null", type_null, false);
		declare(0, "void", type_void, false);

		declare(0, "handle", type_handle, true);
		declare(0, "string", type_string, true);
		declare(0, "text", type_text, true);
		declare(0, "bool", type_bool, true);
		declare(0, "byte", type_byte, true);
		declare(0, "char", type_char, true);
		declare(0, "fixed", type_fixed, true);
		declare(0, "int", type_int, true);

		type_fixed.type->add_conversion(type_int.type, false);

		type_handle.type->add_conversion(type_null.type, true);
		type_string.type->add_conversion(type_null.type, true);
		type_text.type->add_conversion(type_null.type, true);

		declare(&type_handle, "abilcmd", type_abilcmd, true);
		declare(&type_handle, "actor", type_actor, true);
		declare(&type_handle, "actorscope", type_actorscope, true);
		declare(&type_handle, "aifilter", type_aifilter, true);
		declare(&type_handle, "animtarget", type_animtarget, true);
		declare(&type_handle, "bank", type_bank, true);
		declare(&type_handle, "camerainfo", type_camerainfo, true);
		declare(&type_handle, "color", type_color, true);
		declare(&type_handle, "doodad", type_doodad, true);
		declare(&type_handle, "marker", type_marker, true);
		declare(&type_handle, "order", type_order, true);
		declare(&type_handle, "playergroup", type_playergroup, true);
		declare(&type_handle, "point", type_point, true);
		declare(&type_handle, "region", type_region, true);
		declare(&type_handle, "revealer", type_revealer, true);
		declare(&type_handle, "sound", type_sound, true);
		declare(&type_handle, "soundlink", type_soundlink, true);
		declare(&type_handle, "timer", type_timer, true);
		declare(&type_handle, "transmissionsource", type_transmissionsource, true);
		declare(&type_handle, "trigger", type_trigger, true);
		declare(&type_handle, "unit", type_unit, true);
		declare(&type_handle, "unitfilter", type_unitfilter, true);
		declare(&type_handle, "unitgroup", type_unitgroup, true);
		declare(&type_handle, "unitref", type_unitref, true);
		declare(&type_handle, "wave", type_wave, true);
		declare(&type_handle, "waveinfo", type_waveinfo, true);
		declare(&type_handle, "wavetarget", type_wavetarget, true);
	}
};