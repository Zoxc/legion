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
		StatementNode *get_declaration()
		{
			return 0;
		}

		virtual bool use_semi()
		{
			return true;
		}
	};

	typedef NodeList<StatementNode> StatementList;
	
	struct Block:
		public StatementNode
	{
		StatementList statements;
		Scope *scope;

		bool find_declarations(Scope *scope)
		{
			for(StatementList::Iterator i = statements.begin(); i; i++)
			{
				if((*i)->find_declarations(this->scope))
					i.replace((*i)->get_declaration());
			}

			return false;
		}

		std::string string()
		{
			std::string result = "{\n";

			for(StatementList::Iterator i = statements.begin(); i; i++)
			{
				result += "    " + (*i)->string();

				if((*i)->use_semi())
					result += ";\n";
				else
					result += "\n";
			}

			return result + "}";
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

		std::string string();
	};
	
	struct WhileNode:
		public ControlFlowNode
	{
		WhileNode() : condition(0), body(0) {}
		
		ExpressionNode *condition;
		Block *body;
		
		std::string string();
	};
	
	struct DoNode:
		public ControlFlowNode
	{
		DoNode() : body(0), condition(0) {}
		
		Block *body;
		ExpressionNode *condition;
		
		std::string string();
	};

	struct ReturnNode:
		public StatementNode
	{
		ExpressionNode *value;

		std::string string();
	};

	struct BreakNode:
		public StatementNode
	{
		std::string string()
		{
			return wrap("break");
		}
	};

	struct ContinueNode:
		public StatementNode
	{
		std::string string()
		{
			return wrap("continue");
		}
	};

	struct LocalNode:
		public StatementNode
	{
		ExpressionNode *type;
		String *name;
		ExpressionNode *value;
		bool is_const;

		std::string string();
	};
};
