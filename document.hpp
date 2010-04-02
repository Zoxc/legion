#pragma once
#include "common.hpp"
#include "string_pool.hpp"
#include "parser/parser.hpp"
#include "tree/node.hpp"

namespace Legion
{
	class Compiler;
	
	class Document
	{
		private:
			bool map();
			const char_t *input;
			size_t length;
			Compiler *compiler;
		public:
			MemoryPool memory_pool;
			Parser parser;
			std::string filename;
			NamespaceList tree;
			std::vector<std::string> includes;
			Scope *scope;

			bool parse();
			void find_declarations();
			
			Document(Compiler *compiler, std::string filename);
			~Document();
	};
};
