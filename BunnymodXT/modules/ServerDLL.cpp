#include "../stdafx.hpp"

#include "../sptlib-wrapper.hpp"
#include <SPTLib/MemUtils.hpp>
#include <SPTLib/Hooks.hpp>
#include "ServerDLL.hpp"
#include "ClientDLL.hpp"
#include "HwDLL.hpp"
#include "../patterns.hpp"
#include "../cvars.hpp"
#include "../helper_functions.hpp"
#include "../hud_custom.hpp"
#include "../interprocess.hpp"
#include "../runtime_data.hpp"
#include "../custom_triggers.hpp"
#include "../splits.hpp"
#include "../shared.hpp"

#define ALERT(at, format, ...) pEngfuncs->pfnAlertMessage(at, const_cast<char*>(format), ##__VA_ARGS__)

// Linux hooks.
#ifndef _WIN32
extern "C" void __cdecl _Z8CmdStartPK7edict_sPK9usercmd_sj(const edict_t* player, const usercmd_t* cmd, unsigned int random_seed)
{
	return ServerDLL::HOOKED_CmdStart(player, cmd, random_seed);
}

extern "C" void __cdecl _Z6CmdEndPK7edict_s(const edict_t* player)
{
	return ServerDLL::HOOKED_CmdEnd(player);
}

extern "C" void __cdecl _ZN10CNihilanth10DyingThinkEv(void* thisptr)
{
	return ServerDLL::HOOKED_CNihilanth__DyingThink_Linux(thisptr);
}

extern "C" void __cdecl _ZN11COFGeneWorm10DyingThinkEv(void* thisptr)
{
	return ServerDLL::HOOKED_COFGeneWorm__DyingThink_Linux(thisptr);
}

extern "C" void __cdecl _ZN10CBreakable3DieEv(void* thisptr)
{
	return ServerDLL::HOOKED_CBreakable__Die_Linux(thisptr);
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

extern "C" void __cdecl _Z15PlayerPostThinkP7edict_s(edict_t* pEntity)
{
	return ServerDLL::HOOKED_PlayerPostThink(pEntity);
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

extern "C" int __cdecl _ZN11CBaseEntity9IsInWorldEv(void *thisptr)
{
	return ServerDLL::HOOKED_CBaseEntity__IsInWorld_Linux(thisptr);
}

extern "C" void __cdecl _ZN12CBaseTrigger13TeleportTouchEP11CBaseEntity(void* thisptr, void* pOther)
{
	return ServerDLL::HOOKED_CBaseTrigger__TeleportTouch_Linux(thisptr, pOther);
}

extern "C" void __cdecl _Z16DispatchKeyValueP7edict_sP14KeyValueData_s(edict_t* pentKeyvalue, KeyValueData* pkvd)
{
	return ServerDLL::HOOKED_DispatchKeyValue(pentKeyvalue, pkvd);
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
			ORIG_CmdEnd, HOOKED_CmdEnd,
			ORIG_CNihilanth__DyingThink, HOOKED_CNihilanth__DyingThink,
			ORIG_COFGeneWorm__DyingThink, HOOKED_COFGeneWorm__DyingThink,
			ORIG_CApache__DyingThink, HOOKED_CApache__DyingThink,
			ORIG_CBreakable__Die, HOOKED_CBreakable__Die,
			ORIG_CBaseDoor__DoorActivate, HOOKED_CBaseDoor__DoorActivate,
			ORIG_CBaseDoor__DoorGoUp, HOOKED_CBaseDoor__DoorGoUp,
			ORIG_CBaseDoor__DoorHitTop, HOOKED_CBaseDoor__DoorHitTop,
			ORIG_CMultiManager__ManagerThink, HOOKED_CMultiManager__ManagerThink,
			ORIG_AddToFullPack, HOOKED_AddToFullPack,
			ORIG_CTriggerVolume__Spawn, HOOKED_CTriggerVolume__Spawn,
			ORIG_ClientCommand, HOOKED_ClientCommand,
			ORIG_PlayerPostThink, HOOKED_PlayerPostThink,
			ORIG_CPushable__Move, HOOKED_CPushable__Move,
			ORIG_CBasePlayer__TakeDamage, HOOKED_CBasePlayer__TakeDamage,
			ORIG_CGraph__InitGraph, HOOKED_CGraph__InitGraph,
			ORIG_CBasePlayer__CheatImpulseCommands, HOOKED_CBasePlayer__CheatImpulseCommands,
			ORIG_CTriggerSave__SaveTouch, HOOKED_CTriggerSave__SaveTouch,
			ORIG_CChangeLevel__UseChangeLevel, HOOKED_CChangeLevel__UseChangeLevel,
			ORIG_CChangeLevel__TouchChangeLevel, HOOKED_CChangeLevel__TouchChangeLevel,
			ORIG_CTriggerCamera__FollowTarget, HOOKED_CTriggerCamera__FollowTarget,
			ORIG_CBaseMonster__Killed, HOOKED_CBaseMonster__Killed,
			ORIG_CBaseEntity__FireBullets, HOOKED_CBaseEntity__FireBullets,
			ORIG_CBaseEntity__FireBullets_Linux, HOOKED_CBaseEntity__FireBullets_Linux,
			ORIG_CBaseEntity__FireBulletsPlayer, HOOKED_CBaseEntity__FireBulletsPlayer,
			ORIG_CBaseEntity__FireBulletsPlayer_Linux, HOOKED_CBaseEntity__FireBulletsPlayer_Linux,
			ORIG_CBaseButton__ButtonUse, HOOKED_CBaseButton__ButtonUse,
			ORIG_CBaseButton__ButtonTriggerAndWait, HOOKED_CBaseButton__ButtonTriggerAndWait,
			ORIG_CBasePlayer__ViewPunch, HOOKED_CBasePlayer__ViewPunch,
			ORIG_CBasePlayer__Jump, HOOKED_CBasePlayer__Jump,
			ORIG_CTriggerEndSection__EndSectionUse, HOOKED_CTriggerEndSection__EndSectionUse,
			ORIG_CTriggerEndSection__EndSectionTouch, HOOKED_CTriggerEndSection__EndSectionTouch,
			ORIG_ShiftMonsters, HOOKED_ShiftMonsters,
			ORIG_PM_Duck, HOOKED_PM_Duck,
			ORIG_PM_UnDuck, HOOKED_PM_UnDuck,
			ORIG_CBaseEntity__IsInWorld, HOOKED_CBaseEntity__IsInWorld,
			ORIG_CBaseEntity__IsInWorld_Linux, HOOKED_CBaseEntity__IsInWorld_Linux,
			ORIG_CBaseTrigger__TeleportTouch, HOOKED_CBaseTrigger__TeleportTouch,
			ORIG_CBaseTrigger__TeleportTouch_Linux, HOOKED_CBaseTrigger__TeleportTouch_Linux,
			ORIG_DispatchKeyValue, HOOKED_DispatchKeyValue);
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
			ORIG_CmdEnd,
			ORIG_CNihilanth__DyingThink,
			ORIG_COFGeneWorm__DyingThink,
			ORIG_CApache__DyingThink,
			ORIG_CBreakable__Die,
			ORIG_CBaseDoor__DoorActivate,
			ORIG_CBaseDoor__DoorGoUp,
			ORIG_CBaseDoor__DoorHitTop,
			ORIG_CMultiManager__ManagerThink,
			ORIG_AddToFullPack,
			ORIG_CTriggerVolume__Spawn,
			ORIG_ClientCommand,
			ORIG_PlayerPostThink,
			ORIG_CPushable__Move,
			ORIG_CBasePlayer__TakeDamage,
			ORIG_CGraph__InitGraph,
			ORIG_CBasePlayer__CheatImpulseCommands,
			ORIG_CTriggerSave__SaveTouch,
			ORIG_CChangeLevel__UseChangeLevel,
			ORIG_CChangeLevel__TouchChangeLevel,
			ORIG_CTriggerCamera__FollowTarget,
			ORIG_CBaseEntity__FireBullets,
			ORIG_CBaseEntity__FireBullets_Linux,
			ORIG_CBaseEntity__FireBulletsPlayer,
			ORIG_CBaseEntity__FireBulletsPlayer_Linux,
			ORIG_CBaseMonster__Killed,
			ORIG_CBaseButton__ButtonUse,
			ORIG_CBaseButton__ButtonTriggerAndWait,
			ORIG_CBasePlayer__ViewPunch,
			ORIG_CBasePlayer__Jump,
			ORIG_CTriggerEndSection__EndSectionUse,
			ORIG_CTriggerEndSection__EndSectionTouch,
			ORIG_ShiftMonsters,
			ORIG_PM_Duck,
			ORIG_PM_UnDuck,
			ORIG_CBaseEntity__IsInWorld,
			ORIG_CBaseEntity__IsInWorld_Linux,
			ORIG_CBaseTrigger__TeleportTouch,
			ORIG_CBaseTrigger__TeleportTouch_Linux,
			ORIG_DispatchKeyValue);
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
	ORIG_CmdEnd = nullptr;
	ORIG_CNihilanth__DyingThink = nullptr;
	ORIG_CNihilanth__DyingThink_Linux = nullptr;
	ORIG_COFGeneWorm__DyingThink = nullptr;
	ORIG_COFGeneWorm__DyingThink_Linux = nullptr;
	ORIG_CApache__DyingThink = nullptr;
	ORIG_CBreakable__Die = nullptr;
	ORIG_CBreakable__Die_Linux = nullptr;
	ORIG_CBaseDoor__DoorActivate = nullptr;
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
	ORIG_PlayerPostThink = nullptr;
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
	ORIG_CTriggerCamera__FollowTarget = nullptr;
	ORIG_PM_CheckStuck = nullptr;
	ORIG_CBaseEntity__FireBullets = nullptr;
	ORIG_DispatchSpawn = nullptr;
	ORIG_DispatchTouch = nullptr;
	ORIG_DispatchKeyValue = nullptr;
	ORIG_CBaseEntity__FireBullets_Linux = nullptr;
	ORIG_CBaseEntity__FireBulletsPlayer = nullptr;
	ORIG_CBaseEntity__FireBulletsPlayer_Linux = nullptr;
	ORIG_CChangeLevel__InTransitionVolume = nullptr;
	ORIG_CBaseButton__ButtonUse = nullptr;
	ORIG_CBaseButton__ButtonTriggerAndWait = nullptr;
	ORIG_CBasePlayer__ViewPunch = nullptr;
	ORIG_CBasePlayer__Jump = nullptr;
	ORIG_CTriggerEndSection__EndSectionUse = nullptr;
	ORIG_CTriggerEndSection__EndSectionTouch = nullptr;
	ORIG_ShiftMonsters = nullptr;
	ORIG_PM_Duck = nullptr;
	ORIG_PM_UnDuck = nullptr;
	ORIG_CBaseEntity__IsInWorld = nullptr;
	ORIG_CBaseEntity__IsInWorld_Linux = nullptr;
	ORIG_CBaseTrigger__TeleportTouch = nullptr;
	ORIG_CBaseTrigger__TeleportTouch_Linux = nullptr;
	ppmove = nullptr;
	offPlayerIndex = 0;
	offOldbuttons = 0;
	offOnground = 0;
	offVelocity = 0;
	offOrigin = 0;
	offAngles = 0;
	offCmd = 0;
	offBhopcap = 0;
	offMaxspeed = 0;
	offClientMaxspeed = 0;
	offMoveType = 0;
	pBhopcapWindows = 0;
	pCZDS_Velocity_Byte = 0;
	pCBasePlayer__Jump_OldButtons_Check_Byte = 0;
	offm_iClientFOV = 0;
	offm_rgAmmoLast = 0;
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
	pCS_Stamina_Value = 0;
	pCS_Bhopcap = 0;
	pCS_Bhopcap_Windows = 0;
	offm_pClientActiveItem = 0;
	offm_CMultiManager_index = 0;

	// Cry of Fear-specific
	offm_bInfiniteStamina = 0;
	offm_fStamina = 0;
	offm_old_iAmmo = 0;
	offm_iPlayerSaveLock = 0;

	tpLandmarks.clear();
}

