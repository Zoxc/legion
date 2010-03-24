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
	
	void Parser::expected(Lexeme::LexemeType what, bool skip)
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
		
		while(lexeme() == Lexeme::MUL)
		{
			TypePtrNode *node = new (memory_pool) TypePtrNode;
			step();
	 
			node->base = result;
	 
			result = node;
		}
		
		return result;
	}
};