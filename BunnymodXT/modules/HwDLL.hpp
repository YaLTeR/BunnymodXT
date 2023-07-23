#pragma once

#include "../sptlib-wrapper.hpp"
#include <SPTLib/IHookableNameFilterOrdered.hpp>
#include "../cvars.hpp"
#include "taslogger/writer.hpp"
#include "../input_editor.hpp"

enum class TASEditorMode {
	DISABLED,
	EDIT
};

struct client_t;

class HwDLL : public IHookableNameFilterOrdered
{
	HOOK_DECL(void, __cdecl, LoadAndDecryptHwDLL, int a, void* b, void* c)
	HOOK_DECL(void, __cdecl, Cbuf_Execute)
	HOOK_DECL(void, __cdecl, Cbuf_AddText, const char* text)
	HOOK_DECL(void, __cdecl, Cbuf_InsertTextLines, const char* text)
	HOOK_DECL(void, __cdecl, Cmd_TokenizeString, char* text)
	HOOK_DECL(void, __cdecl, SeedRandomNumberGenerator)
	HOOK_DECL(time_t, __cdecl, time, time_t *Time)
	HOOK_DECL(long double, __cdecl, RandomFloat, float a1, float a2)
	HOOK_DECL(long, __cdecl, RandomLong, long low, long high)
	HOOK_DECL(void, __cdecl, Host_Changelevel2_f)
	HOOK_DECL(void, __cdecl, SCR_BeginLoadingPlaque)
	HOOK_DECL(int, __cdecl, Host_FilterTime, float timePassed)
	HOOK_DECL(int, __cdecl, V_FadeAlpha)
	HOOK_DECL(void, __cdecl, V_ApplyShake, float *origin, float *angles, float factor)
	HOOK_DECL(void, __cdecl, R_DrawSkyBox)
	HOOK_DECL(void, __cdecl, SCR_UpdateScreen)
	HOOK_DECL(void, __cdecl, SV_Frame)
	HOOK_DECL(int, __cdecl, SV_SpawnServer, int bIsDemo, char* server, char* startspot)
	HOOK_DECL(void, __cdecl, CL_Stop_f)
	HOOK_DECL(void, __cdecl, Host_Loadgame_f)
	HOOK_DECL(void, __cdecl, Host_Reload_f)
	HOOK_DECL(void, __cdecl, VGuiWrap2_ConDPrintf, const char* msg)
	HOOK_DECL(void, __cdecl, VGuiWrap2_ConPrintf, const char* msg)
	HOOK_DECL(void, __cdecl, CL_Record_f)
	HOOK_DECL(void, __cdecl, Key_Event, int key, int down)
	HOOK_DECL(void, __cdecl, Cmd_Exec_f)
	HOOK_DECL(void, __cdecl, R_DrawSequentialPoly, msurface_t *surf, int face)
	HOOK_DECL(void, __cdecl, R_Clear)
	HOOK_DECL(void, __cdecl, R_DrawViewModel)
	HOOK_DECL(void, __cdecl, R_PreDrawViewModel)
	HOOK_DECL(byte *, __cdecl, Mod_LeafPVS, mleaf_t *leaf, model_t *model)
	HOOK_DECL(void, __cdecl, SV_AddLinksToPM_, void *node, float *pmove_mins, float *pmove_maxs)
	HOOK_DECL(void, __cdecl, SV_WriteEntitiesToClient, client_t* client, void* msg)
	HOOK_DECL(void, __cdecl, VGuiWrap_Paint, int paintAll)
	HOOK_DECL(int, __cdecl, DispatchDirectUserMsg, char* pszName, int iSize, void* pBuf)
	HOOK_DECL(void, __cdecl, SV_SetMoveVars)
	HOOK_DECL(void, __cdecl, VectorTransform, float *in1, float *in2, float *out)
	HOOK_DECL(void, __cdecl, R_StudioCalcAttachments)
	HOOK_DECL(void, __cdecl, EmitWaterPolys, msurface_t *fa, int direction)
	HOOK_DECL(void, __cdecl, S_StartDynamicSound, int entnum, int entchannel, void *sfx, vec_t *origin,
	                                              float fvol, float attenuation, int flags, int pitch)
	HOOK_DECL(void, __cdecl, VGuiWrap2_NotifyOfServerConnect, const char *game, int IP, int port)
	HOOK_DECL(void, __cdecl, R_StudioSetupBones)
	HOOK_DECL(void, __cdecl, MD5Init, MD5Context_t *context)
	HOOK_DECL(void, __cdecl, MD5Update, MD5Context_t *context, unsigned char const *buf, unsigned int len)
	HOOK_DECL(void, __cdecl, MD5Final, unsigned char digest[16], MD5Context_t *context)
	HOOK_DECL(void, __cdecl, MD5Transform, unsigned int buf[4], unsigned int const in[16])
	HOOK_DECL(int, __cdecl, MD5_Hash_File, unsigned char digest[16], char *pszFileName, int bUsefopen, int bSeed, unsigned int seed[4])
	HOOK_DECL(char*, __cdecl, MD5_Print, unsigned char hash[16])
	HOOK_DECL(void, __fastcall, CBaseUI__HideGameUI, void *thisptr)
	HOOK_DECL(void, __cdecl, CBaseUI__HideGameUI_Linux, void *thisptr)
	HOOK_DECL(void, __cdecl, R_DrawWorld)
	HOOK_DECL(void, __cdecl, R_DrawParticles)
	HOOK_DECL(int, __cdecl, BUsesSDLInput)
	HOOK_DECL(void, __cdecl, R_StudioRenderModel)
	HOOK_DECL(void, __cdecl, R_SetFrustum)
	HOOK_DECL(void, __cdecl, SPR_Set, HSPRITE_HL hSprite, int r, int g, int b)
	HOOK_DECL(void, __cdecl, DrawCrosshair, int x, int y)
	HOOK_DECL(void, __cdecl, Draw_FillRGBA, int x, int y, int w, int h, int r, int g, int b, int a)
	HOOK_DECL(void, __cdecl, PF_traceline_DLL, const Vector* v1, const Vector* v2, int fNoMonsters, edict_t* pentToSkip, TraceResult* ptr)
	HOOK_DECL(qboolean, __cdecl, CL_CheckGameDirectory, char *gamedir)
	HOOK_DECL(int, __cdecl, Host_ValidSave)
	HOOK_DECL(int, __cdecl, SaveGameSlot, const char* pSaveName, const char* pSaveComment)
	HOOK_DECL(void, __cdecl, SCR_NetGraph)
	HOOK_DECL(void, __cdecl, Host_Shutdown)
	HOOK_DECL(void, __cdecl, ReleaseEntityDlls)
	HOOK_DECL(qboolean, __cdecl, ValidStuffText, char* buf)
	HOOK_DECL(qboolean, __cdecl, CL_ReadDemoMessage_OLD)

