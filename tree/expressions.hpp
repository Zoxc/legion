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

		Type get_type()
		{
			return Node::IDENT_NODE;
		}

		std::string string()
		{
			return wrap(ident->string());
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

		std::string string()
		{
			return wrap(left->string() + " " + Lexeme::names[op] + " " + right->string());
		}
	};

	struct UnaryOpNode:
		public ExpressionNode
	{
		Lexeme::Type op;
		ExpressionNode *value;
		
		std::string string()
		{
			return wrap(Lexeme::names[op] + value->string());
		}
	};

	struct ArraySubscriptNode:
		public ExpressionNode
	{
		ExpressionNode *index;

		std::string string()
		{
			return wrap("[" + index->string() + "]");
		}
	};

	struct ArrayDefNode:
		public ExpressionNode
	{
		ExpressionList sizes;

		std::string string()
		{
			std::string result;

			for(ExpressionList::Iterator i = sizes.begin(); i; i++)
				result += "[" + (*i)->string() + "]";

			return wrap(result);
		}
	};

	struct MemberRefNode:
		public ExpressionNode
	{
		String *name;
		bool by_ptr;

		std::string string()
		{
			if(by_ptr)
				return wrap("->" + name->string());
			else
				return wrap("." + name->string());
		}
	};

	struct FactorChainNode:
		public ExpressionNode
	{
		ExpressionNode *factor;
		ExpressionList chain;

		std::string string()
		{
			return wrap(factor->string() + chain.join(""));
		}
	};

	struct IntegerNode:
		public ExpressionNode
	{
		int value;

		std::string string()
		{
			std::stringstream out;
			out << value;
			return wrap(out.str());
		}
	};

	struct StringNode:
		public ExpressionNode
	{
		String *value;

		std::string string()
		{
			return wrap("\"" + value->string() + "\"");
		}
	};

	struct FixedNode:
		public ExpressionNode
	{
		double value;

		std::string string()
		{
			std::stringstream out;
			out << value;
			return wrap(out.str());
		}
	};

	struct BooleanNode:
		public ExpressionNode
	{
		bool value;

		std::string string()
		{
			if(value)
				return wrap("true");
			else
				return wrap("false");
		}
	};

	struct NullNode:
		public ExpressionNode
	{
		std::string string()
		{
			return wrap("null");
		}
	};

	struct CallNode:
		public ExpressionNode
	{
		IdentNode *ident;
		ExpressionList arguments;

		std::string string()
		{
			return wrap(ident->string() + "(" + arguments.join(", ") + ")");
		}
	};

};