bool ServerDLL::CanHook(const std::wstring& moduleFullName)
{
	if (!IHookableDirFilter::CanHook(moduleFullName))
		return false;

	std::wstring folderName = GetFolderName(moduleFullName);

	// HACK: In Cry of Fear client and server dlls are in the same directory.
	// When we are going through cl_dlls skip every dll except hl.dll.
	if (folderName == L"cl_dlls" && GetFileName(moduleFullName) != L"hl.dll")
		return false;

	// Filter out addons like metamod which may be located into a "dlls" folder under addons.
	std::wstring pathToLiblist = moduleFullName.substr(0, moduleFullName.rfind(folderName)).append(L"liblist.gam");

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
			offMaxspeed = 0x1f4;
			offClientMaxspeed = 0x1f8;
			offMoveType = 0xdc;
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
				case 0: // HL-SteamPipe
				case 1: // gunman
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(ORIG_PM_Jump) + 2);
					break;
				case 2: // AG-Server
				case 3: // AG-Client, shouldn't happen here but who knows.
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(ORIG_PM_Jump) + 3);
					break;
				case 4: // BigLolly
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(ORIG_PM_Jump) + 21);
					break;
				case 10: // Parasomnia
				case 11: // Reissues
				case 5: // TWHL-Tower-2
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(ORIG_PM_Jump) + 5);
					break;
				case 12: // HL-SWEET
				case 6: // Decay
				case 14: // CoF-Mod-155
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(ORIG_PM_Jump) + 24);
					break;
				case 7: // Halfquake-Trilogy
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(ORIG_PM_Jump) + 6);
					break;
				case 8: // Half-Payne
				case 9: // DSM-Demo-1
				case 13: // CoF-5936
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(ORIG_PM_Jump) + 8);
					break;
				case 15: // CoF-Mod-155-Client
					ppmove = *reinterpret_cast<void***>(reinterpret_cast<uintptr_t>(ORIG_PM_Jump) + 9);
					break;
				}
			}
		});

	auto fPM_ClipVelocity = FindFunctionAsync(ORIG_PM_ClipVelocity, "PM_ClipVelocity", patterns::shared::PM_ClipVelocity);
	auto fPM_WaterMove = FindFunctionAsync(ORIG_PM_WaterMove, "PM_WaterMove", patterns::shared::PM_WaterMove);
	auto fCTriggerVolume__Spawn = FindAsync(ORIG_CTriggerVolume__Spawn,	patterns::server::CTriggerVolume__Spawn);
	auto fCS_Stamina_Value = FindAsync(
		pCS_Stamina_Value,
		patterns::server::CS_Stamina_Value);
	auto fCS_Bhopcap = FindAsync(
		pCS_Bhopcap,
		patterns::shared::Bhopcap_CS);
	auto fCS_Bhopcap_Windows = FindAsync(
		pCS_Bhopcap_Windows,
		patterns::shared::Bhopcap_CS_Windows);

	auto fCBaseEntity__FireBullets = FindAsync(ORIG_CBaseEntity__FireBullets, patterns::server::CBaseEntity__FireBullets);
	auto fCBaseEntity__FireBulletsPlayer = FindAsync(ORIG_CBaseEntity__FireBulletsPlayer, patterns::server::CBaseEntity__FireBulletsPlayer);

	auto fCBasePlayer__ForceClientDllUpdate = FindAsync(
		ORIG_CBasePlayer__ForceClientDllUpdate,
		patterns::server::CBasePlayer__ForceClientDllUpdate,
		[&](auto pattern) {
			switch (pattern - patterns::server::CBasePlayer__ForceClientDllUpdate.cbegin()) {
			case 25: // HL-SWEET
			case 0: // HL-WON
				offm_rgAmmoLast = 0x50C;
				offm_iClientFOV = 0x464;
				break;
			case 1: // HL-SteamPipe
				offm_rgAmmoLast = 0x554;
				offm_iClientFOV = 0x4AC;
				break;
			case 2: // HL-SteamPipe-8684
				offm_rgAmmoLast = 0x558;
				offm_iClientFOV = 0x4B0;
				break;
			case 3: // OP4-WON
				offm_rgAmmoLast = 0x5C0;
				offm_iClientFOV = 0x4A4;
				offFuncIsPlayer = 0xA0;
				offFuncCenter = 0xCC;
				break;
			case 4: // OpposingForce
				offm_rgAmmoLast = 0x604;
				offm_iClientFOV = 0x4E0;
				offFuncIsPlayer = 0xA0;
				offFuncCenter = 0xCC;
				break;
			case 5: // OP4-8684
				offm_rgAmmoLast = 0x608;
				offm_iClientFOV = 0x4E4;
				offFuncIsPlayer = 0xA0;
				offFuncCenter = 0xCC;
				break;
			case 6: // Gunman
				offm_rgAmmoLast = 0x53C;
				offm_iClientFOV = 0x47C;
				offFuncCenter = 0xCC;
				break;
			case 7: // CSCZDS
				offm_rgAmmoLast = 0x53C;
				offm_iClientFOV = 0x898;
				offFuncIsPlayer = 0xA8;
				offFuncCenter = 0xDC;
				offFuncObjectCaps = 0x18;
				is_czeror = true;
				break;
			case 8: // CSCZDS-8684
				offm_rgAmmoLast = 0x540;
				offm_iClientFOV = 0x89C;
				offFuncIsPlayer = 0xA8;
				offFuncCenter = 0xDC;
				offFuncObjectCaps = 0x18;
				is_czeror = true;
				break;
			case 9: // AoMDC
				offm_rgAmmoLast = 0x578;
				offm_iClientFOV = 0x4D0;
				break;
			case 10: // PARANOIA
				offm_rgAmmoLast = 0x62C;
				offm_iClientFOV = 0x584;
				offFuncIsPlayer = 0xD0;
				offFuncCenter = 0xFC;
				offFuncObjectCaps = 0x40;
				break;
			case 11: // CStrike-Latest
				offFuncIsPlayer = 0xA0;
				offFuncCenter = 0xCC;
				offFuncObjectCaps = 0x18;
				break;
			case 12: // TFC-8684
				offm_rgAmmoLast = 0x978;
				offm_iClientFOV = 0x8CC;
				offFuncIsPlayer = 0x98;
				offFuncCenter = 0xC4;
				offFuncObjectCaps = 0x18;
				break;
			case 13: // TWHL-Tower-2
			case 14: // Halfquake Trilogy
				offm_rgAmmoLast = 0x5F4;
				offm_iClientFOV = 0x548;
				offFuncIsPlayer = 0xD4;
				offFuncCenter = 0x100;
				offFuncObjectCaps = 0x44;
				break;
			case 15: // Echoes
				offm_rgAmmoLast = 0x5F4;
				offm_iClientFOV = 0x548;
				offFuncIsPlayer = 0xCC;
				offFuncCenter = 0xF8;
				offFuncObjectCaps = 0x3C;
				break;
			case 16: // Decay
				offm_rgAmmoLast = 0x544;
				offm_iClientFOV = 0x49C;
				break;
			case 22: // TheGate
			case 17: // HazardousCourse2
				offm_rgAmmoLast = 0x540;
				offm_iClientFOV = 0x498;
				break;
			case 18: // Black-Ops
				offm_rgAmmoLast = 0x554;
				offm_iClientFOV = 0x4AC;
				offFuncIsPlayer = 0xA4;
				offFuncCenter = 0xD0;
				break;
			case 19: // Invasion
				offm_rgAmmoLast = 0x514;
				offm_iClientFOV = 0x46C;
				break;
			case 20: // Arctic-Incident
				offm_rgAmmoLast = 0x5B8;
				offm_iClientFOV = 0x510;
				offFuncIsPlayer = 0xCC;
				offFuncCenter = 0xF8;
				offFuncObjectCaps = 0x3C;
				break;
			case 21: // HL-Delta
				offm_rgAmmoLast = 0x5C8;
				offm_iClientFOV = 0x51C;
				offFuncIsPlayer = 0xCC;
				offFuncCenter = 0xF8;
				offFuncObjectCaps = 0x3C;
				break;
			case 23: // OPBT
				offm_rgAmmoLast = 0x65C;
				offm_iClientFOV = 0x5A4;
				break;
			case 24: // Parasomnia
				offm_rgAmmoLast = 0x564;
				offm_iClientFOV = 0x4B8;
				offFuncIsPlayer = 0x98;
				offFuncCenter = 0xC4;
				break;
			case 26: // AVP2
				offm_rgAmmoLast = 0x550;
				offm_iClientFOV = 0x4A8;
				break;
			case 27: // CoF-Mod-10
				offm_rgAmmoLast = 0x2440;
				offm_iClientFOV = 0x238C;
				offFuncIsPlayer = 0xD0;
				offFuncCenter = 0xFC;
				offFuncObjectCaps = 0x40;
				offm_fStamina = 0x2080;
				offm_bInfiniteStamina = 0x2078;
				offm_pClientActiveItem = 0x23B0;
				offm_old_iAmmo = 284;
				offm_CMultiManager_index = 224;
				is_cof = true;
				break;
			case 28: // CoF-Mod-11
				offm_rgAmmoLast = 0x2444;
				offm_iClientFOV = 0x2390;
				offFuncIsPlayer = 0xD0;
				offFuncCenter = 0xFC;
				offFuncObjectCaps = 0x40;
				offm_fStamina = 0x2084;
				offm_bInfiniteStamina = 0x207C;
				offm_pClientActiveItem = 0x23B4;
				offm_old_iAmmo = 284;
				offm_CMultiManager_index = 224;
				is_cof = true;
				break;
			case 29: // CoF-Mod-12
				offm_rgAmmoLast = 0x2450;
				offm_iClientFOV = 0x239C;
				offFuncIsPlayer = 0xD0;
				offFuncCenter = 0xFC;
				offFuncObjectCaps = 0x40;
				offm_fStamina = 0x2090;
				offm_bInfiniteStamina = 0x2088;
				offm_pClientActiveItem = 0x23C0;
				offm_old_iAmmo = 284;
				offm_iPlayerSaveLock = 0x4A8;
				offm_CMultiManager_index = 224;
				is_cof = true;
				break;
			case 30: // CoF-Mod-13-135
				offm_rgAmmoLast = 0x2460;
				offm_iClientFOV = 0x23AC;
				offFuncIsPlayer = 0xD0;
				offFuncCenter = 0xFC;
				offFuncObjectCaps = 0x40;
				offm_fStamina = 0x20A0;
				offm_bInfiniteStamina = 0x2098;
				offm_pClientActiveItem = 0x23D0;
				offm_old_iAmmo = 284;
				offm_iPlayerSaveLock = 0x4B4;
				offm_CMultiManager_index = 224;
				is_cof = true;
				break;
			case 31: // CoF-Mod-14-155
				offm_rgAmmoLast = 0x2464;
				offm_iClientFOV = 0x23B0;
				offFuncIsPlayer = 0xD0;
				offFuncCenter = 0xFC;
				offFuncObjectCaps = 0x40;
				offm_fStamina = 0x20A4;
				offm_bInfiniteStamina = 0x209C;
				offm_pClientActiveItem = 0x23D4;
				offm_old_iAmmo = 284;
				offm_iPlayerSaveLock = 0x4B8;
				offm_CMultiManager_index = 224;
				is_cof = true;
				break;
			case 32: // CoF-Steam-Release
				offm_rgAmmoLast = 0x2474;
				offm_iClientFOV = 0x23C0;
				offFuncIsPlayer = 0xD0;
				offFuncCenter = 0xFC;
				offFuncObjectCaps = 0x40;
				offm_fStamina = 0x20B4;
				offm_bInfiniteStamina = 0x20AC;
				offm_pClientActiveItem = 0x23E4;
				offm_old_iAmmo = 288;
				offm_CMultiManager_index = 228;
				is_cof = true;
				break;
			case 33: // CoF-Steam-Latest
				offm_rgAmmoLast = 0x25C0;
				offm_iClientFOV = 0x250C;
				offFuncIsPlayer = 0xD0;
				offFuncCenter = 0xFC;
				offFuncObjectCaps = 0x40;
				offm_fStamina = 0x21F0;
				offm_bInfiniteStamina = 0x21E8;
				offm_pClientActiveItem = 0x2530;
				offm_old_iAmmo = 288;
				offm_CMultiManager_index = 228;
				is_cof = true;
				break;
			case 34: // DayOfDefeat-Latest
				offFuncIsPlayer = 0xB4;
				offFuncCenter = 0xE0;
				offFuncObjectCaps = 0x20;
				break;
			case 35: // DeathmatchClassic-Latest
				offm_rgAmmoLast = 0x520;
				offm_iClientFOV = 0x478;
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

	auto fCBasePlayer__Jump_OldButtons_Check_Byte = FindAsync(
		pCBasePlayer__Jump_OldButtons_Check_Byte,
		patterns::server::CBasePlayer__Jump_OldButtons_Check_Byte,
		[&](auto pattern) {
			switch (pattern - patterns::server::CBasePlayer__Jump_OldButtons_Check_Byte.cbegin()) {
			case 0: // CoF-Mod-155
				pCBasePlayer__Jump_OldButtons_Check_Byte += 12;
				break;
			case 1: // CoF-5936
				pCBasePlayer__Jump_OldButtons_Check_Byte += 6;
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
	auto fPM_CheckStuck = FindFunctionAsync(ORIG_PM_CheckStuck, "PM_CheckStuck", patterns::server::PM_CheckStuck);
	auto fPM_Duck = FindFunctionAsync(ORIG_PM_Duck, "PM_Duck", patterns::server::PM_Duck);
	auto fPM_UnDuck = FindAsync(ORIG_PM_UnDuck, patterns::server::PM_UnDuck);
	auto fShiftMonsters = FindAsync(ORIG_ShiftMonsters, patterns::server::ShiftMonsters);
	auto fCBasePlayer__ViewPunch = FindAsync(ORIG_CBasePlayer__ViewPunch, patterns::server::CBasePlayer__ViewPunch);
	auto fCBasePlayer__Jump = FindAsync(ORIG_CBasePlayer__Jump, patterns::server::CBasePlayer__Jump);
	auto fCBaseDoor__DoorActivate = FindAsync(ORIG_CBaseDoor__DoorActivate, patterns::server::CBaseDoor__DoorActivate);
	auto fCBaseEntity__IsInWorld = FindAsync(ORIG_CBaseEntity__IsInWorld, patterns::server::CBaseEntity__IsInWorld);

	uintptr_t pDispatchRestore;
	auto fDispatchRestore = FindAsync(
		pDispatchRestore,
		patterns::server::DispatchRestore,
		[&](auto pattern) {
			// HL-SteamPipe
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
		auto pattern = fCBasePlayer__Jump_OldButtons_Check_Byte.get();
		if (pCBasePlayer__Jump_OldButtons_Check_Byte)
			EngineDevMsg("[server dll] Found oldbuttons check from CBasePlayer::Jump at %p (using the %s pattern).\n", pCBasePlayer__Jump_OldButtons_Check_Byte, pattern->name());
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
		auto pattern3 = fCS_Bhopcap.get();
		auto pattern4 = fCS_Bhopcap_Windows.get();
		if (ORIG_PM_Jump) {
			if (pattern == patterns::shared::PM_Jump.cend())
				EngineDevMsg("[server dll] Found PM_Jump at %p.\n", ORIG_PM_Jump);
			else
				EngineDevMsg("[server dll] Found PM_Jump at %p (using the %s pattern).\n", ORIG_PM_Jump, pattern->name());
			if (offBhopcap)
				EngineDevMsg("[server dll] Found the bhopcap pattern at %p.\n", reinterpret_cast<void*>(offBhopcap + reinterpret_cast<uintptr_t>(ORIG_PM_Jump) - 27));
			if (pBhopcapWindows)
				EngineDevMsg("[server dll] Found bhopcap jump instruction at %p (using the %s pattern).\n", pBhopcapWindows, pattern2->name());
			if (pCS_Bhopcap)
				EngineDevMsg("[server dll] Found bhopcap jump pattern at %p (using the %s pattern) [Linux].\n", pCS_Bhopcap, pattern3->name());
			if (pCS_Bhopcap_Windows)
				EngineDevMsg("[server dll] Found bhopcap jump pattern at %p (using the %s pattern).\n", pCS_Bhopcap_Windows, pattern4->name());

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
		auto pattern = fCS_Stamina_Value.get();
		if (pCS_Stamina_Value) {
			EngineDevMsg("[server dll] Found the stamina value pattern at %p (using the %s pattern).\n", pCS_Stamina_Value, pattern->name());
		} else {
			EngineDevWarning("[server dll] Could not find the stamina value pattern.\n");
		}
	}

	{
		auto pattern = fCBasePlayer__ForceClientDllUpdate.get();
		if (ORIG_CBasePlayer__ForceClientDllUpdate) {
			EngineDevMsg("[server dll] Found CBasePlayer::ForceClientDllUpdate at %p (using the %s pattern).\n", ORIG_CBasePlayer__ForceClientDllUpdate, pattern->name());
		} else {
			ORIG_CBasePlayer__ForceClientDllUpdate_Linux = reinterpret_cast<_CBasePlayer__ForceClientDllUpdate_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN11CBasePlayer20ForceClientDllUpdateEv"));
			if (ORIG_CBasePlayer__ForceClientDllUpdate_Linux) {
				offm_rgAmmoLast = 0x56C; // 6153: 0x568
				offm_iClientFOV = 0x4C4; // 6153: 0x4C0
				if (ClientDLL::GetInstance().DoesGameDirContain("czeror")) {
					offm_rgAmmoLast = 0x554; // 6153: 0x550
					offm_iClientFOV = 0x8B0; // 6153: 0x8AC
					offFuncIsPlayer = 0xA8;
					offFuncCenter = 0xDC;
					offFuncObjectCaps = 0x18;
				}
				if (ClientDLL::GetInstance().DoesGameDirContain("bshift")) {
					offm_rgAmmoLast = 0x568; // 8684: 0x56C
					offm_iClientFOV = 0x4C0; // 8684: 0x4C4
				}
				if (ClientDLL::GetInstance().DoesGameDirMatch("cstrike") || ClientDLL::GetInstance().DoesGameDirMatch("czero")) {
					offm_rgAmmoLast = 0x674;
					offm_iClientFOV = 0x5C4;
					offFuncIsPlayer = 0xA0;
					offFuncCenter = 0xCC;
					offFuncObjectCaps = 0x18;
				}
				if (ClientDLL::GetInstance().DoesGameDirMatch("tfc")) {
					offm_rgAmmoLast = 0x98C; // 6153: 0x988
					offm_iClientFOV = 0x8E0; // 6153: 0x8DC
					offFuncObjectCaps = 0x1C;
				}
				if (ClientDLL::GetInstance().DoesGameDirMatch("dod")) {
					offm_rgAmmoLast = 0x4F8; // 6153: 0x4F4
					offm_iClientFOV = 0x448; // 6153: 0x444
					offFuncIsPlayer = 0xB4;
					offFuncCenter = 0xE0;
					offFuncObjectCaps = 0x20;
				}
				if (ClientDLL::GetInstance().DoesGameDirMatch("dmc")) {
					offm_rgAmmoLast = 0x534;
					offm_iClientFOV = 0x48C;
				}
				EngineDevMsg("[server dll] Found CBasePlayer::ForceClientDllUpdate [Linux] at %p.\n", ORIG_CBasePlayer__ForceClientDllUpdate_Linux);
			} else {
				EngineDevWarning("[server dll] Could not find CBasePlayer::ForceClientDllUpdate.\n");
				EngineWarning("Ammo HUD reset prevention is not available.\n");
			}
		}
	}

	auto fCGraph__InitGraph = FindAsync(
		ORIG_CGraph__InitGraph,
		patterns::server::CGraph__InitGraph,
		[&](auto pattern) {
			switch (pattern - patterns::server::CGraph__InitGraph.cbegin()) {
			case 0: // HL-SteamPipe
			case 1: // TWHL-Tower-2
			case 2: // Echoes
			case 3: // Decay
			case 4: // Halfquake-Trilogy
			case 5: // Parasomnia
			case 6: // HL-SWEET
			case 7: // Reissues
			case 8: // CoF-5936
				offm_pNodes = 0x0C;
				offm_vecOrigin = 0x00;
				offm_cNodes = 0x18;
				if (is_czeror)
					size_CNode = 0x60;
				else
					size_CNode = 0x58;
				break;
			default:
				assert(false);
			}
		});

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
				if (ClientDLL::GetInstance().DoesGameDirContain("czeror"))
					size_CNode = 0x60;
				else
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

	ORIG_CTriggerCamera__FollowTarget = reinterpret_cast<_CTriggerCamera__FollowTarget>(MemUtils::GetSymbolAddress(m_Handle, "?FollowTarget@CTriggerCamera@@QAEXXZ"));
	{
		if (ORIG_CTriggerCamera__FollowTarget) {
			EngineDevMsg("[server dll] Found CTriggerCamera::FollowTarget at %p.\n", ORIG_CTriggerCamera__FollowTarget);
		} else {
			EngineDevWarning("[server dll] Could not find CTriggerCamera::FollowTarget.\n");
		}
	}

	ORIG_CBaseButton__ButtonUse = reinterpret_cast<_CBaseButton__ButtonUse>(MemUtils::GetSymbolAddress(m_Handle, "?ButtonUse@CBaseButton@@QAEXPAVCBaseEntity@@0W4USE_TYPE@@M@Z"));
	{
		if (ORIG_CBaseButton__ButtonUse) {
			EngineDevMsg("[server dll] Found CBaseButton::ButtonUse at %p.\n", ORIG_CBaseButton__ButtonUse);
		}
		else {
			EngineDevWarning("[server dll] Could not find CBaseButton::ButtonUse.\n");
		}
	}

	ORIG_CBaseButton__ButtonTriggerAndWait = reinterpret_cast<_CBaseButton__ButtonTriggerAndWait>(MemUtils::GetSymbolAddress(m_Handle, "?TriggerAndWait@CBaseButton@@QAEXXZ"));
	{
		if (ORIG_CBaseButton__ButtonTriggerAndWait) {
			EngineDevMsg("[server dll] Found CBaseButton::TriggerAndWait at %p.\n", ORIG_CBaseButton__ButtonTriggerAndWait);
		}
		else {
			EngineDevWarning("[server dll] Could not find CBaseButton::TriggerAndWait.\n");
			EngineWarning("Using a func_button's target name to split through bxt_splits_add_entity <target_name> may not work.\n");
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

	ORIG_DispatchSpawn = reinterpret_cast<_DispatchSpawn>(MemUtils::GetSymbolAddress(m_Handle, "_Z13DispatchSpawnP7edict_s"));
	ORIG_DispatchTouch = reinterpret_cast<_DispatchTouch>(MemUtils::GetSymbolAddress(m_Handle, "_Z13DispatchTouchP7edict_sS0_"));
	ORIG_DispatchKeyValue = reinterpret_cast<_DispatchKeyValue>(MemUtils::GetSymbolAddress(m_Handle, "_Z16DispatchKeyValueP7edict_sP14KeyValueData_s"));
	ORIG_ClientCommand = reinterpret_cast<_ClientCommand>(MemUtils::GetSymbolAddress(m_Handle, "_Z13ClientCommandP7edict_s"));
	ORIG_PlayerPostThink = reinterpret_cast<_PlayerPostThink>(MemUtils::GetSymbolAddress(m_Handle, "_Z15PlayerPostThinkP7edict_s"));
	ORIG_PM_Move = reinterpret_cast<_PM_Move>(MemUtils::GetSymbolAddress(m_Handle, "PM_Move"));
	ORIG_AddToFullPack = reinterpret_cast<_AddToFullPack>(MemUtils::GetSymbolAddress(m_Handle, "_Z13AddToFullPackP14entity_state_siP7edict_sS2_iiPh"));
	ORIG_CmdStart = reinterpret_cast<_CmdStart>(MemUtils::GetSymbolAddress(m_Handle, "_Z8CmdStartPK7edict_sPK9usercmd_sj"));
	ORIG_CmdEnd = reinterpret_cast<_CmdEnd>(MemUtils::GetSymbolAddress(m_Handle, "_Z6CmdEndPK7edict_s"));

	if (ORIG_DispatchSpawn && ORIG_DispatchTouch && ORIG_DispatchKeyValue && ORIG_ClientCommand && ORIG_PlayerPostThink && 
		ORIG_PM_Move && ORIG_AddToFullPack && ORIG_CmdStart && ORIG_CmdEnd) {
		EngineDevMsg("[server dll] Found DispatchSpawn at %p.\n", ORIG_DispatchSpawn);
		EngineDevMsg("[server dll] Found DispatchTouch at %p.\n", ORIG_DispatchTouch);
		EngineDevMsg("[server dll] Found DispatchKeyValue at %p.\n", ORIG_DispatchKeyValue);
		EngineDevMsg("[server dll] Found ClientCommand at %p.\n", ORIG_ClientCommand);
		EngineDevMsg("[server dll] Found PlayerPostThink at %p.\n", ORIG_PlayerPostThink);
		EngineDevMsg("[server dll] Found PM_Move at %p.\n", ORIG_PM_Move);
		EngineDevMsg("[server dll] Found AddToFullPack at %p.\n", ORIG_AddToFullPack);
		EngineDevMsg("[server dll] Found CmdStart at %p.\n", ORIG_CmdStart);
		EngineDevMsg("[server dll] Found CmdEnd at %p.\n", ORIG_CmdEnd);
	} else {
		ORIG_GetEntityAPI = reinterpret_cast<_GetEntityAPI>(MemUtils::GetSymbolAddress(m_Handle, "GetEntityAPI"));
		if (ORIG_GetEntityAPI) {
			DLL_FUNCTIONS funcs;
			if (ORIG_GetEntityAPI(&funcs, INTERFACE_VERSION)) {
				// Gets our hooked addresses on Windows.
				ORIG_DispatchSpawn = funcs.pfnSpawn;
				ORIG_DispatchTouch = funcs.pfnTouch;
				ORIG_DispatchKeyValue = funcs.pfnKeyValue;
				ORIG_ClientCommand = funcs.pfnClientCommand;
				ORIG_PlayerPostThink = funcs.pfnPlayerPostThink;
				EngineDevMsg("[server dll] Found DispatchSpawn at %p.\n", ORIG_DispatchSpawn);
				EngineDevMsg("[server dll] Found DispatchTouch at %p.\n", ORIG_DispatchTouch);
				EngineDevMsg("[server dll] Found DispatchKeyValue at %p.\n", ORIG_DispatchKeyValue);
				EngineDevMsg("[server dll] Found ClientCommand at %p.\n", ORIG_ClientCommand);
				EngineDevMsg("[server dll] Found PlayerPostThink at %p.\n", ORIG_PlayerPostThink);
				if (INTERFACE_VERSION == 140)
				{
					ORIG_PM_Move = funcs.pfnPM_Move;
					ORIG_AddToFullPack = funcs.pfnAddToFullPack;
					ORIG_CmdStart = funcs.pfnCmdStart;
					ORIG_CmdEnd = funcs.pfnCmdEnd;
					EngineDevMsg("[server dll] Found PM_Move at %p.\n", ORIG_PM_Move);
					EngineDevMsg("[server dll] Found AddToFullPack at %p.\n", ORIG_AddToFullPack);
					EngineDevMsg("[server dll] Found CmdStart at %p.\n", ORIG_CmdStart);
					EngineDevMsg("[server dll] Found CmdEnd at %p.\n", ORIG_CmdEnd);
				}
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

	ORIG_CBreakable__Die = reinterpret_cast<_CBreakable__Die>(MemUtils::GetSymbolAddress(m_Handle, "?Die@CBreakable@@QAEXXZ"));
	if (ORIG_CBreakable__Die) {
		EngineDevMsg("[server dll] Found CBreakable::Die at %p.\n", ORIG_CBreakable__Die);
	} else {
		ORIG_CBreakable__Die_Linux = reinterpret_cast<_CBreakable__Die_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN10CBreakable3DieEv"));
		if (ORIG_CBreakable__Die_Linux) {
			EngineDevMsg("[server dll] Found CBreakable::Die [Linux] at %p.\n", ORIG_CBreakable__Die_Linux);
		} else {
			EngineDevWarning("[server dll] Could not find CBreakable::Die.\n");
			EngineWarning("Automatic timer stopping in Counter-Strike: Condition Zero Deleted Scenes is not available.\n");
		}
	}

	{
		auto pattern = fCBaseDoor__DoorActivate.get();
		if (ORIG_CBaseDoor__DoorActivate) {
			if (pattern == patterns::server::CBaseDoor__DoorActivate.cend())
				EngineDevMsg("[server dll] Found CBaseDoor::DoorActivate at %p.\n", ORIG_CBaseDoor__DoorActivate);
			else
				EngineDevMsg("[server dll] Found CBaseDoor::DoorActivate at %p (using the %s pattern).\n", ORIG_CBaseDoor__DoorActivate, pattern->name());
		} else {
			EngineDevWarning("[server dll] Could not find CBaseDoor::DoorActivate.\n");
			EngineWarning("Splitting on door activation is not available.\n");
		}
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
		auto pattern = fPM_CheckStuck.get();
		if (ORIG_PM_CheckStuck) {
			if (pattern == patterns::server::PM_CheckStuck.cend())
				EngineDevMsg("[server dll] Found PM_CheckStuck at %p.\n", ORIG_PM_CheckStuck);
			else
				EngineDevMsg("[server dll] Found PM_CheckStuck at %p (using the %s pattern).\n", ORIG_PM_CheckStuck, pattern->name());
		} else {
			EngineDevWarning("[server dll] Could not find PM_CheckStuck.\n");
		}
	}

	{
		auto pattern = fPM_Duck.get();
		if (ORIG_PM_Duck) {
			if (pattern == patterns::server::PM_Duck.cend())
				EngineDevMsg("[server dll] Found PM_Duck at %p.\n", ORIG_PM_Duck);
			else
				EngineDevMsg("[server dll] Found PM_Duck at %p (using the %s pattern).\n", ORIG_PM_Duck, pattern->name());
		} else {
			EngineDevWarning("[server dll] Could not find PM_Duck.\n");
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

	{
		auto pattern = fShiftMonsters.get();
		if (ORIG_ShiftMonsters) {
			EngineDevMsg("[server dll] Found ShiftMonsters at %p (using the %s pattern).\n", ORIG_ShiftMonsters, pattern->name());
		} else {
			EngineDevWarning("[server dll] Could not find ShiftMonsters.\n");
			EngineWarning("Fixing monster origin after loading save in Cry of Fear is not available.\n");
		}
	}

	{
		auto pattern = fCBasePlayer__ViewPunch.get();
		if (ORIG_CBasePlayer__ViewPunch) {
			EngineDevMsg("[server dll] Found CBasePlayer::ViewPunch at %p (using the %s pattern).\n", ORIG_CBasePlayer__ViewPunch, pattern->name());
		} else {
			EngineDevWarning("[server dll] Could not find CBasePlayer::ViewPunch.\n");
		}
	}

	{
		auto pattern = fCBasePlayer__Jump.get();
		if (ORIG_CBasePlayer__Jump) {
			EngineDevMsg("[server dll] Found CBasePlayer::Jump at %p (using the %s pattern).\n", ORIG_CBasePlayer__Jump, pattern->name());
		} else {
			EngineDevWarning("[server dll] Could not find CBasePlayer::Jump.\n");
		}
	}

	{
		auto pattern = fPM_UnDuck.get();
		if (ORIG_PM_UnDuck) {
			EngineDevMsg("[server dll] Found PM_UnDuck at %p (using the %s pattern).\n", ORIG_PM_UnDuck, pattern->name());
		} else {
			EngineDevWarning("[server dll] Could not find PM_UnDuck.\n");
			EngineWarning("Enabling ducktap in Cry of Fear is not available.\n");
		}
	}

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

	{
		auto pattern = fCBaseEntity__IsInWorld.get();
		if (ORIG_CBaseEntity__IsInWorld) {
			EngineDevMsg("[server dll] Found CBaseEntity::IsInWorld at %p (using the %s pattern).\n", ORIG_CBaseEntity__IsInWorld, pattern->name());
		} else {
			ORIG_CBaseEntity__IsInWorld_Linux = reinterpret_cast<_CBaseEntity__IsInWorld_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN11CBaseEntity9IsInWorldEv"));
			if (ORIG_CBaseEntity__IsInWorld_Linux)
				EngineDevMsg("[server dll] Found CBaseEntity::IsInWorld [Linux] at %p.\n", ORIG_CBaseEntity__IsInWorld_Linux);
			else
				EngineDevWarning("[server dll] Could not find CBaseEntity::IsInWorld.\n");
		}
	}

	{
		ORIG_CBaseTrigger__TeleportTouch = reinterpret_cast<_CBaseTrigger__TeleportTouch>(MemUtils::GetSymbolAddress(m_Handle, "?TeleportTouch@CBaseTrigger@@QAEXPAVCBaseEntity@@@Z"));
		if (ORIG_CBaseTrigger__TeleportTouch)
			EngineDevMsg("[server dll] Found CBaseTrigger::TriggerTouch at %p.\n", ORIG_CBaseTrigger__TeleportTouch);
		else {
			ORIG_CBaseTrigger__TeleportTouch_Linux = reinterpret_cast<_CBaseTrigger__TeleportTouch_Linux>(MemUtils::GetSymbolAddress(m_Handle, "_ZN12CBaseTrigger13TeleportTouchEP11CBaseEntity"));
			if (ORIG_CBaseTrigger__TeleportTouch_Linux)
				EngineDevMsg("[server dll] Found CBaseTrigger::TriggerTouch [Linux] at %p.\n", ORIG_CBaseTrigger__TeleportTouch_Linux);
			else
				EngineDevWarning("[server dll] Could not find CBaseTrigger::TriggerTouch.\n");
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
		REG(bxt_show_triggers_legacy_alpha);
		REG(bxt_render_far_entities);
	}
	if (ORIG_PM_CheckStuck)
		REG(bxt_fire_on_stuck);
	if (ORIG_CTriggerSave__SaveTouch || ORIG_CTriggerSave__SaveTouch_Linux)
		REG(bxt_disable_autosave);
	if (pCS_Stamina_Value || is_cof)
		REG(bxt_remove_stamina);
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
	if (ORIG_CBaseButton__ButtonUse) {
		REG(bxt_fire_on_button_target);
		REG(bxt_fire_on_button_command);
	}
	if ((ORIG_CBaseEntity__FireBullets && ORIG_CBaseEntity__FireBulletsPlayer) || (ORIG_CBaseEntity__FireBullets_Linux && ORIG_CBaseEntity__FireBulletsPlayer_Linux)) {
		REG(bxt_show_bullets);
		REG(bxt_show_bullets_enemy);
	}
	if (ORIG_PM_Duck && is_cof)
		REG(bxt_cof_slowdown_if_use_on_ground);
	if (ORIG_PM_UnDuck && is_cof)
		REG(bxt_cof_enable_ducktap);
	if (is_cof)
		REG(bxt_cof_disable_save_lock);
	if (ORIG_CBasePlayer__ViewPunch && is_cof)
		REG(bxt_cof_disable_viewpunch_from_jump);
	if (ORIG_ShiftMonsters && is_cof)
		REG(bxt_cof_disable_monsters_teleport_to_spawn_after_load);
	if (ORIG_CTriggerCamera__FollowTarget && is_cof)
		REG(bxt_cof_allow_skipping_all_cutscenes);
	if (ORIG_PM_Move)
		REG(bxt_ch_noclip_speed);
	if (ORIG_CBaseTrigger__TeleportTouch || ORIG_CBaseTrigger__TeleportTouch_Linux) {
		REG(bxt_ch_trigger_tp_keeps_momentum);
		REG(bxt_ch_trigger_tp_keeps_momentum_velocity);
		REG(bxt_ch_trigger_tp_keeps_momentum_velocity_redirect);
		REG(bxt_ch_trigger_tp_keeps_momentum_viewangles);
		REG(bxt_ch_trigger_tp_landmark);
	}

	REG(bxt_splits_print);
	REG(bxt_splits_print_times_at_end);
	REG(bxt_splits_autorecord_on_first_split);
	REG(bxt_splits_start_timer_on_first_split);
	REG(bxt_splits_end_on_last_split);

	if (ORIG_CmdStart && ORIG_CmdEnd) {
		REG(bxt_ch_fix_sticky_slide);
		REG(bxt_ch_fix_sticky_slide_offset);
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

void ServerDLL::SetStamina(bool makeItZero)
{
	if (!pCS_Stamina_Value)
		return;

	if (makeItZero) {
		if (*reinterpret_cast<byte*>(pCS_Stamina_Value + 2) == 0x43 &&
			*reinterpret_cast<byte*>(pCS_Stamina_Value + 3) == 0x79 &&
			*reinterpret_cast<byte*>(pCS_Stamina_Value + 4) == 0xA4 &&
			*reinterpret_cast<byte*>(pCS_Stamina_Value + 5) == 0x44
			)
			MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCS_Stamina_Value + 2), 4, reinterpret_cast<const byte*>("\x00\x00\x00\x00"));
	} else {
		if (*reinterpret_cast<byte*>(pCS_Stamina_Value + 2) == 0x0 &&
			*reinterpret_cast<byte*>(pCS_Stamina_Value + 3) == 0x0 &&
			*reinterpret_cast<byte*>(pCS_Stamina_Value + 4) == 0x0 &&
			*reinterpret_cast<byte*>(pCS_Stamina_Value + 5) == 0x0
			)
			MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCS_Stamina_Value + 2), 4, reinterpret_cast<const byte*>("\x43\x79\xA4\x44"));
	}
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

	if (pCS_Bhopcap)
	{
		if (CVars::bxt_bhopcap.GetBool())
		{
			if (*reinterpret_cast<byte*>(pCS_Bhopcap + 11) == 0x83)
				MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCS_Bhopcap + 11), 1, reinterpret_cast<const byte*>("\x82"));
		}
		else
		{
			if (*reinterpret_cast<byte*>(pCS_Bhopcap + 11) == 0x82)
				MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCS_Bhopcap + 11), 1, reinterpret_cast<const byte*>("\x83"));
		}
	}

	if (pCS_Bhopcap_Windows)
	{
		if (CVars::bxt_bhopcap.GetBool())
		{
			if (*reinterpret_cast<byte*>(pCS_Bhopcap_Windows) == 0x7A)
				MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCS_Bhopcap_Windows), 1, reinterpret_cast<const byte*>("\x7B"));
		}
		else
		{
			if (*reinterpret_cast<byte*>(pCS_Bhopcap_Windows) == 0x7B)
				MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCS_Bhopcap_Windows), 1, reinterpret_cast<const byte*>("\x7A"));
		}
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
	const int *inDuck = reinterpret_cast<const int *>(pmove + offInDuck);
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

void ServerDLL::CoFChanges()
{
	if (is_cof)
	{
		if (pCBasePlayer__Jump_OldButtons_Check_Byte)
		{
			if (offm_rgAmmoLast == 0x25C0) { // CoF-5936
				if ((*reinterpret_cast<byte*>(pCBasePlayer__Jump_OldButtons_Check_Byte) == 0xF6) && CVars::bxt_autojump.GetBool())
					MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCBasePlayer__Jump_OldButtons_Check_Byte), 13, reinterpret_cast<const byte*>("\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90"));
				else if ((*reinterpret_cast<byte*>(pCBasePlayer__Jump_OldButtons_Check_Byte) == 0x90) && !CVars::bxt_autojump.GetBool())
					MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCBasePlayer__Jump_OldButtons_Check_Byte), 13, reinterpret_cast<const byte*>("\xF6\x86\xB0\x22\x00\x00\x02\x0F\x84\x0C\x02\x00\x00"));
			} else { // CoF-Mod
				if ((*reinterpret_cast<byte*>(pCBasePlayer__Jump_OldButtons_Check_Byte) == 0x75) && CVars::bxt_autojump.GetBool())
					MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCBasePlayer__Jump_OldButtons_Check_Byte), 1, reinterpret_cast<const byte*>("\xEB"));
				else if ((*reinterpret_cast<byte*>(pCBasePlayer__Jump_OldButtons_Check_Byte) == 0xEB) && !CVars::bxt_autojump.GetBool())
					MemUtils::ReplaceBytes(reinterpret_cast<void*>(pCBasePlayer__Jump_OldButtons_Check_Byte), 1, reinterpret_cast<const byte*>("\x75"));
			}
		}

		void* classPtr = (*HwDLL::GetInstance().sv_player)->v.pContainingEntity->pvPrivateData;
		uintptr_t thisAddr = reinterpret_cast<uintptr_t>(classPtr);

		// Infinite Stamina
		bool* m_bInfiniteStamina = reinterpret_cast<bool*>(thisAddr + offm_bInfiniteStamina);
		*m_bInfiniteStamina = CVars::bxt_remove_stamina.GetBool();

		// Disable save lock for CoF (Mod version)
		if (offm_iPlayerSaveLock) {
			int* m_iPlayerSaveLock = reinterpret_cast<int*>(thisAddr + offm_iPlayerSaveLock);
			static bool reset_playersavelock = false;

			if ((*m_iPlayerSaveLock != 1337) && CVars::bxt_cof_disable_save_lock.GetBool()) {
				*m_iPlayerSaveLock = 1337;
				reset_playersavelock = true;
			}
			else if ((*m_iPlayerSaveLock == 1337) && !CVars::bxt_cof_disable_save_lock.GetBool() && reset_playersavelock) {
				*m_iPlayerSaveLock = 0;
				reset_playersavelock = false;
			}
		}
	}
}

HOOK_DEF_1(ServerDLL, void, __cdecl, PM_PlayerMove, qboolean, server)
{
	HwDLL &hwDLL = HwDLL::GetInstance();

	bool stuck_cur_frame = false;
	static bool not_stuck_prev_frame = false;

	if (ORIG_PM_CheckStuck)
	{
		stuck_cur_frame = ORIG_PM_CheckStuck();
		if (!CVars::bxt_fire_on_stuck.IsEmpty() && stuck_cur_frame && not_stuck_prev_frame)
		{
			std::ostringstream ss;
			ss << CVars::bxt_fire_on_stuck.GetString().c_str() << "\n";

			hwDLL.ORIG_Cbuf_InsertText(ss.str().c_str());
		}
		not_stuck_prev_frame = !stuck_cur_frame;
	}

	CoFChanges();

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

	if (CVars::_bxt_taslog.GetBool() && pEngfuncs)
	{
		ALERT(at_console, "-- BXT TAS Log Start --\n");
		ALERT(at_console, "Player index: %d; msec: %hhu (%Lf)\n", playerIndex, cmd->msec, static_cast<long double>(cmd->msec) * 0.001);
		ALERT(at_console, "Angles: %.8f; %.8f; %.8f\n", angles[0], angles[1], angles[2]);
		ALERT(at_console, "Velocity: %.8f; %.8f; %.8f; origin: %.8f; %.8f; %.8f\n", velocity[0], velocity[1], velocity[2], origin[0], origin[1], origin[2]);
		ALERT(at_console, "Onground: %d; usehull: %d\n", *groundEntity, *reinterpret_cast<int*>(pmove + 0xBC));
	}

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

	cmdStartOrigin = Vector(player->v.origin);
	cmdStartVelocity = Vector(player->v.velocity);

	return ORIG_CmdStart(player, cmd, seed);
}

HOOK_DEF_1(ServerDLL, void, __cdecl, CmdEnd, const edict_t*, player)
{
	if (CVars::bxt_ch_fix_sticky_slide.GetBool() && CVars::sv_cheats.GetBool()) {
		entvars_t *pev = const_cast<entvars_t *>(&player->v);

		Vector end_origin = Vector(pev->origin);
		Vector end_velocity = Vector(pev->velocity);

		if (end_velocity.Length2D() == 0.0f // stuck, exclude z vel because it will be -4.0
			&& cmdStartVelocity.Length() != 0.0f // not standing still, can include z
			&& cmdStartOrigin == end_origin // origin doesn't change when stuck
			) {
			const auto is_duck = pev->button & (IN_DUCK) || player->v.flags & (FL_DUCKING);

			auto origin_z_offset = CVars::bxt_ch_fix_sticky_slide_offset.GetFloat(); // offset so player isn't "stuck"
			auto tr_down = HwDLL::GetInstance().PlayerTrace(end_origin, 
				end_origin + Vector(0, 0, -2), is_duck ? HLStrafe::HullType::DUCKED : HLStrafe::HullType::NORMAL, 0);
			auto tr_up = HwDLL::GetInstance().PlayerTrace(end_origin, 
				end_origin + Vector(0, 0, 2), is_duck ? HLStrafe::HullType::DUCKED : HLStrafe::HullType::NORMAL, 0);

			// If trace to the bottom and no hit, then it means the slide is up top.
			// Already have offset positive by default so there is no need for other case.
			if (tr_down.Fraction == 1.0 && tr_up.Fraction != 1.0)
				origin_z_offset = -origin_z_offset;
			else if (tr_down.Fraction == 1.0 && tr_up.Fraction == 1.0) {
				// It is not possible to just get stuck in air so this is edge case of being ramp sandwiched.
				// Very rare and stupid case. No changes then so it is more predictable to maneuver.
				origin_z_offset = 0;
				cmdStartVelocity = Vector();
			}

			pev->origin[2] += origin_z_offset;
			pev->velocity = cmdStartVelocity;
		}
	}

	ORIG_CmdEnd(player);
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
		auto pev = GET_PEV(thisptr);
		if (pev && pev->targetname) {
			const char *targetname = HwDLL::GetInstance().ppGlobals->pStringBase + pev->targetname;
			if (!std::strcmp(targetname, "sheriffs_chppr") && ClientDLL::GetInstance().DoesGameDirContain("hunger")) { // They Hunger Episode 3
				DoAutoStopTasks();
			}
		}
	}

	return ORIG_CApache__DyingThink(thisptr);
}

