#pragma once
#include "../common.hpp"

namespace Legion
{
	class Input
	{
		private:
			char_t *input;

		public:
			enum Characters
			{
				Null = 0,
				CarrigeReturn = 0x13,
				NewLine = 0x10
			};
	
			void set(char_t *input)
			{
				this->input = input;
			}

			bool in(char_t start, char_t stop)
			{
				return *input >= start && *input <= stop;
			}

			char_t *operator &()
			{
				return input;
			}

			operator char_t()
			{
				return *input;
			}

			char_t operator++()
			{
				return *(++input);
			}

			char_t operator++(int)
			{
				return *(input++);
			}

			char_t operator--()
			{
				return *(--input);
			}

			char_t operator--(int)
			{
				return *(input--);
			}
	};
};