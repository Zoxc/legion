#include "globals.hpp"
#include "../document.hpp"

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
				type->compitable(document, stack, value);
		}

		return type;
	}
};
