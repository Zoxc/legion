#include "lexer.hpp"
#include "../string_pool.hpp"

namespace Legion
{
	std::string Lexeme::names[TYPES] = {
		"none",	
		"identifier",
		"string",
		"integer",
		"octal",
		"hex",
		"real",
		".",
		"->",
		";",
		",",

		// Binary operators sorted after precedence
		"=",
		"+=",
		"-=",
		"*=",
		"/=",
		"%=",
		"|=",
		"^=",
		"&=",
		"~=",
		"<<=",
		">>=",
		"||=",
		"&&=",

		"||",

		"&&",

		"|",

		"^",

		"&",

		"==",
		"!=",

		"<",
		"<=",
		">",
		">=",

		"<<",
		">>",

		"+",
		"-",

		"*",
		"/",
		"%",

		// Other
		"~",
		"!",
		"{",
		"}",
		"(",
		")",
		"[",
		"]",
		"include",
		"struct",
		"typedef",
		"static",
		"const",
		"native",
		"if",
		"else",
		"while",
		"do",
		"break",
		"continue",
		"return",
		"true",
		"false",
		"null",
		"end of file"
	};
};
