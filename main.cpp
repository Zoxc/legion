#include "common.hpp"
#include "lexer/lexer.hpp"

using namespace Legion;

int main(int argc, char *argv[])
{
	std::cout << "Legion - a compiler that targets Galaxy" << std::endl;

	Lexer lexer;
	
	char str[101];
	std::cin.getline(str, 101);
	
	lexer.setup((char_t *)str, 101);

	for(; lexer.lexme.type != Lexme::End; lexer.step())
	{
		std::cout << "Token " << Lexme::names[lexer.lexme.type] << std::endl;
	}

	return 0;
}
