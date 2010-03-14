#pragma once
#include "../common.hpp"
#include "../memory_pool.hpp"
#include "input.hpp"

namespace Legion
{
	class Compiler;
	
	class Range
	{
		public:
			char_t *start;
			char_t *stop;
			char_t *line_start;
			unsigned int line;
			bool error;

			unsigned int length();

			std::string string();

			void report(const std::string& error);
	};

	class Lexme:
		public Range
	{
		public:
			enum LexmeType
			{
				None,
				Integer,
				Octal,
				Hex,
				Real,
				Point,
				End,
				Types
			};

			LexmeType type;
			
			static std::string names[Types];
	};
	
	class Lexer
	{
		private:
			char_t *input_str;
			Input input;
			unsigned int length;
			
			static bool jump_table_ready;
			static void(Lexer::*jump_table[256])();
			static void setup_jump_table();

			void white();
			void unknown();
			void null();
			
			void zero();
			void number();
			void real();
			void hex();
		public:
			Lexer();
			
			Compiler *compiler;
			Lexme lexme;
			
			void setup(char_t *input, unsigned int length);
			void step();
	};
};
