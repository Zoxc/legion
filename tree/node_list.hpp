#pragma once
#include "../common.hpp"

namespace Legion
{
	template<class T> struct NodeList
	{
		NodeList() : first(0), last(0) {}
		
		template<class N> N *add(MemoryPool *memory_pool)
		{
			N *node = new (memory_pool) N;
			
			append(node);
			
			return node;
		}
		
		template<class N> N *add(MemoryPool &memory_pool)
		{
			N *node = new (memory_pool) N;
			
			append(node);
			
			return node;
		}

		virtual void append(T *node)
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

	template<class T> struct CountedNodeList:
		public NodeList<T>
	{
		public:
			CountedNodeList() : size(0) {}

			size_t size;

			void append(T *node)
			{
				if(!node)
					return;

				NodeList<T>::append(node);

				size++;
			}
	};
};
