#include "stdafx.hpp"
#include "modules.hpp"
#include "helper_functions.hpp"

namespace helper_functions
{
	auto &cl = ClientDLL::GetInstance();
	auto &sv = ServerDLL::GetInstance();
	auto &hw = HwDLL::GetInstance();

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
		int end;

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

	double ret_bxt_time()
	{
		const auto& gt = CustomHud::GetTime();
		return (gt.hours * 60 * 60) + (gt.minutes * 60) + gt.seconds + (gt.milliseconds / 1000);
	}

	void reset_gamedir()
	{
		hw.gamedir.clear();
		hw.gamedir_lw.clear();
		hw.GameDirMatchID = hw.GameDirStartsWithID = -1;
	}

	void set_gamedir_starts_with()
	{
		#define FIND_GAMEDIR_STARTS_WITH(name, id) \
		if (!hw.GetGameDir().compare(0, sizeof(#name) - 1, #name)) \
		{ \
			hw.GameDirStartsWithID = BXT_CONCAT(GAMEDIR_STARTS_WITH_, id); \
			return; \
		}

		FIND_GAMEDIR_STARTS_WITH(valve, HL)
		FIND_GAMEDIR_STARTS_WITH(gearbox, OPFOR)
		FIND_GAMEDIR_STARTS_WITH(czeror, CSCZDS)
		FIND_GAMEDIR_STARTS_WITH(rewolf, GUNMAN)
		FIND_GAMEDIR_STARTS_WITH(hunger, HUNGER)
		FIND_GAMEDIR_STARTS_WITH(wantedsp, WANTED)
		FIND_GAMEDIR_STARTS_WITH(aomdc, AOMDC)
		FIND_GAMEDIR_STARTS_WITH(echoes, ECHOES)
		FIND_GAMEDIR_STARTS_WITH(poke646, POKE646)
		FIND_GAMEDIR_STARTS_WITH(cryoffear, COF)
		FIND_GAMEDIR_STARTS_WITH(paranoia, PARANOIA)
		FIND_GAMEDIR_STARTS_WITH(bshift, BSHIFT)

		#undef FIND_GAMEDIR_STARTS_WITH
	}

	void set_gamedir_match()
	{
		#define FIND_GAMEDIR_MATCH(name, id) \
		if (!hw.GetGameDir().compare(#name)) \
		{ \
			hw.GameDirMatchID = BXT_CONCAT(GAMEDIR_MATCH_, id); \
			return; \
		}

		FIND_GAMEDIR_MATCH(valve, HL)
		FIND_GAMEDIR_MATCH(gearbox, OPFOR)
		FIND_GAMEDIR_MATCH(czeror, CSCZDS)
		FIND_GAMEDIR_MATCH(rewolf, GUNMAN)
		FIND_GAMEDIR_MATCH(hunger, HUNGER)
		FIND_GAMEDIR_MATCH(wantedsp, WANTED)
		FIND_GAMEDIR_MATCH(aomdc, AOMDC)
		FIND_GAMEDIR_MATCH(echoes, ECHOES)
		FIND_GAMEDIR_MATCH(poke646, POKE646)
		FIND_GAMEDIR_MATCH(cryoffear, COF)
		FIND_GAMEDIR_MATCH(paranoia, PARANOIA)
		FIND_GAMEDIR_MATCH(bshift, BSHIFT)
		FIND_GAMEDIR_MATCH(twhltower2, TWHLTOWER2)
		FIND_GAMEDIR_MATCH(hl_urbicide, URBICIDE)
		FIND_GAMEDIR_MATCH(visitors, VISITORS)
		FIND_GAMEDIR_MATCH(hrp, HLRATS_PARASOMNIA)
		FIND_GAMEDIR_MATCH(decay, DECAY)
		FIND_GAMEDIR_MATCH(caged_fgs, CAGED)
		FIND_GAMEDIR_MATCH(hc, HC2)
		FIND_GAMEDIR_MATCH(blackops, BLACKOPS)
		FIND_GAMEDIR_MATCH(timeline2, TIMELINE2)
		FIND_GAMEDIR_MATCH(steamlink, UPLINK)
		FIND_GAMEDIR_MATCH(hqtrilogy, HALFQUAKE)
		FIND_GAMEDIR_MATCH(azuresheep, AZURESHEEP)
		FIND_GAMEDIR_MATCH(shl, HLSWEET)
		FIND_GAMEDIR_MATCH(biglolly, BIGLOLLY)
		FIND_GAMEDIR_MATCH(lm_txp, TXP)
		FIND_GAMEDIR_MATCH(htc, HTC)
		FIND_GAMEDIR_MATCH(invasion, INVASION)

		// Multiplayer-only games:
		FIND_GAMEDIR_MATCH(tfc, MP_TFC)
		FIND_GAMEDIR_MATCH(cstrike, MP_CS)
		FIND_GAMEDIR_MATCH(czero, MP_CSCZ)
		FIND_GAMEDIR_MATCH(dod, MP_DOD)
		FIND_GAMEDIR_MATCH(dmc, MP_DMC)
		FIND_GAMEDIR_MATCH(ricochet, MP_RICOCHET)

		// Less worth mods start here:
		FIND_GAMEDIR_MATCH(before, BEFORE)
		FIND_GAMEDIR_MATCH(tetsu0_cot, CROWBAROFTIME)
		FIND_GAMEDIR_MATCH(minimicus, MINIMICUS)
		FIND_GAMEDIR_MATCH(gloom, GLOOM)
		FIND_GAMEDIR_MATCH(construction, CONSTRUCTION)
		FIND_GAMEDIR_MATCH(plague, PLAGUE)

		#undef FIND_GAMEDIR_MATCH
	}
};