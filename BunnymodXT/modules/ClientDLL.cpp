#include "../stdafx.hpp"

#include "../sptlib-wrapper.hpp"
#include <SPTLib/MemUtils.hpp>
#include <SPTLib/Hooks.hpp>
#include "ClientDLL.hpp"
#include "ServerDLL.hpp"
#include "HwDLL.hpp"
#include "../patterns.hpp"
#include "../cvars.hpp"
#include "../hud_custom.hpp"
#include "../triangle_drawing.hpp"
#include "../discord_integration.hpp"
#include <GL/gl.h>

// Linux hooks.
#ifndef _WIN32
extern "C" void __cdecl HUD_Init()
{
	return ClientDLL::HOOKED_HUD_Init();
}

extern "C" void __cdecl HUD_VidInit()
{
	return ClientDLL::HOOKED_HUD_VidInit();
}

extern "C" void __cdecl HUD_Reset()
{
	return ClientDLL::HOOKED_HUD_Reset();
}

extern "C" void __cdecl HUD_Redraw(float time, int intermission)
{
	return ClientDLL::HOOKED_HUD_Redraw(time, intermission);
}

extern "C" void __cdecl HUD_PostRunCmd(local_state_s* from, local_state_s* to, usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed)
{
	return ClientDLL::HOOKED_HUD_PostRunCmd(from, to, cmd, runfuncs, time, random_seed);
}

extern "C" void __cdecl CL_CreateMove(float frametime, usercmd_s *cmd, int active)
{
	return ClientDLL::HOOKED_CL_CreateMove(frametime, cmd, active);
}

extern "C" void __cdecl HUD_PlayerMove(struct playermove_s* ppmove, int server)
{
	return ClientDLL::HOOKED_HUD_PlayerMove(ppmove, server);
}

extern "C" void __cdecl HUD_Frame(double time)
{
	return ClientDLL::HOOKED_HUD_Frame(time);
}

extern "C" void __cdecl V_CalcRefdef(ref_params_t* pparams)
{
	return ClientDLL::HOOKED_V_CalcRefdef(pparams);
}

extern "C" void __cdecl HUD_DrawTransparentTriangles()
{
	return ClientDLL::HOOKED_HUD_DrawTransparentTriangles();
}

extern "C" int __cdecl HUD_Key_Event(int down, int keynum, const char* pszCurrentBinding)
{
	return ClientDLL::HOOKED_HUD_Key_Event(down, keynum, pszCurrentBinding);
}

extern "C" int __cdecl HUD_UpdateClientData(client_data_t* pcldata, float flTime)
{
	return ClientDLL::HOOKED_HUD_UpdateClientData(pcldata, flTime);
}

extern "C" void __cdecl _ZN20CStudioModelRenderer21StudioCalcAttachmentsEv(void *thisptr)
{
	return ClientDLL::HOOKED_CStudioModelRenderer__StudioCalcAttachments_Linux(thisptr);
}

extern "C" void __cdecl _Z15VectorTransformPKfPA4_fPf(float *in1, float *in2, float *out)
{
	return ClientDLL::HOOKED_VectorTransform(in1, in2, out);
}

extern "C" void __cdecl _Z22EV_GetDefaultShellInfoP12event_args_sPfS1_S1_S1_S1_S1_S1_fff(event_args_t *args, float *origin, float *velocity, float *ShellVelocity, float *ShellOrigin,
																						 float *forward, float *right, float *up, float forwardScale, float upScale, float rightScale)
{
	return ClientDLL::HOOKED_EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, forwardScale, upScale, rightScale);
}

extern "C" void __cdecl _ZN20CStudioModelRenderer16StudioSetupBonesEv(void *thisptr)
{
	return ClientDLL::HOOKED_CStudioModelRenderer__StudioSetupBones_Linux(thisptr);
}

extern "C" int __cdecl HUD_AddEntity(int type, cl_entity_s* ent, char* modelname)
{
	return ClientDLL::HOOKED_HUD_AddEntity(type, ent, modelname);
}

extern "C" int __cdecl CL_IsThirdPerson()
{
	return ClientDLL::HOOKED_CL_IsThirdPerson();
}

extern "C" void __cdecl _ZN20CStudioModelRenderer17StudioRenderModelEv(void *thisptr)
{
	return ClientDLL::HOOKED_CStudioModelRenderer__StudioRenderModel_Linux(thisptr);
}

extern "C" void __cdecl _Z11ScaleColorsRiS_S_i(int* r, int* g, int* b, int a)
{
	return ClientDLL::HOOKED_ScaleColors(r, g, b, a);
}

extern "C" int __cdecl _ZN15HistoryResource15DrawAmmoHistoryEf(void *thisptr, float flTime)
{
	return ClientDLL::HOOKED_HistoryResource__DrawAmmoHistory_Linux(thisptr, flTime);
}

extern "C" int __cdecl _ZN10CHudHealth10DrawDamageEf(void *thisptr, float flTime)
{
	return ClientDLL::HOOKED_CHudHealth__DrawDamage_Linux(thisptr, flTime);
}

extern "C" int __cdecl _ZN10CHudHealth8DrawPainEf(void *thisptr, float flTime)
{
	return ClientDLL::HOOKED_CHudHealth__DrawPain_Linux(thisptr, flTime);
}

extern "C" void __cdecl _ZN14CHudFlashlight15drawNightVisionEv(void *thisptr)
{
	return ClientDLL::HOOKED_CHudFlashlight__drawNightVision_Linux(thisptr);
}

extern "C" bool __cdecl _ZN4CHud18DrawHudNightVisionEf(void *thisptr, float flTime)
{
	return ClientDLL::HOOKED_CHud__DrawHudNightVision_Linux(thisptr, flTime);
}

extern "C" bool __cdecl _ZN4CHud18DrawHudFiberCameraEf(void *thisptr, float flTime)
{
	return ClientDLL::HOOKED_CHud__DrawHudFiberCamera_Linux(thisptr, flTime);
}

extern "C" int __cdecl _ZN9CHudIcons4DrawEf(void *thisptr, float flTime)
{
	return ClientDLL::HOOKED_CHudIcons__Draw_Linux(thisptr, flTime);
}

extern "C" void __cdecl _Z11V_PunchAxisif(int axis, float punch)
{
	return ClientDLL::HOOKED_V_PunchAxis(axis, punch);
}
#endif

void ClientDLL::Hook(const std::wstring& moduleName, void* moduleHandle, void* moduleBase, size_t moduleLength, bool needToIntercept)
{
	Clear(); // Just in case.

	m_Handle = moduleHandle;
	m_Base = moduleBase;
	m_Length = moduleLength;
	m_Name = moduleName;
	m_Intercepted = needToIntercept;

	FindStuff();
	RegisterCVarsAndCommands();

	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_HUD_Init), reinterpret_cast<void*>(HOOKED_HUD_Init));
	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_HUD_VidInit), reinterpret_cast<void*>(HOOKED_HUD_VidInit));
	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_HUD_Reset), reinterpret_cast<void*>(HOOKED_HUD_Reset));
	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_HUD_Redraw), reinterpret_cast<void*>(HOOKED_HUD_Redraw));
	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_HUD_PostRunCmd), reinterpret_cast<void*>(HOOKED_HUD_PostRunCmd));
	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_HUD_Frame), reinterpret_cast<void*>(HOOKED_HUD_Frame));
	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_HUD_DrawTransparentTriangles), reinterpret_cast<void*>(HOOKED_HUD_DrawTransparentTriangles));
	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_HUD_Key_Event), reinterpret_cast<void*>(HOOKED_HUD_Key_Event));
	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_HUD_UpdateClientData), reinterpret_cast<void*>(HOOKED_HUD_UpdateClientData));
	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_HUD_AddEntity), reinterpret_cast<void*>(HOOKED_HUD_AddEntity));
	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_CL_IsThirdPerson), reinterpret_cast<void*>(HOOKED_CL_IsThirdPerson));
	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_HUD_Shutdown), reinterpret_cast<void*>(HOOKED_HUD_Shutdown));
	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_CL_CreateMove), reinterpret_cast<void*>(HOOKED_CL_CreateMove));
	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_HUD_PlayerMove), reinterpret_cast<void*>(HOOKED_HUD_PlayerMove));

	if (needToIntercept)
	{
		MemUtils::Intercept(moduleName,
			ORIG_PM_Jump, HOOKED_PM_Jump,
			ORIG_PM_PreventMegaBunnyJumping, HOOKED_PM_PreventMegaBunnyJumping,
			ORIG_V_CalcRefdef, HOOKED_V_CalcRefdef,
			ORIG_HUD_Init, HOOKED_HUD_Init,
			ORIG_HUD_VidInit, HOOKED_HUD_VidInit,
			ORIG_HUD_Reset, HOOKED_HUD_Reset,
			ORIG_HUD_Redraw, HOOKED_HUD_Redraw,
			ORIG_HUD_PostRunCmd, HOOKED_HUD_PostRunCmd,
			ORIG_HUD_Frame, HOOKED_HUD_Frame,
			ORIG_HUD_DrawTransparentTriangles, HOOKED_HUD_DrawTransparentTriangles,
			ORIG_HUD_Key_Event, HOOKED_HUD_Key_Event,
			ORIG_HUD_UpdateClientData, HOOKED_HUD_UpdateClientData,
			ORIG_HUD_AddEntity, HOOKED_HUD_AddEntity,
			ORIG_HUD_Shutdown, HOOKED_HUD_Shutdown,
			ORIG_EV_GetDefaultShellInfo, HOOKED_EV_GetDefaultShellInfo,
			ORIG_CStudioModelRenderer__StudioCalcAttachments, HOOKED_CStudioModelRenderer__StudioCalcAttachments,
			ORIG_VectorTransform, HOOKED_VectorTransform,
			ORIG_CStudioModelRenderer__StudioSetupBones, HOOKED_CStudioModelRenderer__StudioSetupBones,
			ORIG_CL_IsThirdPerson, HOOKED_CL_IsThirdPerson,
			ORIG_CStudioModelRenderer__StudioRenderModel, HOOKED_CStudioModelRenderer__StudioRenderModel,
			ORIG_ScaleColors, HOOKED_ScaleColors,
			ORIG_V_PunchAxis, HOOKED_V_PunchAxis,
			ORIG_HistoryResource__DrawAmmoHistory, HOOKED_HistoryResource__DrawAmmoHistory,
			ORIG_CHudHealth__DrawDamage, HOOKED_CHudHealth__DrawDamage,
			ORIG_CHudHealth__DrawPain, HOOKED_CHudHealth__DrawPain,
			ORIG_CHudFlashlight__drawNightVision, HOOKED_CHudFlashlight__drawNightVision,
			ORIG_CHud__DrawHudNightVision, HOOKED_CHud__DrawHudNightVision,
			ORIG_CHud__DrawHudFiberCamera, HOOKED_CHud__DrawHudFiberCamera,
			ORIG_CHudIcons__Draw, HOOKED_CHudIcons__Draw,
			ORIG_PM_Duck, HOOKED_PM_Duck,
			ORIG_PM_UnDuck, HOOKED_PM_UnDuck,
			ORIG_CL_CreateMove, HOOKED_CL_CreateMove,
			ORIG_HUD_PlayerMove, HOOKED_HUD_PlayerMove);
	}

	// HACK: on Windows we don't get a LoadLibrary for SDL2, so when starting using the injector
	// we never see it loaded. Try loading it here.
	Hooks::HookModule(L"SDL2.dll");
	// Similar story on Linux. Not present during the initial BXT scan and no dlopen.
	Hooks::HookModule(L"libSDL2-2.0.so.0");
}

void ClientDLL::Unhook()
{
	if (m_Intercepted)
	{
		MemUtils::RemoveInterception(m_Name,
			ORIG_PM_Jump,
			ORIG_PM_PreventMegaBunnyJumping,
			ORIG_V_CalcRefdef,
			ORIG_HUD_Init,
			ORIG_HUD_VidInit,
			ORIG_HUD_Reset,
			ORIG_HUD_Redraw,
			ORIG_HUD_PostRunCmd,
			ORIG_HUD_Frame,
			ORIG_HUD_DrawTransparentTriangles,
			ORIG_HUD_Key_Event,
			ORIG_HUD_UpdateClientData,
			ORIG_HUD_AddEntity,
			ORIG_HUD_Shutdown,
			ORIG_EV_GetDefaultShellInfo,
			ORIG_CStudioModelRenderer__StudioCalcAttachments,
			ORIG_VectorTransform,
			ORIG_CStudioModelRenderer__StudioSetupBones,
			ORIG_CL_IsThirdPerson,
			ORIG_CStudioModelRenderer__StudioRenderModel,
			ORIG_ScaleColors,
			ORIG_V_PunchAxis,
			ORIG_HistoryResource__DrawAmmoHistory,
			ORIG_CHudHealth__DrawDamage,
			ORIG_CHudHealth__DrawPain,
			ORIG_CHudFlashlight__drawNightVision,
			ORIG_CHud__DrawHudNightVision,
			ORIG_CHud__DrawHudFiberCamera,
			ORIG_CHudIcons__Draw,
			ORIG_PM_Duck,
			ORIG_PM_UnDuck,
			ORIG_CL_CreateMove,
			ORIG_HUD_PlayerMove);
	}

	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_HUD_Init));
	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_HUD_VidInit));
	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_HUD_Reset));
	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_HUD_Redraw));
	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_HUD_PostRunCmd));
	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_HUD_Frame));
	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_HUD_DrawTransparentTriangles));
	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_HUD_Key_Event));
	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_HUD_UpdateClientData));
	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_HUD_AddEntity));
	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_CL_IsThirdPerson));
	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_HUD_Shutdown));
	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_CL_CreateMove));
	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_HUD_PlayerMove));

	Clear();
}

