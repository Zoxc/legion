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
			node->range.capture(lexer.lexeme);
			node->name = lexer.lexeme.value;
			step();
			
			return true;
		}
		
		return false;
	}
	
	void Parser::parse_include(NamespaceList *list)
	{
		step();
		
		if(expect(Lexeme::STRING))
		{
			IncludeNode *node = new (memory_pool) IncludeNode(document, document.normalize(lexer.lexeme.value));

			node->range.capture(lexer.lexeme);
			
			document.includes.append(node);

			step();
		}
		
		matches(Lexeme::SEMICOLON);
	}
	
	void Parser::parse_struct(NamespaceList *list)
	{
		step();
		
		StructNode *node = list->add<StructNode>(memory_pool);
		
		node->symbol = declare<TypeSymbol>(0);
		node->symbol->node = node;
		
		if(match(Lexeme::BRACET_OPEN))
		{
			while(lexeme() == Lexeme::IDENT)
			{
				FieldNode *field = node->fields.add<FieldNode>(memory_pool);
				
				if(parse_pair(&field->pair))
					parse_terminator();
			}
			
			match(Lexeme::BRACET_CLOSE);
		}
		
		parse_terminator();
	}
	
	void Parser::parse_typedef(NamespaceList *list)
	{
		step();
		
		PairNode *pair = new (memory_pool) PairNode;
		
		if(parse_pair(pair))
		{
			TypedefNode *type_def = list->add<TypedefNode>(memory_pool);
		
			type_def->symbol = declare<TypeSymbol>(pair, 0);
			type_def->symbol->node = type_def;
			type_def->pair = pair;
			
			parse_terminator();
		}
	}
	
	void Parser::parse_global(NamespaceList *list, bool is_static, bool is_const, bool is_native, PairNode *pair)
	{
		GlobalNode *global = list->add<GlobalNode>(memory_pool);
		global->pair = pair;
		global->is_const = is_const;
		global->is_static = is_static;
		global->symbol = declare<VarSymbol>(pair, is_static ? &document : 0);
		global->symbol->node = global;
		
		if(is_native)
			document.report(pair->range, "Global can not be natives");
		
		if(is_assign_operator(lexeme()))
		{
			if(lexeme() != Lexeme::ASSIGN)
				document.report(lexer.lexeme.dup(memory_pool), "Unexpected assignment with operator " + lexer.lexeme.describe());

			step();

			global->has_value = true;
			global->value = parse_expression();
		}
		else
			global->has_value = false;
		
		parse_terminator();
	}
	
	void Parser::parse_function(NamespaceList *list, bool is_static, bool is_const, bool is_native, PairNode *pair)
	{
		bool prev;

		FuncHeadNode *head = new (memory_pool) FuncHeadNode;
		head->pair = pair;
		head->is_native = is_native;
		head->is_static = is_static;

		if(is_const)
			document.report(pair->range, "Functions can not be declared constant");

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
			if(is_native)
				document.report(pair->range, "You cannot provide an implementation of a native function");

			FunctionNode *func = list->add<FunctionNode>(memory_pool);

			func->head = head;
			
			head->symbol = declare<FuncSymbol>(pair, is_static ? &document : 0, prev);
			head->symbol->node = func;

			Symbol *symbol = head->symbol;

			while(symbol->next_name != head->symbol)
			{
				symbol = symbol->next_name;

				if(symbol->type != Symbol::PROTOTYPE && symbol->document == head->symbol->document)
				{
					head->symbol->redeclared(document);
					break;
				}
			}

			func->scope = push_scope(Scope::FUNCTION);

			for(List<ParamNode>::Iterator i = head->params.begin(); i; i++)
			{
				VarSymbol *symbol = declare<VarSymbol>(&(*i)->pair, 0);
				symbol->node = *i;
			}
			
			func->body = parse_block<true, false>(Scope::EMPTY);

			pop_scope();
		}
		else
		{
			PrototypeNode *proto = list->add<PrototypeNode>(memory_pool);
			proto->head = head;

			head->symbol = declare<PrototypeSymbol>(pair, is_static ? &document : 0, prev);
			head->symbol->node = proto;

			parse_terminator();
		}
	}
	
	template<bool prev_static, bool prev_const, bool prev_native> void Parser::parse_global_ident(NamespaceList *list)
	{
		bool is_static = prev_static;
		bool is_const = prev_const;
		bool is_native = prev_native;

		lexer.identify_keywords();
		
		if(!prev_static && lexeme() == Lexeme::KW_STATIC)
		{
			is_static = true;
			step();
			lexer.identify_keywords();
		}
		
		if(!prev_const && lexeme() == Lexeme::KW_CONST)
		{
			is_const = true;
			step();
			lexer.identify_keywords();
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
				parse_function(list, is_static, is_const, is_native, pair);
				break;
				
			case Lexeme::SEMICOLON:
			case Lexeme::ASSIGN:
			default:
				parse_global(list, is_static, is_const, is_native, pair);
				break;
		}	
	}	
	
	void Parser::parse(NamespaceList *list)
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
