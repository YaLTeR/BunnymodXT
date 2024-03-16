#include "stdafx.hpp"
#include "modules.hpp"
#include "helper_functions.hpp"

namespace helper_functions
{
	void com_fixslashes(std::string &str)
	{
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

	float adjust_fov_for_widescreen(float fov, float def_aspect_ratio, float our_aspect_ratio)
	{
		return static_cast<float>(std::atan(std::tan(fov * M_PI / 360.0f) * def_aspect_ratio * our_aspect_ratio) * 360.0f / M_PI);
	}

	std::string get_movetype(int movetype)
	{
		switch (movetype)
		{
			case MOVETYPE_NONE:             return "MOVETYPE_NONE";
			case MOVETYPE_WALK:             return "MOVETYPE_WALK";
			case MOVETYPE_STEP:             return "MOVETYPE_STEP";
			case MOVETYPE_FLY:              return "MOVETYPE_FLY";
			case MOVETYPE_TOSS:             return "MOVETYPE_TOSS";
			case MOVETYPE_PUSH:             return "MOVETYPE_PUSH";
			case MOVETYPE_NOCLIP:           return "MOVETYPE_NOCLIP";
			case MOVETYPE_FLYMISSILE:       return "MOVETYPE_FLYMISSILE";
			case MOVETYPE_BOUNCE:           return "MOVETYPE_BOUNCE";
			case MOVETYPE_BOUNCEMISSILE:    return "MOVETYPE_BOUNCEMISSILE";
			case MOVETYPE_FOLLOW:           return "MOVETYPE_FOLLOW";
			case MOVETYPE_PUSHSTEP:         return "MOVETYPE_PUSHSTEP";
			default:                        return "Unknown";
		}
	}

	std::string get_flags(int flags)
	{
		std::ostringstream out;
		out << "Flags: ";

		#define FLAG(fl) \
		if (flags & fl) \
			out << "" #fl "; ";

		// The flags here were arranged in order from smallest to highest bits.
		FLAG(FL_FLY);
		FLAG(FL_SWIM);
		FLAG(FL_CONVEYOR);
		FLAG(FL_CLIENT);
		FLAG(FL_INWATER);
		FLAG(FL_MONSTER);
		FLAG(FL_GODMODE);
		FLAG(FL_NOTARGET);
		FLAG(FL_SKIPLOCALHOST);
		FLAG(FL_ONGROUND);
		FLAG(FL_PARTIALGROUND);
		FLAG(FL_WATERJUMP);
		FLAG(FL_FROZEN);
		FLAG(FL_FAKECLIENT);
		FLAG(FL_DUCKING);
		FLAG(FL_FLOAT);
		FLAG(FL_GRAPHED);
		FLAG(FL_IMMUNE_WATER);
		FLAG(FL_IMMUNE_SLIME);
		FLAG(FL_IMMUNE_LAVA);
		FLAG(FL_PROXY);
		FLAG(FL_ALWAYSTHINK);
		FLAG(FL_BASEVELOCITY);
		FLAG(FL_MONSTERCLIP);
		FLAG(FL_ONTRAIN);
		FLAG(FL_WORLDBRUSH);
		FLAG(FL_SPECTATOR);
		// unknown
		// unknown
		FLAG(FL_CUSTOMENTITY);
		FLAG(FL_KILLME);
		FLAG(FL_DORMANT);

		out << '\n';

		#undef FLAG

		return out.str();
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
		if ((!strncmp(classname, "func_door", 9)) // https://github.com/ValveSoftware/halflife/blob/c7240b965743a53a29491dd49320c88eecf6257b/dlls/doors.cpp#L712
		|| (!strcmp(classname, "func_rotating")) // https://github.com/ValveSoftware/halflife/blob/c7240b965743a53a29491dd49320c88eecf6257b/dlls/bmodels.cpp#L716
		|| (!strcmp(classname, "func_train"))) // https://github.com/ValveSoftware/halflife/blob/c7240b965743a53a29491dd49320c88eecf6257b/dlls/plats.cpp#L683
		{
			if (ent->v.dmg < 0.0f)
				return true;
		}

		return false;
	}

	int IsInWorld(Vector origin, Vector velocity, int map_size, int map_max_velocity)
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

	int IsInWorld(const edict_t *ent, int map_size, int map_max_velocity)
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
	}

	bool IsBSPModel(int solid, int movetype)
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

	bool ReflectGauss(int solid, int movetype, float takedamage)
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

	Vector Center(Vector absmin, Vector absmax)
	{
		return (absmin + absmax) * 0.5;
	}

	Vector Center(const edict_t *ent)
	{
		return Center(ent->v.absmin, ent->v.absmax);
	}

	Vector EyePosition(Vector origin, Vector view_ofs)
	{
		return origin + view_ofs;
	}

	Vector EyePosition(const edict_t *ent)
	{
		return EyePosition(ent->v.origin, ent->v.view_ofs);
	}

	bool Intersects(Vector absmin1, Vector absmax1, Vector absmin2, Vector absmax2)
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
};