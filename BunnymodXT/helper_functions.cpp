#include "stdafx.hpp"
#include "modules.hpp"
#include "helper_functions.hpp"
#include <GL/gl.h>

namespace helper_functions
{
	static byte *gpBuf;
	static int giSize, giRead, giBadRead;
	void BEGIN_READ(void *buf, int size)
	{
		giRead = 0;
		giBadRead = 0;
		giSize = size;
		gpBuf = (byte*)buf;
	}

	int READ_SHARED(int type)
	{
		int c = -1;
		int size = 0;

		switch (type)
		{
			case 0: // READ_CHAR
			case 1: // READ_BYTE
				size = 1;
				break;
			case 2: // READ_SHORT
				size = 2;
				break;
			case 3: // READ_LONG
				size = 4;
				break;
			default:
				break;
		}

		if (giRead + size > giSize)
		{
			giBadRead = 1;
			return -1;
		}

		switch (type)
		{
			case 0: // READ_CHAR
				c = (signed char)gpBuf[giRead];
				break;
			case 1: // READ_BYTE
				c = (unsigned char)gpBuf[giRead];
				break;
			case 2: // READ_SHORT
				c = (short)(gpBuf[giRead] + (gpBuf[giRead + 1] << 8));
				break;
			case 3: // READ_LONG
				c = gpBuf[giRead] + (gpBuf[giRead + 1] << 8) + (gpBuf[giRead + 2] << 16) + (gpBuf[giRead + 3] << 24);
				break;
			default:
				break;
		}

		giRead += size;

		return c;
	}

	int READ_CHAR()
	{
		return READ_SHARED(0);
	}

	int READ_BYTE()
	{
		return READ_SHARED(1);
	}

	int READ_SHORT()
	{
		return READ_SHARED(2);
	}

	int READ_WORD()
	{
		return READ_SHORT();
	}

	int READ_LONG()
	{
		return READ_SHARED(3);
	}

	float READ_FLOAT()
	{
		union
		{
			byte b[4];
			float f;
			int l;
		} dat;

		for (int i = 0; i < 4; i++)
		{
			dat.b[i] = gpBuf[giRead + i];
		}

		giRead += 4;

		//dat.l = LittleLong(dat.l);

		return dat.f;
	}

	char* READ_STRING()
	{
		static char string[2048];
		unsigned int l = 0;
		int c;

		string[0] = 0;

		do
		{
			if (giRead + 1 > giSize)
				break; // no more characters

			c = READ_CHAR();

			if (c == -1 || c == 0)
				break;

			string[l] = c;
			l++;
		} while (l < sizeof(string) - 1);

		string[l] = 0;

		return string;
	}

	float READ_COORD()
	{
		return (float)(READ_SHORT() * (1.0 / 8));
	}

	float READ_ANGLE()
	{
		return (float)(READ_CHAR() * (360.0 / 256));
	}

	float READ_HIRESANGLE()
	{
		return (float)(READ_SHORT() * (360.0 / 65536));
	}

	double ret_bxt_time()
	{
		const auto& gt = CustomHud::GetTime();
		return (gt.hours * 60 * 60) + (gt.minutes * 60) + gt.seconds + (gt.milliseconds / 1000);
	}

	int ret_bxt_flags()
	{
		int bxt_flags = 0;

		auto& hw = HwDLL::GetInstance();
		//auto& cl = ClientDLL::GetInstance();
		//auto& sv = ServerDLL::GetInstance();

		if (hw.is_big_map)
			bxt_flags |= BXT_FLAGS_BIG_MAP;

		return bxt_flags;
	}

	void com_fixslashes(std::string &str)
	{
		#ifdef _WIN32
		std::replace(str.begin(), str.end(), '/', '\\');
		#else
		std::replace(str.begin(), str.end(), '\\', '/');
		#endif
	}

	void com_filebase(const char *in, char *out)
	{
		int len, start, end;

		len = strlen(in);
	
		// scan backward for '.'
		end = len - 1;
		while (end && in[end] != '.' && in[end] != '/' && in[end] != '\\')
			end--;
	
		if (in[end] != '.')		// no '.', copy to end
			end = len - 1;
		else 
			end--;					// Found ',', copy to left of '.'

		// Scan backward for '/'
		start = len - 1;
		while (start >= 0 && in[start] != '/' && in[start] != '\\')
			start--;

		if (in[start] != '/' && in[start] != '\\')
			start = 0;
		else 
			start++;

		// Length of new sting
		len = end - start + 1;

		// Copy partial string
		strncpy(out, &in[start], len);
		// Terminate it
		out[len] = 0;
	}

