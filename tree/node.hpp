#pragma once
#include "../common.hpp"
#include "../range.hpp"

namespace Legion
{
	class String;
	class Type;
	
	struct TypeSymbol;
	struct VarSymbol;
	struct FuncSymbol;
	
	struct Node:
		public Range
	{
		Node *next;
		
		template<class T> static T *create(MemoryPool *memory_pool, Range *range)
		{
			T *node = new (memory_pool) T;
			
			node->capture(range);
			
			return node;
		}
		
		template<class T> static T *create(MemoryPool *memory_pool)
		{
			return new (memory_pool) T;
		}
	};
	
	struct NodeList
	{
		NodeList() : first_child(0), last_child(0) {}
		
		template<class T> T *add(MemoryPool *memory_pool)
		{
			T *node = new (memory_pool) T;
			
			append(node);
			
			return node;
		}
		
		template<class T> T *add(MemoryPool *memory_pool, Range *range)
		{
			T *node = new (memory_pool) T;
			
			node->capture(range);
			
			append(node);
			
			return node;
		}
		
		void append(Node *node)
		{
			node->next = 0;
			
			if(last_child)
			{
				last_child->next = node;
				last_child = node;
			}
			else
			{
				first_child = node;
				last_child = node;
			}
		}

		Node *first_child;
		Node *last_child;
	};
	
	struct TypeNode:
		public Node
	{
		TypeNode() : base(0) {}
		
		TypeNode *base;
	};
	
	struct TypePtrNode:
		public TypeNode
	{
		TypeNode *base;
	};
	
	struct TypeBaseNode:
		public TypeNode
	{
		String *type;
	};
	
	struct PairNode:
		public Node
	{
		PairNode() : type(0), name(0) {}
		
		TypeNode *type;
		String *name;
	};
	
	struct StructNode:
		public Node
	{
		TypeSymbol *symbol;
		NodeList fields;
	};
	
	struct TypedefNode:
		public PairNode
	{
		TypedefNode() : symbol(0) {}
		
		TypeSymbol *symbol;
	};
	
	struct GlobalNode:
		public Node
	{
		GlobalNode() : symbol(0) {}
		
		PairNode *pair;
		bool is_const;
		bool is_static;
		VarSymbol *symbol;
	};
	
	struct FuncParamNode:
		public Node
	{
		PairNode *pair;
	};
	
	struct FuncHeadNode:
		public Node
	{
		FuncHeadNode() : symbol(0) {}
		
		PairNode *pair;
		bool is_native;
		bool is_static;
		FuncSymbol *symbol;
		NodeList params;
	};
	
	struct FuncNode:
		public Node
	{
		FuncHeadNode *head;
	};
};
