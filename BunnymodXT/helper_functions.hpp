#pragma once

#include "sptlib-wrapper.hpp"

#ifdef _WIN32
	#define DLL_EXTENSION ".dll"
	#define PATH_SLASH '\\'
#else
	#define DLL_EXTENSION ".so"
	#define PATH_SLASH '/'
#endif

namespace helper_functions
{
	void com_fixslashes(std::string &str);
	std::string swap_lib(const char* current_lib_path, std::string new_lib_path, const char *start);
	void crash_if_failed(std::string str);
	void disable_vsync();
	void com_filebase(const char *in, char *out);
	void com_filebase(const char *in, std::string &out);

	/*
		Using lowercase is specifically designed so as not to encounter case sensitivity when comparing to the name of the map or game directory (although game may not be happy about if the runner uses a name that does not match the original case from game files / folders, but the main thing is that on our side there should be no issue with this)
		When you checking in code for a game directory or map name, always write the names in lowercase!
	*/
	void convert_to_lowercase(const char *str);
	void convert_to_lowercase(std::string &str);

	inline void allow_multiple_instances() // Make it possible to run multiple Half-Life instances.
	{
		#ifdef _WIN32
		auto mutex = OpenMutexA(SYNCHRONIZE, FALSE, "ValveHalfLifeLauncherMutex");
		if (!mutex)
			mutex = OpenMutexA(SYNCHRONIZE, FALSE, "SvenCoopLauncherMutex");

		if (mutex) 
		{
			EngineMsg("Releasing the launcher mutex.\n");
			ReleaseMutex(mutex);
			CloseHandle(mutex);
		}
		#endif
	}
}
