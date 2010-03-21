#pragma once
#include "../common.hpp"
#include "../lexer/lexer.hpp"

namespace Legion
{
	class StringPool;
	class Document;
	class Scope;
	class Node;
	
	class Parser
	{
		private:
			Document *document;
			MemoryPool *memory_pool;
			
			Lexeme::LexemeType lexeme()
			{
				return lexer.lexeme.type;
			}
			
			bool match(Lexeme::LexemeType what)
			{
				if(lexeme() == what)
				{
					lexer.step();
					return true;
				}
				else
				{
					expected(what);
					return false;
				}
			}
			
			bool matches(Lexeme::LexemeType what)
			{
				if(lexeme() == what)
				{
					lexer.step();
					return true;
				}
				else
					return false;
			}
			
			bool expect(Lexeme::LexemeType what)
			{
				if(lexeme() == what)
				{
					return true;
				}
				else
				{
					expected(what);
					return false;
				}
			}
			
			void parse_include(Node *node);	
			void parse_struct(Node *node);	
		public:
			Parser();
			~Parser();
			
			Lexer lexer;
			Scope *scope;
			
			void step()
			{
				lexer.step();
			}
			
			void parse(Node *node);

			void unexpected(bool skip = true);	
			void expected(Lexeme::LexemeType what, bool skip = false);		
				
			void setup(StringPool *string_pool, MemoryPool *memory_pool, Document *document, Scope *scope);
	};
};
