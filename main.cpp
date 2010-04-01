#include "common.hpp"
#include "compiler.hpp"
#include "document.hpp"
#include "tree/printer.hpp"

using namespace Legion;

Compiler compiler;

std::vector<std::string> queued;
std::vector<std::string> queue;

void include(std::string file)
{
	file = file + ".galaxy";

	for(std::vector<std::string>::iterator i = queued.begin(); i != queued.end(); ++i)
	{
		if(*i == file)
			return;
	}
	
	queue.push_back(file);
	queued.push_back(file);
}

void process_file(std::string file)
{
	DebugPrinter printer;

	Document doc(&compiler, file);

	#ifdef WIN32
		__int64 start, stop, freq;
		QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
		QueryPerformanceCounter((LARGE_INTEGER *)&start);
	#endif

	if(doc.parse())
	{
		std::cout << printer.print(&doc.tree);

		doc.find_declarations();
		
		std::cout << printer.print(&doc.tree);

		#ifdef WIN32
			QueryPerformanceCounter((LARGE_INTEGER *)&stop);

			std::cout << "Parsed file '" << file << "' in " << (((double)1000 * (stop - start)) / (double)freq) << " ms." << std::endl;
		#endif

		for(std::vector<std::string>::iterator i = doc.includes.begin(); i != doc.includes.end(); ++i)
			include(*i);
	}
}

int main(int argc, char *argv[])
{
	std::cout << "Legion - a compiler that targets Galaxy" << std::endl;

	for(int i = 1; i < argc; i++)
	{
		queue.push_back(argv[i]);
		queued.push_back(argv[i]);
	}
	
	while(queue.size() > 0)
	{
		std::string file = queue.back();
		queue.pop_back();
		process_file(file);
	}
	
	return 0;
}
