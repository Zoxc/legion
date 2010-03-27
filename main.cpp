#include "common.hpp"
#include "compiler.hpp"
#include "document.hpp"

using namespace Legion;

#ifdef WIN32
__int64 freq;
#endif

int main(int argc, char *argv[])
{
	std::cout << "Legion - a compiler that targets Galaxy" << std::endl;

	if(argc < 2)
	{
		std::cout << "Please pass the document to parse as a command line parameter." << std::endl;

		return 2;
	}

	Compiler compiler;
	
	Document doc(&compiler, argv[1]);

	#ifdef WIN32
		__int64 start, stop;
		QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
		QueryPerformanceCounter((LARGE_INTEGER *)&start);
	#endif
	
	doc.parser.parse(&doc.tree);

	#ifdef WIN32
		QueryPerformanceCounter((LARGE_INTEGER *)&stop);

		std::cout << "Parsed document in " << (((double)1000 * (stop - start)) / (double)freq) << " ms." << std::endl;
	#endif
	
	return 0;
}
