#include "range.hpp"
#include "document.hpp"
#include "tree/types.hpp"

namespace Legion
{
	void Range::report_types(Document &document, Type *from, Type *to)
	{
		report(document, "Unable to convert type '" + from->string() + "' to type '" + to->string() + "'");
	}

	void Range::report(Document *document, const std::string& error)
	{
		std::cout << document->filename << "[" << line + 1 << "]: " << error << std::endl << get_line() << std::endl;
		
		for(const char_t *i = line_start; i < start; i++)
		{
			if(*i == '\t')
				std::cout << '\t';
			else
				std::cout << " ";
		}
		
		if(length() == 1)
			std::cout << "^";
		else
		{
			for(const char_t *i = start; i < stop; i++)
			{
				switch(*i)
				{
					case 0:
					case 13:
					case 10:
						goto done;
						
					default:
						std::cout << "~";	
				}
			}
		}
			
		done:
			
		std::cout << std::endl;
	}
};