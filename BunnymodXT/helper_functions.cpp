#include "stdafx.hpp"
#include "modules.hpp"
#include "helper_functions.hpp"

namespace helper_functions
{
	bool IsPlayer(const edict_t *ent)
	{
		// https://github.com/ValveSoftware/halflife/blob/c7240b965743a53a29491dd49320c88eecf6257b/dlls/player.cpp#L2850

		auto &hw = HwDLL::GetInstance();
		auto &sv = ServerDLL::GetInstance();

		if (strcmp(hw.GetString(ent->v.classname), "player") != 0)
			return false;

		if (!(ent->v.flags & FL_CLIENT))
			return false;

		if (sv.pEngfuncs && hw.ppGlobals)
		{
			int index = sv.pEngfuncs->pfnIndexOfEdict(ent);

			if ((index < 1) || (index > hw.ppGlobals->maxClients)) // gGlobalVariables.maxClients = svs.maxclients
				return false;
		}

		return true;
	}

	void com_fixslashes(std::string &str)
	{
		// https://github.com/ValveSoftware/halflife/blob/c7240b965743a53a29491dd49320c88eecf6257b/game_shared/bot/nav_file.cpp#L680
		#ifdef _WIN32
		std::replace(str.begin(), str.end(), '/', '\\');
		#else
		std::replace(str.begin(), str.end(), '\\', '/');
		#endif
	}

	std::string swap_lib(const char* current_lib_path, std::string new_lib_path, const char *start)
	{
		const std::string filename = current_lib_path;
		const auto index = filename.find(start);

		if ((index == std::string::npos) || // String not found in current path.
			((index > 0) && (filename[index - 1] != PATH_SLASH))) // Previous character from the specified start is not a slash.
			return current_lib_path;

		com_fixslashes(new_lib_path);

		std::string new_path = filename.substr(0, index) + new_lib_path + DLL_EXTENSION;

		return new_path;
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