#include "expressions.hpp"
#include "../document.hpp"
#include "../compiler.hpp"

namespace Legion
{
	void ExpressionNode::setup_type(Document &document, LocalNode &local, bool name)
	{
		document.report_type_modifier(get_range(document.memory_pool));
	}

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
				return symbol->node->get_type(args);
			else
				return 0;
		}

		new WrongSymbolError(args.document, *range, symbol, ident, Symbol::VARIABLE);

		return 0;
	}

	void BinaryOpNode::setup_type(Document &document, LocalNode &local, bool name)
	{
		if(op != Lexeme::MUL)
			document.report_type_modifier(*range);

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
				document.report_type_modifier(*range);

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

	Type *BinaryOpNode::validate_op(ValidationArgs &args, ExpressionNode *node, Lexeme::Type op, Type *left_type, Type *right_type)
	{
		Type *type;

		if(left_type->compatible(args, right_type, false))
			type = left_type;
		else if(right_type->compatible(args, left_type, false))
			type = right_type;
		else
		{
			args.document.report(node->get_range(args.memory_pool), "Binary operator '" + Lexeme::names[op] + "' is incompatible with types '" + left_type->string() + "' and '" + right_type->string() + "'");

			return 0;
		}

		Type *result = Type::resolve(type);
		
		if(!result)
			return 0;

		bool compatible = false;

		result = result->compatible_binary_op(args, op, compatible);

		if(!compatible)
		{
			args.document.report(node->get_range(args.memory_pool), "Binary operator '" + Lexeme::names[op] + "' is incompatible with types '" + left_type->string() + "' and '" + right_type->string() + "'");

			return 0;
		}
		else
			return result;
	}

	Type *BinaryOpNode::validate(ValidationArgs &args)
	{
		Type *left_type = left->validate(args);
		Type *right_type = right->validate(args);

		if(!left_type || !right_type)
			return 0;

		switch(op)
		{
			case Lexeme::EQUAL:
			case Lexeme::NOT_EQUAL:
				left_type->compatible(args, right_type, right);
				return args.types.type_bool.type;

			default:
				return validate_op(args, this, op, left_type, right_type);			
		}
	}

	Type *AssignNode::validate(ValidationArgs &args)
	{
		Type *left_type = left->validate(args);
		Type *right_type = right->validate(args);

		if(!left_type || !right_type)
			return 0;

		switch(op)
		{
			case Lexeme::ASSIGN:
				left_type->compatible(args, right_type, right);
				break;

			case Lexeme::ASSIGN_ADD:
				left_type->compatible(args, validate_op(args, this, Lexeme::ADD, left_type, right_type), right);	
				break;

			case Lexeme::ASSIGN_SUB:
				left_type->compatible(args, validate_op(args, this, Lexeme::SUB, left_type, right_type), right);		
				break;

			case Lexeme::ASSIGN_MUL:
				left_type->compatible(args, validate_op(args, this, Lexeme::MUL, left_type, right_type), right);		
				break;

			case Lexeme::ASSIGN_DIV:
				left_type->compatible(args, validate_op(args, this, Lexeme::DIV, left_type, right_type), right);		
				break;

			case Lexeme::ASSIGN_MOD:
				left_type->compatible(args, validate_op(args, this, Lexeme::MOD, left_type, right_type), right);		
				break;

			case Lexeme::ASSIGN_BITWISE_OR:
				left_type->compatible(args, validate_op(args, this, Lexeme::BITWISE_OR, left_type, right_type), right);		
				break;

			case Lexeme::ASSIGN_BITWISE_XOR:
				left_type->compatible(args, validate_op(args, this, Lexeme::BITWISE_XOR, left_type, right_type), right);		
				break;

			case Lexeme::ASSIGN_BITWISE_AND:
				left_type->compatible(args, validate_op(args, this, Lexeme::BITWISE_AND, left_type, right_type), right);		
				break;

			case Lexeme::ASSIGN_BITWISE_NOT:
				left_type->compatible(args, validate_op(args, this, Lexeme::BITWISE_NOT, left_type, right_type), right);		
				break;

			case Lexeme::ASSIGN_LEFT_SHIFT:
				left_type->compatible(args, validate_op(args, this, Lexeme::LEFT_SHIFT, left_type, right_type), right);		
				break;

			case Lexeme::ASSIGN_RIGHT_SHIFT:
				left_type->compatible(args, validate_op(args, this, Lexeme::RIGHT_SHIFT, left_type, right_type), right);		
				break;

			case Lexeme::ASSIGN_LOGICAL_OR:
				left_type->compatible(args, validate_op(args, this, Lexeme::LOGICAL_OR, left_type, right_type), right);		
				break;

			case Lexeme::ASSIGN_LOGICAL_AND:
				left_type->compatible(args, validate_op(args, this, Lexeme::LOGICAL_AND, left_type, right_type), right);		
				break;

			default:
				assert(0);	
				return 0;
		}

		return left_type;
	}

	StatementNode *AssignNode::get_declaration(Document &document)
	{
		LocalNode *local = (LocalNode *)left->get_declaration(document);

		if(local)
		{
			local->value = right;
			local->has_value = true;

			if(op != Lexeme::ASSIGN)
				document.report(*range, "Unexpected assignment with operator " + Lexeme::describe(range, op));

			return local;
		}
		else
			return 0;
	}

	void UnaryOpNode::setup_type(Document &document, LocalNode &local, bool name)
	{
		if(op != Lexeme::MUL)
			document.report_type_modifier(*range);
		
		value->setup_type(document, local, name);
		
		local.type_node->modifiers.add<TypePointerNode>(document.memory_pool);
	}

	Type *UnaryOpNode::validate(ValidationArgs &args)
	{
		if(op == Lexeme::LOGICAL_NOT) //TODO: Figure out how ! works
			return 0;

		Type *type = value->validate(args);

		if(!type)
			return 0;

		Type *result = Type::resolve(type);
		
		if(!result)
			return 0;

		bool compatible = false;

		result = result->compatible_unary_op(args, op, compatible);

		if(!compatible)
		{
			args.document.report(get_range(args.memory_pool), "Unary operator '" + Lexeme::names[op] + "' is incompatible with type '" + type->string() + "'");

			return 0;
		}
		else
			return result;
	}

	Type *ArraySubscriptNode::validate(ValidationArgs &args)
	{
		args.types.type_int.type->Type::compatible(args, index);

		Type *type = args.parent_type;

		if(type == 0)
			return 0;

		if(type->kind == Type::ARRAY_TYPE)
			return ((ArrayType *)type)->base;
		else
		{
			Range &span = args.parent_node->get_range(args.memory_pool);
			span.expand(*range);

			args.document.report(span, "Cannot get subscript of type '" +  type->string() + "'");
			type = 0;

			return 0;
		}
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
			if(i().node_type() != Node::ARRAY_SUBSCRIPT_NODE)
				document.report_type_modifier(i().get_range(document.memory_pool));
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

		for(ExpressionList::Iterator i = chain.begin(); i; i++)
		{
			args.parent_type = type;
			args.parent_node = factor;
			type = i().validate(args);
		}

		return 0;
	}

	Type *IntegerNode::validate(ValidationArgs &args)
	{
		return args.types.type_int.type;
	}

	Type *StringNode::validate(ValidationArgs &args)
	{
		return args.types.type_string.type;
	}

	Type *FixedNode::validate(ValidationArgs &args)
	{
		return args.types.type_fixed.type;
	}

	Type *BooleanNode::validate(ValidationArgs &args)
	{
		return args.types.type_bool.type;
	}

	Type *NullNode::validate(ValidationArgs &args)
	{
		return args.types.type_null.type;
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
				new WrongSymbolError(args.document, get_range(args.memory_pool), symbol, ident->ident, Symbol::FUNCTION);

				return 0;
			}
		}

		if(!symbol)
			return 0;

		FunctionType *type = 0;
		
		if(symbol && symbol->node)
			type = (FunctionType *)symbol->node->get_type(args);

		if(type && arguments.size != type->params.size)
		{
			std::stringstream message;

			message << "Expected " << type->params.size << "argument";

			if(type->params.size != 1)
				message << "s";

			message << " to function '" << ident->ident->string() << "', but found " << arguments.size << "argument";

			if(arguments.size != 1)
				message << "s";

			args.document.report(get_range(args.memory_pool), message.str());

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
