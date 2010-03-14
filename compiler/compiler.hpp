#pragma once
#include "../common.hpp"
#include "../memory_pool.hpp"

namespace Legion
{
	class Compiler
	{
		private:
		public:
			MemoryPool pool;
			
			Compiler();
			~Compiler();
	};
};