HOOK_DEF_2(ServerDLL, void, __fastcall, CBreakable__Die, void*, thisptr, int, edx)
{
	if (HwDLL::GetInstance().ppGlobals) {
		auto pev = GET_PEV(thisptr);
		if (pev && pev->target) {
			const char *target = HwDLL::GetInstance().ppGlobals->pStringBase + pev->target;
			if (!std::strcmp(target, "BLOOOM") && ClientDLL::GetInstance().DoesGameDirContain("czeror")) { // CSCZDS
				DoAutoStopTasks();
			}
		}
	}

	return ORIG_CBreakable__Die(thisptr, edx);
}

HOOK_DEF_1(ServerDLL, void, __cdecl, CBreakable__Die_Linux, void*, thisptr)
{
	if (HwDLL::GetInstance().ppGlobals) {
		auto pev = GET_PEV(thisptr);
		if (pev && pev->target) {
			const char *target = HwDLL::GetInstance().ppGlobals->pStringBase + pev->target;
			if (!std::strcmp(target, "BLOOOM") && ClientDLL::GetInstance().DoesGameDirContain("czeror")) {
				DoAutoStopTasks();
			}
		}
	}

	return ORIG_CBreakable__Die_Linux(thisptr);
}

HOOK_DEF_2(ServerDLL, int, __fastcall, CBaseDoor__DoorActivate, void*, thisptr, int, edx)
{
	auto isActivated = ORIG_CBaseDoor__DoorActivate(thisptr, edx);

	if (isActivated && HwDLL::GetInstance().ppGlobals) {
		auto pev = GET_PEV(thisptr);
		if (pev && pev->targetname) {
			const char *targetname = HwDLL::GetInstance().ppGlobals->pStringBase + pev->targetname;
			Splits::Activate(targetname);
		}
	}
	return isActivated;
}