void ClientDLL::Clear()
{
	IHookableNameFilter::Clear();
	ORIG_PM_Jump = nullptr;
	ORIG_PM_PlayerMove = nullptr;
	ORIG_PM_PreventMegaBunnyJumping = nullptr;
	ORIG_PM_ClipVelocity = nullptr;
	ORIG_PM_WaterMove = nullptr;
	ORIG_PM_Move = nullptr;
	ORIG_VectorTransform = nullptr;
	ORIG_EV_GetDefaultShellInfo = nullptr;
	ORIG_CStudioModelRenderer__StudioCalcAttachments = nullptr;
	ORIG_CStudioModelRenderer__StudioCalcAttachments_Linux = nullptr;
	ORIG_CStudioModelRenderer__StudioSetupBones = nullptr;
	ORIG_CStudioModelRenderer__StudioSetupBones_Linux = nullptr;
	ORIG_CStudioModelRenderer__StudioRenderModel = nullptr;
	ORIG_CStudioModelRenderer__StudioRenderModel_Linux = nullptr;
	ORIG_ScaleColors = nullptr;
	ORIG_V_PunchAxis = nullptr;
	ORIG_HistoryResource__DrawAmmoHistory = nullptr;
	ORIG_HistoryResource__DrawAmmoHistory_Linux = nullptr;
	ORIG_CHudHealth__DrawDamage = nullptr;
	ORIG_CHudHealth__DrawDamage_Linux = nullptr;
	ORIG_CHudHealth__DrawPain = nullptr;
	ORIG_CHudHealth__DrawPain_Linux = nullptr;
	ORIG_CHudFlashlight__drawNightVision = nullptr;
	ORIG_CHudFlashlight__drawNightVision_Linux = nullptr;
	ORIG_CHud__DrawHudNightVision = nullptr;
	ORIG_CHud__DrawHudNightVision_Linux = nullptr;
	ORIG_CHud__DrawHudFiberCamera = nullptr;
	ORIG_CHud__DrawHudFiberCamera_Linux = nullptr;
	ORIG_CHudIcons__Draw = nullptr;
	ORIG_CHudIcons__Draw_Linux = nullptr;
	ORIG_V_CalcRefdef = nullptr;
	ORIG_HUD_Init = nullptr;
	ORIG_HUD_VidInit = nullptr;
	ORIG_HUD_Reset = nullptr;
	ORIG_HUD_Redraw = nullptr;
	ORIG_HUD_PostRunCmd = nullptr;
	ORIG_HUD_Frame = nullptr;
	ORIG_HUD_DrawTransparentTriangles = nullptr;
	ORIG_HUD_Key_Event = nullptr;
	ORIG_HUD_UpdateClientData = nullptr;
	ORIG_HUD_AddEntity = nullptr;
	ORIG_IN_ActivateMouse = nullptr;
	ORIG_IN_DeactivateMouse = nullptr;
	ORIG_CL_IsThirdPerson = nullptr;
	ORIG_HUD_Shutdown = nullptr;
	ORIG_PM_Duck = nullptr;
	ORIG_PM_UnDuck = nullptr;
	ORIG_CL_CreateMove = nullptr;
	ORIG_HUD_PlayerMove = nullptr;
	ppmove = nullptr;
	offOldbuttons = 0;
	offOnground = 0;
	offIUser1 = 0;
	offCmd = 0;
	offInDuck = 0;
	offFlags = 0;
	offVelocity = 0;
	offWaterlevel = 0;
	offBhopcap = 0;
	pBhopcapWindows = 0;
	memset(originalBhopcapInsn, 0, sizeof(originalBhopcapInsn));
	cantJumpNextTime = false;
	SeedsQueued = 0;
	m_Intercepted = false;
	last_vieworg = Vector();
	last_viewangles = Vector();
	last_viewforward = Vector();
	last_viewup = Vector();
	last_viewright = Vector();
	last_buttons = 0;
	pCS_AngleSpeedCap = 0;
	pCS_AngleSpeedCap_Linux = 0;
	pCS_SpeedScaling = 0;
	pCS_SpeedScaling_Linux = 0;
	offVectorTransform = 0;
	offpCurrentEntity = 0;
	offpStudioHeader = 0;
}

