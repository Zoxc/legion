#pragma once
#include "../common.hpp"
#include "../range.hpp"
#include "node.hpp"
#include "types.hpp"
#include "statements.hpp"

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
	};

	
	struct FieldNode:
		public ListNode
	{
		PairNode pair;
	};
	
	struct StructNode:
		public NamespaceNode
	{
		TypeSymbol *symbol;
		NodeList<FieldNode> fields;
	};
	
	struct TypedefNode:
		public NamespaceNode
	{
		TypedefNode() : pair(0), symbol(0) {}
		
		PairNode *pair;
		TypeSymbol *symbol;
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
	};
	
	struct ParamNode:
		public ListNode
	{
		PairNode pair;
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
	};
	
	struct PrototypeNode:
		public NamespaceNode
	{
		FuncHeadNode *head;
	};
	
	struct Block;
	
	struct FuncNode:
		public NamespaceNode
	{
		FuncHeadNode *head;
		Block *body;

		bool find_declarations()
		{
			if(body)
				body->find_declarations();

			return false;
		}
	};
};