HOOK_DEF_1(ServerDLL, void, __fastcall, CBaseDoor__DoorGoUp, void*, thisptr)
{
	if (HwDLL::GetInstance().ppGlobals) {
		auto pev = GET_PEV(thisptr);
		if (pev && pev->target) {
			const char *target = HwDLL::GetInstance().ppGlobals->pStringBase + pev->target;
			if (!std::strcmp(target, "oil_spouts1_mm") && ClientDLL::GetInstance().DoesGameDirContain("hunger")) { // They Hunger Episode 2
				DoAutoStopTasks();
			}
		}
	}

	return ORIG_CBaseDoor__DoorGoUp(thisptr);
}

HOOK_DEF_1(ServerDLL, void, __fastcall, CBaseDoor__DoorHitTop, void*, thisptr)
{
	if (HwDLL::GetInstance().ppGlobals && pEngfuncs) {
		auto pev = GET_PEV(thisptr);
		edict_t *pPlayer = pEngfuncs->pfnPEntityOfEntIndex(1);
		if (pev && pev->targetname && pPlayer) {
			void *classPtr = pPlayer->pvPrivateData;
			char pVolumeName[] = "lm15";
			const char *targetname = HwDLL::GetInstance().ppGlobals->pStringBase + pev->targetname;

			if (!std::strcmp(targetname, "rocket_dr") && ClientDLL::GetInstance().DoesGameDirMatch("lm_txp") // The Xeno Project
				&& ORIG_CChangeLevel__InTransitionVolume(classPtr, pVolumeName))
				DoAutoStopTasks();
		}
	}

	return ORIG_CBaseDoor__DoorHitTop(thisptr);
}

