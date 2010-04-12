#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#ifdef LEGION_BUILDING
	#define LEGION_API __declspec(dllexport)
#else
	#define LEGION_API __declspec(dllimport)
#endif

struct legion_compiler;
struct legion_document;
struct legion_message;
struct legion_include;

struct legion_string
{
	const char *c_str;
	size_t length;
};

LEGION_API struct legion_compiler *legion_compiler_create();
LEGION_API void legion_compiler_destroy(struct legion_compiler *compiler);

enum legion_stages
{
	LEGION_PARSE,
	LEGION_DECLARE,
	LEGION_VALIDATE,
	LEGION_STAGES
};

LEGION_API struct legion_document *legion_document_create(struct legion_compiler *compiler, const char *name);
LEGION_API void legion_document_destroy(struct legion_document *document);
LEGION_API struct legion_string *legion_document_filename(struct legion_document *document);
LEGION_API void legion_document_load_data(struct legion_document *document, void *data, size_t length);
LEGION_API bool legion_document_load_file(struct legion_document *document, const char *filename);
LEGION_API void legion_document_execute(struct legion_document *document, enum legion_stages stage);

LEGION_API struct legion_include *legion_include_first(struct legion_document *document);
LEGION_API struct legion_include *legion_include_next(struct legion_include *include);
LEGION_API struct legion_string *legion_include_filename(struct legion_include *include);
LEGION_API bool legion_include_included(struct legion_include *include);
LEGION_API void legion_include_report(struct legion_include *include);

enum legion_severity
{
	LEGION_HINT,
	LEGION_WARNING,
	LEGION_ERROR,
	LEGION_SEVERITIES
};

LEGION_API struct legion_message *legion_message_first(struct legion_document *document);
LEGION_API struct legion_message *legion_message_next(struct legion_message *message);
LEGION_API const char *legion_message_string(struct legion_message *message);
LEGION_API enum legion_severity legion_message_severity(struct legion_message *message);
LEGION_API size_t legion_message_start(struct legion_message *message);
LEGION_API size_t legion_message_stop(struct legion_message *message);
LEGION_API size_t legion_message_line(struct legion_message *message);

#ifdef __cplusplus
}
#endif
