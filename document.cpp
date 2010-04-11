#include "document.hpp"
#include "compiler.hpp"

namespace Legion
{
	Document::Document(Compiler &compiler, std::string filename) : input(0), compiler(compiler), parser(compiler.string_pool, memory_pool, *this, &compiler.scope), scope(0)
	{
		this->filename = normalize(compiler.string_pool.get(filename));
		compiler.documents.append(this);
	}

	Document::Document(Compiler &compiler, String *filename) : input(0), compiler(compiler), parser(compiler.string_pool, memory_pool, *this, &compiler.scope), scope(0)
	{
		this->filename = normalize(filename);
		compiler.documents.append(this);
	}

	Document::~Document()
	{
		free((void *)input);
	}

	String *Document::normalize(String *filename)
	{
		char_t *string = new (memory_pool) char_t[filename->length];
		char_t *end = string + filename->length;
		const char_t *i = filename->c_str;

		for(char_t *c = string; c != end; c++, i++)
		{
			if(*i == '\\')
				*c = '/';
			else
				*c = *i;
		}

		return compiler.string_pool.get(string, end);
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

	void Document::report(Range &range, std::string text, Message::Severity severity)
	{
		new StringMessage(*this, range, severity, text);
	}

	void Document::report_type_modifier(Range &range)
	{
		new StringMessage(*this, range, Message::LEGION_ERROR, "Unknown type modifier '" + range.string() + "'");
	}

	bool Document::load(std::string filename)
	{
		return map(filename);
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
			return false;

		fseek(file, 0, SEEK_END);

		size_t length = ftell(file);

		fseek(file, 0, SEEK_SET);

		char_t *data = (char_t *)malloc(length + 1);

		if(fread(data, 1, length, file) != length)
		{
			free(data);
			fclose(file);
			
			return false;
		}

		data[length] = 0;

		fclose(file);
		
		this->input = data;
		this->length = length;
		
		return true;
	}
};
