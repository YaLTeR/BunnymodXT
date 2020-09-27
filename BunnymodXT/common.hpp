#pragma once

#include "stdafx.hpp"

namespace CommonUtils
{
	template<typename T>
	std::string joinStrings(const std::vector<T>& tokens, const char* delimiter)
	{
		std::string out;
		auto it = tokens.cbegin();
		for (; std::distance(it, tokens.cend()) >= 2; ++it) {
			out += *it;
			out += delimiter;
		}
		if (it != tokens.cend()) {
			out += *it;
		}
		return out;
	}
}
