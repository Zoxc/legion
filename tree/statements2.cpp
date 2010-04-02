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
			StatementNode *node = (*i)->get_declaration(*document);

			if(node)
				i.replace(node);
		}

		document->scope = old;

		return false;
	}

	StatementNode *LocalNode::get_declaration(Document &document)
	{
		type = new (document.memory_pool) TypeNode;
		type_expression->setup_type(document, *this, false);

		if(document.scope->declare_symbol(symbol))
			symbol->redeclared(document);

		return 0;
	}
};
