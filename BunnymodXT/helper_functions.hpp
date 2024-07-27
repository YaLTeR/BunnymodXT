#pragma once

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