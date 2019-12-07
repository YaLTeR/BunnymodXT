#include "../stdafx.hpp"

#include <boost/algorithm/string/replace.hpp>
#include <cerrno>
#include <GL/gl.h>
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
#include "../git_revision.hpp"
#include "../custom_triggers.hpp"

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

extern "C" void __cdecl CL_Record_f()
{
	HwDLL::HOOKED_CL_Record_f();
}

extern "C" void __cdecl Cbuf_AddText(const char* text)
{
	HwDLL::HOOKED_Cbuf_AddText(text);
}

extern "C" void __cdecl Cbuf_InsertTextLines(const char* text)
{
	HwDLL::HOOKED_Cbuf_InsertTextLines(text);
}

extern "C" void __cdecl Key_Event(int key, int down)
{
	HwDLL::HOOKED_Key_Event(key, down);
}

extern "C" void __cdecl Cmd_Exec_f()
{
	HwDLL::HOOKED_Cmd_Exec_f();
}

extern "C" void __cdecl Cmd_TokenizeString(char* text)
{
	HwDLL::HOOKED_Cmd_TokenizeString(text);
}

extern "C" void __cdecl R_DrawSequentialPoly(msurface_t *surf, int face)
{
	HwDLL::HOOKED_R_DrawSequentialPoly(surf, face);
}

extern "C" void __cdecl R_Clear()
{
	HwDLL::HOOKED_R_Clear();
}

extern "C" byte *__cdecl Mod_LeafPVS(mleaf_t *leaf, model_t *model)
{
	return HwDLL::HOOKED_Mod_LeafPVS(leaf, model);
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
			MemUtils::MarkAsExecutable(ORIG_Cbuf_AddText);
			MemUtils::MarkAsExecutable(ORIG_Cbuf_InsertTextLines);
			MemUtils::MarkAsExecutable(ORIG_Cmd_TokenizeString);
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
			MemUtils::MarkAsExecutable(ORIG_CL_Record_f);
			MemUtils::MarkAsExecutable(ORIG_Key_Event);
			MemUtils::MarkAsExecutable(ORIG_Cmd_Exec_f);
			MemUtils::MarkAsExecutable(ORIG_R_DrawSequentialPoly);
			MemUtils::MarkAsExecutable(ORIG_R_Clear);
			MemUtils::MarkAsExecutable(ORIG_Mod_LeafPVS);
		}

		MemUtils::Intercept(moduleName,
			ORIG_LoadAndDecryptHwDLL, HOOKED_LoadAndDecryptHwDLL,
			ORIG_Cbuf_Execute, HOOKED_Cbuf_Execute,
			ORIG_Cbuf_AddText, HOOKED_Cbuf_AddText,
			ORIG_Cbuf_InsertTextLines, HOOKED_Cbuf_InsertTextLines,
			ORIG_Cmd_TokenizeString, HOOKED_Cmd_TokenizeString,
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
			ORIG_VGuiWrap2_ConPrintf, HOOKED_VGuiWrap2_ConPrintf,
			ORIG_CL_Record_f, HOOKED_CL_Record_f,
			ORIG_Key_Event, HOOKED_Key_Event,
			ORIG_Cmd_Exec_f, HOOKED_Cmd_Exec_f,
			ORIG_R_DrawSequentialPoly, HOOKED_R_DrawSequentialPoly,
			ORIG_R_Clear, HOOKED_R_Clear,
			ORIG_Mod_LeafPVS, HOOKED_Mod_LeafPVS);
	}
}

void HwDLL::Unhook()
{
	if (m_Intercepted)
	{
		MemUtils::RemoveInterception(m_Name,
			ORIG_LoadAndDecryptHwDLL,
			ORIG_Cbuf_Execute,
			ORIG_Cbuf_AddText,
			ORIG_Cbuf_InsertTextLines,
			ORIG_Cmd_TokenizeString,
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
			ORIG_VGuiWrap2_ConPrintf,
			ORIG_CL_Record_f,
			ORIG_Key_Event,
			ORIG_Cmd_Exec_f,
			ORIG_R_DrawSequentialPoly,
			ORIG_R_Clear,
			ORIG_Mod_LeafPVS);
	}

	for (auto cvar : CVars::allCVars)
		cvar->Refresh();

	Clear();
}

