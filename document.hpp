#pragma once
#include "common.hpp"
#include "message.hpp"
#include "string_pool.hpp"
#include "parser/parser.hpp"
#include "tree/node.hpp"

namespace Legion
{
	class Compiler;

	class Document
	{
		private:
			bool map(std::string filename);
			const char_t *input;
			size_t length;
		public:
			MemoryPool memory_pool;
			Compiler &compiler;
			Parser parser;
			std::string filename;
			NamespaceList tree;
			std::vector<std::string> includes;
			Scope *scope;
			List<Message> messages;

			bool parse();
			void find_declarations();
			void validate();
			void load(std::string filename);
			void load(const char_t *input, size_t length);

			void report(Range &range, std::string text, Message::Severity severity = Message::LEGION_ERROR);
			void report_type_modifier(Range &range);
			
			Document(Compiler &compiler, std::string filename);
			~Document();
	};
};
