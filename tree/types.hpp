#pragma once
#include "../common.hpp"
#include "node.hpp"

namespace Legion
{
	class String;
	class Type;

	struct ExpressionNode;
	
	struct TypeNode:
		public Node
	{
		TypeNode() : base(0) {}
		
		TypeNode *base;
	};
	
	struct TypePointerNode:
		public TypeNode
	{
		TypeNode *base;

		Type get_type()
		{
			return Node::TYPE_POINTER_NODE;
		}
	};
	
	struct TypeArrayNode:
		public TypeNode
	{
		TypeNode *base;
		ExpressionNode *size;

		Type get_type()
		{
			return Node::TYPE_ARRAY_NODE;
		}
	};

	struct TypeBaseNode:
		public TypeNode
	{
		String *type;
		
		Type get_type()
		{
			return Node::TYPE_BASE_NODE;
		}
	};
	
	class Type
	{
	};
};
