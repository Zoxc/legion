#pragma once
#include "../common.hpp"
#include "../range.hpp"
#include "node_list.hpp"
#include "types.hpp"

namespace Legion
{
	class String;
	class Type;
	
	struct TypeSymbol;
	struct VarSymbol;
	struct FuncSymbol;
	
	struct PairNode
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
		public ListNode
	{
		TypeSymbol *symbol;
		NodeList fields;
	};
	
	struct TypedefNode:
		public ListNode
	{
		TypedefNode() : symbol(0) {}
		
		PairNode pair;
		TypeSymbol *symbol;
	};
	
	struct GlobalNode:
		public ListNode
	{
		GlobalNode() : symbol(0) {}
		
		PairNode *pair;
		bool is_const;
		bool is_static;
		VarSymbol *symbol;
	};
	
	struct ParamNode:
		public ListNode
	{
		PairNode pair;
	};
		
	struct FuncHeadNode
	{
		FuncHeadNode() : symbol(0) {}
		
		PairNode *pair;
		bool is_native;
		bool is_static;
		FuncSymbol *symbol;
		NodeList params;
	};
	
	struct PrototypeNode:
		public ListNode
	{
		FuncHeadNode *head;
	};
	
	struct FuncNode:
		public ListNode
	{
		FuncHeadNode *head;
	};
};
