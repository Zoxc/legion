#include "common.hpp"
#include "compiler.hpp"

using namespace Legion;

int __cdecl main(int argc, char *argv[])
{
	std::cout << "Legion - a compiler that targets Galaxy" << std::endl;

	Compiler compiler;
	
	Lexer *lexer = &compiler.parser.lexer;
		
	while(true)
	{
		char *str = (char *)compiler.memory_pool.allocate(101);
		std::cin.getline(str, 101);
		
		if(*str == 0)
			break;
			
		std::cout << "Parsing string: " << str << std::endl;
		
		lexer->load((char_t *)str, strlen(str));

		for(; lexer->lexeme.type != Lexeme::END; lexer->step())
		{
			std::cout << "Token " << Lexeme::names[lexer->lexeme.type] << " (";
			
			switch(lexer->lexeme.type)
			{
				case Lexeme::STRING:
					std::cout << lexer->lexeme.value->c_str;
					break;
					
				default:
					std::cout << lexer->lexeme.string();
			}
			
			std::cout  << ")" << std::endl;
		}
	}

	return 0;
}
