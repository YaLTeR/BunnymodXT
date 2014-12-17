#include "../stdafx.hpp"

#include "../sptlib-wrapper.hpp"
#include <SPTLib/MemUtils.hpp>
#include <SPTLib/Hooks.hpp>
#include "../modules.hpp"
#include "../patterns.hpp"
#include "../cvars.hpp"
#include "../hud_custom.hpp"

void __cdecl ClientDLL::HOOKED_PM_Jump()
{
	return clientDLL.HOOKED_PM_Jump_Func();
}

void __cdecl ClientDLL::HOOKED_PM_PlayerMove(qboolean server)
{
	return clientDLL.HOOKED_PM_PlayerMove_Func(server);
}

void __cdecl ClientDLL::HOOKED_PM_PreventMegaBunnyJumping()
{
	return clientDLL.HOOKED_PM_PreventMegaBunnyJumping_Func();
}

int __cdecl ClientDLL::HOOKED_Initialize(cl_enginefunc_t* pEnginefuncs, int iVersion)
{
	return clientDLL.HOOKED_Initialize_Func(pEnginefuncs, iVersion);
}

void __cdecl ClientDLL::HOOKED_V_CalcRefdef(ref_params_t* pparams)
{
	return clientDLL.HOOKED_V_CalcRefdef_Func(pparams);
}

void __cdecl ClientDLL::HOOKED_HUD_Init()
{
	return clientDLL.HOOKED_HUD_Init_Func();
}

#ifdef _WIN32
void __fastcall ClientDLL::HOOKED_CHud_Init(void* thisptr, int edx)
{
	return clientDLL.HOOKED_CHud_Init_Func(thisptr, edx);
}

void __fastcall ClientDLL::HOOKED_CHud_VidInit(void* thisptr, int edx)
{
	return clientDLL.HOOKED_CHud_VidInit_Func(thisptr, edx);
}
#else
void __cdecl ClientDLL::HOOKED_CHud_Init(void* thisptr)
{
	return clientDLL.HOOKED_CHud_Init_Func(thisptr);
}

void __cdecl ClientDLL::HOOKED_CHud_VidInit(void* thisptr)
{
	return clientDLL.HOOKED_CHud_VidInit_Func(thisptr);
}
#endif

// Linux hooks.
#ifndef _WIN32
extern "C" int __cdecl Initialize(cl_enginefunc_t* pEnginefuncs, int iVersion)
{
	return ClientDLL::HOOKED_Initialize(pEnginefuncs, iVersion);
}

extern "C" void __cdecl _ZN4CHud4InitEv(void* thisptr)
{
	return ClientDLL::HOOKED_CHud_Init(thisptr);
}

