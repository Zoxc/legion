#pragma once
#include "common.hpp"
#include "string_pool.hpp"
#include "parser/parser.hpp"

namespace Legion
{
	class Compiler;
	
	class Document
	{
		private:
			bool map();
			const char_t *input;
			size_t length;
		public:
			MemoryPool memory_pool;
			Parser parser;
			const char *filename;
			
			Document(Compiler *compiler, const char *filename);
			~Document();
			
			void lex();
	};
};
