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
/* dlls/modeltrain.cpp: monster_modeltrain, modelo (ou sprite) que segue path_corners como um
   func_train e, quando morto, cai e explode como o apache. Só existe no PS2
   (não está no HLSDK); os nomes das funções são reconstruídos. */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/saverestore.h"
#include "dlls/effects.h"

#define SF_MODELTRAIN_START_OFF		0x0040	// não começa a andar sozinho
#define SF_MODELTRAIN_NO_SEQUENCE	0x10000	// com START_OFF, usa a sequência indicada
#define SF_MODELTRAIN_KILLABLE		0x20000	// usar de novo quando ativo mata o trem
#define SF_MODELTRAIN_SPRITE		0x40000	// o modelo é um sprite animado

#define SF_CORNER_WAITFORTRIG		0x001
#define GIB_ALWAYS			2

#define TE_EXPLOSION		3
#define TE_SMOKE			5
#define TE_SPRITE			17
#define TE_BREAKMODEL		108
#define TE_EXPLFLAG_NONE	0
#define BREAK_METAL			0x02

extern short g_sModelIndexFireball;
extern short g_sModelIndexSmoke;
extern "C" double fmod( double x, double y );
void FireTargets( const char *targetName, CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

class CModelTrain : public CBaseToggle
{
public:
	void Spawn( void );
	void Precache( void );
	void KeyValue( KeyValueData *pkvd );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	virtual int	ObjectCaps( void ) { return 0; }
	virtual int	Classify( void ) { return CLASS_MACHINE; }
	virtual int	BloodColor( void ) { return DONT_BLEED; }

	void Killed( entvars_t *pevAttacker, int iGib );
	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType );

	void EXPORT TrainUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void LinearMove( Vector vecDest, float flSpeed );
	void Animate( void );
	void EXPORT StartThink( void );
	void EXPORT MoveThink( void );
	float ChangeYaw( int yawSpeed );
	void EXPORT TurnThink( void );
	void EXPORT Next( void );
	void DelayThink( void );
	void EXPORT CrashTouch( CBaseEntity *pOther );
	void EXPORT DyingThink( void );
	void AdvanceSpriteFrame( float flInterval );

	CBaseEntity *m_pGoalEnt;	// 0xD0
	float	m_flStartTime;		// 0xD4
	float	m_flStopTime;		// 0xD8
	float	m_angSpeed;			// 0xDC
	Vector	m_velocity;			// 0xE0
	string_t m_iszExplode;		// 0xEC
	string_t m_iszBodyGibs;		// 0xF0
	int		m_iExplode;			// 0xF4
	int		m_iBodyGibs;		// 0xF8
	int		m_iUnused[2];		// 0xFC
	string_t m_iszRunningSequence;	// 0x104
	BOOL	m_fActive;			// 0x108
	float	m_flMaxSpriteFrame;	// 0x10C
	float	m_flLastFrameTime;	// 0x110
};

LINK_ENTITY_TO_CLASS( monster_modeltrain, CModelTrain );

TYPEDESCRIPTION	CModelTrain::m_SaveData[] =
{
	DEFINE_FIELD( CModelTrain, m_pGoalEnt, FIELD_CLASSPTR ),
	DEFINE_FIELD( CModelTrain, m_flStartTime, FIELD_TIME ),
	DEFINE_FIELD( CModelTrain, m_flStopTime, FIELD_TIME ),
	DEFINE_FIELD( CModelTrain, m_velocity, FIELD_VECTOR ),
	DEFINE_FIELD( CModelTrain, m_angSpeed, FIELD_FLOAT ),
	DEFINE_FIELD( CModelTrain, m_iszExplode, FIELD_STRING ),
	DEFINE_FIELD( CModelTrain, m_iszBodyGibs, FIELD_STRING ),
	DEFINE_FIELD( CModelTrain, m_fActive, FIELD_BOOLEAN ),
	DEFINE_FIELD( CModelTrain, m_iszRunningSequence, FIELD_STRING ),
	DEFINE_FIELD( CModelTrain, m_flMaxSpriteFrame, FIELD_FLOAT ),
	DEFINE_FIELD( CModelTrain, m_flLastFrameTime, FIELD_FLOAT ),
};

