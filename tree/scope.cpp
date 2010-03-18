#include "scope.hpp"

namespace Legion
{
	Scope::Scope() : table(0), parent(0)
	{
	}

	Scope::~Scope()
	{
	}
	
	void Scope::setup(StringPool *string_pool, MemoryPool *memory_pool)
	{
		this->string_pool = string_pool;
		this->memory_pool = memory_pool;
		
		this->size = 3;
		this->entries = 0;
		this->mask = (1 << this->size) - 1;
		this->table = new (this->memory_pool) Symbol *[mask + 1];
	}
	
	void Scope::expand()
	{
		size_t size = this->size + 1;
		size_t real_size = 1 << size;
		size_t mask = real_size - 1;
		Symbol **table = new (this->memory_pool) Symbol *[real_size];
		Symbol **end = table + real_size;
		
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
	
	bool Scope::store(Symbol **table, size_t mask, String *name, Symbol *symbol)
	{
		size_t index = (size_t)name & mask;
		Symbol *entry = table[index];
		Symbol *tail = entry;
		
		while(entry)
		{
			if(entry->name == name)
				return false;
			
			tail = entry;
			entry = entry->next;
		}
		
		if(tail)
			tail->next = symbol;
		else
			table[index] = symbol;
		
		return true;
	}
	
	Symbol *Scope::get(String *name)
	{
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
	
	bool Scope::set(String *name, Symbol *symbol)
	{
		bool result = store(this->table, this->mask, name, symbol);
		
		if(result)
		{
			this->entries++;
			
			if(this->entries > this->mask)
				expand();
		}
		
		return result;
	}
};