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
		virtual StatementNode *get_declaration(Document &document)
		{
			return 0;
		}
	};

	typedef NodeList<StatementNode> StatementList;
	
	struct Block:
		public StatementNode
	{
		StatementList statements;
		Scope *scope;

		bool find_declarations(Document *document);

		Node::Type get_type()
		{
			return BLOCK_NODE;
		}
	};

	struct ControlFlowNode:
		public StatementNode
	{
		virtual bool use_semi()
		{
			return false;
		}
	};
	
	struct IfNode:
		public ControlFlowNode
	{
		IfNode() : condition(0), do_true(0), do_false(0) {}
		
		ExpressionNode *condition;
		Block *do_true;
		Block *do_false;

		Node::Type get_type()
		{
			return IF_NODE;
		}
	};
	
	struct WhileNode:
		public ControlFlowNode
	{
		WhileNode() : condition(0), body(0) {}
		
		ExpressionNode *condition;
		Block *body;

		Node::Type get_type()
		{
			return WHILE_NODE;
		}
	};
	
	struct DoNode:
		public ControlFlowNode
	{
		DoNode() : body(0), condition(0) {}
		
		Block *body;
		ExpressionNode *condition;

		Node::Type get_type()
		{
			return DO_NODE;
		}
	};

	struct ReturnNode:
		public StatementNode
	{
		ExpressionNode *value;
		bool has_value;

		Node::Type get_type()
		{
			return RETURN_NODE;
		}
	};

	struct BreakNode:
		public StatementNode
	{
		Node::Type get_type()
		{
			return BREAK_NODE;
		}
	};

	struct ContinueNode:
		public StatementNode
	{
		Node::Type get_type()
		{
			return CONTINUE_NODE;
		}
	};

	struct LocalNode:
		public StatementNode
	{
		ExpressionNode *type_expression;
		TypeNode *type;
		VarSymbol *symbol;
		ExpressionNode *value;
		bool has_value;
		bool is_const;

		Node::Type get_type()
		{
			return LOCAL_NODE;
		}

		StatementNode *get_declaration(Document &document);
	};
};
