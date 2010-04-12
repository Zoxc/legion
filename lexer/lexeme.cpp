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

	Range &Lexeme::get_prev()
	{
		Range &result = *new (lexer.memory_pool) Range;
		result.start = prev;
		result.stop = prev + 1;
		result.line = line;

		const char_t *c = start;

		while(c != prev)
		{
			switch(*c)
			{
				case '\n':
					if(c - 1 != prev && c[-1] == '\r')
						c--;
				
				case '\r':
					result.line--;

				default:
					c--;
			}
		}

		const char_t *input = lexer.input_str;

		switch(*c)
		{
			case '\n':
				c--;
				if(c != input && *c == '\r')
					c--;
				break;

			case '\r':
				c--;
				break;
			
			default:
				break;
		}

		while(c != input)
		{
			switch(*c)
			{
				case '\n':
				case '\r':
					c++;
					goto end;
				
				default:
					c--;
			}
		}

		end:

		result.line_start = c;

		return result;
	}
};
