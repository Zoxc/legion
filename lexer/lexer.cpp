#include "lexer.hpp"
#include "../string_pool.hpp"

namespace Legion
{
	Keywords::Keywords(StringPool *pool)
	{
		for(size_t i = Lexeme::KW_INCLUDE; i < Lexeme::END; i++)
			mapping.insert(std::pair<String *, Lexeme::Type>(pool->get(Lexeme::names[i].c_str()), (Lexeme::Type)i));
	}
	
	bool Lexer::jump_table_ready = 0;
	void(Lexer::*Lexer::jump_table[sizeof(char_t) << 8])();
	
	template<Lexeme::Type type> void Lexer::single()
	{
		input++;
		
		lexeme.stop = &input;
		lexeme.type = type;
	}

	template<Lexeme::Type type, Lexeme::Type assign_type> void Lexer::assign()
	{
		input++;
		
		if(input == '=')
		{
			input++;
			lexeme.stop = &input;
			lexeme.type = assign_type;
		}
		else
		{
			lexeme.stop = &input;
			lexeme.type = type;
		}
	}

	template<Lexeme::Type type, Lexeme::Type assign_type, char_t match, Lexeme::Type match_type, Lexeme::Type match_assign> void Lexer::assign()
	{
		input++;
		
		switch(input)
		{
			case match:
				input++;
				if(input == '=')
				{
					input++;
					lexeme.stop = &input;
					lexeme.type = match_assign;
				}
				else
				{
					lexeme.stop = &input;
					lexeme.type = match_type;
				}
				break;
				
			case '=':
				input++;
				lexeme.stop = &input;
				lexeme.type = assign_type;
				break;
				
			default:
				lexeme.stop = &input;
				lexeme.type = type;
		}
	}

	void Lexer::setup_jump_table()
	{
		if(jump_table_ready)
			return;
		
		#define forchar(name, start, stop) for(int name = start; name <= stop; name++)

		// Unknown
		forchar(c, 0, 255)
			jump_table[c] = &Lexer::unknown;

		// Stop please
		jump_table[0] = &Lexer::null;

		// Identifiers
		forchar(c, 'a', 'z')
			jump_table[c] = &Lexer::ident;

		forchar(c, 'A', 'Z')
			jump_table[c] = &Lexer::ident;

		// String
		jump_table['"'] = &Lexer::string;


		// Numbers
		jump_table['0'] = &Lexer::zero;
		jump_table['.'] = &Lexer::real;

		forchar(c, '1', '9')
			jump_table[c] = &Lexer::number;

		// Whitespace
		forchar(c, 1, 9)
			jump_table[c] = &Lexer::white;

		forchar(c, 14, 32)
			jump_table[c] = &Lexer::white;

		jump_table[11] = &Lexer::white;
		jump_table[12] = &Lexer::white;
		
		// Newlines
		jump_table['\n'] = &Lexer::newline;
		jump_table['\r'] = &Lexer::carrige_return;
		
		// Arithmetic
		jump_table['+'] = &Lexer::assign<Lexeme::ADD, Lexeme::ASSIGN_ADD>;
		jump_table['-'] = &Lexer::sub;
		jump_table['*'] = &Lexer::assign<Lexeme::MUL, Lexeme::ASSIGN_MUL>;
		jump_table['%'] = &Lexer::assign<Lexeme::MOD, Lexeme::ASSIGN_MOD>;
		jump_table['/'] = &Lexer::div;
		
		// Bitwise operators
		jump_table['^'] = &Lexer::assign<Lexeme::BITWISE_XOR, Lexeme::ASSIGN_BITWISE_XOR>;
		jump_table['&'] = &Lexer::assign<Lexeme::BITWISE_AND, Lexeme::ASSIGN_BITWISE_AND, '&', Lexeme::LOGICAL_AND, Lexeme::ASSIGN_LOGICAL_AND>;
		jump_table['|'] = &Lexer::assign<Lexeme::BITWISE_OR, Lexeme::ASSIGN_BITWISE_OR, '|', Lexeme::LOGICAL_OR, Lexeme::ASSIGN_LOGICAL_OR>;
		jump_table['~'] = &Lexer::assign<Lexeme::BITWISE_NOT, Lexeme::ASSIGN_BITWISE_NOT>;
		
		// Logical operators
		jump_table['!'] = &Lexer::assign<Lexeme::LOGICAL_NOT, Lexeme::NOT_EQUAL>;
		
		// Misc
		jump_table['='] = &Lexer::assign<Lexeme::ASSIGN, Lexeme::EQUAL>;
		
		jump_table['<'] = &Lexer::assign<Lexeme::LESS, Lexeme::LESS_OR_EQUAL, '<', Lexeme::LEFT_SHIFT, Lexeme::ASSIGN_LEFT_SHIFT>;
		jump_table['>'] = &Lexer::assign<Lexeme::GREATER, Lexeme::GREATER_OR_EQUAL, '>', Lexeme::RIGHT_SHIFT, Lexeme::ASSIGN_RIGHT_SHIFT>;
		
		jump_table[';'] = &Lexer::single<Lexeme::SEMICOLON>;
		jump_table[','] = &Lexer::single<Lexeme::COMMA>;
		
		// Bracets
		jump_table['{'] = &Lexer::single<Lexeme::BRACET_OPEN>;
		jump_table['}'] = &Lexer::single<Lexeme::BRACET_CLOSE>;
		
		jump_table['('] = &Lexer::single<Lexeme::PARENT_OPEN>;
		jump_table[')'] = &Lexer::single<Lexeme::PARENT_CLOSE>;
		
		jump_table['['] = &Lexer::single<Lexeme::SQR_BRACET_OPEN>;
		jump_table[']'] = &Lexer::single<Lexeme::SQR_BRACET_CLOSE>;
		
		jump_table_ready = true;
	}
	
	void Lexer::report_null()
	{
		const char_t *start = lexeme.start;
		lexeme.start = &input - 1;
		lexeme.stop = &input;
		lexeme.report(&document, "Unexpected null terminator");
		lexeme.start = start;
	}

	Lexer::Lexer(StringPool *string_pool, MemoryPool *memory_pool, Document *document) : string_pool(string_pool), memory_pool(memory_pool), document(*document), lexeme(*this), keywords(string_pool)
	{
		setup_jump_table();
	}
	
	void Lexer::load(const char_t *input, size_t length)
	{
		this->input_str = input;
		this->input.set(input);
		this->length = length;
		
		lexeme.start = input;
		lexeme.line_start = input;
		lexeme.line = 0;
		
		step();
	}
	
	void Lexer::identify_keywords()
	{
		if(lexeme.type != Lexeme::IDENT)
			return;

		std::map<String *, Lexeme::Type>::iterator value = keywords.mapping.find(lexeme.value);
		
		if(value != keywords.mapping.end())
			lexeme.type = value->second;
	}
	
	void Lexer::step()
	{
		lexeme.prev = lexeme.stop;
		restep();
	}

	void Lexer::restep()
	{
		lexeme.start = &input;
		lexeme.error = false;

		(this->*jump_table[input])();

		#ifdef DEBUG
			assert(lexeme.stop >= lexeme.start);
		#endif
	}
};
