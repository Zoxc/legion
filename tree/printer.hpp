#pragma once
#include "../common.hpp"
#include "globals.hpp"

namespace Legion
{
	class String;

	class Printer
	{
		protected:
			virtual std::string node(Node *node, size_t indent);
			virtual std::string wrap(Node *node, std::string result);
			template<class T> std::string join(T *list, std::string seperator);
			template<class T> std::string join(T *list, std::string pre, std::string post);
			std::string print_node(Node *node, size_t indent);
			std::string print_string(String *string);
		public:
			std::string print_node(Node *node);
			virtual std::string print(NamespaceList *list);
	};

	#ifdef AST_DEBUG
		class DebugPrinter:
			public Printer
		{
			protected:
				virtual std::string wrap(Node *node, std::string result);
		};
	#endif
};
