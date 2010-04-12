#include "message.hpp"
#include "document.hpp"
#include "range.hpp"

namespace Legion
{
	std::string Message::severity_names[SEVERITIES] = {
		"Hint",
		"Warning",
		"Error"
	};

	Message::Message(Document &document, Range &range, Severity severity) : document(document), range(range), severity(severity)
	{
		List<Message>::Iterator i = document.messages.begin();

		for(; i; i++)
			if(i().range.start > range.start)
				break;

		i.insert(this);
	}

	std::string Message::format()
	{
		std::stringstream result;

		result << document.filename->string() << "[" << range.line + 1 << "]: " << severity_names[severity] << ": " << string() << std::endl << range.get_line() << std::endl;
		
		for(const char_t *i = range.line_start; i < range.start; i++)
		{
			if(*i == '\t')
				result << '\t';
			else
				result << " ";
		}
		
		if(range.length() == 1)
			result << "^";
		else
		{
			for(const char_t *i = range.start; i < range.stop; i++)
			{
				switch(*i)
				{
					case 0:
					case 13:
					case 10:
						goto done;
						
					default:
						result << "~";	
				}
			}
		}
			
		done:
		return result.str();
	}

	std::string WrongSymbolError::string()
	{
		if(found)
			return "Expected " + Symbol::names[type] + ", but found '" + found->name->string() + "' (" + Symbol::names[found->type] + ")";
		else
			return "Undeclared " + Symbol::names[type] + " '" + name->string() + "'";
	}

	std::string RedeclaredSymbolError::string()
	{
		return "Redeclared identifier '" + symbol->name->string() + "'";
	}
	
	std::string IncompatableTypesError::string()
	{
		return "Unable to convert type '" + from->string() + "' to type '" + to->string() + "'";
	}
	
	std::string ExpectedError::string()
	{
		return "Expected " + Lexeme::describe_type(what);
	}
};