#include "common.hpp"

namespace CommonUtils
{
	std::vector<std::string_view> splitString(std::string_view input, std::string_view delimiter)
	{
		std::vector<std::string_view> tokens;
		size_t i = 0;
		for (;;) {
			const size_t newI = input.find(delimiter, i);
			if (newI == input.npos) {
				break;
			}
			tokens.emplace_back(input.substr(i, newI - i));
			i = newI + delimiter.size();
		}
		tokens.emplace_back(input.substr(i));
		return tokens;
	}
}
