#pragma once
#include "../common.hpp"
#include "../range.hpp"

namespace Legion
{
	struct ListNode
	{
		ListNode *next;
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
		
		void append(ListNode *node)
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

		ListNode *first_child;
		ListNode *last_child;
	};
};