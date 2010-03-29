#pragma once
#include "../common.hpp"
#include "../lexer/lexer.hpp"
#include "symbols.hpp"

namespace Legion
{
	class String;
	class StringPool;
	struct Symbol;
	class Parser;
	class Document;
	
	class Scope
	{
		private:
			Symbol **table;
			size_t size;
			size_t mask;
			size_t entries;
			
			Symbol *store(Symbol **table, size_t mask, String *name, Symbol *symbol);
			void expand();
		public:
			enum Type
			{
				ROOT,
				FUNCTION,
				LOOP,
				CONDITIONAL,
				EMPTY
			};
			
			Scope(Scope *parent, Type type, MemoryPool *memory_pool);
			~Scope();

			MemoryPool *memory_pool;
			
			Type type;
			Scope *parent;
			
			Symbol *get(String *name);
			Symbol *set(String *name, Symbol *symbol);

			Symbol *declare_symbol(Symbol *symbol)
			{
				if(type == LOOP)
					return parent->declare_symbol(symbol);
				
				if(!symbol)
					return 0;
				
				if(!symbol->name)
					return 0;
				
				return set(symbol->name, symbol);
			}

	};
};
