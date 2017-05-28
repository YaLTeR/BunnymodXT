#include "../stdafx.hpp"

#include <cerrno>
#include "../sptlib-wrapper.hpp"
#include <SPTLib/MemUtils.hpp>
#include <SPTLib/Hooks.hpp>
#include "HwDLL.hpp"
#include "ClientDLL.hpp"
#include "ServerDLL.hpp"
#include "../patterns.hpp"
#include "../cvars.hpp"
#include "../hud_custom.hpp"
#include "../interprocess.hpp"
#include "../bunnymodxt.hpp"
#include "../cmd_wrapper.hpp"
#include "../runtime_data.hpp"

using namespace std::literals;

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

extern "C" void __cdecl SCR_BeginLoadingPlaque()
{
	return HwDLL::HOOKED_SCR_BeginLoadingPlaque();
}

extern "C" int __cdecl Host_FilterTime(float passedTime)
{
	return HwDLL::HOOKED_Host_FilterTime(passedTime);
}

extern "C" int __cdecl V_FadeAlpha()
{
	return HwDLL::HOOKED_V_FadeAlpha();
}

extern "C" void __cdecl SCR_UpdateScreen()
{
	return HwDLL::HOOKED_SCR_UpdateScreen();
}

extern "C" std::time_t time(std::time_t* t)
{
	if (!HwDLL::GetInstance().GetTimeAddr())
		HwDLL::GetInstance().SetTimeAddr(dlsym(RTLD_NEXT, "time"));
	return HwDLL::HOOKED_time(t);
}

extern "C" void __cdecl SV_Frame()
{
	HwDLL::HOOKED_SV_Frame();
}

extern "C" int __cdecl SV_SpawnServer(int bIsDemo, char* server, char* startspot)
{
	return HwDLL::HOOKED_SV_SpawnServer(bIsDemo, server, startspot);
}

extern "C" void __cdecl CL_Stop_f()
{
	HwDLL::HOOKED_CL_Stop_f();
}

extern "C" void __cdecl Host_Loadgame_f()
{
	HwDLL::HOOKED_Host_Loadgame_f();
}

extern "C" void __cdecl Host_Reload_f()
{
	HwDLL::HOOKED_Host_Reload_f();
}

extern "C" void __cdecl VGuiWrap2_ConDPrintf(const char* msg)
{
	HwDLL::HOOKED_VGuiWrap2_ConDPrintf(msg);
}

extern "C" void __cdecl VGuiWrap2_ConPrintf(const char* msg)
{
	HwDLL::HOOKED_VGuiWrap2_ConPrintf(msg);
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

	// Get the seed (if we're not resetting, in that case we have the seed already).
	if (resetState == ResetState::NORMAL) {
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
	} else {
		resetState = ResetState::POSTRESET;
	}

	if (needToIntercept) {
		if (ORIG_LoadAndDecryptHwDLL) {
			// When the old engine loads hw.dll, it marks it as PAGE_READWRITE, without EXECUTE.
			// So we need to mark stuff as executable manually, otherwise MinHook complains.
			MemUtils::MarkAsExecutable(ORIG_Cbuf_Execute);
			MemUtils::MarkAsExecutable(ORIG_SeedRandomNumberGenerator);
			MemUtils::MarkAsExecutable(ORIG_time);
			MemUtils::MarkAsExecutable(ORIG_RandomFloat);
			MemUtils::MarkAsExecutable(ORIG_RandomLong);
			MemUtils::MarkAsExecutable(ORIG_Host_Changelevel2_f);
			MemUtils::MarkAsExecutable(ORIG_SCR_BeginLoadingPlaque);
			MemUtils::MarkAsExecutable(ORIG_Host_FilterTime);
			MemUtils::MarkAsExecutable(ORIG_V_FadeAlpha);
			MemUtils::MarkAsExecutable(ORIG_SCR_UpdateScreen);
			MemUtils::MarkAsExecutable(ORIG_SV_SpawnServer);
			MemUtils::MarkAsExecutable(ORIG_SV_Frame);
			MemUtils::MarkAsExecutable(ORIG_CL_Stop_f);
			MemUtils::MarkAsExecutable(ORIG_Host_Loadgame_f);
			MemUtils::MarkAsExecutable(ORIG_Host_Reload_f);
			MemUtils::MarkAsExecutable(ORIG_VGuiWrap2_ConDPrintf);
			MemUtils::MarkAsExecutable(ORIG_VGuiWrap2_ConPrintf);
		}

		MemUtils::Intercept(moduleName,
			ORIG_LoadAndDecryptHwDLL, HOOKED_LoadAndDecryptHwDLL,
			ORIG_Cbuf_Execute, HOOKED_Cbuf_Execute,
			ORIG_SeedRandomNumberGenerator, HOOKED_SeedRandomNumberGenerator,
			ORIG_time, HOOKED_time,
			ORIG_RandomFloat, HOOKED_RandomFloat,
			ORIG_RandomLong, HOOKED_RandomLong,
			ORIG_Host_Changelevel2_f, HOOKED_Host_Changelevel2_f,
			ORIG_SCR_BeginLoadingPlaque, HOOKED_SCR_BeginLoadingPlaque,
			ORIG_Host_FilterTime, HOOKED_Host_FilterTime,
			ORIG_V_FadeAlpha, HOOKED_V_FadeAlpha,
			ORIG_SCR_UpdateScreen, HOOKED_SCR_UpdateScreen,
			ORIG_SV_SpawnServer, HOOKED_SV_SpawnServer,
			ORIG_SV_Frame, HOOKED_SV_Frame,
			ORIG_CL_Stop_f, HOOKED_CL_Stop_f,
			ORIG_Host_Loadgame_f, HOOKED_Host_Loadgame_f,
			ORIG_Host_Reload_f, HOOKED_Host_Reload_f,
			ORIG_VGuiWrap2_ConDPrintf, HOOKED_VGuiWrap2_ConDPrintf,
			ORIG_VGuiWrap2_ConPrintf, HOOKED_VGuiWrap2_ConPrintf);
	}
}

