#include "lexer.hpp"
#include "../string_pool.hpp"
#include "../document.hpp"

namespace Legion
{
	void Lexer::build_string(const char_t *start, char_t *str)
	{
		char_t *writer = str;
		const char_t *input = start;
	 
		while(true)
			switch(*input)
			{
				case '"':
					goto done;

				case '\\':
					input++;

					switch(*input)
					{
						case '\'':
						case '\"':
						case '\\':
							*writer++ = *input++;
							break;
						
						case '0':
							*writer++ = 0;
							input++;
							break;
						
						case 'a':
							*writer++ = 7;
							input++;
							break;
						
						case 'b':
							*writer++ = 8;
							input++;
							break;
						
						case 'f':
							*writer++ = 12;
							input++;
							break;
						
						case 'n':
							*writer++ = 10;
							input++;
							break;
						
						case 'r':
							*writer++ = 13;
							input++;
							break;
						
						case 't':
							*writer++ = 9;
							input++;
							break;
						
						case 'v':
							*writer++ = 11;
							input++;
							break;
						
						case 0:
							if(process_null(input))
								goto done;

						default:
							*writer++ = '\\';
							*writer++ = *input++;
					}
					break;
					
				case 0:
					if(process_null(input))
						goto done;

				default:
					*writer++ = *input++;
			}
			
		done:;
	}

	void Lexer::string()
	{
		input++;
		
		lexeme.type = Lexeme::STRING;
		size_t overhead = 2;

		while(true)
			switch(input)
			{
				case '"':
					input++;
					goto done;

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

				case '\\':
					input++;

					switch(input)
					{
						case '\'':
						case '\"':
						case '\\':
						case '0':
						case 'a':
						case 'b':
						case 'f':
						case 'n':
						case 'r':
						case 't':
						case 'v':
							overhead++;
							input++;
							break;
						
						case 0:
							if(process_null(&input))
							{
								lexeme.stop = &input;

								document.report(lexeme.dup(memory_pool), "Unterminated string");
								goto done;
							}
							else
								input++;
							break;
							
						default:
							const char_t *start = lexeme.start;
							lexeme.start = &input - 1;
							lexeme.stop = &input + 1;
							document.report(lexeme.dup(memory_pool), "Invalid escape string");
							lexeme.start = start;
					}
					break;

				case 0:
					if(process_null(&input))
					{
						lexeme.stop = &input;
						document.report(lexeme.dup(memory_pool), "Unterminated string");
						goto done;
					}
					else
						report_null();
				
				default:
					input++;		
			}
		
		done:
		lexeme.stop = &input;
		
		size_t str_length = lexeme.length() - overhead;
		char_t *str = new (memory_pool) char_t[str_length];
		
		build_string(lexeme.start + 1, str);
		
		lexeme.value = string_pool.get(str, str_length);
	}
};
