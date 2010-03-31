#include "printer.hpp"
#include "../string_pool.hpp"
#include "expressions.hpp"

namespace Legion
{
	template<class T> std::string Printer::join(T *list, std::string seperator)
	{
		std::string result;

		for(typename T::Iterator i = list->begin(); i; i++)
		{
			result += print_node(*i);

			if((*i)->next)
				result += seperator;
		}

		return result;
	}

	template<class T> std::string Printer::join(T *list, std::string pre, std::string post)
	{
		std::string result;

		for(typename T::Iterator i = list->begin(); i; i++)
			result += pre + print_node(*i) + post;

		return result;
	}

	std::string Printer::get_indent(size_t indent)
	{
		std::string result;

		for(size_t i = 0; i < indent; i++)
			result += "\t";

		return result;
	}

	std::string Printer::print_string(String *string)
	{
		if(string)
			return string->string();
		else
			return "<null_string>";
	}

	std::string Printer::wrap(Node *node, std::string result)
	{
		return result;
	}

	std::string Printer::print_node(Node *node)
	{
		return wrap(node, this->node(node, 0));
	}

	std::string Printer::print_node(Node *node, size_t indent)
	{
		return wrap(node, this->node(node, indent));
	}

	std::string Printer::print(NamespaceList *list)
	{
		std::string result;
		Node::Type prev = Node::NONE;

		for(NamespaceList::Iterator i = list->begin(); i; i++)
		{
			Node::Type type = (*i)->get_type();

			if(prev != Node::NONE && !(prev == type && (type == Node::TYPEDEF_NODE || type == Node::GLOBAL_NODE)))
				result += "\n";

			prev = type;

			result += print_node(*i);

			if((*i)->use_semi())
				result += ";";

			result += "\n";
		}

		return result;
	}

