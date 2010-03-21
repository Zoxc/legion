#pragma once
#include "common.hpp"

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
	};
};
