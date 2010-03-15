#pragma once
#include "common.hpp"

namespace Legion
{
	class Range
	{
		public:
			char_t *start;
			char_t *stop;
			char_t *line_start;
			unsigned int line;
			bool error;

			std::string string()
			{
				std::string result((char *)start, length());

				return result;
			}

			unsigned int length()
			{
				return stop - start;
			}

			void report(const std::string& error)
			{
				std::cout << "Line " << line + 1 << ": " << error << std::endl;
			}
	};
};
