#include "stdafx.hpp"

#include <fstream>

#include "..\sptlib-wrapper.hpp"
#include <SPTLib\memutils.hpp>
#include <SPTLib\detoursutils.hpp>
#include <SPTLib\hooks.hpp>
#include "..\modules.hpp"
#include "..\patterns.hpp"
#include "..\cvars.hpp"

using std::uintptr_t;
using std::size_t;

void __cdecl ServerDLL::HOOKED_PM_Jump()
{
	return serverDLL.HOOKED_PM_Jump_Func();
}

void __cdecl ServerDLL::HOOKED_PM_PreventMegaBunnyJumping()
{
	return serverDLL.HOOKED_PM_PreventMegaBunnyJumping_Func();
}

void __cdecl ServerDLL::HOOKED_PM_PlayerMove(qboolean server)
{
	return serverDLL.HOOKED_PM_PlayerMove_Func(server);
}

void __stdcall ServerDLL::HOOKED_GiveFnptrsToDll(enginefuncs_t* pEngfuncsFromEngine, const void* pGlobals)
{
	return serverDLL.HOOKED_GiveFnptrsToDll_Func(pEngfuncsFromEngine, pGlobals);
}

void ServerDLL::Hook(const std::wstring& moduleName, HMODULE hModule, uintptr_t moduleStart, size_t moduleLength)
{
	Clear(); // Just in case.

	m_hModule = hModule;
	m_Start = moduleStart;
	m_Length = moduleLength;
	m_Name = moduleName;

	MemUtils::ptnvec_size ptnNumber;

	uintptr_t pPMJump = NULL,
		pPMPreventMegaBunnyJumping = NULL,
		pPMPlayerMove = NULL;

	auto fPMPreventMegaBunnyJumping = std::async(std::launch::async, MemUtils::FindUniqueSequence, moduleStart, moduleLength, Patterns::ptnsPMPreventMegaBunnyJumping, &pPMPreventMegaBunnyJumping);
	auto fPMPlayerMove = std::async(std::launch::async, MemUtils::FindUniqueSequence, moduleStart, moduleLength, Patterns::ptnsPMPlayerMove, &pPMPlayerMove);

	ptnNumber = MemUtils::FindUniqueSequence(moduleStart, moduleLength, Patterns::ptnsPMJump, &pPMJump);
	if (ptnNumber != MemUtils::INVALID_SEQUENCE_INDEX)
	{
		ORIG_PM_Jump = (_PM_Jump)pPMJump;
		EngineDevMsg("[server dll] Found PM_Jump at %p (using the %s pattern).\n", pPMJump, Patterns::ptnsPMJump[ptnNumber].build.c_str());

		switch (ptnNumber)
		{
		case 0:
			ppmove = *(uintptr_t *)(pPMJump + 2);
			offPlayerIndex = 0;
			offOldbuttons = 200;
			offOnground = 224;
			break;

		case 1:
			ppmove = *(uintptr_t *)(pPMJump + 2);
			offPlayerIndex = 0;
			offOldbuttons = 200;
			offOnground = 224;
			break;

		case 2:
			ppmove = *(uintptr_t *)(pPMJump + 3);
			offPlayerIndex = 0;
			offOldbuttons = 200;
			offOnground = 224;
			break;

		case 3: // AG-Client, shouldn't happen here but who knows.
			ppmove = *(uintptr_t *)(pPMJump + 3);
			offPlayerIndex = 0;
			offOldbuttons = 200;
			offOnground = 224;
			break;
		}
	}
	else
	{
		EngineDevWarning("[server dll] Could not find PM_Jump!\n");
		EngineWarning("Autojump is not available.\n");
	}

	ptnNumber = fPMPreventMegaBunnyJumping.get();
	if (ptnNumber != MemUtils::INVALID_SEQUENCE_INDEX)
	{
		ORIG_PM_PreventMegaBunnyJumping = (_PM_PreventMegaBunnyJumping)pPMPreventMegaBunnyJumping;
		EngineDevMsg("[server dll] Found PM_PreventMegaBunnyJumping at %p (using the %s pattern).\n", pPMPreventMegaBunnyJumping, Patterns::ptnsPMPreventMegaBunnyJumping[ptnNumber].build.c_str());
	}
	else
	{
		EngineDevWarning("[server dll] Could not find PM_PreventMegaBunnyJumping!\n");
		EngineWarning("Bhopcap disabling is not available.\n");
	}

	ptnNumber = fPMPlayerMove.get();
	if (ptnNumber != MemUtils::INVALID_SEQUENCE_INDEX)
	{
		ORIG_PM_PlayerMove = (_PM_PlayerMove)pPMPlayerMove;
		EngineDevMsg("[server dll] Found PM_PlayerMove at %p (using the %s pattern).\n", pPMPlayerMove, Patterns::ptnsPMPlayerMove[ptnNumber].build.c_str());

		switch (ptnNumber)
		{
		case 0:
			offPlayerIndex = 0;
			offVelocity = 92;
			offOrigin = 56;
			offAngles = 68;
			break;
		}
	}
	else
	{
		EngineDevWarning("[server dll] Could not find PM_PlayerMove!\n");
		//EngineWarning("Bhopcap disabling is not available.\n");
	}

	// This has to be the last thing to check and hook.
	_GiveFnptrsToDll pGiveFnptrsToDll = (_GiveFnptrsToDll)GetProcAddress(hModule, "GiveFnptrsToDll");
	if (pGiveFnptrsToDll != NULL)
	{
		// Find "mov edi, offset dword; rep movsd" inside GiveFnptrsToDll. The pointer to g_engfuncs is that dword.
		const byte pattern[] = { 0xBF, '?', '?', '?', '?', 0xF3, 0xA5 };
		uintptr_t addr = MemUtils::FindPattern((uintptr_t)pGiveFnptrsToDll, 40, pattern, "x????xx");
		if (addr != NULL)
		{
			pEngfuncs = *(enginefuncs_t **)(addr + 1);
			EngineDevMsg("[server dll] pEngfuncs is %p.\n", pEngfuncs);

			// If we have engfuncs, do stuff right away. Otherwise wait till the engine gives us engfuncs.
			if (*(uintptr_t *)pEngfuncs)
				RegisterCVarsAndCommands();
			else
				ORIG_GiveFnptrsToDll = pGiveFnptrsToDll;
		}
		else
		{
			EngineDevWarning("[server dll] Couldn't find the pattern in GiveFnptrsToDll!\n");
			EngineWarning("Serverside CVars and commands are not available.\n");
		}
	}
	else
	{
		EngineDevWarning("[server dll] Couldn't get the address of GiveFnptrsToDll!\n");
		EngineWarning("Serverside CVars and commands are not available.\n");
	}

	DetoursUtils::AttachDetours(moduleName, {
		{ (PVOID *)(&ORIG_PM_Jump), HOOKED_PM_Jump },
		{ (PVOID *)(&ORIG_PM_PreventMegaBunnyJumping), HOOKED_PM_PreventMegaBunnyJumping },
		{ (PVOID *)(&ORIG_PM_PlayerMove), HOOKED_PM_PlayerMove },
		{ (PVOID *)(&ORIG_GiveFnptrsToDll), HOOKED_GiveFnptrsToDll }
	});
}

