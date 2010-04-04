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

	Type *Block::get_type(Document &document, SymbolList &stack)
	{
		Scope *old = document.scope;
		document.scope = scope;

		bool allow_locals = true;
		bool saw_return = false;

		for(StatementList::Iterator i = statements.begin(); i; i++)
		{
			(*i)->get_type(document, stack);

			if(saw_return)
				(*i)->get_range().report(document, "Statement after a return statement");

			switch((*i)->node_type())
			{
				case LOCAL_NODE:
					if(!allow_locals)
						(*i)->get_range().report(document, "Local variables must be declared at the start of a function");
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
					(*i)->get_range().report(document, "Unexpected expresssion");
			}

		}

		document.scope = old;

		return false;
	}

	Type *ControlFlowNode::get_type(Document &document, SymbolList &stack)
	{
		Type *type = condition->get_type(document, stack);

		if(!type)
			return 0;

		if(document.compiler.types.type_bool.type.compatible(document, type) || type->compatible(document, &document.compiler.types.type_null.type))
			return 0;

		condition->get_range().report_types(document, type, &document.compiler.types.type_bool.type);

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

	Type *LocalNode::get_type(Document &document, SymbolList &stack)
	{
		if(!type)
		{
			type = type_node->get_type(document, stack);

			if(has_value)
				type->compatible(document, stack, value);
		}

		return type;
	}
};
