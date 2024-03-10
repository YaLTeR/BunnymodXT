/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
/*

Class Hierachy

CBaseEntity -> CBaseDelay -> CBaseAnimating -> CBasePlayerItem -> CBasePlayerWeapon
CBaseEntity -> CBaseDelay -> CBaseAnimating -> CBaseToggle -> CBaseButton
CBaseEntity -> CBaseDelay -> CBaseAnimating -> CBaseToggle -> CBaseDoor
CBaseEntity -> CBaseDelay -> CBaseAnimating -> CBaseToggle -> CBaseMonster -> CBasePlayer
CBaseEntity -> CBaseDelay -> CBaseAnimating -> CBaseToggle -> CBaseMonster -> CMonsterMaker
CBaseEntity -> CBaseDelay -> CBaseAnimating -> CBaseToggle -> CBaseMonster -> CSquadMonster
CBaseEntity -> CBaseDelay -> CBaseAnimating -> CBaseToggle -> CBaseMonster -> CTalkMonster
CBaseEntity -> CBaseDelay -> CBaseAnimating -> CBaseToggle -> CBasePlatTrain -> CFuncTrain
CBaseEntity -> CBaseDelay -> CBaseAnimating -> CBaseToggle -> CBaseTrigger -> CChangeLevel
CBaseEntity -> CBaseDelay -> CBaseAnimating -> CBaseToggle -> CMultiManager
CBaseEntity -> CBaseDelay -> CBreakable -> CPushable
CBaseEntity -> CBaseDelay -> CTriggerRelay
CBaseEntity -> CBasePlayerAmmo
CBaseEntity -> CPointEntity -> CEnvGlobal
CBaseEntity -> CPointEntity -> CMultiSource
*/

#ifndef CBASE_H
#define CBASE_H

// These are caps bits to indicate what an object's capabilities (currently used for save/restore and level transitions)
const unsigned FCAP_CUSTOMSAVE = 0x00000001;
const unsigned FCAP_ACROSS_TRANSITION = 0x00000002;		// should transfer between transitions
const unsigned FCAP_MUST_SPAWN = 0x00000004;		// Spawn after restore
const unsigned FCAP_DONT_SAVE = 0x80000000;		// Don't save this
const unsigned FCAP_IMPULSE_USE = 0x00000008;		// can be used by the player
const unsigned FCAP_CONTINUOUS_USE = 0x00000010;		// can be used by the player
const unsigned FCAP_ONOFF_USE = 0x00000020;		// can be used by the player
const unsigned FCAP_DIRECTIONAL_USE = 0x00000040;		// Player sends +/- 1 when using (currently only tracktrains)
const unsigned FCAP_MASTER = 0x00000080;		// Can be used to "master" other entities (like multisource)

// UNDONE: This will ignore transition volumes (trigger_transition), but not the PVS!!!
const unsigned FCAP_FORCE_TRANSITION = 0x00000080;		// ALWAYS goes across transitions

#define	SF_NORESPAWN (1 << 30) // !!!set this bit on guns and stuff that should never respawn.

#endif
