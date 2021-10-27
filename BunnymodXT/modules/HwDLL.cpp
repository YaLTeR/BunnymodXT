#include "../stdafx.hpp"

#include <boost/algorithm/string/replace.hpp>
#include <cerrno>
#include <GL/gl.h>
#include "../sptlib-wrapper.hpp"
#include <SPTLib/MemUtils.hpp>
#include <SPTLib/Hooks.hpp>
#include <util.hpp>
#include "HwDLL.hpp"
#include "ClientDLL.hpp"
#include "ServerDLL.hpp"
#include "SDL.hpp"
#include "../patterns.hpp"
#include "../cvars.hpp"
#include "../hud_custom.hpp"
#include "../interprocess.hpp"
#include "../bunnymodxt.hpp"
#include "../cmd_wrapper.hpp"
#include "../runtime_data.hpp"
#include "../git_revision.hpp"
#include "../custom_triggers.hpp"
#include "../simulation_ipc.hpp"

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

extern "C" void __cdecl R_DrawSkyBox()
{
	return HwDLL::HOOKED_R_DrawSkyBox();
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

extern "C" void __cdecl R_DrawViewModel()
{
	HwDLL::HOOKED_R_DrawViewModel();
}

extern "C" byte *__cdecl Mod_LeafPVS(mleaf_t *leaf, model_t *model)
{
	return HwDLL::HOOKED_Mod_LeafPVS(leaf, model);
}

extern "C" void __cdecl SV_AddLinksToPM_(void *node, float *pmove_mins, float *pmove_maxs)
{
	HwDLL::HOOKED_SV_AddLinksToPM_(node, pmove_mins, pmove_maxs);
}

extern "C" void __cdecl SV_WriteEntitiesToClient(client_t* client, void* msg)
{
	HwDLL::HOOKED_SV_WriteEntitiesToClient(client, msg);
}

extern "C" void __cdecl VGuiWrap_Paint(int paintAll)
{
	HwDLL::HOOKED_VGuiWrap_Paint(paintAll);
}

extern "C" int __cdecl DispatchDirectUserMsg(char* pszName, int iSize, void* pBuf)
{
	return HwDLL::HOOKED_DispatchDirectUserMsg(pszName, iSize, pBuf);
}

extern "C" void __cdecl SV_SetMoveVars()
{
	HwDLL::HOOKED_SV_SetMoveVars();
}

extern "C" void __cdecl R_StudioCalcAttachments()
{
	HwDLL::HOOKED_R_StudioCalcAttachments();
}

extern "C" void __cdecl VectorTransform(float *in1, float *in2, float *out)
{
	HwDLL::HOOKED_VectorTransform(in1, in2, out);
}

extern "C" qboolean __cdecl BIsValveGame()
{
	return true;
}

extern "C" void __cdecl EmitWaterPolys(msurface_t *fa, int direction)
{
	return HwDLL::HOOKED_EmitWaterPolys(fa, direction);
}

extern "C" void __cdecl S_StartDynamicSound(int entnum, int entchannel, void *sfx, vec_t *origin,
                                            float fvol, float attenuation, int flags, int pitch)
{
	HwDLL::HOOKED_S_StartDynamicSound(entnum, entchannel, sfx, origin, fvol, attenuation, flags, pitch);
}

extern "C" long __cdecl RandomLong(long low, long high)
{
	return HwDLL::HOOKED_RandomLong(low, high);
}

extern "C" void __cdecl VGuiWrap2_NotifyOfServerConnect(const char *game, int IP, int port)
{
	HwDLL::HOOKED_VGuiWrap2_NotifyOfServerConnect(game, IP, port);
}

// BunnymodXT has a few library dependencies which are loaded before `hw.so`.
// The dependency chain leads to `libbsd.so.0` on certain systems (e.g. Ubuntu 21.04),
// which happens to contain functions (`MD5Init()` and such) clashing with the ones in `hw.so`.
// `hw.so` then unintentionally links to those wrong functions which causes HL to hang.
// Since BXT is the first in the chain, it can provide those functions and forward them to `hw.so`
// so it uses the expected implementations, thus fixing the hang.
extern "C" void __cdecl MD5Init(MD5Context_t *context)
{
	HwDLL::HOOKED_MD5Init(context);
}

extern "C" void __cdecl MD5Update(MD5Context_t *context, unsigned char const *buf, unsigned int len)
{
	HwDLL::HOOKED_MD5Update(context, buf, len);
}

extern "C" void __cdecl MD5Final(unsigned char digest[16], MD5Context_t *context)
{
	HwDLL::HOOKED_MD5Final(digest, context);
}

extern "C" void __cdecl MD5Transform(unsigned int buf[4], unsigned int const in[16])
{
	HwDLL::HOOKED_MD5Transform(buf, in);
}

extern "C" int __cdecl MD5_Hash_File(unsigned char digest[16], char *pszFileName, int bUsefopen, int bSeed, unsigned int seed[4])
{
	return HwDLL::HOOKED_MD5_Hash_File(digest, pszFileName, bUsefopen, bSeed, seed);
}

extern "C" char* __cdecl MD5_Print(unsigned char hash[16])
{
	return HwDLL::HOOKED_MD5_Print(hash);
}

extern "C" void __fastcall _ZN7CBaseUI10HideGameUIEv(void* thisptr)
{
	return HwDLL::HOOKED_CBaseUI__HideGameUI(thisptr);
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

#ifdef _WIN32
	// Make it possible to run multiple Half-Life instances.
	auto mutex = OpenMutexA(SYNCHRONIZE, FALSE, "ValveHalfLifeLauncherMutex");
	if (mutex) {
		EngineMsg("Releasing the launcher mutex.\n");
		ReleaseMutex(mutex);
		CloseHandle(mutex);
	}
#endif

	FindStuff();

	// Get the seed (if we're not resetting, in that case we have the seed already).
	if (resetState == ResetState::NORMAL) {
		auto script = std::getenv("BXT_SCRIPT");
		if (script) {
			std::string filename(script);
			auto err = input.Open(filename);
			if (err.Code != HLTAS::ErrorCode::OK) {
				const auto& message = input.GetErrorMessage();
				if (message.empty()) {
					EngineWarning("Error loading the script file on line %u: %s\n", err.LineNumber, HLTAS::GetErrorMessage(err).c_str());
				} else {
					EngineWarning("Error loading the script: %s\n", message.c_str());
				}
			} else
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
			MemUtils::MarkAsExecutable(ORIG_R_DrawSkyBox);
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
			MemUtils::MarkAsExecutable(ORIG_R_DrawViewModel);
			MemUtils::MarkAsExecutable(ORIG_Mod_LeafPVS);
			MemUtils::MarkAsExecutable(ORIG_SV_AddLinksToPM_);
			MemUtils::MarkAsExecutable(ORIG_SV_WriteEntitiesToClient);
			MemUtils::MarkAsExecutable(ORIG_VGuiWrap_Paint);
			MemUtils::MarkAsExecutable(ORIG_DispatchDirectUserMsg);
			MemUtils::MarkAsExecutable(ORIG_SV_SetMoveVars);
			MemUtils::MarkAsExecutable(ORIG_R_StudioCalcAttachments);
			MemUtils::MarkAsExecutable(ORIG_VectorTransform);
			MemUtils::MarkAsExecutable(ORIG_EmitWaterPolys);
			MemUtils::MarkAsExecutable(ORIG_S_StartDynamicSound);
			MemUtils::MarkAsExecutable(ORIG_VGuiWrap2_NotifyOfServerConnect);
			MemUtils::MarkAsExecutable(ORIG_R_StudioSetupBones);
			MemUtils::MarkAsExecutable(ORIG_CBaseUI__HideGameUI);
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
			ORIG_R_DrawSkyBox, HOOKED_R_DrawSkyBox,
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
			ORIG_R_DrawViewModel, HOOKED_R_DrawViewModel,
			ORIG_Mod_LeafPVS, HOOKED_Mod_LeafPVS,
			ORIG_SV_AddLinksToPM_, HOOKED_SV_AddLinksToPM_,
			ORIG_SV_WriteEntitiesToClient, HOOKED_SV_WriteEntitiesToClient,
			ORIG_VGuiWrap_Paint, HOOKED_VGuiWrap_Paint,
			ORIG_DispatchDirectUserMsg, HOOKED_DispatchDirectUserMsg,
			ORIG_SV_SetMoveVars, HOOKED_SV_SetMoveVars,
			ORIG_VectorTransform, HOOKED_VectorTransform,
			ORIG_R_StudioCalcAttachments, HOOKED_R_StudioCalcAttachments,
			ORIG_EmitWaterPolys, HOOKED_EmitWaterPolys,
			ORIG_S_StartDynamicSound, HOOKED_S_StartDynamicSound,
			ORIG_VGuiWrap2_NotifyOfServerConnect, HOOKED_VGuiWrap2_NotifyOfServerConnect,
			ORIG_R_StudioSetupBones, HOOKED_R_StudioSetupBones,
			ORIG_CBaseUI__HideGameUI, HOOKED_CBaseUI__HideGameUI);
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
			ORIG_Host_Changelevel2_f,
			ORIG_SCR_BeginLoadingPlaque,
			ORIG_Host_FilterTime,
			ORIG_V_FadeAlpha,
			ORIG_R_DrawSkyBox,
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
			ORIG_R_DrawViewModel,
			ORIG_Mod_LeafPVS,
			ORIG_SV_AddLinksToPM_,
			ORIG_SV_WriteEntitiesToClient,
			ORIG_VGuiWrap_Paint,
			ORIG_DispatchDirectUserMsg,
			ORIG_SV_SetMoveVars,
			ORIG_VectorTransform,
			ORIG_R_StudioCalcAttachments,
			ORIG_EmitWaterPolys,
			ORIG_S_StartDynamicSound,
			ORIG_VGuiWrap2_NotifyOfServerConnect,
			ORIG_R_StudioSetupBones,
			ORIG_CBaseUI__HideGameUI);
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
	ORIG_R_DrawSkyBox = nullptr;
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
	ORIG_R_DrawViewModel = nullptr;
	ORIG_Mod_LeafPVS = nullptr;
	ORIG_SV_AddLinksToPM_ = nullptr;
	ORIG_SV_WriteEntitiesToClient = nullptr;
	ORIG_VGuiWrap_Paint = nullptr;
	ORIG_DispatchDirectUserMsg = nullptr;
	ORIG_SV_SetMoveVars = nullptr;
	ORIG_studioapi_GetCurrentEntity = nullptr;
	ORIG_R_StudioCalcAttachments = nullptr;
	ORIG_VectorTransform = nullptr;
	ORIG_EmitWaterPolys = nullptr;
	ORIG_S_StartDynamicSound = nullptr;
	ORIG_VGuiWrap2_NotifyOfServerConnect = nullptr;
	ORIG_R_StudioSetupBones = nullptr;
	ORIG_MD5Init = nullptr;
	ORIG_MD5Update = nullptr;
	ORIG_MD5Final = nullptr;
	ORIG_MD5Transform = nullptr;
	ORIG_MD5_Hash_File = nullptr;
	ORIG_MD5_Print = nullptr;
	ORIG_CBaseUI__HideGameUI = nullptr;

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
	offMaxEdicts = 0;
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
	cvar_vars = nullptr;
	movevars = nullptr;
	offZmax = 0;
	frametime_remainder = nullptr;
	pstudiohdr = nullptr;
	framesTillExecuting = 0;
	executing = false;
	insideCbuf_Execute = false;
	finishingLoad = false;
	dontPauseNextCycle = false;
	changelevel = false;
	recording = false;
	pauseOnTheFirstFrame = false;
	insideSeedRNG = false;
	insideSStartDynamicSound = false;
	LastRandomSeed = 0;
	player = HLStrafe::PlayerData();
	currentRepeat = 0;
	movementFrameCounter = 0;
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
	insideRStudioCalcAttachmentsViewmodel = false;
	insideHideGameUI = false;
	hltas_filename.clear();
	newTASFilename.clear();
	newTASResult.Clear();
	libTASExportFile.close();

	tas_editor_mode = TASEditorMode::DISABLED;
	tas_editor_input = EditedInput();
	tas_editor_delete_point = false;
	tas_editor_insert_point = false;
	tas_editor_toggle_s03 = false;
	tas_editor_toggle_s13 = false;
	tas_editor_toggle_s22 = false;
	tas_editor_toggle_lgagst = false;
	tas_editor_toggle_autojump = false;
	tas_editor_toggle_ducktap = false;
	tas_editor_toggle_jumpbug = false;
	tas_editor_toggle_dbc = false;
	tas_editor_toggle_dbc_ceilings = false;
	tas_editor_toggle_dbg = false;
	tas_editor_toggle_dwj = false;
	tas_editor_toggle_forward = false;
	tas_editor_toggle_left = false;
	tas_editor_toggle_right = false;
	tas_editor_toggle_back = false;
	tas_editor_toggle_up = false;
	tas_editor_toggle_down = false;
	tas_editor_toggle_jump = false;
	tas_editor_toggle_duck = false;
	tas_editor_toggle_use = false;
	tas_editor_toggle_attack1 = false;
	tas_editor_toggle_attack2 = false;
	tas_editor_toggle_reload = false;
	tas_editor_set_yaw = false;
	tas_editor_set_pitch = false;
	tas_editor_set_repeats = false;
	tas_editor_set_commands = false;
	tas_editor_unset_yaw = false;
	tas_editor_unset_pitch = false;
	tas_editor_set_run_point_and_save = false;
	free_cam_active = false;
	extendPlayerTraceDistanceLimit = false;

	if (resetState == ResetState::NORMAL) {
		input.Clear();
		hlstrafe_version = HLStrafe::MAX_SUPPORTED_VERSION;
		demoName.clear();
		saveName.clear();
		frametime0ms.clear();
		runningFrames = false;
		wasRunningFrames = false;
		currentFramebulk = 0;
		totalFramebulks = 0;
		totalFrames = 0;
		StrafeState = HLStrafe::CurrentState();
		PrevStrafeState = HLStrafe::CurrentState();
		PrevFraction = 1;
		PrevNormalz = 0;
		SharedRNGSeedPresent = false;
		SharedRNGSeed = 0;
		ButtonsPresent = false;
		exportFilename.clear();
		exportResult.Clear();
		libTASExportFile.close();
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
			offMaxEdicts = 0x3bc54;
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

		cvar_vars = reinterpret_cast<cvar_t**>(MemUtils::GetSymbolAddress(m_Handle, "cvar_vars"));
		if (cvar_vars)
			EngineDevMsg("[hw dll] Found cvar_vars at %p.\n", sv);
		else
			EngineDevWarning("[hw dll] Could not find cvar_vars.\n");

		movevars = MemUtils::GetSymbolAddress(m_Handle, "movevars");
		if (movevars) {
			EngineDevMsg("[hw dll] Found movevars at %p.\n", movevars);
			offZmax = 0x38;
		} else
			EngineDevWarning("[hw dll] Could not find movevars.\n");

		ORIG_hudGetViewAngles = reinterpret_cast<_hudGetViewAngles>(MemUtils::GetSymbolAddress(m_Handle, "hudGetViewAngles"));
		if (ORIG_hudGetViewAngles)
			EngineDevMsg("[hw dll] Found hudGetViewAngles at %p.\n", ORIG_hudGetViewAngles);
		else
			EngineDevWarning("[hw dll] Could not find hudGetViewAngles.\n");

		ORIG_SV_AddLinksToPM = reinterpret_cast<_SV_AddLinksToPM>(MemUtils::GetSymbolAddress(m_Handle, "SV_AddLinksToPM"));
		if (ORIG_SV_AddLinksToPM)
			EngineDevMsg("[hw dll] Found SV_AddLinksToPM at %p.\n", ORIG_SV_AddLinksToPM);
		else
			EngineDevWarning("[hw dll] Could not find SV_AddLinksToPM.\n");

		ORIG_SV_AddLinksToPM_ = reinterpret_cast<_SV_AddLinksToPM_>(MemUtils::GetSymbolAddress(m_Handle, "SV_AddLinksToPM_"));
		if (ORIG_SV_AddLinksToPM_)
			EngineDevMsg("[hw dll] Found SV_AddLinksToPM_ at %p.\n", ORIG_SV_AddLinksToPM_);
		else
			EngineDevWarning("[hw dll] Could not find SV_AddLinksToPM_.\n");

		ORIG_SV_WriteEntitiesToClient = reinterpret_cast<_SV_WriteEntitiesToClient>(MemUtils::GetSymbolAddress(m_Handle, "SV_WriteEntitiesToClient"));
		if (ORIG_SV_WriteEntitiesToClient)
			EngineDevMsg("[hw dll] Found SV_WriteEntitiesToClient at %p.\n", ORIG_SV_WriteEntitiesToClient);
		else
			EngineDevWarning("[hw dll] Could not find SV_WriteEntitiesToClient.\n");

		ORIG_SV_SetMoveVars = reinterpret_cast<_SV_SetMoveVars>(MemUtils::GetSymbolAddress(m_Handle, "SV_SetMoveVars"));
		if (ORIG_SV_SetMoveVars)
			EngineDevMsg("[hw dll] Found SV_SetMoveVars at %p.\n", ORIG_SV_SetMoveVars);
		else
			EngineDevWarning("[hw dll] Could not find SV_SetMoveVars.\n");

		ORIG_R_StudioCalcAttachments = reinterpret_cast<_R_StudioCalcAttachments>(MemUtils::GetSymbolAddress(m_Handle, "R_StudioCalcAttachments"));
		if (ORIG_R_StudioCalcAttachments)
			EngineDevMsg("[hw dll] Found R_StudioCalcAttachments at %p.\n", ORIG_R_StudioCalcAttachments);
		else
		{
			EngineDevWarning("[hw dll] Could not find R_StudioCalcAttachments.\n");
			EngineWarning("[hw dll] Weapon special effects will be misplaced when using bxt_viewmodel_fov.\n");
		}

		ORIG_VectorTransform = reinterpret_cast<_VectorTransform>(MemUtils::GetSymbolAddress(m_Handle, "VectorTransform"));
		if (ORIG_VectorTransform)
			EngineDevMsg("[hw dll] Found VectorTransform at %p.\n", ORIG_VectorTransform);
		else
		{
			EngineDevWarning("[hw dll] Could not find VectorTransform.\n");
			EngineWarning("[hw dll] Weapon special effects will be misplaced when using bxt_viewmodel_fov.\n");
		}

		ORIG_studioapi_GetCurrentEntity = reinterpret_cast<_studioapi_GetCurrentEntity>(MemUtils::GetSymbolAddress(m_Handle, "studioapi_GetCurrentEntity"));
		if (ORIG_studioapi_GetCurrentEntity)
			EngineDevMsg("[hw dll] Found studioapi_GetCurrentEntity at %p.\n", ORIG_studioapi_GetCurrentEntity);
		else
		{
			EngineDevWarning("[hw dll] Could not find studioapi_GetCurrentEntity.\n");
			EngineWarning("[hw dll] Weapon special effects will be misplaced when using bxt_viewmodel_fov.\n");
		}

		ORIG_CBaseUI__HideGameUI = reinterpret_cast<_CBaseUI__HideGameUI>(MemUtils::GetSymbolAddress(m_Handle, "_ZN7CBaseUI10HideGameUIEv"));
		if (ORIG_CBaseUI__HideGameUI)
			EngineDevMsg("[hw dll] Found CBaseUI__HideGameUI at %p.\n", ORIG_CBaseUI__HideGameUI);
		else
			EngineDevWarning("[hw dll] Could not find CBaseUI__HideGameUI.\n");

		if (!cls || !sv || !svs || !svmove || !ppmove || !host_client || !sv_player || !sv_areanodes || !cmd_text || !cmd_alias || !host_frametime || !cvar_vars || !movevars || !ORIG_hudGetViewAngles || !ORIG_SV_AddLinksToPM || !ORIG_SV_SetMoveVars)
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
		FIND(RandomLong)
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
		FIND(MD5Init)
		FIND(MD5Update)
		FIND(MD5Final)
		FIND(MD5Transform)
		FIND(MD5_Hash_File)
		FIND(MD5_Print)
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

		ORIG_R_DrawSkyBox = reinterpret_cast<_R_DrawSkyBox>(MemUtils::GetSymbolAddress(m_Handle, "R_DrawSkyBox"));
		if (ORIG_R_DrawSkyBox) {
			EngineDevMsg("[hw dll] Found R_DrawSkyBox at %p.\n", ORIG_R_DrawSkyBox);
		} else {
			EngineDevWarning("[hw dll] Could not find R_DrawSkyBox.\n");
			EngineWarning("bxt_skybox_remove is not available.\n");
		}

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

		ORIG_R_DrawViewModel = reinterpret_cast<_R_DrawViewModel>(MemUtils::GetSymbolAddress(m_Handle, "R_DrawViewModel"));
		if (ORIG_R_DrawViewModel)
			EngineDevMsg("[hw dll] Found R_DrawViewModel at %p.\n", ORIG_R_DrawViewModel);
		else
			EngineDevWarning("[hw dll] Could not find R_DrawViewModel.\n");

		ORIG_Mod_LeafPVS = reinterpret_cast<_Mod_LeafPVS>(MemUtils::GetSymbolAddress(m_Handle, "Mod_LeafPVS"));
		if (ORIG_Mod_LeafPVS) {
			EngineDevMsg("[hw dll] Found Mod_LeafPVS at %p.\n", ORIG_Mod_LeafPVS);
		} else {
			EngineDevWarning("[hw dll] Could not find Mod_LeafPVS.\n");
			EngineWarning("bxt_novis has no effect.\n");
		}

		ORIG_VGuiWrap_Paint = reinterpret_cast<_VGuiWrap_Paint>(MemUtils::GetSymbolAddress(m_Handle, "VGuiWrap_Paint"));
		if (ORIG_VGuiWrap_Paint) {
			EngineDevMsg("[hw dll] Found VGuiWrap_Paint at %p.\n", ORIG_VGuiWrap_Paint);
		} else {
			EngineDevWarning("[hw dll] Could not find VGuiWrap_Paint.\n");
			EngineWarning("bxt_disable_vgui has no effect.\n");
		}

		ORIG_DispatchDirectUserMsg = reinterpret_cast<_DispatchDirectUserMsg>(MemUtils::GetSymbolAddress(m_Handle, "DispatchDirectUserMsg"));
		if (ORIG_DispatchDirectUserMsg) {
			EngineDevMsg("[hw dll] Found DispatchDirectUserMsg at %p.\n", ORIG_DispatchDirectUserMsg);
		} else {
			EngineDevWarning("[hw dll] Could not find DispatchDirectUserMsg.\n");
			EngineWarning("Demo crash fix in Counter-Strike: Condition Zero Deleted Scenes is not available.\n");
		}

		ORIG_EmitWaterPolys = reinterpret_cast<_EmitWaterPolys>(MemUtils::GetSymbolAddress(m_Handle, "EmitWaterPolys"));
		if (ORIG_EmitWaterPolys) {
			EngineDevMsg("[hw dll] Found EmitWaterPolys at %p.\n", ORIG_EmitWaterPolys);
		} else {
			EngineDevWarning("[hw dll] Could not find EmitWaterPolys.\n");
			EngineWarning("bxt_water_remove has no effect.\n");
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

		ORIG_S_StartDynamicSound = reinterpret_cast<_S_StartDynamicSound>(MemUtils::GetSymbolAddress(m_Handle, "S_StartDynamicSound"));
		if (ORIG_S_StartDynamicSound)
			EngineDevMsg("[hw dll] Found S_StartDynamicSound at %p.\n", ORIG_S_StartDynamicSound);
		else
			EngineDevWarning("[hw dll] Could not find S_StartDynamicSound.\n");

		ORIG_VGuiWrap2_NotifyOfServerConnect = reinterpret_cast<_VGuiWrap2_NotifyOfServerConnect>(MemUtils::GetSymbolAddress(m_Handle, "VGuiWrap2_NotifyOfServerConnect"));
		if (ORIG_VGuiWrap2_NotifyOfServerConnect)
			EngineDevMsg("[hw dll] Found VGuiWrap2_NotifyOfServerConnect at %p.\n", ORIG_VGuiWrap2_NotifyOfServerConnect);
		else
			EngineDevWarning("[hw dll] Could not find VGuiWrap2_NotifyOfServerConnect.\n");
	}
	else
	{
		#define DEF_FUTURE(name) auto f##name = FindAsync(ORIG_##name, patterns::engine::name);
		DEF_FUTURE(Cvar_DirectSet)
		DEF_FUTURE(Cvar_FindVar)
		DEF_FUTURE(Cbuf_InsertText)
		DEF_FUTURE(Cbuf_AddText)
		DEF_FUTURE(Cmd_AddMallocCommand)
		//DEF_FUTURE(RandomFloat)
		DEF_FUTURE(RandomLong)
		DEF_FUTURE(SCR_BeginLoadingPlaque)
		DEF_FUTURE(PM_PlayerTrace)
		DEF_FUTURE(Host_FilterTime)
		DEF_FUTURE(V_FadeAlpha)
		DEF_FUTURE(R_DrawSkyBox)
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
		DEF_FUTURE(R_DrawViewModel)
		DEF_FUTURE(Mod_LeafPVS)
		DEF_FUTURE(CL_RecordHUDCommand)
		DEF_FUTURE(CL_Record_f)
		DEF_FUTURE(Key_Event)
		DEF_FUTURE(SV_AddLinksToPM_)
		DEF_FUTURE(SV_WriteEntitiesToClient)
		DEF_FUTURE(studioapi_GetCurrentEntity)
		DEF_FUTURE(VGuiWrap_Paint)
		DEF_FUTURE(DispatchDirectUserMsg)
		DEF_FUTURE(EmitWaterPolys)
		DEF_FUTURE(S_StartDynamicSound)
		DEF_FUTURE(VGuiWrap2_NotifyOfServerConnect)
		DEF_FUTURE(CBaseUI__HideGameUI)
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
				case 3: // HL-SteamPipe-8308
					cmd_text = reinterpret_cast<cmdbuf_t*>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cbuf_Execute) + 3));
					break;
				}
			});

		auto fCvar_RegisterVariable = FindAsync(
			ORIG_Cvar_RegisterVariable,
			patterns::engine::Cvar_RegisterVariable,
			[&](auto pattern) {
				switch (pattern - patterns::engine::Cvar_RegisterVariable.cbegin())
				{
				case 0: // HL-SteamPipe
					cvar_vars = reinterpret_cast<cvar_t**>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cvar_RegisterVariable) + 124));
					break;
				case 1: // HL-NGHL
					cvar_vars = reinterpret_cast<cvar_t**>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cvar_RegisterVariable) + 122));
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
				offMaxEdicts = 0x3bc5c;
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
				case 4: // HL-SteamPipe-8308.
					ORIG_Cbuf_InsertTextLines = reinterpret_cast<_Cbuf_InsertTextLines>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cmd_Exec_f) + 769)
						+ reinterpret_cast<uintptr_t>(ORIG_Cmd_Exec_f) + 773);
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

		auto fSV_SetMoveVars = FindAsync(
			ORIG_SV_SetMoveVars,
			patterns::engine::SV_SetMoveVars,
			[&](auto pattern) {
				movevars = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(ORIG_SV_SetMoveVars) + 18);
				offZmax = 0x38;
			}
		);

		auto fR_StudioCalcAttachments = FindAsync(
			ORIG_R_StudioCalcAttachments,
			patterns::engine::R_StudioCalcAttachments,
			[&](auto pattern) {
				ORIG_VectorTransform = reinterpret_cast<_VectorTransform>(
					*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_R_StudioCalcAttachments) + 106)
					+ reinterpret_cast<uintptr_t>(ORIG_R_StudioCalcAttachments) + 110);
			});

		auto fR_StudioSetupBones = FindAsync(
			ORIG_R_StudioSetupBones,
			patterns::engine::R_StudioSetupBones,
			[&](auto pattern) {
				switch (pattern - patterns::engine::R_StudioSetupBones.cbegin())
				{
				case 0: // SteamPipe.
					pstudiohdr = *reinterpret_cast<studiohdr_t***>(reinterpret_cast<uintptr_t>(ORIG_R_StudioSetupBones) + 13);
					break;
				case 1: // 4554.
					pstudiohdr = *reinterpret_cast<studiohdr_t***>(reinterpret_cast<uintptr_t>(ORIG_R_StudioSetupBones) + 7);
					break;
				default:
					assert(false);
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
			auto pattern = fCvar_RegisterVariable.get();
			if (ORIG_Cvar_RegisterVariable) {
				EngineDevMsg("[hw dll] Found Cvar_RegisterVariable at %p (using the %s pattern).\n", ORIG_Cvar_RegisterVariable, pattern->name());
				EngineDevMsg("[hw dll] Found cvar_vars at %p.\n", cvar_vars);
			} else {
				EngineDevWarning("[hw dll] Could not find Cvar_RegisterVariable.\n");
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

		{
			auto pattern = fSV_SetMoveVars.get();
			if (ORIG_SV_SetMoveVars) {
				EngineDevMsg("[hw dll] Found SV_SetMoveVars at %p (using the %s pattern).\n", ORIG_SV_SetMoveVars, pattern->name());
				EngineDevMsg("[hw dll] Found movevars at %p.\n", movevars);
			} else {
				EngineDevWarning("[hw dll] Could not find SV_SetMoveVars.\n");
			}
		}

		{
			auto pattern = fR_StudioCalcAttachments.get();
			if (ORIG_R_StudioCalcAttachments) {
				EngineDevMsg("[hw dll] Found R_StudioCalcAttachments at %p (using the %s pattern).\n", ORIG_R_StudioCalcAttachments, pattern->name());
				EngineDevMsg("[hw dll] Found VectorTransform at %p.\n", ORIG_VectorTransform);
			} else {
				EngineDevWarning("[hw dll] Could not find R_StudioCalcAttachments.\n");
				EngineWarning("[hw dll] Special effects of weapons will be misplaced when bxt_viewmodel_fov is used.\n");
			}
		}

		{
			auto pattern = fR_StudioSetupBones.get();
			if (ORIG_R_StudioSetupBones) {
				EngineDevMsg("[hw dll] Found R_StudioSetupBones at %p (using the %s pattern).\n", ORIG_R_StudioSetupBones, pattern->name());
				EngineDevMsg("[hw dll] Found pstudiohdr at %p.\n", pstudiohdr);
			} else {
				EngineDevWarning("[hw dll] Could not find R_StudioSetupBones.\n");
				EngineWarning("[hw dll] Disabling weapon viewmodel idle or equip sequences is not available.\n");
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
		GET_FUTURE(Cvar_DirectSet)
		GET_FUTURE(Cvar_FindVar)
		GET_FUTURE(Cbuf_InsertText)
		GET_FUTURE(Cbuf_AddText)
		GET_FUTURE(Cmd_AddMallocCommand)
		//GET_FUTURE(RandomFloat)
		GET_FUTURE(RandomLong)
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
		GET_FUTURE(R_DrawSkyBox);
		GET_FUTURE(SV_Frame);
		GET_FUTURE(VGuiWrap2_ConDPrintf);
		GET_FUTURE(VGuiWrap2_ConPrintf);
		GET_FUTURE(SCR_UpdateScreen);
		GET_FUTURE(R_DrawSequentialPoly);
		GET_FUTURE(R_Clear);
		GET_FUTURE(R_DrawViewModel);
		GET_FUTURE(Mod_LeafPVS);
		GET_FUTURE(PF_GetPhysicsKeyValue);
		GET_FUTURE(SV_AddLinksToPM_);
		GET_FUTURE(SV_WriteEntitiesToClient);
		GET_FUTURE(VGuiWrap_Paint);
		GET_FUTURE(DispatchDirectUserMsg);
		GET_FUTURE(studioapi_GetCurrentEntity);
		GET_FUTURE(EmitWaterPolys);
		GET_FUTURE(S_StartDynamicSound);
		GET_FUTURE(VGuiWrap2_NotifyOfServerConnect);
		GET_FUTURE(CBaseUI__HideGameUI);

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

void HwDLL::SetCVarValue(CVarWrapper& cvar, const char *value)
{
	if (!ORIG_Cvar_FindVar || !ORIG_Cvar_DirectSet)
		return;

	if (ORIG_Cvar_FindVar(cvar.GetPointer()->name)) {
		ORIG_Cvar_DirectSet(cvar.GetPointer(), value);
		cvar.MarkAsStale();
	}
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

		// Disable the input editor.
		hw.SetTASEditorMode(TASEditorMode::DISABLED);

		hw.runningFrames = false;
		hw.currentFramebulk = 0;
		hw.currentRepeat = 0;
		hw.movementFrameCounter = 0;
		hw.StrafeState = HLStrafe::CurrentState();
		hw.PrevStrafeState = HLStrafe::CurrentState();
		hw.PrevFraction = 1;
		hw.PrevNormalz = 0;
		hw.ButtonsPresent = false;
		hw.demoName.clear();
		hw.saveName.clear();
		hw.frametime0ms.clear();
		hw.hlstrafe_version = HLStrafe::MAX_SUPPORTED_VERSION;
		hw.SharedRNGSeedPresent = false;
		hw.SetNonSharedRNGSeed = false;
		hw.thisFrameIs0ms = false;
		hw.hltas_filename = fileName;
		hw.clearedImpulsesForTheFirstTime = false;

		simulation_ipc::maybe_lock_mutex();
		auto err = hw.input.Open(fileName);
		simulation_ipc::maybe_unlock_mutex();

		if (err.Code != HLTAS::ErrorCode::OK) {
			const auto& message = hw.input.GetErrorMessage();
			if (message.empty()) {
				hw.ORIG_Con_Printf("Error loading the script file on line %u: %s\n", err.LineNumber, HLTAS::GetErrorMessage(err).c_str());
			} else {
				hw.ORIG_Con_Printf("Error loading the script: %s\n", message.c_str());
			}
			return;
		}

		if (!hw.exportFilename.empty())
			hw.exportResult.ClearProperties();

		bool saw_hlstrafe_version = false;
		std::string load_command;
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
			else if (prop.first == "hlstrafe_version") {
				hw.hlstrafe_version = std::strtoul(prop.second.c_str(), nullptr, 10);

				saw_hlstrafe_version = true;

				if (hw.hlstrafe_version > HLStrafe::MAX_SUPPORTED_VERSION) {
					hw.ORIG_Con_Printf("Error loading the script: hlstrafe_version %u is too high (maximum supported version: %u)\n", hw.hlstrafe_version, HLStrafe::MAX_SUPPORTED_VERSION);
					return;
				}
			} else if (prop.first == "load_command")
				load_command = prop.second;

			if (!hw.exportFilename.empty())
				hw.exportResult.SetProperty(prop.first, prop.second);
		}

		if (saw_hlstrafe_version) {
			if (hw.hlstrafe_version < HLStrafe::MAX_SUPPORTED_VERSION)
				hw.ORIG_Con_Printf("The script's hlstrafe_version is %u, but the latest version is %u. If this is an old script, keep it as is. For new scripts, please add a \"hlstrafe_version %u\" property to get the most accurate TAS prediction.\n", hw.hlstrafe_version, HLStrafe::MAX_SUPPORTED_VERSION, HLStrafe::MAX_SUPPORTED_VERSION);
		} else {
			hw.hlstrafe_version = 1;
			hw.ORIG_Con_Printf("No hlstrafe_version property found in the script. If this is an old script, keep it as is, or add a \"hlstrafe_version 1\" property explicitly. For new scripts, please add a \"hlstrafe_version %u\" property to get the most accurate TAS prediction.\n", HLStrafe::MAX_SUPPORTED_VERSION);
		}

		if (!hw.input.GetFrames().empty()) {
			hw.runningFrames = true;
			hw.wasRunningFrames = false; // So that ResetButtons() and others run in InsertCommands().
			hw.totalFramebulks = hw.input.GetFrames().size();
			HLTAS::Frame f;
			if (hw.GetNextMovementFrame(f)) {
				std::ostringstream ss;
				ss << "host_framerate " << f.Frametime.c_str() << "\n";
				hw.ORIG_Cbuf_InsertText(ss.str().c_str());
			}

			hw.totalFrames = 0;
			for (const auto& frame_bulk : hw.input.GetFrames()) {
				if (!frame_bulk.IsMovement())
					continue;

				hw.totalFrames += frame_bulk.GetRepeats();
			}

			auto norefresh_until_frames = CVars::bxt_tas_norefresh_until_last_frames.GetInt();
			if (norefresh_until_frames > 0 && hw.totalFrames > static_cast<size_t>(norefresh_until_frames))
				hw.ORIG_Cbuf_InsertText("_bxt_norefresh 1\n");

			// Reset the frametime remainder automatically upon starting a script.
			// Fairly certain that's what you want in 100% of cases.
			if (hw.frametime_remainder)
				*hw.frametime_remainder = 0;

			// Disable the freecam. A case could be made for it being useful, however with the
			// current implementation it just uses the viewangles from the strafing and so isn't
			// really useful.
			hw.SetFreeCam(false);

			// It will be enabled by bxt_tas_write_log if needed.
			hw.SetTASLogging(false);
		}

		if (!load_command.empty()) {
			load_command += '\n';
			hw.ORIG_Cbuf_InsertText(load_command.c_str());
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
		hw.exportResult.Clear();

		if (hw.exportFilename.empty())
			return;

		if (hw.runningFrames)
			for (auto prop : hw.input.GetProperties())
				hw.exportResult.SetProperty(prop.first, prop.second);

		hw.ORIG_Con_Printf("Started exporting .hltas frames.\n");
	}
};

struct HwDLL::Cmd_BXT_TAS_ExportLibTASInput
{
	USAGE("Usage: bxt_tas_export_libtas_input <filename>\n Starts exporting the currently running HLTAS script into libTAS input, outputting it into a file with the given filename. You will need to open the file, copy its contents and paste them into the libTAS input editor.\n");

	static void handler(const char *fileName)
	{
		auto &hw = HwDLL::GetInstance();
		hw.libTASExportFile.open(fileName);

		if (!hw.libTASExportFile.is_open()) {
			hw.ORIG_Con_Printf("Error opening %s\n", fileName);
			return;
		}

		hw.ORIG_Con_Printf("Started exporting libTAS input.\n");

		// Seems to be the consistent number of blank frames required after "map something<Return>".
		for (size_t i = 0; i < 13; ++i)
			hw.libTASExportFile << "|K|\n";
	}
};

struct HwDLL::Cmd_BXT_TAS_Split
{
	USAGE("Usage: bxt_tas_split <filename>\n Splits the currently running .hltas script by performing a saveload and putting the remaining frames into the output .hltas script.\n");

	static void handler(const char *fileName)
	{
		auto &hw = HwDLL::GetInstance();

		if (!hw.runningFrames) {
			hw.ORIG_Con_Printf("No TAS script is currently running.\n");
			return;
		}

		std::ostringstream oss;
		oss << "bxt_autopause 1;save \"" << fileName << "\";load \"" << fileName << "\"\n";
		hw.ORIG_Cbuf_InsertText(oss.str().c_str());

		hw.splitFilename = std::string(fileName) + ".hltas";
		hw.splitResult.Clear();

		for (auto prop : hw.input.GetProperties())
			hw.splitResult.SetProperty(prop.first, prop.second);

		// Add a couple of state frames.
		HLTAS::Frame frame;
		frame.SetLgagstMinSpeed(hw.StrafeState.LgagstMinSpeed);
		hw.splitResult.InsertFrame(hw.splitResult.GetFrames().size(), frame);

		if (hw.ButtonsPresent) {
			frame = HLTAS::Frame();
			frame.SetButtons(hw.Buttons);
			hw.splitResult.InsertFrame(hw.splitResult.GetFrames().size(), frame);
		}

		if (hw.SharedRNGSeedPresent) {
			frame = HLTAS::Frame();
			frame.SetSeed(hw.SharedRNGSeed);
			hw.splitResult.InsertFrame(hw.splitResult.GetFrames().size(), frame);
		}

		frame = HLTAS::Frame();
		frame.SetAlgorithm(hw.StrafeState.Algorithm);
		hw.splitResult.InsertFrame(hw.splitResult.GetFrames().size(), frame);

		frame = HLTAS::Frame();
		frame.SetAlgorithmParameters(hw.StrafeState.Parameters);
		hw.splitResult.InsertFrame(hw.splitResult.GetFrames().size(), frame);
	}
};

struct HwDLL::Cmd_BXT_TAS_New
{
	USAGE("Usage: bxt_tas_new <filename> <starting command> <FPS>\n Creates a new TAS script ready to use with the TAS editor.\n\n"
	      " filename is the filename of the script that will be created. The .hltas extension will be added automatically.\n\n"
	      " starting command is the command to launch the map or load the save which the TAS will start from, for example \"map c1a0\" or \"load tas-start\".\n\n"
	      " FPS is the initial FPS for the TAS, for example 100 or 250 or 1000.\n\n"
	      " Example:\n  bxt_tas_new full-game \"map c1a0\" 100\n");

	static void handler(const char *filename, const char *command, int fps)
	{
		auto &hw = HwDLL::GetInstance();

		std::string frametime;
		switch (fps) {
			case 1000:
				frametime = "0.001";
				break;
			case 500:
				frametime = "0.002";
				break;
			case 250:
				frametime = "0.004";
				break;
			case 100:
				frametime = "0.010000001";
				break;

			default:
				hw.ORIG_Con_Printf("You specified FPS = %d, however only FPS = 1000, 500, 250 or 100 are currently supported. If you need another FPS value, use one of the supported FPS values, and then change the frametime manually in the script", fps);
				if (fps > 0)
					hw.ORIG_Con_Printf(" (you will want something around %f)", 1.f / fps);
				hw.ORIG_Con_Printf(".\n");
				return;
		}

		const auto bhopcap = CVars::bxt_bhopcap.GetBool();
		// Assumption: FPS below 1000 is a hard limit, which means we definitely can't set it higher than 1000.
		const auto zero_ms_ducktap = (fps == 1000);

		hw.newTASFilename = std::string(filename) + ".hltas";
		hw.newTASResult.Clear();

		std::ostringstream oss;
		oss << HLStrafe::MAX_SUPPORTED_VERSION;
		hw.newTASResult.SetProperty("hlstrafe_version", oss.str());

		std::string cmd(command);
		hw.newTASResult.SetProperty("load_command", cmd);

		cmd += "\n";
		hw.ORIG_Cbuf_InsertText(cmd.c_str());

		if (zero_ms_ducktap)
			hw.newTASResult.SetProperty("frametime0ms", "0.0000000001");

		HLTAS::Frame frame;
		frame.SetAlgorithm(HLTAS::StrafingAlgorithm::VECTORIAL);
		frame.Comments = " Enable vectorial strafing. This makes the camera movement very smooth.";
		hw.newTASResult.PushFrame(frame);

		frame = HLTAS::Frame();
		HLTAS::AlgorithmParameters parameters;
		parameters.Type = HLTAS::ConstraintsType::VELOCITY_LOCK;
		parameters.Parameters.VelocityLock.Constraints = 0;
		frame.SetAlgorithmParameters(parameters);
		frame.Comments = " Vectorial strafing will make the player look towards where he's moving.";
		hw.newTASResult.PushFrame(frame);

		// The frame bulk for waiting for the load.
		frame = HLTAS::Frame();
		frame.Frametime = frametime;
		frame.Comments = " Wait for the game to fully load.\n On the first frame, ";

		// Automatically check and put in some of the more common custom cvar settings.
		if (!bhopcap)
			frame.Commands += "bxt_bhopcap 0;bxt_bhopcap_prediction 0";

		if (CVars::sv_maxspeed.GetFloat() != 320) // HLKZ uses 300.
		{
			// TODO: this check would malfunction for mods with custom sv_maxspeed when TASing with
			// sv_maxspeed = 320. Is there any way to query the default sv_maxspeed instead?

			if (!frame.Commands.empty())
				frame.Commands += ';';
			frame.Commands += "sv_maxspeed " + std::to_string(CVars::sv_maxspeed.GetFloat());
		}

		if (!frame.Commands.empty()) {
			frame.Commands += ';';
			frame.Comments += "set up custom console variable values, and ";
		}

		frame.Commands += "bxt_timer_reset";
		frame.Comments += "reset the timer.";

		hw.newTASResult.PushFrame(frame);

		// The actual first frame bulk with some reasonable defaults.
		frame = HLTAS::Frame();
		frame.Frametime = frametime;
		frame.SetRepeats(static_cast<unsigned>(1 / std::atof(frametime.c_str())));
		frame.Strafe = true;
		frame.SetDir(HLTAS::StrafeDir::YAW);
		frame.SetType(HLTAS::StrafeType::MAXACCEL);
		frame.Lgagst = true;

		frame.Comments = " The default settings are: \n"
		                 " - s03 (speed increasing strafing),\n"
		                 " - lgagst (leave ground at optimal speed),\n";

		if (bhopcap) {
			frame.Ducktap = true;

			if (zero_ms_ducktap) {
				frame.SetDucktap0ms(true);
				frame.Comments += " - 0ms ducktap without ground friction (since the bunnyhop cap was detected and the FPS is 1000),\n";
			} else {
				frame.Comments += " - regular ducktap (since the bunnyhop cap was detected and the FPS is below 1000),\n";
			}
		} else {
			frame.Autojump = true;
			frame.Comments += " - autojump,\n";
		}
		frame.Dbc = true;
		frame.Comments += " - automatic duck before collision.";
		frame.Commands = "stop;bxt_timer_stop;pause;bxt_tas_editor 1";
		hw.newTASResult.PushFrame(frame);

		// A blank frame bulk in the end since currently it's dropped in the TAS editor.
		// TODO: remove when TAS editor is better.
		frame.Comments.clear();
		frame.Commands.clear();
		frame.SetRepeats(1);
		hw.newTASResult.PushFrame(frame);
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

struct HwDLL::Cmd_BXT_CH_Set_Angles
{
	USAGE("Usage: bxt_ch_set_angles <pitch> <yaw> [roll]\n");

	static void handler(float x, float y)
	{
		auto &hw = HwDLL::GetInstance();
		float vec[3];
		vec[0] = x;
		vec[1] = y;
		vec[2] = 0.0f;
		hw.SetViewangles(vec);
	}

	static void handler(float x, float y, float z)
	{
		auto &hw = HwDLL::GetInstance();
		float vec[3];
		vec[0] = x;
		vec[1] = y;
		vec[2] = z;
		hw.SetViewangles(vec);
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
		hw.free_cam_active = false;
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
		hw.free_cam_active = false;
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
		hw.free_cam_active = false;
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

struct HwDLL::Cmd_BXT_Triggers_Place_Down
{
	NO_USAGE();

	static void handler()
	{
		auto trace = HwDLL::GetInstance().CameraTrace();

		CustomTriggers::placing = true;
		Vector start = trace.EndPos;
		CustomTriggers::place_start = start;
		CustomTriggers::triggers.emplace_back(start, start);
	}

	static void handler(const char*)
	{
		handler();
	}
};

struct HwDLL::Cmd_BXT_Triggers_Place_Up
{
	NO_USAGE();

	static void handler()
	{
		CustomTriggers::placing = false;
	}

	static void handler(const char *)
	{
		handler();
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

struct HwDLL::Cmd_BXT_Interprocess_Stop
{
	NO_USAGE();

	static void handler()
	{
		Interprocess::Shutdown();
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
	USAGE("Usage: bxt_taslog <0|1>\n Enables or disables TAS logging into the file at bxt_taslog_filename.\n");

	static void handler(int enabled)
	{
		auto &hw = HwDLL::GetInstance();
		if (!hw.ORIG_SV_Frame) {
			hw.ORIG_Con_Printf("TAS logging is unavailable.\n");
			return;
		}

		hw.SetTASLogging(enabled);
	}
};

struct HwDLL::Cmd_BXT_Set_Frametime_Remainder
{
	NO_USAGE();

	static void handler(double value)
	{
		if (HwDLL::GetInstance().frametime_remainder)
			*HwDLL::GetInstance().frametime_remainder = value;
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

struct HwDLL::Cmd_BXT_TAS_Editor
{
	USAGE("Usage: bxt_tas_editor <0|1>\n Enables the TAS editor.\n");

	static void handler(int mode)
	{
		TASEditorMode tas_editor_mode;
		if (mode == 0)
			tas_editor_mode = TASEditorMode::DISABLED;
		else if (mode == 1)
			tas_editor_mode = TASEditorMode::EDIT;
		else
			return;

		HwDLL::GetInstance().SetTASEditorMode(tas_editor_mode);
	}
};

struct HwDLL::Cmd_Plus_BXT_TAS_Editor_Look_Around
{
	USAGE("Usage: +bxt_tas_editor_look_around\n Allows to look around while in the TAS editor.\n");

	static void handler()
	{
		auto& hw = HwDLL::GetInstance();

		if (hw.tas_editor_mode == TASEditorMode::EDIT) {
			ClientDLL::GetInstance().SetMouseState(true);
			SDL::GetInstance().SetRelativeMouseMode(true);
		}
	}

	static void handler(int)
	{
		handler();
	}
};

struct HwDLL::Cmd_Minus_BXT_TAS_Editor_Look_Around
{
	USAGE("Usage: -bxt_tas_editor_look_around\n Disables looking around while in the TAS editor.\n");

	static void handler()
	{
		auto& hw = HwDLL::GetInstance();

		if (hw.tas_editor_mode == TASEditorMode::EDIT) {
			ClientDLL::GetInstance().SetMouseState(false);
			SDL::GetInstance().SetRelativeMouseMode(false);
		}
	}

	static void handler(int)
	{
		handler();
	}
};

struct HwDLL::Cmd_BXT_TAS_Editor_Save
{
	USAGE("Usage: bxt_tas_editor_save\n Saves the currently edited input into the script.\n");

	static void handler()
	{
		auto& hw = HwDLL::GetInstance();
		hw.SaveEditedInput();
		hw.SetFreeCam(false);
	}
};

struct HwDLL::Cmd_BXT_TAS_Editor_Delete_Last_Point
{
	USAGE("Usage: bxt_tas_editor_delete_last_point\n Deletes the last point in the edited input.\n");

	static void handler()
	{
		auto& hw = HwDLL::GetInstance();
		auto& frame_bulks = hw.tas_editor_input.frame_bulks;

		if (hw.tas_editor_mode == TASEditorMode::EDIT) {
			if (frame_bulks.size() > 0) {
				hw.tas_editor_input.mark_as_stale(frame_bulks.size() - 1);
				frame_bulks.erase(frame_bulks.end() - 1);
			}
		}
	}
};

struct HwDLL::Cmd_BXT_TAS_Editor_Delete_Point
{
	USAGE("Usage: bxt_tas_editor_delete_point\n Deletes the currently selected point in edit mode.\n");

	static void handler()
	{
		HwDLL::GetInstance().tas_editor_delete_point = true;
	}
};

struct HwDLL::Cmd_BXT_TAS_Editor_Insert_Point
{
	USAGE("Usage: bxt_tas_editor_insert_point\n Inserts an extra point at the mouse cursor position.\n");

	static void handler()
	{
		HwDLL::GetInstance().tas_editor_insert_point = true;
	}
};

struct HwDLL::Cmd_BXT_TAS_Editor_Toggle
{
	USAGE("Usage: bxt_tas_editor_toggle <what>\n Toggles a function on the currently selected point. You can toggle:\n"
	      " - s03 - speed increasing strafing,\n"
	      " - s13 - quick turn strafing,\n"
	      " - s22 - slow down strafing,\n"
	      " - lgagst - makes autojump and ducktap trigger at optimal speed,\n"
	      " - autojump,\n"
	      " - ducktap,\n"
	      " - jumpbug,\n"
	      " - dbc - duck before collision,\n"
	      " - dbcceilings - duck before collision (including ceilings),\n"
	      " - dbg - duck before ground,\n"
	      " - dwj - duck when jump (useful for the long-jump module),\n"
	      " - forward - +forward,\n"
	      " - left - +moveleft,\n"
	      " - right - +moveright,\n"
	      " - back - +back,\n"
	      " - up - +moveup,\n"
	      " - down - +movedown,\n"
	      " - jump - +jump,\n"
	      " - duck - +duck,\n"
	      " - use - +use,\n"
	      " - attack1 - +attack1,\n"
	      " - attack2 - +attack2,\n"
	      " - reload - +reload.\n"
	);

	static void handler(const char *what)
	{
		if (!strcmp(what, "s03")) {
			HwDLL::GetInstance().tas_editor_toggle_s03 = true;
		} else if (!strcmp(what, "s13")) {
			HwDLL::GetInstance().tas_editor_toggle_s13 = true;
		} else if (!strcmp(what, "s22")) {
			HwDLL::GetInstance().tas_editor_toggle_s22 = true;
		} else if (!strcmp(what, "lgagst")) {
			HwDLL::GetInstance().tas_editor_toggle_lgagst = true;
		} else if (!strcmp(what, "autojump")) {
			HwDLL::GetInstance().tas_editor_toggle_autojump = true;
		} else if (!strcmp(what, "ducktap")) {
			HwDLL::GetInstance().tas_editor_toggle_ducktap = true;
		} else if (!strcmp(what, "jumpbug")) {
			HwDLL::GetInstance().tas_editor_toggle_jumpbug = true;
		} else if (!strcmp(what, "dbc")) {
			HwDLL::GetInstance().tas_editor_toggle_dbc = true;
		} else if (!strcmp(what, "dbcceilings")) {
			HwDLL::GetInstance().tas_editor_toggle_dbc_ceilings = true;
		} else if (!strcmp(what, "dbg")) {
			HwDLL::GetInstance().tas_editor_toggle_dbg = true;
		} else if (!strcmp(what, "dwj")) {
			HwDLL::GetInstance().tas_editor_toggle_dwj = true;
		} else if (!strcmp(what, "forward")) {
			HwDLL::GetInstance().tas_editor_toggle_forward = true;
		} else if (!strcmp(what, "left")) {
			HwDLL::GetInstance().tas_editor_toggle_left = true;
		} else if (!strcmp(what, "right")) {
			HwDLL::GetInstance().tas_editor_toggle_right = true;
		} else if (!strcmp(what, "back")) {
			HwDLL::GetInstance().tas_editor_toggle_back = true;
		} else if (!strcmp(what, "up")) {
			HwDLL::GetInstance().tas_editor_toggle_up = true;
		} else if (!strcmp(what, "down")) {
			HwDLL::GetInstance().tas_editor_toggle_down = true;
		} else if (!strcmp(what, "jump")) {
			HwDLL::GetInstance().tas_editor_toggle_jump = true;
		} else if (!strcmp(what, "duck")) {
			HwDLL::GetInstance().tas_editor_toggle_duck = true;
		} else if (!strcmp(what, "use")) {
			HwDLL::GetInstance().tas_editor_toggle_use = true;
		} else if (!strcmp(what, "attack1")) {
			HwDLL::GetInstance().tas_editor_toggle_attack1 = true;
		} else if (!strcmp(what, "attack2")) {
			HwDLL::GetInstance().tas_editor_toggle_attack2 = true;
		} else if (!strcmp(what, "reload")) {
			HwDLL::GetInstance().tas_editor_toggle_reload = true;
		}
	}
};

struct HwDLL::Cmd_BXT_FreeCam
{
	USAGE("Usage: bxt_freecam <0|1>\n Enables the freecam mode. Most useful when paused with bxt_unlock_camera_during_pause 1.\n");

	static void handler(int enabled)
	{
		HwDLL::GetInstance().SetFreeCam(enabled);
	}
};

struct HwDLL::Cmd_BXT_Print_Entities
{
	NO_USAGE();

	static void handler()
	{
		const auto& hw = HwDLL::GetInstance();
		const auto& sv = ServerDLL::GetInstance();

		std::ostringstream out;

		edict_t *edicts;
		const int numEdicts = hw.GetEdicts(&edicts);
		for (int e = 0; e < numEdicts; ++e) {
			const edict_t *ent = edicts + e;
			if (!hw.IsValidEdict(ent))
				continue;

			const char *classname = sv.GetString(ent->v.classname);
			out << e << ": " << classname;

			if (ent->v.targetname != 0) {
				const char *targetname = sv.GetString(ent->v.targetname);
				out << " - " << targetname;
			}

			out << '\n';
		}

		auto str = out.str();
		hw.ORIG_Con_Printf("%s", str.c_str());
	}
};

struct HwDLL::Cmd_BXT_TAS_Editor_Set_Run_Point_And_Save
{
	USAGE("Usage: bxt_tas_editor_set_run_point_and_save\n Makes the script execute up to the selected point and resume editing from it.\n");

	static void handler()
	{
		HwDLL::GetInstance().tas_editor_set_run_point_and_save = true;
	}
};

struct HwDLL::Cmd_BXT_TAS_Editor_Set_Yaw
{
	USAGE("Usage: bxt_tas_editor_set_yaw <yaw>\n Sets the yaw angle on the currently selected point.\n");

	static void handler(float value)
	{
		auto& hw = HwDLL::GetInstance();
		hw.tas_editor_set_yaw = true;
		hw.tas_editor_set_yaw_yaw = value;
	}
};

struct HwDLL::Cmd_BXT_TAS_Editor_Set_Pitch
{
	USAGE("Usage: bxt_tas_editor_set_pitch <pitch>\n Sets the pitch angle on the currently selected point.\n");

	static void handler(float value)
	{
		auto& hw = HwDLL::GetInstance();
		hw.tas_editor_set_pitch = true;
		hw.tas_editor_set_pitch_pitch = value;
	}
};

struct HwDLL::Cmd_BXT_TAS_Editor_Set_Repeats
{
	USAGE("Usage: bxt_tas_editor_set_repeats <repeats>\n Sets the repeats on the currently selected point.\n");

	static void handler(int value)
	{
		auto& hw = HwDLL::GetInstance();

		if (value <= 0) {
			hw.ORIG_Con_Printf("Repeats must be > 0.\n");
			return;
		}

		hw.tas_editor_set_repeats = true;
		hw.tas_editor_set_repeats_repeats = value;
	}
};

struct HwDLL::Cmd_BXT_TAS_Editor_Set_Commands
{
	USAGE("Usage: bxt_tas_editor_set_commands <commands>\n Sets the commands on the currently selected point.\n");

	static void handler(const char* value)
	{
		auto& hw = HwDLL::GetInstance();

		hw.tas_editor_set_commands = true;
		hw.tas_editor_set_commands_commands = value;
	}
};

struct HwDLL::Cmd_BXT_TAS_Editor_Unset_Yaw
{
	USAGE("Usage: bxt_tas_editor_unset_yaw <yaw>\n Unsets the yaw angle on the currently selected point.\n");

	static void handler()
	{
		auto& hw = HwDLL::GetInstance();
		hw.tas_editor_unset_yaw = true;
	}
};

struct HwDLL::Cmd_BXT_TAS_Editor_Unset_Pitch
{
	USAGE("Usage: bxt_tas_editor_unset_pitch <pitch>\n Unsets the pitch angle on the currently selected point.\n");

	static void handler()
	{
		auto& hw = HwDLL::GetInstance();
		hw.tas_editor_unset_pitch = true;
	}
};

struct HwDLL::Cmd_BXT_TAS_Become_Simulator_Client
{
	NO_USAGE()

	static void handler()
	{
		auto err = simulation_ipc::initialize_client();
		if (!err.empty())
			HwDLL::GetInstance().ORIG_Con_Printf("Couldn't become simulator client: %s\n", err.c_str());
	}
};

struct HwDLL::Cmd_BXT_TAS_Server_Send_Command
{
	USAGE("Usage: _bxt_tas_server_send_command <command>\n Sends a console command to the client.\n");

	static void handler(const char *command)
	{
		simulation_ipc::send_command_to_client(std::string(command) + '\n');
	}
};

void HwDLL::SetTASEditorMode(TASEditorMode mode)
{
	auto& cl = ClientDLL::GetInstance();

	// Don't enable unless we're in-game, otherwise the game can crash.
	int *state = reinterpret_cast<int*>(cls);
	if (state && *state != 5 && mode != TASEditorMode::DISABLED) {
		ORIG_Con_Printf("You must be in-game to enable the TAS editor.\n");
		return;
	}

	if (tas_editor_mode != TASEditorMode::DISABLED && mode == TASEditorMode::DISABLED) {
		// Save the script into a backup file in case the editor was disabled by accident.
		auto err = tas_editor_input.save(hltas_filename + ".backup");
		if (err.Code != HLTAS::ErrorCode::OK)
			ORIG_Con_Printf("Error saving a backup script: %s\n", HLTAS::GetErrorMessage(err).c_str());
	}

	if (mode != TASEditorMode::DISABLED) {
		SetFreeCam(true);

		auto err = simulation_ipc::initialize_server_if_needed();
		if (!err.empty() && ORIG_Con_Printf)
			ORIG_Con_Printf("Couldn't initialize simulator server: %s\n", err.c_str());
	}

	if (tas_editor_mode == TASEditorMode::DISABLED && mode != TASEditorMode::DISABLED) {
		tas_editor_input = EditedInput();
		tas_editor_input.initialize();

		// If invoked while running a script, put all frame bulks up until the last one for editing.
		if (runningFrames) {
			if (currentRepeat == 0) {
				// The TAS editor was enabled on a 1-long frame bulk, and the execution
				// has already jumped to the next one. So, move the frame bulk back by one.
				if (currentFramebulk > 0) // Sanity check.
					currentFramebulk--;
			}

			auto limit = input.GetFrames().size() - 1;
			for (size_t i = currentFramebulk; i < limit; ++i) {
				tas_editor_input.frame_bulks.push_back(input.GetFrames()[currentFramebulk]);
				input.RemoveFrame(currentFramebulk);
			}

			runningFrames = false;
			ORIG_Cbuf_InsertText("host_framerate 0;_bxt_norefresh 0;_bxt_min_frametime 0;bxt_taslog 0\n");

			assert(movementFrameCounter >= 1);
			tas_editor_input.first_frame_counter_value = movementFrameCounter - 1;

			tas_editor_input.run_script_in_second_game();
		} else {
			// If invoked outside of a script, make sure the hlstrafe version is latest.
			hlstrafe_version = HLStrafe::MAX_SUPPORTED_VERSION;
		}
	}

	if (mode == TASEditorMode::EDIT) {
		cl.SetMouseState(false);
		SDL::GetInstance().SetRelativeMouseMode(false);
	} else {
		cl.SetMouseState(true);
		SDL::GetInstance().SetRelativeMouseMode(true);
	}

	tas_editor_mode = mode;
}

void HwDLL::SetFreeCam(bool enabled)
{
	auto& cl = ClientDLL::GetInstance();

	if (enabled) {
		free_cam_active = true;

		auto org = cl.last_vieworg;
		auto ang = cl.last_viewangles;

		isOverridingCamera = true;
		isOffsettingCamera = false;
		cameraOverrideOrigin[0] = org.x;
		cameraOverrideOrigin[1] = org.y;
		cameraOverrideOrigin[2] = org.z;
		cameraOverrideAngles[0] = ang.x;
		cameraOverrideAngles[1] = ang.y;
		cameraOverrideAngles[2] = ang.z;
	} else {
		free_cam_active = false;
		isOverridingCamera = false;
	}
}

void HwDLL::SaveEditedInput()
{
	if (tas_editor_mode == TASEditorMode::DISABLED)
		return;

	auto err = tas_editor_input.save(hltas_filename);
	if (err.Code == HLTAS::ErrorCode::OK)
		ORIG_Con_Printf("Saved the script: %s\n", hltas_filename.c_str());
	else
		ORIG_Con_Printf("Error saving the script: %s\n", HLTAS::GetErrorMessage(err).c_str());

	SetTASEditorMode(TASEditorMode::DISABLED);
}

void HwDLL::SetTASLogging(bool enabled)
{
	if (enabled == tasLogging)
		return;

	if (tasLogging) {
		logWriter.EndLog();
		std::fclose(tasLogFile);
		logWriter.Clear();
		tasLogging = false;
		ORIG_Con_Printf("Stopped TAS logging.\n");
	} else {
		const auto filename = CVars::bxt_taslog_filename.GetString();
		tasLogFile = std::fopen(filename.c_str(), "wb");
		if (!tasLogFile) {
			ORIG_Con_Printf("Unable to create log file %s: %s\n", filename.c_str(), std::strerror(errno));
			return;
		}
		const int buildNumber = ORIG_build_number ? ORIG_build_number() : -1;
		const char *gameDir = ClientDLL::GetInstance().pEngfuncs->pfnGetGameDirectory();
		logWriter.StartLog(tasLogFile, BUNNYMODXT_VERSION, buildNumber, gameDir);
		tasLogging = true;
		ORIG_Con_Printf("Started TAS logging into %s\n", filename.c_str());
	}
}

void HwDLL::RegisterCVarsAndCommandsIfNeeded()
{
	if (registeredVarsAndCmds)
		return;

	registeredVarsAndCmds = true;
	RegisterCVar(CVars::_bxt_taslog);
	RegisterCVar(CVars::_bxt_min_frametime);
	RegisterCVar(CVars::_bxt_tas_script_generation);
	RegisterCVar(CVars::bxt_taslog_filename);
	RegisterCVar(CVars::bxt_autopause);
	RegisterCVar(CVars::bxt_bhopcap);
	RegisterCVar(CVars::bxt_interprocess_enable);
	RegisterCVar(CVars::bxt_fade_remove);
	RegisterCVar(CVars::bxt_skybox_remove);
	RegisterCVar(CVars::bxt_water_remove);
	RegisterCVar(CVars::bxt_stop_demo_on_changelevel);
	RegisterCVar(CVars::bxt_tas_editor_simulate_for_ms);
	RegisterCVar(CVars::bxt_tas_norefresh_until_last_frames);
	RegisterCVar(CVars::bxt_tas_write_log);
	RegisterCVar(CVars::bxt_tas_playback_speed);
	RegisterCVar(CVars::bxt_disable_vgui);
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
	RegisterCVar(CVars::bxt_collision_depth_map_pixel_scale);
	RegisterCVar(CVars::bxt_collision_depth_map_remove_distance_limit);
	RegisterCVar(CVars::bxt_force_zmax);
	RegisterCVar(CVars::bxt_viewmodel_disable_idle);
	RegisterCVar(CVars::bxt_viewmodel_disable_equip);

	if (ORIG_R_DrawViewModel)
		RegisterCVar(CVars::bxt_viewmodel_fov);

	CVars::sv_cheats.Assign(FindCVar("sv_cheats"));
	CVars::fps_max.Assign(FindCVar("fps_max"));
	CVars::default_fov.Assign(FindCVar("default_fov"));

	FindCVarsIfNeeded();

	if (!ORIG_Cmd_AddMallocCommand)
		return;

	using CmdWrapper::Handler;
	typedef CmdWrapper::CmdWrapper<CmdFuncs> wrapper;

	wrapper::Add<Cmd_BXT_TAS_LoadScript, Handler<const char *>>("bxt_tas_loadscript");
	wrapper::Add<Cmd_BXT_TAS_ExportScript, Handler<const char *>>("bxt_tas_exportscript");
	wrapper::Add<Cmd_BXT_TAS_ExportLibTASInput, Handler<const char *>>("bxt_tas_export_libtas_input");
	wrapper::Add<Cmd_BXT_TAS_Split, Handler<const char *>>("bxt_tas_split");
	wrapper::Add<Cmd_BXT_TAS_New, Handler<const char *, const char *, int>>("bxt_tas_new");
	wrapper::AddCheat<Cmd_BXT_CH_Set_Health, Handler<float>>("bxt_ch_set_health");
	wrapper::AddCheat<Cmd_BXT_CH_Set_Armor, Handler<float>>("bxt_ch_set_armor");
	wrapper::AddCheat<Cmd_BXT_CH_Set_Origin, Handler<float, float, float>>("bxt_ch_set_pos");
	wrapper::AddCheat<Cmd_BXT_CH_Set_Origin_Offset, Handler<float, float, float>>("bxt_ch_set_pos_offset");
	wrapper::AddCheat<Cmd_BXT_CH_Set_Velocity, Handler<float, float, float>>("bxt_ch_set_vel");
	wrapper::AddCheat<
		Cmd_BXT_CH_Set_Velocity_Angles,
		Handler<float>,
		Handler<float, float, float>>("bxt_ch_set_vel_angles");
	wrapper::AddCheat<
		Cmd_BXT_CH_Set_Angles,
		Handler<float, float>,
		Handler<float, float, float>>("bxt_ch_set_angles");
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
	wrapper::Add<Cmd_BXT_Triggers_Place_Down, Handler<>, Handler<const char*>>("+bxt_triggers_place");
	wrapper::Add<Cmd_BXT_Triggers_Place_Up, Handler<>, Handler<const char*>>("-bxt_triggers_place");
	wrapper::Add<Cmd_BXT_Record, Handler<const char *>>("bxt_record");
	wrapper::Add<Cmd_BXT_AutoRecord, Handler<const char *>>("bxt_autorecord");
	wrapper::Add<Cmd_BXT_Map, Handler<const char *>>("_bxt_map");
	wrapper::Add<Cmd_BXT_Load, Handler<const char *>>("_bxt_load");
	wrapper::Add<Cmd_BXT_Interprocess_Reset, Handler<>>("_bxt_interprocess_reset");
	wrapper::Add<Cmd_BXT_Interprocess_Stop, Handler<>>("_bxt_interprocess_stop");
	wrapper::Add<Cmd_BXT_Set_Frametime_Remainder, Handler<double>>("_bxt_set_frametime_remainder");
	wrapper::Add<Cmd_BXT_Reset_Frametime_Remainder, Handler<>>("_bxt_reset_frametime_remainder");
	wrapper::Add<Cmd_BXT_TASLog, Handler<int>>("bxt_taslog");
	wrapper::Add<Cmd_BXT_Append, Handler<const char *>>("bxt_append");
	wrapper::Add<Cmd_BXT_FreeCam, Handler<int>>("bxt_freecam");
	wrapper::Add<Cmd_BXT_Print_Entities, Handler<>>("bxt_print_entities");

	wrapper::Add<Cmd_BXT_TAS_Editor_Unset_Pitch, Handler<>>("bxt_tas_editor_unset_pitch");
	wrapper::Add<Cmd_BXT_TAS_Editor_Unset_Yaw, Handler<>>("bxt_tas_editor_unset_yaw");
	wrapper::Add<Cmd_BXT_TAS_Editor_Set_Commands, Handler<const char*>>("bxt_tas_editor_set_commands");
	wrapper::Add<Cmd_BXT_TAS_Editor_Set_Repeats, Handler<int>>("bxt_tas_editor_set_repeats");
	wrapper::Add<Cmd_BXT_TAS_Editor_Set_Pitch, Handler<float>>("bxt_tas_editor_set_pitch");
	wrapper::Add<Cmd_BXT_TAS_Editor_Set_Yaw, Handler<float>>("bxt_tas_editor_set_yaw");
	wrapper::Add<Cmd_BXT_TAS_Editor_Toggle, Handler<const char*>>("bxt_tas_editor_toggle");
	wrapper::Add<Cmd_BXT_TAS_Editor_Set_Run_Point_And_Save, Handler<>>("bxt_tas_editor_set_run_point_and_save");
	wrapper::Add<Cmd_BXT_TAS_Editor_Delete_Last_Point, Handler<>>("bxt_tas_editor_delete_last_point");
	wrapper::Add<Cmd_BXT_TAS_Editor_Delete_Point, Handler<>>("bxt_tas_editor_delete_point");
	wrapper::Add<Cmd_BXT_TAS_Editor_Insert_Point, Handler<>>("bxt_tas_editor_insert_point");
	wrapper::Add<Cmd_BXT_TAS_Editor_Save, Handler<>>("bxt_tas_editor_save");
	wrapper::Add<Cmd_Plus_BXT_TAS_Editor_Look_Around, Handler<>, Handler<int>>("+bxt_tas_editor_look_around");
	wrapper::Add<Cmd_Minus_BXT_TAS_Editor_Look_Around, Handler<>, Handler<int>>("-bxt_tas_editor_look_around");
	wrapper::Add<Cmd_BXT_TAS_Editor, Handler<int>>("bxt_tas_editor");

	wrapper::Add<Cmd_BXT_TAS_Become_Simulator_Client, Handler<>>("bxt_tas_become_simulator_client");
	wrapper::Add<Cmd_BXT_TAS_Server_Send_Command, Handler<const char*>>("_bxt_tas_server_send_command");
}

void HwDLL::InsertCommands()
{
	bool runningFramesBackup = runningFrames;

	if (runningFrames && resetState == ResetState::NORMAL) {
		if (!wasRunningFrames) {
			if (CVars::bxt_tas_write_log.GetBool())
				SetTASLogging(true);
		}

		while (currentFramebulk < totalFramebulks) {
			preExecFramebulk = currentFramebulk;
			auto& f = input.GetFrame(currentFramebulk);
			// Movement frame.
			if (currentRepeat || f.IsMovement()) {
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
					edict_t *pl = GetPlayerEdict();
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
				PrevStrafeState = StrafeState;

				float health = 0;
				float armor = 0;
				edict_t* pl = GetPlayerEdict();
				if (pl) {
					health = pl->v.health;
					armor = pl->v.armorvalue;
				}

				simulation_ipc::send_simulated_frame_to_server(simulation_ipc::SimulatedFrame {
					CVars::_bxt_tas_script_generation.GetUint(),
					movementFrameCounter++,
					player,
					StrafeState,
					PrevFraction,
					PrevNormalz,
					thisFrameIs0ms,
					health,
					armor,
				});

				StartTracing();
				auto p = HLStrafe::MainFunc(player, GetMovementVars(), f, StrafeState, Buttons, ButtonsPresent, std::bind(&HwDLL::UnsafePlayerTrace, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), hlstrafe_version);
				StopTracing();

				PrevFraction = p.fractions[0];
				PrevNormalz = p.normalzs[0];

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

				// Save for the libTAS input export code below.
				const double left_state_multiplier = currentKeys.Left.StateMultiplier();
				const double right_state_multiplier = currentKeys.Right.StateMultiplier();

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
					double sidespeed = p.Sidespeed / std::min(left_state_multiplier, right_state_multiplier);
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

				--totalFrames;
				auto norefresh_until_frames = CVars::bxt_tas_norefresh_until_last_frames.GetInt();
				if (norefresh_until_frames > 0 && totalFrames <= static_cast<size_t>(norefresh_until_frames)
						&& CVars::_bxt_norefresh.GetBool())
					ORIG_Cbuf_InsertText("_bxt_norefresh 0\n");

				std::string newFrametime;
				if (p.NextFrameIs0ms) {
					if (!thisFrameIs0ms) {
						newFrametime = frametime0ms;
					}
				} else if (currentRepeat == 0 || thisFrameIs0ms) {
					// This will get the current framebulk and return the framerate back from 0ms
					// if we didn't switch to the next framebulk yet.
					HLTAS::Frame next;
					if (GetNextMovementFrame(next)) {
						if (next.Frametime != f.Frametime || thisFrameIs0ms) {
							newFrametime = next.Frametime.c_str();
						}
					}
				}

				if (!newFrametime.empty()) {
					std::ostringstream ss;
					ss << "host_framerate " << newFrametime << "\n";
					ORIG_Cbuf_InsertText(ss.str().c_str());
				}

				thisFrameIs0ms = p.NextFrameIs0ms;

				if (!exportFilename.empty())
					exportResult.InsertFrame(exportResult.GetFrames().size(), resulting_frame);

				if (libTASExportFile.is_open()) {
					// Keyboard inputs.
					libTASExportFile << "|K";

					std::string keyboard;
					if (p.Jump)
						keyboard += "20:";
					if (p.Duck)
						keyboard += "ffe3:";
					if (p.Use)
						keyboard += "65:";
					if (p.Reload)
						keyboard += "72:";

					// Movement is input with both buttons and joystick. Using just joystick is
					// possible, but breaks a number of things in the game which expect directional
					// buttons themselves to be pressed and not just forward/sidemove.
					//
					// Thankfully, CL_CreateMove automatically presses forward and back buttons for
					// us according to forwardmove. Unfortunately, it doesn't do the same for the
					// left and right buttons.
					//
					// Thus, left/right input works by pressing the directional buttons, then
					// computing the difference between the resulting sidemove and the desired one
					// and inputting that difference via joystick inputs.
					if (p.Left)
						keyboard += "61:";
					if (p.Right)
						keyboard += "64:";

					if (!keyboard.empty())
						// Write, chopping off the trailing ':'.
						libTASExportFile << keyboard.substr(0, keyboard.size() - 1);

					// Mouse inputs.
					//
					// Assumes sensitivity 0.2, m_yaw and m_pitch 0.022 (the default).
					libTASExportFile << "|M";

					const double SENSITIVITY = 0.2 * 0.022;

					int x_delta = 0, y_delta = 0;

					// Angle changes are only input when the state is 5.
					if (*reinterpret_cast<int*>(cls) == 5) {
						if (player.Viewangles[0] != p.Pitch) {
							auto angleDifference = HLStrafe::GetAngleDifference(player.Viewangles[0], p.Pitch);
							y_delta = std::lround(angleDifference / SENSITIVITY);
						}

						// anglemod is applied after +left/+right adjustment, but before the mouse input.
						auto oldYaw = HLStrafe::AngleModDeg(player.Viewangles[1]);
						if (oldYaw != p.Yaw) {
							auto angleDifference = HLStrafe::GetAngleDifference(static_cast<float>(oldYaw), p.Yaw);
							auto newyawIsNegative = (oldYaw + angleDifference < 0.0);
							auto yawDifference = angleDifference + (newyawIsNegative ? -HLStrafe::M_U_DEG_HALF : HLStrafe::M_U_DEG_HALF);
							x_delta = -std::lround(yawDifference / SENSITIVITY);

							// ORIG_Con_Printf("Yaw (pre-anglemod %.6f) %.6f => %.6f, angleDifference = %.8f, yawDifference = %.8f, x_delta = %.8f\n", player.Viewangles[1], oldYaw, p.Yaw, angleDifference, yawDifference, yawDifference / SENSITIVITY);
						}
					}

					libTASExportFile << x_delta << ':' << y_delta << ":R:";

					if (p.Attack1)
						libTASExportFile << "1.";
					else
						libTASExportFile << "..";

					if (p.Attack2)
						libTASExportFile << "3..";
					else
						libTASExportFile << "...";

					// Controller inputs.
					//
					// Assumes the following settings:
					//
					// joyforwardsensitivity -40.96
					// joyforwardthreshold 0
					// joysidesensitivity 40.96
					// joysidethreshold 0
					// joyadvanced 1
					// joyadvaxisx 3
					// joyadvaxisy 1
					libTASExportFile << "|C1";

					// The default value for cl_forward/side/back/upspeed (400) is assumed.
					const double CL_SPEED = 400;

					int x_axis = 0, y_axis = 0;
					double target_sidemove = 0;
					if (p.Forward)
						y_axis -= static_cast<int>(p.Forwardspeed);
					if (p.Back)
						y_axis += static_cast<int>(p.Backspeed);
					if (p.Right) {
						// BXT issues -moveleft, -moveright and the rest of the keys on the first
						// frame of running a .hltas script, which means that on the first input
						// frame the state of the pressed buttons will be 0.75. However, in libTAS
						// this is obviously not done (we always start from un-pressed buttons), so
						// there the state for the first input frame is 0.5.
						//
						// This clearedImpulsesForTheFirstTime bool is false during the period when
						// the button states are equal to 0.75 due to this BXT particularity.
						const double multiplier = clearedImpulsesForTheFirstTime ? right_state_multiplier : 0.5;
						x_axis += static_cast<int>(p.Sidespeed - CL_SPEED * multiplier);
						target_sidemove += p.Sidespeed;
					}
					if (p.Left) {
						const double multiplier = clearedImpulsesForTheFirstTime ? left_state_multiplier : 0.5;
						x_axis -= static_cast<int>(p.Sidespeed - CL_SPEED * multiplier);
						target_sidemove -= p.Sidespeed;
					}

					if (x_axis != 0)
						x_axis = x_axis * 2 + (target_sidemove >= 0 ? 1 : -1);
					if (y_axis != 0)
						y_axis = y_axis * 2 + (y_axis >= 0 ? 1 : -1);

					libTASExportFile << x_axis << ':' << y_axis << ":0:0:0:0:...............";

					// Next frame's FPS.
					libTASExportFile << "|T";

					// If there's new frametime use that, otherwise use current frame's frametime.
					std::string frametime = resulting_frame.Frametime;
					if (!newFrametime.empty())
						frametime = newFrametime;

					// Assume that frametime looks like "0.<some digits>".
					// We want to convert it to numerator = 1<number of digits zeros> and
					// denominator = <digits themselves>. For example, 0.004 becomes 1000/4 and
					// 0.010000001 becomes 1000000000/10000001.
					//
					// First, get rid of the leading "0.".
					auto fractional = frametime.substr(2);
					auto numerator = std::string("1") + std::string(fractional.size(), '0');
					auto denominator = fractional; // libTAS can deal with leading zeros.

					libTASExportFile << numerator << ':' << denominator;

					libTASExportFile << "|\n";
				}

				if (*reinterpret_cast<int*>(cls) == 5) {
					// Set this here, after the libTAS exporting code, and not above where the
					// impulses are actually cleared because the libTAS exporting code relies on
					// this value.
					clearedImpulsesForTheFirstTime = true;
				}

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
			} else if (f.StrafingAlgorithmPresent) {
				StrafeState.Algorithm = f.GetAlgorithm();
			} else if (f.AlgorithmParametersPresent) {
				StrafeState.Parameters = f.GetAlgorithmParameters();
			} else if (f.ChangePresent) {
				switch (f.GetChangeTarget()) {
				case HLTAS::ChangeTarget::YAW:
					StrafeState.ChangeYawFinalValue = f.GetChangeFinalValue();
					StrafeState.ChangeYawOver = f.GetChangeOver();
					break;
				case HLTAS::ChangeTarget::PITCH:
					StrafeState.ChangePitchFinalValue = f.GetChangeFinalValue();
					StrafeState.ChangePitchOver = f.GetChangeOver();
					break;
				case HLTAS::ChangeTarget::TARGET_YAW:
					StrafeState.ChangeTargetYawFinalValue = f.GetChangeFinalValue();
					StrafeState.ChangeTargetYawOver = f.GetChangeOver();
					break;
				default:
					assert(false);
					break;
				}
			}

			currentFramebulk++;
		};

		// Ran through all frames.
		if (currentFramebulk >= totalFramebulks) {
			runningFrames = false;

			if (!exportFilename.empty()) {
				auto error = exportResult.Save(exportFilename);
				if (error.Code == HLTAS::ErrorCode::OK)
					ORIG_Con_Printf("Exporting finished successfully.\n");
				else
					ORIG_Con_Printf("Error saving the exported script: %s\n", HLTAS::GetErrorMessage(error).c_str());

				exportFilename.clear();
				exportResult.Clear();
			}

			if (libTASExportFile.is_open()) {
				libTASExportFile.close();
				ORIG_Con_Printf("Exporting finished successfully.\n");
			}
		}
	} else {
		if (wasRunningFrames) {
			ORIG_Cbuf_InsertText("host_framerate 0;_bxt_min_frametime 0;bxt_taslog 0\n");

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
				edict_t *pl = GetPlayerEdict();
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
			auto traceFunc = std::bind(&HwDLL::PlayerTrace, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, false);
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
		if (currentRepeat || f.IsMovement())
			return true;

		curFramebulk++;
	};

	return false;
}

HLStrafe::PlayerData HwDLL::GetPlayerData()
{
	HLStrafe::PlayerData player{};

	edict_t *pl = GetPlayerEdict();
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

	GetViewangles(player.Viewangles);

	return player;
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
	vars.Frametime = GetFrameTime();
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
		edict_t *pl = GetPlayerEdict();
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

	simulation_ipc::receive_messages_from_server();
	if (!simulation_ipc::command_to_run.empty()
			// Starting a TAS in states 2, 3, 4 (loading) leads to crashes or desyncs.
			&& (*state == 5 || *state == 1)) {
		ORIG_Cbuf_AddText(simulation_ipc::command_to_run.c_str());
		simulation_ipc::command_to_run.clear();
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

	if (!executing) {
		QueuedSharedRNGSeeds = 0;

		if (libTASExportFile.is_open()) {
			// Add an empty frame.
			libTASExportFile << "|K|\n";
		}
	}

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

		if (*state == 4) {
			if (!splitFilename.empty()) {
				auto split_frame_index = currentRepeat > 0 ? currentFramebulk : (currentFramebulk > 0 ? currentFramebulk - 1 : currentFramebulk);
				auto split_frame = input.GetFrame(split_frame_index);

				HLTAS::Frame frame;
				frame.SetRepeats(1);
				frame.Frametime = split_frame.Frametime;
				splitResult.InsertFrame(splitResult.GetFrames().size(), frame);
			}

			if (!newTASFilename.empty()) {
				auto& frame = newTASResult.GetFrame(2);
				frame.SetRepeats(frame.GetRepeats() + 1);
			}
		}

		if (*state == 5) {
			if (!splitFilename.empty()) {
				// Cancel the bxt_autopause that we added.
				pauseOnTheFirstFrame = false;

				// Put the remaining framebulks into our new script.
				// The first movement frame needs to contain a "pause" command.
				if (currentRepeat > 0) {
					// The bxt_tas_split framebulk had multiple repeats.
					auto f = input.GetFrame(currentFramebulk);

					auto f2 = f;
					f2.SetRepeats(1);
					f2.Commands = "pause;" + f2.Commands;
					splitResult.InsertFrame(splitResult.GetFrames().size(), f2);

					if (f.GetRepeats() - currentRepeat - 1 > 0) {
						f.SetRepeats(f.GetRepeats() - currentRepeat - 1);
						splitResult.InsertFrame(splitResult.GetFrames().size(), f);
					}

					for (auto i = currentFramebulk + 1; i < totalFramebulks; ++i) {
						splitResult.InsertFrame(splitResult.GetFrames().size(), input.GetFrame(i));
					}
				} else {
					bool got_movement_frame = false;
					for (auto i = currentFramebulk; i < totalFramebulks; ++i) {
						auto f = input.GetFrame(i);

						if (f.GetRepeats() > 0 && !got_movement_frame) {
							got_movement_frame = true;

							auto f2 = f;
							f2.SetRepeats(1);
							f2.Commands = "pause;" + f2.Commands;
							splitResult.InsertFrame(splitResult.GetFrames().size(), f2);

							if (f.GetRepeats() - 1 > 0) {
								f.SetRepeats(f.GetRepeats() - 1);
								splitResult.InsertFrame(splitResult.GetFrames().size(), f);
							}

							continue;
						}

						splitResult.InsertFrame(splitResult.GetFrames().size(), f);
					}
				}

				std::ifstream file(splitFilename);
				if (file) {
					ORIG_Con_Printf("Error splitting: the target .hltas script exists. Remove it manually if you really want to split.\n");
				} else {
					auto error = splitResult.Save(splitFilename);
					if (error.Code == HLTAS::ErrorCode::OK)
						ORIG_Con_Printf("Splitting finished successfully.\n");
					else
						ORIG_Con_Printf("Error saving the split script: %s\n", HLTAS::GetErrorMessage(error).c_str());
				}

				splitFilename.clear();
				splitResult.Clear();
			}

			if (!newTASFilename.empty()) {
				auto& waitingFrame = newTASResult.GetFrame(2);
				if (waitingFrame.GetRepeats() > 1) {
					// Since the first frame contains commands, make it have 1 repeat and add
					// a separate frame for the rest of the waiting.
					auto newFrame = waitingFrame;
					newFrame.Commands.clear();
					newFrame.Comments.clear();
					newFrame.SetRepeats(waitingFrame.GetRepeats() - 1);
					waitingFrame.SetRepeats(1);
					newTASResult.InsertFrame(3, newFrame);
				}

				// Does this ever happen? If it does, handle by moving the commands to the next frame.
				if (waitingFrame.GetRepeats() == 0)
					newTASResult.RemoveFrame(2);

				std::ifstream file(newTASFilename);
				if (file) {
					ORIG_Con_Printf("Error creating the new TAS: the target .hltas script (%s) already exists. Please rename or remove it first.\n", newTASFilename.c_str());
				} else {
					auto error = newTASResult.Save(newTASFilename);
					if (error.Code == HLTAS::ErrorCode::OK)
						ORIG_Con_Printf("New TAS has been created successfully. Use this bind for launching it:\n bind / \"bxt_tas_loadscript %s\"\n", newTASFilename.c_str());
					else
						ORIG_Con_Printf("Error saving the new TAS: %s\n", HLTAS::GetErrorMessage(error).c_str());
				}

				newTASFilename.clear();
				newTASResult.Clear();
			}
		}

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

bool HwDLL::TryGettingAccurateInfo(float origin[3], float velocity[3], float& health)
{
	if (!svs || svs->num_clients < 1)
		return false;

	edict_t *pl = GetPlayerEdict();
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

void HwDLL::SetViewangles(float* va)
{
	if (ClientDLL::GetInstance().pEngfuncs) {
		ClientDLL::GetInstance().pEngfuncs->SetViewAngles(va);
	}
}

HLStrafe::TraceResult HwDLL::PlayerTrace(const float start[3], const float end[3], HLStrafe::HullType hull, bool extendDistanceLimit)
{
	StartTracing(extendDistanceLimit);
	const auto rv = UnsafePlayerTrace(start, end, hull);
	StopTracing();

	return rv;
}

HLStrafe::TraceResult HwDLL::CameraTrace(float max_distance)
{
	const auto& cl = ClientDLL::GetInstance();

	const auto start = cl.last_vieworg;
	Vector forward, right, up;
	cl.pEngfuncs->pfnAngleVectors(cl.last_viewangles, forward, right, up);
	Vector end = start + forward * max_distance;

	return PlayerTrace(start, end, HLStrafe::HullType::POINT);
}

void HwDLL::StartTracing(bool extendDistanceLimit) {
	if (!ORIG_PM_PlayerTrace || svs->num_clients < 1) {
		return;
	}

	trace_oldclient = *host_client;
	*host_client = svs->clients;
	trace_oldplayer = *sv_player;
	*sv_player = GetPlayerEdict();
	trace_oldmove = *ppmove;
	*ppmove = svmove;

	if (extendDistanceLimit)
		extendPlayerTraceDistanceLimit = true;

	ORIG_SV_AddLinksToPM(sv_areanodes, (*sv_player)->v.origin);

	extendPlayerTraceDistanceLimit = false;
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

	std::unordered_map<std::string, std::string> cvar_values;

	for (auto cvar = *cvar_vars; cvar != nullptr; cvar = cvar->next)
		cvar_values.emplace(cvar->name, cvar->string);

	RuntimeData::Add(std::move(cvar_values));

	if (sv && offMaxEdicts) {
		const int maxEdicts = *reinterpret_cast<int *>(reinterpret_cast<uintptr_t>(sv) + offMaxEdicts);
		RuntimeData::Add(RuntimeData::Edicts{ maxEdicts });
	}

	// Initial BXT timer value.
	CustomHud::SaveTimeToDemo();
}

void HwDLL::UpdateCustomTriggers()
{
	if (!svs || svs->num_clients < 1)
		return;

	edict_t *pl = GetPlayerEdict();
	if (!pl)
		return;

	CustomTriggers::Update(pl->v.origin, (pl->v.flags & FL_DUCKING) != 0);
}

void HwDLL::FreeCamTick()
{
	if (!free_cam_active)
		return;

	auto& cl = ClientDLL::GetInstance();

	Vector forward, right, up;
	cl.pEngfuncs->pfnAngleVectors(cl.last_viewangles, forward, right, up);

	Vector direction(0, 0, 0);
	auto buttons = cl.last_buttons;
	if (buttons & IN_FORWARD)
		direction += forward;
	if (buttons & IN_BACK)
		direction += -forward;
	if (buttons & IN_MOVERIGHT)
		direction += right;
	if (buttons & IN_MOVELEFT)
		direction += -right;

	auto frametime = static_cast<float>(*host_frametime);
	auto step = frametime * 320;

	// No easy access to +speed unfortunately.
	if (buttons & IN_ALT1)
		step *= 2;
	if (buttons & IN_DUCK)
		step /= 2;

	direction *= step;

	cameraOverrideOrigin[0] += direction[0];
	cameraOverrideOrigin[1] += direction[1];
	cameraOverrideOrigin[2] += direction[2];
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

HOOK_DEF_2(HwDLL, long, __cdecl, RandomLong, long, low, long, high)
{
	if (insideSStartDynamicSound)
		return low;

	auto ret = ORIG_RandomLong(low, high);
	//ORIG_Con_Printf("RandomLong(%ld, %ld) => %ld.\n", low, high, ret);
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

	if (libTASExportFile.is_open()) {
		// Seems to be the consistent number of blank frames required for changelevels.
		for (size_t i = 0; i < 2; ++i)
			libTASExportFile << "|K|\n";
	}

	return ORIG_SCR_BeginLoadingPlaque();
}

HOOK_DEF_1(HwDLL, int, __cdecl, Host_FilterTime, float, passedTime)
{
	static double timeCounter = 0.0;
	static bool usePassedTime = false;

	auto minFrametime = CVars::_bxt_min_frametime.GetFloat();

	if (runningFrames) {
		auto playbackSpeed = CVars::bxt_tas_playback_speed.GetFloat();
		if (playbackSpeed != 0 && host_frametime)
			minFrametime = static_cast<float>(*host_frametime) / playbackSpeed;
	}

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

HOOK_DEF_0(HwDLL, void, __cdecl, R_DrawSkyBox)
{
	if (CVars::sv_cheats.GetBool() && CVars::bxt_skybox_remove.GetBool())
		return;

	ORIG_R_DrawSkyBox();
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
	// We are unpausing from the menu (pressing Esc, or console key while console is visible)
	// and the TAS editor is in EDIT mode
	// and HideGameUI wants to add unpause to buffer
	// skip it
	if (insideHideGameUI && strcmp("unpause", text) == 0 && tas_editor_mode == TASEditorMode::EDIT)
		return;

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

HOOK_DEF_0(HwDLL, void, __cdecl, R_DrawViewModel)
{
	// If the current's frame FOV is not default_fov, we are zoomed in, in that case don't override frustum
	if (NeedViewmodelAdjustments())
	{
		glMatrixMode (GL_PROJECTION);
		glLoadIdentity();
		GLdouble w, h;
		GLdouble _near = 3.0;
		GLdouble _far = 4096.0;
		int ScreenWidth = CustomHud::GetScreenInfo().iWidth;
		int ScreenHeight = CustomHud::GetScreenInfo().iHeight;
		float fovY = CVars::bxt_viewmodel_fov.GetFloat();
		float aspect = (float)ScreenWidth / (float)ScreenHeight;

		h = tan (fovY / 360 * M_PI) * _near * ((float)ScreenHeight / (float)ScreenWidth);
		w = h * aspect;

		glFrustum (-w, w, -h, h, _near, _far);
		glMatrixMode (GL_MODELVIEW);
	}

	ORIG_R_DrawViewModel();
}

HOOK_DEF_0(HwDLL, void, __cdecl, R_Clear)
{
	// This is needed or everything will look washed out or with unintended
	// motion blur.
	if (CVars::bxt_water_remove.GetBool() || (CVars::sv_cheats.GetBool() && (CVars::bxt_wallhack.GetBool() || CVars::bxt_skybox_remove.GetBool()))) {
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

HOOK_DEF_3(HwDLL, void, __cdecl, SV_AddLinksToPM_, void *, node, float *, pmove_mins, float *, pmove_maxs)
{
	if (extendPlayerTraceDistanceLimit) {
		for (int i = 0; i < 3; ++i) {
			pmove_mins[i] -= 100000;
			pmove_maxs[i] += 100000;
		}
	}

	ORIG_SV_AddLinksToPM_(node, pmove_mins, pmove_maxs);
}

HOOK_DEF_2(HwDLL, void, __cdecl, SV_WriteEntitiesToClient, client_t*, client, void*, msg)
{
	auto num_edicts = reinterpret_cast<int *>(reinterpret_cast<uintptr_t>(sv) + offNumEdicts);
	const auto orig_num_edicts = *num_edicts;
	if (CVars::_bxt_norefresh.GetBool())
		*num_edicts = 0;

	ORIG_SV_WriteEntitiesToClient(client, msg);

	if (CVars::_bxt_norefresh.GetBool())
		*num_edicts = orig_num_edicts;
}

HOOK_DEF_1(HwDLL, void, __cdecl, VGuiWrap_Paint, int, paintAll)
{
	if (CVars::bxt_disable_vgui.GetBool()) {
		ORIG_VGuiWrap_Paint(0);
		return;
	}

	ORIG_VGuiWrap_Paint(paintAll);
}

HOOK_DEF_3(HwDLL, int, __cdecl, DispatchDirectUserMsg, char*, pszName, int, iSize, void*, pBuf)
{
	const char *gameDir = ClientDLL::GetInstance().pEngfuncs->pfnGetGameDirectory();

	if (!std::strcmp(gameDir, "czeror") && !std::strcmp(pszName, "InitHUD"))
		return ORIG_DispatchDirectUserMsg(0, iSize, pBuf);
	else
		return ORIG_DispatchDirectUserMsg(pszName, iSize, pBuf);
}

HOOK_DEF_0(HwDLL, void, __cdecl, SV_SetMoveVars)
{
	ORIG_SV_SetMoveVars();

	if (CVars::bxt_force_zmax.GetBool()) {
		if (movevars && offZmax) {
			auto movevars_zmax = reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(movevars) + offZmax);
			*movevars_zmax = CVars::bxt_force_zmax.GetFloat();
		}
	}
}

HOOK_DEF_0(HwDLL, void, __cdecl, R_StudioCalcAttachments)
{
	const auto &cl = ClientDLL::GetInstance();

	if (cl.pEngfuncs && ORIG_studioapi_GetCurrentEntity) {
		auto currententity = ORIG_studioapi_GetCurrentEntity();
		if (currententity == cl.pEngfuncs->GetViewModel() && NeedViewmodelAdjustments())
			insideRStudioCalcAttachmentsViewmodel = true;
	}

	ORIG_R_StudioCalcAttachments();
	insideRStudioCalcAttachmentsViewmodel = false;
}

HOOK_DEF_3(HwDLL, void, __cdecl, VectorTransform, float*, in1, float*, in2, float*, out)
{
	// No need for a NeedViewmodelAdjustments() here since insideStudioCalcAttachmentsViewmodel is
	// always FALSE from StudioCalcAttachments if we do NOT need the adjustments
	if (insideRStudioCalcAttachmentsViewmodel == false)
		ORIG_VectorTransform(in1, in2, out);
	else
	{
		ORIG_VectorTransform(in1, in2, out);
		Vector vOrigin(out);
		ClientDLL::GetInstance().StudioAdjustViewmodelAttachments(vOrigin);
		out[0] = vOrigin[0];
		out[1] = vOrigin[1];
		out[2] = vOrigin[2];
	}
}

HOOK_DEF_2(HwDLL, void, __cdecl, EmitWaterPolys, msurface_t *, fa, int, direction)
{
	if (CVars::bxt_water_remove.GetBool())
		return;

	ORIG_EmitWaterPolys(fa, direction);
}

HOOK_DEF_8(HwDLL, void, __cdecl, S_StartDynamicSound, int, entnum, int, entchannel, void*, sfx, vec_t*, origin,
                                                      float, fvol, float, attenuation, int, flags, int, pitch)
{
	insideSStartDynamicSound = true;

	ORIG_S_StartDynamicSound(entnum, entchannel, sfx, origin, fvol, attenuation, flags, pitch);

	insideSStartDynamicSound = false;
}

HOOK_DEF_3(HwDLL, void, __cdecl, VGuiWrap2_NotifyOfServerConnect, const char*, game, int, IP, int, port)
{
	// This function calls a function of interest in GameUI.dll and passes its
	// arguments there, so it is hooked to avoid adding a separate module.
	// This fixes MP3 sound stopping on level transitions in mods.
	// https://github.com/ValveSoftware/halflife/issues/570#issuecomment-486069492

	ORIG_VGuiWrap2_NotifyOfServerConnect("valve", IP, port);
}

HOOK_DEF_0(HwDLL, void, __cdecl, R_StudioSetupBones)
{
	if (pstudiohdr && ORIG_studioapi_GetCurrentEntity) {
		auto& cl = ClientDLL::GetInstance();
		auto currententity = ORIG_studioapi_GetCurrentEntity();
		auto pseqdesc = reinterpret_cast<mstudioseqdesc_t*>(reinterpret_cast<byte*>(*pstudiohdr) +
			(*pstudiohdr)->seqindex) + currententity->curstate.sequence;

		if (cl.pEngfuncs) {
			if (currententity == cl.pEngfuncs->GetViewModel()) {
				if (CVars::bxt_viewmodel_disable_idle.GetBool()) {
					if (strstr(pseqdesc->label, "idle") != NULL || strstr(pseqdesc->label, "fidget") != NULL) {
						currententity->curstate.framerate = 0; // don't animate at all
					}
				}

				if (CVars::bxt_viewmodel_disable_equip.GetBool()) {
					if (strstr(pseqdesc->label, "holster") != NULL || strstr(pseqdesc->label, "draw") != NULL ||
						strstr(pseqdesc->label, "deploy") != NULL || strstr(pseqdesc->label, "up") != NULL ||
						strstr(pseqdesc->label, "down") != NULL) {
						currententity->curstate.sequence = 0; // instead set to idle sequence
						pseqdesc = reinterpret_cast<mstudioseqdesc_t*>(reinterpret_cast<byte*>(*pstudiohdr) +
							(*pstudiohdr)->seqindex) + currententity->curstate.sequence;
						pseqdesc->numframes = 1;
						pseqdesc->fps = 1;
					}
				}
			}
		}
	}

	ORIG_R_StudioSetupBones();
}

HOOK_DEF_1(HwDLL, void, __cdecl, MD5Init, MD5Context_t*, context)
{
	ORIG_MD5Init(context);
}

HOOK_DEF_3(HwDLL, void, __cdecl, MD5Update, MD5Context_t*, context, unsigned char const*, buf, unsigned int, len)
{
	ORIG_MD5Update(context, buf, len);
}

HOOK_DEF_2(HwDLL, void, __cdecl, MD5Final, unsigned char*, digest, MD5Context_t*, context)
{
	ORIG_MD5Final(digest, context);
}

HOOK_DEF_2(HwDLL, void, __cdecl, MD5Transform, unsigned int*, buf, unsigned int const*, in)
{
	ORIG_MD5Transform(buf, in);
}

HOOK_DEF_5(HwDLL, int, __cdecl, MD5_Hash_File, unsigned char*, digest, char*, pszFileName, int, bUsefopen, int, bSeed, unsigned int*, seed)
{
	return ORIG_MD5_Hash_File(digest, pszFileName, bUsefopen, bSeed, seed);
}

HOOK_DEF_1(HwDLL, char*, __cdecl, MD5_Print, unsigned char*, hash)
{
	return ORIG_MD5_Print(hash);
}

HOOK_DEF_1(HwDLL, void, __fastcall, CBaseUI__HideGameUI, void*, thisptr)
{
	insideHideGameUI = true;
	ORIG_CBaseUI__HideGameUI(thisptr);
	insideHideGameUI = false;
}
