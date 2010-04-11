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
			Parser(StringPool &string_pool, MemoryPool &memory_pool, Document &document, Scope *scope);
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
			void expected_prev(Lexeme::Type what);
		private:
			Document &document;
			MemoryPool &memory_pool;
			
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
			static bool is_expression(Lexeme::Type lexeme);
			ExpressionNode *parse_factor();
			static bool is_factor_chain(Lexeme::Type lexeme);
			ExpressionNode *parse_factor_chain();
			ExpressionNode *parse_unary();
			static bool is_binary_operator(Lexeme::Type lexeme);
			ExpressionNode *parse_binary_operator();
			ExpressionNode *parse_binary_operator(ExpressionNode *left, size_t precedence);
			static bool is_assign_operator(Lexeme::Type lexeme);
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

			template<bool bracets_required> Block *parse_block(Scope::Type type)
			{
				Block *block = new (memory_pool) Block;

				block->range.capture(lexer.lexeme);
				block->scope = push_scope(type);

				if(bracets_required)
				{
					match(Lexeme::BRACET_OPEN);
					
					parse_statements(&block->statements);
						
					match(Lexeme::BRACET_CLOSE);
				}
				else
				{
					if(matches(Lexeme::BRACET_OPEN))
					{
						parse_statements(&block->statements);
						
						match(Lexeme::BRACET_CLOSE);
					}
					else
						parse_statements(&block->statements);
				}

				block->range.expand(lexer.lexeme);

				pop_scope();
					
				return block;
			}
			
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

			void parse_terminator();
			
			TypeNode *parse_type();	

			template<class T>  T *declare(T *type, bool &prev)
			{
				T *result = type;
				
				if(expect(Lexeme::IDENT))
				{
					result->range = new (scope->memory_pool) Range(lexer.lexeme);
					result->name = lexer.lexeme.value;
					
					prev = scope->declare_symbol(result);

					step();
				}
				
				return result;
			}

			template<class T>  T *declare(T *type)
			{
				bool prev;

				T *result = declare<T>(type, prev);

				if(prev)
					result->redeclared(document);

				return result;
			}
			
			template<class T>  T *declare(Document *document)
			{
				T *result = new (scope->memory_pool) T;

				result->document = document;

				return declare<T>(result);
			}
			
			template<class T>  T *declare(PairNode *pair, Document *document, bool &prev)
			{
				T *result = new (scope->memory_pool) T;
				
				result->range = new (scope->memory_pool) Range(pair->range);
				result->name = pair->name;
				result->document = document;
				
				prev = scope->declare_symbol(result);
				
				return result;
			}

			template<class T>  T *declare(PairNode *pair, Document *document)
			{
				bool prev;

				T *result = declare<T>(pair, document, prev);

				if(prev)
					result->redeclared(this->document);

				return result;
			}
	};
};
