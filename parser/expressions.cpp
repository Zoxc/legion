#include "parser.hpp"
#include "../tree/scope.hpp"
#include "../tree/types.hpp"
#include "../tree/symbols.hpp"
#include "../tree/globals.hpp"

namespace Legion
{
	ExpressionNode *Parser::parse_expression()
	{
		return 0; //TODO: Fix
	}
	
	ExpressionNode *Parser::parse_grouped_expression()
	{
		ExpressionNode *result;

		match(Lexeme::PARENT_OPEN);
		result = parse_expression();
		match(Lexeme::PARENT_CLOSE);

		return result;
	}
};
