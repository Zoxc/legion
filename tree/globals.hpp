#pragma once
#include "../common.hpp"
#include "../range.hpp"
#include "node.hpp"
#include "types.hpp"
#include "statements.hpp"
#include "expressions.hpp"
#include "symbols.hpp"

namespace Legion
{
	class String;
	class Type;
	
	struct TypeSymbol;
	struct VarSymbol;
	struct FuncSymbol;
	struct ExpressionNode;
	
	struct NamespaceNode:
		public ListNode
	{
	};

	typedef NodeList<NamespaceNode> NamespaceList;

	struct PairNode:
		public Node
	{
		PairNode() : type(0), name(0) {}
		
		TypeNode *type;
		String *name;
		Range range;

		Node::Type get_type()
		{
			return PAIR_NODE;
		}
	};

	
	struct FieldNode:
		public ListNode
	{
		PairNode pair;

		Node::Type get_type()
		{
			return FIELD_NODE;
		}
	};
	
	struct StructNode:
		public NamespaceNode
	{
		TypeSymbol *symbol;
		NodeList<FieldNode> fields;

		Node::Type get_type()
		{
			return STRUCT_NODE;
		}
	};
	
	struct TypedefNode:
		public NamespaceNode
	{
		TypedefNode() : pair(0), symbol(0) {}
		
		PairNode *pair;
		TypeSymbol *symbol;

		Node::Type get_type()
		{
			return TYPEDEF_NODE;
		}
	};
	
	struct GlobalNode:
		public NamespaceNode
	{
		GlobalNode() : symbol(0) {}
		
		PairNode *pair;
		bool is_const;
		bool is_static;
		VarSymbol *symbol;
		ExpressionNode *value;
		bool has_value;

		Node::Type get_type()
		{
			return GLOBAL_NODE;
		}
	};
	
	struct ParamNode:
		public ListNode
	{
		PairNode pair;

		Node::Type get_type()
		{
			return PARAM_NODE;
		}

	};
		
	struct FuncHeadNode:
		public Node
	{
		FuncHeadNode() : symbol(0) {}
		
		PairNode *pair;
		bool is_native;
		bool is_static;
		FuncSymbol *symbol;
		NodeList<ParamNode> params;

		Node::Type get_type()
		{
			return FUNC_HEAD_NODE;
		}
	};
	
	struct PrototypeNode:
		public NamespaceNode
	{
		FuncHeadNode *head;

		Node::Type get_type()
		{
			return PROTOTYPE_NODE;
		}
	};
	
	struct Block;
	
	struct FuncNode:
		public NamespaceNode
	{
		FuncHeadNode *head;
		Block *body;

		bool find_declarations(Document *document)
		{
			if(body)
				body->find_declarations(document);

			return false;
		}

		bool use_semi()
		{
			return false;
		}

		Node::Type get_type()
		{
			return FUNC_NODE;
		}
	};
};