	std::string swap_lib(const char* current_lib_path, std::string new_lib_path, const char *start)
	{
		const std::string filename = current_lib_path;
		const auto index = filename.find(start);

		if ((index == std::string::npos) || // String not found in current path.
			((index > 0) && (filename[index - 1] != PATH_SLASH))) // Previous character from the specified start is not a slash.
			return current_lib_path;

		com_fixslashes(new_lib_path);

		const std::string new_path = filename.substr(0, index) + new_lib_path + DLL_EXTENSION;

		return new_path;
	}

	void crash_if_failed(const std::string str)
	{
		EngineWarning("%s", str.c_str());

		#ifdef _WIN32
		MessageBox(NULL, str.c_str(), "Fatal Error", MB_OK | MB_ICONERROR);
		#endif

		std::exit(1);
	}

	bool is_valid_index_and_edict(const int index)
	{
		auto& hw = HwDLL::GetInstance();
		edict_t* edicts;
		const int numEdicts = hw.GetEdicts(&edicts);

		if (numEdicts > index)
		{
			const edict_t *ent = edicts + index;
			if (hw.IsValidEdict(ent))
				return true;
			else
				hw.ORIG_Con_Printf("Error: entity with index %d is not valid\n", index);
		}
		else
		{
			hw.ORIG_Con_Printf("Error: entity with index %d does not exist; there are %d entities in total\n", index, numEdicts);
		}

		return false;
	}

	float adjust_fov_for_widescreen(const float fov, const float def_aspect_ratio, const float our_aspect_ratio)
	{
		const float calculated_fov = static_cast<float>(std::atan(std::tan(fov * M_PI / 360.0f) * def_aspect_ratio * our_aspect_ratio) * 360.0f / M_PI);
		return std::clamp(calculated_fov, 1.0f, 179.0f);
	}

	void convert_to_lowercase(const char *str)
	{
		/*
			Using lowercase is specifically designed so as not to encounter case sensitivity when comparing to the name of the map or game directory (although game may not be happy about if the runner uses a name that does not match the original case from game files / folders, but the main thing is that on our side there should be no issue with this)
			When you checking in code for a game directory or map name, always write the names in lowercase!
		*/

		unsigned char *str_lw = (unsigned char *)str;
		while (*str_lw) 
		{
			*str_lw = tolower(*str_lw);
			str_lw++;
		}
	}

	bool does_gamedir_starts_with(const char *game, const size_t len)
	{
		const std::string gamedir = ClientDLL::GetInstance().GetGameDirectory(true);
		if (!gamedir.empty() && !strncmp(gamedir.c_str(), game, len))
			return true;

		return false;
	}

	bool does_gamedir_match(const char *game)
	{
		const std::string gamedir = ClientDLL::GetInstance().GetGameDirectory(true);
		if (!gamedir.empty() && !strcmp(gamedir.c_str(), game))
			return true;

		return false;
	}

	bool does_mapname_starts_with(const char *map, const size_t len)
	{
		const std::string mapname = ClientDLL::GetInstance().GetLevelName(true);
		if (!mapname.empty() && !strncmp(mapname.c_str(), map, len))
			return true;

		return false;
	}

	bool does_mapname_match(const char *map)
	{
		const std::string mapname = ClientDLL::GetInstance().GetLevelName(true);
		if (!mapname.empty() && !strcmp(mapname.c_str(), map))
			return true;

		return false;
	}

	std::string get_steam_id(const unsigned long steamID32)
	{
		std::ostringstream ss;
		ss << "STEAM_0:" << ((steamID32 % 2) ? 1 : 0) << ":" << (steamID32 / 2) << "\n";
		return ss.str();
	}

	steamid_t get_steam_id_64(const unsigned long steamID32)
	{
		return STEAMID64_CONST + steamID32;
	}

	void split_console_print_to_chunks(const std::string str, const unsigned int max_string_length)
	{
		const size_t string_length = str.size();
		// We must round the total number of chunks to the next value if it is not an integer in order for the remaining characters to be drawed.
		const size_t count_chunks = static_cast<size_t>(std::ceil(static_cast<double>(string_length) / max_string_length));

		for (size_t chunk = 0; chunk < count_chunks; chunk++)
		{
			const std::string result = str.substr(chunk * max_string_length, max_string_length);

			// Don't start a new chunk on a newline, because we don't want to break the sequence of text!
			// Instead, the newlines must be specified in the text itself!
			HwDLL::GetInstance().ORIG_Con_Printf("%s", result.c_str());
		}
	}

	void split_console_print_to_chunks(const std::string str)
	{
		split_console_print_to_chunks(str, MAXPRINTMSG);
	}

