#include "expressions.hpp"
#include "../document.hpp"
#include "../compiler.hpp"

namespace Legion
{
	bool IdentNode::is_type_name(Document &document, bool lookup)
	{
		return !lookup || document.scope->lookup_type(ident) == Symbol::TYPE;
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
	
	Type *IdentNode::get_type(Document &document, SymbolList &stack)
	{
		Symbol *symbol = document.scope->lookup(ident);

		if(!symbol || symbol->type != Symbol::VARIABLE)
		{
			range->report_expected_symbol(document, symbol, ident, Symbol::VARIABLE);

			return 0;
		}

		return symbol->node->get_type(document, stack);
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
		if(left->is_type_name(document, true) && right->is_declaration_name())
		{
			MemoryPool &memory_pool = document.memory_pool;

			LocalNode *local = new (memory_pool) LocalNode;

			local->is_const = false;
			local->has_value = false;
			local->type_node = new (memory_pool) TypeNode;
			local->symbol = new (memory_pool) VarSymbol;
			local->symbol->node = local;

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

	Type *BinaryOpNode::get_type(Document &document, SymbolList &stack)
	{
		Type *left_type = left->get_type(document, stack);
		Type *right_type = right->get_type(document, stack);

		switch(op)
		{
			case Lexeme::EQUAL:
			case Lexeme::NOT_EQUAL:
			case Lexeme::LOGICAL_AND:
			case Lexeme::LOGICAL_OR:
				left_type->compatible(document, stack, right_type, right);
				return &document.compiler.types.type_bool.type;

			default:
				return 0;
		}

		return left_type;
	}

	void UnaryOpNode::setup_type(Document &document, LocalNode &local, bool name)
	{
		if(op != Lexeme::MUL)
			range->report_type_modifier(document);
		
		value->setup_type(document, local, name);
		
		local.type_node->modifiers.add<TypePointerNode>(document.memory_pool);
	}

	Type *UnaryOpNode::get_type(Document &document, SymbolList &stack)
	{
		Type *type = value->get_type(document, stack);

		switch(op)
		{
			case Lexeme::NOT_EQUAL:
				document.compiler.types.type_bool.type.Type::compatible(document, stack, type, value);
				return &document.compiler.types.type_bool.type;

			default:
				return 0;
		}
	}

	Type *ArraySubscriptNode::get_type(Document &document, SymbolList &stack)
	{
		document.compiler.types.type_int.type.Type::compatible(document, stack, index);
		
		return 0;
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

	Type *FactorChainNode::get_type(Document &document, SymbolList &stack)
	{
		Type *type = factor->get_type(document, stack);

		return 0;
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

	Type *CallNode::get_type(Document &document, SymbolList &stack)
	{
		Symbol *symbol = document.scope->lookup(ident->ident);

		if(!symbol || symbol->type != Symbol::FUNCTION)
		{
			get_range().report_expected_symbol(document, symbol, ident->ident, Symbol::FUNCTION);

			return 0;
		}

		FunctionType *type = 0;
		
		if(symbol && symbol->node)
			type = (FunctionType *)symbol->node->get_type(document, stack);

		if(type && arguments.size != type->params.size)
		{
			std::stringstream message;

			message << "Expected " << type->params.size << "argument";

			if(type->params.size != 1)
				message << "s";

			message << " to function '" << ident->ident->string() << "', but found " << arguments.size << "argument";

			if(arguments.size != 1)
				message << "s";

			get_range().report(document, message.str());

			for(CountedNodeList<ExpressionNode>::Iterator i = arguments.begin(); i; i++)
				(*i)->get_type(document, stack);
		}
		else if(type)
		{
			CountedNodeList<FunctionType::Parameter>::Iterator j = type->params.begin();

			for(CountedNodeList<ExpressionNode>::Iterator i = arguments.begin(); i; i++, j++)
				(*j)->type->compatible(document, stack, *i);
		}
		else
		{
			for(CountedNodeList<ExpressionNode>::Iterator i = arguments.begin(); i; i++)
				(*i)->get_type(document, stack);
		}

		if(type)
			return type->returns;
		else
			return 0;
	}
};
