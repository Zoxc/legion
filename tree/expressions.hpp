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

		virtual void setup_local(LocalNode *local, bool name, MemoryPool *memory_pool)
		{
		}
	};

	typedef NodeList<ExpressionNode> ExpressionList;

	struct IdentNode:
		public ExpressionNode
	{
		String *ident;
		Range *range;

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
			return true;
		};
		
		void setup_local(LocalNode *local, bool name, MemoryPool *memory_pool)
		{
			if(name)
			{
				local->symbol->name = ident;
				local->symbol->range = range;
			}
			else
				local->type->name = ident;
		}
	};

	struct BinaryOpNode:
		public ExpressionNode
	{
		ExpressionNode *left;
		ExpressionNode *right;
		Range *range;
		Lexeme::Type op;

		StatementNode *get_declaration(Document *document);

		bool is_type_name(Scope *scope)
		{
			return op == Lexeme::MUL && left->is_type_name(scope) && right->is_type_array();
		};
		
		void setup_local(LocalNode *local, bool name, MemoryPool *memory_pool)
		{
			left->setup_local(local, name, memory_pool);
			
			local->type->modifiers.add<TypePointerNode>(memory_pool);
			
			right->setup_local(local, name, memory_pool);
		}

		Type get_type()
		{
			return Node::BINARY_OP_NODE;
		}
	};

	struct AssignNode:
		public BinaryOpNode
	{
		StatementNode *get_declaration(Document *document)
		{
			LocalNode *local = (LocalNode *)left->get_declaration(document);

			if(local)
			{
				local->value = right;
				local->has_value = true;

				if(op != Lexeme::ASSIGN)
					range->report(document, "Unexpected assignment with operator " + Lexeme::describe(range, op));

				return local;
			}
			else
				return 0;
		}

		Type get_type()
		{
			return Node::ASSIGN_NODE;
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

		void setup_local(LocalNode *local, bool name, MemoryPool *memory_pool)
		{
			value->setup_local(local, name, memory_pool);
			
			local->type->modifiers.add<TypePointerNode>(memory_pool);
		}
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
		}

		void setup_local(LocalNode *local, bool name, MemoryPool *memory_pool)
		{
			for(ExpressionList::Iterator i = sizes.begin(); i; i++)
			{
				TypeArrayNode *node = local->type->modifiers.add<TypeArrayNode>(memory_pool);
	
				node->size = *i;
			}
		}
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
		}

		void setup_local(LocalNode *local, bool name, MemoryPool *memory_pool)
		{
			for(ExpressionList::Iterator i = chain.begin(); i; i++)
			{
				TypeArrayNode *node = local->type->modifiers.add<TypeArrayNode>(memory_pool);
	
				node->size = ((ArraySubscriptNode *)*i)->index;
			}
		}
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
