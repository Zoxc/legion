#include "globals.hpp"
#include "../document.hpp"
#include "../compiler.hpp"

namespace Legion
{
	Type *StructNode::get_type(Document &document, SymbolList &stack)
	{
		if(!type)
		{
			type = new (document.memory_pool) CompositeType();
			type->name = symbol->name;

			for(NodeList<FieldNode>::Iterator i = fields.begin(); i; i++)
			{
				CompositeType::Field *field = type->fields.add<CompositeType::Field>(document.memory_pool);
				field->name = (*i)->pair.name;
				field->type = (*i)->pair.get_type(document, stack);
			}
		}
		
		return type;
	}

	Type *TypedefNode::get_type(Document &document, SymbolList &stack)
	{
		if(!type)
		{
			type = new (document.memory_pool) TypedefType(pair->get_type(document, stack));
			type->name = pair->name;
		}

		return type;
	}

	Type *GlobalNode::get_type(Document &document, SymbolList &stack)
	{
		if(!type)
		{
			type = pair->get_type(document, stack);

			if(has_value)
				type->compatible(document, stack, value);
		}

		return type;
	}

	Type *ParamNode::get_type(Document &document, SymbolList &stack)
	{
		if(!type)
			type = pair.get_type(document, stack);

		return type;
	}

	Type *FuncHeadNode::get_type(Document &document, SymbolList &stack)
	{
		if(!type)
		{
			type = new (document.memory_pool) FunctionType();

			if(pair->type && !pair->type->modifiers.first && pair->type->name == document.compiler.types.type_void.type.name)
				type->returns = &document.compiler.types.type_void.type;
			else
				type->returns = pair->get_type(document, stack);

			for(NodeList<ParamNode>::Iterator i = params.begin(); i; i++)
			{
				FunctionType::Parameter *param = type->params.add<FunctionType::Parameter>(document.memory_pool);
				param->type = (*i)->pair.get_type(document, stack);
			}

			if(symbol && !symbol->defined)
				pair->range.report(document, "Undefined prototype");
		}

		return type;
	}
};
