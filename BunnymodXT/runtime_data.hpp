#pragma once
#include <boost/variant/variant.hpp>

namespace RuntimeData
{
	using BXTVersion = std::string;

	// Map from CVar name to value.
	using CVarValues = std::unordered_map<std::string, std::string>;

	struct Time {
		uint32_t hours;
		uint8_t minutes;
		uint8_t seconds;
		double remainder;
	};

	struct BoundCommand {
		std::string key;
		std::string command;
	};

	struct AliasExpansion {
		std::string name;
		std::string command;
	};

	struct ScriptExecution {
		std::string filename;
		std::string contents;
	};

	using Data = boost::variant<BXTVersion,
	                            CVarValues,
	                            Time,
	                            BoundCommand,
	                            AliasExpansion,
	                            ScriptExecution>;

	void Add(Data data);
	void Clear();
	void SaveStored();
}