IMPLEMENT_SAVERESTORE( CModelTrain, CBaseToggle );


void CModelTrain :: KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "explode_sprite" ) )
	{
		if ( pkvd->szValue && *pkvd->szValue )
			m_iszExplode = ALLOC_STRING( pkvd->szValue );
		else
			m_iszExplode = 0;
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "body_gibs" ) )
	{
		if ( pkvd->szValue && *pkvd->szValue )
			m_iszBodyGibs = ALLOC_STRING( pkvd->szValue );
		else
			m_iszBodyGibs = 0;
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "running_sequence" ) )
	{
		if ( pkvd->szValue && *pkvd->szValue )
			m_iszRunningSequence = ALLOC_STRING( pkvd->szValue );
		else
			m_iszRunningSequence = 0;
		pkvd->fHandled = TRUE;
	}
	else
		CBaseToggle::KeyValue( pkvd );
}


void CModelTrain :: Spawn( void )
{
	Precache();

	pev->movetype = MOVETYPE_PUSH;
	pev->solid = SOLID_NOT;
	SET_MODEL( ENT(pev), STRING(pev->model) );
	UTIL_SetSize( pev, Vector( -1, -1, 0 ), Vector( 1, 1, 2 ) );
	UTIL_SetOrigin( pev, pev->origin );
	pev->takedamage = DAMAGE_NO;

	SetThink( &CModelTrain::StartThink );
	SetUse( &CModelTrain::TrainUse );

	if ( !( pev->spawnflags & SF_MODELTRAIN_START_OFF ) )
	{
		pev->nextthink = pev->ltime + 0.05f;
		m_fActive = TRUE;
	}
	else
	{
		m_fActive = FALSE;
	}

	if ( pev->spawnflags & SF_MODELTRAIN_SPRITE )
	{
		m_flMaxSpriteFrame = MODEL_FRAMES( pev->modelindex ) - 1.0f;
		pev->frame = 0;
	}
	else
	{
		if ( ( pev->spawnflags & ( SF_MODELTRAIN_NO_SEQUENCE | SF_MODELTRAIN_START_OFF ) ) == SF_MODELTRAIN_START_OFF )
		{
			pev->sequence = 0;
		}
		else if ( m_iszRunningSequence )
		{
			pev->sequence = LookupSequence( STRING( m_iszRunningSequence ) );
			ResetSequenceInfo();
			pev->frame = 0;
		}
		else
		{
			pev->sequence = 0;
		}
		ResetSequenceInfo();
		pev->frame = 0;
	}

	pev->ideal_yaw = pev->angles.y;
	m_pGoalEnt = NULL;
	SetMoveDone( &CModelTrain::Next );
}


void CModelTrain :: Precache( void )
{
	PRECACHE_MODEL( (char *)STRING(pev->model) );

	if ( m_iszExplode )
		m_iExplode = PRECACHE_MODEL( (char *)STRING( m_iszExplode ) );
	else
		m_iExplode = 0;

	if ( m_iszBodyGibs )
		m_iBodyGibs = PRECACHE_MODEL( (char *)STRING( m_iszBodyGibs ) );
	else
		m_iBodyGibs = 0;
}


void CModelTrain :: TrainUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !m_fActive )
	{
		if ( !m_pGoalEnt )
			StartThink();
		else
			TurnThink();
		m_fActive = TRUE;
	}
	else if ( pev->spawnflags & SF_MODELTRAIN_KILLABLE )
	{
		Killed( pActivator->pev, GIB_ALWAYS );
	}
}