void HwDLL::Unhook()
{
	if (m_Intercepted)
	{
		MemUtils::RemoveInterception(m_Name,
			ORIG_LoadAndDecryptHwDLL,
			ORIG_Cbuf_Execute,
			ORIG_SeedRandomNumberGenerator,
			ORIG_time,
			ORIG_RandomFloat,
			ORIG_RandomLong,
			ORIG_RandomLong,
			ORIG_Host_Changelevel2_f,
			ORIG_SCR_BeginLoadingPlaque,
			ORIG_Host_FilterTime,
			ORIG_V_FadeAlpha,
			ORIG_SCR_UpdateScreen,
			ORIG_SV_SpawnServer,
			ORIG_SV_Frame,
			ORIG_CL_Stop_f,
			ORIG_Host_Loadgame_f,
			ORIG_Host_Reload_f,
			ORIG_VGuiWrap2_ConDPrintf,
			ORIG_VGuiWrap2_ConPrintf);
	}

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
	ORIG_V_FadeAlpha = nullptr;
	ORIG_SCR_UpdateScreen = nullptr;
	ORIG_SV_Frame = nullptr;
	ORIG_SV_SpawnServer = nullptr;
	ORIG_CL_Stop_f = nullptr;
	ORIG_Host_Loadgame_f = nullptr;
	ORIG_Host_Reload_f = nullptr;
	ORIG_VGuiWrap2_ConDPrintf = nullptr;
	ORIG_VGuiWrap2_ConPrintf = nullptr;
	ORIG_Cbuf_InsertText = nullptr;
	ORIG_Cbuf_AddText = nullptr;
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
	ORIG_PF_GetPhysicsKeyValue = nullptr;
	ORIG_CL_RecordHUDCommand = nullptr;
	ORIG_build_number = nullptr;
	registeredVarsAndCmds = false;
	autojump = false;
	ducktap = false;
	recordDemoName.clear();
	autoRecordDemoName.clear();
	autoRecordDemoNumber = 1;
	autoRecordNow = false;
	insideHost_Loadgame_f = false;
	insideHost_Reload_f = false;
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
	frametime_remainder = nullptr;
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
	currentRepeat = 0;
	thisFrameIs0ms = false;
	currentKeys.ResetStates();
	CountingSharedRNGSeed = false;
	SharedRNGSeedCounter = 0;
	QueuedSharedRNGSeeds = 0;
	LoadingSeedCounter = 0;
	lastLoadedMap.clear();
	isOverridingCamera = false;
	isOffsettingCamera = false;

	if (resetState == ResetState::NORMAL) {
		input.Clear();
		demoName.clear();
		saveName.clear();
		frametime0ms.clear();
		runningFrames = false;
		wasRunningFrames = false;
		currentFramebulk = 0;
		totalFramebulks = 0;
		StrafeState = HLStrafe::CurrentState();
		SharedRNGSeedPresent = false;
		SharedRNGSeed = 0;
		ButtonsPresent = false;
	}
}

