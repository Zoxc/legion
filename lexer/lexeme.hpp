#pragma once
#include "../common.hpp"
#include "../range.hpp"
#include "../string_pool.hpp"

namespace Legion
{
	class Lexeme:
		public Range
	{
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
				KW_INCLUDE,
				KW_STRUCT,
				KW_TYPEDEF,
				KW_STATIC,
				KW_CONST,
				KW_NATIVE,
				KW_IF,
				KW_ELSE,
				KW_WHILE,
				KW_DO,
				KW_BREAK,
				KW_CONTINUE,
				KW_RETURN,
				END,
				TYPES
			};

			LexemeType type;
			
			union
			{
				String *value;
			};
			
			static std::string names[TYPES];
			
			std::string describe()
			{
				std::string result;
				
				if((type >= MEMBER && type < KW_INCLUDE) || type == END)
					result = "'" + names[type] + "'";
				else if(type >= KW_INCLUDE && type < END)
					result = "'" + names[type] + "' (keyword)";
				else if(type == IDENT)
					result = "'" + string() + "' (identifier)";
				else
					result = string() + " (" + names[type] + ")";
				
				return result;
			}
			
			static std::string describe_type(LexemeType type)
			{
				std::string result;
				
				if((type >= MEMBER && type < KW_INCLUDE) || type == END)
					result = "'" + names[type] + "'";
				else if(type >= KW_INCLUDE && type < END)
					result = "'" + names[type] + "' (keyword)";
				else if(type == IDENT)
					result = names[type];
				
				return result;
			}
	};
};
