#include "parser.hpp"
#include "../tree/scope.hpp"
#include "../tree/types.hpp"
#include "../tree/symbols.hpp"
#include "../tree/types.hpp"

namespace Legion
{
	Parser::Parser(StringPool *string_pool, MemoryPool *memory_pool, Document *document, Scope *scope) : lexer(string_pool, memory_pool, document), scope(scope), document(document), memory_pool(memory_pool)
	{
	}

	Parser::~Parser()
	{
	}
	
	Scope *Parser::push_scope(Scope::Type type)
	{
		return scope = new Scope(scope, type, memory_pool);
	}
	
	void Parser::pop_scope()
	{
		scope = scope->parent;
	}
	
	void Parser::expected(Lexeme::Type what, bool skip)
	{
		lexer.lexeme.report(document, "Expected " + Lexeme::describe_type(what) + ", but found " + lexer.lexeme.describe());
		
		if(skip)
			step();
	}
	
	void Parser::unexpected(bool skip)
	{
		lexer.lexeme.report(document, "Unexpected " + lexer.lexeme.describe());

		if(skip)
			step();
	}
	
	TypeNode *Parser::parse_type()
	{
		TypeNode *type = new (memory_pool) TypeNode;

		if(expect(Lexeme::IDENT))
		{
			type->name = lexer.lexeme.value;
			type->range = new Range(lexer.lexeme);
		
			step();
		}

		while(true)
		{
			switch(lexeme())
			{
				case Lexeme::MUL:
				{
					type->modifiers.add<TypePointerNode>(memory_pool);
					step();
				}
				break;

				case Lexeme::SQR_BRACET_OPEN:
				{
					TypeArrayNode *node = type->modifiers.add<TypeArrayNode>(memory_pool);
					step();
			 
					node->size = parse_expression();
			 
					match(Lexeme::SQR_BRACET_CLOSE);
				}
				break;

				default:
					return type;
			}

		}
	}
};