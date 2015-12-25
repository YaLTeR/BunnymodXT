#include "../stdafx.hpp"

#include "../sptlib-wrapper.hpp"
#include <SPTLib/MemUtils.hpp>
#include <SPTLib/Hooks.hpp>
#include "ServerDLL.hpp"
#include "ClientDLL.hpp"
#include "HwDLL.hpp"
#include "../patterns.hpp"
#include "../cvars.hpp"
#include "../hud_custom.hpp"
#include "../interprocess.hpp"

// Linux hooks.
#ifndef _WIN32
extern "C" void __cdecl _Z8CmdStartPK7edict_sPK9usercmd_sj(const edict_t* player, const usercmd_t* cmd, unsigned int random_seed)
{
	return ServerDLL::HOOKED_CmdStart(player, cmd, random_seed);
}

extern "C" void __cdecl _ZN10CNihilanth10DyingThinkEv(void* thisptr)
{
	return ServerDLL::HOOKED_CNihilanth__DyingThink_Linux(thisptr);
}

extern "C" void __cdecl _ZN11COFGeneWorm10DyingThinkEv(void* thisptr)
{
	return ServerDLL::HOOKED_COFGeneWorm__DyingThink_Linux(thisptr);
}

extern "C" void __cdecl _ZN13CMultiManager10ManagerUseEP11CBaseEntityS1_8USE_TYPEf(void* thisptr, void* pActivator, void* pCaller, int useType, float value)
{
	return ServerDLL::HOOKED_CMultiManager__ManagerUse_Linux(thisptr, pActivator, pCaller, useType, value);
}

extern "C" int __cdecl _Z13AddToFullPackP14entity_state_siP7edict_sS2_iiPh(struct entity_state_s* state, int e, edict_t* ent, edict_t* host, int hostflags, int player, unsigned char* pSet)
{
	return ServerDLL::HOOKED_AddToFullPack(state, e, ent, host, hostflags, player, pSet);
}

extern "C" void __cdecl _ZN14CTriggerVolume5SpawnEv(void* thisptr)
{
	return ServerDLL::HOOKED_CTriggerVolume__Spawn_Linux(thisptr);
}

extern "C" void __cdecl _Z13ClientCommandP7edict_s(edict_t* pEntity)
{
	return ServerDLL::HOOKED_ClientCommand(pEntity);
}
#endif

void ServerDLL::Hook(const std::wstring& moduleName, void* moduleHandle, void* moduleBase, size_t moduleLength, bool needToIntercept)
{
	Clear(); // Just in case.

	m_Handle = moduleHandle;
	m_Base = moduleBase;
	m_Length = moduleLength;
	m_Name = moduleName;
	m_Intercepted = needToIntercept;

	FindStuff();
	RegisterCVarsAndCommands();

	if (needToIntercept)
		MemUtils::Intercept(moduleName, {
			{ reinterpret_cast<void**>(&ORIG_PM_Jump), reinterpret_cast<void*>(HOOKED_PM_Jump) },
			{ reinterpret_cast<void**>(&ORIG_PM_PreventMegaBunnyJumping), reinterpret_cast<void*>(HOOKED_PM_PreventMegaBunnyJumping) },
			{ reinterpret_cast<void**>(&ORIG_PM_PlayerMove), reinterpret_cast<void*>(HOOKED_PM_PlayerMove) },
			{ reinterpret_cast<void**>(&ORIG_PM_ClipVelocity), reinterpret_cast<void*>(HOOKED_PM_ClipVelocity) },
			{ reinterpret_cast<void**>(&ORIG_PM_WaterMove), reinterpret_cast<void*>(HOOKED_PM_WaterMove) },
			{ reinterpret_cast<void**>(&ORIG_CmdStart), reinterpret_cast<void*>(HOOKED_CmdStart) },
			{ reinterpret_cast<void**>(&ORIG_CNihilanth__DyingThink), reinterpret_cast<void*>(HOOKED_CNihilanth__DyingThink) },
			{ reinterpret_cast<void**>(&ORIG_COFGeneWorm__DyingThink), reinterpret_cast<void*>(HOOKED_COFGeneWorm__DyingThink) },
			{ reinterpret_cast<void**>(&ORIG_CMultiManager__ManagerThink), reinterpret_cast<void*>(HOOKED_CMultiManager__ManagerThink) },
			{ reinterpret_cast<void**>(&ORIG_AddToFullPack), reinterpret_cast<void*>(HOOKED_AddToFullPack) },
			{ reinterpret_cast<void**>(&ORIG_CTriggerVolume__Spawn), reinterpret_cast<void*>(HOOKED_CTriggerVolume__Spawn) },
			{ reinterpret_cast<void**>(&ORIG_ClientCommand), reinterpret_cast<void*>(HOOKED_ClientCommand) }
		});
}

