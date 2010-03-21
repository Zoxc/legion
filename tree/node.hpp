#pragma once
#include "../common.hpp"
#include "../range.hpp"

namespace Legion
{
	class String;
	class Type;
	
	struct Node:
		public Range
	{
		enum Type {
			NONE,
			TREE,
			STRUCT,
			FUNCTION,
			VARIABLE
		};
		
		Node(Type type) : type(type), first_child(0), last_child(0), next(0)
		{
		}
		
		template<class T> T *add(MemoryPool *memory_pool)
		{
			T *node = new (memory_pool) T;
			
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

		Type type;
		Node *first_child;
		Node *last_child;
		Node *next;
	};
	
	template <Node::Type type> struct NodeType:
		public Node
	{
		public:
			NodeType() : Node(type)
			{
			}
	};

	struct TypeSymbol;
	
	struct StructNode:
		public NodeType<Node::STRUCT>
	{
		TypeSymbol *symbol;
	};
};