void ServerDLL::Unhook()
{
	DetoursUtils::DetachDetours(m_Name, {
		{ (PVOID *)(&ORIG_PM_Jump), HOOKED_PM_Jump },
		{ (PVOID *)(&ORIG_PM_PreventMegaBunnyJumping), HOOKED_PM_PreventMegaBunnyJumping },
		{ (PVOID *)(&ORIG_PM_PlayerMove), HOOKED_PM_PlayerMove },
		{ (PVOID *)(&ORIG_GiveFnptrsToDll), HOOKED_GiveFnptrsToDll }
	});

	Clear();
}

void ServerDLL::Clear()
{
	IHookableDirFilter::Clear();
	ORIG_PM_Jump = nullptr;
	ORIG_PM_PreventMegaBunnyJumping = nullptr;
	ORIG_PM_PlayerMove = nullptr;
	ORIG_GiveFnptrsToDll = nullptr;
	ppmove = 0;
	offPlayerIndex = 0;
	offOldbuttons = 0;
	offOnground = 0;
	offVelocity = 0;
	offOrigin = 0;
	offAngles = 0;
	pEngfuncs = nullptr;
	cantJumpNextTime.clear();
}

bool ServerDLL::CanHook(const std::wstring& moduleFullName)
{
	if (!IHookableDirFilter::CanHook(moduleFullName))
		return false;

	// Filter out addons like metamod which may be located into a "dlls" folder under addons.
	std::wstring pathToLiblist = moduleFullName.substr(0, moduleFullName.rfind(GetFolderName(moduleFullName))).append(L"liblist.gam");

	// If liblist.gam exists in the parent directory, then we're (hopefully) good.
	std::ifstream liblist(pathToLiblist);
	if (liblist.good())
	{
		liblist.close();
		return true;
	}

	liblist.close();
	return false;
}

