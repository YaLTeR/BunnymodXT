#include "../stdafx.hpp"

#include "../sptlib-wrapper.hpp"
#include <SPTLib/MemUtils.hpp>
#include <SPTLib/Hooks.hpp>
#include "HwDLL.hpp"
#include "ClientDLL.hpp"
#include "../patterns.hpp"
#include "../cvars.hpp"
#include "../hud_custom.hpp"
#include "../interprocess.hpp"

// Linux hooks.
#ifndef _WIN32
#include <dlfcn.h>

extern "C" void __cdecl Cbuf_Execute()
{
	return HwDLL::HOOKED_Cbuf_Execute();
}

extern "C" void __cdecl SeedRandomNumberGenerator()
{
	return HwDLL::HOOKED_SeedRandomNumberGenerator();
}

extern "C" void __cdecl Host_Changelevel2_f()
{
	return HwDLL::HOOKED_Host_Changelevel2_f();
}

extern "C" void __cdecl Host_SCR_BeginLoadingPlaque()
{
	return HwDLL::HOOKED_SCR_BeginLoadingPlaque();
}

extern "C" int __cdecl Host_FilterTime(float passedTime)
{
	return HwDLL::HOOKED_Host_FilterTime(passedTime);
}

extern "C" std::time_t time(std::time_t* t)
{
	if (!HwDLL::GetInstance().GetTimeAddr())
		HwDLL::GetInstance().SetTimeAddr(dlsym(RTLD_NEXT, "time"));
	return HwDLL::HOOKED_time(t);
}
#endif

void HwDLL::Hook(const std::wstring& moduleName, void* moduleHandle, void* moduleBase, size_t moduleLength, bool needToIntercept)
{
	Clear(); // Just in case.

	m_Handle = moduleHandle;
	m_Base = moduleBase;
	m_Length = moduleLength;
	m_Name = moduleName;
	m_Intercepted = needToIntercept;

	size_t number = 0;
	auto filename = GetFileName(m_Name);
	for (auto name : m_Names)
	{
		if (name == filename)
			break;

		number++;
	}
	m_HookedNumber = number;

	FindStuff();

	auto script = std::getenv("BXT_SCRIPT");
	if (script) {
		std::string filename(script);
		auto err = input.Open(filename).get();
		if (err.Code != HLTAS::ErrorCode::OK)
			EngineWarning("Error loading the script file on line %u: %s\n", err.LineNumber, HLTAS::GetErrorMessage(err).c_str());
		else
			for (auto prop : input.GetProperties())
				if (prop.first == "seed") {
					std::istringstream ss(prop.second);
					ss >> SharedRNGSeed >> NonSharedRNGSeed;
					SetNonSharedRNGSeed = true;
					EngineMsg("Loaded the seed from %s.\n", script);
				}
	}

	if (needToIntercept)
		MemUtils::Intercept(moduleName, {
			{ reinterpret_cast<void**>(&ORIG_LoadAndDecryptHwDLL), reinterpret_cast<void*>(HOOKED_LoadAndDecryptHwDLL) },
			{ reinterpret_cast<void**>(&ORIG_Cbuf_Execute), reinterpret_cast<void*>(HOOKED_Cbuf_Execute) },
			{ reinterpret_cast<void**>(&ORIG_SeedRandomNumberGenerator), reinterpret_cast<void*>(HOOKED_SeedRandomNumberGenerator) },
			{ reinterpret_cast<void**>(&ORIG_time), reinterpret_cast<void*>(HOOKED_time) },
			{ reinterpret_cast<void**>(&ORIG_RandomFloat), reinterpret_cast<void*>(HOOKED_RandomFloat) },
			{ reinterpret_cast<void**>(&ORIG_RandomLong), reinterpret_cast<void*>(HOOKED_RandomLong) },
			{ reinterpret_cast<void**>(&ORIG_Host_Changelevel2_f), reinterpret_cast<void*>(HOOKED_Host_Changelevel2_f) },
			{ reinterpret_cast<void**>(&ORIG_SCR_BeginLoadingPlaque), reinterpret_cast<void*>(HOOKED_SCR_BeginLoadingPlaque) },
			{ reinterpret_cast<void**>(&ORIG_Host_FilterTime), reinterpret_cast<void*>(HOOKED_Host_FilterTime) },
			{ reinterpret_cast<void**>(&ORIG_SCR_UpdateScreen), reinterpret_cast<void*>(HOOKED_SCR_UpdateScreen) }
		});
}

void HwDLL::Unhook()
{
	if (m_Intercepted)
		MemUtils::RemoveInterception(m_Name, {
			{ reinterpret_cast<void**>(&ORIG_LoadAndDecryptHwDLL), reinterpret_cast<void*>(HOOKED_LoadAndDecryptHwDLL) },
			{ reinterpret_cast<void**>(&ORIG_Cbuf_Execute), reinterpret_cast<void*>(HOOKED_Cbuf_Execute) },
			{ reinterpret_cast<void**>(&ORIG_SeedRandomNumberGenerator), reinterpret_cast<void*>(HOOKED_SeedRandomNumberGenerator) },
			{ reinterpret_cast<void**>(&ORIG_time), reinterpret_cast<void*>(HOOKED_time) },
			{ reinterpret_cast<void**>(&ORIG_RandomFloat), reinterpret_cast<void*>(HOOKED_RandomFloat) },
			{ reinterpret_cast<void**>(&ORIG_RandomLong), reinterpret_cast<void*>(HOOKED_RandomLong) },
			{ reinterpret_cast<void**>(&ORIG_RandomLong), reinterpret_cast<void*>(HOOKED_RandomLong) },
			{ reinterpret_cast<void**>(&ORIG_Host_Changelevel2_f), reinterpret_cast<void*>(HOOKED_Host_Changelevel2_f) },
			{ reinterpret_cast<void**>(&ORIG_SCR_BeginLoadingPlaque), reinterpret_cast<void*>(HOOKED_SCR_BeginLoadingPlaque) },
			{ reinterpret_cast<void**>(&ORIG_Host_FilterTime), reinterpret_cast<void*>(HOOKED_Host_FilterTime) },
			{ reinterpret_cast<void**>(&ORIG_SCR_UpdateScreen), reinterpret_cast<void*>(HOOKED_SCR_UpdateScreen) }
	});

	for (auto cvar : CVars::allCVars)
		cvar->Refresh();

	m_HookedNumber = m_Names.size();

	Clear();
}

void HwDLL::Clear()
{
	ORIG_LoadAndDecryptHwDLL = nullptr;
	ORIG_Cbuf_Execute = nullptr;
	ORIG_SeedRandomNumberGenerator = nullptr;
	ORIG_time = nullptr;
	ORIG_RandomFloat = nullptr;
	ORIG_RandomLong = nullptr;
	ORIG_Host_Changelevel2_f = nullptr;
	ORIG_SCR_BeginLoadingPlaque = nullptr;
	ORIG_Host_FilterTime = nullptr;
	ORIG_SCR_UpdateScreen = nullptr;
	ORIG_Cbuf_InsertText = nullptr;
	ORIG_Con_Printf = nullptr;
	ORIG_Cvar_RegisterVariable = nullptr;
	ORIG_Cvar_DirectSet = nullptr;
	ORIG_Cvar_FindVar = nullptr;
	ORIG_Cmd_AddMallocCommand = nullptr;
	ORIG_Cmd_Argc = nullptr;
	ORIG_Cmd_Args = nullptr;
	ORIG_Cmd_Argv = nullptr;
	ORIG_hudGetViewAngles = nullptr;
	ORIG_PM_PlayerTrace = nullptr;
	ORIG_SV_AddLinksToPM = nullptr;
	ORIG_Key_Event = nullptr;
	frametime_remainder = nullptr;
	registeredVarsAndCmds = false;
	autojump = false;
	ducktap = false;
	recordDemoName.clear();
	cls = nullptr;
	clientstate = nullptr;
	sv = nullptr;
	offWorldmodel = 0;
	svs = nullptr;
	offEdict = 0;
	svmove = nullptr;
	ppmove = nullptr;
	host_client = nullptr;
	sv_player = nullptr;
	sv_areanodes = nullptr;
	cmd_text = nullptr;
	host_frametime = nullptr;
	pInputInternal = nullptr;
	hfrMultiplayerCheck = 0;
	framesTillExecuting = 0;
	executing = false;
	insideCbuf_Execute = false;
	finishingLoad = false;
	dontPauseNextCycle = false;
	changelevel = false;
	recording = false;
	pauseOnTheFirstFrame = false;
	insideSeedRNG = false;
	LastRandomSeed = 0;
	player = HLStrafe::PlayerData();
	input.Clear();
	demoName.clear();
	saveName.clear();
	runningFrames = false;
	wasRunningFrames = false;
	currentFramebulk = 0;
	totalFramebulks = 0;
	currentRepeat = 0;
	StrafeState = HLStrafe::CurrentState();
	currentKeys.ResetStates();
	SharedRNGSeedPresent = false;
	SharedRNGSeed = 0;
	CountingSharedRNGSeed = false;
	SharedRNGSeedCounter = 0;
	QueuedSharedRNGSeeds = 0;
	LoadingSeedCounter = 0;
	ButtonsPresent = false;
}

