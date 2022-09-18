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
#include "../runtime_data.hpp"
#include "../custom_triggers.hpp"

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

extern "C" void __cdecl _Z11FireTargetsPKcP11CBaseEntityS2_8USE_TYPEf(char* targetName, void* pActivator, void* pCaller, int useType, float value)
{
     return ServerDLL::HOOKED_FireTargets_Linux(targetName, pActivator, pCaller, useType, value);
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

extern "C" void __cdecl _ZN6CGraph9InitGraphEv(void* thisptr)
{
	return ServerDLL::HOOKED_CGraph__InitGraph_Linux(thisptr);
}

extern "C" void __cdecl _ZN11CBasePlayer20CheatImpulseCommandsEi(void* thisptr, int iImpulse)
{
	return ServerDLL::HOOKED_CBasePlayer__CheatImpulseCommands_Linux(thisptr, iImpulse);
}

extern "C" void __cdecl _ZN12CTriggerSave9SaveTouchEP11CBaseEntity(void* thisptr, void* pOther)
{
	return ServerDLL::HOOKED_CTriggerSave__SaveTouch_Linux(thisptr, pOther);
}

extern "C" int __cdecl _ZN11CBasePlayer10TakeDamageEP9entvars_sS1_fi(void* thisptr, entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType)
{
	return ServerDLL::HOOKED_CBasePlayer__TakeDamage_Linux(thisptr, pevInflictor, pevAttacker, flDamage, bitsDamageType);
}

extern "C" void __cdecl _ZN9CPushable4MoveEP11CBaseEntityi(void* thisptr, void* pOther, int push)
{
	return ServerDLL::HOOKED_CPushable__Move_Linux(thisptr, pOther, push);
}

extern "C" void __cdecl _ZN11CBaseEntity11FireBulletsEj6VectorS0_S0_fiiiP9entvars_s(void* thisptr, unsigned long cShots, Vector vecSrc, Vector vecDirShooting, Vector vecSpread, float flDistance, int iBulletType, int iTracerFreq, int iDamage, entvars_t* pevAttacker)
{
	return ServerDLL::HOOKED_CBaseEntity__FireBullets_Linux(thisptr, cShots, vecSrc, vecDirShooting, vecSpread, flDistance, iBulletType, iTracerFreq, iDamage, pevAttacker);
}

extern "C" Vector __cdecl _ZN11CBaseEntity17FireBulletsPlayerEj6VectorS0_S0_fiiiP9entvars_si(void* thisptr, unsigned long cShots, Vector vecSrc, Vector vecDirShooting, Vector vecSpread, float flDistance, int iBulletType, int iTracerFreq, int iDamage, entvars_t* pevAttacker, int shared_rand)
{
	return ServerDLL::HOOKED_CBaseEntity__FireBulletsPlayer_Linux(thisptr, cShots, vecSrc, vecDirShooting, vecSpread, flDistance, iBulletType, iTracerFreq, iDamage, pevAttacker, shared_rand);
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
	{
		MemUtils::Intercept(moduleName,
			ORIG_PM_Jump, HOOKED_PM_Jump,
			ORIG_PM_PreventMegaBunnyJumping, HOOKED_PM_PreventMegaBunnyJumping,
			ORIG_PM_PlayerMove, HOOKED_PM_PlayerMove,
			ORIG_PM_ClipVelocity, HOOKED_PM_ClipVelocity,
			ORIG_PM_WaterMove, HOOKED_PM_WaterMove,
			ORIG_PM_WalkMove, HOOKED_PM_WalkMove,
			ORIG_PM_FlyMove, HOOKED_PM_FlyMove,
			ORIG_PM_AddToTouched, HOOKED_PM_AddToTouched,
			ORIG_PM_Move, HOOKED_PM_Move,
			ORIG_CmdStart, HOOKED_CmdStart,
			ORIG_CNihilanth__DyingThink, HOOKED_CNihilanth__DyingThink,
			ORIG_COFGeneWorm__DyingThink, HOOKED_COFGeneWorm__DyingThink,
			ORIG_CApache__DyingThink, HOOKED_CApache__DyingThink,
			ORIG_CBaseDoor__DoorGoUp, HOOKED_CBaseDoor__DoorGoUp,
			ORIG_CBaseDoor__DoorHitTop, HOOKED_CBaseDoor__DoorHitTop,
			ORIG_CMultiManager__ManagerThink, HOOKED_CMultiManager__ManagerThink,
			ORIG_AddToFullPack, HOOKED_AddToFullPack,
			ORIG_CTriggerVolume__Spawn, HOOKED_CTriggerVolume__Spawn,
			ORIG_ClientCommand, HOOKED_ClientCommand,
			ORIG_CPushable__Move, HOOKED_CPushable__Move,
			ORIG_CBasePlayer__TakeDamage, HOOKED_CBasePlayer__TakeDamage,
			ORIG_CGraph__InitGraph, HOOKED_CGraph__InitGraph,
			ORIG_CBasePlayer__CheatImpulseCommands, HOOKED_CBasePlayer__CheatImpulseCommands,
			ORIG_CTriggerSave__SaveTouch, HOOKED_CTriggerSave__SaveTouch,
			ORIG_CChangeLevel__UseChangeLevel, HOOKED_CChangeLevel__UseChangeLevel,
			ORIG_CChangeLevel__TouchChangeLevel, HOOKED_CChangeLevel__TouchChangeLevel,
			ORIG_CBaseMonster__Killed, HOOKED_CBaseMonster__Killed,
			ORIG_CBaseEntity__FireBullets, HOOKED_CBaseEntity__FireBullets,
			ORIG_CBaseEntity__FireBullets_Linux, HOOKED_CBaseEntity__FireBullets_Linux,
			ORIG_CBaseEntity__FireBulletsPlayer, HOOKED_CBaseEntity__FireBulletsPlayer,
			ORIG_CBaseEntity__FireBulletsPlayer_Linux, HOOKED_CBaseEntity__FireBulletsPlayer_Linux,
			ORIG_CTriggerEndSection__EndSectionUse, HOOKED_CTriggerEndSection__EndSectionUse,
			ORIG_CTriggerEndSection__EndSectionTouch, HOOKED_CTriggerEndSection__EndSectionTouch);
	}
}

void ServerDLL::Unhook()
{
	if (m_Intercepted)
	{
		MemUtils::RemoveInterception(m_Name,
			ORIG_PM_Jump,
			ORIG_PM_PreventMegaBunnyJumping,
			ORIG_PM_PlayerMove,
			ORIG_PM_ClipVelocity,
			ORIG_PM_WaterMove,
			ORIG_PM_WalkMove,
			ORIG_PM_FlyMove,
			ORIG_PM_AddToTouched,
			ORIG_PM_Move,
			ORIG_CmdStart,
			ORIG_CNihilanth__DyingThink,
			ORIG_COFGeneWorm__DyingThink,
			ORIG_CApache__DyingThink,
			ORIG_CBaseDoor__DoorGoUp,
			ORIG_CBaseDoor__DoorHitTop,
			ORIG_CMultiManager__ManagerThink,
			ORIG_AddToFullPack,
			ORIG_CTriggerVolume__Spawn,
			ORIG_ClientCommand,
			ORIG_CPushable__Move,
			ORIG_CBasePlayer__TakeDamage,
			ORIG_CGraph__InitGraph,
			ORIG_CBasePlayer__CheatImpulseCommands,
			ORIG_CTriggerSave__SaveTouch,
			ORIG_CChangeLevel__UseChangeLevel,
			ORIG_CChangeLevel__TouchChangeLevel,
			ORIG_CBaseEntity__FireBullets,
			ORIG_CBaseEntity__FireBullets_Linux,
			ORIG_CBaseEntity__FireBulletsPlayer,
			ORIG_CBaseEntity__FireBulletsPlayer_Linux,
			ORIG_CBaseMonster__Killed,
			ORIG_CTriggerEndSection__EndSectionUse,
			ORIG_CTriggerEndSection__EndSectionTouch);
	}

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
	ORIG_PM_WalkMove = nullptr;
	ORIG_PM_FlyMove = nullptr;
	ORIG_PM_AddToTouched = nullptr;
	ORIG_PM_Ladder = nullptr;
	ORIG_PM_Move = nullptr;
	ORIG_CmdStart = nullptr;
	ORIG_CNihilanth__DyingThink = nullptr;
	ORIG_CNihilanth__DyingThink_Linux = nullptr;
	ORIG_COFGeneWorm__DyingThink = nullptr;
	ORIG_COFGeneWorm__DyingThink_Linux = nullptr;
	ORIG_CApache__DyingThink = nullptr;
	ORIG_CBaseDoor__DoorGoUp = nullptr;
	ORIG_CBaseDoor__DoorHitTop = nullptr;
	ORIG_CBaseMonster__Killed = nullptr;
	ORIG_CMultiManager__ManagerThink = nullptr;
	ORIG_FireTargets_Linux = nullptr;
	ORIG_AddToFullPack = nullptr;
	ORIG_CTriggerVolume__Spawn = nullptr;
	ORIG_CTriggerVolume__Spawn_Linux = nullptr;
	ORIG_CBasePlayer__ForceClientDllUpdate = nullptr;
	ORIG_CBasePlayer__ForceClientDllUpdate_Linux = nullptr;
	ORIG_ClientCommand = nullptr;
	ORIG_CPushable__Move = nullptr;
	ORIG_CPushable__Move_Linux = nullptr;
	ORIG_CBasePlayer__TakeDamage = nullptr;
	ORIG_CBasePlayer__TakeDamage_Linux = nullptr;
	ORIG_GetEntityAPI = nullptr;
	ORIG_CGraph__InitGraph = nullptr;
	ORIG_CGraph__InitGraph_Linux = nullptr;
	ORIG_CBasePlayer__CheatImpulseCommands = nullptr;
	ORIG_CBasePlayer__CheatImpulseCommands_Linux = nullptr;
	ORIG_CTriggerSave__SaveTouch = nullptr;
	ORIG_CTriggerSave__SaveTouch_Linux = nullptr;
	ORIG_CChangeLevel__UseChangeLevel = nullptr;
	ORIG_CChangeLevel__TouchChangeLevel = nullptr;
	ORIG_CBaseEntity__FireBullets = nullptr;
	ORIG_CBaseEntity__FireBullets_Linux = nullptr;
	ORIG_CBaseEntity__FireBulletsPlayer = nullptr;
	ORIG_CBaseEntity__FireBulletsPlayer_Linux = nullptr;
	ORIG_CChangeLevel__InTransitionVolume = nullptr;
	ORIG_CTriggerEndSection__EndSectionUse = nullptr;
	ORIG_CTriggerEndSection__EndSectionTouch = nullptr;
	ppmove = nullptr;
	offPlayerIndex = 0;
	offOldbuttons = 0;
	offOnground = 0;
	offVelocity = 0;
	offOrigin = 0;
	offAngles = 0;
	offCmd = 0;
	offBhopcap = 0;
	pBhopcapWindows = 0;
	pCZDS_Velocity_Byte = 0;
	pAddToFullPack_PVS_Byte = 0;
	offm_iClientFOV = 0;
	offm_rgAmmoLast = 0;
	maxAmmoSlots = 0;
	offEntFriction = 0;
	offEntGravity = 0;
	offPunchangles = 0;
	offWaterlevel = 0;
	offInDuck = 0;
	offFlags = 0;
	offBasevelocity = 0;
	offm_pNodes = 0;
	offm_vecOrigin = 0;
	offm_cNodes = 0;
	size_CNode = 0;
	pGlobalState = nullptr;
	offNihilanthLevel = 0;
	offNihilanthIrritation = 0;
	offNihilanthRecharger = 0;
	offNihilanthSpheres = 0;
	memset(originalBhopcapInsn, 0, sizeof(originalBhopcapInsn));
	cantJumpNextTime.clear();
	m_Intercepted = false;
	WorldGraph = nullptr;
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
	auto fPM_PreventMegaBunnyJumping = FindFunctionAsync(
		ORIG_PM_PreventMegaBunnyJumping,
		"PM_PreventMegaBunnyJumping",
		patterns::shared::PM_PreventMegaBunnyJumping);

	auto fPM_PlayerMove = FindFunctionAsync(
		ORIG_PM_PlayerMove,
		"PM_PlayerMove",
		patterns::shared::PM_PlayerMove,
		[&](auto pattern) {
			offPlayerIndex = 0;
			offVelocity = 92;
			offOrigin = 56;
			offAngles = 68;
			offCmd = 283736;
			offEntFriction = 0xC4;
			offEntGravity = 0xC0;
			offPunchangles = 0xA0;
			offWaterlevel = 0xE4;
			offInDuck = 0x90;
			offFlags = 0xB8;
			offBasevelocity = 0x74;
		});

	auto fPM_Jump = FindFunctionAsync(
		ORIG_PM_Jump,
		"PM_Jump",
		patterns::shared::PM_Jump,
		[&](auto pattern) {
			offPlayerIndex = 0;
			offOldbuttons = 200;
			offOnground = 224;
			if (pattern == patterns::shared::PM_Jump.cend()) // Linux.
			{
				ptrdiff_t bhopcapAddr;
				auto n = MemUtils::find_unique_sequence(
					m_Base,
					m_Length,
					patterns::shared::Bhopcap.cbegin(),
					patterns::shared::Bhopcap.cend(),
					bhopcapAddr);
				if (n != patterns::shared::Bhopcap.cend())
				{
					offBhopcap = bhopcapAddr - reinterpret_cast<ptrdiff_t>(ORIG_PM_Jump) + 27;
					memcpy(originalBhopcapInsn, reinterpret_cast<void*>(static_cast<uintptr_t>(bhopcapAddr) + 27), sizeof(originalBhopcapInsn));
				}
			}
			else
			{
				switch (pattern - patterns::shared::PM_Jump.cbegin())
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
				case 10:
				case 11:
				case 5:
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(ORIG_PM_Jump) + 5);
					break;
				case 12:
				case 6:
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(ORIG_PM_Jump) + 24);
					break;
				case 7:
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(ORIG_PM_Jump) + 6);
					break;
				case 8:
				case 9:
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(ORIG_PM_Jump) + 8);
					break;
				}
			}
		});

	auto fPM_ClipVelocity = FindFunctionAsync(ORIG_PM_ClipVelocity, "PM_ClipVelocity", patterns::shared::PM_ClipVelocity);
	auto fPM_WaterMove = FindFunctionAsync(ORIG_PM_WaterMove, "PM_WaterMove", patterns::shared::PM_WaterMove);
	auto fCTriggerVolume__Spawn = FindAsync(ORIG_CTriggerVolume__Spawn,	patterns::server::CTriggerVolume__Spawn);

	auto fCBasePlayer__ForceClientDllUpdate = FindAsync(
		ORIG_CBasePlayer__ForceClientDllUpdate,
		patterns::server::CBasePlayer__ForceClientDllUpdate,
		[&](auto pattern) {
			switch (pattern - patterns::server::CBasePlayer__ForceClientDllUpdate.cbegin()) {
			case 0:  // HL-SteamPipe
				maxAmmoSlots = MAX_AMMO_SLOTS;
				offm_rgAmmoLast = 0x554;
				offm_iClientFOV = 0x4AC;
				break;
			case 1:  // OpposingForce
				maxAmmoSlots = MAX_AMMO_SLOTS;
				offm_rgAmmoLast = 0x604;
				offm_iClientFOV = 0x4E0;
				offFuncIsPlayer = 0xA0;
				offFuncCenter = 0xCC;
				break;
			case 19: // TheGate
			case 2:  // HazardousCourse2
				maxAmmoSlots = MAX_AMMO_SLOTS;
				offm_rgAmmoLast = 0x540;
				offm_iClientFOV = 0x498;
				break;
			case 3:  // Gunman
				maxAmmoSlots = MAX_AMMO_SLOTS;
				offm_rgAmmoLast = 0x53C;
				offm_iClientFOV = 0x47C;
				offFuncCenter = 0xCC;
				break;
			case 4:  // HL-SteamPipe-8308
				maxAmmoSlots = MAX_AMMO_SLOTS;
				offm_rgAmmoLast = 0x558;
				offm_iClientFOV = 0x4B0;
				break;
			case 5: // TWHL-Tower-2
			case 6: // Halfquake Trilogy
				maxAmmoSlots = MAX_AMMO_SLOTS;
				offm_rgAmmoLast = 0x5F4;
				offm_iClientFOV = 0x548;
				offFuncIsPlayer = 0xD4;
				offFuncCenter = 0x100;
				offFuncObjectCaps = 0x44;
				spirit_sdk = true;
				break;
			case 7: // Echoes
				maxAmmoSlots = MAX_AMMO_SLOTS;
				offm_rgAmmoLast = 0x5F4;
				offm_iClientFOV = 0x548;
				offFuncIsPlayer = 0xCC;
				offFuncCenter = 0xF8;
				offFuncObjectCaps = 0x3C;
				spirit_sdk = true;
				break;
			case 8: // Decay
				maxAmmoSlots = MAX_AMMO_SLOTS;
				offm_rgAmmoLast = 0x544;
				offm_iClientFOV = 0x49C;
				break;
			case 9: // AoMDC
				maxAmmoSlots = MAX_AMMO_SLOTS;
				offm_rgAmmoLast = 0x578;
				offm_iClientFOV = 0x4D0;
				break;
			case 10: // PARANOIA
				maxAmmoSlots = MAX_AMMO_SLOTS;
				offm_rgAmmoLast = 0x62C;
				offm_iClientFOV = 0x584;
				offFuncIsPlayer = 0xD0;
				offFuncCenter = 0xFC;
				offFuncObjectCaps = 0x40;
				break;
			case 11: // OP4-8684
				maxAmmoSlots = MAX_AMMO_SLOTS;
				offm_rgAmmoLast = 0x608;
				offm_iClientFOV = 0x4E4;
				offFuncIsPlayer = 0xA0;
				offFuncCenter = 0xCC;
				break;
			case 22: // HL-SWEET
			case 12: // HL-WON
				maxAmmoSlots = MAX_AMMO_SLOTS;
				offm_rgAmmoLast = 0x50C;
				offm_iClientFOV = 0x464;
				break;
			case 13: // OP4-WON
				maxAmmoSlots = MAX_AMMO_SLOTS;
				offm_rgAmmoLast = 0x5C0;
				offm_iClientFOV = 0x4A4;
				offFuncIsPlayer = 0xA0;
				offFuncCenter = 0xCC;
				break;
			case 14: // CSCZDS
				maxAmmoSlots = MAX_AMMO_SLOTS;
				offm_rgAmmoLast = 0x540;
				offm_iClientFOV = 0x89C;
				offFuncIsPlayer = 0xA8;
				offFuncCenter = 0xDC;
				offFuncObjectCaps = 0x18;
				break;
			case 15: // Black-Ops
				maxAmmoSlots = MAX_AMMO_SLOTS;
				offm_rgAmmoLast = 0x554;
				offm_iClientFOV = 0x4AC;
				offFuncIsPlayer = 0xA4;
				offFuncCenter = 0xD0;
				break;
			case 16: // Invasion
				maxAmmoSlots = MAX_AMMO_SLOTS;
				offm_rgAmmoLast = 0x514;
				offm_iClientFOV = 0x46C;
				break;
			case 17: // Arctic-Incident
				maxAmmoSlots = MAX_AMMO_SLOTS;
				offm_rgAmmoLast = 0x5B8;
				offm_iClientFOV = 0x510;
				offFuncIsPlayer = 0xCC;
				offFuncCenter = 0xF8;
				offFuncObjectCaps = 0x3C;
				break;
			case 18: // HL-Delta
				maxAmmoSlots = MAX_AMMO_SLOTS;
				offm_rgAmmoLast = 0x5C8;
				offm_iClientFOV = 0x51C;
				offFuncIsPlayer = 0xCC;
				offFuncCenter = 0xF8;
				offFuncObjectCaps = 0x3C;
				break;
			case 20: // OPBT
				maxAmmoSlots = MAX_AMMO_SLOTS;
				offm_rgAmmoLast = 0x65C;
				offm_iClientFOV = 0x5A4;
				break;
			case 21: // Parasomnia
				maxAmmoSlots = MAX_AMMO_SLOTS;
				offm_rgAmmoLast = 0x564;
				offm_iClientFOV = 0x4B8;
				offFuncIsPlayer = 0x98;
				offFuncCenter = 0xC4;
				break;
			case 23: // AVP2
				maxAmmoSlots = MAX_AMMO_SLOTS;
				offm_rgAmmoLast = 0x550;
				offm_iClientFOV = 0x4A8;
				break;
			case 24: // CStrike-Latest
				maxAmmoSlots = MAX_AMMO_SLOTS;
				offm_rgAmmoLast = 0x660;
				offm_iClientFOV = 0x5B0;
				offFuncIsPlayer = 0xA0;
				offFuncCenter = 0xCC;
				offFuncObjectCaps = 0x18;
				break;
			case 25: // TFC-Latest
				maxAmmoSlots = MAX_AMMO_SLOTS;
				offm_rgAmmoLast = 0x978;
				offm_iClientFOV = 0x8CC;
				offFuncIsPlayer = 0x98;
				offFuncCenter = 0xC4;
				offFuncObjectCaps = 0x18;
				break;
			default:
				assert(false);
			}
		});

	auto fPM_Jump_CZDS_Velocity_Byte = FindAsync(
		pCZDS_Velocity_Byte,
		patterns::server::CZDS_Velocity_Byte,
		[&](auto pattern) {
			switch (pattern - patterns::server::CZDS_Velocity_Byte.cbegin()) {
			case 0: // HL-SteamPipe
				// the actual byte inside the pattern that needs changing
				pCZDS_Velocity_Byte += 6;
				break;
			default:
				assert(false);
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

	auto fAddToFullPack_PVS_Byte = FindAsync(
		pAddToFullPack_PVS_Byte,
		patterns::server::AddToFullPack_PVS_Byte,
		[&](auto pattern) {
			switch (pattern - patterns::server::AddToFullPack_PVS_Byte.cbegin()) {
			case 0: // HL-SteamPipe
			case 1: // Parasomnia
				pAddToFullPack_PVS_Byte += 2;
				break;
			case 2: // AoMDC
				pAddToFullPack_PVS_Byte += 6;
				break;
			case 3: // Counter-Strike 1.6
				pAddToFullPack_PVS_Byte += 17;
				break;
			default:
				assert(false);
			}
		});

	auto fPM_WalkMove = FindFunctionAsync(ORIG_PM_WalkMove, "PM_WalkMove", patterns::shared::PM_WalkMove);
	auto fPM_FlyMove = FindFunctionAsync(ORIG_PM_FlyMove, "PM_FlyMove", patterns::shared::PM_FlyMove);
	auto fPM_AddToTouched = FindFunctionAsync(ORIG_PM_AddToTouched, "PM_AddToTouched", patterns::shared::PM_AddToTouched);
	auto fPM_Ladder = FindFunctionAsync(ORIG_PM_Ladder, "PM_Ladder", patterns::shared::PM_Ladder);
	auto fCPushable__Move = FindAsync(ORIG_CPushable__Move, patterns::server::CPushable__Move);
	auto fCBasePlayer__TakeDamage = FindAsync(ORIG_CBasePlayer__TakeDamage, patterns::server::CBasePlayer__TakeDamage);
	auto fCBasePlayer__CheatImpulseCommands = FindAsync(ORIG_CBasePlayer__CheatImpulseCommands, patterns::server::CBasePlayer__CheatImpulseCommands);
	auto fCBaseMonster__Killed = FindAsync(ORIG_CBaseMonster__Killed, patterns::server::CBaseMonster__Killed);
	auto fCChangeLevel__InTransitionVolume = FindAsync(ORIG_CChangeLevel__InTransitionVolume, patterns::server::CChangeLevel__InTransitionVolume);

	auto fCGraph__InitGraph = FindAsync(
		ORIG_CGraph__InitGraph,
		patterns::server::CGraph__InitGraph,
		[&](auto pattern) {
			switch (pattern - patterns::server::CGraph__InitGraph.cbegin()) {
			case 0:  // HL-SteamPipe
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
				offm_pNodes = 0x0C;
				offm_vecOrigin = 0x00;
				offm_cNodes = 0x18;
				size_CNode = 0x58;
				break;
			default:
				assert(false);
			}
		});

	uintptr_t pDispatchRestore;
	auto fDispatchRestore = FindAsync(
		pDispatchRestore,
		patterns::server::DispatchRestore,
		[&](auto pattern) {
			pGlobalState = *reinterpret_cast<void**>(pDispatchRestore + 153);
		});

	uintptr_t pMiddleOfCNihilanth__NextActivity;
	auto fMiddleOfCNihilanth__NextActivity = FindAsync(
		pMiddleOfCNihilanth__NextActivity,
		patterns::server::MiddleOfCNihilanth__NextActivity,
		[&](auto pattern) {
			switch (pattern - patterns::server::MiddleOfCNihilanth__NextActivity.cbegin()) {
			case 0: // HL-SteamPipe-Linux
				offNihilanthLevel = *reinterpret_cast<ptrdiff_t*>(pMiddleOfCNihilanth__NextActivity + 0x298);
				offNihilanthIrritation = *reinterpret_cast<ptrdiff_t*>(pMiddleOfCNihilanth__NextActivity + 0x21);
				offNihilanthRecharger = *reinterpret_cast<ptrdiff_t*>(pMiddleOfCNihilanth__NextActivity + 0x282);
				break;
			case 1: // HL-SteamPipe
				offNihilanthLevel = *reinterpret_cast<ptrdiff_t*>(pMiddleOfCNihilanth__NextActivity + 0x42);
				offNihilanthIrritation = *reinterpret_cast<ptrdiff_t*>(pMiddleOfCNihilanth__NextActivity - 0x1ab);
				offNihilanthRecharger = *reinterpret_cast<ptrdiff_t*>(pMiddleOfCNihilanth__NextActivity + 0x2d);
				break;
			case 2: // HL-WON
				offNihilanthLevel = *reinterpret_cast<ptrdiff_t*>(pMiddleOfCNihilanth__NextActivity + 0x40);
				offNihilanthIrritation = *reinterpret_cast<ptrdiff_t*>(pMiddleOfCNihilanth__NextActivity - 0x1ab);
				offNihilanthRecharger = *reinterpret_cast<ptrdiff_t*>(pMiddleOfCNihilanth__NextActivity + 0x2b);
				break;
			case 3: // Half-Payne
				offNihilanthLevel = *reinterpret_cast<ptrdiff_t *>(pMiddleOfCNihilanth__NextActivity + 0x44);
				offNihilanthIrritation = *reinterpret_cast<ptrdiff_t *>(pMiddleOfCNihilanth__NextActivity - 0x1a1);
				offNihilanthRecharger = *reinterpret_cast<ptrdiff_t *>(pMiddleOfCNihilanth__NextActivity + 0x2b);
				break;
			default:
				assert(false);
			}
		});

	uintptr_t pCNihilanth__EmitSphere;
	auto fCNihilanth__EmitSphere = FindAsync(
		pCNihilanth__EmitSphere,
		patterns::server::CNihilanth__EmitSphere,
		[&](auto pattern) {
			switch (pattern - patterns::server::CNihilanth__EmitSphere.cbegin()) {
			case 0: // HL-SteamPipe-Linux
				offNihilanthSpheres = *reinterpret_cast<ptrdiff_t*>(pCNihilanth__EmitSphere + 0x2f);
				break;
			case 1: // HL-SteamPipe
				offNihilanthSpheres = *reinterpret_cast<ptrdiff_t*>(pCNihilanth__EmitSphere + 0x15);
				break;
			case 2: // Half-Payne
				offNihilanthSpheres = *reinterpret_cast<ptrdiff_t *>(pCNihilanth__EmitSphere + 0x1a);
				break;
			default:
				assert(false);
			}
		});

	bool noBhopcap = false;
	{
		auto pattern = fPM_PreventMegaBunnyJumping.get();
		if (ORIG_PM_PreventMegaBunnyJumping) {
			if (pattern == patterns::shared::PM_PreventMegaBunnyJumping.cend())
				EngineDevMsg("[server dll] Found PM_PreventMegaBunnyJumping at %p.\n", ORIG_PM_PreventMegaBunnyJumping);
			else
				EngineDevMsg("[server dll] Found PM_PreventMegaBunnyJumping at %p (using the %s pattern).\n", ORIG_PM_PreventMegaBunnyJumping, pattern->name());
		} else {
			EngineDevWarning("[server dll] Could not find PM_PreventMegaBunnyJumping.\n");
			EngineWarning("Bhopcap disabling is not available.\n");
			noBhopcap = true;
		}
	}

	{
		auto pattern = fPM_Jump_CZDS_Velocity_Byte.get();
		if (pCZDS_Velocity_Byte) {
			EngineDevMsg("[server dll] Found CZDS Velocity Reset Byte at %p (using the %s pattern).\n", pCZDS_Velocity_Byte, pattern->name());
		} else {
			EngineDevWarning("[server dll] Could not find CZDS Velocity Reset Byte.\n");
		}
	}

	{
		auto pattern = fAddToFullPack_PVS_Byte.get();
		if (pAddToFullPack_PVS_Byte) {
			EngineDevMsg("[server dll] Found AddToFullPack PVS Byte at %p (using the %s pattern).\n", pAddToFullPack_PVS_Byte, pattern->name());
		} else {
			EngineDevWarning("[server dll] Could not find AddToFullPack PVS Byte.\n");
		}
	}

	{
		auto pattern = fPM_PlayerMove.get();
		if (ORIG_PM_PlayerMove) {
			if (pattern == patterns::shared::PM_PlayerMove.cend())
				EngineDevMsg("[server dll] Found PM_PlayerMove at %p.\n", ORIG_PM_PlayerMove);
			else
				EngineDevMsg("[server dll] Found PM_PlayerMove at %p (using the %s pattern).\n", ORIG_PM_PlayerMove, pattern->name());
		} else {
			EngineDevWarning("[server dll] Could not find PM_PlayerMove.\n");
		}
	}

	{
		auto pattern = fPM_Jump.get();
		auto pattern2 = fPM_Jump_Bhopcap_Windows.get();
		if (ORIG_PM_Jump) {
			if (pattern == patterns::shared::PM_Jump.cend())
				EngineDevMsg("[server dll] Found PM_Jump at %p.\n", ORIG_PM_Jump);
			else
				EngineDevMsg("[server dll] Found PM_Jump at %p (using the %s pattern).\n", ORIG_PM_Jump, pattern->name());
			if (offBhopcap)
				EngineDevMsg("[server dll] Found the bhopcap pattern at %p.\n", reinterpret_cast<void*>(offBhopcap + reinterpret_cast<uintptr_t>(ORIG_PM_Jump) - 27));
			if (pBhopcapWindows)
				EngineDevMsg("[server dll] Found bhopcap jump instruction at %p (using the %s pattern).\n", pBhopcapWindows, pattern2->name());
		} else {
			EngineDevWarning("[server dll] Could not find PM_Jump.\n");
			EngineWarning("Autojump is not available.\n");
			if (!noBhopcap)
				EngineWarning("Bhopcap disabling is not available.\n");
		}

		if (!ppmove)
			ppmove = reinterpret_cast<void**>(MemUtils::GetSymbolAddress(m_Handle, "pmove"));
	}

	{
		auto pattern = fCTriggerVolume__Spawn.get();
		if (ORIG_CTriggerVolume__Spawn) {
			EngineDevMsg("[server dll] Found CTriggerVolume::Spawn at %p (using the %s pattern).\n", ORIG_CTriggerVolume__Spawn, pattern->name());
		} else {
			ORIG_CTriggerVolume__Spawn_Linux = reinterpret_cast<_CTriggerVolume__Spawn_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN14CTriggerVolume5SpawnEv"));
			if (ORIG_CTriggerVolume__Spawn_Linux) {
				EngineDevMsg("[server dll] Found CTriggerVolume::Spawn [Linux] at %p.\n", ORIG_CTriggerVolume__Spawn_Linux);
			} else {
				EngineDevWarning("[server dll] Could not find CTriggerVolume::Spawn.\n");
				EngineWarning("trigger_transition entities will not be displayed.\n");
			}
		}
	}

	{
		auto pattern = fCBasePlayer__ForceClientDllUpdate.get();
		if (ORIG_CBasePlayer__ForceClientDllUpdate) {
			EngineDevMsg("[server dll] Found CBasePlayer::ForceClientDllUpdate at %p (using the %s pattern).\n", ORIG_CBasePlayer__ForceClientDllUpdate, pattern->name());
		} else {
			ORIG_CBasePlayer__ForceClientDllUpdate_Linux = reinterpret_cast<_CBasePlayer__ForceClientDllUpdate_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN11CBasePlayer20ForceClientDllUpdateEv"));
			if (ORIG_CBasePlayer__ForceClientDllUpdate_Linux) {
				maxAmmoSlots = MAX_AMMO_SLOTS;
				offm_rgAmmoLast = 0x56C; // 6153: 0x568
				offm_iClientFOV = 0x4C4; // 6153: 0x4C0
				if (ClientDLL::GetInstance().DoesGameDirMatch("czeror")) {
					offm_rgAmmoLast = 0x554;
					offm_iClientFOV = 0x8B0;
					offFuncIsPlayer = 0xA8;
					offFuncCenter = 0xDC;
					offFuncObjectCaps = 0x18;
				}
				if (ClientDLL::GetInstance().DoesGameDirMatch("bshift") || ClientDLL::GetInstance().DoesGameDirMatch("bshift_cutsceneless")) {
					offm_rgAmmoLast = 0x568;
					offm_iClientFOV = 0x4C0;
				}
				if (ClientDLL::GetInstance().DoesGameDirMatch("cstrike") || ClientDLL::GetInstance().DoesGameDirMatch("czero")) {
					offm_rgAmmoLast = 0x674;
					offm_iClientFOV = 0x5C4;
					offFuncIsPlayer = 0xA0;
					offFuncCenter = 0xCC;
					offFuncObjectCaps = 0x18;
				}
				if (ClientDLL::GetInstance().DoesGameDirMatch("tfc")) {
					offm_rgAmmoLast = 0x98C;
					offm_iClientFOV = 0x8E0;
					offFuncObjectCaps = 0x1C;
				}
				EngineDevMsg("[server dll] Found CBasePlayer::ForceClientDllUpdate [Linux] at %p.\n", ORIG_CBasePlayer__ForceClientDllUpdate_Linux);
			} else {
				EngineDevWarning("[server dll] Could not find CBasePlayer::ForceClientDllUpdate.\n");
				EngineWarning("Ammo HUD reset prevention is not available.\n");
			}
		}
	}

	{
		auto pattern = fCPushable__Move.get();
		if (ORIG_CPushable__Move) {
			if (pattern == patterns::server::CPushable__Move.cend())
				EngineDevMsg("[server dll] Found CPushable::Move at %p.\n", ORIG_CPushable__Move);
			else
				EngineDevMsg("[server dll] Found CPushable::Move at %p (using the %s pattern).\n", ORIG_CPushable__Move, pattern->name());
		} else {
			ORIG_CPushable__Move_Linux = reinterpret_cast<_CPushable__Move_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN9CPushable4MoveEP11CBaseEntityi"));
			if (ORIG_CPushable__Move_Linux)
				EngineDevMsg("[server dll] Found CPushable::Move [Linux] at %p.\n", ORIG_CPushable__Move_Linux);
			else {
				EngineDevWarning("[server dll] Could not find CPushable::Move.\n");
				EngineWarning("Object boost logging is not available.\n");
			}
		}
	}

	{
		auto pattern = fCBasePlayer__TakeDamage.get();
		if (ORIG_CBasePlayer__TakeDamage) {
			EngineDevMsg("[server dll] Found CBasePlayer::TakeDamage at %p (using the %s pattern).\n", ORIG_CBasePlayer__TakeDamage, pattern->name());
		} else {
			ORIG_CBasePlayer__TakeDamage_Linux = reinterpret_cast<_CBasePlayer__TakeDamage_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN11CBasePlayer10TakeDamageEP9entvars_sS1_fi"));
			if (ORIG_CBasePlayer__TakeDamage_Linux) {
				EngineDevMsg("[server dll] Found CBasePlayer::TakeDamage_Linux at %p.\n", ORIG_CBasePlayer__TakeDamage_Linux);
			} else {
				EngineDevWarning("[server dll] Could not find CBasePlayer::TakeDamage.\n");
				EngineWarning("Damage logging is not available.\n");
			}
		}
	}

	{
		auto pattern = fCGraph__InitGraph.get();
		if (ORIG_CGraph__InitGraph) {
			EngineDevMsg("[server dll] Found CGraph::InitGraph at %p (using the %s pattern).\n", ORIG_CGraph__InitGraph, pattern->name());
		} else {
			ORIG_CGraph__InitGraph_Linux = reinterpret_cast<_CGraph__InitGraph_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN6CGraph9InitGraphEv"));
			if (ORIG_CGraph__InitGraph_Linux) {
				offm_pNodes = 0x0C;
				offm_vecOrigin = 0x00;
				offm_cNodes = 0x18;
				size_CNode = 0x58;

				EngineDevMsg("[server dll] Found CGraph::InitGraph [Linux] at %p.\n", ORIG_CGraph__InitGraph_Linux);
			} else {
				EngineDevWarning("[server dll] Could not find CGraph::InitGraph.\n");
				EngineWarning("AI node display is not available.\n");
			}
		}
	}

	{
		auto pattern = fMiddleOfCNihilanth__NextActivity.get();
		if (pMiddleOfCNihilanth__NextActivity) {
			EngineDevMsg("[server dll] Found the CNihilanth::NextActivity pattern at %p (using the %s pattern).\n", pMiddleOfCNihilanth__NextActivity, pattern->name());
		} else {
			EngineDevWarning("[server dll] Could not find the CNihilanth::NextActivity pattern.\n");
			EngineWarning("bxt_hud_nihilanth is not available.\n");
		}
	}

	{
		auto pattern = fCNihilanth__EmitSphere.get();
		if (pCNihilanth__EmitSphere) {
			EngineDevMsg("[server dll] Found CNihilanth::EmitSphere at %p (using the %s pattern).\n", pCNihilanth__EmitSphere, pattern->name());
		} else {
			EngineDevWarning("[server dll] Could not find CNihilanth::EmitSphere.\n");
			EngineWarning("bxt_hud_nihilanth is not available.\n");
		}
	}

	{
		auto pattern = fCBasePlayer__CheatImpulseCommands.get();
		if (ORIG_CBasePlayer__CheatImpulseCommands) {
			EngineDevMsg("[server dll] Found CBasePlayer::CheatImpulseCommands at %p (using the %s pattern).\n", ORIG_CBasePlayer__CheatImpulseCommands, pattern->name());
		} else {
			ORIG_CBasePlayer__CheatImpulseCommands_Linux = reinterpret_cast<_CBasePlayer__CheatImpulseCommands_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN11CBasePlayer20CheatImpulseCommandsEi"));
			if (ORIG_CBasePlayer__CheatImpulseCommands_Linux) {
				EngineDevMsg("[server dll] Found CBasePlayer::CheatImpulseCommands [Linux] at %p.\n", ORIG_CBasePlayer__CheatImpulseCommands_Linux);
			} else {
				EngineDevWarning("[server dll] Could not find CBasePlayer::CheatImpulseCommands.\n");
			}
		}
	}

	{
		auto pattern = fCBaseMonster__Killed.get();
		if (ORIG_CBaseMonster__Killed) {
			EngineDevMsg("[server dll] Found CBaseMonster::Killed at %p (using the %s pattern).\n", ORIG_CBaseMonster__Killed, pattern->name());
		}
		else {
			EngineDevWarning("[server dll] Could not find CBaseMonster::Killed.\n");
			EngineWarning("Wanted! and Crowbar of Time automatic timer stopping is not available.\n");
		}
	}

	ORIG_CBaseDoor__DoorHitTop = reinterpret_cast<_CBaseDoor__DoorHitTop>(MemUtils::GetSymbolAddress(m_Handle, "?DoorHitTop@CBaseDoor@@QAEXXZ"));
	{
		auto pattern = fCChangeLevel__InTransitionVolume.get();
		if (ORIG_CBaseDoor__DoorHitTop && ORIG_CChangeLevel__InTransitionVolume) {
			EngineDevMsg("[server dll] Found CBaseDoor::DoorHitTop at %p.\n", ORIG_CBaseDoor__DoorHitTop);
			EngineDevMsg("[server dll] Found CChangeLevel::InTransitionVolume at %p (using the %s pattern).\n", ORIG_CChangeLevel__InTransitionVolume, pattern->name());
		} else {
			EngineDevWarning("[server dll] Could not find CBaseDoor::DoorHitTop and CChangeLevel::InTransitionVolume.\n");
			EngineWarning("The Xeno Project automatic timer stopping is not available.\n");
			ORIG_CBaseDoor__DoorHitTop = nullptr;
			ORIG_CChangeLevel__InTransitionVolume = nullptr;
		}
	}

	ORIG_CChangeLevel__UseChangeLevel = reinterpret_cast<_CChangeLevel__UseChangeLevel>(MemUtils::GetSymbolAddress(m_Handle, "?UseChangeLevel@CChangeLevel@@QAEXPAVCBaseEntity@@0W4USE_TYPE@@M@Z"));
	ORIG_CChangeLevel__TouchChangeLevel = reinterpret_cast<_CChangeLevel__TouchChangeLevel>(MemUtils::GetSymbolAddress(m_Handle, "?TouchChangeLevel@CChangeLevel@@QAEXPAVCBaseEntity@@@Z"));
	{
		if (ORIG_CChangeLevel__UseChangeLevel && ORIG_CChangeLevel__TouchChangeLevel) {
			EngineDevMsg("[server dll] Found CChangeLevel::UseChangeLevel at %p.\n", ORIG_CChangeLevel__UseChangeLevel);
			EngineDevMsg("[server dll] Found CChangeLevel::TouchChangeLevel at %p.\n", ORIG_CChangeLevel__TouchChangeLevel);
		} else {
			EngineDevWarning("[server dll] Could not find CChangeLevel::UseChangeLevel and CChangeLevel::TouchChangeLevel.\n");
			EngineWarning("bxt_disable_changelevel is not available.\n");
		}
	}

	ORIG_CTriggerEndSection__EndSectionUse = reinterpret_cast<_CTriggerEndSection__EndSectionUse>(MemUtils::GetSymbolAddress(m_Handle, "?EndSectionUse@CTriggerEndSection@@QAEXPAVCBaseEntity@@0W4USE_TYPE@@M@Z"));
	ORIG_CTriggerEndSection__EndSectionTouch = reinterpret_cast<_CTriggerEndSection__EndSectionTouch>(MemUtils::GetSymbolAddress(m_Handle, "?EndSectionTouch@CTriggerEndSection@@QAEXPAVCBaseEntity@@@Z"));
	{
		if (ORIG_CTriggerEndSection__EndSectionUse && ORIG_CTriggerEndSection__EndSectionTouch) {
			EngineDevMsg("[server dll] Found CTriggerEndSection::EndSectionUse at %p.\n", ORIG_CTriggerEndSection__EndSectionUse);
			EngineDevMsg("[server dll] Found CTriggerEndSection::EndSectionTouch at %p.\n", ORIG_CTriggerEndSection__EndSectionTouch);
		} else {
			EngineDevWarning("[server dll] Could not find CTriggerEndSection::EndSectionUse and CTriggerEndSection::EndSectionTouch.\n");
			EngineWarning("trigger_endsection automatic timer stopping is not available (e.g.: for Hazard Course).\n");
		}
	}

	ORIG_CmdStart = reinterpret_cast<_CmdStart>(MemUtils::GetSymbolAddress(m_Handle, "_Z8CmdStartPK7edict_sPK9usercmd_sj"));
	ORIG_AddToFullPack = reinterpret_cast<_AddToFullPack>(MemUtils::GetSymbolAddress(m_Handle, "_Z13AddToFullPackP14entity_state_siP7edict_sS2_iiPh"));
	ORIG_ClientCommand = reinterpret_cast<_ClientCommand>(MemUtils::GetSymbolAddress(m_Handle, "_Z13ClientCommandP7edict_s"));
	ORIG_PM_Move = reinterpret_cast<_PM_Move>(MemUtils::GetSymbolAddress(m_Handle, "PM_Move"));

	if (ORIG_CmdStart && ORIG_AddToFullPack && ORIG_ClientCommand && ORIG_PM_Move) {
		EngineDevMsg("[server dll] Found CmdStart at %p.\n", ORIG_CmdStart);
		EngineDevMsg("[server dll] Found AddToFullPack at %p.\n", ORIG_AddToFullPack);
		EngineDevMsg("[server dll] Found ClientCommand at %p.\n", ORIG_ClientCommand);
		EngineDevMsg("[server dll] Found PM_Move at %p.\n", ORIG_PM_Move);
	} else {
		ORIG_GetEntityAPI = reinterpret_cast<_GetEntityAPI>(MemUtils::GetSymbolAddress(m_Handle, "GetEntityAPI"));
		if (ORIG_GetEntityAPI) {
			DLL_FUNCTIONS funcs;
			if (ORIG_GetEntityAPI(&funcs, 140)) {
				// Gets our hooked addresses on Windows.
				ORIG_CmdStart = funcs.pfnCmdStart;
				ORIG_AddToFullPack = funcs.pfnAddToFullPack;
				ORIG_ClientCommand = funcs.pfnClientCommand;
				ORIG_PM_Move = funcs.pfnPM_Move;
				EngineDevMsg("[server dll] Found CmdStart at %p.\n", ORIG_CmdStart);
				EngineDevMsg("[server dll] Found AddToFullPack at %p.\n", ORIG_AddToFullPack);
				EngineDevMsg("[server dll] Found ClientCommand at %p.\n", ORIG_ClientCommand);
				EngineDevMsg("[server dll] Found PM_Move at %p.\n", ORIG_PM_Move);
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
	if (ORIG_CNihilanth__DyingThink) {
		EngineDevMsg("[server dll] Found CNihilanth::DyingThink at %p.\n", ORIG_CNihilanth__DyingThink);
	} else {
		ORIG_CNihilanth__DyingThink_Linux = reinterpret_cast<_CNihilanth__DyingThink_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN10CNihilanth10DyingThinkEv"));
		if (ORIG_CNihilanth__DyingThink_Linux)
			EngineDevMsg("[server dll] Found CNihilanth::DyingThink [Linux] at %p.\n", ORIG_CNihilanth__DyingThink_Linux);
		else {
			EngineDevWarning("[server dll] Could not find CNihilanth::DyingThink.\n");
			EngineWarning("Nihilanth automatic timer stopping is not available.\n");
		}
	}

	ORIG_COFGeneWorm__DyingThink = reinterpret_cast<_COFGeneWorm__DyingThink>(MemUtils::GetSymbolAddress(m_Handle, "?DyingThink@COFGeneWorm@@QAEXXZ"));
	if (ORIG_COFGeneWorm__DyingThink) {
		EngineDevMsg("[server dll] Found COFGeneWorm::DyingThink at %p.\n", ORIG_COFGeneWorm__DyingThink);
	} else {
		ORIG_COFGeneWorm__DyingThink_Linux = reinterpret_cast<_COFGeneWorm__DyingThink_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN11COFGeneWorm10DyingThinkEv"));
		if (ORIG_COFGeneWorm__DyingThink_Linux) {
			EngineDevMsg("[server dll] Found COFGeneWorm::DyingThink [Linux] at %p.\n", ORIG_COFGeneWorm__DyingThink_Linux);
			maxAmmoSlots = MAX_AMMO_SLOTS;
			offm_rgAmmoLast = 0x61c; // 6153: 0x618
			offm_iClientFOV = 0x4f8; // 6153: 0x4f4
		} else {
			EngineDevWarning("[server dll] Could not find COFGeneWorm::DyingThink.\n");
			EngineWarning("Gene Worm automatic timer stopping is not available.\n");
		}
	}

	ORIG_CApache__DyingThink = reinterpret_cast<_CApache__DyingThink>(MemUtils::GetSymbolAddress(m_Handle, "?DyingThink@CApache@@AAEXXZ"));
	if (ORIG_CApache__DyingThink) {
		EngineDevMsg("[server dll] Found CApache::DyingThink at %p.\n", ORIG_CApache__DyingThink);
	} else {
		EngineDevWarning("[server dll] Could not find CApache::DyingThink.\n");
		EngineWarning("They Hunger Episode 3 automatic timer stopping is not available.\n");
	}

	ORIG_CBaseDoor__DoorGoUp = reinterpret_cast<_CBaseDoor__DoorGoUp>(MemUtils::GetSymbolAddress(m_Handle, "?DoorGoUp@CBaseDoor@@QAEXXZ"));
	if (ORIG_CBaseDoor__DoorGoUp) {
		EngineDevMsg("[server dll] Found CBaseDoor::DoorGoUp at %p.\n", ORIG_CBaseDoor__DoorGoUp);
	} else {
		EngineDevWarning("[server dll] Could not find CBaseDoor::DoorGoUp.\n");
		EngineWarning("They Hunger Episode 2 automatic timer stopping is not available.\n");
	}

	ORIG_CTriggerSave__SaveTouch = reinterpret_cast<_CTriggerSave__SaveTouch>(MemUtils::GetSymbolAddress(m_Handle, "?SaveTouch@CTriggerSave@@QAEXPAVCBaseEntity@@@Z"));
	if (ORIG_CTriggerSave__SaveTouch) {
		EngineDevMsg("[server dll] Found CTriggerSave::SaveTouch at %p.\n", ORIG_CTriggerSave__SaveTouch);
	}
	else {
		ORIG_CTriggerSave__SaveTouch_Linux = reinterpret_cast<_CTriggerSave__SaveTouch_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN12CTriggerSave9SaveTouchEP11CBaseEntity"));
		if (ORIG_CTriggerSave__SaveTouch_Linux)
			EngineDevMsg("[server dll] Found CTriggerSave::SaveTouch [Linux] at %p.\n", ORIG_CTriggerSave__SaveTouch_Linux);
		else {
			EngineDevWarning("[server dll] Could not find CTriggerSave::SaveTouch.\n");
			EngineWarning("bxt_disable_autosave is not available.\n");
		}
	}

	ORIG_CMultiManager__ManagerThink = reinterpret_cast<_CMultiManager__ManagerThink>(MemUtils::GetSymbolAddress(m_Handle, "?ManagerThink@CMultiManager@@QAEXXZ"));
	if (ORIG_CMultiManager__ManagerThink) {
		EngineDevMsg("[server dll] Found CMultiManager::ManagerThink at %p.\n", ORIG_CMultiManager__ManagerThink);
	} else {
		// https://github.com/YaLTeR/BunnymodXT/issues/63 <- because of this issue FireTargets is hooked on Linux instead, which is what MM::Think calls anyway
		ORIG_FireTargets_Linux = reinterpret_cast<_FireTargets_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_Z11FireTargetsPKcP11CBaseEntityS2_8USE_TYPEf"));
		if (ORIG_FireTargets_Linux)
			EngineDevMsg("[server dll] Found FireTargets [Linux] at %p.\n", ORIG_FireTargets_Linux);
		else {
			EngineDevWarning("[server dll] Could not find FireTargets or CMultiManager::ManagerUse.\n");
			EngineWarning("Blue Shift and Gunman Chronicles automatic timer stopping is not available.\n");
		}
	}

	{
		auto pattern = fPM_ClipVelocity.get();
		if (ORIG_PM_ClipVelocity) {
			if (pattern == patterns::shared::PM_ClipVelocity.cend())
				EngineDevMsg("[server dll] Found PM_ClipVelocity at %p.\n", ORIG_PM_ClipVelocity);
			else
				EngineDevMsg("[server dll] Found PM_ClipVelocity at %p (using the %s pattern).\n", ORIG_PM_ClipVelocity, pattern->name());
		} else {
			EngineDevWarning("[server dll] Could not find PM_ClipVelocity.\n");
			EngineWarning("Velocity clip logging is not available.\n");
		}
	}

	{
		auto pattern = fPM_WaterMove.get();
		if (ORIG_PM_WaterMove) {
			if (pattern == patterns::shared::PM_WaterMove.cend())
				EngineDevMsg("[server dll] Found PM_WaterMove at %p.\n", ORIG_PM_WaterMove);
			else
				EngineDevMsg("[server dll] Found PM_WaterMove at %p (using the %s pattern).\n", ORIG_PM_WaterMove, pattern->name());
		} else {
			EngineDevWarning("[server dll] Could not find PM_WaterMove.\n");
			EngineWarning("Water frame logging is not available.\n");
		}
	}

	{
		auto pattern = fPM_WalkMove.get();
		if (ORIG_PM_WalkMove) {
			if (pattern == patterns::shared::PM_WalkMove.cend())
				EngineDevMsg("[server dll] Found PM_WalkMove at %p.\n", ORIG_PM_WalkMove);
			else
				EngineDevMsg("[server dll] Found PM_WalkMove at %p (using the %s pattern).\n", ORIG_PM_WalkMove, pattern->name());
		} else {
			EngineDevWarning("[server dll] Could not find PM_WalkMove.\n");
		}
	}

	{
		auto pattern = fPM_FlyMove.get();
		if (ORIG_PM_FlyMove) {
			if (pattern == patterns::shared::PM_FlyMove.cend())
				EngineDevMsg("[server dll] Found PM_FlyMove at %p.\n", ORIG_PM_FlyMove);
			else
				EngineDevMsg("[server dll] Found PM_FlyMove at %p (using the %s pattern).\n", ORIG_PM_FlyMove, pattern->name());
		} else {
			EngineDevWarning("[server dll] Could not find PM_FlyMove.\n");
		}
	}

	{
		auto pattern = fPM_AddToTouched.get();
		if (ORIG_PM_AddToTouched) {
			if (pattern == patterns::shared::PM_AddToTouched.cend())
				EngineDevMsg("[server dll] Found PM_AddToTouched at %p.\n", ORIG_PM_AddToTouched);
			else
				EngineDevMsg("[server dll] Found PM_AddToTouched at %p (using the %s pattern).\n", ORIG_PM_AddToTouched, pattern->name());
		} else {
			EngineDevWarning("[server dll] Could not find PM_AddToTouched.\n");
		}
	}

	{
		auto pattern = fPM_Ladder.get();
		if (ORIG_PM_Ladder) {
			if (pattern == patterns::shared::PM_Ladder.cend())
				EngineDevMsg("[server dll] Found PM_Ladder at %p.\n", ORIG_PM_Ladder);
			else
				EngineDevMsg("[server dll] Found PM_Ladder at %p (using the %s pattern).\n", ORIG_PM_Ladder, pattern->name());
		} else {
			EngineDevWarning("[server dll] Could not find PM_Ladder.\n");
			EngineWarning("TAS logging for onladder status is unavailable.\n");
		}
	}

	{
		auto pattern = fDispatchRestore.get();
		if (pDispatchRestore) {
			EngineDevMsg("[server dll] Found DispatchRestore at %p (using the %s pattern).\n", pDispatchRestore, pattern->name());
			EngineDevMsg("[server dll] Found gGlobalState at %p.\n", pGlobalState);
		} else {
			EngineDevWarning("[server dll] Could not find DispatchRestore.\n");
			EngineWarning("Blue Shift \"A Leap of Faith\" chapter autosplit is unavailable.\n");
		}
	}

	auto fCBaseEntity__FireBullets = FindAsync(ORIG_CBaseEntity__FireBullets, patterns::server::CBaseEntity__FireBullets);
	auto fCBaseEntity__FireBulletsPlayer = FindAsync(ORIG_CBaseEntity__FireBulletsPlayer, patterns::server::CBaseEntity__FireBulletsPlayer);

	{
		auto pattern = fCBaseEntity__FireBullets.get();
		if (ORIG_CBaseEntity__FireBullets) {
			EngineDevMsg("[server dll] Found CBaseEntity::FireBullets at %p (using the %s pattern).\n", ORIG_CBaseEntity__FireBullets, pattern->name());
		}
		else {
			ORIG_CBaseEntity__FireBullets_Linux = reinterpret_cast<_CBaseEntity__FireBullets_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN11CBaseEntity11FireBulletsEj6VectorS0_S0_fiiiP9entvars_s"));
			if (ORIG_CBaseEntity__FireBullets_Linux)
				EngineDevMsg("[server dll] Found CBaseEntity::FireBullets [Linux] at %p.\n", ORIG_CBaseEntity__FireBullets_Linux);
			else
				EngineDevWarning("[server dll] Could not find CBaseEntity::FireBullets.\n");
		}
	}

	{
		// Reversed the order, because the Windows pattern matches something entirely different in the Linux ServerDLL.
		ORIG_CBaseEntity__FireBulletsPlayer_Linux = reinterpret_cast<_CBaseEntity__FireBulletsPlayer_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN11CBaseEntity17FireBulletsPlayerEj6VectorS0_S0_fiiiP9entvars_si"));
		if (ORIG_CBaseEntity__FireBulletsPlayer_Linux)
			EngineDevMsg("[server dll] Found CBaseEntity::FireBulletsPlayer [Linux] at %p.\n", ORIG_CBaseEntity__FireBulletsPlayer_Linux);
		else
		{
			auto pattern = fCBaseEntity__FireBulletsPlayer.get();
			if (ORIG_CBaseEntity__FireBulletsPlayer) {
				EngineDevMsg("[server dll] Found CBaseEntity::FireBulletsPlayer at %p (using the %s pattern).\n", ORIG_CBaseEntity__FireBulletsPlayer, pattern->name());
			}
			else
				EngineDevWarning("[server dll] Could not find CBaseEntity::FireBulletsPlayer.\n");
		}
	}

	if (!pEngfuncs)
	{
		pEngfuncs = reinterpret_cast<enginefuncs_t*>(MemUtils::GetSymbolAddress(m_Handle, "g_engfuncs"));
		EngineDevMsg("[server dll] g_engfuncs [Linux] is %p.\n", pEngfuncs);
	}
}

void ServerDLL::RegisterCVarsAndCommands()
{
	EngineDevMsg("[server dll] Registering CVars.\n");

	#define REG(cvar) HwDLL::GetInstance().RegisterCVar(CVars::cvar)
	REG(bxt_timer_autostop);
	if (ORIG_PM_Jump) {
		REG(bxt_autojump);
		REG(bxt_autojump_priority);
	}
	if (!ORIG_PM_PreventMegaBunnyJumping && !pBhopcapWindows)
		HwDLL::GetInstance().SetCVarValue(CVars::bxt_bhopcap, "0");
	if (ORIG_AddToFullPack) {
		REG(bxt_show_hidden_entities);
		REG(bxt_show_triggers_legacy);
		REG(bxt_render_far_entities);
	}
	if (ORIG_CTriggerSave__SaveTouch || ORIG_CTriggerSave__SaveTouch_Linux)
		REG(bxt_disable_autosave);
	if (ORIG_CChangeLevel__UseChangeLevel && ORIG_CChangeLevel__TouchChangeLevel)
		REG(bxt_disable_changelevel);
	if (ORIG_PM_PlayerMove)
		REG(bxt_force_duck);
	if (ORIG_PM_PlayerMove && ORIG_PM_Jump)
		REG(bxt_force_jumpless);
	if (ORIG_CMultiManager__ManagerThink || ORIG_FireTargets_Linux) {
		REG(bxt_fire_on_mm_targetname);
		REG(bxt_fire_on_mm_command);
	}
	if ((ORIG_CBaseEntity__FireBullets && ORIG_CBaseEntity__FireBulletsPlayer) || (ORIG_CBaseEntity__FireBullets_Linux && ORIG_CBaseEntity__FireBulletsPlayer_Linux)) {
		REG(bxt_show_bullets);
		REG(bxt_show_bullets_enemy);
	}
	#undef REG
}

std::vector<const edict_t *> ServerDLL::GetUseableEntities(const Vector &origin, float radius) const
{
	std::vector<const edict_t *> entities;
	edict_t *pent = nullptr;

	for (;;) {
		pent = pEngfuncs->pfnFindEntityInSphere(pent, origin, radius);
		if (!pent || !pEngfuncs->pfnEntOffsetOfPEntity(pent))
			break;

		void *base = pent->pvPrivateData;
		auto ObjectCapsFunc = *reinterpret_cast<_ObjectCaps *>(*reinterpret_cast<uintptr_t *>(base) + offFuncObjectCaps);
		if (!(ObjectCapsFunc(base) & (FCAP_IMPULSE_USE | FCAP_CONTINUOUS_USE | FCAP_ONOFF_USE)))
			continue;

		entities.push_back(pent);
	}

	return entities;
}

HOOK_DEF_0(ServerDLL, void, __cdecl, PM_Jump)
{
	auto pmove = reinterpret_cast<uintptr_t>(*ppmove);
	int playerIndex = *reinterpret_cast<int*>(pmove + offPlayerIndex);

	int *onground = reinterpret_cast<int*>(pmove + offOnground);
	int orig_onground = *onground;

	int *oldbuttons = reinterpret_cast<int*>(pmove + offOldbuttons);
	int orig_oldbuttons = *oldbuttons;

	if (CVars::bxt_force_jumpless.GetBool())
		return;

	if (CVars::bxt_autojump.GetBool())
	{
		if ((orig_onground != -1) && !cantJumpNextTime[playerIndex]) {
			if (HwDLL::GetInstance().ducktap == false || (HwDLL::GetInstance().ducktap == true && CVars::bxt_autojump_priority.GetBool())) {
					*oldbuttons &= ~IN_JUMP;
			}
		}
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

	if (pCZDS_Velocity_Byte)
	{
		if (*reinterpret_cast<byte*>(pCZDS_Velocity_Byte) == !CVars::bxt_bhopcap.GetBool())
			MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCZDS_Velocity_Byte), 1, reinterpret_cast<const byte*>(CVars::bxt_bhopcap.GetBool() ? "\x01" : "\x00"));
	}

	if (pBhopcapWindows)
		MemUtils::ReplaceBytes(reinterpret_cast<void*>(pBhopcapWindows), 1, reinterpret_cast<const byte*>(CVars::bxt_bhopcap.GetBool() ? "\x83" : "\x82"));

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

void ServerDLL::LogPlayerMove(bool pre, uintptr_t pmove) const
{
	HwDLL &hwDLL = HwDLL::GetInstance();

	const float *velocity = reinterpret_cast<const float *>(pmove + offVelocity);
	const float *baseVelocity = reinterpret_cast<const float *>(pmove + offBasevelocity);
	const float *origin = reinterpret_cast<const float *>(pmove + offOrigin);
	const int *flags = reinterpret_cast<const int *>(pmove + offFlags);
	const bool *inDuck = reinterpret_cast<const bool *>(pmove + offInDuck);
	const int *groundEntity = reinterpret_cast<const int *>(pmove + offOnground);
	const int *waterLevel = reinterpret_cast<const int *>(pmove + offWaterlevel);

	if (pre)
		hwDLL.logWriter.StartPrePlayer();
	else
		hwDLL.logWriter.StartPostPlayer();

	hwDLL.logWriter.SetPosition(origin);
	hwDLL.logWriter.SetVelocity(velocity);
	hwDLL.logWriter.SetBaseVelocity(baseVelocity);
	hwDLL.logWriter.SetWaterLevel(*waterLevel);

	if (ORIG_PM_Ladder)
		hwDLL.logWriter.SetOnLadder(ORIG_PM_Ladder() != nullptr);

	hwDLL.logWriter.SetOnGround(*groundEntity != -1);

	if (*flags & FL_DUCKING)
		hwDLL.logWriter.SetDuckState(TASLogger::DUCKED);
	else if (*inDuck)
		hwDLL.logWriter.SetDuckState(TASLogger::INDUCK);
	else
		hwDLL.logWriter.SetDuckState(TASLogger::UNDUCKED);

	if (pre)
		hwDLL.logWriter.EndPrePlayer();
	else
		hwDLL.logWriter.EndPostPlayer();
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
	int *flags = reinterpret_cast<int*>(pmove + offFlags);

	static bool reset_waterjump_flag = false;

	if (CVars::bxt_force_jumpless.GetBool())
	{
		cmd->buttons &= ~IN_JUMP;
		*flags |= FL_WATERJUMP;
		reset_waterjump_flag = true;
	}

	if (!CVars::bxt_force_jumpless.GetBool() && reset_waterjump_flag)
	{
		*flags &= ~FL_WATERJUMP;
		reset_waterjump_flag = false;
	}

	if (CVars::bxt_force_duck.GetBool())
		cmd->buttons |= IN_DUCK;

	#define ALERT(at, format, ...) pEngfuncs->pfnAlertMessage(at, const_cast<char*>(format), ##__VA_ARGS__)
	if (CVars::_bxt_taslog.GetBool() && pEngfuncs)
	{
		ALERT(at_console, "-- BXT TAS Log Start --\n");
		ALERT(at_console, "Player index: %d; msec: %hhu (%Lf)\n", playerIndex, cmd->msec, static_cast<long double>(cmd->msec) * 0.001);
		ALERT(at_console, "Angles: %.8f; %.8f; %.8f\n", angles[0], angles[1], angles[2]);
		ALERT(at_console, "Velocity: %.8f; %.8f; %.8f; origin: %.8f; %.8f; %.8f\n", velocity[0], velocity[1], velocity[2], origin[0], origin[1], origin[2]);
		ALERT(at_console, "Onground: %d; usehull: %d\n", *groundEntity, *reinterpret_cast<int*>(pmove + 0xBC));
	}

	HwDLL &hwDLL = HwDLL::GetInstance();
	if (hwDLL.IsTASLogging()) {
		hwDLL.logWriter.SetEntFriction(*reinterpret_cast<float *>(pmove + offEntFriction));
		hwDLL.logWriter.SetEntGravity(*reinterpret_cast<float *>(pmove + offEntGravity));
		const float *punchangles = reinterpret_cast<const float *>(pmove + offPunchangles);
		hwDLL.logWriter.SetPunchangles(punchangles[1], punchangles[0], punchangles[2]);
		LogPlayerMove(true, pmove);
	}

	ORIG_PM_PlayerMove(server);

	if (CVars::_bxt_taslog.GetBool() && pEngfuncs)
	{
		ALERT(at_console, "New velocity: %.8f; %.8f; %.8f; new origin: %.8f; %.8f; %.8f\n", velocity[0], velocity[1], velocity[2], origin[0], origin[1], origin[2]);
		ALERT(at_console, "New onground: %d; new usehull: %d\n", *groundEntity, *reinterpret_cast<int*>(pmove + 0xBC));
		ALERT(at_console, "-- BXT TAS Log End --\n");
	}
	#undef ALERT

	if (hwDLL.IsTASLogging()) {
		LogPlayerMove(false, pmove);
		hwDLL.logWriter.EndCmdFrame();
	}

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

	if (normal[2] != 1.0f && normal[2] != -1.0f)
		Interprocess::WriteClip(normal[2], in, out);

	return ret;
}

HOOK_DEF_0(ServerDLL, void, __cdecl, PM_WaterMove)
{
	Interprocess::WriteWater();

	return ORIG_PM_WaterMove();
}

HOOK_DEF_2(ServerDLL, bool, __cdecl, PM_AddToTouched, pmtrace_t, tr, float*, impactvelocity)
{
	const bool ret = ORIG_PM_AddToTouched(tr, impactvelocity);

	if (!HwDLL::GetInstance().IsTASLogging() || !callerIsFlyMove)
		return ret;

	TASLogger::Collision collision;
	collision.entity = tr.ent;
	collision.normal[0] = tr.plane.normal[0];
	collision.normal[1] = tr.plane.normal[1];
	collision.normal[2] = tr.plane.normal[2];
	collision.distance = tr.plane.dist;
	collision.impactVelocity[0] = impactvelocity[0];
	collision.impactVelocity[1] = impactvelocity[1];
	collision.impactVelocity[2] = impactvelocity[2];

	if (firstFlyMoveEnded)
		secondFlyMoveTouchQueue.push_back(collision);
	else
		firstFlyMoveTouchQueue.push_back(collision);

	return ret;
}

HOOK_DEF_0(ServerDLL, void, __cdecl, PM_WalkMove)
{
	if (!HwDLL::GetInstance().IsTASLogging()) {
		ORIG_PM_WalkMove();
		return;
	}

	firstFlyMoveTouchQueue.clear();
	secondFlyMoveTouchQueue.clear();

	firstFlyMoveEnded = false;
	callerIsWalkMove = true;
	ORIG_PM_WalkMove();
	callerIsWalkMove = false;

	// If PM_FlyMove wasn't called, then no clipping occurs.
	if (!firstFlyMoveEnded)
		return;

	const uintptr_t pmove = reinterpret_cast<uintptr_t>(*ppmove);
	const float *velocity = reinterpret_cast<const float *>(pmove + offVelocity);
	const float *origin = reinterpret_cast<const float *>(pmove + offOrigin);

	// Otherwise, PM_FlyMove must have been called twice, but the game obviously used only
	// one of the results. So, we need to determine which one of them was ultimately
	// used in order to get the correct collision results for logging.
	if (velocity[0] == firstFlyMoveEndVelocity[0] && velocity[1] == firstFlyMoveEndVelocity[1] && velocity[2] == firstFlyMoveEndVelocity[2]
		&& origin[0] == firstFlyMoveEndOrigin[0] && origin[1] == firstFlyMoveEndOrigin[1] && origin[2] == firstFlyMoveEndOrigin[2])
		HwDLL::GetInstance().logWriter.SetCollisions(firstFlyMoveTouchQueue);
	else
		HwDLL::GetInstance().logWriter.SetCollisions(secondFlyMoveTouchQueue);

	firstFlyMoveEnded = false;
}

HOOK_DEF_0(ServerDLL, void, __cdecl, PM_FlyMove)
{
	callerIsFlyMove = true;
	ORIG_PM_FlyMove();
	callerIsFlyMove = false;

	if (!HwDLL::GetInstance().IsTASLogging())
		return;

	if (!callerIsWalkMove) {
		HwDLL::GetInstance().logWriter.SetCollisions(firstFlyMoveTouchQueue);
		firstFlyMoveTouchQueue.clear();
		return;
	}

	if (!firstFlyMoveEnded) {
		firstFlyMoveEnded = true;

		const uintptr_t pmove = reinterpret_cast<uintptr_t>(*ppmove);
		const float *velocity = reinterpret_cast<const float *>(pmove + offVelocity);
		const float *origin = reinterpret_cast<const float *>(pmove + offOrigin);

		firstFlyMoveEndVelocity[0] = velocity[0];
		firstFlyMoveEndVelocity[1] = velocity[1];
		firstFlyMoveEndVelocity[2] = velocity[2];
		firstFlyMoveEndOrigin[0] = origin[0];
		firstFlyMoveEndOrigin[1] = origin[1];
		firstFlyMoveEndOrigin[2] = origin[2];
	}
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

	HwDLL &hwDLL = HwDLL::GetInstance();
	if (hwDLL.IsTASLogging()) {
		hwDLL.logWriter.StartCmdFrame(hwDLL.GetPreExecFramebulk(), cmd->msec, hwDLL.frametime_remainder ? *hwDLL.frametime_remainder : 0.0);
		hwDLL.logWriter.SetSharedSeed(seed);
		hwDLL.logWriter.SetButtons(cmd->buttons);
		hwDLL.logWriter.SetImpulse(cmd->impulse);
		hwDLL.logWriter.SetFSU(cmd->forwardmove, cmd->sidemove, cmd->upmove);
		hwDLL.logWriter.SetViewangles(cmd->viewangles[1], cmd->viewangles[0], cmd->viewangles[2]);
		hwDLL.logWriter.SetHealth(hwDLL.GetPlayerEdict()->v.health);
		hwDLL.logWriter.SetArmor(hwDLL.GetPlayerEdict()->v.armorvalue);
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
		ALERT(at_console, "Paused: %s\n", (hwDLL.IsPaused() ? "true" : "false"));
		ALERT(at_console, "-- CmdStart End --\n");
	}
	#undef ALERT

	return ORIG_CmdStart(player, cmd, seed);
}

HOOK_DEF_2(ServerDLL, void, __fastcall, CNihilanth__DyingThink, void*, thisptr, int, edx)
{
	DoAutoStopTasks();

	return ORIG_CNihilanth__DyingThink(thisptr, edx);
}

HOOK_DEF_1(ServerDLL, void, __cdecl, CNihilanth__DyingThink_Linux, void*, thisptr)
{
	DoAutoStopTasks();

	return ORIG_CNihilanth__DyingThink_Linux(thisptr);
}

HOOK_DEF_2(ServerDLL, void, __fastcall, COFGeneWorm__DyingThink, void*, thisptr, int, edx)
{
	DoAutoStopTasks();

	return ORIG_COFGeneWorm__DyingThink(thisptr, edx);
}

HOOK_DEF_1(ServerDLL, void, __cdecl, COFGeneWorm__DyingThink_Linux, void*, thisptr)
{
	DoAutoStopTasks();

	return ORIG_COFGeneWorm__DyingThink_Linux(thisptr);
}

HOOK_DEF_1(ServerDLL, void, __fastcall, CApache__DyingThink, void*, thisptr)
{
	if (HwDLL::GetInstance().ppGlobals) {
		entvars_t *pev = *reinterpret_cast<entvars_t**>(reinterpret_cast<uintptr_t>(thisptr) + 4);
		if (pev && pev->targetname) {
			const char *targetname = HwDLL::GetInstance().ppGlobals->pStringBase + pev->targetname;
			if (!std::strcmp(targetname, "sheriffs_chppr")) {
				DoAutoStopTasks();
			}
		}
	}

	return ORIG_CApache__DyingThink(thisptr);
}

HOOK_DEF_1(ServerDLL, void, __fastcall, CBaseDoor__DoorGoUp, void*, thisptr)
{
	if (HwDLL::GetInstance().ppGlobals) {
		entvars_t *pev = *reinterpret_cast<entvars_t**>(reinterpret_cast<uintptr_t>(thisptr) + 4);
		if (pev && pev->target) {
			const char *target = HwDLL::GetInstance().ppGlobals->pStringBase + pev->target;
			if (!std::strcmp(target, "oil_spouts1_mm")) {
				DoAutoStopTasks();
			}
		}
	}

	return ORIG_CBaseDoor__DoorGoUp(thisptr);
}

HOOK_DEF_1(ServerDLL, void, __fastcall, CBaseDoor__DoorHitTop, void*, thisptr)
{
	if (HwDLL::GetInstance().ppGlobals && pEngfuncs) {
		entvars_t *pev = *reinterpret_cast<entvars_t**>(reinterpret_cast<uintptr_t>(thisptr) + 4);
		edict_t *pPlayer = pEngfuncs->pfnPEntityOfEntIndex(1);
		if (pev && pev->targetname && pPlayer) {
			void *classPtr = pPlayer->pvPrivateData;
			char pVolumeName[] = "lm15";
			const char *targetname = HwDLL::GetInstance().ppGlobals->pStringBase + pev->targetname;
			const char *gameDir = "";

			if (ClientDLL::GetInstance().pEngfuncs)
				gameDir = ClientDLL::GetInstance().pEngfuncs->pfnGetGameDirectory();

			if (!std::strcmp(targetname, "rocket_dr") && !std::strcmp(gameDir, "lm_txp")
				&& ORIG_CChangeLevel__InTransitionVolume(classPtr, pVolumeName))
				DoAutoStopTasks();
		}
	}

	return ORIG_CBaseDoor__DoorHitTop(thisptr);
}

HOOK_DEF_4(ServerDLL, void, __fastcall, CBaseMonster__Killed, void*, thisptr, int, edx, entvars_t*, pevAttacker, int, iGib)
{
	if (HwDLL::GetInstance().ppGlobals) {
		entvars_t* pev = *reinterpret_cast<entvars_t**>(reinterpret_cast<uintptr_t>(thisptr) + 4);
		if (pev && pev->classname) {
			const char* classname = HwDLL::GetInstance().ppGlobals->pStringBase + pev->classname;
			const char* gameDir = "";
			if (ClientDLL::GetInstance().pEngfuncs)
				gameDir = ClientDLL::GetInstance().pEngfuncs->pfnGetGameDirectory();
			if ((!std::strcmp(classname, "monster_ramone") && !std::strcmp(gameDir, "wantedsp"))
				|| (!std::strcmp(classname, "monster_gargantua") && !std::strcmp(gameDir, "tetsu0_cot"))) {
				DoAutoStopTasks();
			}
		}
	}

	return ORIG_CBaseMonster__Killed(thisptr, edx, pevAttacker, iGib);
}

HOOK_DEF_2(ServerDLL, void, __fastcall, CMultiManager__ManagerThink, void*, thisptr, int, edx)
{
	if (HwDLL::GetInstance().ppGlobals) {
		entvars_t *pev = *reinterpret_cast<entvars_t**>(reinterpret_cast<uintptr_t>(thisptr) + 4);
		if (pev && pev->targetname) {
			const char *targetname = HwDLL::GetInstance().ppGlobals->pStringBase + pev->targetname;
			OnMultiManagerFired(targetname);
		}
	}

	return ORIG_CMultiManager__ManagerThink(thisptr, edx);
}

HOOK_DEF_5(ServerDLL, void, __cdecl, FireTargets_Linux, char*, targetName, void*, pActivator, void*, pCaller, int, useType, float, value)
{
	if (HwDLL::GetInstance().ppGlobals && targetName != NULL && pCaller) {
		entvars_t *pev = *reinterpret_cast<entvars_t**>(reinterpret_cast<uintptr_t>(pCaller) + 4);
		if(pev && pev->targetname)
		{
			const char *targetname = HwDLL::GetInstance().ppGlobals->pStringBase + pev->targetname;
			const char *classname = HwDLL::GetInstance().ppGlobals->pStringBase + pev->classname;
			// We first need to check if the pCaller is a multi_manager since FireTargets can be called by anyone
			if (!std::strcmp(classname, "multi_manager")) {
				OnMultiManagerFired(targetname);
			}
		}
	}

	return ORIG_FireTargets_Linux(targetName, pActivator, pCaller, useType, value);
}

void ServerDLL::OnMultiManagerFired(const char *targetname)
{
	const char *gameDir = "";
	if (ClientDLL::GetInstance().pEngfuncs)
		gameDir = ClientDLL::GetInstance().pEngfuncs->pfnGetGameDirectory();

	if(!std::strcmp(targetname, "roll_the_credits")
		|| !std::strcmp(targetname, "youwinmulti")
		|| !std::strcmp(targetname, "previctory_mm")
		|| !std::strcmp(targetname, "stairscene_mngr")
		|| !std::strcmp(targetname, "boot_radio_seq")
		|| !std::strcmp(targetname, "BLOOOM") // CSCZDS
		|| (!std::strcmp(targetname, "telmm") && !std::strcmp(gameDir, "biglolly")) // Big Lolly
		|| (!std::strcmp(targetname, "mm_player_camera1") && !std::strcmp(gameDir, "htc")) // HTC
		|| (!std::strcmp(targetname, "multimanager_1") && !std::strcmp(gameDir, "construction")) // Construction
		|| (!std::strcmp(targetname, "the_endgame_mm") && !std::strcmp(gameDir, "gloom")) // The Gloom
		|| (!std::strcmp(targetname, "endbox_mm0") && !std::strcmp(gameDir, "echoes")) // Echoes
		|| (!std::strcmp(targetname, "sendmm") && !std::strcmp(gameDir, "MINIMICUS"))  // Minimicus
		|| (!std::strcmp(targetname, "kill2") && !std::strcmp(gameDir, "before")) // Before
		|| (!std::strcmp(targetname, "tele_in") && !std::strcmp(gameDir, "plague")) // Plague
		|| (!std::strcmp(targetname, "exit_seq") && !std::strcmp(gameDir, "timeline2")) // Timeline 2
		|| (!std::strcmp(targetname, "spawn_garg_sci_mm") && !std::strcmp(gameDir, "SteamLink")) // Uplink
		|| (!std::strcmp(targetname, "fc_mm1") && !std::strcmp(gameDir, "hc")) // Hazardous Course 2
		|| (!std::strcmp(targetname, "medicosprey") && !std::strcmp(gameDir, "visitors"))) { // Visitors
		DoAutoStopTasks();
	}

	if (!CVars::bxt_fire_on_mm_targetname.IsEmpty() && !CVars::bxt_fire_on_mm_command.IsEmpty()) {
		if (!std::strcmp(targetname, CVars::bxt_fire_on_mm_targetname.GetString().c_str()))
		{
			std::ostringstream ss;
			ss << CVars::bxt_fire_on_mm_command.GetString().c_str() << "\n";

			HwDLL::GetInstance().ORIG_Cbuf_InsertText(ss.str().c_str());
		}
	}
}

void ServerDLL::DoAutoStopTasks()
{
	if (CVars::bxt_timer_autostop.GetBool())
	{
		if (CustomHud::GetCountingTime())
			HwDLL::GetInstance().Called_Timer = true;

		CustomHud::SetCountingTime(false);
	}
	Interprocess::WriteGameEnd(CustomHud::GetTime());
	CustomHud::SaveTimeToDemo();
	RuntimeData::Add(RuntimeData::GameEndMarker{});
}

void ServerDLL::GetTriggerColor(const char *classname, bool inactive, bool additive, float &r, float &g, float &b, float &a)
{
	assert(std::strncmp(classname, "trigger_", 8) == 0);

	// The alpha should be lower in additive modes.
	constexpr std::array<std::array<float, 2>, 2> common_alphas{
		std::array<float, 2>{ 120.0f, 50.0f },
		std::array<float, 2>{ 50.0f, 20.0f }
	};

	classname += 8;
	if (std::strcmp(classname, "changelevel") == 0) {
		// Bright green
		r = 79;
		g = 255;
		b = 10;
		a = common_alphas[inactive][additive];
	} else if (std::strcmp(classname, "hurt") == 0) {
		// Red
		r = 255;
		g = 0;
		b = 0;
		a = common_alphas[inactive][additive];
	} else if (std::strcmp(classname, "multiple") == 0) {
		// Blue
		r = 0;
		g = 0;
		b = 255;
		a = common_alphas[inactive][additive];
	} else if (std::strcmp(classname, "once") == 0) {
		// Cyan
		r = 0;
		g = 255;
		b = 255;
		a = common_alphas[inactive][additive];
	} else if (std::strcmp(classname, "push") == 0) {
		// Bright yellow
		r = 255;
		g = 255;
		b = 0;
		a = common_alphas[inactive][additive];
	} else if (std::strcmp(classname, "teleport") == 0) {
		// Dull green
		r = 81;
		g = 147;
		b = 49;
		a = common_alphas[inactive][additive];
	} else if (std::strcmp(classname, "transition") == 0) {
		// Magenta
		r = 203;
		g = 103;
		b = 212;
		a = additive ? 50.0f : 120.0f;
	} else {
		// White
		r = 255;
		g = 255;
		b = 255;
		a = common_alphas[inactive][additive];
	}
}

HOOK_DEF_7(ServerDLL, int, __cdecl, AddToFullPack, struct entity_state_s*, state, int, e, edict_t*, ent, edict_t*, host, int, hostflags, int, player, unsigned char*, pSet)
{
	if (!HwDLL::GetInstance().ppGlobals) {
		return ORIG_AddToFullPack(state, e, ent, host, hostflags, player, pSet);
	}

	auto oldEffects = ent->v.effects;
	auto oldRendermode = ent->v.rendermode;
	auto oldRenderColor = ent->v.rendercolor;
	auto oldRenderAmount = ent->v.renderamt;
	auto oldRenderFx = ent->v.renderfx;
	auto oldFlags = ent->v.flags;
	auto oldIUser1 = ent->v.iuser1;
	auto oldIUser2 = ent->v.iuser2;

	static bool is_0x75 = false;

	if (pAddToFullPack_PVS_Byte)
	{
		if (CVars::bxt_render_far_entities.GetBool())
		{
			if (*reinterpret_cast<byte*>(pAddToFullPack_PVS_Byte) == 0x75)
				is_0x75 = true;

			if ((*reinterpret_cast<byte*>(pAddToFullPack_PVS_Byte) == 0x74) || (*reinterpret_cast<byte*>(pAddToFullPack_PVS_Byte) == 0x75))
				MemUtils::ReplaceBytes(reinterpret_cast<void*>(pAddToFullPack_PVS_Byte), 1, reinterpret_cast<const byte*>("\xEB"));
		}
		else if (*reinterpret_cast<byte*>(pAddToFullPack_PVS_Byte) == 0xEB)
		{
			if (is_0x75)
				MemUtils::ReplaceBytes(reinterpret_cast<void*>(pAddToFullPack_PVS_Byte), 1, reinterpret_cast<const byte*>("\x75"));
			else
				MemUtils::ReplaceBytes(reinterpret_cast<void*>(pAddToFullPack_PVS_Byte), 1, reinterpret_cast<const byte*>("\x74"));
		}
	}

	static bool is_czeror = ClientDLL::GetInstance().DoesGameSubDirMatch("czeror");

	if (is_czeror && CVars::bxt_render_far_entities.GetBool())
	{
		ent->v.flags |= FL_IMMUNE_LAVA; // Because the PVS check in AddToFullPack points to '524288' flags bit
		ent->v.iuser1 = 1; // Similar to above explanation
		ent->v.iuser2 = 1; // Mappers used on some entities 'nopvs = 1' keyvalue, which is 'iuser2 = 1` in game code
	}

	if (CVars::bxt_render_far_entities.GetInt() == 2 || (CVars::bxt_render_far_entities.GetBool() && spirit_sdk))
		ent->v.renderfx = 22; // kRenderFxEntInPVS from Spirit SDK

	const char *classname = HwDLL::GetInstance().ppGlobals->pStringBase + ent->v.classname;
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
	} else if (!is_trigger && CVars::bxt_show_hidden_entities_clientside.GetBool()) {
		if (ent->v.effects & EF_NODRAW)
		{
			ent->v.effects &= ~EF_NODRAW;
			ent->v.renderamt = 0;

			// e.g. func_wall_toggle is kRenderNormal when it's EF_NODRAW'd, so that'd make it visible always, fix that
			if (ent->v.rendermode == kRenderNormal)
				ent->v.rendermode = kRenderTransTexture;
		}
	}
	else if (is_trigger && CVars::bxt_show_triggers_legacy.GetBool()) {
		ent->v.effects &= ~EF_NODRAW;
		ent->v.rendermode = kRenderTransColor;
		if (ent->v.solid == SOLID_NOT && std::strcmp(classname + 8, "transition") != 0)
			ent->v.renderfx = kRenderNormal;
		else
			ent->v.renderfx = kRenderFxPulseFast;
		GetTriggerColor(classname, ent->v.solid == SOLID_NOT, false, ent->v.rendercolor.x, ent->v.rendercolor.y, ent->v.rendercolor.z, ent->v.renderamt);
	}

	auto ret = ORIG_AddToFullPack(state, e, ent, host, hostflags, player, pSet);

	ent->v.effects = oldEffects;
	ent->v.rendermode = oldRendermode;
	ent->v.rendercolor = oldRenderColor;
	ent->v.renderamt = oldRenderAmount;
	ent->v.renderfx = oldRenderFx;
	ent->v.flags = oldFlags;
	ent->v.iuser1 = oldIUser1;
	ent->v.iuser2 = oldIUser2;

	return ret;
}

HOOK_DEF_1(ServerDLL, void, __fastcall, CTriggerVolume__Spawn, void*, thisptr)
{
	if (!HwDLL::GetInstance().ppGlobals || !pEngfuncs) {
		ORIG_CTriggerVolume__Spawn(thisptr);
		return;
	}

	entvars_t *pev = *reinterpret_cast<entvars_t**>(reinterpret_cast<uintptr_t>(thisptr) + 4);
	string_t old_model = pev->model;
	ORIG_CTriggerVolume__Spawn(thisptr);
	pev->model = old_model;
	pev->modelindex = pEngfuncs->pfnModelIndex(HwDLL::GetInstance().ppGlobals->pStringBase + old_model);
	pev->effects |= EF_NODRAW;
}

HOOK_DEF_1(ServerDLL, void, __cdecl, CTriggerVolume__Spawn_Linux, void*, thisptr)
{
	if (!HwDLL::GetInstance().ppGlobals || !pEngfuncs) {
		ORIG_CTriggerVolume__Spawn_Linux(thisptr);
		return;
	}

	entvars_t *pev = *reinterpret_cast<entvars_t**>(reinterpret_cast<uintptr_t>(thisptr) + 4);
	string_t old_model = pev->model;
	ORIG_CTriggerVolume__Spawn_Linux(thisptr);
	pev->model = old_model;
	pev->modelindex = pEngfuncs->pfnModelIndex(HwDLL::GetInstance().ppGlobals->pStringBase + old_model);
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

bool ServerDLL::IsPlayerMovingPushable(const entvars_t *pevPushable, const entvars_t *pevToucher, int push) const
{
	if (!HwDLL::GetInstance().ppGlobals)
		return false;

	if (pevToucher->flags & FL_ONGROUND && pevToucher->groundentity && &pevToucher->groundentity->v == pevPushable)
		return false;

	void *pToucher = pevToucher->pContainingEntity->pvPrivateData;
	_IsPlayer IsPlayerFunc = *reinterpret_cast<_IsPlayer *>(*reinterpret_cast<uintptr_t *>(pToucher) + offFuncIsPlayer);
	if (!IsPlayerFunc(pToucher))
		return false;

	if (push && !(pevToucher->button & (IN_FORWARD | IN_USE)))
		return false;

	if (!(pevToucher->flags & FL_ONGROUND) && pevPushable->waterlevel < 1)
		return false;

	return true;
}

HOOK_DEF_4(ServerDLL, void, __fastcall, CPushable__Move, void*, thisptr, int, edx, void*, pOther, int, push)
{
	const entvars_t *pevToucher = *reinterpret_cast<entvars_t **>(reinterpret_cast<uintptr_t>(pOther) + 4);
	entvars_t *pevPushable = *reinterpret_cast<entvars_t **>(reinterpret_cast<uintptr_t>(thisptr) + 4);

	if (IsPlayerMovingPushable(pevPushable, pevToucher, push)) {
		if (!push)
			obboPushable = pevPushable;

		if (HwDLL::GetInstance().IsTASLogging()) {
			TASLogger::ObjectMove objectMove;
			objectMove.pull = !push;
			objectMove.velocity[0] = pevPushable->velocity[0];
			objectMove.velocity[1] = pevPushable->velocity[1];
			objectMove.velocity[2] = pevPushable->velocity[2];
			objectMove.position[0] = pevPushable->origin[0];
			objectMove.position[1] = pevPushable->origin[1];
			objectMove.position[2] = pevPushable->origin[2];
			HwDLL::GetInstance().logWriter.PushObjectMove(objectMove);
		}
	}

	ORIG_CPushable__Move(thisptr, edx, pOther, push);
}

HOOK_DEF_3(ServerDLL, void, __cdecl, CPushable__Move_Linux, void*, thisptr, void*, pOther, int, push)
{
	const entvars_t *pevToucher = *reinterpret_cast<entvars_t **>(reinterpret_cast<uintptr_t>(pOther) + 4);
	entvars_t *pevPushable = *reinterpret_cast<entvars_t **>(reinterpret_cast<uintptr_t>(thisptr) + 4);

	if (IsPlayerMovingPushable(pevPushable, pevToucher, push)) {
		if (!push)
			obboPushable = pevPushable;

		if (HwDLL::GetInstance().IsTASLogging()) {
			TASLogger::ObjectMove objectMove;
			objectMove.pull = !push;
			objectMove.velocity[0] = pevPushable->velocity[0];
			objectMove.velocity[1] = pevPushable->velocity[1];
			objectMove.velocity[2] = pevPushable->velocity[2];
			objectMove.position[0] = pevPushable->origin[0];
			objectMove.position[1] = pevPushable->origin[1];
			objectMove.position[2] = pevPushable->origin[2];
			HwDLL::GetInstance().logWriter.PushObjectMove(objectMove);
		}
	}

	return ORIG_CPushable__Move_Linux(thisptr, pOther, push);
}

void ServerDLL::DoWouldCrashMessage()
{
	char command[] = "say The game would have crashed. BXT prevented the crash, however this run is no longer valid.\n";
	// We send a say message from the ServerDLL so that we have the notice in the demo.
	pEngfuncs->pfnServerCommand(command);
	// Console
	pEngfuncs->pfnServerPrint("[BXT] The game would have crashed. BXT prevented the crash, however this run is no longer valid.\n");

	CustomHud::SaveTimeToDemo();
	CustomHud::SetInvalidRun(true);

	if (CustomHud::GetCountingTime())
		HwDLL::GetInstance().Called_Timer = true;

	// Some people might be running with LiveSplit only and hud_saytext CAN be 0, enable timer for those players, so they know
	if (!CVars::bxt_hud_timer.GetBool())
		HwDLL::GetInstance().SetCVarValue(CVars::bxt_hud_timer, "1");
}

HOOK_DEF_6(ServerDLL, int, __fastcall, CBasePlayer__TakeDamage, void*, thisptr, int, edx, entvars_t*, pevInflictor, entvars_t*, pevAttacker, float, flDamage, int, bitsDamageType)
{
	if (HwDLL::GetInstance().IsTASLogging()) {
		TASLogger::Damage damage;
		damage.damage = flDamage;
		damage.damageBits = bitsDamageType;

		if (!pevInflictor || !pEngfuncs->pfnEntOffsetOfPEntity(pevInflictor->pContainingEntity) || !pevInflictor->pContainingEntity->pvPrivateData) {
			damage.direction[0] = 0.0;
			damage.direction[1] = 0.0;
			damage.direction[2] = 0.0;
		} else {
			void *pInflictor = pevInflictor->pContainingEntity->pvPrivateData;
			_Center playerCenterFunc = *reinterpret_cast<_Center *>(*reinterpret_cast<uintptr_t *>(thisptr) + offFuncCenter);
			_Center inflictorCenterFunc = *reinterpret_cast<_Center *>(*reinterpret_cast<uintptr_t *>(pInflictor) + offFuncCenter);

			Vector playerCenter;
			Vector inflictorCenter;
			playerCenterFunc(thisptr, edx, &playerCenter);
			inflictorCenterFunc(pInflictor, edx, &inflictorCenter);
			Vector vecDir = playerCenter + Vector(0, 0, 10) - inflictorCenter;

			damage.direction[0] = vecDir.x;
			damage.direction[1] = vecDir.y;
			damage.direction[2] = vecDir.z;
		}

		HwDLL::GetInstance().logWriter.PushDamage(damage);
	}

	if (pevAttacker == nullptr)
	{
		DoWouldCrashMessage();
		return 0;
	}
	else
	{
		return ORIG_CBasePlayer__TakeDamage(thisptr, edx, pevInflictor, pevAttacker, flDamage, bitsDamageType);
	}
}

HOOK_DEF_5(ServerDLL, int, __cdecl, CBasePlayer__TakeDamage_Linux, void*, thisptr, entvars_t*, pevInflictor, entvars_t*, pevAttacker, float, flDamage, int, bitsDamageType)
{
	if (pevAttacker == nullptr)
	{
		DoWouldCrashMessage();
		return 0;
	}
	else
	{
		return ORIG_CBasePlayer__TakeDamage_Linux(thisptr, pevInflictor, pevAttacker, flDamage, bitsDamageType);
	}
}

HOOK_DEF_1(ServerDLL, void, __fastcall, CGraph__InitGraph, void*, thisptr)
{
	WorldGraph = thisptr;
	return ORIG_CGraph__InitGraph(thisptr);
}

HOOK_DEF_1(ServerDLL, void, __cdecl, CGraph__InitGraph_Linux, void*, thisptr)
{
	WorldGraph = thisptr;
	return ORIG_CGraph__InitGraph_Linux(thisptr);
}

HOOK_DEF_2(ServerDLL, void, __cdecl, PM_Move, struct playermove_s*, ppmove, int, server)
{
	auto pmove = reinterpret_cast<uintptr_t>(ppmove);
	auto origin = reinterpret_cast<float *>(pmove + offOrigin);
	auto flags = reinterpret_cast<int *>(pmove + offFlags);

	auto start_origin = Vector(origin);

	ORIG_PM_Move(ppmove, server);

	/*
	 * Assuming linear motion from start_origin to origin.
	 * This is not always the case but it is a good approximation.
	 */
	CustomTriggers::Update(start_origin, Vector(origin), (*flags & FL_DUCKING) != 0);
}

bool ServerDLL::GetGlobalState(const std::string& name, int& state)
{
	if (!pGlobalState)
		return false;

	for (uintptr_t entry = *reinterpret_cast<uintptr_t*>(pGlobalState);
		entry != 0;
		entry = *reinterpret_cast<uintptr_t*>(entry + 100)) {
		if (name == reinterpret_cast<char*>(entry)) {
			state = *reinterpret_cast<int*>(entry + 96);
			return true;
		}
	}

	return false;
}

float ServerDLL::GetTime()
{
	if (HwDLL::GetInstance().ppGlobals)
		return HwDLL::GetInstance().ppGlobals->time;

	return 0.0f;
}

std::vector<const Vector *> ServerDLL::GetNodePositions() const
{
	std::vector<const Vector *> positions;
	if (!WorldGraph) {
		return positions;
	}

	const int count = *reinterpret_cast<int *>(reinterpret_cast<uintptr_t>(WorldGraph) + offm_cNodes);
	uintptr_t nodes = *reinterpret_cast<uintptr_t *>(reinterpret_cast<uintptr_t>(WorldGraph) + offm_pNodes);
	for (int i = 0; i < count; ++i) {
		positions.push_back(reinterpret_cast<Vector *>(nodes + offm_vecOrigin));
		nodes += size_CNode;
	}

	return positions;
}

std::vector<const Vector *> ServerDLL::GetDisplacerTargets() const
{
	std::vector<const Vector *> targets;
	edict_t *pent = nullptr;

	for (;;) {
		pent = pEngfuncs->pfnFindEntityByString(pent, "classname", "info_displacer_earth_target");
		if (!pent || !pEngfuncs->pfnEntOffsetOfPEntity(pent))
			break;

		targets.push_back(&pent->v.origin);
	}

	return targets;
}

bool ServerDLL::GetNihilanthInfo(float &health, int &level, int &irritation, bool &recharger, int &nspheres, int &sequence, float &frame) const
{
	if (offNihilanthLevel == 0
		|| offNihilanthIrritation == 0
		|| offNihilanthRecharger == 0
		|| offNihilanthSpheres == 0)
		return false;

	// Assume there's only one nihilanth!
	edict_t *pent = pEngfuncs->pfnFindEntityByString(nullptr, "classname", "monster_nihilanth");
	if (!pent || !pEngfuncs->pfnEntOffsetOfPEntity(pent)) {
		return false;
	}

	health = pent->v.health;
	sequence = pent->v.sequence;
	frame = pent->v.frame;
	const auto pobj = reinterpret_cast<uintptr_t>(pent->pvPrivateData);
	level = *reinterpret_cast<int *>(pobj + offNihilanthLevel);
	irritation = *reinterpret_cast<int *>(pobj + offNihilanthIrritation);
	// sizeof(EHANDLE) is 8, like uint64_t
	recharger = *reinterpret_cast<uint64_t *>(pobj + offNihilanthRecharger) != 0;
	const auto *spheres = reinterpret_cast<uint64_t *>(pobj + offNihilanthSpheres);

	nspheres = 0;
	// Exactly 20 spheres in total
	for (int i = 0; i < 20; ++i) {
		if (spheres[i]) {
			++nspheres;
		}
	}

	return true;
}

bool ServerDLL::GetGonarchInfo(float &health, int& sequence, float& frame) const {
	// There should be only 1 big momma
	edict_t* pent = pEngfuncs->pfnFindEntityByString(nullptr, "classname", "monster_bigmomma");
	if (!pent || !pEngfuncs->pfnEntOffsetOfPEntity(pent)) {
		return false;
	}

	health = pent->v.health;
	sequence = pent->v.sequence;
	frame = pent->v.frame;

	return true;
}

TraceResult ServerDLL::TraceLine(const float v1[3], const float v2[3], int fNoMonsters, edict_t *pentToSkip) const
{
	TraceResult tr{};

	if (pEngfuncs && pentToSkip)
		pEngfuncs->pfnTraceLine(v1, v2, fNoMonsters, pentToSkip, &tr);

	return tr;
}

static float (*ORIG_CVarGetFloat) (const char* szVarName);
float fast_cvar_get_float(const char* name)
{
	if (!strcmp(name, "sv_cheats"))
		return CVars::sv_cheats.GetFloat();

	return ORIG_CVarGetFloat(name);
}

HOOK_DEF_3(ServerDLL, void, __fastcall, CBasePlayer__CheatImpulseCommands, void*, thisptr, int, edx, int, iImpulse)
{
	if (pEngfuncs) {
		ORIG_CVarGetFloat = pEngfuncs->pfnCVarGetFloat;
		pEngfuncs->pfnCVarGetFloat = fast_cvar_get_float;
	}

	ORIG_CBasePlayer__CheatImpulseCommands(thisptr, edx, iImpulse);

	if (pEngfuncs) {
		pEngfuncs->pfnCVarGetFloat = ORIG_CVarGetFloat;
	}
}

HOOK_DEF_2(ServerDLL, void, __cdecl, CBasePlayer__CheatImpulseCommands_Linux, void*, thisptr, int, iImpulse)
{
	if (pEngfuncs) {
		ORIG_CVarGetFloat = pEngfuncs->pfnCVarGetFloat;
		pEngfuncs->pfnCVarGetFloat = fast_cvar_get_float;
	}

	ORIG_CBasePlayer__CheatImpulseCommands_Linux(thisptr, iImpulse);

	if (pEngfuncs) {
		pEngfuncs->pfnCVarGetFloat = ORIG_CVarGetFloat;
	}
}

HOOK_DEF_3(ServerDLL, void, __fastcall, CTriggerSave__SaveTouch, void*, thisptr, int, edx, void*, pOther)
{
	if (CVars::bxt_disable_autosave.GetBool())
		return;

	return ORIG_CTriggerSave__SaveTouch(thisptr, edx, pOther);
}

HOOK_DEF_2(ServerDLL, void, __cdecl, CTriggerSave__SaveTouch_Linux, void*, thisptr, void*, pOther)
{
	if (CVars::bxt_disable_autosave.GetBool())
		return;

	return ORIG_CTriggerSave__SaveTouch_Linux(thisptr, pOther);
}

HOOK_DEF_6(ServerDLL, void, __fastcall, CChangeLevel__UseChangeLevel, void*, thisptr, int, edx, void*, pActivator, void*, pCaller, int, useType, float, value)
{
	if (CVars::bxt_disable_changelevel.GetBool())
		return;

	ServerDLL::GetInstance().ClearBulletsEnemyTrace();
	ServerDLL::GetInstance().ClearBulletsTrace();

	return ORIG_CChangeLevel__UseChangeLevel(thisptr, edx, pActivator, pCaller, useType, value);
}

HOOK_DEF_3(ServerDLL, void, __fastcall, CChangeLevel__TouchChangeLevel, void*, thisptr, int, edx, void*, pOther)
{
	if (CVars::bxt_disable_changelevel.GetBool())
		return;

	ServerDLL::GetInstance().ClearBulletsEnemyTrace();
	ServerDLL::GetInstance().ClearBulletsTrace();

	return ORIG_CChangeLevel__TouchChangeLevel(thisptr, edx, pOther);
}

void ServerDLL::TraceLineWrap(const Vector* vecStart, const Vector* vecEnd, int igmon, edict_t* pentIgnore, TraceResult* ptr)
{
	if (igmon || (!fireBullets_count && !fireBulletsPlayer_count))
		return;

	const char* SOLID_HIT_ENTITIES[] = {
		"func_breakable",
		"func_pushable"
	};

	bool hitSomething = !(ptr->pHit->v.solid == SOLID_BSP || ptr->pHit->v.movetype == MOVETYPE_PUSHSTEP);
	if (!hitSomething)
		for (size_t i = 0; i < std::size(SOLID_HIT_ENTITIES); i++)
		{
			hitSomething = !std::strcmp(HwDLL::GetInstance().ppGlobals->pStringBase + ptr->pHit->v.classname, SOLID_HIT_ENTITIES[i]);

			if (hitSomething)
				break;
		}

	if (fireBullets_count) {
		traceLineFireBullets.push_back({ Vector(*vecStart), Vector(ptr->vecEndPos) });
		traceLineFireBulletsHit.push_back(hitSomething);

		while (traceLineFireBullets.size() && traceLineFireBullets.size() > 20) {
			traceLineFireBullets.pop_front();
			traceLineFireBulletsHit.pop_front();
		}

		fireBullets_count--;
	} else if (fireBulletsPlayer_count) {
		traceLineFireBulletsPlayer.push_back({ Vector(*vecStart), Vector(ptr->vecEndPos) });
		traceLineFireBulletsPlayerHit.push_back(hitSomething);

		// 12 = mouse2 shotgun shot count
		while (traceLineFireBulletsPlayer.size() && traceLineFireBulletsPlayer.size() > 12) {
			traceLineFireBulletsPlayer.pop_front();
			traceLineFireBulletsPlayerHit.pop_front();
		}

		fireBulletsPlayer_count--;
	}
}

HOOK_DEF_11(ServerDLL, void, __fastcall, CBaseEntity__FireBullets, void*, thisptr, int, param1, unsigned long, cShots, Vector, vecSrc, Vector, vecDirShooting, Vector, vecSpread, float, flDistance, int, iBulletType, int, iTracerFreq, int, iDamage, entvars_t*, pevAttacker)
{
	fireBullets_count = cShots;
	ORIG_CBaseEntity__FireBullets(thisptr, param1, cShots, vecSrc, vecDirShooting, vecSpread, flDistance, iBulletType, iTracerFreq, iDamage, pevAttacker);
	// just in case
	fireBullets_count = 0;
}

HOOK_DEF_10(ServerDLL, void, __cdecl, CBaseEntity__FireBullets_Linux, void*, thisptr, unsigned long, cShots, Vector, vecSrc, Vector, vecDirShooting, Vector, vecSpread, float, flDistance, int, iBulletType, int, iTracerFreq, int, iDamage, entvars_t*, pevAttacker)
{
	fireBullets_count = cShots;
	ORIG_CBaseEntity__FireBullets_Linux(thisptr, cShots, vecSrc, vecDirShooting, vecSpread, flDistance, iBulletType, iTracerFreq, iDamage, pevAttacker);
	// just in case
	fireBullets_count = 0;
}

HOOK_DEF_13(ServerDLL, void, __fastcall, CBaseEntity__FireBulletsPlayer, void*, thisptr, int, edx, float, param1, unsigned long, cShots, Vector, vecSrc, Vector, vecDirShooting, Vector, vecSpread, float, flDistance, int, iBulletType, int, iTracerFreq, int, iDamage, entvars_t*, pevAttacker, int, shared_rand)
{
	fireBulletsPlayer_count = cShots;
	ORIG_CBaseEntity__FireBulletsPlayer(thisptr, edx, param1, cShots, vecSrc, vecDirShooting, vecSpread, flDistance, iBulletType, iTracerFreq, iDamage, pevAttacker, shared_rand);
	// just in case
	fireBulletsPlayer_count = 0;
}

HOOK_DEF_11(ServerDLL, Vector, __cdecl, CBaseEntity__FireBulletsPlayer_Linux,void*, thisptr, unsigned long, cShots, Vector, vecSrc, Vector, vecDirShooting, Vector, vecSpread, float, flDistance, int, iBulletType, int, iTracerFreq, int, iDamage, entvars_t*, pevAttacker, int, shared_rand)
{
	fireBulletsPlayer_count = cShots;
	auto ret = ORIG_CBaseEntity__FireBulletsPlayer_Linux(thisptr, cShots, vecSrc, vecDirShooting, vecSpread, flDistance, iBulletType, iTracerFreq, iDamage, pevAttacker, shared_rand);
	// just in case
	fireBulletsPlayer_count = 0;
	return ret;
}

const std::deque<std::array<Vector, 2>>& ServerDLL::GetBulletsEnemyTrace() const {
	return traceLineFireBullets;
}

const std::deque<std::array<Vector, 2>>& ServerDLL::GetBulletsPlayerTrace() const {
	return traceLineFireBulletsPlayer;
}

const std::deque<bool>& ServerDLL::GetBulletsEnemyTraceHit() const {
	return traceLineFireBulletsHit;
}

const std::deque<bool>& ServerDLL::GetBulletsPlayerTraceHit() const {
	return traceLineFireBulletsPlayerHit;
}

void ServerDLL::ClearBulletsEnemyTrace() {
	traceLineFireBullets.clear();
	traceLineFireBulletsHit.clear();
}

void ServerDLL::ClearBulletsTrace() {
	traceLineFireBulletsPlayer.clear();
	traceLineFireBulletsPlayerHit.clear();
}

HOOK_DEF_6(ServerDLL, void, __fastcall, CTriggerEndSection__EndSectionUse, void*, thisptr, int, edx, void*, pActivator, void*, pCaller, int, useType, float, value)
{
	// Smiley: HL mods may a player to switch between chapters with a manual select after disconnect, so don't stop timer if bxt_timer_autostop 1
	// and, naz said that it would useful to have such a extra mode for auto-stopping in own custom campaigns to not make a code changes
	if (CVars::bxt_timer_autostop.GetInt() == 2) {
		DoAutoStopTasks();
		return ORIG_CTriggerEndSection__EndSectionUse(thisptr, edx, pActivator, pCaller, useType, value);
	}

	return ORIG_CTriggerEndSection__EndSectionUse(thisptr, edx, pActivator, pCaller, useType, value);
}

HOOK_DEF_3(ServerDLL, void, __fastcall, CTriggerEndSection__EndSectionTouch, void*, thisptr, int, edx, void*, pOther)
{
	if (CVars::bxt_timer_autostop.GetInt() == 2) {
		DoAutoStopTasks();
		return ORIG_CTriggerEndSection__EndSectionTouch(thisptr, edx, pOther);
	}

	// trigger_endsection sends you to the menu, effectively stopping the demo,
	// but not the timer and neither LiveSplit of course, so we have to do it here
	if (HwDLL::GetInstance().ppGlobals) {
		entvars_t *pev = *reinterpret_cast<entvars_t**>(reinterpret_cast<uintptr_t>(thisptr) + 4);
		if (pev && pev->targetname) {
			const char *targetname = HwDLL::GetInstance().ppGlobals->pStringBase + pev->targetname;
			if (!std::strcmp(targetname, "tr_endchange")) {
				DoAutoStopTasks();
			}
		}
	}

	return ORIG_CTriggerEndSection__EndSectionTouch(thisptr, edx, pOther);
}