void HwDLL::Clear()
{
	IHookableNameFilterOrdered::Clear();
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
	ORIG_Cbuf_InsertTextLines = nullptr;
	ORIG_Cmd_TokenizeString = nullptr;
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
	ORIG_CL_Record_f = nullptr;
	ORIG_build_number = nullptr;
	ORIG_Key_Event = nullptr;
	ORIG_Cmd_Exec_f = nullptr;
	ORIG_R_DrawSequentialPoly = nullptr;
	ORIG_R_Clear = nullptr;
	ORIG_Mod_LeafPVS = nullptr;

	registeredVarsAndCmds = false;
	autojump = false;
	ducktap = false;
	jumpbug = false;
	recordDemoName.clear();
	autoRecordDemoName.clear();
	autoRecordDemoNumber = 1;
	autoRecordNow = false;
	insideHost_Loadgame_f = false;
	insideHost_Reload_f = false;
	cls = nullptr;
	clientstate = nullptr;
	sv = nullptr;
	offTime = 0;
	offWorldmodel = 0;
	offModels = 0;
	offNumEdicts = 0;
	offEdicts = 0;
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
	insideKeyEvent = false;
	insideExec = false;
	execScript.clear();
	insideHost_Changelevel2_f = false;
	dontStopAutorecord = false;

	finding_coarse_nodes = false;
	coarse_nodes.clear();
	coarse_nodes_vector.clear();
	next_coarse_nodes = std::queue<CoarseNode>();
	finding_coarse_path = false;
	coarse_path_open_set.clear();
	coarse_path_nodes.clear();
	coarse_path_distances.clear();
	coarse_path_closed_set.clear();
	coarse_path_target = CoarseNode(0, 0, 0, 0, 0);
	coarse_path_final.clear();
	following_coarse_path = false;
	going_back_frames_left = 0;

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
		exportFilename.clear();
		exportResult.ClearProperties();
		exportResult.ClearFrames();
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
			offTime = 0xc;
			offWorldmodel = 296;
			offModels = 0x30948;
			offNumEdicts = 0x3bc50;
			offEdicts = 0x3bc58;
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

		cmd_alias = reinterpret_cast<cmdalias_t*>(MemUtils::GetSymbolAddress(m_Handle, "cmd_alias"));
		if (cmd_alias)
			EngineDevMsg("[hw dll] Found cmd_alias at %p.\n", cmd_alias);
		else
			EngineDevWarning("[hw dll] Could not find cmd_alias.\n");

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

		if (!cls || !sv || !svs || !svmove || !ppmove || !host_client || !sv_player || !sv_areanodes || !cmd_text || !cmd_alias || !host_frametime || !ORIG_hudGetViewAngles || !ORIG_SV_AddLinksToPM)
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
		FIND(Cbuf_InsertTextLines)
		FIND(Cmd_TokenizeString)
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
		FIND(CL_Record_f)
		FIND(Key_Event)
		FIND(Cmd_Exec_f)
		#undef FIND

		ORIG_Host_FilterTime = reinterpret_cast<_Host_FilterTime>(MemUtils::GetSymbolAddress(m_Handle, "Host_FilterTime"));
		if (ORIG_Host_FilterTime) {
			EngineDevMsg("[hw dll] Found Host_FilterTime at %p.\n", ORIG_Host_FilterTime);
			unsigned char bytes[] = { 0x90, 0xE9 };
			MemUtils::ReplaceBytes(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(ORIG_Host_FilterTime) + 0x2E), 2, bytes);
		} else
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

		ORIG_R_DrawSequentialPoly = reinterpret_cast<_R_DrawSequentialPoly>(MemUtils::GetSymbolAddress(m_Handle, "R_DrawSequentialPoly"));
		if (ORIG_R_DrawSequentialPoly) {
			EngineDevMsg("[hw dll] Found R_DrawSequentialPoly at %p.\n", ORIG_R_DrawSequentialPoly);
		} else {
			EngineDevWarning("[hw dll] Could not find R_DrawSequentialPoly.\n");
			EngineWarning("Wallhacking is not available.\n");
		}

		ORIG_R_Clear = reinterpret_cast<_R_Clear>(MemUtils::GetSymbolAddress(m_Handle, "R_Clear"));
		if (ORIG_R_Clear)
			EngineDevMsg("[hw dll] Found R_Clear at %p.\n", ORIG_R_Clear);
		else
			EngineDevWarning("[hw dll] Could not find R_Clear.\n");

		ORIG_Mod_LeafPVS = reinterpret_cast<_Mod_LeafPVS>(MemUtils::GetSymbolAddress(m_Handle, "Mod_LeafPVS"));
		if (ORIG_Mod_LeafPVS) {
			EngineDevMsg("[hw dll] Found Mod_LeafPVS at %p.\n", ORIG_Mod_LeafPVS);
		} else {
			EngineDevWarning("[hw dll] Could not find Mod_LeafPVS.\n");
			EngineWarning("bxt_novis has no effect.\n");
		}

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
		DEF_FUTURE(R_DrawSequentialPoly)
		DEF_FUTURE(R_Clear)
		DEF_FUTURE(Mod_LeafPVS)
		DEF_FUTURE(CL_RecordHUDCommand)
		DEF_FUTURE(CL_Record_f)
		DEF_FUTURE(Key_Event)
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
				case 0: // HL-SteamPipe-8183
					cmd_text = reinterpret_cast<cmdbuf_t*>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cbuf_Execute) + 3));
					break;
				case 1: // HL-SteamPipe
					cmd_text = reinterpret_cast<cmdbuf_t*>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cbuf_Execute) + 11) - offsetof(cmdbuf_t, cursize));
					break;
				case 2: // HL-NGHL
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
				offTime = 0x10;
				offWorldmodel = 304;
				offModels = 0x30950;
				offNumEdicts = 0x3bc58;
				offEdicts = 0x3bc60;
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
					break;
				case 3: // WON-1712.
					host_client = *reinterpret_cast<client_t***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ReadClientMessage) + 17);
					svmove = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(MiddleOfSV_ReadClientMessage) + 43);
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ReadClientMessage) + 39);
					sv_player = *reinterpret_cast<edict_t***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ReadClientMessage) + 23);
					break;
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

		auto fCmd_Exec_f = FindAsync(
			ORIG_Cmd_Exec_f,
			patterns::engine::Cmd_Exec_f,
			[&](auto pattern) {
				switch (pattern - patterns::engine::Cmd_Exec_f.cbegin())
				{
				case 0: // SteamPipe.
					ORIG_Cbuf_InsertTextLines = reinterpret_cast<_Cbuf_InsertTextLines>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cmd_Exec_f) + 510)
						+ reinterpret_cast<uintptr_t>(ORIG_Cmd_Exec_f) + 514);
					break;
				case 1: // 4554.
					ORIG_Cbuf_InsertTextLines = reinterpret_cast<_Cbuf_InsertTextLines>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cmd_Exec_f) + 459)
						+ reinterpret_cast<uintptr_t>(ORIG_Cmd_Exec_f) + 463);
					break;
				case 2: // WON.
					ORIG_Cbuf_InsertTextLines = reinterpret_cast<_Cbuf_InsertTextLines>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cmd_Exec_f) + 175)
						+ reinterpret_cast<uintptr_t>(ORIG_Cmd_Exec_f) + 179);
					break;
				case 3: // WON-2.
					ORIG_Cbuf_InsertTextLines = reinterpret_cast<_Cbuf_InsertTextLines>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cmd_Exec_f) + 441)
						+ reinterpret_cast<uintptr_t>(ORIG_Cmd_Exec_f) + 445);
					break;
				}
			});

		void* Cmd_ExecuteStringWithPrivilegeCheck;
		auto fCmd_ExecuteStringWithPrivilegeCheck = FindAsync(
			Cmd_ExecuteStringWithPrivilegeCheck,
			patterns::engine::Cmd_ExecuteStringWithPrivilegeCheck,
			[&](auto pattern) {
			switch (pattern - patterns::engine::Cmd_ExecuteStringWithPrivilegeCheck.cbegin())
			{
			case 0: // SteamPipe-8183.
				ORIG_Cmd_TokenizeString = reinterpret_cast<_Cmd_TokenizeString>(
					*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(Cmd_ExecuteStringWithPrivilegeCheck) + 22)
					+ reinterpret_cast<uintptr_t>(Cmd_ExecuteStringWithPrivilegeCheck) + 26);
				cmd_alias = *reinterpret_cast<cmdalias_t**>(reinterpret_cast<uintptr_t>(Cmd_ExecuteStringWithPrivilegeCheck) + 83);
				break;
			}
		});

		void* Cmd_ExecuteString;
		auto fCmd_ExecuteString = FindAsync(
			Cmd_ExecuteString,
			patterns::engine::Cmd_ExecuteString,
			[&](auto pattern) {
				switch (pattern - patterns::engine::Cmd_ExecuteString.cbegin())
				{
				case 0: // SteamPipe.
					ORIG_Cmd_TokenizeString = reinterpret_cast<_Cmd_TokenizeString>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(Cmd_ExecuteString) + 17)
						+ reinterpret_cast<uintptr_t>(Cmd_ExecuteString) + 21);
					cmd_alias = *reinterpret_cast<cmdalias_t**>(reinterpret_cast<uintptr_t>(Cmd_ExecuteString) + 77);
					break;
				case 1: // 4554.
					ORIG_Cmd_TokenizeString = reinterpret_cast<_Cmd_TokenizeString>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(Cmd_ExecuteString) + 16)
						+ reinterpret_cast<uintptr_t>(Cmd_ExecuteString) + 20);
					cmd_alias = *reinterpret_cast<cmdalias_t**>(reinterpret_cast<uintptr_t>(Cmd_ExecuteString) + 76);
					break;
				case 2: // NGHL.
					ORIG_Cmd_TokenizeString = reinterpret_cast<_Cmd_TokenizeString>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(Cmd_ExecuteString) + 16)
						+ reinterpret_cast<uintptr_t>(Cmd_ExecuteString) + 20);
					cmd_alias = *reinterpret_cast<cmdalias_t**>(reinterpret_cast<uintptr_t>(Cmd_ExecuteString) + 72);
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

		{
			auto pattern = fCmd_Exec_f.get();
			if (ORIG_Cmd_Exec_f) {
				EngineDevMsg("[hw dll] Found Cmd_Exec_f at %p (using the %s pattern).\n", ORIG_Cmd_Exec_f, pattern->name());
				EngineDevMsg("[hw dll] Found Cbuf_InsertTextLines at %p.\n", ORIG_Cbuf_InsertTextLines);
			} else {
				EngineDevWarning("[hw dll] Could not find Cmd_Exec_f.\n");
				ORIG_Cbuf_Execute = nullptr;
			}
		}

		{
			auto found = false;
			auto pattern1 = fCmd_ExecuteStringWithPrivilegeCheck.get();
			if (Cmd_ExecuteStringWithPrivilegeCheck) {
				found = true;
				EngineDevMsg("[hw dll] Found Cmd_ExecuteStringWithPrivilegeCheck at %p (using the %s pattern).\n", Cmd_ExecuteStringWithPrivilegeCheck, pattern1->name());
			} else {
				auto pattern2 = fCmd_ExecuteString.get();
				if (Cmd_ExecuteString) {
					found = true;
					EngineDevMsg("[hw dll] Found Cmd_ExecuteString at %p (using the %s pattern).\n", Cmd_ExecuteString, pattern2->name());
				}
			}

			if (found) {
				EngineDevMsg("[hw dll] Found Cmd_TokenizeString at %p.\n", ORIG_Cmd_TokenizeString);
				EngineDevMsg("[hw dll] Found cmd_alias at %p.\n", cmd_alias);
			} else {
				EngineDevWarning("[hw dll] Could not find Cmd_ExecuteString or Cmd_ExecuteStringWithPrivilegeCheck.\n");
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
		GET_FUTURE(CL_Record_f)
		GET_FUTURE(build_number);
		GET_FUTURE(Key_Event);
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
		GET_FUTURE(R_DrawSequentialPoly);
		GET_FUTURE(R_Clear);
		GET_FUTURE(Mod_LeafPVS);
		GET_FUTURE(PF_GetPhysicsKeyValue);

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

		if (!hw.exportFilename.empty())
			hw.exportResult.ClearProperties();

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

			if (!hw.exportFilename.empty())
				hw.exportResult.SetProperty(prop.first, prop.second);
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

struct HwDLL::Cmd_BXT_TAS_ExportScript
{
	USAGE("Usage: bxt_tas_exportscript <filename>\n Starts exporting the currently running HLTAS script into a HLTAS script with the given filename. The exported script will contain no autofuncs.\n");

	static void handler(const char *fileName)
	{
		auto &hw = HwDLL::GetInstance();
		hw.exportFilename = fileName;
		hw.exportResult.ClearProperties();
		hw.exportResult.ClearFrames();

		if (hw.exportFilename.empty())
			return;

		if (hw.runningFrames)
			for (auto prop : hw.input.GetProperties())
				hw.exportResult.SetProperty(prop.first, prop.second);

		hw.ORIG_Con_Printf("Started exporting .hltas frames.\n");
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
		CustomHud::SaveTimeToDemo();
		return CustomHud::SetCountingTime(true);
	}
};

struct HwDLL::Cmd_BXT_Timer_Stop
{
	NO_USAGE();

	static void handler()
	{
		CustomHud::SaveTimeToDemo();
		return CustomHud::SetCountingTime(false);
	}
};

struct HwDLL::Cmd_BXT_Timer_Reset
{
	NO_USAGE();

	static void handler()
	{
		CustomHud::SaveTimeToDemo();
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

struct HwDLL::Cmd_BXT_TAS_Jumpbug_Down
{
	NO_USAGE();

	static void handler()
	{
		HwDLL::GetInstance().jumpbug = true;
	}

	static void handler(const char*)
	{
		HwDLL::GetInstance().jumpbug = true;
	}
};

struct HwDLL::Cmd_BXT_TAS_Jumpbug_Up
{
	NO_USAGE();

	static void handler()
	{
		HwDLL::GetInstance().jumpbug = false;
	}

	static void handler(const char*)
	{
		HwDLL::GetInstance().jumpbug = false;
	}
};

struct HwDLL::Cmd_BXT_Triggers_Add
{
	USAGE("Usage: bxt_triggers_add <x1> <y1> <z1> <x2> <y2> <z2>\n Adds a custom trigger in a form of axis-aligned cuboid with opposite corners at coordinates (x1, y1, z1) and (x2, y2, z2).\n");

	static void handler(float x1, float y1, float z1, float x2, float y2, float z2)
	{
		CustomTriggers::triggers.emplace_back(Vector(x1, y1, z1), Vector(x2, y2, z2));
	}
};

struct HwDLL::Cmd_BXT_Triggers_Clear
{
	NO_USAGE();

	static void handler()
	{
		CustomTriggers::triggers.clear();
	}
};

struct HwDLL::Cmd_BXT_Triggers_Delete
{
	USAGE("Usage: bxt_triggers_delete [id]\n Deletes the last placed trigger.\n If an id is given, deletes the trigger with the given id.\n");

	static void handler()
	{
		if (CustomTriggers::triggers.empty()) {
			HwDLL::GetInstance().ORIG_Con_Printf("You haven't placed any triggers.\n");
			return;
		}

		CustomTriggers::triggers.erase(--CustomTriggers::triggers.end());
	}

	static void handler(unsigned long id)
	{
		if (id == 0 || CustomTriggers::triggers.size() < id) {
			HwDLL::GetInstance().ORIG_Con_Printf("There's no trigger with this id.\n");
			return;
		}

		CustomTriggers::triggers.erase(CustomTriggers::triggers.begin() + (id - 1));
	}
};

struct HwDLL::Cmd_BXT_Triggers_Export
{
	USAGE("Usage: bxt_triggers_export [cmd|script]\n");

	static void handler(const char* type)
	{
		auto& hw = HwDLL::GetInstance();

		enum class ExportType {
			CMD,
			SCRIPT
		} export_type;

		if (!std::strcmp(type, "cmd")) {
			export_type = ExportType::CMD;
		} else if (!std::strcmp(type, "script")) {
			export_type = ExportType::SCRIPT;
		} else {
			hw.ORIG_Con_Printf("%s", GET_USAGE());
			return;
		}

		auto command_separator = (export_type == ExportType::SCRIPT) ? '\n' : ';';

		if (CustomTriggers::triggers.empty()) {
			hw.ORIG_Con_Printf("You haven't placed any triggers.\n");
			return;
		}

		bool first = true;
		for (const auto& t : CustomTriggers::triggers) {
			auto corners = t.get_corner_positions();

			std::ostringstream oss;

			if (!first)
				oss << command_separator;

			oss << "bxt_triggers_add "
				<< corners.first.x << " " << corners.first.y << " " << corners.first.z << " "
				<< corners.second.x << " " << corners.second.y << " " << corners.second.z;

			if (t.get_command().size() > 1)
				oss << command_separator << "bxt_triggers_setcommand \""
					<< t.get_command().substr(0, t.get_command().size() - 1) << '\"';

			hw.ORIG_Con_Printf("%s", oss.str().c_str());

			first = false;
		}

		hw.ORIG_Con_Printf("\n");
	}
};

struct HwDLL::Cmd_BXT_Triggers_List
{
	NO_USAGE();

	static void handler()
	{
		auto& hw = HwDLL::GetInstance();

		if (CustomTriggers::triggers.empty()) {
			hw.ORIG_Con_Printf("You haven't placed any triggers.\n");
			return;
		}

		for (size_t i = 0; i < CustomTriggers::triggers.size(); ++i) {
			const auto& t = CustomTriggers::triggers[i];
			const auto corners = t.get_corner_positions();

			std::ostringstream oss;
			oss << i + 1 << ": `" << t.get_command().substr(0, t.get_command().size() - 1) << "` - ("
				<< corners.first.x << ", " << corners.first.y << ", " << corners.first.z << ") | ("
				<< corners.second.x << ", " << corners.second.y << ", " << corners.second.z << ")\n";

			hw.ORIG_Con_Printf("%s", oss.str().c_str());
		}
	}
};

struct HwDLL::Cmd_BXT_Triggers_SetCommand
{
	USAGE("Usage: bxt_triggers_setcommand <command>\n Sets the last placed trigger's command.\n bxt_triggers_setcommand <id> <command>\n Sets the command of a trigger with the given id.\n");

	static void handler(const char* command)
	{
		if (CustomTriggers::triggers.empty()) {
			HwDLL::GetInstance().ORIG_Con_Printf("You haven't placed any triggers.\n");
			return;
		}

		CustomTriggers::triggers.back().set_command(command);
	}

	static void handler(unsigned long id, const char* command)
	{
		if (id == 0 || CustomTriggers::triggers.size() < id) {
			HwDLL::GetInstance().ORIG_Con_Printf("There's no trigger with this id.\n");
			return;
		}

		CustomTriggers::triggers[id - 1].set_command(command);
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

struct HwDLL::Cmd_BXT_Heuristic
{
	NO_USAGE();

	static void handler()
	{
		auto& hw = HwDLL::GetInstance();

		if (hw.svs->num_clients >= 1) {
			edict_t *pl = *reinterpret_cast<edict_t**>(reinterpret_cast<uintptr_t>(hw.svs->clients) + hw.offEdict);
			if (pl) {
				PlayerState player;
				player.Origin[0] = pl->v.origin[0];
				player.Origin[1] = pl->v.origin[1];
				player.Origin[2] = pl->v.origin[2];
				player.Velocity[0] = pl->v.velocity[0];
				player.Velocity[1] = pl->v.velocity[1];
				player.Velocity[2] = pl->v.velocity[2];
				player.Ducking = (pl->v.flags & FL_DUCKING) != 0;

				hw.ORIG_Con_Printf("Heuristic = %u\n", HwDLL::Heuristic(player));
				//for (int i = 0; i < 1000; ++i)
				//        Heuristic(player);
			}
		}
	}
};

struct HwDLL::Cmd_BXT_StartSearch
{
	NO_USAGE();

	static void handler()
	{
		HwDLL::GetInstance().StartSearch();
	}
};

struct HwDLL::Cmd_BXT_FindCoarseNodes
{
	NO_USAGE();

	static void handler()
	{
		HwDLL::GetInstance().FindCoarseNodes();
	}
};

struct HwDLL::Cmd_BXT_FindCoarsePath
{
	NO_USAGE();

	static void handler()
	{
		HwDLL::GetInstance().FindCoarsePath();
	}
};

struct HwDLL::Cmd_BXT_FollowCoarsePath
{
	NO_USAGE();

	static void handler()
	{
		HwDLL::GetInstance().FollowCoarsePath();
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
	RegisterCVar(CVars::bxt_stop_demo_on_changelevel);
	RegisterCVar(CVars::bxt_wallhack);
	RegisterCVar(CVars::bxt_wallhack_additive);
	RegisterCVar(CVars::bxt_wallhack_alpha);
	RegisterCVar(CVars::bxt_novis);
	RegisterCVar(CVars::_bxt_norefresh);
	RegisterCVar(CVars::_bxt_bunnysplit_time_update_frequency);
	RegisterCVar(CVars::_bxt_save_runtime_data_in_demos);
	RegisterCVar(CVars::bxt_collision_depth_map);
	RegisterCVar(CVars::bxt_collision_depth_map_colors);
	RegisterCVar(CVars::bxt_collision_depth_map_hull);
	RegisterCVar(CVars::bxt_collision_depth_map_max_depth);

	CVars::sv_cheats.Assign(FindCVar("sv_cheats"));
	CVars::fps_max.Assign(FindCVar("fps_max"));
	CVars::default_fov.Assign(FindCVar("default_fov"));

	if (!ORIG_Cmd_AddMallocCommand)
		return;

	using CmdWrapper::Handler;
	typedef CmdWrapper::CmdWrapper<CmdFuncs> wrapper;

	wrapper::Add<Cmd_BXT_TAS_LoadScript, Handler<const char *>>("bxt_tas_loadscript");
	wrapper::Add<Cmd_BXT_TAS_ExportScript, Handler<const char *>>("bxt_tas_exportscript");
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
	wrapper::Add<Cmd_BXT_TAS_Jumpbug_Down, Handler<>, Handler<const char*>>("+bxt_tas_jumpbug");
	wrapper::Add<Cmd_BXT_TAS_Jumpbug_Up, Handler<>, Handler<const char*>>("-bxt_tas_jumpbug");
	wrapper::Add<Cmd_BXT_Triggers_Add, Handler<float, float, float, float, float, float>>("bxt_triggers_add");
	wrapper::Add<Cmd_BXT_Triggers_Clear, Handler<>>("bxt_triggers_clear");
	wrapper::Add<Cmd_BXT_Triggers_Delete, Handler<>, Handler<unsigned long>>("bxt_triggers_delete");
	wrapper::Add<Cmd_BXT_Triggers_Export, Handler<const char*>>("bxt_triggers_export");
	wrapper::Add<Cmd_BXT_Triggers_List, Handler<>>("bxt_triggers_list");
	wrapper::Add<
		Cmd_BXT_Triggers_SetCommand,
		Handler<const char*>,
		Handler<unsigned long, const char*>>("bxt_triggers_setcommand");
	wrapper::Add<Cmd_BXT_Record, Handler<const char *>>("bxt_record");
	wrapper::Add<Cmd_BXT_AutoRecord, Handler<const char *>>("bxt_autorecord");
	wrapper::Add<Cmd_BXT_Map, Handler<const char *>>("_bxt_map");
	wrapper::Add<Cmd_BXT_Load, Handler<const char *>>("_bxt_load");
	wrapper::Add<Cmd_BXT_Interprocess_Reset, Handler<>>("_bxt_interprocess_reset");
	wrapper::Add<Cmd_BXT_Reset_Frametime_Remainder, Handler<>>("_bxt_reset_frametime_remainder");
	wrapper::Add<Cmd_BXT_TASLog, Handler<>>("bxt_taslog");
	wrapper::Add<Cmd_BXT_Append, Handler<const char *>>("bxt_append");
	wrapper::Add<Cmd_BXT_Heuristic, Handler<>>("bxt_heuristic");
	wrapper::Add<Cmd_BXT_StartSearch, Handler<>>("bxt_startsearch");
	wrapper::Add<Cmd_BXT_FindCoarseNodes, Handler<>>("bxt_find_coarse_nodes");
	wrapper::Add<Cmd_BXT_FindCoarsePath, Handler<>>("bxt_find_coarse_path");
	wrapper::Add<Cmd_BXT_FollowCoarsePath, Handler<>>("bxt_follow_coarse_path");
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
				HLTAS::Frame resulting_frame;

				if (thisFrameIs0ms)
					resulting_frame.Frametime = frametime0ms;
				else
					resulting_frame.Frametime = f.Frametime;

				resulting_frame.SetRepeats(1);

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
						
						log = fopen("gt.log", "a");
						fprintf(log, "o(%.8f %.8f %.8f) vel(%.8f %.8f %.8f) ",
										     player.Origin[0], player.Origin[1], player.Origin[2],
										     player.Velocity[0], player.Velocity[1], player.Velocity[2]);
						fclose(log);
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
				log = fopen("gt.log", "a");
				fprintf(log, "\n");
				fclose(log);

				f.ResetAutofuncs();

				resulting_frame.SetPitch(p.Pitch);
				resulting_frame.SetYaw(p.Yaw + 180.0 / 65536);

				#define INS(btn) \
					if (p.btn && !currentKeys.btn.IsDown()) \
						KeyDown(currentKeys.btn); \
					else if (!p.btn && currentKeys.btn.IsDown()) \
						KeyUp(currentKeys.btn); \
					resulting_frame.btn = p.btn;
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

				resulting_frame.Commands = boost::replace_all_copy(speeds_ss.str(), "\n", ";") + f.Commands;

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

				if (!exportFilename.empty())
					exportResult.InsertFrame(exportResult.GetFrames().size(), resulting_frame);

				break;
			} else if (!f.SaveName.empty()) { // Saveload frame.
				std::ostringstream ss;
				ss << "save " << f.SaveName << ";load " << f.SaveName << "\n";
				ORIG_Cbuf_InsertText(ss.str().c_str());
				currentFramebulk++;

				HLTAS::Frame resulting_frame;
				resulting_frame.SaveName = f.SaveName;
				if (!exportFilename.empty())
					exportResult.InsertFrame(exportResult.GetFrames().size(), resulting_frame);

				break;
			} else if (f.SeedPresent) { // Seeds frame.
				SharedRNGSeedPresent = true;
				SharedRNGSeed = f.GetSeed();

				HLTAS::Frame resulting_frame;
				resulting_frame.SetSeed(f.GetSeed());
				if (!exportFilename.empty())
					exportResult.InsertFrame(exportResult.GetFrames().size(), resulting_frame);
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

				HLTAS::Frame resulting_frame;
				resulting_frame.ResetFrame = true;
				resulting_frame.SetResetNonSharedRNGSeed(f.GetResetNonSharedRNGSeed());
				if (!exportFilename.empty())
					exportResult.InsertFrame(exportResult.GetFrames().size(), resulting_frame);

				break;
			}

			currentFramebulk++;
		};

		// Ran through all frames.
		if (currentFramebulk >= totalFramebulks) {
			runningFrames = false;

			if (!exportFilename.empty()) {
				auto error = exportResult.Save(exportFilename).get();
				if (error.Code == HLTAS::ErrorCode::OK)
					ORIG_Con_Printf("Exporting finished successfully.\n");
				else
					ORIG_Con_Printf("Error saving the exported script: %s\n", HLTAS::GetErrorMessage(error).c_str());

				exportFilename.clear();
				exportResult.ClearProperties();
				exportResult.ClearFrames();
			}
		}
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
		if (autojump || ducktap || jumpbug) {
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
			auto traceFunc = std::bind(&HwDLL::PlayerTrace, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
			auto postype = GetPositionType(playerCopy, traceFunc);
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
				} else if (autojump && !currentKeys.Jump.IsDown()) {
					Jump = true;
				}
			} else if (jumpbug && postype == HLStrafe::PositionType::AIR) {
				if (player.Ducking) {
					// Predict what will happen if we unduck.
					playerCopy.Ducking = false;
					playerCopy.InDuckAnimation = false;
					playerCopy.DuckTime = 0;

					auto nextPostype = HLStrafe::GetPositionType(playerCopy, traceFunc);
					if (nextPostype == HLStrafe::PositionType::GROUND) {
						// Jumpbug if we're about to land.
						Jump = true;
						Duck = false;
					}
				} else {
					auto vars = GetMovementVars();
					auto nextPostype = HLStrafe::Move(playerCopy, vars, postype, vars.Maxspeed, traceFunc);
					if (nextPostype == HLStrafe::PositionType::GROUND) {
						// Duck to prepare for the Jumpbug.
						Duck = true;
						Jump = false;
					}
				}
			}

			#define INS(btn) \
					if (btn && !currentKeys.btn.IsDown()) \
						KeyDown(currentKeys.btn); \
					else if (!btn && currentKeys.btn.IsDown()) \
						KeyUp(currentKeys.btn);
			if (jumpbug) {
				INS(Duck)
				INS(Jump)
			} else if (ducktap) {
				INS(Duck)
			} else if (autojump) {
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

	UpdateCustomTriggers();

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

	FindCoarseNodesStep();
	FindCoarsePathStep();
	FollowCoarsePathStep();

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

void HwDLL::SetPlayerOrigin(const float origin[3])
{
	player.Origin[0] = origin[0];
	player.Origin[1] = origin[1];
	player.Origin[2] = origin[2];
}

void HwDLL::SetPlayerVelocity(const float velocity[3])
{
	player.Velocity[0] = velocity[0];
	player.Velocity[1] = velocity[1];
	player.Velocity[2] = velocity[2];
}

bool HwDLL::TryGettingAccurateInfo(float origin[3], float velocity[3], float& health)
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
	health = pl->v.health;

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
	StartTracing();
	const auto rv = UnsafePlayerTrace(start, end, hull);
	StopTracing();

	return rv;
}

void HwDLL::StartTracing() {
	if (!ORIG_PM_PlayerTrace || svs->num_clients < 1) {
		return;
	}

	trace_oldclient = *host_client;
	*host_client = svs->clients;
	trace_oldplayer = *sv_player;
	*sv_player = *reinterpret_cast<edict_t**>(reinterpret_cast<uintptr_t>(svs->clients) + offEdict);
	trace_oldmove = *ppmove;
	*ppmove = svmove;
	ORIG_SV_AddLinksToPM(sv_areanodes, (*sv_player)->v.origin);
}

void HwDLL::StopTracing() {
	if (!ORIG_PM_PlayerTrace || svs->num_clients < 1) {
		return;
	}

	*ppmove = trace_oldmove;
	*sv_player = trace_oldplayer;
	*host_client = trace_oldclient;
}

HLStrafe::TraceResult HwDLL::UnsafePlayerTrace(const float start[3], const float end[3], HLStrafe::HullType hull) {
	auto tr = HLStrafe::TraceResult{};

	if (!ORIG_PM_PlayerTrace || svs->num_clients < 1) {
		tr.Fraction = 1.f;
		tr.EndPos[0] = end[0];
		tr.EndPos[1] = end[1];
		tr.EndPos[2] = end[2];
		tr.Entity = -1;
		return tr;
	}

	auto usehull = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(svmove) + 0xBC);
	auto oldhull = *usehull;
	*usehull = static_cast<int>(hull);

	const int PM_NORMAL = 0x00000000;
	auto pmtr = ORIG_PM_PlayerTrace(start, end, PM_NORMAL, -1);

	*usehull = oldhull;

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

void HwDLL::SaveInitialDataToDemo()
{
	RuntimeData::Add(RuntimeData::VersionInfo {
		ORIG_build_number(),
		Git::GetRevision()
	});

	auto modules = MemUtils::GetLoadedModules();

	std::vector<std::string> filenames;
	for (auto module : modules) {
		auto path = MemUtils::GetModulePath(module);
		filenames.emplace_back(Convert(GetFileName(path)));
	}

	RuntimeData::Add(RuntimeData::LoadedModules { std::move(filenames) });

	constexpr const char* cvars_to_save[] = {
		"bxt_autojump",
		"bxt_bhopcap",
		"bxt_collision_depth_map",
		"bxt_fade_remove",
		"bxt_hud_distance",
		"bxt_hud_entity_hp",
		"bxt_hud_health",
		"bxt_hud_origin",
		"bxt_hud_selfgauss",
		"bxt_hud_useables",
		"bxt_hud_nihilanth",
		"bxt_hud_velocity",
		"bxt_hud_visible_landmarks",
		"bxt_novis",
		"bxt_show_hidden_entities",
		"bxt_show_triggers",
		"bxt_show_triggers_legacy",
		"bxt_wallhack",
		"bxt_wallhack_additive",
		"bxt_wallhack_alpha",
		"chase_active",
		"cl_anglespeedkey",
		"cl_backspeed",
		"cl_forwardspeed",
		"cl_pitchdown",
		"cl_pitchspeed",
		"cl_pitchup",
		"cl_sidespeed",
		"cl_upspeed",
		"cl_yawspeed",
		"fps_max",
		"fps_override",
		"gl_monolights",
		"host_framerate",
		"host_speeds",
		"r_drawentities",
		"r_fullbright",
		"s_show",
		"snd_show",
		"sv_cheats",
	};

	std::unordered_map<std::string, std::string> cvar_values;

	for (const auto cvar_name : cvars_to_save) {
		const auto cvar = FindCVar(cvar_name);

		if (cvar)
			cvar_values.emplace(cvar_name, cvar->string);
	}

	RuntimeData::Add(std::move(cvar_values));

	// Initial BXT timer value.
	CustomHud::SaveTimeToDemo();
}

void HwDLL::UpdateCustomTriggers()
{
	if (!svs || svs->num_clients < 1)
		return;

	edict_t *pl = *reinterpret_cast<edict_t**>(reinterpret_cast<uintptr_t>(svs->clients) + offEdict);
	if (!pl)
		return;

	CustomTriggers::Update(pl->v.origin, (pl->v.flags & FL_DUCKING) != 0);
}

void HwDLL::InitTracing()
{
	oldclient = *host_client;
	*host_client = svs->clients;
	oldplayer = *sv_player;
	*sv_player = *reinterpret_cast<edict_t**>(reinterpret_cast<uintptr_t>(svs->clients) + offEdict);
	oldmove = *ppmove;
	*ppmove = svmove;
	ORIG_SV_AddLinksToPM(sv_areanodes, (*sv_player)->v.origin);
}

void HwDLL::DeinitTracing()
{
	*ppmove = oldmove;
	*sv_player = oldplayer;
	*host_client = oldclient;
}

HLStrafe::TraceResult HwDLL::PlayerTrace2(const float start[3], const float end[3], HLStrafe::HullType hull)
{
	auto tr = HLStrafe::TraceResult{};

	auto usehull = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(svmove) + 0xBC);
	auto oldhull = *usehull;
	*usehull = static_cast<int>(hull);

	const int PM_NORMAL = 0x00000000;
	auto pmtr = ORIG_PM_PlayerTrace(start, end, PM_NORMAL, -1);

	*usehull = oldhull;

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

static constexpr float FRAMETIME = 10 * 0.001f;
static constexpr float MAXSPEED = 300.0f;
static constexpr float ACCELERATE = 10.0f;
static constexpr float AIRACCELERATE = 10.0f;

using namespace HLTAS;
using HLStrafe::TraceFunc;
using HLStrafe::PositionType;
using HLStrafe::HullType;
using HLStrafe::VecCopy;
using HLStrafe::VecScale;
using HLStrafe::VecSubtract;
using HLStrafe::VecAdd;
using HLStrafe::Length;
using HLStrafe::Distance;
using HLStrafe::DotProduct;
using HLStrafe::CrossProduct;
using HLStrafe::IsZero;
using HLStrafe::AngleModRad;
using HLStrafe::Normalize;
using HLStrafe::Atan2;
using HLStrafe::M_DEG2RAD;
using HLStrafe::M_RAD2DEG;
using HLStrafe::M_U_RAD;
using HLStrafe::M_U_DEG;
using HLStrafe::M_U_DEG_HALF;
using HLStrafe::VEC_HULL_MIN;
using HLStrafe::VEC_HULL_MAX;
using HLStrafe::VEC_DUCK_HULL_MIN;
using HLStrafe::VEC_DUCK_HULL_MAX;
using PlayerState = HwDLL::PlayerState;

#define TF(x) ((x) ? "true" : "false")

class PathSimulator
{
public:
	PathSimulator(PlayerState& player, TraceFunc traceFunc)
		: player(player)
		, traceFunc(traceFunc)
	{
	}

	void SimulateFrame(StrafeDir strafe_dir, bool jump, bool duck)
	{
		auto postype = GetPositionType();
		//HwDLL::GetInstance().ORIG_Con_Printf("onground: %s, jump: %s, duck: %s\n", TF(postype == PositionType::GROUND), TF(jump), TF(duck));

		const bool reduce_wishspeed = player.Ducking;

		ReduceTimers();

		postype = PredictDuck(postype, duck);

		AddCorrectGravity();

		postype = PredictJump(postype, jump);

		Friction(postype);
		CheckVelocity();

		postype = Strafe(postype, reduce_wishspeed, strafe_dir);

		player.WasPressingJump = jump;
		player.WasPressingDuck = duck;
	}

protected:
	PlayerState& player;
	TraceFunc traceFunc;

	void CheckVelocity()
	{
		for (std::size_t i = 0; i < 3; ++i) {
			if (player.Velocity[i] > 2000)
				player.Velocity[i] = 2000;
			if (player.Velocity[i] < -2000)
				player.Velocity[i] = -2000;
		}
	}

	PositionType GetPositionType()
	{
		if (player.Velocity[2] > 180)
			return PositionType::AIR;

		float point[3];
		VecCopy<float, 3>(player.Origin, point);
		point[2] -= 2;

		auto tr = traceFunc(player.Origin, point, (player.Ducking) ? HullType::DUCKED : HullType::NORMAL);
		if (tr.PlaneNormal[2] < 0.7 || tr.Entity == -1)
			return PositionType::AIR;

		if (!tr.StartSolid && !tr.AllSolid)
			VecCopy<float, 3>(tr.EndPos, player.Origin);
		return PositionType::GROUND;
	}

	void ReduceTimers()
	{
		player.DuckTime = std::max(player.DuckTime - static_cast<int>(FRAMETIME * 1000), 0.f);
	}

	PositionType PredictDuck(PositionType postype, bool duck)
	{
		if (!duck
			&& !player.InDuckAnimation
			&& !player.Ducking)
			return postype;

		if (duck) {
			if (!player.WasPressingDuck && !player.Ducking) {
				player.DuckTime = 1000;
				player.InDuckAnimation = true;
			}

			if (player.InDuckAnimation
				&& (player.DuckTime / 1000.0 <= (1.0 - 0.4)
					|| postype != PositionType::GROUND)) {
				player.Ducking = true;
				player.InDuckAnimation = false;
				if (postype == PositionType::GROUND) {
					for (std::size_t i = 0; i < 3; ++i)
						player.Origin[i] -= (VEC_DUCK_HULL_MIN[i] - VEC_HULL_MIN[i]);
					// Is PM_FixPlayerCrouchStuck() prediction needed here?
					return GetPositionType();
				}
			}
		} else {
			// Unduck.
			float newOrigin[3];
			VecCopy<float, 3>(player.Origin, newOrigin);
			if (postype == PositionType::GROUND)
				for (std::size_t i = 0; i < 3; ++i)
					newOrigin[i] += (VEC_DUCK_HULL_MIN[i] - VEC_HULL_MIN[i]);

			auto tr = traceFunc(newOrigin, newOrigin, (player.Ducking) ? HullType::DUCKED : HullType::NORMAL);
			if (!tr.StartSolid) {
				tr = traceFunc(newOrigin, newOrigin, HullType::NORMAL);
				if (!tr.StartSolid) {
					player.Ducking = false;
					player.InDuckAnimation = false;
					player.DuckTime = 0;
					VecCopy<float, 3>(newOrigin, player.Origin);

					return GetPositionType();
				}
			}
		}

		return postype;
	}

	PositionType PredictJump(PositionType postype, bool jump)
	{
		if (!jump // Not pressing Jump.
			|| player.WasPressingJump // Jump was already down.
			|| postype != PositionType::GROUND) // Not on ground.
			return postype;

		player.Velocity[2] = static_cast<float>(std::sqrt(2 * 800 * 45.0));

		FixupGravityVelocity();

		return PositionType::AIR;
	}

	void AddCorrectGravity()
	{
		player.Velocity[2] -= static_cast<float>(800 * 0.5 * FRAMETIME);
		CheckVelocity();
	}

	void FixupGravityVelocity()
	{
		player.Velocity[2] -= static_cast<float>(800 * 0.5 * FRAMETIME);
		CheckVelocity();
	}

	void Friction(PositionType postype)
	{
		if (postype != PositionType::GROUND)
			return;

		// Doing all this in floats, mismatch is too real otherwise.
		auto speed = static_cast<float>( std::sqrt(static_cast<double>(player.Velocity[0] * player.Velocity[0] + player.Velocity[1] * player.Velocity[1] + player.Velocity[2] * player.Velocity[2])) );
		if (speed < 0.1)
			return;

		auto friction = 4.0f;

		float start[3], stop[3];
		start[0] = static_cast<float>(player.Origin[0] + player.Velocity[0] / speed * 16);
		start[1] = static_cast<float>(player.Origin[1] + player.Velocity[1] / speed * 16);
		start[2] = player.Origin[2] + ((player.Ducking) ? VEC_DUCK_HULL_MIN[2] : VEC_HULL_MIN[2]);
		VecCopy<float, 3>(start, stop);
		stop[2] -= 34;

		auto tr = traceFunc(start, stop, (player.Ducking) ? HullType::DUCKED : HullType::NORMAL);
		if (tr.Fraction == 1.0)
			friction *= 2;

		auto control = (speed < 100) ? 100 : speed;
		auto drop = control * friction * FRAMETIME;
		auto newspeed = std::max(speed - drop, 0.f);
		VecScale<float, 3>(player.Velocity, newspeed / speed, player.Velocity);
	}

	PositionType Strafe(PositionType postype, bool reduce_wishspeed, StrafeDir strafe_dir)
	{
		double wishspeed = MAXSPEED;
		if (reduce_wishspeed)
			wishspeed *= 0.333;

		player.Yaw = static_cast<float>(SideStrafeMaxAccel(postype, wishspeed, player.Yaw * M_DEG2RAD, strafe_dir == StrafeDir::RIGHT) * M_RAD2DEG);

		//fprintf(HwDLL::GetInstance().log, "y(%uu)", static_cast<unsigned>((player.Yaw + M_U_DEG_HALF) / M_U_DEG));

		return Move(postype, wishspeed);
	}

	double SideStrafeMaxAccel(PositionType postype, double wishspeed, double vel_yaw, bool right)
	{
		double theta = MaxAccelTheta(postype, wishspeed);
		float velocities[2][2];
		double yaws[2];
		SideStrafeGeneral(postype, wishspeed, vel_yaw, theta, right, velocities, yaws);

		double speedsqrs[2] = {
			DotProduct<float, float, 2>(velocities[0], velocities[0]),
			DotProduct<float, float, 2>(velocities[1], velocities[1])
		};

		//fprintf(HwDLL::GetInstance().log, "\ny1(%uu) v1(%.8f %.8f) %.16f y2(%uu) v2(%.8f %.8f) %.16f ", static_cast<unsigned>((yaws[0] + M_U_RAD / 2) / M_U_RAD), velocities[0][0], velocities[0][1], speedsqrs[0], static_cast<unsigned>((yaws[1] + M_U_RAD / 2) / M_U_RAD), velocities[1][0], velocities[1][1], speedsqrs[1]);

		if (speedsqrs[0] > speedsqrs[1]) {
			VecCopy<float, 2>(velocities[0], player.Velocity);
			return yaws[0];
		} else {
			VecCopy<float, 2>(velocities[1], player.Velocity);
			return yaws[1];
		}
	}

	double MaxAccelTheta(PositionType postype, double wishspeed)
	{
		bool onground = (postype == PositionType::GROUND);
		double accel = onground ? ACCELERATE : AIRACCELERATE;
		double accelspeed = accel * wishspeed * FRAMETIME;
		if (accelspeed <= 0.0)
			return M_PI;

		if (IsZero<float, 2>(player.Velocity))
			return 0.0;

		double wishspeed_capped = onground ? wishspeed : 30;
		double tmp = wishspeed_capped - accelspeed;
		if (tmp <= 0.0)
			return M_PI / 2;

		double speed = Length<float, 2>(player.Velocity);
		if (tmp < speed)
			return std::acos(tmp / speed);

		return 0.0;
	}

	static inline double ButtonsPhi(HLTAS::Button button)
	{
		switch (button) {
		case HLTAS::Button::      FORWARD: return 0;
		case HLTAS::Button:: FORWARD_LEFT: return M_PI / 4;
		case HLTAS::Button::         LEFT: return M_PI / 2;
		case HLTAS::Button::    BACK_LEFT: return 3 * M_PI / 4;
		case HLTAS::Button::         BACK: return -M_PI;
		case HLTAS::Button::   BACK_RIGHT: return -3 * M_PI / 4;
		case HLTAS::Button::        RIGHT: return -M_PI / 2;
		case HLTAS::Button::FORWARD_RIGHT: return -M_PI / 4;
		default: return 0;
		}
	}

	static inline HLTAS::Button GetBestButtons(double theta, bool right)
	{
		if (theta < M_PI / 8)
			return HLTAS::Button::FORWARD;
		else if (theta < 3 * M_PI / 8)
			return right ? HLTAS::Button::FORWARD_RIGHT : HLTAS::Button::FORWARD_LEFT;
		else if (theta < 5 * M_PI / 8)
			return right ? HLTAS::Button::RIGHT : HLTAS::Button::LEFT;
		else if (theta < 7 * M_PI / 8)
			return right ? HLTAS::Button::BACK_RIGHT : HLTAS::Button::BACK_LEFT;
		else
			return HLTAS::Button::BACK;
	}

	static inline void GetAVec(float yaw, double wishspeed, HLTAS::Button buttons, double avec[2])
	{
		float sy = std::sin(yaw * (M_PI * 2 / 360));
		float cy = std::cos(yaw * (M_PI * 2 / 360));

		float forward[3] = { cy, sy, 0 };
		float right[3] = { sy, -cy, 0 };

		float fmove, smove;

		switch (buttons) {
		case Button::FORWARD:
			fmove = wishspeed;
			smove = 0;
			break;

		case Button::BACK:
			fmove = -wishspeed;
			smove = 0;
			break;

		case Button::LEFT:
			fmove = 0;
			smove = -wishspeed;
			break;

		case Button::RIGHT:
			fmove = 0;
			smove = wishspeed;
			break;

		case Button::FORWARD_RIGHT:
			fmove = wishspeed / std::sqrt(2.0);
			smove = wishspeed / std::sqrt(2.0);
			break;

		case Button::FORWARD_LEFT:
			fmove = wishspeed / std::sqrt(2.0);
			smove = -wishspeed / std::sqrt(2.0);
			break;

		case Button::BACK_RIGHT:
			fmove = -wishspeed / std::sqrt(2.0);
			smove = wishspeed / std::sqrt(2.0);
			break;

		case Button::BACK_LEFT:
			fmove = -wishspeed / std::sqrt(2.0);
			smove = -wishspeed / std::sqrt(2.0);
			break;
		}

		float wishvel[2] = { forward[0] * fmove + right[0] * smove, forward[1] * fmove + right[1] * smove };
		Normalize<float, 2>(wishvel, wishvel);

		avec[0] = wishvel[0];
		avec[1] = wishvel[1];
	}

	void SideStrafeGeneral(PositionType postype, double wishspeed, double vel_yaw, double theta, bool right, float velocities[2][2], double yaws[2])
	{
		auto usedButton = GetBestButtons(theta, right);
		double phi = ButtonsPhi(usedButton);
		theta = right ? -theta : theta;

		if (!IsZero<float, 2>(player.Velocity))
			vel_yaw = std::atan2(player.Velocity[1], player.Velocity[0]);

		double yaw = vel_yaw - phi + theta;
		yaws[0] = AngleModRad(yaw);
		// Very rare case of yaw == anglemod(yaw).
		if (yaws[0] == yaw) {
			// Multiply by 1.5 because the fp precision might make the yaw a value not enough to reach the next anglemod.
			// Or divide by 2 because it might throw us a value too far back.
			yaws[1] = AngleModRad(yaw + std::copysign(M_U_RAD * 1.5, yaw));
		} else
			yaws[1] = AngleModRad(yaw + std::copysign(M_U_RAD, yaw));

		//double avec[2] = { std::cos(static_cast<float>(yaws[0] + phi)), std::sin(static_cast<float>(yaws[0] + phi)) };
		//double avec[2] = { std::cos(yaws[0] + phi), std::sin(yaws[0] + phi) };
		double avec[2];
		GetAVec(yaws[0] * M_RAD2DEG, wishspeed, usedButton, avec);

		PlayerState pl = player;
		VectorFME(pl, postype, wishspeed, avec);
		VecCopy<float, 2>(pl.Velocity, velocities[0]);

		//avec[0] = std::cos(yaws[1] + phi);
		//avec[1] = std::sin(yaws[1] + phi);
		GetAVec(yaws[1] * M_RAD2DEG, wishspeed, usedButton, avec);

		VecCopy<float, 2>(player.Velocity, pl.Velocity);
		VectorFME(pl, postype, wishspeed, avec);
		VecCopy<float, 2>(pl.Velocity, velocities[1]);

		player.buttons = usedButton;
	}

	void VectorFME(PlayerState& pl, PositionType postype, double wishspeed, const double a[2])
	{
		bool onground = (postype == PositionType::GROUND);
		double wishspeed_capped = onground ? wishspeed : 30;
		double tmp = wishspeed_capped - DotProduct<float, double, 2>(pl.Velocity, a);
		if (tmp <= 0.0)
			return;

		double accel = onground ? ACCELERATE : AIRACCELERATE;
		double accelspeed = accel * wishspeed * FRAMETIME;
		if (accelspeed <= tmp)
			tmp = accelspeed;

		pl.Velocity[0] += static_cast<float>(a[0] * tmp);
		pl.Velocity[1] += static_cast<float>(a[1] * tmp);
	}

	PositionType Move(PositionType postype, double wishspeed)
	{
		bool onground = (postype == PositionType::GROUND);
		CheckVelocity();

		// Move
		wishspeed = std::min(wishspeed, static_cast<double>(MAXSPEED));
		if (onground)
			player.Velocity[2] = 0;

		// Move
		if (onground) {
			// WalkMove
			auto spd = Length<float, 3>(player.Velocity);
			if (spd < 1) {
				VecScale<float, 3>(player.Velocity, 0, player.Velocity); // Clear velocity.
			} else {
				float dest[3];
				VecCopy<float, 3>(player.Origin, dest);
				dest[0] += player.Velocity[0] * FRAMETIME;
				dest[1] += player.Velocity[1] * FRAMETIME;

				auto tr = traceFunc(player.Origin, dest, (player.Ducking) ? HullType::DUCKED : HullType::NORMAL);
				if (tr.Fraction == 1.0f) {
					VecCopy<float, 3>(tr.EndPos, player.Origin);
				} else {
					// Figure out the end position when trying to walk up a step.
					auto playerUp = PlayerState(player);
					dest[2] += 18;
					tr = traceFunc(playerUp.Origin, dest, (player.Ducking) ? HullType::DUCKED : HullType::NORMAL);
					if (!tr.StartSolid && !tr.AllSolid)
						VecCopy<float, 3>(tr.EndPos, playerUp.Origin);

					FlyMove(playerUp, postype);
					VecCopy<float, 3>(playerUp.Origin, dest);
					dest[2] -= 18;

					tr = traceFunc(playerUp.Origin, dest, (player.Ducking) ? HullType::DUCKED : HullType::NORMAL);
					if (!tr.StartSolid && !tr.AllSolid)
						VecCopy<float, 3>(tr.EndPos, playerUp.Origin);

					// Figure out the end position when _not_ trying to walk up a step.
					auto playerDown = PlayerState(player);
					FlyMove(playerDown, postype);

					// Take whichever move was the furthest.
					auto downdist = (playerDown.Origin[0] - player.Origin[0]) * (playerDown.Origin[0] - player.Origin[0])
						+ (playerDown.Origin[1] - player.Origin[1]) * (playerDown.Origin[1] - player.Origin[1]);
					auto updist = (playerUp.Origin[0] - player.Origin[0]) * (playerUp.Origin[0] - player.Origin[0])
						+ (playerUp.Origin[1] - player.Origin[1]) * (playerUp.Origin[1] - player.Origin[1]);

					if ((tr.PlaneNormal[2] < 0.7) || (downdist > updist)) {
						VecCopy<float, 3>(playerDown.Origin, player.Origin);
						VecCopy<float, 3>(playerDown.Velocity, player.Velocity);
					} else {
						VecCopy<float, 3>(playerUp.Origin, player.Origin);
						VecCopy<float, 2>(playerUp.Velocity, player.Velocity);
						player.Velocity[2] = playerDown.Velocity[2];
					}
				}
			}
		} else {
			// AirMove
			FlyMove(player, postype);
		}

		postype = GetPositionType();
		CheckVelocity();
		if (postype != PositionType::GROUND) {
			FixupGravityVelocity();
		}
		if (postype == PositionType::GROUND)
			player.Velocity[2] = 0;

		return postype;
	}

	void FlyMove(PlayerState& pl, PositionType postype)
	{
		const auto MAX_BUMPS = 4;
		const auto MAX_CLIP_PLANES = 5;

		//std::fprintf(HwDLL::GetInstance().log, "FlyMove i(%.8f %.8f %.8f)", pl.Velocity[0], pl.Velocity[1], pl.Velocity[2]);

		float originalVelocity[3], savedVelocity[3];
		VecCopy<float, 3>(pl.Velocity, originalVelocity);
		VecCopy<float, 3>(pl.Velocity, savedVelocity);

		auto timeLeft = FRAMETIME;
		auto allFraction = 0.0f;
		auto numPlanes = 0;
		auto blockedState = 0;
		float planes[MAX_CLIP_PLANES][3];

		for (auto bumpCount = 0; bumpCount < MAX_BUMPS; ++bumpCount) {
			if (IsZero<float, 3>(pl.Velocity))
				break;

			float end[3];
			for (size_t i = 0; i < 3; ++i)
				end[i] = pl.Origin[i] + timeLeft * pl.Velocity[i];

			auto tr = traceFunc(pl.Origin, end, (pl.Ducking) ? HullType::DUCKED : HullType::NORMAL);

			allFraction += tr.Fraction;
			if (tr.AllSolid) {
				VecScale<float, 3>(pl.Velocity, 0, pl.Velocity);
				blockedState = 4;
				break;
			}
			if (tr.Fraction > 0) {
				VecCopy<float, 3>(tr.EndPos, pl.Origin);
				VecCopy<float, 3>(pl.Velocity, savedVelocity);
				numPlanes = 0;
			}
			if (tr.Fraction == 1)
				break;

			if (tr.PlaneNormal[2] > 0.7)
				blockedState |= 1;
			else if (tr.PlaneNormal[2] == 0)
				blockedState |= 2;

			timeLeft -= timeLeft * tr.Fraction;

			if (numPlanes >= MAX_CLIP_PLANES) {
				VecScale<float, 3>(pl.Velocity, 0, pl.Velocity);
				break;
			}

			VecCopy<float, 3>(tr.PlaneNormal, planes[numPlanes]);
			numPlanes++;

			if (postype != PositionType::GROUND) {
				for (auto i = 0; i < numPlanes; ++i)
					if (planes[i][2] > 0.7)
						ClipVelocity(savedVelocity, planes[i], 1.0f);
					else
						ClipVelocity(savedVelocity, planes[i], 1.0f);

				VecCopy<float, 3>(savedVelocity, pl.Velocity);
			} else {
				int i = 0;
				for (i = 0; i < numPlanes; ++i) {
					VecCopy<float, 3>(savedVelocity, pl.Velocity);
					ClipVelocity(pl.Velocity, planes[i], 1);

					int j;
					for (j = 0; j < numPlanes; ++j)
						if (j != i)
							if (DotProduct<float, float, 3>(pl.Velocity, planes[j]) < 0)
								break;

					if (j == numPlanes)
						break;
				}

				if (i == numPlanes) {
					if (numPlanes != 2) {
						VecScale<float, 3>(pl.Velocity, 0, pl.Velocity);
						break;
					}

					float dir[3];
					CrossProduct<float, float>(planes[0], planes[1], dir);
					auto d = static_cast<float>(DotProduct<float, float, 3>(dir, pl.Velocity));
					VecScale<float, 3>(dir, d, pl.Velocity);
				}

				if (DotProduct<float, float, 3>(pl.Velocity, originalVelocity) <= 0) {
					VecScale<float, 3>(pl.Velocity, 0, pl.Velocity);
					break;
				}
			}
		}

		if (allFraction == 0)
			VecScale<float, 3>(pl.Velocity, 0, pl.Velocity);

		//std::fprintf(HwDLL::GetInstance().log, " o(%.8f %.8f %.8f)\n", pl.Velocity[0], pl.Velocity[1], pl.Velocity[2]);
	}

	int ClipVelocity(float velocity[3], const float normal[3], float overbounce)
	{
		const auto STOP_EPSILON = 0.1;

		auto backoff = static_cast<float>(DotProduct<float, float, 3>(velocity, normal) * overbounce);

		for (size_t i = 0; i < 3; ++i) {
			auto change = normal[i] * backoff;
			velocity[i] -= change;

			if (velocity[i] > -STOP_EPSILON && velocity[i] < STOP_EPSILON)
				velocity[i] = 0;
		}

		if (normal[2] > 0)
			return 1;
		else if (normal[2] == 0)
			return 2;
		else
			return 0;
	}
};

struct Path
{
	static constexpr size_t BITS_PER_FRAME = 3;

	struct Node
	{
		HLTAS::StrafeDir strafe_dir;
		bool jump;
		bool duck;
	};

	std::vector<bool> path;

	inline HLTAS::StrafeDir StrafeDir(size_t frame) const
	{
		return path[frame * BITS_PER_FRAME] ? HLTAS::StrafeDir::RIGHT : HLTAS::StrafeDir::LEFT;
	}

	inline bool Jump(size_t frame) const
	{
		return path[frame * BITS_PER_FRAME + 1];
	}

	inline bool Duck(size_t frame) const
	{
		return path[frame * BITS_PER_FRAME + 2];
	}

	inline size_t FrameCount() const
	{
		return path.size() / BITS_PER_FRAME;
	}

	void Append(int frame)
	{
		path.push_back(frame & 1);
		path.push_back(frame & 2);
		path.push_back(frame & 4);
	}

	void Append(const Node& node)
	{
		path.push_back(node.strafe_dir == HLTAS::StrafeDir::RIGHT ? true : false);
		path.push_back(node.jump);
		path.push_back(node.duck);
	}

	PlayerState Simulate(PlayerState player, HLStrafe::TraceFunc traceFunc) const
	{
		PathSimulator simulator(player, traceFunc);

		for (size_t frame = 0; frame < FrameCount(); ++frame)
			simulator.SimulateFrame(StrafeDir(frame), Jump(frame), Duck(frame));

		return player;
	}

	void Export(PlayerState player, HLStrafe::TraceFunc traceFunc) const
	{
		HLTAS::Input templ;
		auto err = templ.Open("template.hltas").get();

		if (err.Code != HLTAS::ErrorCode::OK) {
			HwDLL::GetInstance().ORIG_Con_Printf("Error loading the script file on line %u: %s\n", err.LineNumber, HLTAS::GetErrorMessage(err).c_str());
			return;
		}

		auto last_frame = templ.GetFrames().back();
		templ.RemoveFrame(templ.GetFrames().size() - 1);
		last_frame.Comments = "End of automatically generated frames.";

		PathSimulator simulator(player, traceFunc);

		for (size_t i = 0; i < FrameCount(); ++i) {
			HLTAS::Frame frame;
			//frame.SetType(HLTAS::StrafeType::MAXACCEL);
			//frame.SetDir(StrafeDir(i));
			frame.Jump = Jump(i);
			frame.Duck = Duck(i);
			frame.Frametime = "0.010000001";

			fprintf(HwDLL::GetInstance().log, "o(%.8f %.8f %.8f) vel(%.8f %.8f %.8f)",
							     player.Origin[0], player.Origin[1], player.Origin[2],
							     player.Velocity[0], player.Velocity[1], player.Velocity[2]);
			simulator.SimulateFrame(StrafeDir(i), Jump(i), Duck(i));
			fprintf(HwDLL::GetInstance().log, "\n");

			frame.SetYaw(player.Yaw + M_U_DEG_HALF);
			frame.Forward = (player.buttons == Button::FORWARD || player.buttons == Button::FORWARD_LEFT || player.buttons == Button::FORWARD_RIGHT);
			frame.Back = (player.buttons == Button::BACK || player.buttons == Button::BACK_LEFT || player.buttons == Button::BACK_RIGHT);
			frame.Left = (player.buttons == Button::LEFT || player.buttons == Button::BACK_LEFT || player.buttons == Button::FORWARD_LEFT);
			frame.Right = (player.buttons == Button::RIGHT || player.buttons == Button::BACK_RIGHT || player.buttons == Button::FORWARD_RIGHT);

			frame.SetRepeats(1);

			if (i == 0)
				frame.Comments = "Beginning of automatically generated frames.";

			templ.InsertFrame(templ.GetFrames().size(), frame);
		}

		fprintf(HwDLL::GetInstance().log, "o(%.8f %.8f %.8f) vel(%.8f %.8f %.8f)\n",
						     player.Origin[0], player.Origin[1], player.Origin[2],
						     player.Velocity[0], player.Velocity[1], player.Velocity[2]);

		templ.InsertFrame(templ.GetFrames().size(), last_frame);
		templ.Save("export.hltas");
	}
};

inline static bool reached_goal(const PlayerState& player)
{
	const auto x = player.Origin[0];
	const auto y = player.Origin[1];

	return (x >= -3264 && x <= -2992) && (y >= 1120 && y <= 1168);
}

unsigned HwDLL::Heuristic(PlayerState player)
{
	// Did we fall down too far?
	if (player.Origin[2] < -430)
		return std::numeric_limits<unsigned>::max();

	unsigned frame_count = 0;

	while (!reached_goal(player) && frame_count < 3000) {
		//HwDLL::GetInstance().ORIG_Con_Printf("Pos: %f %f; Vel: %f %f\n", player.Origin[0], player.Origin[1], player.Velocity[0], player.Velocity[1]);

		++frame_count;

		// Account for possible slopeboosts (assuming perfect full slopeboosts
		// every frame, which is not possible in-game).
		if (player.Velocity[2] < 0
			&& player.Origin[1] >= -2800 && player.Origin[1] <= -1724) {
			float zmul, ymul;
			
			if (player.Origin[1] <= -1770) {
				constexpr float ANGLE = 10.96f * M_PI / 180.0f;
				const float ZM = std::sin(ANGLE) * std::sin(ANGLE);
				const float YM = std::sin(ANGLE) * std::cos(ANGLE);

				zmul = ZM;
				ymul = YM;
			} else {
				constexpr float ANGLE = 28.89f * M_PI / 180.0f;
				const float ZM = std::sin(ANGLE) * std::sin(ANGLE);
				const float YM = std::sin(ANGLE) * std::cos(ANGLE);

				zmul = ZM;
				ymul = YM;
			}

			// -= because Z speed is negative.
			player.Velocity[1] -= player.Velocity[2] * ymul;
			player.Velocity[2] *= zmul;
		}

		// Calculate optimal yaw towards the ending trigger.
		constexpr float TRIGGER_ABSMINS[2] = { -3264, 1120 };
		constexpr float TRIGGER_ABSMAXS[2] = { -2992, 1168 };
		constexpr float TRIGGER_HALFSIZE[2] = {
			(TRIGGER_ABSMAXS[0] - TRIGGER_ABSMINS[0]) / 2,
			(TRIGGER_ABSMAXS[1] - TRIGGER_ABSMINS[1]) / 2
		};
		constexpr float TRIGGER_CENTER[2] = {
			TRIGGER_ABSMINS[0] + TRIGGER_HALFSIZE[0],
			TRIGGER_ABSMINS[1] + TRIGGER_HALFSIZE[1]
		};

		float dir[2] = {
			TRIGGER_CENTER[0] - player.Origin[0],
			TRIGGER_CENTER[1] - player.Origin[1]
		};

		if (dir[0] <= -TRIGGER_HALFSIZE[0])
			dir[0] += TRIGGER_HALFSIZE[0];
		else if (dir[0] >= TRIGGER_HALFSIZE[0])
			dir[0] -= TRIGGER_HALFSIZE[0];
		else
			dir[0] = 0;

		if (dir[1] <= -TRIGGER_HALFSIZE[1])
			dir[1] += TRIGGER_HALFSIZE[1];
		else if (dir[1] >= TRIGGER_HALFSIZE[1])
			dir[1] -= TRIGGER_HALFSIZE[1];
		else
			dir[1] = 0;

		// dir cannot be unit (otherwise reached_goal() would return true).
		const float dir_yaw = std::atan2(dir[1], dir[0]);

		// Max speed strafe towards the ending trigger.
		// Just assume we can groundstrafe and airstrafe always.
		constexpr float AGST = 454.568645048495f;

		const float maxspeed = player.Ducking ? 300 * 0.333f : 300;
		player.Ducking = false;

		float speed = std::hypot(player.Velocity[0], player.Velocity[1]);
		if (speed < 270 / (1 - 4 * 0.01f)) {
			player.Velocity[0] *= (1 - 4 * 0.01f);
			player.Velocity[1] *= (1 - 4 * 0.01f);

			const float dir_length = std::hypot(dir[0], dir[1]);
			dir[0] /= dir_length;
			dir[1] /= dir_length;

			player.Velocity[0] += dir[0] * 0.01f * maxspeed * 10;
			player.Velocity[1] += dir[1] * 0.01f * maxspeed * 10;
		} else {
			const float vel_yaw = std::atan2(player.Velocity[1], player.Velocity[0]);
			float difference = dir_yaw - vel_yaw;
			if (difference >= M_PI)
				difference -= 2 * M_PI;
			else if (difference < -M_PI)
				difference += 2 * M_PI;

			const int strafe_dir = (difference >= 0 ? -1 : 1);

			float L = 30;
			if (speed < AGST) {
				// Groundstrafe
				player.Velocity[0] *= (1 - 4 * 0.01f);
				player.Velocity[1] *= (1 - 4 * 0.01f);
				speed *= (1 - 4 * 0.01f);
				
				L = maxspeed;
			}

			const float tauMA = 0.01f * maxspeed * 10;
			const float tmp = L - tauMA;
			const float ct = tmp / speed;
			const float st = strafe_dir * std::sqrt(1 - ct * ct);

			const float ax = tauMA * (player.Velocity[0] * ct + player.Velocity[1] * st) / speed;
			const float ay = tauMA * (-player.Velocity[0] * st + player.Velocity[1] * ct) / speed;
			player.Velocity[0] += ax;
			player.Velocity[1] += ay;
		}

		// Fine tuning.
		player.Velocity[2] = 0;

		player.Velocity[2] -= 800 * 0.01f;

		// Can't get less than this without falling down.
		if (player.Velocity[2] < -750)
			player.Velocity[2] = -750;

		// Fine tuning.
		if (player.Origin[1] < -3088)
			player.Velocity[2] = 0;

		player.Origin[0] += player.Velocity[0] * 0.01f;
		player.Origin[1] += player.Velocity[1] * 0.01f;
	}

	return frame_count;
}

PlayerState HwDLL::GetCurrentState()
{
	PlayerState player{};

	if (svs->num_clients >= 1) {
		edict_t *pl = *reinterpret_cast<edict_t**>(reinterpret_cast<uintptr_t>(svs->clients) + offEdict);
		if (pl) {
			player.Origin[0] = pl->v.origin[0];
			player.Origin[1] = pl->v.origin[1];
			player.Origin[2] = pl->v.origin[2];
			player.Velocity[0] = pl->v.velocity[0];
			player.Velocity[1] = pl->v.velocity[1];
			player.Velocity[2] = pl->v.velocity[2];

			if (clientstate) {
				float *viewangles = reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(clientstate) + 0x2ABE4);
				player.Yaw = viewangles[1];
			}
		}
	}

	return player;
}

static void GetNeighbors(const PlayerState& player,
                         HLStrafe::TraceFunc traceFunc,
                         std::array<std::pair<Path::Node, PlayerState>, 8>& neighbors,
                         uint8_t& neighbor_count)
{
	static constexpr Path::Node nodes[] = {
		{ HLTAS::StrafeDir::LEFT, false, false },
		{ HLTAS::StrafeDir::RIGHT, false, false },
		{ HLTAS::StrafeDir::LEFT, false, true },
		{ HLTAS::StrafeDir::RIGHT, false, true },
		{ HLTAS::StrafeDir::LEFT, true, false },
		{ HLTAS::StrafeDir::RIGHT, true, false },
		{ HLTAS::StrafeDir::LEFT, true, true },
		{ HLTAS::StrafeDir::RIGHT, true, true }
	};

	neighbor_count = 0;

	for (uint8_t i = 0; i < 4; ++i) {
		Path p;
		p.Append(nodes[i]);
		neighbors[neighbor_count++] = std::make_pair(nodes[i], p.Simulate(player, traceFunc));;
	}

	for (uint8_t i = 4; i < 8; ++i) {
		Path p;
		p.Append(nodes[i]);

		auto state = p.Simulate(player, traceFunc);

		// If jump did nothing, don't consider it.
		if (state.Velocity[2] == neighbors[i - 4].second.Velocity[2])
			continue;

		neighbors[neighbor_count++] = std::make_pair(nodes[i], state);
	}
}

static Path Search(const PlayerState& starting_state, HLStrafe::TraceFunc traceFunc)
{
	// A*
	std::vector<std::pair<Path, unsigned>> open_set = { { Path(), 0 } };

	const auto start = std::chrono::steady_clock::now();
	unsigned long long iterations = 0;

	double total_best = 0.0, total_sim = 0.0, total_erase = 0.0, total_rest = 0.0;

	while (!open_set.empty()) {
		++iterations;

		//auto perf_start = std::chrono::steady_clock::now();
		// Find the node with the lowest F score.
		auto best_node_it = --open_set.cend();
		auto best_score = best_node_it->second;

		auto it = best_node_it;
		while (it != open_set.cbegin()) {
			--it;
			const auto score = it->second;
			if (score < best_score) {
				best_node_it = it;
				best_score = score;
			}
		}
		//double best = std::chrono::duration<double>(std::chrono::steady_clock::now() - perf_start).count();

		if ((iterations & 0xFF) == 0) {
			HwDLL::GetInstance().ORIG_Con_Printf("Open set size: %zu; best node score: %u; best node frame count: %zu\n", open_set.size(), best_node_it->second, best_node_it->first.FrameCount());

			const auto n = std::chrono::steady_clock::now();
			std::chrono::duration<double> diff = n - start;
			if (diff.count() >= 3600) {
				//HwDLL::GetInstance().ORIG_Con_Printf("best: %.8f, sim: %.8f, erase: %.8f, rest: %.8f\n", 1000 * total_best / (iterations - 1), 1000 * total_sim / (iterations - 1), 1000 * total_erase / (iterations - 1), 1000 * total_rest / (iterations - 1));
				return best_node_it->first;
			}
		}

		//total_best += best;

		//perf_start = std::chrono::steady_clock::now();
		// Compute where it takes the player.
		auto player = best_node_it->first.Simulate(starting_state, traceFunc);
		//total_sim += std::chrono::duration<double>(std::chrono::steady_clock::now() - perf_start).count();

		// Did the player reach the goal?
		if (reached_goal(player))
			return best_node_it->first;

		auto best_node = std::move(*best_node_it);

		//perf_start = std::chrono::steady_clock::now();
		open_set.erase(best_node_it);
		//total_erase += std::chrono::duration<double>(std::chrono::steady_clock::now() - perf_start).count();

		std::array<std::pair<Path::Node, PlayerState>, 8> neighbors;
		uint8_t neighbor_count;
		GetNeighbors(player, traceFunc, neighbors, neighbor_count);

		for (uint8_t i = 0; i < neighbor_count; ++i) {
			const auto& neighbor = neighbors[neighbor_count - 1 - i];

			auto node = best_node.first;
			node.Append(neighbor.first);

			//perf_start = std::chrono::steady_clock::now();
			const auto score = node.FrameCount() + HwDLL::Heuristic(neighbor.second);
			//open_set.emplace(std::upper_bound(open_set.cbegin(), open_set.cend(), score, [](unsigned a, const std::pair<Path, unsigned>& b) {
			//                         return a < b.second;
			//                 }),
			//                 node,
			//                 score);
			//total_rest += std::chrono::duration<double>(std::chrono::steady_clock::now() - perf_start).count();
			open_set.emplace_back(std::move(node), score);
		}
	}

	return Path();
}

void HwDLL::StartSearch()
{
	const PlayerState starting_state = GetCurrentState();

	ORIG_Con_Printf("Starting state: Origin{ %.8f %.8f %.8f }, Velocity{ %.8f %.8f %.8f }.\n",
	                starting_state.Origin[0], starting_state.Origin[1], starting_state.Origin[2],
	                starting_state.Velocity[0], starting_state.Velocity[1], starting_state.Velocity[2]);

	InitTracing();

	const auto traceFunc = std::bind(&HwDLL::PlayerTrace2, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	const auto best_path = Search(starting_state, traceFunc);

	//Path p;
	//for (int i = 0; i < 2000; ++i)
	//        p.Append(i);

	//p.Simulate(player, std::bind(&HwDLL::PlayerTrace2, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	log = fopen("sim.log", "w");
	best_path.Export(starting_state, traceFunc);
	fclose(log);

	DeinitTracing();
}

void HwDLL::ForEachCoarseNodeNeighbor(
	const CoarseNode& node,
	std::function<bool(const CoarseNode& neighbor)> already_found,
	std::function<void(CoarseNode)> callback
) {
	// No-damage fall distance.
	const float MAX_FALL_DISTANCE = 210;
	// Jump + duck height is a little above 59.
	const float MAX_JUMP_HEIGHT = 60;

	float current_origin[3];
	CoarseNodeOrigin(node, current_origin);

	for (int dx = -4; dx <= 4; dx++) {
		for (int dy = -4; dy <= 4; dy++) {
			auto adjacent = CoarseNode(
				node.x + dx,
				node.y + dy,
				node.z,
				0,
				node.index
			);

			// Check if the node is too far.
			if (node.distance_to(adjacent) >= 136) {
#ifdef NEIGHBOR_DEBUG
				ORIG_Con_Printf("%d, %d: too far\n", dx, dy);
#endif
				continue;
			}

			adjacent.jump = (abs(dx) > 1 || abs(dy) > 1);

			// Check to filter out some same node searches quickly.
			if (already_found(adjacent)) {
#ifdef NEIGHBOR_DEBUG
				ORIG_Con_Printf("%d, %d: already found 1\n", dx, dy);
#endif
				continue;
			}

			float origin[3];
			CoarseNodeOrigin(adjacent, origin);

			// Trace down to find the ground.
			float adjusted_origin[3];
			VecCopy(origin, adjusted_origin);
			adjusted_origin[2] -= MAX_FALL_DISTANCE;

			auto tr = PlayerTrace(origin, adjusted_origin, HullType::NORMAL);

			// The node is inside a wall.
			if (tr.StartSolid)
			{
				// Try starting from 18 units up for the stepsize.
				adjusted_origin[2] = origin[2];

				float stepsize = 18;
				if (adjacent.jump)
					// When jumping we can jump a little higher.
					stepsize = 33;
				origin[2] += stepsize;

				tr = PlayerTrace(origin, adjusted_origin, HullType::NORMAL);

				// Still inside a wall.
				if (tr.StartSolid) {
					// See if we can jump up.
					if (adjacent.jump) {
						// Not if we need to jump forward though.
#ifdef NEIGHBOR_DEBUG
						ORIG_Con_Printf("%d, %d: jump forward but higher than stepsize\n", dx, dy);
#endif
						continue;
					}

					origin[2] = adjusted_origin[2] + MAX_JUMP_HEIGHT;

					tr = PlayerTrace(origin, adjusted_origin, HullType::NORMAL);

					// Still inside a wall.
					if (tr.StartSolid) {
#ifdef NEIGHBOR_DEBUG
						ORIG_Con_Printf("%d, %d: higher than jump up\n", dx, dy);
#endif
						continue;
					}

					// All good.
					adjacent.jump_up = true;
				}

				adjusted_origin[2] = tr.EndPos[2];

				// Since our coarse node distance is large, tracing directly from current origin to
				// adjusted origin is likely to hit a corner. So trace up and then forward.
				float up[3];
				VecCopy(current_origin, up);
				up[2] = tr.EndPos[2];

				tr = PlayerTrace(current_origin, up, HullType::NORMAL);
				if (tr.Fraction != 1.f) {
#ifdef NEIGHBOR_DEBUG
					ORIG_Con_Printf("%d, %d: can't trace up\n", dx, dy);
#endif
					continue;
				}

				tr = PlayerTrace(up, adjusted_origin, HullType::NORMAL);
				if (tr.Fraction != 1.f) {
#ifdef NEIGHBOR_DEBUG
					ORIG_Con_Printf("%d, %d: can't trace from up to position\n", dx, dy);
#endif
					continue;
				}

				// We are good to go.
			}
			// The node is ontop of a pit.
			else if (tr.Fraction == 1.f) {
#ifdef NEIGHBOR_DEBUG
				ORIG_Con_Printf("%d, %d: ontop of a pit\n", dx, dy);
#endif
				continue;
			} else {
				adjusted_origin[2] = tr.EndPos[2];

				// Trace from the current position to check if we can move there.
				// Trace to the original (not adjusted) origin, otherwise the trace hits corners.
				tr = PlayerTrace(current_origin, origin, HullType::NORMAL);

				// Can't move there.
				if (tr.Fraction != 1.f) {
#ifdef NEIGHBOR_DEBUG
					ORIG_Con_Printf("%d, %d: can't trace forward to above position\n", dx, dy);
#endif
					continue;
				}
			}

			// Check if there's a gap in the middle.
			//
			// Find the middle point by tracing between current and adjusted origin, but at the
			// highest z between the two. This should work correctly both when going up and when
			// doing down.
			if (!adjacent.jump) {
				float max_z_current_origin[3];
				VecCopy(current_origin, max_z_current_origin);
				float max_z_adjusted_origin[3];
				VecCopy(adjusted_origin, max_z_adjusted_origin);
				max_z_current_origin[2] = std::max(max_z_current_origin[2], max_z_adjusted_origin[2]);
				max_z_adjusted_origin[2] = max_z_current_origin[2];

				float direction[3];
				VecSubtract(max_z_adjusted_origin, max_z_current_origin, direction);
				VecScale(direction, 0.5, direction);
				float middle[3];
				VecAdd(max_z_current_origin, direction, middle);
				float middle_down[3];
				VecCopy(middle, middle_down);
				middle_down[2] -= MAX_FALL_DISTANCE;
				tr = PlayerTrace(middle, middle_down, HullType::NORMAL);

				if (tr.StartSolid) {
					// Should be impossible.
					ORIG_Con_Printf("%d, %d: tr.StartSolid when checking middle gap.\n", dx, dy);
					continue;
				}

				// There's a large gap in the middle.
				if (tr.Fraction == 1.f) {
#ifdef NEIGHBOR_DEBUG
					ORIG_Con_Printf("%d, %d: middle gap too large\n", dx, dy);
#endif
					continue;
				}

				// The gap is larger than stepsize would allow.
				float max_gap = 18;
				if (adjacent.jump_up) {
					max_gap = MAX_JUMP_HEIGHT;
				}

				if (adjusted_origin[2] - tr.EndPos[2] > max_gap) {
#ifdef NEIGHBOR_DEBUG
					ORIG_Con_Printf("%d, %d: middle gap larger than %f\n", dx, dy, max_gap);
#endif
					continue;
				}
			}

			// Set the origin to the down origin to force the node to the ground.
			adjacent.z = adjusted_origin[2];

			// Don't push nodes that we already know about.
			if (already_found(adjacent)) {
#ifdef NEIGHBOR_DEBUG
				ORIG_Con_Printf("%d, %d: already found 2\n", dx, dy);
#endif
				continue;
			}

			callback(adjacent);
		}
	}
}

void HwDLL::FindCoarseNodes()
{
	if (finding_coarse_nodes)
	{
		finding_coarse_nodes = false;
		return;
	}

	finding_coarse_nodes = true;

	finding_coarse_path = false;
	coarse_path_open_set.clear();
	coarse_path_nodes.clear();
	coarse_path_distances.clear();
	coarse_path_closed_set.clear();
	coarse_path_target = CoarseNode(0, 0, 0, 0, 0);
	coarse_path_final.clear();
	following_coarse_path = false;

	const PlayerState starting_state = GetCurrentState();
	ORIG_Con_Printf("Starting state: Origin{ %.8f %.8f %.8f }.\n",
	                starting_state.Origin[0], starting_state.Origin[1], starting_state.Origin[2]);

	VecCopy(starting_state.Origin, coarse_node_base_origin);
	auto starting_node = CoarseNode(0, 0, starting_state.Origin[2], 0, 0);

	coarse_nodes.clear();
	coarse_nodes_vector.clear();
	next_coarse_nodes = std::queue<CoarseNode>();
	next_coarse_nodes.push(starting_node);
	coarse_nodes.push_back(starting_node);
	coarse_nodes_vector.push_back(starting_node);
}

void HwDLL::FindCoarseNodesStep()
{
	if (!finding_coarse_nodes)
		return;

	// Found all nodes.
	if (next_coarse_nodes.empty())
		return;

	InitTracing();

	const auto traceFunc = std::bind(&HwDLL::PlayerTrace2, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

	// Empty the whole queue on a single iteration.
	auto steps = next_coarse_nodes.size();
	ORIG_Con_Printf("Queue size: %u\n.", (unsigned) steps);
	for (size_t step = 0; step < steps; ++step) {
		CoarseNode current = next_coarse_nodes.front();
		next_coarse_nodes.pop();

		ForEachCoarseNodeNeighbor(current, [this](const CoarseNode& adjacent){
			auto it = std::find(coarse_nodes.cbegin(), coarse_nodes.cend(), adjacent);
			return it != coarse_nodes.cend();
		}, [this](CoarseNode adjacent){
			adjacent.index = coarse_nodes_vector.size();
			coarse_nodes.push_back(adjacent);
			coarse_nodes_vector.push_back(adjacent);
			next_coarse_nodes.push(adjacent);
		});
	}

	DeinitTracing();

#ifdef NEIGHBOR_DEBUG
	finding_coarse_nodes = false;
#endif

	if (next_coarse_nodes.empty()) {
		finding_coarse_nodes = false;
		ORIG_Con_Printf("Found %u nodes.", (unsigned) coarse_nodes.size());
	}
}

void HwDLL::FindCoarsePath()
{
	if (finding_coarse_path)
	{
		finding_coarse_path = false;
		return;
	}

	finding_coarse_path = true;

	finding_coarse_nodes = false;
	coarse_nodes.clear();
	coarse_nodes_vector.clear();
	next_coarse_nodes = std::queue<CoarseNode>();
	following_coarse_path = false;

	const PlayerState starting_state = GetCurrentState();
	ORIG_Con_Printf("Starting state: Origin{ %.8f %.8f %.8f }.\n",
	                starting_state.Origin[0], starting_state.Origin[1], starting_state.Origin[2]);

	VecCopy(starting_state.Origin, coarse_node_base_origin);
	auto starting_node = CoarseNode(0, 0, starting_state.Origin[2], 0, 0);

	CustomHud::UpdatePlayerInfo(starting_state.Velocity, starting_state.Origin);
	float start[3], end[3];
	CustomHud::SetupTraceVectors(start, end);

	InitTracing();
	auto tr = PlayerTrace(start, end, HullType::POINT);

	coarse_path_target = CoarseNode(
		(tr.EndPos[0] - starting_state.Origin[0]) / COARSE_NODE_STEP,
		(tr.EndPos[1] - starting_state.Origin[1]) / COARSE_NODE_STEP,
		tr.EndPos[2],
		0,
		0
	);

	// Put it at the correct height.
	float pos[3], pos_up[3];
	CoarseNodeOrigin(coarse_path_target, pos);
	VecCopy(pos, pos_up);
	pos_up[2] += 37;
	tr = PlayerTrace(pos_up, pos, HullType::NORMAL);
	coarse_path_target.z = tr.EndPos[2];
	DeinitTracing();

	coarse_path_open_set.clear();
	coarse_path_nodes.clear();
	coarse_path_distances.clear();
	coarse_path_closed_set.clear();
	coarse_path_final.clear();

	// This 0.f should be heuristic but it doesn't matter here.
	coarse_path_open_set.emplace_back(starting_node, 0.f);
	coarse_path_nodes.push_back(starting_node);
	coarse_path_distances.push_back(0.f);
}

void HwDLL::FindCoarsePathStep()
{
	if (!finding_coarse_path)
		return;

	// Nothing to search.
	if (coarse_path_open_set.empty())
		return;

	InitTracing();

	ORIG_Con_Printf("Open set size: %u\n", (unsigned) coarse_path_open_set.size());

	for (int steps = 0; steps < 10; ++steps) {
		if (coarse_path_open_set.empty())
			break;

		// Find the node with the lowest F score.
		auto best_node_it = --coarse_path_open_set.cend();
		auto best_score = best_node_it->second;

		auto it = best_node_it;
		while (it != coarse_path_open_set.cbegin()) {
			--it;
			const auto score = it->second;
			if (score < best_score) {
				best_node_it = it;
				best_score = score;
			}
		}

		auto current = best_node_it->first;
		coarse_path_open_set.erase(best_node_it);
		coarse_path_closed_set.push_back(current);

		if (current == coarse_path_target) {
			ORIG_Con_Printf("Reached target, distance = %f.\n", coarse_path_distances[current.index]);
			coarse_path_target = current;
			finding_coarse_path = false;

			while (current.index != current.parent) {
				coarse_path_final.push_back(current.index);
				current = coarse_path_nodes[current.parent];
			}

			return;
		}

		ForEachCoarseNodeNeighbor(current, [this](const CoarseNode& adjacent){
			auto it = std::find(coarse_path_closed_set.cbegin(), coarse_path_closed_set.cend(),
				adjacent);
			return it != coarse_path_closed_set.cend();
		}, [this, &current](CoarseNode adjacent){
			adjacent.index = coarse_path_nodes.size();

			float distance = coarse_path_distances[current.index] + current.distance_to(adjacent);
			float heuristic = adjacent.distance_to(coarse_path_target);

			for (auto& p: coarse_path_open_set) {
				if (p.first == adjacent) {
					if (distance + heuristic < p.second) {
						p.second = distance + heuristic;
						p.first.parent = current.index;
						p.first.jump = adjacent.jump;
						p.first.jump_up = adjacent.jump_up;
						coarse_path_nodes[p.first.index] = p.first;
						coarse_path_distances[p.first.index] = distance;
					}

					adjacent = p.first;
					break;
				}
			}

			if (adjacent.index == coarse_path_nodes.size()) {
				coarse_path_open_set.emplace_back(adjacent, distance + heuristic);
				coarse_path_nodes.push_back(adjacent);
				coarse_path_distances.push_back(distance);
			}
		});
	}

	DeinitTracing();

	if (coarse_path_open_set.empty())
		ORIG_Con_Printf("Couldn't reach the target.");
}

void HwDLL::FollowCoarsePath()
{
	if (following_coarse_path) {
		following_coarse_path = false;
		return;
	}

	if (coarse_path_final.empty()) {
		ORIG_Con_Printf("No path to follow.");
		return;
	}

	following_coarse_path = true;
	following_next_node = coarse_path_final.size() - 1;
	going_back_frames_left = 0;
}

void HwDLL::FollowCoarsePathStep()
{
	if (!following_coarse_path)
		return;

	FillPlayerData();
	float speed = Length(player.Velocity);

	auto target = coarse_path_nodes[coarse_path_final[following_next_node]];
	float target_origin[3];
	CoarseNodeOrigin(target, target_origin);
	auto distance = Distance(player.Origin, target_origin);
	auto distance_2d = Distance<float, float, 2>(player.Origin, target_origin);

	InitTracing();

	// Only switch to the next node once fully stopped.
	if (distance < 1.f && IsZero(player.Velocity)) {
		// Came close enough to the node.
		if (following_next_node == 0) {
			ORIG_Con_Printf("Reached target.\n");
			following_coarse_path = false;
			DeinitTracing();
			return;
		}

		// Switch to the next one.
		following_next_node--;
		target = coarse_path_nodes[coarse_path_final[following_next_node]];
		CoarseNodeOrigin(target, target_origin);
		distance = Distance(player.Origin, target_origin);
		distance_2d = Distance<float, float, 2>(player.Origin, target_origin);

		float difference[3];
		VecSubtract(target_origin, player.Origin, difference);
		Normalize(difference, difference);
		VecScale(difference, 8, difference); // Approximately the number of units when we jump.
		float jump_point[3];
		VecAdd(player.Origin, difference, jump_point);
		float down[3];
		VecCopy(jump_point, down);
		down[2] -= 1;
		auto tr = PlayerTrace(jump_point, down, HullType::NORMAL);
		if (tr.Fraction == 1) {
			// There's a gap where we need to jump so go back first.
			ORIG_Con_Printf("There's a gap, going back\n");
			going_back_frames_left = 7;
		}
	}

	float difference[3];
	VecSubtract(target_origin, player.Origin, difference);
	double yaw = Atan2(difference[1], difference[0]) * M_RAD2DEG;

	bool slow_down_in_the_air = false;

	auto traceFunc = std::bind(&HwDLL::PlayerTrace, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	bool in_air = GetPositionType(player, traceFunc) != PositionType::GROUND;
	if (in_air) {
		if (speed > 30.f && distance_2d < 5.f) {
			// We're in the air and moving too quickly. Try to slow down.
			slow_down_in_the_air = true;
		}
	}
	DeinitTracing();

	auto frame = Frame();

	if (slow_down_in_the_air) {
		yaw = Atan2(player.Velocity[1], player.Velocity[0]) * M_RAD2DEG;
		frame.Back = true;
	} else {
		if (distance >= 1.)
			frame.Forward = true;
		if (distance < 5.f)
			frame.Use = true;

		if (going_back_frames_left > 0) {
			going_back_frames_left--;
			frame.Back = true;
			frame.Forward = false;
		}

		if (distance_2d >= 5 && target.jump && !in_air && speed > 200
				&& going_back_frames_left == 0) {
			frame.Jump = true;

			auto& parent = coarse_path_nodes[target.parent];
			float origin[3];
			CoarseNodeOrigin(parent, origin);
			float distance = Distance<float, float, 2>(origin, player.Origin);
			ORIG_Con_Printf("Jumping %f units from parent\n", distance);
		}

		if (target_origin[2] - player.Origin[2] > 18 && target.jump_up && !in_air
				&& !player.Ducking) {
			frame.Jump = true;
		}
	}

	if ((target.jump_up || target.jump) && in_air) {
		frame.Dbc = true;
	}

	frame.SetYaw(yaw);

	runningFrames = true;
	totalFramebulks = 1;
	currentFramebulk = 0;
	currentRepeat = 0;
	StrafeState = HLStrafe::CurrentState();
	ButtonsPresent = false;
	SharedRNGSeedPresent = false;
	SetNonSharedRNGSeed = false;
	thisFrameIs0ms = false;

	input.Clear();
	input.InsertFrame(0, frame);
}

void HwDLL::FillPlayerData()
{
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
		}
	}
}

void HwDLL::CoarseNodeOrigin(const CoarseNode& node, float origin[3])
{
	VecCopy(coarse_node_base_origin, origin);
	origin[0] += node.x * COARSE_NODE_STEP;
	origin[1] += node.y * COARSE_NODE_STEP;
	origin[2] = node.z;
}

void HwDLL::CoarseNodeDrawOrigin(const CoarseNode& node, float origin[3])
{
	CoarseNodeOrigin(node, origin);
	origin[2] -= 36;
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

	CustomHud::SaveTimeToDemo();

	if (CVars::bxt_stop_demo_on_changelevel.GetBool()) {
		dontStopAutorecord = true;
		ORIG_CL_Stop_f();
		dontStopAutorecord = false;
	}

	insideHost_Changelevel2_f = true;
	ORIG_Host_Changelevel2_f();
	insideHost_Changelevel2_f = false;
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

	if (insideHost_Changelevel2_f) {
		if (ret && !autoRecordDemoName.empty()) {
			if (*demorecording == 0)
				autoRecordNow = true;
		} else {
			autoRecordNow = false;
			autoRecordDemoName.clear();
		}
	}

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
	if (!insideHost_Loadgame_f && !insideHost_Reload_f && !dontStopAutorecord) {
		autoRecordNow = false;
		autoRecordDemoName.clear();
	}

	// Write the final time to the demo.
	CustomHud::SaveTimeToDemo();
	RuntimeData::SaveStored();

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
	HwDLL::GetInstance().Unhook();
	Hooks::HookModule(L"hl.exe");
}

HOOK_DEF_0(HwDLL, void, __cdecl, CL_Record_f)
{
	RuntimeData::Clear();

	ORIG_CL_Record_f();

	if (IsRecordingDemo())
		SaveInitialDataToDemo();
}

HOOK_DEF_1(HwDLL, void, __cdecl, Cbuf_AddText, const char*, text)
{
	// This isn't necessarily a bound command
	// (because something might have been added in the VGUI handler)
	// but until something like that comes up it should be fine.
	if (insideKeyEvent && !ClientDLL::GetInstance().IsInsideKeyEvent()
		&& !(text[0] == '\n' && text[1] == '\0'))
		RuntimeData::Add(RuntimeData::BoundCommand { text });

	ORIG_Cbuf_AddText(text);
}

HOOK_DEF_1(HwDLL, void, __cdecl, Cbuf_InsertTextLines, const char*, text)
{
	if (insideExec)
		execScript += text;

	ORIG_Cbuf_InsertTextLines(text);
}

HOOK_DEF_1(HwDLL, void, __cdecl, Cmd_TokenizeString, char*, text)
{
	ORIG_Cmd_TokenizeString(text);

	if (insideCbuf_Execute && ORIG_Cmd_Argc() > 0) {
		auto command = ORIG_Cmd_Argv(0);

		for (auto alias = cmd_alias; alias; alias = alias->next) {
			if (!std::strncmp(alias->name, command, sizeof(alias->name))) {
				RuntimeData::Add(RuntimeData::AliasExpansion {
					alias->name,
					alias->value
				});

				return;
			}
		}

		RuntimeData::Add(RuntimeData::CommandExecution { text });
	}
}

HOOK_DEF_2(HwDLL, void, __cdecl, Key_Event, int, key, int, down)
{
	insideKeyEvent = true;

	ORIG_Key_Event(key, down);

	insideKeyEvent = false;
}

HOOK_DEF_0(HwDLL, void, __cdecl, Cmd_Exec_f)
{
	insideExec = true;

	ORIG_Cmd_Exec_f();

	insideExec = false;

	if (!execScript.empty()) {
		RuntimeData::Add(RuntimeData::ScriptExecution {
			ORIG_Cmd_Argv(1),
			std::move(execScript)
		});

		execScript.clear();
	}
}

// This function is hooked instead of some top-level drawing functions because
// we want NPCs to remain opaque, to make them more visible. This function draws
// the worldspawn and other brush entities but not studio models (NPCs).
HOOK_DEF_2(HwDLL, void, __cdecl, R_DrawSequentialPoly, msurface_t *, surf, int, face)
{
	if (!(CVars::sv_cheats.GetBool() && CVars::bxt_wallhack.GetBool())) {
		ORIG_R_DrawSequentialPoly(surf, face);
		return;
	}

	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	if (CVars::bxt_wallhack_additive.GetBool()) {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	} else {
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	glColor4f(1.0f, 1.0f, 1.0f, CVars::bxt_wallhack_alpha.GetFloat());

	ORIG_R_DrawSequentialPoly(surf, face);

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

HOOK_DEF_0(HwDLL, void, __cdecl, R_Clear)
{
	// This is needed or everything will look washed out or with unintended
	// motion blur.
	if (CVars::sv_cheats.GetBool() && CVars::bxt_wallhack.GetBool()) {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	ORIG_R_Clear();
}

HOOK_DEF_2(HwDLL, byte *, __cdecl, Mod_LeafPVS, mleaf_t *, leaf, model_t *, model)
{
	// Idea from advancedfx: this is done so that distant NPCs don't disappear,
	// as they do with r_novis 1.
	return ORIG_Mod_LeafPVS(CVars::bxt_novis.GetBool() ? model->leafs : leaf, model);
}
