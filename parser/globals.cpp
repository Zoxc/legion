#include "parser.hpp"
#include "../document.hpp"
#include "../tree/scope.hpp"
#include "../tree/types.hpp"
#include "../tree/symbols.hpp"
#include "../tree/globals.hpp"

namespace Legion
{
	bool Parser::parse_pair(PairNode *node)
	{
		node->type = parse_type();
			
		if(expect(Lexeme::IDENT))
		{
			node->range.capture(&lexer.lexeme);
			node->name = lexer.lexeme.value;
			step();
			
			return true;
		}
		
		return false;
	}
	
	void Parser::parse_include(NodeList *list)
	{
		step();
		
		if(expect(Lexeme::STRING))
		{
			document->includes.push_back(lexer.lexeme.value->string());
			step();
		}
		
		matches(Lexeme::SEMICOLON);
	}
	
	void Parser::parse_struct(NodeList *list)
	{
		step();
		
		StructNode *node = list->add<StructNode>(memory_pool);
		
		node->symbol = declare<TypeSymbol>();
		
		if(match(Lexeme::BRACET_OPEN))
		{
			while(lexeme() == Lexeme::IDENT)
			{
				FieldNode *field = node->fields.add<FieldNode>(memory_pool);
				
				if(parse_pair(&field->pair))
					match(Lexeme::SEMICOLON);
			}
			
			match(Lexeme::BRACET_CLOSE);
		}
		
		match(Lexeme::SEMICOLON);
	}
	
	void Parser::parse_typedef(NodeList *list)
	{
		step();
		
		TypedefNode *type_def = list->add<TypedefNode>(memory_pool);
		
		type_def->symbol = new (memory_pool) TypeSymbol;
		
		if(parse_pair(&type_def->pair))
		{
			scope->declare_symbol(document, type_def->symbol);
			
			match(Lexeme::SEMICOLON);
		}
	}
	
	void Parser::parse_global(NodeList *list, bool is_static, bool is_const, bool is_native, PairNode *pair)
	{
		GlobalNode *global = list->add<GlobalNode>(memory_pool);
		global->pair = pair;
		global->is_const = is_const;
		global->is_static = is_static;
		global->symbol = declare<VarSymbol>(pair);
		
		if(is_native)
			pair->range.report(document, "Global can not be natives");
		
		if(matches(Lexeme::ASSIGN))
			global->value = parse_expression();
		else
			global->value = 0;
		
		match(Lexeme::SEMICOLON);
	}
	
	void Parser::parse_function(NodeList *list, bool is_static, bool is_const, bool is_native, PairNode *pair)
	{
		FuncHeadNode *head = new (memory_pool) FuncHeadNode;
		head->pair = pair;
		head->is_native = is_native;
		head->is_static = is_static;
		head->symbol = declare<FuncSymbol>(pair);
		
		if(is_const)
			pair->range.report(document, "Functions can not be declared constant");

		step();
		
		if(lexeme() != Lexeme::PARENT_CLOSE)
		{
			do
			{
				ParamNode *param = head->params.add<ParamNode>(memory_pool);
				
				parse_pair(&param->pair);
			}
			while(matches(Lexeme::COMMA));
		}		
		match(Lexeme::PARENT_CLOSE);
		
		if(lexeme() == Lexeme::BRACET_OPEN)
		{
			FuncNode *func = list->add<FuncNode>(memory_pool);
			func->head = head;
			func->body = new (memory_pool) Block;

			step();
			
			parse_statements(&func->body->statements);	

			match(Lexeme::BRACET_CLOSE);
		}
		else
		{
			PrototypeNode *proto = list->add<PrototypeNode>(memory_pool);
			proto->head = head;
			
			match(Lexeme::SEMICOLON);
		}
	}
	
	template<bool prev_static, bool prev_const, bool prev_native> void Parser::parse_global_ident(NodeList *list)
	{
		bool is_static = prev_static;
		bool is_const = prev_const;
		bool is_native = prev_native;
		
		if(!prev_static && lexeme() == Lexeme::KW_STATIC)
		{
			is_static = true;
			step();
		}
		
		if(!prev_const && lexeme() == Lexeme::KW_CONST)
		{
			is_const = true;
			step();
		}
		
		if(!prev_native && lexeme() == Lexeme::KW_NATIVE)
		{
			is_native = true;
			step();
		}
		
		PairNode *pair = new (memory_pool) PairNode;
		
		parse_pair(pair);

		switch(lexeme())
		{
			case Lexeme::PARENT_OPEN:
				parse_function(list, is_const, is_static, is_native, pair);
				break;
				
			case Lexeme::SEMICOLON:
			case Lexeme::ASSIGN:
			default:
				parse_global(list, is_const, is_static, is_native, pair);
				break;
		}	
	}	
	
	void Parser::parse(NodeList *list)
	{
		while(true)
		{
			lexer.identify_keywords();
			
			switch(lexeme())
			{
				case Lexeme::KW_INCLUDE:
					parse_include(list);
					break;
					
				case Lexeme::KW_STRUCT:
					parse_struct(list);
					break;
					
				case Lexeme::KW_TYPEDEF:
					parse_typedef(list);
					break;
				
				case Lexeme::IDENT:
					parse_global_ident<false, false, false>(list);
					break;
				
				case Lexeme::KW_STATIC:
					step();
					parse_global_ident<true, false, false>(list);
					break;
				
				case Lexeme::KW_CONST:
					step();
					parse_global_ident<false, true, false>(list);
					break;
				
				case Lexeme::KW_NATIVE:
					step();
					parse_global_ident<false, false, true>(list);
					break;
					
				case Lexeme::END:
					return;
				
				default:
					unexpected();
			}
		}
	}
};