void HwDLL::FindStuff()
{
	ORIG_Cbuf_Execute = reinterpret_cast<_Cbuf_Execute>(MemUtils::GetSymbolAddress(m_Handle, "Cbuf_Execute"));
	if (ORIG_Cbuf_Execute)
	{
		EngineDevMsg("[hw dll] Found Cbuf_Execute at %p.\n", ORIG_Cbuf_Execute);

		cls = MemUtils::GetSymbolAddress(m_Handle, "cls");
		if (cls) {
			EngineDevMsg("[hw dll] Found cls at %p.\n", cls);
			demorecording = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(cls) + 0x405c);
		} else
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
		FIND(Cbuf_AddText)
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
		FIND(CL_Stop_f)
		FIND(Host_Loadgame_f)
		FIND(Host_Reload_f)
		FIND(SV_SpawnServer)
		FIND(CL_RecordHUDCommand)
		#undef FIND

		ORIG_Host_FilterTime = reinterpret_cast<_Host_FilterTime>(MemUtils::GetSymbolAddress(m_Handle, "Host_FilterTime"));
		if (ORIG_Host_FilterTime)
			EngineDevMsg("[hw dll] Found Host_FilterTime at %p.\n", ORIG_Host_FilterTime);
		else
			EngineDevWarning("[hw dll] Could not find Host_FilterTime.\n");

		ORIG_V_FadeAlpha = reinterpret_cast<_V_FadeAlpha>(MemUtils::GetSymbolAddress(m_Handle, "V_FadeAlpha"));
		if (ORIG_V_FadeAlpha)
			EngineDevMsg("[hw dll] Found V_FadeAlpha at %p.\n", ORIG_V_FadeAlpha);
		else
			EngineDevWarning("[hw dll] Could not find V_FadeAlpha.\n");

		ORIG_SCR_UpdateScreen = reinterpret_cast<_SCR_UpdateScreen>(MemUtils::GetSymbolAddress(m_Handle, "SCR_UpdateScreen"));
		if (ORIG_SCR_UpdateScreen)
			EngineDevMsg("[hw dll] Found SCR_UpdateScreen at %p.\n", ORIG_SCR_UpdateScreen);
		else
			EngineDevWarning("[hw dll] Could not find SCR_UpdateScreen.\n");

		ORIG_PF_GetPhysicsKeyValue = reinterpret_cast<_PF_GetPhysicsKeyValue>(MemUtils::GetSymbolAddress(m_Handle, "PF_GetPhysicsKeyValue"));
		if (ORIG_PF_GetPhysicsKeyValue)
			EngineDevMsg("[hw dll] Found PF_GetPhysicsKeyValue at %p.\n", ORIG_PF_GetPhysicsKeyValue);
		else
			EngineDevWarning("[hw dll] Could not find PF_GetPhysicsKeyValue.\n");

		ORIG_build_number = reinterpret_cast<_build_number>(MemUtils::GetSymbolAddress(m_Handle, "build_number"));
		if (ORIG_build_number)
			EngineDevMsg("[hw dll] Found build_number at %p.\n", ORIG_build_number);
		else
			EngineDevWarning("[hw dll] Could not find build_number.\n");

		ORIG_SV_Frame = reinterpret_cast<_SV_Frame>(MemUtils::GetSymbolAddress(m_Handle, "SV_Frame"));
		if (ORIG_SV_Frame)
			EngineDevMsg("[hw dll] Found SV_Frame at %p.\n", ORIG_SV_Frame);
		else
			EngineDevWarning("[hw dll] Could not find SV_Frame.\n");

		ORIG_VGuiWrap2_ConDPrintf = reinterpret_cast<_VGuiWrap2_ConDPrintf>(MemUtils::GetSymbolAddress(m_Handle, "VGuiWrap2_ConDPrintf"));
		if (ORIG_VGuiWrap2_ConDPrintf)
			EngineDevMsg("[hw dll] Found VGuiWrap2_ConDPrintf at %p.\n", ORIG_VGuiWrap2_ConDPrintf);
		else
			EngineDevWarning("[hw dll] Could not find VGuiWrap2_ConDPrintf.\n");

		ORIG_VGuiWrap2_ConPrintf = reinterpret_cast<_VGuiWrap2_ConPrintf>(MemUtils::GetSymbolAddress(m_Handle, "VGuiWrap2_ConPrintf"));
		if (ORIG_VGuiWrap2_ConPrintf)
			EngineDevMsg("[hw dll] Found VGuiWrap2_ConPrintf at %p.\n", ORIG_VGuiWrap2_ConDPrintf);
		else
			EngineDevWarning("[hw dll] Could not find VGuiWrap2_ConPrintf.\n");

		const auto CL_Move = reinterpret_cast<uintptr_t>(MemUtils::GetSymbolAddress(m_Handle, "CL_Move"));
		if (CL_Move)
		{
			EngineDevMsg("[hw dll] Found CL_Move at %p.\n", CL_Move);
			frametime_remainder = *reinterpret_cast<double**>(CL_Move + 0x1C4);
			EngineDevMsg("[hw dll] Found frametime_remainder at %p.\n", frametime_remainder);
		} else {
			EngineDevWarning("[hw dll] Could not find CL_Move.\n");
			EngineWarning("_bxt_reset_frametime_remainder has no effect.\n");
		}
	}
	else
	{
		#define DEF_FUTURE(name) auto f##name = FindAsync(ORIG_##name, patterns::engine::name);
		DEF_FUTURE(Cvar_RegisterVariable)
		DEF_FUTURE(Cvar_DirectSet)
		DEF_FUTURE(Cvar_FindVar)
		DEF_FUTURE(Cbuf_InsertText)
		DEF_FUTURE(Cbuf_AddText)
		DEF_FUTURE(Cmd_AddMallocCommand)
		//DEF_FUTURE(RandomFloat)
		//DEF_FUTURE(RandomLong)
		DEF_FUTURE(SCR_BeginLoadingPlaque)
		DEF_FUTURE(PM_PlayerTrace)
		DEF_FUTURE(Host_FilterTime)
		DEF_FUTURE(V_FadeAlpha)
		DEF_FUTURE(SCR_UpdateScreen)
		DEF_FUTURE(PF_GetPhysicsKeyValue)
		DEF_FUTURE(build_number)
		DEF_FUTURE(SV_Frame)
		DEF_FUTURE(Host_Loadgame_f)
		DEF_FUTURE(Host_Reload_f)
		DEF_FUTURE(VGuiWrap2_ConDPrintf)
		DEF_FUTURE(VGuiWrap2_ConPrintf)
		DEF_FUTURE(CL_RecordHUDCommand)
		#undef DEF_FUTURE

		bool oldEngine = (m_Name.find(L"hl.exe") != std::wstring::npos);
		std::future<typename decltype(patterns::engine::LoadAndDecryptHwDLL)::const_iterator> fLoadAndDecryptHwDLL;
		if (oldEngine) {
			// In WON after the engine DLL has been loaded once for some reason there are multiple identical LoadAndDecrypt functions in the memory, we need the first one always.
			fLoadAndDecryptHwDLL = std::async([&]() {
					return MemUtils::find_first_sequence(
						m_Base,
						m_Length,
						patterns::engine::LoadAndDecryptHwDLL.cbegin(),
						patterns::engine::LoadAndDecryptHwDLL.cend(),
						ORIG_LoadAndDecryptHwDLL);
				});
		}

		auto fCbuf_Execute = FindAsync(
			ORIG_Cbuf_Execute,
			patterns::engine::Cbuf_Execute,
			[&](auto pattern) {
				switch (pattern - patterns::engine::Cbuf_Execute.cbegin())
				{
				case 0:
					cmd_text = reinterpret_cast<cmdbuf_t*>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cbuf_Execute) + 11) - offsetof(cmdbuf_t, cursize));
					break;
				case 1:
					cmd_text = reinterpret_cast<cmdbuf_t*>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cbuf_Execute) + 2) - offsetof(cmdbuf_t, cursize));
					break;
				}
			});

		auto fSeedRandomNumberGenerator = FindAsync(
			ORIG_SeedRandomNumberGenerator,
			patterns::engine::SeedRandomNumberGenerator,
			[&](auto pattern) {
				ORIG_time = reinterpret_cast<_time>(
					*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_SeedRandomNumberGenerator) + 3)
					+ reinterpret_cast<uintptr_t>(ORIG_SeedRandomNumberGenerator) + 7
				);
			});

		auto fCL_Stop_f = FindAsync(
			ORIG_CL_Stop_f,
			patterns::engine::CL_Stop_f,
			[&](auto pattern) {
				ptrdiff_t offset;
				switch (pattern - patterns::engine::CL_Stop_f.cbegin())
				{
				default:
				case 0: // SteamPipe
					offset = 25;
					break;

				case 1: // NGHL
					offset = 22;
					break;
				}

				demorecording = *reinterpret_cast<int**>(reinterpret_cast<uintptr_t>(ORIG_CL_Stop_f) + offset);
			});

		void *SCR_DrawFPS;
		auto fSCR_DrawFPS = FindAsync(
			SCR_DrawFPS,
			patterns::engine::SCR_DrawFPS,
			[&](auto pattern) {
				host_frametime = *reinterpret_cast<double**>(reinterpret_cast<uintptr_t>(SCR_DrawFPS) + 21);
			});

		void *CL_Move;
		auto fCL_Move = FindAsync(
			CL_Move,
			patterns::engine::CL_Move,
			[&](auto pattern) {
				frametime_remainder = *reinterpret_cast<double**>(reinterpret_cast<uintptr_t>(CL_Move) + 451);
			});

		void *Host_Tell_f;
		auto fHost_Tell_f = FindAsync(
			Host_Tell_f,
			patterns::engine::Host_Tell_f,
			[&](auto pattern) {
				uintptr_t offCmd_Argc, offCmd_Args, offCmd_Argv;
				switch (pattern - patterns::engine::Host_Tell_f.cbegin())
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
				case 3: // HL-WON
					offCmd_Argc = 25;
					offCmd_Args = 78;
					offCmd_Argv = 151;
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
			});

		void *Host_AutoSave_f;
		auto fHost_AutoSave_f = FindAsync(
			Host_AutoSave_f,
			patterns::engine::Host_AutoSave_f,
			[&](auto pattern) {
				auto f = reinterpret_cast<uintptr_t>(Host_AutoSave_f);
				sv = *reinterpret_cast<void**>(f + 19);
				offWorldmodel = 304;
				ORIG_Con_Printf = reinterpret_cast<_Con_Printf>(
					*reinterpret_cast<ptrdiff_t*>(f + 33)
					+ (f + 37)
					);
				cls = *reinterpret_cast<void**>(f + 69);
				svs = reinterpret_cast<svs_t*>(*reinterpret_cast<uintptr_t*>(f + 45) - 8);
				offEdict = *reinterpret_cast<ptrdiff_t*>(f + 122);
				clientstate = reinterpret_cast<void*>(*reinterpret_cast<uintptr_t*>(f + 86) - 0x2AF80);
			});

		void *MiddleOfSV_ReadClientMessage;
		auto fMiddleOfSV_ReadClientMessage = FindAsync(
			MiddleOfSV_ReadClientMessage,
			patterns::engine::MiddleOfSV_ReadClientMessage,
			[&](auto pattern) {
				switch (pattern - patterns::engine::MiddleOfSV_ReadClientMessage.cbegin())
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
				case 3: // WON-1712.
					host_client = *reinterpret_cast<client_t***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ReadClientMessage) + 17);
					svmove = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(MiddleOfSV_ReadClientMessage) + 43);
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ReadClientMessage) + 39);
					sv_player = *reinterpret_cast<edict_t***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ReadClientMessage) + 23);
				}
			});

		void *MiddleOfSV_RunCmd;
		auto fMiddleOfSV_RunCmd = FindAsync(
			MiddleOfSV_RunCmd,
			patterns::engine::MiddleOfSV_RunCmd,
			[&](auto pattern) {
				sv_areanodes = *reinterpret_cast<char**>(reinterpret_cast<uintptr_t>(MiddleOfSV_RunCmd) + 20);
				ORIG_SV_AddLinksToPM = reinterpret_cast<_SV_AddLinksToPM>(
					*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(MiddleOfSV_RunCmd) + 25)
					+ reinterpret_cast<uintptr_t>(MiddleOfSV_RunCmd) + 29);
			});

		auto fHost_Changelevel2_f = FindAsync(
			ORIG_Host_Changelevel2_f,
			patterns::engine::Host_Changelevel2_f,
			[&](auto pattern) {
				switch (pattern - patterns::engine::Host_Changelevel2_f.cbegin())
				{
				case 0: // SteamPipe.
					ORIG_SV_SpawnServer = reinterpret_cast<_SV_SpawnServer>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Host_Changelevel2_f) + 285)
						+ reinterpret_cast<uintptr_t>(ORIG_Host_Changelevel2_f) + 289);
					break;
				case 1: // WON.
					ORIG_SV_SpawnServer = reinterpret_cast<_SV_SpawnServer>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Host_Changelevel2_f) + 274)
						+ reinterpret_cast<uintptr_t>(ORIG_Host_Changelevel2_f) + 278);
					break;
				case 2: // NGHL.
					ORIG_SV_SpawnServer = reinterpret_cast<_SV_SpawnServer>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Host_Changelevel2_f) + 296)
						+ reinterpret_cast<uintptr_t>(ORIG_Host_Changelevel2_f) + 300);
					break;
				case 3: // WON-2.
					ORIG_SV_SpawnServer = reinterpret_cast<_SV_SpawnServer>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Host_Changelevel2_f) + 284)
						+ reinterpret_cast<uintptr_t>(ORIG_Host_Changelevel2_f) + 288);
					break;
				case 4: // WON-3.
					ORIG_SV_SpawnServer = reinterpret_cast<_SV_SpawnServer>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Host_Changelevel2_f) + 248)
						+ reinterpret_cast<uintptr_t>(ORIG_Host_Changelevel2_f) + 252);
					break;
				}
			});

		{
			auto pattern = fCbuf_Execute.get();
			if (ORIG_Cbuf_Execute) {
				EngineDevMsg("[hw dll] Found Cbuf_Execute at %p (using the %s pattern).\n", ORIG_Cbuf_Execute, pattern->name());
				EngineDevMsg("[hw dll] Found cmd_text at %p.\n", cmd_text);
			} else {
				EngineDevWarning("[hw dll] Could not find Cbuf_Execute.\n");
			}
		}

		{
			auto pattern = fHost_AutoSave_f.get();
			if (Host_AutoSave_f) {
				EngineDevMsg("[hw dll] Found Host_AutoSave_f at %p (using the %s pattern).\n", Host_AutoSave_f, pattern->name());
				EngineDevMsg("[hw dll] Found cls at %p.\n", cls);
				EngineDevMsg("[hw dll] Found clientstate at %p.\n", clientstate);
				EngineDevMsg("[hw dll] Found sv at %p.\n", sv);
				EngineDevMsg("[hw dll] Found svs at %p.\n", svs);
				EngineDevMsg("[hw dll] Found Con_Printf at %p.\n", ORIG_Con_Printf);
			} else {
				EngineDevWarning("[hw dll] Could not find Host_AutoSave_f.\n");
				ORIG_Cbuf_Execute = nullptr;
			}
		}

		{
			auto pattern = fSeedRandomNumberGenerator.get();
			if (ORIG_SeedRandomNumberGenerator) {
				EngineDevMsg("[hw dll] Found SeedRandomNumberGenerator at %p (using the %s pattern).\n", ORIG_SeedRandomNumberGenerator, pattern->name());
				EngineDevMsg("[hw dll] ORIG_time is %p.\n", ORIG_time);
			} else {
				EngineDevWarning("[hw dll] Could not find SeedRandomNumberGenerator.\n");
				ORIG_Cbuf_Execute = nullptr;
			}
		}

		{
			auto pattern = fSCR_DrawFPS.get();
			if (SCR_DrawFPS) {
				EngineDevMsg("[hw dll] Found SCR_DrawFPS at %p (using the %s pattern).\n", SCR_DrawFPS, pattern->name());
				EngineDevMsg("[hw dll] Found host_frametime at %p.\n", host_frametime);
			} else {
				EngineDevWarning("[hw dll] Could not find SCR_DrawFPS.\n");
				ORIG_Cbuf_Execute = nullptr;
			}
		}

		{
			auto pattern = fCL_Move.get();
			if (CL_Move) {
				EngineDevMsg("[hw dll] Found CL_Move at %p (using the %s pattern).\n", CL_Move, pattern->name());
				EngineDevMsg("[hw dll] Found frametime_remainder at %p.\n", frametime_remainder);
			} else {
				EngineDevWarning("[hw dll] Could not find CL_Move.\n");
			}
		}

		{
			auto pattern = fMiddleOfSV_ReadClientMessage.get();
			if (MiddleOfSV_ReadClientMessage) {
				EngineDevMsg("[hw dll] Found the g_svmove pattern at %p (using the %s pattern).\n", MiddleOfSV_ReadClientMessage, pattern->name());
				EngineDevMsg("[hw dll] Found g_svmove at %p.\n", svmove);
				EngineDevMsg("[hw dll] Found pmove at %p.\n", ppmove);
				EngineDevMsg("[hw dll] Found host_client at %p.\n", host_client);
				EngineDevMsg("[hw dll] Found sv_player at %p.\n", sv_player);
			} else {
				EngineDevWarning("[hw dll] Could not find the g_svmove pattern.\n");
				ORIG_Cbuf_Execute = nullptr;
			}
		}

		{
			auto pattern = fMiddleOfSV_RunCmd.get();
			if (MiddleOfSV_RunCmd) {
				EngineDevMsg("[hw dll] Found the sv_areanodes pattern at %p (using the %s pattern).\n", MiddleOfSV_RunCmd, pattern->name());
				EngineDevMsg("[hw dll] Found sv_areanodes at %p.\n", sv_areanodes);
				EngineDevMsg("[hw dll] Found SV_AddLinksToPM at %p.\n", ORIG_SV_AddLinksToPM);
			} else {
				EngineDevWarning("[hw dll] Could not find the sv_areanodes pattern.\n");
				ORIG_Cbuf_Execute = nullptr;
			}
		}

		{
			auto pattern = fHost_Changelevel2_f.get();
			if (ORIG_Host_Changelevel2_f) {
				EngineDevMsg("[hw dll] Found Host_Changelevel2_f at %p (using the %s pattern).\n", ORIG_Host_Changelevel2_f, pattern->name());
				if (ORIG_SV_SpawnServer) {
					EngineDevMsg("[hw dll] Found SV_SpawnServer at %p.\n", ORIG_SV_SpawnServer);
				} else {
					EngineDevWarning("[hw dll] Could not find SV_SpawnServer.\n");
					EngineWarning("BunnySplit autosplitting is not available.\n");
				}
			} else {
				EngineDevWarning("[hw dll] Could not find Host_Changelevel2_f.\n");
				ORIG_Cbuf_Execute = nullptr;
			}
		}

		{
			auto pattern = fCL_Stop_f.get();
			if (ORIG_CL_Stop_f) {
				EngineDevMsg("[hw dll] Found CL_Stop_f at %p (using the %s pattern).\n", ORIG_CL_Stop_f, pattern->name());
				EngineDevMsg("[hw dll] Found demorecording at %p.\n", demorecording);
			} else {
				EngineDevWarning("[hw dll] Could not find CL_Stop_f.\n");
				ORIG_Cbuf_Execute = nullptr;
			}
		}

		#define GET_FUTURE(future_name) \
			{ \
				auto pattern = f##future_name.get(); \
				if (ORIG_##future_name) { \
					EngineDevMsg("[hw dll] Found " #future_name " at %p (using the %s pattern).\n", ORIG_##future_name, pattern->name()); \
				} else { \
					EngineDevWarning("[hw dll] Could not find " #future_name ".\n"); \
					ORIG_Cbuf_Execute = nullptr; \
				} \
			}
		GET_FUTURE(Cvar_RegisterVariable)
		GET_FUTURE(Cvar_DirectSet)
		GET_FUTURE(Cvar_FindVar)
		GET_FUTURE(Cbuf_InsertText)
		GET_FUTURE(Cbuf_AddText)
		GET_FUTURE(Cmd_AddMallocCommand)
		//GET_FUTURE(RandomFloat)
		//GET_FUTURE(RandomLong)
		GET_FUTURE(SCR_BeginLoadingPlaque)
		GET_FUTURE(PM_PlayerTrace)
		GET_FUTURE(Host_Loadgame_f)
		GET_FUTURE(Host_Reload_f)
		GET_FUTURE(CL_RecordHUDCommand)
		#undef GET_FUTURE

		{
			auto pattern = fHost_Tell_f.get();
			if (Host_Tell_f) {
				EngineDevMsg("[hw dll] Found Host_Tell_f at %p (using the %s pattern).\n", Host_Tell_f, pattern->name());
				EngineDevMsg("[hw dll] Found Cmd_Argc at %p.\n", ORIG_Cmd_Argc);
				EngineDevMsg("[hw dll] Found Cmd_Args at %p.\n", ORIG_Cmd_Args);
				EngineDevMsg("[hw dll] Found Cmd_Argv at %p.\n", ORIG_Cmd_Argv);
			} else {
				EngineDevWarning("[hw dll] Could not find Host_Tell_f.\n");
				ORIG_Cmd_AddMallocCommand = nullptr;
			}
		}

		#define GET_FUTURE(future_name) \
			{ \
				auto pattern = f##future_name.get(); \
				if (ORIG_##future_name) { \
					EngineDevMsg("[hw dll] Found " #future_name " at %p (using the %s pattern).\n", ORIG_##future_name, pattern->name()); \
				} else { \
					EngineDevWarning("[hw dll] Could not find " #future_name ".\n"); \
				} \
			}
		GET_FUTURE(Host_FilterTime);
		GET_FUTURE(V_FadeAlpha);
		GET_FUTURE(SV_Frame);
		GET_FUTURE(VGuiWrap2_ConDPrintf);
		GET_FUTURE(VGuiWrap2_ConPrintf);
		GET_FUTURE(SCR_UpdateScreen);
		GET_FUTURE(PF_GetPhysicsKeyValue);
		GET_FUTURE(build_number);

		if (oldEngine) {
			GET_FUTURE(LoadAndDecryptHwDLL);
		}

		#undef GET_FUTURE
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

struct HwDLL::Cmd_BXT_TAS_LoadScript
{
	USAGE("Usage: bxt_tas_loadscript <filename>\n");

	static void handler(const char *fileName)
	{
		auto &hw = HwDLL::GetInstance();
		if (hw.resetState != ResetState::NORMAL)
			return;

		hw.runningFrames = false;
		hw.currentFramebulk = 0;
		hw.currentRepeat = 0;
		hw.StrafeState = HLStrafe::CurrentState();
		hw.ButtonsPresent = false;
		hw.demoName.clear();
		hw.saveName.clear();
		hw.frametime0ms.clear();
		hw.SharedRNGSeedPresent = false;
		hw.SetNonSharedRNGSeed = false;
		hw.thisFrameIs0ms = false;

		auto err = hw.input.Open(fileName).get();
		if (err.Code != HLTAS::ErrorCode::OK) {
			hw.ORIG_Con_Printf("Error loading the script file on line %u: %s\n", err.LineNumber, HLTAS::GetErrorMessage(err).c_str());
			return;
		}

		for (auto prop : hw.input.GetProperties()) {
			if (prop.first == "demo")
				hw.demoName = prop.second;
			else if (prop.first == "save")
				hw.saveName = prop.second;
			else if (prop.first == "seed") {
				std::istringstream ss(prop.second);
				ss >> hw.SharedRNGSeed >> hw.NonSharedRNGSeed;
				hw.SharedRNGSeedPresent = true;
				hw.SetNonSharedRNGSeed = true;
			} else if (prop.first == "frametime0ms")
				hw.frametime0ms = prop.second;
		}

		if (!hw.input.GetFrames().empty()) {
			hw.runningFrames = true;
			hw.totalFramebulks = hw.input.GetFrames().size();
			HLTAS::Frame f;
			if (hw.GetNextMovementFrame(f)) {
				std::ostringstream ss;
				ss << "host_framerate " << f.Frametime.c_str() << "\n";
				hw.ORIG_Cbuf_InsertText(ss.str().c_str());
			}
		}
	}
};

struct HwDLL::Cmd_BXT_CH_Set_Health
{
	USAGE("Usage: bxt_ch_set_health <health>\n");

	static void handler(float health)
	{
		auto &hw = HwDLL::GetInstance();
		(*hw.sv_player)->v.health = health;
	}
};

struct HwDLL::Cmd_BXT_CH_Set_Armor
{
	USAGE("Usage: bxt_ch_set_armor <armor>\n");

	static void handler(float armor)
	{
		auto &hw = HwDLL::GetInstance();
		(*hw.sv_player)->v.armorvalue = armor;
	}
};

struct HwDLL::Cmd_BXT_CH_Set_Origin
{
	USAGE("Usage: bxt_ch_set_pos <x> <y> <z>\n");

	static void handler(float x, float y, float z)
	{
		auto &hw = HwDLL::GetInstance();
		(*hw.sv_player)->v.origin[0] = x;
		(*hw.sv_player)->v.origin[1] = y;
		(*hw.sv_player)->v.origin[2] = z;
	}
};

struct HwDLL::Cmd_BXT_CH_Set_Velocity
{
	USAGE("Usage: bxt_ch_set_vel <x> <y> <z>\n");

	static void handler(float vx, float vy, float vz)
	{
		auto &hw = HwDLL::GetInstance();
		(*hw.sv_player)->v.velocity[0] = vx;
		(*hw.sv_player)->v.velocity[1] = vy;
		(*hw.sv_player)->v.velocity[2] = vz;
	}
};

struct HwDLL::Cmd_BXT_CH_Set_Velocity_Angles
{
	USAGE("Usage:\n bxt_ch_set_vel_angles <magnitude>\n bxt_ch_set_vel_angles <pitch> <yaw> <magnitude>\n");

	static void handler(float magnitude)
	{
		auto &hw = HwDLL::GetInstance();
		handler((*hw.sv_player)->v.v_angle[0], (*hw.sv_player)->v.v_angle[1], magnitude);
	}

	static void handler(float pitch, float yaw, float magnitude)
	{
		auto &hw = HwDLL::GetInstance();
		pitch *= static_cast<float>(M_PI / 180.0);
		yaw *= static_cast<float>(M_PI / 180.0);
		(*hw.sv_player)->v.velocity[0] = magnitude * std::cos(pitch) * std::cos(yaw);
		(*hw.sv_player)->v.velocity[1] = magnitude * std::cos(pitch) * std::sin(yaw);
		(*hw.sv_player)->v.velocity[2] = -magnitude * std::sin(pitch);
	}
};

struct HwDLL::Cmd_BXT_CH_Set_Origin_Offset
{
	USAGE("Usage: bxt_ch_set_pos_offset <dx> <dy> <dz>\n");

	static void handler(float dx, float dy, float dz)
	{
		auto &hw = HwDLL::GetInstance();
		(*hw.sv_player)->v.origin[0] += dx;
		(*hw.sv_player)->v.origin[1] += dy;
		(*hw.sv_player)->v.origin[2] += dz;
	}
};

struct HwDLL::Cmd_Multiwait
{
	USAGE("Usage: w [number of waits]\n");

	static void handler()
	{
		HwDLL::GetInstance().ORIG_Cbuf_InsertText("wait\n");
	}

	static void handler(int num)
	{
		std::ostringstream ss;
		if (num > 1)
			ss << "wait\nw " << num - 1 << '\n';
		else if (num == 1)
			ss << "wait\n";
		else
			return;

		HwDLL::GetInstance().ORIG_Cbuf_InsertText(ss.str().c_str());
	}
};

struct HwDLL::Cmd_BXT_Camera_Fixed
{
	USAGE("Usage: bxt_cam_fixed <x> <y> <z> <pitch> <yaw> <magnitude>\n");

	static void handler(float x, float y, float z, float pitch, float yaw, float roll)
	{
		auto &hw = HwDLL::GetInstance();
		hw.isOverridingCamera = true;
		hw.isOffsettingCamera = false;
		hw.cameraOverrideOrigin[0] = x;
		hw.cameraOverrideOrigin[1] = y;
		hw.cameraOverrideOrigin[2] = z;
		hw.cameraOverrideAngles[0] = pitch;
		hw.cameraOverrideAngles[1] = yaw;
		hw.cameraOverrideAngles[2] = roll;
	}
};

struct HwDLL::Cmd_BXT_Camera_Clear
{
	NO_USAGE();

	static void handler()
	{
		auto &hw = HwDLL::GetInstance();
		hw.isOverridingCamera = false;
		hw.isOffsettingCamera = false;
	}
};

struct HwDLL::Cmd_BXT_Camera_Offset
{
	USAGE("Usage: bxt_cam_offset <x> <y> <z> <pitch> <yaw> <roll>\n");

	static void handler(float x, float y, float z, float pitch, float yaw, float roll)
	{
		auto &hw = HwDLL::GetInstance();
		hw.isOverridingCamera = false;
		hw.isOffsettingCamera = true;
		hw.cameraOffsetOrigin[0] = x;
		hw.cameraOffsetOrigin[1] = y;
		hw.cameraOffsetOrigin[2] = z;
		hw.cameraOffsetAngles[0] = pitch;
		hw.cameraOffsetAngles[1] = yaw;
		hw.cameraOffsetAngles[2] = roll;
	}
};

struct HwDLL::Cmd_BXT_Timer_Start
{
	NO_USAGE();

	static void handler()
	{
		return CustomHud::SetCountingTime(true);
	}
};

struct HwDLL::Cmd_BXT_Timer_Stop
{
	NO_USAGE();

	static void handler()
	{
		return CustomHud::SetCountingTime(false);
	}
};

struct HwDLL::Cmd_BXT_Timer_Reset
{
	NO_USAGE();

	static void handler()
	{
		return CustomHud::ResetTime();
	}
};

struct HwDLL::Cmd_BXT_TAS_Autojump_Down
{
	NO_USAGE();

	static void handler()
	{
		HwDLL::GetInstance().autojump = true;
	}

	static void handler(const char*)
	{
		HwDLL::GetInstance().autojump = true;
	}
};

struct HwDLL::Cmd_BXT_TAS_Autojump_Up
{
	NO_USAGE();

	static void handler()
	{
		HwDLL::GetInstance().autojump = false;
	}

	static void handler(const char*)
	{
		HwDLL::GetInstance().autojump = false;
	}
};

struct HwDLL::Cmd_BXT_TAS_Ducktap_Down
{
	NO_USAGE();

	static void handler()
	{
		HwDLL::GetInstance().ducktap = true;
	}

	static void handler(const char*)
	{
		HwDLL::GetInstance().ducktap = true;
	}
};

struct HwDLL::Cmd_BXT_TAS_Ducktap_Up
{
	NO_USAGE();

	static void handler()
	{
		HwDLL::GetInstance().ducktap = false;
	}

	static void handler(const char*)
	{
		HwDLL::GetInstance().ducktap = false;
	}
};

struct HwDLL::Cmd_BXT_Record
{
	USAGE("Usage: bxt_record <demoname>\n");

	static void handler(const char *demoName)
	{
		auto &hw = HwDLL::GetInstance();
		hw.recordDemoName.clear();
		hw.recordDemoName.assign(demoName);
	}
};

struct HwDLL::Cmd_BXT_AutoRecord
{
	USAGE("Usage: bxt_autorecord <demoname>\n Records demoname_1; if a load (or changelevel on older engines) occurs - automatically records demoname_2, and so on.\n");

	static void handler(const char *demoName)
	{
		auto &hw = HwDLL::GetInstance();
		hw.autoRecordDemoName.assign(demoName);
		hw.autoRecordDemoNumber = 1;
		hw.autoRecordNow = true;
	}
};

struct HwDLL::Cmd_BXT_Interprocess_Reset
{
	NO_USAGE();

	static void handler()
	{
		Interprocess::Shutdown();
		Interprocess::Initialize();
	}
};

struct HwDLL::Cmd_BXT_Map
{
	USAGE("Usage: _bxt_map <mapname>\n");

	static void handler(const char *mapName)
	{
		auto &hw = HwDLL::GetInstance();

		// This version of map doesn't trigger after reset frames
		// when put in the command line args.
		if (hw.resetState != ResetState::NORMAL)
			return;

		std::ostringstream ss;
		ss << "map" << " " << mapName << "\n";
		hw.ORIG_Cbuf_InsertText(ss.str().c_str());
	}
};

struct HwDLL::Cmd_BXT_Load
{
	USAGE("Usage: _bxt_load <savename>\n");

	static void handler(const char *saveName)
	{
		auto &hw = HwDLL::GetInstance();

		// This version of load doesn't trigger after reset frames
		// when put in the command line args.
		if (hw.resetState != ResetState::NORMAL)
			return;

		std::ostringstream ss;
		ss << "load" << " " << saveName << "\n";
		hw.ORIG_Cbuf_InsertText(ss.str().c_str());
	}
};

struct HwDLL::Cmd_BXT_TASLog
{
	NO_USAGE();

	static void handler()
	{
		auto &hw = HwDLL::GetInstance();
		if (!hw.ORIG_SV_Frame) {
			hw.ORIG_Con_Printf("TAS logging is unavailable.\n");
			return;
		}

		if (hw.tasLogging) {
			hw.logWriter.EndLog();
			std::fclose(hw.tasLogFile);
			hw.logWriter.Clear();
			hw.tasLogging = false;
		} else {
			hw.tasLogFile = std::fopen(CVars::bxt_taslog_filename.GetString().c_str(), "wb");
			if (!hw.tasLogFile) {
				hw.ORIG_Con_Printf("Unable to create log file: %s\n", std::strerror(errno));
				return;
			}
			const int buildNumber = hw.ORIG_build_number ? hw.ORIG_build_number() : -1;
			const char *gameDir = ClientDLL::GetInstance().pEngfuncs->pfnGetGameDirectory();
			hw.logWriter.StartLog(hw.tasLogFile, BUNNYMODXT_VERSION, buildNumber, gameDir);
			hw.tasLogging = true;
		}
	}
};

struct HwDLL::Cmd_BXT_Reset_Frametime_Remainder
{
	NO_USAGE();

	static void handler()
	{
		if (HwDLL::GetInstance().frametime_remainder)
			*HwDLL::GetInstance().frametime_remainder = 0;
	}
};

struct HwDLL::Cmd_BXT_Append
{
	USAGE("Usage: bxt_append <command>\n Appends command to the end of the command buffer, similar to how special appends _special.\n");

	static void handler(const char *command)
	{
		auto& hw = HwDLL::GetInstance();

		hw.ORIG_Cbuf_AddText(command);
		hw.ORIG_Cbuf_AddText("\n");
	}
};

void HwDLL::RegisterCVarsAndCommandsIfNeeded()
{
	if (registeredVarsAndCmds)
		return;

	registeredVarsAndCmds = true;
	RegisterCVar(CVars::_bxt_taslog);
	RegisterCVar(CVars::_bxt_min_frametime);
	RegisterCVar(CVars::bxt_taslog_filename);
	RegisterCVar(CVars::bxt_autopause);
	RegisterCVar(CVars::bxt_interprocess_enable);
	RegisterCVar(CVars::bxt_fade_remove);
	RegisterCVar(CVars::_bxt_norefresh);
	RegisterCVar(CVars::_bxt_bunnysplit_time_update_frequency);

	if (!ORIG_Cmd_AddMallocCommand)
		return;

	using CmdWrapper::Handler;
	typedef CmdWrapper::CmdWrapper<CmdFuncs> wrapper;

	wrapper::Add<Cmd_BXT_TAS_LoadScript, Handler<const char *>>("bxt_tas_loadscript");
	wrapper::AddCheat<Cmd_BXT_CH_Set_Health, Handler<float>>("bxt_ch_set_health");
	wrapper::AddCheat<Cmd_BXT_CH_Set_Armor, Handler<float>>("bxt_ch_set_armor");
	wrapper::AddCheat<Cmd_BXT_CH_Set_Origin, Handler<float, float, float>>("bxt_ch_set_pos");
	wrapper::AddCheat<Cmd_BXT_CH_Set_Origin_Offset, Handler<float, float, float>>("bxt_ch_set_pos_offset");
	wrapper::AddCheat<Cmd_BXT_CH_Set_Velocity, Handler<float, float, float>>("bxt_ch_set_vel");
	wrapper::AddCheat<
		Cmd_BXT_CH_Set_Velocity_Angles,
		Handler<float>,
		Handler<float, float, float>>("bxt_ch_set_vel_angles");
	wrapper::Add<
		Cmd_Multiwait,
		Handler<>,
		Handler<int>>("w");
	wrapper::Add<Cmd_BXT_Camera_Fixed, Handler<float, float, float, float, float, float>>("bxt_cam_fixed");
	wrapper::Add<Cmd_BXT_Camera_Offset, Handler<float, float, float, float, float, float>>("bxt_cam_offset");
	wrapper::Add<Cmd_BXT_Camera_Clear, Handler<>>("bxt_cam_clear");
	wrapper::Add<Cmd_BXT_Timer_Start, Handler<>>("bxt_timer_start");
	wrapper::Add<Cmd_BXT_Timer_Stop, Handler<>>("bxt_timer_stop");
	wrapper::Add<Cmd_BXT_Timer_Reset, Handler<>>("bxt_timer_reset");
	wrapper::Add<Cmd_BXT_TAS_Autojump_Down, Handler<>, Handler<const char*>>("+bxt_tas_autojump");
	wrapper::Add<Cmd_BXT_TAS_Autojump_Up, Handler<>, Handler<const char*>>("-bxt_tas_autojump");
	wrapper::Add<Cmd_BXT_TAS_Ducktap_Down, Handler<>, Handler<const char*>>("+bxt_tas_ducktap");
	wrapper::Add<Cmd_BXT_TAS_Ducktap_Up, Handler<>, Handler<const char*>>("-bxt_tas_ducktap");
	wrapper::Add<Cmd_BXT_Record, Handler<const char *>>("bxt_record");
	wrapper::Add<Cmd_BXT_AutoRecord, Handler<const char *>>("bxt_autorecord");
	wrapper::Add<Cmd_BXT_Map, Handler<const char *>>("_bxt_map");
	wrapper::Add<Cmd_BXT_Load, Handler<const char *>>("_bxt_load");
	wrapper::Add<Cmd_BXT_Interprocess_Reset, Handler<>>("_bxt_interprocess_reset");
	wrapper::Add<Cmd_BXT_Reset_Frametime_Remainder, Handler<>>("_bxt_reset_frametime_remainder");
	wrapper::Add<Cmd_BXT_TASLog, Handler<>>("bxt_taslog");
	wrapper::Add<Cmd_BXT_Append, Handler<const char *>>("bxt_append");
}

void HwDLL::InsertCommands()
{
	bool runningFramesBackup = runningFrames;

	if (runningFrames && resetState == ResetState::NORMAL) {
		while (currentFramebulk < totalFramebulks) {
			preExecFramebulk = currentFramebulk;
			auto& f = input.GetFrame(currentFramebulk);
			// Movement frame.
			if (currentRepeat || (f.SaveName.empty() && !f.SeedPresent && f.BtnState == HLTAS::ButtonState::NOTHING && !f.LgagstMinSpeedPresent && !f.ResetFrame)) {
				auto c = f.Commands;
				if (!c.empty()) {
					c += '\n';
					ORIG_Cbuf_InsertText(c.c_str());
				}

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

						if (ORIG_PF_GetPhysicsKeyValue) {
							auto slj = std::atoi(ORIG_PF_GetPhysicsKeyValue(pl, "slj"));
							player.HasLJModule = (slj == 1);
						} else {
							player.HasLJModule = false;
						}

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

				// If we're at state = 4, do not do anything with angles, otherwise there's
				// an edge case where the first frame of state = 5 has in_left and in_right inactive
				// even though we want them to be active.
				if (*reinterpret_cast<int*>(cls) == 5) {
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
				}

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
				}

				if (p.NextFrameIs0ms) {
					if (!thisFrameIs0ms) {
						std::ostringstream ss;
						ss << "host_framerate " << frametime0ms << "\n";
						ORIG_Cbuf_InsertText(ss.str().c_str());
					}
				} else if (currentRepeat == 0 || thisFrameIs0ms) {
					// This will get the current framebulk and return the framerate back from 0ms
					// if we didn't switch to the next framebulk yet.
					HLTAS::Frame next;
					if (GetNextMovementFrame(next)) {
						if (next.Frametime != f.Frametime || thisFrameIs0ms) {
							std::ostringstream ss;
							ss << "host_framerate " << next.Frametime.c_str() << "\n";
							ORIG_Cbuf_InsertText(ss.str().c_str());
						}
					}
				}

				thisFrameIs0ms = p.NextFrameIs0ms;

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
			} else if (f.ResetFrame) { // Reset frame.
				resetState = ResetState::PRERESET;
				NonSharedRNGSeed = f.GetResetNonSharedRNGSeed();
				SetNonSharedRNGSeed = true;
				ORIG_Cbuf_InsertText("_restart\n");

				// Stop a demo manually if one was going on, otherwise it ends up corrupt.
				ORIG_Cbuf_InsertText("stop\n");

				currentFramebulk++;
				break;
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

					if (ORIG_PF_GetPhysicsKeyValue) {
						auto slj = std::atoi(ORIG_PF_GetPhysicsKeyValue(pl, "slj"));
						player.HasLJModule = (slj == 1);
					} else {
						player.HasLJModule = false;
					}

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

	if (*reinterpret_cast<int*>(cls) == 5) {
		if (!recordDemoName.empty()) {
			std::ostringstream ss;
			ss << "record " << recordDemoName.c_str() << "\n";
			ORIG_Cbuf_InsertText(ss.str().c_str());
			recordDemoName.clear();
		}

		if (autoRecordNow) {
			std::ostringstream ss;
			ss << "record \"" << autoRecordDemoName.c_str() << '_' << autoRecordDemoNumber++ << "\"\n";
			ORIG_Cbuf_InsertText(ss.str().c_str());
			autoRecordNow = false;
		}
	}
}

bool HwDLL::GetNextMovementFrame(HLTAS::Frame& f)
{
	auto curFramebulk = currentFramebulk;
	while (curFramebulk < totalFramebulks) {
		f = input.GetFrame(curFramebulk);
		// Only movement frames can have repeats.
		if (currentRepeat || (f.SaveName.empty() && !f.SeedPresent && f.BtnState == HLTAS::ButtonState::NOTHING && !f.LgagstMinSpeedPresent && !f.ResetFrame))
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

	// Stuffcmds is inside valve.rc, which is executed by the very first Cbuf_Execute().
	// So everything that we wanted to not happen if we're resetting already did its checks now.
	// The original host_framerate was also already set from stuffcmds if it was there,
	// so we can set the correct one now.
	if (resetState == ResetState::POSTRESET) {
		resetState = ResetState::NORMAL;

		HLTAS::Frame next;
		if (GetNextMovementFrame(next)) {
			std::ostringstream ss;
			ss << "host_framerate " << next.Frametime << "\n";
			ORIG_Cbuf_InsertText(ss.str().c_str());
			ORIG_Cbuf_Execute();
		}
	}

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

		loggedCbuf.assign(cmd_text->data, cmd_text->cursize);

		ORIG_Cbuf_Execute();

		// If still executing (didn't load a save).
		if (executing && resetState != ResetState::PRERESET)
			CustomHud::TimePassed(*host_frametime);
	} else if (changelevel) {
		LoadingSeedCounter++;
	}
	insideCbuf_Execute = false;

	RuntimeData::SaveStored();

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

bool HwDLL::TryGettingAccurateInfo(float origin[3], float velocity[3])
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

void HwDLL::StoreCommand(const char* command)
{
	if (!ORIG_CL_RecordHUDCommand || !demorecording || *demorecording != 1)
		return;

	ORIG_CL_RecordHUDCommand(command);
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

	if (ORIG_Cmd_Argc && ORIG_Cmd_Argc() > 1 && ORIG_Cmd_Argv) {
		if (lastLoadedMap == "ba_power1"s && !std::strcmp(ORIG_Cmd_Argv(1), "ba_teleport2")) {
			int state;
			if (ServerDLL::GetInstance().GetGlobalState("powercomplete"s, state) && state == 1) {
				Interprocess::WriteBSALeapOfFaith(CustomHud::GetTime());
			}
		}
	}

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
	if (minFrametime == 0.0f || CVars::_bxt_norefresh.GetBool()) {
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

HOOK_DEF_0(HwDLL, int, __cdecl, V_FadeAlpha)
{
	if (CVars::bxt_fade_remove.GetBool())
		return 0;
	else
		return ORIG_V_FadeAlpha();
}

HOOK_DEF_3(HwDLL, int, __cdecl, SV_SpawnServer, int, bIsDemo, char*, server, char*, startspot)
{
	auto ret = ORIG_SV_SpawnServer(bIsDemo, server, startspot);
	if (ret) {
		Interprocess::WriteMapChange(CustomHud::GetTime(), server);
		lastLoadedMap = server;
	}

	if (insideHost_Loadgame_f) {
		if (ret && !autoRecordDemoName.empty()) {
			autoRecordNow = true;
		} else {
			autoRecordNow = false;
			autoRecordDemoName.clear();
		}
	}

	if (insideHost_Reload_f && !autoRecordDemoName.empty())
		autoRecordNow = true;

	return ret;
}

HOOK_DEF_0(HwDLL, void, __cdecl, SV_Frame)
{
	if (tasLogging) {
		const bool paused = *(reinterpret_cast<const int *>(sv) + 1) != 0;
		const int *clstate = reinterpret_cast<const int *>(cls);
		logWriter.StartPhysicsFrame(*host_frametime, *clstate, paused, loggedCbuf.c_str());
	}

	ORIG_SV_Frame();

	if (tasLogging)
		logWriter.EndPhysicsFrame();
}

HOOK_DEF_0(HwDLL, void, __cdecl, CL_Stop_f)
{
	if (!insideHost_Loadgame_f && !insideHost_Reload_f) {
		autoRecordNow = false;
		autoRecordDemoName.clear();
	}

	ORIG_CL_Stop_f();
}

HOOK_DEF_0(HwDLL, void, __cdecl, Host_Loadgame_f)
{
	insideHost_Loadgame_f = true;

	ORIG_Host_Loadgame_f();

	insideHost_Loadgame_f = false;
}

HOOK_DEF_0(HwDLL, void, __cdecl, Host_Reload_f)
{
	insideHost_Reload_f = true;

	ORIG_Host_Reload_f();

	insideHost_Reload_f = false;
}

HOOK_DEF_1(HwDLL, void, __cdecl, VGuiWrap2_ConDPrintf, const char*, msg)
{
	if (tasLogging)
		logWriter.PushConsolePrint(msg);
	ORIG_VGuiWrap2_ConDPrintf(msg);
}

HOOK_DEF_1(HwDLL, void, __cdecl, VGuiWrap2_ConPrintf, const char*, msg)
{
	if (tasLogging)
		logWriter.PushConsolePrint(msg);
	ORIG_VGuiWrap2_ConPrintf(msg);
}

HOOK_DEF_0(HwDLL, void, __cdecl, SCR_UpdateScreen)
{
	if (CVars::_bxt_norefresh.GetBool())
		return;
	else
		return ORIG_SCR_UpdateScreen();
}

HOOK_DEF_3(HwDLL, void, __cdecl, LoadAndDecryptHwDLL, int, a, void*, b, void*, c)
{
	ORIG_LoadAndDecryptHwDLL(a, b, c);
	EngineDevMsg("[hw dll] LoadAndDecryptHwDLL has been called. Rehooking.\n");
	Hooks::HookModule(L"hl.exe");
}
