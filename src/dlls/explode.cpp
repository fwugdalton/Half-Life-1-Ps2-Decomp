/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
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

===== explode.cpp ========================================================

  Explosion-related code

*/
#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/decals.h"
#include "include_asm.h"
#include "inline_include_asm.h"

#define TE_EXPLOSION		3
#define TE_SMOKE			5
#define TE_EXPLFLAG_NONE	0

class CBasePlayer;
extern "C" void func_002071C8( CBasePlayer *pPlayer, int iUnk, unsigned char intensity, unsigned short duration );

/* explode.h */
#define	SF_ENVEXPLOSION_NODAMAGE	( 1 << 0 ) // when set, ENV_EXPLOSION will not actually inflict damage
#define	SF_ENVEXPLOSION_REPEATABLE	( 1 << 1 ) // can this entity be refired?
#define SF_ENVEXPLOSION_NOFIREBALL	( 1 << 2 ) // don't draw the fireball
#define SF_ENVEXPLOSION_NOSMOKE		( 1 << 3 ) // don't draw the smoke
#define SF_ENVEXPLOSION_NODECAL		( 1 << 4 ) // don't make a scorch mark
#define SF_ENVEXPLOSION_NOSPARKS	( 1 << 5 ) // don't make a scorch mark

extern short g_sModelIndexFireball;
extern short g_sModelIndexSmoke;


extern void ExplosionCreate( const Vector &center, const Vector &angles, edict_t *pOwner, int magnitude, BOOL doDamage );



// Spark Shower
class CShower : public CBaseEntity
{
	void Spawn( void );
	void Think( void );
	void Touch( CBaseEntity *pOther );
	int ObjectCaps( void ) { return FCAP_DONT_SAVE; }
}; 

LINK_ENTITY_TO_CLASS( spark_shower, CShower );

void CShower::Spawn( void )
{
	pev->velocity = RANDOM_FLOAT( 200, 300 ) * pev->angles;
	pev->velocity.x += RANDOM_FLOAT(-100.f,100.f);
	pev->velocity.y += RANDOM_FLOAT(-100.f,100.f);
	if ( pev->velocity.z >= 0 )
		pev->velocity.z += 200;
	else
		pev->velocity.z -= 200;
	pev->movetype = MOVETYPE_BOUNCE;
	pev->gravity = 0.5f;
	pev->nextthink = gpGlobals->time + 0.1f;
	pev->solid = SOLID_NOT;
	SET_MODEL( edict(), "models/grenade.dol");	// Need a model, just use the grenade, we don't draw it anyway
	UTIL_SetSize(pev, g_vecZero, g_vecZero );
	pev->effects |= EF_NODRAW;
	pev->speed = RANDOM_FLOAT( 0.5f, 1.5f );

	pev->angles = g_vecZero;
}


void CShower::Think( void )
{
	UTIL_Sparks( pev->origin );

	pev->speed -= 0.1f;
	if ( pev->speed > 0 )
		pev->nextthink = gpGlobals->time + 0.1f;
	else
		UTIL_Remove( this );
	pev->flags &= ~FL_ONGROUND;
}

void CShower::Touch( CBaseEntity *pOther )
{
	if ( pev->flags & FL_ONGROUND )
		pev->velocity = pev->velocity * 0.1f;
	else
		pev->velocity = pev->velocity * 0.6f;

	if ( (pev->velocity.x*pev->velocity.x+pev->velocity.y*pev->velocity.y) < 10.0f )
		pev->speed = 0;
}

class CEnvExplosion : public CBaseMonster
{
public:
	void Spawn( );
	void EXPORT Smoke ( void );
	void KeyValue( KeyValueData *pkvd );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	int m_iMagnitude;// how large is the fireball? how much damage?
	int m_spriteScale; // what's the exact fireball sprite scale? 
};

TYPEDESCRIPTION	CEnvExplosion::m_SaveData[] = 
{
	DEFINE_FIELD( CEnvExplosion, m_iMagnitude, FIELD_INTEGER ),
	DEFINE_FIELD( CEnvExplosion, m_spriteScale, FIELD_INTEGER ),
};

IMPLEMENT_SAVERESTORE( CEnvExplosion, CBaseMonster );
LINK_ENTITY_TO_CLASS( env_explosion, CEnvExplosion );

void CEnvExplosion::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "iMagnitude"))
	{
		m_iMagnitude = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}

void CEnvExplosion::Spawn( void )
{ 
	pev->solid = SOLID_NOT;
	pev->effects = EF_NODRAW;

	pev->movetype = MOVETYPE_NONE;
	/*
	if ( m_iMagnitude > 250 )
	{
		m_iMagnitude = 250;
	}
	*/

	float flSpriteScale;
	flSpriteScale = ( m_iMagnitude - 50) * 0.6f;
	
	/*
	if ( flSpriteScale > 50 )
	{
		flSpriteScale = 50;
	}
	*/
	if ( flSpriteScale < 10 )
	{
		flSpriteScale = 10;
	}

	m_spriteScale = (int)flSpriteScale;
}

