#include "common.hpp"
#include "compiler.hpp"
#include "document.hpp"
#include "tree/printer.hpp"

using namespace Legion;

Compiler compiler;

std::vector<Document *> documents;
std::vector<Document *> queue;

void include(std::string file)
{
	file = file + ".galaxy";

	for(std::vector<Document *>::iterator i = documents.begin(); i != documents.end(); i++)
	{
		if((*i)->filename == file)
			return;
	}
	
	Document *document = new Document(compiler, file);

	queue.push_back(document);
	documents.push_back(document);
}

#ifdef WIN32
	#define BENCHMARK_VARS __int64 _start, _stop, _freq

	#define BENCHMARK_START \
		QueryPerformanceFrequency((LARGE_INTEGER *)&_freq); \
		QueryPerformanceCounter((LARGE_INTEGER *)&_start)

	#define BENCHMARK_END(action) \
				QueryPerformanceCounter((LARGE_INTEGER *)&_stop); \
				std::cout << action << " in " << (((double)1000 * (_stop - _start)) / (double)_freq) << " ms." << std::endl
#else
	#define BENCHMARK_VARS
	#define BENCHMARK_START
	#define BENCHMARK_END(action)
#endif

void print_ast(std::string name)
{
	#ifdef AST_DEBUG
		DebugPrinter printer;

		for(std::vector<Document *>::iterator i = documents.begin(); i != documents.end(); i++)
		{
			std::cout << std::endl << "Printing " + name + " for " + (*i)->filename << std::endl;
			std::cout << printer.print(&(*i)->tree) << std::endl;
		}
	#endif
}

int main(int argc, char *argv[])
{
	for(int i = 1; i < argc; i++)
	{
		Document *document = new Document(compiler, argv[i]);

		document->load(argv[i]);

		queue.push_back(document);
		documents.push_back(document);
	}

	BENCHMARK_VARS;

	BENCHMARK_START;

	while(queue.size() > 0)
	{
		Document *document = queue.back();
		queue.pop_back();

		document->parse();

		for(std::vector<std::string>::iterator i = document->includes.begin(); i != document->includes.end(); ++i)
			include(*i);
	}

	BENCHMARK_END("Parsed files");

	print_ast("pre-AST");

	BENCHMARK_START;

	for(std::vector<Document *>::iterator i = documents.begin(); i != documents.end(); i++)
		(*i)->find_declarations();

	BENCHMARK_END("Found declarations");

	print_ast("post-AST");

	BENCHMARK_START;

	for(std::vector<Document *>::iterator i = documents.begin(); i != documents.end(); i++)
		(*i)->validate();

	BENCHMARK_END("Typechecked");

	print_ast("post-typechecking-AST");


	for(std::vector<Document *>::iterator i = documents.begin(); i != documents.end(); i++)
		for(List<Message>::Iterator j = (*i)->messages.begin(); j; j++)
			std::cout << j().format() << std::endl;

	return 0;
}
