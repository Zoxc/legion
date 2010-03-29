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
	};

	typedef NodeList<StatementNode> StatementList;
	
	struct Block:
		public StatementNode
	{
		StatementList statements;
		Scope *scope;

		bool find_declarations()
		{
			for(StatementList::Iterator i = statements.begin(); i; i++)
			{
				/*if((*i)->find_declarations());
					i.replace((*i)->get_declaration());*/
			}

			return false;
		}
	};
	
	struct IfNode:
		public StatementNode
	{
		IfNode() : condition(0), do_true(0), do_false(0) {}
		
		ExpressionNode *condition;
		Block *do_true;
		Block *do_false;
	};
	
	struct WhileNode:
		public StatementNode
	{
		WhileNode() : condition(0), body(0) {}
		
		ExpressionNode *condition;
		Block *body;
	};
	
	struct DoNode:
		public StatementNode
	{
		DoNode() : body(0), condition(0) {}
		
		Block *body;
		ExpressionNode *condition;
	};

	struct ReturnNode:
		public StatementNode
	{
		ExpressionNode *value;
	};

	struct BreakNode:
		public StatementNode
	{
	};

	struct ContinueNode:
		public StatementNode
	{
	};

	struct LocalNode:
		public StatementNode
	{
		ExpressionNode *type;
		String *name;
		ExpressionNode *value;
		bool is_const;
	};
};
