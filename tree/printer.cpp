#include "printer.hpp"
#include "expressions.hpp"

namespace Legion
{
	std::string pretty_print(NamespaceList *list)
	{
		std::string result;
		Node::Type prev = Node::NONE;

		for(NamespaceList::Iterator i = list->begin(); i; i++)
		{
			Node::Type type = (*i)->get_type();

			if(prev != Node::NONE && !(prev == type && (type == Node::TYPEDEF_NODE || type == Node::GLOBAL_NODE)))
				result += "\n";

			prev = type;

			result += (*i)->string();

			if((*i)->use_semi())
				result += ";";

			result += "\n";
		}

		return result;
	}

	std::string Node::string()
	{
		return "";
	}

	std::string Node::wrap(std::string string)
	{
		/*const std::type_info &info = typeid(*this);

		std::string name = info.name();

		return "(" + name.substr(name.find("::") + 2) + ": " +  string + ")";*/

		return string;
	}

	/*
	 * Globals
	 */

	std::string PairNode::string()
	{
		return wrap(type->string() + " " + name->string());
	}

	std::string FieldNode::string()
	{
		return wrap(pair.string());
	}

	std::string StructNode::string()
	{
		return wrap("struct " + symbol->name->string() + "\n{\n" + fields.join("    ", ";\n") + "}");
	}

	std::string TypedefNode::string()
	{
		return wrap("typedef " + pair->string());
	}

	std::string GlobalNode::string()
	{
		std::string result = pair->string();

		if(is_const)
			result = "const " + result;

		if(is_static)
			result = "static " + result;

		if(value)
			result += " = " + value->string();

		return wrap(result);
	}

	std::string ParamNode::string()
	{
		return wrap(pair.string());
	}

	std::string FuncHeadNode::string()
	{
		std::string result = pair->string() + "(";

		if(is_native)
			result = "native " + result;

		if(is_static)
			result = "static " + result;

		result += params.join(", ") + ")";

		return wrap(result);
	}

	std::string PrototypeNode::string()
	{
		return wrap(head->string());
	}

	std::string FuncNode::string()
	{
		return wrap(head->string() + "\n" + body->indent_string(0));
	}

	/*
	 * Statements
	 */

	std::string Block::indent_string(size_t indent)
	{
		std::string indentation;

		for(size_t i = 0; i < indent; i++)
			indentation += "\t";

		std::string result = indentation + "{\n";

		Node::Type prev = Node::NONE;

		for(StatementList::Iterator i = statements.begin(); i; i++)
		{
			Node::Type type = (*i)->get_type();

			if(prev != Node::NONE && !(prev == type && ((*i)->use_semi() || type == Node::LOCAL_NODE)))
				result += "\n";

			prev = type;

			result += indentation + "\t" + (*i)->indent_string(indent + 1);

			if((*i)->use_semi())
				result += ";";

			result += "\n";
		}

		return wrap(result + indentation + "}");
	}

	std::string IfNode::indent_string(size_t indent)
	{
		std::string result = "if(" + condition->string() + ")\n" + do_true->indent_string(indent);

		if(do_false)
			result += "else\n" + do_false->indent_string(indent);

		return wrap(result);
	}

	std::string WhileNode::indent_string(size_t indent)
	{
		return wrap("while(" + condition->string() + ")\n" + body->indent_string(indent));
	}

	std::string DoNode::indent_string(size_t indent)
	{
		return wrap("do\n" + body->indent_string(indent) + "while(" + condition->string() + ")\n");
	}

	std::string ReturnNode::string()
	{
		return wrap("return " + value->string());
	}

	std::string BreakNode::string()
	{
		return wrap("break");
	}

	std::string ContinueNode::string()
	{
		return wrap("continue");
	}

	std::string LocalNode::string()
	{
		std::string result = type->string() + " " + name->string();

		if(is_const)
			result = "const " + result;

		if(value)
			result += " = " + value->string();

		return wrap(result);
	}

	/*
	 * Expressions
	 */

	std::string IdentNode::string()
	{
		return wrap(ident->string());
	}

	std::string BinaryOpNode::string()
	{
		return wrap(left->string() + " " + Lexeme::names[op] + " " + right->string());
	}

	std::string UnaryOpNode::string()
	{
		return wrap(Lexeme::names[op] + value->string());
	}

	std::string ArraySubscriptNode::string()
	{
		return wrap("[" + index->string() + "]");
	}

	std::string ArrayDefNode::string()
	{
		std::string result;

		for(ExpressionList::Iterator i = sizes.begin(); i; i++)
			result += "[" + (*i)->string() + "]";

		return wrap(result);
	}

	std::string MemberRefNode::string()
	{
		if(by_ptr)
			return wrap("->" + name->string());
		else
			return wrap("." + name->string());
	}

	std::string FactorChainNode::string()
	{
		return wrap(factor->string() + chain.join(""));
	}

	std::string IntegerNode::string()
	{
		std::stringstream out;
		out << value;
		return wrap(out.str());
	}

	std::string StringNode::string()
	{
		return wrap("\"" + value->string() + "\"");
	}

	std::string FixedNode::string()
	{
		std::stringstream out;
		out << value;
		return wrap(out.str());
	}

	std::string BooleanNode::string()
	{
		if(value)
			return wrap("true");
		else
			return wrap("false");
	}

	std::string NullNode::string()
	{
		return wrap("null");
	}

	std::string CallNode::string()
	{
		return wrap(ident->string() + "(" + arguments.join(", ") + ")");
	}

	/*
	 * Types
	 */

	std::string TypeNode::string()
	{
		return wrap(base->string());
	}

	std::string TypePointerNode::string()
	{
		return wrap(base->string() + " *");
	}

	std::string TypeArrayNode::string()
	{
		return wrap(base->string() + "[" + size->string() + "]");
	}

	std::string TypeBaseNode::string()
	{
		return wrap(type->string());
	}
};
