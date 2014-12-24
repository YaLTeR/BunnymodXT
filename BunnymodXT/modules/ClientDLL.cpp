#include "../stdafx.hpp"

#include "../sptlib-wrapper.hpp"
#include <SPTLib/MemUtils.hpp>
#include <SPTLib/Hooks.hpp>
#include "ClientDLL.hpp"
#include "../patterns.hpp"
#include "../cvars.hpp"
#include "../hud_custom.hpp"

// Linux hooks.
#ifndef _WIN32
extern "C" int __cdecl Initialize(cl_enginefunc_t* pEnginefuncs, int iVersion)
{
	return ClientDLL::HOOKED_Initialize(pEnginefuncs, iVersion);
}

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

extern "C" void __cdecl V_CalcRefdef(ref_params_t* pparams)
{
	return ClientDLL::HOOKED_V_CalcRefdef(pparams);
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

	MemUtils::ptnvec_size ptnNumber;

	void *pPMJump, *pPMPreventMegaBunnyJumping;
	std::future<MemUtils::ptnvec_size> fPMPreventMegaBunnyJumping;

	ORIG_PM_PlayerMove = reinterpret_cast<_PM_PlayerMove>(MemUtils::GetSymbolAddress(moduleHandle, "PM_PlayerMove"));

	pPMPreventMegaBunnyJumping = MemUtils::GetSymbolAddress(moduleHandle, "PM_PreventMegaBunnyJumping");
	if (pPMPreventMegaBunnyJumping)
	{
		ORIG_PM_PreventMegaBunnyJumping = reinterpret_cast<_PM_PreventMegaBunnyJumping>(pPMPreventMegaBunnyJumping);
		EngineDevMsg("[client dll] Found PM_PreventMegaBunnyJumping at %p.\n", pPMPreventMegaBunnyJumping);
	}
	else
		fPMPreventMegaBunnyJumping = std::async(MemUtils::FindUniqueSequence, moduleBase, moduleLength, Patterns::ptnsPMPreventMegaBunnyJumping, &pPMPreventMegaBunnyJumping);

	pPMJump = MemUtils::GetSymbolAddress(moduleHandle, "PM_Jump");
	if (pPMJump)
	{
		if (*reinterpret_cast<byte*>(pPMJump) == 0xA1)
		{
			ORIG_PM_Jump = reinterpret_cast<_PM_Jump>(pPMJump);
			EngineDevMsg("[client dll] Found PM_Jump at %p.\n", pPMJump);
			ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(pPMJump) + 1); // Linux
			offOldbuttons = 200;
			offOnground = 224;

			void *bhopcapAddr;
			ptnNumber = MemUtils::FindUniqueSequence(moduleBase, moduleLength, Patterns::ptnsBhopcap, &bhopcapAddr);
			if (ptnNumber != MemUtils::INVALID_SEQUENCE_INDEX)
			{
				EngineDevMsg("Found the bhopcap pattern at %p.\n", bhopcapAddr);
				offBhopcap = reinterpret_cast<ptrdiff_t>(bhopcapAddr) - reinterpret_cast<ptrdiff_t>(pPMJump) + 27;
				memcpy(originalBhopcapInsn, reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(bhopcapAddr) + 27), sizeof(originalBhopcapInsn));
			}
		}
		else
			pPMJump = nullptr; // Try pattern searching.
	}
	
	if (!pPMJump)
	{
		ptnNumber = MemUtils::FindUniqueSequence(moduleBase, moduleLength, Patterns::ptnsPMJump, &pPMJump);
		if (ptnNumber != MemUtils::INVALID_SEQUENCE_INDEX)
		{
			ORIG_PM_Jump = reinterpret_cast<_PM_Jump>(pPMJump);
			EngineDevMsg("[client dll] Found PM_Jump at %p (using the %s pattern).\n", pPMJump, Patterns::ptnsPMJump[ptnNumber].build.c_str());
			offOldbuttons = 200;
			offOnground = 224;

			switch (ptnNumber)
			{
			case 0:
			case 1:
				ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(pPMJump) + 2);
				break;

			case 2: // AG-Server, shouldn't happen here but who knows.
			case 3:
				ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(pPMJump) + 3);
				break;
			}
		}
		else
		{
			EngineDevWarning("[client dll] Could not find PM_Jump!\n");
			EngineWarning("Autojump prediction is not available.\n");
		}
	}

	if (!ORIG_PM_PreventMegaBunnyJumping)
	{
		ptnNumber = fPMPreventMegaBunnyJumping.get();
		if (ptnNumber != MemUtils::INVALID_SEQUENCE_INDEX)
		{
			ORIG_PM_PreventMegaBunnyJumping = reinterpret_cast<_PM_PreventMegaBunnyJumping>(pPMPreventMegaBunnyJumping);
			EngineDevMsg("[client dll] Found PM_PreventMegaBunnyJumping at %p (using the %s pattern).\n", pPMPreventMegaBunnyJumping, Patterns::ptnsPMPreventMegaBunnyJumping[ptnNumber].build.c_str());
		}
		else
		{
			EngineDevWarning("[client dll] Could not find PM_PreventMegaBunnyJumping!\n");
			EngineWarning("Bhopcap disabling prediction is not available.\n");
		}
	}

	pEngfuncs = reinterpret_cast<cl_enginefunc_t*>(MemUtils::GetSymbolAddress(moduleHandle, "gEngfuncs"));
	if (pEngfuncs)
	{
		EngineDevMsg("[client dll] pEngfuncs is %p.\n", pEngfuncs);
		if (!*reinterpret_cast<uintptr_t*>(pEngfuncs))
		{
			ORIG_Initialize = reinterpret_cast<_Initialize>(MemUtils::GetSymbolAddress(moduleHandle, "?Initialize_Body@@YAHPAUcl_enginefuncs_s@@H@Z"));
			if (!ORIG_Initialize)
				ORIG_Initialize = reinterpret_cast<_Initialize>(MemUtils::GetSymbolAddress(moduleHandle, "Initialize"));
			if (!ORIG_Initialize)
			{
				EngineDevWarning("[client dll] Couldn't get the address of Initialize!\n");
				EngineWarning("Clientside CVars and commands are not available.\n");
				EngineWarning("Custom HUD is not available.\n");
			}
		}
	}
	else
	{
		// In AG, this thing is the main function, so check that first.
		_Initialize pInitialize = reinterpret_cast<_Initialize>(MemUtils::GetSymbolAddress(moduleHandle, "?Initialize_Body@@YAHPAUcl_enginefuncs_s@@H@Z"));

		if (!pInitialize)
			pInitialize = reinterpret_cast<_Initialize>(MemUtils::GetSymbolAddress(moduleHandle, "Initialize"));

		if (pInitialize)
		{
			// Find "mov edi, offset dword; rep movsd" inside Initialize. The pointer to gEngfuncs is that dword.
			const byte pattern[] = { 0xBF, '?', '?', '?', '?', 0xF3, 0xA5 };
			auto addr = MemUtils::FindPattern(reinterpret_cast<void*>(pInitialize), 40, pattern, "x????xx");
			if (!addr)
			{
				const byte pattern_[] = { 0xB9, '?', '?', '?', '?', 0x8B, 0x54, 0x24, 0x10 };
				addr = MemUtils::FindPattern(reinterpret_cast<void*>(pInitialize), 40, pattern_, "x????xxxx");
			}

			if (addr)
			{
				pEngfuncs = *reinterpret_cast<cl_enginefunc_t**>(reinterpret_cast<uintptr_t>(addr) + 1);
				EngineDevMsg("[client dll] pEngfuncs is %p.\n", pEngfuncs);

				// If we have engfuncs, register cvars and whatnot right away (in the end of this function because other stuff need to be done first). Otherwise wait till the engine gives us engfuncs.
				// This works because global variables are zero by default.
				if (!*reinterpret_cast<uintptr_t*>(pEngfuncs))
					ORIG_Initialize = pInitialize;
			}
			else
			{
				EngineDevWarning("[client dll] Couldn't find the pattern in Initialize!\n");
				EngineWarning("Clientside CVars and commands are not available.\n");
				EngineWarning("Custom HUD is not available.\n");
			}
		}
		else
		{
			EngineDevWarning("[client dll] Couldn't get the address of Initialize!\n");
			EngineWarning("Clientside CVars and commands are not available.\n");
			EngineWarning("Custom HUD is not available.\n");
		}
	}

	// We can draw stuff only if we know that we have already received / will receive engfuncs.
	if (pEngfuncs)
		if (!FindHUDFunctions()) {
			ORIG_HUD_Init = nullptr;
			ORIG_HUD_VidInit = nullptr;
			ORIG_HUD_Reset = nullptr;
			ORIG_HUD_Redraw = nullptr;
			EngineDevWarning("Custom HUD is not available.\n");
		}

	ORIG_V_CalcRefdef = reinterpret_cast<_V_CalcRefdef>(MemUtils::GetSymbolAddress(moduleHandle, "V_CalcRefdef"));
	if (!ORIG_V_CalcRefdef)
	{
		EngineDevWarning("[client dll] Could not find V_CalcRefdef!\n");
		EngineWarning("Velocity display during demo playback is not available.\n");
	}

	ORIG_HUD_PostRunCmd = reinterpret_cast<_HUD_PostRunCmd>(MemUtils::GetSymbolAddress(moduleHandle, "HUD_PostRunCmd"));
	if (!ORIG_HUD_PostRunCmd)
	{
		EngineDevMsg("[client dll] Could not find HUD_PostRunCmd!\n");
	}
	
	// Now we can register cvars and commands provided that we already have engfuncs.
	if (pEngfuncs && *reinterpret_cast<uintptr_t*>(pEngfuncs))
		RegisterCVarsAndCommands();

	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_Initialize), reinterpret_cast<void*>(HOOKED_Initialize));
	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_HUD_Init), reinterpret_cast<void*>(HOOKED_HUD_Init));
	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_HUD_VidInit), reinterpret_cast<void*>(HOOKED_HUD_VidInit));
	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_HUD_Reset), reinterpret_cast<void*>(HOOKED_HUD_Reset));
	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_HUD_Redraw), reinterpret_cast<void*>(HOOKED_HUD_Redraw));

	if (needToIntercept)
		MemUtils::Intercept(moduleName, {
			{ reinterpret_cast<void**>(&ORIG_PM_Jump), reinterpret_cast<void*>(HOOKED_PM_Jump) },
			{ reinterpret_cast<void**>(&ORIG_PM_PreventMegaBunnyJumping), reinterpret_cast<void*>(HOOKED_PM_PreventMegaBunnyJumping) },
			{ reinterpret_cast<void**>(&ORIG_Initialize), reinterpret_cast<void*>(HOOKED_Initialize) },
			{ reinterpret_cast<void**>(&ORIG_V_CalcRefdef), reinterpret_cast<void*>(HOOKED_V_CalcRefdef) },
			{ reinterpret_cast<void**>(&ORIG_HUD_Init), reinterpret_cast<void*>(HOOKED_HUD_Init) },
			{ reinterpret_cast<void**>(&ORIG_HUD_VidInit), reinterpret_cast<void*>(HOOKED_HUD_VidInit) },
			{ reinterpret_cast<void**>(&ORIG_HUD_Reset), reinterpret_cast<void*>(HOOKED_HUD_Reset) },
			{ reinterpret_cast<void**>(&ORIG_HUD_Redraw), reinterpret_cast<void*>(HOOKED_HUD_Redraw) },
			{ reinterpret_cast<void**>(&ORIG_HUD_PostRunCmd), reinterpret_cast<void*>(HOOKED_HUD_PostRunCmd) }
		});
}

