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
	// https://github.com/ValveSoftware/halflife/blob/c7240b965743a53a29491dd49320c88eecf6257b/dlls/cbase.h#L336-L337
	Vector Center(const Vector absmin, const Vector absmax);
	Vector Center(const edict_t *ent);

	bool IsPlayer(const edict_t *ent);

	void com_fixslashes(std::string &str);
	std::string swap_lib(const char* current_lib_path, std::string new_lib_path, const char *start);
	void crash_if_failed(std::string str);
}