#pragma once
#include "common.hpp"

namespace Legion
{
	class Range;
	
	class String
	{
		public:
			String *next;
			size_t length;
			const char_t *c_str;
	};
	
	class StringPool
	{
		private:
			MemoryPool *memory_pool;
			static const size_t table_size = 0x1000;
			String *table[table_size];
			
		public:
			StringPool(MemoryPool *memory_pool);
			~StringPool();
			
			String *get(Range *range);
			String *get(const char_t *string, size_t length);
			String *get(const char_t *start, const char_t *stop);
			String *get(const char *string);
	};
};
