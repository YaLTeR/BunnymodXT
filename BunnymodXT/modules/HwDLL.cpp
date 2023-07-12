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
#include "../splits.hpp"
#include "../discord_integration.hpp"

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

extern "C" void __cdecl V_ApplyShake(float *origin, float *angles, float factor)
{
	return HwDLL::HOOKED_V_ApplyShake(origin, angles, factor);
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

extern "C" void __cdecl Cbuf_InsertText(const char* text)
{
	HwDLL::HOOKED_Cbuf_InsertText(text);
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

extern "C" void __cdecl R_PreDrawViewModel()
{
	HwDLL::HOOKED_R_PreDrawViewModel();
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

extern "C" void __cdecl _ZN7CBaseUI10HideGameUIEv(void* thisptr)
{
	return HwDLL::HOOKED_CBaseUI__HideGameUI_Linux(thisptr);
}

extern "C" void __cdecl R_DrawWorld()
{
	HwDLL::HOOKED_R_DrawWorld();
}

extern "C" void __cdecl R_DrawParticles()
{
	HwDLL::HOOKED_R_DrawParticles();
}

extern "C" void __cdecl R_SetFrustum()
{
	HwDLL::HOOKED_R_SetFrustum();
}

extern "C" void __cdecl ClientDLL_CalcRefdef(ref_params_s *pparams)
{
	ClientDLL::HOOKED_V_CalcRefdef(pparams);
}

extern "C" void __cdecl SPR_Set(HSPRITE_HL hSprite, int r, int g, int b)
{
	HwDLL::HOOKED_SPR_Set(hSprite, r, g, b);
}

extern "C" void __cdecl DrawCrosshair(int x, int y)
{
	HwDLL::HOOKED_DrawCrosshair(x, y);
}

extern "C" void __cdecl Draw_FillRGBA(int x, int y, int w, int h, int r, int g, int b, int a)
{
	HwDLL::HOOKED_Draw_FillRGBA(x, y, w, h, r, g, b, a);
}

extern "C" void __cdecl PF_traceline_DLL(const Vector* v1, const Vector* v2, int fNoMonsters, edict_t* pentToSkip, TraceResult* ptr)
{
	HwDLL::HOOKED_PF_traceline_DLL(v1, v2, fNoMonsters, pentToSkip, ptr);
}

extern "C" qboolean __cdecl CL_CheckGameDirectory(char *gamedir)
{
	return HwDLL::HOOKED_CL_CheckGameDirectory(gamedir);
}

extern "C" qboolean __cdecl Cvar_Command()
{
	return HwDLL::HOOKED_Cvar_Command();
}

extern "C" qboolean __cdecl Cvar_CommandWithPrivilegeCheck(qboolean bIsPrivileged)
{
	return HwDLL::HOOKED_Cvar_CommandWithPrivilegeCheck(bIsPrivileged);
}

extern "C" void __cdecl R_ForceCvars(qboolean mp)
{
	return HwDLL::HOOKED_R_ForceCvars(mp);
}

extern "C" void __cdecl CL_Restore(const char *mapName)
{
	return HwDLL::HOOKED_CL_Restore(mapName);
}

extern "C" void __cdecl CL_SignonReply()
{
	return HwDLL::HOOKED_CL_SignonReply();
}

extern "C" void __cdecl Sequence_OnLevelLoad(const char *fileName)
{
	return HwDLL::HOOKED_Sequence_OnLevelLoad(fileName);
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
	auto mutex = OpenMutexA(SYNCHRONIZE, FALSE, "SvenCoopLauncherMutex");
	if (!mutex)
		mutex = OpenMutexA(SYNCHRONIZE, FALSE, "ValveHalfLifeLauncherMutex");

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
			MemUtils::MarkAsExecutable(ORIG_Cbuf_InsertText);
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
			MemUtils::MarkAsExecutable(ORIG_V_ApplyShake);
			MemUtils::MarkAsExecutable(ORIG_R_DrawSkyBox);
			MemUtils::MarkAsExecutable(ORIG_SCR_UpdateScreen);
			MemUtils::MarkAsExecutable(ORIG_SV_Frame);
			MemUtils::MarkAsExecutable(ORIG_SV_SpawnServer);
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
			MemUtils::MarkAsExecutable(ORIG_R_PreDrawViewModel);
			MemUtils::MarkAsExecutable(ORIG_Mod_LeafPVS);
			MemUtils::MarkAsExecutable(ORIG_SV_AddLinksToPM_);
			MemUtils::MarkAsExecutable(ORIG_SV_WriteEntitiesToClient);
			MemUtils::MarkAsExecutable(ORIG_VGuiWrap_Paint);
			MemUtils::MarkAsExecutable(ORIG_DispatchDirectUserMsg);
			MemUtils::MarkAsExecutable(ORIG_SV_SetMoveVars);
			MemUtils::MarkAsExecutable(ORIG_VectorTransform);
			MemUtils::MarkAsExecutable(ORIG_R_StudioCalcAttachments);
			MemUtils::MarkAsExecutable(ORIG_EmitWaterPolys);
			MemUtils::MarkAsExecutable(ORIG_S_StartDynamicSound);
			MemUtils::MarkAsExecutable(ORIG_VGuiWrap2_NotifyOfServerConnect);
			MemUtils::MarkAsExecutable(ORIG_R_StudioSetupBones);
			MemUtils::MarkAsExecutable(ORIG_CBaseUI__HideGameUI);
			MemUtils::MarkAsExecutable(ORIG_R_DrawWorld);
			MemUtils::MarkAsExecutable(ORIG_R_DrawParticles);
			MemUtils::MarkAsExecutable(ORIG_BUsesSDLInput);
			MemUtils::MarkAsExecutable(ORIG_R_StudioRenderModel);
			MemUtils::MarkAsExecutable(ORIG_R_SetFrustum);
			MemUtils::MarkAsExecutable(ORIG_SPR_Set);
			MemUtils::MarkAsExecutable(ORIG_DrawCrosshair);
			MemUtils::MarkAsExecutable(ORIG_Draw_FillRGBA);
			MemUtils::MarkAsExecutable(ORIG_PF_traceline_DLL);
			MemUtils::MarkAsExecutable(ORIG_CL_CheckGameDirectory);
			MemUtils::MarkAsExecutable(ORIG_Host_ValidSave);
			MemUtils::MarkAsExecutable(ORIG_SaveGameSlot);
			MemUtils::MarkAsExecutable(ORIG_SCR_NetGraph);
			MemUtils::MarkAsExecutable(ORIG_Host_Shutdown);
			MemUtils::MarkAsExecutable(ORIG_ReleaseEntityDlls);
			MemUtils::MarkAsExecutable(ORIG_Cvar_Command);
			MemUtils::MarkAsExecutable(ORIG_Cvar_CommandWithPrivilegeCheck);
			MemUtils::MarkAsExecutable(ORIG_R_ForceCvars);
			MemUtils::MarkAsExecutable(ORIG_GL_EndRendering);
			MemUtils::MarkAsExecutable(ORIG_V_RenderView);
			MemUtils::MarkAsExecutable(ORIG_SV_AddToFullPack);
			MemUtils::MarkAsExecutable(ORIG_CL_EmitEntities);
			MemUtils::MarkAsExecutable(ORIG_LoadAdjacentEntities);
			MemUtils::MarkAsExecutable(ORIG_JumpButton);
			MemUtils::MarkAsExecutable(ORIG_PlayerMove);
			MemUtils::MarkAsExecutable(ORIG_CL_Restore);
			MemUtils::MarkAsExecutable(ORIG_CL_SignonReply);
			MemUtils::MarkAsExecutable(ORIG_Sequence_OnLevelLoad);
		}

		MemUtils::Intercept(moduleName,
			ORIG_LoadAndDecryptHwDLL, HOOKED_LoadAndDecryptHwDLL,
			ORIG_Cbuf_Execute, HOOKED_Cbuf_Execute,
			ORIG_Cbuf_AddText, HOOKED_Cbuf_AddText,
			ORIG_Cbuf_InsertText, HOOKED_Cbuf_InsertText,
			ORIG_Cbuf_InsertTextLines, HOOKED_Cbuf_InsertTextLines,
			ORIG_Cmd_TokenizeString, HOOKED_Cmd_TokenizeString,
			ORIG_SeedRandomNumberGenerator, HOOKED_SeedRandomNumberGenerator,
			ORIG_time, HOOKED_time,
			ORIG_RandomFloat, HOOKED_RandomFloat,
			ORIG_RandomLong, HOOKED_RandomLong,
			ORIG_Host_Changelevel2_f, HOOKED_Host_Changelevel2_f,
			ORIG_SCR_BeginLoadingPlaque, HOOKED_SCR_BeginLoadingPlaque,
			ORIG_Host_FilterTime, HOOKED_Host_FilterTime,
			ORIG_Host_ValidSave, HOOKED_Host_ValidSave,
			ORIG_SCR_NetGraph, HOOKED_SCR_NetGraph,
			ORIG_V_FadeAlpha, HOOKED_V_FadeAlpha,
			ORIG_V_ApplyShake, HOOKED_V_ApplyShake,
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
			ORIG_R_PreDrawViewModel, HOOKED_R_PreDrawViewModel,
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
			ORIG_CBaseUI__HideGameUI, HOOKED_CBaseUI__HideGameUI,
			ORIG_R_DrawWorld, HOOKED_R_DrawWorld,
			ORIG_R_DrawParticles, HOOKED_R_DrawParticles,
			ORIG_BUsesSDLInput, HOOKED_BUsesSDLInput,
			ORIG_R_StudioRenderModel, HOOKED_R_StudioRenderModel,
			ORIG_R_SetFrustum, HOOKED_R_SetFrustum,
			ORIG_SPR_Set, HOOKED_SPR_Set,
			ORIG_DrawCrosshair, HOOKED_DrawCrosshair,
			ORIG_Draw_FillRGBA, HOOKED_Draw_FillRGBA,
			ORIG_PF_traceline_DLL, HOOKED_PF_traceline_DLL,
			ORIG_CL_CheckGameDirectory, HOOKED_CL_CheckGameDirectory,
			ORIG_SaveGameSlot, HOOKED_SaveGameSlot,
			ORIG_ReleaseEntityDlls, HOOKED_ReleaseEntityDlls,
			ORIG_Host_Shutdown, HOOKED_Host_Shutdown,
			ORIG_Cvar_Command, HOOKED_Cvar_Command,
			ORIG_Cvar_CommandWithPrivilegeCheck, HOOKED_Cvar_CommandWithPrivilegeCheck,
			ORIG_R_ForceCvars, HOOKED_R_ForceCvars,
			ORIG_GL_EndRendering, HOOKED_GL_EndRendering,
			ORIG_SV_AddToFullPack, HOOKED_SV_AddToFullPack,
			ORIG_CL_EmitEntities, HOOKED_CL_EmitEntities,
			ORIG_V_RenderView, HOOKED_V_RenderView,
			ORIG_LoadAdjacentEntities, HOOKED_LoadAdjacentEntities,
			ORIG_JumpButton, HOOKED_JumpButton,
			ORIG_PlayerMove, HOOKED_PlayerMove,
			ORIG_CL_Restore, HOOKED_CL_Restore,
			ORIG_CL_SignonReply, HOOKED_CL_SignonReply,
			ORIG_Sequence_OnLevelLoad, HOOKED_Sequence_OnLevelLoad);
	}

	#ifdef _WIN32
		#ifdef COF_BUILD
		if (ORIG_Host_ValidSave && !is_cof_steam)
			WinErrorIfWrongBXTVersion("Loaded Bunnymod XT (CoF Steam version) in non-CoF game! Download the right version!");
		#elif SDK10_BUILD
		if (ORIG_Host_ValidSave && !is_sdk10)
			WinErrorIfWrongBXTVersion("Loaded Bunnymod XT (HLSDK 1.0) in post-1.0.1.6 WON! Download the right version!");
		#else
		if (is_cof_steam)
			WinErrorIfWrongBXTVersion("Loaded BunnymodXT (HL version) in CoF Steam! Download the right version!");
		else if (is_sdk10)
			WinErrorIfWrongBXTVersion("Loaded BunnymodXT (HLSDK 2.0+) in pre-1.1.0.0 WON! Download the right version!");
		#endif
	#endif
}

void HwDLL::Unhook()
{
	if (m_Intercepted)
	{
		MemUtils::RemoveInterception(m_Name,
			ORIG_LoadAndDecryptHwDLL,
			ORIG_Cbuf_Execute,
			ORIG_Cbuf_AddText,
			ORIG_Cbuf_InsertText,
			ORIG_Cbuf_InsertTextLines,
			ORIG_Cmd_TokenizeString,
			ORIG_SeedRandomNumberGenerator,
			ORIG_time,
			ORIG_RandomFloat,
			ORIG_RandomLong,
			ORIG_Host_Changelevel2_f,
			ORIG_SCR_BeginLoadingPlaque,
			ORIG_Host_FilterTime,
			ORIG_Host_ValidSave,
			ORIG_SCR_NetGraph,
			ORIG_V_FadeAlpha,
			ORIG_V_ApplyShake,
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
			ORIG_R_PreDrawViewModel,
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
			ORIG_CBaseUI__HideGameUI,
			ORIG_R_DrawWorld,
			ORIG_R_DrawParticles,
			ORIG_BUsesSDLInput,
			ORIG_R_StudioRenderModel,
			ORIG_R_SetFrustum,
			ORIG_SPR_Set,
			ORIG_DrawCrosshair,
			ORIG_Draw_FillRGBA,
			ORIG_PF_traceline_DLL,
			ORIG_CL_CheckGameDirectory,
			ORIG_SaveGameSlot,
			ORIG_ReleaseEntityDlls,
			ORIG_Host_Shutdown,
			ORIG_Cvar_Command,
			ORIG_Cvar_CommandWithPrivilegeCheck,
			ORIG_R_ForceCvars,
			ORIG_GL_EndRendering,
			ORIG_SV_AddToFullPack,
			ORIG_CL_EmitEntities,
			ORIG_V_RenderView,
			ORIG_LoadAdjacentEntities,
			ORIG_JumpButton,
			ORIG_PlayerMove,
			ORIG_CL_Restore,
			ORIG_CL_SignonReply,
			ORIG_Sequence_OnLevelLoad);
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
	ORIG_Host_ValidSave = nullptr;
	ORIG_SCR_NetGraph = nullptr;
	ORIG_V_FadeAlpha = nullptr;
	ORIG_V_ApplyShake = nullptr;
	ORIG_R_DrawSkyBox = nullptr;
	ORIG_SCR_UpdateScreen = nullptr;
	ORIG_SV_Frame = nullptr;
	ORIG_SV_SpawnServer = nullptr;
	ORIG_CL_Stop_f = nullptr;
	ORIG_Host_Loadgame_f = nullptr;
	ORIG_Host_Reload_f = nullptr;
	ORIG_VGuiWrap2_ConDPrintf = nullptr;
	ORIG_VGuiWrap2_ConPrintf = nullptr;
	ORIG_Cbuf_AddText = nullptr;
	ORIG_Cbuf_InsertText = nullptr;
	ORIG_Cbuf_InsertTextLines = nullptr;
	ORIG_Cmd_TokenizeString = nullptr;
	ORIG_Con_Printf = nullptr;
	ORIG_Cvar_RegisterVariable = nullptr;
	ORIG_Cvar_DirectSet = nullptr;
	ORIG_Cvar_FindVar = nullptr;
	ORIG_Cmd_FindCmd = nullptr;
	ORIG_Cmd_Exists = nullptr;
	ORIG_Draw_String = nullptr;
	ORIG_Host_Notarget_f = nullptr;
	ORIG_Host_Noclip_f = nullptr;
	ORIG_Cmd_AddMallocCommand = nullptr;
	ORIG_Cmd_AddHUDCommand = nullptr;
	ORIG_Cmd_Argc = nullptr;
	ORIG_Cmd_Args = nullptr;
	ORIG_Cmd_Argv = nullptr;
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
	ORIG_R_PreDrawViewModel = nullptr;
	ORIG_Mod_LeafPVS = nullptr;
	ORIG_SV_AddLinksToPM_ = nullptr;
	ORIG_SV_WriteEntitiesToClient = nullptr;
	ORIG_VGuiWrap_Paint = nullptr;
	ORIG_DispatchDirectUserMsg = nullptr;
	ORIG_SV_SetMoveVars = nullptr;
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
	ORIG_CBaseUI__HideGameUI_Linux = nullptr;
	ORIG_R_DrawWorld = nullptr;
	ORIG_R_DrawParticles = nullptr;
	ORIG_BUsesSDLInput = nullptr;
	ORIG_R_StudioRenderModel = nullptr;
	ORIG_R_SetFrustum = nullptr;
	ORIG_SPR_Set = nullptr;
	ORIG_DrawCrosshair = nullptr;
	ORIG_Draw_FillRGBA = nullptr;
	ORIG_PF_traceline_DLL = nullptr;
	ORIG_CL_CheckGameDirectory = nullptr;
	ORIG_CL_HudMessage = nullptr;
	ORIG_SaveGameSlot = nullptr;
	ORIG_SCR_NetGraph = nullptr;
	ORIG_VGuiWrap2_IsGameUIVisible = nullptr;
	ORIG_SCR_DrawPause = nullptr;
	ORIG_Host_Shutdown = nullptr;
	ORIG_ReleaseEntityDlls = nullptr;
	ORIG_Cvar_Command = nullptr;
	ORIG_Cvar_CommandWithPrivilegeCheck = nullptr;
	ORIG_R_ForceCvars = nullptr;
	ORIG_GL_BuildLightmaps = nullptr;
	ORIG_VID_FlipScreen = nullptr;
	ORIG_GL_EndRendering = nullptr;
	ORIG_SV_AddToFullPack = nullptr;
	ORIG_CL_EmitEntities = nullptr;
	ORIG_Cmd_ForwardToServer = nullptr;
	ORIG_MSG_WriteByte = nullptr;
	ORIG_V_RenderView = nullptr;
	ORIG_LoadAdjacentEntities = nullptr;
	ORIG_JumpButton = nullptr;
	ORIG_PlayerMove = nullptr;
	ORIG_PM_CheckStuck = nullptr;
	ORIG_R_LoadSkys = nullptr;
	ORIG_CL_Restore = nullptr;
	ORIG_CL_SignonReply = nullptr;
	ORIG_Sequence_OnLevelLoad = nullptr;
	cantJumpNextTime.clear();

	ClientDLL::GetInstance().pEngfuncs = nullptr;
	ServerDLL::GetInstance().pEngfuncs = nullptr;
	ppGlobals = nullptr;
	pEngStudio = nullptr;
	pEngineAPI = nullptr;

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
	offActiveAddr = 0;
	pcl = nullptr;
	offMaxEdictsAddr = 0;
	cls = nullptr;
	psv = nullptr;
	lastRecordedHealth = 0;
	offTime = 0;
	offName = 0;
	offModels = 0;
	offNumEdicts = 0;
	offMaxEdicts = 0;
	offEdicts = 0;
	svs = nullptr;
	offEdict = 0;
	svmove = nullptr;
	ppmove = nullptr;
	won_ppmove = nullptr;
	host_client = nullptr;
	sv_player = nullptr;
	sv_areanodes = nullptr;
	cl_entities = nullptr;
	viewangles = nullptr;
	simorg = nullptr;
	simvel = nullptr;
	cl_paused = nullptr;
	cl_time = nullptr;
	playernum = nullptr;
	levelname = nullptr;
	viewent = nullptr;
	currententity = nullptr;
	cmd_text = nullptr;
	host_frametime = nullptr;
	cvar_vars = nullptr;
	movevars = nullptr;
	cmd_source = nullptr;
	r_refdef = nullptr;
	gLoadSky = nullptr;
	reliable_datagram = nullptr;
	pHost_FilterTime_FPS_Cap_Byte = 0;
	pHost_FilterTime_Check_72FPS = 0;
	pHost_FilterTime_Set_72FPS = 0;
	pGL_EndRendering_VID_FlipScreen_Bytes = 0;
	cofSaveHack = nullptr;
	noclip_anglehack = nullptr;
	frametime_remainder = nullptr;
	pstudiohdr = nullptr;
	scr_fov_value = nullptr;
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
	TargetYawOverrideIndex = 0;
	TargetYawOverrides.clear();
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
	tas_editor_insert_point_held = false;
	tas_editor_toggle_s00 = false;
	tas_editor_toggle_s01 = false;
	tas_editor_toggle_s03 = false;
	tas_editor_toggle_s10 = false;
	tas_editor_toggle_s11 = false;
	tas_editor_toggle_s13 = false;
	tas_editor_toggle_s22 = false;
	tas_editor_toggle_s06 = false;
	tas_editor_toggle_s07 = false;
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
	tas_editor_set_frametime = false;
	tas_editor_set_yaw = false;
	tas_editor_set_pitch = false;
	tas_editor_set_repeats = false;
	tas_editor_set_commands = false;
	tas_editor_set_left_right_count = false;
	tas_editor_unset_yaw = false;
	tas_editor_unset_pitch = false;
	tas_editor_apply_smoothing = false;
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
		PrevFractions = {1, 0, 0, 0 };
		PrevNormalzs = {0, 0, 0, 0 };
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

		pcl = MemUtils::GetSymbolAddress(m_Handle, "cl");
		if (pcl) {
			EngineDevMsg("[hw dll] Found cl at %p.\n", pcl);
			viewangles = reinterpret_cast<Vector*>(reinterpret_cast<uintptr_t>(pcl) + 0x2abe4);
			simorg = reinterpret_cast<Vector*>(reinterpret_cast<uintptr_t>(pcl) + 0x2ac08);
			simvel = reinterpret_cast<Vector*>(reinterpret_cast<uintptr_t>(pcl) + 0x2ac14);
			cl_paused = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(pcl) + 0x2af60);
			cl_time = reinterpret_cast<double*>(reinterpret_cast<uintptr_t>(pcl) + 0x2af94);
			playernum = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(pcl) + 0x19bca8);
			levelname = reinterpret_cast<char*>(reinterpret_cast<uintptr_t>(pcl) + 0x1a34b8);
			viewent = reinterpret_cast<cl_entity_t*>(reinterpret_cast<uintptr_t>(pcl) + 0x1a34fc);
		} else
			EngineDevWarning("[hw dll] Could not find cl.\n");

		cls = MemUtils::GetSymbolAddress(m_Handle, "cls");
		if (cls) {
			EngineDevMsg("[hw dll] Found cls at %p.\n", cls);
			signon = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(cls) + 0x3408);
			demorecording = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(cls) + 0x405c);
			demoplayback = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(cls) + 0x4060);
		} else
			EngineDevWarning("[hw dll] Could not find cls.\n");

		psv = MemUtils::GetSymbolAddress(m_Handle, "sv");
		if (psv) {
			EngineDevMsg("[hw dll] Found sv at %p.\n", psv);
			offTime = 0xc;
			offName = 0x28;
			offModels = 0x30948;
			offNumEdicts = 0x3bc50;
			offMaxEdicts = offNumEdicts + 0x4;
			offEdicts = offNumEdicts + 0x8;
			reliable_datagram = reinterpret_cast<sizebuf_t*>(reinterpret_cast<uintptr_t>(psv) + 0x3cc1c);
		} else
			EngineDevWarning("[hw dll] Could not find sv.\n");

		svs = reinterpret_cast<server_static_t*>(MemUtils::GetSymbolAddress(m_Handle, "svs"));
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

		cmd_text = reinterpret_cast<sizebuf_t*>(MemUtils::GetSymbolAddress(m_Handle, "cmd_text"));
		if (cmd_text)
			EngineDevMsg("[hw dll] Found cmd_text at %p.\n", cmd_text);
		else
			EngineDevWarning("[hw dll] Could not find cmd_text.\n");

		cmd_alias = reinterpret_cast<cmdalias_t*>(MemUtils::GetSymbolAddress(m_Handle, "cmd_alias"));
		if (cmd_alias)
			EngineDevMsg("[hw dll] Found cmd_alias at %p.\n", cmd_alias);
		else
			EngineDevWarning("[hw dll] Could not find cmd_alias.\n");

		cmd_source = reinterpret_cast<cmd_source_t*>(MemUtils::GetSymbolAddress(m_Handle, "cmd_source"));
		if (cmd_source)
			EngineDevMsg("[hw dll] Found cmd_source at %p.\n", cmd_source);
		else
			EngineDevWarning("[hw dll] Could not find cmd_source.\n");

		r_refdef = reinterpret_cast<refdef_t*>(MemUtils::GetSymbolAddress(m_Handle, "r_refdef"));
		if (r_refdef)
			EngineDevMsg("[hw dll] Found r_refdef at %p.\n", r_refdef);
		else
			EngineDevWarning("[hw dll] Could not find r_refdef.\n");

		host_frametime = reinterpret_cast<double*>(MemUtils::GetSymbolAddress(m_Handle, "host_frametime"));
		if (host_frametime)
			EngineDevMsg("[hw dll] Found host_frametime at %p.\n", host_frametime);
		else
			EngineDevWarning("[hw dll] Could not find host_frametime.\n");

		cvar_vars = reinterpret_cast<cvar_t**>(MemUtils::GetSymbolAddress(m_Handle, "cvar_vars"));
		if (cvar_vars)
			EngineDevMsg("[hw dll] Found cvar_vars at %p.\n", cvar_vars);
		else
			EngineDevWarning("[hw dll] Could not find cvar_vars.\n");

		movevars = reinterpret_cast<movevars_t*>(MemUtils::GetSymbolAddress(m_Handle, "movevars"));
		if (movevars) {
			EngineDevMsg("[hw dll] Found movevars at %p.\n", movevars);
		} else
			EngineDevWarning("[hw dll] Could not find movevars.\n");

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

		ORIG_R_LoadSkys = reinterpret_cast<_R_LoadSkys>(MemUtils::GetSymbolAddress(m_Handle, "R_LoadSkys"));
		if (ORIG_R_LoadSkys)
			EngineDevMsg("[hw dll] Found R_LoadSkys at %p.\n", ORIG_R_LoadSkys);
		else
			EngineDevWarning("[hw dll] Could not find R_LoadSkys.\n");

		gLoadSky = reinterpret_cast<int*>(MemUtils::GetSymbolAddress(m_Handle, "gLoadSky"));
		if (gLoadSky)
			EngineDevMsg("[hw dll] Found gLoadSky [Linux] at %p.\n", gLoadSky);
		else
			EngineDevWarning("[hw dll] Could not find gLoadSky [Linux].\n");

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

		ORIG_CBaseUI__HideGameUI_Linux = reinterpret_cast<_CBaseUI__HideGameUI_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN7CBaseUI10HideGameUIEv"));
		if (ORIG_CBaseUI__HideGameUI_Linux)
			EngineDevMsg("[hw dll] Found CBaseUI::HideGameUI [Linux] at %p.\n", ORIG_CBaseUI__HideGameUI_Linux);
		else
			EngineDevWarning("[hw dll] Could not find CBaseUI::HideGameUI [Linux].\n");

		ppGlobals = reinterpret_cast<globalvars_t*>(MemUtils::GetSymbolAddress(m_Handle, "gGlobalVariables"));
		if (ppGlobals)
			EngineDevMsg("[hw dll] Found gGlobalVariables [Linux] at %p.\n", ppGlobals);
		else
			EngineDevWarning("[hw dll] Could not find gGlobalVariables [Linux].\n");

		pEngStudio = reinterpret_cast<engine_studio_api_t*>(MemUtils::GetSymbolAddress(m_Handle, "engine_studio_api"));
		if (pEngStudio)
			EngineDevMsg("[hw dll] Found engine_studio_api [Linux] at %p.\n", pEngStudio);
		else
			EngineDevWarning("[hw dll] Could not find engine_studio_api [Linux].\n");

		cl_entities = reinterpret_cast<cl_entity_t**>(MemUtils::GetSymbolAddress(m_Handle, "cl_entities"));
		if (cl_entities)
			EngineDevMsg("[hw dll] Found cl_entities [Linux] at %p.\n", cl_entities);
		else
			EngineDevWarning("[hw dll] Could not find cl_entities [Linux].\n");

		currententity = reinterpret_cast<cl_entity_t**>(MemUtils::GetSymbolAddress(m_Handle, "currententity"));
		if (currententity)
			EngineDevMsg("[hw dll] Found currententity [Linux] at %p.\n", currententity);
		else
			EngineDevWarning("[hw dll] Could not find currententity [Linux].\n");

		ORIG_SPR_Set = reinterpret_cast<_SPR_Set>(MemUtils::GetSymbolAddress(m_Handle, "SPR_Set"));
		if (ORIG_SPR_Set)
			EngineDevMsg("[hw dll] Found SPR_Set at %p.\n", ORIG_SPR_Set);
		else
			EngineDevWarning("[hw dll] Could not find SPR_Set.\n");

		ORIG_DrawCrosshair = reinterpret_cast<_DrawCrosshair>(MemUtils::GetSymbolAddress(m_Handle, "DrawCrosshair"));
		if (ORIG_DrawCrosshair)
			EngineDevMsg("[hw dll] Found DrawCrosshair at %p.\n", ORIG_DrawCrosshair);
		else
			EngineDevWarning("[hw dll] Could not find DrawCrosshair.\n");

		ORIG_Draw_FillRGBA = reinterpret_cast<_Draw_FillRGBA>(MemUtils::GetSymbolAddress(m_Handle, "Draw_FillRGBA"));
		if (ORIG_Draw_FillRGBA)
			EngineDevMsg("[hw dll] Found Draw_FillRGBA at %p.\n", ORIG_Draw_FillRGBA);
		else
			EngineDevWarning("[hw dll] Could not find Draw_FillRGBA.\n");

		ORIG_PF_traceline_DLL = reinterpret_cast<_PF_traceline_DLL>(MemUtils::GetSymbolAddress(m_Handle, "PF_traceline_DLL"));
		if (ORIG_PF_traceline_DLL)
			EngineDevMsg("[hw dll] Found PF_traceline_DLL at %p.\n", ORIG_PF_traceline_DLL);
		else
			EngineDevWarning("[hw dll] Could not find PF_traceline_DLL.\n");

		ORIG_CL_CheckGameDirectory = reinterpret_cast<_CL_CheckGameDirectory>(MemUtils::GetSymbolAddress(m_Handle, "CL_CheckGameDirectory"));
		if (ORIG_CL_CheckGameDirectory)
			EngineDevMsg("[hw dll] Found CL_CheckGameDirectory at %p.\n", ORIG_CL_CheckGameDirectory);
		else
			EngineDevWarning("[hw dll] Could not find CL_CheckGameDirectory.\n");

		ORIG_Cvar_Command = reinterpret_cast<_Cvar_Command>(MemUtils::GetSymbolAddress(m_Handle, "Cvar_Command"));
		if (ORIG_Cvar_Command)
			EngineDevMsg("[hw dll] Found Cvar_Command at %p.\n", ORIG_Cvar_Command);
		else
			EngineDevWarning("[hw dll] Could not find Cvar_Command.\n");

		ORIG_Cvar_CommandWithPrivilegeCheck = reinterpret_cast<_Cvar_CommandWithPrivilegeCheck>(MemUtils::GetSymbolAddress(m_Handle, "Cvar_CommandWithPrivilegeCheck"));
		if (ORIG_Cvar_CommandWithPrivilegeCheck)
			EngineDevMsg("[hw dll] Found Cvar_CommandWithPrivilegeCheck at %p.\n", ORIG_Cvar_CommandWithPrivilegeCheck);
		else
			EngineDevWarning("[hw dll] Could not find Cvar_CommandWithPrivilegeCheck.\n");

		ORIG_R_ForceCvars = reinterpret_cast<_R_ForceCvars>(MemUtils::GetSymbolAddress(m_Handle, "R_ForceCvars"));
		if (ORIG_R_ForceCvars)
			EngineDevMsg("[hw dll] Found R_ForceCvars at %p.\n", ORIG_R_ForceCvars);
		else
			EngineDevWarning("[hw dll] Could not find R_ForceCvars.\n");

		ORIG_GL_BuildLightmaps = reinterpret_cast<_GL_BuildLightmaps>(MemUtils::GetSymbolAddress(m_Handle, "GL_BuildLightmaps"));
		if (ORIG_GL_BuildLightmaps)
			EngineDevMsg("[hw dll] Found GL_BuildLightmaps at %p.\n", ORIG_GL_BuildLightmaps);
		else
			EngineDevWarning("[hw dll] Could not find GL_BuildLightmaps.\n");

		ORIG_CL_Restore = reinterpret_cast<_CL_Restore>(MemUtils::GetSymbolAddress(m_Handle, "CL_Restore"));
		if (ORIG_R_ForceCvars)
			EngineDevMsg("[hw dll] Found CL_Restore at %p.\n", ORIG_CL_Restore);
		else
			EngineDevWarning("[hw dll] Could not find CL_Restore.\n");

		ORIG_CL_SignonReply = reinterpret_cast<_CL_SignonReply>(MemUtils::GetSymbolAddress(m_Handle, "CL_SignonReply"));
		if (ORIG_CL_SignonReply)
			EngineDevMsg("[hw dll] Found CL_SignonReply at %p.\n", ORIG_CL_SignonReply);
		else
			EngineDevWarning("[hw dll] Could not find CL_SignonReply.\n");

		ORIG_Sequence_OnLevelLoad = reinterpret_cast<_Sequence_OnLevelLoad>(MemUtils::GetSymbolAddress(m_Handle, "Sequence_OnLevelLoad"));
		if (ORIG_Sequence_OnLevelLoad)
			EngineDevMsg("[hw dll] Found Sequence_OnLevelLoad at %p.\n", ORIG_Sequence_OnLevelLoad);
		else
			EngineDevWarning("[hw dll] Could not find Sequence_OnLevelLoad.\n");

		if (!pcl || !cls || !psv || !svs || !svmove || !ppmove || !host_client || !sv_player || !sv_areanodes || !cmd_text || !cmd_alias || !host_frametime || !cvar_vars || !movevars || !ORIG_SV_AddLinksToPM || !ORIG_SV_SetMoveVars)
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
		FIND(Cmd_FindCmd)
		FIND(Cmd_Exists)
		FIND(Cbuf_AddText)
		FIND(Cbuf_InsertText)
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
		FIND(Cmd_ForwardToServer)
		FIND(MSG_WriteByte)
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

		ORIG_V_ApplyShake = reinterpret_cast<_V_ApplyShake>(MemUtils::GetSymbolAddress(m_Handle, "V_ApplyShake"));
		if (ORIG_V_ApplyShake)
			EngineDevMsg("[hw dll] Found V_ApplyShake at %p.\n", ORIG_V_ApplyShake);
		else
			EngineDevWarning("[hw dll] Could not find V_ApplyShake.\n");

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

		ORIG_R_PreDrawViewModel = reinterpret_cast<_R_PreDrawViewModel>(MemUtils::GetSymbolAddress(m_Handle, "R_PreDrawViewModel"));
		if (ORIG_R_PreDrawViewModel)
			EngineDevMsg("[hw dll] Found R_PreDrawViewModel at %p.\n", ORIG_R_PreDrawViewModel);
		else
			EngineDevWarning("[hw dll] Could not find R_PreDrawViewModel.\n");

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

		ORIG_R_DrawWorld = reinterpret_cast<_R_DrawWorld>(MemUtils::GetSymbolAddress(m_Handle, "R_DrawWorld"));
		if (ORIG_R_DrawWorld)
			EngineDevMsg("[hw dll] Found R_DrawWorld at %p.\n", ORIG_R_DrawWorld);
		else
			EngineDevWarning("[hw dll] Could not find R_DrawWorld.\n");

		ORIG_R_DrawParticles = reinterpret_cast<_R_DrawParticles>(MemUtils::GetSymbolAddress(m_Handle, "R_DrawParticles"));
		if (ORIG_R_DrawParticles)
			EngineDevMsg("[hw dll] Found R_DrawParticles at %p.\n", ORIG_R_DrawParticles);
		else
			EngineDevWarning("[hw dll] Could not find R_DrawParticles.\n");

		ORIG_R_StudioRenderModel = reinterpret_cast<_R_StudioRenderModel>(MemUtils::GetSymbolAddress(m_Handle, "R_StudioRenderModel"));
		if (ORIG_R_StudioRenderModel) {
			EngineDevMsg("[hw dll] Found R_StudioRenderModel at %p.\n", ORIG_R_StudioRenderModel);
		} else {
			EngineDevWarning("[hw dll] Could not find R_StudioRenderModel.\n");
			EngineWarning("Changing weapon viewmodel opacity is not available.\n");
		}

		scr_fov_value = reinterpret_cast<float*>(MemUtils::GetSymbolAddress(m_Handle, "scr_fov_value"));
		if (scr_fov_value)
			EngineDevMsg("[hw dll] Found scr_fov_value at %p.\n", scr_fov_value);
		else
			EngineDevWarning("[hw dll] Could not find scr_fov_value.\n");

		ORIG_R_SetFrustum = reinterpret_cast<_R_SetFrustum>(MemUtils::GetSymbolAddress(m_Handle, "R_SetFrustum"));
		if (ORIG_R_SetFrustum) {
			EngineDevMsg("[hw dll] Found R_SetFrustum at %p.\n", ORIG_R_SetFrustum);
		} else {
			EngineDevWarning("[hw dll] Could not find R_SetFrustum.\n");
		}
	}
	else
	{
		#define DEF_FUTURE(name) auto f##name = FindAsync(ORIG_##name, patterns::engine::name);
		DEF_FUTURE(Cvar_DirectSet)
		DEF_FUTURE(Cvar_FindVar)
		DEF_FUTURE(Cmd_FindCmd)
		DEF_FUTURE(Cmd_Exists)
		DEF_FUTURE(Host_Noclip_f)
		DEF_FUTURE(Host_Notarget_f)
		DEF_FUTURE(Cbuf_InsertText)
		DEF_FUTURE(Cbuf_AddText)
		DEF_FUTURE(Cmd_AddMallocCommand)
		DEF_FUTURE(Cmd_AddHUDCommand)
		//DEF_FUTURE(RandomFloat)
		DEF_FUTURE(RandomLong)
		DEF_FUTURE(SCR_BeginLoadingPlaque)
		DEF_FUTURE(PM_PlayerTrace)
		DEF_FUTURE(Host_FilterTime)
		DEF_FUTURE(V_FadeAlpha)
		DEF_FUTURE(V_ApplyShake)
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
		DEF_FUTURE(R_PreDrawViewModel)
		DEF_FUTURE(Mod_LeafPVS)
		DEF_FUTURE(CL_RecordHUDCommand)
		DEF_FUTURE(CL_Record_f)
		DEF_FUTURE(Key_Event)
		DEF_FUTURE(SV_AddLinksToPM_)
		DEF_FUTURE(SV_WriteEntitiesToClient)
		DEF_FUTURE(VGuiWrap_Paint)
		DEF_FUTURE(DispatchDirectUserMsg)
		DEF_FUTURE(EmitWaterPolys)
		DEF_FUTURE(S_StartDynamicSound)
		DEF_FUTURE(VGuiWrap2_NotifyOfServerConnect)
		DEF_FUTURE(CBaseUI__HideGameUI)
		DEF_FUTURE(R_DrawWorld)
		DEF_FUTURE(R_DrawParticles)
		DEF_FUTURE(BUsesSDLInput)
		DEF_FUTURE(R_StudioRenderModel)
		DEF_FUTURE(SPR_Set)
		DEF_FUTURE(DrawCrosshair)
		DEF_FUTURE(Draw_FillRGBA)
		DEF_FUTURE(PF_traceline_DLL)
		DEF_FUTURE(CL_CheckGameDirectory)
		DEF_FUTURE(SaveGameSlot)
		DEF_FUTURE(CL_HudMessage)
		DEF_FUTURE(SCR_NetGraph)
		DEF_FUTURE(VGuiWrap2_IsGameUIVisible)
		DEF_FUTURE(Host_Shutdown)
		DEF_FUTURE(ReleaseEntityDlls)
		DEF_FUTURE(Cvar_Command)
		DEF_FUTURE(Cvar_CommandWithPrivilegeCheck)
		DEF_FUTURE(R_ForceCvars)
		DEF_FUTURE(GL_BuildLightmaps)
		DEF_FUTURE(VID_FlipScreen)
		DEF_FUTURE(GL_EndRendering)
		DEF_FUTURE(SV_AddToFullPack)
		DEF_FUTURE(CL_EmitEntities)
		DEF_FUTURE(Draw_String)
		DEF_FUTURE(V_RenderView)
		DEF_FUTURE(LoadAdjacentEntities)
		DEF_FUTURE(PlayerMove)
		DEF_FUTURE(CL_Restore)
		DEF_FUTURE(Sequence_OnLevelLoad)
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

		void* ClientDLL_CheckStudioInterface;
		auto fClientDLL_CheckStudioInterface = FindAsync(
			ClientDLL_CheckStudioInterface,
			patterns::engine::ClientDLL_CheckStudioInterface,
			[&](auto pattern) {
				switch (pattern - patterns::engine::ClientDLL_CheckStudioInterface.cbegin())
				{
				default:
				case 0: // HL-Steampipe
					pEngStudio = *reinterpret_cast<engine_studio_api_t**>(reinterpret_cast<uintptr_t>(ClientDLL_CheckStudioInterface) + 42);
					break;
				case 1: // HL-4554
					pEngStudio = *reinterpret_cast<engine_studio_api_t**>(reinterpret_cast<uintptr_t>(ClientDLL_CheckStudioInterface) + 40);
					break;
				case 2: // HL-WON-1712
					pEngStudio = *reinterpret_cast<engine_studio_api_t**>(reinterpret_cast<uintptr_t>(ClientDLL_CheckStudioInterface) + 30);
					break;
				case 3: // CoF-5936
					pEngStudio = *reinterpret_cast<engine_studio_api_t**>(reinterpret_cast<uintptr_t>(ClientDLL_CheckStudioInterface) + 48);
					break;
				case 4: // Sven-v525
					pEngStudio = *reinterpret_cast<engine_studio_api_t**>(reinterpret_cast<uintptr_t>(ClientDLL_CheckStudioInterface) + 74);
					break;
				}
			});

		void* Sys_EngineAPI;
		auto fSys_EngineAPI = FindAsync(
			Sys_EngineAPI,
			patterns::engine::Sys_EngineAPI,
			[&](auto pattern) {
				switch (pattern - patterns::engine::Sys_EngineAPI.cbegin())
				{
				default:
				case 0: // HL-WON-1712
					pEngineAPI = *reinterpret_cast<engine_api_t**>(reinterpret_cast<uintptr_t>(Sys_EngineAPI) + 41);
					break;
				}
			});

		void* ClientDLL_Init;
		auto fClientDLL_Init = FindAsync(
			ClientDLL_Init,
			patterns::engine::ClientDLL_Init,
			[&](auto pattern) {
				switch (pattern - patterns::engine::ClientDLL_Init.cbegin())
				{
				default:
				case 0: // HL-Steampipe
					ClientDLL::GetInstance().pEngfuncs = *reinterpret_cast<cl_enginefunc_t**>(reinterpret_cast<uintptr_t>(ClientDLL_Init) + 181);
					is_steamid_build = true;
					is_steam_build = true;
					break;
				case 1: // HL-4554
					ClientDLL::GetInstance().pEngfuncs = *reinterpret_cast<cl_enginefunc_t**>(reinterpret_cast<uintptr_t>(ClientDLL_Init) + 226);
					is_steamid_build = true;
					is_steam_build = true;
					break;
				case 2: // HL-NGHL
					ClientDLL::GetInstance().pEngfuncs = *reinterpret_cast<cl_enginefunc_t**>(reinterpret_cast<uintptr_t>(ClientDLL_Init) + 203);
					is_steam_build = true;
					break;
				case 3: // HL-WON-1712
					ClientDLL::GetInstance().pEngfuncs = *reinterpret_cast<cl_enginefunc_t**>(reinterpret_cast<uintptr_t>(ClientDLL_Init) + 1456);
					is_won_build = true;
					is_1712_build = true;
					break;
				case 4: // CoF-5936
					ClientDLL::GetInstance().pEngfuncs = *reinterpret_cast<cl_enginefunc_t**>(reinterpret_cast<uintptr_t>(ClientDLL_Init) + 230);
					is_steamid_build = true;
					break;
				case 5: // Sven-v525
					ClientDLL::GetInstance().pEngfuncs = *reinterpret_cast<cl_enginefunc_t**>(reinterpret_cast<uintptr_t>(ClientDLL_Init) + 332);
					is_steamid_build = true;
					break;
				case 6: // HL-1202
					ClientDLL::GetInstance().pEngfuncs = *reinterpret_cast<cl_enginefunc_t**>(reinterpret_cast<uintptr_t>(ClientDLL_Init) + 352);
					is_won_build = true;
					break;
				case 7: // BShift-WON-1001
					ClientDLL::GetInstance().pEngfuncs = *reinterpret_cast<cl_enginefunc_t**>(reinterpret_cast<uintptr_t>(ClientDLL_Init) + 341);
					is_won_build = true;
					break;
				}
			});

		void* LoadThisDll;
		auto fLoadThisDll = FindAsync(
			LoadThisDll,
			patterns::engine::LoadThisDll,
			[&](auto pattern) {
				auto f = reinterpret_cast<uintptr_t>(LoadThisDll);
				switch (pattern - patterns::engine::LoadThisDll.cbegin())
				{
				default:
				case 0: // HL-Steampipe
					ServerDLL::GetInstance().pEngfuncs = *reinterpret_cast<enginefuncs_t**>(f + 95);
					ppGlobals = *reinterpret_cast<globalvars_t**>(f + 90);
					break;
				case 1: // HL-4554
					ServerDLL::GetInstance().pEngfuncs = *reinterpret_cast<enginefuncs_t**>(f + 91);
					ppGlobals = *reinterpret_cast<globalvars_t**>(f + 86);
					break;
				case 2: // HL-WON-1712
				case 6: // BShift-WON-1001
					ServerDLL::GetInstance().pEngfuncs = *reinterpret_cast<enginefuncs_t**>(f + 89);
					ppGlobals = *reinterpret_cast<globalvars_t**>(f + 84);
					break;
				case 3: // CoF-5936
					ServerDLL::GetInstance().pEngfuncs = *reinterpret_cast<enginefuncs_t**>(f + 118);
					ppGlobals = *reinterpret_cast<globalvars_t**>(f + 113);
					break;
				case 4: // Sven-v525
					ServerDLL::GetInstance().pEngfuncs = reinterpret_cast<enginefuncs_t*>(*reinterpret_cast<uintptr_t*>(f + 109) + 0x8);
					ppGlobals = *reinterpret_cast<globalvars_t**>(f + 103);
					break;
				case 5: // HL-1202
					ServerDLL::GetInstance().pEngfuncs = *reinterpret_cast<enginefuncs_t**>(f + 80);
					ppGlobals = *reinterpret_cast<globalvars_t**>(f + 75);
					break;
				}
			});

		auto fHost_FilterTime_FPS_Cap_Byte = FindAsync(
			pHost_FilterTime_FPS_Cap_Byte,
			patterns::engine::Host_FilterTime_FPS_Cap_Byte,
			[&](auto pattern) {
				switch (pattern - patterns::engine::Host_FilterTime_FPS_Cap_Byte.cbegin()) {
				case 0: // HL-SteamPipe
				case 1: // HL-4554
					pHost_FilterTime_FPS_Cap_Byte += 7;
					break;
				case 2: // HL-WON-1712
					pHost_FilterTime_FPS_Cap_Byte += 19;
					break;
				case 3: // Sven-v525
					pHost_FilterTime_FPS_Cap_Byte += 5;
					break;
				default:
					assert(false);
				}
			});

		auto fHost_FilterTime_Check_72FPS = FindAsync(
			pHost_FilterTime_Check_72FPS,
			patterns::engine::Host_FilterTime_Check_72FPS,
			[&](auto pattern) {
				switch (pattern - patterns::engine::Host_FilterTime_Check_72FPS.cbegin()) {
				case 0: // HL-1202
					pHost_FilterTime_Check_72FPS += 3;
					break;
				case 1: // BShift-WON-1001
					pHost_FilterTime_Check_72FPS += 4;
					break;
				default:
					assert(false);
				}
			});

		auto fHost_FilterTime_Set_72FPS = FindAsync(
			pHost_FilterTime_Set_72FPS,
			patterns::engine::Host_FilterTime_Set_72FPS,
			[&](auto pattern) {
				switch (pattern - patterns::engine::Host_FilterTime_Set_72FPS.cbegin()) {
				case 0: // HL-1202
					pHost_FilterTime_Set_72FPS += 4;
					break;
				case 1: // BShift-WON-1001
					pHost_FilterTime_Set_72FPS += 1;
					is_bshift_won = true;
					break;
				default:
					assert(false);
				}
			});

		auto fGL_EndRendering_VID_FlipScreen_Bytes = FindAsync(
			pGL_EndRendering_VID_FlipScreen_Bytes,
			patterns::engine::GL_EndRendering_VID_FlipScreen_Bytes,
			[&](auto pattern) {
				switch (pattern - patterns::engine::GL_EndRendering_VID_FlipScreen_Bytes.cbegin()) {
				case 0: // HL-SteamPipe
					pGL_EndRendering_VID_FlipScreen_Bytes += 1;
					break;
				default:
					assert(false);
				}
			});

		auto fCbuf_Execute = FindAsync(
			ORIG_Cbuf_Execute,
			patterns::engine::Cbuf_Execute,
			[&](auto pattern) {
				switch (pattern - patterns::engine::Cbuf_Execute.cbegin())
				{
				case 0: // HL-SteamPipe-8183
				case 3: // HL-SteamPipe-8308
					cmd_text = reinterpret_cast<sizebuf_t*>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cbuf_Execute) + 3));
					break;
				case 1: // HL-SteamPipe
					cmd_text = reinterpret_cast<sizebuf_t*>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cbuf_Execute) + 11) - offsetof(sizebuf_t, cursize));
					break;
				case 2: // HL-NGHL
				case 7: // BShift-WON-1001
					cmd_text = reinterpret_cast<sizebuf_t*>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cbuf_Execute) + 2) - offsetof(sizebuf_t, cursize));
					break;
				case 4: // CoF-5936
					cmd_text = reinterpret_cast<sizebuf_t*>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cbuf_Execute) + 21) - offsetof(sizebuf_t, cursize));
					break;
				case 5: // Sven-v525
					cmd_text = reinterpret_cast<sizebuf_t*>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cbuf_Execute) + 1));
					break;
				case 6: // HL-1202
					cmd_text = reinterpret_cast<sizebuf_t*>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cbuf_Execute) + 13) - offsetof(sizebuf_t, cursize));
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
				case 5: // BShift-WON-1001
					cvar_vars = reinterpret_cast<cvar_t**>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cvar_RegisterVariable) + 124));
					break;
				case 1: // HL-NGHL
				case 4: // HL-1202
					cvar_vars = reinterpret_cast<cvar_t**>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cvar_RegisterVariable) + 122));
					break;
				case 2: // CoF-5936
					cvar_vars = reinterpret_cast<cvar_t**>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cvar_RegisterVariable) + 183));
					break;
				case 3: // Sven-v525
					cvar_vars = reinterpret_cast<cvar_t**>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cvar_RegisterVariable) + 23));
					break;
				}
			});

		auto fR_SetFrustum = FindAsync(
			ORIG_R_SetFrustum,
			patterns::engine::R_SetFrustum,
			[&](auto pattern) {
				switch (pattern - patterns::engine::R_SetFrustum.cbegin())
				{
				case 0: // HL-SteamPipe
					scr_fov_value = reinterpret_cast<float*>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_R_SetFrustum) + 13));
					break;
				case 1: // HL-4554
				case 3: // HL-1202
				case 4: // BShift-WON-1001
					scr_fov_value = reinterpret_cast<float*>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_R_SetFrustum) + 10));
					break;
				case 2: // CoF-5936
					scr_fov_value = reinterpret_cast<float*>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_R_SetFrustum) + 7));
					break;
				}
			});

		auto fSCR_DrawPause = FindAsync(
			ORIG_SCR_DrawPause,
			patterns::engine::SCR_DrawPause,
			[&](auto pattern) {
				switch (pattern - patterns::engine::SCR_DrawPause.cbegin())
				{
				case 0: // HL-Steampipe
				case 1: // HL-4554
				case 2: // HL-NGHL
					cl_paused = reinterpret_cast<int*>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_SCR_DrawPause) + 0x14));
					break;
				case 3: // CoF-5936
					cl_paused = reinterpret_cast<int*>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_SCR_DrawPause) + 0x1A));
					break;
				case 4: // Sven-v525
					cl_paused = reinterpret_cast<int*>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_SCR_DrawPause) + 0x3D));
					break;
				case 5: // HL-1202
					cl_paused = reinterpret_cast<int*>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_SCR_DrawPause) + 0x15));
					break;
				}
			});

		auto fSeedRandomNumberGenerator = FindAsync(
			ORIG_SeedRandomNumberGenerator,
			patterns::engine::SeedRandomNumberGenerator,
			[&](auto pattern) {
				switch (pattern - patterns::engine::SeedRandomNumberGenerator.cbegin())
				{
				case 0: // HL-SteamPipe
				case 2: // Sven-v525
				case 4: // HL-1202
					ORIG_time = reinterpret_cast<_time>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_SeedRandomNumberGenerator) + 3)
						+ reinterpret_cast<uintptr_t>(ORIG_SeedRandomNumberGenerator) + 7);
					break;
				case 1: // CoF-5936
					ORIG_time = reinterpret_cast<_time>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_SeedRandomNumberGenerator) + 6)
						+ reinterpret_cast<uintptr_t>(ORIG_SeedRandomNumberGenerator) + 10);
					break;
				}
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
				case 2: // CoF-5936
				case 4: // HL-1202
					offset = 25;
					break;
				case 1: // NGHL
					offset = 22;
					break;
				case 3: // Sven-v525
					offset = 35;
					break;
				}

				demorecording = *reinterpret_cast<int**>(reinterpret_cast<uintptr_t>(ORIG_CL_Stop_f) + offset);
				demoplayback = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(demorecording) + 0x4);
			});

		void *EndOfHost_FilterTime;
		auto fEndOfHost_FilterTime = FindAsync(
			EndOfHost_FilterTime,
			patterns::engine::EndOfHost_FilterTime,
			[&](auto pattern) {
				switch (pattern - patterns::engine::EndOfHost_FilterTime.cbegin())
				{
				default:
				case 0: // HL-Steampipe
					host_frametime = *reinterpret_cast<double**>(reinterpret_cast<uintptr_t>(EndOfHost_FilterTime) + 17);
					break;
				case 1: // HL-4554
				case 4: // BShift-WON-1001
					host_frametime = *reinterpret_cast<double**>(reinterpret_cast<uintptr_t>(EndOfHost_FilterTime) + 15);
					break;
				case 2: // Sven-v525
					host_frametime = *reinterpret_cast<double**>(reinterpret_cast<uintptr_t>(EndOfHost_FilterTime) + 11);
					break;
				case 3: // HL-1202
					host_frametime = *reinterpret_cast<double**>(reinterpret_cast<uintptr_t>(EndOfHost_FilterTime) + 16);
					break;
				}
			});

		void *CL_Move;
		auto fCL_Move = FindAsync(
			CL_Move,
			patterns::engine::CL_Move,
			[&](auto pattern) {
				switch (pattern - patterns::engine::CL_Move.cbegin())
				{
				default:
				case 0: // HL-Steampipe
					frametime_remainder = *reinterpret_cast<double**>(reinterpret_cast<uintptr_t>(CL_Move) + 451);
					break;
				case 1: // Sven-v525
					frametime_remainder = *reinterpret_cast<double**>(reinterpret_cast<uintptr_t>(CL_Move) + 581);
					break;
				}
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
					break;
				case 4: // CoF-5936
					offCmd_Argc = 26;
					offCmd_Args = 41;
					offCmd_Argv = 180;
					break;
				case 5: // Sven-v525
					offCmd_Argc = 44;
					offCmd_Args = 59;
					offCmd_Argv = 163;
					break;
				case 6: // HL-1202
					offCmd_Argc = 32;
					offCmd_Args = 88;
					offCmd_Argv = 280;
					break;
				case 7: // BShift-WON-1001
					offCmd_Argc = 25;
					offCmd_Args = 78;
					offCmd_Argv = 281;
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

		auto fHost_ValidSave = FindAsync(
			ORIG_Host_ValidSave,
			patterns::engine::Host_ValidSave,
			[&](auto pattern) {
				auto f = reinterpret_cast<uintptr_t>(ORIG_Host_ValidSave);
				switch (pattern - patterns::engine::Host_ValidSave.cbegin())
				{
				default:
				case 0: // HL-Steampipe
					psv = *reinterpret_cast<void**>(f + 19);
					offTime = 0x10;
					offName = 0x30;
					ORIG_Con_Printf = reinterpret_cast<_Con_Printf>(
						*reinterpret_cast<ptrdiff_t*>(f + 33)
						+ (f + 37)
						);
					cls = *reinterpret_cast<void**>(f + 69);
					svs = reinterpret_cast<server_static_t*>(*reinterpret_cast<uintptr_t*>(f + 45) - 8);
					offEdict = *reinterpret_cast<ptrdiff_t*>(f + 122);
					offActiveAddr = *reinterpret_cast<uintptr_t*>(f + 0x13);
					break;
				case 1: // CoF-5936
					psv = *reinterpret_cast<void**>(f + 50);
					offTime = 0x10;
					offName = 0x30;
					ORIG_Con_Printf = reinterpret_cast<_Con_Printf>(
						*reinterpret_cast<ptrdiff_t*>(f + 63)
						+ (f + 67)
						);
					cls = *reinterpret_cast<void**>(f + 105);
					svs = reinterpret_cast<server_static_t*>(*reinterpret_cast<uintptr_t*>(f + 79) - 8);
					offEdict = *reinterpret_cast<ptrdiff_t*>(f + 182);
					offActiveAddr = *reinterpret_cast<uintptr_t*>(f + 0x32);
					cofSaveHack = *reinterpret_cast<qboolean**>(f + 21);
					is_cof_steam = true;
					break;
				case 2: // Sven-v525
					psv = *reinterpret_cast<void**>(f + 15);
					offTime = 0x10;
					offName = 0x30;
					ORIG_Con_Printf = reinterpret_cast<_Con_Printf>(
						*reinterpret_cast<ptrdiff_t*>(f + 28)
						+ (f + 32)
						);
					cls = *reinterpret_cast<void**>(f + 65);
					svs = reinterpret_cast<server_static_t*>(*reinterpret_cast<uintptr_t*>(f + 40) - 8);
					offEdict = *reinterpret_cast<ptrdiff_t*>(f + 118);
					offActiveAddr = *reinterpret_cast<uintptr_t*>(f + 0xF);
					is_sven_525 = true;
					break;
				case 3: // HL-1202
					psv = *reinterpret_cast<void**>(f + 14);
					offTime = 0x10;
					offName = 0x28;
					ORIG_Con_Printf = reinterpret_cast<_Con_Printf>(
						*reinterpret_cast<ptrdiff_t*>(f + 28)
						+ (f + 32)
						);
					cls = *reinterpret_cast<void**>(f + 48);
					svs = reinterpret_cast<server_static_t*>(*reinterpret_cast<uintptr_t*>(f + 58) - 0);
					offEdict = *reinterpret_cast<ptrdiff_t*>(f + 95);
					offActiveAddr = *reinterpret_cast<uintptr_t*>(f + 0xE);
					is_sdk10 = true;
					break;
				case 4: // BShift-WON-1001
					psv = *reinterpret_cast<void**>(f + 19);
					offTime = 0x10;
					offName = 0x28;
					ORIG_Con_Printf = reinterpret_cast<_Con_Printf>(
						*reinterpret_cast<ptrdiff_t*>(f + 33)
						+ (f + 37)
						);
					cls = *reinterpret_cast<void**>(f + 54);
					svs = reinterpret_cast<server_static_t*>(*reinterpret_cast<uintptr_t*>(f + 63) - 0);
					offEdict = *reinterpret_cast<ptrdiff_t*>(f + 97);
					offActiveAddr = *reinterpret_cast<uintptr_t*>(f + 0x13);
					is_sdk10 = true;
					break;
				}
			});

		void* NUM_FOR_EDICT;
		auto fNUM_FOR_EDICT = FindAsync(
			NUM_FOR_EDICT,
			patterns::engine::NUM_FOR_EDICT,
			[&](auto pattern) {
				auto f = reinterpret_cast<uintptr_t>(NUM_FOR_EDICT);
				if (offActiveAddr)
				{
					switch (pattern - patterns::engine::NUM_FOR_EDICT.cbegin())
					{
					default:
					case 0: // HL-Steampipe
					case 4: // HL-1202
						offEdicts = *reinterpret_cast<uintptr_t*>(f + 8) - offActiveAddr;
						break;
					case 1: // HL-4554
						offEdicts = *reinterpret_cast<uintptr_t*>(f + 6) - offActiveAddr;
						break;
					case 2: // CoF-5936
						offEdicts = *reinterpret_cast<uintptr_t*>(f + 9) - offActiveAddr;
						break;
					case 3: // Sven-v525
						offEdicts = *reinterpret_cast<uintptr_t*>(f + 0xB) - offActiveAddr;
						break;
					}
				}
				else
				{
					if (is_steam_build)
						offEdicts = 0x3BC60;
					else if (is_1712_build)
						offEdicts = 0x3BA20;
					else if (is_sdk10)
						offEdicts = 0x23204;
					else if (is_cof_steam)
						offEdicts = 0x52160;
					else if (is_sven_525)
						offEdicts = 0x4A1560;
				}
			});

		void* CL_EntityNum;
		auto fCL_EntityNum = FindAsync(
			CL_EntityNum,
			patterns::engine::CL_EntityNum,
			[&](auto pattern) {
				auto f = reinterpret_cast<uintptr_t>(CL_EntityNum);
				switch (pattern - patterns::engine::CL_EntityNum.cbegin())
				{
				default:
				case 0: // HL-Steampipe
					pcl = *reinterpret_cast<void**>(f + 0x12);
					offMaxEdictsAddr = *reinterpret_cast<uintptr_t*>(f + 0x12);
					cl_entities = *reinterpret_cast<cl_entity_t***>(reinterpret_cast<uintptr_t>(CL_EntityNum) + 56);
					break;
				case 1: // HL-4554
					pcl = *reinterpret_cast<void**>(f + 0x10);
					offMaxEdictsAddr = *reinterpret_cast<uintptr_t*>(f + 0x10);
					cl_entities = *reinterpret_cast<cl_entity_t***>(reinterpret_cast<uintptr_t>(CL_EntityNum) + 54);
					break;
				case 2: // CoF-5936
					pcl = *reinterpret_cast<void**>(f + 0x13);
					offMaxEdictsAddr = *reinterpret_cast<uintptr_t*>(f + 0x13);
					cl_entities = *reinterpret_cast<cl_entity_t***>(reinterpret_cast<uintptr_t>(CL_EntityNum) + 108);
					break;
				case 3: // Sven-v525
					pcl = *reinterpret_cast<void**>(f + 0x10);
					offMaxEdictsAddr = *reinterpret_cast<uintptr_t*>(f + 0x10);
					cl_entities = *reinterpret_cast<cl_entity_t***>(reinterpret_cast<uintptr_t>(CL_EntityNum) + 65);
					break;
				case 4: // HL-1202
					pcl = *reinterpret_cast<void**>(f + 0xF);
					offMaxEdictsAddr = *reinterpret_cast<uintptr_t*>(f + 0xF);
					cl_entities = *reinterpret_cast<cl_entity_t***>(reinterpret_cast<uintptr_t>(CL_EntityNum) + 66);
					break;
				case 5: // BShift-WON-1001
					pcl = *reinterpret_cast<void**>(f + 0x10);
					offMaxEdictsAddr = *reinterpret_cast<uintptr_t*>(f + 0x10);
					cl_entities = *reinterpret_cast<cl_entity_t***>(reinterpret_cast<uintptr_t>(CL_EntityNum) + 60);
					break;
				}
			});

		void* CL_FxBlend;
		auto fCL_FxBlend = FindAsync(
			CL_FxBlend,
			patterns::engine::CL_FxBlend,
			[&](auto pattern) {
				switch (pattern - patterns::engine::CL_FxBlend.cbegin())
				{
				default:
				case 0: // HL-SteamPipe
					cl_time = *reinterpret_cast<double**>(reinterpret_cast<uintptr_t>(CL_FxBlend) + 0x3a);
					break;
				case 1: // HL-4554
				case 4: // HL-1202
					cl_time = *reinterpret_cast<double**>(reinterpret_cast<uintptr_t>(CL_FxBlend) + 0x40);
					break;
				case 2: // CoF-5936
					cl_time = *reinterpret_cast<double**>(reinterpret_cast<uintptr_t>(CL_FxBlend) + 0x4f);
					break;
				case 3: // Sven-v525
					cl_time = *reinterpret_cast<double**>(reinterpret_cast<uintptr_t>(CL_FxBlend) + 0x2a);
					break;
				case 5: // BShift-WON-1001
					cl_time = *reinterpret_cast<double**>(reinterpret_cast<uintptr_t>(CL_FxBlend) + 0x2f);
					break;
				}
			});

		void* ModelFrames;
		auto fModelFrames = FindAsync(
			ModelFrames,
			patterns::engine::ModelFrames,
			[&](auto pattern) {
				auto f = reinterpret_cast<uintptr_t>(ModelFrames);
				if (offActiveAddr)
				{
					switch (pattern - patterns::engine::ModelFrames.cbegin())
					{
					default:
					case 0: // HL-Steampipe
					case 3: // Sven-v525
						offModels = *reinterpret_cast<uintptr_t*>(f + 0x14) - offActiveAddr;
						break;
					case 1: // HL-4554
						offModels = *reinterpret_cast<uintptr_t*>(f + 0x12) - offActiveAddr;
						break;
					case 2: // CoF-5936
						offModels = *reinterpret_cast<uintptr_t*>(f + 0x19) - offActiveAddr;
						break;
					}
				}
				else
				{
					if (is_steam_build)
						offModels = 0x30950;
					else if (is_1712_build)
						offModels = 0x30910;
					else if (is_sdk10)
						offModels = 0x210F8;
					else if (is_cof_steam)
						offModels = 0x41D50;
					else if (is_sven_525)
						offModels = 0x276150;
				}
			});

		void* MiddleOfHost_SavegameComment;
		auto fMiddleOfHost_SavegameComment = FindAsync(
			MiddleOfHost_SavegameComment,
			patterns::engine::MiddleOfHost_SavegameComment,
			[&](auto pattern) {
				switch (pattern - patterns::engine::MiddleOfHost_SavegameComment.cbegin())
				{
				default:
				case 0: // HL-Steampipe
				case 1: // HL-4554
				case 2: // CoF-5936
				case 3: // Sven-v525
				case 4: // HL-1202
				case 5: // BShift-WON-1001
					levelname = *reinterpret_cast<char**>(reinterpret_cast<uintptr_t>(MiddleOfHost_SavegameComment) + 1);
					break;
				}
			});

		void* MiddleOfCL_PredictMove;
		auto fMiddleOfCL_PredictMove = FindAsync(
			MiddleOfCL_PredictMove,
			patterns::engine::MiddleOfCL_PredictMove,
			[&](auto pattern) {
				switch (pattern - patterns::engine::MiddleOfCL_PredictMove.cbegin())
				{
				default:
				case 0: // HL-Steampipe
				case 1: // HL-WON-1712
					simorg = *reinterpret_cast<Vector**>(reinterpret_cast<uintptr_t>(MiddleOfCL_PredictMove) + 1);
					simvel = *reinterpret_cast<Vector**>(reinterpret_cast<uintptr_t>(MiddleOfCL_PredictMove) + 0x17);
					break;
				case 2: // CoF-5936
					simorg = *reinterpret_cast<Vector**>(reinterpret_cast<uintptr_t>(MiddleOfCL_PredictMove) + 1);
					simvel = *reinterpret_cast<Vector**>(reinterpret_cast<uintptr_t>(MiddleOfCL_PredictMove) + 0x20);
					break;
				case 3: // Sven-v525
					simorg = *reinterpret_cast<Vector**>(reinterpret_cast<uintptr_t>(MiddleOfCL_PredictMove) + 1);
					simvel = *reinterpret_cast<Vector**>(reinterpret_cast<uintptr_t>(MiddleOfCL_PredictMove) + 0x21);
					break;
				case 4: // HL-1202
					simorg = *reinterpret_cast<Vector**>(reinterpret_cast<uintptr_t>(MiddleOfCL_PredictMove) + 0x1A);
					simvel = *reinterpret_cast<Vector**>(reinterpret_cast<uintptr_t>(MiddleOfCL_PredictMove) + 1);
					break;
				case 5: // BShift-WON-1001
					simorg = *reinterpret_cast<Vector**>(reinterpret_cast<uintptr_t>(MiddleOfCL_PredictMove) + 0x20);
					simvel = *reinterpret_cast<Vector**>(reinterpret_cast<uintptr_t>(MiddleOfCL_PredictMove) + 1);
					break;
				}
			});

		void* MiddleOfR_Envmap_f;
		auto fMiddleOfR_Envmap_f = FindAsync(
			MiddleOfR_Envmap_f,
			patterns::engine::MiddleOfR_Envmap_f,
			[&](auto pattern) {
				switch (pattern - patterns::engine::MiddleOfR_Envmap_f.cbegin())
				{
				default:
				case 0: // HL-Steampipe
				case 1: // CoF-5936
				case 2: // Sven-v525
				case 3: // HL-1202
				case 4: // BShift-WON-1001
					r_refdef = *reinterpret_cast<refdef_t**>(reinterpret_cast<uintptr_t>(MiddleOfR_Envmap_f) + 2);
					break;
				}
			});

		void* Host_TogglePause_f;
		auto fHost_TogglePause_f = FindAsync(
			Host_TogglePause_f,
			patterns::engine::Host_TogglePause_f,
			[&](auto pattern) {
				switch (pattern - patterns::engine::Host_TogglePause_f.cbegin())
				{
				default:
				case 0: // HL-1202
					cmd_source = *reinterpret_cast<cmd_source_t**>(reinterpret_cast<uintptr_t>(Host_TogglePause_f) + 1);
					ORIG_Cmd_ForwardToServer = reinterpret_cast<_Cmd_ForwardToServer>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(Host_TogglePause_f) + 11)
						+ reinterpret_cast<uintptr_t>(Host_TogglePause_f) + 15);
					reliable_datagram = *reinterpret_cast<sizebuf_t**>(reinterpret_cast<uintptr_t>(Host_TogglePause_f) + 110);
					ORIG_MSG_WriteByte = reinterpret_cast<_MSG_WriteByte>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(Host_TogglePause_f) + 115)
						+ reinterpret_cast<uintptr_t>(Host_TogglePause_f) + 119);
					break;
				case 1: // BShift-WON-1001
					cmd_source = *reinterpret_cast<cmd_source_t**>(reinterpret_cast<uintptr_t>(Host_TogglePause_f) + 1);
					ORIG_Cmd_ForwardToServer = reinterpret_cast<_Cmd_ForwardToServer>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(Host_TogglePause_f) + 15)
						+ reinterpret_cast<uintptr_t>(Host_TogglePause_f) + 19);
					reliable_datagram = *reinterpret_cast<sizebuf_t**>(reinterpret_cast<uintptr_t>(Host_TogglePause_f) + 129);
					ORIG_MSG_WriteByte = reinterpret_cast<_MSG_WriteByte>(*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(Host_TogglePause_f) + 134)
						+ reinterpret_cast<uintptr_t>(Host_TogglePause_f) + 138);
					break;
				}
			});

		void* Chase_Update;
		auto fChase_Update = FindAsync(
			Chase_Update,
			patterns::engine::Chase_Update,
			[&](auto pattern) {
				switch (pattern - patterns::engine::Chase_Update.cbegin())
				{
				default:
				case 0: // HL-Steampipe
				case 1: // HL-NGHL
				case 2: // CoF-5936
				case 4: // BShift-WON-1001
					viewangles = *reinterpret_cast<Vector**>(reinterpret_cast<uintptr_t>(Chase_Update) + 0x13);
					break;
				case 3: // Sven-v525
					viewangles = *reinterpret_cast<Vector**>(reinterpret_cast<uintptr_t>(Chase_Update) + 0x1E);
					break;
				}
			});

		void* MiddleOfV_RenderView;
		auto fMiddleOfV_RenderView = FindAsync(
			MiddleOfV_RenderView,
			patterns::engine::MiddleOfV_RenderView,
			[&](auto pattern) {
				switch (pattern - patterns::engine::MiddleOfV_RenderView.cbegin())
				{
				default:
				case 0: // HL-Steampipe
					playernum = *reinterpret_cast<int**>(reinterpret_cast<uintptr_t>(MiddleOfV_RenderView) + 0x2);
					viewent = *reinterpret_cast<cl_entity_t**>(reinterpret_cast<uintptr_t>(MiddleOfV_RenderView) + 0x2D);
					break;
				case 1: // HL-4554
					playernum = *reinterpret_cast<int**>(reinterpret_cast<uintptr_t>(MiddleOfV_RenderView) + 0x2);
					viewent = *reinterpret_cast<cl_entity_t**>(reinterpret_cast<uintptr_t>(MiddleOfV_RenderView) + 0x2B);
					break;
				case 2: // HL-WON-1712
					playernum = *reinterpret_cast<int**>(reinterpret_cast<uintptr_t>(MiddleOfV_RenderView) + 0x2);
					viewent = *reinterpret_cast<cl_entity_t**>(reinterpret_cast<uintptr_t>(MiddleOfV_RenderView) + 0x34);
					break;
				case 3: // CoF-5936
					playernum = *reinterpret_cast<int**>(reinterpret_cast<uintptr_t>(MiddleOfV_RenderView) + 0x4A); // It really has the reverse order, it's not a typo!
					viewent = *reinterpret_cast<cl_entity_t**>(reinterpret_cast<uintptr_t>(MiddleOfV_RenderView) + 0x2);
					break;
				case 4: // Sven-v525
					playernum = *reinterpret_cast<int**>(reinterpret_cast<uintptr_t>(MiddleOfV_RenderView) + 0x1);
					viewent = *reinterpret_cast<cl_entity_t**>(reinterpret_cast<uintptr_t>(MiddleOfV_RenderView) + 0x13);
					break;
				case 5: // HL-1202
					playernum = *reinterpret_cast<int**>(reinterpret_cast<uintptr_t>(MiddleOfV_RenderView) + 0x1); // In reality that is middle of V_CalcRefDef function
					viewent = *reinterpret_cast<cl_entity_t**>(reinterpret_cast<uintptr_t>(MiddleOfV_RenderView) + 0x7);
					break;
				case 6: // BShift-WON-1001
					playernum = *reinterpret_cast<int**>(reinterpret_cast<uintptr_t>(MiddleOfV_RenderView) + 0x2); // In reality that is middle of V_CalcRefDef function
					viewent = *reinterpret_cast<cl_entity_t**>(reinterpret_cast<uintptr_t>(MiddleOfV_RenderView) + 0x43);
					break;
				}
			});

		void *MiddleOfSV_ExecuteClientMessage;
		auto fMiddleOfSV_ExecuteClientMessage = FindAsync(
			MiddleOfSV_ExecuteClientMessage,
			patterns::engine::MiddleOfSV_ExecuteClientMessage,
			[&](auto pattern) {
				switch (pattern - patterns::engine::MiddleOfSV_ExecuteClientMessage.cbegin())
				{
				default:
				case 0: // SteamPipe & NGHL.
					host_client = *reinterpret_cast<client_t***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ExecuteClientMessage) + 14);
					svmove = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(MiddleOfSV_ExecuteClientMessage) + 39);
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ExecuteClientMessage) + 35);
					sv_player = *reinterpret_cast<edict_t***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ExecuteClientMessage) + 19);
					break;
				case 1: // 4554.
					host_client = *reinterpret_cast<client_t***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ExecuteClientMessage) + 17);
					svmove = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(MiddleOfSV_ExecuteClientMessage) + 42);
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ExecuteClientMessage) + 38);
					sv_player = *reinterpret_cast<edict_t***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ExecuteClientMessage) + 22);
					break;
				case 2: // WON.
					host_client = *reinterpret_cast<client_t***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ExecuteClientMessage) + 14);
					svmove = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(MiddleOfSV_ExecuteClientMessage) + 40);
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ExecuteClientMessage) + 36);
					sv_player = *reinterpret_cast<edict_t***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ExecuteClientMessage) + 20);
					break;
				case 3: // WON-1712.
					host_client = *reinterpret_cast<client_t***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ExecuteClientMessage) + 17);
					svmove = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(MiddleOfSV_ExecuteClientMessage) + 43);
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ExecuteClientMessage) + 39);
					sv_player = *reinterpret_cast<edict_t***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ExecuteClientMessage) + 23);
					break;
				case 4: // CoF-5936.
					host_client = *reinterpret_cast<client_t***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ExecuteClientMessage) + 17);
					svmove = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(MiddleOfSV_ExecuteClientMessage) + 57);
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ExecuteClientMessage) + 53);
					sv_player = *reinterpret_cast<edict_t***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ExecuteClientMessage) + 34);
					break;
				case 5: // Sven-v525.
					host_client = *reinterpret_cast<client_t***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ExecuteClientMessage) + 8);
					svmove = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(MiddleOfSV_ExecuteClientMessage) + 48);
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ExecuteClientMessage) + 44);
					sv_player = *reinterpret_cast<edict_t***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ExecuteClientMessage) + 20);
					break;
				case 6: // HL-1202
					host_client = *reinterpret_cast<client_t***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ExecuteClientMessage) + 2);
					sv_player = *reinterpret_cast<edict_t***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ExecuteClientMessage) + 20);
					break;
				case 7: // BShift-WON-1001
					host_client = *reinterpret_cast<client_t***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ExecuteClientMessage) + 2);
					sv_player = *reinterpret_cast<edict_t***>(reinterpret_cast<uintptr_t>(MiddleOfSV_ExecuteClientMessage) + 13);
					break;
				}
			});

		void *MiddleOfSV_RunCmd;
		auto fMiddleOfSV_RunCmd = FindAsync(
			MiddleOfSV_RunCmd,
			patterns::engine::MiddleOfSV_RunCmd,
			[&](auto pattern) {
				switch (pattern - patterns::engine::MiddleOfSV_RunCmd.cbegin())
				{
				default:
				case 0: // SteamPipe
					sv_areanodes = *reinterpret_cast<char**>(reinterpret_cast<uintptr_t>(MiddleOfSV_RunCmd) + 20);
					ORIG_SV_AddLinksToPM = reinterpret_cast<_SV_AddLinksToPM>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(MiddleOfSV_RunCmd) + 25)
						+ reinterpret_cast<uintptr_t>(MiddleOfSV_RunCmd) + 29);
					break;
				case 1: // Sven-v525
					sv_areanodes = *reinterpret_cast<char**>(reinterpret_cast<uintptr_t>(MiddleOfSV_RunCmd) + 22);
					ORIG_SV_AddLinksToPM = reinterpret_cast<_SV_AddLinksToPM>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(MiddleOfSV_RunCmd) + 27)
						+ reinterpret_cast<uintptr_t>(MiddleOfSV_RunCmd) + 31);
					break;
				case 2: // HL-1202
				case 3: // BShift-WON-1001
					sv_areanodes = *reinterpret_cast<char**>(reinterpret_cast<uintptr_t>(MiddleOfSV_RunCmd) + 1);
					break;
				}
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
				case 5: // CoF-5936
					ORIG_SV_SpawnServer = reinterpret_cast<_SV_SpawnServer>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Host_Changelevel2_f) + 335)
						+ reinterpret_cast<uintptr_t>(ORIG_Host_Changelevel2_f) + 339);
					break;
				case 6: // Sven-v525
					ORIG_SV_SpawnServer = reinterpret_cast<_SV_SpawnServer>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Host_Changelevel2_f) + 328)
						+ reinterpret_cast<uintptr_t>(ORIG_Host_Changelevel2_f) + 332);
					break;
				case 7: // HL-1202
					ORIG_SV_SpawnServer = reinterpret_cast<_SV_SpawnServer>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Host_Changelevel2_f) + 291)
						+ reinterpret_cast<uintptr_t>(ORIG_Host_Changelevel2_f) + 295);
					break;
				case 8: // BShift-WON-1001
					ORIG_SV_SpawnServer = reinterpret_cast<_SV_SpawnServer>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Host_Changelevel2_f) + 281)
						+ reinterpret_cast<uintptr_t>(ORIG_Host_Changelevel2_f) + 285);
					break;
				}
			});

		auto fJumpButton = FindAsync(
			ORIG_JumpButton,
			patterns::engine::JumpButton,
			[&](auto pattern) {
				switch (pattern - patterns::engine::JumpButton.cbegin())
				{
					case 0: // HL-1202
						onground = *reinterpret_cast<int**>(reinterpret_cast<uintptr_t>(ORIG_JumpButton) + 0x75);
						is_jumpbutton_found = true;
						break;
					case 1: // BShift-WON-1001
						onground = *reinterpret_cast<int**>(reinterpret_cast<uintptr_t>(ORIG_JumpButton) + 0x6C);
						is_jumpbutton_found = true;
						break;
				}
			});

		auto fPM_CheckStuck = FindAsync(
			ORIG_PM_CheckStuck,
			patterns::engine::PM_CheckStuck,
			[&](auto pattern) {
				switch (pattern - patterns::engine::PM_CheckStuck.cbegin())
				{
				case 0: // HL-1202
					won_ppmove = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(ORIG_PM_CheckStuck) + 0x46);
					break;
				case 1: // BShift-WON-1001
					won_ppmove = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(ORIG_PM_CheckStuck) + 0x36);
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
				case 2: // BShift-WON-1001.
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
				case 5: // CoF-5936.
					ORIG_Cbuf_InsertTextLines = reinterpret_cast<_Cbuf_InsertTextLines>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cmd_Exec_f) + 550)
						+ reinterpret_cast<uintptr_t>(ORIG_Cmd_Exec_f) + 554);
					break;
				case 6: // Sven-v525.
					ORIG_Cbuf_InsertTextLines = reinterpret_cast<_Cbuf_InsertTextLines>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cmd_Exec_f) + 631)
						+ reinterpret_cast<uintptr_t>(ORIG_Cmd_Exec_f) + 635);
					break;
				case 7: // HL-1202.
					ORIG_Cbuf_InsertTextLines = reinterpret_cast<_Cbuf_InsertTextLines>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_Cmd_Exec_f) + 178)
						+ reinterpret_cast<uintptr_t>(ORIG_Cmd_Exec_f) + 182);
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
				case 3: // CoF-5936.
					ORIG_Cmd_TokenizeString = reinterpret_cast<_Cmd_TokenizeString>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(Cmd_ExecuteString) + 19)
						+ reinterpret_cast<uintptr_t>(Cmd_ExecuteString) + 23);
					cmd_alias = *reinterpret_cast<cmdalias_t**>(reinterpret_cast<uintptr_t>(Cmd_ExecuteString) + 149);
					break;
				case 4: // Sven-v525.
					ORIG_Cmd_TokenizeString = reinterpret_cast<_Cmd_TokenizeString>(reinterpret_cast<uintptr_t>(Cmd_ExecuteString) - 0xE0);
					cmd_alias = *reinterpret_cast<cmdalias_t**>(reinterpret_cast<uintptr_t>(Cmd_ExecuteString) + 641);
					break;
				case 5: // HL-1202.
					ORIG_Cmd_TokenizeString = reinterpret_cast<_Cmd_TokenizeString>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(Cmd_ExecuteString) + 16)
						+ reinterpret_cast<uintptr_t>(Cmd_ExecuteString) + 20);
					cmd_alias = *reinterpret_cast<cmdalias_t**>(reinterpret_cast<uintptr_t>(Cmd_ExecuteString) + 73);
					break;
				}
			});

		auto fSV_SetMoveVars = FindAsync(
			ORIG_SV_SetMoveVars,
			patterns::engine::SV_SetMoveVars,
			[&](auto pattern) {
				switch (pattern - patterns::engine::SV_SetMoveVars.cbegin())
				{
				case 0: // SteamPipe.
					movevars = *reinterpret_cast<movevars_t**>(reinterpret_cast<uintptr_t>(ORIG_SV_SetMoveVars) + 18);
					break;
				case 1: // CoF-5936.
					movevars = *reinterpret_cast<movevars_t**>(reinterpret_cast<uintptr_t>(ORIG_SV_SetMoveVars) + 9);
					break;
				case 2: // Sven-v525.
					movevars = *reinterpret_cast<movevars_t**>(reinterpret_cast<uintptr_t>(ORIG_SV_SetMoveVars) + 8);
					break;
				case 3: // HL-1202
					movevars = *reinterpret_cast<movevars_t**>(reinterpret_cast<uintptr_t>(ORIG_SV_SetMoveVars) + 20);
					break;
				}
			}
		);

		auto fCL_SignonReply = FindAsync(
			ORIG_CL_SignonReply,
			patterns::engine::CL_SignonReply,
			[&](auto pattern) {
				switch (pattern - patterns::engine::CL_SignonReply.cbegin())
				{
				case 0: // SteamPipe.
				case 5: // CoF-5936.
					signon = *reinterpret_cast<int**>(reinterpret_cast<uintptr_t>(ORIG_CL_SignonReply) + 0xA);
					break;
				case 1: // 4554.
				case 2: // HL-WON-1712.
					signon = *reinterpret_cast<int**>(reinterpret_cast<uintptr_t>(ORIG_CL_SignonReply) + 1);
					break;
				case 3: // HL-1202.
				case 4: // BShift-WON-1001.
					signon = *reinterpret_cast<int**>(reinterpret_cast<uintptr_t>(ORIG_CL_SignonReply) + 0xB);
					break;
				case 6: // Sven-v525.
					signon = *reinterpret_cast<int**>(reinterpret_cast<uintptr_t>(ORIG_CL_SignonReply) + 0x16);
					break;
				}
			}
		);

		auto fR_LoadSkys = FindAsync(
			ORIG_R_LoadSkys,
			patterns::engine::R_LoadSkys,
			[&](auto pattern) {
				switch (pattern - patterns::engine::R_LoadSkys.cbegin())
				{
				case 0: // SteamPipe.
					gLoadSky = *reinterpret_cast<int**>(reinterpret_cast<uintptr_t>(ORIG_R_LoadSkys) + 7);
					break;
				case 1: // HL-4554.
				case 2: // HL-WON-1712.
				case 4: // HL-1202
					gLoadSky = *reinterpret_cast<int**>(reinterpret_cast<uintptr_t>(ORIG_R_LoadSkys) + 4);
					break;
				case 3: // CoF-5936
					gLoadSky = *reinterpret_cast<int**>(reinterpret_cast<uintptr_t>(ORIG_R_LoadSkys) + 0x12);
					break;
				case 5: // BShift-WON-1001
					gLoadSky = *reinterpret_cast<int**>(reinterpret_cast<uintptr_t>(ORIG_R_LoadSkys) + 1);
					break;
				}
			}
		);

		auto fR_StudioCalcAttachments = FindAsync(
			ORIG_R_StudioCalcAttachments,
			patterns::engine::R_StudioCalcAttachments,
			[&](auto pattern) {
				switch (pattern - patterns::engine::R_StudioCalcAttachments.cbegin())
				{
				case 0: // SteamPipe.
					ORIG_VectorTransform = reinterpret_cast<_VectorTransform>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_R_StudioCalcAttachments) + 106)
						+ reinterpret_cast<uintptr_t>(ORIG_R_StudioCalcAttachments) + 110);
					currententity = *reinterpret_cast<cl_entity_t***>(reinterpret_cast<uintptr_t>(ORIG_R_StudioCalcAttachments) + 0x10);
					break;
				case 1: // CoF-5936.
					ORIG_VectorTransform = reinterpret_cast<_VectorTransform>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_R_StudioCalcAttachments) + 157)
						+ reinterpret_cast<uintptr_t>(ORIG_R_StudioCalcAttachments) + 161);
					currententity = *reinterpret_cast<cl_entity_t***>(reinterpret_cast<uintptr_t>(ORIG_R_StudioCalcAttachments) + 0x16);
					break;
				case 2: // Sven-v525.
					ORIG_VectorTransform = reinterpret_cast<_VectorTransform>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_R_StudioCalcAttachments) + 110)
						+ reinterpret_cast<uintptr_t>(ORIG_R_StudioCalcAttachments) + 114);
					currententity = *reinterpret_cast<cl_entity_t***>(reinterpret_cast<uintptr_t>(ORIG_R_StudioCalcAttachments) + 0xF);
					break;
				case 3: // HL-1202.
					ORIG_VectorTransform = reinterpret_cast<_VectorTransform>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_R_StudioCalcAttachments) + 117)
						+ reinterpret_cast<uintptr_t>(ORIG_R_StudioCalcAttachments) + 121);
					currententity = *reinterpret_cast<cl_entity_t***>(reinterpret_cast<uintptr_t>(ORIG_R_StudioCalcAttachments) + 0x9);
					break;
				default:
					assert(false);
					break;
				}
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
				case 5: // BShift-WON-1001.
					pstudiohdr = *reinterpret_cast<studiohdr_t***>(reinterpret_cast<uintptr_t>(ORIG_R_StudioSetupBones) + 7);
					break;
				case 2: // CoF-5936.
					pstudiohdr = *reinterpret_cast<studiohdr_t***>(reinterpret_cast<uintptr_t>(ORIG_R_StudioSetupBones) + 16);
					break;
				case 3: // Sven-v525.
					pstudiohdr = *reinterpret_cast<studiohdr_t***>(reinterpret_cast<uintptr_t>(ORIG_R_StudioSetupBones) + 22);
					break;
				case 4: // HL-1202.
					pstudiohdr = *reinterpret_cast<studiohdr_t***>(reinterpret_cast<uintptr_t>(ORIG_R_StudioSetupBones) + 10);
					break;
				default:
					assert(false);
					break;
				}
			});

		void *CL_RegisterResources;
		auto fCL_RegisterResources = FindAsync(
			CL_RegisterResources,
			patterns::engine::CL_RegisterResources,
			[&](auto pattern) {
				switch (pattern - patterns::engine::CL_RegisterResources.cbegin())
				{
				default:
				case 0: // CoF-5936.
					noclip_anglehack = *reinterpret_cast<qboolean**>(reinterpret_cast<uintptr_t>(CL_RegisterResources) + 216);
					break;
				case 1: // Steampipe.
					noclip_anglehack = *reinterpret_cast<qboolean**>(reinterpret_cast<uintptr_t>(CL_RegisterResources) + 237);
					break;
				case 2: // 4554.
					noclip_anglehack = *reinterpret_cast<qboolean**>(reinterpret_cast<uintptr_t>(CL_RegisterResources) + 204);
					break;
				}
			});

		{
			auto pattern = fClientDLL_CheckStudioInterface.get();
			if (ClientDLL_CheckStudioInterface) {
				EngineDevMsg("[hw dll] Found ClientDLL_CheckStudioInterface at %p (using the %s pattern).\n", ClientDLL_CheckStudioInterface, pattern->name());
				EngineDevMsg("[hw dll] Found engine_studio_api at %p.\n", pEngStudio);
			}
			else {
				EngineDevWarning("[hw dll] Could not find ClientDLL_CheckStudioInterface.\n");
			}
		}

		{
			auto pattern = fSys_EngineAPI.get();
			if (Sys_EngineAPI) {
				EngineDevMsg("[hw dll] Found Sys_EngineAPI at %p (using the %s pattern).\n", Sys_EngineAPI, pattern->name());
				EngineDevMsg("[hw dll] Found engineapi at %p.\n", pEngineAPI);
			}
			else {
				EngineDevWarning("[hw dll] Could not find Sys_EngineAPI.\n");
			}
		}

		{
			auto pattern = fClientDLL_Init.get();
			if (ClientDLL_Init) {
				EngineDevMsg("[hw dll] Found ClientDLL_Init at %p (using the %s pattern).\n", ClientDLL_Init, pattern->name());
				EngineDevMsg("[hw dll] Found cl_enginefuncs at %p.\n", ClientDLL::GetInstance().pEngfuncs);
			}
			else {
				EngineDevWarning("[hw dll] Could not find ClientDLL_Init.\n");
			}
		}

		{
			auto pattern = fLoadThisDll.get();
			if (LoadThisDll) {
				EngineDevMsg("[hw dll] Found LoadThisDll at %p (using the %s pattern).\n", LoadThisDll, pattern->name());
				EngineDevMsg("[hw dll] Found g_engfuncsExportedToDlls at %p.\n", ServerDLL::GetInstance().pEngfuncs);
				EngineDevMsg("[hw dll] Found gGlobalVariables at %p.\n", ppGlobals);
			}
			else {
				EngineDevWarning("[hw dll] Could not find LoadThisDll.\n");
			}
		}

		{
			auto pattern = fHost_FilterTime_FPS_Cap_Byte.get();
			if (pHost_FilterTime_FPS_Cap_Byte) {
				EngineDevMsg("[hw dll] Found Host_FilterTime FPS Cap Byte at %p (using the %s pattern).\n", pHost_FilterTime_FPS_Cap_Byte, pattern->name());
			} else {
				EngineDevWarning("[hw dll] Could not find Host_FilterTime FPS Cap Byte.\n");
			}
		}

		{
			auto pattern = fHost_FilterTime_Check_72FPS.get();
			if (pHost_FilterTime_Check_72FPS) {
				EngineDevMsg("[hw dll] Found check for 72 FPS in Host_FilterTime at %p (using the %s pattern).\n", pHost_FilterTime_Check_72FPS, pattern->name());
			} else {
				EngineDevWarning("[hw dll] Could not find check for 72 FPS in Host_FilterTime.\n");
			}
		}

		{
			auto pattern = fHost_FilterTime_Set_72FPS.get();
			if (pHost_FilterTime_Set_72FPS) {
				EngineDevMsg("[hw dll] Found set to 72 FPS in Host_FilterTime at %p (using the %s pattern).\n", pHost_FilterTime_Set_72FPS, pattern->name());
			} else {
				EngineDevWarning("[hw dll] Could not find set to 72 FPS in Host_FilterTime.\n");
			}
		}

		{
			auto pattern = fGL_EndRendering_VID_FlipScreen_Bytes.get();
			if (pGL_EndRendering_VID_FlipScreen_Bytes) {
				EngineDevMsg("[hw dll] Found inline VID_FlipScreen in GL_EndRendering at %p (using the %s pattern).\n", pGL_EndRendering_VID_FlipScreen_Bytes, pattern->name());
			} else {
				EngineDevWarning("[hw dll] Could not find inline VID_FlipScreen in GL_EndRendering.\n");
			}
		}

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
			auto pattern = fR_SetFrustum.get();
			if (ORIG_R_SetFrustum) {
				EngineDevMsg("[hw dll] Found R_SetFrustum at %p (using the %s pattern).\n", ORIG_R_SetFrustum, pattern->name());
				EngineDevMsg("[hw dll] Found scr_fov_value at %p.\n", scr_fov_value);
			} else {
				EngineDevWarning("[hw dll] Could not find R_SetFrustum.\n");
			}
		}

		{
			auto pattern = fSCR_DrawPause.get();
			if (ORIG_SCR_DrawPause) {
				EngineDevMsg("[hw dll] Found SCR_DrawPause at %p (using the %s pattern).\n", ORIG_SCR_DrawPause, pattern->name());
				EngineDevMsg("[hw dll] Found cl.paused at %p.\n", cl_paused);
			} else {
				EngineDevWarning("[hw dll] Could not find SCR_DrawPause.\n");
			}
		}

		{
			auto pattern = fCL_RegisterResources.get();
			if (CL_RegisterResources) {
				EngineDevMsg("[hw dll] Found CL_RegisterResources at %p (using the %s pattern).\n", CL_RegisterResources, pattern->name());
				EngineDevMsg("[hw dll] Found noclip_anglehack at %p.\n", noclip_anglehack);
			} else {
				EngineDevWarning("[hw dll] Could not find CL_RegisterResources.\n");
			}
		}

		{
			auto pattern = fHost_ValidSave.get();
			if (ORIG_Host_ValidSave) {
				EngineDevMsg("[hw dll] Found Host_ValidSave at %p (using the %s pattern).\n", ORIG_Host_ValidSave, pattern->name());
				EngineDevMsg("[hw dll] Found cls at %p.\n", cls);
				EngineDevMsg("[hw dll] Found sv at %p.\n", psv);
				EngineDevMsg("[hw dll] Found svs at %p.\n", svs);
				EngineDevMsg("[hw dll] Found Con_Printf at %p.\n", ORIG_Con_Printf);
				if (is_cof_steam)
					EngineDevMsg("[hw dll] Found cof_savehack at %p.\n", cofSaveHack);
			} else {
				EngineDevWarning("[hw dll] Could not find Host_ValidSave.\n");
				EngineWarning("[hw dll] Quick saving in Cry of Fear is not available.\n");
				ORIG_Cbuf_Execute = nullptr;
			}
		}

		{
			auto pattern = fNUM_FOR_EDICT.get();
			if (NUM_FOR_EDICT) {
				EngineDevMsg("[hw dll] Found NUM_FOR_EDICT at %p (using the %s pattern).\n", NUM_FOR_EDICT, pattern->name());
				if (offEdicts)
				{
					EngineDevMsg("[hw dll] Offset to sv.edicts is %p.\n", offEdicts);
					offMaxEdicts = offEdicts - 0x4;
					offNumEdicts = offEdicts - 0x8;
				}
				else
				{
					EngineDevWarning("[hw dll] Offset to sv.edicts not found!\n");
				}
			}
			else {
				EngineDevWarning("[hw dll] Could not find NUM_FOR_EDICT.\n");
			}
		}

		{
			auto pattern = fCL_EntityNum.get();
			if (CL_EntityNum) {
				EngineDevMsg("[hw dll] Found CL_EntityNum at %p (using the %s pattern).\n", CL_EntityNum, pattern->name());
				EngineDevMsg("[hw dll] Found cl at %p.\n", pcl);
				EngineDevMsg("[hw dll] Found cl_entities at %p.\n", cl_entities);
			}
			else {
				EngineDevWarning("[hw dll] Could not find CL_EntityNum.\n");
			}
		}

		{
			auto pattern = fCL_FxBlend.get();
			if (CL_FxBlend) {
				EngineDevMsg("[hw dll] Found CL_FxBlend at %p (using the %s pattern).\n", CL_FxBlend, pattern->name());
				EngineDevMsg("[hw dll] Found cl.time at %p.\n", cl_time);
			}
			else {
				EngineDevWarning("[hw dll] Could not find CL_FxBlend.\n");
			}
		}

		{
			auto pattern = fModelFrames.get();
			if (ModelFrames) {
				EngineDevMsg("[hw dll] Found ModelFrames at %p (using the %s pattern).\n", ModelFrames, pattern->name());
				if (offModels)
					EngineDevMsg("[hw dll] Offset to sv.models is %p.\n", offModels);
				else
					EngineDevWarning("[hw dll] Offset to sv.models not found!\n");
			}
			else {
				EngineDevWarning("[hw dll] Could not find ModelFrames.\n");
			}
		}

		{
			auto pattern = fMiddleOfHost_SavegameComment.get();
			if (MiddleOfHost_SavegameComment) {
				EngineDevMsg("[hw dll] Found middle of Host_SavegameComment at %p (using the %s pattern).\n", MiddleOfHost_SavegameComment, pattern->name());
				EngineDevMsg("[hw dll] cl.levelname is %p.\n", levelname);
			}
			else {
				EngineDevWarning("[hw dll] Could not find middle of Host_SavegameComment.\n");
			}
		}

		{
			auto pattern = fMiddleOfCL_PredictMove.get();
			if (MiddleOfCL_PredictMove) {
				EngineDevMsg("[hw dll] Found middle of CL_PredictMove at %p (using the %s pattern).\n", MiddleOfCL_PredictMove, pattern->name());
				EngineDevMsg("[hw dll] cl.simorg is %p.\n", simorg);
				EngineDevMsg("[hw dll] cl.simvel is %p.\n", simvel);
			}
			else {
				EngineDevWarning("[hw dll] Could not find middle of CL_PredictMove.\n");
			}
		}

		{
			auto pattern = fMiddleOfR_Envmap_f.get();
			if (MiddleOfR_Envmap_f) {
				EngineDevMsg("[hw dll] Found middle of R_Envmap_f at %p (using the %s pattern).\n", MiddleOfR_Envmap_f, pattern->name());
				EngineDevMsg("[hw dll] Found r_refdef at %p.\n", r_refdef);
			}
			else {
				EngineDevWarning("[hw dll] Could not find middle of R_Envmap_f.\n");
			}
		}

		{
			auto pattern = fHost_TogglePause_f.get();
			if (Host_TogglePause_f) {
				EngineDevMsg("[hw dll] Found Host_TogglePause_f at %p (using the %s pattern).\n", Host_TogglePause_f, pattern->name());
				EngineDevMsg("[hw dll] Found cmd_source at %p.\n", cmd_source);
				EngineDevMsg("[hw dll] Found Cmd_ForwardToServer at %p.\n", ORIG_Cmd_ForwardToServer);
				EngineDevMsg("[hw dll] sv.reliable_datagram is %p.\n", reliable_datagram);
				EngineDevMsg("[hw dll] Found MSG_WriteByte at %p.\n", ORIG_MSG_WriteByte);
			}
			else {
				EngineDevWarning("[hw dll] Could not find Host_TogglePause_f.\n");
			}
		}

		{
			auto pattern = fChase_Update.get();
			if (Chase_Update) {
				EngineDevMsg("[hw dll] Found Chase_Update at %p (using the %s pattern).\n", Chase_Update, pattern->name());
				EngineDevMsg("[hw dll] cl.viewangles is %p.\n", viewangles);
			}
			else {
				EngineDevWarning("[hw dll] Could not find Chase_Update.\n");
			}
		}

		{
			auto pattern = fMiddleOfV_RenderView.get();
			if (MiddleOfV_RenderView) {
				EngineDevMsg("[hw dll] Found middle of V_RenderView at %p (using the %s pattern).\n", MiddleOfV_RenderView, pattern->name());
				EngineDevMsg("[hw dll] cl.playernum is %p.\n", playernum);
				EngineDevMsg("[hw dll] cl.viewent is %p.\n", viewent);
			}
			else {
				EngineDevWarning("[hw dll] Could not find middle of V_RenderView.\n");
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
			auto pattern = fEndOfHost_FilterTime.get();
			if (EndOfHost_FilterTime) {
				EngineDevMsg("[hw dll] Found end of Host_FilterTime at %p (using the %s pattern).\n", EndOfHost_FilterTime, pattern->name());
				EngineDevMsg("[hw dll] Found host_frametime at %p.\n", host_frametime);
			} else {
				EngineDevWarning("[hw dll] Could not find end of Host_FilterTime.\n");
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
			auto pattern = fMiddleOfSV_ExecuteClientMessage.get();
			if (MiddleOfSV_ExecuteClientMessage) {
				EngineDevMsg("[hw dll] Found middle of SV_ExecuteClientMessage at %p (using the %s pattern).\n", MiddleOfSV_ExecuteClientMessage, pattern->name());
				if (svmove)
					EngineDevMsg("[hw dll] Found g_svmove at %p.\n", svmove);
				if (ppmove)
					EngineDevMsg("[hw dll] Found pmove at %p.\n", ppmove);
				EngineDevMsg("[hw dll] Found host_client at %p.\n", host_client);
				EngineDevMsg("[hw dll] Found sv_player at %p.\n", sv_player);
			} else {
				EngineDevWarning("[hw dll] Could not find middle of SV_ExecuteClientMessage.\n");
				ORIG_Cbuf_Execute = nullptr;
			}
		}

		{
			auto pattern = fMiddleOfSV_RunCmd.get();
			if (MiddleOfSV_RunCmd) {
				EngineDevMsg("[hw dll] Found the sv_areanodes pattern at %p (using the %s pattern).\n", MiddleOfSV_RunCmd, pattern->name());
				EngineDevMsg("[hw dll] Found sv_areanodes at %p.\n", sv_areanodes);
				if (ORIG_SV_AddLinksToPM)
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
				EngineDevMsg("[hw dll] Found demoplayback at %p.\n", demoplayback);
			} else {
				EngineDevWarning("[hw dll] Could not find CL_Stop_f.\n");
				ORIG_Cbuf_Execute = nullptr;
			}
		}

		{
			auto pattern = fCmd_Exec_f.get();
			if (ORIG_Cmd_Exec_f) {
				EngineDevMsg("[hw dll] Found Cmd_Exec_f at %p (using the %s pattern).\n", ORIG_Cmd_Exec_f, pattern->name());
				if (ORIG_Cbuf_InsertTextLines)
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
			auto pattern = fCL_SignonReply.get();
			if (ORIG_CL_SignonReply) {
				EngineDevMsg("[hw dll] Found CL_SignonReply at %p (using the %s pattern).\n", ORIG_CL_SignonReply, pattern->name());
				EngineDevMsg("[hw dll] Found cls.signon at %p.\n", signon);
			} else {
				EngineDevWarning("[hw dll] Could not find CL_SignonReply.\n");
			}
		}

		{
			auto pattern = fR_LoadSkys.get();
			if (ORIG_R_LoadSkys) {
				EngineDevMsg("[hw dll] Found R_LoadSkys at %p (using the %s pattern).\n", ORIG_R_LoadSkys, pattern->name());
				EngineDevMsg("[hw dll] Found gLoadSky at %p.\n", gLoadSky);
			} else {
				EngineDevWarning("[hw dll] Could not find R_LoadSkys.\n");
			}
		}

		{
			auto pattern = fJumpButton.get();
			if (ORIG_JumpButton) {
				EngineDevMsg("[hw dll] Found JumpButton at %p (using the %s pattern).\n", ORIG_JumpButton, pattern->name());
				EngineDevMsg("[hw dll] Found onground at %p.\n", onground);
			}
			else {
				EngineDevWarning("[hw dll] Could not find JumpButton.\n");
			}
		}

		{
			auto pattern = fPM_CheckStuck.get();
			if (ORIG_PM_CheckStuck) {
				EngineDevMsg("[hw dll] Found PM_CheckStuck at %p (using the %s pattern).\n", ORIG_PM_CheckStuck, pattern->name());
				EngineDevMsg("[hw dll] Found pmove at %p.\n", won_ppmove);
			}
			else {
				EngineDevWarning("[hw dll] Could not find PM_CheckStuck.\n");
			}
		}

		{
			auto pattern = fR_StudioCalcAttachments.get();
			if (ORIG_R_StudioCalcAttachments) {
				EngineDevMsg("[hw dll] Found R_StudioCalcAttachments at %p (using the %s pattern).\n", ORIG_R_StudioCalcAttachments, pattern->name());
				EngineDevMsg("[hw dll] Found VectorTransform at %p.\n", ORIG_VectorTransform);
				EngineDevMsg("[hw dll] Found currententity at %p.\n", currententity);
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

		{
			auto pattern = fCmd_AddMallocCommand.get();
			auto pattern2 = fCmd_AddHUDCommand.get();
			if (ORIG_Cmd_AddMallocCommand) {
				EngineDevMsg("[hw dll] Found Cmd_AddMallocCommand at %p (using the %s pattern).\n", ORIG_Cmd_AddMallocCommand, pattern->name());
			} else if (ORIG_Cmd_AddHUDCommand) {
				EngineDevMsg("[hw dll] Found Cmd_AddHUDCommand at %p (using the %s pattern).\n", ORIG_Cmd_AddHUDCommand, pattern2->name());
			} else {
				EngineDevWarning("[hw dll] Could not find Cmd_AddMallocCommand.\n");
				EngineDevWarning("[hw dll] Could not find Cmd_AddHUDCommand.\n");
				ORIG_Cbuf_Execute = nullptr;
			}
		}

		//GET_FUTURE(RandomFloat)
		GET_FUTURE(RandomLong)
		GET_FUTURE(SCR_BeginLoadingPlaque)
		GET_FUTURE(Host_Loadgame_f)
		GET_FUTURE(Host_Reload_f)
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
				ORIG_Cmd_AddHUDCommand = nullptr;
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

		GET_FUTURE(PM_PlayerTrace);	
		GET_FUTURE(CL_RecordHUDCommand);
		GET_FUTURE(Cmd_FindCmd);
		GET_FUTURE(Cmd_Exists);
		GET_FUTURE(Host_FilterTime);
		GET_FUTURE(V_FadeAlpha);
		GET_FUTURE(V_ApplyShake);
		GET_FUTURE(R_DrawSkyBox);
		GET_FUTURE(SV_Frame);
		GET_FUTURE(VGuiWrap2_ConDPrintf);
		GET_FUTURE(VGuiWrap2_ConPrintf);
		GET_FUTURE(SCR_UpdateScreen);
		GET_FUTURE(R_DrawSequentialPoly);
		GET_FUTURE(R_Clear);
		GET_FUTURE(R_DrawViewModel);
		GET_FUTURE(R_PreDrawViewModel);
		GET_FUTURE(Mod_LeafPVS);
		GET_FUTURE(PF_GetPhysicsKeyValue);
		GET_FUTURE(SV_AddLinksToPM_);
		GET_FUTURE(SV_WriteEntitiesToClient);
		GET_FUTURE(VGuiWrap_Paint);
		GET_FUTURE(DispatchDirectUserMsg);
		GET_FUTURE(EmitWaterPolys);
		GET_FUTURE(S_StartDynamicSound);
		GET_FUTURE(VGuiWrap2_NotifyOfServerConnect);
		GET_FUTURE(CBaseUI__HideGameUI);
		GET_FUTURE(R_DrawWorld);
		GET_FUTURE(R_DrawParticles);
		GET_FUTURE(BUsesSDLInput);
		GET_FUTURE(R_StudioRenderModel);
		GET_FUTURE(SPR_Set);
		GET_FUTURE(DrawCrosshair);
		GET_FUTURE(Draw_FillRGBA);
		GET_FUTURE(PF_traceline_DLL);
		GET_FUTURE(CL_CheckGameDirectory);
		GET_FUTURE(Host_Noclip_f);
		GET_FUTURE(Host_Notarget_f);
		GET_FUTURE(SaveGameSlot);
		GET_FUTURE(CL_HudMessage);
		GET_FUTURE(SCR_NetGraph);
		GET_FUTURE(VGuiWrap2_IsGameUIVisible);
		GET_FUTURE(Host_Shutdown);
		GET_FUTURE(ReleaseEntityDlls);
		GET_FUTURE(Cvar_Command);
		GET_FUTURE(Cvar_CommandWithPrivilegeCheck);
		GET_FUTURE(R_ForceCvars);
		GET_FUTURE(GL_BuildLightmaps);
		GET_FUTURE(VID_FlipScreen);
		GET_FUTURE(GL_EndRendering);
		GET_FUTURE(SV_AddToFullPack);
		GET_FUTURE(CL_EmitEntities);
		GET_FUTURE(Draw_String);
		GET_FUTURE(V_RenderView);
		GET_FUTURE(LoadAdjacentEntities);
		GET_FUTURE(PlayerMove);
		GET_FUTURE(CL_Restore);
		GET_FUTURE(Sequence_OnLevelLoad);

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

void HwDLL::ResetStateBeforeTASPlayback()
{
	// Disable the input editor.
	SetTASEditorMode(TASEditorMode::DISABLED);

	runningFrames = false;
	currentFramebulk = 0;
	currentRepeat = 0;
	movementFrameCounter = 0;
	StrafeState = HLStrafe::CurrentState();
	PrevStrafeState = HLStrafe::CurrentState();
	PrevFractions = {1, 0, 0, 0 };
	PrevNormalzs = {0, 0, 0, 0 };
	ButtonsPresent = false;
	hltas_filename.clear();
	demoName.clear();
	saveName.clear();
	frametime0ms.clear();
	hlstrafe_version = HLStrafe::MAX_SUPPORTED_VERSION;
	SharedRNGSeedPresent = false;
	SetNonSharedRNGSeed = false;
	thisFrameIs0ms = false;
	clearedImpulsesForTheFirstTime = false;
	TargetYawOverrideIndex = 0;
	TargetYawOverrides.clear();
}

void HwDLL::StartTASPlayback()
{
	if (!exportFilename.empty())
		exportResult.ClearProperties();

	bool saw_hlstrafe_version = false;
	std::string load_command;
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
		} else if (prop.first == "frametime0ms")
			frametime0ms = prop.second;
		else if (prop.first == "hlstrafe_version") {
			hlstrafe_version = std::strtoul(prop.second.c_str(), nullptr, 10);

			saw_hlstrafe_version = true;

			if (hlstrafe_version > HLStrafe::MAX_SUPPORTED_VERSION) {
				ORIG_Con_Printf("Error loading the script: hlstrafe_version %u is too high (maximum supported version: %u)\n", hlstrafe_version, HLStrafe::MAX_SUPPORTED_VERSION);
				return;
			}
		} else if (prop.first == "load_command")
			load_command = prop.second;

		if (!exportFilename.empty())
			exportResult.SetProperty(prop.first, prop.second);
	}

	if (saw_hlstrafe_version) {
		if (hlstrafe_version < HLStrafe::MAX_SUPPORTED_VERSION)
			ORIG_Con_Printf("The script's hlstrafe_version is %u, but the latest version is %u. If this is an old script, keep it as is. For new scripts, please add a \"hlstrafe_version %u\" property to get the most accurate TAS prediction.\n", hlstrafe_version, HLStrafe::MAX_SUPPORTED_VERSION, HLStrafe::MAX_SUPPORTED_VERSION);
	} else {
		hlstrafe_version = 1;
		ORIG_Con_Printf("No hlstrafe_version property found in the script. If this is an old script, keep it as is, or add a \"hlstrafe_version 1\" property explicitly. For new scripts, please add a \"hlstrafe_version %u\" property to get the most accurate TAS prediction.\n", HLStrafe::MAX_SUPPORTED_VERSION);
	}

	if (!input.GetFrames().empty()) {
		runningFrames = true;
		wasRunningFrames = false; // So that ResetButtons() and others run in InsertCommands().
		totalFramebulks = input.GetFrames().size();
		HLTAS::Frame f;
		if (GetNextMovementFrame(f)) {
			std::ostringstream ss;
			ss << "host_framerate " << f.Frametime.c_str() << "\n";
			ORIG_Cbuf_InsertText(ss.str().c_str());
		}

		totalFrames = 0;
		for (const auto& frame_bulk : input.GetFrames()) {
			if (!frame_bulk.IsMovement())
				continue;

			totalFrames += frame_bulk.GetRepeats();
		}

		auto norefresh_until_frames = CVars::bxt_tas_norefresh_until_last_frames.GetInt();
		if (norefresh_until_frames > 0 && totalFrames > static_cast<size_t>(norefresh_until_frames))
			ORIG_Cbuf_InsertText("_bxt_norefresh 1\n");

		// Reset the frametime remainder automatically upon starting a script.
		// Fairly certain that's what you want in 100% of cases.
		if (frametime_remainder)
			*frametime_remainder = 0;

		// Disable the freecam. A case could be made for it being useful, however with the
		// current implementation it just uses the viewangles from the strafing and so isn't
		// really useful.
		SetFreeCam(false);

		// It will be enabled by bxt_tas_write_log if needed.
		SetTASLogging(false);
	}

	if (!load_command.empty()) {
		load_command += '\n';
		ORIG_Cbuf_InsertText(load_command.c_str());
	}
}

struct HwDLL::Cmd_BXT_TAS_LoadScript
{
	USAGE("Usage: bxt_tas_loadscript <filename>\n");

	static void handler(const char *fileName) {
		auto& hw = HwDLL::GetInstance();
		if (hw.resetState != ResetState::NORMAL)
			return;

		hw.ResetStateBeforeTASPlayback();
		hw.hltas_filename = fileName;

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

		hw.StartTASPlayback();
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

struct HwDLL::Cmd_BXT_TAS_Check_Position
{
	USAGE("Usage: _bxt_tas_check_position <x> <y> <z>\n Checks that the current player position matches the given coordinates, and if it doesn't, restarts the TAS.\n");

	static void handler(float x, float y, float z)
	{
		auto &hw = HwDLL::GetInstance();

		if (!hw.runningFrames)
		{
			hw.ORIG_Con_Printf("Not playing back a TAS.\n");
			return;
		}

		const auto& origin = (*hw.sv_player)->v.origin;

		if (fabs(origin.x - x) < 0.001 &&
			fabs(origin.y - y) < 0.001 &&
			fabs(origin.z - z) < 0.001)
		{
			hw.ORIG_Con_Printf("Position check succeeded.\n");
			return;
		}

		hw.ORIG_Con_Printf("Player position %.4f %.4f %.4f doesn't match the expected position %.4f %.4f %.4f, restarting the script.\n",
			origin.x, origin.y, origin.z, x, y, z);

		const auto filename = hw.hltas_filename;
		hw.ResetStateBeforeTASPlayback();
		hw.hltas_filename = filename;

		if (std::getenv("BXT_SCRIPT"))
		{
			// BXT_SCRIPT is set; assume that this TAS requires an RNG seed set at startup and restart the game.
			hw.ORIG_Cbuf_InsertText("_restart\n");
		}
		else
		{
			// No seed, just start the playback.
			hw.StartTASPlayback();
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

struct HwDLL::Cmd_BXT_Get_Origin_And_Angles
{
	NO_USAGE();

	static void handler()
	{
		auto &hw = HwDLL::GetInstance();
		auto &cl = ClientDLL::GetInstance();
		auto &sv = ServerDLL::GetInstance();
		float angles[3];
		cl.GetViewAngles(angles);

		float view[3], end[3];
		cl.SetupTraceVectors(view, end);
		const auto tr = sv.TraceLine(view, end, 0, hw.GetPlayerEdict());

		Vector vorg;
		if (hw.is_sdk10 && hw.r_refdef)
			vorg = hw.r_refdef->vieworg;
		else
			vorg = cl.last_vieworg;

		hw.ORIG_Con_Printf("bxt_set_angles %f %f %f;", angles[0], angles[1], angles[2]);
		if (CVars::bxt_hud_origin.GetInt() == 2)
			hw.ORIG_Con_Printf("bxt_ch_set_pos %f %f %f\n", vorg.x, vorg.y, vorg.z);
		else
			hw.ORIG_Con_Printf("bxt_ch_set_pos %f %f %f\n", (*hw.sv_player)->v.origin[0], (*hw.sv_player)->v.origin[1], (*hw.sv_player)->v.origin[2]);

		hw.ORIG_Con_Printf("bxt_cam_fixed %f %f %f %f %f %f\n", vorg.x, vorg.y, vorg.z, angles[0], angles[1], angles[2]);
		hw.ORIG_Con_Printf("Traced point origin: %f %f %f\n", tr.vecEndPos[0], tr.vecEndPos[1], tr.vecEndPos[2]);
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

struct HwDLL::Cmd_BXT_CH_Set_Ammo_Primary
{
	USAGE("Usage: bxt_ch_set_ammo_primary <value>\n bxt_ch_set_ammo_primary <value> <player_index>\n");

	static void handler(int val, int pl_num)
	{
		auto& hw = HwDLL::GetInstance();
		if (pl_num > 1)
		{
			hw.player_index = pl_num;
			hw.set_default_player_index = false;
		}
		handler(val);
	}

	static void handler(int val)
	{
		auto& sv = ServerDLL::GetInstance();
		auto& hw = HwDLL::GetInstance();

		hw.SetPlayerIndexToDefaultIfNecessary();

		if (sv.offm_pClientActiveItem && sv.offm_iPrimaryAmmoType && sv.offm_rgAmmoLast)
		{
			edict_t* edicts;
			hw.GetEdicts(&edicts);

			edict_t* ent = edicts + hw.player_index;
			bool is_ent_failed = hw.CheckIfEntityIsValidAndPlayer(ent, hw.player_index, true, true);
			if (is_ent_failed)
				return;

			void* classPtr = ent->v.pContainingEntity->pvPrivateData;
			uintptr_t thisAddr = reinterpret_cast<uintptr_t>(classPtr);
			void** m_pActiveItem = reinterpret_cast<void**>(thisAddr + (sv.offm_pClientActiveItem - 4));

			if (*m_pActiveItem != NULL) {
				uintptr_t* m_pClientActiveItem = reinterpret_cast<uintptr_t*>(thisAddr + sv.offm_pClientActiveItem);
				int* m_rgAmmo = reinterpret_cast<int*>(thisAddr + (sv.offm_rgAmmoLast - (sv.maxAmmoSlots * 4)));
				int* m_iPrimaryAmmoType = reinterpret_cast<int*>(*m_pClientActiveItem + sv.offm_iPrimaryAmmoType);
				m_rgAmmo[*m_iPrimaryAmmoType] = val;
			}
		}
	}
};

struct HwDLL::Cmd_BXT_CH_Set_Ammo_Secondary
{
	USAGE("Usage: bxt_ch_set_ammo_secondary <value>\n bxt_ch_set_ammo_secondary <value> <player_index>\n");

	static void handler(int val, int pl_num)
	{
		auto& hw = HwDLL::GetInstance();
		if (pl_num > 1)
		{
			hw.player_index = pl_num;
			hw.set_default_player_index = false;
		}
		handler(val);
	}

	static void handler(int val)
	{
		auto& sv = ServerDLL::GetInstance();
		auto& hw = HwDLL::GetInstance();

		hw.SetPlayerIndexToDefaultIfNecessary();

		if (sv.offm_pClientActiveItem && sv.offm_iPrimaryAmmoType && sv.offm_rgAmmoLast)
		{
			edict_t* edicts;
			hw.GetEdicts(&edicts);

			edict_t* ent = edicts + hw.player_index;
			bool is_ent_failed = hw.CheckIfEntityIsValidAndPlayer(ent, hw.player_index, true, true);
			if (is_ent_failed)
				return;

			void* classPtr = ent->v.pContainingEntity->pvPrivateData;
			uintptr_t thisAddr = reinterpret_cast<uintptr_t>(classPtr);
			void** m_pActiveItem = reinterpret_cast<void**>(thisAddr + (sv.offm_pClientActiveItem - 4));

			if (*m_pActiveItem != NULL) {
				uintptr_t* m_pClientActiveItem = reinterpret_cast<uintptr_t*>(thisAddr + sv.offm_pClientActiveItem);
				int* m_rgAmmo = reinterpret_cast<int*>(thisAddr + (sv.offm_rgAmmoLast - (sv.maxAmmoSlots * 4)));
				int* m_iSecondaryAmmoType = reinterpret_cast<int*>(*m_pClientActiveItem + (sv.offm_iPrimaryAmmoType + 4));
				m_rgAmmo[*m_iSecondaryAmmoType] = val;
			}
		}
	}
};

struct HwDLL::Cmd_BXT_CH_Set_Ammo_Clip
{
	USAGE("Usage: bxt_ch_set_ammo_clip <value>\n bxt_ch_set_ammo_clip <value> <player_index>\n");

	static void handler(int val, int pl_num)
	{
		auto& hw = HwDLL::GetInstance();
		if (pl_num > 1)
		{
			hw.player_index = pl_num;
			hw.set_default_player_index = false;
		}
		handler(val);
	}

	static void handler(int val)
	{
		auto& sv = ServerDLL::GetInstance();
		auto& hw = HwDLL::GetInstance();

		hw.SetPlayerIndexToDefaultIfNecessary();

		if (sv.offm_pClientActiveItem && sv.offm_iPrimaryAmmoType)
		{
			edict_t* edicts;
			hw.GetEdicts(&edicts);

			edict_t* ent = edicts + hw.player_index;
			bool is_ent_failed = hw.CheckIfEntityIsValidAndPlayer(ent, hw.player_index, true, true);
			if (is_ent_failed)
				return;

			void* classPtr = ent->v.pContainingEntity->pvPrivateData;
			uintptr_t thisAddr = reinterpret_cast<uintptr_t>(classPtr);
			void** m_pActiveItem = reinterpret_cast<void**>(thisAddr + (sv.offm_pClientActiveItem - 4));

			if (*m_pActiveItem != NULL) {
				uintptr_t* m_pClientActiveItem = reinterpret_cast<uintptr_t*>(thisAddr + sv.offm_pClientActiveItem);
				int* m_iClip = reinterpret_cast<int*>(*m_pClientActiveItem + (sv.offm_iPrimaryAmmoType + 8));
				*m_iClip = val;
			}
		}
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

struct HwDLL::Cmd_BXT_Set_Angles
{
	USAGE("Usage: bxt_set_angles <pitch> <yaw> [roll]\n");

	static void handler(float x, float y)
	{
		auto &cl = ClientDLL::GetInstance();
		float vec[3];
		vec[0] = x;
		vec[1] = y;
		vec[2] = 0.0f;
		cl.SetViewAngles(vec);
	}

	static void handler(float x, float y, float z)
	{
		auto &cl = ClientDLL::GetInstance();
		float vec[3];
		vec[0] = x;
		vec[1] = y;
		vec[2] = z;
		cl.SetViewAngles(vec);
	}
};

struct HwDLL::Cmd_BXT_Set_Skybox
{
	USAGE("Usage: bxt_set_skybox <name>\n");

	static void handler(const char *name)
	{
		auto &hw = HwDLL::GetInstance();
		if (hw.ORIG_R_LoadSkys && hw.gLoadSky && hw.movevars)
		{
			*hw.gLoadSky = 1; // Same as calling R_InitSky() function
			strncpy(hw.movevars->skyName, name, sizeof(hw.movevars->skyName) - 1);
			hw.ORIG_R_LoadSkys();
		}
	}
};

struct HwDLL::Cmd_BXT_CH_Client_Set_Armor
{
	USAGE("Usage:\n"
		"bxt_ch_client_set_armor <armor>\n"
		"bxt_ch_client_set_armor <armor> <entity_index>\n"
	);

	static void handler(float arm)
	{
		const auto& serv = ServerDLL::GetInstance();
		float view[3], end[3];
		ClientDLL::GetInstance().SetupTraceVectors(view, end);

		const auto tr = serv.TraceLine(view, end, 0, HwDLL::GetInstance().GetPlayerEdict());

		if (tr.pHit)
		{
			const auto ent = tr.pHit;

			if (ent->v.flags & FL_CLIENT)
				ent->v.armorvalue = arm;
		}
	}

	static void handler(float arm, int num)
	{
		auto& hw = HwDLL::GetInstance();

		edict_t* edicts;
		hw.GetEdicts(&edicts);

		edict_t* ent = edicts + num;
		bool is_ent_failed = hw.CheckIfEntityIsValidAndPlayer(ent, num, false, true);
		if (is_ent_failed)
			return;

		if (ent->v.flags & FL_CLIENT)
			ent->v.armorvalue = arm;
	}
};

struct HwDLL::Cmd_BXT_CH_Client_Set_Velocity
{
	USAGE("Usage:\n"
		"bxt_ch_client_set_velocity <x> <y> <z>\n"
		"bxt_ch_client_set_velocity <x> <y> <z> <entity_index>\n"
	);

	static void handler(float x, float y, float z)
	{
		const auto& serv = ServerDLL::GetInstance();
		float view[3], end[3];
		ClientDLL::GetInstance().SetupTraceVectors(view, end);

		const auto tr = serv.TraceLine(view, end, 0, HwDLL::GetInstance().GetPlayerEdict());

		if (tr.pHit)
		{
			const auto ent = tr.pHit;

			if (ent->v.flags & FL_CLIENT)
			{
				ent->v.velocity[0] = x;
				ent->v.velocity[1] = y;
				ent->v.velocity[2] = z;
			}
		}
	}

	static void handler(float x, float y, float z, int num)
	{
		auto& hw = HwDLL::GetInstance();

		edict_t* edicts;
		hw.GetEdicts(&edicts);

		edict_t* ent = edicts + num;
		bool is_ent_failed = hw.CheckIfEntityIsValidAndPlayer(ent, num, false, true);
		if (is_ent_failed)
			return;

		if (ent->v.flags & FL_CLIENT)
		{
			ent->v.velocity[0] = x;
			ent->v.velocity[1] = y;
			ent->v.velocity[2] = z;
		}
	}
};

struct HwDLL::Cmd_BXT_CH_Client_Toggle_Noclip
{
	NO_USAGE();

	static void handler()
	{
		const auto& serv = ServerDLL::GetInstance();
		float view[3], end[3];
		ClientDLL::GetInstance().SetupTraceVectors(view, end);

		const auto tr = serv.TraceLine(view, end, 0, HwDLL::GetInstance().GetPlayerEdict());

		if (tr.pHit)
		{
			const auto ent = tr.pHit;

			if (ent->v.flags & FL_CLIENT)
			{
				if (ent->v.movetype == MOVETYPE_NOCLIP)
					ent->v.movetype = MOVETYPE_WALK;
				else
					ent->v.movetype = MOVETYPE_NOCLIP;
			}
		}
	}

	static void handler(int num)
	{
		auto& hw = HwDLL::GetInstance();

		edict_t* edicts;
		hw.GetEdicts(&edicts);

		edict_t* ent = edicts + num;
		bool is_ent_failed = hw.CheckIfEntityIsValidAndPlayer(ent, num, true, true);
		if (is_ent_failed)
			return;

		if (ent->v.movetype == MOVETYPE_NOCLIP)
			ent->v.movetype = MOVETYPE_WALK;
		else
			ent->v.movetype = MOVETYPE_NOCLIP;
	}
};

struct HwDLL::Cmd_BXT_CH_Get_Velocity
{
	NO_USAGE();

	static void handler()
	{
		auto &hw = HwDLL::GetInstance();
		const auto& vel = (*hw.sv_player)->v.velocity;
		hw.ORIG_Con_Printf("bxt_ch_set_vel %f %f %f\n", vel.x, vel.y, vel.z);
		hw.ORIG_Con_Printf("Velocity (XY): %f\n", vel.Length2D());
		hw.ORIG_Con_Printf("Velocity (XYZ): %f\n", vel.Length());
	}
};

struct HwDLL::Cmd_BXT_CH_Entity_Set_Health
{
	USAGE("Usage:\n"
		"bxt_ch_entity_set_health <health>\n"
		"bxt_ch_entity_set_health <health> <entity_index>\n"
	);

	static void handler(float hp)
	{
		const auto& serv = ServerDLL::GetInstance();
		float view[3], end[3];
		ClientDLL::GetInstance().SetupTraceVectors(view, end);

		const auto tr = serv.TraceLine(view, end, 0, HwDLL::GetInstance().GetPlayerEdict());

		if (tr.pHit)
		{
			const auto ent = tr.pHit;

			ent->v.health = hp;
		}
	}

	static void handler(float hp, int num)
	{
		auto& hw = HwDLL::GetInstance();

		edict_t* edicts;
		hw.GetEdicts(&edicts);

		edict_t* ent = edicts + num;
		bool is_ent_failed = hw.CheckIfEntityIsValidAndPlayer(ent, num, false, true);
		if (is_ent_failed)
			return;

		ent->v.health = hp;
	}
};

void HwDLL::TeleportMonsterToPosition(float x, float y, float z, int index)
{
	auto& hw = HwDLL::GetInstance();
	edict_t* edicts;
	hw.GetEdicts(&edicts);
	edict_t* ent = edicts + index;

	bool is_ent_failed = hw.CheckIfEntityIsValidAndPlayer(ent, index, false, true);
	if (is_ent_failed)
		return;

	if ((ent->v.flags & FL_MONSTER) || (ent->v.flags & FL_CLIENT))
	{
		ent->v.origin[0] = x;
		ent->v.origin[1] = y;
		ent->v.origin[2] = z;
	}
}

#ifdef _WIN32
void HwDLL::WinErrorIfWrongBXTVersion(LPCTSTR text)
{
	ClientDLL::GetInstance().pEngfuncs = nullptr;
	ServerDLL::GetInstance().pEngfuncs = nullptr;
	MessageBox(NULL, text, "Fatal Error", MB_OK | MB_ICONERROR);
}
#endif

void HwDLL::PauseGame(bool pause_state)
{
	if (ORIG_MSG_WriteByte && levelname && psv && reliable_datagram)
	{
		if (levelname[0])
		{
			if (CVars::pausable.GetBool())
			{
				int *paused = reinterpret_cast<int*>(psv)+1;
				*paused = pause_state;
				ORIG_MSG_WriteByte(reliable_datagram, 24); // svc_setpause
				ORIG_MSG_WriteByte(reliable_datagram, *paused);
			}
		}
	}
}

struct HwDLL::Cmd_BXT_CH_Monster_Set_Origin
{
	USAGE("Usage:\n"
		"bxt_ch_monster_set_origin <entity_index>\n"
		"bxt_ch_monster_set_origin <x> <y> <z>\n"
		"bxt_ch_monster_set_origin <x> <y> <z> <entity_index>\n"
		"bxt_ch_monster_set_origin <offset_x> <offset_y> <offset_z> <entity_index> *\n"
	);

	static void handler(int num)
	{
		auto& hw = HwDLL::GetInstance();

		const auto& p_pos = (*hw.sv_player)->v.origin;
		hw.TeleportMonsterToPosition(p_pos[0], p_pos[1], p_pos[2], num);
	}

	static void handler(float x, float y, float z)
	{
		const auto& serv = ServerDLL::GetInstance();
		float view[3], end[3];
		ClientDLL::GetInstance().SetupTraceVectors(view, end);

		const auto tr = serv.TraceLine(view, end, 0, HwDLL::GetInstance().GetPlayerEdict());

		if (tr.pHit)
		{
			const auto ent = tr.pHit;
			if ((ent->v.flags & FL_MONSTER) || (ent->v.flags & FL_CLIENT))
			{
				ent->v.origin[0] = x;
				ent->v.origin[1] = y;
				ent->v.origin[2] = z;
			}
		}
	}

	static void handler(float x, float y, float z, int num)
	{
		HwDLL::GetInstance().TeleportMonsterToPosition(x, y, z, num);
	}

	static void handler(float off_x, float off_y, float off_z, int num, const char *name)
	{
		auto& hw = HwDLL::GetInstance();

		edict_t* edicts;
		hw.GetEdicts(&edicts);

		edict_t* ent = edicts + num;
		bool is_ent_failed = hw.CheckIfEntityIsValidAndPlayer(ent, num, false, true);
		if (is_ent_failed)
			return;

		bool match_offstring = std::strcmp(name, "*") == 0;
		if (match_offstring)
		{
			if ((ent->v.flags & FL_MONSTER) || (ent->v.flags & FL_CLIENT))
			{
				ent->v.origin[0] += off_x;
				ent->v.origin[1] += off_y;
				ent->v.origin[2] += off_z;
			}
		}
	}
};

struct HwDLL::Cmd_BXT_CH_Get_Other_Player_Info
{
	NO_USAGE();

	static void handler(int pl_num)
	{
		auto& hw = HwDLL::GetInstance();
		if (pl_num > 1)
		{
			hw.player_index = pl_num;
			hw.set_default_player_index = false;
		}
		handler();
	}

	static void handler()
	{
		auto &hw = HwDLL::GetInstance();
		auto &cl = ClientDLL::GetInstance();
		auto &sv = ServerDLL::GetInstance();

		hw.SetPlayerIndexToDefaultIfNecessary();

		edict_t* edicts;
		hw.GetEdicts(&edicts);

		edict_t* ent = edicts + hw.player_index;
		bool is_ent_player_failed = hw.CheckIfEntityIsValidAndPlayer(ent, hw.player_index, true, true);
		if (is_ent_player_failed)
			return;

		if (sv.is_cof)
		{
			void* classPtr = ent->v.pContainingEntity->pvPrivateData;
			uintptr_t thisAddr = reinterpret_cast<uintptr_t>(classPtr);

			if (sv.offm_iKeypadNumber)
			{
				int* m_iKeypadNumber = reinterpret_cast<int*>(thisAddr + sv.offm_iKeypadNumber);
				hw.ORIG_Con_Printf("Keypad code [CoF]: %d\n", *m_iKeypadNumber);
			}

			if (sv.offm_iPadlockNumber)
			{
				int* m_iPadlockNumber = reinterpret_cast<int*>(thisAddr + sv.offm_iPadlockNumber);

				std::ostringstream ss_padlock;

				for (int i = 1; i < 5; i++)
				{
					ss_padlock << m_iPadlockNumber[i];
				}

				hw.ORIG_Con_Printf("Padlock code [CoF]: %s\n", ss_padlock.str().c_str());
			}

			if (sv.offplayerstats)
			{
				ptrdiff_t pshots_fired;
				if (sv.is_cof_old_stats)
					pshots_fired = 0x0;
				else
					pshots_fired = 0x4;
				ptrdiff_t pshots_hit = 0x4 + pshots_fired;
				ptrdiff_t psyringes_used = 0x8 + pshots_fired;
				ptrdiff_t pshots_missed = 0xC + pshots_fired;
				ptrdiff_t pgunaccuracy = 0x10 + pshots_fired;
				ptrdiff_t ptimessaved = 0x14 + pshots_fired;
				void* playerstats = reinterpret_cast<void*>(thisAddr + sv.offplayerstats);
				uintptr_t playerstatsAddr = reinterpret_cast<uintptr_t>(playerstats);
				int* shots_fired = reinterpret_cast<int*>(playerstatsAddr + pshots_fired);
				int* shots_hit = reinterpret_cast<int*>(playerstatsAddr + pshots_hit);
				int* syringes_used = reinterpret_cast<int*>(playerstatsAddr + psyringes_used);
				int* shots_missed = reinterpret_cast<int*>(playerstatsAddr + pshots_missed);
				float* gunaccuracy = reinterpret_cast<float*>(playerstatsAddr + pgunaccuracy);
				int* timessaved = reinterpret_cast<int*>(playerstatsAddr + ptimessaved);

				hw.ORIG_Con_Printf("Fired: %d\nHit: %d\nMissed: %d\nSyringes used: %d\nGun accuracy: %f\nTimes saved: %d\n", *shots_fired, *shots_hit, *shots_missed, *syringes_used, *gunaccuracy, *timessaved);
			}
		}

		const auto& mvtype = ent->v.movetype;
		const auto& sld = ent->v.solid;
		const auto& vel = ent->v.velocity;
		const auto& basevel = ent->v.basevelocity;
		const auto& punch = ent->v.punchangle;
		const auto& v_angle = ent->v.v_angle;
		const auto& orin = ent->v.origin;
		const auto& rnmode = ent->v.rendermode;
		const auto& rncolor = ent->v.rendercolor;
		const auto& rnfx = ent->v.renderfx;

		#ifndef SDK10_BUILD
		if (cl.pEngfuncs && (hw.player_index == 1))
			hw.ORIG_Con_Printf("Client maxspeed: %f\n", cl.pEngfuncs->GetClientMaxspeed());
		#endif
		hw.ORIG_Con_Printf("Index: %d\n", hw.player_index);
		hw.ORIG_Con_Printf("Movetype: %d (%s)\n", mvtype, hw.GetMovetypeName(mvtype));
		hw.ORIG_Con_Printf("Solid: %d (%s)\n", sld, hw.GetSolidName(sld));
		hw.ORIG_Con_Printf("Health: %f\n", ent->v.health);
		hw.ORIG_Con_Printf("Armor: %f\n", ent->v.armorvalue);
		hw.ORIG_Con_Printf("Waterlevel: %d\n", ent->v.waterlevel);
		hw.ORIG_Con_Printf("Watertype: %d\n", ent->v.watertype);
		hw.ORIG_Con_Printf("Max health: %f\n", ent->v.max_health);
		hw.ORIG_Con_Printf("Gravity: %f\n", ent->v.gravity);
		hw.ORIG_Con_Printf("Friction: %f\n", ent->v.friction);
		std::ostringstream out_flags;
		hw.GetFlags(out_flags, hw.player_index);
		hw.ORIG_Con_Printf("%s", out_flags.str().c_str());
		hw.ORIG_Con_Printf("v_angle: %f %f %f\n", v_angle.x, v_angle.y, v_angle.z);
		hw.ORIG_Con_Printf("Origin: %f %f %f\n", orin.x, orin.y, orin.z);
		hw.ORIG_Con_Printf("Velocity: %f %f %f; XY = %f; XYZ = %f\n", vel.x, vel.y, vel.z, vel.Length2D(), vel.Length());
		hw.ORIG_Con_Printf("Basevelocity: %f %f %f; XY = %f; XYZ = %f\n", basevel.x, basevel.y, basevel.z, basevel.Length2D(), basevel.Length());
		hw.ORIG_Con_Printf("Server punchangle: %f %f %f\n", punch.x, punch.y, punch.z);
		std::ostringstream out_button;
		hw.GetButton(out_button, hw.player_index);
		hw.ORIG_Con_Printf("%s", out_button.str().c_str());
		std::ostringstream out_effects;
		hw.GetEffects(out_effects, hw.player_index);
		hw.ORIG_Con_Printf("%s", out_effects.str().c_str());
		hw.ORIG_Con_Printf("Rendermode: %d (%s)\n", rnmode, hw.GetRenderModeName(rnmode));
		hw.ORIG_Con_Printf("Renderamt: %f\n", ent->v.renderamt);
		hw.ORIG_Con_Printf("Rendercolor: %f %f %f\n", rncolor.x, rncolor.y, rncolor.z);
		hw.ORIG_Con_Printf("Renderfx: %d (%s)\n", rnfx, hw.GetRenderFxName(rnfx));
		hw.ORIG_Con_Printf("Sequence: %d\n", ent->v.sequence);
		hw.ORIG_Con_Printf("Gaitsequence: %d\n", ent->v.gaitsequence);

		#ifndef SDK10_BUILD
		hw.ORIG_Con_Printf("bInDuck: %d\n", ent->v.bInDuck);
		hw.ORIG_Con_Printf("iuser1: %d; iuser2: %d; iuser3: %d; iuser4: %d\n", ent->v.iuser1, ent->v.iuser2, ent->v.iuser3, ent->v.iuser4);
		hw.ORIG_Con_Printf("fuser1: %f; fuser2: %f; fuser3: %f; fuser4: %f\n", ent->v.fuser1, ent->v.fuser2, ent->v.fuser3, ent->v.fuser4);

		const auto& vusr1 = ent->v.vuser1;
		const auto& vusr2 = ent->v.vuser2;
		const auto& vusr3 = ent->v.vuser3;
		const auto& vusr4 = ent->v.vuser4;
		hw.ORIG_Con_Printf("vuser1: %f %f %f; XY = %f; XYZ = %f\n", vusr1.x, vusr1.y, vusr1.z, vusr1.Length2D(), vusr1.Length());
		hw.ORIG_Con_Printf("vuser2: %f %f %f; XY = %f; XYZ = %f\n", vusr2.x, vusr2.y, vusr2.z, vusr2.Length2D(), vusr2.Length());
		hw.ORIG_Con_Printf("vuser3: %f %f %f; XY = %f; XYZ = %f\n", vusr3.x, vusr3.y, vusr3.z, vusr3.Length2D(), vusr3.Length());
		hw.ORIG_Con_Printf("vuser4: %f %f %f; XY = %f; XYZ = %f\n", vusr4.x, vusr4.y, vusr4.z, vusr4.Length2D(), vusr4.Length());
		#endif
	}
};

struct HwDLL::Cmd_BXT_Get_SteamID_In_Demo
{
	NO_USAGE();

	static void handler()
	{
		auto& hw = HwDLL::GetInstance();
		auto& cl = ClientDLL::GetInstance();
		if (hw.is_steamid_build && hw.IsPlayingbackDemo())
		{
			int player = cl.GetLocalPlayer()->index;
			player_info_s* player_info = hw.pEngStudio->PlayerInfo(player - 1);

			hw.ORIG_Con_Printf("Steam ID: %" PRIu64 "\n", player_info->m_nSteamID);
		}
	}
};

struct HwDLL::Cmd_BXT_Test_Output
{
	NO_USAGE();

	// That is just a blank command created for insert code to output information from different variables or functions when it necessary for debugging.
	static void handler()
	{
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

void HwDLL::TimerStart()
{
	if (!CustomHud::GetCountingTime())
		HwDLL::GetInstance().Called_Timer = true;

	CustomHud::SaveTimeToDemo();
	return CustomHud::SetCountingTime(true);
}

struct HwDLL::Cmd_BXT_Timer_Start
{
	NO_USAGE();

	static void handler()
	{
		auto &hw = HwDLL::GetInstance();
		return hw.TimerStart();
	}
};

struct HwDLL::Cmd_BXT_Timer_Stop
{
	NO_USAGE();

	static void handler()
	{
		if (CustomHud::GetCountingTime())
			HwDLL::GetInstance().Called_Timer = true;

		CustomHud::SaveTimeToDemo();
		return CustomHud::SetCountingTime(false);
	}
};

void HwDLL::TimerReset()
{
	const auto& gt = CustomHud::GetTime();
	int total_time = (gt.hours * 60 * 60) + (gt.minutes * 60) + gt.seconds;

	if (gt.milliseconds > 0 || total_time > 0)
		HwDLL::GetInstance().Called_Timer = true;

	CustomHud::SaveTimeToDemo();
	CustomHud::SetInvalidRun(false);
	Splits::Reset();
	return CustomHud::ResetTime();
}

struct HwDLL::Cmd_BXT_Timer_Reset
{
	NO_USAGE();

	static void handler()
	{
		auto &hw = HwDLL::GetInstance();
		return hw.TimerReset();
	}
};

struct HwDLL::Cmd_BXT_Get_Server_Time
{
	NO_USAGE();

	static void handler()
	{
		HwDLL::GetInstance().ORIG_Con_Printf("Server time: %f\n", ServerDLL::GetInstance().GetTime());
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
		if (HwDLL::GetInstance().is_sdk10)
		{
			HwDLL::GetInstance().ORIG_Con_Printf("bxt_triggers_place_down is not supported for pre-1.1.0.0 versions");
			return;
		}

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

			oss << "bxt_triggers_add " << std::fixed << std::setprecision(1)
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

struct HwDLL::Cmd_BXT_SetPause
{
	NO_USAGE();

	static void handler()
	{
		auto &hw = HwDLL::GetInstance();
		hw.PauseGame(true);
	}
};

struct HwDLL::Cmd_BXT_UnPause
{
	NO_USAGE();

	static void handler()
	{
		auto &hw = HwDLL::GetInstance();
		hw.PauseGame(false);
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

struct HwDLL::Cmd_Plus_BXT_TAS_Editor_Insert_Point
{
	NO_USAGE();

	static void handler()
	{
		HwDLL::GetInstance().tas_editor_insert_point = true;
		HwDLL::GetInstance().tas_editor_insert_point_held = true;
	}

	static void handler(int)
	{
		handler();
	}
};

struct HwDLL::Cmd_Minus_BXT_TAS_Editor_Insert_Point
{
	NO_USAGE();

	static void handler()
	{
		HwDLL::GetInstance().tas_editor_insert_point = false;
		HwDLL::GetInstance().tas_editor_insert_point_held = false;
	}

	static void handler(int)
	{
		handler();
	}
};

struct HwDLL::Cmd_BXT_TAS_Editor_Resimulate
{
	USAGE("Usage: bxt_tas_editor_resimulate\n Forces simulator client to resimulate.\n");

	static void handler()
	{
		auto& hw = HwDLL::GetInstance();
		auto& frame_bulks = hw.tas_editor_input.frame_bulks;

		if (hw.tas_editor_mode == TASEditorMode::EDIT) {
			if (frame_bulks.size() > 0) {
				hw.tas_editor_input.mark_as_stale(0);
			}
		}
	}
};

struct HwDLL::Cmd_BXT_TAS_Editor_Toggle
{
	USAGE("Usage: bxt_tas_editor_toggle <what>\n Toggles a function on the currently selected point. You can toggle:\n"
	      " - s03 - speed increasing strafing,\n"
	      " - s13 - quick turn strafing,\n"
	      " - s22 - slow down strafing,\n"
	      " - s00 - speed increasing strafing to the left,\n"
	      " - s01 - speed increasing strafing to the right,\n"
	      " - s10 - quick turn strafing to the left,\n"
	      " - s11 - quick turn strafing to the right,\n"
	      " - s06 - left-right strafing,\n"
	      " - s07 - right-left strafing,\n"
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
		if (!strcmp(what, "s00")) {
			HwDLL::GetInstance().tas_editor_toggle_s00 = true;
		} else if (!strcmp(what, "s01")) {
			HwDLL::GetInstance().tas_editor_toggle_s01 = true;
		} else if (!strcmp(what, "s03")) {
			HwDLL::GetInstance().tas_editor_toggle_s03 = true;
		} else if (!strcmp(what, "s10")) {
			HwDLL::GetInstance().tas_editor_toggle_s10 = true;
		} else if (!strcmp(what, "s11")) {
			HwDLL::GetInstance().tas_editor_toggle_s11 = true;
		} else if (!strcmp(what, "s13")) {
			HwDLL::GetInstance().tas_editor_toggle_s13 = true;
		} else if (!strcmp(what, "s22")) {
			HwDLL::GetInstance().tas_editor_toggle_s22 = true;
		} else if (!strcmp(what, "s06")) {
			HwDLL::GetInstance().tas_editor_toggle_s06 = true;
		} else if (!strcmp(what, "s07")) {
			HwDLL::GetInstance().tas_editor_toggle_s07 = true;
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

struct HwDLL::Cmd_BXT_TAS_Editor_Set_Frametime
{
	USAGE("Usage: bxt_tas_editor_set_frametime <frametime>\n Sets frametime on the currently selected point.\n");

	static void handler(const char *value)
	{
		auto& hw = HwDLL::GetInstance();

		if (std::atof(value) <= 0.f) {
			hw.ORIG_Con_Printf("Frametime must be greater than 0.\n");
		} else {
			hw.tas_editor_set_frametime = true;
			hw.tas_editor_set_frametime_time = value;
		}
	}
};

struct HwDLL::Cmd_BXT_TAS_Editor_Set_Change_Type
{
	USAGE("Usage: bxt_tas_editor_set_change_type <type>\n Set type of change for a point in the camera editor. Valid types are target_yaw, target_yaw_offset, yaw, pitch.\n");

	static void handler(const char *what)
	{
		if (!strcmp(what, "target_yaw")) {
			HwDLL::GetInstance().tas_editor_set_change_to_target_yaw = true;
		} else if (!strcmp(what, "target_yaw_offset")) {
			HwDLL::GetInstance().tas_editor_set_change_to_target_yaw_offset = true;
		} else if (!strcmp(what, "yaw")) {
			HwDLL::GetInstance().tas_editor_set_change_to_yaw = true;
		} else if (!strcmp(what, "pitch")) {
			HwDLL::GetInstance().tas_editor_set_change_to_pitch = true;
		}
	}
};

struct HwDLL::Cmd_BXT_TAS_Editor_Set_Target_Yaw_Type
{
	USAGE("Usage: bxt_tas_editor_set_target_yaw_type <type>\n Set type of target_yaw for a point in the camera editor. Valid types (currently supported) are velocity_lock and look_at [entity <index>] [<x> <y> <z>].\n");

	static void handler(const char *what)
	{
		auto& hw = HwDLL::GetInstance();

		if (!strcmp(what, "velocity_lock")) {
			hw.tas_editor_set_target_yaw_velocity_lock = true;
		} else {
			unsigned int entity;
			float x = 0, y = 0, z = 0;
			hw.tas_editor_set_target_yaw_look_at_entity = 0;
			hw.tas_editor_set_target_yaw_look_at_x = 0;
			hw.tas_editor_set_target_yaw_look_at_y = 0;
			hw.tas_editor_set_target_yaw_look_at_z = 0;

			int scan_entity = sscanf(what, "look_at entity %d %f %f %f", &entity, &x, &y, &z);
			if (scan_entity) {
				hw.tas_editor_set_target_yaw_look_at = true;
				hw.tas_editor_set_target_yaw_look_at_entity = entity;

				if (scan_entity == 4) {
					hw.tas_editor_set_target_yaw_look_at_x = x;
					hw.tas_editor_set_target_yaw_look_at_y = y;
					hw.tas_editor_set_target_yaw_look_at_z = z;
				}
			} else if (sscanf(what, "look_at %f %f %f", &x, &y, &z) == 3) {
				hw.tas_editor_set_target_yaw_look_at = true;
				hw.tas_editor_set_target_yaw_look_at_entity = 0;
				hw.tas_editor_set_target_yaw_look_at_x = x;
				hw.tas_editor_set_target_yaw_look_at_y = y;
				hw.tas_editor_set_target_yaw_look_at_z = z;
			}
		}
	}
};

struct HwDLL::Cmd_BXT_FreeCam
{
	USAGE("Usage: bxt_freecam <0|1>\n Enables the freecam mode. Most useful when paused with bxt_unlock_camera_during_pause 1.\n");

	static void handler(int enabled)
	{
		if (HwDLL::GetInstance().is_sdk10)
		{
			HwDLL::GetInstance().ORIG_Con_Printf("bxt_freecam is not supported for pre-1.1.0.0 versions");
			return;
		}

		HwDLL::GetInstance().SetFreeCam(enabled);
	}
};

void HwDLL::PrintEntity(std::ostringstream &out, int index)
{
	const auto& hw = HwDLL::GetInstance();
	edict_t* edicts;
	hw.GetEdicts(&edicts);
	const edict_t* ent = edicts + index;
	const char* classname = hw.GetString(ent->v.classname);
	const char* targetname = hw.GetString(ent->v.targetname);
	const char* target = hw.GetString(ent->v.target);

	out << index << ": " << classname;

	if (ent->v.targetname != 0) {
		out << "; name: " << targetname;
	}

	if (ent->v.target != 0) {
		out << "; target: " << target;
	}

	out << "; hp: " << ent->v.health;

	if ((!strncmp(classname, "func_door", 9)) || (!strncmp(classname, "func_rotating", 13)) || (!strncmp(classname, "func_train", 10)))
		out << "; dmg: " << ent->v.dmg;

	Vector origin;
	HwDLL::GetInstance().GetOriginOfEntity(origin, ent);

	out << "; xyz: " << origin.x << " " << origin.y << " " << origin.z;

	out << '\n';
}

struct HwDLL::Cmd_BXT_Print_Entities
{
	NO_USAGE();

	static void handler(const char *name1, const char *name2)
	{
		const auto& hw = HwDLL::GetInstance();

		std::ostringstream out;

		bool match_substring = std::strcmp(name2, "*") == 0;

		edict_t *edicts;
		const int numEdicts = hw.GetEdicts(&edicts);
		for (int e = 0; e < numEdicts; ++e) {
			const edict_t *ent = edicts + e;
			if (!hw.IsValidEdict(ent))
				continue;

			const char *classname = hw.GetString(ent->v.classname);
			if (match_substring)
			{
				if ((strstr(classname, name1) == 0))
					continue;
			}
			else
			{
				if ((strcmp(classname, name1) != 0) && (strcmp(classname, name2) != 0))
					continue;
			}

			HwDLL::GetInstance().PrintEntity(out, e);
		}

		auto str = out.str();
		hw.ORIG_Con_Printf("%s", str.c_str());
	}

	static void handler(const char *name)
	{
		const auto& hw = HwDLL::GetInstance();

		std::ostringstream out;

		edict_t *edicts;
		const int numEdicts = hw.GetEdicts(&edicts);
		for (int e = 0; e < numEdicts; ++e) {
			const edict_t *ent = edicts + e;
			if (!hw.IsValidEdict(ent))
				continue;

			const char* classname = hw.GetString(ent->v.classname);
			const char* targetname = hw.GetString(ent->v.targetname);
			const char* target = hw.GetString(ent->v.target);
			if ((std::strcmp(classname, name) != 0) && (std::strcmp(targetname, name) != 0) && (std::strcmp(target, name) != 0))
				continue;

			HwDLL::GetInstance().PrintEntity(out, e);
		}

		auto str = out.str();
		hw.ORIG_Con_Printf("%s", str.c_str());
	}

	static void handler()
	{
		const auto& hw = HwDLL::GetInstance();

		std::ostringstream out;

		edict_t* edicts;
		const int numEdicts = hw.GetEdicts(&edicts);
		for (int e = 0; e < numEdicts; ++e) {
			const edict_t* ent = edicts + e;
			if (!hw.IsValidEdict(ent))
				continue;

			HwDLL::GetInstance().PrintEntity(out, e);
		}

		auto str = out.str();
		hw.ORIG_Con_Printf("%s", str.c_str());
	}
};

struct HwDLL::Cmd_BXT_Print_Entities_By_Index
{
	USAGE("Usage:\n bxt_print_entities_by_index <index>\n bxt_print_entities_by_index <min_range> <max_range>\n");

	static void handler(int num)
	{
		auto& hw = HwDLL::GetInstance();

		std::ostringstream out;

		edict_t* edicts;
		hw.GetEdicts(&edicts);

		const edict_t *ent = edicts + num;
		bool is_ent_failed = hw.CheckIfEntityIsValidAndPlayer(ent, num, false, true);
		if (is_ent_failed)
			return;

		HwDLL::GetInstance().PrintEntity(out, num);

		auto str = out.str();
		hw.ORIG_Con_Printf("%s", str.c_str());
	}

	static void handler(int value1, int value2)
	{
		const auto& hw = HwDLL::GetInstance();

		std::ostringstream out;

		edict_t* edicts;
		const int numEdicts = hw.GetEdicts(&edicts);
		for (int e = 0; e < numEdicts; ++e) {
			const edict_t* ent = edicts + e;
			if (!hw.IsValidEdict(ent))
				continue;

			if ((e < value1) || (e > value2))
				continue;

			HwDLL::GetInstance().PrintEntity(out, e);
		}

		auto str = out.str();
		hw.ORIG_Con_Printf("%s", str.c_str());
	}
};

void HwDLL::GetOriginOfEntity(Vector& origin, const edict_t* ent)
{
	const auto& hw = HwDLL::GetInstance();
	const char* classname = hw.GetString(ent->v.classname);
	bool is_trigger = std::strncmp(classname, "trigger_", 8) == 0;
	bool is_ladder = std::strncmp(classname, "func_ladder", 11) == 0;
	bool is_friction = std::strncmp(classname, "func_friction", 13) == 0;
	bool is_water = std::strncmp(classname, "func_water", 10) == 0;

	// Credits to 'goldsrc_monitor' tool for their code to get origin of entities
	if (ent->v.solid == SOLID_BSP || ent->v.movetype == MOVETYPE_PUSHSTEP || is_trigger || is_ladder || is_friction || is_water)
		origin = ent->v.origin + ((ent->v.mins + ent->v.maxs) / 2.f);
	else
		origin = ent->v.origin;
}

bool HwDLL::CheckIfEntityIsValidAndPlayer(const edict_t* ent, int ent_number, bool check_for_player, bool print_message)
{
	auto& hw = HwDLL::GetInstance();
	edict_t* edicts;
	const int numEdicts = hw.GetEdicts(&edicts);

	if (ent_number < numEdicts)
	{
		if (hw.IsValidEdict(ent))
		{
			if ((ent->v.flags & FL_CLIENT) && check_for_player)
			{
				return false;
			}
			else if (check_for_player)
			{
				if (print_message)
					hw.ORIG_Con_Printf("Error: entity with index %d is not player!\n", ent_number);
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			if (print_message)
				hw.ORIG_Con_Printf("Error: entity with index %d is not valid\n", ent_number);
			return true;
		}
	}
	else
	{
		if (print_message)
			hw.ORIG_Con_Printf("Error: entity with index %d does not exist; there are %d entities in total\n", ent_number, numEdicts);
		return true;
	}

	return true;
}

void HwDLL::SetPlayerIndexToDefaultIfNecessary()
{
	auto& hw = HwDLL::GetInstance();
	if (hw.set_default_player_index)
		hw.player_index = 1;
	hw.set_default_player_index = true;
}

struct HwDLL::Cmd_BXT_CH_Teleport_To_Entity
{
	USAGE("Usage: bxt_ch_teleport_to_entity <index>\n bxt_ch_teleport_to_entity <index> <player_index>\n");

	static void handler(int num, int pl_num)
	{
		auto& hw = HwDLL::GetInstance();
		if (pl_num > 1)
		{
			hw.player_index = pl_num;
			hw.set_default_player_index = false;
		}
		handler(num);
	}

	static void handler(int num)
	{
		auto& hw = HwDLL::GetInstance();

		hw.SetPlayerIndexToDefaultIfNecessary();

		edict_t *edicts;
		hw.GetEdicts(&edicts);

		const edict_t *ent = edicts + num;
		bool is_ent_failed = hw.CheckIfEntityIsValidAndPlayer(ent, num, false, true);
		if (is_ent_failed)
			return;

		edict_t* ent_player = edicts + hw.player_index;
		bool is_ent_player_failed = hw.CheckIfEntityIsValidAndPlayer(ent_player, hw.player_index, true, true);
		if (is_ent_player_failed)
			return;

		Vector origin;
		HwDLL::GetInstance().GetOriginOfEntity(origin, ent);

		ent_player->v.origin[0] = origin[0];
		ent_player->v.origin[1] = origin[1];
		ent_player->v.origin[2] = origin[2];
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

struct HwDLL::Cmd_BXT_TAS_Editor_Set_Left_Right_Count
{
	USAGE("Usage: bxt_tas_editor_set_left_right_count <count>\n Sets the left-right or right-left frame count on the currently selected point.\n");

	static void handler(unsigned long value)
	{
		auto& hw = HwDLL::GetInstance();
		hw.tas_editor_set_left_right_count = true;
		hw.tas_editor_set_left_right_count_count = value;
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

struct HwDLL::Cmd_BXT_TAS_Editor_Apply_Smoothing
{
	NO_USAGE();

	static void handler()
	{
		auto& hw = HwDLL::GetInstance();
		hw.tas_editor_apply_smoothing = true;
	}
};

struct HwDLL::Cmd_BXT_TAS_Optim_Init
{
	NO_USAGE();

	static void handler()
	{
		auto& hw = HwDLL::GetInstance();

		std::ostringstream oss;
		oss << "_bxt_tas_optim_init \"" << hw.hltas_filename << "\" " << hw.movementFrameCounter - 1 << ";";
		hw.ORIG_Cbuf_InsertText(oss.str().c_str());
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
		if (simulation_ipc::write_command(std::string(command) + '\n'))
			simulation_ipc::send_message_to_client();
	}
};

struct HwDLL::Cmd_BXT_TAS_Client_Load_Received_Script
{
	NO_USAGE()

	static void handler() {
		if (!simulation_ipc::is_client_initialized())
			return;

		if (simulation_ipc::message.script[0] == 0)
			return;

		auto& hw = HwDLL::GetInstance();
		hw.ResetStateBeforeTASPlayback();

		auto err = hw.input.FromString(simulation_ipc::message.script);
		simulation_ipc::message.script[0] = 0;

		if (err.Code != HLTAS::ErrorCode::OK) {
			const auto& message = hw.input.GetErrorMessage();
			if (message.empty()) {
				hw.ORIG_Con_Printf("Error loading the script file on line %u: %s\n", err.LineNumber, HLTAS::GetErrorMessage(err).c_str());
			} else {
				hw.ORIG_Con_Printf("Error loading the script: %s\n", message.c_str());
			}
			return;
		}

		hw.StartTASPlayback();
	}
};

struct HwDLL::Cmd_BXT_Show_Bullets_Clear
{
	NO_USAGE()

	static void handler() {
		auto& serverDLL = ServerDLL::GetInstance();

		serverDLL.ClearBulletsTrace();
	}
};

struct HwDLL::Cmd_BXT_Show_Bullets_Enemy_Clear
{
	NO_USAGE()

	static void handler() {
		auto& serverDLL = ServerDLL::GetInstance();

		serverDLL.ClearBulletsEnemyTrace();
	}
};

struct HwDLL::Cmd_BXT_Split
{
	USAGE("Usage: bxt_split <name>\n Tells BunnySplit to split by the specified map name, and prints to console the current time. Mostly intended to be used in scripts, or in custom triggers (these may not be valid in runs, check the rules for your game/category).\n");

	// TODO: it should be possible to split without a map/split name, like when you right-click on LiveSplit and click on Split,
	// but I don't think BunnySplit supports this

	static void handler(const char* name)
	{
		Vector speed;
		Vector origin;
		const auto& player = HwDLL::GetInstance().GetPlayerEdict();
		if (player)
		{
			speed = player->v.velocity;
			origin = player->v.origin;
		}
		const auto& time = CustomHud::GetTime();

		Splits::Split fake_split;
		fake_split.name = std::move(name);
		fake_split.time = time;
		fake_split.speed = speed;
		fake_split.origin = origin;

		Splits::PrintSplitCompletion(fake_split);
		Interprocess::WriteMapChange(time, name);
	}
};

struct HwDLL::Cmd_BXT_Splits_Add_Entity
{
	USAGE("Usage: bxt_splits_add_entity <target_name> [map_name]\n Tells BunnySplit to split when activating an entity by the specified name, and prints to console the current time.\n");

	static void handler(const char* targetname)
	{
		Splits::splits.emplace_back(std::string(targetname));
	}

	static void handler(const char* targetname, const char* map_name)
	{
		Splits::splits.emplace_back(std::string(targetname), std::string(map_name));
	}
};

struct HwDLL::Cmd_BXT_Splits_Add_Trigger
{
	USAGE("Usage: bxt_splits_add_trigger <x1> <y1> <z1> <x2> <y2> <z2> [map_name] [name]\n Adds a split trigger in a form of axis-aligned cuboid with opposite corners at coordinates (x1, y1, z1) and (x2, y2, z2).\n");

	static void handler(float x1, float y1, float z1, float x2, float y2, float z2)
	{
		Splits::splits.emplace_back(Vector(x1, y1, z1), Vector(x2, y2, z2));
	}

	static void handler(float x1, float y1, float z1, float x2, float y2, float z2, const char* map_name)
	{
		Splits::splits.emplace_back(Vector(x1, y1, z1), Vector(x2, y2, z2), std::string(map_name));
	}

	static void handler(float x1, float y1, float z1, float x2, float y2, float z2, const char* map_name, const char* split_name)
	{
		Splits::splits.emplace_back(Vector(x1, y1, z1), Vector(x2, y2, z2), std::string(map_name), std::string(split_name));
	}
};

struct HwDLL::Cmd_BXT_Splits_Clear
{
	NO_USAGE()

	static void handler()
	{
		Splits::splits.clear();
	}
};

struct HwDLL::Cmd_BXT_Splits_Delete
{
	USAGE("Usage: bxt_splits_delete [id]\n Deletes the last placed split.\n If an id is given, deletes the split with the given id.\n");

	static void handler()
	{
		if (Splits::splits.empty()) {
			HwDLL::GetInstance().ORIG_Con_Printf("You haven't placed any splits.\n");
			return;
		}

		Splits::splits.erase(--Splits::splits.end());
	}

	static void handler(const char* id_or_name)
	{
		// First try to find it by name, otherwise we'll try to find by id
		const auto itr = std::find_if(Splits::splits.begin(), Splits::splits.end(),
			[&id_or_name](const Splits::Split& s) { return !strcmp(id_or_name, s.name.c_str()); });

		unsigned long idx = 0;
		if (itr == Splits::splits.end())
			idx = std::strtoul(id_or_name, nullptr, 10);
		else
			idx = itr - Splits::splits.begin() + 1;

		if (idx == 0 || Splits::splits.size() < idx) {
			HwDLL::GetInstance().ORIG_Con_Printf("There's no split with this name or id.\n");
			return;
		}

		Splits::splits.erase(Splits::splits.begin() + (idx - 1));
	}
};

struct HwDLL::Cmd_BXT_Splits_Export
{
	USAGE("Usage: bxt_splits_export [cmd|script]\n");

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

		if (Splits::splits.empty()) {
			hw.ORIG_Con_Printf("You haven't placed any splits.\n");
			return;
		}

		bool first = true;
		for (const auto& split : Splits::splits) {
			auto corners = split.get_corner_positions();

			std::ostringstream oss;

			if (!first)
				oss << command_separator;

			if (split.targets_entity) {
				oss << "bxt_splits_add_entity " << split.name << " " << split.map_name;
			} else {
				oss << "bxt_splits_add_trigger " << std::fixed << std::setprecision(1)
					<< corners.first.x << " " << corners.first.y << " " << corners.first.z << " "
					<< corners.second.x << " " << corners.second.y << " " << corners.second.z;

				if (!split.map_name.empty())
					oss << command_separator << "bxt_splits_set_map \"" << split.map_name << '\"';

				if (!split.name.empty())
					oss << command_separator << "bxt_splits_set_name \"" << split.name << '\"';
			}

			// Note that by default a split always tracks horizontal speed. If this behaviour changes,
			// we have to change this part too. If we always print the command regardless of the value,
			// then the output command/script will be huge
			if (!split.track_horizontal_speed)
				oss << command_separator << "bxt_splits_track_horizontal_speed 0";

			if (split.track_vertical_speed)
				oss << command_separator << "bxt_splits_track_vertical_speed 1";

			if (split.track_x)
				oss << command_separator << "bxt_splits_track_x 1";

			if (split.track_y)
				oss << command_separator << "bxt_splits_track_y 1";

			if (split.track_z)
				oss << command_separator << "bxt_splits_track_z 1";

			hw.ORIG_Con_Printf(oss.str().c_str());

			first = false;
		}

		hw.ORIG_Con_Printf("\n");
	}
};

struct HwDLL::Cmd_BXT_Splits_List
{
	USAGE("Usage: bxt_splits_list [map_name]. You can specify a map name to list splits that are limited to that map.\n");

	static void handler()
	{
		if (Splits::splits.empty()) {
			HwDLL::GetInstance().ORIG_Con_Printf("You haven't placed any splits.\n");
			return;
		}
		Splits::PrintList(Splits::splits);
	}

	static void handler(const char* map_name)
	{
		std::vector<Splits::Split> map_splits;
		std::copy_if(Splits::splits.begin(), Splits::splits.end(), std::back_inserter(map_splits),
			[&map_name](Splits::Split &s){ return !strcmp(map_name, s.map_name.c_str()); } );

		if (map_splits.empty()) {
			HwDLL::GetInstance().ORIG_Con_Printf("There are no splits in the specified map.\n");
			return;
		}
		Splits::PrintList(map_splits);
	}
};

struct HwDLL::Cmd_BXT_Splits_Print_Times
{
	NO_USAGE()

	static void handler()
	{
		Splits::PrintAll();
	}
};

struct HwDLL::Cmd_BXT_Splits_Set_Map
{
	USAGE("Usage: bxt_splits_set_map <name>\n Sets the last placed split's map (scope).\n bxt_splits_set_map <id> <name>\n Sets the map of a split with the given id.\n");

	static void handler(const char* newMap)
	{
		if (Splits::splits.empty()) {
			HwDLL::GetInstance().ORIG_Con_Printf("You haven't placed any splits.\n");
			return;
		}

		Splits::splits.back().map_name = std::move(newMap);
	}

	static void handler(const char* idOrName, const char* newMap)
	{
		auto split = Splits::GetSplitByNameOrId(idOrName);
		if (split)
			split->map_name = std::move(newMap);
	}
};

struct HwDLL::Cmd_BXT_Splits_Set_Name
{
	USAGE("Usage: bxt_splits_set_name <name>\n Sets the last placed split's name.\n bxt_splits_set_name <id> <name>\n Sets the name of a split with the given id.\n");

	static void handler(const char* newName)
	{
		if (Splits::splits.empty()) {
			HwDLL::GetInstance().ORIG_Con_Printf("You haven't placed any splits.\n");
			return;
		}

		Splits::splits.back().name = std::move(newName);
	}

	static void handler(const char* idOrName, const char* newName)
	{
		auto split = Splits::GetSplitByNameOrId(idOrName);
		if (split)
			split->name = std::move(newName);
	}
};

struct HwDLL::Cmd_BXT_Splits_Track_Horizontal_Speed
{
	USAGE("Usage: bxt_splits_track_horizontal_speed <0|1>\n Makes the last placed split account for the horizontal components of velocity when printing the speed.\n bxt_splits_track_horizontal_speed <split name or id> <0|1>\n Same but you can specify the split name or id instead.\n");

	static void handler(int value)
	{
		if (Splits::splits.empty()) {
			HwDLL::GetInstance().ORIG_Con_Printf("You haven't placed any splits.\n");
			return;
		}

		Splits::splits.back().track_horizontal_speed = value;
	}

	static void handler(const char* idOrName, int value)
	{
		auto split = Splits::GetSplitByNameOrId(idOrName);
		if (split)
			split->track_horizontal_speed = value;
	}
};

struct HwDLL::Cmd_BXT_Splits_Track_Vertical_Speed
{
	USAGE("Usage: bxt_splits_track_vertical_speed <0|1>\n Makes the last placed split account for the vertical component of velocity when printing the speed.\n bxt_splits_track_vertical_speed <split name or id> <0|1>\n Same but you can specify the split name or id instead.\n");

	static void handler(int value)
	{
		if (Splits::splits.empty()) {
			HwDLL::GetInstance().ORIG_Con_Printf("You haven't placed any splits.\n");
			return;
		}

		Splits::splits.back().track_vertical_speed = value;
	}

	static void handler(const char* idOrName, int value)
	{
		auto split = Splits::GetSplitByNameOrId(idOrName);
		if (split)
			split->track_vertical_speed = value;
	}
};

struct HwDLL::Cmd_BXT_Splits_Track_X
{
	USAGE("Usage: bxt_splits_track_x <0|1>\n Makes the last placed split account for the X component when printing your position.\n bxt_splits_track_x <split name or id> <0|1>\n Same but you can specify the split name or id instead.\n");

	static void handler(int value)
	{
		if (Splits::splits.empty()) {
			HwDLL::GetInstance().ORIG_Con_Printf("You haven't placed any splits.\n");
			return;
		}

		Splits::splits.back().track_x = value;
	}

	static void handler(const char* idOrName, int value)
	{
		auto split = Splits::GetSplitByNameOrId(idOrName);
		if (split)
			split->track_x = value;
	}
};

struct HwDLL::Cmd_BXT_Splits_Track_Y
{
	USAGE("Usage: bxt_splits_track_y <0|1>\n Makes the last placed split account for the Y component when printing your position.\n bxt_splits_track_y <split name or id> <0|1>\n Same but you can specify the split name or id instead.\n");

	static void handler(int value)
	{
		if (Splits::splits.empty()) {
			HwDLL::GetInstance().ORIG_Con_Printf("You haven't placed any splits.\n");
			return;
		}

		Splits::splits.back().track_y = value;
	}

	static void handler(const char* idOrName, int value)
	{
		auto split = Splits::GetSplitByNameOrId(idOrName);
		if (split)
			split->track_y = value;
	}
};

struct HwDLL::Cmd_BXT_Splits_Track_Z
{
	USAGE("Usage: bxt_splits_track_z <0|1>\n Makes the last placed split account for the Z component when printing your position.\n bxt_splits_track_z <split name or id> <0|1>\n Same but you can specify the split name or id instead.\n");

	static void handler(int value)
	{
		if (Splits::splits.empty()) {
			HwDLL::GetInstance().ORIG_Con_Printf("You haven't placed any splits.\n");
			return;
		}

		Splits::splits.back().track_z = value;
	}

	static void handler(const char* idOrName, int value)
	{
		auto split = Splits::GetSplitByNameOrId(idOrName);
		if (split)
			split->track_z = value;
	}
};

struct HwDLL::Cmd_BXT_Splits_Place_Up
{
	NO_USAGE()

	static void handler()
	{
		Splits::placing = false;
	}

	static void handler(const char *)
	{
		handler();
	}
};

struct HwDLL::Cmd_BXT_Splits_Place_Down
{
	NO_USAGE()

	static void handler()
	{
		if (HwDLL::GetInstance().is_sdk10)
		{
			HwDLL::GetInstance().ORIG_Con_Printf("bxt_splits_place_down is not supported for pre-1.1.0.0 versions");
			return;
		}

		auto trace = HwDLL::GetInstance().CameraTrace();

		Splits::placing = true;
		Vector start = trace.EndPos;
		Splits::place_start = start;
		Splits::splits.emplace_back(start, start, HwDLL::GetInstance().lastLoadedMap);
	}

	static void handler(const char*)
	{
		handler();
	}
};

extern "C" DLLEXPORT void bxt_tas_load_script_from_string(const char *script)
{
	auto& hw = HwDLL::GetInstance();
	hw.ResetStateBeforeTASPlayback();

	auto err = hw.input.FromString(script);

	if (err.Code != HLTAS::ErrorCode::OK) {
		const auto& message = hw.input.GetErrorMessage();
		if (message.empty()) {
			hw.ORIG_Con_Printf("Error loading the script file on line %u: %s\n", err.LineNumber, HLTAS::GetErrorMessage(err).c_str());
		} else {
			hw.ORIG_Con_Printf("Error loading the script: %s\n", message.c_str());
		}
		return;
	}

	hw.StartTASPlayback();
}

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
	if (ORIG_Cbuf_Execute)
		RegisterCVar(CVars::bxt_autopause);
	RegisterCVar(CVars::bxt_bhopcap);
	RegisterCVar(CVars::bxt_interprocess_enable);
	if (ORIG_V_FadeAlpha)
		RegisterCVar(CVars::bxt_fade_remove);
	if (ORIG_V_ApplyShake)
		RegisterCVar(CVars::bxt_shake_remove);
	if (ORIG_R_DrawSkyBox)
		RegisterCVar(CVars::bxt_skybox_remove);
	if (ORIG_EmitWaterPolys)
		RegisterCVar(CVars::bxt_water_remove);
	RegisterCVar(CVars::bxt_stop_demo_on_changelevel);
	RegisterCVar(CVars::bxt_tas_editor_simulate_for_ms);
	RegisterCVar(CVars::bxt_tas_editor_camera_editor);
	RegisterCVar(CVars::bxt_tas_norefresh_until_last_frames);
	RegisterCVar(CVars::bxt_tas_write_log);
	RegisterCVar(CVars::bxt_tas_playback_speed);
	RegisterCVar(CVars::bxt_tas_editor_apply_smoothing_over_s);
	RegisterCVar(CVars::_bxt_tas_editor_apply_smoothing_high_weight_duration);
	RegisterCVar(CVars::_bxt_tas_editor_apply_smoothing_high_weight_multiplier);
	if (ORIG_VGuiWrap_Paint)
		RegisterCVar(CVars::bxt_disable_vgui);
	if (ORIG_R_DrawSequentialPoly) {
		RegisterCVar(CVars::bxt_wallhack);
		RegisterCVar(CVars::bxt_wallhack_additive);
		RegisterCVar(CVars::bxt_wallhack_alpha);
	}
	if (ORIG_Mod_LeafPVS)
		RegisterCVar(CVars::bxt_novis);
	RegisterCVar(CVars::_bxt_norefresh);
	RegisterCVar(CVars::_bxt_bunnysplit_time_update_frequency);
	RegisterCVar(CVars::_bxt_save_runtime_data_in_demos);
	if (ORIG_PM_PlayerTrace)
	{
		RegisterCVar(CVars::bxt_collision_depth_map);
		RegisterCVar(CVars::bxt_collision_depth_map_colors);
		RegisterCVar(CVars::bxt_collision_depth_map_hull);
		RegisterCVar(CVars::bxt_collision_depth_map_max_depth);
		RegisterCVar(CVars::bxt_collision_depth_map_pixel_scale);
		RegisterCVar(CVars::bxt_collision_depth_map_remove_distance_limit);
		RegisterCVar(CVars::bxt_collision_depth_map_fov);
	}
	if (ORIG_SV_SetMoveVars && movevars)
		RegisterCVar(CVars::bxt_force_zmax);
	if (pEngStudio)
		RegisterCVar(CVars::bxt_viewmodel_semitransparent);
	RegisterCVar(CVars::bxt_viewmodel_disable_idle);
	RegisterCVar(CVars::bxt_viewmodel_disable_equip);
	if (ORIG_R_Clear) {
		RegisterCVar(CVars::bxt_clear_color);
		RegisterCVar(CVars::bxt_force_clear);
	}
	if (ORIG_BUsesSDLInput)
		RegisterCVar(CVars::bxt_fix_mouse_horizontal_limit);
	if (ORIG_CL_CheckGameDirectory)
		RegisterCVar(CVars::bxt_disable_gamedir_check_in_demo);
	if (pHost_FilterTime_FPS_Cap_Byte || (pHost_FilterTime_Check_72FPS && pHost_FilterTime_Set_72FPS))
		RegisterCVar(CVars::bxt_remove_fps_limit);
	if (ORIG_R_DrawWorld)
		RegisterCVar(CVars::bxt_disable_world);
	if (ORIG_R_DrawParticles)
		RegisterCVar(CVars::bxt_disable_particles);
	RegisterCVar(CVars::bxt_tas_autojump_priority);
	if (ORIG_Cvar_Command || ORIG_Cvar_CommandWithPrivilegeCheck || ORIG_R_ForceCvars)
		RegisterCVar(CVars::bxt_disable_cheats_check_in_demo);
	if (ORIG_R_SetFrustum && scr_fov_value)
	{
		RegisterCVar(CVars::bxt_force_fov);
		RegisterCVar(CVars::bxt_fix_widescreen_fov);
	}
	if (ORIG_R_DrawViewModel) {
		if (pEngStudio)
			RegisterCVar(CVars::bxt_viewmodel_fov);

		if (ORIG_R_PreDrawViewModel)
			RegisterCVar(CVars::bxt_remove_viewmodel);
	}
	if (ORIG_SV_AddToFullPack)
	{
		RegisterCVar(CVars::bxt_show_hidden_entities);
		RegisterCVar(CVars::bxt_show_hidden_entities_classname);
		RegisterCVar(CVars::bxt_show_triggers_legacy);
	}
	if (ORIG_PM_CheckStuck)
		RegisterCVar(CVars::bxt_fire_on_stuck);

	CVars::sv_cheats.Assign(FindCVar("sv_cheats"));
	CVars::fps_max.Assign(FindCVar("fps_max"));
	CVars::fps_single.Assign(FindCVar("fps_single"));
	CVars::default_fov.Assign(FindCVar("default_fov"));
	CVars::skill.Assign(FindCVar("skill"));
	CVars::host_framerate.Assign(FindCVar("host_framerate"));
	CVars::gl_monolights.Assign(FindCVar("gl_monolights"));
	CVars::r_fullbright.Assign(FindCVar("r_fullbright"));
	CVars::pausable.Assign(FindCVar("pausable"));

	FindCVarsIfNeeded();

	if (!ORIG_Cmd_AddMallocCommand && !ORIG_Cmd_AddHUDCommand)
		return;

	using CmdWrapper::Handler;
	typedef CmdWrapper::CmdWrapper<CmdFuncs> wrapper;

	if (is_cof_steam)
	{
		CmdFuncs::AddCommand("noclip", ORIG_Host_Noclip_f);
		CmdFuncs::AddCommand("notarget", ORIG_Host_Notarget_f);
	}

	wrapper::Add<Cmd_BXT_TAS_LoadScript, Handler<const char *>>("bxt_tas_loadscript");
	wrapper::Add<Cmd_BXT_TAS_ExportScript, Handler<const char *>>("bxt_tas_exportscript");
	wrapper::Add<Cmd_BXT_TAS_ExportLibTASInput, Handler<const char *>>("bxt_tas_export_libtas_input");
	wrapper::Add<Cmd_BXT_TAS_Split, Handler<const char *>>("bxt_tas_split");
	wrapper::Add<Cmd_BXT_TAS_New, Handler<const char *, const char *, int>>("bxt_tas_new");
	wrapper::Add<Cmd_BXT_TAS_Check_Position, Handler<float, float, float>>("_bxt_tas_check_position");
	wrapper::AddCheat<Cmd_BXT_CH_Set_Health, Handler<float>>("bxt_ch_set_health");
	wrapper::AddCheat<Cmd_BXT_CH_Set_Armor, Handler<float>>("bxt_ch_set_armor");
	wrapper::AddCheat<Cmd_BXT_CH_Set_Origin, Handler<float, float, float>>("bxt_ch_set_pos");
	wrapper::AddCheat<Cmd_BXT_CH_Set_Origin_Offset, Handler<float, float, float>>("bxt_ch_set_pos_offset");
	wrapper::AddCheat<Cmd_BXT_CH_Set_Velocity, Handler<float, float, float>>("bxt_ch_set_vel");
	wrapper::AddCheat<Cmd_BXT_CH_Set_Ammo_Primary, Handler<int>, Handler<int, int>>("bxt_ch_set_ammo_primary");
	wrapper::AddCheat<Cmd_BXT_CH_Set_Ammo_Secondary, Handler<int>, Handler<int, int>>("bxt_ch_set_ammo_secondary");
	wrapper::AddCheat<Cmd_BXT_CH_Set_Ammo_Clip, Handler<int>, Handler<int, int>>("bxt_ch_set_ammo_clip");
	wrapper::AddCheat<Cmd_BXT_CH_Teleport_To_Entity, Handler<int>, Handler<int, int>>("bxt_ch_teleport_to_entity");
	wrapper::AddCheat<Cmd_BXT_CH_Get_Velocity, Handler<>>("bxt_ch_get_vel");
	wrapper::AddCheat<Cmd_BXT_CH_Get_Other_Player_Info, Handler<>, Handler<int>>("bxt_ch_get_other_player_info");
	wrapper::Add<Cmd_BXT_Get_SteamID_In_Demo, Handler<>>("bxt_get_steamid_in_demo");
	wrapper::AddCheat<Cmd_BXT_Test_Output, Handler<>>("bxt_test_output");
	wrapper::AddCheat<Cmd_BXT_CH_Entity_Set_Health, Handler<float>, Handler<float, int>>("bxt_ch_entity_set_health");
	wrapper::AddCheat<Cmd_BXT_CH_Monster_Set_Origin, Handler<int>, Handler<float, float, float>, Handler<float, float, float, int>, Handler<float, float, float, int, const char *>>("bxt_ch_monster_set_origin");
	wrapper::AddCheat<Cmd_BXT_CH_Client_Set_Armor, Handler<float>, Handler<float, int>>("bxt_ch_client_set_armor");
	wrapper::AddCheat<Cmd_BXT_CH_Client_Set_Velocity, Handler<float, float, float>, Handler<float, float, float, int>>("bxt_ch_client_set_velocity");
	wrapper::AddCheat<Cmd_BXT_CH_Client_Toggle_Noclip, Handler<>, Handler<int>>("bxt_ch_client_toggle_noclip");
	wrapper::AddCheat<
		Cmd_BXT_CH_Set_Velocity_Angles,
		Handler<float>,
		Handler<float, float, float>>("bxt_ch_set_vel_angles");
	wrapper::Add<
		Cmd_BXT_Set_Angles,
		Handler<float, float>,
		Handler<float, float, float>>("bxt_set_angles");
	wrapper::Add<Cmd_BXT_Get_Server_Time, Handler<>>("bxt_get_server_time");
	wrapper::Add<
		Cmd_Multiwait,
		Handler<>,
		Handler<int>>("w");
	wrapper::Add<Cmd_BXT_Set_Skybox, Handler<const char*>>("bxt_set_skybox");
	wrapper::Add<Cmd_BXT_Camera_Fixed, Handler<float, float, float, float, float, float>>("bxt_cam_fixed");
	wrapper::Add<Cmd_BXT_Camera_Offset, Handler<float, float, float, float, float, float>>("bxt_cam_offset");
	wrapper::Add<Cmd_BXT_Camera_Clear, Handler<>>("bxt_cam_clear");
	wrapper::Add<Cmd_BXT_Timer_Start, Handler<>>("bxt_timer_start");
	wrapper::Add<Cmd_BXT_Timer_Stop, Handler<>>("bxt_timer_stop");
	wrapper::Add<Cmd_BXT_Timer_Reset, Handler<>>("bxt_timer_reset");
	wrapper::Add<Cmd_BXT_Get_Origin_And_Angles, Handler<>>("bxt_get_pos");
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
	wrapper::Add<Cmd_BXT_SetPause, Handler<>>("bxt_setpause");
	wrapper::Add<Cmd_BXT_UnPause, Handler<>>("bxt_unpause");
	wrapper::Add<Cmd_BXT_Interprocess_Reset, Handler<>>("_bxt_interprocess_reset");
	wrapper::Add<Cmd_BXT_Interprocess_Stop, Handler<>>("_bxt_interprocess_stop");
	wrapper::Add<Cmd_BXT_Set_Frametime_Remainder, Handler<double>>("_bxt_set_frametime_remainder");
	wrapper::Add<Cmd_BXT_Reset_Frametime_Remainder, Handler<>>("_bxt_reset_frametime_remainder");
	wrapper::Add<Cmd_BXT_TASLog, Handler<int>>("bxt_taslog");
	wrapper::Add<Cmd_BXT_Append, Handler<const char *>>("bxt_append");
	wrapper::Add<Cmd_BXT_FreeCam, Handler<int>>("bxt_freecam");
	wrapper::Add<Cmd_BXT_Print_Entities, Handler<>, Handler<const char*>, Handler<const char*, const char*>>("bxt_print_entities");
	wrapper::Add<Cmd_BXT_Print_Entities_By_Index, Handler<int>, Handler<int, int>>("bxt_print_entities_by_index");

	wrapper::Add<Cmd_BXT_TAS_Editor_Resimulate, Handler<>>("bxt_tas_editor_resimulate");
	wrapper::Add<Cmd_BXT_TAS_Editor_Apply_Smoothing, Handler<>>("bxt_tas_editor_apply_smoothing");
	wrapper::Add<Cmd_BXT_TAS_Optim_Init, Handler<>>("bxt_tas_optim_init");
	wrapper::Add<Cmd_BXT_TAS_Editor_Unset_Pitch, Handler<>>("bxt_tas_editor_unset_pitch");
	wrapper::Add<Cmd_BXT_TAS_Editor_Unset_Yaw, Handler<>>("bxt_tas_editor_unset_yaw");
	wrapper::Add<Cmd_BXT_TAS_Editor_Set_Commands, Handler<const char*>>("bxt_tas_editor_set_commands");
	wrapper::Add<Cmd_BXT_TAS_Editor_Set_Repeats, Handler<int>>("bxt_tas_editor_set_repeats");
	wrapper::Add<Cmd_BXT_TAS_Editor_Set_Frametime, Handler<const char*>>("bxt_tas_editor_set_frametime");
	wrapper::Add<Cmd_BXT_TAS_Editor_Set_Pitch, Handler<float>>("bxt_tas_editor_set_pitch");
	wrapper::Add<Cmd_BXT_TAS_Editor_Set_Yaw, Handler<float>>("bxt_tas_editor_set_yaw");
	wrapper::Add<Cmd_BXT_TAS_Editor_Set_Left_Right_Count, Handler<unsigned long>>("bxt_tas_editor_set_left_right_count");
	wrapper::Add<Cmd_BXT_TAS_Editor_Toggle, Handler<const char*>>("bxt_tas_editor_toggle");
	wrapper::Add<Cmd_BXT_TAS_Editor_Set_Change_Type, Handler<const char*>>("bxt_tas_editor_set_change_type");
	wrapper::Add<Cmd_BXT_TAS_Editor_Set_Target_Yaw_Type, Handler<const char*>>("bxt_tas_editor_set_target_yaw_type");
	wrapper::Add<Cmd_BXT_TAS_Editor_Set_Run_Point_And_Save, Handler<>>("bxt_tas_editor_set_run_point_and_save");
	wrapper::Add<Cmd_BXT_TAS_Editor_Delete_Last_Point, Handler<>>("bxt_tas_editor_delete_last_point");
	wrapper::Add<Cmd_BXT_TAS_Editor_Delete_Point, Handler<>>("bxt_tas_editor_delete_point");
	wrapper::Add<Cmd_BXT_TAS_Editor_Insert_Point, Handler<>>("bxt_tas_editor_insert_point");
	wrapper::Add<Cmd_Plus_BXT_TAS_Editor_Insert_Point, Handler<>, Handler<int>>("+bxt_tas_editor_insert_point");
	wrapper::Add<Cmd_Minus_BXT_TAS_Editor_Insert_Point, Handler<>, Handler<int>>("-bxt_tas_editor_insert_point");
	wrapper::Add<Cmd_BXT_TAS_Editor_Save, Handler<>>("bxt_tas_editor_save");
	wrapper::Add<Cmd_Plus_BXT_TAS_Editor_Look_Around, Handler<>, Handler<int>>("+bxt_tas_editor_look_around");
	wrapper::Add<Cmd_Minus_BXT_TAS_Editor_Look_Around, Handler<>, Handler<int>>("-bxt_tas_editor_look_around");
	wrapper::Add<Cmd_BXT_TAS_Editor, Handler<int>>("bxt_tas_editor");

	wrapper::Add<Cmd_BXT_TAS_Become_Simulator_Client, Handler<>>("bxt_tas_become_simulator_client");
	wrapper::Add<Cmd_BXT_TAS_Server_Send_Command, Handler<const char*>>("_bxt_tas_server_send_command");
	wrapper::Add<Cmd_BXT_TAS_Client_Load_Received_Script, Handler<>>("_bxt_tas_client_load_received_script");

	wrapper::Add<Cmd_BXT_Show_Bullets_Clear, Handler<>>("bxt_show_bullets_clear");
	wrapper::Add<Cmd_BXT_Show_Bullets_Enemy_Clear, Handler<>>("bxt_show_bullets_enemy_clear");

	wrapper::Add<Cmd_BXT_Split, Handler<const char*>>("bxt_split");
	wrapper::Add<Cmd_BXT_Splits_Add_Entity, Handler<const char*>, Handler<const char*, const char*>>("bxt_splits_add_entity");
	wrapper::Add<
		Cmd_BXT_Splits_Add_Trigger,
		Handler<float, float, float, float, float, float>,
		Handler<float, float, float, float, float, float, const char*>,
		Handler<float, float, float, float, float, float, const char*, const char*>>("bxt_splits_add_trigger");
	wrapper::Add<Cmd_BXT_Splits_Clear, Handler<>>("bxt_splits_clear");
	wrapper::Add<Cmd_BXT_Splits_Delete, Handler<>, Handler<const char*>>("bxt_splits_delete");
	wrapper::Add<Cmd_BXT_Splits_Export, Handler<const char*>>("bxt_splits_export");
	wrapper::Add<Cmd_BXT_Splits_List, Handler<>, Handler<const char*>>("bxt_splits_list");
	wrapper::Add<Cmd_BXT_Splits_Print_Times, Handler<>>("bxt_splits_print_times");
	wrapper::Add<Cmd_BXT_Splits_Set_Map, Handler<const char*>, Handler<const char*, const char*>>("bxt_splits_set_map");
	wrapper::Add<Cmd_BXT_Splits_Set_Name, Handler<const char*>, Handler<const char*, const char*>>("bxt_splits_set_name");
	wrapper::Add<Cmd_BXT_Splits_Track_Horizontal_Speed, Handler<int>, Handler<const char*, int>>("bxt_splits_track_horizontal_speed");
	wrapper::Add<Cmd_BXT_Splits_Track_Vertical_Speed, Handler<int>, Handler<const char*, int>>("bxt_splits_track_vertical_speed");
	wrapper::Add<Cmd_BXT_Splits_Track_X, Handler<int>, Handler<const char*, int>>("bxt_splits_track_x");
	wrapper::Add<Cmd_BXT_Splits_Track_Y, Handler<int>, Handler<const char*, int>>("bxt_splits_track_y");
	wrapper::Add<Cmd_BXT_Splits_Track_Z, Handler<int>, Handler<const char*, int>>("bxt_splits_track_z");
	wrapper::Add<Cmd_BXT_Splits_Place_Down, Handler<>, Handler<const char*>>("+bxt_splits_place");
	wrapper::Add<Cmd_BXT_Splits_Place_Up, Handler<>, Handler<const char*>>("-bxt_splits_place");
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

				if (svs->maxclients >= 1) {
					edict_t *pl = GetPlayerEdict();
					if (pl) {
						player.Origin[0] = pl->v.origin[0];
						player.Origin[1] = pl->v.origin[1];
						player.Origin[2] = pl->v.origin[2];
						player.Velocity[0] = pl->v.velocity[0];
						player.Velocity[1] = pl->v.velocity[1];
						player.Velocity[2] = pl->v.velocity[2];
						player.Ducking = (pl->v.flags & FL_DUCKING) != 0;
						#ifndef SDK10_BUILD
							player.InDuckAnimation = (pl->v.bInDuck != 0);
							player.DuckTime = static_cast<float>(pl->v.flDuckTime);
							player.StaminaTime = pl->v.fuser2;
						#else
							player.InDuckAnimation = false;
							player.DuckTime = 0.0f;
							player.StaminaTime = 0.0f;
						#endif
						player.Walking = (pl->v.movetype == MOVETYPE_WALK);

						if (ORIG_PF_GetPhysicsKeyValue) {
							auto slj = std::atoi(ORIG_PF_GetPhysicsKeyValue(pl, "slj"));
							player.HasLJModule = (slj == 1);
						} else {
							player.HasLJModule = false;
						}

						// Hope the viewangles aren't changed in ClientDLL's HUD_UpdateClientData() (that happens later in Host_Frame()).
						ClientDLL::GetInstance().GetViewAngles(player.Viewangles);
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

				const auto movement_vars = GetMovementVars();

				std::array<simulation_ipc::PushableInfo, 25> pushables{};

				const auto obbo_pushable = ServerDLL::GetInstance().obboPushable;
				ServerDLL::GetInstance().obboPushable = nullptr;

				if (simulation_ipc::is_client_initialized()) {
					size_t i = 0;

					edict_t *edicts;
					const int numEdicts = GetEdicts(&edicts);
					for (int e = 0; e < numEdicts; ++e) {
						const edict_t *ent = edicts + e;
						if (!IsValidEdict(ent))
							continue;

						const entvars_t *pev = &(ent->v);
						if (std::strcmp(GetString(pev->classname), "func_pushable") != 0)
							continue;

						pushables[i].index = reinterpret_cast<uintptr_t>(pev);

						Vector origin = pev->origin + ((pev->mins + pev->maxs) / 2.f);
						pushables[i].origin[0] = origin[0];
						pushables[i].origin[1] = origin[1];
						pushables[i].origin[2] = origin[2];

						pushables[i].water_level = pev->waterlevel;
						pushables[i].did_obbo = (pev == obbo_pushable);

						if (++i == pushables.size())
							break;
					}
				}

				StrafeState.TargetYawLookAtOrigin[0] = 0;
				StrafeState.TargetYawLookAtOrigin[1] = 0;
				StrafeState.TargetYawLookAtOrigin[2] = 0;
				if (StrafeState.Parameters.Parameters.LookAt.Entity > 0) {
					edict_t *edicts;
					const int numEdicts = GetEdicts(&edicts);

					if (StrafeState.Parameters.Parameters.LookAt.Entity >= (unsigned int) numEdicts) {
						StrafeState.Parameters.Parameters.LookAt.Entity = 0;
					} else {				
						const edict_t *ent = edicts + StrafeState.Parameters.Parameters.LookAt.Entity;
						const entvars_t *pev = &(ent->v);
						Vector origin = pev->origin + ((pev->mins + pev->maxs) / 2.f);

						StrafeState.TargetYawLookAtOrigin[0] = origin[0];
						StrafeState.TargetYawLookAtOrigin[1] = origin[1];
						StrafeState.TargetYawLookAtOrigin[2] = origin[2];
					}
				}

				simulation_ipc::send_simulated_frame_to_server(simulation_ipc::SimulatedFrame {
					CVars::_bxt_tas_script_generation.GetUint(),
					movementFrameCounter++,
					player,
					StrafeState,
					PrevFractions,
					PrevNormalzs,
					thisFrameIs0ms,
					movement_vars.Frametime,
					health,
					armor,
					pushables,
				});

				StartTracing();
				auto p = HLStrafe::MainFunc(
					player,
					movement_vars,
					f,
					StrafeState,
					Buttons,
					ButtonsPresent,
					std::bind(&HwDLL::UnsafePlayerTrace, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
					std::bind(*(ClientDLL::GetInstance().pEngfuncs->PM_PointContents), std::placeholders::_1, nullptr),
					hlstrafe_version);
				StopTracing();

				PrevFractions = { p.fractions[0], p.fractions[1], p.fractions[2], p.fractions[3] };
				PrevNormalzs = { p.normalzs[0], p.normalzs[1], p.normalzs[2], p.normalzs[3] };

				if (TargetYawOverrideIndex == TargetYawOverrides.size()) {
					TargetYawOverrides.clear();
					TargetYawOverrideIndex = 0;
				}

				if (TargetYawOverrides.empty()) {
					StrafeState.TargetYawOverrideActive = false;
				} else {
					StrafeState.TargetYawOverride = TargetYawOverrides[TargetYawOverrideIndex++];
					StrafeState.TargetYawOverrideActive = true;
				}

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
				case HLTAS::ChangeTarget::TARGET_YAW_OFFSET:
					StrafeState.ChangeTargetYawOffsetValue = f.GetChangeFinalValue();
					StrafeState.ChangeTargetYawOffsetOver = f.GetChangeOver();
					break;
				default:
					assert(false);
					break;
				}
			} else if (!f.TargetYawOverride.empty()) {
				TargetYawOverrides = f.TargetYawOverride;
				StrafeState.TargetYawOverrideActive = true;
				StrafeState.TargetYawOverride = TargetYawOverrides[0];
				TargetYawOverrideIndex = 1;
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
			if (svs->maxclients >= 1) {
				edict_t *pl = GetPlayerEdict();
				if (pl) {
					player.Origin[0] = pl->v.origin[0];
					player.Origin[1] = pl->v.origin[1];
					player.Origin[2] = pl->v.origin[2];
					player.Velocity[0] = pl->v.velocity[0];
					player.Velocity[1] = pl->v.velocity[1];
					player.Velocity[2] = pl->v.velocity[2];
					player.Ducking = (pl->v.flags & FL_DUCKING) != 0;
					#ifndef SDK10_BUILD
						player.InDuckAnimation = (pl->v.bInDuck != 0);
						player.DuckTime = static_cast<float>(pl->v.flDuckTime);
						player.StaminaTime = pl->v.fuser2;
					#else
						player.InDuckAnimation = false;
						player.DuckTime = 0.0f;
						player.StaminaTime = 0.0f;
					#endif
					player.Walking = (pl->v.movetype == MOVETYPE_WALK);

					if (ORIG_PF_GetPhysicsKeyValue) {
						auto slj = std::atoi(ORIG_PF_GetPhysicsKeyValue(pl, "slj"));
						player.HasLJModule = (slj == 1);
					} else {
						player.HasLJModule = false;
					}

					// Hope the viewangles aren't changed in ClientDLL's HUD_UpdateClientData() (that happens later in Host_Frame()).
					ClientDLL::GetInstance().GetViewAngles(player.Viewangles);
					//ORIG_Con_Printf("Player viewangles: %f %f %f\n", player.Viewangles[0], player.Viewangles[1], player.Viewangles[2]);
				}
			}

			bool Duck = false, Jump = false;

			auto playerCopy = HLStrafe::PlayerData(player); // Our copy that we will mess with.
			auto traceFunc = std::bind(&HwDLL::PlayerTrace, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, false);
			auto pointContentsFunc = std::bind(*(ClientDLL::GetInstance().pEngfuncs->PM_PointContents), std::placeholders::_1, nullptr);
			auto postype = GetPositionType(playerCopy, traceFunc, pointContentsFunc, HLStrafe::MAX_SUPPORTED_VERSION);
			if (ducktap && postype == HLStrafe::PositionType::GROUND && (autojump == false || (autojump == true && !CVars::bxt_tas_autojump_priority.GetBool()))) {
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
			} else if (autojump && !currentKeys.Jump.IsDown() && (postype != HLStrafe::PositionType::AIR || !player.Walking)) {
				Jump = true;
			} else if (jumpbug && postype == HLStrafe::PositionType::AIR) {
				if (player.Ducking) {
					// Predict what will happen if we unduck.
					playerCopy.Ducking = false;
					playerCopy.InDuckAnimation = false;
					playerCopy.DuckTime = 0;

					auto nextPostype = HLStrafe::GetPositionType(playerCopy, traceFunc, pointContentsFunc, HLStrafe::MAX_SUPPORTED_VERSION);
					if (nextPostype == HLStrafe::PositionType::GROUND) {
						// Jumpbug if we're about to land.
						Jump = true;
						Duck = false;
					}
				} else {
					auto vars = GetMovementVars();
					auto nextPostype = HLStrafe::Move(playerCopy, vars, postype, vars.Maxspeed, traceFunc, pointContentsFunc, HLStrafe::MAX_SUPPORTED_VERSION);
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
			} else if (ducktap && (autojump == false || (autojump == true && !CVars::bxt_tas_autojump_priority.GetBool()))) {
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
	#ifndef SDK10_BUILD
		player.InDuckAnimation = (pl->v.bInDuck != 0);
		player.DuckTime = static_cast<float>(pl->v.flDuckTime);
		player.StaminaTime = pl->v.fuser2;
	#else
		player.InDuckAnimation = false;
		player.DuckTime = 0.0f;
		player.StaminaTime = 0.0f;
	#endif
	player.Walking = (pl->v.movetype == MOVETYPE_WALK);

	if (ORIG_PF_GetPhysicsKeyValue) {
		auto slj = std::atoi(ORIG_PF_GetPhysicsKeyValue(pl, "slj"));
		player.HasLJModule = (slj == 1);
	} else {
		player.HasLJModule = false;
	}

	ClientDLL::GetInstance().GetViewAngles(player.Viewangles);

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
	auto &cl = ClientDLL::GetInstance();

	FindCVarsIfNeeded();
	vars.Frametime = GetFrameTime();
	vars.Maxvelocity = CVars::sv_maxvelocity.GetFloat();
	vars.Stopspeed = CVars::sv_stopspeed.GetFloat();
	vars.Friction = CVars::sv_friction.GetFloat();
	vars.Edgefriction = CVars::edgefriction.GetFloat();
	vars.Accelerate = CVars::sv_accelerate.GetFloat();
	vars.Airaccelerate = CVars::sv_airaccelerate.GetFloat();
	vars.Gravity = CVars::sv_gravity.GetFloat();
	vars.Stepsize = CVars::sv_stepsize.GetFloat();
	vars.Bounce = CVars::sv_bounce.GetFloat();
	vars.Bhopcap = CVars::bxt_bhopcap.GetBool();

	static bool is_paranoia = cl.DoesGameDirMatch("paranoia");
	static bool is_cstrike = cl.DoesGameDirMatch("cstrike");
	static bool is_czero = cl.DoesGameDirMatch("czero");
	static bool is_tfc = cl.DoesGameDirMatch("tfc");

	if (is_paranoia)
		vars.Maxspeed = cl.pEngfuncs->GetClientMaxspeed() * CVars::sv_maxspeed.GetFloat() / 100.0f; // GetMaxSpeed is factor here
	else if (pEngStudio && cl.pEngfuncs && (cl.pEngfuncs->GetClientMaxspeed() > 0.0f) && (CVars::sv_maxspeed.GetFloat() > cl.pEngfuncs->GetClientMaxspeed()))
		vars.Maxspeed = cl.pEngfuncs->GetClientMaxspeed(); // Get true maxspeed in other mods (example: CS 1.6)
	else
		vars.Maxspeed = CVars::sv_maxspeed.GetFloat();

	if (is_cstrike || is_czero) {
		vars.BhopcapMultiplier = 0.8f;
		vars.BhopcapMaxspeedScale = 1.2f;
		vars.HasStamina = !CVars::bxt_remove_stamina.GetBool();
		vars.DuckTapSlow = true;
	} else {
		vars.BhopcapMultiplier = 0.65f;
		vars.BhopcapMaxspeedScale = 1.7f;
	}

	if (!is_cstrike && !is_czero && !is_tfc)
		vars.UseSlow = true;

	if (svs->maxclients >= 1) {
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

const char* HwDLL::GetMovetypeName(int moveType)
{
	switch (moveType)
	{
		case MOVETYPE_NONE:             return "None";
		case MOVETYPE_WALK:             return "Walk";
		case MOVETYPE_STEP:             return "Step";
		case MOVETYPE_FLY:              return "Fly";
		case MOVETYPE_TOSS:             return "Toss";
		case MOVETYPE_PUSH:             return "Push";
		case MOVETYPE_NOCLIP:           return "Noclip";
		case MOVETYPE_FLYMISSILE:       return "Fly-missile";
		case MOVETYPE_BOUNCE:           return "Bounce";
		case MOVETYPE_BOUNCEMISSILE:    return "Bounce-missile";
		case MOVETYPE_FOLLOW:           return "Follow";
		case MOVETYPE_PUSHSTEP:         return "Push-step";
		default:                        return "Unknown";
	}
}

const char *HwDLL::GetSolidName(int solid)
{
	switch (solid)
	{
		case SOLID_NOT:                 return "Not";
		case SOLID_TRIGGER:             return "Trigger";
		case SOLID_BBOX:                return "Bounding-box";
		case SOLID_SLIDEBOX:            return "Slide-box";
		case SOLID_BSP:                 return "BSP";
		default:                        return "Unknown";
	}
}

const char *HwDLL::GetRenderModeName(int rendermode)
{
	switch (rendermode)
	{
		case kRenderNormal:             return "Normal";
		case kRenderTransColor:         return "Transparent-color";
		case kRenderTransTexture:       return "Transparent-texture";
		case kRenderGlow:               return "Glow";
		case kRenderTransAlpha:         return "Transparent-alpha";
		case kRenderTransAdd:           return "Transparent-add";
		default:                        return "Unknown";
	}
}

const char *HwDLL::GetRenderFxName(int renderfx)
{
	switch (renderfx)
	{
		case kRenderFxNone:             return "None";
		case kRenderFxPulseSlow:        return "Pulse-slow";
		case kRenderFxPulseFast:        return "Pulse-fast";
		case kRenderFxPulseSlowWide:    return "Pulse-slow-wide";
		case kRenderFxPulseFastWide:    return "Pulse-fast-wide";
		case kRenderFxFadeSlow:         return "Fade-slow";
		case kRenderFxFadeFast:         return "Fade-fast";
		case kRenderFxSolidSlow:        return "Solid-slow";
		case kRenderFxSolidFast:        return "Solid-fast";
		case kRenderFxStrobeSlow:       return "Strobe-slow";
		case kRenderFxStrobeFast:       return "Strobe-fast";
		case kRenderFxStrobeFaster:     return "Strobe-faster";
		case kRenderFxFlickerSlow:      return "Flicker-slow";
		case kRenderFxFlickerFast:      return "Flicker-fast";
		case kRenderFxNoDissipation:    return "No dissipation";
		case kRenderFxDistort:          return "Distort";
		case kRenderFxHologram:         return "Hologram";
		case kRenderFxDeadPlayer:       return "Dead player";
		case kRenderFxExplode:          return "Explode";
		case kRenderFxGlowShell:        return "Glowing shell";
		case kRenderFxClampMinScale:    return "Clamp min scale";
		case kRenderFxLightMultiplier:  return "Light multiplier";
		default:                        return "Unknown";
	}
}

void HwDLL::GetFlags(std::ostringstream &out, int index)
{
	const auto& hw = HwDLL::GetInstance();
	edict_t* edicts;
	hw.GetEdicts(&edicts);
	const edict_t* ent = edicts + index;

	out << "Flags: ";
	if (ent->v.flags & FL_FLY)
		out << "FL_FLY; ";
	if (ent->v.flags & FL_SWIM)
		out << "FL_SWIM; ";
	if (ent->v.flags & FL_CONVEYOR)
		out << "FL_CONVEYOR; ";
	if (ent->v.flags & FL_INWATER)
		out << "FL_INWATER; ";
	if (ent->v.flags & FL_MONSTER)
		out << "FL_MONSTER; ";
	if (ent->v.flags & FL_GODMODE)
		out << "FL_GODMODE; ";
	if (ent->v.flags & FL_NOTARGET)
		out << "FL_NOTARGET; ";
	if (ent->v.flags & FL_SKIPLOCALHOST)
		out << "FL_SKIPLOCALHOST; ";
	if (ent->v.flags & FL_ONGROUND)
		out << "FL_ONGROUND; ";
	if (ent->v.flags & FL_PARTIALGROUND)
		out << "FL_PARTIALGROUND; ";
	if (ent->v.flags & FL_WATERJUMP)
		out << "FL_WATERJUMP; ";
	if (ent->v.flags & FL_FROZEN)
		out << "FL_FROZEN; ";
	if (ent->v.flags & FL_FAKECLIENT)
		out << "FL_FAKECLIENT; ";
	if (ent->v.flags & FL_DUCKING)
		out << "FL_DUCKING; ";
	if (ent->v.flags & FL_FLOAT)
		out << "FL_FLOAT; ";
	if (ent->v.flags & FL_GRAPHED)
		out << "FL_GRAPHED; ";
	if (ent->v.flags & FL_IMMUNE_WATER)
		out << "FL_IMMUNE_WATER; ";
	if (ent->v.flags & FL_IMMUNE_SLIME)
		out << "FL_IMMUNE_SLIME; ";
	if (ent->v.flags & FL_IMMUNE_LAVA)
		out << "FL_IMMUNE_LAVA; ";
	if (ent->v.flags & FL_PROXY)
		out << "FL_PROXY; ";
	if (ent->v.flags & FL_ALWAYSTHINK)
		out << "FL_ALWAYSTHINK; ";
	if (ent->v.flags & FL_BASEVELOCITY)
		out << "FL_BASEVELOCITY; ";
	if (ent->v.flags & FL_MONSTERCLIP)
		out << "FL_MONSTERCLIP; ";
	if (ent->v.flags & FL_ONTRAIN)
		out << "FL_ONTRAIN; ";
	if (ent->v.flags & FL_WORLDBRUSH)
		out << "FL_WORLDBRUSH; ";
	if (ent->v.flags & FL_SPECTATOR)
		out << "FL_SPECTATOR; ";
	if (ent->v.flags & FL_CUSTOMENTITY)
		out << "FL_CUSTOMENTITY; ";
	if (ent->v.flags & FL_KILLME)
		out << "FL_KILLME; ";
	if (ent->v.flags & FL_DORMANT)
		out << "FL_DORMANT; ";
	out << '\n';
}

void HwDLL::GetButton(std::ostringstream &out, int index)
{
	const auto& hw = HwDLL::GetInstance();
	edict_t* edicts;
	hw.GetEdicts(&edicts);
	const edict_t* ent = edicts + index;

	out << "Button: ";
	if (ent->v.button & IN_JUMP)
		out << "IN_JUMP; ";
	if (ent->v.button & IN_DUCK)
		out << "IN_DUCK; ";
	if (ent->v.button & IN_FORWARD)
		out << "IN_FORWARD; ";
	if (ent->v.button & IN_BACK)
		out << "IN_BACK; ";
	if (ent->v.button & IN_MOVELEFT)
		out << "IN_MOVELEFT; ";
	if (ent->v.button & IN_MOVERIGHT)
		out << "IN_MOVERIGHT; ";
	if (ent->v.button & IN_ATTACK)
		out << "IN_ATTACK; ";
	if (ent->v.button & IN_ATTACK2)
		out << "IN_ATTACK2; ";
	if (ent->v.button & IN_RELOAD)
		out << "IN_RELOAD; ";
	if (ent->v.button & IN_USE)
		out << "IN_USE; ";
	if (ent->v.button & IN_LEFT)
		out << "IN_LEFT; ";
	if (ent->v.button & IN_RIGHT)
		out << "IN_RIGHT; ";
	out << '\n';
}

void HwDLL::GetEffects(std::ostringstream &out, int index)
{
	const auto& hw = HwDLL::GetInstance();
	edict_t* edicts;
	hw.GetEdicts(&edicts);
	const edict_t* ent = edicts + index;

	out << "Effects: ";
	if (ent->v.effects & EF_BRIGHTFIELD)
		out << "EF_BRIGHTFIELD; ";
	if (ent->v.effects & EF_MUZZLEFLASH)
		out << "EF_MUZZLEFLASH; ";
	if (ent->v.effects & EF_BRIGHTLIGHT)
		out << "EF_BRIGHTLIGHT; ";
	if (ent->v.effects & EF_DIMLIGHT)
		out << "EF_DIMLIGHT; ";
	if (ent->v.effects & EF_INVLIGHT)
		out << "EF_INVLIGHT; ";
	if (ent->v.effects & EF_NOINTERP)
		out << "EF_NOINTERP; ";
	if (ent->v.effects & EF_LIGHT)
		out << "EF_LIGHT; ";
	if (ent->v.effects & EF_NODRAW)
		out << "EF_NODRAW; ";
	out << '\n';
}

HOOK_DEF_0(HwDLL, void, __cdecl, Cbuf_Execute)
{
	RegisterCVarsAndCommandsIfNeeded();

	UpdateCustomTriggersAndSplits();

	int *state = reinterpret_cast<int*>(cls);
	int *paused = reinterpret_cast<int*>(psv)+1;
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
	if (simulation_ipc::is_client_initialized()
			&& simulation_ipc::message.command[0] != 0
			// Starting a TAS in states 2, 3, 4 (loading) leads to crashes or desyncs.
			&& (*state == 5 || *state == 1)) {
		ORIG_Cbuf_AddText(simulation_ipc::message.command);
		simulation_ipc::message.command[0] = 0;
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
			if (is_won_build)
				ORIG_Cbuf_InsertText("bxt_setpause\n");
			else
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

	ClientDLL::GetInstance().SetAngleSpeedCap(CVars::bxt_anglespeed_cap.GetBool());

	ClientDLL::GetInstance().SetSpeedScaling(CVars::bxt_speed_scaling.GetBool());

	ServerDLL::GetInstance().SetStamina(CVars::bxt_remove_stamina.GetBool());

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

bool HwDLL::TryGettingAccurateInfo(float origin[3], float velocity[3], float& health, float& armorvalue, int& waterlevel, float& stamina)
{
	if (!svs || svs->maxclients < 1)
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
	armorvalue = pl->v.armorvalue;
	waterlevel = pl->v.waterlevel;

	auto& sv = ServerDLL::GetInstance();
	if (sv.is_cof) {
		edict_t *edicts;
		GetEdicts(&edicts);
		int num = 1;
		edict_t *ent = edicts + num;
		bool is_ent_failed = CheckIfEntityIsValidAndPlayer(ent, num, true, false);
		if (is_ent_failed)
		{
			stamina = 0.0f;
			return true;
		}

		void* classPtr = ent->v.pContainingEntity->pvPrivateData;
		uintptr_t thisAddr = reinterpret_cast<uintptr_t>(classPtr);
		float* m_fStamina = reinterpret_cast<float*>(thisAddr + sv.offm_fStamina);
		stamina = *m_fStamina;
	} else {
		#ifndef SDK10_BUILD
			stamina = pl->v.fuser2;
		#else
			stamina = 0.0f;
		#endif
	}

	return true;
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
	if (!ORIG_PM_PlayerTrace || svs->maxclients < 1) {
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
	if (!ORIG_PM_PlayerTrace || svs->maxclients < 1) {
		return;
	}

	*ppmove = trace_oldmove;
	*sv_player = trace_oldplayer;
	*host_client = trace_oldclient;
}

HLStrafe::TraceResult HwDLL::UnsafePlayerTrace(const float start[3], const float end[3], HLStrafe::HullType hull) {
	auto tr = HLStrafe::TraceResult{};

	if (!ORIG_PM_PlayerTrace || svs->maxclients < 1) {
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

	if (psv && offMaxEdicts) {
		const int maxEdicts = *reinterpret_cast<int *>(reinterpret_cast<uintptr_t>(psv) + offMaxEdicts);
		RuntimeData::Add(RuntimeData::Edicts{ maxEdicts });
	}

	auto &hw = HwDLL::GetInstance();

	edict_t *edicts;
	GetEdicts(&edicts);
	int num = 1;
	edict_t *ent = edicts + num;
	bool is_ent_failed = CheckIfEntityIsValidAndPlayer(ent, num, true, false);

	if (is_ent_failed)
		lastRecordedHealth = 0;
	else
		lastRecordedHealth = static_cast<int>((*hw.sv_player)->v.health);

	RuntimeData::Add(RuntimeData::PlayerHealth{lastRecordedHealth});

	// Initial BXT timer value.
	CustomHud::SaveTimeToDemo();
}

void HwDLL::UpdateCustomTriggersAndSplits()
{
	if (!svs || svs->maxclients < 1)
		return;

	edict_t *pl = GetPlayerEdict();
	if (!pl)
		return;

	CustomTriggers::Update(pl->v.origin, (pl->v.flags & FL_DUCKING) != 0);
	Splits::Update(pl->v.origin, (pl->v.flags & FL_DUCKING) != 0);
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
	if (insideSStartDynamicSound && runningFrames)
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

	auto &hw = HwDLL::GetInstance();

	if (pHost_FilterTime_FPS_Cap_Byte)
	{
		const auto pByte = *reinterpret_cast<byte*>(pHost_FilterTime_FPS_Cap_Byte);
		static bool is_0x7E_on_init = (pByte == 0x7E);

		if (CVars::bxt_remove_fps_limit.GetBool() && ((pByte == 0x7E && CVars::sv_cheats.GetBool()) || pByte == 0x75))
			MemUtils::ReplaceBytes(reinterpret_cast<void*>(pHost_FilterTime_FPS_Cap_Byte), 1, reinterpret_cast<const byte*>("\xEB"));

		if (pByte == 0xEB && ((is_0x7E_on_init && !CVars::sv_cheats.GetBool()) || !CVars::bxt_remove_fps_limit.GetBool()))
		{
			if (is_0x7E_on_init)
				MemUtils::ReplaceBytes(reinterpret_cast<void*>(pHost_FilterTime_FPS_Cap_Byte), 1, reinterpret_cast<const byte*>("\x7E"));
			else
				MemUtils::ReplaceBytes(reinterpret_cast<void*>(pHost_FilterTime_FPS_Cap_Byte), 1, reinterpret_cast<const byte*>("\x75"));
		}
	}

	if (pHost_FilterTime_Check_72FPS && pHost_FilterTime_Set_72FPS)
	{
		const auto pByte1 = *reinterpret_cast<byte*>(pHost_FilterTime_Check_72FPS);
		const auto pByte2 = *reinterpret_cast<byte*>(pHost_FilterTime_Set_72FPS);

		if (CVars::bxt_remove_fps_limit.GetBool() && ((pByte1 == 0x00) && (pByte2 == 0x00)))
		{
			if (is_bshift_won)
				MemUtils::ReplaceBytes(reinterpret_cast<void*>(pHost_FilterTime_Check_72FPS), 5, reinterpret_cast<const byte*>("\x20\x5F\xA0\x02\x42"));
			else
				MemUtils::ReplaceBytes(reinterpret_cast<void*>(pHost_FilterTime_Check_72FPS), 9, reinterpret_cast<const byte*>("\x20\x5F\xA0\x02\x42\xF9\x02\x15\x50"));
			MemUtils::ReplaceBytes(reinterpret_cast<void*>(pHost_FilterTime_Set_72FPS), 4, reinterpret_cast<const byte*>("\xF9\x02\x15\x50"));
		}

		if (!CVars::bxt_remove_fps_limit.GetBool() && ((pByte1 == 0x20) && (pByte2 == 0xF9)))
		{
			if (is_bshift_won)
				MemUtils::ReplaceBytes(reinterpret_cast<void*>(pHost_FilterTime_Check_72FPS), 5, reinterpret_cast<const byte*>("\x00\x00\x00\x52\x40"));
			else
				MemUtils::ReplaceBytes(reinterpret_cast<void*>(pHost_FilterTime_Check_72FPS), 9, reinterpret_cast<const byte*>("\x00\x00\x00\x52\x40\x00\x00\x90\x42"));
			MemUtils::ReplaceBytes(reinterpret_cast<void*>(pHost_FilterTime_Set_72FPS), 4, reinterpret_cast<const byte*>("\x00\x00\x90\x42"));
		}
	}

	if (IsRecordingDemo())
	{
		int playerhealth = 0; 

		edict_t *edicts;
		GetEdicts(&edicts);
		int num = 1;
		edict_t *ent = edicts + num;
		bool is_ent_failed = CheckIfEntityIsValidAndPlayer(ent, num, true, false);

		if (!is_ent_failed)
			playerhealth = static_cast<int>((*hw.sv_player)->v.health);

		if (playerhealth != lastRecordedHealth)
			RuntimeData::Add(RuntimeData::PlayerHealth{playerhealth});

		lastRecordedHealth = playerhealth;
	}

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

HOOK_DEF_3(HwDLL, void, __cdecl, V_ApplyShake, float*, origin, float*, angles, float, factor)
{
	if (CVars::bxt_shake_remove.GetBool() && CVars::sv_cheats.GetBool())
		return;

	ORIG_V_ApplyShake(origin, angles, factor);
}

HOOK_DEF_0(HwDLL, void, __cdecl, R_DrawSkyBox)
{
	if (CVars::bxt_skybox_remove.GetBool() || (CVars::sv_cheats.GetBool() && CVars::bxt_wallhack.GetBool()))
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

	if (insideHost_Changelevel2_f && !is_won_build) {
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
		const bool paused = *(reinterpret_cast<const int *>(psv) + 1) != 0;
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

HOOK_DEF_1(HwDLL, void, __cdecl, Cbuf_InsertText, const char*, text)
{
	if (insideExec)
		execScript += text;

	ORIG_Cbuf_InsertText(text);
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
	if (CVars::bxt_remove_viewmodel.GetBool())
		return;

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

HOOK_DEF_0(HwDLL, void, __cdecl, R_PreDrawViewModel)
{
	if (CVars::bxt_remove_viewmodel.GetBool())
		return;

	ORIG_R_PreDrawViewModel();
}

HOOK_DEF_0(HwDLL, void, __cdecl, R_Clear)
{
	// This is needed or everything will look washed out or with unintended
	// motion blur.
	if (CVars::bxt_water_remove.GetBool() || CVars::bxt_force_clear.GetBool() || CVars::bxt_disable_world.GetBool() || CVars::bxt_skybox_remove.GetBool() || (CVars::sv_cheats.GetBool() && (CVars::bxt_wallhack.GetBool()))) {
		if (!CVars::bxt_clear_color.IsEmpty()) {
			unsigned r = 0, g = 0, b = 0;
			std::istringstream ss(CVars::bxt_clear_color.GetString());
			ss >> r >> g >> b;

			static float clearColor[3];
			clearColor[0] = r / 255.0f;
			clearColor[1] = g / 255.0f;
			clearColor[2] = b / 255.0f;
			glClearColor(clearColor[0], clearColor[1], clearColor[2], 1.0f);
		} else {
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		}
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
	auto num_edicts = reinterpret_cast<int *>(reinterpret_cast<uintptr_t>(psv) + offNumEdicts);
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
	if (ClientDLL::GetInstance().DoesGameDirContain("czeror") && !std::strcmp(pszName, "InitHUD"))
		return ORIG_DispatchDirectUserMsg(0, iSize, pBuf);
	else
		return ORIG_DispatchDirectUserMsg(pszName, iSize, pBuf);
}

HOOK_DEF_0(HwDLL, void, __cdecl, SV_SetMoveVars)
{
	ORIG_SV_SetMoveVars();

	if (CVars::bxt_force_zmax.GetBool()) {
		if (movevars) {
			movevars->zmax = CVars::bxt_force_zmax.GetFloat();
		}
	}
}

HOOK_DEF_0(HwDLL, void, __cdecl, R_StudioCalcAttachments)
{
	auto &cl = ClientDLL::GetInstance();

	if (cl.pEngfuncs && pEngStudio) {
		auto current = cl.GetCurrentEntity();
		if (current == cl.GetViewModel() && NeedViewmodelAdjustments())
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
	auto& cl = ClientDLL::GetInstance();
	if (pstudiohdr && cl.GetCurrentEntity()) {
		auto current = cl.GetCurrentEntity();
		auto pseqdesc = reinterpret_cast<mstudioseqdesc_t*>(reinterpret_cast<byte*>(*pstudiohdr) +
			(*pstudiohdr)->seqindex) + current->curstate.sequence;

		if (cl.pEngfuncs) {
			if (current == cl.GetViewModel()) {
				if (cl.orig_righthand_not_found && CVars::cl_righthand.GetFloat() > 0 && pEngStudio)
				{
					float(*rotationmatrix)[3][4] = reinterpret_cast<float(*)[3][4]>(pEngStudio->StudioGetRotationMatrix());

					(*rotationmatrix)[0][1] *= -1;
					(*rotationmatrix)[1][1] *= -1;
					(*rotationmatrix)[2][1] *= -1;
				}

				if (CVars::bxt_viewmodel_disable_idle.GetBool()) {
					if (strstr(pseqdesc->label, "idle") != NULL || strstr(pseqdesc->label, "fidget") != NULL) {
						current->curstate.framerate = 0; // don't animate at all
					}
				}

				if (CVars::bxt_viewmodel_disable_equip.GetBool()) {
					if (strstr(pseqdesc->label, "holster") != NULL || strstr(pseqdesc->label, "draw") != NULL ||
						strstr(pseqdesc->label, "deploy") != NULL || strstr(pseqdesc->label, "up") != NULL ||
						strstr(pseqdesc->label, "down") != NULL) {
						current->curstate.sequence = 0; // instead set to idle sequence
						pseqdesc = reinterpret_cast<mstudioseqdesc_t*>(reinterpret_cast<byte*>(*pstudiohdr) +
							(*pstudiohdr)->seqindex) + current->curstate.sequence;
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

HOOK_DEF_1(HwDLL, void, __cdecl, CBaseUI__HideGameUI_Linux, void*, thisptr)
{
	insideHideGameUI = true;
	ORIG_CBaseUI__HideGameUI_Linux(thisptr);
	insideHideGameUI = false;
}

HOOK_DEF_0(HwDLL, void, __cdecl, R_DrawWorld)
{
	if (CVars::bxt_disable_world.GetBool())
		return;

	ORIG_R_DrawWorld();
}

HOOK_DEF_0(HwDLL, void, __cdecl, R_DrawParticles)
{
	if (CVars::bxt_disable_particles.GetBool())
		return;

	ORIG_R_DrawParticles();
}

HOOK_DEF_0(HwDLL, int, __cdecl, BUsesSDLInput)
{
	if (ClientDLL::GetInstance().DoesGameDirMatch("bshift_cutsceneless") || CVars::bxt_fix_mouse_horizontal_limit.GetBool())
		return true;
	else
		return ORIG_BUsesSDLInput();
}

HOOK_DEF_0(HwDLL, void, __cdecl, R_StudioRenderModel)
{
	if (pEngStudio) {
		auto& cl = ClientDLL::GetInstance();
		auto current = cl.GetCurrentEntity();

		int old_rendermode = current->curstate.rendermode;

		if (cl.pEngfuncs) {
			if (current == cl.GetViewModel()) {
				if (CVars::bxt_viewmodel_semitransparent.GetBool()) {
				cl.pEngfuncs->pTriAPI->RenderMode(kRenderTransAdd);
				cl.pEngfuncs->pTriAPI->Brightness(2);
			} else {
				cl.pEngfuncs->pTriAPI->RenderMode(old_rendermode); }
			}
		}
	}

	ORIG_R_StudioRenderModel();
}

HOOK_DEF_0(HwDLL, void, __cdecl, R_SetFrustum)
{
	if (CVars::bxt_fix_widescreen_fov.GetBool())
	{
		float ScreenWidth = static_cast<float>(CustomHud::GetScreenInfo().iWidth);
		float ScreenHeight = static_cast<float>(CustomHud::GetScreenInfo().iHeight);

		float def_aspect_ratio = 3.0f / 4.0f;
		float our_aspect_ratio = ScreenWidth / ScreenHeight;

		float fov = *scr_fov_value;
		float calculated_fov = static_cast<float>(std::atan(std::tan(fov*M_PI / 360.0f) * def_aspect_ratio * our_aspect_ratio) * 360.0f/M_PI);

		*scr_fov_value = std::clamp(calculated_fov, 10.0f, 150.0f); // Engine does the clamp of FOV if less 10 or higher than 150, let's do it too!
		
		// Although, it could be extended to 1 for min. value and 180 for max. value
	}
	else if (CVars::bxt_force_fov.GetFloat() >= 1.0)
	{
		*scr_fov_value = CVars::bxt_force_fov.GetFloat();
	}

	ORIG_R_SetFrustum();
}

HOOK_DEF_4(HwDLL, void, __cdecl, SPR_Set, HSPRITE_HL, hSprite, int, r, int, g, int, b)
{
	auto& cl = ClientDLL::GetInstance();

	if (cl.custom_hud_color_set && !cl.bxt_hud_color_set && !insideDrawCrosshair && !cl.insideDrawAmmoHistory && !cl.insideDrawHealthDamage && !cl.insideDrawHealthPain && !cl.insideDrawNightVision && !cl.insideDrawFiberCameraCZDS && !cl.insideDrawHudIconsCZDS)
	{
		r = cl.custom_r;
		g = cl.custom_g;
		b = cl.custom_b;
	}

	ORIG_SPR_Set(hSprite, r, g, b);
}

HOOK_DEF_2(HwDLL, void, __cdecl, DrawCrosshair, int, x, int, y)
{
	insideDrawCrosshair = true;
	ORIG_DrawCrosshair(x, y);
	insideDrawCrosshair = false;
}

HOOK_DEF_8(HwDLL, void, __cdecl, Draw_FillRGBA, int, x, int, y, int, w, int, h, int, r, int, g, int, b, int, a)
{
	auto& cl = ClientDLL::GetInstance();

	if (cl.custom_hud_color_set && !cl.bxt_hud_color_fill)
	{
		r = cl.custom_r;
		g = cl.custom_g;
		b = cl.custom_b;
	}

	if (cl.custom_hud_color_set)
		a = 255;
	else if (CVars::bxt_hud_game_alpha.GetInt() >= 1 && CVars::bxt_hud_game_alpha.GetInt() <= 255)
		a = CVars::bxt_hud_game_alpha.GetInt();

	ORIG_Draw_FillRGBA(x, y, w, h, r, g, b, a);
}

HOOK_DEF_5(HwDLL, void, __cdecl, PF_traceline_DLL, const Vector*, v1, const Vector*, v2, int, fNoMonsters, edict_t*, pentToSkip, TraceResult*, ptr)
{
	ORIG_PF_traceline_DLL(v1, v2, fNoMonsters, pentToSkip, ptr);

	ServerDLL::GetInstance().TraceLineWrap(v1, v2, fNoMonsters, pentToSkip, ptr);
}

HOOK_DEF_1(HwDLL, qboolean, __cdecl, CL_CheckGameDirectory, char*, gamedir)
{
	if (IsPlayingbackDemo() && CVars::bxt_disable_gamedir_check_in_demo.GetBool())
		return true;
	else
		return ORIG_CL_CheckGameDirectory(gamedir);
}

HOOK_DEF_0(HwDLL, int, __cdecl, Host_ValidSave)
{
	if (cofSaveHack) {
		*cofSaveHack = CVars::bxt_cof_disable_save_lock.GetBool() ? 1 : 0;
	}

	return ORIG_Host_ValidSave();
}

HOOK_DEF_2(HwDLL, int, __cdecl, SaveGameSlot, const char*, pSaveName, const char*, pSaveComment)
{
	auto rv = ORIG_SaveGameSlot(pSaveName, pSaveComment);
	// Cry of Fear-specific, draw "Saved..." on the screen.
	if (ORIG_CL_HudMessage)
		ORIG_CL_HudMessage("GAMESAVED");

	return rv;
}

HOOK_DEF_0(HwDLL, void, __cdecl, SCR_NetGraph)
{
	ORIG_SCR_NetGraph();

	// Draw "PAUSED" on the screen in pre-Steampipe builds.
	if (ORIG_VGuiWrap2_IsGameUIVisible && ORIG_SCR_DrawPause)
	{
		if (ORIG_VGuiWrap2_IsGameUIVisible() == 0)
			ORIG_SCR_DrawPause();
	}
}

HOOK_DEF_0(HwDLL, void, __cdecl, Host_Shutdown)
{
	ORIG_Host_Shutdown();
	Unhook();
	SDL::GetInstance().Unhook();
}

HOOK_DEF_0(HwDLL, void, __cdecl, ReleaseEntityDlls)
{
	ServerDLL::GetInstance().Unhook();
	ORIG_ReleaseEntityDlls();
}

HOOK_DEF_0(HwDLL, qboolean, __cdecl, Cvar_Command)
{
	int *state;
	int orig_state;
	bool return_orig_value = false;

	if (cls && CVars::bxt_disable_cheats_check_in_demo.GetBool() && IsPlayingbackDemo())
	{
		state = reinterpret_cast<int*>(cls);
		orig_state = *state;
		*state = 1;
		return_orig_value = true;
	}

	auto ret = ORIG_Cvar_Command();
	if (return_orig_value)
		*state = orig_state;

	return ret;
}

HOOK_DEF_1(HwDLL, qboolean, __cdecl, Cvar_CommandWithPrivilegeCheck, qboolean, bIsPrivileged)
{
	int *state;
	int orig_state;
	bool return_orig_value = false;

	if (cls && CVars::bxt_disable_cheats_check_in_demo.GetBool() && IsPlayingbackDemo())
	{
		state = reinterpret_cast<int*>(cls);
		orig_state = *state;
		*state = 1;
		return_orig_value = true;
	}

	auto ret = ORIG_Cvar_CommandWithPrivilegeCheck(bIsPrivileged);
	if (return_orig_value)
		*state = orig_state;

	return ret;
}

HOOK_DEF_1(HwDLL, void, __cdecl, R_ForceCvars, qboolean, mp)
{
	static bool is_monolights_changed = CVars::gl_monolights.GetBool();
	static bool is_fullbright_changed = CVars::r_fullbright.GetBool();

	if (CVars::bxt_disable_cheats_check_in_demo.GetBool() && IsPlayingbackDemo())
	{
		if ((is_monolights_changed != CVars::gl_monolights.GetBool()) || (is_fullbright_changed != CVars::r_fullbright.GetBool()))
		{
			if (ORIG_GL_BuildLightmaps)
				ORIG_GL_BuildLightmaps();
		}
		is_monolights_changed = CVars::gl_monolights.GetBool();
		is_fullbright_changed = CVars::r_fullbright.GetBool();
		return;
	}

	ORIG_R_ForceCvars(mp);

	is_monolights_changed = CVars::gl_monolights.GetBool();
	is_fullbright_changed = CVars::r_fullbright.GetBool();
}

HOOK_DEF_0(HwDLL, void, __cdecl, GL_EndRendering)
{
	/*
		_Smiley - That workaround needed to get working bxt-rs capture in Sven Co-op

		Principle: we get rid of the inline function with NOP bytes, then we hook the original function and call it at the end of GL_EndRendering

		Next, we hooking the VID_FlipScreen in bxt-rs that allow us to do the needed manipulations with it
	*/

	static bool is_patched = false;

	if (pGL_EndRendering_VID_FlipScreen_Bytes && ORIG_VID_FlipScreen)
	{
		if (!is_patched)
		{
			MemUtils::ReplaceBytes(reinterpret_cast<void*>(pGL_EndRendering_VID_FlipScreen_Bytes), 19, reinterpret_cast<const byte*>("\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"));
			is_patched = true;
		}

		ORIG_GL_EndRendering();
		ORIG_VID_FlipScreen();
	}
	else
	{
		ORIG_GL_EndRendering();
	}
}

HOOK_DEF_3(HwDLL, int, __cdecl, SV_AddToFullPack, struct entity_state_s*, state, int, e, unsigned char*, pSet)
{
	if (!psv || !offEdicts || (!CVars::bxt_show_hidden_entities.GetBool() && !CVars::bxt_show_triggers_legacy.GetBool()))
		return ORIG_SV_AddToFullPack(state, e, pSet);

	edict_t *edicts;
	GetEdicts(&edicts);
	edict_t *ent = edicts + e;

	auto oldEffects = ent->v.effects;
	auto oldRendermode = ent->v.rendermode;
	auto oldRenderColor = ent->v.rendercolor;
	auto oldRenderAmount = ent->v.renderamt;
	auto oldRenderFx = ent->v.renderfx;

	const char *classname = ppGlobals->pStringBase + ent->v.classname;
	bool is_trigger = std::strncmp(classname, "trigger_", 8) == 0;
	bool is_ladder = std::strncmp(classname, "func_ladder", 11) == 0;

	if (!is_trigger && CVars::bxt_show_hidden_entities.GetBool() && (CVars::bxt_show_hidden_entities_classname.IsEmpty() || !std::strcmp(classname, CVars::bxt_show_hidden_entities_classname.GetString().c_str()))) {
		bool show = ent->v.rendermode != kRenderNormal && ent->v.rendermode != kRenderGlow;
		switch (CVars::bxt_show_hidden_entities.GetInt()) {
		case 1:
			show = show && ent->v.renderamt == 0;
			break;
		case 2:
			break;
		default:
			show = show || (ent->v.effects & EF_NODRAW) != 0;
		}

		if (show) {
			ent->v.effects &= ~EF_NODRAW;
			ent->v.rendermode = kRenderNormal;
		}
	} else if ((is_trigger || is_ladder) && CVars::bxt_show_triggers_legacy.GetBool()) {
		ent->v.effects &= ~EF_NODRAW;
		ent->v.rendermode = kRenderTransColor;
		if (ent->v.solid == SOLID_NOT && std::strcmp(classname + 8, "transition") != 0)
			ent->v.renderfx = kRenderNormal;
		else
			ent->v.renderfx = kRenderFxPulseFast;
		ServerDLL::GetTriggerColor(classname, ent->v.rendercolor.x, ent->v.rendercolor.y, ent->v.rendercolor.z);
		ServerDLL::GetTriggerAlpha(classname, ent->v.solid == SOLID_NOT, false, ent->v.renderamt);
	}

	auto ret = ORIG_SV_AddToFullPack(state, e, pSet);

	ent->v.effects = oldEffects;
	ent->v.rendermode = oldRendermode;
	ent->v.rendercolor = oldRenderColor;
	ent->v.renderamt = oldRenderAmount;
	ent->v.renderfx = oldRenderFx;

	return ret;
}

HOOK_DEF_0(HwDLL, void, __cdecl, CL_EmitEntities)
{
	// That function is called in the same position in the engine code as ClientDLL_Frame, which is why we using code from HUD_Frame in the start of CL_EmitEntities.
	if (is_sdk10)
		ClientDLL::GetInstance().SetStuffInHudFrame();

	ORIG_CL_EmitEntities();
}

HOOK_DEF_0(HwDLL, void, __cdecl, V_RenderView)
{
	// Starting from HLSDK 2.0 the ClientDLL_CalcRefDef called at the start of V_RenderView, that is why we using code from V_CalcRefDef here.
	if (is_sdk10 && simvel && simorg)
		CustomHud::UpdatePlayerInfoInaccurate(*simvel, *simorg);

	ORIG_V_RenderView();
}

HOOK_DEF_2(HwDLL, void, __cdecl, LoadAdjacentEntities, char*, pOldLevel, char*, pLandmarkName)
{
	if (insideHost_Changelevel2_f && is_won_build) {
		if (!autoRecordDemoName.empty()) {
			if (*demorecording == 0)
				autoRecordNow = true;
		}
		else {
			autoRecordNow = false;
			autoRecordDemoName.clear();
		}
	}

	ORIG_LoadAdjacentEntities(pOldLevel, pLandmarkName);
}

HOOK_DEF_1(HwDLL, void, __cdecl, PlayerMove, qboolean, server)
{
	bool stuck_cur_frame = false;
	static bool not_stuck_prev_frame = false;

	if (ORIG_PM_CheckStuck)
	{
		stuck_cur_frame = ORIG_PM_CheckStuck();
		if (!CVars::bxt_fire_on_stuck.IsEmpty() && stuck_cur_frame && not_stuck_prev_frame)
		{
			std::ostringstream ss;
			ss << CVars::bxt_fire_on_stuck.GetString().c_str() << "\n";

			ORIG_Cbuf_InsertText(ss.str().c_str());
		}
		not_stuck_prev_frame = !stuck_cur_frame;
	}

	if (!won_ppmove)
	{
		ORIG_PlayerMove(server);
		return;
	}

	ORIG_PlayerMove(server);
}

HOOK_DEF_0(HwDLL, void, __cdecl, JumpButton)
{
	if (CVars::bxt_force_jumpless.GetBool())
		return;

	if (!won_ppmove || !onground)
	{
		ORIG_JumpButton();
		return;
	}

	auto pmove = reinterpret_cast<uintptr_t>(won_ppmove);
	int playerIndex = *reinterpret_cast<int*>(pmove + 0x0);

	int orig_onground = *onground;

	int* oldbuttons = reinterpret_cast<int*>(pmove + 0x6C);
	int orig_oldbuttons = *oldbuttons;

	if (CVars::bxt_autojump.GetBool())
	{
		if ((orig_onground != -1) && !cantJumpNextTime[playerIndex]) {
			*oldbuttons &= ~IN_JUMP;
		}
	}

	cantJumpNextTime[playerIndex] = false;

	ORIG_JumpButton();

	if ((orig_onground != -1) && (*onground == -1))
		cantJumpNextTime[playerIndex] = true;

	if (CVars::bxt_autojump.GetBool())
		*oldbuttons = orig_oldbuttons;
}

HOOK_DEF_1(HwDLL, void, __cdecl, CL_Restore, const char*, mapName)
{
	ORIG_CL_Restore(mapName);
}

HOOK_DEF_0(HwDLL, void, __cdecl, CL_SignonReply)
{
	ORIG_CL_SignonReply();
}

HOOK_DEF_1(HwDLL, void, __cdecl, Sequence_OnLevelLoad, const char*, fileName)
{
	ORIG_Sequence_OnLevelLoad(fileName);
} 