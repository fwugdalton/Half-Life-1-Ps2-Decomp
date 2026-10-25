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
/* dlls/mtank.cpp: monster_tank, tanque que procura inimigos como uma torreta e dispara
   monster_tank_shell (projétil que explode ao tocar). Só existe no PS2 (não está no
   HLSDK); mistura o turret.cpp com o func_tank.cpp. O nome do arquivo e das funções
   são reconstruídos. */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/saverestore.h"
#include "dlls/effects.h"

#define EF_INVLIGHT		16
#define TE_SMOKE		5
#define TE_EXPLOSION	3

extern short g_sModelIndexFireball;
extern short g_sModelIndexWExplosion;
extern short g_sModelIndexSmoke;
extern int UTIL_PointContents( const Vector &vec );
extern void RadiusDamage( Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType );
extern void AddMultiDamage( entvars_t *pevInflictor, CBaseEntity *pEntity, float flDamage, int bitsDamageType );

// do func_tank.cpp (sem uso aqui); fica no .bss do asm em 0x3A3B88
Vector gTankSpread[5] asm("D_003A3B88") =
{
	Vector( 0, 0, 0 ),		// perfect
	Vector( 0.025, 0.025, 0.025 ),	// small cone
	Vector( 0.05, 0.05, 0.05 ),  // medium cone
	Vector( 0.1, 0.1, 0.1 ),	// large cone
	Vector( 0.25, 0.25, 0.25 ),	// extra-large cone
};

typedef enum
{
	TANK_ANIM_NONE = 0,
	TANK_ANIM_FIRE,
	TANK_ANIM_SPIN,
	TANK_ANIM_DEPLOY,
	TANK_ANIM_RETIRE,
	TANK_ANIM_DIE,
} TANK_ANIM;


class CTankShell : public CBaseEntity
{
public:
	void Spawn( void );
	void Precache( void );
	int  Classify( void );
	void EXPORT ShellTouch( CBaseEntity *pOther );

	static CTankShell *Create( void );

	int m_iTrail;	// 0x30
};

LINK_ENTITY_TO_CLASS( monster_tank_shell, CTankShell );

CTankShell *CTankShell::Create( void )
{
	CTankShell *pShell = GetClassPtr( (CTankShell *)NULL );
	pShell->pev->classname = MAKE_STRING( "monster_tank_shell" );
	pShell->Spawn();
	return pShell;
}

void CTankShell::Spawn( void )
{
	Precache();
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;
	pev->gravity = 0.5f;

	SET_MODEL( ENT( pev ), "models/tankshell.dol" );
	UTIL_SetOrigin( pev, pev->origin );
	UTIL_SetSize( pev, Vector( 0, 0, 0 ), Vector( 0, 0, 0 ) );

	SetTouch( &CTankShell::ShellTouch );
}

void CTankShell::Precache( void )
{
	PRECACHE_MODEL( "models/tankshell.dol" );
	m_iTrail = PRECACHE_MODEL( "sprites/streak.spz" );
}

int CTankShell::Classify( void )
{
	return CLASS_NONE;
}

void CTankShell::ShellTouch( CBaseEntity *pOther )
{
	int iContents = UTIL_PointContents( pev->origin );

	SetTouch( NULL );
	SetThink( NULL );

	if ( pev->dmg == 0 )
		pev->dmg = 100;

	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_EXPLOSION );
		WRITE_COORD( pev->origin.x );
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		if ( iContents != CONTENTS_WATER )
			WRITE_SHORT( g_sModelIndexFireball );
		else
			WRITE_SHORT( g_sModelIndexWExplosion );
		WRITE_BYTE( 10 );
		WRITE_BYTE( 15 );
		WRITE_BYTE( 0 );
	MESSAGE_END();

	entvars_t *pevOwner = VARS( pev->owner );
	pev->owner = NULL;

	RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, 128, CLASS_NONE, DMG_BLAST | DMG_ALWAYSGIB );
	UTIL_Remove( this );
}


