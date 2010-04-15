#pragma once
#include "../common.hpp"
#include "../range.hpp"
#include "../list.hpp"
#include "symbols.hpp"

namespace Legion
{
	class Document;
	class Types;
	class Type;
	class Scope;
	
	struct ExpressionNode;
	struct StatementNode;

	struct ValidationArgs
	{
		ValidationArgs(Document &document, Types &types, Scope *scope, MemoryPool &memory_pool) : document(document), types(types), scope(scope), memory_pool(memory_pool) {}

		Document &document;
		Types &types;
		Scope *scope;
		MemoryPool &memory_pool;
		SymbolList stack;
		Type *parent_type;
		StatementNode *parent_node;
	};

	struct Node
	{
		enum NodeType
		{
			NONE,

			PAIR_NODE,
			FIELD_NODE,
			STRUCT_NODE,
			TYPEDEF_NODE,
			GLOBAL_NODE,
			PARAM_NODE,
			FUNC_HEAD_NODE,
			PROTOTYPE_NODE,
			FUNC_NODE,

			BLOCK_NODE,
			IF_NODE,
			WHILE_NODE,
			DO_NODE,
			RETURN_NODE,
			BREAK_NODE,
			CONTINUE_NODE,
			LOCAL_NODE,

			IDENT_NODE,
			BINARY_OP_NODE,
			ASSIGN_NODE,
			UNARY_OP_NODE,
			ARRAY_SUBSCRIPT_NODE,
			ARRAY_DEF_NODE,
			MEMBER_REF_NODE,
			FACTOR_CHAIN_NODE,
			INT_NODE,
			STRING_NODE,
			FIXED_NODE,
			BOOL_NODE,
			NULL_NODE,
			CALL_NODE,
			GROUPED_EXPRESSION_NODE,

			TYPE_NODE,
			TYPE_POINTER_NODE,
			TYPE_ARRAY_NODE,
			TYPE_NATIVE_NODE
		};

		virtual NodeType node_type()
		{
			return NONE;
		}

		virtual bool use_semi()
		{
			return true;
		}

		virtual bool find_declarations(Document *document)
		{
			return false;
		}

		virtual Type *validate(ValidationArgs &args)
		{
			return 0;
		}
		
		virtual Type *get_type(ValidationArgs &args)
		{
			return validate(args);
		}
	};
	
	struct ListNode:
		public Node
	{
		ListNode *next;
	};
};
