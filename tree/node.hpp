#pragma once
#include "../common.hpp"
#include "../range.hpp"

namespace Legion
{	
	class Scope;

	struct Node
	{
		enum Type
		{
			NONE,

			STRUCT_NODE,
			TYPEDEF_NODE,
			GLOBAL_NODE,
			PROTOTYPE_NODE,
			FUNC_NODE,

			BLOCK_NODE,
			IF_NODE,
			WHILE_NODE,
			DO_NODE,
			RETURN_NODE,
			BREAK_NODE,
			CONTINUE_NODE,
			LOCAL_NODE,

			IDENT_NODE,
			BINARY_OP_NODE,
			UNARY_OP_NODE,
			ARRAY_SUBSCRIPT_NODE,
			ARRAY_DEF_NODE,
			MEMBER_REF_NODE,
			FACTOR_CHAIN_NODE,
			INT_NODE,
			STRING_NODE,
			FIXED_NODE,
			BOOL_NODE,
			NULL_NODE,
			CALL_NODE
		};

		virtual Type get_type()
		{
			return NONE;
		}

		virtual bool use_semi()
		{
			return true;
		}

		virtual std::string string();
		std::string wrap(std::string string);

		virtual bool find_declarations(Scope *scope)
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
		
		template<class N> N *add(MemoryPool *memory_pool)
		{
			N *node = new (memory_pool) N;
			
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

		std::string join(std::string seperator)
		{
			std::string result;

			for(Iterator i = begin(); i; i++)
			{
				result += (*i)->string();

				if((*i)->next)
					result += seperator;
			}

			return result;
		}

		std::string join(std::string pre, std::string post)
		{
			std::string result;

			for(Iterator i = begin(); i; i++)
				result += pre + (*i)->string() + post;

			return result;
		}

		Iterator begin()
		{
			return Iterator(this);
		}
	};
};