class CMTank : public CBaseMonster
{
public:
	void Spawn( void );
	void Precache( void );
	void KeyValue( KeyValueData *pkvd );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	virtual void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType );
	virtual int	 TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );
	virtual int	 Classify( void );

	int BloodColor( void ) { return DONT_BLEED; }
	void GibMonster( void ) {}

	void EXPORT Initialize( void );
	void EXPORT TankUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void EXPORT ActiveThink( void );
	void EXPORT SearchThink( void );
	void EXPORT TankDeath( void );

	void TankTrace( const Vector &vecStart, const Vector &vecForward, const Vector &vecSpread, TraceResult &tr );
	void Fire( Vector &vecSrc, Vector &vecDir );
	void SetTurretAnim( TANK_ANIM anim );
	int MoveTurret( void );

	int		m_iMinPitch;		// 0x2A0
	int		m_iBaseTurnRate;	// 0x2A4
	float	m_fTurnRate;		// 0x2A8
	int		m_iOrientation;		// 0x2AC
	Vector	m_vecLastSight;		// 0x2B0
	float	m_flLastSight;		// 0x2BC
	float	m_flMaxWait;		// 0x2C0
	int		m_iSearchSpeed;		// 0x2C4
	float	m_flFireRate;		// 0x2C8
	float	m_flNextFireTime;	// 0x2CC
	float	m_flStartYaw;		// 0x2D0
	Vector	m_vecCurAngles;		// 0x2D4
	Vector	m_vecGoalAngles;	// 0x2E0
	string_t m_iszSpriteSmoke;	// 0x2EC
	string_t m_iszSpriteFlash;	// 0x2F0
	float	m_spriteScale;		// 0x2F4
	int		m_iShellDamage;		// 0x2F8
	int		m_iTankRange;		// 0x2FC
};

TYPEDESCRIPTION	CMTank::m_SaveData[] =
{
	DEFINE_FIELD( CMTank, m_iMinPitch, FIELD_INTEGER ),
	DEFINE_FIELD( CMTank, m_iBaseTurnRate, FIELD_INTEGER ),
	DEFINE_FIELD( CMTank, m_fTurnRate, FIELD_FLOAT ),
	DEFINE_FIELD( CMTank, m_iOrientation, FIELD_INTEGER ),
	DEFINE_FIELD( CMTank, m_vecLastSight, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( CMTank, m_flLastSight, FIELD_TIME ),
	DEFINE_FIELD( CMTank, m_flMaxWait, FIELD_FLOAT ),
	DEFINE_FIELD( CMTank, m_iSearchSpeed, FIELD_INTEGER ),
	DEFINE_FIELD( CMTank, m_flFireRate, FIELD_FLOAT ),
	DEFINE_FIELD( CMTank, m_flNextFireTime, FIELD_FLOAT ),
	DEFINE_FIELD( CMTank, m_flStartYaw, FIELD_FLOAT ),
	DEFINE_FIELD( CMTank, m_vecCurAngles, FIELD_VECTOR ),
	DEFINE_FIELD( CMTank, m_vecGoalAngles, FIELD_VECTOR ),
	DEFINE_FIELD( CMTank, m_iszSpriteSmoke, FIELD_STRING ),
	DEFINE_FIELD( CMTank, m_iszSpriteFlash, FIELD_STRING ),
	DEFINE_FIELD( CMTank, m_spriteScale, FIELD_FLOAT ),
	DEFINE_FIELD( CMTank, m_iShellDamage, FIELD_INTEGER ),
	DEFINE_FIELD( CMTank, m_iTankRange, FIELD_INTEGER ),
};

IMPLEMENT_SAVERESTORE( CMTank, CBaseMonster );

LINK_ENTITY_TO_CLASS( monster_tank, CMTank );

void CMTank::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "orientation" ) )
	{
		m_iOrientation = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "turnrate" ) )
	{
		m_iBaseTurnRate = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "firerate" ) )
	{
		m_flFireRate = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "spritesmoke" ) )
	{
		m_iszSpriteSmoke = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "spriteflash" ) )
	{
		m_iszSpriteFlash = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "spritescale" ) )
	{
		m_spriteScale = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "minpitch" ) )
	{
		m_iMinPitch = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "shelldamage" ) )
	{
		m_iShellDamage = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "tankrange" ) )
	{
		m_iTankRange = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseMonster::KeyValue( pkvd );
}