	struct cmdbuf_t
	{
		char *name;
		unsigned flags;
		char *data;
		unsigned maxsize;
		unsigned cursize;
	};

	struct svs_t
	{
		char unk[4];
		client_t *clients;
		int num_clients;
	};

	struct Key
	{
		Key(const char* name) : State(0), Name(name) {};
		void Down() { State |= (1 + 2); }
		void Up() { State = 4; }
		void ClearImpulses() { State &= ~(2 + 4); }
		bool IsDown() { return (State & 1); }
		double StateMultiplier()
		{
			bool impulsedown = ((State & 2) != 0);
			bool impulseup = ((State & 4) != 0);

			if (impulsedown)
			{
				if (impulseup)
					return 0.75;
				else
					return 0.5;
			}

			return 1;
		}

		int State;
		const std::string Name;
	};

	struct CmdFuncs
	{
		inline static void AddCommand(const char *name, void (*handler)())
		{
			HwDLL::GetInstance().ORIG_Cmd_AddMallocCommand(name, handler, 2);  // 2 - Cmd_AddGameCommand.
		}

		inline static const char *Argv(int i)
		{
			return HwDLL::GetInstance().ORIG_Cmd_Argv(i);
		}

		inline static int Argc()
		{
			return HwDLL::GetInstance().ORIG_Cmd_Argc();
		}

