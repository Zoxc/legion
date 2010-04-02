#include "expressions.hpp"
#include "../document.hpp"

namespace Legion
{
	StatementNode *BinaryOpNode::get_declaration(Document *document)
	{
		Scope *scope = document->scope;

		if(op == Lexeme::MUL && right->is_declaration_name() && left->is_type_name(scope))
		{
			MemoryPool *memory_pool = scope->memory_pool;

			LocalNode *local = new (memory_pool) LocalNode;

			local->is_const = false;
			local->has_value = false;
			local->type = new (memory_pool) TypeNode;
			local->symbol = new (memory_pool) VarSymbol;

			left->setup_local(local, false, memory_pool);
			right->setup_local(local, true, memory_pool);

			local->type->modifiers.add<TypePointerNode>(memory_pool);

			if(scope->declare_symbol(local->symbol))
				local->symbol->redeclared(document);

			return local;
		}
		else
			return 0;
	}
};
