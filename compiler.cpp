#include "compiler.hpp"

namespace Legion
{
	Compiler::Compiler() : string_pool(&memory_pool), scope(0, Scope::ROOT, &memory_pool)
	{
	}

	Compiler::~Compiler()
	{
	}
};