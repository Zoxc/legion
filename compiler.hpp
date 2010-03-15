#pragma once
#include "common.hpp"
#include "memory_pool.hpp"
#include "string_pool.hpp"
#include "parser/parser.hpp"

namespace Legion
{
	class Compiler
	{
		private:
		public:
			MemoryPool memory_pool;
			StringPool string_pool;
			Parser parser;
			
			Compiler();
			~Compiler();
	};
};
