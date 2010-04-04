#include "compiler.hpp"

namespace Legion
{
	String *StringPool::get(Range *range)
	{
		StringKey key;

		key.c_str = range->start;
		key.length = range->stop - key.c_str;

		return StringPoolHashTable::get(key);
	};
};