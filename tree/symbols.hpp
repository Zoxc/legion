#pragma once
#include "../common.hpp"

namespace Legion
{
	class String;
	class Type;
	
	class TypeSymbol;
	
	class Symbol
	{
		public:
			enum Type {
				NONE,
				TYPE,
				FUNCTION,
				VARIABLE
			};
			
			Type type;
			String *name;
			Symbol *next;
	};
	
	template<Symbol::Type type> class SymbolType:
		public Symbol
	{
		public:
			Symbol::Type symbol_type()
			{
				return type;
			}
	};

	class TypeSymbol:
		public SymbolType<Symbol::TYPE>
	{
		Type *type_ptr;
	};
};
