#pragma once
#include "common.hpp"
#include "range.hpp"

namespace Legion
{
	class MemoryPool;
	
	class String
	{
		public:
			String *next;
			size_t length;
			char_t *cstr;
	};
	
	class StringPool
	{
		private:
			MemoryPool *memory_pool;
			static const size_t table_size = 0x1000;
			String *table[table_size];
			
		public:
			StringPool();
			~StringPool();
			
			String *get(Range *range);
			
			void setup(MemoryPool *memory_pool);
	};
};