void HwDLL::FindStuff()
{
	ORIG_Cbuf_Execute = reinterpret_cast<_Cbuf_Execute>(MemUtils::GetSymbolAddress(m_Handle, "Cbuf_Execute"));
	if (ORIG_Cbuf_Execute)
	{
		EngineDevMsg("[hw dll] Found Cbuf_Execute at %p.\n", ORIG_Cbuf_Execute);

		cls = MemUtils::GetSymbolAddress(m_Handle, "cls");
		if (cls)
			EngineDevMsg("[hw dll] Found cls at %p.\n", cls);
		else
			EngineDevWarning("[hw dll] Could not find cls.\n");

		sv = MemUtils::GetSymbolAddress(m_Handle, "sv");
		if (sv) {
			EngineDevMsg("[hw dll] Found sv at %p.\n", sv);
			offWorldmodel = 296;
		} else
			EngineDevWarning("[hw dll] Could not find sv.\n");

		svs = reinterpret_cast<svs_t*>(MemUtils::GetSymbolAddress(m_Handle, "svs"));
		if (svs) {
			EngineDevMsg("[hw dll] Found svs at %p.\n", svs);
			offEdict = 0x4a84;
		} else
			EngineDevWarning("[hw dll] Could not find svs.\n");

		svmove = MemUtils::GetSymbolAddress(m_Handle, "g_svmove");
		if (svmove)
			EngineDevMsg("[hw dll] Found g_svmove at %p.\n", svmove);
		else
			EngineDevWarning("[hw dll] Could not find g_svmove.\n");

		ppmove = reinterpret_cast<void**>(MemUtils::GetSymbolAddress(m_Handle, "pmove"));
		if (ppmove)
			EngineDevMsg("[hw dll] Found pmove at %p.\n", ppmove);
		else
			EngineDevWarning("[hw dll] Could not find pmove.\n");

		host_client = reinterpret_cast<client_t**>(MemUtils::GetSymbolAddress(m_Handle, "host_client"));
		if (host_client)
			EngineDevMsg("[hw dll] Found host_client at %p.\n", host_client);
		else
			EngineDevWarning("[hw dll] Could not find host_client.\n");

		sv_player = reinterpret_cast<edict_t**>(MemUtils::GetSymbolAddress(m_Handle, "sv_player"));
		if (sv_player)
			EngineDevMsg("[hw dll] Found sv_player at %p.\n", sv_player);
		else
			EngineDevWarning("[hw dll] Could not find sv_player.\n");

		sv_areanodes = reinterpret_cast<char*>(MemUtils::GetSymbolAddress(m_Handle, "sv_areanodes"));
		if (sv_areanodes)
			EngineDevMsg("[hw dll] Found sv_areanodes at %p.\n", sv_areanodes);
		else
			EngineDevWarning("[hw dll] Could not find sv_areanodes.\n");

		cmd_text = reinterpret_cast<cmdbuf_t*>(MemUtils::GetSymbolAddress(m_Handle, "cmd_text"));
		if (cmd_text)
			EngineDevMsg("[hw dll] Found cmd_text at %p.\n", cmd_text);
		else
			EngineDevWarning("[hw dll] Could not find cmd_text.\n");

		host_frametime = reinterpret_cast<double*>(MemUtils::GetSymbolAddress(m_Handle, "host_frametime"));
		if (host_frametime)
			EngineDevMsg("[hw dll] Found host_frametime at %p.\n", sv);
		else
			EngineDevWarning("[hw dll] Could not find host_frametime.\n");

		ORIG_hudGetViewAngles = reinterpret_cast<_hudGetViewAngles>(MemUtils::GetSymbolAddress(m_Handle, "hudGetViewAngles"));
		if (ORIG_hudGetViewAngles)
			EngineDevMsg("[hw dll] Found hudGetViewAngles at %p.\n", ORIG_hudGetViewAngles);
		else
			EngineDevWarning("[hw dll] Could not find ORIG_hudGetViewAngles.\n");

		ORIG_SV_AddLinksToPM = reinterpret_cast<_SV_AddLinksToPM>(MemUtils::GetSymbolAddress(m_Handle, "SV_AddLinksToPM"));
		if (ORIG_SV_AddLinksToPM)
			EngineDevMsg("[hw dll] Found SV_AddLinksToPM at %p.\n", ORIG_SV_AddLinksToPM);
		else
			EngineDevWarning("[hw dll] Could not find ORIG_SV_AddLinksToPM.\n");

		if (!cls || !sv || !svs || !svmove || !ppmove || !host_client || !sv_player || !sv_areanodes || !cmd_text || !host_frametime || !ORIG_hudGetViewAngles || !ORIG_SV_AddLinksToPM)
			ORIG_Cbuf_Execute = nullptr;

		#define FIND(f) \
			ORIG_##f = reinterpret_cast<_##f>(MemUtils::GetSymbolAddress(m_Handle, #f)); \
			if (ORIG_##f) \
				EngineDevMsg("[hw dll] Found " #f " at %p.\n", ORIG_##f); \
			else \
			{ \
				EngineDevWarning("[hw dll] Could not find " #f ".\n"); \
				ORIG_Cbuf_Execute = nullptr; \
			}
		FIND(Con_Printf)
		FIND(Cvar_RegisterVariable)
		FIND(Cvar_DirectSet)
		FIND(Cvar_FindVar)
		FIND(Cbuf_InsertText)
		FIND(Cmd_AddMallocCommand)
		FIND(Cmd_Argc)
		FIND(Cmd_Args)
		FIND(Cmd_Argv)
		FIND(SeedRandomNumberGenerator)
		//FIND(RandomFloat)
		//FIND(RandomLong)
		FIND(Host_Changelevel2_f)
		FIND(SCR_BeginLoadingPlaque)
		FIND(PM_PlayerTrace)
		#undef FIND

		ORIG_Host_FilterTime = reinterpret_cast<_Host_FilterTime>(MemUtils::GetSymbolAddress(m_Handle, "Host_FilterTime"));
		if (ORIG_Host_FilterTime)
			EngineDevMsg("[hw dll] Found Host_FilterTime at %p.\n", ORIG_Host_FilterTime);
		else
			EngineDevWarning("[hw dll] Could not find ORIG_Host_FilterTime.\n");
	}
	else
	{
		#define DEF_FUTURE(name) auto f##name = MemUtils::FindPatternOnly(reinterpret_cast<void**>(&ORIG_##name), m_Base, m_Length, Patterns::ptns##name, [](MemUtils::ptnvec_size ptnNumber) { }, []() { });
		DEF_FUTURE(Cvar_RegisterVariable)
		DEF_FUTURE(Cvar_DirectSet)
		DEF_FUTURE(Cvar_FindVar)
		DEF_FUTURE(Cbuf_InsertText)
		DEF_FUTURE(Cmd_AddMallocCommand)
		//DEF_FUTURE(RandomFloat)
		//DEF_FUTURE(RandomLong)
		DEF_FUTURE(Host_Changelevel2_f)
		DEF_FUTURE(SCR_BeginLoadingPlaque)
		DEF_FUTURE(PM_PlayerTrace)
		DEF_FUTURE(Host_FilterTime)
		DEF_FUTURE(SCR_UpdateScreen)
		DEF_FUTURE(Key_Event)
		bool oldEngine = (m_Name.find(L"hl.exe") != std::wstring::npos);
		std::future<MemUtils::ptnvec_size> fLoadAndDecryptHwDLL;
		if (oldEngine) {
			// In WON after the engine DLL has been loaded once for some reason there are multiple identical LoadAndDecrypt functions in the memory, we need the first one always.
			fLoadAndDecryptHwDLL = std::async(MemUtils::FindFirstSequence, m_Base, m_Length, Patterns::ptnsLoadAndDecryptHwDLL, reinterpret_cast<void**>(&ORIG_LoadAndDecryptHwDLL));
		}
		#undef DEF_FUTURE

		auto fCbuf_Execute = MemUtils::FindPatternOnly(reinterpret_cast<void**>(&ORIG_Cbuf_Execute), m_Base, m_Length, Patterns::ptnsCbuf_Execute,
			[&](MemUtils::ptnvec_size ptnNumber) {
				switch (ptnNumber)
				{
				case 0:
					cmd_text = reinterpret_cast<cmdbuf_t*>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cbuf_Execute) + 11) - offsetof(cmdbuf_t, cursize));
					break;
				case 1:
					cmd_text = reinterpret_cast<cmdbuf_t*>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cbuf_Execute) + 2) - offsetof(cmdbuf_t, cursize));
					break;
				}
			}, []() {}
		);

		auto fSeedRandomNumberGenerator = MemUtils::FindPatternOnly(reinterpret_cast<void**>(&ORIG_SeedRandomNumberGenerator), m_Base, m_Length, Patterns::ptnsSeedRandomNumberGenerator,
			[&](MemUtils::ptnvec_size ptnNumber) {
				ORIG_time = reinterpret_cast<_time>(
					*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_SeedRandomNumberGenerator) + 3)
					+ reinterpret_cast<uintptr_t>(ORIG_SeedRandomNumberGenerator) + 7
				);
			}, []() {}
		);

		void *SCR_DrawFPS;
		auto fSCR_DrawFPS = MemUtils::FindPatternOnly(&SCR_DrawFPS, m_Base, m_Length, Patterns::ptnsSCR_DrawFPS,
			[&](MemUtils::ptnvec_size ptnNumber) {
				host_frametime = *reinterpret_cast<double**>(reinterpret_cast<uintptr_t>(SCR_DrawFPS) + 21);
			}, []() {}
		);

		void *Host_Tell_f;
		auto fHost_Tell_f = MemUtils::FindPatternOnly(&Host_Tell_f, m_Base, m_Length, Patterns::ptnsHost_Tell_f,
			[&](MemUtils::ptnvec_size ptnNumber) {
				uintptr_t offCmd_Argc, offCmd_Args, offCmd_Argv;
				switch (ptnNumber)
				{
				default:
				case 0: // SteamPipe.
					offCmd_Argc = 28;
					offCmd_Args = 42;
					offCmd_Argv = 145;
					break;
				case 1: // NGHL.
					offCmd_Argc = 24;
					offCmd_Args = 38;
					offCmd_Argv = 143;
					break;
				case 2: // 4554.
					offCmd_Argc = 25;
					offCmd_Args = 39;
					offCmd_Argv = 144;
					break;
				}

				auto f = reinterpret_cast<uintptr_t>(Host_Tell_f);
				ORIG_Cmd_Argc = reinterpret_cast<_Cmd_Argc>(
					*reinterpret_cast<uintptr_t*>(f + offCmd_Argc)
					+ (f + offCmd_Argc + 4)
				);
				ORIG_Cmd_Args = reinterpret_cast<_Cmd_Args>(
					*reinterpret_cast<uintptr_t*>(f + offCmd_Args)
					+ (f + offCmd_Args + 4)
				);
				ORIG_Cmd_Argv = reinterpret_cast<_Cmd_Argv>(
					*reinterpret_cast<uintptr_t*>(f + offCmd_Argv)
					+ (f + offCmd_Argv + 4)
				);
			}, []() { }
		);

		void *Host_AutoSave_f;
		auto fHost_AutoSave_f = MemUtils::FindPatternOnly(&Host_AutoSave_f, m_Base, m_Length, Patterns::ptnsHost_AutoSave_f,
			[&](MemUtils::ptnvec_size ptnNumber) {
				auto f = reinterpret_cast<uintptr_t>(Host_AutoSave_f);
				sv = *reinterpret_cast<void**>(f + 19);
				offWorldmodel = 304;
				ORIG_Con_Printf = reinterpret_cast<_Con_Printf>(
					*reinterpret_cast<ptrdiff_t*>(f + 33)
					+ (f + 37)
					);
				cls = *reinterpret_cast<void**>(f + 69);
				svs = reinterpret_cast<svs_t*>(*reinterpret_cast<uintptr_t*>(f + 45) - 8);
				offEdict = 19356;
				clientstate = reinterpret_cast<void*>(*reinterpret_cast<uintptr_t*>(f + 86) - 0x2AF80);
			}, []() {}
		);

		void *MiddleOfSV_ReadClientMessage;
		auto fMiddleOfSV_ReadClientMessage = MemUtils::FindPatternOnly(&MiddleOfSV_ReadClientMessage, m_Base, m_Length, Patterns::ptnsMiddleOfSV_ReadClientMessage,
			[&](MemUtils::ptnvec_size ptnNumber) {
				switch (ptnNumber)
				{
				default:
				case 0: // SteamPipe & NGHL.
					host_client = *reinterpret_cast<client_t***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ReadClientMessage) + 14);
					svmove = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(MiddleOfSV_ReadClientMessage) + 39);
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ReadClientMessage) + 35);
					sv_player = *reinterpret_cast<edict_t***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ReadClientMessage) + 19);
					break;
				case 1: // 4554.
					host_client = *reinterpret_cast<client_t***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ReadClientMessage) + 17);
					svmove = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(MiddleOfSV_ReadClientMessage) + 42);
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ReadClientMessage) + 38);
					sv_player = *reinterpret_cast<edict_t***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ReadClientMessage) + 22);
					break;
				case 2: // WON.
					host_client = *reinterpret_cast<client_t***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ReadClientMessage) + 14);
					svmove = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(MiddleOfSV_ReadClientMessage) + 40);
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ReadClientMessage) + 36);
					sv_player = *reinterpret_cast<edict_t***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ReadClientMessage) + 20);
				}
			}, []() {}
		);

		void *MiddleOfSV_RunCmd;
		auto fMiddleOfSV_RunCmd = MemUtils::FindPatternOnly(&MiddleOfSV_RunCmd, m_Base, m_Length, Patterns::ptnsMiddleOfSV_RunCmd,
			[&](MemUtils::ptnvec_size ptnNumber) {
				sv_areanodes = *reinterpret_cast<char**>(reinterpret_cast<uintptr_t>(MiddleOfSV_RunCmd) + 20);
				ORIG_SV_AddLinksToPM = reinterpret_cast<_SV_AddLinksToPM>(
					*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(MiddleOfSV_RunCmd) + 25)
					+ reinterpret_cast<uintptr_t>(MiddleOfSV_RunCmd) + 29);
			}, []() {}
		);

		void *CL_Move;
		auto fCL_Move = MemUtils::FindPatternOnly(&CL_Move, m_Base, m_Length, Patterns::ptnsCL_Move,
			[&](MemUtils::ptnvec_size ptnNumber) {
				frametime_remainder = *reinterpret_cast<double**>(reinterpret_cast<uintptr_t>(CL_Move) + 451);
			}, []() {}
		);

		void *CBaseUI__Initialize;
		auto fCBaseUI__Initialize = MemUtils::FindPatternOnly(&CBaseUI__Initialize, m_Base, m_Length, Patterns::ptnsCBaseUI__Initialize,
			[&](MemUtils::ptnvec_size ptnNumber) {
				pInputInternal = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(CBaseUI__Initialize) + 274);
			}, []() {}
		);

		auto fHFRMultiplayerCheck = MemUtils::FindPatternOnly(reinterpret_cast<void**>(&hfrMultiplayerCheck), m_Base, m_Length, Patterns::ptnsHFRMultiplayerCheck,
			[&](MemUtils::ptnvec_size ptnNumber) {
				if (*reinterpret_cast<byte*>(hfrMultiplayerCheck + 16) != 0x75)
					hfrMultiplayerCheck = 0;
			}, []() {}
		);

		auto n = fCbuf_Execute.get();
		if (ORIG_Cbuf_Execute) {
			EngineDevMsg("[hw dll] Found Cbuf_Execute at %p (using the %s pattern).\n", ORIG_Cbuf_Execute, Patterns::ptnsCbuf_Execute[n].build.c_str());
			EngineDevMsg("[hw dll] Found cmd_text at %p.\n", cmd_text);
		} else
			EngineDevWarning("[hw dll] Could not find Cbuf_Execute.\n");

		n = fHost_AutoSave_f.get();
		if (Host_AutoSave_f) {
			EngineDevMsg("[hw dll] Found Host_AutoSave_f at %p (using the %s pattern).\n", Host_AutoSave_f, Patterns::ptnsHost_AutoSave_f[n].build.c_str());
			EngineDevMsg("[hw dll] Found cls at %p.\n", cls);
			EngineDevMsg("[hw dll] Found clientstate at %p.\n", clientstate);
			EngineDevMsg("[hw dll] Found sv at %p.\n", sv);
			EngineDevMsg("[hw dll] Found svs at %p.\n", svs);
			EngineDevMsg("[hw dll] Found Con_Printf at %p.\n", ORIG_Con_Printf);
		} else {
			EngineDevWarning("[hw dll] Could not find Host_AutoSave_f.\n");
			ORIG_Cbuf_Execute = nullptr;
		}

		n = fSeedRandomNumberGenerator.get();
		if (ORIG_SeedRandomNumberGenerator) {
			EngineDevMsg("[hw dll] Found SeedRandomNumberGenerator at %p (using the %s pattern).\n", ORIG_SeedRandomNumberGenerator, Patterns::ptnsSeedRandomNumberGenerator[n].build.c_str());
			EngineDevMsg("[hw dll] ORIG_time is %p.\n", ORIG_time);
		} else {
			EngineDevWarning("[hw dll] Could not find SeedRandomNumberGenerator.\n");
			ORIG_Cbuf_Execute = nullptr;
		}

		n = fSCR_DrawFPS.get();
		if (SCR_DrawFPS) {
			EngineDevMsg("[hw dll] Found SCR_DrawFPS at %p (using the %s pattern).\n", SCR_DrawFPS, Patterns::ptnsSCR_DrawFPS[n].build.c_str());
			EngineDevMsg("[hw dll] Found host_frametime at %p.\n", host_frametime);
		} else {
			EngineDevWarning("[hw dll] Could not find SCR_DrawFPS.\n");
			ORIG_Cbuf_Execute = nullptr;
		}

		n = fMiddleOfSV_ReadClientMessage.get();
		if (MiddleOfSV_ReadClientMessage) {
			EngineDevMsg("[hw dll] Found the g_svmove pattern at %p (using the %s pattern).\n", MiddleOfSV_ReadClientMessage, Patterns::ptnsMiddleOfSV_ReadClientMessage[n].build.c_str());
			EngineDevMsg("[hw dll] Found g_svmove at %p.\n", svmove);
			EngineDevMsg("[hw dll] Found pmove at %p.\n", ppmove);
			EngineDevMsg("[hw dll] Found host_client at %p.\n", host_client);
			EngineDevMsg("[hw dll] Found sv_player at %p.\n", sv_player);
		} else {
			EngineDevWarning("[hw dll] Could not find the g_svmove pattern.\n");
			ORIG_Cbuf_Execute = nullptr;
		}

		n = fMiddleOfSV_RunCmd.get();
		if (MiddleOfSV_RunCmd) {
			EngineDevMsg("[hw dll] Found the sv_areanodes pattern at %p (using the %s pattern).\n", MiddleOfSV_RunCmd, Patterns::ptnsMiddleOfSV_RunCmd[n].build.c_str());
			EngineDevMsg("[hw dll] Found sv_areanodes at %p.\n", sv_areanodes);
			EngineDevMsg("[hw dll] Found SV_AddLinksToPM at %p.\n", ORIG_SV_AddLinksToPM);
		} else {
			EngineDevWarning("[hw dll] Could not find the sv_areanodes pattern.\n");
			ORIG_Cbuf_Execute = nullptr;
		}

		n = fCBaseUI__Initialize.get();
		if (CBaseUI__Initialize) {
			EngineDevMsg("[hw dll] Found the gInputInternal pattern at %p (using the %s pattern).\n", CBaseUI__Initialize, Patterns::ptnsCBaseUI__Initialize[n].build.c_str());
			EngineDevMsg("[hw dll] Found gInputInternal at %p.\n", pInputInternal);
		} else {
			EngineDevWarning("[hw dll] Could not find the gInputInternal pattern.\n");
			EngineWarning("_bxt_key_event is not available.\n");
		}

		#define GET_FUTURE(name) \
			n = f##name.get(); \
			if (ORIG_##name) { \
				EngineDevMsg("[hw dll] Found " #name " at %p (using the %s pattern).\n", ORIG_##name, Patterns::ptns##name[n].build.c_str()); \
			} else { \
				EngineDevWarning("[hw dll] Could not find " #name ".\n"); \
				ORIG_Cbuf_Execute = nullptr; \
			}
		GET_FUTURE(Cvar_RegisterVariable)
		GET_FUTURE(Cvar_DirectSet)
		GET_FUTURE(Cvar_FindVar)
		GET_FUTURE(Cbuf_InsertText)
		GET_FUTURE(Cmd_AddMallocCommand)
		//GET_FUTURE(RandomFloat)
		//GET_FUTURE(RandomLong)
		GET_FUTURE(Host_Changelevel2_f)
		GET_FUTURE(SCR_BeginLoadingPlaque)
		GET_FUTURE(PM_PlayerTrace)
		#undef GET_FUTURE

		n = fHost_Tell_f.get();
		if (Host_Tell_f) {
			EngineDevMsg("[hw dll] Found Host_Tell_f at %p (using the %s pattern).\n", Host_Tell_f, Patterns::ptnsHost_Tell_f[n].build.c_str());
			EngineDevMsg("[hw dll] Found Cmd_Argc at %p.\n", ORIG_Cmd_Argc);
			EngineDevMsg("[hw dll] Found Cmd_Args at %p.\n", ORIG_Cmd_Args);
			EngineDevMsg("[hw dll] Found Cmd_Argv at %p.\n", ORIG_Cmd_Argv);
		} else {
			EngineDevWarning("[hw dll] Could not find Host_Tell_f.\n");
			ORIG_Cmd_AddMallocCommand = nullptr;
		}

		n = fSCR_UpdateScreen.get();
		if (ORIG_SCR_UpdateScreen) {
			EngineDevMsg("[hw dll] Found SCR_UpdateScreen at %p (using the %s pattern).\n", ORIG_SCR_UpdateScreen, Patterns::ptnsSCR_UpdateScreen[n].build.c_str());
		} else {
			EngineDevWarning("[hw dll] Could not find SCR_UpdateScreen.\n");
			EngineWarning("_bxt_norefresh is not available.\n");
		}

		n = fCL_Move.get();
		if (CL_Move) {
			EngineDevMsg("[hw dll] Found CL_Move at %p (using the %s pattern).\n", CL_Move, Patterns::ptnsCL_Move[n].build.c_str());
			EngineDevMsg("[hw dll] Found frametime_remainder at %p.\n", frametime_remainder);
		} else {
			EngineDevWarning("[hw dll] Could not find CL_Move.\n");
			EngineWarning("[hw dll] _bxt_reset_frametime_remainder is not available.\n");
		}

		n = fHost_FilterTime.get();
		if (ORIG_Host_FilterTime)
			EngineDevMsg("[hw dll] Found Host_FilterTime at %p (using the %s pattern).\n", ORIG_Host_FilterTime, Patterns::ptnsHost_FilterTime[n].build.c_str());
		else
			EngineDevWarning("[hw dll] Could not find Host_FilterTime.\n");

		n = fHFRMultiplayerCheck.get();
		if (hfrMultiplayerCheck) {
			EngineDevMsg("[hw dll] Found the host_framerate multiplayer check pattern at %p (using the %s pattern).\n", reinterpret_cast<void*>(hfrMultiplayerCheck), Patterns::ptnsHFRMultiplayerCheck[n].build.c_str());
		} else {
			EngineDevWarning("[hw dll] Could not find the host_framerate multiplayer check pattern.\n");
			EngineWarning("Host_framerate multiplayer check removal is not available.\n");
		}

		n = fKey_Event.get();
		if (ORIG_Key_Event) {
			EngineDevMsg("[hw dll] Found Key_Event at %p (using the %s pattern).\n", reinterpret_cast<void*>(ORIG_Key_Event), Patterns::ptnsKey_Event[n].build.c_str());
		} else {
			EngineDevWarning("[hw dll] Could not find Key_Event.\n");
			EngineWarning("_bxt_key_event is not available.\n");
		}

		if (oldEngine) {
			n = fLoadAndDecryptHwDLL.get();
			if (ORIG_LoadAndDecryptHwDLL)
				EngineDevMsg("[hw dll] Found LoadAndDecryptHwDLL at %p (using the %s pattern).\n", ORIG_LoadAndDecryptHwDLL, Patterns::ptnsLoadAndDecryptHwDLL[n].build.c_str());
			else
				EngineDevWarning("[hw dll] Could not find LoadAndDecryptHwDLL.\n");
		}
	}
}