	bool is_entity_give_infinite_health(const edict_t* ent)
	{
		/*
			An explanation of how to determine whether an entity can give you health when blocking it:

			When you block an entity, the Blocked function of that entity class is called
			In the Blocked function the TakeDamage function is called with the value in the flDamage parameter taken from pev->dmg
			The value that the mapper sets in the map editor for the "dmg" parameter will be set in pev->dmg (this is handled in EntvarsKeyvalue server function)
			Well, if pev->dmg is still a negative value when calling this TakeDamage function
			Then, in this case the game begins not to take your health, but to give it more instead
		*/

		auto &hw = HwDLL::GetInstance();

		const char* classname = hw.GetString(ent->v.classname);
		if ((!strcmp(classname, "func_door")) || (!strcmp(classname, "func_door_rotating")) // https://github.com/ValveSoftware/halflife/blob/c7240b965743a53a29491dd49320c88eecf6257b/dlls/doors.cpp#L712
		|| (!strcmp(classname, "func_rotating")) // https://github.com/ValveSoftware/halflife/blob/c7240b965743a53a29491dd49320c88eecf6257b/dlls/bmodels.cpp#L716
		|| (!strcmp(classname, "func_train"))) // https://github.com/ValveSoftware/halflife/blob/c7240b965743a53a29491dd49320c88eecf6257b/dlls/plats.cpp#L683
		{
			if (ent->v.dmg < 0.0f)
				return true;
		}

		return false;
	}

	inline void Draw_FillRGBAShared(int x, int y, int width, int height, int r, int g, int b, int a, bool blend)
	{
		glDisable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		if (blend)
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		else
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);

		glColor4f(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);

		glBegin(GL_QUADS);
			glVertex2f((float)x, (float)y);
			glVertex2f((float)(x + width), (float)y);
			glVertex2f((float)(x + width), (float)(y + height));
			glVertex2f((float)x, (float)(y + height));
		glEnd();

