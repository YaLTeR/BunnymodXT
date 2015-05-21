#pragma once

#include "../sptlib-wrapper.hpp"
#include <SPTLib/IHookableNameFilterOrdered.hpp>
#include "../cvars.hpp"

class HwDLL : public IHookableNameFilterOrdered
{
	HOOK_DECL(void, __cdecl, LoadAndDecryptHwDLL, int a, void* b, void* c)
	HOOK_DECL(void, __cdecl, Cbuf_Execute)
	HOOK_DECL(void, __cdecl, SeedRandomNumberGenerator)
	HOOK_DECL(time_t, __cdecl, time, time_t *Time)
	HOOK_DECL(long double, __cdecl, RandomFloat, float a1, float a2)
	HOOK_DECL(long, __cdecl, RandomLong, long a1, long a2)
	HOOK_DECL(void, __cdecl, Host_Changelevel2_f)
	HOOK_DECL(void, __cdecl, SCR_BeginLoadingPlaque)
	HOOK_DECL(int, __cdecl, Host_FilterTime, float timePassed)

	struct cmdbuf_t
	{
		char *name;
		unsigned flags;
		char *data;
		unsigned maxsize;
		unsigned cursize;
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
	bool TryGettingAccurateInfo(float origin[3], float velocity[3]);
	void GetViewangles(float* va);

	inline void SetLastRandomSeed(unsigned seed) { LastRandomSeed = seed; }
	inline bool IsCountingSharedRNGSeed() { return CountingSharedRNGSeed; }
	inline unsigned GetSharedRNGSeedCounter() { return SharedRNGSeedCounter; }

	inline bool IsPaused() { return (sv && *(reinterpret_cast<int*>(sv) + 1)); }

	HLStrafe::TraceResult PlayerTrace(const float start[3], const float end[3], HLStrafe::HullType hull);

	unsigned QueuedSharedRNGSeeds;

private:
	// Make sure to have hl.exe last here, so that it is the lowest priority.
	HwDLL() : IHookableNameFilterOrdered({ L"hw.dll", L"hw.so", L"sw.dll", L"hl.exe" }) {};
	HwDLL(const HwDLL&);
	void operator=(const HwDLL&);

protected:
	typedef void(__cdecl *_Cbuf_InsertText) (const char* text);
	_Cbuf_InsertText ORIG_Cbuf_InsertText;
	typedef void(__cdecl *_Con_Printf) (const char* fmt, ...);
	_Con_Printf ORIG_Con_Printf;
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

	void FindStuff();

	static void Cmd_BXT_TAS_LoadScript();
	void Cmd_BXT_TAS_LoadScript_f();
	static void Cmd_BXT_Timer_Start();
	static void Cmd_BXT_Timer_Stop();
	static void Cmd_BXT_Timer_Reset();
	static void Cmd_BXT_TAS_Autojump_Down();
	static void Cmd_BXT_TAS_Autojump_Up();
	static void Cmd_BXT_TAS_Ducktap_Down();
	static void Cmd_BXT_TAS_Ducktap_Up();
	static void Cmd_BXT_Record();
	void Cmd_BXT_Record_f();
	static void Cmd_BXT_Setpos();
	void Cmd_BXT_Setpos_f();
	static void Cmd_BXT_ResetPlayer();
	void Cmd_BXT_ResetPlayer_f();

	void SetHFRMultiplayerCheck(bool enabled);
	void RegisterCVarsAndCommandsIfNeeded();
	bool CheckUnpause();
	void InsertCommands();
	bool GetNextMovementFrame(HLTAS::Frame& f);
	void ResetButtons();
	void FindCVarsIfNeeded();
	HLStrafe::MovementVars GetMovementVars();
	void KeyDown(Key& btn);
	void KeyUp(Key& btn);

	bool registeredVarsAndCmds;

	bool autojump;
	bool ducktap;
	std::string recordDemoName;

	void *cls;
	void *clientstate;
	void *sv;
	ptrdiff_t offWorldmodel;
	svs_t *svs;
	ptrdiff_t offEdict;
	void *svmove;
	void **ppmove;
	client_t **host_client;
	edict_t **sv_player;
	char *sv_areanodes;
	cmdbuf_t *cmd_text;
	double *host_frametime;

	uintptr_t hfrMultiplayerCheck;

	int framesTillExecuting;
	bool executing;
	bool insideCbuf_Execute;
	bool finishingLoad;
	bool dontPauseNextCycle;
	bool changelevel;
	bool recording;
	bool pauseOnTheFirstFrame;

	bool insideSeedRNG;
	unsigned LastRandomSeed;

	HLStrafe::PlayerData player;
	HLTAS::Input input;
	std::string demoName;
	std::string saveName;
	bool runningFrames;
	bool wasRunningFrames;
	size_t currentFramebulk;
	size_t totalFramebulks;
	size_t currentRepeat;
	bool SharedRNGSeedPresent;
	unsigned SharedRNGSeed;
	bool CountingSharedRNGSeed;
	unsigned SharedRNGSeedCounter;
	unsigned LoadingSeedCounter;
	bool ButtonsPresent;
	HLTAS::StrafeButtons Buttons;
	HLStrafe::CurrentState StrafeState;

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
};
