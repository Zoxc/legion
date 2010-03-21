#include "compiler.hpp"

namespace Legion
{
	Compiler::Compiler() : string_pool(&memory_pool), scope(0, &memory_pool)
	{
	}

	Compiler::~Compiler()
	{
	}
};