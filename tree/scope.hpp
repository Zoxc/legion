#pragma once
#include "../common.hpp"
#include "../lexer/lexer.hpp"
#include "../hash_table.hpp"
#include "symbols.hpp"

namespace Legion
{
	class String;
	class StringPool;
	struct Symbol;
	class Parser;
	class Document;

	class ScopeFunctions:
		public HashTableFunctions<String *, Symbol *>
	{
		public:
			static bool compare_key_value(String *key, Symbol *value)
			{
				return value->name == key;
			}

			static String *get_key(Symbol *value)
			{
				return value->name;
			}

			static Symbol *get_value_next(Symbol *value)
			{
				return value->next_match;
			}

			static void set_value_next(Symbol *value, Symbol *next)
			{
				value->next_match = next;
			}
	};

	typedef HashTable<String *, Symbol *, ScopeFunctions> ScopeHashTable;

	class Scope:
		public ScopeHashTable
	{
		public:
			enum Type
			{
				ROOT,
				FUNCTION,
				LOOP,
				CONDITIONAL,
				EMPTY
			};
			
			Scope(Scope *parent, Type type, MemoryPool &memory_pool) : ScopeHashTable(memory_pool, 2), type(type), parent(parent) {}

			Type type;
			Scope *parent;
				
			Symbol *lookup(String *name)
			{
				Symbol *result = get(name);

				if(result)
					return result;
				else if(parent)
					return parent->lookup(name);
				else
					return 0;
			}
			
			bool find_type(Type target)
			{
				if((size_t)target < (size_t)type)
					return parent->find_type(target);
				else
					return type == target;
			}

			Symbol::SymbolType lookup_type(String *name)
			{
				Symbol *symbol = lookup(name);

				if(!symbol)
					return Symbol::NONE;

				return symbol->type;
			}

			Symbol *declare_symbol(Symbol *symbol)
			{
				switch(type)
				{
					case LOOP:
					case CONDITIONAL:
					case EMPTY:
						return parent->declare_symbol(symbol);

					default:
						break;
				}
				
				if(!symbol)
					return 0;
				
				if(!symbol->name)
					return 0;
				
				return set(symbol->name, symbol);
			}
	};
};