void CModelTrain :: LinearMove( Vector vecDest, float flSpeed )
{
	m_vecFinalDest = vecDest;

	if ( vecDest == pev->origin )
	{
		LinearMoveDone();
		return;
	}

	Vector vecDestDelta = vecDest - pev->origin;
	float flTravelTime = vecDestDelta.Length() / flSpeed;

	m_flStopTime = pev->ltime + flTravelTime;
	pev->velocity = vecDestDelta / flTravelTime;

	SetThink( &CModelTrain::MoveThink );
	pev->nextthink = pev->ltime + 0.05f;
}


void CModelTrain :: Animate( void )
{
	if ( pev->spawnflags & SF_MODELTRAIN_SPRITE )
	{
		AdvanceSpriteFrame( pev->framerate * ( gpGlobals->time - m_flLastFrameTime ) );
		m_flLastFrameTime = gpGlobals->time;
	}
	else
	{
		StudioFrameAdvance( 0 );
		if ( m_fSequenceFinished && !m_fSequenceLoops )
		{
			pev->frame = 0;
			ResetSequenceInfo();
		}
	}
}


void CModelTrain :: StartThink( void )
{
	Animate();

	if ( !m_fActive )
	{
		pev->nextthink = pev->ltime + 0.05f;
		return;
	}

	if ( !m_pGoalEnt && pev->target )
	{
		m_pGoalEnt = CBaseEntity::Instance( FIND_ENTITY_BY_TARGETNAME( NULL, STRING( pev->target ) ) );
		LinearMove( m_pGoalEnt->pev->origin, pev->speed );
	}
}


void CModelTrain :: MoveThink( void )
{
	Animate();

	pev->nextthink = pev->ltime + 0.05f;

	if ( m_fActive && m_flStopTime < pev->ltime )
		LinearMoveDone();
}


float CModelTrain :: ChangeYaw( int yawSpeed )
{
	float ideal, current, move, speed;

	current = UTIL_AngleMod( pev->angles.y );
	ideal = pev->ideal_yaw;
	if ( current != ideal )
	{
		speed = (float)yawSpeed * gpGlobals->frametime * 10;
		move = ideal - current;

		if ( ideal > current )
		{
			if ( move >= 180 )
				move = move - 360;
		}
		else
		{
			if ( move <= -180 )
				move = move + 360;
		}

		if ( move > 0 )
		{// turning to the monster's left
			if ( move > speed )
				move = speed;
		}
		else
		{// turning to the monster's right
			if ( move < -speed )
				move = -speed;
		}

		pev->angles.y = UTIL_AngleMod( current + move );
	}
	else
		move = 0;

	return move;
}


void CModelTrain :: TurnThink( void )
{
	Animate();

	if ( m_fActive )
	{
		if ( pev->angles.y == pev->ideal_yaw )
			goto arrived;
		ChangeYaw( pev->yaw_speed );
	}

	pev->nextthink = pev->ltime + 0.05f;
	return;

arrived:
	if ( m_pGoalEnt )
	{
		LinearMove( m_pGoalEnt->pev->origin, pev->speed );
	}
	else
	{
		SetThink( NULL );
		pev->nextthink = -1;
		pev->velocity = g_vecZero;
	}
}


