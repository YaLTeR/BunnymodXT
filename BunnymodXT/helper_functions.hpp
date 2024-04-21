#pragma once

#define RET_CASE_STR(c) case c: return "" #c "";
#define PRINT_FLAG(fl) \
	if (flags & fl) \
		out << "" #fl "; ";

#ifdef _WIN32
	#define DLL_EXTENSION ".dll"
	#define PATH_SLASH '\\'
	#define Q_strncasecmp _strnicmp
	#define Q_stricmp _stricmp
#else
	#define DLL_EXTENSION ".so"
	#define PATH_SLASH '/'
	#define Q_strncasecmp strncasecmp
	#define Q_stricmp strcasecmp
#endif

#define Q_strcasecmp(s1, s2) \
	Q_strncasecmp(s1, s2, 99999)

#define MAXPRINTMSG 4096 // Con_Printf is limited to 4096 characters.

const steamid_t STEAMID64_CONST = 76561197960265728; // 0x110000100000000

// entvars_t from HLSDK 1.0 does not have such variables, for this reason wrappers were made
#ifndef HLSDK10_BUILD
	#define RET_EntVars_bInDuck(ent) ent->v.bInDuck
	#define RET_EntVars_flDuckTime(ent) ent->v.flDuckTime
	#define RET_EntVars_fuser2(ent) ent->v.fuser2
#else
	#define RET_EntVars_bInDuck(ent) 0
	#define RET_EntVars_flDuckTime(ent) 0
	#define RET_EntVars_fuser2(ent) 0.0f
#endif

// Usage:
// if (!strncmp(var, HF_StrAndLen("test")))
#define HF_StrAndLen(str) \
	str, sizeof(str) - 1

// This is done in order to find out the length of string at the compile-time
// We subtract 1 from sizeof due of the null terminator
#define HF_DoesGameDirStartsWith(game) \
	helper_functions::does_gamedir_starts_with(game, sizeof(game) - 1)
#define HF_DoesMapNameStartsWith(map) \
	helper_functions::does_mapname_starts_with(map, sizeof(map) - 1)
#define HF_DoesGameDirMatch(game) \
	helper_functions::does_gamedir_match(game)
#define HF_DoesMapNameMatch(map) \
	helper_functions::does_mapname_match(map)

// Don't remove parentheses from this macro or we're doomed!
#define HF_IsEntityTrigger(classname) \
	(!strncmp(classname, "trigger_", 8) || !strcmp(classname, "func_ladder"))

namespace helper_functions
{
	// https://github.com/ValveSoftware/halflife/blob/master/common/parsemsg.cpp
	void BEGIN_READ(void *buf, int size);
	int READ_CHAR();
	int READ_BYTE();
	int READ_SHORT();
	int READ_WORD();
	int READ_LONG();
	float READ_FLOAT();
	char* READ_STRING();
	float READ_COORD();
	float READ_ANGLE();
	float READ_HIRESANGLE();

	// Please add type/flags determining functions (e.g. get_flags, get_movetype) to the end of the .cpp file!
	// Any other functions should be added before them for better readability!

	inline void rgb_to_float(float &r, float &g, float &b)
	{
		r = std::clamp(r / 255.0f, 0.0f, 1.0f);
		g = std::clamp(g / 255.0f, 0.0f, 1.0f);
		b = std::clamp(b / 255.0f, 0.0f, 1.0f);
	}

	inline void rgb_to_float(float *dest, unsigned r, unsigned g, unsigned b)
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

	inline void rgba_to_float(float *dest, unsigned r, unsigned g, unsigned b, unsigned a)
	{
		rgb_to_float(dest, r, g, b);
		dest[3] = std::clamp(a / 255.0f, 0.0f, 1.0f);
	}

	void Draw_FillRGBA(int x, int y, int width, int height, int r, int g, int b, int a);
	void Draw_FillRGBABlend(int x, int y, int width, int height, int r, int g, int b, int a);

	bool does_gamedir_starts_with(const char *game, size_t len);
	bool does_gamedir_match(const char *game);
	bool does_mapname_starts_with(const char *map, size_t len);
	bool does_mapname_match(const char *map);

