#pragma once
#include "../common.hpp"
#include "../range.hpp"
#include "node.hpp"
#include "statements.hpp"
#include "scope.hpp"

namespace Legion
{
	class String;
	class Type;

	struct ExpressionNode:
		public StatementNode
	{
		virtual bool is_type()
		{
			return false;
		}

		virtual bool is_type_name()
		{
			return false;
		}
	};

	typedef NodeList<ExpressionNode> ExpressionList;

	struct IdentNode:
		public ExpressionNode
	{
		String *ident;

		std::string string();

		Type get_type()
		{
			return Node::IDENT_NODE;
		}
	};

	struct BinaryOpNode:
		public ExpressionNode
	{
		ExpressionNode *left;
		ExpressionNode *right;
		Lexeme::Type op;

		bool find_declarations(Scope *scope)
		{
			if(op == Lexeme::MUL && left->get_type() == Node::IDENT_NODE)
			{
				IdentNode *left = (IdentNode *)this->left;
				
				if(scope->get_type(left->ident) != Symbol::TYPE)
					return false;
				
				return right->is_type_name();
			}
			else
				return false;
		};

		bool is_type_name()
		{
			if(op == Lexeme::MUL)
			{
				return left->is_type() && right->is_type_name();
			}
			else
				return false;
		};

		std::string string();

		Type get_type()
		{
			return Node::BINARY_OP_NODE;
		}
	};

	struct UnaryOpNode:
		public ExpressionNode
	{
		Lexeme::Type op;
		ExpressionNode *value;
		
		std::string string();

		Type get_type()
		{
			return Node::UNARY_OP_NODE;
		}
	};

	struct ArraySubscriptNode:
		public ExpressionNode
	{
		ExpressionNode *index;

		std::string string();

		Type get_type()
		{
			return Node::ARRAY_SUBSCRIPT_NODE;
		}
	};

	struct ArrayDefNode:
		public ExpressionNode
	{
		ExpressionList sizes;

		std::string string();

		Type get_type()
		{
			return Node::ARRAY_DEF_NODE;
		}
	};

	struct MemberRefNode:
		public ExpressionNode
	{
		String *name;
		bool by_ptr;

		std::string string();

		Type get_type()
		{
			return Node::MEMBER_REF_NODE;
		}
	};

	struct FactorChainNode:
		public ExpressionNode
	{
		ExpressionNode *factor;
		ExpressionList chain;

		std::string string();

		Type get_type()
		{
			return Node::FACTOR_CHAIN_NODE;
		}
	};

	struct IntegerNode:
		public ExpressionNode
	{
		int value;

		std::string string();

		Type get_type()
		{
			return Node::INT_NODE;
		}
	};

	struct StringNode:
		public ExpressionNode
	{
		String *value;

		std::string string();

		Type get_type()
		{
			return Node::STRING_NODE;
		}
	};

	struct FixedNode:
		public ExpressionNode
	{
		double value;

		std::string string();

		Type get_type()
		{
			return Node::FIXED_NODE;
		}
	};

	struct BooleanNode:
		public ExpressionNode
	{
		bool value;

		std::string string();

		Type get_type()
		{
			return Node::BOOL_NODE;
		}
	};

	struct NullNode:
		public ExpressionNode
	{
		std::string string();

		Type get_type()
		{
			return Node::NULL_NODE;
		}
	};

	struct CallNode:
		public ExpressionNode
	{
		IdentNode *ident;
		ExpressionList arguments;

		std::string string();

		Type get_type()
		{
			return Node::CALL_NODE;
		}
	};

};