void ServerDLL::RegisterCVarsAndCommands()
{
	if (!pEngfuncs || !*(uintptr_t *)pEngfuncs)
		return;

	if (ORIG_PM_Jump)
		pEngfuncs->pfnCVarRegister(&y_bxt_autojump);

	if (ORIG_PM_PreventMegaBunnyJumping)
		pEngfuncs->pfnCVarRegister(&y_bxt_bhopcap);

	if (ORIG_PM_PlayerMove)
		pEngfuncs->pfnCVarRegister(&_y_bxt_taslog);

	EngineDevMsg("[server dll] Registered CVars.\n");
}

void __cdecl ServerDLL::HOOKED_PM_Jump_Func()
{
	int playerIndex = *(int *)(*(uintptr_t *)ppmove + offPlayerIndex);

	int *onground = (int *)(*(uintptr_t *)ppmove + offOnground);
	int orig_onground = *onground;

	int *oldbuttons = (int *)(*(uintptr_t *)ppmove + offOldbuttons);
	int orig_oldbuttons = *oldbuttons;

	if (y_bxt_autojump.value != 0.0f)
	{
		if ((orig_onground != -1) && !cantJumpNextTime[playerIndex])
			*oldbuttons &= ~IN_JUMP;
	}

	cantJumpNextTime[playerIndex] = false;

	ORIG_PM_Jump();

	if ((orig_onground != -1) && (*onground == -1))
		cantJumpNextTime[playerIndex] = true;

	if (y_bxt_autojump.value != 0.0f)
	{
		*oldbuttons = orig_oldbuttons;
	}
}

void __cdecl ServerDLL::HOOKED_PM_PreventMegaBunnyJumping_Func()
{
	if (y_bxt_bhopcap.value != 0.0f)
		return ORIG_PM_PreventMegaBunnyJumping();
}

void __cdecl ServerDLL::HOOKED_PM_PlayerMove_Func(qboolean server)
{
	if (!ppmove)
		return ORIG_PM_PlayerMove(server);

	int playerIndex = *(int *)(*(uintptr_t *)ppmove + offPlayerIndex);

	float *velocity, *origin, *angles;
	velocity = (float *)(*(uintptr_t *)ppmove + offVelocity);
	origin =   (float *)(*(uintptr_t *)ppmove + offOrigin);
	angles =   (float *)(*(uintptr_t *)ppmove + offAngles);

	if (_y_bxt_taslog.value != 0.0f)
	{
		pEngfuncs->pfnAlertMessage(at_console, "-- BXT TAS Log Start --\n");
		pEngfuncs->pfnAlertMessage(at_console, "Player index: %d\n", playerIndex);
		pEngfuncs->pfnAlertMessage(at_console, "Velocity: %.8f; %.8f; %.8f; origin: %.8f; %.8f; %.8f\n",velocity[0], velocity[1], velocity[2], origin[0], origin[1], origin[2]);
	}

	ORIG_PM_PlayerMove(server);

	if (_y_bxt_taslog.value != 0.0f)
	{
		pEngfuncs->pfnAlertMessage(at_console, "Angles: %.8f; %.8f; %.8f\n", angles[0], angles[1], angles[2]);
		pEngfuncs->pfnAlertMessage(at_console, "New velocity: %.8f; %.8f; %.8f; new origin: %.8f; %.8f; %.8f\n", velocity[0], velocity[1], velocity[2], origin[0], origin[1], origin[2]);
		pEngfuncs->pfnAlertMessage(at_console, "-- BXT TAS Log End --\n");
	}

	CustomHud::UpdatePlayerInfo(velocity, origin);
}

void __stdcall ServerDLL::HOOKED_GiveFnptrsToDll_Func(enginefuncs_t* pEngfuncsFromEngine, const void* pGlobals)
{
	ORIG_GiveFnptrsToDll(pEngfuncsFromEngine, pGlobals);

	RegisterCVarsAndCommands();
}