HOOK_DEF_4(ServerDLL, void, __fastcall, CBaseMonster__Killed, void*, thisptr, int, edx, entvars_t*, pevAttacker, int, iGib)
{
	if (HwDLL::GetInstance().ppGlobals) {
		auto pev = GET_PEV(thisptr);
		if (pev && pev->classname) {
			const char* classname = HwDLL::GetInstance().ppGlobals->pStringBase + pev->classname;
			if (!std::strcmp(classname, "monster_gargantua") && ClientDLL::GetInstance().DoesGameDirMatch("tetsu0_cot")) {
				DoAutoStopTasks();
			}
		}
	}

	return ORIG_CBaseMonster__Killed(thisptr, edx, pevAttacker, iGib);
}

HOOK_DEF_2(ServerDLL, void, __fastcall, CMultiManager__ManagerThink, void*, thisptr, int, edx)
{
	auto &hw = HwDLL::GetInstance();

	if (hw.ppGlobals) {
		auto pev = GET_PEV(thisptr);
		if (pev && pev->targetname) {
			const char *targetname = hw.ppGlobals->pStringBase + pev->targetname;
			OnMultiManagerFired(targetname);

			if (offm_CMultiManager_index) {
				auto m_index = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(thisptr) + offm_CMultiManager_index);
				if (!std::strcmp(targetname, "whensimondies") && (m_index == 2)) // Cry of Fear (Ending 4)
					DoAutoStopTasks();
			}
		}
	}

	return ORIG_CMultiManager__ManagerThink(thisptr, edx);
}

