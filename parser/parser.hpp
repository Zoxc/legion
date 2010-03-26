#pragma once
#include "../common.hpp"
#include "../lexer/lexer.hpp"
#include "../tree/node.hpp"
#include "../tree/globals.hpp"
#include "../tree/scope.hpp"

namespace Legion
{
	class StringPool;
	class Document;
	class Scope;
	
	class Parser
	{
		public:
			Parser(StringPool *string_pool, MemoryPool *memory_pool, MemoryPool *global_pool, Document *document, Scope *scope);
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
		private:
			Document *document;
			MemoryPool *memory_pool;
			MemoryPool *global_pool;
			
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
			
			// Expressions
			ExpressionNode *parse_expression();
			ExpressionNode *parse_grouped_expression();
			
			// Statements
			void parse_continue(NodeList *list);
			void parse_break(NodeList *list);
			void parse_return(NodeList *list);
			void parse_if(NodeList *list);
			void parse_while(NodeList *list);
			void parse_do(NodeList *list);
			
			bool parse_statement(NodeList *list);
			void parse_statements(NodeList *list);	
			Block *parse_block();
			
			// Globals
			void parse_include(NodeList *list);	
			void parse_struct(NodeList *list);	
			void parse_typedef(NodeList *list);
			void parse_global(NodeList *list, bool is_static, bool is_const, bool is_native, PairNode *pair);
			void parse_function(NodeList *list, bool is_static, bool is_const, bool is_native, PairNode *pair);
			template<bool prev_static, bool prev_const, bool prev_native> void parse_global_ident(NodeList *list);
			
			bool parse_pair(PairNode *node);	
			
			Scope *push_scope(Scope::Type type);
			void pop_scope();
			
			TypeNode *parse_type();	
			
			template<class T>  T *declare(T *type)
			{
				T *result = type;
				
				if(lexer.lexeme.type == Lexeme::IDENT)
				{
					result->range = new (memory_pool) Range;
					result->range->capture(&lexer.lexeme);
					result->name = lexer.lexeme.value;
					
					scope->declare_symbol(document, result);
					
					step();
				}
				else
					expected(Lexeme::IDENT);
				
				return result;
			}
			
			template<class T>  T *declare()
			{
				return declare<T>(new (memory_pool) T);
			}
			
			template<class T>  T *declare(PairNode *pair)
			{
				T *result = new (memory_pool) T;
				
				result->range = &pair->range;
				result->name = pair->name;
				
				scope->declare_symbol(document, result);
				
				return result;
			}
	};
};
