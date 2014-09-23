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

bool ServerDLL::CanHook(const std::wstring& moduleFullName)
{
	if ( !IHookableDirFilter::CanHook(moduleFullName) )
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

void __cdecl ServerDLL::HOOKED_PM_Jump()
{
	return serverDLL.HOOKED_PM_Jump_Func();
}

void __cdecl ServerDLL::HOOKED_PM_PreventMegaBunnyJumping()
{
	return serverDLL.HOOKED_PM_PreventMegaBunnyJumping_Func();
}

void __stdcall ServerDLL::HOOKED_GiveFnptrsToDll(enginefuncs_t* pEngfuncsFromEngine, const void* pGlobals)
{
	return serverDLL.HOOKED_GiveFnptrsToDll_Func(pEngfuncsFromEngine, pGlobals);
}

void ServerDLL::Hook(const std::wstring& moduleName, HMODULE hModule, uintptr_t moduleStart, size_t moduleLength)
{
	Clear(); // Just in case.

	this->hModule = hModule;
	this->moduleStart = moduleStart;
	this->moduleLength = moduleLength;
	this->moduleName = moduleName;

	MemUtils::ptnvec_size ptnNumber;

	uintptr_t pPMJump = NULL,
		pPMPreventMegaBunnyJumping = NULL;

	auto fPMPreventMegaBunnyJumping = std::async(std::launch::async, MemUtils::FindUniqueSequence, moduleStart, moduleLength, Patterns::ptnsPMPreventMegaBunnyJumping, &pPMPreventMegaBunnyJumping);

	ptnNumber = MemUtils::FindUniqueSequence(moduleStart, moduleLength, Patterns::ptnsPMJump, &pPMJump);
	if (ptnNumber != MemUtils::INVALID_SEQUENCE_INDEX)
	{
		ORIG_PM_Jump = (_PM_Jump)pPMJump;
		EngineDevMsg("[server dll] Found PM_Jump at %p (using the build %s pattern).\n", pPMJump, Patterns::ptnsPMJump[ptnNumber].build.c_str());

		switch (ptnNumber)
		{
		case 0:
			ppmove = *(uintptr_t *)(pPMJump + 2);
			offOldbuttons = 200;
			offOnground = 224;
			break;

		case 1:
			ppmove = *(uintptr_t *)(pPMJump + 2);
			offOldbuttons = 200;
			offOnground = 224;
			break;

		case 2:
			ppmove = *(uintptr_t *)(pPMJump + 3);
			offOldbuttons = 200;
			offOnground = 224;
			break;

		case 3: // AG-Client, shouldn't happen here but who knows.
			ppmove = *(uintptr_t *)(pPMJump + 3);
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
		EngineDevMsg("[server dll] Found PM_PreventMegaBunnyJumping at %p (using the build %s pattern).\n", pPMPreventMegaBunnyJumping, Patterns::ptnsPMPreventMegaBunnyJumping[ptnNumber].build.c_str());
	}
	else
	{
		EngineDevWarning("[server dll] Could not find PM_PreventMegaBunnyJumping!\n");
		EngineWarning("Bhopcap disabling is not available.\n");
	}

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
		{ (PVOID *)(&ORIG_GiveFnptrsToDll), HOOKED_GiveFnptrsToDll }
	});
}

void ServerDLL::Unhook()
{
	DetoursUtils::DetachDetours(moduleName, {
		{ (PVOID *)(&ORIG_PM_Jump), HOOKED_PM_Jump },
		{ (PVOID *)(&ORIG_PM_PreventMegaBunnyJumping), HOOKED_PM_PreventMegaBunnyJumping },
		{ (PVOID *)(&ORIG_GiveFnptrsToDll), HOOKED_GiveFnptrsToDll }
	});

	Clear();
}

void ServerDLL::Clear()
{
	IHookableDirFilter::Clear();
	ORIG_PM_Jump = nullptr;
	ORIG_PM_PreventMegaBunnyJumping = nullptr;
	ORIG_GiveFnptrsToDll = nullptr;
	ppmove = 0;
	offOldbuttons = 0;
	offOnground = 0;
	pEngfuncs = nullptr;
	cantJumpNextTime = false;
}

void ServerDLL::RegisterCVarsAndCommands()
{
	if (!pEngfuncs || !*(uintptr_t *)pEngfuncs)
		return;

	if (ORIG_PM_Jump)
		pEngfuncs->pfnCVarRegister(&y_bxt_autojump);

	if (ORIG_PM_PreventMegaBunnyJumping)
		pEngfuncs->pfnCVarRegister(&y_bxt_bhopcap);

	EngineDevMsg("[server dll] Registered CVars.\n");
}

void __cdecl ServerDLL::HOOKED_PM_Jump_Func()
{
	int *onground = (int *)(*(uintptr_t *)ppmove + offOnground);
	int orig_onground = *onground;

	int *oldbuttons = (int *)(*(uintptr_t *)ppmove + offOldbuttons);
	int orig_oldbuttons = *oldbuttons;

	if (y_bxt_autojump.value != 0.0f)
	{
		if ((orig_onground != -1) && !cantJumpNextTime)
			*oldbuttons &= ~IN_JUMP;
	}

	cantJumpNextTime = false;

	ORIG_PM_Jump();

	if ((orig_onground != -1) && (*onground == -1))
		cantJumpNextTime = true;

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

void __stdcall ServerDLL::HOOKED_GiveFnptrsToDll_Func(enginefuncs_t* pEngfuncsFromEngine, const void* pGlobals)
{
	ORIG_GiveFnptrsToDll(pEngfuncsFromEngine, pGlobals);

	RegisterCVarsAndCommands();
}
