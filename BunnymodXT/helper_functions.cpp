#include "stdafx.hpp"
#include "modules.hpp"
#include "helper_functions.hpp"

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

	int READ_SHARED(int type, int size)
	{
		int c;

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
		return READ_SHARED(0, 1);
	}

	int READ_BYTE()
	{
		return READ_SHARED(1, 1);
	}

	int READ_SHORT()
	{
		return READ_SHARED(2, 2);
	}

	int READ_WORD()
	{
		return READ_SHORT();
	}

	int READ_LONG()
	{
		return READ_SHARED(3, 4);
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
		int l, c;

		string[0] = 0;
		l = 0;

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
		return (CustomHud::GetTime().hours * 60 * 60) + (CustomHud::GetTime().minutes * 60) + CustomHud::GetTime().seconds + (CustomHud::GetTime().milliseconds / 1000);
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

	bool is_valid_index_and_edict(int index)
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

	float adjust_fov_for_widescreen(float fov, float def_aspect_ratio, float our_aspect_ratio)
	{
		return static_cast<float>(std::atan(std::tan(fov * M_PI / 360.0f) * def_aspect_ratio * our_aspect_ratio) * 360.0f / M_PI);
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

	void split_console_print_to_chunks(std::string str, const unsigned int max_string_length)
	{
		size_t string_length = str.size();
		// We must round the total number of chunks to the next value if it is not an integer in order for the remaining characters to be drawed.
		size_t count_chunks = static_cast<size_t>(std::ceil(static_cast<double>(string_length) / max_string_length));

		for (size_t chunk = 0; chunk < count_chunks; chunk++)
		{
			std::string result = str.substr(chunk * max_string_length, max_string_length);

			// Don't start a new chunk on a newline, because we don't want to break the sequence of text!
			// Instead, the newlines must be specified in the text itself!
			HwDLL::GetInstance().ORIG_Con_Printf("%s", result.c_str());
		}
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

		return true;
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

	// Below this comment are only functions for determining type or flags!

	std::string get_renderfx(int renderfx)
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

	std::string get_rendermode(int rendermode)
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

	std::string get_effects(int effects)
	{
		std::ostringstream out;

		#define FLAG(fl) \
		if (effects & fl) \
			out << "" #fl "; ";

		// The flags here were arranged in order from smallest to highest bits.
		FLAG(EF_BRIGHTFIELD);
		FLAG(EF_MUZZLEFLASH);
		FLAG(EF_BRIGHTLIGHT);
		FLAG(EF_DIMLIGHT);
		FLAG(EF_INVLIGHT);
		FLAG(EF_NOINTERP);
		FLAG(EF_LIGHT);
		FLAG(EF_NODRAW);

		out << '\n';

		#undef FLAG

		return out.str();
	}

	std::string get_solid(int solid)
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

	std::string get_movetype(int movetype)
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

	std::string get_flags(int flags)
	{
		std::ostringstream out;

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

	std::string get_monster_triggercondition(int m_iTriggerCondition)
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

	std::string get_monster_task(int iTask)
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