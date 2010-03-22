#include "parser.hpp"
#include "../tree/scope.hpp"
#include "../tree/types.hpp"
#include "../tree/symbols.hpp"
#include "../tree/node.hpp"

namespace Legion
{
	bool Parser::parse_pair(PairNode *node)
	{
		node->capture(&lexer.lexeme);
		
		node->type = parse_type();
			
		if(expect(Lexeme::IDENT))
		{
			node->capture(&lexer.lexeme);
			node->name = lexer.lexeme.value;
			step();
			
			return true;
		}
		
		node->expand(&lexer.lexeme);
		
		return false;
	}
	
	void Parser::parse_include(NodeList *list)
	{
		step();
		
		if(expect(Lexeme::STRING))
		{
			std::cout << "Found include " << lexer.lexeme.string() << std::endl;
			step();
		}
		
		matches(Lexeme::SEMICOLON);
	}
	
	void Parser::parse_struct(NodeList *list)
	{
		step();
		
		StructNode *node = list->add<StructNode>(memory_pool, &lexer.lexeme);
		
		node->symbol = scope->declare<TypeSymbol>(document, this);
		
		if(match(Lexeme::BRACET_OPEN))
		{
			while(lexeme() == Lexeme::IDENT)
			{
				PairNode *pair = node->fields.add<PairNode>(memory_pool, &lexer.lexeme);
				
				if(parse_pair(pair))
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
		
		if(parse_pair(type_def))
		{
			type_def->symbol = scope->declare<TypeSymbol>(document, type_def->name, type_def);
			
			match(Lexeme::SEMICOLON);
		}
	}
	
	void Parser::parse_global(NodeList *list, bool is_static, bool is_const, bool is_native, PairNode *pair)
	{
		GlobalNode *global = list->add<GlobalNode>(memory_pool, pair);
		global->pair = pair;
		global->is_const = is_const;
		global->is_static = is_static;
		global->symbol = scope->declare<VarSymbol>(document, pair->name, pair);
		
		if(is_native)
			global->report(document, "Global can not be natives");
		
		if(matches(Lexeme::ASSIGN))
		{
		}
		
		match(Lexeme::SEMICOLON);
	}
	
	void Parser::parse_function(NodeList *list, bool is_static, bool is_const, bool is_native, PairNode *pair)
	{
		FuncHeadNode *head = Node::create<FuncHeadNode>(memory_pool, pair);
		head->pair = pair;
		head->is_native = is_native;
		head->is_static = is_static;
		
		if(is_const)
			head->report(document, "Functions can not be declared constant");

		head->symbol = scope->declare<FuncSymbol>(document, pair->name, pair);
		
		step();
		
		if(lexeme() != Lexeme::PARENT_CLOSE)
		{
			do
			{
				PairNode *pair = head->params.add<PairNode>(memory_pool, &lexer.lexeme);
				
				parse_pair(pair);
			}
			while(matches(Lexeme::COMMA));
		}		
		match(Lexeme::PARENT_CLOSE);
		
		if(matches(Lexeme::BRACET_OPEN))
		{
			FuncNode *func = list->add<FuncNode>(memory_pool, head);
			func->head = head;
			
			match(Lexeme::BRACET_CLOSE);
		}
		else
		{
			list->append(head);
			
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
		
		PairNode *pair = Node::create<PairNode>(memory_pool);
		
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
					
				case Lexeme::END:
					return;
					
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
				
				default:
					unexpected();
			}
		}
	}
};
