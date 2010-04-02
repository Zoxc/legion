#include "statements.hpp"
#include "../document.hpp"

namespace Legion
{
	bool Block::find_declarations(Document *document)
	{
		Scope *old = document->scope;
		document->scope = this->scope;

		for(StatementList::Iterator i = statements.begin(); i; i++)
		{
			StatementNode *node = (*i)->get_declaration(document);

			if(node)
				i.replace(node);
		}

		document->scope = old;

		return false;
	}

	StatementNode *LocalNode::get_declaration(Document *document)
	{
		Scope *scope = document->scope;

		if(type_expression->is_type_name(scope))
		{
			MemoryPool *memory_pool = scope->memory_pool;

			type = new (memory_pool) TypeNode;
			type_expression->setup_local(this, false, memory_pool);

			if(scope->declare_symbol(symbol))
				symbol->redeclared(document);

			return 0;
		}
		else
		{
			symbol->range->report(document, "Unexpected " + Lexeme::describe(symbol->range, Lexeme::IDENT));

			return type_expression;
		}
	}
};
