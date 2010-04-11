#pragma once
#include "common.hpp"
#include "list.hpp"
#include "string_pool.hpp"
#include "tree/scope.hpp"
#include "tree/types.hpp"

namespace Legion
{
	class Document;

	class Compiler
	{
		private:
		public:
			MemoryPool memory_pool;
			StringPool string_pool;
			Scope scope;
			Types types;
			List<Document> documents;

			Compiler();
			~Compiler();
	};
};
