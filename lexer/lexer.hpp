#pragma once
#include "../common.hpp"
#include "../range.hpp"
#include "input.hpp"

namespace Legion
{
	class String;
	class StringPool;
	
	class Lexme:
		public Range
	{
		public:
			enum LexmeType
			{
				None,
				Ident,	
				Integer,
				Octal,
				Hex,
				Real,	
				Point,		
				End,
				Types
			};

			LexmeType type;
			
			union
			{
				String *value;
			};
			
			static std::string names[Types];
	};
	
	class Lexer
	{
		private:
			char_t *input_str;
			Input input;
			size_t length;
			
			StringPool *string_pool;
			
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
			
			void ident();
		public:
			Lexer();
			
			Lexme lexme;
			
			void setup(StringPool *string_pool);
			void load(char_t *input, unsigned int length);
			void step();
	};
};
