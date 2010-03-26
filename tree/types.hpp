#pragma once
#include "../common.hpp"

namespace Legion
{
	class String;
	class Type;

	struct ExpressionNode;
	
	struct TypeNode
	{
		TypeNode() : base(0) {}
		
		TypeNode *base;
	};
	
	struct TypePointerNode:
		public TypeNode
	{
		TypeNode *base;
	};
	
	struct TypeArrayNode:
		public TypeNode
	{
		TypeNode *base;
		ExpressionNode *size;
	};

	struct TypeBaseNode:
		public TypeNode
	{
		String *type;
	};
	
	class Type
	{
	};
	
};
