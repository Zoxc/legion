#pragma once
#include "../common.hpp"

namespace Legion
{
	class String;
	class Type;
	
	struct TypeNode
	{
		TypeNode() : base(0) {}
		
		TypeNode *base;
	};
	
	struct TypePtrNode:
		public TypeNode
	{
		TypeNode *base;
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
