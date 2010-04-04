#pragma once
#include "../common.hpp"
#include "../string_pool.hpp"
#include "node_list.hpp"

namespace Legion
{
	class Type;
	
	struct Node;
	struct TypeSymbol;
	
	struct Symbol
	{
		enum SymbolType {
			NONE,
			TYPE,
			FUNCTION,
			VARIABLE,
			TYPES
		};
		
		Symbol(SymbolType type) : type(type), name(0) {}

		void redeclared(Document &document)
		{
			range->report(document, "Redeclared identifier '" + name->string() + "'");
		}

		static std::string names[TYPES];
		
		SymbolType type;
		Range *range;
		String *name;
		Symbol *next_match; // Next for chained symbols in a Scope
		Symbol *next; // Next for symbols in a NodeList
	};

	typedef NodeList<Symbol> SymbolList;
	
	template<Symbol::SymbolType symbol_type> struct SymbolType:
		public Symbol
	{
		SymbolType() : Symbol(symbol_type) {}
	};

	struct TypeSymbol:
		public SymbolType<Symbol::TYPE>
	{
		TypeSymbol() : node(0) {}

		Node *node;
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
