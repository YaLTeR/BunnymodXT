#pragma once

namespace helper_functions
{
	void com_fixslashes(std::string &str);
	std::string add_os_library_extension(std::string str);
	const char *swap_lib(const char* current_lib_path, std::string new_lib_path, const char *start);
	void crash_if_failed(std::string str);
}