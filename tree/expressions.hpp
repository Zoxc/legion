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

		virtual bool is_type_name(Document &document, bool lookup)
		{
			return false;
		}
		
		virtual bool is_type_array()
		{
			return false;
		}

		virtual void setup_type(Document &document, LocalNode &local, bool name);
	};

	typedef List<ExpressionNode> ExpressionList;

	struct IdentNode:
		public ExpressionNode
	{
		String *ident;
		Range *range;

		NodeType node_type()
		{
			return Node::IDENT_NODE;
		}

		bool is_type_name(Document &document, bool lookup);
		
		bool is_declaration_name()
		{
			return true;
		};
		
		void setup_type(Document &document, LocalNode &local, bool name);

		Range &get_range(MemoryPool &memory_pool)
		{
			return *range;
		}

		Type *validate(ValidationArgs &args);
	};

	struct BinaryOpNode:
		public ExpressionNode
	{
		ExpressionNode *left;
		ExpressionNode *right;
		Range *range;
		Lexeme::Type op;

		StatementNode *get_declaration(Document &document);

		Range &get_range(MemoryPool &memory_pool)
		{
			Range &range = left->get_range(memory_pool).dup(memory_pool);
			Range &end = right->get_range(memory_pool);
			range.expand(end);
			return range;
		}

		bool is_type_name(Document &document, bool lookup)
		{
			return op == Lexeme::MUL && left->is_type_name(document, lookup) && right->is_type_array();
		};
		
		bool is_declaration_name()
		{
			return op == Lexeme::MUL && right->is_declaration_name();
		};

		Type *validate_op(ValidationArgs &args, ExpressionNode *node, Lexeme::Type op, Type *left_type, Type *right_type);
		Type *validate(ValidationArgs &args);

		void setup_type(Document &document, LocalNode &local, bool name);

		NodeType node_type()
		{
			return Node::BINARY_OP_NODE;
		}
	};

	struct AssignNode:
		public BinaryOpNode
	{
		StatementNode *get_declaration(Document &document);

		Type *validate(ValidationArgs &args);

		NodeType node_type()
		{
			return Node::ASSIGN_NODE;
		}
	};

	struct UnaryOpNode:
		public ExpressionNode
	{
		Lexeme::Type op;
		ExpressionNode *value;
		Range *range;
		
		NodeType node_type()
		{
			return Node::UNARY_OP_NODE;
		}

		bool is_type_array()
		{
			return op == Lexeme::MUL && value->is_type_array();
		};

		bool is_declaration_name()
		{
			return op == Lexeme::MUL && value->is_declaration_name();
		};

		void setup_type(Document &document, LocalNode &local, bool name);
		Type *validate(ValidationArgs &args);

		Range &get_range(MemoryPool &memory_pool)
		{
			Range &range = this->range->dup(memory_pool);
			Range &end = value->get_range(memory_pool);
			range.expand(end);
			return range;
		}
	};

	struct ArraySubscriptNode:
		public ExpressionNode
	{
		ExpressionNode *index;
		Range *range;

		NodeType node_type()
		{
			return Node::ARRAY_SUBSCRIPT_NODE;
		}
		
		Range &get_range(MemoryPool &memory_pool)
		{
			return *range;
		}

		Type *validate(ValidationArgs &args);
	};

	struct ArrayDefNode:
		public ExpressionNode
	{
		ExpressionList sizes;

		NodeType node_type()
		{
			return Node::ARRAY_DEF_NODE;
		}

		bool is_type_array(Document &document, bool report)
		{
			return true;
		}

		void setup_type(Document &document, LocalNode &local, bool name);
		
		Range &get_range(MemoryPool &memory_pool)
		{
			Range &range = *new (memory_pool) Range;

			assert(0);

			return range;
		}
	};

	struct MemberRefNode:
		public ExpressionNode
	{
		String *name;
		bool by_ptr;
		Range *range;

		NodeType node_type()
		{
			return Node::MEMBER_REF_NODE;
		}

		Range &get_range(MemoryPool &memory_pool)
		{
			return *range;
		}

		Type *validate(ValidationArgs &args);
	};

	struct FactorChainNode:
		public ExpressionNode
	{
		ExpressionNode *factor;
		ExpressionList chain;

		NodeType node_type()
		{
			return Node::FACTOR_CHAIN_NODE;
		}

		Range &get_range(MemoryPool &memory_pool)
		{
			
			Range &range = factor->get_range(memory_pool).dup(memory_pool);
			Range &end = chain.last->get_range(memory_pool);
			range.expand(end);
			return range;
		}

		bool is_type_name(Document &document, bool lookup)
		{
			if(!factor->is_type_name(document, lookup))
				return false;

			for(ExpressionList::Iterator i = chain.begin(); i; i++)
			{
				if((*i)->node_type() != Node::ARRAY_SUBSCRIPT_NODE)
					return false;
			}

			return true;
		}

		void setup_type(Document &document, LocalNode &local, bool name);
		Type *validate(ValidationArgs &args);
	};

	struct IntegerNode:
		public ExpressionNode
	{
		int value;
		Range *range;

		NodeType node_type()
		{
			return Node::INT_NODE;
		}

		Range &get_range(MemoryPool &memory_pool)
		{
			return *range;
		}

		Type *validate(ValidationArgs &args);
	};

	struct StringNode:
		public ExpressionNode
	{
		String *value;
		Range *range;

		NodeType node_type()
		{
			return Node::STRING_NODE;
		}

		Range &get_range(MemoryPool &memory_pool)
		{
			return *range;
		}

		Type *validate(ValidationArgs &args);
	};

	struct FixedNode:
		public ExpressionNode
	{
		double value;
		Range *range;

		NodeType node_type()
		{
			return Node::FIXED_NODE;
		}

		Range &get_range(MemoryPool &memory_pool)
		{
			return *range;
		}

		Type *validate(ValidationArgs &args);
	};

	struct BooleanNode:
		public ExpressionNode
	{
		bool value;
		Range *range;

		NodeType node_type()
		{
			return Node::BOOL_NODE;
		}

		Range &get_range(MemoryPool &memory_pool)
		{
			return *range;
		}

		Type *validate(ValidationArgs &args);
	};

	struct NullNode:
		public ExpressionNode
	{
		Range *range;

		NodeType node_type()
		{
			return Node::NULL_NODE;
		}

		Range &get_range(MemoryPool &memory_pool)
		{
			return *range;
		}

		Type *validate(ValidationArgs &args);
	};

	struct CallNode:
		public ExpressionNode
	{
		IdentNode *ident;
		List<ExpressionNode> arguments;

		NodeType node_type()
		{
			return Node::CALL_NODE;
		}

		Range &get_range(MemoryPool &memory_pool)
		{
			return *ident->range;
		}

		Type *validate(ValidationArgs &args);
	};

	struct GroupedExpressionNode:
		public ExpressionNode
	{
		Range range;
		ExpressionNode *value;

		NodeType node_type()
		{
			return Node::GROUPED_EXPRESSION_NODE;
		}

		Range &get_range(MemoryPool &memory_pool)
		{
			return range;
		}

		Type *validate(ValidationArgs &args)
		{
			if(value)
				return value->validate(args);
			else
				return 0;
		}
	};
};
