#include "symbols.hpp"
#include "../range.hpp"

namespace Legion
{
	std::string Symbol::names[TYPES] = {
		"none",
		"type",
		"function",
		"variable"
	};

	void Symbol::redeclared(Document &document)
	{
		range->report(document, "Redeclared identifier '" + name->string() + "'");
	}
};