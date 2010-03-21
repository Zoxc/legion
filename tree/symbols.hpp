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
			enum SymbolType {
				NONE,
				TYPE,
				FUNCTION,
				VARIABLE
			};
			
			template<class T> static SymbolType symbol_type() { return NONE; }
			template<> static SymbolType symbol_type<TypeSymbol>() { return TYPE; }
			
			SymbolType type;
			String *name;
			Symbol *next;
	};
	
	class TypeSymbol:
		public Symbol
	{
		Type *type_ptr;
	};
};