void ClientDLL::Unhook()
{
	if (m_Intercepted)
		MemUtils::RemoveInterception(m_Name, {
			{ reinterpret_cast<void**>(&ORIG_PM_Jump), reinterpret_cast<void*>(HOOKED_PM_Jump) },
			{ reinterpret_cast<void**>(&ORIG_PM_PreventMegaBunnyJumping), reinterpret_cast<void*>(HOOKED_PM_PreventMegaBunnyJumping) },
			{ reinterpret_cast<void**>(&ORIG_Initialize), reinterpret_cast<void*>(HOOKED_Initialize) },
			{ reinterpret_cast<void**>(&ORIG_V_CalcRefdef), reinterpret_cast<void*>(HOOKED_V_CalcRefdef) },
			{ reinterpret_cast<void**>(&ORIG_HUD_Init), reinterpret_cast<void*>(HOOKED_HUD_Init) },
			{ reinterpret_cast<void**>(&ORIG_HUD_VidInit), reinterpret_cast<void*>(HOOKED_HUD_VidInit) },
			{ reinterpret_cast<void**>(&ORIG_HUD_Reset), reinterpret_cast<void*>(HOOKED_HUD_Reset) },
			{ reinterpret_cast<void**>(&ORIG_HUD_Redraw), reinterpret_cast<void*>(HOOKED_HUD_Redraw) },
			{ reinterpret_cast<void**>(&ORIG_HUD_PostRunCmd), reinterpret_cast<void*>(HOOKED_HUD_PostRunCmd) }
		});

	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_Initialize));
	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_HUD_Init));
	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_HUD_VidInit));
	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_HUD_Reset));
	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_HUD_Redraw));

	Clear();
}

