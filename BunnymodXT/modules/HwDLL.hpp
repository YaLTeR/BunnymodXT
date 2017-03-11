#pragma once

#include "../sptlib-wrapper.hpp"
#include <SPTLib/IHookableNameFilterOrdered.hpp>
#include "../cvars.hpp"
#include "taslogger/writer.hpp"

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
	HOOK_DECL(long, __cdecl, RandomLong, long a1, long a2)
	HOOK_DECL(void, __cdecl, Host_Changelevel2_f)
	HOOK_DECL(void, __cdecl, SCR_BeginLoadingPlaque)
	HOOK_DECL(int, __cdecl, Host_FilterTime, float timePassed)
	HOOK_DECL(int, __cdecl, V_FadeAlpha)
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
	HOOK_DECL(byte *, __cdecl, Mod_LeafPVS, mleaf_t *leaf, model_t *model)

	struct cmdbuf_t
	{
		char *name;
		unsigned flags;
		char *data;
		unsigned maxsize;
		unsigned cursize;
	};

	struct cmdalias_t
	{
		cmdalias_t* next;
		char name[32];
		char* value;
	};

	struct client_t;
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

	void SetPlayerOrigin(float origin[3]);
	void SetPlayerVelocity(float velocity[3]);
	bool TryGettingAccurateInfo(float origin[3], float velocity[3], float& health);
	void GetViewangles(float* va);

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

	inline bool IsPaused() { return (sv && *(reinterpret_cast<int*>(sv) + 1)); }

	inline bool IsRecordingDemo() const { return demorecording && *demorecording == 1; }
	void StoreCommand(const char* command);

	inline double GetTime() const {
		return *reinterpret_cast<double *>(reinterpret_cast<uintptr_t>(sv) + offTime);
	}
	inline edict_t* GetPlayerEdict() const { return *sv_player; }
	inline bool IsTASLogging() const { return tasLogging; }
	inline size_t GetPreExecFramebulk() const { return preExecFramebulk; }

	inline int GetEdicts(edict_t **edicts) const {
		*edicts = *reinterpret_cast<edict_t **>(reinterpret_cast<uintptr_t>(sv) + offEdicts);
		return *reinterpret_cast<int *>(reinterpret_cast<uintptr_t>(sv) + offNumEdicts);
	}
	inline model_t *GetModelByIndex(int index) const {
		model_t **models = reinterpret_cast<model_t **>(reinterpret_cast<uintptr_t>(sv) + offModels);
		return *(models + index);
	}
	inline bool IsValidEdict(const edict_t *edict) const {
		return edict && !edict->free;
	}

	HLStrafe::TraceResult PlayerTrace(const float start[3], const float end[3], HLStrafe::HullType hull);
	void InitTracing();
	void DeinitTracing();
	HLStrafe::TraceResult PlayerTrace2(const float start[3], const float end[3], HLStrafe::HullType hull);

	// Don't call StartTrace() or StopTracing() twice in a row.
	// The sequence must always be StartTracing() => StopTracing().
	void StartTracing();
	void StopTracing();
	// You must call StartTracting() before calling this and StopTracing() after calling this.
	HLStrafe::TraceResult UnsafePlayerTrace(const float start[3], const float end[3], HLStrafe::HullType hull);

	FILE* log;

	struct PlayerState {
		float Origin[3];
		float Velocity[3];
		float Yaw;

		bool Ducking;
		bool InDuckAnimation;
		float DuckTime;

		bool WasPressingJump;
		bool WasPressingDuck;
	};

	static unsigned Heuristic(PlayerState player);
	void StartSearch();
	PlayerState GetCurrentState();

	unsigned QueuedSharedRNGSeeds;

	double *frametime_remainder;
	TASLogger::LogWriter logWriter;

	typedef void(__cdecl *_Cbuf_InsertText) (const char* text);
	_Cbuf_InsertText ORIG_Cbuf_InsertText;

private:
	// Make sure to have hl.exe last here, so that it is the lowest priority.
	HwDLL() : IHookableNameFilterOrdered({ L"hw.dll", L"hw.so", L"sw.dll", L"hl.exe" }) {};
	HwDLL(const HwDLL&);
	void operator=(const HwDLL&);

protected:
	typedef void(__cdecl *_Con_Printf) (const char* fmt, ...);
public:
	_Con_Printf ORIG_Con_Printf;