extern "C" void __cdecl _ZN4CHud7VidInitEv(void* thisptr)
{
	return ClientDLL::HOOKED_CHud_VidInit(thisptr);
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

	void *pPMJump, *pPMPreventMegaBunnyJumping, *pCHud_AddHudElem;

	ORIG_PM_PlayerMove = reinterpret_cast<_PM_PlayerMove>(MemUtils::GetSymbolAddress(moduleHandle, "PM_PlayerMove"));

	std::future<MemUtils::ptnvec_size> fPMPreventMegaBunnyJumping, fCHud_AddHudElem;

	pPMPreventMegaBunnyJumping = MemUtils::GetSymbolAddress(moduleHandle, "PM_PreventMegaBunnyJumping");
	if (pPMPreventMegaBunnyJumping)
	{
		ORIG_PM_PreventMegaBunnyJumping = reinterpret_cast<_PM_PreventMegaBunnyJumping>(pPMPreventMegaBunnyJumping);
		EngineDevMsg("[client dll] Found PM_PreventMegaBunnyJumping at %p.\n", pPMPreventMegaBunnyJumping);
	}
	else
		fPMPreventMegaBunnyJumping = std::async(std::launch::async, MemUtils::FindUniqueSequence, moduleBase, moduleLength, Patterns::ptnsPMPreventMegaBunnyJumping, &pPMPreventMegaBunnyJumping);

	pCHud_AddHudElem = MemUtils::GetSymbolAddress(moduleHandle, "_ZN4CHud10AddHudElemEP8CHudBase");
	if (pCHud_AddHudElem)
	{
		CHud_AddHudElem = reinterpret_cast<_CHud_AddHudElem>(pCHud_AddHudElem);
		EngineDevMsg("[client dll] Found CHud::AddHudElem at %p.\n", pCHud_AddHudElem);
	}
	else
		fCHud_AddHudElem = std::async(std::launch::async, MemUtils::FindUniqueSequence, moduleBase, moduleLength, Patterns::ptnsCHud_AddHudElem, &pCHud_AddHudElem);

	auto fIsOP4 = std::async(std::launch::deferred, MemUtils::FindPattern, moduleBase, moduleLength, reinterpret_cast<const byte *>("weapon_pipewrench"), "xxxxxxxxxxxxxxxxxx");
	auto fIsGMC = std::async(std::launch::deferred, MemUtils::FindPattern, moduleBase, moduleLength, reinterpret_cast<const byte *>("weapon_SPchemicalgun"), "xxxxxxxxxxxxxxxxxxxxx");

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

			switch (ptnNumber)
			{
			case 0:
			case 1:
				ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(pPMJump) + 2);
				offOldbuttons = 200;
				offOnground = 224;
				break;

			case 2: // AG-Server, shouldn't happen here but who knows.
			case 3:
				ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(pPMJump) + 3);
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
	{
		auto pHUD_Init = MemUtils::GetSymbolAddress(moduleHandle, "HUD_Init");
		if (pHUD_Init)
		{
			// Just in case some HUD_Init contains extra stuff, find the first "mov ecx, offset dword; call func" sequence. Dword is the pointer to gHud and func is CHud::Init.
			const byte pattern[] = { 0xB9, '?', '?', '?', '?', 0xE8 };

			// BS has jmp instead of call.
			const byte pattern_bs[] = { 0xB9, '?', '?', '?', '?', 0xE9 };

			// The Linux version has a more complicated mov.
			const byte pattern_linux[] = { 0xC7, 0x04, 0x24, '?', '?', '?', '?', 0xE8 };

			// OP4 contains some stuff between our instructions.
			const byte pattern_op4[] = { 0xB9, '?', '?', '?', '?', 0xA3, '?', '?', '?', '?',
				0xC7, 0x05, '?', '?', '?', '?', 0xA0, 0x00, 0x00, 0x00,
				0xA3, '?', '?', '?', '?', 0xE8 };

			ptrdiff_t offGHudOffset = 1;
			ptrdiff_t offCallOffset = 6;
			auto addr = MemUtils::FindPattern(pHUD_Init, 16, pattern, "x????x");
			if (!addr)
			{
				addr = MemUtils::FindPattern(pHUD_Init, 16, pattern_bs, "x????x");
				if (addr)
					ORIG_HUD_Init = reinterpret_cast<_HUD_Init>(pHUD_Init);
			}
			if (!addr)
			{
				addr = MemUtils::FindPattern(pHUD_Init, 40, pattern_linux, "xxx????x");
				if (addr)
				{
					offCallOffset = 8;
					offGHudOffset = 3;
				}
			}
			if (!addr)
			{
				addr = MemUtils::FindPattern(pHUD_Init, 40, pattern_op4, "x????x????xx????xxxxx????x");
				if (addr)
				{
					offCallOffset = 26;
					novd = true;
					EngineDevMsg("[client dll] Using CHudBase without a virtual destructor.\n");
				}
			}
			else
			{
				// Check for GMC or Linux OP4.
				if (fIsGMC.get() || fIsOP4.get())
				{
					novd = true;
					EngineDevMsg("[client dll] Using CHudBase without a virtual destructor.\n");
				}
			}

			if (addr)
			{
				pHud = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(addr) + offGHudOffset);

				ORIG_CHud_Init = reinterpret_cast<_CHud_InitFunc>(MemUtils::GetSymbolAddress(moduleHandle, "_ZN4CHud4InitEv"));
				if (!ORIG_CHud_Init && !ORIG_HUD_Init)
				{
					ORIG_CHud_Init = reinterpret_cast<_CHud_InitFunc>(
						*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(addr)+offCallOffset)
						+ (reinterpret_cast<uintptr_t>(addr)+offCallOffset + 4)); // Call by offset.
					EngineDevMsg("[client dll] pHud is %p; CHud::Init is located at %p.\n", pHud, ORIG_CHud_Init);
				}
				
				if (ORIG_HUD_Init)
					EngineDevMsg("[client dll] pHud is %p; hooking HUD_Init at %p.\n", pHud, ORIG_HUD_Init);

				ORIG_CHud_VidInit = reinterpret_cast<_CHud_InitFunc>(MemUtils::GetSymbolAddress(moduleHandle, "_ZN4CHud7VidInitEv"));
				if (!ORIG_CHud_VidInit)
				{
					auto pHUD_Reset = MemUtils::GetSymbolAddress(moduleHandle, "HUD_Reset");
					if (pHUD_Reset)
					{
						// Same as with HUD_Init earlier, but we have another possibility - jmp instead of call.
						auto pHud_uintptr = reinterpret_cast<uintptr_t>(pHud);
						#define getbyte(a, n) static_cast<byte>((a >> n*8) & 0xFF)
						const byte ptn1[] = { 0xB9, getbyte(pHud_uintptr, 0), getbyte(pHud_uintptr, 1), getbyte(pHud_uintptr, 2), getbyte(pHud_uintptr, 3), 0xE8 },
							ptn2[] = { 0xB9, getbyte(pHud_uintptr, 0), getbyte(pHud_uintptr, 1), getbyte(pHud_uintptr, 2), getbyte(pHud_uintptr, 3), 0xE9 },
							ptn3[] = { 0x24, getbyte(pHud_uintptr, 0), getbyte(pHud_uintptr, 1), getbyte(pHud_uintptr, 2), getbyte(pHud_uintptr, 3), 0xE8 };
						#undef getbyte

						auto addr_ = MemUtils::FindPattern(pHUD_Reset, 20, ptn1, "xxxxxx");
						if (!addr_)
							addr_ = MemUtils::FindPattern(pHUD_Reset, 20, ptn2, "xxxxxx");
						if (!addr_)
							addr_ = MemUtils::FindPattern(pHUD_Reset, 20, ptn3, "xxxxxx");

						if (addr_)
						{
							ORIG_CHud_VidInit = reinterpret_cast<_CHud_InitFunc>(
								*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(addr_) + 6)
								+ (reinterpret_cast<uintptr_t>(addr_) + 10));

							EngineDevMsg("[client dll] CHud::VidInit is located at %p.\n", ORIG_CHud_VidInit);
						}
						else
						{
							pHUD_Reset = nullptr; // Try with HUD_VidInit.
						}
					}

					if (!pHUD_Reset)
					{
						auto pHUD_VidInit = MemUtils::GetSymbolAddress(moduleHandle, "HUD_VidInit");
						if (pHUD_VidInit)
						{
							auto pHud_uintptr = reinterpret_cast<uintptr_t>(pHud);
							#define getbyte(a, n) (byte)((a >> n*8) & 0xFF)
							const byte ptn[] = { 0xB9, getbyte(pHud_uintptr, 0), getbyte(pHud_uintptr, 1), getbyte(pHud_uintptr, 2), getbyte(pHud_uintptr, 3), 0xE8 },
								ptn2[] = { 0x24, getbyte(pHud_uintptr, 0), getbyte(pHud_uintptr, 1), getbyte(pHud_uintptr, 2), getbyte(pHud_uintptr, 3), 0xE8 };
							#undef getbyte

							auto addr_ = MemUtils::FindPattern(pHUD_VidInit, 20, ptn, "xxxxxx");
							if (!addr_)
								addr_ = MemUtils::FindPattern(pHUD_VidInit, 20, ptn2, "xxxxxx");
							if (addr_)
							{
								ORIG_CHud_VidInit = reinterpret_cast<_CHud_InitFunc>(
									*reinterpret_cast<uintptr_t*>(reinterpret_cast<uintptr_t>(addr_) + 6)
									+ (reinterpret_cast<uintptr_t>(addr_) + 10));
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

		if (ORIG_CHud_Init || ORIG_HUD_Init)
		{
			if (!CHud_AddHudElem)
			{
				ptnNumber = fCHud_AddHudElem.get();
				if (ptnNumber != MemUtils::INVALID_SEQUENCE_INDEX)
				{
					CHud_AddHudElem = reinterpret_cast<_CHud_AddHudElem>(pCHud_AddHudElem);
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
	}

	ORIG_V_CalcRefdef = reinterpret_cast<_V_CalcRefdef>(MemUtils::GetSymbolAddress(moduleHandle, "V_CalcRefdef"));
	if (!ORIG_V_CalcRefdef)
	{
		EngineDevWarning("[client dll] Couldn't find V_CalcRefdef!\n");
		EngineWarning("Velocity display during demo playback is not available.\n");
	}
	
	// Now we can register cvars and commands provided that we already have engfuncs.
	if (pEngfuncs && *reinterpret_cast<uintptr_t*>(pEngfuncs))
		RegisterCVarsAndCommands();

	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_Initialize), reinterpret_cast<void*>(HOOKED_Initialize));
	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_HUD_Init), reinterpret_cast<void*>(HOOKED_HUD_Init));

	if (needToIntercept)
		MemUtils::Intercept(moduleName, {
			{ reinterpret_cast<void**>(&ORIG_PM_Jump), reinterpret_cast<void*>(HOOKED_PM_Jump) },
			{ reinterpret_cast<void**>(&ORIG_PM_PreventMegaBunnyJumping), reinterpret_cast<void*>(HOOKED_PM_PreventMegaBunnyJumping) },
			{ reinterpret_cast<void**>(&ORIG_Initialize), reinterpret_cast<void*>(HOOKED_Initialize) },
			{ reinterpret_cast<void**>(&ORIG_CHud_Init), reinterpret_cast<void*>(HOOKED_CHud_Init) },
			{ reinterpret_cast<void**>(&ORIG_CHud_VidInit), reinterpret_cast<void*>(HOOKED_CHud_VidInit) },
			{ reinterpret_cast<void**>(&ORIG_V_CalcRefdef), reinterpret_cast<void*>(HOOKED_V_CalcRefdef) },
			{ reinterpret_cast<void**>(&ORIG_HUD_Init), reinterpret_cast<void*>(HOOKED_HUD_Init) }
		});
}

void ClientDLL::Unhook()
{
	if (m_Intercepted)
		MemUtils::RemoveInterception(m_Name, {
			{ reinterpret_cast<void**>(&ORIG_PM_Jump), reinterpret_cast<void*>(HOOKED_PM_Jump) },
			{ reinterpret_cast<void**>(&ORIG_PM_PreventMegaBunnyJumping), reinterpret_cast<void*>(HOOKED_PM_PreventMegaBunnyJumping) },
			{ reinterpret_cast<void**>(&ORIG_Initialize), reinterpret_cast<void*>(HOOKED_Initialize) },
			{ reinterpret_cast<void**>(&ORIG_CHud_Init), reinterpret_cast<void*>(HOOKED_CHud_Init) },
			{ reinterpret_cast<void**>(&ORIG_CHud_VidInit), reinterpret_cast<void*>(HOOKED_CHud_VidInit) },
			{ reinterpret_cast<void**>(&ORIG_V_CalcRefdef), reinterpret_cast<void*>(HOOKED_V_CalcRefdef) },
			{ reinterpret_cast<void**>(&ORIG_HUD_Init), reinterpret_cast<void*>(HOOKED_HUD_Init) }
		});

	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_Initialize));
	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_HUD_Init));

	Clear();
}

