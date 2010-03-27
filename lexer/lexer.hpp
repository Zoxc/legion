#pragma once
#include "../common.hpp"
#include "../range.hpp"
#include "input.hpp"
#include "lexeme.hpp"

namespace Legion
{
	class Document;
	class String;
	class StringPool;

	class Keywords
	{
		public:
			Keywords(StringPool *pool);
			
			std::map<String *, Lexeme::Type> mapping;
	};
	
	class Lexer
	{
		private:
			const char_t *input_str;
			Input input;
			size_t length;
			
			StringPool *string_pool;
			MemoryPool *memory_pool;
			Document *document;
			
			static bool jump_table_ready;
			static void(Lexer::*jump_table[sizeof(char_t) << 8])();
			static void setup_jump_table();

			bool process_null(const char_t *input, bool expected = false);
			void build_string(const char_t *start, char_t *str);
			
			void report_null();
			
			template<Lexeme::Type type> void single();
			template<Lexeme::Type type, Lexeme::Type assign_type> void assign();
			template<Lexeme::Type type, Lexeme::Type assign_type, char_t match, Lexeme::Type match_type, Lexeme::Type match_assign> void assign();

			void eol();
			void white();
			void unknown();
			void null();
			void newline();
			void carrige_return();
			
			void sub();
			void div();
			
			void zero();
			void number();
			void real();
			void hex();
			
			void ident();
			void string();
		public:
			Lexer(StringPool *string_pool, MemoryPool *memory_pool, Document *document);
			
			Lexeme lexeme;
			Keywords keywords;
			
			void load(const char_t *input, size_t length);
			void step();
			void identify_keywords();
	};
};
