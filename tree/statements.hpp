#pragma once
#include "../common.hpp"
#include "../range.hpp"
#include "node.hpp"
#include "types.hpp"

namespace Legion
{
	class String;
	class Type;
	class Scope;
	
	struct TypeSymbol;
	struct VarSymbol;
	struct FuncSymbol;
	
	struct ExpressionNode;
	
	struct StatementNode:
		public ListNode
	{
		virtual Range get_range() = 0;

		virtual StatementNode *get_declaration(Document &document)
		{
			return 0;
		}
	};

	typedef List<StatementNode> StatementList;
	
	struct Block:
		public StatementNode
	{
		StatementList statements;
		Scope *scope;
		Range range;

		bool find_declarations(Document *document);
		Type *validate(ValidationArgs &args);

		Node::NodeType node_type()
		{
			return BLOCK_NODE;
		}

		Range get_range()
		{
			return range;
		}
	};

	struct ControlFlowNode:
		public StatementNode
	{
		ControlFlowNode() : condition(0) {}

		ExpressionNode *condition;

		Type *validate(ValidationArgs &args);
		virtual bool use_semi()
		{
			return false;
		}
	};
	
	struct IfNode:
		public ControlFlowNode
	{
		IfNode() : do_true(0), do_false(0) {}
		
		Block *do_true;
		Block *do_false;
		Range range;

		Node::NodeType node_type()
		{
			return IF_NODE;
		}

		Range get_range()
		{
			return range;
		}
	};
	
	struct WhileNode:
		public ControlFlowNode
	{
		WhileNode() : body(0) {}
		
		Block *body;
		Range range;

		Node::NodeType node_type()
		{
			return WHILE_NODE;
		}

		Range get_range()
		{
			return range;
		}
	};
	
	struct DoNode:
		public ControlFlowNode
	{
		DoNode() : body(0) {}
		
		Block *body;
		Range range;

		Node::NodeType node_type()
		{
			return DO_NODE;
		}

		Range get_range()
		{
			return range;
		}
	};

	struct ReturnNode:
		public StatementNode
	{
		ExpressionNode *value;
		bool has_value;
		Range range;

		Node::NodeType node_type()
		{
			return RETURN_NODE;
		}

		Range get_range()
		{
			return range;
		}
	};

	struct BreakNode:
		public StatementNode
	{
		Range range;

		Node::NodeType node_type()
		{
			return BREAK_NODE;
		}

		Range get_range()
		{
			return range;
		}
	};

	struct ContinueNode:
		public StatementNode
	{
		Range range;

		Node::NodeType node_type()
		{
			return CONTINUE_NODE;
		}

		Range get_range()
		{
			return range;
		}
	};

	struct LocalNode:
		public StatementNode
	{
		LocalNode() : type_node(0), type(0)  {}

		ExpressionNode *type_expression;
		TypeNode *type_node;
		Type *type;
		VarSymbol *symbol;
		ExpressionNode *value;
		bool has_value;
		bool is_const;
		Range range;

		Node::NodeType node_type()
		{
			return LOCAL_NODE;
		}

		Range get_range()
		{
			return range;
		}

		StatementNode *get_declaration(Document &document);

		Type *validate(ValidationArgs &args);
	};
};