		glColor3f(1.0f, 1.0f, 1.0f);
		glEnable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
	}

	void Draw_FillRGBA(int x, int y, int width, int height, int r, int g, int b, int a)
	{
		Draw_FillRGBAShared(x, y, width, height, r, g, b, a, false);
	}

	void Draw_FillRGBABlend(int x, int y, int width, int height, int r, int g, int b, int a)
	{
		Draw_FillRGBAShared(x, y, width, height, r, g, b, a, true);
	}

	int build_number(const char *date)
	{
		static const char *mon[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
		static const char mond[12] = { 31,    28,    31,    30,    31,    30,    31,    31,    30,    31,    30,    31 };

		int m = 0, d = 0, y = 0, build = 0;
		for (m = 0; m < 11; m++)
		{
			if (Q_strncasecmp(&date[0], mon[m], 3) == 0)
				break;
			d += mond[m];
		}

		d += atoi(&date[4]) - 1;
		y = atoi(&date[7]) - 1900;
		build = d + (int)((y - 1) * 365.25);

		if (((y % 4) == 0) && m > 1)
			build += 1;

		#define START_DATE_GOLDSRC 34995 // Used in GoldSrc and Quake builds (Oct 24 1996)
		#define START_DATE_SOURCE 35739 // Used in builds on the Source Engine (Nov 7 1998)
		#define START_DATE_HL2_BETA 37527 // Used in builds on the Source Engine before the official release of HL2 (Sep 30 2003)

		build -= START_DATE_GOLDSRC;

		return build;
	}

	int build_number()
	{
		static const int build = build_number(__DATE__);
		return build;
	}

	bool is_entity_kz_start(const char *targetname)
	{
		static const char* kz_start[] = {"hlkz_start", "counter_start", "clockstartbutton", "firsttimerelay", "but_start", "counter_start_button", 
										"multi_start", "timer_startbutton", "start_timer_emi", "gogogo", "startcounter"};

		for (const auto &name : kz_start)
		{
			if (!strcmp(targetname, name))
				return true;
		}

		return false;
	}

	bool is_entity_kz_stop(const char *targetname)
	{
		static const char* kz_stop[] = {"hlkz_finish", "counter_off", "clockstopbutton", "clockstop", "but_stop", "counter_stop_button", 
										"multi_stop", "stop_counter", "m_counter_end_emi", "stopcounter"};

		for (const auto &name : kz_stop)
		{
			if (!strcmp(targetname, name))
				return true;
		}

		return false;
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

	void update_data_at_shutdown_sv()
	{
	}

	void gamedir_set_booleans(const char *gamedir)
	{
		if (gamedir && gamedir[0])
		{
			auto &hw = HwDLL::GetInstance();

			if (!strcmp(gamedir, "cstrike") || !strcmp(gamedir, "czero"))
				hw.is_cs_dir = true;
			else if (!strncmp(gamedir, HF_StrAndLen("czeror")))
				hw.is_csczds_dir = true;
			else if (!strcmp(gamedir, "tfc"))
				hw.is_tfc_dir = true;
			else if (!strncmp(gamedir, HF_StrAndLen("cryoffear")))
				hw.is_cof_dir = true;
			else if (!strcmp(gamedir, "dod"))
				hw.is_dod_dir = true;
		}
	}

	void gamedir_reset()
	{
		auto &cl = ClientDLL::GetInstance();
		auto &hw = HwDLL::GetInstance();

		cl.gamedir_clean.clear();
		cl.gamedir_clean_lw.clear();
		hw.is_cs_dir = hw.is_csczds_dir = hw.is_tfc_dir = hw.is_cof_dir = hw.is_dod_dir = false;
	}

// Below this comment are only functions from CBaseEntity or CBasePlayer class!

	int IsInWorld(const Vector origin, const Vector velocity, const int map_size, const int map_max_velocity)
	{
		/*
			We specifically return int instead of bool
			Because we are returning this custom function instead of the original one in specific case
			And the type of the original function is BOOL, which is int
		*/
		
		// position
		if (origin.x >= map_size) return 0;
		if (origin.y >= map_size) return 0;
		if (origin.z >= map_size) return 0;
		if (origin.x <= -map_size) return 0;
		if (origin.y <= -map_size) return 0;
		if (origin.z <= -map_size) return 0;
		// speed
		if (velocity.x >= map_max_velocity) return 0;
		if (velocity.y >= map_max_velocity) return 0;
		if (velocity.z >= map_max_velocity) return 0;
		if (velocity.x <= -map_max_velocity) return 0;
		if (velocity.y <= -map_max_velocity) return 0;
		if (velocity.z <= -map_max_velocity) return 0;

		return 1;
	}

	int IsInWorld(const edict_t *ent, const int map_size, const int map_max_velocity)
	{
		return IsInWorld(ent->v.origin, ent->v.velocity, map_size, map_max_velocity);
	}

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

	bool IsOnLadder(const int movetype)
	{
		if (movetype == MOVETYPE_FLY)
			return true;
		else
			return false;
	}

	bool IsOnLadder(const edict_t *ent)
	{
		return IsOnLadder(ent->v.movetype);
	}

	bool IsOnLadder(const edict_t *ent, const bool is_player)
	{
		if (is_player && !IsPlayer(ent))
			return false;
		else
			return IsOnLadder(ent->v.movetype);
	}

	bool IsNoClipping(const int movetype)
	{
		if (movetype == MOVETYPE_NOCLIP)
			return true;
		else
			return false;
	}

	bool IsNoClipping(const edict_t *ent)
	{
		return IsNoClipping(ent->v.movetype);
	}

	bool IsNoClipping(const edict_t *ent, const bool is_player)
	{
		if (is_player && !IsPlayer(ent))
			return false;
		else
			return IsNoClipping(ent->v.movetype);
	}

	bool IsBSPModel(const int solid, const int movetype)
	{
		if ((solid == SOLID_BSP) || (movetype == MOVETYPE_PUSHSTEP))
			return true;
		else
			return false;
	}

	bool IsBSPModel(const edict_t *ent)
	{
		return IsBSPModel(ent->v.solid, ent->v.movetype);
	}

	bool ReflectGauss(const int solid, const int movetype, const float takedamage)
	{
		if (!IsBSPModel(solid, movetype))
			return false;

		if (!takedamage)
			return false;

		return true;
	}

	bool ReflectGauss(const edict_t *ent)
	{
		return ReflectGauss(ent->v.solid, ent->v.movetype, ent->v.takedamage);
	}

	Vector Center(const Vector absmin, const Vector absmax)
	{
		return (absmin + absmax) * 0.5;
	}

	Vector Center(const edict_t *ent)
	{
		return Center(ent->v.absmin, ent->v.absmax);
	}

	Vector EyePosition(const Vector origin, const Vector view_ofs)
	{
		return origin + view_ofs;
	}

	Vector EyePosition(const edict_t *ent)
	{
		return EyePosition(ent->v.origin, ent->v.view_ofs);
	}

	bool Intersects(const Vector absmin1, const Vector absmax1, const Vector absmin2, const Vector absmax2)
	{
		if (absmin1.x > absmax2.x ||
			absmin1.y > absmax2.y ||
			absmin1.z > absmax2.z ||
			absmax1.x < absmin2.x ||
			absmax1.y < absmin2.y ||
			absmax1.y < absmin2.y)
			return false;

		return true;
	}

	bool Intersects(const edict_t *ent1, const edict_t *ent2)
	{
		return Intersects(ent1->v.absmin, ent1->v.absmax, ent2->v.absmin, ent2->v.absmax);
	}

// Below this comment are only functions for determining type or flags!

	std::string get_difficulty(const int skill)
	{
		switch (skill)
		{
			case 1: return "Easy";
			case 2: return "Normal";
			case 3: return "Hard";
			default: return "Unknown";
		}
	}

	std::string get_hitgroup(const int iHitgroup)
	{
		switch (iHitgroup)
		{
			case HITGROUP_GENERIC: return "Generic";
			case HITGROUP_HEAD: return "Head";
			case HITGROUP_CHEST: return "Chest";
			case HITGROUP_STOMACH: return "Stomach";
			case HITGROUP_LEFTARM: return "Left Arm";
			case HITGROUP_RIGHTARM: return "Right Arm";
			case HITGROUP_LEFTLEG: return "Left Leg";
			case HITGROUP_RIGHTLEG: return "Right Leg";
			default: return "Unknown";
		}
	}

	std::string get_typedescription_fieldtype(const int fieldType)
	{
		switch (fieldType)
		{
			RET_CASE_STR(FIELD_FLOAT)
			RET_CASE_STR(FIELD_STRING)
			RET_CASE_STR(FIELD_ENTITY)
			RET_CASE_STR(FIELD_CLASSPTR)
			RET_CASE_STR(FIELD_EHANDLE)
			RET_CASE_STR(FIELD_EVARS)
			RET_CASE_STR(FIELD_EDICT)
			RET_CASE_STR(FIELD_VECTOR)
			RET_CASE_STR(FIELD_POSITION_VECTOR)
			RET_CASE_STR(FIELD_POINTER)
			RET_CASE_STR(FIELD_INTEGER)
			RET_CASE_STR(FIELD_FUNCTION)
			RET_CASE_STR(FIELD_BOOLEAN)
			RET_CASE_STR(FIELD_SHORT)
			RET_CASE_STR(FIELD_CHARACTER)
			RET_CASE_STR(FIELD_TIME)
			RET_CASE_STR(FIELD_MODELNAME)
			RET_CASE_STR(FIELD_SOUNDNAME)
			RET_CASE_STR(FIELD_TYPECOUNT)
			default: return "Unknown";
		}
	}

	std::string get_renderfx(const int renderfx)
	{
		switch (renderfx)
		{
			RET_CASE_STR(kRenderFxNone)
			RET_CASE_STR(kRenderFxPulseSlow)
			RET_CASE_STR(kRenderFxPulseFast)
			RET_CASE_STR(kRenderFxPulseSlowWide)
			RET_CASE_STR(kRenderFxPulseFastWide)
			RET_CASE_STR(kRenderFxFadeSlow)
			RET_CASE_STR(kRenderFxFadeFast)
			RET_CASE_STR(kRenderFxSolidSlow)
			RET_CASE_STR(kRenderFxSolidFast)
			RET_CASE_STR(kRenderFxStrobeSlow)
			RET_CASE_STR(kRenderFxStrobeFast)
			RET_CASE_STR(kRenderFxStrobeFaster)
			RET_CASE_STR(kRenderFxFlickerSlow)
			RET_CASE_STR(kRenderFxFlickerFast)
			RET_CASE_STR(kRenderFxNoDissipation)
			RET_CASE_STR(kRenderFxDistort)
			RET_CASE_STR(kRenderFxHologram)
			RET_CASE_STR(kRenderFxDeadPlayer)
			RET_CASE_STR(kRenderFxExplode)
			RET_CASE_STR(kRenderFxGlowShell)
			RET_CASE_STR(kRenderFxClampMinScale)
			default: return "Unknown";
		}
	}

	std::string get_rendermode(const int rendermode)
	{
		switch (rendermode)
		{
			RET_CASE_STR(kRenderNormal)
			RET_CASE_STR(kRenderTransColor)
			RET_CASE_STR(kRenderTransTexture)
			RET_CASE_STR(kRenderGlow)
			RET_CASE_STR(kRenderTransAlpha)
			RET_CASE_STR(kRenderTransAdd)
			default: return "Unknown";
		}
	}

	std::string get_effects(const int flags)
	{
		std::ostringstream out;

		// The flags here were arranged in order from smallest to highest bits.
		PRINT_FLAG(EF_BRIGHTFIELD);
		PRINT_FLAG(EF_MUZZLEFLASH);
		PRINT_FLAG(EF_BRIGHTLIGHT);
		PRINT_FLAG(EF_DIMLIGHT);
		PRINT_FLAG(EF_INVLIGHT);
		PRINT_FLAG(EF_NOINTERP);
		PRINT_FLAG(EF_LIGHT);
		PRINT_FLAG(EF_NODRAW);

		out << '\n';

		return out.str();
	}

	std::string get_solid(const int solid)
	{
		switch (solid)
		{
			RET_CASE_STR(SOLID_NOT)
			RET_CASE_STR(SOLID_TRIGGER)
			RET_CASE_STR(SOLID_BBOX)
			RET_CASE_STR(SOLID_SLIDEBOX)
			RET_CASE_STR(SOLID_BSP)
			default: return "Unknown";
		}
	}

	std::string get_movetype(const int movetype)
	{
		switch (movetype)
		{
			RET_CASE_STR(MOVETYPE_NONE)
			RET_CASE_STR(MOVETYPE_WALK)
			RET_CASE_STR(MOVETYPE_STEP)
			RET_CASE_STR(MOVETYPE_FLY)
			RET_CASE_STR(MOVETYPE_TOSS)
			RET_CASE_STR(MOVETYPE_PUSH)
			RET_CASE_STR(MOVETYPE_NOCLIP)
			RET_CASE_STR(MOVETYPE_FLYMISSILE)
			RET_CASE_STR(MOVETYPE_BOUNCE)
			RET_CASE_STR(MOVETYPE_BOUNCEMISSILE)
			RET_CASE_STR(MOVETYPE_FOLLOW)
			RET_CASE_STR(MOVETYPE_PUSHSTEP)
			default: return "Unknown";
		}
	}

	std::string get_flags(const int flags)
	{
		std::ostringstream out;

		// The flags here were arranged in order from smallest to highest bits.
		PRINT_FLAG(FL_FLY);
		PRINT_FLAG(FL_SWIM);
		PRINT_FLAG(FL_CONVEYOR);
		PRINT_FLAG(FL_CLIENT);
		PRINT_FLAG(FL_INWATER);
		PRINT_FLAG(FL_MONSTER);
		PRINT_FLAG(FL_GODMODE);
		PRINT_FLAG(FL_NOTARGET);
		PRINT_FLAG(FL_SKIPLOCALHOST);
		PRINT_FLAG(FL_ONGROUND);
		PRINT_FLAG(FL_PARTIALGROUND);
		PRINT_FLAG(FL_WATERJUMP);
		PRINT_FLAG(FL_FROZEN);
		PRINT_FLAG(FL_FAKECLIENT);
		PRINT_FLAG(FL_DUCKING);
		PRINT_FLAG(FL_FLOAT);
		PRINT_FLAG(FL_GRAPHED);
		PRINT_FLAG(FL_IMMUNE_WATER);
		PRINT_FLAG(FL_IMMUNE_SLIME);
		PRINT_FLAG(FL_IMMUNE_LAVA);
		PRINT_FLAG(FL_PROXY);
		PRINT_FLAG(FL_ALWAYSTHINK);
		PRINT_FLAG(FL_BASEVELOCITY);
		PRINT_FLAG(FL_MONSTERCLIP);
		PRINT_FLAG(FL_ONTRAIN);
		PRINT_FLAG(FL_WORLDBRUSH);
		PRINT_FLAG(FL_SPECTATOR);
		// unknown
		// unknown
		PRINT_FLAG(FL_CUSTOMENTITY);
		PRINT_FLAG(FL_KILLME);
		PRINT_FLAG(FL_DORMANT);

		out << '\n';

		return out.str();
	}

	std::string get_spawnflags_breakable(const int flags, const bool pushable)
	{
		std::ostringstream out;

		// The flags here were arranged in order from smallest to highest bits.
		if (flags & SF_BREAK_TRIGGER_ONLY)
			out << "Only trigger; ";
		if (flags & SF_BREAK_TOUCH)
			out << "Touch; ";
		if (flags & SF_BREAK_PRESSURE)
			out << "Pressure; ";
		// unknown
		// unknown
		// unknown
		// unknown
		if (pushable && (flags & SF_PUSH_BREAKABLE))
			out << "Breakable; ";
		if (flags & SF_BREAK_CROWBAR)
			out << "Instant crowbar; ";

		out << '\n';

		return out.str();
	}

	std::string get_spawnflags_door(const int flags, const bool rotating)
	{
		std::ostringstream out;

		// The flags here were arranged in order from smallest to highest bits.
		if (flags & SF_DOOR_START_OPEN)
			out << "Starts Open; ";
		if (rotating && (flags & SF_DOOR_ROTATE_BACKWARDS))
			out << "Reverse Dir; ";
		// unknown
		if (flags & SF_DOOR_PASSABLE)
			out << "Passable; ";
		if (rotating && (flags & SF_DOOR_ONEWAY))
			out << "One-way; ";
		if (flags & SF_DOOR_NO_AUTO_RETURN)
			out << "Toggle; ";
		if (rotating && (flags & SF_DOOR_ROTATE_Z))
			out << "X Axis; ";
		if (rotating && (flags & SF_DOOR_ROTATE_X))
			out << "Y Axis; ";
		if (flags & SF_DOOR_USE_ONLY)
			out << "Use Only; ";
		if (flags & SF_DOOR_NOMONSTERS)
			out << "Monsters Can't; ";

		out << '\n';

		return out.str();
	}

	std::string get_spawnflags_trigger(const int flags)
	{
		std::ostringstream out;

		// The flags here were arranged in order from smallest to highest bits.
		if (flags & SF_TRIGGER_ALLOWMONSTERS)
			out << "Monsters; ";
		if (flags & SF_TRIGGER_NOCLIENTS)
			out << "No clients; ";
		if (flags & SF_TRIGGER_PUSHABLES)
			out << "Pushables; ";

		out << '\n';

		return out.str();
	}

	std::string get_spawnflags_monster(const int flags)
	{
		std::ostringstream out;

		// The flags here were arranged in order from smallest to highest bits.
		if (flags & SF_MONSTER_WAIT_TILL_SEEN)
			out << "WaitTillSeen; ";
		if (flags & SF_MONSTER_GAG)
			out << "Gag; ";
		if (flags & SF_MONSTER_HITMONSTERCLIP)
			out << "MonsterClip; ";
		// unknown
		if (flags & SF_MONSTER_PRISONER)
			out << "Prisoner; ";
		// unknown
		// unknown
		if (flags & SF_MONSTER_WAIT_FOR_SCRIPT)
			out << "WaitForScript; ";
		if (flags & SF_MONSTER_PREDISASTER)
			out << "Pre-Disaster; ";
		if (flags & SF_MONSTER_FADECORPSE)
			out << "Fade Corpse; ";

		out << '\n';

		return out.str();
	}

	std::string get_spawnflags(const int spawnflags, const char *classname)
	{
		if (!strcmp(classname, "func_breakable"))
			return get_spawnflags_breakable(spawnflags, false);
		else if (!strcmp(classname, "func_pushable"))
			return get_spawnflags_breakable(spawnflags, true);
		else if (!strcmp(classname, "func_door") || !strcmp(classname, "momentary_door"))
			return get_spawnflags_door(spawnflags, false);
		else if (!strcmp(classname, "func_door_rotating"))
			return get_spawnflags_door(spawnflags, true);

		return "\n";
	}

	std::string get_monster_triggercondition(const int m_iTriggerCondition)
	{
		switch (m_iTriggerCondition)
		{
			case AITRIGGER_NONE: return "No Trigger";
			case AITRIGGER_SEEPLAYER_ANGRY_AT_PLAYER: return "See Player, Mad at Player";
			case AITRIGGER_TAKEDAMAGE: return "Take Damage";
			case AITRIGGER_HALFHEALTH: return "50% Health Remaining";
			case AITRIGGER_DEATH: return "Death";
			case AITRIGGER_SQUADMEMBERDIE: return "Squad Member Dead";
			case AITRIGGER_SQUADLEADERDIE: return "Squad Leader Dead";
			case AITRIGGER_HEARWORLD: return "Hear World";
			case AITRIGGER_HEARPLAYER: return "Hear Player";
			case AITRIGGER_HEARCOMBAT: return "Hear Combat";
			case AITRIGGER_SEEPLAYER_UNCONDITIONAL: return "See Player Unconditional";
			case AITRIGGER_SEEPLAYER_NOT_IN_COMBAT: return "See Player, Not In Combat";
			default: return "Unknown";
		}
	}

	std::string get_monster_task(const int iTask)
	{
		switch (iTask)
		{
			RET_CASE_STR(TASK_INVALID)
			RET_CASE_STR(TASK_WAIT)
			RET_CASE_STR(TASK_WAIT_FACE_ENEMY)
			RET_CASE_STR(TASK_WAIT_PVS)
			RET_CASE_STR(TASK_SUGGEST_STATE)
			RET_CASE_STR(TASK_WALK_TO_TARGET)
			RET_CASE_STR(TASK_RUN_TO_TARGET)
			RET_CASE_STR(TASK_MOVE_TO_TARGET_RANGE)
			RET_CASE_STR(TASK_GET_PATH_TO_ENEMY)
			RET_CASE_STR(TASK_GET_PATH_TO_ENEMY_LKP)
			RET_CASE_STR(TASK_GET_PATH_TO_ENEMY_CORPSE)
			RET_CASE_STR(TASK_GET_PATH_TO_LEADER)
			RET_CASE_STR(TASK_GET_PATH_TO_SPOT)
			RET_CASE_STR(TASK_GET_PATH_TO_TARGET)
			RET_CASE_STR(TASK_GET_PATH_TO_HINTNODE)
			RET_CASE_STR(TASK_GET_PATH_TO_LASTPOSITION)
			RET_CASE_STR(TASK_GET_PATH_TO_BESTSOUND)
			RET_CASE_STR(TASK_GET_PATH_TO_BESTSCENT)
			RET_CASE_STR(TASK_RUN_PATH)
			RET_CASE_STR(TASK_WALK_PATH)
			RET_CASE_STR(TASK_STRAFE_PATH)
			RET_CASE_STR(TASK_CLEAR_MOVE_WAIT)
			RET_CASE_STR(TASK_STORE_LASTPOSITION)
			RET_CASE_STR(TASK_CLEAR_LASTPOSITION)
			RET_CASE_STR(TASK_PLAY_ACTIVE_IDLE)
			RET_CASE_STR(TASK_FIND_HINTNODE)
			RET_CASE_STR(TASK_CLEAR_HINTNODE)
			RET_CASE_STR(TASK_SMALL_FLINCH)
			RET_CASE_STR(TASK_FACE_IDEAL)
			RET_CASE_STR(TASK_FACE_ROUTE)
			RET_CASE_STR(TASK_FACE_ENEMY)
			RET_CASE_STR(TASK_FACE_HINTNODE)
			RET_CASE_STR(TASK_FACE_TARGET)
			RET_CASE_STR(TASK_FACE_LASTPOSITION)
			RET_CASE_STR(TASK_RANGE_ATTACK1)
			RET_CASE_STR(TASK_RANGE_ATTACK2)
			RET_CASE_STR(TASK_MELEE_ATTACK1)
			RET_CASE_STR(TASK_MELEE_ATTACK2)
			RET_CASE_STR(TASK_RELOAD)
			RET_CASE_STR(TASK_RANGE_ATTACK1_NOTURN)
			RET_CASE_STR(TASK_RANGE_ATTACK2_NOTURN)
			RET_CASE_STR(TASK_MELEE_ATTACK1_NOTURN)
			RET_CASE_STR(TASK_MELEE_ATTACK2_NOTURN)
			RET_CASE_STR(TASK_RELOAD_NOTURN)
			RET_CASE_STR(TASK_SPECIAL_ATTACK1)
			RET_CASE_STR(TASK_SPECIAL_ATTACK2)
			RET_CASE_STR(TASK_CROUCH)
			RET_CASE_STR(TASK_STAND)
			RET_CASE_STR(TASK_GUARD)
			RET_CASE_STR(TASK_STEP_LEFT)
			RET_CASE_STR(TASK_STEP_RIGHT)
			RET_CASE_STR(TASK_STEP_FORWARD)
			RET_CASE_STR(TASK_STEP_BACK)
			RET_CASE_STR(TASK_DODGE_LEFT)
			RET_CASE_STR(TASK_DODGE_RIGHT)
			RET_CASE_STR(TASK_SOUND_ANGRY)
			RET_CASE_STR(TASK_SOUND_DEATH)
			RET_CASE_STR(TASK_SET_ACTIVITY)
			RET_CASE_STR(TASK_SET_SCHEDULE)
			RET_CASE_STR(TASK_SET_FAIL_SCHEDULE)
			RET_CASE_STR(TASK_CLEAR_FAIL_SCHEDULE)
			RET_CASE_STR(TASK_PLAY_SEQUENCE)
			RET_CASE_STR(TASK_PLAY_SEQUENCE_FACE_ENEMY)
			RET_CASE_STR(TASK_PLAY_SEQUENCE_FACE_TARGET)
			RET_CASE_STR(TASK_SOUND_IDLE)
			RET_CASE_STR(TASK_SOUND_WAKE)
			RET_CASE_STR(TASK_SOUND_PAIN)
			RET_CASE_STR(TASK_SOUND_DIE)
			RET_CASE_STR(TASK_FIND_COVER_FROM_BEST_SOUND)
			RET_CASE_STR(TASK_FIND_COVER_FROM_ENEMY)
			RET_CASE_STR(TASK_FIND_LATERAL_COVER_FROM_ENEMY)
			RET_CASE_STR(TASK_FIND_NODE_COVER_FROM_ENEMY)
			RET_CASE_STR(TASK_FIND_NEAR_NODE_COVER_FROM_ENEMY)
			RET_CASE_STR(TASK_FIND_FAR_NODE_COVER_FROM_ENEMY)
			RET_CASE_STR(TASK_FIND_COVER_FROM_ORIGIN)
			RET_CASE_STR(TASK_EAT)
			RET_CASE_STR(TASK_DIE)
			RET_CASE_STR(TASK_WAIT_FOR_SCRIPT)
			RET_CASE_STR(TASK_PLAY_SCRIPT)
			RET_CASE_STR(TASK_ENABLE_SCRIPT)
			RET_CASE_STR(TASK_PLANT_ON_SCRIPT)
			RET_CASE_STR(TASK_FACE_SCRIPT)
			RET_CASE_STR(TASK_WAIT_RANDOM)
			RET_CASE_STR(TASK_WAIT_INDEFINITE)
			RET_CASE_STR(TASK_STOP_MOVING)
			RET_CASE_STR(TASK_TURN_LEFT)
			RET_CASE_STR(TASK_TURN_RIGHT)
			RET_CASE_STR(TASK_REMEMBER)
			RET_CASE_STR(TASK_FORGET)
			RET_CASE_STR(TASK_WAIT_FOR_MOVEMENT)
			RET_CASE_STR(LAST_COMMON_TASK)
			default: return "Unknown";
		}
	}
};