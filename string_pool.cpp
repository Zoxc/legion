#include "string_pool.hpp"

namespace Legion
{
	StringPool::StringPool()
	{
		memset(table, 0, sizeof(table));
	}

	StringPool::~StringPool()
	{
	}
	
	void StringPool::setup(MemoryPool *memory_pool)
	{
		this->memory_pool = memory_pool;
	}
	
	String *StringPool::get(const char *string)
	{
		return get((const char_t *)string, strlen(string));
	}

	String *StringPool::get(const char_t *string, size_t length)
	{
		return get(string, string + length);
	}
	
	String *StringPool::get(Range *range)
	{
		return get(range->start, range->stop);
	}

	String *StringPool::get(const char_t *start, const char_t *stop)
	{
		/*
		 * Calculate hash
		 */
		
		size_t hash = 0;
		
		for(const char_t *c = start; c < stop; c++)
		{
			hash += *c;
			hash += hash << 10;
			hash ^= hash >> 6;
		}
		
		hash += hash << 3;
		hash ^= hash >> 11;
		hash += hash << 15;
		
		
		/* 
		 * Look for the string in the table
		 */
		 
		size_t index = hash % table_size;
		size_t length = (size_t)(stop - start);
		String *str = table[index];
		String *tail = str;
		
		while(str)
		{
			if(str->length == length)
			{
				if(memcmp(str->c_str, start, length) == 0)
					return str;		
			}
			
			tail = str;
			str = str->next;
		}
		
		
		/* 
		 * Create a new string
		 */
		
		String *result = new (memory_pool) String;
		result->next = 0;
		result->length = length;
		
		char_t *result_str = new (memory_pool) char_t[length + 1];
		memcpy(result_str, start, length);
		result_str[length] = 0;
		
		result->c_str = result_str;
		
		if(tail)
			tail->next = result;
		else
			table[index] = result;
			
		return result;
	}
};
