#include "stdafx.hpp"

#include "..\sptlib-wrapper.hpp"
#include <SPTLib\memutils.hpp>
#include <SPTLib\detoursutils.hpp>
#include <SPTLib\hooks.hpp>
#include "..\modules.hpp"
#include "..\patterns.hpp"
#include "..\cvars.hpp"
#include "..\hud_custom.hpp"

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

void __fastcall ClientDLL::HOOKED_CHud_Init(void* thisptr, int edx)
{
	return clientDLL.HOOKED_CHud_Init_Func(thisptr, edx);
}

void __fastcall ClientDLL::HOOKED_CHud_VidInit(void* thisptr, int edx)
{
	return clientDLL.HOOKED_CHud_VidInit_Func(thisptr, edx);
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
		pPMPreventMegaBunnyJumping = NULL,
		pCHud_AddHudElem = NULL;

	auto fPMPreventMegaBunnyJumping = std::async(std::launch::async, MemUtils::FindUniqueSequence, moduleStart, moduleLength, Patterns::ptnsPMPreventMegaBunnyJumping, &pPMPreventMegaBunnyJumping);
	auto fCHud_AddHudElem = std::async(std::launch::async, MemUtils::FindUniqueSequence, moduleStart, moduleLength, Patterns::ptnsCHud_AddHudElem, &pCHud_AddHudElem);
	auto fIsGMC = std::async(std::launch::deferred, MemUtils::FindPattern, moduleStart, moduleLength, (const byte *)"weapon_SPchemicalgun", "xxxxxxxxxxxxxxxxxxxx");

	ptnNumber = MemUtils::FindUniqueSequence(moduleStart, moduleLength, Patterns::ptnsPMJump, &pPMJump);
	if (ptnNumber != MemUtils::INVALID_SEQUENCE_INDEX)
	{
		ORIG_PM_Jump = (_PM_Jump)pPMJump;
		EngineDevMsg("[client dll] Found PM_Jump at %p (using the %s pattern).\n", pPMJump, Patterns::ptnsPMJump[ptnNumber].build.c_str());

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

		case 2: // AG-Server, shouldn't happen here but who knows.
			ppmove = *(uintptr_t *)(pPMJump + 3);
			offOldbuttons = 200;
			offOnground = 224;
			break;

		case 3:
			ppmove = *(uintptr_t *)(pPMJump + 3);
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
		EngineDevMsg("[client dll] Found PM_PreventMegaBunnyJumping at %p (using the %s pattern).\n", pPMPreventMegaBunnyJumping, Patterns::ptnsPMPreventMegaBunnyJumping[ptnNumber].build.c_str());
	}
	else
	{
		EngineDevWarning("[client dll] Could not find PM_PreventMegaBunnyJumping!\n");
		EngineWarning("Bhopcap disabling prediction is not available.\n");
	}

	// In AG, this thing is the main function, so check that first.
	_Initialize pInitialize = (_Initialize)GetProcAddress(hModule, "?Initialize_Body@@YAHPAUcl_enginefuncs_s@@H@Z");

	if (!pInitialize)
		pInitialize = (_Initialize)GetProcAddress(hModule, "Initialize");

	if (pInitialize)
	{
		// Find "mov edi, offset dword; rep movsd" inside Initialize. The pointer to gEngfuncs is that dword.
		const byte pattern[] = { 0xBF, '?', '?', '?', '?', 0xF3, 0xA5 };
		uintptr_t addr = MemUtils::FindPattern((uintptr_t)pInitialize, 40, pattern, "x????xx");
		if (addr != NULL)
		{
			pEngfuncs = *(cl_enginefunc_t **)(addr + 1);
			EngineDevMsg("[client dll] pEngfuncs is %p.\n", pEngfuncs);

			// If we have engfuncs, register cvars and whatnot right away (in the end of this function because other stuff need to be done first). Otherwise wait till the engine gives us engfuncs.
			// This works because global variables are zero by default.
			if (!*(uintptr_t *)pEngfuncs)
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

	// We can draw stuff only if we know that we have already received / will receive engfuncs.
	if (pEngfuncs != NULL)
	{
		void *pHUD_Init = GetProcAddress(hModule, "HUD_Init");
		if (pHUD_Init)
		{
			// Just in case some HUD_Init contains extra stuff, find the first "mov ecx, offset dword; call func" sequence. Dword is the pointer to gHud and func is CHud::Init.
			const byte pattern[] = { 0xB9, '?', '?', '?', '?', 0xE8 };

			// BS has jmp instead of call.
			const byte pattern_bs[] = { 0xB9, '?', '?', '?', '?', 0xE9 };

			// OP4 contains some stuff between our instructions.
			const byte pattern_op4[] = { 0xB9, '?', '?', '?', '?', 0xA3, '?', '?', '?', '?',
				0xC7, 0x05, '?', '?', '?', '?', 0xA0, 0x00, 0x00, 0x00,
				0xA3, '?', '?', '?', '?', 0xE8 };

			ptrdiff_t offCallOffset = 6;
			uintptr_t addr = MemUtils::FindPattern((uintptr_t)pHUD_Init, 0x15, pattern, "x????x");
			if (addr == NULL)
				addr = MemUtils::FindPattern((uintptr_t)pHUD_Init, 0x15, pattern_bs, "x????x");

			if (addr == NULL)
			{
				addr = MemUtils::FindPattern((uintptr_t)pHUD_Init, 0x15, pattern_op4, "x????x????xx????xxxxx????x");
				offCallOffset = 26;
				novd = true;
				EngineDevMsg("[client dll] Using CHudBase without a virtual destructor.\n");
			}
			else
			{
				// Check for GMC.
				if (fIsGMC.get() != NULL)
				{
					novd = true;
					EngineDevMsg("[client dll] Using CHudBase without a virtual destructor.\n");
				}
			}

			if (addr != NULL)
			{
				pHud = *(uintptr_t *)(addr + 1);
				ORIG_CHud_Init = (_CHud_InitFunc)(*(uintptr_t *)(addr + offCallOffset) + (addr + offCallOffset + 4)); // Call by offset.
				EngineDevMsg("[client dll] pHud is %p; CHud::Init is located at %p.\n", pHud, ORIG_CHud_Init);

				void *pHUD_Reset = GetProcAddress(hModule, "HUD_Reset");
				if (pHUD_Reset)
				{
					// Same as with HUD_Init earlier, but we have another possibility - jmp instead of call.
					#define getbyte(a, n) (byte)((a >> n*8) & 0xFF)
					const byte ptn1[] = { 0xB9, getbyte(pHud, 0), getbyte(pHud, 1), getbyte(pHud, 2), getbyte(pHud, 3), 0xE8 },
						ptn2[] = { 0xB9, getbyte(pHud, 0), getbyte(pHud, 1), getbyte(pHud, 2), getbyte(pHud, 3), 0xE9 };
					#undef getbyte

					uintptr_t addr_ = MemUtils::FindPattern((uintptr_t)pHUD_Reset, 0x10, ptn1, "xxxxxx");
					if (addr_ == NULL)
						addr_ = MemUtils::FindPattern((uintptr_t)pHUD_Reset, 0x10, ptn2, "xxxxxx");

					if (addr_ != NULL)
					{
						ORIG_CHud_VidInit = (_CHud_InitFunc)(*(uintptr_t *)(addr_ + 6) + (addr_ + 10));
						EngineDevMsg("[client dll] CHud::VidInit is located at %p.\n", ORIG_CHud_VidInit);
					}
					else
					{
						pHUD_Reset = nullptr; // Try with HUD_VidInit.
					}
				}

				if (!pHUD_Reset)
				{
					void *pHUD_VidInit = GetProcAddress(hModule, "HUD_VidInit");
					if (pHUD_VidInit)
					{
						#define getbyte(a, n) (byte)((a >> n*8) & 0xFF)
						const byte ptn[] = { 0xB9, getbyte(pHud, 0), getbyte(pHud, 1), getbyte(pHud, 2), getbyte(pHud, 3), 0xE8 };
						#undef getbyte

						uintptr_t addr_ = MemUtils::FindPattern((uintptr_t)pHUD_VidInit, 0x10, ptn, "xxxxxx");
						if (addr_ != NULL)
						{
							ORIG_CHud_VidInit = (_CHud_InitFunc)(*(uintptr_t *)(addr_ + 6) + (addr_ + 10));
							EngineDevMsg("[client dll] CHud::VidInit is located at %p.\n", ORIG_CHud_VidInit);
						}
						else
						{
							ORIG_CHud_Init = nullptr;

							EngineDevWarning("[client dll] Couldn't find the pattern in HUD_Reset or HUD_VidInit!\n");
							EngineWarning("Custom HUD is not available.\n");
						}
					}
					else
					{
						ORIG_CHud_Init = nullptr;

						EngineDevWarning("[client dll] Couldn't get the address of HUD_Reset or HUD_VidInit!\n");
						EngineWarning("Custom HUD is not available.\n");
					}
				}
			}
			else
			{
				EngineDevWarning("[client dll] Couldn't find the pattern in HUD_Init!\n");
				EngineWarning("Custom HUD is not available.\n");
			}
		}
		else
		{
			EngineDevWarning("[client dll] Couldn't get the address of HUD_Init!\n");
			EngineWarning("Custom HUD is not available.\n");
		}

		if (ORIG_CHud_Init)
		{
			ptnNumber = fCHud_AddHudElem.get();
			if (ptnNumber != MemUtils::INVALID_SEQUENCE_INDEX)
			{
				CHud_AddHudElem = (_CHud_AddHudElem)pCHud_AddHudElem;
				EngineDevMsg("[client dll] Found CHud::AddHudElem at %p (using the %s pattern).\n", pCHud_AddHudElem, Patterns::ptnsCHud_AddHudElem[ptnNumber].build.c_str());
			}
			else
			{
				ORIG_CHud_Init = nullptr;
				ORIG_CHud_VidInit = nullptr;

				EngineDevWarning("[client dll] Could not find CHud::AddHudElem!\n");
				EngineWarning("Custom HUD is not available.\n");
			}
		}
	}
	
	// Now we can register cvars and commands provided that we already have engfuncs.
	if (*(uintptr_t *)pEngfuncs)
		RegisterCVarsAndCommands();

	DetoursUtils::AttachDetours(moduleName, {
		{ (PVOID *)(&ORIG_PM_Jump), HOOKED_PM_Jump },
		{ (PVOID *)(&ORIG_PM_PreventMegaBunnyJumping), HOOKED_PM_PreventMegaBunnyJumping },
		{ (PVOID *)(&ORIG_Initialize), HOOKED_Initialize },
		{ (PVOID *)(&ORIG_CHud_Init), HOOKED_CHud_Init },
		{ (PVOID *)(&ORIG_CHud_VidInit), HOOKED_CHud_VidInit }
	});
}

void ClientDLL::Unhook()
{
	DetoursUtils::DetachDetours(moduleName, {
		{ (PVOID *)(&ORIG_PM_Jump), HOOKED_PM_Jump },
		{ (PVOID *)(&ORIG_PM_PreventMegaBunnyJumping), HOOKED_PM_PreventMegaBunnyJumping },
		{ (PVOID *)(&ORIG_Initialize), HOOKED_Initialize },
		{ (PVOID *)(&ORIG_CHud_Init), HOOKED_CHud_Init },
		{ (PVOID *)(&ORIG_CHud_VidInit), HOOKED_CHud_VidInit }
	});

	Clear();
}

void ClientDLL::Clear()
{
	IHookableNameFilter::Clear();
	ORIG_PM_Jump = nullptr;
	ORIG_PM_PreventMegaBunnyJumping = nullptr;
	ORIG_Initialize = nullptr;
	ORIG_CHud_Init = nullptr;
	ORIG_CHud_VidInit = nullptr;
	CHud_AddHudElem = nullptr;
	ppmove = 0;
	offOldbuttons = 0;
	offOnground = 0;
	pEngfuncs = nullptr;
	pHud = 0;
	cantJumpNextTime = false;
	novd = false;
}

void ClientDLL::RegisterCVarsAndCommands()
{
	if (!pEngfuncs || !*(uintptr_t *)pEngfuncs)
		return;

	if (ORIG_PM_Jump)
		y_bxt_autojump_prediction = pEngfuncs->pfnRegisterVariable("y_bxt_autojump_prediction", "0", 0);

	if (ORIG_PM_PreventMegaBunnyJumping)
		y_bxt_bhopcap_prediction = pEngfuncs->pfnRegisterVariable("y_bxt_bhopcap_prediction", "0", 0);

	if (ORIG_CHud_Init)
	{
		y_bxt_hud = pEngfuncs->pfnRegisterVariable("y_bxt_hud", "1", 0);
		y_bxt_hud_precision = pEngfuncs->pfnRegisterVariable("y_bxt_hud_precision", "0", 0);
		y_bxt_hud_velocity = pEngfuncs->pfnRegisterVariable("y_bxt_hud_velocity", "1", 0);
		y_bxt_hud_velocity_pos = pEngfuncs->pfnRegisterVariable("y_bxt_hud_velocity_pos", "-200 0", 0);
	}

	EngineDevMsg("[client dll] Registered CVars.\n");
}

void ClientDLL::AddHudElem(void* pHudElem)
{
	if (pHud && CHud_AddHudElem)
		CHud_AddHudElem((void *)pHud, 0, pHudElem);
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

void __fastcall ClientDLL::HOOKED_CHud_Init_Func(void* thisptr, int edx)
{
	ORIG_CHud_Init(thisptr, edx);

	if (novd)
		customHudWrapper_NoVD.Init();
	else
		customHudWrapper.Init();
}

void __fastcall ClientDLL::HOOKED_CHud_VidInit_Func(void* thisptr, int edx)
{
	ORIG_CHud_VidInit(thisptr, edx);

	if (novd)
	{
		customHudWrapper_NoVD.InitIfNecessary();
		customHudWrapper_NoVD.VidInit();
	}
	else
	{
		customHudWrapper.InitIfNecessary();
		customHudWrapper.VidInit();
	}
}