void CMTank::Spawn( void )
{
	Vector vecSrc, vecAng;

	Precache();
	SET_MODEL( ENT( pev ), "models/tank.dol" );
	pev->health = 1000;
	pev->nextthink = gpGlobals->time + 1;
	pev->movetype = MOVETYPE_FLY;
	pev->sequence = 0;
	pev->frame = 0;
	pev->solid = SOLID_SLIDEBOX;
	pev->takedamage = DAMAGE_AIM;

	GetAttachment( 1, vecSrc, vecAng );
	pev->view_ofs = vecSrc - pev->origin;

	pev->mins = Vector( -200, -200, 0 );
	pev->maxs = Vector( 200, 200, 108 );
	UTIL_SetSize( pev, pev->mins, pev->maxs );

	SetBits( pev->flags, FL_MONSTER );
	SetUse( &CMTank::TankUse );

	ResetSequenceInfo();
	SetBoneController( 0, 0 );
	SetBoneController( 1, 0 );
	m_flFieldOfView = VIEW_FIELD_FULL;

	SetThink( &CMTank::Initialize );
	pev->nextthink = gpGlobals->time + 0.3f;
}

void CMTank::Precache( void )
{
	PRECACHE_MODEL( "models/tank.dol" );

	if ( m_iszSpriteSmoke )
		PRECACHE_MODEL( (char *)STRING( m_iszSpriteSmoke ) );
	if ( m_iszSpriteFlash )
		PRECACHE_MODEL( (char *)STRING( m_iszSpriteFlash ) );

	UTIL_PrecacheOther( "monster_tank_shell" );
}

void CMTank::Initialize( void )
{
	SetBoneController( 0, 0 );
	SetBoneController( 1, 0 );

	if ( m_iBaseTurnRate == 0 ) m_iBaseTurnRate = 30;
	if ( m_flMaxWait == 0 ) m_flMaxWait = 15;
	if ( m_flFireRate == 0 ) m_flFireRate = 5;
	if ( m_iTankRange == 0 ) m_iTankRange = 1200;

	m_flStartYaw = pev->angles.y;
	if ( m_iOrientation == 1 )
	{
		pev->idealpitch = 0;
		pev->angles.x = 0;
		pev->effects |= EF_INVLIGHT;
		pev->angles.y = pev->angles.y + 180;
		if ( pev->angles.y >= 360 )
			pev->angles.y = pev->angles.y - 360;
	}

	m_vecGoalAngles.x = 0;

	m_flLastSight = gpGlobals->time + m_flMaxWait;
	SetThink( &CMTank::SearchThink );
	pev->nextthink = gpGlobals->time + 0.1f;
}

void CMTank::TankUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
}