void ClientDLL::FindStuff()
{
	auto fPM_PreventMegaBunnyJumping = FindFunctionAsync(
		ORIG_PM_PreventMegaBunnyJumping,
		"PM_PreventMegaBunnyJumping",
		patterns::shared::PM_PreventMegaBunnyJumping);

	auto fPM_Jump = FindFunctionAsync(
		ORIG_PM_Jump,
		"PM_Jump",
		patterns::shared::PM_Jump,
		[&](auto pattern) {
			offOldbuttons = 200;
			offOnground = 224;
			offIUser1 = 508;
			offCmd = 283736;
			offInDuck = 0x90;
			offFlags = 0xB8;
			offVelocity = 92;
			offWaterlevel = 0xE4;
			if (pattern == patterns::shared::PM_Jump.cend()) // Linux.
			{
				void *bhopcapAddr;
				auto n = MemUtils::find_unique_sequence(
					m_Base,
					m_Length,
					patterns::shared::Bhopcap.cbegin(),
					patterns::shared::Bhopcap.cend(),
					bhopcapAddr);
				if (n != patterns::shared::Bhopcap.cend())
				{
					offBhopcap = reinterpret_cast<ptrdiff_t>(bhopcapAddr) - reinterpret_cast<ptrdiff_t>(ORIG_PM_Jump) + 27;
					memcpy(originalBhopcapInsn, reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(bhopcapAddr) + 27), sizeof(originalBhopcapInsn));
				}
			}
			else
			{
				switch (pattern - patterns::shared::PM_Jump.cbegin())
				{
				case 0: // HL-SteamPipe
				case 1: // gunman
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(ORIG_PM_Jump) + 2);
					break;
				case 2: // AG-Server, shouldn't happen here but who knows.
				case 3: // AG-Client
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(ORIG_PM_Jump) + 3);
					break;
				case 4: // BigLolly
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(ORIG_PM_Jump) + 21);
					break;
				case 10: // Parasomnia
				case 11: // Reissues
				case 5: // TWHL-Tower-2
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(ORIG_PM_Jump) + 5);
					break;
				case 12: // HL-SWEET
				case 6: // Decay
				case 14: // CoF-Mod-155
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(ORIG_PM_Jump) + 24);
					break;
				case 7: // Halfquake-Trilogy
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(ORIG_PM_Jump) + 6);
					break;
				case 8: // Half-Payne
				case 9: // DSM-Demo-1
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(ORIG_PM_Jump) + 8);
					break;
				case 13: // CoF-5936
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(ORIG_PM_Jump) + 8);
					is_cof_client = true;
					break;
				case 15: // CoF-Mod-155-Client
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(ORIG_PM_Jump) + 9);
					is_cof_client = true;
					break;
				}
			}
		});

	auto fPM_Jump_Bhopcap_Windows = FindAsync(
		pBhopcapWindows,
		patterns::shared::Bhopcap_Windows,
		[&](auto pattern) {
			switch (pattern - patterns::shared::Bhopcap_Windows.cbegin()) {
			case 0: // DSM-Demo-1
				pBhopcapWindows += 36;
				break;
			default:
				assert(false);
			}
		});

	auto fCStudioModelRenderer__StudioSetupBones = FindAsync(
		ORIG_CStudioModelRenderer__StudioSetupBones,
		patterns::client::CStudioModelRenderer__StudioSetupBones,
		[&](auto pattern) {
			switch (pattern - patterns::client::CStudioModelRenderer__StudioSetupBones.cbegin()) {
			case 0: // HL-SteamPipe
			case 1: // HL-WON
			case 2: // CSCZDS
			case 3: // HL-Restored
			case 4: // Echoes
			case 5: // They Hunger Trilogy
			case 6: // PARANOIA
			case 7: // AoMDC
			case 8: // TWHL-Tower-2
			case 9: // Invasion
			case 10: // Halfquake-Trilogy
			case 11: // Reissues
			case 12: // AVP2
			case 13: // CStrike-Latest
			case 14: // CoF-5936
			case 15: // CoF-Mod-155
			case 17: // Sven-v525
				offpCurrentEntity = 48;
				offpStudioHeader = 68;
				break;
			case 16: // DayOfDefeat
				offpCurrentEntity = 56;
				offpStudioHeader = 76;
				break;
			default:
				assert(false);
				break;
			}
		});

	auto fCStudioModelRenderer__StudioCalcAttachments = FindAsync(
		ORIG_CStudioModelRenderer__StudioCalcAttachments,
		patterns::client::CStudioModelRenderer__StudioCalcAttachments,
		[&](auto pattern) {
			switch (pattern - patterns::client::CStudioModelRenderer__StudioCalcAttachments.cbegin()) {
			case 0: // HL-WON-1712
			case 1: // CSCZDS
				offVectorTransform = 101;
				break;
			case 2: // Echoes
				offVectorTransform = 187;
				break;
			case 3: // AoMDC
				offVectorTransform = 190;
				break;
			case 4: // TWHL-Tower-2
				offVectorTransform = 100;
				break;
			case 5: // Halfquake-Trilogy
				offVectorTransform = 70;
				break;
			case 6: // Reissues
				offVectorTransform = 97;
				break;
			case 7: // CoF-5936
			case 10: // Sven-v525
				offVectorTransform = 77;
				break;
			case 8: // CoF-Mod-155
				offVectorTransform = 111;
				break;
			case 9: // Decay
				offVectorTransform = 188;
				break;
			default:
				assert(false);
				break;
			}
		});

	auto fCS_AngleSpeedCap = FindAsync(
		pCS_AngleSpeedCap,
		patterns::client::CS_AngleSpeedCap);
	auto fCS_AngleSpeedCap_Linux = FindAsync(
		pCS_AngleSpeedCap_Linux,
		patterns::client::CS_AngleSpeedCap_Linux);
	auto fCS_SpeedScaling = FindAsync(
		pCS_SpeedScaling,
		patterns::client::CS_SpeedScaling);
	auto fCS_SpeedScaling_Linux = FindAsync(
		pCS_SpeedScaling_Linux,
		patterns::client::CS_SpeedScaling_Linux);
	auto fEV_GetDefaultShellInfo = FindAsync(ORIG_EV_GetDefaultShellInfo, patterns::client::EV_GetDefaultShellInfo);
	auto fCStudioModelRenderer__StudioRenderModel = FindAsync(
		ORIG_CStudioModelRenderer__StudioRenderModel,
		patterns::client::CStudioModelRenderer__StudioRenderModel);
	auto fScaleColors = FindAsync(ORIG_ScaleColors, patterns::client::ScaleColors);
	auto fV_PunchAxis = FindAsync(ORIG_V_PunchAxis, patterns::client::V_PunchAxis);
	auto fHistoryResource__DrawAmmoHistory = FindAsync(
		ORIG_HistoryResource__DrawAmmoHistory,
		patterns::client::HistoryResource__DrawAmmoHistory);
	auto fCHudHealth__DrawDamage = FindAsync(
		ORIG_CHudHealth__DrawDamage,
		patterns::client::CHudHealth__DrawDamage);
	auto fCHudHealth__DrawPain = FindAsync(
		ORIG_CHudHealth__DrawPain,
		patterns::client::CHudHealth__DrawPain);
	auto fCHudFlashlight__drawNightVision = FindAsync(
		ORIG_CHudFlashlight__drawNightVision,
		patterns::client::CHudFlashlight__drawNightVision);
	auto fCHud__DrawHudNightVision = FindAsync(
		ORIG_CHud__DrawHudNightVision,
		patterns::client::CHud__DrawHudNightVision);
	auto fCHud__DrawHudFiberCamera = FindAsync(
		ORIG_CHud__DrawHudFiberCamera,
		patterns::client::CHud__DrawHudFiberCamera);
	auto fCHudIcons__Draw = FindAsync(
		ORIG_CHudIcons__Draw,
		patterns::client::CHudIcons__Draw);
	auto fPM_Duck = FindAsync(
		ORIG_PM_Duck,
		patterns::client::PM_Duck);
	auto fPM_UnDuck = FindAsync(
		ORIG_PM_UnDuck,
		patterns::client::PM_UnDuck);

	ORIG_PM_PlayerMove = reinterpret_cast<_PM_PlayerMove>(MemUtils::GetSymbolAddress(m_Handle, "PM_PlayerMove")); // For Linux.
	ORIG_PM_ClipVelocity = reinterpret_cast<_PM_ClipVelocity>(MemUtils::GetSymbolAddress(m_Handle, "PM_ClipVelocity")); // For Linux.
	ORIG_PM_WaterMove = reinterpret_cast<_PM_WaterMove>(MemUtils::GetSymbolAddress(m_Handle, "PM_WaterMove")); // For Linux.
	ORIG_PM_Move = reinterpret_cast<_PM_Move>(MemUtils::GetSymbolAddress(m_Handle, "PM_Move")); // For Linux.

	if (!pEngfuncs)
	{
		pEngfuncs = reinterpret_cast<cl_enginefunc_t*>(MemUtils::GetSymbolAddress(m_Handle, "gEngfuncs"));
		EngineDevMsg("[client dll] gEngfuncs [Linux] is %p.\n", pEngfuncs);
	}

	// We can draw stuff only if we know that we have already received / will receive engfuncs.
	if (pEngfuncs) {
		if (!FindHUDFunctions()) {
			ORIG_HUD_Init = nullptr;
			ORIG_HUD_VidInit = nullptr;
			ORIG_HUD_Reset = nullptr;
			ORIG_HUD_Redraw = nullptr;
			EngineWarning("Custom HUD is not available.\n");
		}
	}

	ORIG_V_CalcRefdef = reinterpret_cast<_V_CalcRefdef>(MemUtils::GetSymbolAddress(m_Handle, "V_CalcRefdef"));
	if (ORIG_V_CalcRefdef) {
		EngineDevMsg("[client dll] Found V_CalcRefdef at %p.\n", ORIG_V_CalcRefdef);
	} else {
		EngineDevWarning("[client dll] Could not find V_CalcRefdef.\n");
		EngineWarning("Velocity display during demo playback is not available.\n");
	}

	ORIG_HUD_PostRunCmd = reinterpret_cast<_HUD_PostRunCmd>(MemUtils::GetSymbolAddress(m_Handle, "HUD_PostRunCmd"));
	if (ORIG_HUD_PostRunCmd)
		EngineDevMsg("[client dll] Found HUD_PostRunCmd at %p.\n", ORIG_HUD_PostRunCmd);
	else
		EngineDevWarning("[client dll] Could not find HUD_PostRunCmd.\n");

	ORIG_CL_CreateMove = reinterpret_cast<_CL_CreateMove>(MemUtils::GetSymbolAddress(m_Handle, "CL_CreateMove"));
	if (ORIG_CL_CreateMove)
		EngineDevMsg("[client dll] Found CL_CreateMove at %p.\n", ORIG_CL_CreateMove);
	else
		EngineDevWarning("[client dll] Could not find CL_CreateMove.\n");

	ORIG_HUD_PlayerMove = reinterpret_cast<_HUD_PlayerMove>(MemUtils::GetSymbolAddress(m_Handle, "HUD_PlayerMove"));
	if (ORIG_HUD_PlayerMove)
		EngineDevMsg("[client dll] Found HUD_PlayerMove at %p.\n", ORIG_HUD_PlayerMove);
	else
		EngineDevWarning("[client dll] Could not find HUD_PlayerMove.\n");

	ORIG_HUD_Frame = reinterpret_cast<_HUD_Frame>(MemUtils::GetSymbolAddress(m_Handle, "HUD_Frame"));
	if (ORIG_HUD_Frame) {
		EngineDevMsg("[client dll] Found HUD_Frame at %p.\n", ORIG_HUD_Frame);
	} else {
		EngineDevWarning("[client dll] Could not find HUD_Frame.\n");
		EngineWarning("In-game timer is not available.\n");
	}

	ORIG_HUD_DrawTransparentTriangles = reinterpret_cast<_HUD_DrawTransparentTriangles>(MemUtils::GetSymbolAddress(m_Handle, "HUD_DrawTransparentTriangles"));
	if (ORIG_HUD_DrawTransparentTriangles) {
		EngineDevMsg("[client dll] Found HUD_DrawTransparentTriangles at %p.\n", ORIG_HUD_DrawTransparentTriangles);
	} else {
		EngineDevWarning("[client dll] Could not find HUD_DrawTransparentTriangles.\n");
		EngineWarning("Features utilizing TriAPI are unavailable.\n");
	}

	ORIG_HUD_Key_Event = reinterpret_cast<_HUD_Key_Event>(MemUtils::GetSymbolAddress(m_Handle, "HUD_Key_Event"));
	if (ORIG_HUD_Key_Event) {
		EngineDevMsg("[client dll] Found HUD_Key_Event at %p.\n", ORIG_HUD_Key_Event);
	} else {
		EngineDevWarning("[client dll] Could not find HUD_Key_Event.\n");
	}

	ORIG_HUD_UpdateClientData = reinterpret_cast<_HUD_UpdateClientData>(MemUtils::GetSymbolAddress(m_Handle, "HUD_UpdateClientData"));
	if (ORIG_HUD_UpdateClientData) {
		EngineDevMsg("[client dll] Found HUD_UpdateClientData at %p.\n", ORIG_HUD_UpdateClientData);
	} else {
		EngineDevWarning("[client dll] Could not find HUD_UpdateClientData.\n");
	}

	ORIG_HUD_AddEntity = reinterpret_cast<_HUD_AddEntity>(MemUtils::GetSymbolAddress(m_Handle, "HUD_AddEntity"));
	if (ORIG_HUD_AddEntity) {
		EngineDevMsg("[client dll] Found HUD_AddEntity at %p.\n", ORIG_HUD_AddEntity);
	} else {
		EngineDevWarning("[client dll] Could not find HUD_AddEntity.\n");
		EngineWarning("bxt_show_hidden_entities_clientside is not available.\n");
	}

	ORIG_IN_ActivateMouse = reinterpret_cast<_IN_ActivateMouse>(MemUtils::GetSymbolAddress(m_Handle, "IN_ActivateMouse"));
	if (ORIG_IN_ActivateMouse) {
		EngineDevMsg("[client dll] Found IN_ActivateMouse at %p.\n", ORIG_IN_ActivateMouse);
	} else {
		EngineDevWarning("[client dll] Could not find IN_ActivateMouse.\n");
	}

	ORIG_IN_DeactivateMouse = reinterpret_cast<_IN_DeactivateMouse>(MemUtils::GetSymbolAddress(m_Handle, "IN_DeactivateMouse"));
	if (ORIG_IN_DeactivateMouse) {
		EngineDevMsg("[client dll] Found IN_DeactivateMouse at %p.\n", ORIG_IN_DeactivateMouse);
	} else {
		EngineDevWarning("[client dll] Could not find IN_DeactivateMouse.\n");
	}

	ORIG_CL_IsThirdPerson = reinterpret_cast<_CL_IsThirdPerson>(MemUtils::GetSymbolAddress(m_Handle, "CL_IsThirdPerson"));
	if (ORIG_CL_IsThirdPerson) {
		EngineDevMsg("[client dll] Found CL_IsThirdPerson at %p.\n", ORIG_CL_IsThirdPerson);
	} else {
		EngineDevWarning("[client dll] Could not find CL_IsThirdPerson.\n");
	}

	ORIG_HUD_Shutdown = reinterpret_cast<_HUD_Shutdown>(MemUtils::GetSymbolAddress(m_Handle, "HUD_Shutdown"));
	if (ORIG_HUD_Shutdown) {
		EngineDevMsg("[client dll] Found HUD_Shutdown at %p.\n", ORIG_HUD_Shutdown);
	} else {
		EngineDevWarning("[client dll] Could not find HUD_Shutdown.\n");
	}

	bool noBhopcap = false;
	{
		auto pattern = fPM_PreventMegaBunnyJumping.get();
		if (ORIG_PM_PreventMegaBunnyJumping) {
			if (pattern == patterns::shared::PM_PreventMegaBunnyJumping.cend())
				EngineDevMsg("[client dll] Found PM_PreventMegaBunnyJumping at %p.\n", ORIG_PM_PreventMegaBunnyJumping);
			else
				EngineDevMsg("[client dll] Found PM_PreventMegaBunnyJumping at %p (using the %s pattern).\n", ORIG_PM_PreventMegaBunnyJumping, pattern->name());
		} else {
			EngineDevWarning("[client dll] Could not find PM_PreventMegaBunnyJumping.\n");
			EngineWarning("Bhopcap prediction disabling is not available.\n");
			noBhopcap = true;
		}
	}

	{
		auto pattern = fPM_Jump.get();
		auto pattern2 = fPM_Jump_Bhopcap_Windows.get();
		if (ORIG_PM_Jump) {
			if (pattern == patterns::shared::PM_Jump.cend())
				EngineDevMsg("[client dll] Found PM_Jump at %p.\n", ORIG_PM_Jump);
			else
				EngineDevMsg("[client dll] Found PM_Jump at %p (using the %s pattern).\n", ORIG_PM_Jump, pattern->name());
			if (offBhopcap)
				EngineDevMsg("[client dll] Found the bhopcap pattern at %p.\n", reinterpret_cast<void*>(offBhopcap + reinterpret_cast<uintptr_t>(ORIG_PM_Jump)-27));
			if (pBhopcapWindows)
				EngineDevMsg("[client dll] Found bhopcap jump instruction at %p (using the %s pattern).\n", pBhopcapWindows, pattern2->name());
		} else {
			EngineDevWarning("[client dll] Could not find PM_Jump.\n");
			EngineWarning("Autojump prediction is not available.\n");
			if (!noBhopcap)
				EngineWarning("Bhopcap prediction disabling is not available.\n");
		}

		if (!ppmove)
			ppmove = reinterpret_cast<void**>(MemUtils::GetSymbolAddress(m_Handle, "pmove"));
	}

	{
		auto pattern = fEV_GetDefaultShellInfo.get();
		if (ORIG_EV_GetDefaultShellInfo) {
			EngineDevMsg("[client dll] Found EV_GetDefaultShellInfo at %p (using the %s pattern).\n", ORIG_EV_GetDefaultShellInfo, pattern->name());
		} else {
			ORIG_EV_GetDefaultShellInfo = reinterpret_cast<_EV_GetDefaultShellInfo>(MemUtils::GetSymbolAddress(m_Handle, "_Z22EV_GetDefaultShellInfoP12event_args_sPfS1_S1_S1_S1_S1_S1_fff"));
			if (ORIG_EV_GetDefaultShellInfo) {
				EngineDevMsg("[client dll] Found EV_GetDefaultShellInfo at %p.\n", ORIG_EV_GetDefaultShellInfo);
			} else {
				EngineDevWarning("[client dll] Could not find EV_GetDefaultShellInfo.\n");
				EngineWarning("[client dll] Special effects of weapons will be misplaced when bxt_viewmodel_fov is used.\n");
			}
		}
	}

	{
		auto pattern = fCStudioModelRenderer__StudioCalcAttachments.get();
		if (ORIG_CStudioModelRenderer__StudioCalcAttachments) {
			EngineDevMsg("[client dll] Found CStudioModelRenderer::StudioCalcAttachments at %p (using the %s pattern).\n", ORIG_CStudioModelRenderer__StudioCalcAttachments, pattern->name());
			if (offVectorTransform) {
				ORIG_VectorTransform = reinterpret_cast<_VectorTransform>(
					*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(ORIG_CStudioModelRenderer__StudioCalcAttachments) + offVectorTransform)
					+ reinterpret_cast<uintptr_t>(ORIG_CStudioModelRenderer__StudioCalcAttachments) + offVectorTransform + 4
				);
				EngineDevMsg("[client dll] Found VectorTransform at %p.\n", ORIG_VectorTransform);
			}
		} else {
			ORIG_CStudioModelRenderer__StudioCalcAttachments_Linux = reinterpret_cast<_CStudioModelRenderer__StudioCalcAttachments_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN20CStudioModelRenderer21StudioCalcAttachmentsEv"));
			ORIG_VectorTransform = reinterpret_cast<_VectorTransform>(MemUtils::GetSymbolAddress(m_Handle, "_Z15VectorTransformPKfPA4_fPf"));
			if (ORIG_CStudioModelRenderer__StudioCalcAttachments_Linux && ORIG_VectorTransform) {
				EngineDevMsg("[client dll] Found CStudioModelRenderer::StudioCalcAttachments [Linux] at %p.\n", ORIG_CStudioModelRenderer__StudioCalcAttachments_Linux);
				EngineDevMsg("[client dll] Found VectorTransform at %p.\n", ORIG_VectorTransform);
			} else {
				EngineDevWarning("[client dll] Could not find CStudioModelRenderer::StudioCalcAttachments.\n");
				EngineDevWarning("[client dll] Could not find VectorTransform.\n");
				EngineWarning("[client dll] Special effects of weapons will be misplaced when bxt_viewmodel_fov is used.\n");
			}
		}
	}

	{
		auto pattern = fCStudioModelRenderer__StudioSetupBones.get();
		if (ORIG_CStudioModelRenderer__StudioSetupBones) {
			EngineDevMsg("[client dll] Found CStudioModelRenderer::StudioSetupBones at %p (using the %s pattern).\n", ORIG_CStudioModelRenderer__StudioSetupBones, pattern->name());
		} else {
			ORIG_CStudioModelRenderer__StudioSetupBones_Linux = reinterpret_cast<_CStudioModelRenderer__StudioSetupBones_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN20CStudioModelRenderer16StudioSetupBonesEv"));
			if (ORIG_CStudioModelRenderer__StudioSetupBones_Linux) {
				EngineDevMsg("[client dll] Found CStudioModelRenderer::StudioSetupBones [Linux] at %p.\n", ORIG_CStudioModelRenderer__StudioSetupBones_Linux);
			} else {
				EngineDevWarning("[client dll] Could not find CStudioModelRenderer::StudioSetupBones.\n");
				EngineWarning("[client dll] Disabling weapon viewmodel idle or equip sequences is not available.\n");
			}
		}
	}

	{
		auto pattern = fCStudioModelRenderer__StudioRenderModel.get();
		if (ORIG_CStudioModelRenderer__StudioRenderModel) {
			EngineDevMsg("[client dll] Found CStudioModelRenderer::StudioRenderModel at %p (using the %s pattern).\n", ORIG_CStudioModelRenderer__StudioRenderModel, pattern->name());
		} else {
			ORIG_CStudioModelRenderer__StudioRenderModel_Linux = reinterpret_cast<_CStudioModelRenderer__StudioRenderModel_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN20CStudioModelRenderer17StudioRenderModelEv"));
			if (ORIG_CStudioModelRenderer__StudioRenderModel_Linux) {
				EngineDevMsg("[client dll] Found CStudioModelRenderer::StudioRenderModel [Linux] at %p.\n", ORIG_CStudioModelRenderer__StudioRenderModel_Linux);
			} else {
				EngineDevWarning("[client dll] Could not find CStudioModelRenderer::StudioRenderModel.\n");
				EngineWarning("[client dll] Changing weapon viewmodel opacity is not available.\n");
			}
		}
	}

	{
		auto pattern = fScaleColors.get();
		if (ORIG_ScaleColors) {
			EngineDevMsg("[client dll] Found ScaleColors at %p (using the %s pattern).\n", ORIG_ScaleColors, pattern->name());
		} else {
			ORIG_ScaleColors = reinterpret_cast<_ScaleColors>(MemUtils::GetSymbolAddress(m_Handle, "_Z11ScaleColorsRiS_S_i"));
			if (ORIG_ScaleColors) {
				EngineDevMsg("[client dll] Found ScaleColors at %p.\n", ORIG_ScaleColors);
			} else {
				EngineDevWarning("[client dll] Could not find ScaleColors.\n");
				EngineWarning("[client dll] Changing HUD color for ammo history and damage icons is not available.\n");
			}
		}
	}

	{
		auto pattern = fV_PunchAxis.get();
		if (ORIG_V_PunchAxis) {
			EngineDevMsg("[client dll] Found V_PunchAxis at %p (using the %s pattern).\n", ORIG_V_PunchAxis, pattern->name());
		}
		else {
			ORIG_V_PunchAxis = reinterpret_cast<_V_PunchAxis>(MemUtils::GetSymbolAddress(m_Handle, "_Z11V_PunchAxisif"));
			if (ORIG_V_PunchAxis) {
				EngineDevMsg("[client dll] Found V_PunchAxis at %p.\n", ORIG_V_PunchAxis);
			} else {
				EngineDevWarning("[client dll] Could not find V_PunchAxis.\n");
			}
		}
	}

	{
		auto pattern = fPM_Duck.get();
		if (ORIG_PM_Duck) {
			EngineDevMsg("[client dll] Found PM_Duck at %p (using the %s pattern).\n", ORIG_PM_Duck, pattern->name());
		} else {
			EngineDevWarning("[client dll] Could not find PM_Duck.\n");
		}
	}

	{
		auto pattern = fPM_UnDuck.get();
		if (ORIG_PM_UnDuck) {
			EngineDevMsg("[client dll] Found PM_UnDuck at %p (using the %s pattern).\n", ORIG_PM_UnDuck, pattern->name());
		} else {
			EngineDevWarning("[client dll] Could not find PM_UnDuck.\n");
		}
	}

	{
		auto pattern = fHistoryResource__DrawAmmoHistory.get();
		if (ORIG_HistoryResource__DrawAmmoHistory) {
			EngineDevMsg("[client dll] Found HistoryResource::DrawAmmoHistory at %p (using the %s pattern).\n", ORIG_HistoryResource__DrawAmmoHistory, pattern->name());
		} else {
			ORIG_HistoryResource__DrawAmmoHistory_Linux = reinterpret_cast<_HistoryResource__DrawAmmoHistory_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN15HistoryResource15DrawAmmoHistoryEf"));
			if (ORIG_HistoryResource__DrawAmmoHistory_Linux) {
				EngineDevMsg("[client dll] Found HistoryResource::DrawAmmoHistory [Linux] at %p.\n", ORIG_HistoryResource__DrawAmmoHistory_Linux);
			} else {
				EngineDevWarning("[client dll] Could not find HistoryResource::DrawAmmoHistory [Linux].\n");
			}
		}
	}

	{
		auto pattern = fCHudHealth__DrawDamage.get();
		if (ORIG_CHudHealth__DrawDamage) {
			EngineDevMsg("[client dll] Found CHudHealth::DrawDamage at %p (using the %s pattern).\n", ORIG_CHudHealth__DrawDamage, pattern->name());
		} else {
			ORIG_CHudHealth__DrawDamage_Linux = reinterpret_cast<_CHudHealth__DrawDamage_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN10CHudHealth10DrawDamageEf"));
			if (ORIG_CHudHealth__DrawDamage_Linux) {
				EngineDevMsg("[client dll] Found CHudHealth::DrawDamage [Linux] at %p.\n", ORIG_CHudHealth__DrawDamage_Linux);
			} else {
				EngineDevWarning("[client dll] Could not find CHudHealth::DrawDamage [Linux].\n");
			}
		}
	}

	{
		auto pattern = fCHudHealth__DrawPain.get();
		if (ORIG_CHudHealth__DrawPain) {
			EngineDevMsg("[client dll] Found CHudHealth::DrawPain at %p (using the %s pattern).\n", ORIG_CHudHealth__DrawPain, pattern->name());
		} else {
			ORIG_CHudHealth__DrawPain_Linux = reinterpret_cast<_CHudHealth__DrawPain_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN10CHudHealth8DrawPainEf"));
			if (ORIG_CHudHealth__DrawPain_Linux) {
				EngineDevMsg("[client dll] Found CHudHealth::DrawPain [Linux] at %p.\n", ORIG_CHudHealth__DrawPain_Linux);
			} else {
				EngineDevWarning("[client dll] Could not find CHudHealth::DrawPain [Linux].\n");
			}
		}
	}

	{
		auto pattern = fCHudFlashlight__drawNightVision.get();
		if (ORIG_CHudFlashlight__drawNightVision) {
			EngineDevMsg("[client dll] Found CHudFlashlight::drawNightVision at %p (using the %s pattern).\n", ORIG_CHudFlashlight__drawNightVision, pattern->name());
		} else {
			ORIG_CHudFlashlight__drawNightVision_Linux = reinterpret_cast<_CHudFlashlight__drawNightVision_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN14CHudFlashlight15drawNightVisionEv"));
			if (ORIG_CHudFlashlight__drawNightVision_Linux) {
				EngineDevMsg("[client dll] Found CHudFlashlight::drawNightVision [Linux] at %p.\n", ORIG_CHudFlashlight__drawNightVision_Linux);
			} else {
				EngineDevWarning("[client dll] Could not find HudFlashlight::drawNightVision [Linux].\n");
				EngineWarning("[client dll] Disabling Opposing Force nightvision sprite is unavailable.\n");
			}
		}
	}

	{
		auto pattern = fCHud__DrawHudNightVision.get();
		if (ORIG_CHud__DrawHudNightVision) {
			EngineDevMsg("[client dll] Found CHud::DrawHudNightVision at %p (using the %s pattern).\n", ORIG_CHud__DrawHudNightVision, pattern->name());
		} else {
			ORIG_CHud__DrawHudNightVision_Linux = reinterpret_cast<_CHud__DrawHudNightVision_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN4CHud18DrawHudNightVisionEf"));
			if (ORIG_CHud__DrawHudNightVision_Linux) {
				EngineDevMsg("[client dll] Found CHud::DrawHudNightVision [Linux] at %p.\n", ORIG_CHud__DrawHudNightVision_Linux);
			} else {
				EngineDevWarning("[client dll] Could not find CHud::DrawHudNightVision [Linux].\n");
			}
		}
	}

	{
		auto pattern = fCHud__DrawHudFiberCamera.get();
		if (ORIG_CHud__DrawHudFiberCamera) {
			EngineDevMsg("[client dll] Found CHud::DrawHudFiberCamera at %p (using the %s pattern).\n", ORIG_CHud__DrawHudFiberCamera, pattern->name());
		} else {
			ORIG_CHud__DrawHudFiberCamera_Linux = reinterpret_cast<_CHud__DrawHudFiberCamera_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN4CHud18DrawHudFiberCameraEf"));
			if (ORIG_CHud__DrawHudFiberCamera_Linux) {
				EngineDevMsg("[client dll] Found CHud::DrawHudFiberCamera [Linux] at %p.\n", ORIG_CHud__DrawHudFiberCamera_Linux);
			} else {
				EngineDevWarning("[client dll] Could not find CHud::DrawHudFiberCamera [Linux].\n");
			}
		}
	}

	{
		auto pattern = fCHudIcons__Draw.get();
		if (ORIG_CHudIcons__Draw) {
			EngineDevMsg("[client dll] Found CHudIcons::Draw at %p (using the %s pattern).\n", ORIG_CHudIcons__Draw, pattern->name());
		} else {
			ORIG_CHudIcons__Draw_Linux = reinterpret_cast<_CHudIcons__Draw_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN9CHudIcons4DrawEf"));
			if (ORIG_CHudIcons__Draw_Linux) {
				EngineDevMsg("[client dll] Found CHudIcons::Draw [Linux] at %p.\n", ORIG_CHudIcons__Draw_Linux);
			} else {
				EngineDevWarning("[client dll] Could not find CHudIcons::Draw [Linux].\n");
			}
		}
	}

	{
		auto pattern = fCS_AngleSpeedCap.get();
		if (pCS_AngleSpeedCap) {
				EngineDevMsg("[client dll] Found the angle speed cap pattern at %p (using the %s pattern).\n", pCS_AngleSpeedCap, pattern->name());
		} else {
			if (pCS_AngleSpeedCap_Linux) {
				pattern = fCS_AngleSpeedCap_Linux.get();
				EngineDevMsg("[client dll] Found the angle speed cap pattern [Linux] at %p (using the %s pattern).\n", pCS_AngleSpeedCap_Linux, pattern->name());
			} else {
				EngineDevWarning("[client dll] Could not find angle speed cap pattern.\n");
			}
		}
	}

	{
		auto pattern = fCS_SpeedScaling.get();
		if (pCS_SpeedScaling) {
			EngineDevMsg("[client dll] Found the speed scaling pattern at %p (using the %s pattern).\n", pCS_SpeedScaling, pattern->name());
		} else {
			if (pCS_SpeedScaling_Linux) {
				pattern = fCS_SpeedScaling_Linux.get();
				EngineDevMsg("[client dll] Found the speed scaling pattern [Linux] at %p (using the %s pattern).\n", pCS_SpeedScaling_Linux, pattern->name());
			} else {
				EngineDevWarning("[client dll] Could not find the speed scaling pattern.\n");
			}
		}
	}
}

