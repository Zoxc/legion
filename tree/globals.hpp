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

		std::string string();
	};

	
	struct FieldNode:
		public ListNode
	{
		PairNode pair;

		std::string string();
	};
	
	struct StructNode:
		public NamespaceNode
	{
		TypeSymbol *symbol;
		NodeList<FieldNode> fields;

		Node::Type get_type()
		{
			return STRUCT_NODE;
		}

		std::string string();
	};
	
	struct TypedefNode:
		public NamespaceNode
	{
		TypedefNode() : pair(0), symbol(0) {}
		
		PairNode *pair;
		TypeSymbol *symbol;

		Node::Type get_type()
		{
			return TYPEDEF_NODE;
		}

		std::string string();
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

		Node::Type get_type()
		{
			return GLOBAL_NODE;
		}

		std::string string();
	};
	
	struct ParamNode:
		public ListNode
	{
		PairNode pair;

		std::string string();
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

		std::string string();
	};
	
	struct PrototypeNode:
		public NamespaceNode
	{
		FuncHeadNode *head;

		Node::Type get_type()
		{
			return PROTOTYPE_NODE;
		}

		std::string string();
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

		bool use_semi()
		{
			return false;
		}

		Node::Type get_type()
		{
			return FUNC_NODE;
		}

		std::string string();
	};
};
