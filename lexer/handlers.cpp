#include "lexer.hpp"
#include "../string_pool.hpp"
#include "../document.hpp"

namespace Legion
{
	void Lexer::white()
	{
		input++;
		
		while(input.in(1, 9) || input.in(11, 12) || input.in(14, 32))
			input++;

		restep();
	}

	void Lexer::unknown()
	{
		input++;
		
		while(jump_table[input] == &Lexer::unknown)
			input++;

		lexeme.stop = &input;
		lexeme.type = Lexeme::NONE;

		if (lexeme.length() == 1)
			document.report(lexeme.dup(memory_pool), "Invalid character '" + lexeme.string() + "'");
		else
			document.report(lexeme.dup(memory_pool), "Invalid characters '" + lexeme.string() + "'");

		restep();
	}
	
	void Lexer::sub()
	{
		input++;
		
		switch(input)
		{
			case '=':
				input++;
				lexeme.stop = &input;
				lexeme.type = Lexeme::ASSIGN_SUB;
				break;
				
			case '>':
				input++;
				lexeme.stop = &input;
				lexeme.type = Lexeme::MEMBER_PTR;
				break;
			
			default:
				lexeme.stop = &input;
				lexeme.type = Lexeme::SUB;
		}
	}
		
		
	bool Lexer::process_null(const char_t *input, bool expected)
	{
		bool result = (size_t)(input - input_str) >= length;
		
		if(result && !expected)
			report_null();
			
		return result;
	}

	void Lexer::null()
	{
		if(process_null(&input, true))
		{
			lexeme.stop = &input;
			lexeme.type = Lexeme::END;
		}
		else
			unknown();
	}

	void Lexer::newline()
	{
		input++;
		lexeme.line++;
		lexeme.line_start = &input;
		
		restep();
	}

	void Lexer::carrige_return()
	{
		input++;
		lexeme.line++;

		if(input == '\n')
			input++;

		lexeme.line_start = &input;

		restep();
	}

	void Lexer::eol()
	{
		while(input != '\n' && input != '\r' && input != 0)
			input++;
	}
	
	void Lexer::div()
	{
		input++;
		
		switch(input)
		{
			case '=':
				input++;
				lexeme.stop = &input;
				lexeme.type = Lexeme::ASSIGN_DIV;
				break;
				
			case '/': // C++ comment
				eol();
				restep();
				break;
				
			case '*': // C comment
				input++;

				while(true)
				{
					switch(input)
					{
						case '*':
							input++;
							if(input == '/')
							{
								input++;
								goto done;
							}
							break;

						case '\n':
							input++;
							lexeme.line++;
							lexeme.line_start = &input;
							break;
								
						case '\r':
							input++;
							lexeme.line++;

							if(input == '\n')
								input++;

							lexeme.line_start = &input;
							break;

						case 0:
							if(process_null(&input))
								goto done;
							
						default:
							input++;
					}
				}
				done:

				#ifdef GALAXY
					lexeme.stop = &input;
					document.report(lexeme.dup(memory_pool), "C comments are not supported in Galaxy");
				#endif

				restep(); 		
				break;
				
			default:
				lexeme.stop = &input;
				lexeme.type = Lexeme::DIV;
		}
	}

	void Lexer::ident()
	{
		input++;
		
		while(input.in('a', 'z') || input.in('A', 'Z') || input.in('0', '9') || input == '_')
			input++;

		lexeme.stop = &input;
		lexeme.type = Lexeme::IDENT;
		lexeme.value = string_pool.get(&lexeme);
	}
};
