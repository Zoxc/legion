#include "statements.hpp"
#include "expressions.hpp"

namespace Legion
{
	std::string Node::string()
	{
		return "";
	}

	std::string Node::wrap(std::string string)
	{
		const std::type_info &info = typeid(*this);

		std::string name = info.name();

		return "(" + name.substr(name.find("::") + 2) + ": " +  string + ")";
	}

	std::string IfNode::string()
	{
		std::string result = "if(" + condition->string() + ")\n" + do_true->string();

		if(do_false)
			result += "else\n" + do_false->string();

		return wrap(result);
	}

	std::string WhileNode::string()
	{
		return wrap("while(" + condition->string() + ")\n" + body->string());
	}

	std::string DoNode::string()
	{
		return wrap("do\n" + body->string() + "while(" + condition->string() + ")\n");
	}

	std::string ReturnNode::string()
	{
		return wrap("return " + value->string());
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

	std::string TypeArrayNode::string()
	{
		return wrap(base->string() + "[" + size->string() + "]");
	}

	std::string TypeBaseNode::string()
	{
		return wrap(type->string());
	}
};
