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
			
			MemoryPool *memory_pool;
			
			bool store(Symbol **table, size_t mask, String *name, Symbol *symbol);
			void expand();
		public:
			enum Type
			{
				ROOT,
				FUNCTION,
				LOOP
			};
			
			Scope(Scope *parent, Type type, MemoryPool *memory_pool);
			~Scope();
			
			Type type;
			Scope *parent;
			
			Symbol *get(String *name);
			bool set(String *name, Symbol *symbol);

			bool declare_symbol(Document *document, Symbol *symbol)
			{
				if(type == LOOP)
					return parent->declare_symbol(document, symbol);
				
				if(!symbol)
					return false;
				
				if(!symbol->name)
					return false;
					
				if(!set(symbol->name, symbol))
				{
					symbol->range->report(document, "Redeclared identifier '" + symbol->name->string() + "'");
					
					return false;
				}
				
				return true;
			}

	};
};
