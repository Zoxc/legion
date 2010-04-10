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
				
			Symbol *lookup(String *name, Document *document, Symbol::SymbolType type, bool &found)
			{
				Symbol *start = lookup(name);
				Symbol *current = start;
				Symbol *result = 0;

				bool found_document = false;

				found = false;

				if(!start)
					return 0;

				do
				{
					if(current->type == type)
					{
						if(current->document == document)
						{
							if(found_document)
								return 0;
							else
							{
								found_document = true;
								result = current;
							}
						}
						else
						{
							if(found && !found_document)
								return 0;
							else
							{
								found = true;
								result = current;
							}
						}
					}

					current = current->next_name;
				}
				while(current != start);

				if(found_document)
					found = true;

				return result;
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

			bool declare_symbol(Symbol *symbol)
			{
				#ifdef GALAXY
					switch(type)
					{
						case LOOP:
						case CONDITIONAL:
						case EMPTY:
							return parent->declare_symbol(symbol);

						default:
							break;
					}
				#endif
				
				if(!symbol)
					return false;
				
				if(!symbol->name)
					return false;
				
				Symbol *prev = set(symbol->name, symbol);

				if(prev)
				{
					symbol->next_name = prev->next_name;
					prev->next_name = symbol;

					return true;
				}
				else
				{
					symbol->next_name = symbol;

					return false;
				}
			}
	};
};
