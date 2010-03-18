#pragma once
#include "../common.hpp"
#include "../string_pool.hpp"

namespace Legion
{
	class Symbol
	{
		public:
			String *name;
			Symbol *next;
	};
	
	class Scope
	{
		private:
			Symbol **table;
			size_t size;
			size_t mask;
			size_t entries;
			
			StringPool *string_pool;
			MemoryPool *memory_pool;
			
			bool store(Symbol **table, size_t mask, String *name, Symbol *symbol);
			void expand();
		public:
			Scope();
			~Scope();
			
			Scope *parent;
			
			Symbol *get(String *name);
			bool set(String *name, Symbol *symbol);
			
			void setup(StringPool *string_pool, MemoryPool *memory_pool);
	};
};
