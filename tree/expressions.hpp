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
		virtual bool is_declaration_name()
		{
			return false;
		}

		virtual bool is_type_name(Scope *scope)
		{
			return false;
		}

		virtual bool is_type_array()
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

		bool is_type_name(Scope *scope)
		{
			return scope->lookup_type(ident) == Symbol::TYPE;
		}
		
		bool is_declaration_name()
		{
			std::cout << "true @ is_declaration_name\n";
			return true;
		};
	};

	struct BinaryOpNode:
		public ExpressionNode
	{
		ExpressionNode *left;
		ExpressionNode *right;
		Lexeme::Type op;

		StatementNode *get_declaration(Scope *scope)
		{
			if(op == Lexeme::MUL && right->is_declaration_name() && left->is_type_name(scope))
			{
				LocalNode *declaration = new (scope->memory_pool) LocalNode;

				declaration->is_const = false;
				declaration->name = 0;
				declaration->value = 0;
				declaration->type = 0;

				return declaration;
			}
			else
				return 0;
		}

		bool is_type_name(Scope *scope)
		{
			return op == Lexeme::MUL && left->is_type_name(scope) && right->is_type_array();
		};

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
		
		Type get_type()
		{
			return Node::UNARY_OP_NODE;
		}

		bool is_declaration_name()
		{
			if(op == Lexeme::MUL)
				return value->is_declaration_name();
			else
				return false;
		};

		bool is_type_array()
		{
			if(op == Lexeme::MUL)
				return value->is_type_array();
			else
				return false;
		};
	};

	struct ArraySubscriptNode:
		public ExpressionNode
	{
		ExpressionNode *index;

		Type get_type()
		{
			return Node::ARRAY_SUBSCRIPT_NODE;
		}
	};

	struct ArrayDefNode:
		public ExpressionNode
	{
		ExpressionList sizes;

		Type get_type()
		{
			return Node::ARRAY_DEF_NODE;
		}

		bool is_type_array()
		{
			return true;
		};
	};

	struct MemberRefNode:
		public ExpressionNode
	{
		String *name;
		bool by_ptr;

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

		Type get_type()
		{
			return Node::FACTOR_CHAIN_NODE;
		}

		bool is_type_name(Scope *scope)
		{
			if(!factor->is_type_name(scope))
				return false;

			for(ExpressionList::Iterator i = chain.begin(); i; i++)
			{
				if((*i)->get_type() != Node::ARRAY_SUBSCRIPT_NODE)
					return false;
			}

			return true;
		};
	};

	struct IntegerNode:
		public ExpressionNode
	{
		int value;

		Type get_type()
		{
			return Node::INT_NODE;
		}
	};

	struct StringNode:
		public ExpressionNode
	{
		String *value;

		Type get_type()
		{
			return Node::STRING_NODE;
		}
	};

	struct FixedNode:
		public ExpressionNode
	{
		double value;

		Type get_type()
		{
			return Node::FIXED_NODE;
		}
	};

	struct BooleanNode:
		public ExpressionNode
	{
		bool value;

		Type get_type()
		{
			return Node::BOOL_NODE;
		}
	};

	struct NullNode:
		public ExpressionNode
	{
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

		Type get_type()
		{
			return Node::CALL_NODE;
		}
	};

};