bool ClientDLL::FindHUDFunctions()
{
	if ((ORIG_HUD_Init = reinterpret_cast<_HUD_Init>(MemUtils::GetSymbolAddress(m_Handle, "HUD_Init")))) {
		EngineDevMsg("[client dll] Found HUD_Init at %p.\n", ORIG_HUD_Init);
	} else {
		EngineDevWarning("[client dll] Could not find HUD_Init.\n");
		return false;
	}

	if ((ORIG_HUD_VidInit = reinterpret_cast<_HUD_VidInit>(MemUtils::GetSymbolAddress(m_Handle, "HUD_VidInit")))) {
		EngineDevMsg("[client dll] Found HUD_VidInit at %p.\n", ORIG_HUD_VidInit);
	} else {
		EngineDevWarning("[client dll] Could not find HUD_VidInit.\n");
		return false;
	}

	if ((ORIG_HUD_Reset = reinterpret_cast<_HUD_Reset>(MemUtils::GetSymbolAddress(m_Handle, "HUD_Reset")))) {
		EngineDevMsg("[client dll] Found HUD_Reset at %p.\n", ORIG_HUD_Reset);
	} else {
		EngineDevWarning("[client dll] Could not find HUD_Reset.\n");
		return false;
	}

	if ((ORIG_HUD_Redraw = reinterpret_cast<_HUD_Redraw>(MemUtils::GetSymbolAddress(m_Handle, "HUD_Redraw")))) {
		EngineDevMsg("[client dll] Found HUD_Redraw at %p.\n", ORIG_HUD_Redraw);
	} else {
		EngineDevWarning("[client dll] Could not find HUD_Redraw.\n");
		EngineWarning("bxt_disable_hud is not available.\n");
		return false;
	}

	return true;
}

