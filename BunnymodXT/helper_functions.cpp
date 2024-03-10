#include "stdafx.hpp"
#include "modules.hpp"

namespace helper_functions
{
	void com_fixslashes(std::string &str)
	{
		// https://github.com/ValveSoftware/halflife/blob/c7240b965743a53a29491dd49320c88eecf6257b/game_shared/bot/nav_file.cpp#L680
		#ifdef _WIN32
		str = std::regex_replace(str, std::regex("/"), "\\");
		#else
		str = std::regex_replace(str, std::regex("\\"), "/");
		#endif
	}

	const char *swap_lib(const char* current_lib_path, std::string new_lib_path, const char *start)
	{
		const std::string filename = current_lib_path;
		const auto index = filename.find(start);

		if (index == std::string::npos)
			return current_lib_path;

		com_fixslashes(new_lib_path);

		static std::string new_path;
		new_path = filename.substr(0, index) + new_lib_path + DLL_EXTENSION;

		return new_path.c_str();
	}

	void crash_if_failed(std::string str)
	{
		EngineWarning("%s", str.c_str());

		#ifdef _WIN32
		MessageBox(NULL, str.c_str(), "Fatal Error", MB_OK | MB_ICONERROR);
		#endif

		std::exit(1);
	}
};