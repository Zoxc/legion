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

		std::string string();
	};
	
	struct TypePointerNode:
		public TypeNode
	{
		TypeNode *base;

		std::string string();
	};
	
	struct TypeArrayNode:
		public TypeNode
	{
		TypeNode *base;
		ExpressionNode *size;

		std::string string();
	};

	struct TypeBaseNode:
		public TypeNode
	{
		String *type;
		
		std::string string();
	};
	
	class Type
	{
	};
};
