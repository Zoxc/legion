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
		"+",
		"-",
		"*",
		"/",
		"%",
		"|",
		"^",
		"&",
		"~",
		"<<",
		">>",
		"||",
		"&&",
		"!",
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
		"==",
		"!=",
		"<",
		"<=",
		">",
		">=",
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
		"end of file"
	};
};
