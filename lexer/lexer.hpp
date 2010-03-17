#pragma once
#include "../common.hpp"
#include "../range.hpp"
#include "input.hpp"

namespace Legion
{
	class String;
	class StringPool;

	class Lexeme:
		public Range
	{
		public:
			enum LexmeType
			{
				NONE,
				IDENT,	
				STRING,
				INTEGER,
				OCTAL,
				HEX,
				REAL,	
				MEMBER,	
				MEMBER_PTR,
				SEMICOLON,
				COMMA,
				ADD,
				SUB,
				MUL,
				DIV,
				MOD,	
				BITWISE_OR,
				BITWISE_XOR,
				BITWISE_AND,
				BITWISE_NOT,
				LEFT_SHIFT,
				RIGHT_SHIFT,
				LOGICAL_OR,
				LOGICAL_AND,
				LOGICAL_NOT,
				ASSIGN,
				ASSIGN_ADD,
				ASSIGN_SUB,
				ASSIGN_MUL,
				ASSIGN_DIV,
				ASSIGN_MOD,
				ASSIGN_BITWISE_OR,
				ASSIGN_BITWISE_XOR,
				ASSIGN_BITWISE_AND,
				ASSIGN_BITWISE_NOT,
				ASSIGN_LEFT_SHIFT,
				ASSIGN_RIGHT_SHIFT,
				ASSIGN_LOGICAL_OR,
				ASSIGN_LOGICAL_AND,
				EQUAL,
				NOT_EQUAL,
				LESS,
				LESS_OR_EQUAL,
				GREATER,
				GREATER_OR_EQUAL,
				BRACET_OPEN,
				BRACET_CLOSE,
				PARENT_OPEN,
				PARENT_CLOSE,
				SQR_BRACET_OPEN,
				SQR_BRACET_CLOSE,
				END,
				TYPES
			};

			LexmeType type;
			
			union
			{
				String *value;
			};
			
			static std::string names[TYPES];
	};
	
	class Lexer
	{
		private:
			const char_t *input_str;
			Input input;
			size_t length;
			
			StringPool *string_pool;
			MemoryPool *memory_pool;
			
			static bool jump_table_ready;
			static void(Lexer::*jump_table[sizeof(char_t) << 8])();
			static void setup_jump_table();

			bool process_null(const char_t *input, bool expected = false);
			void build_string(const char_t *start, char_t *str);
			
			void report_null();
			
			template<Lexeme::LexmeType type> void single();
			template<Lexeme::LexmeType type, Lexeme::LexmeType assign_type> void assign();
			template<Lexeme::LexmeType type, Lexeme::LexmeType assign_type, char_t match, Lexeme::LexmeType match_type, Lexeme::LexmeType match_assign> void assign();

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
			Lexer();
			
			Lexeme lexeme;
			
			void setup(StringPool *string_pool, MemoryPool *memory_pool);
			void load(const char_t *input, size_t length);
			void step();
	};
};
