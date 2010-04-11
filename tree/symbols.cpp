#include "symbols.hpp"
#include "../range.hpp"
#include "../message.hpp"

namespace Legion
{
	std::string Symbol::names[TYPES] = {
		"none",
		"type",
		"function",
		"prototype",
		"variable"
	};

	void Symbol::redeclared(Document &document)
	{
		new RedeclaredSymbolError(document, this);
	}
};