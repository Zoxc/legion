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

		virtual void setup_type(Document &document, LocalNode &local, bool name)
		{
			get_range().report_type_modifier(document);
		}

	};

	typedef NodeList<ExpressionNode> ExpressionList;

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

		Range get_range()
		{
			return *range;
		}

		Type *get_type(Document &document, SymbolList &stack);
	};

	struct BinaryOpNode:
		public ExpressionNode
	{
		ExpressionNode *left;
		ExpressionNode *right;
		Range *range;
		Lexeme::Type op;

		StatementNode *get_declaration(Document &document);

		Range get_range()
		{
			Range range = left->get_range();
			Range end = right->get_range();
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

		Type *get_type(Document &document, SymbolList &stack);

		void setup_type(Document &document, LocalNode &local, bool name);

		NodeType node_type()
		{
			return Node::BINARY_OP_NODE;
		}
	};

	struct AssignNode:
		public BinaryOpNode
	{
		StatementNode *get_declaration(Document &document)
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

		Type *get_type(Document &document, SymbolList &stack)
		{
			Type *left_type = left->get_type(document, stack);

			left_type->compatible(document, stack, right);

			return left_type;
		}

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
		Type *get_type(Document &document, SymbolList &stack);

		Range get_range()
		{
			Range range = *this->range;
			Range end = value->get_range();
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
		
		Range get_range()
		{
			return *range;
		}

		Type *get_type(Document &document, SymbolList &stack);
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
		
		Range get_range()
		{
			Range range;

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

		Range get_range()
		{
			return *range;
		}
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

		Range get_range()
		{
			Range range;

			assert(0);

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
		Type *get_type(Document &document, SymbolList &stack);
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

		Range get_range()
		{
			return *range;
		}

		Type *get_type(Document &document, SymbolList &stack);
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

		Range get_range()
		{
			return *range;
		}

		Type *get_type(Document &document, SymbolList &stack);
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

		Range get_range()
		{
			return *range;
		}

		Type *get_type(Document &document, SymbolList &stack);
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

		Range get_range()
		{
			return *range;
		}

		Type *get_type(Document &document, SymbolList &stack);
	};

	struct NullNode:
		public ExpressionNode
	{
		Range *range;

		NodeType node_type()
		{
			return Node::NULL_NODE;
		}

		Range get_range()
		{
			return *range;
		}

		Type *get_type(Document &document, SymbolList &stack);
	};

	struct CallNode:
		public ExpressionNode
	{
		IdentNode *ident;
		ExpressionList arguments;

		NodeType node_type()
		{
			return Node::CALL_NODE;
		}

		Range get_range()
		{
			return *ident->range;
		}

		Type *get_type(Document &document, SymbolList &stack);
	};
};
