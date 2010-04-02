#include "memory_pool.hpp"

#ifdef VALGRIND
	#include <malloc.h>
#endif

namespace Legion
{
	const unsigned int page_size = 0x1000 * 4;

	MemoryPool::MemoryPool()
	{
		current = current_page = allocate_page();
		max = current + page_size;
	}

	MemoryPool::~MemoryPool()
	{
		for(std::vector<char_t *>::iterator page = pages.begin(); page != pages.end(); page++)
			free_page(*page);

		free_page(current_page);
	}

	char_t *MemoryPool::allocate_page()
	{
		#ifdef WIN32
			return (char_t *)VirtualAlloc(0, page_size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
		#else	
			return (char_t *)malloc(page_size);
		#endif
	}

	void MemoryPool::free_page(char_t *page)
	{
		#ifdef WIN32
			VirtualFree((void *)page, 0, 0);
		#else
			free(page);
		#endif
	}

	void *MemoryPool::get_page(size_t bytes)
	{
		pages.push_back(current_page);
		
		char_t *result = allocate_page();
		
		current_page = result;
		current = result + bytes;
		max = result + page_size;
		
		return result;
	}

	void *MemoryPool::allocate(size_t bytes)
	{
		char_t *result;

		#ifdef VALGRIND
			result = (char_t *)malloc(bytes);

			pages.push_back(result);
		#else
			result = current;

			char_t *next = result + bytes;
		
			if(next >= max)
				return get_page(bytes);

			current = next;
		#endif

		return (void *)result;
	}
};

void *operator new(size_t bytes, Legion::MemoryPool *memory_pool) throw()
{
	return memory_pool->allocate(bytes);
}

void operator delete(void *, Legion::MemoryPool *memory_pool) throw()
{
}

void *operator new[](size_t bytes, Legion::MemoryPool *memory_pool) throw()
{
	return memory_pool->allocate(bytes);
}

void operator delete[](void *, Legion::MemoryPool *memory_pool) throw()
{
}

void *operator new(size_t bytes, Legion::MemoryPool &memory_pool) throw()
{
	return memory_pool.allocate(bytes);
}

void operator delete(void *, Legion::MemoryPool &memory_pool) throw()
{
}

void *operator new[](size_t bytes, Legion::MemoryPool &memory_pool) throw()
{
	return memory_pool.allocate(bytes);
}

void operator delete[](void *, Legion::MemoryPool &memory_pool) throw()
{
}
