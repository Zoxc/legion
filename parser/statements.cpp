#include "parser.hpp"
#include "../tree/scope.hpp"
#include "../tree/types.hpp"
#include "../tree/symbols.hpp"
#include "../tree/globals.hpp"
#include "../tree/expressions.hpp"

namespace Legion
{
	void Parser::parse_local(bool is_const, ExpressionNode *type, StatementList *list)
	{
		LocalNode *node = list->add<LocalNode>(memory_pool);

		node->type = type;

		if(expect(Lexeme::IDENT))
		{
			node->name = lexer.lexeme.value;
			step();
		}
		else
			node->name = 0;

		node->is_const = is_const;

		if(matches(Lexeme::ASSIGN))
			node->value = parse_expression();
		else
			node->value = 0;
	}

	void Parser::parse_if(StatementList *list)
	{
		IfNode *node = list->add<IfNode>(memory_pool);
		
		step();
		
		node->condition = parse_grouped_expression();
		node->do_true = parse_block<false>(Scope::CONDITIONAL);

		lexer.identify_keywords();
		
		if(matches(Lexeme::KW_ELSE))
			node->do_false = parse_block<false>(Scope::CONDITIONAL);;
	}
	
	void Parser::parse_while(StatementList *list)
	{
		WhileNode *node = list->add<WhileNode>(memory_pool);

		step();
		
		node->condition = parse_grouped_expression();
		node->body = parse_block<false>(Scope::LOOP);
	}
	
	void Parser::parse_do(StatementList *list)
	{
		DoNode *node = list->add<DoNode>(memory_pool);

		step();
		
		node->body = parse_block<false>(Scope::LOOP);

		lexer.identify_keywords();
		
		match(Lexeme::KW_WHILE);		
		node->condition = parse_grouped_expression();
		match(Lexeme::SEMICOLON);
	}
		
	void Parser::parse_return(StatementList *list)
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
	
	void Parser::parse_break(StatementList *list)
	{
		if(scope->type == Scope::LOOP)
			list->add<BreakNode>(memory_pool);
		else
			lexer.lexeme.report(document, "Unexpected " + lexer.lexeme.describe() + " outside of loop");
		
		step();
	}
	
	void Parser::parse_continue(StatementList *list)
	{
		if(scope->type == Scope::LOOP)
			list->add<ContinueNode>(memory_pool);
		else
			lexer.lexeme.report(document, "Unexpected " + lexer.lexeme.describe() + " outside of loop");
		
		step();
	}
	
	void Parser::parse_statements(StatementList *list)
	{
		while(parse_statement(list));
	}
	
	bool Parser::parse_statement(StatementList *list)	
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
			
			case Lexeme::KW_CONST:
				step();
				parse_local(true, parse_expression(), list);
				break;
			
			case Lexeme::BRACET_OPEN:
				list->append(parse_block<true>(Scope::EMPTY));
				break;
			
			case Lexeme::SEMICOLON:
				step();
				break;
			
			case Lexeme::END:
			case Lexeme::BRACET_CLOSE:
				return false;
	
			default:
				if(is_expression(lexeme()))
				{
					ExpressionNode *node = parse_expression();

					if(lexeme() == Lexeme::IDENT)
						parse_local(false, node, list);
					else
						list->append(node);
				}
				else
					return false;
		}
		
		return true;
	}
};