void* HwDLL::GetTimeAddr()
{
	return reinterpret_cast<void*>(ORIG_time);
}

void HwDLL::SetTimeAddr(void* addr)
{
	ORIG_time = reinterpret_cast<_time>(addr);
}

void HwDLL::RegisterCVar(CVarWrapper& cvar)
{
	if (!ORIG_Cvar_FindVar || !ORIG_Cvar_RegisterVariable)
		return;

	if (ORIG_Cvar_FindVar(cvar.GetPointer()->name))
		return;

	ORIG_Cvar_RegisterVariable(cvar.GetPointer());
	cvar.MarkAsStale();
}

cvar_t* HwDLL::FindCVar(const char* name)
{
	if (!ORIG_Cvar_FindVar)
		return nullptr;

	return ORIG_Cvar_FindVar(name);
}

void HwDLL::Cmd_BXT_TAS_LoadScript()
{
	return HwDLL::GetInstance().Cmd_BXT_TAS_LoadScript_f();
}
void HwDLL::Cmd_BXT_TAS_LoadScript_f()
{
	runningFrames = false;
	currentFramebulk = 0;
	currentRepeat = 0;
	StrafeState = HLStrafe::CurrentState();
	ButtonsPresent = false;
	demoName.clear();
	saveName.clear();
	SharedRNGSeedPresent = false;
	SetNonSharedRNGSeed = false;

	if (ORIG_Cmd_Argc() != 2) {
		ORIG_Con_Printf("Usage: bxt_tas_loadscript <filename>\n");
		return;
	}

	std::string filename(ORIG_Cmd_Argv(1));
	auto err = input.Open(filename).get();
	if (err.Code != HLTAS::ErrorCode::OK) {
		ORIG_Con_Printf("Error loading the script file on line %u: %s\n", err.LineNumber, HLTAS::GetErrorMessage(err).c_str());
		return;
	}

	for (auto prop : input.GetProperties()) {
		if (prop.first == "demo")
			demoName = prop.second;
		else if (prop.first == "save")
			saveName = prop.second;
		else if (prop.first == "seed") {
			std::istringstream ss(prop.second);
			ss >> SharedRNGSeed >> NonSharedRNGSeed;
			SharedRNGSeedPresent = true;
			SetNonSharedRNGSeed = true;
		}
	}

	if (!input.GetFrames().empty()) {
		runningFrames = true;
		totalFramebulks = input.GetFrames().size();
		HLTAS::Frame f;
		if (GetNextMovementFrame(f)) {
			std::ostringstream ss;
			ss << "host_framerate " << f.Frametime.c_str() << "\n";
			ORIG_Cbuf_InsertText(ss.str().c_str());
		}
	}
}