void ClientDLL::RegisterCVarsAndCommands()
{
	EngineDevMsg("[client dll] Registering CVars.\n");

	#define REG(cvar) HwDLL::GetInstance().RegisterCVar(CVars::cvar)

	if (ORIG_PM_Jump)
		REG(bxt_autojump_prediction);

	if (ORIG_PM_PreventMegaBunnyJumping || pBhopcapWindows)
		REG(bxt_bhopcap_prediction);

	if (ORIG_HUD_DrawTransparentTriangles && pEngfuncs) {
		REG(bxt_show_triggers);
		REG(bxt_show_custom_triggers);
		REG(bxt_triggers_color);
		REG(bxt_show_nodes);
		REG(bxt_show_pickup_bbox);
		REG(bxt_show_player_bbox);
		REG(bxt_show_monster_bbox);
		REG(bxt_show_displacer_earth_targets);
		REG(bxt_hud_useables);
		REG(bxt_hud_useables_radius);
		REG(bxt_show_splits);
		REG(bxt_splits_color);
	}

	if (ORIG_V_CalcRefdef) {
		REG(bxt_unlock_camera_during_pause);
		REG(bxt_viewmodel_ofs_forward);
		REG(bxt_viewmodel_ofs_right);
		REG(bxt_viewmodel_ofs_up);
		REG(bxt_viewmodel_bob_angled);
		REG(bxt_remove_punchangles);
	}

	if (ORIG_HUD_Init)
	{
		CVars::con_color.Assign(HwDLL::GetInstance().FindCVar("con_color"));
		REG(bxt_hud);
		REG(bxt_hud_color);
		REG(bxt_hud_precision);
		REG(bxt_hud_quickgauss);
		REG(bxt_hud_quickgauss_offset);
		REG(bxt_hud_quickgauss_anchor);
		REG(bxt_hud_velocity);
		REG(bxt_hud_velocity_offset);
		REG(bxt_hud_velocity_anchor);
		REG(bxt_hud_origin);
		REG(bxt_hud_origin_offset);
		REG(bxt_hud_origin_anchor);
		REG(bxt_hud_viewangles);
		REG(bxt_hud_viewangles_offset);
		REG(bxt_hud_viewangles_anchor);
		REG(bxt_hud_distance);
		REG(bxt_hud_distance_offset);
		REG(bxt_hud_distance_anchor);
		REG(bxt_hud_entity_info);
		REG(bxt_hud_entity_info_offset);
		REG(bxt_hud_entity_info_anchor);
		REG(bxt_hud_selfgauss);
		REG(bxt_hud_selfgauss_offset);
		REG(bxt_hud_selfgauss_anchor);
		REG(bxt_hud_armor);
		REG(bxt_hud_armor_offset);
		REG(bxt_hud_armor_anchor);
		REG(bxt_hud_waterlevel);
		REG(bxt_hud_waterlevel_offset);
		REG(bxt_hud_waterlevel_anchor);
		REG(bxt_hud_speedometer);
		REG(bxt_hud_speedometer_offset);
		REG(bxt_hud_speedometer_anchor);
		REG(bxt_hud_jumpspeed);
		REG(bxt_hud_jumpspeed_offset);
		REG(bxt_hud_jumpspeed_anchor);
		REG(bxt_hud_jumpdistance);
		REG(bxt_hud_jumpdistance_offset);
		REG(bxt_hud_jumpdistance_anchor);
		REG(bxt_hud_health);
		REG(bxt_hud_health_offset);
		REG(bxt_hud_health_anchor);
		REG(bxt_hud_health_override_in_demo);
		REG(bxt_hud_nihilanth);
		REG(bxt_hud_nihilanth_offset);
		REG(bxt_hud_nihilanth_anchor);
		REG(bxt_hud_gonarch);
		REG(bxt_hud_gonarch_offset);
		REG(bxt_hud_gonarch_anchor);
		REG(bxt_hud_timer);
		REG(bxt_hud_timer_offset);
		REG(bxt_hud_timer_anchor);
		REG(bxt_hud_visible_landmarks);
		REG(bxt_hud_visible_landmarks_offset);
		REG(bxt_hud_visible_landmarks_anchor);
		REG(bxt_hud_incorrect_fps_indicator);
		REG(bxt_hud_tas_editor_status);
		REG(bxt_hud_tas_editor_status_offset);
		REG(bxt_hud_tas_editor_status_anchor);
		REG(bxt_hud_entities);
		REG(bxt_hud_entities_offset);
		REG(bxt_hud_entities_anchor);
		REG(bxt_cross);
		REG(bxt_cross_color);
		REG(bxt_cross_alpha);
		REG(bxt_cross_thickness);
		REG(bxt_cross_size);
		REG(bxt_cross_gap);
		REG(bxt_cross_outline);
		REG(bxt_cross_circle_radius);
		REG(bxt_cross_dot_color);
		REG(bxt_cross_dot_size);
		REG(bxt_cross_top_line);
		REG(bxt_cross_bottom_line);
		REG(bxt_cross_left_line);
		REG(bxt_cross_right_line);
		REG(bxt_hud_stamina);
		REG(bxt_hud_stamina_offset);
		REG(bxt_hud_stamina_anchor);
		REG(bxt_hud_split);
		REG(bxt_hud_split_speed);
		REG(bxt_hud_split_offset);
		REG(bxt_hud_split_anchor);
		REG(bxt_hud_split_duration);
		REG(bxt_hud_split_fadeout);
	}

	if (ORIG_HUD_Redraw) {
		REG(bxt_disable_hud);
		REG(bxt_hud_game_color);
	}

	if (ORIG_HUD_AddEntity) {
		REG(bxt_show_hidden_entities_clientside);
		REG(bxt_show_triggers_legacy_alpha);
		REG(bxt_show_only_players);
		REG(bxt_disable_beams);
		REG(bxt_disable_brush_entities);
		REG(bxt_disable_sprite_entities);
		REG(bxt_disable_studio_entities);
		REG(bxt_disable_player_corpses);
		REG(bxt_hide_other_players);
	}

	if (ORIG_ScaleColors) {
		REG(bxt_hud_game_alpha);
	}

	if (ORIG_CHudFlashlight__drawNightVision_Linux || ORIG_CHudFlashlight__drawNightVision || ORIG_CHud__DrawHudNightVision_Linux || ORIG_CHud__DrawHudNightVision) {
		REG(bxt_disable_nightvision_sprite);
	}

	if (pCS_AngleSpeedCap || pCS_AngleSpeedCap_Linux) {
		REG(bxt_anglespeed_cap);
	}

	if (pCS_SpeedScaling || pCS_SpeedScaling_Linux) {
		REG(bxt_speed_scaling);
	}

	if ((ORIG_CHudHealth__DrawDamage || ORIG_CHudHealth__DrawDamage_Linux) && ORIG_ScaleColors) {
		REG(bxt_hud_game_alpha_damage);
	}

	if (is_cof_client) {
		if (ORIG_PM_Duck)
			REG(bxt_cof_slowdown_if_use_on_ground_prediction);
		if (ORIG_PM_UnDuck)
			REG(bxt_cof_enable_ducktap_prediction);
	}
	#undef REG
}

void ClientDLL::SetMouseState(bool active)
{
	if (ORIG_IN_ActivateMouse && ORIG_IN_DeactivateMouse) {
		if (active)
			ORIG_IN_ActivateMouse();
		else
			ORIG_IN_DeactivateMouse();
	}
}

Vector ClientDLL::AnglesToForward(const Vector &angles) {
	assert(pEngfuncs);

	Vector forward, right, up;
	pEngfuncs->pfnAngleVectors(angles, forward, right, up);
	return forward;
}

void ClientDLL::StudioAdjustViewmodelAttachments(Vector &vOrigin)
{
	// V_CalcRefDef didn't give us info, let's bail out since we'd just make the attachments be somewhere in the world
	// and nowhere near the player's viewmodel
	if (last_vieworg.x == 0.0f && last_vieworg.y == 0.0f && last_vieworg.z == 0.0f)
		return;

	double worldx = tan(HwDLL::GetInstance().currentRenderFOV  * M_PI / 360.0);
	double viewx = tan(CVars::bxt_viewmodel_fov.GetFloat() * M_PI / 360.0);

	// aspect ratio cancels out, so only need one factor
	// the difference between the screen coordinates of the 2 systems is the ratio
	// of the coefficients of the projection matrices (tan (fov/2) is that coefficient)
	float factor = (float)worldx / (float)viewx;

	// Get the coordinates in the viewer's space.
	Vector tmp = vOrigin - last_vieworg;

	Vector vTransformed(DotProduct(last_viewright, tmp), DotProduct(last_viewup, tmp), DotProduct(last_viewforward, tmp));

	// Now squash X and Y.
	vTransformed.x *= factor;
	vTransformed.y *= factor;

	// Transform back to world space.
	Vector vOut = (last_viewright * vTransformed.x) + (last_viewup * vTransformed.y) + (last_viewforward * vTransformed.z);
	vOrigin = last_vieworg + vOut;
}

void ClientDLL::GetViewAngles(float *va)
{
	auto &hw = HwDLL::GetInstance();
	if (pEngfuncs && hw.pEngStudio)
	{
		pEngfuncs->GetViewAngles(va);
	}
	else if (hw.viewangles)
	{
		va[0] = hw.viewangles->x;
		va[1] = hw.viewangles->y;
		va[2] = hw.viewangles->z;
	}
}

void ClientDLL::SetViewAngles(float *va)
{
	auto &hw = HwDLL::GetInstance();
	if (pEngfuncs && hw.pEngStudio)
	{
		pEngfuncs->SetViewAngles(va);
	}
	else if (hw.viewangles)
	{
		hw.viewangles->x = va[0];
		hw.viewangles->y = va[1];
		hw.viewangles->z = va[2];
	}
}

cl_entity_t* ClientDLL::GetLocalPlayer()
{
	auto &hw = HwDLL::GetInstance();
	if (pEngfuncs && hw.pEngStudio)
	{
		return pEngfuncs->GetLocalPlayer();
	}
	else if (hw.cl_entities && hw.playernum)
	{
		int pl_num = *hw.playernum;
		return (*hw.cl_entities) + pl_num + 1;
	} 
	else 
	{
		return NULL;
	}
}

const char* ClientDLL::GetLevelName()
{
	auto& hw = HwDLL::GetInstance();

	if (hw.is_sdk10)
		return GetLevelNameSv();
	else if (pEngfuncs && hw.pEngStudio)
		return pEngfuncs->pfnGetLevelName();
	else
		return GetLevelNameCl();
}

const char* ClientDLL::GetLevelNameCl()
{
	auto& hw = HwDLL::GetInstance();
	if (!hw.cls || !hw.levelname)
		return "";

	int* state = reinterpret_cast<int*>(hw.cls);
	if (*state < 3)
		return "";

	return hw.levelname;
}

const char* ClientDLL::GetLevelNameSv()
{
	auto &hw = HwDLL::GetInstance();

	if (!hw.offName || !hw.psv)
		return "";

	auto levelname = reinterpret_cast<char*>(reinterpret_cast<uintptr_t>(hw.psv) + hw.offName);

	return levelname;
}

cl_entity_t* ClientDLL::GetViewModel()
{
	auto& hw = HwDLL::GetInstance();

	if (hw.pEngStudio && pEngfuncs)
		return pEngfuncs->GetViewModel();
	else if (hw.viewent)
		return hw.viewent;
	else
		return NULL;
}

cl_entity_t* ClientDLL::GetCurrentEntity()
{
	auto& hw = HwDLL::GetInstance();

	if (hw.pEngStudio)
		return hw.pEngStudio->GetCurrentEntity();
	else if (hw.currententity)
		return *hw.currententity;
	else
		return NULL;
}

float ClientDLL::GetTime()
{
	if (HwDLL::GetInstance().cl_time)
		return static_cast<float>(*HwDLL::GetInstance().cl_time);

	return 0.0f;
}

void ClientDLL::FileBase(const char *in, char *out)
{
	int len, start, end;

	len = strlen(in);

	// scan backward for '.'
	end = len - 1;
	while (0 != end && in[end] != '.' && in[end] != '/' && in[end] != '\\')
		end--;

	if (in[end] != '.')		// no '.', copy to end
		end = len - 1;
	else
		end--;			// Found ',', copy to left of '.'

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

void ClientDLL::ConvertToLowerCase(const char *str)
{
	unsigned char *str_lw = (unsigned char *)str;
	while (*str_lw) {
		*str_lw = tolower(*str_lw);
		str_lw++;
	}
}

bool ClientDLL::DoesGameDirMatch(const char *game)
{
	if (!ServerDLL::GetInstance().pEngfuncs)
		return false;

	char gameDir[260];
	char gd[260];

	ServerDLL::GetInstance().pEngfuncs->pfnGetGameDir(gameDir);

	if (gameDir && gameDir[0])
	{
		FileBase(gameDir, gd);
		ConvertToLowerCase(gd);
	}

	return !std::strcmp(gd, game);
}

bool ClientDLL::DoesGameDirContain(const char *game)
{
	if (!ServerDLL::GetInstance().pEngfuncs)
		return false;

	char gameDir[260];
	char gd[260];

	ServerDLL::GetInstance().pEngfuncs->pfnGetGameDir(gameDir);

	if (gameDir && gameDir[0])
	{
		FileBase(gameDir, gd);
		ConvertToLowerCase(gd);
	}

	return std::strstr(gd, game);
}

size_t ClientDLL::GetMapName(char* dest, size_t count)
{
	auto map_path = GetLevelName();

	const char* slash = strrchr(map_path, '/');
	if (!slash)
		slash = map_path - 1;

	const char* dot = strrchr(map_path, '.');
	if (!dot)
		dot = map_path + strlen(map_path);

	size_t bytes_to_copy = std::min(count - 1, static_cast<size_t>(dot - slash - 1));

	strncpy(dest, slash + 1, bytes_to_copy);
	dest[bytes_to_copy] = '\0';

	return bytes_to_copy;
}

bool ClientDLL::DoesMapNameMatch(const char *map)
{
	if (!pEngfuncs)
		return false;

	char map_name[64];

	GetMapName(map_name, ARRAYSIZE_HL(map_name));

	if (map_name[0])
		ConvertToLowerCase(map_name);

	return !std::strcmp(map_name, map);
}

bool ClientDLL::DoesMapNameContain(const char *map)
{
	if (!pEngfuncs)
		return false;

	char map_name[64];

	GetMapName(map_name, ARRAYSIZE_HL(map_name));

	if (map_name[0])
		ConvertToLowerCase(map_name);

	return std::strstr(map_name, map);
}

void ClientDLL::SetAngleSpeedCap(bool capped)
{
	if (!pCS_AngleSpeedCap && !pCS_AngleSpeedCap_Linux) {
		return;
	}

	if (capped) { // restore the bytes
		if (pCS_AngleSpeedCap
			&& *reinterpret_cast<byte*>(pCS_AngleSpeedCap + 5) == 0xEB
			&& *reinterpret_cast<byte*>(pCS_AngleSpeedCap + 37) == 0xEB
			&& *reinterpret_cast<byte*>(pCS_AngleSpeedCap + 328) == 0xEB
			&& *reinterpret_cast<byte*>(pCS_AngleSpeedCap + 360) == 0xEB)
		{
			MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCS_AngleSpeedCap + 5), 1, reinterpret_cast<const byte*>("\x7B"));
			MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCS_AngleSpeedCap + 37), 1, reinterpret_cast<const byte*>("\x7A"));
			MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCS_AngleSpeedCap + 328), 1, reinterpret_cast<const byte*>("\x7B"));
			MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCS_AngleSpeedCap + 360), 1, reinterpret_cast<const byte*>("\x7A"));
		}
		else if (pCS_AngleSpeedCap_Linux
			&& *reinterpret_cast<byte*>(pCS_AngleSpeedCap_Linux + 79) == 0xD8
			&& *reinterpret_cast<byte*>(pCS_AngleSpeedCap_Linux + 1089) == 0xD8
			&& *reinterpret_cast<byte*>(pCS_AngleSpeedCap_Linux + 359) == 0xD8
			&& *reinterpret_cast<byte*>(pCS_AngleSpeedCap_Linux + 801) == 0xD8)
		{
			MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCS_AngleSpeedCap_Linux + 79), 1, reinterpret_cast<const byte*>("\xD9"));
			MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCS_AngleSpeedCap_Linux + 1089), 1, reinterpret_cast<const byte*>("\xD9"));
			MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCS_AngleSpeedCap_Linux + 359), 1, reinterpret_cast<const byte*>("\xD9"));
			MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCS_AngleSpeedCap_Linux + 801), 1, reinterpret_cast<const byte*>("\xD9"));
		}
	} else {
		if (pCS_AngleSpeedCap
			&& *reinterpret_cast<byte*>(pCS_AngleSpeedCap + 5) == 0x7B
			&& *reinterpret_cast<byte*>(pCS_AngleSpeedCap + 37) == 0x7A
			&& *reinterpret_cast<byte*>(pCS_AngleSpeedCap + 328) == 0x7B
			&& *reinterpret_cast<byte*>(pCS_AngleSpeedCap + 360) == 0x7A)
		{
			MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCS_AngleSpeedCap + 5), 1, reinterpret_cast<const byte*>("\xEB"));
			MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCS_AngleSpeedCap + 37), 1, reinterpret_cast<const byte*>("\xEB"));
			MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCS_AngleSpeedCap + 328), 1, reinterpret_cast<const byte*>("\xEB"));
			MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCS_AngleSpeedCap + 360), 1, reinterpret_cast<const byte*>("\xEB"));
		}
		else if (pCS_AngleSpeedCap_Linux
			&& *reinterpret_cast<byte*>(pCS_AngleSpeedCap_Linux + 79) == 0xD9
			&& *reinterpret_cast<byte*>(pCS_AngleSpeedCap_Linux + 1089) == 0xD9
			&& *reinterpret_cast<byte*>(pCS_AngleSpeedCap_Linux + 359) == 0xD9
			&& *reinterpret_cast<byte*>(pCS_AngleSpeedCap_Linux + 801) == 0xD9)
		{
			MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCS_AngleSpeedCap_Linux + 79), 1, reinterpret_cast<const byte*>("\xD8"));
			MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCS_AngleSpeedCap_Linux + 1089), 1, reinterpret_cast<const byte*>("\xD8"));
			MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCS_AngleSpeedCap_Linux + 359), 1, reinterpret_cast<const byte*>("\xD8"));
			MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCS_AngleSpeedCap_Linux + 801), 1, reinterpret_cast<const byte*>("\xD8"));
		}
	}
}

