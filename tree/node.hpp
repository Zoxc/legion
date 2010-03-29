#pragma once
#include "../common.hpp"
#include "../range.hpp"

namespace Legion
{	
	struct Node
	{
		virtual bool find_declarations()
		{
			return false;
		}
	};
	
	struct ListNode:
		public Node
	{
		ListNode *next;
	};
	
	template<class T> struct NodeList
	{
		NodeList() : first(0), last(0) {}
		
		template<class T> T *add(MemoryPool *memory_pool)
		{
			T *node = new (memory_pool) T;
			
			append(node);
			
			return node;
		}
		
		void append(T *node)
		{
			if(!node)
				return;

			node->next = 0;
			
			if(last)
			{
				last->next = node;
				last = node;
			}
			else
			{
				first = node;
				last = node;
			}
		}

		T *next(T *node)
		{
			return static_cast<T *>(node->next);
		}

		T *first;
		T *last;
	};
};
