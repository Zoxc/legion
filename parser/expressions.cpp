#include "parser.hpp"
#include "../tree/scope.hpp"
#include "../tree/types.hpp"
#include "../tree/symbols.hpp"
#include "../tree/expressions.hpp"

namespace Legion
{
	bool Parser::is_expression(Lexeme::Type lexeme)
	{
		switch(lexeme)
		{
			case Lexeme::ADD:
			case Lexeme::SUB:
			case Lexeme::LOGICAL_NOT:
			case Lexeme::BITWISE_NOT:
			case Lexeme::BITWISE_AND:
			case Lexeme::MUL:

			case Lexeme::PARENT_OPEN:
			case Lexeme::IDENT:
			case Lexeme::STRING:
			case Lexeme::KW_TRUE:
			case Lexeme::KW_FALSE:
			case Lexeme::KW_NULL:
			case Lexeme::INTEGER:
			case Lexeme::HEX:
			case Lexeme::OCTAL:
			case Lexeme::REAL:
				return true;

			default:
				return false;
		}
	}

	ExpressionNode *Parser::parse_factor()
	{
		switch(lexeme())
		{
			case Lexeme::PARENT_OPEN:
				return parse_grouped_expression();

			case Lexeme::IDENT:
			{
				IdentNode *ident = new (memory_pool) IdentNode;

				ident->ident = lexer.lexeme.value;

				step();

				if(matches(Lexeme::PARENT_OPEN))
				{
					CallNode *node = new (memory_pool) CallNode;

					node->ident = ident;

					if(lexeme() != Lexeme::PARENT_CLOSE)
					{
						do
						{
							node->arguments.append(parse_expression());
						}
						while(matches(Lexeme::COMMA));
					}

					match(Lexeme::PARENT_CLOSE);

					return node;
				}

				return ident;
			}

			case Lexeme::STRING:
			{
				StringNode *node = new (memory_pool) StringNode;

				node->value = lexer.lexeme.value;

				step();

				return node;
			}

			case Lexeme::KW_TRUE:
			case Lexeme::KW_FALSE:
			{
				BooleanNode *node = new (memory_pool) BooleanNode;

				node->value = lexeme() == Lexeme::KW_TRUE;

				step();

				return node;
			}

			case Lexeme::KW_NULL:
				return new (memory_pool) NullNode;

			case Lexeme::INTEGER:
			case Lexeme::HEX:
			case Lexeme::OCTAL:
			{
				IntegerNode *node = new (memory_pool) IntegerNode;

				node->value = atoi(lexer.lexeme.string().c_str());

				step();

				return node;
			}

			case Lexeme::REAL:
			{
				FixedNode *node = new (memory_pool) FixedNode;

				node->value = atof(lexer.lexeme.string().c_str());

				step();

				return node;
			}

			default:
				lexer.lexeme.report(document, "Expected expression");
				return 0;
		}
	}

	ExpressionNode *Parser::parse_array_subscript()
	{
		ArraySubscriptNode *node = new (memory_pool) ArraySubscriptNode;

		step();

		node->index = parse_expression();

		match(Lexeme::SQR_BRACET_CLOSE);

		return node;
	}

	bool Parser::is_factor_chain(Lexeme::Type lexeme)
	{
		switch(lexeme)
		{
			case Lexeme::SQR_BRACET_OPEN:
			case Lexeme::MEMBER:
			case Lexeme::MEMBER_PTR:
				return true;

			default:
				return false;
		}
	}

	ExpressionNode *Parser::parse_factor_chain()
	{
		if(lexeme() == Lexeme::SQR_BRACET_OPEN) //TODO: Make type only
		{
			ArrayDefNode *node = new (memory_pool) ArrayDefNode;

			do
			{
				node->sizes.append(parse_array_subscript());
			}
			while(lexeme() == Lexeme::SQR_BRACET_OPEN);

			return node;
		}
		else
		{
			ExpressionNode *result = parse_factor();

			if(is_factor_chain(lexeme()))
			{
				FactorChainNode *chain = new (memory_pool) FactorChainNode;

				chain->factor = result;

				do
				{
					switch(lexeme())
					{
						case Lexeme::SQR_BRACET_OPEN:
							chain->chain.append(parse_array_subscript());
							break;

						case Lexeme::MEMBER:
						{
							step();
							MemberRefNode *node = chain->chain.add<MemberRefNode>(memory_pool);

							if(expect(Lexeme::IDENT))
							{
								node->name = lexer.lexeme.value;
								step();
							}
							else
								node->name = 0;

							node->by_ptr = false;

							break;
						}
						

						case Lexeme::MEMBER_PTR:
						{
							step();
							MemberRefNode *node = chain->chain.add<MemberRefNode>(memory_pool);

							if(matches(Lexeme::IDENT))
								node->name = lexer.lexeme.value;
							else
								node->name = 0;

							node->by_ptr = true;

							break;
						}
						
						default:
							goto end;
					}
				}
				while(true);
				end:

				return chain;
			}
			else
				return result;
		}
	}

