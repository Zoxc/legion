#include "parser.hpp"

namespace Legion
{
	Parser::Parser()
	{
	}

	Parser::~Parser()
	{
	}
	
	void Parser::setup(StringPool *string_pool)
	{
		lexer.setup(string_pool);
	}
};