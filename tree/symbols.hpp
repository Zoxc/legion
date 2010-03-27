#pragma once
#include "../common.hpp"

namespace Legion
{
	class String;
	class Type;
	
	struct TypeSymbol;
	
	struct Symbol
	{
		enum Type {
			NONE,
			TYPE,
			FUNCTION,
			VARIABLE
		};
		
		Symbol(Type type) : type(type), name(0) {}

		void redeclared(Document *document)
		{
			range->report(document, "Redeclared identifier '" + name->string() + "'");
		}
		
		Type type;
		Range *range;
		String *name;
		Symbol *next;
	};
	
	template<Symbol::Type symbol_type> struct SymbolType:
		public Symbol
	{
		SymbolType() : Symbol(symbol_type) {}
	};

	struct TypeSymbol:
		public SymbolType<Symbol::TYPE>
	{
		Type *type_ptr;
	};

	struct VarSymbol:
		public SymbolType<Symbol::VARIABLE>
	{
		Type *type_ptr;
	};
	
	struct FuncSymbol:
		public SymbolType<Symbol::FUNCTION>
	{
		FuncSymbol() : returns(0), defined(false) {}

		Type *returns;
		bool defined;
	};
};
