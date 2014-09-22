#include "stdafx.hpp"

#include "..\sptlib-wrapper.hpp"
#include <SPTLib\memutils.hpp>
#include <SPTLib\detoursutils.hpp>
#include <SPTLib\hooks.hpp>
#include "..\modules.hpp"
#include "..\patterns.hpp"
#include "..\cvars.hpp"

using std::uintptr_t;
using std::size_t;

void __cdecl ClientDLL::HOOKED_PM_Jump()
{
	return clientDLL.HOOKED_PM_Jump_Func();
}

void __cdecl ClientDLL::HOOKED_PM_PreventMegaBunnyJumping()
{
	return clientDLL.HOOKED_PM_PreventMegaBunnyJumping_Func();
}

int __cdecl ClientDLL::HOOKED_Initialize(cl_enginefunc_t* pEnginefuncs, int iVersion)
{
	return clientDLL.HOOKED_Initialize_Func(pEnginefuncs, iVersion);
}

void ClientDLL::Hook(const std::wstring& moduleName, HMODULE hModule, uintptr_t moduleStart, size_t moduleLength)
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
		EngineDevMsg("[client dll] Found PM_Jump at %p (using the build %s pattern).\n", pPMJump, Patterns::ptnsPMJump[ptnNumber].build.c_str());

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
		}
	}
	else
	{
		EngineDevWarning("[client dll] Could not find PM_Jump!\n");
		EngineWarning("Autojump prediction is not available.\n");
	}

	ptnNumber = fPMPreventMegaBunnyJumping.get();
	if (ptnNumber != MemUtils::INVALID_SEQUENCE_INDEX)
	{
		ORIG_PM_PreventMegaBunnyJumping = (_PM_PreventMegaBunnyJumping)pPMPreventMegaBunnyJumping;
		EngineDevMsg("[client dll] Found PM_PreventMegaBunnyJumping at %p (using the build %s pattern).\n", pPMPreventMegaBunnyJumping, Patterns::ptnsPMPreventMegaBunnyJumping[ptnNumber].build.c_str());
	}
	else
	{
		EngineDevWarning("[client dll] Could not find PM_PreventMegaBunnyJumping!\n");
		EngineWarning("Bhopcap disabling prediction is not available.\n");
	}

	_Initialize pInitialize = (_Initialize)GetProcAddress(hModule, "Initialize");
	if (pInitialize != NULL)
	{
		// Find "mov edi, offset dword; rep movsd" inside Initialize. The pointer to gEngfuncs is that dword.
		const byte pattern[] = { 0xBF, '?', '?', '?', '?', 0xF3, 0xA5 };
		uintptr_t addr = MemUtils::FindPattern((uintptr_t)pInitialize, 40, pattern, "x????xx");
		if (addr != NULL)
		{
			pEngfuncs = *(cl_enginefunc_t **)(addr + 1);
			EngineDevMsg("[client dll] pEngfuncs is %p.\n", pEngfuncs);

			// If we have engfuncs, do stuff right away. Otherwise wait till the engine gives us engfuncs.
			if (*(uintptr_t *)pEngfuncs)
				RegisterCVarsAndCommands();
			else
				ORIG_Initialize = pInitialize;
		}
		else
		{
			EngineDevWarning("[client dll] Couldn't find the pattern in Initialize!\n");
			EngineWarning("Clientside CVars and commands are not available.\n");
		}
	}
	else
	{
		EngineDevWarning("[client dll] Couldn't get the address of Initialize!\n");
		EngineWarning("Clientside CVars and commands are not available.\n");
	}

	DetoursUtils::AttachDetours(moduleName, {
		{ (PVOID *)(&ORIG_PM_Jump), HOOKED_PM_Jump },
		{ (PVOID *)(&ORIG_PM_PreventMegaBunnyJumping), HOOKED_PM_PreventMegaBunnyJumping },
		{ (PVOID *)(&ORIG_Initialize), HOOKED_Initialize }
	});
}

void ClientDLL::Unhook()
{
	DetoursUtils::DetachDetours(moduleName, {
		{ (PVOID *)(&ORIG_PM_Jump), HOOKED_PM_Jump },
		{ (PVOID *)(&ORIG_PM_PreventMegaBunnyJumping), HOOKED_PM_PreventMegaBunnyJumping },
		{ (PVOID *)(&ORIG_Initialize), HOOKED_Initialize }
	});

	Clear();
}

void ClientDLL::Clear()
{
	IHookableNameFilter::Clear();
	ORIG_PM_Jump = nullptr;
	ORIG_PM_PreventMegaBunnyJumping = nullptr;
	ORIG_Initialize = nullptr;
	ppmove = 0;
	offOldbuttons = 0;
	offOnground = 0;
	pEngfuncs = nullptr;
	cantJumpNextTime = false;
}

void ClientDLL::RegisterCVarsAndCommands()
{
	if (!pEngfuncs || !*(uintptr_t *)pEngfuncs)
		return;

	if (ORIG_PM_Jump)
		y_bxt_autojump_prediction = pEngfuncs->pfnRegisterVariable("y_bxt_autojump_prediction", "0", 0);

	if (ORIG_PM_PreventMegaBunnyJumping)
		y_bxt_bhopcap_prediction = pEngfuncs->pfnRegisterVariable("y_bxt_bhopcap_prediction",  "0", 0);

	EngineDevMsg("[client dll] Registered CVars.\n");
}

void __cdecl ClientDLL::HOOKED_PM_Jump_Func()
{
	int *onground = (int *)(*(uintptr_t *)ppmove + offOnground);
	int orig_onground = *onground;

	int *oldbuttons = (int *)(*(uintptr_t *)ppmove + offOldbuttons);
	int orig_oldbuttons = *oldbuttons;

	if (!y_bxt_autojump_prediction || (y_bxt_autojump_prediction->value != 0.0f))
	{
		if ((orig_onground != -1) && !cantJumpNextTime)
			*oldbuttons &= ~IN_JUMP;
	}

	cantJumpNextTime = false;

	ORIG_PM_Jump();

	if ((orig_onground != -1) && (*onground == -1))
		cantJumpNextTime = true;

	if (!y_bxt_autojump_prediction || (y_bxt_autojump_prediction->value != 0.0f))
	{
		*oldbuttons = orig_oldbuttons;
	}
}

void __cdecl ClientDLL::HOOKED_PM_PreventMegaBunnyJumping_Func()
{
	if (y_bxt_bhopcap_prediction && (y_bxt_bhopcap_prediction->value != 0.0f))
		ORIG_PM_PreventMegaBunnyJumping();
}

int __cdecl ClientDLL::HOOKED_Initialize_Func(cl_enginefunc_t* pEnginefuncs, int iVersion)
{
	int rv = ORIG_Initialize(pEnginefuncs, iVersion);

	RegisterCVarsAndCommands();

	return rv;
}
