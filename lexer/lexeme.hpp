#pragma once
#include "../common.hpp"
#include "../range.hpp"
#include "../string_pool.hpp"

namespace Legion
{
	class Lexeme:
		public Range
	{
		private:
			bool uses_value()
			{
				switch(type)
				{
					case Lexeme::STRING:
						return true;
						
					default:
						return false;
				}
			}
			
		public:
			enum LexemeType
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
				INCLUDE,
				STRUCT,
				END,
				TYPES
			};

			LexemeType type;
			
			union
			{
				String *value;
			};
			
			static std::string names[TYPES];
			
			bool has_value()
			{
				switch(type)
				{
					case STRING:
					case IDENT:
					case INTEGER:
					case OCTAL:
					case HEX:
					case REAL:
					case INCLUDE:
						return true;
						
					default:
						return false;
				}
			}
			
			std::string get_value()
			{
				if(uses_value())
				{
					std::string result = (const char *)value->c_str;
					return result;
				}
						
				return string();
			}
	};
};
