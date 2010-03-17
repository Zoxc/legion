#pragma once
#include "../common.hpp"
#include "../lexer/lexer.hpp"

namespace Legion
{
	class StringPool;
	
	class Parser
	{
		private:
		public:
			Parser();
			~Parser();
			
			Lexer lexer;
			
			void parse();
				
			void setup(StringPool *string_pool, MemoryPool *memory_pool);
	};
};
