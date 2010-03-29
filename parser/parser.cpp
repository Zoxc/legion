#include "parser.hpp"
#include "../tree/scope.hpp"
#include "../tree/types.hpp"
#include "../tree/symbols.hpp"
#include "../tree/types.hpp"

namespace Legion
{
	Parser::Parser(StringPool *string_pool, MemoryPool *memory_pool, Document *document, Scope *scope) : document(document), memory_pool(memory_pool), lexer(string_pool, memory_pool, document), scope(scope)
	{
	}

	Parser::~Parser()
	{
	}
	
	Scope *Parser::push_scope(Scope::Type type)
	{
		return scope = new Scope(scope, type, memory_pool);
	}
	
	void Parser::pop_scope()
	{
		scope = scope->parent;
	}
	
	void Parser::expected(Lexeme::Type what, bool skip)
	{
		lexer.lexeme.report(document, "Expected " + Lexeme::describe_type(what) + ", but found " + lexer.lexeme.describe());
		
		if(skip)
			step();
	}
	
	void Parser::unexpected(bool skip)
	{
		lexer.lexeme.report(document, "Unexpected " + lexer.lexeme.describe());

		if(skip)
			step();
	}
	
	TypeNode *Parser::parse_type()
	{
		TypeNode *result = 0;
		
		if(expect(Lexeme::IDENT))
		{
			TypeBaseNode *node = new (memory_pool) TypeBaseNode;
			
			node->type = lexer.lexeme.value;
			step();
			
			result = node;
		}
		while(true)
		{
			switch(lexeme())
			{
				case Lexeme::MUL:
				{
					TypePointerNode *node = new (memory_pool) TypePointerNode;
					step();
			 
					node->base = result;
			 
					result = node;
				}
				break;

				case Lexeme::SQR_BRACET_OPEN:
				{
					TypeArrayNode *node = new (memory_pool) TypeArrayNode;
					step();
			 
					node->base = result;
					node->size = parse_expression();
			 
					result = node;

					match(Lexeme::SQR_BRACET_CLOSE);
				}
				break;

				default:
					return result;
			}

		}
	}
};