void HwDLL::Cmd_BXT_Timer_Start()
{
	return CustomHud::SetCountingTime(true);
}

void HwDLL::Cmd_BXT_Timer_Stop()
{
	return CustomHud::SetCountingTime(false);
}

void HwDLL::Cmd_BXT_Timer_Reset()
{
	return CustomHud::ResetTime();
}

void HwDLL::Cmd_BXT_TAS_Autojump_Down()
{
	HwDLL::GetInstance().autojump = true;
}

void HwDLL::Cmd_BXT_TAS_Autojump_Up()
{
	HwDLL::GetInstance().autojump = false;
}

void HwDLL::Cmd_BXT_TAS_Ducktap_Down()
{
	HwDLL::GetInstance().ducktap = true;
}

void HwDLL::Cmd_BXT_TAS_Ducktap_Up()
{
	HwDLL::GetInstance().ducktap = false;
}

void HwDLL::Cmd_BXT_Record()
{
	return HwDLL::GetInstance().Cmd_BXT_Record_f();
}

void HwDLL::Cmd_BXT_Record_f()
{
	recordDemoName.clear();

	if (ORIG_Cmd_Argc() != 2) {
		ORIG_Con_Printf("Usage: bxt_record <demoname>\n");
		return;
	}

	recordDemoName.assign(ORIG_Cmd_Argv(1));
}

