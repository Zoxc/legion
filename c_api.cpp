#include "c_api.h"
#include "compiler.hpp"
#include "document.hpp"

using namespace Legion;

LEGION_API struct legion_compiler *legion_compiler_create()
{
	return (struct legion_compiler *)new Compiler;
}

LEGION_API void legion_compiler_destroy(struct legion_compiler *compiler)
{
	delete (Compiler *)compiler;
}

LEGION_API struct legion_string *legion_compiler_string(struct legion_compiler *compiler, const char *string, size_t length)
{
	return (struct legion_string *)((Compiler *)compiler)->string_pool.get((const char_t*)string, length);
}

LEGION_API struct legion_document *legion_document_create(struct legion_compiler *compiler, struct legion_string *name)
{
	return (struct legion_document *)new Document(*(Compiler *)compiler, (String *)name);
}

LEGION_API void legion_document_destroy(struct legion_document *document)
{
	delete (Document *)document;
}

LEGION_API struct legion_string *legion_document_filename(struct legion_document *document)
{
	return (struct legion_string *)((Document *)document)->filename;
}

LEGION_API void legion_document_load_data(struct legion_document *document, const void *data, size_t length)
{
	((Document *)document)->load((const char_t *)data, length);
}

LEGION_API bool legion_document_load_file(struct legion_document *document, struct legion_string *filename)
{
	return ((Document *)document)->load(((String *)filename)->string());
}

LEGION_API void legion_document_execute(struct legion_document *document, enum legion_stages stage)
{
	Document *doc = (Document *)document;

	switch(stage)
	{
		case LEGION_PARSE:
			doc->parse();
			break;

		case LEGION_DECLARE:
			doc->find_declarations();
			break;

		case LEGION_VALIDATE:
			doc->validate();
			break;
	}
}


LEGION_API struct legion_include *legion_include_first(struct legion_document *document)
{
	return (struct legion_include *)((Document *)document)->includes.first;
}

LEGION_API struct legion_include *legion_include_next(struct legion_include *include)
{
	return (struct legion_include *)((IncludeNode *)include)->next;
}

LEGION_API struct legion_string *legion_include_filename(struct legion_include *include)
{
	return (struct legion_string *)((IncludeNode *)include)->filename;
}

LEGION_API bool legion_include_included(struct legion_include *include)
{
	return ((IncludeNode *)include)->found;
}

LEGION_API void legion_include_report(struct legion_include *include)
{
	return ((IncludeNode *)include)->report();
}


LEGION_API struct legion_message *legion_message_first(struct legion_document *document)
{
	return (struct legion_message *)((Document *)document)->messages.first;
}

LEGION_API struct legion_message *legion_message_next(struct legion_message *message)
{
	return (struct legion_message *)((Message *)message)->next;
}

LEGION_API struct legion_string *legion_message_string(struct legion_message *message)
{
	Message *msg = (Message *)message;

	std::string string = msg->string();

	return (struct legion_string *)msg->document.compiler.string_pool.get(string);
}

LEGION_API enum legion_severity legion_message_severity(struct legion_message *message)
{
	return (enum legion_severity)((Message *)message)->severity;
}

LEGION_API size_t legion_message_start(struct legion_message *message)
{
	Message *msg = (Message *)message;

	return msg->range.start - msg->document.input;
}

LEGION_API size_t legion_message_stop(struct legion_message *message)
{
	Message *msg = (Message *)message;

	return msg->range.stop - msg->document.input;
}

LEGION_API size_t legion_message_line(struct legion_message *message)
{
	return ((Message *)message)->range.line;
}
