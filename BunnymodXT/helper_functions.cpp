#include "stdafx.hpp"
#include "modules.hpp"
#include "helper_functions.hpp"

namespace helper_functions
{
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

	void disable_vsync()
	{
		#ifdef _WIN32
		auto &hw = HwDLL::GetInstance();
		if (hw.check_vsync)
		{
			const bool bxtDisableVSync = getenv("BXT_DISABLE_VSYNC");
			if (bxtDisableVSync)
			{
				typedef BOOL(APIENTRY* PFNWGLSWAPINTERVALPROC)(int);
				PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = 0;
				wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress("wglSwapIntervalEXT");
				if (wglSwapIntervalEXT)
					wglSwapIntervalEXT(0);
			}
			hw.check_vsync = false;
		}
		#endif
	}

	void _com_filebase(const char *in, int &len, int &start)
	{
		int len, start, end;

		len = strlen(in);

		// Scan backward for '.'
		end = len - 1;
		while (end && in[end] != '.' && in[end] != '/' && in[end] != '\\')
			end--;

		if (in[end] != '.') // No '.', copy to end
			end = len - 1;
		else 
			end--; // Found ',', copy to left of '.'

		// Scan backward for '/'
		start = len - 1;
		while (start >= 0 && in[start] != '/' && in[start] != '\\')
			start--;

		if (in[start] != '/' && in[start] != '\\')
			start = 0;
		else 
			start++;

		// Length of new string
		len = end - start + 1;
	}

	void com_filebase(const char *in, char *out)
	{
		int len, start;
		_com_filebase(in, len, start);
		strncpy(out, &in[start], len);
		out[len] = 0;
	}

	void com_filebase(const char *in, std::string &out)
	{
		int len, start;
		_com_filebase(in, len, start);
		out = std::string(&in[start], len);
	}

	void convert_to_lowercase(const char *str)
	{
		unsigned char *str_lw = (unsigned char *)str;
		while (*str_lw) 
		{
			*str_lw = tolower(*str_lw);
			str_lw++;
		}
	}

	void convert_to_lowercase(std::string &str)
	{
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c){ return tolower(c); });
	}
};