	ExpressionNode *Parser::parse_unary()
	{
		switch(lexeme())
		{
			case Lexeme::ADD:
			case Lexeme::SUB:
			case Lexeme::LOGICAL_NOT:
			case Lexeme::BITWISE_NOT:
			case Lexeme::BITWISE_AND:
			case Lexeme::MUL:
			{
				UnaryOpNode *node = new (memory_pool) UnaryOpNode;

				node->op = lexeme();

				step();

				node->value = parse_unary();

				return node;
			}
			break;

			default:
				return parse_factor_chain();
		}
	}

	ExpressionNode *Parser::parse_assign()
	{
		ExpressionNode *result = parse_binary_operator();

		switch(lexeme())
		{
			case Lexeme::ASSIGN:
			case Lexeme::ASSIGN_ADD:
			case Lexeme::ASSIGN_SUB:
			case Lexeme::ASSIGN_MUL:
			case Lexeme::ASSIGN_DIV:
			case Lexeme::ASSIGN_MOD:
			case Lexeme::ASSIGN_BITWISE_OR:
			case Lexeme::ASSIGN_BITWISE_XOR:
			case Lexeme::ASSIGN_BITWISE_AND:
			case Lexeme::ASSIGN_LEFT_SHIFT:
			case Lexeme::ASSIGN_RIGHT_SHIFT:
			{
				BinaryOpNode *node = new (memory_pool) BinaryOpNode;

				node->op = lexeme();
				node->left = result;

				step();

				node->right = parse_assign();

				result = node;
			}
			break;

			default:
				break;
		}

		return result;
	}

	ExpressionNode *Parser::parse_expression()
	{
		return parse_assign();
	}

	ExpressionNode *Parser::parse_grouped_expression()
	{
		ExpressionNode *result;

		match(Lexeme::PARENT_OPEN);
		result = parse_expression();
		match(Lexeme::PARENT_CLOSE);

		return result;
	}

	static const Lexeme::Type binary_operator_start = Lexeme::LOGICAL_OR;
	static const Lexeme::Type binary_operator_end = Lexeme::MOD;

	static size_t operator_precedences[] = {
			0, // LOGICAL_OR

			1, // LOGICAL_AND

			2, // BITWISE_OR,

			3, // BITWISE_XOR,

			4, // BITWISE_AND,

			5, // EQUAL,
			5, // NOT_EQUAL,

			6, // LESS,
			6, // LESS_OR_EQUAL,
			6, // GREATER,
			6, // GREATER_OR_EQUAL,

			7, // LEFT_SHIFT,
			7, // RIGHT_SHIFT,

			8, // ADD,
			8, // SUB,

			9, // MUL,
			9, // DIV,
			9 // MOD
	};

	bool Parser::is_binary_operator(Lexeme::Type op)
	{
		return op >= binary_operator_start && op <= binary_operator_end;
	}

	ExpressionNode *Parser::parse_binary_operator()
	{
		return parse_binary_operator(parse_unary(), 0);
	}

	ExpressionNode *Parser::parse_binary_operator(ExpressionNode *left, size_t min_precedence)
	{
		while(true)
		{
			Lexeme::Type op = lexeme();

			if(!is_binary_operator(op))
				break;

			size_t precedence = operator_precedences[op - binary_operator_start];

			if(precedence < min_precedence)
				break;

			step();

			ExpressionNode *right = parse_unary();

			while(true)
			{
				Lexeme::Type next_op = lexeme();

				if(!is_binary_operator(next_op))
					break;

				size_t next_precedence = operator_precedences[next_op - binary_operator_start];

				if(next_precedence <= precedence)
					break;

				right = parse_binary_operator(right, next_precedence);
			}

			BinaryOpNode *node = new (memory_pool) BinaryOpNode;

			node->op = op;
			node->left = left;
			node->right = right;

			left = node;
		}

		return left;
	}
};