void CEnvExplosion::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{ 
#ifndef NON_MATCHING
	/* não casa (a conversão float -> int da vibração, m_iMagnitude * 1.7, sai com o
	   trunc.w.s em outro registrador e outra ordem, como nos outros arquivos); o corpo
	   vem do asm */
	INLINE_INCLUDE_ASM( "asm/nonmatchings/dlls/explode", Use__13CEnvExplosionP11CBaseEntityT18USE_TYPEf );
#endif
	TraceResult tr;

	pev->model = iStringNull;//invisible
	pev->solid = SOLID_NOT;// intangible

	Vector		vecSpot;// trace starts here!

	vecSpot = pev->origin + Vector ( 0 , 0 , 8 );
	
	UTIL_TraceLine ( vecSpot, vecSpot + Vector ( 0, 0, -40 ),  ignore_monsters, ENT(pev), & tr);
	
	// Pull out of the wall a bit
	if ( tr.flFraction != 1.0f )
	{
		pev->origin = tr.vecEndPos + (tr.vecPlaneNormal * (m_iMagnitude - 24) * 0.6f);
	}
	else
	{
		pev->origin = pev->origin;
	}

	// draw decal
	if (! ( pev->spawnflags & SF_ENVEXPLOSION_NODECAL))
	{
		if ( RANDOM_FLOAT( 0 , 1 ) < 0.5f )
		{
			UTIL_DecalTrace( &tr, DECAL_SCORCH1 );
		}
		else
		{
			UTIL_DecalTrace( &tr, DECAL_SCORCH2 );
		}
	}

	// PS2: vibra o controle dos jogadores perto da explosão
	CBaseEntity *pPlayer1 = CBaseEntity::Instance( INDEXENT( 1 ) );
	CBaseEntity *pPlayer2 = CBaseEntity::Instance( INDEXENT( 2 ) );

	if ( pPlayer1 && pPlayer1->IsPlayer() )
	{
		if ( ( pev->origin - pPlayer1->pev->origin ).Length() < 800 )
			func_002071C8( (CBasePlayer *)pPlayer1, 1, (BYTE)( m_iMagnitude * 1.7f ), 250 );
	}

	if ( pPlayer2 && pPlayer2->IsPlayer() )
	{
		if ( ( pev->origin - pPlayer2->pev->origin ).Length() < 800 )
			func_002071C8( (CBasePlayer *)pPlayer2, 1, (BYTE)( m_iMagnitude * 1.7f ), 250 );
	}

	// draw fireball
	if ( !( pev->spawnflags & SF_ENVEXPLOSION_NOFIREBALL ) )
	{
		MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_EXPLOSION);
			WRITE_COORD( pev->origin.x );
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z );
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( (BYTE)m_spriteScale ); // scale * 10
			WRITE_BYTE( 15  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();
	}
	else
	{
		MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_EXPLOSION);
			WRITE_COORD( pev->origin.x );
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z );
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( 0 ); // no sprite
			WRITE_BYTE( 15  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();
	}

	// do damage
	if ( !( pev->spawnflags & SF_ENVEXPLOSION_NODAMAGE ) )
	{
		RadiusDamage ( pev, pev, m_iMagnitude, CLASS_NONE, DMG_BLAST );
	}

	SetThink( &CEnvExplosion::Smoke );
	pev->nextthink = gpGlobals->time + 0.3f;

	// draw sparks
	if ( !( pev->spawnflags & SF_ENVEXPLOSION_NOSPARKS ) )
	{
		int sparkCount = RANDOM_LONG(0,3);

		for ( int i = 0; i < sparkCount; i++ )
		{
			Create( "spark_shower", pev->origin, tr.vecPlaneNormal, NULL );
		}
	}
}

void CEnvExplosion::Smoke( void )
{
	if ( !( pev->spawnflags & SF_ENVEXPLOSION_NOSMOKE ) )
	{
		MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_SMOKE );
			WRITE_COORD( pev->origin.x );
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z );
			WRITE_SHORT( g_sModelIndexSmoke );
			WRITE_BYTE( (BYTE)m_spriteScale ); // scale * 10
			WRITE_BYTE( 12  ); // framerate
		MESSAGE_END();
	}
	
	if ( !(pev->spawnflags & SF_ENVEXPLOSION_REPEATABLE) )
	{
		UTIL_Remove( this );
	}
}


// HACKHACK -- create one of these and fake a keyvalue to get the right explosion setup
void ExplosionCreate( const Vector &center, const Vector &angles, edict_t *pOwner, int magnitude, BOOL doDamage )
{
	KeyValueData	kvd;
	char			buf[128];

	CBaseEntity *pExplosion = CBaseEntity::Create( "env_explosion", center, angles, pOwner );
	sprintf( buf, "%3d", magnitude );
	kvd.szKeyName = "iMagnitude";
	kvd.szValue = buf;
	pExplosion->KeyValue( &kvd );
	if ( !doDamage )
		pExplosion->pev->spawnflags |= SF_ENVEXPLOSION_NODAMAGE;

	pExplosion->Spawn();
	pExplosion->Use( NULL, NULL, USE_TOGGLE, 0 );
}

/* PS2: registra o ponteiro de think para o save */
extern "C" void func_002397D0( const char *name, BASEPTR pfn );
extern "C" void func_00164370( void )
{
	func_002397D0( "105", static_cast<BASEPTR>( &CEnvExplosion::Smoke ) );
}
