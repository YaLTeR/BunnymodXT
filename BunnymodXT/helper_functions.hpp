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

	inline void rgb_to_float(float &r, float &g, float &b)
	{
		r = std::clamp(r / 255.0f, 0.0f, 1.0f);
		g = std::clamp(g / 255.0f, 0.0f, 1.0f);
		b = std::clamp(b / 255.0f, 0.0f, 1.0f);
	}

	inline void rgb_to_float(float *dest, const unsigned r, const unsigned g, const unsigned b)
	{
		dest[0] = std::clamp(r / 255.0f, 0.0f, 1.0f);
		dest[1] = std::clamp(g / 255.0f, 0.0f, 1.0f);
		dest[2] = std::clamp(b / 255.0f, 0.0f, 1.0f);
	}

	inline void rgba_to_float(float &r, float &g, float &b, float &a)
	{
		rgb_to_float(r, g, b);
		a = std::clamp(a / 255.0f, 0.0f, 1.0f);
	}

	inline void rgba_to_float(float *dest, const unsigned r, const unsigned g, const unsigned b, const unsigned a)
	{
		rgb_to_float(dest, r, g, b);
		dest[3] = std::clamp(a / 255.0f, 0.0f, 1.0f);
	}
}