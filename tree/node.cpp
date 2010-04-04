#pragma once
#include "../common.hpp"
#include "../range.hpp"
#include "node_list.hpp"
#include "symbols.hpp"

namespace Legion
{
	class Document;
	class Type;

	struct ExpressionNode;

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

		void compitable(Document &document, SymbolList &stack, ExpressionNode *node)
		{
			Type *left = get_type(document, stack);
			Type *right = node->get_type(document, stack);

			if(!left || !right)
				return;

			if(!left->compitable(right))
			{
				node->get_range(
			}
		}

		virtual Legion::Type *get_type(Document &document, SymbolList &stack)
		{
			return 0;
		}
	};
	
	struct ListNode:
		public Node
	{
		ListNode *next;
	};
};
