#include "lexer.hpp"
#include "../string_pool.hpp"

namespace Legion
{
	std::string Lexme::names[Types] = {
		"none",	
		"identifier",
		"integer",
		"octal",
		"hex",
		"real",
		"point",
		"end"
	};
	
	bool Lexer::jump_table_ready = 0;
	void(Lexer::*Lexer::jump_table[256])();

	void Lexer::white()
	{
		input++;
		
		while(input.in(1, 9) || input.in(11, 12) || input.in(14, 32))
			input++;

		step();
	}

	void Lexer::unknown()
	{
		input++;
		
		while(jump_table[input] == &Lexer::unknown)
			input++;

		lexme.stop = &input;
		lexme.type = Lexme::None;

		if (lexme.length() == 1)
			lexme.report("Invalid character '" + lexme.string() + "'");
		else
			lexme.report("Invalid characters '" + lexme.string() + "'");

		step();
	}

	void Lexer::null()
	{
		if((size_t)(&input - input_str) >= length)
		{
			lexme.stop = &input;
			lexme.type = Lexme::End;
		}
		else
		{
			unknown();
		}
	}

	void Lexer::ident()
	{
		input++;
		
		while(input.in('a', 'z') || input.in('A', 'Z') || input.in('0', '9') || input == '_')
			input++;

		lexme.stop = &input;
		lexme.type = Lexme::Ident;
		lexme.value = string_pool->get(&lexme);
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

		// Identifiers
		forchar(c, 'A', 'Z')
			jump_table[c] = &Lexer::ident;

		// Numbers
		jump_table['0'] = &Lexer::zero;
		jump_table['.'] = &Lexer::real;
		jump_table['$'] = &Lexer::hex;

		forchar(c, '1', '9')
			jump_table[c] = &Lexer::number;

		// Whitespace
		forchar(c, 1, 9)
			jump_table[c] = &Lexer::white;

		forchar(c, 14, 32)
			jump_table[c] = &Lexer::white;

		jump_table[11] = &Lexer::white;
		jump_table[12] = &Lexer::white;
		
		jump_table_ready = true;
	}

	Lexer::Lexer()
	{
		setup_jump_table();
	}
	
	void Lexer::setup(StringPool *string_pool)
	{
		this->string_pool = string_pool;
	}
	
	void Lexer::load(char_t *input, unsigned int length)
	{
		this->input_str = input;
		this->input.set(input);
		this->length = length;
		
		lexme.start = input;
		lexme.line_start = input;
		lexme.line = 0;
		
		step();
	}
	
	void Lexer::step()
	{
		lexme.start = &input;
		lexme.error = false;

		(this->*jump_table[input])();

		#ifdef DEBUG
			assert(lexme.stop > lexme.start);
		#endif
	}
};
