#pragma once
#include "common.hpp"
#include "string_pool.hpp"

namespace Legion
{
	class Compiler
	{
		private:
		public:
			MemoryPool memory_pool;
			StringPool string_pool;
			
			Compiler();
			~Compiler();
	};
};