HOOK_DEF_5(ServerDLL, void, __cdecl, FireTargets_Linux, char*, targetName, void*, pActivator, void*, pCaller, int, useType, float, value)
{
	if (HwDLL::GetInstance().ppGlobals && targetName != NULL && pCaller) {
		auto pev = GET_PEV(pCaller);
		if(pev && pev->targetname)
		{
			const char *targetname = HwDLL::GetInstance().ppGlobals->pStringBase + pev->targetname;
			const char *classname = HwDLL::GetInstance().ppGlobals->pStringBase + pev->classname;
			// We first need to check if the pCaller is a multi_manager since FireTargets can be called by anyone
			if (!std::strcmp(classname, "multi_manager")) {
				OnMultiManagerFired(targetname);
			} else {
				// Fire any splitter that matches this fired entity's name
				Splits::Activate(targetname);
			}
		}
	}

	return ORIG_FireTargets_Linux(targetName, pActivator, pCaller, useType, value);
}

void ServerDLL::OnMultiManagerFired(const char *targetname)
{
	auto &cl = ClientDLL::GetInstance();

	if ((!std::strcmp(targetname, "roll_the_credits") && cl.DoesGameDirContain("bshift")) // Blue Shift
		|| (!std::strcmp(targetname, "youwinmulti") && cl.DoesGameDirContain("rewolf")) // Gunman Chronicles
		|| (!std::strcmp(targetname, "previctory_mm") && cl.DoesGameDirContain("decay")) // Decay
		|| (!std::strcmp(targetname, "stairscene_mngr") && cl.DoesGameDirContain("hunger")) // They Hunger Episode 1
		|| (!std::strcmp(targetname, "boot_radio_seq") && cl.DoesGameDirContain("gearbox")) // Opposing Force: Boot Camp
		|| (!std::strcmp(targetname, "telmm") && cl.DoesGameDirMatch("biglolly")) // Big Lolly
		|| (!std::strcmp(targetname, "mm_player_camera1") && cl.DoesGameDirMatch("htc")) // HTC
		|| (!std::strcmp(targetname, "multimanager_1") && cl.DoesGameDirMatch("construction")) // Construction
		|| (!std::strcmp(targetname, "the_endgame_mm") && cl.DoesGameDirMatch("gloom")) // The Gloom
		|| (!std::strcmp(targetname, "endbox_mm0") && cl.DoesGameDirContain("echoes")) // Echoes
		|| (!std::strcmp(targetname, "sendmm") && cl.DoesGameDirMatch("minimicus"))  // Minimicus
		|| (!std::strcmp(targetname, "kill2") && cl.DoesGameDirMatch("before")) // Before
		|| (!std::strcmp(targetname, "tele_in") && cl.DoesGameDirMatch("plague")) // Plague
		|| (!std::strcmp(targetname, "exit_seq") && cl.DoesGameDirMatch("timeline2") && cl.DoesMapNameMatch("ice29")) // Timeline 2
		|| (!std::strcmp(targetname, "spawn_garg_sci_mm") && cl.DoesGameDirMatch("steamlink")) // Uplink
		|| (!std::strcmp(targetname, "fc_mm1") && cl.DoesGameDirMatch("hc")) // Hazardous Course 2
		|| (!std::strcmp(targetname, "medicosprey") && cl.DoesGameDirMatch("visitors")) // Visitors
		|| (!std::strcmp(targetname, "change_mm") && cl.DoesGameDirMatch("wantedsp") && cl.DoesMapNameMatch("want36")) // Wanted
		|| (!std::strcmp(targetname, "multiend1") && cl.DoesGameDirMatch("cryoffear") && !cl.DoesMapNameContain("cof_suicide")) // Cry of Fear (Ending 1, 2, 3)
		|| (!std::strcmp(targetname, "multicrash") && cl.DoesGameDirMatch("cryoffear")) // Cry of Fear (Ending 5)
		|| (!std::strcmp(targetname, "multicoopend") && cl.DoesGameDirMatch("cryoffear"))) { // Cry of Fear (Co-op)
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

	// Fire any splitter that matches this multimanager name
	// This is also repeated in FireTargets for non-multimanagers, because there may be multimanagers
	// that are triggered by other means that are not FireTargets. So there may be some overlap but
	// we cover more cases this way
	Splits::Activate(targetname);
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

	if (CVars::bxt_splits_print_times_at_end.GetBool() && !Splits::printed_on_end)
	{
		// Print all the splits data at the end of the run, and avoid printing it more than once
		Splits::PrintAll();
		Splits::printed_on_end = true;
	}
}

void ServerDLL::GetTriggerColor(const char *classname, float &r, float &g, float &b)
{
	bool is_trigger = std::strncmp(classname, "trigger_", 8) == 0;
	bool is_ladder = std::strncmp(classname, "func_ladder", 11) == 0;
	if (!is_trigger && !is_ladder)
		return;

	if (is_ladder)
	{
		// Sky
		r = 102;
		g = 178;
		b = 255;
	}
	else if (is_trigger)
	{
		classname += 8;
		if (std::strcmp(classname, "changelevel") == 0) {
			// Bright green
			r = 79;
			g = 255;
			b = 10;
		}
		else if (std::strcmp(classname, "hurt") == 0) {
			// Red
			r = 255;
			g = 0;
			b = 0;
		}
		else if (std::strcmp(classname, "multiple") == 0) {
			// Blue
			r = 0;
			g = 0;
			b = 255;
		}
		else if (std::strcmp(classname, "once") == 0) {
			// Cyan
			r = 0;
			g = 255;
			b = 255;
		}
		else if (std::strcmp(classname, "push") == 0) {
			// Bright yellow
			r = 255;
			g = 255;
			b = 0;
		}
		else if (std::strcmp(classname, "teleport") == 0) {
			// Dull green
			r = 81;
			g = 147;
			b = 49;
		}
		else if (std::strcmp(classname, "transition") == 0) {
			// Magenta
			r = 203;
			g = 103;
			b = 212;
		}
		else {
			// White
			r = 255;
			g = 255;
			b = 255;
		}
	}
}

void ServerDLL::GetTriggerAlpha(const char *classname, bool inactive, bool additive, float &a)
{
	bool is_trigger = std::strncmp(classname, "trigger_", 8) == 0;
	bool is_ladder = std::strncmp(classname, "func_ladder", 11) == 0;
	if (!is_trigger && !is_ladder)
		return;

	// The alpha should be lower in additive modes.
	constexpr std::array<std::array<float, 2>, 2> common_alphas{
		std::array<float, 2>{ 120.0f, 50.0f },
		std::array<float, 2>{ 50.0f, 20.0f }
	};

	if (std::strcmp(classname, "trigger_transition") == 0)
		a = additive ? 50.0f : 120.0f;
	else
		a = common_alphas[inactive][additive];
}

void ServerDLL::AddToFullPack_Pre(edict_t *ent, unsigned char **pSet, int &status)
{
	if (CVars::bxt_render_far_entities.GetBool()) // https://github.com/ValveSoftware/halflife/blob/c7240b965743a53a29491dd49320c88eecf6257b/dlls/client.cpp#L1114-L1122
		*pSet = NULL;

	if (!CVars::bxt_show_hidden_entities.GetBool() && !CVars::bxt_show_hidden_entities_clientside.GetBool() && !CVars::bxt_show_triggers_legacy.GetBool())
		return;

	const char *classname = HwDLL::GetInstance().ppGlobals->pStringBase + ent->v.classname;
	bool is_trigger = !std::strncmp(classname, "trigger_", 8) || !std::strcmp(classname, "func_ladder");

	if (!is_trigger)
	{
		if (CVars::bxt_show_hidden_entities.GetBool())
		{
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

				status = BXT_ADDTOFULLPACK_STATE_HIDDEN_ENTITIES;
			}
		}
		else if (CVars::bxt_show_hidden_entities_clientside.GetBool())
		{
			if (ent->v.effects & EF_NODRAW)
			{
				ent->v.effects &= ~EF_NODRAW;
				ent->v.renderamt = 0;

				// e.g. func_wall_toggle is kRenderNormal when it's EF_NODRAW'd, so that'd make it visible always, fix that
				if (ent->v.rendermode == kRenderNormal)
					ent->v.rendermode = kRenderTransTexture;

				status = BXT_ADDTOFULLPACK_STATE_HIDDEN_ENTITIES_CLIENTSIDE;
			}
		}
	}
	else
	{
		if (CVars::bxt_show_triggers_legacy.GetBool())
		{
			ent->v.effects &= ~EF_NODRAW;
			ent->v.renderamt = 0;
			ent->v.rendermode = kRenderTransColor;
			ent->v.renderfx = kRenderFxTrigger;
			GetTriggerColor(classname, ent->v.rendercolor.x, ent->v.rendercolor.y, ent->v.rendercolor.z);

			status = BXT_ADDTOFULLPACK_STATE_TRIGGERS;
		}
	}
}

