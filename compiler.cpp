#include "compiler.hpp"

namespace Legion
{
	Compiler::Compiler() : string_pool(&memory_pool), scope(&memory_pool)
	{
	}

	Compiler::~Compiler()
	{
	}
};