#pragma once
#include "../common.hpp"
#include "../string_pool.hpp"
#include "../list.hpp"

namespace Legion
{
	class Type;
	class Range;
	class Document;
	
	struct Node;
	struct TypeSymbol;
	
	struct Symbol
	{
		enum SymbolType {
			NONE,
			TYPE,
			FUNCTION,
			PROTOTYPE,
			VARIABLE,
			TYPES
		};
		
		Symbol(SymbolType type) : type(type), name(0), node(0), document(0) {}

		void redeclared(Document &document);

		static std::string names[TYPES];
		
		SymbolType type;
		Range *range;
		String *name;
		Node *node;
		Document *document; // The document this symbol belongs to, if any
		Symbol *next_match; // Next for chained symbols in a Scope
		Symbol *next_name; // Next symbol that shares it's name
		Symbol *next; // Next for symbols in a NodeList
	};

	typedef List<Symbol> SymbolList;
	
	template<Symbol::SymbolType symbol_type> struct SymbolType:
		public Symbol
	{
		SymbolType() : Symbol(symbol_type) {}
	};

	struct TypeSymbol:
		public SymbolType<Symbol::TYPE>
	{
	};

	struct VarSymbol:
		public SymbolType<Symbol::VARIABLE>
	{
	};
	
	struct FuncSymbol:
		public SymbolType<Symbol::FUNCTION>
	{
	};
	
	struct PrototypeSymbol:
		public SymbolType<Symbol::PROTOTYPE>
	{
	};
};
