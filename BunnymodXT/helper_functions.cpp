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