		inline static void UsagePrint(const char *s)
		{
			return HwDLL::GetInstance().ORIG_Con_Printf("%s", s);
		}
	};

public:
	static HwDLL& GetInstance()
	{
		static HwDLL instance;
		return instance;
	}

	virtual void Hook(const std::wstring& moduleName, void* moduleHandle, void* moduleBase, size_t moduleLength, bool needToIntercept);
	virtual void Unhook();
	virtual void Clear();

	void* GetTimeAddr();
	void SetTimeAddr(void* addr);

	void RegisterCVar(CVarWrapper& cvar);
	cvar_t* FindCVar(const char* name);
	void SetCVarValue(CVarWrapper& cvar, const char *value);

	void SetPlayerOrigin(float origin[3]);
	void SetPlayerVelocity(float velocity[3]);
	bool TryGettingAccurateInfo(float origin[3], float velocity[3], float& health, float& armorvalue, int& waterlevel, float& stamina);

	inline bool NeedViewmodelAdjustments()
	{
		auto desired_viewmodel_fov = CVars::bxt_viewmodel_fov.GetFloat();
		return (desired_viewmodel_fov > 0 && desired_viewmodel_fov < 179 && currentRenderFOV == CVars::default_fov.GetFloat());
	}

	inline bool GetIsOverridingCamera() const { return isOverridingCamera; }
	inline void GetCameraOverrideOrigin(float origin[3]) const
	{
		for (int i = 0; i < 3; ++i)
			origin[i] = cameraOverrideOrigin[i];
	}
	inline void GetCameraOverrideAngles(float angles[3]) const
	{
		for (int i = 0; i < 3; ++i)
			angles[i] = cameraOverrideAngles[i];
	}

	inline bool GetIsOffsettingCamera() const { return isOffsettingCamera; }
	inline void GetCameraOffsetOrigin(float origin[3]) const
	{
		for (int i = 0; i < 3; ++i)
			origin[i] = cameraOffsetOrigin[i];
	}
	inline void GetCameraOffsetAngles(float angles[3]) const
	{
		for (int i = 0; i < 3; ++i)
			angles[i] = cameraOffsetAngles[i];
	}

	inline void SetLastRandomSeed(unsigned seed) { LastRandomSeed = seed; }
	inline bool IsCountingSharedRNGSeed() { return CountingSharedRNGSeed; }
	inline unsigned GetSharedRNGSeedCounter() { return SharedRNGSeedCounter; }

	inline bool IsPaused() { return (psv && *(reinterpret_cast<int*>(psv) + 1)); }
	inline bool IsActive() { return (psv && *reinterpret_cast<int*>(psv)); }

	inline bool IsRecordingDemo() const { return demorecording && *demorecording == 1; }
	void StoreCommand(const char* command);

	inline double GetTime() const {
		return *reinterpret_cast<double *>(reinterpret_cast<uintptr_t>(psv) + offTime);
	}
	inline edict_t* GetPlayerEdict() const {
		if (!svs || svs->num_clients == 0)
			return nullptr;

		return *reinterpret_cast<edict_t**>(reinterpret_cast<uintptr_t>(svs->clients) + offEdict);
	}
	inline bool IsTASLogging() const { return tasLogging; }
	inline size_t GetPreExecFramebulk() const { return preExecFramebulk; }

	inline int GetEdicts(edict_t **edicts) const {
		*edicts = *reinterpret_cast<edict_t **>(reinterpret_cast<uintptr_t>(psv) + offEdicts);
		return *reinterpret_cast<int *>(reinterpret_cast<uintptr_t>(psv) + offNumEdicts);
	}
	inline model_t *GetModelByIndex(int index) const {
		model_t **models = reinterpret_cast<model_t **>(reinterpret_cast<uintptr_t>(psv) + offModels);
		return *(models + index);
	}
	inline bool IsValidEdict(const edict_t *edict) const {
		return edict && !edict->free;
	}

	inline float GetFrameTime() const {
		// This is how it's done inside the game.
		return static_cast<float>(static_cast<float>(std::floor(*host_frametime * 1000)) * 0.001);
	}

