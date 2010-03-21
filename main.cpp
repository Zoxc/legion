#include "common.hpp"
#include "compiler.hpp"
#include "document.hpp"

using namespace Legion;

int main(int argc, char *argv[])
{
	std::cout << "Legion - a compiler that targets Galaxy" << std::endl;

	Compiler compiler;
	
	Document doc(&compiler, "MapScript.galaxy");
	
	doc.parser.parse(&doc.tree);
	
	getchar();
	
	return 0;
}
