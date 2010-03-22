#pragma once
#include "../common.hpp"
#include "../lexer/lexer.hpp"
#include "../tree/node.hpp"

namespace Legion
{
	class StringPool;
	class Document;
	class Scope;
	
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
			
			void parse_include(NodeList *list);	
			void parse_struct(NodeList *list);	
			void parse_typedef(NodeList *list);
			void parse_global(NodeList *list, bool is_static, bool is_const, bool is_native, PairNode *pair);
			void parse_function(NodeList *list, bool is_static, bool is_const, bool is_native, PairNode *pair);
			template<bool prev_static, bool prev_const, bool prev_native> void parse_global_ident(NodeList *list);
			
			bool parse_pair(PairNode *node);	
			TypeNode *parse_type();	
		public:
			Parser(StringPool *string_pool, MemoryPool *memory_pool, Document *document, Scope *scope);
			~Parser();
			
			Lexer lexer;
			Scope *scope;
			
			void step()
			{
				lexer.step();
			}
			
			void parse(NodeList *list);

			void unexpected(bool skip = true);	
			void expected(Lexeme::LexemeType what, bool skip = false);
	};
};