void HwDLL::Cmd_BXT_Reset_Frametime_Remainder()
{
	return HwDLL::GetInstance().Cmd_BXT_Reset_Frametime_Remainder_f();
}

void HwDLL::Cmd_BXT_Reset_Frametime_Remainder_f()
{
	if (frametime_remainder)
		*frametime_remainder = 0;
}

void HwDLL::Cmd_BXT_Setpos()
{
	return HwDLL::GetInstance().Cmd_BXT_Setpos_f();
}

void HwDLL::Cmd_BXT_Setpos_f()
{
	if (ORIG_Cmd_Argc() != 4) {
		ORIG_Con_Printf("Usage: bxt_setpos <x> <y> <z>\n");
		return;
	}

	if (svs->num_clients >= 1) {
		edict_t *pl = *reinterpret_cast<edict_t**>(reinterpret_cast<uintptr_t>(svs->clients) + offEdict);
		if (pl) {
			pl->v.origin[0] = boost::lexical_cast<float>(ORIG_Cmd_Argv(1));
			pl->v.origin[1] = boost::lexical_cast<float>(ORIG_Cmd_Argv(2));
			pl->v.origin[2] = boost::lexical_cast<float>(ORIG_Cmd_Argv(3));
		}
	}
}

void HwDLL::Cmd_BXT_ResetPlayer()
{
	return HwDLL::GetInstance().Cmd_BXT_ResetPlayer_f();
}

void HwDLL::Cmd_BXT_ResetPlayer_f()
{
	if (ORIG_Cmd_Argc() != 1) {
		ORIG_Con_Printf("Usage: bxt_resetplayer\n");
		return;
	}

	if (svs->num_clients >= 1) {
		edict_t *pl = *reinterpret_cast<edict_t**>(reinterpret_cast<uintptr_t>(svs->clients) + offEdict);
		if (pl) {
			pl->v.velocity[0] = 0;
			pl->v.velocity[1] = 0;
			pl->v.velocity[2] = 0;
			pl->v.basevelocity[0] = 0;
			pl->v.basevelocity[1] = 0;
			pl->v.basevelocity[2] = 0;
			pl->v.health = 100;
			pl->v.fuser2 = 0; // Stamina.
			pl->v.bInDuck = false;
			pl->v.flDuckTime = 0;
			pl->v.flags &= ~FL_DUCKING;

			if (clientstate) {
				float *viewangles = reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(clientstate) + 0x2ABE4);
				viewangles[0] = 0;
				viewangles[1] = 0;
				viewangles[2] = 0;
			}
		}
	}
}

void HwDLL::Cmd_BXT_Key_Event()
{
	return HwDLL::GetInstance().Cmd_BXT_Key_Event_f();
}

void HwDLL::Cmd_BXT_Key_Event_f()
{
	if (ORIG_Cmd_Argc() != 2) {
		ORIG_Con_Printf("Usage: _bxt_key_event <keynum>\n");
		return;
	}

	if (!ORIG_Key_Event)
		return;

	auto keynum = boost::lexical_cast<int>(ORIG_Cmd_Argv(1));
	ORIG_Key_Event(keynum, 1);
	ORIG_Key_Event(keynum, 0);
}

void HwDLL::Cmd_BXT_Input()
{
	return HwDLL::GetInstance().Cmd_BXT_Input_f();
}

void HwDLL::Cmd_BXT_Input_f()
{
	if (ORIG_Cmd_Argc() != 2) {
		ORIG_Con_Printf("Usage: _bxt_input <keynum>\n");
		return;
	}

	if (!pInputInternal)
		return;

	auto keynum = boost::lexical_cast<int>(ORIG_Cmd_Argv(1));
	auto gInputInternal = *pInputInternal;
	auto vtable = *reinterpret_cast<uintptr_t*>(gInputInternal);
	using f = void (__fastcall *) (void* thisptr, int edx, int arg);
	auto firstFunc = *reinterpret_cast<f*>(vtable + 120);
	auto secondFunc = *reinterpret_cast<f*>(vtable + 124);
	auto thirdFunc = *reinterpret_cast<f*>(vtable + 132);
	firstFunc(gInputInternal, 0, keynum);
	secondFunc(gInputInternal, 0, keynum);
	thirdFunc(gInputInternal, 0, keynum);
}

void HwDLL::Cmd_BXT_Input_Edit()
{
	return HwDLL::GetInstance().Cmd_BXT_Input_Edit_f();
}

void HwDLL::Cmd_BXT_Input_Edit_f()
{
	if (ORIG_Cmd_Argc() != 2) {
		ORIG_Con_Printf("Usage: _bxt_input_edit <keynum>\n");
		return;
	}

	if (!pInputInternal)
		return;

	auto keynum = boost::lexical_cast<int>(ORIG_Cmd_Argv(1));
	auto gInputInternal = *pInputInternal;
	auto vtable = *reinterpret_cast<uintptr_t*>(gInputInternal);
	using f = void (__fastcall *) (void* thisptr, int edx, int arg);
	auto firstFunc = *reinterpret_cast<f*>(vtable + 128);
	firstFunc(gInputInternal, 0, keynum);
}

void HwDLL::SetHFRMultiplayerCheck(bool enabled)
{
	if (!hfrMultiplayerCheck)
		return;

	if (enabled)
		MemUtils::ReplaceBytes(reinterpret_cast<void*>(hfrMultiplayerCheck + 16), 1, reinterpret_cast<byte*>("\x75"));
	else
		MemUtils::ReplaceBytes(reinterpret_cast<void*>(hfrMultiplayerCheck + 16), 1, reinterpret_cast<byte*>("\xEB"));
}

void HwDLL::Cmd_BXT_Interprocess_Reset()
{
	Interprocess::Initialize();
}

void HwDLL::RegisterCVarsAndCommandsIfNeeded()
{
	if (!registeredVarsAndCmds)
	{
		registeredVarsAndCmds = true;
		RegisterCVar(CVars::_bxt_taslog);
		RegisterCVar(CVars::_bxt_min_frametime);
		RegisterCVar(CVars::_bxt_norefresh);
		RegisterCVar(CVars::bxt_autopause);
		RegisterCVar(CVars::bxt_hfr_multiplayer_check);
		RegisterCVar(CVars::bxt_interprocess_enable);
		if (ORIG_Cmd_AddMallocCommand) {
			ORIG_Cmd_AddMallocCommand("bxt_tas_loadscript", Cmd_BXT_TAS_LoadScript, 2); // 2 - Cmd_AddGameCommand.
			ORIG_Cmd_AddMallocCommand("bxt_timer_start", Cmd_BXT_Timer_Start, 2);
			ORIG_Cmd_AddMallocCommand("bxt_timer_stop", Cmd_BXT_Timer_Stop, 2);
			ORIG_Cmd_AddMallocCommand("bxt_timer_reset", Cmd_BXT_Timer_Reset, 2);
			ORIG_Cmd_AddMallocCommand("+bxt_tas_autojump", Cmd_BXT_TAS_Autojump_Down, 2);
			ORIG_Cmd_AddMallocCommand("-bxt_tas_autojump", Cmd_BXT_TAS_Autojump_Up, 2);
			ORIG_Cmd_AddMallocCommand("+bxt_tas_ducktap", Cmd_BXT_TAS_Ducktap_Down, 2);
			ORIG_Cmd_AddMallocCommand("-bxt_tas_ducktap", Cmd_BXT_TAS_Ducktap_Up, 2);
			ORIG_Cmd_AddMallocCommand("bxt_record", Cmd_BXT_Record, 2);
			ORIG_Cmd_AddMallocCommand("bxt_setpos", Cmd_BXT_Setpos, 2);
			ORIG_Cmd_AddMallocCommand("bxt_resetplayer", Cmd_BXT_ResetPlayer, 2);
			ORIG_Cmd_AddMallocCommand("_bxt_interprocess_reset", Cmd_BXT_Interprocess_Reset, 2);
			ORIG_Cmd_AddMallocCommand("_bxt_reset_frametime_remainder", Cmd_BXT_Reset_Frametime_Remainder, 2);
			ORIG_Cmd_AddMallocCommand("_bxt_key_event", Cmd_BXT_Key_Event, 2);
			ORIG_Cmd_AddMallocCommand("_bxt_input", Cmd_BXT_Input, 2);
			ORIG_Cmd_AddMallocCommand("_bxt_input_edit", Cmd_BXT_Input_Edit, 2);
		}
	}
}