void CMTank::ActiveThink( void )
{
	int fAttack = 0;
	Vector vecDirToEnemy;

	pev->nextthink = gpGlobals->time + 0.1f;
	StudioFrameAdvance();

	if ( m_hEnemy == NULL )
	{
		m_flLastSight = gpGlobals->time + m_flMaxWait;
		SetThink( &CMTank::SearchThink );
		return;
	}

	// if it's dead, look for something new
	if ( !m_hEnemy->IsAlive() )
	{
		if ( !m_flLastSight )
		{
			m_flLastSight = gpGlobals->time + 0.5f; // continue-shooting timeout
		}
		else
		{
			if ( gpGlobals->time > m_flLastSight )
			{
				m_hEnemy = NULL;
				m_flLastSight = gpGlobals->time + m_flMaxWait;
				SetThink( &CMTank::SearchThink );
				return;
			}
		}
	}

	Vector vecMid = pev->origin + pev->view_ofs;
	Vector vecMidEnemy = m_hEnemy->BodyTarget( vecMid );
	vecMidEnemy = m_hEnemy->Center() + m_hEnemy->pev->view_ofs;

	// Look for our current enemy
	int fEnemyVisible = FVisible( vecMidEnemy );

	vecDirToEnemy = vecMidEnemy - vecMid;	// calculate dir and dist to enemy
	float flDistToEnemy = vecDirToEnemy.Length();

	Vector vec = UTIL_VecToAngles( vecMidEnemy - vecMid );

	// Current enmey is not visible.
	if ( !fEnemyVisible || ( flDistToEnemy > m_iTankRange ) )
	{
		if ( !m_flLastSight )
			m_flLastSight = gpGlobals->time + 0.5f;
		else
		{
			// Should we look for a new target?
			if ( gpGlobals->time > m_flLastSight )
			{
				m_hEnemy = NULL;
				m_flLastSight = gpGlobals->time + m_flMaxWait;
				SetThink( &CMTank::SearchThink );
				return;
			}
		}
		fEnemyVisible = 0;
	}
	else
	{
		m_vecLastSight = vecMidEnemy;
	}

	UTIL_MakeAimVectors( m_vecCurAngles );

	Vector vecLOS = vecDirToEnemy;
	vecLOS = vecLOS.Normalize();

	// Is the Gun looking at the target
	if ( DotProduct( vecLOS, gpGlobals->v_forward ) <= 0.866f ) // 30 degree slop
		fAttack = FALSE;
	else
		fAttack = TRUE;

	if ( gpGlobals->time < m_flNextFireTime )
		fAttack = FALSE;

	// fire the gun
	if ( fAttack )
	{
		Vector vecSrc, vecAng;
		GetAttachment( 0, vecSrc, vecAng );
		SetTurretAnim( TANK_ANIM_FIRE );
		Fire( vecSrc, gpGlobals->v_forward );
		m_flNextFireTime = gpGlobals->time + m_flFireRate;
	}
	else
	{
		SetTurretAnim( TANK_ANIM_SPIN );
	}

	//move the gun
	if ( fEnemyVisible )
	{
		if ( vec.y > 360 )
			vec.y -= 360;

		if ( vec.y < 0 )
			vec.y += 360;

		if ( vec.x < -180 )
			vec.x += 360;

		if ( vec.x > 180 )
			vec.x -= 360;

		if ( m_iOrientation == 0 )
		{
			if ( vec.x > 90 )
				vec.x = 90;
			else if ( vec.x < m_iMinPitch )
				vec.x = m_iMinPitch;
		}
		else
		{
			if ( vec.x < -90 )
				vec.x = -90;
			else if ( vec.x > -m_iMinPitch )
				vec.x = -m_iMinPitch;
		}

		m_vecGoalAngles.y = vec.y;
		m_vecGoalAngles.x = vec.x;
	}

	MoveTurret();
}

void CMTank::TankTrace( const Vector &vecStart, const Vector &vecForward, const Vector &vecSpread, TraceResult &tr )
{
	// get circular gaussian spread
	float x, y, z;
	do {
		x = RANDOM_FLOAT( -0.5f, 0.5f ) + RANDOM_FLOAT( -0.5f, 0.5f );
		y = RANDOM_FLOAT( -0.5f, 0.5f ) + RANDOM_FLOAT( -0.5f, 0.5f );
		z = x * x + y * y;
	} while ( z > 1 );
	Vector vecDir = vecForward +
		x * vecSpread.x * gpGlobals->v_right +
		y * vecSpread.y * gpGlobals->v_up;
	Vector vecEnd;

	vecEnd = vecStart + vecDir * 4096;
	UTIL_TraceLine( vecStart, vecEnd, dont_ignore_monsters, edict(), &tr );
}

void CMTank::Fire( Vector &vecSrc, Vector &vecDir )
{
	if ( m_iszSpriteSmoke )
	{
		CSprite *pSprite = CSprite::SpriteCreate( STRING( m_iszSpriteSmoke ), vecSrc, TRUE );
		pSprite->AnimateAndDie( RANDOM_FLOAT( 15, 20 ) );
		pSprite->SetTransparency( kRenderTransAlpha, pev->rendercolor.x, pev->rendercolor.y, pev->rendercolor.z, 255, kRenderFxNone );
		pSprite->pev->velocity.z = RANDOM_FLOAT( 40, 80 );
		pSprite->SetScale( m_spriteScale );
	}
	if ( m_iszSpriteFlash )
	{
		CSprite *pSprite = CSprite::SpriteCreate( STRING( m_iszSpriteFlash ), vecSrc, TRUE );
		pSprite->AnimateAndDie( 60 );
		pSprite->SetTransparency( kRenderTransAdd, 255, 255, 255, 255, kRenderFxNoDissipation );
		pSprite->SetScale( m_spriteScale );
	}

	CTankShell *pShell = CTankShell::Create();
	pShell->pev->origin = vecSrc;
	pShell->pev->angles = UTIL_VecToAngles( vecDir );
	pShell->pev->angles.x -= 90;
	if ( pShell->pev->angles.x < 0 )
		pShell->pev->angles.x += 360;
	pShell->pev->owner = edict();
	pShell->pev->velocity = vecDir * 900;
	pShell->pev->speed = 900;
	pShell->pev->avelocity.z = 10;
	pShell->pev->dmg = m_iShellDamage;
}

