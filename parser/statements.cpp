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
		
		node->condition = parse_grouped_expression();
		node->do_true = parse_block();
		
		if(matches(Lexeme::KW_ELSE))
			node->do_false = parse_block();
	}
	
	void Parser::parse_while(NodeList *list)
	{
		WhileNode *node = list->add<WhileNode>(memory_pool);

		step();
		
		node->condition = parse_grouped_expression();

		push_scope(Scope::LOOP);
		node->body = parse_block();
		pop_scope();
	}
	
	void Parser::parse_do(NodeList *list)
	{
		DoNode *node = list->add<DoNode>(memory_pool);

		step();
		
		push_scope(Scope::LOOP);
		node->body = parse_block();
		pop_scope();
		
		match(Lexeme::KW_WHILE);		
		node->condition = parse_grouped_expression();
		match(Lexeme::SEMICOLON);
	}
		
	void Parser::parse_return(NodeList *list)
	{
		ReturnNode *node = list->add<ReturnNode>(memory_pool);
		
		step();

		if(matches(Lexeme::SEMICOLON))
			node->value = 0;
		else
		{
			node->value = parse_expression();
			match(Lexeme::SEMICOLON);
		}
	}
	
	void Parser::parse_break(NodeList *list)
	{
		if(scope->type == Scope::LOOP)
			list->add<BreakNode>(memory_pool);
		else
			lexer.lexeme.report(document, "Unexpected " + lexer.lexeme.describe() + " outside of loop");
		
		step();
	}
	
	void Parser::parse_continue(NodeList *list)
	{
		if(scope->type == Scope::LOOP)
			list->add<ContinueNode>(memory_pool);
		else
			lexer.lexeme.report(document, "Unexpected " + lexer.lexeme.describe() + " outside of loop");
		
		step();
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

			case Lexeme::KW_RETURN:
				parse_return(list);
				break;
			
			case Lexeme::KW_BREAK:
				parse_break(list);
				break;
			
			case Lexeme::KW_CONTINUE:
				parse_continue(list);
				break;
			
			case Lexeme::BRACET_OPEN:
				list->append(parse_block());
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