	HLStrafe::TraceResult PlayerTrace(const float start[3], const float end[3], HLStrafe::HullType hull, bool extendDistanceLimit = false);
	HLStrafe::TraceResult CameraTrace(float max_distance = 8192);

	// Don't call StartTrace() or StopTracing() twice in a row.
	// The sequence must always be StartTracing() => StopTracing().
	void StartTracing(bool extendDistanceLimit = false);
	void StopTracing();
	// You must call StartTracting() before calling this and StopTracing() after calling this.
	HLStrafe::TraceResult UnsafePlayerTrace(const float start[3], const float end[3], HLStrafe::HullType hull);

	unsigned QueuedSharedRNGSeeds;

	double *frametime_remainder;
	TASLogger::LogWriter logWriter;

	typedef void(__cdecl *_Cbuf_InsertText) (const char* text);
	_Cbuf_InsertText ORIG_Cbuf_InsertText;

	TASEditorMode tas_editor_mode;
	EditedInput tas_editor_input;
	bool tas_editor_delete_point;
	bool tas_editor_insert_point;
	bool tas_editor_insert_point_held;
	bool tas_editor_toggle_s00;
	bool tas_editor_toggle_s01;
	bool tas_editor_toggle_s03;
	bool tas_editor_toggle_s10;
	bool tas_editor_toggle_s11;
	bool tas_editor_toggle_s13;
	bool tas_editor_toggle_s22;
	bool tas_editor_toggle_s06;
	bool tas_editor_toggle_s07;
	bool tas_editor_toggle_lgagst;
	bool tas_editor_toggle_autojump;
	bool tas_editor_toggle_ducktap;
	bool tas_editor_toggle_jumpbug;
	bool tas_editor_toggle_dbc;
	bool tas_editor_toggle_dbc_ceilings;
	bool tas_editor_toggle_dbg;
	bool tas_editor_toggle_dwj;
	bool tas_editor_toggle_forward;
	bool tas_editor_toggle_left;
	bool tas_editor_toggle_right;
	bool tas_editor_toggle_back;
	bool tas_editor_toggle_up;
	bool tas_editor_toggle_down;
	bool tas_editor_toggle_jump;
	bool tas_editor_toggle_duck;
	bool tas_editor_toggle_use;
	bool tas_editor_toggle_attack1;
	bool tas_editor_toggle_attack2;
	bool tas_editor_toggle_reload;
	bool tas_editor_set_frametime;
	std::string tas_editor_set_frametime_time;
	bool tas_editor_set_change_to_target_yaw;
	bool tas_editor_set_change_to_target_yaw_offset;
	bool tas_editor_set_change_to_yaw;
	bool tas_editor_set_change_to_pitch;
	bool tas_editor_set_run_point_and_save;
	bool tas_editor_set_target_yaw_velocity_lock;
	bool tas_editor_set_target_yaw_look_at;
	unsigned int tas_editor_set_target_yaw_look_at_entity;
	float tas_editor_set_target_yaw_look_at_x;
	float tas_editor_set_target_yaw_look_at_y;
	float tas_editor_set_target_yaw_look_at_z;
	bool tas_editor_set_yaw;
	float tas_editor_set_yaw_yaw;
	bool tas_editor_set_pitch;
	float tas_editor_set_pitch_pitch;
	bool tas_editor_set_repeats;
	unsigned tas_editor_set_repeats_repeats;
	bool tas_editor_set_commands;
	std::string tas_editor_set_commands_commands;
	bool tas_editor_set_left_right_count;
	unsigned tas_editor_set_left_right_count_count;
	bool tas_editor_unset_yaw;
	bool tas_editor_unset_pitch;
	bool tas_editor_apply_smoothing;
	void SetTASEditorMode(TASEditorMode mode);
	void SaveEditedInput();

	bool free_cam_active;
	void SetFreeCam(bool enabled);
	void FreeCamTick();

	float currentRenderFOV = 0;

	bool insideDrawCrosshair = false;

	int lastRecordedHealth;

	globalvars_t *ppGlobals;
	engine_studio_api_t *pEngStudio;
	engine_api_t *pEngineAPI;