void ClientDLL::SetSpeedScaling(bool scaled)
{
	if (!pCS_SpeedScaling && !pCS_SpeedScaling_Linux) {
		return;
	}

	if (scaled) {
		if (pCS_SpeedScaling && *reinterpret_cast<byte*>(pCS_SpeedScaling + 19) == 0xEB)
			MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCS_SpeedScaling + 19), 1, reinterpret_cast<const byte*>("\x75"));
		else if (pCS_SpeedScaling_Linux
			&& *reinterpret_cast<byte*>(pCS_SpeedScaling_Linux + 2) == 0xE9
			&& *reinterpret_cast<byte*>(pCS_SpeedScaling_Linux + 3) == 0x62) {
			if (*reinterpret_cast<byte*>(pCS_SpeedScaling_Linux + 4) == 0xFE)
				MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCS_SpeedScaling_Linux + 2), 4, reinterpret_cast<const byte*>("\x0F\x86\x61\xFE"));
			else // csczds byte
				MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCS_SpeedScaling_Linux + 2), 4, reinterpret_cast<const byte*>("\x0F\x86\x5D\xFD"));
		}

	} else {
		if (pCS_SpeedScaling && *reinterpret_cast<byte*>(pCS_SpeedScaling + 19) == 0x75)
			MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCS_SpeedScaling + 19), 1, reinterpret_cast<const byte*>("\xEB"));
		else if (pCS_SpeedScaling_Linux
			&& *reinterpret_cast<byte*>(pCS_SpeedScaling_Linux + 2) == 0x0F
			&& *reinterpret_cast<byte*>(pCS_SpeedScaling_Linux + 3) == 0x86) {
			if (*reinterpret_cast<byte*>(pCS_SpeedScaling_Linux + 4) == 0x61)
				MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCS_SpeedScaling_Linux + 2), 4, reinterpret_cast<const byte*>("\xE9\x62\xFE\xFF"));
			else
				MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCS_SpeedScaling_Linux + 2), 4, reinterpret_cast<const byte*>("\xE9\x5E\xFD\xFF"));
		}

	}
}

void ClientDLL::SetupTraceVectors(float start[3], float end[3])
{
	auto& hw = HwDLL::GetInstance();

	Vector forward, right, up, view, angles;
	if (hw.is_sdk10 && hw.r_refdef)
	{
		view = hw.r_refdef->vieworg;
		angles = hw.r_refdef->viewangles;
	}
	else
	{
		view = last_vieworg;
		angles = last_viewangles;
	}

	pEngfuncs->pfnAngleVectors(angles, forward, right, up);

	Vector end_ = view + forward * 8192;

	start[0] = view[0];
	start[1] = view[1];
	start[2] = view[2];
	end[0] = end_[0];
	end[1] = end_[1];
	end[2] = end_[2];
}

void ClientDLL::SetStuffInHudFrame()
{
	auto& hw = HwDLL::GetInstance();
	static bool check_forcehltv = true;
	if (check_forcehltv) {
		check_forcehltv = false;
		if (hw.ORIG_Cmd_FindCmd)
			orig_forcehltv_found = hw.ORIG_Cmd_FindCmd("dem_forcehltv");
		else if (hw.ORIG_Cmd_Exists)
			orig_forcehltv_found = hw.ORIG_Cmd_Exists("dem_forcehltv");
	}

	#ifdef _WIN32
	static bool check_vsync = true;
	if (check_vsync)
	{
		bool bxtDisableVSync = getenv("BXT_DISABLE_VSYNC");
		if (bxtDisableVSync)
		{
			typedef BOOL(APIENTRY* PFNWGLSWAPINTERVALPROC)(int);
			PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = 0;
			wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress("wglSwapIntervalEXT");
			if (wglSwapIntervalEXT)
				wglSwapIntervalEXT(0);
		}
		check_vsync = false;
	}
	#endif

	discord_integration::on_frame();
}

HOOK_DEF_0(ClientDLL, void, __cdecl, PM_Jump)
{
	auto pmove = reinterpret_cast<uintptr_t>(*ppmove);
	int *onground = reinterpret_cast<int*>(pmove + offOnground);
	int orig_onground = *onground;

	int *oldbuttons = reinterpret_cast<int*>(pmove + offOldbuttons);
	int orig_oldbuttons = *oldbuttons;

	if (CVars::bxt_force_jumpless.GetBool())
		return;

	if (CVars::bxt_autojump_prediction.GetBool())
	{
		if ((orig_onground != -1) && !cantJumpNextTime) {
			if (HwDLL::GetInstance().ducktap == false || (HwDLL::GetInstance().ducktap == true && CVars::bxt_autojump_priority.GetBool())) {
				*oldbuttons &= ~IN_JUMP;
			}
		}
	}

	cantJumpNextTime = false;

	if (offBhopcap)
	{
		auto pPMJump = reinterpret_cast<ptrdiff_t>(ORIG_PM_Jump);
		if (CVars::bxt_bhopcap_prediction.GetBool())
		{
			if (*reinterpret_cast<byte*>(pPMJump + offBhopcap) == 0x90
				&& *reinterpret_cast<byte*>(pPMJump + offBhopcap + 1) == 0x90)
				MemUtils::ReplaceBytes(reinterpret_cast<void*>(pPMJump + offBhopcap), 6, originalBhopcapInsn);
		}
		else if (*reinterpret_cast<byte*>(pPMJump + offBhopcap) == 0x0F
				&& *reinterpret_cast<byte*>(pPMJump + offBhopcap + 1) == 0x82)
				MemUtils::ReplaceBytes(reinterpret_cast<void*>(pPMJump + offBhopcap), 6, reinterpret_cast<const byte*>("\x90\x90\x90\x90\x90\x90"));
	}

	if (pBhopcapWindows)
		MemUtils::ReplaceBytes(reinterpret_cast<void*>(pBhopcapWindows), 1, reinterpret_cast<const byte*>(CVars::bxt_bhopcap_prediction.GetBool() ? "\x83" : "\x82"));

	ORIG_PM_Jump();

	if ((orig_onground != -1) && (*onground == -1))
		cantJumpNextTime = true;

	if (CVars::bxt_autojump_prediction.GetBool())
		*oldbuttons = orig_oldbuttons;
}

HOOK_DEF_1(ClientDLL, void, __cdecl, PM_PlayerMove, qboolean, server)
{
	ORIG_PM_PlayerMove(server);
}

HOOK_DEF_0(ClientDLL, void, __cdecl, PM_PreventMegaBunnyJumping)
{
	if (CVars::bxt_bhopcap_prediction.GetBool())
		ORIG_PM_PreventMegaBunnyJumping();
}

HOOK_DEF_4(ClientDLL, int, __cdecl, PM_ClipVelocity, float*, in, float*, normal, float*, out, float, overbounce)
{
	return ORIG_PM_ClipVelocity(in, normal, out, overbounce);
}

HOOK_DEF_0(ClientDLL, void, __cdecl, PM_WaterMove)
{
	return ORIG_PM_WaterMove();
}

HOOK_DEF_2(ClientDLL, void, __cdecl, PM_Move, struct playermove_s*, ppmove, int, server)
{
	return ORIG_PM_Move(ppmove, server);
}

HOOK_DEF_1(ClientDLL, void, __cdecl, V_CalcRefdef, ref_params_t*, pparams)
{
	if (CVars::bxt_remove_punchangles.GetBool() && CVars::sv_cheats.GetBool()) {
		pparams->punchangle[0] = 0.0f;
		pparams->punchangle[1] = 0.0f;
		pparams->punchangle[2] = 0.0f;
	}

	CustomHud::UpdatePlayerInfoInaccurate(pparams->simvel, pparams->simorg);

	const HwDLL &hwDLL = HwDLL::GetInstance();

	static bool free_cam_was_active = false;

	auto paused = pparams->paused;
	auto unlock_camera = CVars::bxt_unlock_camera_during_pause.GetBool() || hwDLL.free_cam_active
		// We want to unlock the camera 1 frame after free cam has been disabled so that it can
		// snap back to its original position.
		|| (!hwDLL.free_cam_active && free_cam_was_active);
	free_cam_was_active = hwDLL.free_cam_active;

	if (unlock_camera)
		pparams->paused = false;

	ORIG_V_CalcRefdef(pparams);

	float forward_offset = CVars::bxt_viewmodel_ofs_forward.GetFloat();
	float right_offset = CVars::bxt_viewmodel_ofs_right.GetFloat();
	float up_offset = CVars::bxt_viewmodel_ofs_up.GetFloat();

	if (pEngfuncs && HwDLL::GetInstance().pEngStudio) {
		auto view = GetViewModel();

		if (!paused) {
			if (orig_righthand_not_found && CVars::cl_righthand.GetFloat() > 0)
				right_offset *= -1;

			for (int i = 0; i < 3; i++) {
				view->origin[i] += forward_offset * pparams->forward[i] +
					right_offset * pparams->right[i] +
					up_offset * pparams->up[i];
			}

			if (CVars::bxt_viewmodel_bob_angled.GetBool())
				view->curstate.angles = view->angles;
		}
	}

	if (hwDLL.GetIsOverridingCamera()) {
		// We want to keep looking as is in freecam.
		if (!hwDLL.free_cam_active)
			hwDLL.GetCameraOverrideAngles(pparams->viewangles);

		hwDLL.GetCameraOverrideOrigin(pparams->vieworg);
	} else if (hwDLL.GetIsOffsettingCamera()) {
		static float old_camera_offset_angles[3] = { 0.0f };
		static float old_camera_offset_origin[3] = { 0.0f };

		float vector[3];
		hwDLL.GetCameraOffsetAngles(vector);

		if (pparams->paused) {
			for (int i = 0; i < 3; ++i)
				pparams->viewangles[i] += vector[i] - old_camera_offset_angles[i];
		} else {
			for (int i = 0; i < 3; ++i)
				pparams->viewangles[i] += vector[i];
		}

		for (int i = 0; i < 3; ++i)
			old_camera_offset_angles[i] = vector[i];

		hwDLL.GetCameraOffsetOrigin(vector);

		if (pparams->paused) {
			for (int i = 0; i < 3; ++i)
				pparams->vieworg[i] += vector[i] - old_camera_offset_origin[i];
		} else {
			for (int i = 0; i < 3; ++i)
				pparams->vieworg[i] += vector[i];
		}

		for (int i = 0; i < 3; ++i)
			old_camera_offset_origin[i] = vector[i];
	}

	if (unlock_camera)
		pparams->paused = paused;

	last_vieworg = pparams->vieworg;
	last_viewangles = pparams->viewangles;
	last_viewforward = pparams->forward;
	last_viewright = pparams->right;
	last_viewup = pparams->up;
}

HOOK_DEF_0(ClientDLL, void, __cdecl, HUD_Init)
{
	ORIG_HUD_Init();

	auto& hw = HwDLL::GetInstance();

	if (!hw.ORIG_Cvar_FindVar("cl_righthand"))
	{
		orig_righthand_not_found = true;
		hw.RegisterCVar(CVars::cl_righthand);
	}

	CustomHud::Init();
}

