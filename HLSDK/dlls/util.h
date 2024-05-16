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

#ifndef UTIL_DLLS_H
#define UTIL_DLLS_H

const Vector VEC_HULL_MIN(-16, -16, -36);
const Vector VEC_HULL_MAX(16, 16, 36);
const Vector VEC_VIEW(0, 0, 28);

const Vector VEC_DUCK_HULL_MIN(-16, -16, -18);
const Vector VEC_DUCK_HULL_MAX(16, 16, 18);
const Vector VEC_DUCK_VIEW(0, 0, 12);

const Vector VEC_HUMAN_HULL_MIN(-16, -16, 0);
const Vector VEC_HUMAN_HULL_MAX(16, 16, 72);
const Vector VEC_HUMAN_HULL_DUCK(16, 16, 36);

// triggers
#define	SF_TRIGGER_ALLOWMONSTERS	1 // monsters allowed to fire this trigger
#define	SF_TRIGGER_NOCLIENTS		2 // players not allowed to fire this trigger
#define SF_TRIGGER_PUSHABLES		4 // only pushables can fire this trigger

// func breakable
#define SF_BREAK_TRIGGER_ONLY	1 // may only be broken by trigger
#define	SF_BREAK_TOUCH			2 // can be 'crashed through' by running player (plate glass)
#define SF_BREAK_PRESSURE		4 // can be broken by a player standing on it
#define SF_BREAK_CROWBAR		256 // instant break if hit with crowbar

// func_pushable (it's also func_breakable, so don't collide with those flags)
#define SF_PUSH_BREAKABLE		128

#endif