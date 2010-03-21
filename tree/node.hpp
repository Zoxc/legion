#pragma once
#include "../common.hpp"

namespace Legion
{
	class String;
	class Type;
	
	class Node
	{
		public:
			enum Type {
				NONE,
				TREE,
				STRUCT,
				FUNCTION,
				VARIABLE
			};

			Type type;
			Node *children;
			Node *next;
	};
	
	template <Node::Type type> class NodeType:
		public Node
	{
		Node::Type node_type()
		{
			return type;
		}
	};

	class StructNode:
		public NodeType<Node::STRUCT>
	{
	};
	
};
