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
	float adjust_fov_for_widescreen(float fov, float def_aspect_ratio, float our_aspect_ratio);
}