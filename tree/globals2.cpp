#include "globals.hpp"
#include "../document.hpp"
#include "../compiler.hpp"

namespace Legion
{
	Type *StructNode::validate(ValidationArgs &args)
	{
		if(!has_type)
		{
			has_type = true;
			type = new (args.memory_pool) CompositeType();
			type->name = symbol->name;

			for(List<FieldNode>::Iterator i = fields.begin(); i; i++)
			{
				CompositeType::Field *field = type->fields.add<CompositeType::Field>(args.memory_pool);
				field->name = (*i)->pair.name;
				field->type = (*i)->pair.validate(args);
			}
		}
		
		return type;
	}

	Type *TypedefNode::validate(ValidationArgs &args)
	{
		if(!has_type)
		{
			has_type = true;
			type = new (args.memory_pool) TypedefType(pair->validate(args));
			type->name = pair->name;
		}

		return type;
	}

	Type *GlobalNode::validate(ValidationArgs &args)
	{
		Type *type = get_type(args);

		if(has_value)
			type->compatible(args, value);

		return type;
	}

	Type *GlobalNode::get_type(ValidationArgs &args)
	{
		if(!has_type)
		{
			has_type = true;
			type = pair->validate(args);
		}

		return type;
	}

	Type *ParamNode::validate(ValidationArgs &args)
	{
		if(!has_type)
		{
			has_type = true;
			type = pair.validate(args);
		}

		return type;
	}

	Type *FuncHeadNode::validate(ValidationArgs &args)
	{
		if(!has_type)
		{
			has_type = true;
			type = new (args.memory_pool) FunctionType();

			if(pair->type && !pair->type->modifiers.first && pair->type->name == args.types.type_void.type.name)
				type->returns = &args.types.type_void.type;
			else
				type->returns = pair->validate(args);

			for(List<ParamNode>::Iterator i = params.begin(); i; i++)
			{
				FunctionType::Parameter *param = type->params.add<FunctionType::Parameter>(args.memory_pool);
				param->type = i().pair.validate(args);
			}
		}

		return type;
	}

	Type *PrototypeNode::validate(ValidationArgs &args)
	{
		Symbol *symbol = head->symbol;
		Type *type = head->validate(args);

		if(symbol->type == Symbol::PROTOTYPE)
		{
			PrototypeNode *prototype = (PrototypeNode *)symbol->node;

			if(!prototype->head->is_native)
			{
				Symbol *current = symbol;

				current = current->next_name;

				while(current->next_name != symbol)
				{
					if(current->type == Symbol::FUNCTION && type->exact(current->node->validate(args)))
						goto end;

					current = current->next_name;
				}

				args.document.report(head->pair->range, "Undefined prototype");

				end:;
			}
		}
		
		return type;
	}
};
