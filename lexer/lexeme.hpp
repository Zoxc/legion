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
			enum Type
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

				// Binary operators sorted after precedence
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

				LOGICAL_OR,

				LOGICAL_AND,

				BITWISE_OR,

				BITWISE_XOR,

				BITWISE_AND,

				EQUAL,
				NOT_EQUAL,

				LESS,
				LESS_OR_EQUAL,
				GREATER,
				GREATER_OR_EQUAL,

				LEFT_SHIFT,
				RIGHT_SHIFT,

				ADD,
				SUB,

				MUL,
				DIV,
				MOD,

				// Other
				BITWISE_NOT,
				LOGICAL_NOT,
				BRACET_OPEN,
				BRACET_CLOSE,
				PARENT_OPEN,
				PARENT_CLOSE,
				SQR_BRACET_OPEN,
				SQR_BRACET_CLOSE,

				// Keywords
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
				KW_TRUE,
				KW_FALSE,
				KW_NULL,
				END,
				TYPES
			};

			Type type;
			
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
			
			static std::string describe_type(Type type)
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
