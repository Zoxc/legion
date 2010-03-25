#include "parser.hpp"
#include "../tree/scope.hpp"
#include "../tree/types.hpp"
#include "../tree/symbols.hpp"
#include "../tree/globals.hpp"

namespace Legion
{
	void Parser::parse_if(NodeList *list)
	{
		IfNode *node = list->add<IfNode>(memory_pool);
		
		step();
		
		match(Lexeme::PARENT_OPEN);
		node->condition = parse_expression();
		match(Lexeme::PARENT_CLOSE);
		
		node->do_true = parse_block();
		
		if(matches(Lexeme::KW_ELSE))
			node->do_false = parse_block();
	}
	
	void Parser::parse_while(NodeList *list)
	{
		WhileNode *node = list->add<WhileNode>(memory_pool);
		
		step();
		
		match(Lexeme::PARENT_OPEN);
		node->condition = parse_expression();
		match(Lexeme::PARENT_CLOSE);
		
		node->body = parse_block();
	}
	
	void Parser::parse_do(NodeList *list)
	{
		DoNode *node = list->add<DoNode>(memory_pool);
		
		step();
		
		node->body = parse_block();
		
		match(Lexeme::KW_WHILE);		
		match(Lexeme::PARENT_OPEN);
		node->condition = parse_expression();
		match(Lexeme::PARENT_CLOSE);
		
	}
	
	void Parser::parse_statements(NodeList *list)
	{
		while(parse_statement(list));
	}
	
	bool Parser::parse_statement(NodeList *list)	
	{
		lexer.identify_keywords();
		
		switch(lexeme())
		{
			case Lexeme::KW_IF:
				parse_if(list);
				break;
				
			case Lexeme::KW_WHILE:
				parse_while(list);
				break;
				
			case Lexeme::KW_DO:
				parse_do(list);
				break;
			
			case Lexeme::SEMICOLON:
				step();
				break;
			
			case Lexeme::END:
				return false;
						
			default:
				unexpected();
		}
		
		return true;
	}
	
	Block *Parser::parse_block()
	{
		Block *block = new (memory_pool) Block;
		
		if(matches(Lexeme::BRACET_OPEN))
		{
			parse_statements(&block->statements);
			
			match(Lexeme::BRACET_CLOSE);
		}
		else
			parse_statements(&block->statements);
			
		return block;
	}
};
