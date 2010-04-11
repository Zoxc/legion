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

LEGION_API struct legion_document *legion_document_create(struct legion_compiler *compiler, const char *name)
{
	return (struct legion_document *)new Document(*(Compiler *)compiler, name);
}

LEGION_API void legion_document_destroy(struct legion_document *document)
{
	delete (Document *)document;
}

LEGION_API const char *legion_document_filename(struct legion_document *document)
{
	return ((Document *)document)->filename.c_str();
}

LEGION_API void legion_document_load_data(struct legion_document *document, void *data, size_t length)
{
	((Document *)document)->load((const char_t *)data, length);
}

LEGION_API void legion_document_load_file(struct legion_document *document, const char *filename)
{
	((Document *)document)->load(filename);
}

LEGION_API size_t legion_document_include_count(struct legion_document *document)
{
	return ((Document *)document)->includes.size();
}

LEGION_API const char *legion_document_include_get(struct legion_document *document, size_t index)
{
	return ((Document *)document)->includes.at(index).c_str();
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

LEGION_API struct legion_message *legion_message_first(struct legion_document *document)
{
	return (struct legion_message *)((Document *)document)->messages.first;
}

LEGION_API struct legion_message *legion_message_next(struct legion_message *message)
{
	return (struct legion_message *)((Message *)message)->next;
}

LEGION_API const char *legion_message_string(struct legion_message *message)
{
	return ((Message *)message)->string().c_str();
}

LEGION_API enum legion_severity legion_message_severity(struct legion_message *message)
{
	return (enum legion_severity)((Message *)message)->severity;
}

LEGION_API const char *legion_message_start(struct legion_message *message)
{
	return (const char *)((Message *)message)->range.start;
}

LEGION_API const char *legion_message_stop(struct legion_message *message)
{
	return (const char *)((Message *)message)->range.stop;
}

LEGION_API size_t legion_message_line(struct legion_message *message)
{
	return ((Message *)message)->range.line;
}
