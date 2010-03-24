#pragma once
#include "../common.hpp"
#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"
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
			Scope(Scope *parent, MemoryPool *memory_pool);
			~Scope();
			
			Scope *parent;
			
			Symbol *get(String *name);
			bool set(String *name, Symbol *symbol);

			bool declare_symbol(Document *document, Symbol *symbol)
			{
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

			template<class T>  T *declare(Document *document, Parser *parser, T *type)
			{
				T *result = type;
				
				if(parser->lexer.lexeme.type == Lexeme::IDENT)
				{
					result->range = new (memory_pool) Range;
					result->range->capture(&parser->lexer.lexeme);
					result->name = parser->lexer.lexeme.value;
					
					declare_symbol(document, result);
					
					parser->step();
				}
				else
					parser->expected(Lexeme::IDENT);
				
				return result;
			}
/*			
			template<class T>  T *declare(Document *document, String *name, Range *range, T *type)
			{
				T *result = type;
				
				result->capture(range);
				
				declare_symbol(document, result);
				
				return result;
			}
		*/
			template<class T>  T *declare(Document *document, Parser *parser)
			{
				return declare<T>(document, parser, new (memory_pool) T);
			}
			
			template<class T>  T *declare(Document *document, PairNode *pair)
			{
				T *result = new (memory_pool) T;
				
				result->range = &pair->range;
				result->name = pair->name;
				
				declare_symbol(document, result);
				
				return result;
			}
			/*
			template<class T>  T *declare(Document *document, String *name, Range *range)
			{
				return declare<T>(document, name, range, new (memory_pool) T);
			}*/
	};
};
