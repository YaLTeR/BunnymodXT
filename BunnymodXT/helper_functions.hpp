#pragma once

#define RET_CASE_STR(c) case c: return "" #c "";

#ifdef _WIN32
	#define DLL_EXTENSION ".dll"
	#define PATH_SLASH '\\'
#else
	#define DLL_EXTENSION ".so"
	#define PATH_SLASH '/'
#endif

const steamid_t STEAMID64_CONST = 76561197960265728; // 0x110000100000000

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

	inline void rgba_to_float(float &r, float &g, float &b, float &a)
	{
		rgb_to_float(r, g, b);
		a = std::clamp(a / 255.0f, 0.0f, 1.0f);
	}

	double ret_bxt_time();
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
	void split_console_print_to_chunks(std::string str, const unsigned int max_string_length);
	void convert_to_lowercase(const char *str);

	// https://developer.valvesoftware.com/wiki/SteamID
	std::string get_steam_id(const unsigned long steamID32);
	steamid_t get_steam_id_64(const unsigned long steamID32);

	// https://github.com/ValveSoftware/halflife/blob/c7240b965743a53a29491dd49320c88eecf6257b/dlls/cbase.cpp#L706
	int IsInWorld(Vector origin, Vector velocity, int map_size, int map_max_velocity);
	int IsInWorld(const edict_t *ent, int map_size, int map_max_velocity);

	bool IsPlayer(const edict_t *ent);

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