	inline const char* GetString(int string) const {
		assert(ppGlobals);
		return ppGlobals->pStringBase + string;
	}

	bool Called_Timer = false;

	bool is_cof_steam = false; // Cry of Fear-specific

	int CallOnTASPlaybackFrame();
	void CallOnTASPlaybackStopped();
	void ResetTASPlaybackState();
	void StartTASPlayback();

	void TimerReset();
	void TimerStart();

private:
	// Make sure to have hl.exe last here, so that it is the lowest priority.
	HwDLL() : IHookableNameFilterOrdered({ L"hw.dll", L"hw.so", L"sw.dll", L"hl.exe" }) {};
	HwDLL(const HwDLL&);
	void operator=(const HwDLL&);

public:
	typedef cmd_function_t*(__cdecl *_Cmd_FindCmd) (const char* cmd_name);
	_Cmd_FindCmd ORIG_Cmd_FindCmd;
	typedef void(__cdecl *_Con_Printf) (const char* fmt, ...);
	_Con_Printf ORIG_Con_Printf;
	typedef cvar_t*(__cdecl *_Cvar_FindVar) (const char* name);
	_Cvar_FindVar ORIG_Cvar_FindVar;
	typedef int(__cdecl *_build_number)();
	_build_number ORIG_build_number;
	typedef void(__cdecl* _Host_Notarget_f) ();
	_Host_Notarget_f ORIG_Host_Notarget_f;
	typedef void(__cdecl* _Host_Noclip_f) ();
	_Host_Noclip_f ORIG_Host_Noclip_f;

	HLStrafe::PlayerData GetPlayerData();

protected:
	typedef void(__cdecl *_Cvar_RegisterVariable) (cvar_t* cvar);
	_Cvar_RegisterVariable ORIG_Cvar_RegisterVariable;
	typedef void(__cdecl *_Cvar_DirectSet) (cvar_t* cvar, const char* value);
	_Cvar_DirectSet ORIG_Cvar_DirectSet;
	typedef void(__cdecl *_Cmd_AddMallocCommand) (const char* name, void(*func)(void), int flags);
	_Cmd_AddMallocCommand ORIG_Cmd_AddMallocCommand;
	typedef int(__cdecl *_Cmd_Argc) ();
	_Cmd_Argc ORIG_Cmd_Argc;
	typedef char*(__cdecl *_Cmd_Args) ();
	_Cmd_Args ORIG_Cmd_Args;
	typedef char*(__cdecl *_Cmd_Argv) (unsigned n);
	_Cmd_Argv ORIG_Cmd_Argv;
	typedef pmtrace_t(__cdecl *_PM_PlayerTrace) (const float* start, const float* end, int traceFlags, int ignore_pe);
	_PM_PlayerTrace ORIG_PM_PlayerTrace;
	typedef void(__cdecl *_SV_AddLinksToPM) (char* node, float* origin);
	_SV_AddLinksToPM ORIG_SV_AddLinksToPM;
	typedef char*(__cdecl *_PF_GetPhysicsKeyValue) (const edict_t* pClient, const char* key);
	_PF_GetPhysicsKeyValue ORIG_PF_GetPhysicsKeyValue;
	typedef void(__cdecl *_CL_RecordHUDCommand) (const char* cmdname);
	_CL_RecordHUDCommand ORIG_CL_RecordHUDCommand;
	typedef void(__cdecl *_CL_HudMessage) (const char *pMessage);
	_CL_HudMessage ORIG_CL_HudMessage;
	typedef int(__cdecl *_VGuiWrap2_IsGameUIVisible) ();
	_VGuiWrap2_IsGameUIVisible ORIG_VGuiWrap2_IsGameUIVisible;
	typedef void(__cdecl *_SCR_DrawPause) ();
	_SCR_DrawPause ORIG_SCR_DrawPause;

	void FindStuff();

