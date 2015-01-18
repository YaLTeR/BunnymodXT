#include "../stdafx.hpp"

#include "../sptlib-wrapper.hpp"
#include <SPTLib/MemUtils.hpp>
#include <SPTLib/Hooks.hpp>
#include "ServerDLL.hpp"
#include "../patterns.hpp"
#include "../cvars.hpp"
#include "../hud_custom.hpp"

void ServerDLL::Hook(const std::wstring& moduleName, void* moduleHandle, void* moduleBase, size_t moduleLength, bool needToIntercept)
{
	Clear(); // Just in case.

	m_Handle = moduleHandle;
	m_Base = moduleBase;
	m_Length = moduleLength;
	m_Name = moduleName;
	m_Intercepted = needToIntercept;

	FindStuff();

	MemUtils::AddSymbolLookupHook(moduleHandle, reinterpret_cast<void*>(ORIG_GiveFnptrsToDll), reinterpret_cast<void*>(HOOKED_GiveFnptrsToDll));

	if (needToIntercept)
		MemUtils::Intercept(moduleName, {
			{ reinterpret_cast<void**>(&ORIG_PM_Jump), reinterpret_cast<void*>(HOOKED_PM_Jump) },
			{ reinterpret_cast<void**>(&ORIG_PM_PreventMegaBunnyJumping), reinterpret_cast<void*>(HOOKED_PM_PreventMegaBunnyJumping) },
			{ reinterpret_cast<void**>(&ORIG_PM_PlayerMove), reinterpret_cast<void*>(HOOKED_PM_PlayerMove) },
			{ reinterpret_cast<void**>(&ORIG_GiveFnptrsToDll), reinterpret_cast<void*>(HOOKED_GiveFnptrsToDll) }
		});
}

void ServerDLL::Unhook()
{
	if (m_Intercepted)
		MemUtils::RemoveInterception(m_Name, {
			{ reinterpret_cast<void**>(&ORIG_PM_Jump), reinterpret_cast<void*>(HOOKED_PM_Jump) },
			{ reinterpret_cast<void**>(&ORIG_PM_PreventMegaBunnyJumping), reinterpret_cast<void*>(HOOKED_PM_PreventMegaBunnyJumping) },
			{ reinterpret_cast<void**>(&ORIG_PM_PlayerMove), reinterpret_cast<void*>(HOOKED_PM_PlayerMove) },
			{ reinterpret_cast<void**>(&ORIG_GiveFnptrsToDll), reinterpret_cast<void*>(HOOKED_GiveFnptrsToDll) }
		});

	MemUtils::RemoveSymbolLookupHook(m_Handle, reinterpret_cast<void*>(ORIG_GiveFnptrsToDll));

	Clear();
}

void ServerDLL::Clear()
{
	IHookableDirFilter::Clear();
	ORIG_PM_Jump = nullptr;
	ORIG_PM_PreventMegaBunnyJumping = nullptr;
	ORIG_PM_PlayerMove = nullptr;
	ORIG_GiveFnptrsToDll = nullptr;
	ppmove = nullptr;
	offPlayerIndex = 0;
	offOldbuttons = 0;
	offOnground = 0;
	offVelocity = 0;
	offOrigin = 0;
	offAngles = 0;
	offCmd = 0;
	offBhopcap = 0;
	memset(originalBhopcapInsn, 0, sizeof(originalBhopcapInsn));
	pEngfuncs = nullptr;
	cantJumpNextTime.clear();
	m_Intercepted = false;
}

bool ServerDLL::CanHook(const std::wstring& moduleFullName)
{
	if (!IHookableDirFilter::CanHook(moduleFullName))
		return false;

	// Filter out addons like metamod which may be located into a "dlls" folder under addons.
	std::wstring pathToLiblist = moduleFullName.substr(0, moduleFullName.rfind(GetFolderName(moduleFullName))).append(L"liblist.gam");

	// If liblist.gam exists in the parent directory, then we're (hopefully) good.
	struct wrapper {
		wrapper(FILE* f) : file(f) {};
		~wrapper() {
			if (file)
				fclose(file);
		}
		operator FILE*() const
		{
			return file;
		}

		FILE* file;
	} liblist(fopen(Convert(pathToLiblist).c_str(), "r"));

	if (liblist)
		return true;

	return false;
}