void CModelTrain :: Next( void )
{
	Vector vecAngles;

	if ( m_pGoalEnt )
	{
		if ( m_pGoalEnt->pev->message && *STRING( m_pGoalEnt->pev->message ) )
			FireTargets( STRING( m_pGoalEnt->pev->message ), this, this, USE_TOGGLE, 0 );

		vecAngles = m_pGoalEnt->pev->angles;

		if ( m_pGoalEnt->pev->speed != 0 )
			pev->speed = m_pGoalEnt->pev->speed;

		if ( m_pGoalEnt->pev->yaw_speed != 0 )
			pev->yaw_speed = m_pGoalEnt->pev->yaw_speed;

		if ( m_pGoalEnt->pev->spawnflags & SF_CORNER_WAITFORTRIG )
		{
			if ( m_pGoalEnt->pev->target && *STRING( m_pGoalEnt->pev->target ) )
			{
				m_pGoalEnt = CBaseEntity::Instance( FIND_ENTITY_BY_TARGETNAME( NULL, STRING( m_pGoalEnt->pev->target ) ) );
			}
			else
			{
				m_pGoalEnt = NULL;
				return;
			}

			pev->ideal_yaw = UTIL_AngleMod( vecAngles.y );
			pev->velocity = g_vecZero;
			SetThink( &CModelTrain::TurnThink );
			pev->nextthink = pev->ltime + 0.05f;
			m_fActive = FALSE;
			return;
		}

		if ( m_pGoalEnt->pev->target && *STRING( m_pGoalEnt->pev->target ) )
			m_pGoalEnt = CBaseEntity::Instance( FIND_ENTITY_BY_TARGETNAME( NULL, STRING( m_pGoalEnt->pev->target ) ) );
		else
			m_pGoalEnt = NULL;
	}

	pev->ideal_yaw = UTIL_AngleMod( vecAngles.y );
	UTIL_MakeVectors( vecAngles );
	pev->velocity = gpGlobals->v_forward;

	if ( m_pGoalEnt && pev->angles.y == pev->ideal_yaw )
		LinearMove( m_pGoalEnt->pev->origin, pev->speed );
	else
		SetThink( &CModelTrain::TurnThink );

	pev->nextthink = pev->ltime + 0.05f;
}


void CModelTrain :: DelayThink( void )
{
	pev->nextthink = gpGlobals->time + 2.0f;
}


void CModelTrain :: Killed( entvars_t *pevAttacker, int iGib )
{
	pev->movetype = MOVETYPE_TOSS;
	pev->gravity = 0.3f;
	pev->velocity = m_velocity;
	pev->avelocity = Vector( RANDOM_FLOAT( -20, 20 ), 0, RANDOM_FLOAT( -50, 50 ) );
	STOP_SOUND( ENT(pev), CHAN_STATIC, "apache/ap_rotor4.wav" );

	UTIL_SetSize( pev, Vector( -32, -32, -64 ), Vector( 32, 32, 0 ) );
	SetThink( &CModelTrain::DyingThink );
	SetTouch( &CModelTrain::CrashTouch );
	pev->nextthink = gpGlobals->time + 0.1f;
	pev->health = 0;
	pev->takedamage = DAMAGE_NO;

	m_flStartTime = gpGlobals->time + 4.0f;
}


void CModelTrain :: CrashTouch( CBaseEntity *pOther )
{
	// only crash if we hit something solid
	if ( pOther->pev->solid == SOLID_BSP )
	{
		SetTouch( NULL );
		m_flStartTime = gpGlobals->time;
		pev->nextthink = gpGlobals->time;
		m_velocity = pev->velocity;
	}
}