void ClientDLL::Clear()
{
	IHookableNameFilter::Clear();
	ORIG_PM_Jump = nullptr;
	ORIG_PM_PlayerMove = nullptr;
	ORIG_PM_PreventMegaBunnyJumping = nullptr;
	ORIG_Initialize = nullptr;
	ORIG_V_CalcRefdef = nullptr;
	ORIG_HUD_Init = nullptr;
	ORIG_HUD_VidInit = nullptr;
	ORIG_HUD_Reset = nullptr;
	ORIG_HUD_Redraw = nullptr;
	ORIG_HUD_PostRunCmd = nullptr;
	ppmove = nullptr;
	offOldbuttons = 0;
	offOnground = 0;
	offBhopcap = 0;
	memset(originalBhopcapInsn, 0, sizeof(originalBhopcapInsn));
	pEngfuncs = nullptr;
	cantJumpNextTime = false;
	m_Intercepted = false;
}

bool ClientDLL::FindHUDFunctions()
{
	if ((ORIG_HUD_Init = reinterpret_cast<_HUD_Init>(MemUtils::GetSymbolAddress(m_Handle, "HUD_Init"))))
		EngineDevMsg("[client dll] Found HUD_Init at %p.\n", ORIG_HUD_Init);
	else {
		EngineDevMsg("[client dll] Could not HUD_Init!\n");
		return false;
	}

	if ((ORIG_HUD_VidInit = reinterpret_cast<_HUD_VidInit>(MemUtils::GetSymbolAddress(m_Handle, "HUD_VidInit"))))
		EngineDevMsg("[client dll] Found HUD_VidInit at %p.\n", ORIG_HUD_VidInit);
	else {
		EngineDevMsg("[client dll] Could not HUD_VidInit!\n");
		return false;
	}

	if ((ORIG_HUD_Reset = reinterpret_cast<_HUD_Reset>(MemUtils::GetSymbolAddress(m_Handle, "HUD_Reset"))))
		EngineDevMsg("[client dll] Found HUD_Reset at %p.\n", ORIG_HUD_Reset);
	else {
		EngineDevMsg("[client dll] Could not HUD_Reset!\n");
		return false;
	}

	if ((ORIG_HUD_Redraw = reinterpret_cast<_HUD_Redraw>(MemUtils::GetSymbolAddress(m_Handle, "HUD_Redraw"))))
		EngineDevMsg("[client dll] Found HUD_Redraw at %p.\n", ORIG_HUD_Redraw);
	else {
		EngineDevMsg("[client dll] Could not HUD_Redraw!\n");
		return false;
	}

	return true;
}

