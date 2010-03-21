#pragma once
#include "../common.hpp"
#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"

namespace Legion
{
	class String;
	class StringPool;
	class Symbol;
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
			Scope(MemoryPool *memory_pool);
			~Scope();
			
			Scope *parent;
			
			Symbol *get(String *name);
			bool set(String *name, Symbol *symbol);

			template<class T>  T *declare(Document *document, Parser *parser, T *type)
			{
				T *result = type;
				
				if(parser->lexer.lexeme.type == Lexeme::IDENT)
				{
					result->name = parser->lexer.lexeme.value;
					
					if(!set(result->name, result))
						parser->lexer.lexeme.report(document, "Redeclared identifier '" + parser->lexer.lexeme.string() + "'");
						
					parser->step();
				}
				else
					parser->expected(Lexeme::IDENT);
				
				return result;
			}
			
			template<class T>  T *alloc_declare(Document *document, Parser *parser)
			{
				T *symbol = new (memory_pool) T;
				
				symbol->type = symbol->symbol_type();
				
				return declare<T>(document, parser, symbol);
			}
			
	};
};
