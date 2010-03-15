#pragma once
#include "../common.hpp"
#include "../memory_pool.hpp"
#include "../string_pool.hpp"

namespace Legion
{
	class Parser
	{
		private:
		public:
			StringPool pool;
			Lexer lexer;
			
			Parser();
			~Parser();
	};
};