void ClientDLL::Clear()
{
	IHookableNameFilter::Clear();
	ORIG_PM_Jump = nullptr;
	ORIG_PM_PlayerMove = nullptr;
	ORIG_PM_PreventMegaBunnyJumping = nullptr;
	ORIG_Initialize = nullptr;
	ORIG_CHud_Init = nullptr;
	ORIG_CHud_VidInit = nullptr;
	CHud_AddHudElem = nullptr;
	ORIG_V_CalcRefdef = nullptr;
	ORIG_HUD_Init = nullptr;
	ppmove = nullptr;
	offOldbuttons = 0;
	offOnground = 0;
	offBhopcap = 0;
	memset(originalBhopcapInsn, 0, sizeof(originalBhopcapInsn));
	pEngfuncs = nullptr;
	pHud = nullptr;
	cantJumpNextTime = false;
	novd = false;
	m_Intercepted = false;
}

void ClientDLL::RegisterCVarsAndCommands()
{
	if (!pEngfuncs || !*reinterpret_cast<uintptr_t*>(pEngfuncs))
		return;

	#define REG(name, str) pEngfuncs->pfnRegisterVariable(const_cast<char*>(name), const_cast<char*>(str), 0)

	if (ORIG_PM_Jump)
		bxt_autojump_prediction.Assign(REG("bxt_autojump_prediction", "0"));

	if (ORIG_PM_PreventMegaBunnyJumping)
		bxt_bhopcap_prediction.Assign(REG("bxt_bhopcap_prediction", "0"));

	if (ORIG_CHud_Init || ORIG_HUD_Init)
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

void ClientDLL::AddHudElem(void* pHudElem)
{
	if (pHud && CHud_AddHudElem)
	{
		#ifdef _WIN32
		CHud_AddHudElem(pHud, 0, pHudElem);
		#else
		CHud_AddHudElem(pHud, pHudElem);
		#endif
	}
}

void __cdecl ClientDLL::HOOKED_PM_Jump_Func()
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

void __cdecl ClientDLL::HOOKED_PM_PlayerMove_Func(qboolean server)
{
	ORIG_PM_PlayerMove(server);
}

void __cdecl ClientDLL::HOOKED_PM_PreventMegaBunnyJumping_Func()
{
	if (bxt_bhopcap_prediction.GetBool())
		ORIG_PM_PreventMegaBunnyJumping();
}

int __cdecl ClientDLL::HOOKED_Initialize_Func(cl_enginefunc_t* pEnginefuncs, int iVersion)
{
	int rv = ORIG_Initialize(pEnginefuncs, iVersion);

	RegisterCVarsAndCommands();

	return rv;
}

#ifdef _WIN32
void __fastcall ClientDLL::HOOKED_CHud_Init_Func(void* thisptr, int edx)
#else
void __cdecl ClientDLL::HOOKED_CHud_Init_Func(void* thisptr)
#endif
{
	#ifdef _WIN32
	ORIG_CHud_Init(thisptr, edx);
	#else
	ORIG_CHud_Init(thisptr);
	#endif

	if (novd)
		customHudWrapper_NoVD.Init();
	else
		customHudWrapper.Init();
}

#ifdef _WIN32
void __fastcall ClientDLL::HOOKED_CHud_VidInit_Func(void* thisptr, int edx)
#else
void __cdecl ClientDLL::HOOKED_CHud_VidInit_Func(void* thisptr)
#endif
{
	#ifdef _WIN32
	ORIG_CHud_VidInit(thisptr, edx);
	#else
	ORIG_CHud_VidInit(thisptr);
	#endif

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

void __cdecl ClientDLL::HOOKED_V_CalcRefdef_Func(ref_params_t* pparams)
{
	ORIG_V_CalcRefdef(pparams);

	CustomHud::UpdatePlayerInfoInaccurate(pparams->simvel, pparams->simorg);
}

void __cdecl ClientDLL::HOOKED_HUD_Init_Func()
{
	ORIG_HUD_Init();

	if (novd)
		customHudWrapper_NoVD.Init();
	else
		customHudWrapper.Init();
}
