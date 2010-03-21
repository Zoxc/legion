#include "compiler.hpp"

namespace Legion
{
	Compiler::Compiler()
	{
		string_pool.setup(&memory_pool);
		scope.setup(&memory_pool);
	}

	Compiler::~Compiler()
	{
	}
};