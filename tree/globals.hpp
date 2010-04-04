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

		Node::NodeType node_type()
		{
			return PAIR_NODE;
		}

		Type *get_type(Document &document, SymbolList &stack)
		{
			return type->get_type(document, stack);
		}
	};

	
	struct FieldNode:
		public ListNode
	{
		PairNode pair;

		Node::NodeType node_type()
		{
			return FIELD_NODE;
		}
	};
	
	struct StructNode:
		public NamespaceNode
	{
		StructNode() : type(0) {}

		TypeSymbol *symbol;
		NodeList<FieldNode> fields;
		CompositeType *type;

		Node::NodeType node_type()
		{
			return STRUCT_NODE;
		}

		Type *get_type(Document &document, SymbolList &stack);
	};
	
	struct TypedefNode:
		public NamespaceNode
	{
		TypedefNode() : type(0) {}

		PairNode *pair;
		TypeSymbol *symbol;
		TypedefType *type;

		Node::NodeType node_type()
		{
			return TYPEDEF_NODE;
		}

		Type *get_type(Document &document, SymbolList &stack);
	};
	
	struct GlobalNode:
		public NamespaceNode
	{
		GlobalNode() : symbol(0), type(0) {}
		
		PairNode *pair;
		bool is_const;
		bool is_static;
		VarSymbol *symbol;
		ExpressionNode *value;
		bool has_value;
		Type *type;

		Node::NodeType node_type()
		{
			return GLOBAL_NODE;
		}

		Type *get_type(Document &document, SymbolList &stack);
	};
	
	struct ParamNode:
		public ListNode
	{
		PairNode pair;

		Node::NodeType node_type()
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

		Node::NodeType node_type()
		{
			return FUNC_HEAD_NODE;
		}
	};
	
	struct PrototypeNode:
		public NamespaceNode
	{
		FuncHeadNode *head;

		Node::NodeType node_type()
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

		Node::NodeType node_type()
		{
			return FUNC_NODE;
		}

		Type *get_type(Document &document, SymbolList &stack)
		{
			body->get_type(document, stack);

			return 0;
		}
	};
};
