#include "parser.hpp"
#include "../tree/scope.hpp"
#include "../tree/types.hpp"
#include "../tree/symbols.hpp"
#include "../tree/node.hpp"

namespace Legion
{
	Parser::Parser(StringPool *string_pool, MemoryPool *memory_pool, Document *document, Scope *scope) : memory_pool(memory_pool), document(document), scope(scope), lexer(string_pool, memory_pool, document)
	{
	}

	Parser::~Parser()
	{
	}
	
	void Parser::expected(Lexeme::LexemeType what, bool skip)
	{
		std::string message = "Expected " + Lexeme::names[what] + ", but found ";
		
		if(lexer.lexeme.has_value())
			message += lexer.lexeme.string() + " (" + Lexeme::names[lexer.lexeme.type] + ")";
		else
			message += "'" + Lexeme::names[lexer.lexeme.type] + "'";
			
		lexer.lexeme.report(document, message);
		
		if(skip)
			step();
	}
	
	void Parser::unexpected(bool skip)
	{
		if(lexer.lexeme.has_value())
			lexer.lexeme.report(document, "Unexpected " + lexer.lexeme.string() + " (" + Lexeme::names[lexer.lexeme.type] + ")");
		else
			lexer.lexeme.report(document, "Unexpected '" + Lexeme::names[lexer.lexeme.type] + "'");
		
		if(skip)
			step();
	}
	
	void Parser::parse_include(Node *node)
	{
		step();
		
		if(expect(Lexeme::STRING))
		{
			std::cout << "Found include " << lexer.lexeme.string() << std::endl;
			step();
		}
		
		matches(Lexeme::SEMICOLON);
	}
	
	void Parser::parse_struct(Node *node)
	{
		step();
		
		TypeSymbol *type = scope->alloc_declare<TypeSymbol>(document, this);
		
		if(match(Lexeme::BRACET_OPEN))
		{
			match(Lexeme::BRACET_CLOSE);
		}
		
		match(Lexeme::SEMICOLON);
	}
	
	void Parser::parse(Node *node)
	{
		while(true)
		{
			lexer.identify_keywords();
			
			switch(lexeme())
			{
				case Lexeme::INCLUDE:
					parse_include(node);
					break;
					
				case Lexeme::STRUCT:
					parse_struct(node);
					break;
					
				case Lexeme::END:
					return;
				
				default:
					unexpected();
			}
		}
	}
};