#pragma once

namespace helper_functions
{
	#ifdef _WIN32
	#define DLL_EXTENSION ".dll"
	#else
	#define DLL_EXTENSION ".so"
	#endif

	void com_fixslashes(std::string &str);
	const char *swap_lib(const char* current_lib_path, std::string new_lib_path, const char *start);
	void crash_if_failed(std::string str);
}