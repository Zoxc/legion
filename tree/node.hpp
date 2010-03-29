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

		T *first;
		T *last;

		class Iterator
		{
			private:
				NodeList<T> *list;
				T *prev;
				T *current;

			public:
				Iterator(NodeList<T> *list) : list(list), prev(0), current(list->first) {}

				void step()
				{
					prev = current;
					current = static_cast<T *>(current->next);
				}

				void replace(T *node)
				{
					if(prev)
						prev->next = node;
					else
						list->first = node;

					node->next = current->next;

					if(node->next == 0)
						list->last = node;

					current = node;
				}

				operator bool()
				{
					return current != 0;
				}

				T *operator ++()
				{
					step();
					return current;
				}

				T *operator ++(int)
				{
					step();
					return prev;
				}

				T *operator*()
				{
					return current;
				}
		};

		Iterator begin()
		{
			return Iterator(this);
		}
	};
};