HOOK_DEF_7(ServerDLL, int, __cdecl, AddToFullPack, struct entity_state_s*, state, int, e, edict_t*, ent, edict_t*, host, int, hostflags, int, player, unsigned char*, pSet)
{
	if (!HwDLL::GetInstance().ppGlobals) {
		return ORIG_AddToFullPack(state, e, ent, host, hostflags, player, pSet);
	}

	int status = BXT_ADDTOFULLPACK_STATE_NO;
	bool oldEffectsNodraw = ent->v.effects & EF_NODRAW;
	auto oldRendermode = ent->v.rendermode;
	auto oldRenderAmount = ent->v.renderamt;
	auto oldRenderColor = ent->v.rendercolor;
	auto oldRenderFx = ent->v.renderfx;

	AddToFullPack_Pre(ent, &pSet, status);

	auto ret = ORIG_AddToFullPack(state, e, ent, host, hostflags, player, pSet);

	if (status != BXT_ADDTOFULLPACK_STATE_NO) {
		ent->v.effects = SET_OR_UNSET_FLAG(oldEffectsNodraw, ent->v.effects, EF_NODRAW);
		ent->v.rendermode = oldRendermode;
	}
	if (status == BXT_ADDTOFULLPACK_STATE_TRIGGERS) {
		ent->v.rendercolor = oldRenderColor;
		ent->v.renderfx = oldRenderFx;
	}
	if (status == BXT_ADDTOFULLPACK_STATE_HIDDEN_ENTITIES_CLIENTSIDE || status == BXT_ADDTOFULLPACK_STATE_TRIGGERS)
		ent->v.renderamt = oldRenderAmount;

	return ret;
}

HOOK_DEF_1(ServerDLL, void, __fastcall, CTriggerVolume__Spawn, void*, thisptr)
{
	if (!HwDLL::GetInstance().ppGlobals || !pEngfuncs) {
		ORIG_CTriggerVolume__Spawn(thisptr);
		return;
	}

	auto pev = GET_PEV(thisptr);
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

	auto pev = GET_PEV(thisptr);
	string_t old_model = pev->model;
	ORIG_CTriggerVolume__Spawn_Linux(thisptr);
	pev->model = old_model;
	pev->modelindex = pEngfuncs->pfnModelIndex(HwDLL::GetInstance().ppGlobals->pStringBase + old_model);
	pev->effects |= EF_NODRAW;
}

HOOK_DEF_1(ServerDLL, void, __cdecl, ClientCommand, edict_t*, pEntity)
{
	if (!pEngfuncs || !pEntity->pvPrivateData) {
		ORIG_ClientCommand(pEntity);
		return;
	}

	const char *cmd = pEngfuncs->pfnCmd_Argv(0);
	void* classPtr = pEntity->v.pContainingEntity->pvPrivateData;
	uintptr_t thisAddr = reinterpret_cast<uintptr_t>(classPtr);

	if ((std::strcmp(cmd, "fullupdate") == 0) && offm_iClientFOV && offm_rgAmmoLast) 
	{
		int* m_iClientFOV = reinterpret_cast<int*>(thisAddr + offm_iClientFOV);
		int* m_rgAmmoLast = reinterpret_cast<int*>(thisAddr + offm_rgAmmoLast);

		if (is_cof && offm_pClientActiveItem && offm_old_iAmmo) {
			void** m_pActiveItem = reinterpret_cast<void**>(thisAddr + (offm_pClientActiveItem - 4));
			if (*m_pActiveItem != NULL) {
				uintptr_t* m_pClientActiveItem = reinterpret_cast<uintptr_t*>(thisAddr + offm_pClientActiveItem);
				int* old_m_iAmmo = reinterpret_cast<int*>(*m_pClientActiveItem + offm_old_iAmmo);
				*old_m_iAmmo = -1;
			}
		}

		*m_iClientFOV = -1;
		for (int i = 0; i < maxAmmoSlots; i++)
			m_rgAmmoLast[i] = -1;

		#ifdef _WIN32
			ORIG_CBasePlayer__ForceClientDllUpdate(classPtr);
		#else
			ORIG_CBasePlayer__ForceClientDllUpdate_Linux(classPtr);
		#endif
	}
	else
	{
		ORIG_ClientCommand(pEntity);
		return;
	}
}

HOOK_DEF_1(ServerDLL, void, __cdecl, PlayerPostThink, edict_t*, pEntity)
{
	bool noclip_set = false;
	if (pEntity && pEntity->pvPrivateData)
	{
		// In Cry of Fear, the developers reset the noclip in the CBasePlayer::PostThink function as a hidden anti-cheat measure.
		// We will set this ourselves after the original function completes if noclip was actually enabled before.
		if (CVars::sv_cheats.GetBool())
		{
			if (is_cof)
			{
				if (pEntity->v.movetype == MOVETYPE_NOCLIP)
					noclip_set = true;
			}
		}
	}

	ORIG_PlayerPostThink(pEntity);

	if (noclip_set)
		pEntity->v.movetype = MOVETYPE_NOCLIP;
}

