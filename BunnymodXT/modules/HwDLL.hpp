#pragma once

#include "./sptlib-wrapper.hpp"
#include <SPTLib/IHookableNameFilterOrdered.hpp>

class HwDLL : public IHookableNameFilterOrdered
{
	HOOK_DECL(void, __cdecl, Cbuf_Execute)
	HOOK_DECL(void, __cdecl, SeedRandomNumberGenerator)
	HOOK_DECL(time_t, __cdecl, time, time_t *Time)
	HOOK_DECL(long double, __cdecl, RandomFloat, float a1, float a2)
	HOOK_DECL(long, __cdecl, RandomLong, long a1, long a2)

	struct cmdbuf_t
	{
		char *name;
		unsigned flags;
		char *data;
		unsigned maxsize;
		unsigned cursize;
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

	void SetPlayerOrigin(float origin[3]);
	void SetPlayerVelocity(float velocity[3]);

private:
	// Make sure to have hl.exe last here, so that it is the lowest priority.
	HwDLL() : IHookableNameFilterOrdered({ L"hw.dll", L"hw.so", L"sw.dll", L"hl.exe" }) {};
	HwDLL(const HwDLL&);
	void operator=(const HwDLL&);

protected:
	typedef void(__cdecl *_Cbuf_InsertText) (const char* text);
	_Cbuf_InsertText ORIG_Cbuf_InsertText;
	typedef void(__cdecl *_Con_Printf) (char* fmt, ...);
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

	void FindStuff();

	static void Cmd_BXT_TAS_LoadScript();
	void Cmd_BXT_TAS_LoadScript_f();

	void RegisterCVarsAndCommandsIfNeeded();
	bool CheckUnpause();
	void InsertCommands();
	void ResetButtons();
	void FindCVarsIfNeeded();
	HLStrafe::MovementVars GetMovementVars();

	bool registeredVarsAndCmds;

	void *cls;
	void *sv;
	cmdbuf_t *cmd_text;
	double *host_frametime;

	bool executing;
	bool loading;
	bool insideCbuf_Execute;
	bool finishingLoad;
	bool dontPauseNextCycle;

	bool insideSeedRNG;

	HLStrafe::PlayerData player;
	HLTAS::Input input;
	bool runningFrames;
	bool wasRunningFrames;
	size_t currentFramebulk;
	size_t totalFramebulks;
	size_t currentRepeat;
	HLStrafe::ProcessedFrame previousButtons;
};