void ClientDLL::RegisterCVarsAndCommands()
{
	if (!pEngfuncs || !*reinterpret_cast<uintptr_t*>(pEngfuncs))
		return;

	#define REG(name, str) pEngfuncs->pfnRegisterVariable(const_cast<char*>(name), const_cast<char*>(str), 0)

	if (ORIG_PM_Jump)
		bxt_autojump_prediction.Assign(REG("bxt_autojump_prediction", "0"));

	if (ORIG_PM_PreventMegaBunnyJumping)
		bxt_bhopcap_prediction.Assign(REG("bxt_bhopcap_prediction", "1"));

	if (ORIG_HUD_Init)
	{
		con_color_.Assign(pEngfuncs->pfnGetCvarPointer("con_color"));
		bxt_hud.Assign(REG("bxt_hud", "1"));
		bxt_hud_color.Assign(REG("bxt_hud_color", ""));
		bxt_hud_precision.Assign(REG("bxt_hud_precision", "6"));
		bxt_hud_velocity.Assign(REG("bxt_hud_velocity", "0"));
		bxt_hud_velocity_offset.Assign(REG("bxt_hud_velocity_offset", ""));
		bxt_hud_velocity_anchor.Assign(REG("bxt_hud_velocity_anchor", "1 0"));
		bxt_hud_origin.Assign(REG("bxt_hud_origin", "0"));
		bxt_hud_origin_offset.Assign(REG("bxt_hud_origin_offset", ""));
		bxt_hud_origin_anchor.Assign(REG("bxt_hud_origin_anchor", "1 0"));
		bxt_hud_speedometer.Assign(REG("bxt_hud_speedometer", "1"));
		bxt_hud_speedometer_offset.Assign(REG("bxt_hud_speedometer_offset", ""));
		bxt_hud_speedometer_anchor.Assign(REG("bxt_hud_speedometer_anchor", "0.5 1"));
		bxt_hud_jumpspeed.Assign(REG("bxt_hud_jumpspeed", "0"));
		bxt_hud_jumpspeed_offset.Assign(REG("bxt_hud_jumpspeed_offset", ""));
		bxt_hud_jumpspeed_anchor.Assign(REG("bxt_hud_jumpspeed_anchor", "0.5 1"));
	}

	#undef REG

	EngineDevMsg("[client dll] Registered CVars.\n");
}

