#pragma once
#include "common.hpp"

namespace Legion
{
	class Range
	{
		public:
			const char_t *start;
			const char_t *stop;
			const char_t *line_start;
			size_t line;
			bool error;

			std::string string()
			{
				std::string result((const char *)start, length());

				return result;
			}

			size_t length()
			{
				return stop - start;
			}

			void report(const std::string& error)
			{
				std::cout << "Line " << line + 1 << ": " << error << std::endl;
			}
	};
};