	struct Cmd_BXT_TAS_LoadScript;
	struct Cmd_BXT_TAS_ExportScript;
	struct Cmd_BXT_TAS_ExportLibTASInput;
	struct Cmd_BXT_TAS_Split;
	struct Cmd_BXT_TAS_New;
	struct Cmd_BXT_TAS_Check_Position;
	struct Cmd_BXT_CH_Set_Health;
	struct Cmd_BXT_Set_Angles;
	struct Cmd_BXT_CH_Set_Armor;
	struct Cmd_BXT_CH_Set_Origin;
	struct Cmd_BXT_CH_Set_Origin_Offset;
	struct Cmd_BXT_CH_Set_Velocity;
	struct Cmd_BXT_CH_Set_Velocity_Angles;
	struct Cmd_BXT_CH_Teleport_To_Entity;
	struct Cmd_BXT_CH_Get_Velocity;
	struct Cmd_BXT_CH_Get_Other_Player_Info;
	struct Cmd_BXT_CH_Entity_Set_Health;
	struct Cmd_BXT_CH_Monster_Set_Origin;
	struct Cmd_BXT_Get_Origin_And_Angles;
	struct Cmd_BXT_Get_Server_Time;
	struct Cmd_Multiwait;
	struct Cmd_BXT_Camera_Fixed;
	struct Cmd_BXT_Camera_Clear;
	struct Cmd_BXT_Camera_Offset;
	struct Cmd_BXT_Timer_Start;
	struct Cmd_BXT_Timer_Stop;
	struct Cmd_BXT_Timer_Reset;
	struct Cmd_BXT_TAS_Autojump_Down;
	struct Cmd_BXT_TAS_Autojump_Up;
	struct Cmd_BXT_TAS_Ducktap_Down;
	struct Cmd_BXT_TAS_Ducktap_Up;
	struct Cmd_BXT_TAS_Jumpbug_Down;
	struct Cmd_BXT_TAS_Jumpbug_Up;
	struct Cmd_BXT_Triggers_Add;
	struct Cmd_BXT_Triggers_Clear;
	struct Cmd_BXT_Triggers_Delete;
	struct Cmd_BXT_Triggers_Export;
	struct Cmd_BXT_Triggers_List;
	struct Cmd_BXT_Triggers_SetCommand;
	struct Cmd_BXT_Triggers_Place_Up;
	struct Cmd_BXT_Triggers_Place_Down;
	struct Cmd_BXT_Record;
	struct Cmd_BXT_AutoRecord;
	struct Cmd_BXT_Interprocess_Reset;
	struct Cmd_BXT_Interprocess_Stop;
	struct Cmd_BXT_Map;
	struct Cmd_BXT_Load;
	struct Cmd_BXT_Set_Frametime_Remainder;
	struct Cmd_BXT_Reset_Frametime_Remainder;
	struct Cmd_BXT_TASLog;
	struct Cmd_BXT_Append;
	struct Cmd_BXT_TAS_Editor;
	struct Cmd_Plus_BXT_TAS_Editor_Look_Around;
	struct Cmd_Minus_BXT_TAS_Editor_Look_Around;
	struct Cmd_BXT_TAS_Editor_Save;
	struct Cmd_BXT_TAS_Editor_Delete_Last_Point;
	struct Cmd_BXT_TAS_Editor_Delete_Point;
	struct Cmd_BXT_TAS_Editor_Insert_Point;
	struct Cmd_Plus_BXT_TAS_Editor_Insert_Point;
	struct Cmd_Minus_BXT_TAS_Editor_Insert_Point;
	struct Cmd_BXT_TAS_Editor_Resimulate;
	struct Cmd_BXT_TAS_Editor_Toggle;
	struct Cmd_BXT_TAS_Editor_Set_Frametime;
	struct Cmd_BXT_TAS_Editor_Set_Change_Type;
	struct Cmd_BXT_TAS_Editor_Set_Target_Yaw_Type;
	struct Cmd_BXT_TAS_Editor_Set_Run_Point_And_Save;
	struct Cmd_BXT_TAS_Editor_Set_Yaw;
	struct Cmd_BXT_TAS_Editor_Set_Pitch;
	struct Cmd_BXT_TAS_Editor_Set_Repeats;
	struct Cmd_BXT_TAS_Editor_Set_Commands;
	struct Cmd_BXT_TAS_Editor_Set_Left_Right_Count;
	struct Cmd_BXT_TAS_Editor_Unset_Yaw;
	struct Cmd_BXT_TAS_Editor_Unset_Pitch;
	struct Cmd_BXT_TAS_Editor_Apply_Smoothing;
	struct Cmd_BXT_TAS_Optim_Init;
	struct Cmd_BXT_FreeCam;
	struct Cmd_BXT_Print_Entities;
	struct Cmd_BXT_Print_Entities_By_Index;
	struct Cmd_BXT_TAS_Become_Simulator_Client;
	struct Cmd_BXT_TAS_Server_Send_Command;
	struct Cmd_BXT_TAS_Client_Load_Received_Script;
	struct Cmd_BXT_Show_Bullets_Clear;
	struct Cmd_BXT_Show_Bullets_Enemy_Clear;
	struct Cmd_BXT_Split;
	struct Cmd_BXT_Splits_Add_Entity;
	struct Cmd_BXT_Splits_Add_Trigger;
	struct Cmd_BXT_Splits_Clear;
	struct Cmd_BXT_Splits_Delete;
	struct Cmd_BXT_Splits_Export;
	struct Cmd_BXT_Splits_List;
	struct Cmd_BXT_Splits_Print_Times;
	struct Cmd_BXT_Splits_Set_Map;
	struct Cmd_BXT_Splits_Set_Name;
	struct Cmd_BXT_Splits_Track_Horizontal_Speed;
	struct Cmd_BXT_Splits_Track_Vertical_Speed;
	struct Cmd_BXT_Splits_Track_X;
	struct Cmd_BXT_Splits_Track_Y;
	struct Cmd_BXT_Splits_Track_Z;
	struct Cmd_BXT_Splits_Place_Up;
	struct Cmd_BXT_Splits_Place_Down;

