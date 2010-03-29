#include "scope.hpp"
#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"
#include "symbols.hpp"

namespace Legion
{
	Scope::Scope(Scope *parent, Type type, MemoryPool *memory_pool) : table(0), memory_pool(memory_pool), type(type), parent(parent)
	{
		size = 4;
		
		size_t real_size = 1 << size;
		
		entries = 0;
		mask = real_size - 1;
		table = new (memory_pool) Symbol *[real_size];
		
		memset(table, 0, real_size * sizeof(Symbol *));
	}

	Scope::~Scope()
	{
	}
	
	void Scope::expand()
	{
		size_t size = this->size + 1;
		size_t real_size = 1 << size;
		size_t mask = real_size - 1;
		Symbol **table = new (this->memory_pool) Symbol *[real_size];
		Symbol **end = table + real_size;
		
		memset(table, 0, real_size * sizeof(Symbol *));
		
		for(Symbol **slot = table; slot != end; slot++)
		{
			Symbol *symbol = *slot;
			
			while(symbol)
			{
				Symbol *next = symbol->next; 
				
				store(table, mask, symbol->name, symbol);
				
				symbol = next;
			}
		}
		
		this->size = size;
		this->mask = mask;
		this->table = table;
	}
	
	Symbol *Scope::store(Symbol **table, size_t mask, String *name, Symbol *symbol)
	{
		size_t index = (size_t)name & mask;
		Symbol *entry = table[index];
		Symbol *tail = entry;
		
		while(entry)
		{
			if(entry->name == name)
				return entry;
			
			tail = entry;
			entry = entry->next;
		}
		
		if(tail)
			tail->next = symbol;
		else
			table[index] = symbol;

		symbol->next = 0;
		
		return 0;
	}
	
	Symbol *Scope::get(String *name)
	{
		if(!name)
			return 0;

		size_t index = (size_t)name & this->mask;
		Symbol *entry = this->table[index];
		
		while(entry)
		{
			if(entry->name == name)
				return entry;
				
			entry = entry->next;
		}
		
		return 0;
	}
	
	Symbol *Scope::set(String *name, Symbol *symbol)
	{
		Symbol *result = store(this->table, this->mask, name, symbol);
		
		if(!result)
		{
			this->entries++;
			
			if(this->entries > this->mask)
				expand();
		}
		
		return result;
	}
};