HOOK_DEF_0(ClientDLL, void, __cdecl, PM_Jump)
{
	auto pmove = reinterpret_cast<uintptr_t>(*ppmove);
	int *onground = reinterpret_cast<int*>(pmove + offOnground);
	int orig_onground = *onground;

	int *oldbuttons = reinterpret_cast<int*>(pmove + offOldbuttons);
	int orig_oldbuttons = *oldbuttons;

	if (bxt_autojump_prediction.GetBool())
	{
		if ((orig_onground != -1) && !cantJumpNextTime)
			*oldbuttons &= ~IN_JUMP;
	}

	cantJumpNextTime = false;

	if (offBhopcap)
	{
		auto pPMJump = reinterpret_cast<ptrdiff_t>(ORIG_PM_Jump);
		if (bxt_bhopcap_prediction.GetBool())
		{
			if (*reinterpret_cast<byte*>(pPMJump + offBhopcap) == 0x90
				&& *reinterpret_cast<byte*>(pPMJump + offBhopcap + 1) == 0x90)
				MemUtils::ReplaceBytes(reinterpret_cast<void*>(pPMJump + offBhopcap), 6, originalBhopcapInsn);
		}
		else if (*reinterpret_cast<byte*>(pPMJump + offBhopcap) == 0x0F
				&& *reinterpret_cast<byte*>(pPMJump + offBhopcap + 1) == 0x82)
				MemUtils::ReplaceBytes(reinterpret_cast<void*>(pPMJump + offBhopcap), 6, reinterpret_cast<const byte*>("\x90\x90\x90\x90\x90\x90"));
	}

	ORIG_PM_Jump();

	if ((orig_onground != -1) && (*onground == -1))
		cantJumpNextTime = true;

	if (bxt_autojump_prediction.GetBool())
		*oldbuttons = orig_oldbuttons;
}