void CMTank::SetTurretAnim( TANK_ANIM anim )
{
	if ( pev->sequence != anim )
	{
		switch ( anim )
		{
		case TANK_ANIM_FIRE:
		case TANK_ANIM_SPIN:
			if ( pev->sequence != TANK_ANIM_FIRE && pev->sequence != TANK_ANIM_SPIN )
			{
				pev->frame = 0;
			}
			break;
		default:
			pev->frame = 0;
			break;
		}

		pev->sequence = anim;
		ResetSequenceInfo();

		switch ( anim )
		{
		case TANK_ANIM_RETIRE:
			pev->frame = 255;
			pev->framerate = -1.0f;
			break;
		case TANK_ANIM_DIE:
			pev->framerate = 1.0f;
			break;
		}
	}
}

void CMTank::SearchThink( void )
{
	StudioFrameAdvance();
	pev->nextthink = gpGlobals->time + 0.3f;

	// If we have a target and we're still healthy
	if ( m_hEnemy != NULL )
	{
		if ( !m_hEnemy->IsAlive() )
			m_hEnemy = NULL;// Dead enemy forces a search for new one
	}

	// Acquire Target
	if ( m_hEnemy == NULL )
	{
		Look( m_iTankRange );
		m_hEnemy = BestVisibleEnemy();
	}

	if ( m_hEnemy != NULL )
	{
		SetThink( &CMTank::ActiveThink );
	}
}

void CMTank::TankDeath( void )
{
	StudioFrameAdvance();
	pev->nextthink = gpGlobals->time + 0.1f;

	if ( pev->deadflag != DEAD_DEAD )
	{
		SetTurretAnim( TANK_ANIM_DIE );
	}

	if ( pev->dmgtime + RANDOM_FLOAT( 0, 2 ) > gpGlobals->time )
	{
		// lots of smoke
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
			WRITE_BYTE( TE_SMOKE );
			WRITE_COORD( RANDOM_FLOAT( pev->absmin.x, pev->absmax.x ) );
			WRITE_COORD( RANDOM_FLOAT( pev->absmin.y, pev->absmax.y ) );
			WRITE_COORD( pev->origin.z - m_iOrientation * 64 );
			WRITE_SHORT( g_sModelIndexSmoke );
			WRITE_BYTE( 25 ); // scale * 10
			WRITE_BYTE( 10 - m_iOrientation * 5 ); // framerate
		MESSAGE_END();
	}

	if ( pev->dmgtime + RANDOM_FLOAT( 0, 5 ) > gpGlobals->time )
	{
		Vector vecSrc = Vector( RANDOM_FLOAT( pev->absmin.x, pev->absmax.x ), RANDOM_FLOAT( pev->absmin.y, pev->absmax.y ), 0 );
		if ( m_iOrientation == 0 )
			vecSrc = vecSrc + Vector( 0, 0, RANDOM_FLOAT( pev->origin.z, pev->absmax.z ) );
		else
			vecSrc = vecSrc + Vector( 0, 0, RANDOM_FLOAT( pev->absmin.z, pev->origin.z ) );

		UTIL_Sparks( vecSrc );
	}

	if ( m_fSequenceFinished && !MoveTurret() && pev->dmgtime + 5 < gpGlobals->time )
	{
		pev->framerate = 0;
		SetThink( NULL );
	}
}

void CMTank::TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType )
{
	if ( bitsDamageType & ( DMG_BULLET | DMG_SLASH | DMG_CLUB ) )
	{
		if ( !( bitsDamageType & DMG_CLUB ) )
		{
			if ( pev->dmgtime != gpGlobals->time || ( RANDOM_LONG( 0, 10 ) < 1 ) )
			{
				UTIL_Ricochet( ptr->vecEndPos, RANDOM_FLOAT( 1, 2 ) );
				pev->dmgtime = gpGlobals->time;
			}
		}

		flDamage = 0.1f;
	}

	if ( !pev->takedamage )
		return;

	AddMultiDamage( pevAttacker, this, flDamage, bitsDamageType );
}