	double ret_bxt_time();
	int ret_bxt_flags();
	void com_fixslashes(std::string &str); // https://github.com/ValveSoftware/halflife/blob/c7240b965743a53a29491dd49320c88eecf6257b/game_shared/bot/nav_file.cpp#L680
	void com_filebase(const char *in, char *out); // https://github.com/ValveSoftware/halflife/blob/c7240b965743a53a29491dd49320c88eecf6257b/cl_dll/hud.cpp#L537
	std::string swap_lib(const char* current_lib_path, std::string new_lib_path, const char *start);
	void crash_if_failed(std::string str);
	bool is_valid_index_and_edict(int index);
	float adjust_fov_for_widescreen(float fov, float def_aspect_ratio, float our_aspect_ratio);
	bool is_entity_give_infinite_health(const edict_t* ent);
	std::string get_renderfx(int renderfx);
	std::string get_rendermode(int rendermode);
	std::string get_effects(int effects);
	std::string get_flags(int flags);
	std::string get_movetype(int movetype);
	std::string get_solid(int solid);
	std::string get_monster_triggercondition(int m_iTriggerCondition);
	std::string get_monster_task(int iTask);
	std::string get_difficulty(int skill);
	std::string get_typedescription_fieldtype(int fieldType);
	std::string get_hitgroup(int iHitgroup);
	std::string get_spawnflags(int spawnflags, const char *classname);
	void split_console_print_to_chunks(std::string str, const unsigned int max_string_length);
	void split_console_print_to_chunks(std::string str);
	void convert_to_lowercase(const char *str);
	int build_number(const char *date);
	int build_number();
	bool is_entity_kz_start(const char *targetname);
	bool is_entity_kz_stop(const char *targetname);

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

	void disable_vsync();
	void update_data_at_shutdown_sv(); // Update custom data when disconnected from the server, this should be called in SV_SpawnServer (engine-side) or ServerDeactivate (server-side).

	// https://developer.valvesoftware.com/wiki/SteamID
	std::string get_steam_id(const unsigned long steamID32);
	steamid_t get_steam_id_64(const unsigned long steamID32);

	// https://github.com/ValveSoftware/halflife/blob/c7240b965743a53a29491dd49320c88eecf6257b/dlls/cbase.cpp#L706
	int IsInWorld(Vector origin, Vector velocity, int map_size, int map_max_velocity);
	int IsInWorld(const edict_t *ent, int map_size, int map_max_velocity);

	bool IsPlayer(const edict_t *ent);

	// https://github.com/ValveSoftware/halflife/blob/c7240b965743a53a29491dd49320c88eecf6257b/dlls/player.cpp#L1262-L1266
	bool IsOnLadder(int movetype);
	bool IsOnLadder(const edict_t *ent);
	bool IsOnLadder(const edict_t *ent, bool is_player);

	bool IsNoClipping(int movetype);
	bool IsNoClipping(const edict_t *ent);
	bool IsNoClipping(const edict_t *ent, bool is_player);

	// https://github.com/ValveSoftware/halflife/blob/c7240b965743a53a29491dd49320c88eecf6257b/dlls/cbase.h#L197-L198
	bool IsBSPModel(int solid, int movetype);
	bool IsBSPModel(const edict_t *ent);
	bool ReflectGauss(int solid, int movetype, float takedamage);
	bool ReflectGauss(const edict_t *ent);

	// https://github.com/ValveSoftware/halflife/blob/c7240b965743a53a29491dd49320c88eecf6257b/dlls/cbase.h#L336-L337
	Vector Center(Vector absmin, Vector absmax);
	Vector Center(const edict_t *ent);
	Vector EyePosition(Vector origin, Vector view_ofs);
	Vector EyePosition(const edict_t *ent);

	// https://github.com/ValveSoftware/halflife/blob/c7240b965743a53a29491dd49320c88eecf6257b/dlls/cbase.cpp#L673
	bool Intersects(Vector absmin1, Vector absmax1, Vector absmin2, Vector absmax2);
	bool Intersects(const edict_t *ent1, const edict_t *ent2);
}