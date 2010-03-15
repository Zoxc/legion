#include "compiler.hpp"

namespace Legion
{
	Parser::Parser()
	{
		lexer.pool = &pool;
	}

	Parser::~Parser()
	{
	}
};