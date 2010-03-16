#include "parser.hpp"

namespace Legion
{
	Parser::Parser()
	{
	}

	Parser::~Parser()
	{
	}
	
	void Parser::setup(StringPool *string_pool, MemoryPool *memory_pool)
	{
		lexer.setup(string_pool, memory_pool);
	}
};