HOOK_DEF_0(ClientDLL, void, __cdecl, HUD_VidInit)
{
	ORIG_HUD_VidInit();

	CustomHud::InitIfNecessary();
	CustomHud::VidInit();

	TriangleDrawing::VidInit();
}

HOOK_DEF_0(ClientDLL, void, __cdecl, HUD_Reset)
{
	ORIG_HUD_Reset();

	CustomHud::InitIfNecessary();
	CustomHud::VidInit();
}

HOOK_DEF_2(ClientDLL, void, __cdecl, HUD_Redraw, float, time, int, intermission)
{
	custom_hud_color_set = false;

	if (sscanf(CVars::bxt_hud_game_color.GetString().c_str(), "%hhu %hhu %hhu", &custom_r, &custom_g, &custom_b) == 3)
		custom_hud_color_set = true;

	if (!CVars::bxt_disable_hud.GetBool())
		ORIG_HUD_Redraw(time, intermission);

	CustomHud::Draw(time);
}

HOOK_DEF_6(ClientDLL, void, __cdecl, HUD_PostRunCmd, local_state_s*, from, local_state_s*, to, usercmd_s*, cmd, int, runfuncs, double, time, unsigned int, random_seed)
{
	HwDLL::GetInstance().SetLastRandomSeed(random_seed);
	auto seed = random_seed;
	bool changedSeed = false;
	if (HwDLL::GetInstance().IsCountingSharedRNGSeed()) {
		auto lastSeed = HwDLL::GetInstance().GetSharedRNGSeedCounter();
		seed = lastSeed - HwDLL::GetInstance().QueuedSharedRNGSeeds + 1 + SeedsQueued;
		SeedsQueued++;
		changedSeed = true;
	}

	last_buttons = cmd->buttons;

	// Not sure if this is the best spot.
	HwDLL::GetInstance().FreeCamTick();

	if (CVars::_bxt_taslog.GetBool())
		if (pEngfuncs)
		{
			#define PRINTF(format, ...) pEngfuncs->Con_Printf(const_cast<char*>(format), ##__VA_ARGS__)
			PRINTF("-- HUD_PostRunCmd Start --\n");
			PRINTF("Msec %hhu (%Lf)\n", cmd->msec, static_cast<long double>(cmd->msec) * 0.001);
			PRINTF("Viewangles: %.8f %.8f %.8f; forwardmove: %f; sidemove: %f; upmove: %f\n", cmd->viewangles[0], cmd->viewangles[1], cmd->viewangles[2], cmd->forwardmove, cmd->sidemove, cmd->upmove);
			PRINTF("Buttons: %hu\n", cmd->buttons);
			PRINTF("Random seed: %u", random_seed);
			if (changedSeed)
				PRINTF(" (overriding with %u)", seed);
			PRINTF("\n");
			PRINTF("-- HUD_PostRunCmd End --\n");
			#undef P
		}

	return ORIG_HUD_PostRunCmd(from, to, cmd, runfuncs, time, seed);
}

HOOK_DEF_3(ClientDLL, void, __cdecl, CL_CreateMove, float, frametime, usercmd_s*, cmd, int, active)
{
	ORIG_CL_CreateMove(frametime, cmd, active);
}

HOOK_DEF_2(ClientDLL, void, __cdecl, HUD_PlayerMove, struct playermove_s*, ppmove, int, server)
{
	ORIG_HUD_PlayerMove(ppmove, server);
}

HOOK_DEF_1(ClientDLL, void, __cdecl, HUD_Frame, double, time)
{
	ORIG_HUD_Frame(time);

	SetStuffInHudFrame();

	if (CVars::_bxt_taslog.GetBool() && pEngfuncs)
		pEngfuncs->Con_Printf(const_cast<char*>("HUD_Frame time: %f\n"), time);

	SeedsQueued = 0;
}

HOOK_DEF_0(ClientDLL, void, __cdecl, HUD_DrawTransparentTriangles)
{
	ORIG_HUD_DrawTransparentTriangles();

	if (pEngfuncs) {
		glDisable(GL_TEXTURE_2D);

		TriangleDrawing::Draw();

		glEnable(GL_TEXTURE_2D);

		// This is required for the WON DLLs.
		pEngfuncs->pTriAPI->RenderMode(kRenderNormal);
	}
}

HOOK_DEF_3(ClientDLL, int, __cdecl, HUD_Key_Event, int, down, int, keynum, const char*, pszCurrentBinding)
{
	insideKeyEvent = true;

	auto rv = ORIG_HUD_Key_Event(down, keynum, pszCurrentBinding);

	insideKeyEvent = false;

	return rv;
}

HOOK_DEF_2(ClientDLL, int, __cdecl, HUD_UpdateClientData, client_data_t*, pcldata, float, flTime)
{
	const auto norefresh = CVars::_bxt_norefresh.GetBool();
	int (*ORIG_GetScreenInfo)(SCREENINFO *pscrinfo) = nullptr;

	if (norefresh && pEngfuncs) {
		ORIG_GetScreenInfo = pEngfuncs->pfnGetScreenInfo;
		pEngfuncs->pfnGetScreenInfo = [](SCREENINFO *pscrinfo) { return 0; };
	}

	if (!HwDLL::GetInstance().IsPlayingbackDemo())
		discord_integration::on_update_client_data();

	const auto rv = ORIG_HUD_UpdateClientData(pcldata, flTime);

	auto &hw = HwDLL::GetInstance();
	hw.currentRenderFOV = pcldata->fov;

	if (norefresh && pEngfuncs) {
		pEngfuncs->pfnGetScreenInfo = ORIG_GetScreenInfo;
	}

	return rv;
}

HOOK_DEF_3(ClientDLL, void, __cdecl, VectorTransform, float*, in1, float*, in2, float*, out)
{
	// No need for a NeedViewmodelAdjustments() here since insideStudioCalcAttachmentsViewmodel is
	// always FALSE from StudioCalcAttachments if we do NOT need the adjustments
	if (insideStudioCalcAttachmentsViewmodel == false)
		ORIG_VectorTransform(in1, in2, out);
	else
	{
		ORIG_VectorTransform(in1, in2, out);
		Vector vOrigin(out);
		StudioAdjustViewmodelAttachments(vOrigin);
		out[0] = vOrigin[0];
		out[1] = vOrigin[1];
		out[2] = vOrigin[2];
	}
}

HOOK_DEF_1(ClientDLL, void, __fastcall, CStudioModelRenderer__StudioCalcAttachments, void*, thisptr)
{
	auto& hw = HwDLL::GetInstance();

	if (pEngfuncs && hw.pEngStudio)
	{
		auto currententity = GetCurrentEntity();
		if (currententity == GetViewModel() && hw.NeedViewmodelAdjustments())
			insideStudioCalcAttachmentsViewmodel = true;
	}
	ORIG_CStudioModelRenderer__StudioCalcAttachments(thisptr);
	insideStudioCalcAttachmentsViewmodel = false;
}

HOOK_DEF_1(ClientDLL, void, __cdecl, CStudioModelRenderer__StudioCalcAttachments_Linux, void*, thisptr)
{
	auto& hw = HwDLL::GetInstance();

	if (pEngfuncs && hw.pEngStudio)
	{
		auto currententity = GetCurrentEntity();
		if (currententity == GetViewModel() && hw.NeedViewmodelAdjustments())
			insideStudioCalcAttachmentsViewmodel = true;
	}
	ORIG_CStudioModelRenderer__StudioCalcAttachments_Linux(thisptr);
	insideStudioCalcAttachmentsViewmodel = false;
}

HOOK_DEF_11(ClientDLL, void, __cdecl, EV_GetDefaultShellInfo, event_args_t*, args, float*, origin, float*, velocity, float*, ShellVelocity, float*, ShellOrigin,
            float*, forward, float*, right, float*, up, float, forwardScale, float, upScale, float, rightScale)
{
	if (ORIG_CL_IsThirdPerson) {
		if (!ORIG_CL_IsThirdPerson()) {
			rightScale += CVars::bxt_viewmodel_ofs_right.GetFloat();
			forwardScale += CVars::bxt_viewmodel_ofs_forward.GetFloat();
			upScale += CVars::bxt_viewmodel_ofs_up.GetFloat();

			if (orig_righthand_not_found && CVars::cl_righthand.GetFloat() > 0)
				rightScale *= -1;
		}
	}

	ORIG_EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, forwardScale, upScale, rightScale);

	if (pEngfuncs)
	{
		// Are we overriding viewmodel fov && is the entity that the shell info is for the local player?
		if (HwDLL::GetInstance().NeedViewmodelAdjustments() && pEngfuncs->pEventAPI->EV_IsLocal(args->entindex - 1))
		{
			Vector ShellOriginVec(ShellOrigin);
			StudioAdjustViewmodelAttachments(ShellOriginVec);
			ShellOrigin[0] = ShellOriginVec.x;
			ShellOrigin[1] = ShellOriginVec.y;
			ShellOrigin[2] = ShellOriginVec.z;
		}
	}
}

HOOK_DEF_1(ClientDLL, void, __fastcall, CStudioModelRenderer__StudioSetupBones, void*, thisptr)
{
	if ((offpCurrentEntity == 0) || (offpStudioHeader == 0))
	{
		ORIG_CStudioModelRenderer__StudioSetupBones(thisptr);
		return;
	}

	auto pCurrentEntity = *reinterpret_cast<cl_entity_t**>(reinterpret_cast<uintptr_t>(thisptr) + offpCurrentEntity);
	auto pStudioHeader = *reinterpret_cast<studiohdr_t**>(reinterpret_cast<uintptr_t>(thisptr) + offpStudioHeader);
	auto pseqdesc = reinterpret_cast<mstudioseqdesc_t*>(reinterpret_cast<byte*>(pStudioHeader) +
		pStudioHeader->seqindex) + pCurrentEntity->curstate.sequence;

	if (pEngfuncs && HwDLL::GetInstance().pEngStudio) {
		if (pCurrentEntity == GetViewModel()) {
			if (orig_righthand_not_found && CVars::cl_righthand.GetFloat() > 0)
			{
				float(*rotationmatrix)[3][4] = reinterpret_cast<float(*)[3][4]>(HwDLL::GetInstance().pEngStudio->StudioGetRotationMatrix());

				(*rotationmatrix)[0][1] *= -1;
				(*rotationmatrix)[1][1] *= -1;
				(*rotationmatrix)[2][1] *= -1;
			}

			if (CVars::bxt_viewmodel_disable_idle.GetBool()) {
				if (strstr(pseqdesc->label, "idle") != NULL || strstr(pseqdesc->label, "fidget") != NULL) {
					pCurrentEntity->curstate.framerate = 0; // don't animate at all
				}
			}

			if (CVars::bxt_viewmodel_disable_equip.GetBool()) {
				if (strstr(pseqdesc->label, "holster") != NULL || strstr(pseqdesc->label, "draw") != NULL ||
					strstr(pseqdesc->label, "deploy") != NULL || strstr(pseqdesc->label, "up") != NULL ||
					strstr(pseqdesc->label, "down") != NULL) {
					pCurrentEntity->curstate.sequence = 0; // instead set to idle sequence
					pseqdesc = reinterpret_cast<mstudioseqdesc_t*>(reinterpret_cast<byte*>(pStudioHeader) +
						pStudioHeader->seqindex) + pCurrentEntity->curstate.sequence;
					pseqdesc->numframes = 1;
					pseqdesc->fps = 1;
				}
			}
		}
	}

	ORIG_CStudioModelRenderer__StudioSetupBones(thisptr);
}

HOOK_DEF_1(ClientDLL, void, __cdecl, CStudioModelRenderer__StudioSetupBones_Linux, void*, thisptr)
{
	ptrdiff_t offpCurrentEntity_Linux;
	ptrdiff_t offpStudioHeader_Linux;
	if (DoesGameDirMatch("dod")) {
		offpCurrentEntity_Linux = 52;
		offpStudioHeader_Linux = 72;
	} else {
		offpCurrentEntity_Linux = 44;
		offpStudioHeader_Linux = 64;
	}

	auto pCurrentEntity = *reinterpret_cast<cl_entity_t**>(reinterpret_cast<uintptr_t>(thisptr) + offpCurrentEntity_Linux);
	auto pStudioHeader = *reinterpret_cast<studiohdr_t**>(reinterpret_cast<uintptr_t>(thisptr) + offpStudioHeader_Linux);
	auto pseqdesc = reinterpret_cast<mstudioseqdesc_t*>(reinterpret_cast<byte*>(pStudioHeader) +
		pStudioHeader->seqindex) + pCurrentEntity->curstate.sequence;

	if (pEngfuncs && HwDLL::GetInstance().pEngStudio) {
		if (pCurrentEntity == GetViewModel()) {
			if (orig_righthand_not_found && CVars::cl_righthand.GetFloat() > 0)
			{
				float(*rotationmatrix)[3][4] = reinterpret_cast<float(*)[3][4]>(HwDLL::GetInstance().pEngStudio->StudioGetRotationMatrix());

				(*rotationmatrix)[0][1] *= -1;
				(*rotationmatrix)[1][1] *= -1;
				(*rotationmatrix)[2][1] *= -1;
			}

			if (CVars::bxt_viewmodel_disable_idle.GetBool()) {
				if (strstr(pseqdesc->label, "idle") != NULL || strstr(pseqdesc->label, "fidget") != NULL) {
					pCurrentEntity->curstate.framerate = 0; // don't animate at all
				}
			}

			if (CVars::bxt_viewmodel_disable_equip.GetBool()) {
				if (strstr(pseqdesc->label, "holster") != NULL || strstr(pseqdesc->label, "draw") != NULL ||
					strstr(pseqdesc->label, "deploy") != NULL || strstr(pseqdesc->label, "up") != NULL ||
					strstr(pseqdesc->label, "down") != NULL) {
					pCurrentEntity->curstate.sequence = 0; // instead set to idle sequence
					pseqdesc = reinterpret_cast<mstudioseqdesc_t*>(reinterpret_cast<byte*>(pStudioHeader) +
						pStudioHeader->seqindex) + pCurrentEntity->curstate.sequence;
					pseqdesc->numframes = 1;
					pseqdesc->fps = 1;
				}
			}
		}
	}

	ORIG_CStudioModelRenderer__StudioSetupBones_Linux(thisptr);
}