void HwDLL::InsertCommands()
{
	bool runningFramesBackup = runningFrames;

	if (runningFrames) {
		while (currentFramebulk < totalFramebulks) {
			auto& f = input.GetFrame(currentFramebulk);
			// Movement frame.
			if (currentRepeat || (f.SaveName.empty() && !f.SeedPresent && f.BtnState == HLTAS::ButtonState::NOTHING && !f.LgagstMinSpeedPresent)) {
				auto c = f.Commands;
				if (!c.empty())
					ORIG_Cbuf_InsertText(c.c_str());

				if (svs->num_clients >= 1) {
					edict_t *pl = *reinterpret_cast<edict_t**>(reinterpret_cast<uintptr_t>(svs->clients) + offEdict);
					if (pl) {
						player.Origin[0] = pl->v.origin[0];
						player.Origin[1] = pl->v.origin[1];
						player.Origin[2] = pl->v.origin[2];
						player.Velocity[0] = pl->v.velocity[0];
						player.Velocity[1] = pl->v.velocity[1];
						player.Velocity[2] = pl->v.velocity[2];
						player.Ducking = (pl->v.flags & FL_DUCKING) != 0;
						player.InDuckAnimation = (pl->v.bInDuck != 0);
						player.DuckTime = static_cast<float>(pl->v.flDuckTime);
						player.HasLJModule = false; // TODO

						player.StaminaTime = pl->v.fuser2;

						// Hope the viewangles aren't changed in ClientDLL's HUD_UpdateClientData() (that happens later in Host_Frame()).
						GetViewangles(player.Viewangles);
						//ORIG_Con_Printf("Player viewangles: %f %f %f\n", player.Viewangles[0], player.Viewangles[1], player.Viewangles[2]);
					}
				}

				if (!wasRunningFrames) {
					// We will reset buttons, set up the impulses accordingly.
					currentKeys.Forward.State = 4;
					currentKeys.Left.State = 4;
					currentKeys.Right.State = 4;
					currentKeys.Back.State = 4;
					currentKeys.Up.State = 4;
					currentKeys.Down.State = 4;

					currentKeys.CamLeft.State = 4;
					currentKeys.CamRight.State = 4;
					currentKeys.CamUp.State = 4;
					currentKeys.CamDown.State = 4;

					currentKeys.Jump.State = 4;
					currentKeys.Duck.State = 4;
					currentKeys.Use.State = 4;
					currentKeys.Attack1.State = 4;
					currentKeys.Attack2.State = 4;
					currentKeys.Reload.State = 4;
				}

				StrafeState.Jump = currentKeys.Jump.IsDown();
				StrafeState.Duck = currentKeys.Duck.IsDown();
				auto p = HLStrafe::MainFunc(player, GetMovementVars(), f, StrafeState, Buttons, ButtonsPresent, std::bind(&HwDLL::PlayerTrace, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

				f.ResetAutofuncs();

				#define INS(btn) \
					if (p.btn && !currentKeys.btn.IsDown()) \
						KeyDown(currentKeys.btn); \
					else if (!p.btn && currentKeys.btn.IsDown()) \
						KeyUp(currentKeys.btn);
				INS(Forward)
				INS(Left)
				INS(Right)
				INS(Back)
				INS(Up)
				INS(Down)
				INS(Jump)
				INS(Duck)
				INS(Use)
				INS(Attack1)
				INS(Attack2)
				INS(Reload)
				#undef INS

				//if (f.PitchPresent)
				//	ORIG_Con_Printf("Frame pitch: %f; ", f.GetPitch());
				//if (f.GetYawPresent())
				//	ORIG_Con_Printf("Frame yaw: %f; ", f.GetYaw());
				//ORIG_Con_Printf("Wish viewangles: %f %f\n", p.Pitch, p.Yaw);

				auto pitchStateMultiplier = 1.0;
				auto yawStateMultiplier = 1.0;
				if (p.Pitch == player.Viewangles[0]) {
					// Only one of those is to be pressed at any given time.
					if (currentKeys.CamUp.IsDown())
						KeyUp(currentKeys.CamUp);
					else if (currentKeys.CamDown.IsDown())
						KeyUp(currentKeys.CamDown);
				} else {
					double pitchDifference = HLStrafe::GetAngleDifference(player.Viewangles[0], p.Pitch);
					if (pitchDifference >= 0.0) {
						if (currentKeys.CamUp.IsDown())
							KeyUp(currentKeys.CamUp);
						if (!currentKeys.CamDown.IsDown())
							KeyDown(currentKeys.CamDown);

						pitchStateMultiplier = currentKeys.CamDown.StateMultiplier();
					} else {
						if (currentKeys.CamDown.IsDown())
							KeyUp(currentKeys.CamDown);
						if (!currentKeys.CamUp.IsDown())
							KeyDown(currentKeys.CamUp);

						pitchStateMultiplier = currentKeys.CamUp.StateMultiplier();
					}
				}

				if (p.Yaw == player.Viewangles[1]) {
					// Only one of those is to be pressed at any given time.
					if (currentKeys.CamLeft.IsDown())
						KeyUp(currentKeys.CamLeft);
					else if (currentKeys.CamRight.IsDown())
						KeyUp(currentKeys.CamRight);
				} else {
					double yawDifference = HLStrafe::GetAngleDifference(player.Viewangles[1], p.Yaw);
					if (yawDifference >= 0.0) {
						if (currentKeys.CamRight.IsDown())
							KeyUp(currentKeys.CamRight);
						if (!currentKeys.CamLeft.IsDown())
							KeyDown(currentKeys.CamLeft);

						yawStateMultiplier = currentKeys.CamLeft.StateMultiplier();
					} else {
						if (currentKeys.CamLeft.IsDown())
							KeyUp(currentKeys.CamLeft);
						if (!currentKeys.CamRight.IsDown())
							KeyDown(currentKeys.CamRight);

						yawStateMultiplier = currentKeys.CamRight.StateMultiplier();
					}
				}

				ORIG_Cbuf_InsertText(HLStrafe::GetAngleSpeedString(player.Viewangles[0], player.Viewangles[1], p.Pitch, p.Yaw, pitchStateMultiplier, yawStateMultiplier, static_cast<float>(*host_frametime)).c_str());

				std::ostringstream speeds_ss;
				speeds_ss.setf(std::ios::fixed, std::ios::floatfield);
				speeds_ss.precision(std::numeric_limits<float>::digits10);
				if (currentKeys.Forward.IsDown()) {
					double forwardspeed = p.Forwardspeed / currentKeys.Forward.StateMultiplier();
					speeds_ss << "cl_forwardspeed " << forwardspeed << '\n';
				}
				if (currentKeys.Back.IsDown()) {
					double backspeed = p.Backspeed / currentKeys.Back.StateMultiplier();
					speeds_ss << "cl_backspeed " << backspeed << '\n';
				}
				if (currentKeys.Left.IsDown() || currentKeys.Right.IsDown()) {
					// Kind of a collision here.
					double sidespeed = p.Sidespeed / std::min(currentKeys.Left.StateMultiplier(), currentKeys.Right.StateMultiplier());
					speeds_ss << "cl_sidespeed " << sidespeed << '\n';
				}
				if (currentKeys.Up.IsDown() || currentKeys.Down.IsDown()) {
					// And here.
					double upspeed = p.Upspeed / std::min(currentKeys.Up.StateMultiplier(), currentKeys.Down.StateMultiplier());
					speeds_ss << "cl_upspeed " << upspeed << '\n';
				}
				ORIG_Cbuf_InsertText(speeds_ss.str().c_str());

				// Clear impulses AFTER we handled viewangles and speeds, and only if we're active.
				if (*reinterpret_cast<int*>(cls) == 5) {
					currentKeys.Forward.ClearImpulses();
					currentKeys.Left.ClearImpulses();
					currentKeys.Right.ClearImpulses();
					currentKeys.Back.ClearImpulses();
					currentKeys.Up.ClearImpulses();
					currentKeys.Down.ClearImpulses();
					currentKeys.CamLeft.ClearImpulses();
					currentKeys.CamRight.ClearImpulses();
					currentKeys.CamUp.ClearImpulses();
					currentKeys.CamDown.ClearImpulses();
				}

				// We need this to be in the before all our movement commands,
				// so insert it last.
				if (!wasRunningFrames)
					ResetButtons();
				if (*reinterpret_cast<int*>(cls) == 5 && !recording && !demoName.empty()) {
					recording = true;
					std::ostringstream ss;
					ss << "record " << demoName.c_str() << "\n";
					ORIG_Cbuf_InsertText(ss.str().c_str());
				}

				if (++currentRepeat >= f.GetRepeats()) {
					currentRepeat = 0;
					currentFramebulk++;
					HLTAS::Frame next;
					if (GetNextMovementFrame(next)) {
						if (next.Frametime != f.Frametime) {
							std::ostringstream ss;
							ss << "host_framerate " << next.Frametime.c_str() << "\n";
							ORIG_Cbuf_InsertText(ss.str().c_str());
						}
					}
				}
				break;
			} else if (!f.SaveName.empty()) { // Saveload frame.
				std::ostringstream ss;
				ss << "save " << f.SaveName << ";load " << f.SaveName << "\n";
				ORIG_Cbuf_InsertText(ss.str().c_str());
				currentFramebulk++;
				break;
			} else if (f.SeedPresent) { // Seeds frame.
				SharedRNGSeedPresent = true;
				SharedRNGSeed = f.GetSeed();
			} else if (f.BtnState != HLTAS::ButtonState::NOTHING) { // Buttons frame.
				if (f.BtnState == HLTAS::ButtonState::SET) {
					ButtonsPresent = true;
					Buttons = f.GetButtons();
				} else
					ButtonsPresent = false;
			} else if (f.LgagstMinSpeedPresent) { // Lgagstminspeed frame.
				StrafeState.LgagstMinSpeed = f.GetLgagstMinSpeed();
			}

			currentFramebulk++;
		};

		// Ran through all frames.
		if (currentFramebulk >= totalFramebulks)
			runningFrames = false;
	} else {
		if (wasRunningFrames) {
			if (!demoName.empty()) {
				ORIG_Cbuf_InsertText("stop\n");
				recording = false;
			}
			if (!saveName.empty()) {
				std::ostringstream ss;
				ss << "save " << saveName.c_str() << "\n";
				ORIG_Cbuf_InsertText(ss.str().c_str());
			}
			ResetButtons();
			currentKeys.ResetStates();
			CountingSharedRNGSeed = false;
		}

		// Manual autofuncs.
		if (autojump || ducktap) {
			if (svs->num_clients >= 1) {
				edict_t *pl = *reinterpret_cast<edict_t**>(reinterpret_cast<uintptr_t>(svs->clients) + offEdict);
				if (pl) {
					player.Origin[0] = pl->v.origin[0];
					player.Origin[1] = pl->v.origin[1];
					player.Origin[2] = pl->v.origin[2];
					player.Velocity[0] = pl->v.velocity[0];
					player.Velocity[1] = pl->v.velocity[1];
					player.Velocity[2] = pl->v.velocity[2];
					player.Ducking = (pl->v.flags & FL_DUCKING) != 0;
					player.InDuckAnimation = (pl->v.bInDuck != 0);
					player.DuckTime = static_cast<float>(pl->v.flDuckTime);
					player.HasLJModule = false; // TODO

					player.StaminaTime = pl->v.fuser2;

												// Hope the viewangles aren't changed in ClientDLL's HUD_UpdateClientData() (that happens later in Host_Frame()).
					GetViewangles(player.Viewangles);
					//ORIG_Con_Printf("Player viewangles: %f %f %f\n", player.Viewangles[0], player.Viewangles[1], player.Viewangles[2]);
				}
			}

			bool Duck = false, Jump = false;

			auto playerCopy = HLStrafe::PlayerData(player); // Our copy that we will mess with.
			auto postype = GetPositionType(playerCopy, std::bind(&HwDLL::PlayerTrace, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
			if (postype == HLStrafe::PositionType::GROUND) {
				if (ducktap) {
					if (!currentKeys.Duck.IsDown() && !playerCopy.InDuckAnimation) {
						// This should check against the next frame's origin but meh.
						const float VEC_HULL_MIN[3] = { -16, -16, -36 };
						const float VEC_DUCK_HULL_MIN[3] = { -16, -16, -18 };
						float newOrigin[3];
						for (std::size_t i = 0; i < 3; ++i)
							newOrigin[i] = playerCopy.Origin[i] + (VEC_DUCK_HULL_MIN[i] - VEC_HULL_MIN[i]);

						auto tr = PlayerTrace(newOrigin, newOrigin, HLStrafe::HullType::NORMAL);
						if (!tr.StartSolid)
							Duck = true;
					}
				} else if (!currentKeys.Jump.IsDown()) {
					Jump = true;
				}
			}

			#define INS(btn) \
					if (btn && !currentKeys.btn.IsDown()) \
						KeyDown(currentKeys.btn); \
					else if (!btn && currentKeys.btn.IsDown()) \
						KeyUp(currentKeys.btn);
			if (ducktap) {
				INS(Duck)
			} else {
				INS(Jump)
			}
			#undef INS
		} else {
			if (currentKeys.Jump.IsDown())
				KeyUp(currentKeys.Jump);
			if (currentKeys.Duck.IsDown())
				KeyUp(currentKeys.Duck);
		}
	}
	wasRunningFrames = runningFramesBackup;

	if (*reinterpret_cast<int*>(cls) == 5 && !recordDemoName.empty()) {
		std::ostringstream ss;
		ss << "record " << recordDemoName.c_str() << "\n";
		ORIG_Cbuf_InsertText(ss.str().c_str());
		recordDemoName.clear();
	}
}

bool HwDLL::GetNextMovementFrame(HLTAS::Frame& f)
{
	auto curFramebulk = currentFramebulk;
	while (curFramebulk < totalFramebulks) {
		f = input.GetFrame(curFramebulk);
		// Only movement frames can have repeats.
		if (currentRepeat || (f.SaveName.empty() && !f.SeedPresent && f.BtnState == HLTAS::ButtonState::NOTHING && !f.LgagstMinSpeedPresent))
			return true;

		curFramebulk++;
	};

	return false;
}

void HwDLL::ResetButtons()
{
	ORIG_Cbuf_InsertText("-forward\n"
	                     "-moveleft\n"
	                     "-moveright\n"
	                     "-back\n"
	                     "-moveup\n"
	                     "-movedown\n"
	                     "-left\n"
	                     "-right\n"
	                     "-lookup\n"
	                     "-lookdown\n"
	                     "-jump\n"
	                     "-duck\n"
	                     "-use\n"
	                     "-attack\n"
	                     "-attack2\n"
	                     "-reload\n");
}

void HwDLL::FindCVarsIfNeeded()
{
	#define FIND(cvar) if (!CVars::cvar.GetPointer()) CVars::cvar.Assign(FindCVar(#cvar))
	FIND(sv_maxvelocity);
	FIND(sv_maxspeed);
	FIND(sv_stopspeed);
	FIND(sv_friction);
	FIND(edgefriction);
	FIND(sv_accelerate);
	FIND(sv_airaccelerate);
	FIND(sv_gravity);
	FIND(sv_stepsize);
	FIND(sv_bounce);
	#undef FIND
}

HLStrafe::MovementVars HwDLL::GetMovementVars()
{
	auto vars = HLStrafe::MovementVars();
	
	FindCVarsIfNeeded();
	vars.Frametime = static_cast<float>(static_cast<float>(std::floor(*host_frametime * 1000)) * 0.001); // This is how it's done inside the game.
	vars.Maxvelocity = CVars::sv_maxvelocity.GetFloat();
	vars.Maxspeed = CVars::sv_maxspeed.GetFloat();
	vars.Stopspeed = CVars::sv_stopspeed.GetFloat();
	vars.Friction = CVars::sv_friction.GetFloat();
	vars.Edgefriction = CVars::edgefriction.GetFloat();
	vars.Accelerate = CVars::sv_accelerate.GetFloat();
	vars.Airaccelerate = CVars::sv_airaccelerate.GetFloat();
	vars.Gravity = CVars::sv_gravity.GetFloat();
	vars.Stepsize = CVars::sv_stepsize.GetFloat();
	vars.Bounce = CVars::sv_bounce.GetFloat();
	vars.Bhopcap = CVars::bxt_bhopcap.GetBool();

	if (svs->num_clients >= 1) {
		edict_t *pl = *reinterpret_cast<edict_t**>(reinterpret_cast<uintptr_t>(svs->clients) + offEdict);
		if (pl) {
			vars.EntFriction = pl->v.friction;
			vars.EntGravity = pl->v.gravity;

			if (pl->v.maxspeed != 0.0f)
				vars.Maxspeed = std::min(pl->v.maxspeed, vars.Maxspeed);
		} else {
			vars.EntFriction = 1.0f;
			vars.EntGravity = 1.0f;
		}
	} else {
		vars.EntFriction = 1.0f;
		vars.EntGravity = 1.0f;
	}

	return vars;
}

void HwDLL::KeyDown(Key& key)
{
	key.Down();

	std::ostringstream ss;
	ss << '+' << key.Name << '\n';
	ORIG_Cbuf_InsertText(ss.str().c_str());
}

void HwDLL::KeyUp(Key& key)
{
	key.Up();

	std::ostringstream ss;
	ss << '-' << key.Name << '\n';
	ORIG_Cbuf_InsertText(ss.str().c_str());
}

HOOK_DEF_0(HwDLL, void, __cdecl, Cbuf_Execute)
{
	RegisterCVarsAndCommandsIfNeeded();

	int *state = reinterpret_cast<int*>(cls);
	int *paused = reinterpret_cast<int*>(sv)+1;
	static unsigned counter = 1;
	auto c = counter++;
	if (CVars::_bxt_taslog.GetBool()){
		std::string buf(cmd_text->data, cmd_text->cursize);
		ORIG_Con_Printf("Cbuf_Execute() #%u begin; cls.state: %d; sv.paused: %d; executing: %s; host_frametime: %f; buffer: %s\n", c, *state, *paused, (executing ? "true" : "false"), *host_frametime, buf.c_str());
	}

	if (insideCbuf_Execute) {
		ORIG_Cbuf_Execute();

		if (CVars::_bxt_taslog.GetBool()) {
			std::string buf(cmd_text->data, cmd_text->cursize);
			ORIG_Con_Printf("Cbuf_Execute() #%u end; sv.paused: %d; host_frametime: %f; buffer: %s\n", c, *paused, *host_frametime, buf.c_str());
		}

		return;
	}

	if (!finishingLoad && *state == 4 && !executing)
	{
		if (!*paused)
			framesTillExecuting = 2;
		finishingLoad = true;
	}
	if (finishingLoad && !*paused && !framesTillExecuting)
		executing = true;
	if (framesTillExecuting > 0)
		framesTillExecuting--;

	if (*state == 4 && !*paused && CVars::bxt_autopause.GetBool()) {
		ORIG_Cbuf_InsertText("pause\n");
		pauseOnTheFirstFrame = true;
	}

	if (*state != 5 && *state != 4)
		executing = false;

	insideCbuf_Execute = true;
	ORIG_Cbuf_Execute(); // executing might change inside if we had some kind of load command in the buffer.

	if (!executing)
		QueuedSharedRNGSeeds = 0;
	// Insert our commands after any commands that might have been on this frame
	// and call Cbuf_Execute again to execute them.
	if (executing)
	{
		changelevel = false;
		if (finishingLoad) { // First frame after load.
			finishingLoad = false;
			if (SharedRNGSeedPresent) {
				if (LoadingSeedCounter)
					SharedRNGSeedCounter += SharedRNGSeed;
				else
					SharedRNGSeedCounter = SharedRNGSeed;
				SharedRNGSeedPresent = false; // This should come after the RNG setting as that checks SeedsPresent itself.
				CountingSharedRNGSeed = true;
			} else {
				if (LoadingSeedCounter)
					SharedRNGSeedCounter += LoadingSeedCounter;
				else
					CountingSharedRNGSeed = false;
			}
			LoadingSeedCounter = 0;
		} else {
			SharedRNGSeedCounter++;
		}
		QueuedSharedRNGSeeds++;
		ClientDLL::GetInstance().ResetSeedsQueued();

		// For stopping Cbuf_Execute. Goes first because InsertCommands() inserts into beginning.
		if (cmd_text->cursize)
			ORIG_Cbuf_InsertText("wait\n");
		InsertCommands();

		if (*state == 5 && pauseOnTheFirstFrame) {
			ORIG_Cbuf_InsertText("setpause\n");
			pauseOnTheFirstFrame = false;
		}

		if (CVars::_bxt_taslog.GetBool()) {
			std::string buf(cmd_text->data, cmd_text->cursize);
			ORIG_Con_Printf("Cbuf_Execute() #%u executing; sv.paused: %d; buffer: %s\n", c, *paused, buf.c_str());
		}

		ORIG_Cbuf_Execute();

		// If still executing (didn't load a save).
		if (executing)
			CustomHud::TimePassed(*host_frametime);
	} else if (changelevel) {
		LoadingSeedCounter++;
	}
	insideCbuf_Execute = false;

	ClientDLL::GetInstance().SetAngleSpeedCap(CVars::bxt_anglespeed_cap.GetBool());
	ClientDLL::GetInstance().SetSpeedScaling(CVars::bxt_speed_scaling.GetBool());
	SetHFRMultiplayerCheck(CVars::bxt_hfr_multiplayer_check.GetBool());

	if (CVars::_bxt_taslog.GetBool()) {
		std::string buf(cmd_text->data, cmd_text->cursize);
		ORIG_Con_Printf("Cbuf_Execute() #%u end; sv.paused: %d; host_frametime: %f; buffer: %s\n", c, *paused, *host_frametime, buf.c_str());
	}
}

void HwDLL::SetPlayerOrigin(float origin[3])
{
	player.Origin[0] = origin[0];
	player.Origin[1] = origin[1];
	player.Origin[2] = origin[2];
}

void HwDLL::SetPlayerVelocity(float velocity[3])
{
	player.Velocity[0] = velocity[0];
	player.Velocity[1] = velocity[1];
	player.Velocity[2] = velocity[2];
}

bool HwDLL::TryGettingAccurateInfo(float origin[3], float velocity[3], float& stamina)
{
	if (!svs || svs->num_clients < 1)
		return false;

	edict_t *pl = *reinterpret_cast<edict_t**>(reinterpret_cast<uintptr_t>(svs->clients) + offEdict);
	if (!pl)
		return false;

	origin[0] = pl->v.origin[0];
	origin[1] = pl->v.origin[1];
	origin[2] = pl->v.origin[2];
	velocity[0] = pl->v.velocity[0];
	velocity[1] = pl->v.velocity[1];
	velocity[2] = pl->v.velocity[2];
	stamina = pl->v.fuser2;

	return true;
}

void HwDLL::GetViewangles(float* va)
{
	if (clientstate) {
		float *viewangles = reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(clientstate) + 0x2ABE4);
		va[0] = viewangles[0];
		va[1] = viewangles[1];
		va[2] = viewangles[2];
	} else
		ORIG_hudGetViewAngles(va);
}

HLStrafe::TraceResult HwDLL::PlayerTrace(const float start[3], const float end[3], HLStrafe::HullType hull)
{
	auto tr = HLStrafe::TraceResult{};

	if (!ORIG_PM_PlayerTrace || svs->num_clients < 1) {
		tr.Fraction = 1.f;
		tr.EndPos[0] = end[0];
		tr.EndPos[1] = end[1];
		tr.EndPos[2] = end[2];
		tr.Entity = -1;
		return tr;
	}

	auto oldclient = *host_client;
	*host_client = svs->clients;
	auto oldplayer = *sv_player;
	*sv_player = *reinterpret_cast<edict_t**>(reinterpret_cast<uintptr_t>(svs->clients) + offEdict);
	auto oldmove = *ppmove;
	*ppmove = svmove;
	ORIG_SV_AddLinksToPM(sv_areanodes, (*sv_player)->v.origin);

	auto usehull = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(svmove) + 0xBC);
	auto oldhull = *usehull;
	*usehull = static_cast<int>(hull);

	const int PM_NORMAL = 0x00000000;
	auto pmtr = ORIG_PM_PlayerTrace(start, end, PM_NORMAL, -1);

	*usehull = oldhull;
	*ppmove = oldmove;
	*sv_player = oldplayer;
	*host_client = oldclient;

	tr.AllSolid = (pmtr.allsolid != 0);
	tr.StartSolid = (pmtr.startsolid != 0);
	tr.Fraction = pmtr.fraction;
	tr.EndPos[0] = pmtr.endpos[0];
	tr.EndPos[1] = pmtr.endpos[1];
	tr.EndPos[2] = pmtr.endpos[2];
	tr.PlaneNormal[0] = pmtr.plane.normal[0];
	tr.PlaneNormal[1] = pmtr.plane.normal[1];
	tr.PlaneNormal[2] = pmtr.plane.normal[2];
	tr.Entity = pmtr.ent;
	return tr;
}

HOOK_DEF_0(HwDLL, void, __cdecl, SeedRandomNumberGenerator)
{
	insideSeedRNG = true;
	ORIG_SeedRandomNumberGenerator();
	insideSeedRNG = false;
}

HOOK_DEF_1(HwDLL, time_t, __cdecl, time, time_t*, Time)
{
	if (insideSeedRNG)
	{
		time_t ret = (SetNonSharedRNGSeed) ? NonSharedRNGSeed : ORIG_time(Time);
		SetNonSharedRNGSeed = false;

		std::ostringstream ss;
		ss << "Called time from SeedRandomNumberGenerator -> " << ret << ".\n";
		EngineMsg("%s", ss.str().c_str());

		return ret;
	}
	else
		return ORIG_time(Time);
}

HOOK_DEF_2(HwDLL, long double, __cdecl, RandomFloat, float, a1, float, a2)
{
	auto ret = ORIG_RandomFloat(a1, a2);
	ORIG_Con_Printf("RandomFloat(%f, %f) => %Lf.\n", a1, a2, ret);
	return ret;
}

HOOK_DEF_2(HwDLL, long, __cdecl, RandomLong, long, a1, long, a2)
{
	auto ret = ORIG_RandomLong(a1, a2);
	ORIG_Con_Printf("RandomLong(%ld, %ld) => %ld.\n", a1, a2, ret);
	return ret;
}

HOOK_DEF_0(HwDLL, void, __cdecl, Host_Changelevel2_f)
{
	//ORIG_Con_Printf("Host_Changelevel2_f\n");
	changelevel = true;
	if (!CountingSharedRNGSeed && SharedRNGSeedPresent)
		SharedRNGSeedCounter = LastRandomSeed;

	return ORIG_Host_Changelevel2_f();
}

HOOK_DEF_0(HwDLL, void, __cdecl, SCR_BeginLoadingPlaque)
{
	executing = false;
	return ORIG_SCR_BeginLoadingPlaque();
}

HOOK_DEF_1(HwDLL, int, __cdecl, Host_FilterTime, float, passedTime)
{
	static double timeCounter = 0.0;
	static bool usePassedTime = false;

	auto minFrametime = CVars::_bxt_min_frametime.GetFloat();
	if (minFrametime == 0.0f) {
		timeCounter = 0.0;
		usePassedTime = false;
		return ORIG_Host_FilterTime(passedTime);
	}

	timeCounter += passedTime;
	if (timeCounter < minFrametime)
		return 0;

	if (ORIG_Host_FilterTime(usePassedTime ? passedTime : static_cast<float>(timeCounter))) {
		usePassedTime = false;
		timeCounter = std::fmod(timeCounter, minFrametime);
		return 1;
	} else {
		usePassedTime = true;
		return 0;
	}
}

HOOK_DEF_0(HwDLL, void, __cdecl, SCR_UpdateScreen)
{
	if (!CVars::_bxt_norefresh.GetBool())
		ORIG_SCR_UpdateScreen();
}

HOOK_DEF_3(HwDLL, void, __cdecl, LoadAndDecryptHwDLL, int, a, void*, b, void*, c)
{
	ORIG_LoadAndDecryptHwDLL(a, b, c);
	EngineDevMsg("[hw dll] LoadAndDecryptHwDLL has been called. Rehooking.\n");
	Hooks::HookModule(L"hl.exe");
}
