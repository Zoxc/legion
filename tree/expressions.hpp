#pragma once
#include "../common.hpp"
#include "../range.hpp"
#include "node.hpp"
#include "statements.hpp"

namespace Legion
{
	class String;
	class Type;

	struct ExpressionNode:
		public StatementNode
	{
	};

	struct BinaryOpNode:
		public ExpressionNode
	{
		ExpressionNode *left;
		ExpressionNode *right;
		Lexeme::Type op;
	};

	struct UnaryOpNode:
		public ExpressionNode
	{
		Lexeme::Type op;
		ExpressionNode *value;
	};

	struct ArraySubscriptNode:
		public ExpressionNode
	{
		ExpressionNode *index;
	};

	struct ArrayDefNode:
		public ExpressionNode
	{
		NodeList sizes;
	};

	struct MemberRefNode:
		public ExpressionNode
	{
		String *name;
		bool by_ptr;
	};

	struct FactorChainNode:
		public ExpressionNode
	{
		ExpressionNode *factor;
		NodeList chain;
	};

	struct IntegerNode:
		public ExpressionNode
	{
		int value;
	};

	struct StringNode:
		public ExpressionNode
	{
		String *value;
	};

	struct FixedNode:
		public ExpressionNode
	{
		double value;
	};

	struct BooleanNode:
		public ExpressionNode
	{
		bool value;
	};

	struct NullNode:
		public ExpressionNode
	{
	};

	struct IdentNode:
		public ExpressionNode
	{
		String *ident;
	};

	struct CallNode:
		public ExpressionNode
	{
		IdentNode *ident;
		NodeList arguments;
	};

};
