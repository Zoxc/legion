#include "compiler.hpp"

namespace Legion
{
	Compiler::Compiler()
	{
		string_pool.setup(&memory_pool);
		parser.setup(&string_pool, &memory_pool);
	}

	Compiler::~Compiler()
	{
	}
};