#pragma once
#include "../common.hpp"
#include "../lexer/lexer.hpp"
#include "../tree/scope.hpp"

namespace Legion
{
	class StringPool;
	
	class Parser
	{
		private:
			Lexeme::LexemeType lexeme()
			{
				return lexer.lexeme.type;
			}
			
			void step()
			{
				lexer.step();
			}
			
			void match(Lexeme::LexemeType what)
			{
				if(lexeme() == what)
					lexer.step();
				else
					expected(what);
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
			
			bool verify(Lexeme::LexemeType what)
			{
				if(lexeme() == what)
				{
					return true;
				}
				else
					return false;
			}
			
			void unexpected(bool skip = true);	
			void expected(Lexeme::LexemeType what, bool skip = false);		
			
			void include();	
		public:
			Parser();
			~Parser();
			
			Lexer lexer;
			Scope scope;
			
			void parse();
				
			void setup(StringPool *string_pool, MemoryPool *memory_pool);
	};
};
