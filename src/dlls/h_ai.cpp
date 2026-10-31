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
/* dlls/h_ai.cpp: funções utilitárias de IA (FBoxVisible, VecCheckToss, VecCheckThrow) do HLSDK. */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "include_asm.h"

typedef struct ps2cvar_s
{
	char	*name;
	char	*string;
	int		flags;
	float	value;
	struct ps2cvar_s *next;
} ps2cvar_t;

extern "C" ps2cvar_t *D_00312984;
#define g_psv_gravity D_00312984

enum { ignore_glass = 1, dont_ignore_glass = 0 };
void UTIL_TraceLine( const Vector &vecStart, const Vector &vecEnd, int igmon, int ignoreGlass, edict_t *pentIgnore, TraceResult *ptr );
void UTIL_TraceLineNoEnts( const Vector &vecStart, const Vector &vecEnd, int igmon, edict_t *pentIgnore, TraceResult *ptr );

BOOL	g_fDrawLines = FALSE;

//=========================================================
// FBoxVisible - a more accurate ( and slower ) version
// of FVisible. 
//=========================================================
BOOL FBoxVisible ( entvars_t *pevLooker, entvars_t *pevTarget, Vector &vecTargetOrigin, float flSize )
{
	// don't look through water
	if ((pevLooker->waterlevel != 3 && pevTarget->waterlevel == 3) 
		|| (pevLooker->waterlevel == 3 && pevTarget->waterlevel == 0))
		return FALSE;

	TraceResult tr;
	Vector	vecLookerOrigin = pevLooker->origin + pevLooker->view_ofs;//look through the monster's 'eyes'
	for (int i = 0; i < 5; i++)
	{
		Vector vecTarget = pevTarget->origin;
		vecTarget.x += RANDOM_FLOAT( pevTarget->mins.x + flSize, pevTarget->maxs.x - flSize);
		vecTarget.y += RANDOM_FLOAT( pevTarget->mins.y + flSize, pevTarget->maxs.y - flSize);
		vecTarget.z += RANDOM_FLOAT( pevTarget->mins.z + flSize, pevTarget->maxs.z - flSize);

		UTIL_TraceLine(vecLookerOrigin, vecTarget, ignore_monsters, ignore_glass, ENT(pevLooker)/*pentIgnore*/, &tr);
		
		if (tr.flFraction == 1.0f)
		{
			vecTargetOrigin = vecTarget;
			return TRUE;// line of sight is valid.
		}
	}
	return FALSE;// Line of sight is not established
}

//
// VecCheckToss - returns the velocity at which an object should be lobbed from vecspot1 to land near vecspot2.
// returns g_vecZero if toss is not feasible.
// 
Vector VecCheckToss ( entvars_t *pev, const Vector &vecSpot1, Vector vecSpot2, float flGravityAdj )
{
	TraceResult		tr;
	Vector			vecMidPoint;// halfway point between Spot1 and Spot2
	Vector			vecApex;// highest point 
	Vector			vecGrenadeVel;
	float			flGravity = g_psv_gravity->value * flGravityAdj;

	if (vecSpot2.z - vecSpot1.z > 500)
	{
		// to high, fail
		return g_vecZero;
	}

	UTIL_MakeVectors (pev->angles);

	// toss a little bit to the left or right, not right down on the enemy's bean (head). 
	vecSpot2 = vecSpot2 + gpGlobals->v_right * ( RANDOM_FLOAT(-8,8) + RANDOM_FLOAT(-16,16) );
	vecSpot2 = vecSpot2 + gpGlobals->v_forward * ( RANDOM_FLOAT(-8,8) + RANDOM_FLOAT(-16,16) );
	
	// get a rough idea of how high it can be thrown
	vecMidPoint = vecSpot1 + (vecSpot2 - vecSpot1) * 0.5;
	UTIL_TraceLine(vecMidPoint, vecMidPoint + Vector(0,0,500), ignore_monsters, ENT(pev), &tr);
	vecMidPoint = tr.vecEndPos;
	// (subtract 15 so the grenade doesn't hit the ceiling)
	vecMidPoint.z -= 15;

	if (vecMidPoint.z < vecSpot1.z || vecMidPoint.z < vecSpot2.z)
	{
		// to not enough space, fail
		return g_vecZero;
	}

	// How high should the grenade travel to reach the apex
	float distance1 = (vecMidPoint.z - vecSpot1.z);
	float distance2 = (vecMidPoint.z - vecSpot2.z);

	// How long will it take for the grenade to travel this distance
	float time1 = sqrtf( distance1 / (0.5f * flGravity) );
	float time2 = sqrtf( distance2 / (0.5f * flGravity) );

	if (time1 < 0.1f)
	{
		// too close
		return g_vecZero;
	}

	// how hard to throw sideways to get there in time.
	vecGrenadeVel = (vecSpot2 - vecSpot1) / (time1 + time2);
	// how hard upwards to reach the apex at the right time.
	vecGrenadeVel.z = flGravity * time1;

	// find the apex
	vecApex  = vecSpot1 + vecGrenadeVel * time1;
	vecApex.z = vecMidPoint.z;

	UTIL_TraceLineNoEnts(vecSpot1, vecApex, dont_ignore_monsters, ENT(pev), &tr);	// PS2: era UTIL_TraceLine
	if (tr.flFraction != 1.0f)
	{
		// fail!
		return g_vecZero;
	}

	// UNDONE: either ignore monsters or change it to not care if we hit our enemy
	UTIL_TraceLine(vecSpot2, vecApex, ignore_monsters, ENT(pev), &tr); 
	if (tr.flFraction != 1.0f)
	{
		// fail!
		return g_vecZero;
	}
	
	return vecGrenadeVel;
}

//
// VecCheckThrow - returns the velocity vector at which an object should be thrown from vecspot1 to hit vecspot2.
// returns g_vecZero if throw is not feasible.
// 
Vector VecCheckThrow ( entvars_t *pev, const Vector &vecSpot1, Vector vecSpot2, float flSpeed, float flGravityAdj )
{
	float			flGravity = g_psv_gravity->value * flGravityAdj;

	Vector vecGrenadeVel = (vecSpot2 - vecSpot1);

	// throw at a constant time
	float time = vecGrenadeVel.Length( ) / flSpeed;
	vecGrenadeVel = vecGrenadeVel * (1.0f / time);

	// adjust upward toss to compensate for gravity loss
	vecGrenadeVel.z += flGravity * time * 0.5f;

	Vector vecApex = vecSpot1 + (vecSpot2 - vecSpot1) * 0.5;
	vecApex.z += 0.5f * flGravity * (time * 0.5f) * (time * 0.5f);
	
	TraceResult tr;
	UTIL_TraceLineNoEnts(vecSpot1, vecApex, dont_ignore_monsters, ENT(pev), &tr);	// PS2: era UTIL_TraceLine
	if (tr.flFraction != 1.0f)
	{
		// fail!
		return g_vecZero;
	}

	UTIL_TraceLine(vecSpot2, vecApex, ignore_monsters, ENT(pev), &tr);
	if (tr.flFraction != 1.0f)
	{
		// fail!
		return g_vecZero;
	}

	return vecGrenadeVel;
}
