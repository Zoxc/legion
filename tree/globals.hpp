#pragma once
#include "../common.hpp"
#include "../range.hpp"
#include "node.hpp"
#include "types.hpp"
#include "statements.hpp"
#include "expressions.hpp"
#include "symbols.hpp"

namespace Legion
{
	class String;
	class Type;
	
	struct TypeSymbol;
	struct VarSymbol;
	struct FuncSymbol;
	struct ExpressionNode;
	
	struct NamespaceNode:
		public ListNode
	{
	};

	typedef NodeList<NamespaceNode> NamespaceList;

	struct PairNode:
		public Node
	{
		PairNode() : type(0), name(0) {}
		
		TypeNode *type;
		String *name;
		Range range;

		std::string string()
		{
			return wrap(type->string() + " " + name->string());
		}
	};

	
	struct FieldNode:
		public ListNode
	{
		PairNode pair;

		std::string string()
		{
			return wrap(pair.string());
		}
	};
	
	struct StructNode:
		public NamespaceNode
	{
		TypeSymbol *symbol;
		NodeList<FieldNode> fields;

		std::string string()
		{
			return wrap("struct " + symbol->name->string() + "\n{\n" + fields.join("    ", ";\n") + "}");
		}
	};
	
	struct TypedefNode:
		public NamespaceNode
	{
		TypedefNode() : pair(0), symbol(0) {}
		
		PairNode *pair;
		TypeSymbol *symbol;

		std::string string()
		{
			return wrap("typedef " + pair->string());
		}
	};
	
	struct GlobalNode:
		public NamespaceNode
	{
		GlobalNode() : symbol(0) {}
		
		PairNode *pair;
		bool is_const;
		bool is_static;
		VarSymbol *symbol;
		ExpressionNode *value;

		std::string string()
		{
			std::string result = pair->string();

			if(is_const)
				result = "const " + result;

			if(is_static)
				result = "static " + result;

			if(value)
				result += " = " + value->string();

			return wrap(result);
		}
	};
	
	struct ParamNode:
		public ListNode
	{
		PairNode pair;

		std::string string()
		{
			return wrap(pair.string());
		}
	};
		
	struct FuncHeadNode:
		public Node
	{
		FuncHeadNode() : symbol(0) {}
		
		PairNode *pair;
		bool is_native;
		bool is_static;
		FuncSymbol *symbol;
		NodeList<ParamNode> params;

		std::string string()
		{
			std::string result = pair->string() + "(";

			if(is_native)
				result = "native " + result;

			if(is_static)
				result = "static " + result;

			result += params.join(", ") + ")";

			return wrap(result);
		}
	};
	
	struct PrototypeNode:
		public NamespaceNode
	{
		FuncHeadNode *head;

		std::string string()
		{
			return wrap(head->string());
		}
	};
	
	struct Block;
	
	struct FuncNode:
		public NamespaceNode
	{
		FuncHeadNode *head;
		Block *body;

		bool find_declarations(Scope *scope)
		{
			if(body)
				body->find_declarations(scope);

			return false;
		}

		std::string string()
		{
			return wrap(head->string() + "\n" + body->string());
		}
	};
};