protected:
	typedef void(__cdecl *_Cvar_RegisterVariable) (cvar_t* cvar);
	_Cvar_RegisterVariable ORIG_Cvar_RegisterVariable;
	typedef void(__cdecl *_Cvar_DirectSet) (cvar_t* cvar, const char* value);
	_Cvar_DirectSet ORIG_Cvar_DirectSet;
	typedef cvar_t*(__cdecl *_Cvar_FindVar) (const char* name);
	_Cvar_FindVar ORIG_Cvar_FindVar;
	typedef void(__cdecl *_Cmd_AddMallocCommand) (const char* name, void(*func)(void), int flags);
	_Cmd_AddMallocCommand ORIG_Cmd_AddMallocCommand;
	typedef int(__cdecl *_Cmd_Argc) ();
	_Cmd_Argc ORIG_Cmd_Argc;
	typedef char*(__cdecl *_Cmd_Args) ();
	_Cmd_Args ORIG_Cmd_Args;
	typedef char*(__cdecl *_Cmd_Argv) (unsigned n);
	_Cmd_Argv ORIG_Cmd_Argv;
	typedef void(__cdecl *_hudGetViewAngles) (float* va);
	_hudGetViewAngles ORIG_hudGetViewAngles;
	typedef pmtrace_t(__cdecl *_PM_PlayerTrace) (const float* start, const float* end, int traceFlags, int ignore_pe);
	_PM_PlayerTrace ORIG_PM_PlayerTrace;
	typedef void(__cdecl *_SV_AddLinksToPM) (char* node, float* origin);
	_SV_AddLinksToPM ORIG_SV_AddLinksToPM;
	typedef char*(__cdecl *_PF_GetPhysicsKeyValue) (const edict_t* pClient, const char* key);
	_PF_GetPhysicsKeyValue ORIG_PF_GetPhysicsKeyValue;
	typedef void(__cdecl *_CL_RecordHUDCommand) (const char* cmdname);
	_CL_RecordHUDCommand ORIG_CL_RecordHUDCommand;
	typedef int(__cdecl *_build_number)();
	_build_number ORIG_build_number;

	void FindStuff();

	struct Cmd_BXT_TAS_LoadScript;
	struct Cmd_BXT_TAS_ExportScript;
	struct Cmd_BXT_CH_Set_Health;
	struct Cmd_BXT_CH_Set_Armor;
	struct Cmd_BXT_CH_Set_Origin;
	struct Cmd_BXT_CH_Set_Origin_Offset;
	struct Cmd_BXT_CH_Set_Velocity;
	struct Cmd_BXT_CH_Set_Velocity_Angles;
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
	struct Cmd_BXT_Record;
	struct Cmd_BXT_AutoRecord;
	struct Cmd_BXT_Interprocess_Reset;
	struct Cmd_BXT_Map;
	struct Cmd_BXT_Load;
	struct Cmd_BXT_Reset_Frametime_Remainder;
	struct Cmd_BXT_TASLog;
	struct Cmd_BXT_Append;
	struct Cmd_BXT_Heuristic;
	struct Cmd_BXT_StartSearch;

	void RegisterCVarsAndCommandsIfNeeded();
	void InsertCommands();
	bool GetNextMovementFrame(HLTAS::Frame& f);
	void ResetButtons();
	void FindCVarsIfNeeded();
	HLStrafe::MovementVars GetMovementVars();
	void KeyDown(Key& btn);
	void KeyUp(Key& btn);
	void SaveInitialDataToDemo();
	void UpdateCustomTriggers();

	bool registeredVarsAndCmds;

	bool autojump;
	bool ducktap;
	bool jumpbug;

	std::string recordDemoName;
	std::string autoRecordDemoName;
	size_t autoRecordDemoNumber;
	bool autoRecordNow;

	bool insideHost_Loadgame_f;
	bool insideHost_Reload_f;

	void *cls;
	void *clientstate;
	void *sv;
	ptrdiff_t offTime;
	ptrdiff_t offWorldmodel;
	ptrdiff_t offModels;
	ptrdiff_t offNumEdicts;
	ptrdiff_t offEdicts;
public:
	svs_t *svs;
	ptrdiff_t offEdict;
private:
	void *svmove;
	void *oldmove;
	void **ppmove;
	client_t **host_client;
	client_t *oldclient;
	edict_t **sv_player;
	edict_t *oldplayer;
	char *sv_areanodes;
	cmdbuf_t *cmd_text;
	double *host_frametime;
	int *demorecording;
	cmdalias_t* cmd_alias;

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

	bool isOverridingCamera = false;
	float cameraOverrideOrigin[3];
	float cameraOverrideAngles[3];
	bool isOffsettingCamera = false;
	float cameraOffsetOrigin[3];
	float cameraOffsetAngles[3];

	bool insideSeedRNG;
	unsigned LastRandomSeed;

	client_t* trace_oldclient;
	edict_t* trace_oldplayer;
	void* trace_oldmove;

	HLStrafe::PlayerData player;
	HLTAS::Input input;
	std::string demoName;
	std::string saveName;
	std::string frametime0ms;
	bool runningFrames;
	bool wasRunningFrames;
	size_t currentFramebulk;
	size_t preExecFramebulk = 0;
	size_t totalFramebulks;
	size_t currentRepeat;
	bool thisFrameIs0ms;
	bool SharedRNGSeedPresent;
	unsigned SharedRNGSeed;
	bool CountingSharedRNGSeed;
	unsigned SharedRNGSeedCounter;
	unsigned LoadingSeedCounter;
	bool ButtonsPresent;
	HLTAS::StrafeButtons Buttons;
	HLStrafe::CurrentState StrafeState;
	std::string exportFilename;
	HLTAS::Input exportResult;

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

	// Do not clear these inside Clear().
	bool SetNonSharedRNGSeed = false;
	std::time_t NonSharedRNGSeed;

	enum class ResetState {
		NORMAL = 0,
		PRERESET,
		POSTRESET
	} resetState = ResetState::NORMAL;

	std::string lastLoadedMap;
	bool insideKeyEvent;
	bool insideExec;
	std::string execScript;
	bool insideHost_Changelevel2_f;
	bool dontStopAutorecord;
};
