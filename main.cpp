#include "common.hpp"
#include "compiler.hpp"
#include "document.hpp"
#include "tree/printer.hpp"

using namespace Legion;

Compiler compiler;

bool include(String *filename)
{
	std::string file = filename->string() + "_beta.galaxy";

	Document &document = *new Document(compiler, filename);

	if(!document.load(file))
	{
		file = filename->string() + ".galaxy";

		if(!document.load(file))
			return false;
	}

	document.parse();

	for(List<IncludeNode>::Iterator i = document.includes.begin(); i; i++)
		if(!i().found)
			if(!include(i().filename))
				i().report(document);

	return true;
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

		for(List<Document>::Iterator i = compiler.documents.begin(); i; i++)
		{
			std::cout << std::endl << "Printing " + name + " for " + i().filename->string() << std::endl;
			std::cout << printer.print(&i().tree) << std::endl;
		}
	#endif
}

int main(int argc, char *argv[])
{
	BENCHMARK_VARS;

	BENCHMARK_START;

	for(int i = 1; i < argc; i++)
		if(!include(compiler.string_pool.get(argv[i])))
			std::cout << "Unable to open file '" << argv[i] << "'" << std::endl;

	BENCHMARK_END("Parsed files");

	print_ast("pre-AST");

	BENCHMARK_START;

	for(List<Document>::Iterator i = compiler.documents.begin(); i; i++)
		i().find_declarations();

	BENCHMARK_END("Found declarations");

	print_ast("post-AST");

	BENCHMARK_START;

	for(List<Document>::Iterator i = compiler.documents.begin(); i; i++)
		i().validate();

	BENCHMARK_END("Typechecked");

	print_ast("post-typechecking-AST");

	for(List<Document>::Iterator i = compiler.documents.begin(); i; i++)
		for(List<Message>::Iterator j = i().messages.begin(); j; j++)
			std::cout << j().format() << std::endl;

	return 0;
}
