//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#if !defined EDICT_H
#define EDICT_H
#ifdef _WIN32
#pragma once
#endif
#define	MAX_ENT_LEAFS	48

#include "progdefs.h"

/*
	Size of 'edict_t' in HLSDK 1.0 is 0x2C8 (712)
	Size of 'edict_t' in HLSDK 2.0, Sven Co-op is 0x324 (804)
	Size of 'edict_t' in Cry of Fear [Steam] is 0x32C (812)
	Size of 'edict_t' in James Bond 007: Nightfire [PC] is 0x3E0 (992)
*/

struct edict_s
{
	qboolean	free;
	int			serialnumber;
	link_t		area;				// linked to a division node or leaf
	
	int			headnode;			// -1 to use normal leaf check
	int			num_leafs;
	short		leafnums[MAX_ENT_LEAFS];

	float		freetime;			// sv.time when the object was freed

	void*		pvPrivateData;		// Alloced and freed by engine, used by DLLs

	entvars_t	v;					// C exported fields from progs

	// other fields from progs come immediately after
};

#endif
