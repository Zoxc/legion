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
			
			void parse(NamespaceList *list);

			void unexpected(bool skip = true);	
			void expected(Lexeme::Type what, bool skip = false);
		private:
			Document *document;
			MemoryPool *memory_pool;
			MemoryPool *global_pool;
			
			Lexeme::Type lexeme()
			{
				return lexer.lexeme.type;
			}
			
			bool match(Lexeme::Type what)
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
			
			bool matches(Lexeme::Type what)
			{
				if(lexeme() == what)
				{
					lexer.step();
					return true;
				}
				else
					return false;
			}
			
			bool expect(Lexeme::Type what)
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
			ExpressionNode *parse_array_subscript();
			bool is_expression(Lexeme::Type lexeme);
			ExpressionNode *parse_factor();
			bool is_factor_chain(Lexeme::Type lexeme);
			ExpressionNode *parse_factor_chain();
			ExpressionNode *parse_unary();
			bool is_binary_operator(Lexeme::Type op);
			ExpressionNode *parse_binary_operator();
			ExpressionNode *parse_binary_operator(ExpressionNode *left, size_t precedence);
			ExpressionNode *parse_assign();
			ExpressionNode *parse_expression();
			ExpressionNode *parse_grouped_expression();
			
			// Statements
			void parse_local(bool is_const, ExpressionNode *type, StatementList *list);
			void parse_continue(StatementList *list);
			void parse_break(StatementList *list);
			void parse_return(StatementList *list);
			void parse_if(StatementList *list);
			void parse_while(StatementList *list);
			void parse_do(StatementList *list);
			
			bool parse_statement(StatementList *list);
			void parse_statements(StatementList *list);	
			Block *parse_block();
			
			// Globals
			void parse_include(NamespaceList *list);	
			void parse_struct(NamespaceList *list);	
			void parse_typedef(NamespaceList *list);
			void parse_global(NamespaceList *list, bool is_static, bool is_const, bool is_native, PairNode *pair);
			void parse_function(NamespaceList *list, bool is_static, bool is_const, bool is_native, PairNode *pair);
			template<bool prev_static, bool prev_const, bool prev_native> void parse_global_ident(NamespaceList *list);
			
			bool parse_pair(PairNode *node);	
			
			Scope *push_scope(Scope::Type type);
			void pop_scope();
			
			TypeNode *parse_type();	

			template<class T>  T *declare(T *type, Symbol **prev)
			{
				T *result = type;
				
				if(lexer.lexeme.type == Lexeme::IDENT)
				{
					result->range = new (global_pool) Range;
					result->range->capture(&lexer.lexeme);
					result->name = lexer.lexeme.value;
					
					*prev = scope->declare_symbol(result);

					step();
				}
				else
					expected(Lexeme::IDENT);
				
				return result;
			}

			template<class T>  T *declare(T *type)
			{
				Symbol *prev;

				T *result = declare<T>(type, &prev);

				if(prev)
					result->redeclared(document);

				return result;
			}
			
			template<class T>  T *declare()
			{
				return declare<T>(new (global_pool) T);
			}
			
			template<class T>  T *declare(PairNode *pair, Symbol **prev)
			{
				T *result = new (global_pool) T;
				
				result->range = new (global_pool) Range(&pair->range);
				result->name = pair->name;
				
				*prev = scope->declare_symbol(result);
				
				return result;
			}

			template<class T>  T *declare(PairNode *pair)
			{
				Symbol *prev;

				T *result = declare<T>(pair, &prev);

				if(prev)
					result->redeclared(document);

				return result;
			}
	};
};
