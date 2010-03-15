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

	String *StringPool::get(Range *range)
	{
		/*
		 * Calculate hash
		 */
		
		size_t hash = 0;
		char_t *stop = range->stop;
		
		for(char_t *c = range->start; c < stop; c++)
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
		size_t length = range->length();
		String *str = table[index];
		String *tail = str;
		char_t *cstr = range->start;
		
		while(str)
		{
			if(str->length != length)
				continue;
				
			if(memcmp(str->cstr, cstr, length) == 0)
				return str;
				
			tail = str;
			str = str->next;
		}
		
		
		/* 
		 * Create a new string
		 */
		String *result = new (memory_pool) String;
		result->next = 0;
		result->length = length;
		result->cstr = new (memory_pool) char_t[length + 1];
		memcpy(result->cstr, cstr, length);
		result->cstr[length] = 0;
		
		if(tail)
			tail->next = result;
		else
			table[index] = result;
			
		return result;
	}
};
