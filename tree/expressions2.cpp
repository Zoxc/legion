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
	
	Type *IdentNode::validate(ValidationArgs &args)
	{
		bool found;

		Symbol *symbol = args.scope->lookup(ident, &args.document, Symbol::VARIABLE, found);

		if(found)
		{
			if(symbol)
				return symbol->node->validate(args);
			else
				return 0;
		}

		range->report_expected_symbol(args.document, symbol, ident, Symbol::VARIABLE);

		return 0;
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

	Type *BinaryOpNode::validate(ValidationArgs &args)
	{
		Type *left_type = left->validate(args);
		Type *right_type = right->validate(args);

		switch(op)
		{
			case Lexeme::EQUAL:
			case Lexeme::NOT_EQUAL:
			case Lexeme::LOGICAL_AND:
			case Lexeme::LOGICAL_OR:
				left_type->compatible(args, right_type, right);
				return &args.types.type_bool.type;

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

	Type *UnaryOpNode::validate(ValidationArgs &args)
	{
		Type *type = value->validate(args);

		switch(op)
		{
			case Lexeme::NOT_EQUAL:
				args.types.type_bool.type.Type::compatible(args, type, value);
				return &args.types.type_bool.type;

			default:
				return 0;
		}
	}

	Type *ArraySubscriptNode::validate(ValidationArgs &args)
	{
		args.types.type_int.type.Type::compatible(args, index);
		
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

	Type *FactorChainNode::validate(ValidationArgs &args)
	{
		Type *type = factor->validate(args);

		return 0;
	}

	Type *IntegerNode::validate(ValidationArgs &args)
	{
		return &args.types.type_int.type;
	}

	Type *StringNode::validate(ValidationArgs &args)
	{
		return &args.types.type_string.type;
	}

	Type *FixedNode::validate(ValidationArgs &args)
	{
		return &args.types.type_fixed.type;
	}

	Type *BooleanNode::validate(ValidationArgs &args)
	{
		return &args.types.type_bool.type;
	}

	Type *NullNode::validate(ValidationArgs &args)
	{
		return &args.types.type_null.type;
	}

	Type *CallNode::validate(ValidationArgs &args)
	{
		bool found;

		Symbol *symbol = args.scope->lookup(ident->ident, &args.document, Symbol::FUNCTION, found);

		if(!found)
		{
			symbol = args.scope->lookup(ident->ident, &args.document, Symbol::PROTOTYPE, found);

			if(!found)
			{
				get_range().report_expected_symbol(args.document, symbol, ident->ident, Symbol::FUNCTION);

				return 0;
			}
		}

		if(!symbol)
			return 0;

		FunctionType *type = 0;
		
		if(symbol && symbol->node)
			type = (FunctionType *)symbol->node->validate(args);

		if(type && arguments.size != type->params.size)
		{
			std::stringstream message;

			message << "Expected " << type->params.size << "argument";

			if(type->params.size != 1)
				message << "s";

			message << " to function '" << ident->ident->string() << "', but found " << arguments.size << "argument";

			if(arguments.size != 1)
				message << "s";

			get_range().report(args.document, message.str());

			for(ExpressionList::Iterator i = arguments.begin(); i; i++)
				i().validate(args);
		}
		else if(type)
		{
			List<FunctionType::Parameter>::Iterator j = type->params.begin();

			for(ExpressionList::Iterator i = arguments.begin(); i; i++, j++)
				j().type->compatible(args, *i);
		}
		else
		{
			for(ExpressionList::Iterator i = arguments.begin(); i; i++)
				i().validate(args);
		}

		if(type)
			return type->returns;
		else
			return 0;
	}
};
