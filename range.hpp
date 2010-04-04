#pragma once
#include "common.hpp"
#include "tree/symbols.hpp"

namespace Legion
{
	class Document;

	class Range
	{
		public:
			const char_t *start;
			const char_t *stop;
			const char_t *line_start;
			size_t line;
			bool error;
			
			Range() : start(0), stop(0), error(false) {}
			
			void capture(const Range &range)
			{
				start = range.start;
				stop = range.stop;
				line_start = range.line_start;
				line = range.line;
				error = range.error;
			}
			
			Range(const Range &range) : start(0), stop(0), error(false)
			{
				capture(range);
			}
			
			void expand(Range &range)
			{
				stop = range.stop;
			}

			std::string string()
			{
				std::string result((const char *)start, length());

				return result;
			}
			
			std::string get_line()
			{
				const char_t *input = line_start;
				
				while(true)
					switch(*input)
					{
						case 0:
						case 13:
						case 10:
							goto done;
							
						default:
							input++;	
					}
					
				done:
					
				std::string result((const char *)line_start, (size_t)(input - line_start));
						
				return result;
			}

			size_t length()
			{
				return stop - start;
			}

			void report(Document *document, const std::string& error);
			
			void report(Document &document, const std::string& error)
			{
				report(&document, error);
			}

			void report_type_modifier(Document &document)
			{
				report(&document, "Unknown type modifier '" + string() + "'");
			}
			
			void report_expected_symbol(Document &document, Symbol *symbol, String *name, Symbol::SymbolType type)
			{
				if(symbol)
					report(document, "Expected " + Symbol::names[type] + ", but found '" + name->string() + "' (" + Symbol::names[symbol->type] + ")");
				else
					report(document, "Undeclared " + Symbol::names[type] + " '" + name->string() + "'");
			}
	};
};
