#include "common.hpp"
#include "compiler.hpp"

using namespace Legion;

int main(int argc, char *argv[])
{
	std::cout << "Legion - a compiler that targets Galaxy" << std::endl;

	Compiler compiler;
	
	char str[101];
	std::cin.getline(str, 101);
	
	Lexer *lexer = &compiler.parser.lexer;
	
	lexer->load((char_t *)str, strlen(str));

	for(; lexer->lexme.type != Lexme::End; lexer->step())
	{
		std::cout << "Token " << Lexme::names[lexer->lexme.type] << " (" << string_pool->get(&lexer->lexme)->cstr << ")" << std::endl;
	}

	return 0;
}