void ServerDLL::GiveNamedItem(entvars_t *pev, int istr)
{
	auto &hw = HwDLL::GetInstance();

	if (pEngfuncs && hw.ppGlobals && ORIG_DispatchSpawn && ORIG_DispatchTouch)
	{
		edict_t *pent = pEngfuncs->pfnCreateNamedEntity(istr);
		if (!pent || !pEngfuncs->pfnEntOffsetOfPEntity(pent))
		{
			ALERT (at_console, "NULL Ent in GiveNamedItem!\n");
			return;
		}
		pent->v.origin = pev->origin;
		pent->v.spawnflags |= SF_NORESPAWN;

		ORIG_DispatchSpawn(pent);
		ORIG_DispatchTouch(pent, pev->pContainingEntity);
	}
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
	const auto pevToucher = GET_PEV(pOther);
	auto pevPushable = GET_PEV(thisptr);

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
	const auto pevToucher = GET_PEV(pOther);
	auto pevPushable = GET_PEV(thisptr);

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
	usercmd_t *cmd = reinterpret_cast<usercmd_t*>(pmove + offCmd);
	float *maxspeed = reinterpret_cast<float*>(pmove + offMaxspeed);
	float *clientmaxspeed = reinterpret_cast<float*>(pmove + offClientMaxspeed);
	int *movetype = reinterpret_cast<int*>(pmove + offMoveType);

	auto start_origin = Vector(origin);
	auto ch_noclip_vel = CVars::bxt_ch_noclip_speed.GetFloat();

	if (*movetype == MOVETYPE_NOCLIP && ch_noclip_vel != 0.f) {
		ch_noclip_vel_prev_maxspeed = *maxspeed;
		ch_noclip_vel_prev_clientmaxspeed = *clientmaxspeed;

		if (*clientmaxspeed == 0.0f)
			*clientmaxspeed = *maxspeed; 

		cmd->forwardmove = cmd->forwardmove / *clientmaxspeed * ch_noclip_vel;
		cmd->sidemove = cmd->sidemove / *clientmaxspeed * ch_noclip_vel;
		cmd->upmove = cmd->upmove / *clientmaxspeed * ch_noclip_vel;
		*maxspeed = *clientmaxspeed = ch_noclip_vel;
	}

	ORIG_PM_Move(ppmove, server);

	/*
	 * Assuming linear motion from start_origin to origin.
	 * This is not always the case but it is a good approximation.
	 */
	CustomTriggers::Update(start_origin, Vector(origin), (*flags & FL_DUCKING) != 0);

	if (*movetype == MOVETYPE_NOCLIP && ch_noclip_vel != 0.f) {
		*maxspeed = ch_noclip_vel_prev_maxspeed;
		*clientmaxspeed = ch_noclip_vel_prev_clientmaxspeed;
	}
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

HOOK_DEF_1(ServerDLL, void, __fastcall, CTriggerCamera__FollowTarget, void*, thisptr)
{
	auto pev = GET_PEV(thisptr);
	if (pev)
	{
		bool changed = false;
		auto oldSpawnFlags = pev->spawnflags;
		if (CVars::bxt_cof_allow_skipping_all_cutscenes.GetBool())
		{
			if (pev->spawnflags & COF_TRIGGER_CAMERA_FLAGS_UNSKIPPABLE) // "Unskippable" flag from .fgd
			{
				pev->spawnflags &= ~COF_TRIGGER_CAMERA_FLAGS_UNSKIPPABLE;
				changed = true;
			}
		}

		ORIG_CTriggerCamera__FollowTarget(thisptr);

		if (changed)
			pev->spawnflags = oldSpawnFlags;
	}
	else 
	{
		ORIG_CTriggerCamera__FollowTarget(thisptr);
	}
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

HOOK_DEF_6(ServerDLL, void, __fastcall, CBaseButton__ButtonUse, void*, thisptr, int, edx, void*, pActivator, void*, pCaller, int, useType, float, value)
{
	if (HwDLL::GetInstance().ppGlobals) {
		auto pev = GET_PEV(thisptr);
		if (pev) {
			if (pev->target) {
				const char* target = HwDLL::GetInstance().ppGlobals->pStringBase + pev->target;
				if (!CVars::bxt_fire_on_button_target.IsEmpty() && !CVars::bxt_fire_on_button_command.IsEmpty()) {
					if (!std::strcmp(target, CVars::bxt_fire_on_button_target.GetString().c_str()))
					{
						std::ostringstream ss;
						ss << CVars::bxt_fire_on_button_command.GetString().c_str() << "\n";

						HwDLL::GetInstance().ORIG_Cbuf_InsertText(ss.str().c_str());
					}
				}
			}
			if (pev->targetname)
			{
				// Fire any splitter that matches this button's name
				// TODO: check for this button's availability, it may not be usable yet, like the button in the test chamber,
				// that only works after the master entity has been triggered
				const char *targetname = HwDLL::GetInstance().ppGlobals->pStringBase + pev->targetname;
				Splits::Activate(targetname);
			}
		}
	}

	return ORIG_CBaseButton__ButtonUse(thisptr, edx, pActivator, pCaller, useType, value);
}

HOOK_DEF_2(ServerDLL, void, __fastcall, CBaseButton__ButtonTriggerAndWait, void*, thisptr, int, edx)
{
	if (HwDLL::GetInstance().ppGlobals) {
		auto pev = GET_PEV(thisptr);
		if (pev && pev->target) {
			// Fire any splitter that matches this button's target's name. This one compared to ButtonUse, should
			// already have the button's master entity triggered and the button should already be enabled, so it's
			// better than the ButtonUse in that regard, but it doesn't get called for every button...
			const char *target = HwDLL::GetInstance().ppGlobals->pStringBase + pev->target;
			Splits::Activate(target);
		}
	}

	return ORIG_CBaseButton__ButtonTriggerAndWait(thisptr, edx);
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
		auto pev = GET_PEV(thisptr);
		if (pev && pev->targetname) {
			const char *targetname = HwDLL::GetInstance().ppGlobals->pStringBase + pev->targetname;
			if (!std::strcmp(targetname, "tr_endchange")) {
				DoAutoStopTasks();
			}
		}
	}

	return ORIG_CTriggerEndSection__EndSectionTouch(thisptr, edx, pOther);
}

HOOK_DEF_0(ServerDLL, void, __cdecl, PM_Duck)
{
	ORIG_PM_Duck();

	if (ppmove && CVars::bxt_cof_slowdown_if_use_on_ground.GetBool()) {
		auto pmove = reinterpret_cast<uintptr_t>(*ppmove);
		int *onground = reinterpret_cast<int*>(pmove + offOnground);
		usercmd_t *cmd = reinterpret_cast<usercmd_t*>(pmove + offCmd);
		float *velocity = reinterpret_cast<float*>(pmove + offVelocity);

		if ((*onground != -1) && (cmd->buttons & IN_USE)) {
			velocity[0] *= HL1_SLOWDOWN_PLAYER_BY_USE;
			velocity[1] *= HL1_SLOWDOWN_PLAYER_BY_USE;
			velocity[2] *= HL1_SLOWDOWN_PLAYER_BY_USE;
		}
	}
}

HOOK_DEF_0(ServerDLL, void, __cdecl, PM_UnDuck)
{
	if (ppmove && offFlags && offInDuck && CVars::bxt_cof_enable_ducktap.GetBool()) {
		auto pmove = reinterpret_cast<uintptr_t>(*ppmove);
		int *flags = reinterpret_cast<int*>(pmove + offFlags);
		qboolean *inDuck = reinterpret_cast<qboolean*>(pmove + offInDuck);
		*flags |= FL_DUCKING;
		*inDuck = false;
	}

	ORIG_PM_UnDuck();
}

HOOK_DEF_1(ServerDLL, void, __cdecl, ShiftMonsters, Vector, origin)
{
	// Cry of Fear-specific, fix monsters teleport to their spawn points.
	if (CVars::bxt_cof_disable_monsters_teleport_to_spawn_after_load.GetBool())
		return;
	else
		return ORIG_ShiftMonsters(origin);
}

HOOK_DEF_5(ServerDLL, void, __fastcall, CBasePlayer__ViewPunch, void*, thisptr, int, edx, float, p, float, y, float, r)
{
	if (CVars::bxt_cof_disable_viewpunch_from_jump.GetBool() && insideCBasePlayerJump)
		return;

	ORIG_CBasePlayer__ViewPunch(thisptr, edx, p, y, r);
}

HOOK_DEF_1(ServerDLL, void, __fastcall, CBasePlayer__Jump, void*, thisptr)
{
	insideCBasePlayerJump = true;
	ORIG_CBasePlayer__Jump(thisptr);
	insideCBasePlayerJump = false;
}

int ServerDLL::IsInWorld(Vector origin, Vector velocity, int map_size, int map_max_velocity) // https://github.com/ValveSoftware/halflife/blob/c7240b965743a53a29491dd49320c88eecf6257b/dlls/cbase.cpp#L706
{	
	// position
	if (origin.x >= map_size) return 0;
	if (origin.y >= map_size) return 0;
	if (origin.z >= map_size) return 0;
	if (origin.x <= -map_size) return 0;
	if (origin.y <= -map_size) return 0;
	if (origin.z <= -map_size) return 0;
	// speed
	if (velocity.x >= map_max_velocity) return 0;
	if (velocity.y >= map_max_velocity) return 0;
	if (velocity.z >= map_max_velocity) return 0;
	if (velocity.x <= -map_max_velocity) return 0;
	if (velocity.y <= -map_max_velocity) return 0;
	if (velocity.z <= -map_max_velocity) return 0;

	return 1;
}

HOOK_DEF_1(ServerDLL, int, __fastcall, CBaseEntity__IsInWorld, void*, thisptr)
{
	if (HwDLL::GetInstance().is_big_map) 
	{
		auto pev = GET_PEV(thisptr);
		if (pev)
			return IsInWorld(pev->origin, pev->velocity, BIG_MAP_SIZE, BIG_MAP_MAX_VELOCITY);
	}

	return ORIG_CBaseEntity__IsInWorld(thisptr);
}

HOOK_DEF_1(ServerDLL, int, __cdecl, CBaseEntity__IsInWorld_Linux, void*, thisptr)
{
	if (HwDLL::GetInstance().is_big_map) 
	{
		auto pev = GET_PEV(thisptr);
		if (pev)
			return IsInWorld(pev->origin, pev->velocity, BIG_MAP_SIZE, BIG_MAP_MAX_VELOCITY);
	}

	return ORIG_CBaseEntity__IsInWorld_Linux(thisptr);
}

bool ServerDLL::IsPlayer(edict_t *ent)
{
	// https://github.com/ValveSoftware/halflife/blob/c7240b965743a53a29491dd49320c88eecf6257b/dlls/player.cpp#L2850

	auto &hw = HwDLL::GetInstance();

	if (strcmp(hw.GetString(ent->v.classname), "player") != 0)
		return false;

	if (!(ent->v.flags & FL_CLIENT))
		return false;

	if (pEngfuncs && hw.ppGlobals)
	{
		int index = pEngfuncs->pfnIndexOfEdict(ent);

		if ((index < 1) || (index > hw.ppGlobals->maxClients)) // gGlobalVariables.maxClients = svs.maxclients
			return false;
	}

	return true;
}

void TriggerTpKeepsMomentumRestore(Vector prev_vel, Vector prev_view, Vector prev_angles, Vector prev_basevelocity, entvars_t *pev, enginefuncs_t *pEngfuncs)
{
	// Set velocity before viewangles because viewangles will mess with the velocity angle for redirection
	if (CVars::bxt_ch_trigger_tp_keeps_momentum_velocity.GetBool()) {
		if (CVars::bxt_ch_trigger_tp_keeps_momentum_velocity_redirect.GetBool()) {
			// https://github.com/fireblizzard/agmod/blob/bf06e4ffd31c1427784685118820e15552803bcb/dlls/triggers.cpp#L1935
			// After teleportation, pevToucher has the same viewangles as pentTarget.
			Vector vecAngles = Vector(0, pev->v_angle.y, 0);
			Vector vecForward;

			pEngfuncs->pfnAngleVectors(vecAngles, vecForward, nullptr, nullptr);

			// For velocity
			float xy_vel = prev_vel.Length2D();

			pev->velocity.x = vecForward.x * xy_vel;
			pev->velocity.y = vecForward.y * xy_vel;

			// For base velocity
			float xy_basevel = prev_basevelocity.Length2D();

			pev->basevelocity.x = vecForward.x * xy_basevel;
			pev->basevelocity.y = vecForward.y * xy_basevel;
		} else {
			pev->velocity = prev_vel;
			pev->basevelocity = prev_basevelocity;
		}
	}

	if (CVars::bxt_ch_trigger_tp_keeps_momentum_viewangles.GetBool()) {
		// In HLSDK, due to some inheritance stuffs, pevToucher's viewangles is changed differently.
		// In and only in TeleportTouch, pev->fixangle is set to 1.
		// If not set back to 0, we cannot set our viewangles, due to inheritance stuffs.
		pev->fixangle = 0;
		pev->v_angle = prev_view;
		pev->angles = prev_angles;
	}
}

HOOK_DEF_2(ServerDLL, void, __cdecl, DispatchKeyValue, edict_t*, pentKeyvalue, KeyValueData*, pkvd)
{
	if (pkvd && pentKeyvalue
		&& pkvd->szClassName // some entities don't have classname, this avoids crash.
		&& pkvd->szKeyName && pkvd->szValue // just to make sure
		&& !strcmp(pkvd->szClassName, "trigger_teleport") 
		&& !strcmp(pkvd->szKeyName, "landmark")
		) {

		edict_t *edicts;
		HwDLL::GetInstance().GetEdicts(&edicts);
		const auto index = pentKeyvalue - edicts;

		tpLandmarks[index] = pkvd->szValue;
		// Don't set pkvd->fHandled = 1, 
		// as some games could handle the same cases for the same entities, 
		// which means you won't give the game a chance to read that data!
		// -- _Smiley
		// pkvd->fHandled = 1;
	}

	ORIG_DispatchKeyValue(pentKeyvalue, pkvd);
}

// std::optional is at least C++17 :DDDDDDDDDDDDDDDDDDDDDDD
std::tuple<bool, Vector> TriggerTpLandmarkBefore(bool enabled, entvars_t *this_pev, entvars_t *pev, enginefuncs_t *pEngfuncs)
{
	if (!enabled)
		return std::make_tuple(false, Vector());

	const auto index = ServerDLL::GetInstance().pEngfuncs->pfnIndexOfEdict(this_pev->pContainingEntity);
	const auto tpLandmarks = ServerDLL::GetInstance().tpLandmarks;

	if (!tpLandmarks.count(index))
		return std::make_tuple(false, Vector());

	const auto landmarkName = tpLandmarks.at(index);
	const edict_t *landmark = pEngfuncs->pfnFindEntityByString(NULL, "targetname", landmarkName.c_str());

	if (HwDLL::GetInstance().IsValidEdict(landmark))
		return std::make_tuple(true, Vector(pev->origin) - Vector(landmark->v.origin));

	return std::make_tuple(false, Vector());
}

void TriggerTpLandmarkAfter(entvars_t *pev, Vector offset)
{
	pev->origin = pev->origin + offset;
	// have to offset by some HULL because of origin z diff
	const auto is_duck = pev->bInDuck || pev->flags & (FL_DUCKING);
	const auto hull_offset = is_duck ? VEC_DUCK_HULL_MIN : VEC_HULL_MIN;

	pev->origin[2] += hull_offset[2];
	// https://github.com/ValveSoftware/halflife/blob/c7240b965743a53a29491dd49320c88eecf6257b/dlls/triggers.cpp#L1908
	pev->origin[2] -= 1;
}

void ServerDLL::ClearTPLandmarks()
{
	tpLandmarks.clear();
}

HOOK_DEF_3(ServerDLL, void, __fastcall, CBaseTrigger__TeleportTouch, void*, thisptr, int, edx, void*, pOther)
{
	auto is_bxt_ch_trigger_tp_keeps_momentum_enabled = CVars::sv_cheats.GetBool() && CVars::bxt_ch_trigger_tp_keeps_momentum.GetBool();
	auto is_bxt_ch_trigger_tp_landmark_enabled = CVars::sv_cheats.GetBool() && CVars::bxt_ch_trigger_tp_landmark.GetBool() && HwDLL::GetInstance().ppGlobals;

	entvars_t *this_pev = *reinterpret_cast<entvars_t**>(reinterpret_cast<uintptr_t>(thisptr) + 4);
	entvars_t *pev = *reinterpret_cast<entvars_t**>(reinterpret_cast<uintptr_t>(pOther) + 4);
	Vector prev_vel;
	Vector prev_view;
	Vector prev_angles;
	Vector prev_basevelocity;

	if (pev) {
		prev_vel = pev->velocity;
		prev_view = pev->v_angle;
		prev_angles = pev->angles;
		prev_basevelocity = pev->basevelocity;
	}

	const auto landmark_info = TriggerTpLandmarkBefore(is_bxt_ch_trigger_tp_landmark_enabled, this_pev, pev, pEngfuncs);

	ORIG_CBaseTrigger__TeleportTouch(thisptr, edx, pOther);

	if (is_bxt_ch_trigger_tp_keeps_momentum_enabled && pev && pEngfuncs && IsPlayer(pev->pContainingEntity)) {
		TriggerTpKeepsMomentumRestore(prev_vel, prev_vel, prev_angles, prev_basevelocity, pev, pEngfuncs);
	}

	if (is_bxt_ch_trigger_tp_landmark_enabled && std::get<0>(landmark_info)) {
		TriggerTpLandmarkAfter(pev, std::get<1>(landmark_info));
	}
}

HOOK_DEF_2(ServerDLL, void, __cdecl, CBaseTrigger__TeleportTouch_Linux, void*, thisptr, void*, pOther)
{
	auto is_bxt_ch_trigger_tp_keeps_momentum_enabled = CVars::sv_cheats.GetBool() && CVars::bxt_ch_trigger_tp_keeps_momentum.GetBool();
	auto is_bxt_ch_trigger_tp_landmark_enabled = CVars::sv_cheats.GetBool() && CVars::bxt_ch_trigger_tp_landmark.GetBool() && pEngfuncs && HwDLL::GetInstance().ppGlobals;

	entvars_t *this_pev = *reinterpret_cast<entvars_t**>(reinterpret_cast<uintptr_t>(thisptr) + 4);
	entvars_t *pev = *reinterpret_cast<entvars_t**>(reinterpret_cast<uintptr_t>(pOther) + 4);
	Vector prev_vel;
	Vector prev_view;
	Vector prev_angles;
	Vector prev_basevelocity;

	if (pev) {
		prev_vel = pev->velocity;
		prev_view = pev->v_angle;
		prev_angles = pev->angles;
		prev_basevelocity = pev->basevelocity;
	}

	const auto landmark_info = TriggerTpLandmarkBefore(is_bxt_ch_trigger_tp_landmark_enabled, this_pev, pev, pEngfuncs);

	ORIG_CBaseTrigger__TeleportTouch_Linux(thisptr, pOther);

	if (is_bxt_ch_trigger_tp_keeps_momentum_enabled && pev && pEngfuncs && IsPlayer(pev->pContainingEntity)) {
		TriggerTpKeepsMomentumRestore(prev_vel, prev_vel, prev_angles, prev_basevelocity, pev, pEngfuncs);
	}

	if (is_bxt_ch_trigger_tp_landmark_enabled && std::get<0>(landmark_info)) {
		TriggerTpLandmarkAfter(pev, std::get<1>(landmark_info));
	}
}

#undef ALERT