void ServerDLL::Unhook()
{
	if (m_Intercepted)
		MemUtils::RemoveInterception(m_Name, {
			{ reinterpret_cast<void**>(&ORIG_PM_Jump), reinterpret_cast<void*>(HOOKED_PM_Jump) },
			{ reinterpret_cast<void**>(&ORIG_PM_PreventMegaBunnyJumping), reinterpret_cast<void*>(HOOKED_PM_PreventMegaBunnyJumping) },
			{ reinterpret_cast<void**>(&ORIG_PM_PlayerMove), reinterpret_cast<void*>(HOOKED_PM_PlayerMove) },
			{ reinterpret_cast<void**>(&ORIG_PM_ClipVelocity), reinterpret_cast<void*>(HOOKED_PM_ClipVelocity) },
			{ reinterpret_cast<void**>(&ORIG_PM_WaterMove), reinterpret_cast<void*>(HOOKED_PM_WaterMove) },
			{ reinterpret_cast<void**>(&ORIG_CmdStart), reinterpret_cast<void*>(HOOKED_CmdStart) },
			{ reinterpret_cast<void**>(&ORIG_CNihilanth__DyingThink), reinterpret_cast<void*>(HOOKED_CNihilanth__DyingThink) },
			{ reinterpret_cast<void**>(&ORIG_COFGeneWorm__DyingThink), reinterpret_cast<void*>(HOOKED_COFGeneWorm__DyingThink) },
			{ reinterpret_cast<void**>(&ORIG_CMultiManager__ManagerThink), reinterpret_cast<void*>(HOOKED_CMultiManager__ManagerThink) },
			{ reinterpret_cast<void**>(&ORIG_AddToFullPack), reinterpret_cast<void*>(HOOKED_AddToFullPack) },
			{ reinterpret_cast<void**>(&ORIG_CTriggerVolume__Spawn), reinterpret_cast<void*>(HOOKED_CTriggerVolume__Spawn) },
			{ reinterpret_cast<void**>(&ORIG_ClientCommand), reinterpret_cast<void*>(HOOKED_ClientCommand) }
		});

	Clear();
}