HOOK_DEF_3(ClientDLL, int, __cdecl, HUD_AddEntity, int, type, cl_entity_s*, ent, char*, modelname)
{
	if (CVars::bxt_show_hidden_entities_clientside.GetBool()) {
		if (ent->curstate.rendermode != kRenderNormal)
			ent->curstate.renderamt = 255;
	}

	if (ent->curstate.rendermode == kRenderTransColor && ent->curstate.renderfx == kRenderFxTrigger && CVars::bxt_show_triggers_legacy.GetBool())
		ent->curstate.renderamt = std::clamp(CVars::bxt_show_triggers_legacy_alpha.GetInt(), 0, 255);

	if ((CVars::bxt_show_only_players.GetBool() && CVars::sv_cheats.GetBool() && !ent->player) || (CVars::bxt_disable_world.GetBool() && !CVars::sv_cheats.GetBool() && ent->player))
		return 0;

	if (CVars::bxt_disable_brush_entities.GetBool() && ((!CVars::sv_cheats.GetBool() && ent->player) || (ent->model->type == mod_brush && ent->curstate.rendermode != kRenderTransColor && ent->curstate.renderfx != kRenderFxTrigger)))
		return 0;

	if (CVars::bxt_disable_beams.GetBool() && (ent->model->type == mod_sprite) && (ent->curstate.entityType & ENTITY_BEAM))
		return 0;

	if ((CVars::bxt_disable_sprite_entities.GetBool() && ent->model->type == mod_sprite) || (CVars::bxt_disable_studio_entities.GetBool() && ent->model->type == mod_studio))
		return 0;

	if (pEngfuncs)
	{
		if (CVars::bxt_disable_player_corpses.GetBool() && ent->curstate.renderfx == kRenderFxDeadPlayer && HwDLL::GetInstance().IsPlayingbackDemo())
			return 0;

		if (ppmove)
		{
			auto pmove = reinterpret_cast<uintptr_t>(*ppmove);
			int* iuser2 = reinterpret_cast<int*>(pmove + (offIUser1 + 4));

			if (CVars::bxt_hide_other_players.GetBool() && ent->player && HwDLL::GetInstance().IsPlayingbackDemo() && ent->index != *iuser2)
				return 0;
		}
	}

	return ORIG_HUD_AddEntity(type, ent, modelname);
}

HOOK_DEF_0(ClientDLL, int, __cdecl, CL_IsThirdPerson)
{
	if (!ppmove || !pEngfuncs || !orig_forcehltv_found)
		return ORIG_CL_IsThirdPerson();

	auto pmove = reinterpret_cast<uintptr_t>(*ppmove);
	int *iuser1 = reinterpret_cast<int*>(pmove + offIUser1);

	if (HwDLL::GetInstance().IsPlayingbackDemo() && pEngfuncs->IsSpectateOnly() && (*iuser1 != 4))
		return 1;

	return ORIG_CL_IsThirdPerson();
}

HOOK_DEF_1(ClientDLL, void, __fastcall, CStudioModelRenderer__StudioRenderModel, void*, thisptr)
{
	if (offpCurrentEntity == 0)
	{
		ORIG_CStudioModelRenderer__StudioRenderModel(thisptr);
		return;
	}

	auto pCurrentEntity = *reinterpret_cast<cl_entity_t**>(reinterpret_cast<uintptr_t>(thisptr) + offpCurrentEntity);

	int old_rendermode = pCurrentEntity->curstate.rendermode;

	if (pEngfuncs && HwDLL::GetInstance().pEngStudio) {
		if (pCurrentEntity == GetViewModel()) {
			if (CVars::bxt_viewmodel_semitransparent.GetBool()) {
				pEngfuncs->pTriAPI->RenderMode(kRenderTransAdd);
				pEngfuncs->pTriAPI->Brightness(2);
			} else {
				pEngfuncs->pTriAPI->RenderMode(old_rendermode);
			}
		}
	}

	ORIG_CStudioModelRenderer__StudioRenderModel(thisptr);
}

HOOK_DEF_1(ClientDLL, void, __cdecl, CStudioModelRenderer__StudioRenderModel_Linux, void*, thisptr)
{
	ptrdiff_t offpCurrentEntity_Linux;
	if (DoesGameDirMatch("dod"))
		offpCurrentEntity_Linux = 52;
	else
		offpCurrentEntity_Linux = 44;

	auto pCurrentEntity = *reinterpret_cast<cl_entity_t**>(reinterpret_cast<uintptr_t>(thisptr) + offpCurrentEntity_Linux);

	int old_rendermode = pCurrentEntity->curstate.rendermode;

	if (pEngfuncs && HwDLL::GetInstance().pEngStudio) {
		if (pCurrentEntity == GetViewModel()) {
			if (CVars::bxt_viewmodel_semitransparent.GetBool()) {
				pEngfuncs->pTriAPI->RenderMode(kRenderTransAdd);
				pEngfuncs->pTriAPI->Brightness(2);
			} else {
				pEngfuncs->pTriAPI->RenderMode(old_rendermode);
			}
		}
	}

	ORIG_CStudioModelRenderer__StudioRenderModel_Linux(thisptr);
}

HOOK_DEF_4(ClientDLL, void, __cdecl, ScaleColors, int*, r, int*, g, int*, b, int, a)
{
	if (custom_hud_color_set && !insideDrawHealthPain && (insideDrawAmmoHistory || insideDrawHealthDamage)) {
		*r = custom_r;
		*g = custom_g;
		*b = custom_b;
	}

	if (CVars::bxt_hud_game_alpha.GetInt() >= 1 && CVars::bxt_hud_game_alpha.GetInt() <= 255 && !insideDrawAmmoHistory && !insideDrawHealthDamage && !insideDrawHealthPain)
		a = CVars::bxt_hud_game_alpha.GetInt();

	if (insideDrawHealthDamage && CVars::bxt_hud_game_alpha_damage.GetBool() && CVars::bxt_hud_game_alpha.GetInt() >= 1 && CVars::bxt_hud_game_alpha.GetInt() <= 255)
	{
		a = static_cast<int>(std::fabs(std::sin(drawdamage_flTime*2.0f)) * CVars::bxt_hud_game_alpha.GetFloat());
	}

	ORIG_ScaleColors(r, g, b, a);
}

HOOK_DEF_3(ClientDLL, int, __fastcall, HistoryResource__DrawAmmoHistory, void*, thisptr, int, edx, float, flTime)
{
	insideDrawAmmoHistory = true;
	auto ret = ORIG_HistoryResource__DrawAmmoHistory(thisptr, edx, flTime);
	insideDrawAmmoHistory = false;

	return ret;
}

HOOK_DEF_2(ClientDLL, int, __cdecl, HistoryResource__DrawAmmoHistory_Linux, void*, thisptr, float, flTime)
{
	insideDrawAmmoHistory = true;
	auto ret = ORIG_HistoryResource__DrawAmmoHistory_Linux(thisptr, flTime);
	insideDrawAmmoHistory = false;

	return ret;
}

HOOK_DEF_3(ClientDLL, int, __fastcall, CHudHealth__DrawDamage, void*, thisptr, int, edx, float, flTime)
{
	drawdamage_flTime = flTime;
	insideDrawHealthDamage = true;
	auto ret = ORIG_CHudHealth__DrawDamage(thisptr, edx, flTime);
	insideDrawHealthDamage = false;

	return ret;
}

HOOK_DEF_2(ClientDLL, int, __cdecl, CHudHealth__DrawDamage_Linux, void*, thisptr, float, flTime)
{
	drawdamage_flTime = flTime;
	insideDrawHealthDamage = true;
	auto ret = ORIG_CHudHealth__DrawDamage_Linux(thisptr, flTime);
	insideDrawHealthDamage = false;

	return ret;
}

HOOK_DEF_3(ClientDLL, int, __fastcall, CHudHealth__DrawPain, void*, thisptr, int, edx, float, flTime)
{
	insideDrawHealthPain = true;
	auto ret = ORIG_CHudHealth__DrawPain(thisptr, edx, flTime);
	insideDrawHealthPain = false;

	return ret;
}

HOOK_DEF_2(ClientDLL, int, __cdecl, CHudHealth__DrawPain_Linux, void*, thisptr, float, flTime)
{
	insideDrawHealthPain = true;
	auto ret = ORIG_CHudHealth__DrawPain_Linux(thisptr, flTime);
	insideDrawHealthPain = false;

	return ret;
}

HOOK_DEF_1(ClientDLL, void, __fastcall, CHudFlashlight__drawNightVision, void*, thisptr)
{
	if (CVars::bxt_disable_nightvision_sprite.GetBool())
		return;

	insideDrawNightVision = true;
	ORIG_CHudFlashlight__drawNightVision(thisptr);
	insideDrawNightVision = false;
}

HOOK_DEF_1(ClientDLL, void, __cdecl, CHudFlashlight__drawNightVision_Linux, void*, thisptr)
{
	if (CVars::bxt_disable_nightvision_sprite.GetBool())
		return;

	insideDrawNightVision = true;
	ORIG_CHudFlashlight__drawNightVision_Linux(thisptr);
	insideDrawNightVision = false;
}

HOOK_DEF_3(ClientDLL, bool, __fastcall, CHud__DrawHudNightVision, void*, thisptr, int, edx, float, flTime)
{
	if (CVars::bxt_disable_nightvision_sprite.GetBool())
		return false;

	insideDrawNightVision = true;
	auto ret = ORIG_CHud__DrawHudNightVision(thisptr, edx, flTime);
	insideDrawNightVision = false;

	return ret;
}

HOOK_DEF_2(ClientDLL, bool, __cdecl, CHud__DrawHudNightVision_Linux, void*, thisptr, float, flTime)
{
	if (CVars::bxt_disable_nightvision_sprite.GetBool())
		return false;

	insideDrawNightVision = true;
	auto ret = ORIG_CHud__DrawHudNightVision_Linux(thisptr, flTime);
	insideDrawNightVision = false;

	return ret;
}

HOOK_DEF_3(ClientDLL, bool, __fastcall, CHud__DrawHudFiberCamera, void*, thisptr, int, edx, float, flTime)
{
	insideDrawFiberCameraCZDS = true;
	auto ret = ORIG_CHud__DrawHudFiberCamera(thisptr, edx, flTime);
	insideDrawFiberCameraCZDS = false;

	return ret;
}

HOOK_DEF_2(ClientDLL, bool, __cdecl, CHud__DrawHudFiberCamera_Linux, void*, thisptr, float, flTime)
{
	insideDrawFiberCameraCZDS = true;
	auto ret = ORIG_CHud__DrawHudFiberCamera_Linux(thisptr, flTime);
	insideDrawFiberCameraCZDS = false;

	return ret;
}

HOOK_DEF_3(ClientDLL, int, __fastcall, CHudIcons__Draw, void*, thisptr, int, edx, float, flTime)
{
	insideDrawHudIconsCZDS = true;
	auto ret = ORIG_CHudIcons__Draw(thisptr, edx, flTime);
	insideDrawHudIconsCZDS = false;

	return ret;
}

HOOK_DEF_2(ClientDLL, int, __cdecl, CHudIcons__Draw_Linux, void*, thisptr, float, flTime)
{
	insideDrawHudIconsCZDS = true;
	auto ret = ORIG_CHudIcons__Draw_Linux(thisptr, flTime);
	insideDrawHudIconsCZDS = false;

	return ret;
}

HOOK_DEF_2(ClientDLL, void, __cdecl, V_PunchAxis, int, axis, float, punch)
{
	if (CVars::bxt_remove_punchangles.GetBool() && CVars::sv_cheats.GetBool())
		return;

	ORIG_V_PunchAxis(axis, punch);
}

HOOK_DEF_0(ClientDLL, void, __cdecl, HUD_Shutdown)
{
	ORIG_HUD_Shutdown();
	Unhook();
}

HOOK_DEF_0(ClientDLL, void, __cdecl, PM_Duck)
{
	ORIG_PM_Duck();

	if (ppmove && CVars::bxt_cof_slowdown_if_use_on_ground_prediction.GetBool()) {
		auto pmove = reinterpret_cast<uintptr_t>(*ppmove);
		int *onground = reinterpret_cast<int*>(pmove + offOnground);
		usercmd_t *cmd = reinterpret_cast<usercmd_t*>(pmove + offCmd);
		float *velocity = reinterpret_cast<float*>(pmove + offVelocity);

		if ((*onground != -1) && (cmd->buttons & IN_USE)) {
			velocity[0] *= 0.3f;
			velocity[1] *= 0.3f;
			velocity[2] *= 0.3f;
		}
	}
}

HOOK_DEF_0(ClientDLL, void, __cdecl, PM_UnDuck)
{
	if (ppmove && offFlags && offInDuck && CVars::bxt_cof_enable_ducktap_prediction.GetBool()) {
		auto pmove = reinterpret_cast<uintptr_t>(*ppmove);
		int *flags = reinterpret_cast<int*>(pmove + offFlags);
		qboolean *inDuck = reinterpret_cast<qboolean*>(pmove + offInDuck);
		*flags |= FL_DUCKING;
		*inDuck = false;
	}

	ORIG_PM_UnDuck();
}