	std::string Printer::node(Node *node, size_t indent)
	{
		if(!node)
			return "<null_node>";

		switch(node->get_type())
		{
			/*
			 * Globals
			 */

			case Node::PAIR_NODE:
			{
				PairNode *target = (PairNode *)node;

				return print_node(target->type) + " " + print_string(target->name);
			}

			case Node::FIELD_NODE:
			{
				FieldNode *target = (FieldNode *)node;

				return print_node(&target->pair);
			}

			case Node::STRUCT_NODE:
			{
				StructNode *target = (StructNode *)node;

				return "struct " + print_string(target->symbol->name) + "\n{\n" + join(&target->fields, "\t", ";\n") + "}";
			}

			case Node::TYPEDEF_NODE:
			{
				TypedefNode *target = (TypedefNode *)node;

				return "typedef " + print_node(target->pair);
			}

			case Node::GLOBAL_NODE:
			{
				GlobalNode *target = (GlobalNode *)node;

				std::string result = print_node(target->pair);

				if(target->is_const)
					result = "const " + result;

				if(target->is_static)
					result = "static " + result;

				if(target->has_value)
					result += " = " + print_node(target->value);

				return result;
			}

			case Node::PARAM_NODE:
			{
				ParamNode *target = (ParamNode *)node;

				return print_node(&target->pair);
			}

			case Node::FUNC_HEAD_NODE:
			{
				FuncHeadNode *target = (FuncHeadNode *)node;

				std::string result = print_node(target->pair) + "(";

				if(target->is_native)
					result = "native " + result;

				if(target->is_static)
					result = "static " + result;

				result += join(&target->params, ", ") + ")";

				return result;
			}

			case Node::PROTOTYPE_NODE:
			{
				PrototypeNode *target = (PrototypeNode *)node;

				return print_node(target->head);
			}

			case Node::FUNC_NODE:
			{
				FuncNode *target = (FuncNode *)node;

				return print_node(target->head) + "\n" + print_node(target->body);
			}

			/*
			 * Statements
			 */

			case Node::BLOCK_NODE:
			{
				Block *target = (Block *)node;

				std::string indentation = get_indent(indent);

				std::string result = indentation + "{\n";

				Node::Type prev = Node::NONE;

				for(StatementList::Iterator i = target->statements.begin(); i; i++)
				{
					Node::Type type = (*i)->get_type();

					if(prev != Node::NONE && !(prev == type && (*i)->use_semi()))
						result += "\n";

					prev = type;

					result += indentation + "\t" + print_node(*i, indent + 1);

					if((*i)->use_semi())
						result += ";";

					result += "\n";
				}

				return result + indentation + "}";
			}

			case Node::IF_NODE:
			{
				IfNode *target = (IfNode *)node;

				std::string result = "if(" + print_node(target->condition) + ")\n" + print_node(target->do_true, indent);

				if(target->do_false)
					result += "\n" + get_indent(indent) + "else\n" + print_node(target->do_false, indent);

				return result;
			}

			case Node::WHILE_NODE:
			{
				WhileNode *target = (WhileNode *)node;

				return "while(" + print_node(target->condition) + ")\n" + print_node(target->body, indent);
			}

			case Node::DO_NODE:
			{
				DoNode *target = (DoNode *)node;

				return "do\n" + print_node(target->body, indent) + "while(" + print_node(target->condition) + ")";
			}

			case Node::RETURN_NODE:
			{
				ReturnNode *target = (ReturnNode *)node;

				if(target->has_value)
					return "return " + print_node(target->value);
				else
					return "return";
			}

			case Node::BREAK_NODE:
				return "break";

			case Node::CONTINUE_NODE:
				return "continue";

			case Node::LOCAL_NODE:
			{
				LocalNode *target = (LocalNode *)node;

				std::string result = print_node(target->type) + " " + print_string(target->name);

				if(target->is_const)
					result = "const " + result;

				if(target->has_value)
					result += " = " + print_node(target->value);

				return result;
			}

			/*
			 * Expressions
			 */

			case Node::IDENT_NODE:
			{
				IdentNode *target = (IdentNode *)node;

				return print_string(target->ident);
			}

			case Node::BINARY_OP_NODE:
			{
				BinaryOpNode *target = (BinaryOpNode *)node;

				return print_node(target->left) + " " + Lexeme::names[target->op] + " " + print_node(target->right);
			}

			case Node::UNARY_OP_NODE:
			{
				UnaryOpNode *target = (UnaryOpNode *)node;

				return Lexeme::names[target->op] + print_node(target->value);
			}

			case Node::ARRAY_SUBSCRIPT_NODE:
			{
				ArraySubscriptNode *target = (ArraySubscriptNode *)node;

				return "[" + print_node(target->index) + "]";
			}

			case Node::ARRAY_DEF_NODE:
			{
				ArrayDefNode *target = (ArrayDefNode *)node;

				std::string result;

				for(ExpressionList::Iterator i = target->sizes.begin(); i; i++)
					result += "[" + print_node(*i) + "]";

				return result;
			}

			case Node::MEMBER_REF_NODE:
			{
				MemberRefNode *target = (MemberRefNode *)node;

				if(target->by_ptr)
					return "->" + print_string(target->name);
				else
					return "." + print_string(target->name);
			}

			case Node::FACTOR_CHAIN_NODE:
			{
				FactorChainNode *target = (FactorChainNode *)node;

				return print_node(target->factor) + join(&target->chain, "");
			}

			case Node::INT_NODE:
			{
				IntegerNode *target = (IntegerNode *)node;

				std::stringstream out;
				out << target->value;
				return out.str();
			}

			case Node::STRING_NODE:
			{
				StringNode *target = (StringNode *)node;

				return "\"" + print_string(target->value) + "\"";
			}

			case Node::FIXED_NODE:
			{
				FixedNode *target = (FixedNode *)node;

				std::stringstream out;
				out << target->value;
				return out.str();
			}

			case Node::BOOL_NODE:
			{
				BooleanNode *target = (BooleanNode *)node;

				if(target->value)
					return "true";
				else
					return "false";
			}

			case Node::NULL_NODE:
				return "null";

			case Node::CALL_NODE:
			{
				CallNode *target = (CallNode *)node;

				return print_node(target->ident) + "(" + join(&target->arguments, ", ") + ")";
			}

			/*
			 * Types
			 */

			case Node::TYPE_POINTER_NODE:
			{
				TypePointerNode *target = (TypePointerNode *)node;

				return print_node(target->base) + "*";
			}

			case Node::TYPE_ARRAY_NODE:
			{
				TypeArrayNode *target = (TypeArrayNode *)node;

				return print_node(target->base) + "[" + print_node(target->size) + "]";
			}

			case Node::TYPE_BASE_NODE:
			{
				TypeBaseNode *target = (TypeBaseNode *)node;

				return print_string(target->type);
			}

			default:
				return "<unknown>";
		}
	}

	#ifdef AST_DEBUG
		std::string DebugPrinter::wrap(Node *node, std::string result)
		{
			const std::type_info &info = typeid(*node);
	 
			std::string name = info.name();
	 
			return "(" + name.substr(name.find("::") + 2) + ": " +  result + ")";
		}	
	#endif
};
