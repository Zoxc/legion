#include "lexer.hpp"

namespace Legion
{
	void Lexer::zero()
	{
		input++;

		switch(input)
		{
			case 'X':
			case 'x':
				hex();
				break;
		
			case '.':
				real();
				break;

			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				{
					while(input.in('0', '7'))
						input++;

					if(input.in('8', '9'))
					{
						while(input.in('0', '9'))
							input++;

						lexme.type = Lexme::Integer;
						lexme.stop = &input;

						lexme.report("Invalid octal number '" + lexme.string() + "'");
					}
					else
					{
						lexme.type = Lexme::Octal;
						lexme.stop = &input;
					}
				}
				break;

			default: // Single 0
				lexme.type = Lexme::Integer;
				lexme.stop = &input;
				break;
		}
	}

	void Lexer::number()
	{
		input++;

		while(input >= '0' && input <= '9')
			input++;

		if(input == '.')
			real();
		else
		{
			lexme.stop = &input;
			lexme.type = Lexme::Integer;
		}
	}

	void Lexer::real()
	{
		input++;
		
		if(input.in('0', '9'))
		{
			while(input.in('0', '9'))
				input++;

			lexme.stop = &input;
			lexme.type = Lexme::Real;
		}
		else
		{
			lexme.stop = &input;
			lexme.type = Lexme::Point;
		}
	}

	void Lexer::hex()
	{
		input++;

		if(input.in('0', '9') || input.in('A', 'F'))
		{
			while(input.in('0', '9') || input.in('A', 'F'))
				input++;

			lexme.stop = &input;
			lexme.type = Lexme::Hex;
		}
		else
		{
			lexme.stop = &input;
			lexme.type = Lexme::Hex;

			lexme.report("Invalid hex number: '" + lexme.string() + "'");
		}
	}
};
