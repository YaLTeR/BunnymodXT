#include "stdafx.hpp"

#include "..\sptlib-wrapper.hpp"
#include <SPTLib\memutils.hpp>
#include <SPTLib\detoursutils.hpp>
#include <SPTLib\hooks.hpp>
#include "..\modules.hpp"
#include "..\patterns.hpp"

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

	DetoursUtils::AttachDetours(moduleName, {
		{ (PVOID *)(&ORIG_PM_Jump), HOOKED_PM_Jump },
		{ (PVOID *)(&ORIG_PM_PreventMegaBunnyJumping), HOOKED_PM_PreventMegaBunnyJumping }
	});
}

void ClientDLL::Unhook()
{
	DetoursUtils::DetachDetours(moduleName, {
		{ (PVOID *)(&ORIG_PM_Jump), HOOKED_PM_Jump },
		{ (PVOID *)(&ORIG_PM_PreventMegaBunnyJumping), HOOKED_PM_PreventMegaBunnyJumping }
	});

	Clear();
}

void ClientDLL::Clear()
{
	IHookableNameFilter::Clear();
	ORIG_PM_Jump = nullptr;
	ORIG_PM_PreventMegaBunnyJumping = nullptr;
	ppmove = 0;
	offOldbuttons = 0;
	offOnground = 0;
	cantJumpNextTime = false;
}

void __cdecl ClientDLL::HOOKED_PM_Jump_Func()
{
	int *onground = (int *)(*(uintptr_t *)ppmove + offOnground);
	int orig_onground = *onground;

	int *oldbuttons = (int *)(*(uintptr_t *)ppmove + offOldbuttons);
	int orig_oldbuttons = *oldbuttons;

	if ((orig_onground != -1) && !cantJumpNextTime)
		*oldbuttons &= ~IN_JUMP;

	cantJumpNextTime = false;

	ORIG_PM_Jump();

	if ((orig_onground != -1) && (*onground == -1))
		cantJumpNextTime = true;

	*oldbuttons = orig_oldbuttons;
}

void __cdecl ClientDLL::HOOKED_PM_PreventMegaBunnyJumping_Func()
{
	return;
}
