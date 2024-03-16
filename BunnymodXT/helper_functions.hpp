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
	void com_fixslashes(std::string &str); // https://github.com/ValveSoftware/halflife/blob/c7240b965743a53a29491dd49320c88eecf6257b/game_shared/bot/nav_file.cpp#L680
	std::string swap_lib(const char* current_lib_path, std::string new_lib_path, const char *start);
	void crash_if_failed(std::string str);
	float adjust_fov_for_widescreen(float fov, float def_aspect_ratio, float our_aspect_ratio);
	bool is_entity_give_infinite_health(const edict_t* ent)
	std::string get_flags(int flags);

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