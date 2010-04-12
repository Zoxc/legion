#pragma once
#include "common.hpp"
#include "message.hpp"
#include "string_pool.hpp"
#include "parser/parser.hpp"
#include "tree/node.hpp"
#include "tree/globals.hpp"

namespace Legion
{
	class Compiler;

	class Document
	{
		private:
			bool map(std::string filename);
		public:
			const char_t *input;
			size_t length;

			MemoryPool memory_pool;
			Compiler &compiler;
			Parser parser;
			String *filename;
			NamespaceList tree;
			List<IncludeNode> includes;
			Scope *scope;
			List<Message> messages;
			Document *next;

			String *normalize(String *filename);

			bool parse();
			void find_declarations();
			void validate();
			bool load(std::string filename);
			void load(const char_t *input, size_t length);

			void report(Range &range, std::string text, Message::Severity severity = Message::LEGION_ERROR);
			void report_type_modifier(Range &range);
			
			Document(Compiler &compiler, std::string filename);
			Document(Compiler &compiler, String *string);
			~Document();
	};
};
