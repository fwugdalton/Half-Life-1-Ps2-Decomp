/* dlls/h_cine.cpp: os monstros de cinemática antigos (monster_cine*) e o
   cine_blood do HLSDK, código obsoleto já no PC. Igual ao SDK. */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/decals.h"
#include "include_asm.h"

void UTIL_BloodStream( const Vector &origin, const Vector &direction, int color, int amount );
Vector UTIL_RandomBloodVector( void );
void UTIL_BloodDecalTrace( TraceResult *pTrace, int bloodColor );

class CLegacyCineMonster : public CBaseMonster
{
public:
	void CineSpawn( char *szModel );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void EXPORT CineThink( void );
	void Pain( void );
	void Die( void );
};

class CCineScientist : public CLegacyCineMonster
{
public:
	void Spawn( void ) { CineSpawn("models/cine-scientist.dol"); }
};
class CCine2Scientist : public CLegacyCineMonster
{
public:
	void Spawn( void ) { CineSpawn("models/cine2-scientist.dol"); }
};
class CCinePanther : public CLegacyCineMonster
{
public:
	void Spawn( void ) { CineSpawn("models/cine-panther.dol"); }
};

class CCineBarney : public CLegacyCineMonster
{
public:
	void Spawn( void ) { CineSpawn("models/cine-barney.dol"); }
};

class CCine2HeavyWeapons : public CLegacyCineMonster
{
public:
	void Spawn( void ) { CineSpawn("models/cine2_hvyweapons.dol"); }
};

class CCine2Slave : public CLegacyCineMonster
{
public:
	void Spawn( void ) { CineSpawn("models/cine2_slave.dol"); }
};

class CCine3Scientist : public CLegacyCineMonster
{
public:
	void Spawn( void ) { CineSpawn("models/cine3-scientist.dol"); }
};

class CCine3Barney : public CLegacyCineMonster
{
public:
	void Spawn( void ) { CineSpawn("models/cine3-barney.dol"); }
};

//
// ********** Scientist SPAWN **********
//

LINK_ENTITY_TO_CLASS( monster_cine_scientist, CCineScientist );
LINK_ENTITY_TO_CLASS( monster_cine_panther, CCinePanther );
LINK_ENTITY_TO_CLASS( monster_cine_barney, CCineBarney );
LINK_ENTITY_TO_CLASS( monster_cine2_scientist, CCine2Scientist );
LINK_ENTITY_TO_CLASS( monster_cine2_hvyweapons, CCine2HeavyWeapons );
LINK_ENTITY_TO_CLASS( monster_cine2_slave, CCine2Slave );
LINK_ENTITY_TO_CLASS( monster_cine3_scientist, CCine3Scientist );
LINK_ENTITY_TO_CLASS( monster_cine3_barney, CCine3Barney );

//
// ********** Scientist SPAWN **********
//

void CLegacyCineMonster :: CineSpawn( char *szModel )
{
	PRECACHE_MODEL(szModel);
	SET_MODEL(ENT(pev), szModel);
	UTIL_SetSize(pev, Vector(-16, -16, 0), Vector(16, 16, 64));

	pev->solid			= SOLID_SLIDEBOX;
	pev->movetype		= MOVETYPE_STEP;
	pev->effects		= 0;
	pev->health			= 1;
	pev->yaw_speed		= 10;
	
	// ugly alpha hack, can't set ints from the bsp.	
	pev->sequence		= (int)pev->impulse;
	ResetSequenceInfo( );
	pev->framerate = 0.0f;

	m_bloodColor = BLOOD_COLOR_RED;

	// if no targetname, start now
	if ( FStringNull(pev->targetname) )	
	{
		SetThink( &CLegacyCineMonster::CineThink );
		pev->nextthink += 1.0f;
	}
}


//
// CineStart
//
void CLegacyCineMonster :: Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	pev->animtime = 0;	// reset the sequence
	SetThink( &CLegacyCineMonster::CineThink );
	pev->nextthink = gpGlobals->time;
}

