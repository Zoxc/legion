#include "lexer.hpp"

namespace Legion
{
	std::string Range::string()
	{
		std::string result((char *)start, length());

		return result;
	}

	unsigned int Range::length()
	{
		return stop - start;
	}

	void Range::report(const std::string& error)
	{
		std::cout << "Line " << line + 1 << ": " << error << std::endl;
	}
	
	std::string Lexme::names[Types] = {
		"none",	
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
		while(input.in(1, 9) || input.in(11, 12) || input.in(14, 32))
			input++;

		step();
	}

	void Lexer::unknown()
	{
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
		lexme.stop = &input;
		lexme.type = Lexme::End;
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
	
	void Lexer::setup(char_t *input, unsigned int length)
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
	}
};
