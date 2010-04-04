#include "expressions.hpp"
#include "../document.hpp"
#include "../compiler.hpp"

namespace Legion
{
	bool IdentNode::is_type_name(Document &document)
	{
		return document.scope->lookup_type(ident) == Symbol::TYPE;
	}

	void IdentNode::setup_type(Document &document, LocalNode &local, bool name)
	{
		if(name)
		{
			local.symbol->name = ident;
			local.symbol->range = range;
		}
		else
		{
			local.type_node->name = ident;
			local.type_node->range = range;
		}
	}

	void BinaryOpNode::setup_type(Document &document, LocalNode &local, bool name)
	{
		if(op != Lexeme::MUL)
			range->report_type_modifier(document);

		left->setup_type(document, local, name);
		
		local.type_node->modifiers.add<TypePointerNode>(&document.memory_pool);
		
		right->setup_type(document, local, name);
	}

	StatementNode *BinaryOpNode::get_declaration(Document &document)
	{
		if(left->is_type_name(document) && right->is_declaration_name())
		{
			MemoryPool &memory_pool = document.memory_pool;

			LocalNode *local = new (memory_pool) LocalNode;

			local->is_const = false;
			local->has_value = false;
			local->type_node = new (memory_pool) TypeNode;
			local->symbol = new (memory_pool) VarSymbol;

			if(op != Lexeme::MUL)
				range->report_type_modifier(document);

			left->setup_type(document, *local, false);

			local->type_node->modifiers.add<TypePointerNode>(memory_pool);

			right->setup_type(document, *local, true);

			if(document.scope->declare_symbol(local->symbol))
				local->symbol->redeclared(document);

			return local;
		}
		else
			return 0;
	}

	void UnaryOpNode::setup_type(Document &document, LocalNode &local, bool name)
	{
		if(op != Lexeme::MUL)
			range->report_type_modifier(document);
		
		value->setup_type(document, local, name);
		
		local.type_node->modifiers.add<TypePointerNode>(document.memory_pool);
	}

	void ArrayDefNode::setup_type(Document &document, LocalNode &local, bool name)
	{
		for(ExpressionList::Iterator i = sizes.begin(); i; i++)
		{
			TypeArrayNode *node = local.type_node->modifiers.add<TypeArrayNode>(document.memory_pool);

			node->size = *i;
		}
	}

	void FactorChainNode::setup_type(Document &document, LocalNode &local, bool name)
	{
		for(ExpressionList::Iterator i = chain.begin(); i; i++)
		{
			if((*i)->node_type() != Node::ARRAY_SUBSCRIPT_NODE)
				(*i)->get_range().report_type_modifier(document);
			else
			{
				TypeArrayNode *node = local.type_node->modifiers.add<TypeArrayNode>(document.memory_pool);

				node->size = ((ArraySubscriptNode *)*i)->index;
			}
		}
	}

	Type *IntegerNode::get_type(Document &document, SymbolList &stack)
	{
		return &document.compiler.types.type_int.type;
	}

	Type *StringNode::get_type(Document &document, SymbolList &stack)
	{
		return &document.compiler.types.type_string.type;
	}

	Type *FixedNode::get_type(Document &document, SymbolList &stack)
	{
		return &document.compiler.types.type_fixed.type;
	}

	Type *BooleanNode::get_type(Document &document, SymbolList &stack)
	{
		return &document.compiler.types.type_bool.type;
	}

	Type *NullNode::get_type(Document &document, SymbolList &stack)
	{
		return &document.compiler.types.type_null.type;
	}
};