int CMTank::TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	if ( !pev->takedamage )
		return 0;

	pev->health -= flDamage;
	if ( pev->health <= 0 )
	{
		pev->health = 0;
		pev->takedamage = DAMAGE_NO;
		pev->dmgtime = gpGlobals->time;

		ClearBits( pev->flags, FL_MONSTER );

		SetUse( NULL );
		SetThink( &CMTank::TankDeath );
		SUB_UseTargets( this, USE_ON, 0 );
		pev->nextthink = gpGlobals->time + 0.1f;

		return 0;
	}

	return 1;
}

int CMTank::MoveTurret( void )
{
	int state = 0;
	// any x movement?

	if ( m_vecCurAngles.x != m_vecGoalAngles.x )
	{
		float flDir = m_vecGoalAngles.x > m_vecCurAngles.x ? 1 : -1;

		m_vecCurAngles.x += 0.1f * m_fTurnRate * flDir;

		// if we started below the goal, and now we're past, peg to goal
		if ( flDir == 1 )
		{
			if ( m_vecCurAngles.x > m_vecGoalAngles.x )
				m_vecCurAngles.x = m_vecGoalAngles.x;
		}
		else
		{
			if ( m_vecCurAngles.x < m_vecGoalAngles.x )
				m_vecCurAngles.x = m_vecGoalAngles.x;
		}

		if ( m_iOrientation == 0 )
			SetBoneController( 0, -m_vecCurAngles.x );
		else
			SetBoneController( 0, m_vecCurAngles.x );
		state = 1;
	}

	if ( m_vecCurAngles.y != m_vecGoalAngles.y )
	{
		float flDir = m_vecGoalAngles.y > m_vecCurAngles.y ? 1 : -1;
		float flDist = __builtin_fabsf( m_vecGoalAngles.y - m_vecCurAngles.y );

		if ( flDist > 180 )
		{
			flDist = 360 - flDist;
			flDir = -flDir;
		}
		if ( flDist > 30 )
		{
			if ( m_fTurnRate < m_iBaseTurnRate * 10 )
			{
				m_fTurnRate += m_iBaseTurnRate;
			}
		}
		else if ( m_fTurnRate > 45 )
		{
			m_fTurnRate -= m_iBaseTurnRate;
		}
		else
		{
			m_fTurnRate += m_iBaseTurnRate;
		}

		m_vecCurAngles.y += 0.1f * m_fTurnRate * flDir;

		if ( m_vecCurAngles.y < 0 )
			m_vecCurAngles.y += 360;
		else if ( m_vecCurAngles.y >= 360 )
			m_vecCurAngles.y -= 360;

		if ( flDist < ( 0.05f * m_iBaseTurnRate ) )
			m_vecCurAngles.y = m_vecGoalAngles.y;

		if ( m_iOrientation == 0 )
			SetBoneController( 1, m_vecCurAngles.y - pev->angles.y );
		else
			SetBoneController( 1, pev->angles.y - 180 - m_vecCurAngles.y );
		state = 1;
	}

	if ( !state )
		m_fTurnRate = m_iBaseTurnRate;

	return state;
}

int CMTank::Classify( void )
{
	return CLASS_MACHINE;
}


/* registro dos ponteiros de Think/Touch/Use para o save do PS2 */
extern "C" void func_002397D0(const char *name, BASEPTR pfn);
extern "C" void func_002397F0(const char *name, ENTITYFUNCPTR pfn);
extern "C" void func_00239810(const char *name, USEPTR pfn);

extern "C" void func_0019F2F0(void)
{
	func_002397D0("171", static_cast<BASEPTR>(&CMTank::SearchThink));
	func_002397D0("172", static_cast<BASEPTR>(&CMTank::ActiveThink));
	func_002397D0("173", static_cast<BASEPTR>(&CMTank::TankDeath));
	func_002397D0("174", static_cast<BASEPTR>(&CMTank::Initialize));
	func_002397F0("175", static_cast<ENTITYFUNCPTR>(&CTankShell::ShellTouch));
	func_00239810("176", static_cast<USEPTR>(&CMTank::TankUse));
}