void ServerDLL::FindStuff()
{
	auto fPM_PreventMegaBunnyJumping = MemUtils::Find(reinterpret_cast<void**>(&ORIG_PM_PreventMegaBunnyJumping), m_Handle, "PM_PreventMegaBunnyJumping", m_Base, m_Length, Patterns::ptnsPMPreventMegaBunnyJumping,
		[](MemUtils::ptnvec_size ptnNumber) { }, []() { }
	);

	auto fPM_PlayerMove = MemUtils::Find(reinterpret_cast<void**>(&ORIG_PM_PlayerMove), m_Handle, "PM_PlayerMove", m_Base, m_Length, Patterns::ptnsPMPlayerMove,
		[&](MemUtils::ptnvec_size ptnNumber) {
			offPlayerIndex = 0;
			offVelocity = 92;
			offOrigin = 56;
			offAngles = 68;
			offCmd = 283736;
		}, []() { }
	);

	auto fPM_Jump = MemUtils::Find(reinterpret_cast<void**>(&ORIG_PM_Jump), m_Handle, "PM_Jump", m_Base, m_Length, Patterns::ptnsPMJump,
		[&](MemUtils::ptnvec_size ptnNumber) {
			offPlayerIndex = 0;
			offOldbuttons = 200;
			offOnground = 224;
			if (ptnNumber == MemUtils::INVALID_SEQUENCE_INDEX) // Linux.
			{
				ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(ORIG_PM_Jump) + 1);
				void *bhopcapAddr;
				auto n = MemUtils::FindUniqueSequence(m_Base, m_Length, Patterns::ptnsBhopcap, &bhopcapAddr);
				if (n != MemUtils::INVALID_SEQUENCE_INDEX)
				{
					offBhopcap = reinterpret_cast<ptrdiff_t>(bhopcapAddr) - reinterpret_cast<ptrdiff_t>(ORIG_PM_Jump) + 27;
					memcpy(originalBhopcapInsn, reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(bhopcapAddr) + 27), sizeof(originalBhopcapInsn));
				}
			}
			else
			{
				switch (ptnNumber)
				{
				case 0:
				case 1:
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(ORIG_PM_Jump) + 2);
					break;

				case 2:
				case 3: // AG-Client, shouldn't happen here but who knows.
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(ORIG_PM_Jump) + 3);
					break;
				}
			}
		}, []() { }
	);

	bool noBhopcap = false;
	auto n = fPM_PreventMegaBunnyJumping.get();
	if (ORIG_PM_PreventMegaBunnyJumping) {
		if (n == MemUtils::INVALID_SEQUENCE_INDEX)
			EngineDevMsg("[server dll] Found PM_PreventMegaBunnyJumping at %p.\n", ORIG_PM_PreventMegaBunnyJumping);
		else
			EngineDevMsg("[server dll] Found PM_PreventMegaBunnyJumping at %p (using the %s pattern).\n", ORIG_PM_PreventMegaBunnyJumping, Patterns::ptnsPMPreventMegaBunnyJumping[n].build.c_str());
	} else {
		EngineDevWarning("[server dll] Could not find PM_PreventMegaBunnyJumping.\n");
		EngineWarning("Bhopcap disabling is not available.\n");
		noBhopcap = true;
	}

	n = fPM_PlayerMove.get();
	if (ORIG_PM_PlayerMove) {
		if (n == MemUtils::INVALID_SEQUENCE_INDEX)
			EngineDevMsg("[server dll] Found PM_PlayerMove at %p.\n", ORIG_PM_PlayerMove);
		else
			EngineDevMsg("[server dll] Found PM_PlayerMove at %p (using the %s pattern).\n", ORIG_PM_PlayerMove, Patterns::ptnsPMPlayerMove[n].build.c_str());
	} else
		EngineDevWarning("[server dll] Could not find PM_PlayerMove.\n");

	n = fPM_Jump.get();
	if (ORIG_PM_Jump) {
		if (n == MemUtils::INVALID_SEQUENCE_INDEX)
			EngineDevMsg("[server dll] Found PM_Jump at %p.\n", ORIG_PM_Jump);
		else
			EngineDevMsg("[server dll] Found PM_Jump at %p (using the %s pattern).\n", ORIG_PM_Jump, Patterns::ptnsPMJump[n].build.c_str());
		if (offBhopcap)
			EngineDevMsg("[server dll] Found the bhopcap pattern at %p.\n", reinterpret_cast<void*>(offBhopcap + reinterpret_cast<uintptr_t>(ORIG_PM_Jump) - 27));
	} else {
		EngineDevWarning("[server dll] Could not find PM_Jump.\n");
		EngineWarning("Autojump is not available.\n");
		if (!noBhopcap)
			EngineWarning("Bhopcap disabling is not available.\n");
	}
	
	// This has to be the last thing to check and hook.
	pEngfuncs = reinterpret_cast<enginefuncs_t*>(MemUtils::GetSymbolAddress(m_Handle, "g_engfuncs"));
	if (pEngfuncs)
	{
		EngineDevMsg("[server dll] pEngfuncs is %p.\n", pEngfuncs);
		if (*reinterpret_cast<uintptr_t*>(pEngfuncs))
			RegisterCVarsAndCommands();
		else
		{
			ORIG_GiveFnptrsToDll = reinterpret_cast<_GiveFnptrsToDll>(MemUtils::GetSymbolAddress(m_Handle, "GiveFnptrsToDll"));
			if (!ORIG_GiveFnptrsToDll)
			{
				EngineDevWarning("[server dll] Couldn't get the address of GiveFnptrsToDll.\n");
				EngineWarning("Serverside CVars and commands are not available.\n");
			}
		}
	}
	else
	{
		_GiveFnptrsToDll pGiveFnptrsToDll = reinterpret_cast<_GiveFnptrsToDll>(MemUtils::GetSymbolAddress(m_Handle, "GiveFnptrsToDll"));
		if (pGiveFnptrsToDll)
		{
			// Find "mov edi, offset dword; rep movsd" inside GiveFnptrsToDll. The pointer to g_engfuncs is that dword.
			const byte pattern[] = { 0xBF, '?', '?', '?', '?', 0xF3, 0xA5 };
			auto addr = MemUtils::FindPattern(reinterpret_cast<void*>(pGiveFnptrsToDll), 40, pattern, "x????xx");

			// Linux version: mov offset dword[eax], esi; mov [ecx+eax+4], ebx
			if (!addr)
			{
				const byte pattern_[] = { 0x89, 0xB0, '?', '?', '?', '?', 0x89, 0x5C, 0x01, 0x04 };
				addr = MemUtils::FindPattern(reinterpret_cast<void*>(pGiveFnptrsToDll), 40, pattern_, "xx????xxxx");
				if (addr)
					addr = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(addr)+1); // So we're compatible with the previous pattern.
			}

			if (addr)
			{
				pEngfuncs = *reinterpret_cast<enginefuncs_t**>(reinterpret_cast<uintptr_t>(addr)+1);
				EngineDevMsg("[server dll] pEngfuncs is %p.\n", pEngfuncs);

				// If we have engfuncs, do stuff right away. Otherwise wait till the engine gives us engfuncs.
				if (*reinterpret_cast<uintptr_t*>(pEngfuncs))
					RegisterCVarsAndCommands();
				else
					ORIG_GiveFnptrsToDll = pGiveFnptrsToDll;
			}
			else
			{
				EngineDevWarning("[server dll] Couldn't find the pattern in GiveFnptrsToDll.\n");
				EngineWarning("Serverside CVars and commands are not available.\n");
			}
		}
		else
		{
			EngineDevWarning("[server dll] Couldn't get the address of GiveFnptrsToDll.\n");
			EngineWarning("Serverside CVars and commands are not available.\n");
		}
	}
}