	void RegisterCVarsAndCommandsIfNeeded();
	void InsertCommands();
	bool GetNextMovementFrame(HLTAS::Frame& f);
	void ResetButtons();
	void FindCVarsIfNeeded();
	void PrintEntity(std::ostringstream &out, int index);
	void TeleportMonsterToPosition(float x, float y, float z, int index);
public:
	HLStrafe::MovementVars GetMovementVars();
	const char* GetMovetypeName(int moveType);
	void GetOriginOfEntity(Vector& origin, const edict_t* ent);

	bool ducktap;
	edict_t **sv_player;
	qboolean *noclip_anglehack;
protected:
	void KeyDown(Key& btn);
	void KeyUp(Key& btn);
	void SaveInitialDataToDemo();
	void UpdateCustomTriggersAndSplits();

	bool registeredVarsAndCmds;

	bool autojump;
	bool jumpbug;

	std::string recordDemoName;
	std::string autoRecordDemoName;
	size_t autoRecordDemoNumber;
	bool autoRecordNow;

	bool insideHost_Loadgame_f;
	bool insideHost_Reload_f;

	void *pcl;
	void *cls;
	void *psv;
	ptrdiff_t offTime;
	ptrdiff_t offWorldmodel;
	ptrdiff_t offModels;
	ptrdiff_t offNumEdicts;
	ptrdiff_t offMaxEdicts;
	ptrdiff_t offEdicts;
	svs_t *svs;
	ptrdiff_t offEdict;
	void *svmove;
	void **ppmove;
	client_t **host_client;
	char *sv_areanodes;
	cmdbuf_t *cmd_text;
	double *host_frametime;
	int *demorecording;
	cmdalias_t* cmd_alias;
	cvar_t **cvar_vars;
	movevars_t *movevars;
	studiohdr_t **pstudiohdr;
	float *scr_fov_value;
	ptrdiff_t pHost_FilterTime_FPS_Cap_Byte;
	qboolean *cofSaveHack; // Cry of Fear-specific

	int framesTillExecuting;
	bool executing;
	bool insideCbuf_Execute;
	bool finishingLoad;
	bool dontPauseNextCycle;
	bool changelevel;
	bool recording;
	bool pauseOnTheFirstFrame;