HOOK_DEF_1(ClientDLL, void, __cdecl, PM_PlayerMove, qboolean, server)
{
	ORIG_PM_PlayerMove(server);
}

HOOK_DEF_0(ClientDLL, void, __cdecl, PM_PreventMegaBunnyJumping)
{
	if (bxt_bhopcap_prediction.GetBool())
		ORIG_PM_PreventMegaBunnyJumping();
}

HOOK_DEF_2(ClientDLL, int, __cdecl, Initialize, cl_enginefunc_t*, pEnginefuncs, int, iVersion)
{
	int rv = ORIG_Initialize(pEnginefuncs, iVersion);

	RegisterCVarsAndCommands();

	return rv;
}

HOOK_DEF_1(ClientDLL, void, __cdecl, V_CalcRefdef, ref_params_t*, pparams)
{
	CustomHud::UpdatePlayerInfoInaccurate(pparams->simvel, pparams->simorg);

	ORIG_V_CalcRefdef(pparams);
}

HOOK_DEF_0(ClientDLL, void, __cdecl, HUD_Init)
{
	ORIG_HUD_Init();

	CustomHud::Init();
}

HOOK_DEF_0(ClientDLL, void, __cdecl, HUD_VidInit)
{
	ORIG_HUD_VidInit();

	CustomHud::InitIfNecessary();
	CustomHud::VidInit();
}

HOOK_DEF_0(ClientDLL, void, __cdecl, HUD_Reset)
{
	ORIG_HUD_Reset();

	CustomHud::InitIfNecessary();
	CustomHud::VidInit();
}

HOOK_DEF_2(ClientDLL, void, __cdecl, HUD_Redraw, float, time, int, intermission)
{
	ORIG_HUD_Redraw(time, intermission);

	CustomHud::Draw(time);
}

HOOK_DEF_6(ClientDLL, void, __cdecl, HUD_PostRunCmd, local_state_s*, from, local_state_s*, to, usercmd_s*, cmd, int, runfuncs, double, time, unsigned int, random_seed)
{
	//if (_bxt_taslog.GetBool())
	if (pEngfuncs)
	{
		pEngfuncs->Con_Printf("-- HUD_PostRunCmd Start --\n");
		pEngfuncs->Con_Printf("Msec %hhu (%Lf)\n", cmd->msec, static_cast<long double>(cmd->msec) * 0.001);
		pEngfuncs->Con_Printf("Viewangles: %.8f %.8f %.8f; forwardmove: %f; sidemove: %f; upmove: %f\n", cmd->viewangles[0], cmd->viewangles[1], cmd->viewangles[2], cmd->forwardmove, cmd->sidemove, cmd->upmove);
		pEngfuncs->Con_Printf("Buttons: %hu\n", cmd->buttons);
		pEngfuncs->Con_Printf("Random seed: %d\n", random_seed);
		pEngfuncs->Con_Printf("-- HUD_PostRunCmd End --\n");
	}

	return ORIG_HUD_PostRunCmd(from, to, cmd, runfuncs, time, random_seed);
}