void ServerDLL::RegisterCVarsAndCommands()
{
	if (!pEngfuncs || !*reinterpret_cast<uintptr_t*>(pEngfuncs))
		return;

	if (ORIG_PM_Jump)
		pEngfuncs->pfnCVarRegister(bxt_autojump.GetPointer());

	if (ORIG_PM_PreventMegaBunnyJumping)
		pEngfuncs->pfnCVarRegister(bxt_bhopcap.GetPointer());

	EngineDevMsg("[server dll] Registered CVars.\n");
}

HOOK_DEF_0(ServerDLL, void, __cdecl, PM_Jump)
{
	auto pmove = reinterpret_cast<uintptr_t>(*ppmove);
	int playerIndex = *reinterpret_cast<int*>(pmove + offPlayerIndex);

	int *onground = reinterpret_cast<int*>(pmove + offOnground);
	int orig_onground = *onground;

	int *oldbuttons = reinterpret_cast<int*>(pmove + offOldbuttons);
	int orig_oldbuttons = *oldbuttons;

	if (bxt_autojump.GetBool())
	{
		if ((orig_onground != -1) && !cantJumpNextTime[playerIndex])
			*oldbuttons &= ~IN_JUMP;
	}

	cantJumpNextTime[playerIndex] = false;

	if (offBhopcap)
	{
		auto pPMJump = reinterpret_cast<ptrdiff_t>(ORIG_PM_Jump);
		if (bxt_bhopcap.GetBool())
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
		cantJumpNextTime[playerIndex] = true;

	if (bxt_autojump.GetBool())
		*oldbuttons = orig_oldbuttons;
}

HOOK_DEF_0(ServerDLL, void, __cdecl, PM_PreventMegaBunnyJumping)
{
	if (bxt_bhopcap.GetBool())
		return ORIG_PM_PreventMegaBunnyJumping();
}

HOOK_DEF_1(ServerDLL, void, __cdecl, PM_PlayerMove, qboolean, server)
{
	if (!ppmove)
		return ORIG_PM_PlayerMove(server);

	auto pmove = reinterpret_cast<uintptr_t>(*ppmove);

	int playerIndex = *reinterpret_cast<int*>(pmove + offPlayerIndex);

	float *velocity, *origin, *angles;
	velocity = reinterpret_cast<float*>(pmove + offVelocity);
	origin =   reinterpret_cast<float*>(pmove + offOrigin);
	angles =   reinterpret_cast<float*>(pmove + offAngles);
	usercmd_t *cmd = reinterpret_cast<usercmd_t*>(pmove + offCmd);

	#define ALERT(at, format, ...) pEngfuncs->pfnAlertMessage(at, const_cast<char*>(format), ##__VA_ARGS__)

	//if (_bxt_taslog.GetBool())
	{
		ALERT(at_console, "-- BXT TAS Log Start --\n");
		ALERT(at_console, "Player index: %d; msec: %hhu (%Lf)\n", playerIndex, cmd->msec, static_cast<long double>(cmd->msec) * 0.001);
		ALERT(at_console, "Velocity: %.8f; %.8f; %.8f; origin: %.8f; %.8f; %.8f\n", velocity[0], velocity[1], velocity[2], origin[0], origin[1], origin[2]);
	}

	ORIG_PM_PlayerMove(server);

	//if (_bxt_taslog.GetBool())
	{
		ALERT(at_console, "Angles: %.8f; %.8f; %.8f\n", angles[0], angles[1], angles[2]);
		ALERT(at_console, "New velocity: %.8f; %.8f; %.8f; new origin: %.8f; %.8f; %.8f\n", velocity[0], velocity[1], velocity[2], origin[0], origin[1], origin[2]);
		ALERT(at_console, "-- BXT TAS Log End --\n");
	}

	#undef ALERT

	CustomHud::UpdatePlayerInfo(velocity, origin);
}

HOOK_DEF_2(ServerDLL, void, __stdcall, GiveFnptrsToDll, enginefuncs_t*, pEngfuncsFromEngine, const void*, pGlobals)
{
	ORIG_GiveFnptrsToDll(pEngfuncsFromEngine, pGlobals);

	RegisterCVarsAndCommands();
}