	bool tasLogging;
	std::string loggedCbuf;
	FILE *tasLogFile = nullptr;
	void SetTASLogging(bool enabled);

public:
	bool isOverridingCamera = false;
	Vector cameraOverrideOrigin;
	float cameraOverrideAngles[3];
protected:
	bool isOffsettingCamera = false;
	float cameraOffsetOrigin[3];
	float cameraOffsetAngles[3];

	bool insideSeedRNG;
	bool insideSStartDynamicSound;
	unsigned LastRandomSeed;

	client_t* trace_oldclient;
	edict_t* trace_oldplayer;
	void* trace_oldmove;

	HLStrafe::PlayerData player;
public:
	std::string hltas_filename;
	HLTAS::Input input;
	unsigned hlstrafe_version;
	size_t totalFrames;
protected:
	std::string demoName;
	std::string saveName;
public:
	std::string frametime0ms;
protected:
	bool runningFrames;
	bool wasRunningFrames;
	size_t currentFramebulk;
	size_t preExecFramebulk = 0;
	size_t totalFramebulks;
	size_t currentRepeat;
	size_t movementFrameCounter;
	bool thisFrameIs0ms;
	bool SharedRNGSeedPresent;
	unsigned SharedRNGSeed;
	bool CountingSharedRNGSeed;
	unsigned SharedRNGSeedCounter;
	unsigned LoadingSeedCounter;
	size_t TargetYawOverrideIndex;
	std::vector<float> TargetYawOverrides;
	float sensitivityToRestore = 0;
public:
	size_t RenderYawOverrideIndex;
	std::vector<float> RenderYawOverrides;
	bool ButtonsPresent;
	HLTAS::StrafeButtons Buttons;
	HLStrafe::CurrentState StrafeState;
	HLStrafe::CurrentState PrevStrafeState;
	std::array<float, 4> PrevFractions;
	std::array<float, 4> PrevNormalzs;
protected:
	std::string exportFilename;
	HLTAS::Input exportResult;
	std::string splitFilename;
	HLTAS::Input splitResult;
public:
	std::string newTASFilename;
	HLTAS::Input newTASResult;
	bool newTASIsForStudio;
protected:
	std::ofstream libTASExportFile;

public:
	struct KeyStates
	{
		KeyStates() :
			Forward("forward"),
			Left("moveleft"),
			Right("moveright"),
			Back("back"),
			Up("moveup"),
			Down("movedown"),
			CamLeft("left"),
			CamRight("right"),
			CamUp("lookup"),
			CamDown("lookdown"),
			Jump("jump"),
			Duck("duck"),
			Use("use"),
			Attack1("attack"),
			Attack2("attack2"),
			Reload("reload") {}

		void ResetStates()
		{
			Forward.State = 0;
			Left.State = 0;
			Right.State = 0;
			Back.State = 0;
			Up.State = 0;
			Down.State = 0;

			CamLeft.State = 0;
			CamRight.State = 0;
			CamUp.State = 0;
			CamDown.State = 0;

			Jump.State = 0;
			Duck.State = 0;
			Use.State = 0;
			Attack1.State = 0;
			Attack2.State = 0;
			Reload.State = 0;
		}

		Key Forward;
		Key Left;
		Key Right;
		Key Back;
		Key Up;
		Key Down;

		Key CamLeft;
		Key CamRight;
		Key CamUp;
		Key CamDown;

		Key Jump;
		Key Duck;
		Key Use;
		Key Attack1;
		Key Attack2;
		Key Reload;
	} currentKeys;
protected:

	bool clearedImpulsesForTheFirstTime;

	// Do not clear these inside Clear().
	bool SetNonSharedRNGSeed = false;
	std::time_t NonSharedRNGSeed;

	enum class ResetState {
		NORMAL = 0,
		PRERESET,
		POSTRESET
	} resetState = ResetState::NORMAL;

public:
	std::string lastLoadedMap;

protected:
	bool insideKeyEvent;
	bool insideExec;
	std::string execScript;
	bool insideHost_Changelevel2_f;
	bool dontStopAutorecord;
	bool insideRStudioCalcAttachmentsViewmodel;
	bool insideHideGameUI;

	bool extendPlayerTraceDistanceLimit;

	bool insideCL_ReadDemoMessage;
	std::vector<char> runtimeDataBuffer;
};