void CModelTrain :: DyingThink( void )
{
	StudioFrameAdvance( );
	pev->nextthink = gpGlobals->time + 0.1f;

	pev->avelocity = pev->avelocity * 1.02f;

	// still falling?
	if ( m_flStartTime > gpGlobals->time )
	{
		UTIL_MakeAimVectors( pev->angles );

		Vector vecSpot = pev->origin + pev->velocity * 0.2f;

		// fireball
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSpot );
			WRITE_BYTE( TE_EXPLOSION );
			WRITE_COORD( vecSpot.x + RANDOM_FLOAT( -150, 150 ) );
			WRITE_COORD( vecSpot.y + RANDOM_FLOAT( -150, 150 ) );
			WRITE_COORD( vecSpot.z + RANDOM_FLOAT( -150, -50 ) );
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( RANDOM_LONG( 0, 29 ) + 30 ); // scale * 10
			WRITE_BYTE( 12 ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();

		// lots of smoke
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSpot );
			WRITE_BYTE( TE_SMOKE );
			WRITE_COORD( vecSpot.x + RANDOM_FLOAT( -150, 150 ) );
			WRITE_COORD( vecSpot.y + RANDOM_FLOAT( -150, 150 ) );
			WRITE_COORD( vecSpot.z + RANDOM_FLOAT( -150, -50 ) );
			WRITE_SHORT( g_sModelIndexSmoke );
			WRITE_BYTE( 100 ); // scale * 10
			WRITE_BYTE( 10 ); // framerate
		MESSAGE_END();

		pev->flags &= ~FL_ONGROUND;
		pev->nextthink = gpGlobals->time + 0.2f;
		return;
	}
	else
	{
		Vector vecSpot = pev->origin + (pev->mins + pev->maxs) * 0.5f;

		if ( m_iExplode )
		{
			// fireball
			MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSpot );
				WRITE_BYTE( TE_SPRITE );
				WRITE_COORD( vecSpot.x );
				WRITE_COORD( vecSpot.y );
				WRITE_COORD( vecSpot.z + 512 );
				WRITE_SHORT( m_iExplode );
				WRITE_BYTE( 250 ); // scale * 10
				WRITE_BYTE( 255 ); // brightness
			MESSAGE_END();
		}

		vecSpot = pev->origin + (pev->mins + pev->maxs) * 0.5f;

		if ( m_iBodyGibs )
		{
			MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, vecSpot );
				WRITE_BYTE( TE_BREAKMODEL );

				// position
				WRITE_COORD( vecSpot.x );
				WRITE_COORD( vecSpot.y );
				WRITE_COORD( vecSpot.z + 64 );

				// size
				WRITE_COORD( 800 );
				WRITE_COORD( 800 );
				WRITE_COORD( 128 );

				// velocity
				WRITE_COORD( m_velocity.x );
				WRITE_COORD( m_velocity.y );
				WRITE_COORD( fabsf( m_velocity.z ) * 0.25f );

				// randomization
				WRITE_BYTE( 40 );

				// Model
				WRITE_SHORT( m_iBodyGibs );	//model id#

				// # of shards
				WRITE_BYTE( 128 );

				// duration
				WRITE_BYTE( 200 );// 20.0 seconds

				// flags
				WRITE_BYTE( BREAK_METAL );
			MESSAGE_END();
		}

		UTIL_Remove( this );
	}
}


void CModelTrain :: TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType )
{
	UTIL_Sparks( ptr->vecEndPos );
}


void CModelTrain :: AdvanceSpriteFrame( float flInterval )
{
	if ( m_flMaxSpriteFrame > 0 )
		pev->frame = fmod( pev->frame + flInterval, m_flMaxSpriteFrame );
}


/* registro dos ponteiros de Think/Touch/Use/MoveDone para o save do PS2 */
typedef void (CBaseToggle::*MOVEPTR)(void);
extern "C" void func_002397D0(const char *name, BASEPTR pfn);
extern "C" void func_002397F0(const char *name, ENTITYFUNCPTR pfn);
extern "C" void func_00239810(const char *name, USEPTR pfn);
extern "C" void func_00239830(const char *name, MOVEPTR pfn);

extern "C" void func_001A0CC8(void)
{
	func_002397D0("177", static_cast<BASEPTR>(&CModelTrain::TurnThink));
	func_002397D0("178", static_cast<BASEPTR>(&CModelTrain::DyingThink));
	func_002397D0("179", static_cast<BASEPTR>(&CModelTrain::MoveThink));
	func_002397D0("180", static_cast<BASEPTR>(&CModelTrain::StartThink));
	func_002397F0("181", static_cast<ENTITYFUNCPTR>(&CModelTrain::CrashTouch));
	func_00239810("182", static_cast<USEPTR>(&CModelTrain::TrainUse));
	func_00239830("183", static_cast<MOVEPTR>(&CModelTrain::Next));
}
