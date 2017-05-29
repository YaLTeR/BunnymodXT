#pragma once
#include <boost/variant/variant.hpp>

namespace RuntimeData
{
	struct VersionInfo {
		int build_number;
		std::string bxt_version;
	};

	// Map from CVar name to value.
	using CVarValues = std::unordered_map<std::string, std::string>;

	struct Time {
		uint32_t hours;
		uint8_t minutes;
		uint8_t seconds;
		double remainder;
	};

	struct BoundCommand {
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

	struct CommandExecution {
		std::string command;
	};

	struct GameEndMarker {};

	struct LoadedModules {
		std::vector<std::string> filenames;
	};

	struct CustomTriggerCommand {
		Vector corner_min;
		Vector corner_max;
		std::string command;
	};

	using Data = boost::variant<VersionInfo,
	                            CVarValues,
	                            Time,
	                            BoundCommand,
	                            AliasExpansion,
	                            ScriptExecution,
	                            CommandExecution,
	                            GameEndMarker,
	                            LoadedModules,
				    CustomTriggerCommand>;

	void Add(Data data);
	void Clear();
	void SaveStored();
}
