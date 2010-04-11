#pragma once
#include "common.hpp"

namespace Legion
{
	template<class T> class ListFunctions
	{
		public:
			static T *get_next(T *item)
			{
				return static_cast<T *>(item->next);
			}

			static void set_next(T *item, T *next)
			{
				item->next = next;
			}
	};

	template<class T, class F> struct ListBase
	{
		ListBase() : first(0), last(0) {}
		
		T *first;
		T *last;
		size_t size;

		template<class N> N *add(MemoryPool *memory_pool)
		{
			return add<N>(*memory_pool);
		}
		
		template<class N> N *add(MemoryPool &memory_pool)
		{
			N *node = new (memory_pool) N;
			
			append(node);
			
			return node;
		}

		void append(T *node)
		{
			if(!node)
				return;

			F::set_next(node, 0);

			size++;
			
			if(last)
			{
				F::set_next(last, node);
				last = node;
			}
			else
			{
				first = node;
				last = node;
			}
		}

		class Iterator
		{
			private:
				ListBase<T, F> &list;
				T *prev;
				T *current;

			public:
				Iterator(ListBase<T, F> &list) : list(list), prev(0), current(list.first) {}

				void step()
				{
					prev = current;
					current = F::get_next(current);
				}

				void replace(T *node)
				{
					if(prev)
						F::set_next(prev, node);
					else
						list.first = node;

					F::set_next(node, F::get_next(current));

					if(F::get_next(node) == 0)
						list.last = node;

					current = node;
				}

				void insert(T *node)
				{
					if(prev)
						F::set_next(prev, node);
					else
						list.first = node;

					F::set_next(node, current);

					if(current == 0)
						list.last = node;
				}

				operator bool()
				{
					return current != 0;
				}

				T &operator ++()
				{
					step();
					return *current;
				}

				T &operator ++(int)
				{
					step();
					return *prev;
				}

				T *operator*()
				{
					return current;
				}

				T &operator ()()
				{
					return *current;
				}
		};

		Iterator begin()
		{
			return Iterator(*this);
		}
	};

	template<class T> struct List:
		public ListBase<T, ListFunctions<T> >
	{
	};
};
