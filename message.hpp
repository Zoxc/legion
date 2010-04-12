#pragma once
#include "common.hpp"
#include "list.hpp"
#include "lexer/lexeme.hpp"
#include "tree/symbols.hpp"

namespace Legion
{
	class Document;
	class Range;

	class Message
	{
		public:
			enum Severity
			{
				LEGION_HINT,
				LEGION_WARNING,
				LEGION_ERROR,
				SEVERITIES
			};

			Message(Document &document, Range &range, Severity severity);

			static std::string severity_names[SEVERITIES];

			Document &document;
			Range &range;
			Severity severity;
			List<Message> children;
			Message *next;

			virtual std::string string() = 0;

			std::string format();
	};

	class StringMessage:
		public Message
	{
		private:
			std::string text;
		public:
			StringMessage(Document &document, Range &range, Severity severity, std::string text) : Message(document, range, severity), text(text) {}

			std::string string()
			{
				return text;
			}
	};

	struct Symbol;
	class String;

	class WrongSymbolError:
		public Message
	{
		private:
			Symbol *found;
			String *name;
			Symbol::SymbolType type;
		public:
			WrongSymbolError(Document &document, Range &range, Symbol *found, String *name, Symbol::SymbolType type) : Message(document, range, LEGION_ERROR), found(found), name(name), type(type) {}

			std::string string();
	};

	class RedeclaredSymbolError:
		public Message
	{
		private:
			Symbol *symbol;
		public:
			RedeclaredSymbolError(Document &document, Symbol *symbol) : Message(document, *symbol->range, LEGION_ERROR), symbol(symbol) {}

			std::string string();
	};

	class IncompatableTypesError:
		public Message
	{
		private:
			Type *from;
			Type *to;
		public:
			IncompatableTypesError(Document &document, Range &range, Type *from, Type *to) : Message(document, range, LEGION_ERROR), from(from), to(to) {}

			std::string string();
	};

	class ExpectedError:
		public Message
	{
		private:
			Lexeme::Type what;
		public:
			ExpectedError(Document &document, Range &range, Lexeme::Type what) : Message(document, range, LEGION_ERROR), what(what) {}

			std::string string();
	};
};