void ServerDLL::Clear()
{
	IHookableDirFilter::Clear();
	ORIG_PM_Jump = nullptr;
	ORIG_PM_PreventMegaBunnyJumping = nullptr;
	ORIG_PM_PlayerMove = nullptr;
	ORIG_PM_ClipVelocity = nullptr;
	ORIG_PM_WaterMove = nullptr;
	ORIG_CmdStart = nullptr;
	ORIG_CNihilanth__DyingThink = nullptr;
	ORIG_CNihilanth__DyingThink_Linux = nullptr;
	ORIG_COFGeneWorm__DyingThink = nullptr;
	ORIG_COFGeneWorm__DyingThink_Linux = nullptr;
	ORIG_CMultiManager__ManagerThink = nullptr;
	ORIG_CMultiManager__ManagerUse_Linux = nullptr;
	ORIG_AddToFullPack = nullptr;
	ORIG_CTriggerVolume__Spawn = nullptr;
	ORIG_CTriggerVolume__Spawn_Linux = nullptr;
	ORIG_CBasePlayer__ForceClientDllUpdate = nullptr;
	ORIG_CBasePlayer__ForceClientDllUpdate_Linux = nullptr;
	ORIG_ClientCommand = nullptr;
	ORIG_GetEntityAPI = nullptr;
	ppmove = nullptr;
	offPlayerIndex = 0;
	offOldbuttons = 0;
	offOnground = 0;
	offVelocity = 0;
	offOrigin = 0;
	offAngles = 0;
	offCmd = 0;
	offBhopcap = 0;
	offm_iClientFOV = 0;
	offm_rgAmmoLast = 0;
	maxAmmoSlots = 0;
	memset(originalBhopcapInsn, 0, sizeof(originalBhopcapInsn));
	pEngfuncs = nullptr;
	ppGlobals = nullptr;
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

				case 4:
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(ORIG_PM_Jump) + 21);
					break;
				}
			}
		}, []() { }
	);

	auto fPM_ClipVelocity = MemUtils::Find(reinterpret_cast<void**>(&ORIG_PM_ClipVelocity), m_Handle, "PM_ClipVelocity", m_Base, m_Length, Patterns::ptnsPM_ClipVelocity,
		[](MemUtils::ptnvec_size ptnNumber) { }, []() { }
	);

	auto fPM_WaterMove = MemUtils::Find(reinterpret_cast<void**>(&ORIG_PM_WaterMove), m_Handle, "PM_WaterMove", m_Base, m_Length, Patterns::ptnsPM_WaterMove,
		[](MemUtils::ptnvec_size ptnNumber) {}, []() {}
	);

	auto fCTriggerVolume__Spawn = MemUtils::FindPatternOnly(reinterpret_cast<void**>(&ORIG_CTriggerVolume__Spawn), m_Base, m_Length,
		Patterns::ptnsCTriggerVolume__Spawn, [](MemUtils::ptnvec_size ptnNumber) { }, []() { }
	);

	auto fCBasePlayer__ForceClientDllUpdate = MemUtils::FindPatternOnly(reinterpret_cast<void**>(&ORIG_CBasePlayer__ForceClientDllUpdate), m_Base, m_Length, Patterns::ptnsCBasePlayer__ForceClientDllUpdate,
		[&](MemUtils::ptnvec_size ptnNumber) {
			switch (ptnNumber) {
			case 0:  // HL-SteamPipe
				maxAmmoSlots = MAX_AMMO_SLOTS;
				offm_rgAmmoLast = 0x554;
				offm_iClientFOV = 0x4AC;
				break;
			case 1:  // OpposingForce
				maxAmmoSlots = MAX_AMMO_SLOTS;
				offm_rgAmmoLast = 0x604;
				offm_iClientFOV = 0x4E0;
				break;
			case 2:  // HazardousCourse2
				maxAmmoSlots = MAX_AMMO_SLOTS;
				offm_rgAmmoLast = 0x540;
				offm_iClientFOV = 0x498;
				break;
			case 3:  // Gunman
				maxAmmoSlots = MAX_AMMO_SLOTS;
				offm_rgAmmoLast = 0x53C;
				offm_iClientFOV = 0x47C;
				break;
			default:
				assert(false);
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

	n = fCTriggerVolume__Spawn.get();
	if (ORIG_CTriggerVolume__Spawn) {
		EngineDevMsg("[server dll] Found CTriggerVolume::Spawn at %p (using the %s pattern).\n", ORIG_CTriggerVolume__Spawn, Patterns::ptnsCTriggerVolume__Spawn[n].build.c_str());
	} else {
		ORIG_CTriggerVolume__Spawn_Linux = reinterpret_cast<_CTriggerVolume__Spawn_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN14CTriggerVolume5SpawnEv"));
		if (ORIG_CTriggerVolume__Spawn_Linux) {
			EngineDevMsg("[server dll] Found CTriggerVolume::Spawn [Linux] at %p.\n", ORIG_CTriggerVolume__Spawn_Linux);
		} else {
			EngineDevWarning("[server dll] Could not find CTriggerVolume::Spawn.\n");
			EngineWarning("trigger_transition entities will not be displayed.\n");
		}
	}

	n = fCBasePlayer__ForceClientDllUpdate.get();
	if (ORIG_CBasePlayer__ForceClientDllUpdate) {
		EngineDevMsg("[server dll] Found CBasePlayer::ForceClientDllUpdate at %p (using the %s pattern).\n", ORIG_CBasePlayer__ForceClientDllUpdate, Patterns::ptnsCBasePlayer__ForceClientDllUpdate[n].build.c_str());
	} else {
		ORIG_CBasePlayer__ForceClientDllUpdate_Linux = reinterpret_cast<_CBasePlayer__ForceClientDllUpdate_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN11CBasePlayer20ForceClientDllUpdateEv"));
		if (ORIG_CBasePlayer__ForceClientDllUpdate_Linux) {
			maxAmmoSlots = MAX_AMMO_SLOTS;
			offm_rgAmmoLast = 0x568;
			offm_iClientFOV = 0x4C0;
			EngineDevMsg("[server dll] Found CBasePlayer::ForceClientDllUpdate [Linux] at %p.\n", ORIG_CBasePlayer__ForceClientDllUpdate_Linux);
		} else {
			EngineDevWarning("[server dll] Could not find CBasePlayer::ForceClientDllUpdate.\n");
			EngineWarning("Ammo HUD reset prevention is not available.\n");
		}
	}

	ORIG_CmdStart = reinterpret_cast<_CmdStart>(MemUtils::GetSymbolAddress(m_Handle, "_Z8CmdStartPK7edict_sPK9usercmd_sj"));
	ORIG_AddToFullPack = reinterpret_cast<_AddToFullPack>(MemUtils::GetSymbolAddress(m_Handle, "_Z13AddToFullPackP14entity_state_siP7edict_sS2_iiPh"));
	ORIG_ClientCommand = reinterpret_cast<_ClientCommand>(MemUtils::GetSymbolAddress(m_Handle, "_Z13ClientCommandP7edict_s"));
	if (ORIG_CmdStart && ORIG_AddToFullPack && ORIG_ClientCommand) {
		EngineDevMsg("[server dll] Found CmdStart at %p.\n", ORIG_CmdStart);
		EngineDevMsg("[server dll] Found AddToFullPack at %p.\n", ORIG_AddToFullPack);
		EngineDevMsg("[server dll] Found ClientCommand at %p.\n", ORIG_ClientCommand);
	} else {
		ORIG_GetEntityAPI = reinterpret_cast<_GetEntityAPI>(MemUtils::GetSymbolAddress(m_Handle, "GetEntityAPI"));
		if (ORIG_GetEntityAPI) {
			DLL_FUNCTIONS funcs;
			if (ORIG_GetEntityAPI(&funcs, 140)) {
				// Gets our hooked addresses on Linux.
				ORIG_CmdStart = funcs.pfnCmdStart;
				ORIG_AddToFullPack = funcs.pfnAddToFullPack;
				ORIG_ClientCommand = funcs.pfnClientCommand;
				EngineDevMsg("[server dll] Found CmdStart at %p.\n", ORIG_CmdStart);
				EngineDevMsg("[server dll] Found AddToFullPack at %p.\n", ORIG_AddToFullPack);
				EngineDevMsg("[server dll] Found ClientCommand at %p.\n", ORIG_ClientCommand);
			} else {
				EngineDevWarning("[server dll] Could not get the server DLL function table.\n");
				EngineWarning("Serverside shared RNG manipulation and usercommand logging are not available.\n");
				EngineWarning("Showing hidden entities is not available.\n");
			}
		} else {
			EngineDevWarning("[server dll] Could not get the address of GetEntityAPI.\n");
			EngineWarning("Serverside shared RNG manipulation and usercommand logging are not available.\n");
			EngineWarning("Showing hidden entities is not available.\n");
		}
	}

	ORIG_CNihilanth__DyingThink = reinterpret_cast<_CNihilanth__DyingThink>(MemUtils::GetSymbolAddress(m_Handle, "?DyingThink@CNihilanth@@QAEXXZ"));
	if (ORIG_CNihilanth__DyingThink)
		EngineDevMsg("[server dll] Found CNihilanth::DyingThink at %p.\n", ORIG_CNihilanth__DyingThink);
	else {
		ORIG_CNihilanth__DyingThink_Linux = reinterpret_cast<_CNihilanth__DyingThink_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN10CNihilanth10DyingThinkEv"));
		if (ORIG_CNihilanth__DyingThink_Linux)
			EngineDevMsg("[server dll] Found CNihilanth::DyingThink [Linux] at %p.\n", ORIG_CNihilanth__DyingThink_Linux);
		else {
			EngineDevWarning("[server dll] Could not find CNihilanth::DyingThink.\n");
			EngineWarning("Nihilanth automatic timer stopping is not available.\n");
		}
	}

	ORIG_COFGeneWorm__DyingThink = reinterpret_cast<_COFGeneWorm__DyingThink>(MemUtils::GetSymbolAddress(m_Handle, "?DyingThink@COFGeneWorm@@QAEXXZ"));
	if (ORIG_COFGeneWorm__DyingThink)
		EngineDevMsg("[server dll] Found COFGeneWorm::DyingThink at %p.\n", ORIG_COFGeneWorm__DyingThink);
	else {
		ORIG_COFGeneWorm__DyingThink_Linux = reinterpret_cast<_COFGeneWorm__DyingThink_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN11COFGeneWorm10DyingThinkEv"));
		if (ORIG_COFGeneWorm__DyingThink_Linux)
			EngineDevMsg("[server dll] Found COFGeneWorm::DyingThink [Linux] at %p.\n", ORIG_COFGeneWorm__DyingThink_Linux);
		else {
			EngineDevWarning("[server dll] Could not find COFGeneWorm::DyingThink.\n");
			EngineWarning("Gene Worm automatic timer stopping is not available.\n");
		}
	}

	ORIG_CMultiManager__ManagerThink = reinterpret_cast<_CMultiManager__ManagerThink>(MemUtils::GetSymbolAddress(m_Handle, "?ManagerThink@CMultiManager@@QAEXXZ"));
	if (ORIG_CMultiManager__ManagerThink)
		EngineDevMsg("[server dll] Found CMultiManager::ManagerThink at %p.\n", ORIG_CMultiManager__ManagerThink);
	else {
		ORIG_CMultiManager__ManagerUse_Linux = reinterpret_cast<_CMultiManager__ManagerUse_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN13CMultiManager10ManagerUseEP11CBaseEntityS1_8USE_TYPEf"));
		if (ORIG_CMultiManager__ManagerUse_Linux)
			EngineDevMsg("[server dll] Found CMultiManager::ManagerUse [Linux] at %p.\n", ORIG_CMultiManager__ManagerUse_Linux);
		else {
			EngineDevWarning("[server dll] Could not find CMultiManager::ManagerThink or CMultiManager::ManagerUse.\n");
			EngineWarning("Blue Shift and Gunman Chronicles automatic timer stopping is not available.\n");
		}
	}

	n = fPM_ClipVelocity.get();
	if (ORIG_PM_ClipVelocity) {
		if (n == MemUtils::INVALID_SEQUENCE_INDEX)
			EngineDevMsg("[server dll] Found PM_ClipVelocity at %p.\n", ORIG_PM_ClipVelocity);
		else
			EngineDevMsg("[server dll] Found PM_ClipVelocity at %p (using the %s pattern).\n", ORIG_PM_ClipVelocity, Patterns::ptnsPM_ClipVelocity[n].build.c_str());
	} else {
		EngineDevWarning("[server dll] Could not find PM_ClipVelocity.\n");
		EngineWarning("Velocity clip logging is not available.\n");
	}

	n = fPM_WaterMove.get();
	if (ORIG_PM_WaterMove) {
		if (n == MemUtils::INVALID_SEQUENCE_INDEX)
			EngineDevMsg("[server dll] Found PM_WaterMove at %p.\n", ORIG_PM_WaterMove);
		else
			EngineDevMsg("[server dll] Found PM_WaterMove at %p (using the %s pattern).\n", ORIG_PM_WaterMove, Patterns::ptnsPM_WaterMove[n].build.c_str());
	} else {
		EngineDevWarning("[server dll] Could not find PM_WaterMove.\n");
		EngineWarning("Water frame logging is not available.\n");
	}
	
	// This has to be the last thing to check and hook.
	pEngfuncs = reinterpret_cast<enginefuncs_t*>(MemUtils::GetSymbolAddress(m_Handle, "g_engfuncs"));
	ppGlobals = reinterpret_cast<globalvars_t**>(MemUtils::GetSymbolAddress(m_Handle, "gpGlobals"));
	if (pEngfuncs && ppGlobals) {
		EngineDevMsg("[server dll] pEngfuncs is %p.\n", pEngfuncs);
		EngineDevMsg("[server dll] ppGlobals is %p.\n", ppGlobals);
	} else {
		auto pGiveFnptrsToDll = MemUtils::GetSymbolAddress(m_Handle, "GiveFnptrsToDll");
		if (pGiveFnptrsToDll)
		{
			// Find "mov edi, offset dword; rep movsd" inside GiveFnptrsToDll. The pointer to g_engfuncs is that dword.
			const byte pattern[] = { 0xBF, '?', '?', '?', '?', 0xF3, 0xA5 };
			auto addr = MemUtils::FindPattern(pGiveFnptrsToDll, 40, pattern, "x????xx");

			// Big Lolly version: push eax; push offset dword; call memcpy
			auto blolly = false;
			if (!addr)
			{
				const byte pattern_[] = { 0x50, 0x68, '?', '?', '?', '?', 0xE8 };
				addr = MemUtils::FindPattern(pGiveFnptrsToDll, 40, pattern_, "xx????x");
				if (addr)
					blolly = true;
			}

			// Linux version: mov offset dword[eax], esi; mov [ecx+eax+4], ebx
			// if (!addr)
			// {
			// 	const byte pattern_[] = { 0x89, 0xB0, '?', '?', '?', '?', 0x89, 0x5C, 0x01, 0x04 };
			// 	addr = MemUtils::FindPattern(pGiveFnptrsToDll, 40, pattern_, "xx????xxxx");
			// 	if (addr)
			// 		addr = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(addr)+1); // So we're compatible with the previous pattern.
			// }

			if (addr)
			{
				if (blolly)
				{
					pEngfuncs = *reinterpret_cast<enginefuncs_t**>(reinterpret_cast<uintptr_t>(addr) + 2);
					ppGlobals = *reinterpret_cast<globalvars_t***>(reinterpret_cast<uintptr_t>(addr) + 19);
				}
				else
				{
					pEngfuncs = *reinterpret_cast<enginefuncs_t**>(reinterpret_cast<uintptr_t>(addr) + 1);
					ppGlobals = *reinterpret_cast<globalvars_t***>(reinterpret_cast<uintptr_t>(addr) + 9);
				}

				EngineDevMsg("[server dll] pEngfuncs is %p.\n", pEngfuncs);
				EngineDevMsg("[server dll] ppGlobals is %p.\n", ppGlobals);
			}
			else
			{
				EngineDevWarning("[server dll] Couldn't find the pattern in GiveFnptrsToDll.\n");
				EngineWarning("Serverside logging is not available.\n");
				EngineWarning("Blue Shift and Gunman Chronicles automatic timer stopping is not available.\n");
			}
		}
		else
		{
			EngineDevWarning("[server dll] Couldn't get the address of GiveFnptrsToDll.\n");
			EngineWarning("Serverside logging is not avaliable.\n");
			EngineWarning("Blue Shift and Gunman Chronicles automatic timer stopping is not available.\n");
		}
	}
}

void ServerDLL::RegisterCVarsAndCommands()
{
	EngineDevMsg("[server dll] Registering CVars.\n");

	#define REG(cvar) HwDLL::GetInstance().RegisterCVar(CVars::cvar)
	if (ORIG_PM_Jump)
		REG(bxt_autojump);
	if (!ORIG_PM_PreventMegaBunnyJumping)
		CVars::bxt_bhopcap.Set("0");
	REG(bxt_bhopcap);
	if (ORIG_CNihilanth__DyingThink || ORIG_CNihilanth__DyingThink_Linux || ORIG_COFGeneWorm__DyingThink || ORIG_COFGeneWorm__DyingThink_Linux)
		REG(bxt_timer_autostop);
	if (ORIG_AddToFullPack) {
		REG(bxt_show_hidden_entities);
		REG(bxt_show_triggers);
	}
	#undef REG
}

HOOK_DEF_0(ServerDLL, void, __cdecl, PM_Jump)
{
	auto pmove = reinterpret_cast<uintptr_t>(*ppmove);
	int playerIndex = *reinterpret_cast<int*>(pmove + offPlayerIndex);

	int *onground = reinterpret_cast<int*>(pmove + offOnground);
	int orig_onground = *onground;

	int *oldbuttons = reinterpret_cast<int*>(pmove + offOldbuttons);
	int orig_oldbuttons = *oldbuttons;

	if (CVars::bxt_autojump.GetBool())
	{
		if ((orig_onground != -1) && !cantJumpNextTime[playerIndex])
			*oldbuttons &= ~IN_JUMP;
	}

	cantJumpNextTime[playerIndex] = false;

	if (offBhopcap)
	{
		auto pPMJump = reinterpret_cast<ptrdiff_t>(ORIG_PM_Jump);
		if (CVars::bxt_bhopcap.GetBool())
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

	if (CVars::bxt_autojump.GetBool())
		*oldbuttons = orig_oldbuttons;
}

HOOK_DEF_0(ServerDLL, void, __cdecl, PM_PreventMegaBunnyJumping)
{
	if (CVars::bxt_bhopcap.GetBool())
		return ORIG_PM_PreventMegaBunnyJumping();
}

HOOK_DEF_1(ServerDLL, void, __cdecl, PM_PlayerMove, qboolean, server)
{
	if (!ppmove)
		return ORIG_PM_PlayerMove(server);

	auto pmove = reinterpret_cast<uintptr_t>(*ppmove);

	int playerIndex = *reinterpret_cast<int*>(pmove + offPlayerIndex);
	int *groundEntity = reinterpret_cast<int*>(pmove + offOnground);

	float *velocity, *origin, *angles;
	velocity = reinterpret_cast<float*>(pmove + offVelocity);
	origin =   reinterpret_cast<float*>(pmove + offOrigin);
	angles =   reinterpret_cast<float*>(pmove + offAngles);
	usercmd_t *cmd = reinterpret_cast<usercmd_t*>(pmove + offCmd);

	#define ALERT(at, format, ...) pEngfuncs->pfnAlertMessage(at, const_cast<char*>(format), ##__VA_ARGS__)
	if (CVars::_bxt_taslog.GetBool() && pEngfuncs)
	{
		ALERT(at_console, "-- BXT TAS Log Start --\n");
		ALERT(at_console, "Player index: %d; msec: %hhu (%Lf)\n", playerIndex, cmd->msec, static_cast<long double>(cmd->msec) * 0.001);
		ALERT(at_console, "Angles: %.8f; %.8f; %.8f\n", angles[0], angles[1], angles[2]);
		ALERT(at_console, "Velocity: %.8f; %.8f; %.8f; origin: %.8f; %.8f; %.8f\n", velocity[0], velocity[1], velocity[2], origin[0], origin[1], origin[2]);
		ALERT(at_console, "Onground: %d; usehull: %d\n", *groundEntity, *reinterpret_cast<int*>(pmove + 0xBC));
	}

	ORIG_PM_PlayerMove(server);

	if (CVars::_bxt_taslog.GetBool() && pEngfuncs)
	{
		ALERT(at_console, "New velocity: %.8f; %.8f; %.8f; new origin: %.8f; %.8f; %.8f\n", velocity[0], velocity[1], velocity[2], origin[0], origin[1], origin[2]);
		ALERT(at_console, "New onground: %d; new usehull: %d\n", *groundEntity, *reinterpret_cast<int*>(pmove + 0xBC));
		ALERT(at_console, "-- BXT TAS Log End --\n");
	}
	#undef ALERT

	CustomHud::UpdatePlayerInfo(velocity, origin);
}

HOOK_DEF_4(ServerDLL, int, __cdecl, PM_ClipVelocity, float*, in, float*, normal, float*, out, float, overbounce)
{
	auto ret = ORIG_PM_ClipVelocity(in, normal, out, overbounce);

	if (CVars::_bxt_taslog.GetBool() && pEngfuncs) {
		if (normal[2] != 1.0f && normal[2] != -1.0f)
			pEngfuncs->pfnAlertMessage(at_console, const_cast<char*>("PM_ClipVelocity: %f (%f %f %f [%f] -> %f %f %f [%f])\n"),
				std::acos(static_cast<double>(normal[2])) * 180 / M_PI, in[0], in[1], in[2], std::hypot(in[0], in[1]), out[0], out[1], out[2], std::hypot(out[0], out[1]));
	}

	if (normal[2] != 1.0f && normal[2] != -1.0f && CVars::bxt_interprocess_enable.GetBool() && Interprocess::mq) {
		try {
			unsigned char buf[30];
			buf[0] = 30;
			buf[1] = 0x01;
			std::memcpy(buf + 2, &normal[2], sizeof(normal[2]));
			std::memcpy(buf + 6, in, 12);
			std::memcpy(buf + 18, out, 12);

			Interprocess::mq->send(buf, sizeof(buf), 0);
		} catch (boost::interprocess::interprocess_exception) {
			// Do nothing.
		}
	}

	return ret;
}

HOOK_DEF_0(ServerDLL, void, __cdecl, PM_WaterMove)
{
	if (CVars::bxt_interprocess_enable.GetBool() && Interprocess::mq) {
		try {
			unsigned char buf[2];
			buf[0] = 2;
			buf[1] = 0x02;

			Interprocess::mq->send(buf, sizeof(buf), 0);
		} catch (boost::interprocess::interprocess_exception) {
			// Do nothing.
		}
	}

	return ORIG_PM_WaterMove();
}

HOOK_DEF_3(ServerDLL, void, __cdecl, CmdStart, const edict_t*, player, const usercmd_t*, cmd, unsigned int, random_seed)
{
	HwDLL::GetInstance().SetLastRandomSeed(random_seed);
	auto seed = random_seed;
	bool changedSeed = false;
	if (HwDLL::GetInstance().IsCountingSharedRNGSeed()) {
		auto lastSeed = HwDLL::GetInstance().GetSharedRNGSeedCounter();
		seed = lastSeed - (--HwDLL::GetInstance().QueuedSharedRNGSeeds);
		changedSeed = true;
	}

	#define ALERT(at, format, ...) pEngfuncs->pfnAlertMessage(at, const_cast<char*>(format), ##__VA_ARGS__)
	if (CVars::_bxt_taslog.GetBool() && pEngfuncs)
	{
		ALERT(at_console, "-- CmdStart Start --\n");
		ALERT(at_console, "Buttons: %hu\n", cmd->buttons);
		ALERT(at_console, "Random_seed: %u", random_seed);
		if (changedSeed)
			ALERT(at_console, " (overriding with %u)", seed);
		ALERT(at_console, "\n");
		ALERT(at_console, "Paused: %s\n", (HwDLL::GetInstance().IsPaused() ? "true" : "false"));
		ALERT(at_console, "-- CmdStart End --\n");
	}
	#undef ALERT

	return ORIG_CmdStart(player, cmd, seed);
}

HOOK_DEF_2(ServerDLL, void, __fastcall, CNihilanth__DyingThink, void*, thisptr, int, edx)
{
	if (CVars::bxt_timer_autostop.GetBool())
		CustomHud::SetCountingTime(false);

	return ORIG_CNihilanth__DyingThink(thisptr, edx);
}

HOOK_DEF_1(ServerDLL, void, __cdecl, CNihilanth__DyingThink_Linux, void*, thisptr)
{
	if (CVars::bxt_timer_autostop.GetBool())
		CustomHud::SetCountingTime(false);

	return ORIG_CNihilanth__DyingThink_Linux(thisptr);
}

HOOK_DEF_2(ServerDLL, void, __fastcall, COFGeneWorm__DyingThink, void*, thisptr, int, edx)
{
	if (CVars::bxt_timer_autostop.GetBool())
		CustomHud::SetCountingTime(false);

	return ORIG_COFGeneWorm__DyingThink(thisptr, edx);
}

HOOK_DEF_1(ServerDLL, void, __cdecl, COFGeneWorm__DyingThink_Linux, void*, thisptr)
{
	if (CVars::bxt_timer_autostop.GetBool())
		CustomHud::SetCountingTime(false);

	return ORIG_COFGeneWorm__DyingThink_Linux(thisptr);
}

HOOK_DEF_2(ServerDLL, void, __fastcall, CMultiManager__ManagerThink, void*, thisptr, int, edx)
{
	if (CVars::bxt_timer_autostop.GetBool() && ppGlobals) {
		entvars_t *pev = *reinterpret_cast<entvars_t**>(reinterpret_cast<uintptr_t>(thisptr) + 4);
		if (pev && pev->targetname) {
			const char *targetname = (*ppGlobals)->pStringBase + pev->targetname;
			if (!std::strcmp(targetname, "roll_the_credits") || !std::strcmp(targetname, "youwinmulti"))
				CustomHud::SetCountingTime(false);
		}
	}

	return ORIG_CMultiManager__ManagerThink(thisptr, edx);
}

HOOK_DEF_5(ServerDLL, void, __cdecl, CMultiManager__ManagerUse_Linux, void*, thisptr, void*, pActivator, void*, pCaller, int, useType, float, value)
{
	if (CVars::bxt_timer_autostop.GetBool() && ppGlobals && pCaller) {
		entvars_t *pev = *reinterpret_cast<entvars_t**>(reinterpret_cast<uintptr_t>(thisptr) + 4);
		if (pev && pev->targetname) {
			const char *targetname = (*ppGlobals)->pStringBase + pev->targetname;
			if (!std::strcmp(targetname, "roll_the_credits") || !std::strcmp(targetname, "youwinmulti")) {
				entvars_t *callerPev = *reinterpret_cast<entvars_t**>(reinterpret_cast<uintptr_t>(pCaller) + 4);
				if (callerPev && callerPev->targetname) {
					const char *callerTargetname = (*ppGlobals)->pStringBase + callerPev->targetname;
					if (!std::strcmp(callerTargetname, "mgr_take_over") || !std::strcmp(callerTargetname, "endbot"))
						CustomHud::SetCountingTime(false);
				}
			}
		}
	}

	return ORIG_CMultiManager__ManagerUse_Linux(thisptr, pActivator, pCaller, useType, value);
}

void ServerDLL::GetTriggerColor(const char *classname, float &r, float &g, float &b, float &a) const
{
	assert(std::strncmp(classname, "trigger_", 8) == 0);

	classname += 8;
	if (std::strcmp(classname, "changelevel") == 0) {
		// Bright green
		r = 79;
		g = 255;
		b = 10;
		a = 120;
	} else if (std::strcmp(classname, "hurt") == 0) {
		// Red
		r = 255;
		g = 0;
		b = 0;
		a = 120;
	} else if (std::strcmp(classname, "multiple") == 0) {
		// Blue
		r = 0;
		g = 0;
		b = 255;
		a = 120;
	} else if (std::strcmp(classname, "once") == 0) {
		// Cyan
		r = 0;
		g = 255;
		b = 255;
		a = 120;
	} else if (std::strcmp(classname, "push") == 0) {
		// Bright yellow
		r = 255;
		g = 255;
		b = 0;
		a = 120;
	} else if (std::strcmp(classname, "teleport") == 0) {
		// Dull green
		r = 81;
		g = 147;
		b = 49;
		a = 120;
	} else if (std::strcmp(classname, "transition") == 0) {
		// Magenta
		r = 203;
		g = 103;
		b = 212;
		a = 120;
	} else {
		// White
		r = 255;
		g = 255;
		b = 255;
		a = 100;
	}
}

HOOK_DEF_7(ServerDLL, int, __cdecl, AddToFullPack, struct entity_state_s*, state, int, e, edict_t*, ent, edict_t*, host, int, hostflags, int, player, unsigned char*, pSet)
{
	if (!ppGlobals) {
		return ORIG_AddToFullPack(state, e, ent, host, hostflags, player, pSet);
	}

	auto oldEffects = ent->v.effects;
	auto oldRendermode = ent->v.rendermode;
	auto oldRenderColor = ent->v.rendercolor;
	auto oldRenderAmount = ent->v.renderamt;
	auto oldRenderFx = ent->v.renderfx;

	const char *classname = (*ppGlobals)->pStringBase + ent->v.classname;
	bool is_trigger = std::strncmp(classname, "trigger_", 8) == 0;

	if (!is_trigger && CVars::bxt_show_hidden_entities.GetBool()) {
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
	} else if (is_trigger && CVars::bxt_show_triggers.GetBool()) {
		ent->v.effects &= ~EF_NODRAW;
		ent->v.rendermode = kRenderTransColor;
		ent->v.renderfx = kRenderFxPulseFast;
		GetTriggerColor(classname, ent->v.rendercolor.x, ent->v.rendercolor.y, ent->v.rendercolor.z, ent->v.renderamt);
	}

	auto ret = ORIG_AddToFullPack(state, e, ent, host, hostflags, player, pSet);

	ent->v.effects = oldEffects;
	ent->v.rendermode = oldRendermode;
	ent->v.rendercolor = oldRenderColor;
	ent->v.renderamt = oldRenderAmount;
	ent->v.renderfx = oldRenderFx;

	return ret;
}

HOOK_DEF_1(ServerDLL, void, __fastcall, CTriggerVolume__Spawn, void*, thisptr)
{
	if (!ppGlobals || !pEngfuncs) {
		ORIG_CTriggerVolume__Spawn(thisptr);
		return;
	}

	entvars_t *pev = *reinterpret_cast<entvars_t**>(reinterpret_cast<uintptr_t>(thisptr) + 4);
	string_t old_model = pev->model;
	ORIG_CTriggerVolume__Spawn(thisptr);
	pev->model = old_model;
	pev->modelindex = pEngfuncs->pfnModelIndex((*ppGlobals)->pStringBase + old_model);
	pev->effects |= EF_NODRAW;
}

HOOK_DEF_1(ServerDLL, void, __cdecl, CTriggerVolume__Spawn_Linux, void*, thisptr)
{
	if (!ppGlobals || !pEngfuncs) {
		ORIG_CTriggerVolume__Spawn_Linux(thisptr);
		return;
	}

	entvars_t *pev = *reinterpret_cast<entvars_t**>(reinterpret_cast<uintptr_t>(thisptr) + 4);
	string_t old_model = pev->model;
	ORIG_CTriggerVolume__Spawn_Linux(thisptr);
	pev->model = old_model;
	pev->modelindex = pEngfuncs->pfnModelIndex((*ppGlobals)->pStringBase + old_model);
	pev->effects |= EF_NODRAW;
}

HOOK_DEF_1(ServerDLL, void, __cdecl, ClientCommand, edict_t*, pEntity)
{
#ifdef _WIN32
	if (!ORIG_CBasePlayer__ForceClientDllUpdate) {
		ORIG_ClientCommand(pEntity);
		return;
	}
#else
	if (!ORIG_CBasePlayer__ForceClientDllUpdate_Linux) {
		ORIG_ClientCommand(pEntity);
		return;
	}
#endif

	const char *cmd = pEngfuncs->pfnCmd_Argv(0);
	if (std::strcmp(cmd, "fullupdate") != 0) {
		ORIG_ClientCommand(pEntity);
		return;
	}

	void *classPtr = pEntity->v.pContainingEntity->pvPrivateData;
	uintptr_t thisAddr = reinterpret_cast<uintptr_t>(classPtr);
	int *m_iClientFOV = reinterpret_cast<int *>(thisAddr + offm_iClientFOV);
	int *m_rgAmmoLast = reinterpret_cast<int *>(thisAddr + offm_rgAmmoLast);
	*m_iClientFOV = -1;
	for (int i = 0; i < maxAmmoSlots; i++)
		m_rgAmmoLast[i] = -1;

#ifdef _WIN32
	ORIG_CBasePlayer__ForceClientDllUpdate(classPtr);
#else
	ORIG_CBasePlayer__ForceClientDllUpdate_Linux(classPtr);
#endif
}
