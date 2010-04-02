#pragma once
#include "common.hpp"

namespace Legion
{
	class MemoryPool
	{
		private:
			std::vector<char_t *> pages;
			
			char_t *current_page;
			char_t *current;
			char_t *max;

			char_t *allocate_page();
			void free_page(char_t *page);
			
			void *get_page(size_t bytes);
			
		public:
			MemoryPool();
			~MemoryPool();
			
			void *allocate(size_t bytes);
	};
};

void *operator new(size_t bytes, Legion::MemoryPool *memory_pool) throw();
void operator delete(void *, Legion::MemoryPool *memory_pool) throw();
void *operator new[](size_t bytes, Legion::MemoryPool *memory_pool) throw();
void operator delete[](void *, Legion::MemoryPool *memory_pool) throw();

void *operator new(size_t bytes, Legion::MemoryPool &memory_pool) throw();
void operator delete(void *, Legion::MemoryPool &memory_pool) throw();
void *operator new[](size_t bytes, Legion::MemoryPool &memory_pool) throw();
void operator delete[](void *, Legion::MemoryPool &memory_pool) throw();
