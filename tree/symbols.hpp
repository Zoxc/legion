#pragma once
#include "../common.hpp"

namespace Legion
{
	class String;
	class Type;
	
	struct TypeSymbol;
	
	struct Symbol:
		public Range
	{
		enum Type {
			NONE,
			TYPE,
			FUNCTION,
			VARIABLE
		};
		
		Symbol(Type type) : type(type), name(0)
		{
		}
		
		Type type;
		String *name;
		Symbol *next;
	};
	
	template<Symbol::Type type> struct SymbolType:
		public Symbol
	{
		SymbolType() : Symbol(type)
		{
		}
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
		public SymbolType<Symbol::VARIABLE>
	{
		Type *returns;
	};
};