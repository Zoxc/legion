#pragma once
#include "../common.hpp"
#include "node.hpp"

namespace Legion
{
	class String;
	class Type;

	struct ExpressionNode;
	
	struct TypeModifierNode:
		public ListNode
	{
	};

	typedef NodeList<TypeModifierNode> TypeModifierList;

	struct TypeNode:
		public Node
	{
		TypeNode() : name(0) {}

		String *name;
		TypeModifierList modifiers;
		
		Type get_type()
		{
			return Node::TYPE_NODE;
		}
	};
	
	struct TypePointerNode:
		public TypeModifierNode
	{
		Type get_type()
		{
			return Node::TYPE_POINTER_NODE;
		}
	};
	
	struct TypeArrayNode:
		public TypeModifierNode
	{
		ExpressionNode *size;

		Type get_type()
		{
			return Node::TYPE_ARRAY_NODE;
		}
	};
	
	class Type
	{
	};
};
