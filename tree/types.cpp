#include "types.hpp"
#include "../compiler.hpp"
#include "../document.hpp"
#include "symbols.hpp"

namespace Legion
{
	Type *lookup_type(Document &document, SymbolList &stack, String *name, Range *range)
	{
		Symbol *symbol = document.scope->lookup(name);

		if(!symbol || symbol->type != Symbol::TYPE)
		{
			range->report_expected_symbol(document, symbol, name, Symbol::TYPE);

			return 0;
		}

		return ((TypeSymbol *)symbol)->node->get_type(document, stack);
	}

	Type *TypeNode::get_type(Document &document, SymbolList &stack)
	{
		Type *result = lookup_type(document, stack, name, range);

		if(result)
		{
			for(TypeModifierList::Iterator i = modifiers.begin(); i; i++)
			{
				switch((*i)->node_type())
				{
					case TYPE_POINTER_NODE:
						result = result->get_indirect(document);

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

	Type *Type::get_indirect(Document &document)
	{
		if(!indirect)
			indirect = new (document.memory_pool) PointerType(this); 

		return indirect;
	}

	void Type::compitable(Document &document, SymbolList &stack, ExpressionNode *node)
	{
		Type *type = node->get_type(document, stack);

		if(!this || !type)
			return;

		if(!compitable(type))
			node->get_range().report(document, "Unable to convert type '" + type->string() + "' to type '" + this->string() + "'");
	}

	void Types::declare(Compiler &compiler, const char *name, TypeNativeNode &type, bool declare)
	{
		TypeSymbol *result = new (compiler.memory_pool) TypeSymbol;
		result->range = 0;
		result->name = compiler.string_pool.get(name);
		result->node = &type;
		type.type.name = result->name;

		if(declare)
			compiler.scope.declare_symbol(result);
	}

	Types::Types(Compiler &compiler)
	{
		declare(compiler, "null", type_null, false);

		declare(compiler, "abilcmd", type_abilcmd, true);
		declare(compiler, "actor", type_actor, true);
		declare(compiler, "actorscope", type_actorscope, true);
		declare(compiler, "aifilter", type_aifilter, true);
		declare(compiler, "animtarget", type_animtarget, true);
		declare(compiler, "bank", type_bank, true);
		declare(compiler, "bool", type_bool, true);
		declare(compiler, "byte", type_byte, true);
		declare(compiler, "camerainfo", type_camerainfo, true);
		declare(compiler, "char", type_char, true);
		declare(compiler, "color", type_color, true);
		declare(compiler, "doodad", type_doodad, true);
		declare(compiler, "fixed", type_fixed, true);
		declare(compiler, "handle", type_handle, true);
		declare(compiler, "int", type_int, true);
		declare(compiler, "marker", type_marker, true);
		declare(compiler, "order", type_order, true);
		declare(compiler, "playergroup", type_playergroup, true);
		declare(compiler, "point", type_point, true);
		declare(compiler, "region", type_region, true);
		declare(compiler, "revealer", type_revealer, true);
		declare(compiler, "sound", type_sound, true);
		declare(compiler, "soundlink", type_soundlink, true);
		declare(compiler, "string", type_string, true);
		declare(compiler, "text", type_text, true);
		declare(compiler, "timer", type_timer, true);
		declare(compiler, "transmissionsource", type_transmissionsource, true);
		declare(compiler, "trigger", type_trigger, true);
		declare(compiler, "unit", type_unit, true);
		declare(compiler, "unitfilter", type_unitfilter, true);
		declare(compiler, "unitgroup", type_unitgroup, true);
		declare(compiler, "unitref", type_unitref, true);
		declare(compiler, "wave", type_wave, true);
		declare(compiler, "waveinfo", type_waveinfo, true);
		declare(compiler, "wavetarget", type_wavetarget, true);

		type_fixed.type.implicit_conversions.insert(&type_int.type);

		type_marker.type.implicit_conversions.insert(&type_null.type);
		type_point.type.implicit_conversions.insert(&type_null.type);
		type_string.type.implicit_conversions.insert(&type_null.type);
		type_unit.type.implicit_conversions.insert(&type_null.type);
		type_wave.type.implicit_conversions.insert(&type_null.type);
	}
};