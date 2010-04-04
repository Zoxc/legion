#pragma once
#include "common.hpp"
#include "hash_table.hpp"

namespace Legion
{
	class Range;

	
	class StringKey
	{
		public:
			const char_t *c_str;
			size_t length;
	};
	
	class String:
		public StringKey
	{
		public:
			String *next;
			
			std::string string()
			{
				std::string result((const char *)c_str, length);

				return result;
			}
	};

	class StringPoolFunctions:
		public HashTableFunctions<StringKey &, String *>
	{
		public:
			static bool compare_key_value(StringKey &key, String *value)
			{
				return key.length == value->length && memcmp(key.c_str, value->c_str, key.length) == 0;
			}

			static StringKey &get_key(String *value)
			{
				return *value;
			}

			static String *get_value_next(String *value)
			{
				return value->next;
			}

			static void set_value_next(String *value, String *next)
			{
				value->next = next;
			}

			static size_t hash_key(StringKey &key)
			{
				size_t hash = 0;
				const char_t *start = key.c_str;
				const char_t *stop = start + key.length;
				
				for(const char_t *c = start; c < stop; c++)
				{
					hash += *c;
					hash += hash << 10;
					hash ^= hash >> 6;
				}
				
				hash += hash << 3;
				hash ^= hash >> 11;
				hash += hash << 15;

				return hash;
			}

			static bool valid_key(StringKey &key)
			{
				return true;
			}

			static bool create_value()
			{
				return true;
			}

			static String *create_value(StringKey &key, MemoryPool &memory_pool)
			{
				size_t length = key.length;
				String *result = new (memory_pool) String;

				result->next = 0;
				result->length = key.length;
				
				char_t *result_str = new (memory_pool) char_t[length + 1];
				memcpy(result_str, key.c_str, length);
				result_str[length] = 0;
				
				result->c_str = result_str;

				return result;
			}
	};

	typedef HashTable<StringKey &, String *, StringPoolFunctions> StringPoolHashTable;

	class StringPool:
		public StringPoolHashTable
	{
		public:
			StringPool(MemoryPool &memory_pool) : StringPoolHashTable(memory_pool, 8) {}

			String *get(const char *string)
			{
				StringKey key;

				key.c_str = (const char_t *)string;
				key.length = strlen(string);

				return StringPoolHashTable::get(key);
			}

			String *get(const char_t *string, size_t length)
			{
				StringKey key;

				key.c_str = string;
				key.length = length;

				return StringPoolHashTable::get(key);
			}
			
			String *get(Range *range);

			String *get(const char_t *start, const char_t *stop)
			{
				StringKey key;

				key.c_str = start;
				key.length = stop - key.c_str;

				return StringPoolHashTable::get(key);
			}
	};
};
