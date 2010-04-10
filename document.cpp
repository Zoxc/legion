#include "document.hpp"
#include "compiler.hpp"

namespace Legion
{
	Document::Document(Compiler &compiler, std::string filename) : input(0), compiler(compiler), parser(&compiler.string_pool, &memory_pool, this, &compiler.scope), scope(0)
	{
		this->filename = filename;
	}

	Document::~Document()
	{
		free((void *)input);
	}

	bool Document::parse()
	{
		if(input)
		{
			parser.lexer.load(input, length);
			parser.parse(&tree);
			return true;
		}
		else
			return false;
	}

	void Document::load(std::string filename)
	{
		map(filename);
	}

	void Document::load(const char_t *input, size_t length)
	{
		this->input = input;
		this->length = length;
	}

	void Document::find_declarations()
	{
		scope = &compiler.scope;

		for(NamespaceList::Iterator i = tree.begin(); i; i++)
			(*i)->find_declarations(this);

		scope = 0;
	}

	void Document::validate()
	{
		ValidationArgs args(*this, compiler.types, &compiler.scope, memory_pool);

		for(NamespaceList::Iterator i = tree.begin(); i; i++)
			(*i)->validate(args);
	}
	
	bool Document::map(std::string filename)
	{
		FILE* file = fopen(filename.c_str(), "rb");

		if(!file)
		{
			std::cout << "Unable to open file '" << filename << "'." << std::endl;
			return false;
		}
		
		fseek(file, 0, SEEK_END);

		size_t length = ftell(file);

		fseek(file, 0, SEEK_SET);

		char_t *data = (char_t *)malloc(length + 1);

		if(fread(data, 1, length, file) != length)
		{
			free(data);
			fclose(file);
			
			std::cout << "Unable to read file '" << filename << "'." << std::endl;
			return false;
		}

		data[length] = 0;

		fclose(file);
		
		this->input = data;
		this->length = length;
		
		return true;
	}
};
