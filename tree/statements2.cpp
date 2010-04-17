#include "statements.hpp"
#include "../document.hpp"
#include "../compiler.hpp"

namespace Legion
{
	bool Block::find_declarations(Document *document)
	{
		Scope *old = document->scope;
		document->scope = scope;

		for(StatementList::Iterator i = statements.begin(); i; i++)
		{
			StatementNode *node = (*i)->get_declaration(*document);

			if(node)
				i.replace(node);
		}

		document->scope = old;

		return false;
	}

	Type *Block::validate(ValidationArgs &args)
	{
		Scope *old = args.scope;
		args.scope = scope;

		bool allow_locals = true;
		bool saw_return = false;

		for(StatementList::Iterator i = statements.begin(); i; i++)
		{
			i().validate(args);

			if(saw_return)
				args.document.report(i().get_range(args.memory_pool), "Statement after a return statement");

			switch(i().node_type())
			{
				case LOCAL_NODE:
					if(!allow_locals)
						args.document.report(i().get_range(args.memory_pool), "Local variables must be declared at the start of a function");
					break;
	
				case RETURN_NODE:
					saw_return = true;

				case BLOCK_NODE:
				case IF_NODE:
				case WHILE_NODE:
				case DO_NODE:
				case BREAK_NODE:
				case CONTINUE_NODE:
				case ASSIGN_NODE:
				case CALL_NODE:
					allow_locals = false;
					break;

				default:
					allow_locals = false;
					args.document.report(i().get_range(args.memory_pool), "Unexpected expresssion");
			}

		}

		args.scope = old;

		return false;
	}

	Type *ControlFlowNode::validate(ValidationArgs &args)
	{
		Type *type = condition->validate(args);

		if(!type)
			return 0;

		if(args.types.type_bool.type->compatible(args, type, Type::WEAK) || type->compatible(args, args.types.type_null.type, Type::WEAK))
			return 0;

		new IncompatableTypesError(args.document, condition->get_range(args.memory_pool), type, args.types.type_bool.type);

		return 0;
	}

	StatementNode *LocalNode::get_declaration(Document &document)
	{
		type_node = new (document.memory_pool) TypeNode;
		type_expression->setup_type(document, *this, false);

		if(document.scope->declare_symbol(symbol))
			symbol->redeclared(document);

		return 0;
	}

	Type *LocalNode::validate(ValidationArgs &args)
	{
		if(!has_type)
		{
			has_type = true;
			type = type_node->validate(args);

			if(has_value)
				type->compatible(args, value);
		}

		return type;
	}
};
