#pragma once
#include "../common.hpp"
#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"

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

			template<class T>  T *declare(Document *document, Parser *parser, T *type)
			{
				T *result = type;
				
				result->capture(&parser->lexer.lexeme);
				
				if(parser->lexer.lexeme.type == Lexeme::IDENT)
				{
					result->name = parser->lexer.lexeme.value;
					
					if(!set(result->name, result))
						parser->lexer.lexeme.report(document, "Redeclared identifier '" + result->name->string() + "'");
						
					parser->step();
				}
				else
					parser->expected(Lexeme::IDENT);
				
				return result;
			}

			template<class T>  T *declare(Document *document, String *name, Range *range, T *type)
			{
				T *result = type;
				
				result->capture(range);
				result->name = name;
				
				if(name && !set(name, result))
					range->report(document, "Redeclared identifier '" + name->string() + "'");
				
				return result;
			}
			
			template<class T>  T *declare(Document *document, Parser *parser)
			{
				return declare<T>(document, parser, new (memory_pool) T);
			}
			
			template<class T>  T *declare(Document *document, String *name, Range *range)
			{
				return declare<T>(document, name, range, new (memory_pool) T);
			}
	};
};
