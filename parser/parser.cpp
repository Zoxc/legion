#include "parser.hpp"

namespace Legion
{
	Parser::Parser()
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
			
		lexer.lexeme.report(message);
		
		if(skip)
			step();
	}
	
	void Parser::unexpected(bool skip)
	{
		if(lexer.lexeme.has_value())
			lexer.lexeme.report("Unexpected " + lexer.lexeme.string() + " (" + Lexeme::names[lexer.lexeme.type] + ")");
		else
			lexer.lexeme.report("Unexpected '" + Lexeme::names[lexer.lexeme.type] + "'");
		
		if(skip)
			step();
	}
	
	void Parser::include()
	{
		step();
		
		if(verify(Lexeme::STRING))
		{
			std::cout << "Found include " << lexer.lexeme.string() << std::endl;
			step();
		}
		
		matches(Lexeme::SEMICOLON);
	}	
	
	void Parser::parse()
	{
		while(true)
		{
			lexer.identify_keywords();
			
			switch(lexeme())
			{
				case Lexeme::INCLUDE:
					include();
					break;
					
				case Lexeme::END:
					return;
				
				default:
					unexpected();
			}
		}
	}
			
	void Parser::setup(StringPool *string_pool, MemoryPool *memory_pool)
	{
		lexer.setup(string_pool, memory_pool);
	}
};