//
// ********** Scientist DIE **********
//
void CLegacyCineMonster :: Die( void )
{
	SetThink( &CLegacyCineMonster::SUB_Remove );
}

//
// ********** Scientist PAIN **********
//
void CLegacyCineMonster :: Pain( void )
{
	EMIT_SOUND(ENT(pev), CHAN_VOICE, "player/pain3.wav", 1, ATTN_NORM);
}

void CLegacyCineMonster :: CineThink( void )
{
	// DBG_CheckMonsterData(pev);
	
	// Emit particles from origin (double check animator's placement of model)
	// THIS is a test feature
	//UTIL_ParticleEffect(pev->origin, g_vecZero, 255, 20);

	if (!pev->animtime)
		ResetSequenceInfo( );

	pev->nextthink = gpGlobals->time + 1.0f;

	if (pev->spawnflags != 0 && m_fSequenceFinished)
	{
		Die();
		return;
	}

	StudioFrameAdvance ( );
}

//
// cine_blood
//
// e3/prealpha only. 
class CCineBlood : public CBaseEntity
{
public:
	void Spawn( void );
	void EXPORT BloodStart ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void EXPORT BloodGush ( void );
};

LINK_ENTITY_TO_CLASS( cine_blood, CCineBlood );


void CCineBlood :: BloodGush ( void )
{
	Vector	vecSplatDir;
	TraceResult	tr;
	pev->nextthink = gpGlobals->time + 0.1f;

	UTIL_MakeVectors(pev->angles);
	if ( pev->health-- < 0 )
		REMOVE_ENTITY(ENT(pev));
// CHANGE_METHOD ( ENT(pev), em_think, SUB_Remove );

	if ( RANDOM_FLOAT ( 0 , 1 ) < 0.7f )// larger chance of globs
	{
		UTIL_BloodDrips( pev->origin, UTIL_RandomBloodVector(), BLOOD_COLOR_RED, 10 );
	}
	else// slim chance of geyser
	{
		UTIL_BloodStream( pev->origin, UTIL_RandomBloodVector(), BLOOD_COLOR_RED, RANDOM_LONG(50, 150) );
	}

	if ( RANDOM_FLOAT ( 0, 1 ) < 0.75f )
	{// decals the floor with blood.
		vecSplatDir = Vector ( 0 , 0 , -1 );
		vecSplatDir = vecSplatDir + (RANDOM_FLOAT(-1,1) * 0.6f * gpGlobals->v_right) + (RANDOM_FLOAT(-1,1) * 0.6f * gpGlobals->v_forward);// randomize a bit
		UTIL_TraceLine( pev->origin + Vector ( 0, 0 , 64) , pev->origin + vecSplatDir * 256, ignore_monsters, ENT(pev), &tr);
		if ( tr.flFraction != 1.0f )
		{
			// Decal with a bloodsplat
			UTIL_BloodDecalTrace( &tr, BLOOD_COLOR_RED );
		}
	}
}

void CCineBlood :: BloodStart ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	SetThink( &CCineBlood::BloodGush );
	pev->nextthink = gpGlobals->time;// now!
}

void CCineBlood :: Spawn ( void )
{
	pev->solid = SOLID_NOT;
	SetUse ( &CCineBlood::BloodStart );
	pev->health = 20;//hacked health to count iterations
}


/* registro dos ponteiros de Think e Use para o save do PS2 */
extern "C" void func_002397D0(const char *name, BASEPTR pfn);
extern "C" void func_00239810(const char *name, USEPTR pfn);

extern "C" void func_0017EA78(void)
{
	func_002397D0("136", static_cast<BASEPTR>(&CLegacyCineMonster::CineThink));
	func_002397D0("137", static_cast<BASEPTR>(&CCineBlood::BloodGush));
	func_002397D0("138", static_cast<BASEPTR>(&CLegacyCineMonster::SUB_Remove));
	func_00239810("139", static_cast<USEPTR>(&CCineBlood::BloodStart));
}
