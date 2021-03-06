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

	typedef List<NamespaceNode> NamespaceList;

	struct IncludeNode:
		public ListNode
	{
		IncludeNode(Document &document, String *filename);
		
		Document &document;
		String *filename;
		Range range;
		bool found;

		Node::NodeType node_type()
		{
			return PAIR_NODE;
		}

		Type *validate(ValidationArgs &args)
		{
			return 0;
		}

		void report();
	};

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

		Type *validate(ValidationArgs &args)
		{
			return type->validate(args);
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
		StructNode() : has_type(false) {}

		TypeSymbol *symbol;
		List<FieldNode> fields;
		bool has_type;
		CompositeType *type;

		Node::NodeType node_type()
		{
			return STRUCT_NODE;
		}

		Type *validate(ValidationArgs &args);
	};
	
	struct TypedefNode:
		public NamespaceNode
	{
		TypedefNode() : has_type(false) {}

		PairNode *pair;
		TypeSymbol *symbol;
		bool has_type;
		TypedefType *type;

		Node::NodeType node_type()
		{
			return TYPEDEF_NODE;
		}

		Type *validate(ValidationArgs &args);
	};
	
	struct GlobalNode:
		public NamespaceNode
	{
		GlobalNode() : symbol(0), has_type(false) {}
		
		PairNode *pair;
		bool is_const;
		bool is_static;
		VarSymbol *symbol;
		ExpressionNode *value;
		bool has_value;
		bool has_type;
		Type *type;

		Node::NodeType node_type()
		{
			return GLOBAL_NODE;
		}

		Type *validate(ValidationArgs &args);
		Type *get_type(ValidationArgs &args);
	};
	
	struct ParamNode:
		public ListNode
	{
		ParamNode() : has_type(false) {}

		PairNode pair;
		bool has_type;
		Type *type;

		Node::NodeType node_type()
		{
			return PARAM_NODE;
		}

		Type *validate(ValidationArgs &args);
	};
	
	struct FuncHeadNode:
		public Node
	{
		FuncHeadNode() : symbol(0), has_type(false) {}
		
		PairNode *pair;
		bool is_native;
		bool is_static;
		Symbol *symbol;
		List<ParamNode> params;
		bool has_type;
		FunctionType *type;

		Node::NodeType node_type()
		{
			return FUNC_HEAD_NODE;
		}

		Type *validate(ValidationArgs &args);
	};
	
	struct PrototypeNode:
		public NamespaceNode
	{
		FuncHeadNode *head;

		Node::NodeType node_type()
		{
			return PROTOTYPE_NODE;
		}

		Type *validate(ValidationArgs &args);

		Type *get_type(ValidationArgs &args)
		{
			return head->validate(args);
		}
	};
	
	struct Block;
	
	struct FunctionNode:
		public NamespaceNode
	{
		FuncHeadNode *head;
		Scope *scope;
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
			return FUNCTION_NODE;
		}

		Type *validate(ValidationArgs &args)
		{
			FunctionType *type = (FunctionType *)head->validate(args);
			args.func = this;
			body->validate(args);

			return type;
		}
		
		Type *get_type(ValidationArgs &args)
		{
			return head->validate(args);
		}
	};
};
