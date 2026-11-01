/* dlls/alienflyer.cpp: a nave alienígena do Half-Life: Decay (monster_alienflyer).
   Não existe no HLSDK: é uma cópia do osprey.cpp com trilhas ("aftrack_N"), ataque de
   raio contra o chão e alien grunts. Nomes de arquivo, métodos e campos são inventados;
   a ordem saiu do .text, os tipos do asm e os nomes de campo do m_SaveData da ROM. */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/weapons.h"
#include "dlls/soundent.h"
#include "dlls/effects.h"
#include "dlls/skill.h"
#include "include_asm.h"

#define TE_EXPLOSION		3
#define TE_SMOKE			5
#define TE_SPRITE			17
#define TE_BEAMCYLINDER		21
#define TE_BREAKMODEL		108
#define TE_EXPLFLAG_NONE	0
#define BREAK_FLESH			0x04

extern float UTIL_SplineFraction( float value, float scale );
extern void FireTargets( const char *targetName, CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
extern "C" float fabsf( float );
extern int DispatchSpawn( edict_t *pent );

#define SF_WAITFORTRIGGER	0x40
#define MAX_FLYER_GRUNTS	3
#define MAX_FLYER_TRACKS	10

class CAlienFlyer : public CBaseMonster
{
public:
	int		Save( CSave &save );
	int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];
	int		ObjectCaps( void ) { return CBaseMonster :: ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

	void KeyValue( KeyValueData *pkvd );
	void Spawn( void );
	void Precache( void );
	int  Classify( void ) { return CLASS_ALIEN_MONSTER; };
	int  BloodColor( void ) { return BLOOD_COLOR_YELLOW; }
	void Killed( entvars_t *pevAttacker, int iGib );

	void UpdateGoal( BOOL fFirst );
	void EXPORT FlyThink( void );
	void EXPORT AttackThink( void );
	void Flight( void );
	void EXPORT HitTouch( CBaseEntity *pOther );
	CBaseMonster *MakeGrunt( Vector vecSrc );
	void EXPORT CrashTouch( CBaseEntity *pOther );
	void EXPORT DyingThink( void );
	void EXPORT CommandUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	int  TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType );
	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
	void ShowDamage( void );

	CBaseEntity *m_pGoalEnt;
	Vector m_vel1;
	Vector m_vel2;
	Vector m_pos1;
	Vector m_pos2;
	Vector m_ang1;
	Vector m_ang2;
	float m_startTime;
	float m_dTime;

	Vector m_velocity;

	float m_flIdealtilt;
	float m_flRotortilt;

	float m_flRightHealth;
	float m_flLeftHealth;

	int m_iSoundState;
	int m_iSpriteTexture;
	string_t m_iszDeathTarget;
	int m_iPitch;
	int m_iExplode;
	int m_iBodyGibs;
	int m_iShockwave;

	int m_iDoLeftSmokePuff;
	int m_iDoRightSmokePuff;

	int m_iUnits;
	EHANDLE m_hGrunt[MAX_FLYER_GRUNTS];

	int m_iNumTracks;
	int m_iCurTrack;
	string_t m_iszTrack[MAX_FLYER_TRACKS];
	BOOL m_fChangedTrack;
	BOOL m_fStopAttack;
};

LINK_ENTITY_TO_CLASS( monster_alienflyer, CAlienFlyer );

TYPEDESCRIPTION	CAlienFlyer::m_SaveData[] =
{
	DEFINE_FIELD( CAlienFlyer, m_pGoalEnt, FIELD_CLASSPTR ),
	DEFINE_FIELD( CAlienFlyer, m_vel1, FIELD_VECTOR ),
	DEFINE_FIELD( CAlienFlyer, m_vel2, FIELD_VECTOR ),
	DEFINE_FIELD( CAlienFlyer, m_pos1, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( CAlienFlyer, m_pos2, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( CAlienFlyer, m_ang1, FIELD_VECTOR ),
	DEFINE_FIELD( CAlienFlyer, m_ang2, FIELD_VECTOR ),

	DEFINE_FIELD( CAlienFlyer, m_startTime, FIELD_TIME ),
	DEFINE_FIELD( CAlienFlyer, m_dTime, FIELD_FLOAT ),
	DEFINE_FIELD( CAlienFlyer, m_velocity, FIELD_VECTOR ),

	DEFINE_FIELD( CAlienFlyer, m_flIdealtilt, FIELD_FLOAT ),
	DEFINE_FIELD( CAlienFlyer, m_flRotortilt, FIELD_FLOAT ),

	DEFINE_FIELD( CAlienFlyer, m_flRightHealth, FIELD_FLOAT ),
	DEFINE_FIELD( CAlienFlyer, m_flLeftHealth, FIELD_FLOAT ),

	DEFINE_FIELD( CAlienFlyer, m_iDoLeftSmokePuff, FIELD_INTEGER ),
	DEFINE_FIELD( CAlienFlyer, m_iDoRightSmokePuff, FIELD_INTEGER ),
	DEFINE_FIELD( CAlienFlyer, m_iszDeathTarget, FIELD_STRING ),
};
IMPLEMENT_SAVERESTORE( CAlienFlyer, CBaseMonster );


void CAlienFlyer :: KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "death_target"))
	{
		if (pkvd->szValue && *pkvd->szValue)
			m_iszDeathTarget = ALLOC_STRING(pkvd->szValue);
		else
			m_iszDeathTarget = 0;
		pkvd->fHandled = TRUE;
	}
	else
	{
		CBaseMonster::KeyValue( pkvd );
		if (!pkvd->fHandled && !strncmp(pkvd->szKeyName, "aftrack_", 8))
		{
			m_iszTrack[pkvd->szKeyName[8] - '0'] = ALLOC_STRING(pkvd->szValue);
			if (m_iNumTracks < pkvd->szKeyName[8] - '0' + 1)
				m_iNumTracks = pkvd->szKeyName[8] - '0' + 1;
		}
	}
}


void CAlienFlyer :: Spawn( void )
{
	Precache( );
	// motor
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/flyer.dol");
	UTIL_SetSize(pev, Vector( -400, -400, -100), Vector(400, 400, 32));
	UTIL_SetOrigin( pev, pev->origin );

	pev->flags |= FL_MONSTER;
	pev->takedamage		= DAMAGE_AIM;
	m_flRightHealth		= 200;
	m_flLeftHealth		= 200;
	pev->health			= gSkillData.flyerHealth;
	pev->max_health		= gSkillData.flyerHealth;

	m_flFieldOfView = 0; // 180 degrees

	pev->sequence = 0;
	ResetSequenceInfo( );
	pev->frame = RANDOM_LONG(0,0xFF);

	InitBoneControllers();

	for (int i = 0; i < MAX_FLYER_GRUNTS; i++)
		m_hGrunt[i] = NULL;

	SetThink( &CAlienFlyer::FlyThink );
	m_startTime = gpGlobals->time;
	SetUse( &CAlienFlyer::CommandUse );

	if (!(pev->spawnflags & SF_WAITFORTRIGGER))
	{
		pev->nextthink = gpGlobals->time + 1.0f;
	}
	else
	{
		pev->effects |= EF_NODRAW;
		pev->solid = SOLID_NOT;
	}

	m_pos2 = pev->origin;
	m_ang2 = pev->angles;
	m_vel2 = pev->velocity;
}


void CAlienFlyer::Precache( void )
{
	UTIL_PrecacheOther( "monster_alien_grunt" );

	PRECACHE_MODEL("models/flyer.dol");
	PRECACHE_MODEL("models/HVR.dol");

	PRECACHE_SOUND("ambience/mechwhine.wav");
	PRECACHE_SOUND("weapons/mortarhit.wav");
	PRECACHE_SOUND("debris/beamstart1.wav");

	m_iSpriteTexture = PRECACHE_MODEL( "sprites/xenobeam.spz" );
	m_iSpriteTexture = PRECACHE_MODEL( "sprites/laserbeam.spz" );

	m_iExplode	= PRECACHE_MODEL( "sprites/fexplo.spz" );
	m_iBodyGibs = PRECACHE_MODEL( "models/flyer_gibs.dol" );
	m_iShockwave = PRECACHE_MODEL( "sprites/shockwave.spz" );
}

void CAlienFlyer::CommandUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	pev->nextthink = gpGlobals->time + 0.1f;
	pev->solid = SOLID_BBOX;
	pev->effects &= ~EF_NODRAW;
}


void CAlienFlyer :: AttackThink( void )
{
	if (!m_fStopAttack)
	{
	TraceResult tr;
	Vector vecSrc;

	UTIL_MakeVectors( pev->angles );

	vecSrc = pev->origin + gpGlobals->v_forward * 64;

	CSoundEnt::InsertSound ( bits_SOUND_DANGER, tr.vecEndPos, 400, 0.3f );
	EMIT_SOUND_DYN( ENT(pev), CHAN_WEAPON, "debris/beamstart1.wav", 1.0f, 0.2f, 0, 100 );
	UTIL_TraceLine( vecSrc, vecSrc + Vector( 0, 0, -8192 ), ignore_monsters, ENT(pev), &tr );

	CBeam *pBeam = CBeam::BeamCreate( "sprites/xenobeam.spz", 255 );
	pBeam->PointsInit( vecSrc, tr.vecEndPos );
	pBeam->SetColor( 254, 244, 233 );
	pBeam->SetNoise( 3 );
	pBeam->SetScrollRate( 40 );
	pBeam->SetBrightness( 150 );
	pBeam->SetThink( &CBeam::SUB_Remove );
	pBeam->pev->nextthink = gpGlobals->time + 1.0f;

	pBeam = CBeam::BeamCreate( "sprites/laserbeam.spz", 30 );
	pBeam->PointsInit( vecSrc, tr.vecEndPos );
	pBeam->SetColor( 255, 255, 255 );
	pBeam->SetNoise( 100 );
	pBeam->SetScrollRate( 35 );
	pBeam->SetBrightness( 125 );
	pBeam->SetThink( &CBeam::SUB_Remove );
	pBeam->pev->nextthink = gpGlobals->time + 1.0f;

	MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, tr.vecEndPos );
		WRITE_BYTE( TE_BEAMCYLINDER );
		WRITE_COORD( tr.vecEndPos.x );
		WRITE_COORD( tr.vecEndPos.y );
		WRITE_COORD( tr.vecEndPos.z + 16 );
		WRITE_COORD( tr.vecEndPos.x );
		WRITE_COORD( tr.vecEndPos.y );
		WRITE_COORD( tr.vecEndPos.z + 16 + 1920 );
		WRITE_SHORT( m_iShockwave );
		WRITE_BYTE( 0 ); // startframe
		WRITE_BYTE( 0 ); // framerate
		WRITE_BYTE( 4 ); // life
		WRITE_BYTE( 16 );  // width
		WRITE_BYTE( 0 );   // noise
		WRITE_BYTE( 254 );   // r, g, b
		WRITE_BYTE( 244 );   // r, g, b
		WRITE_BYTE( 233 );   // r, g, b
		WRITE_BYTE( 255 ); // brightness
		WRITE_BYTE( 0 );		// speed
	MESSAGE_END();

	MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, tr.vecEndPos );
		WRITE_BYTE( TE_BEAMCYLINDER );
		WRITE_COORD( tr.vecEndPos.x );
		WRITE_COORD( tr.vecEndPos.y );
		WRITE_COORD( tr.vecEndPos.z + 16 );
		WRITE_COORD( tr.vecEndPos.x );
		WRITE_COORD( tr.vecEndPos.y );
		WRITE_COORD( tr.vecEndPos.z + 16 + 960 );
		WRITE_SHORT( m_iShockwave );
		WRITE_BYTE( 0 ); // startframe
		WRITE_BYTE( 0 ); // framerate
		WRITE_BYTE( 4 ); // life
		WRITE_BYTE( 16 );  // width
		WRITE_BYTE( 0 );   // noise
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 255 ); // brightness
		WRITE_BYTE( 0 );		// speed
	MESSAGE_END();

	SetThink( &CAlienFlyer::FlyThink );
	pev->nextthink = gpGlobals->time + 1.0f;
	m_startTime += 1.0f;

	RadiusDamage( tr.vecEndPos, pev, pev, gSkillData.flyerDamage, CLASS_NONE, DMG_SHOCK );

	CBaseEntity *pEmitter = UTIL_FindEntityByClassname( NULL, "item_focusemitter" );
	if (pEmitter)
		pEmitter->TakeDamage( pev, pev, 1.0f, 0 );
	}
	else
	{
		m_fStopAttack = FALSE;
		SetThink( &CAlienFlyer::FlyThink );
		pev->nextthink = gpGlobals->time + 0.1f;
	}
}


void CAlienFlyer::UpdateGoal( BOOL fFirst )
{
	if (m_pGoalEnt)
	{
		if (fFirst)
		{
			m_pos1 = pev->origin;
			m_ang1 = pev->angles;
			m_vel1 = m_velocity;
		}
		else
		{
			m_pos1 = m_pos2;
			m_ang1 = m_ang2;
			m_vel1 = m_vel2;
		}
		m_pos2 = m_pGoalEnt->pev->origin;
		m_ang2 = m_pGoalEnt->pev->angles;
		UTIL_MakeAimVectors( Vector( 0, m_ang2.y, 0 ) );
		m_vel2 = gpGlobals->v_forward * m_pGoalEnt->pev->speed;

		if (fFirst)
			m_startTime = gpGlobals->time;
		else
			m_startTime = m_startTime + m_dTime;
		m_dTime = 2.0f * (m_pos1 - m_pos2).Length() / (m_vel1.Length() + m_pGoalEnt->pev->speed);

		if (m_ang1.y - m_ang2.y < -180)
		{
			m_ang1.y += 360;
		}
		else if (m_ang1.y - m_ang2.y > 180)
		{
			m_ang1.y -= 360;
		}

		if (m_pGoalEnt->pev->speed < 400)
			m_flIdealtilt = 0;
		else
			m_flIdealtilt = -90;
	}
	else
	{
		ALERT( at_console, "alienflyer missing target");
	}
}


void CAlienFlyer::FlyThink( void )
{
	StudioFrameAdvance( );
	pev->nextthink = gpGlobals->time + 0.1f;

	if ( m_pGoalEnt == NULL && !FStringNull(pev->target) )// this monster has a target
	{
		m_pGoalEnt = CBaseEntity::Instance( FIND_ENTITY_BY_TARGETNAME ( NULL, m_iNumTracks ? STRING( m_iszTrack[m_iCurTrack] ) : STRING( pev->target ) ) );
		ALERT( at_console, "Alien Flyer Goal Ent: %s\n", STRING( pev->target ) );
		UpdateGoal( TRUE );
	}

	if (gpGlobals->time > m_startTime + m_dTime)
	{
		if (m_pGoalEnt == NULL || m_pGoalEnt->pev->speed == 0)
		{
			SetThink( &CAlienFlyer::AttackThink );
			if (m_fChangedTrack)
				m_fStopAttack = TRUE;
		}

		if (m_pGoalEnt)
		{
			if (m_pGoalEnt->pev->message)
				FireTargets( STRING( m_pGoalEnt->pev->message ), this, this, USE_TOGGLE, 0 );
			m_pGoalEnt = CBaseEntity::Instance( FIND_ENTITY_BY_TARGETNAME ( NULL, STRING( m_pGoalEnt->pev->target ) ) );
			ALERT( at_console, "Alien Flyer Goal Ent: %s\n", STRING( m_pGoalEnt->pev->targetname ) );
		}
		m_fChangedTrack = FALSE;
		UpdateGoal( FALSE );
	}

	Flight( );
	ShowDamage( );
}


void CAlienFlyer::Flight( )
{
	float t = (gpGlobals->time - m_startTime);
	float scale = 1.0f / m_dTime;

	float f = UTIL_SplineFraction( t * scale, 1.0f );

	Vector pos = (m_pos1 + m_vel1 * t) * (1.0f - f) + (m_pos2 - m_vel2 * (m_dTime - t)) * f;
	Vector ang = (m_ang1) * (1.0f - f) + (m_ang2) * f;
	m_velocity = m_vel1 * (1.0f - f) + m_vel2 * f;

	UTIL_SetOrigin( pev, pos );
	pev->angles = ang;
	UTIL_MakeAimVectors( pev->angles );
}


void CAlienFlyer::HitTouch( CBaseEntity *pOther )
{
	pev->nextthink = gpGlobals->time + 2.0f;
}


int CAlienFlyer::TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	if (pevInflictor->owner == edict())
		return 0;

	if (bitsDamageType & DMG_BLAST)
		flDamage *= 2;

	return CBaseEntity::TakeDamage( pevInflictor, pevAttacker, flDamage, bitsDamageType );
}


void CAlienFlyer :: Killed( entvars_t *pevAttacker, int iGib )
{
	pev->movetype = MOVETYPE_TOSS;
	pev->gravity = 0.3f;
	pev->velocity = m_velocity;
	pev->avelocity = Vector( RANDOM_FLOAT( -20, 20 ), 0, RANDOM_FLOAT( -50, 50 ) );
	STOP_SOUND( ENT(pev), CHAN_STATIC, "ambience/mechwhine.wav" );

	UTIL_SetSize( pev, Vector( -32, -32, -64), Vector( 32, 32, 0) );
	SetThink( &CAlienFlyer::DyingThink );
	SetTouch( &CAlienFlyer::CrashTouch );
	pev->nextthink = gpGlobals->time + 0.1f;
	pev->health = 0;
	pev->takedamage = DAMAGE_NO;

	m_startTime = gpGlobals->time + 4.0f;
}

void CAlienFlyer::CrashTouch( CBaseEntity *pOther )
{
	// only crash if we hit something solid
	if ( pOther->pev->solid == SOLID_BSP)
	{
		SetTouch( NULL );
		m_startTime = gpGlobals->time;
		pev->nextthink = gpGlobals->time;
		m_velocity = pev->velocity;
	}
}


void CAlienFlyer :: DyingThink( void )
{
	StudioFrameAdvance( );
	pev->nextthink = gpGlobals->time + 0.1f;

	pev->avelocity = pev->avelocity * 1.02f;

	// still falling?
	if (m_startTime > gpGlobals->time )
	{
		UTIL_MakeAimVectors( pev->angles );
		ShowDamage( );

		Vector vecSpot = pev->origin + pev->velocity * 0.2f;

		// random explosions
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSpot );
			WRITE_BYTE( TE_EXPLOSION);		// This just makes a dynamic light now
			WRITE_COORD( vecSpot.x + RANDOM_FLOAT( -150, 150 ));
			WRITE_COORD( vecSpot.y + RANDOM_FLOAT( -150, 150 ));
			WRITE_COORD( vecSpot.z + RANDOM_FLOAT( -150, -50 ));
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( RANDOM_LONG(0,29) + 30  ); // scale * 10
			WRITE_BYTE( 12  ); // framerate
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();

		// lots of smoke
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSpot );
			WRITE_BYTE( TE_SMOKE );
			WRITE_COORD( vecSpot.x + RANDOM_FLOAT( -150, 150 ));
			WRITE_COORD( vecSpot.y + RANDOM_FLOAT( -150, 150 ));
			WRITE_COORD( vecSpot.z + RANDOM_FLOAT( -150, -50 ));
			WRITE_SHORT( g_sModelIndexSmoke );
			WRITE_BYTE( 100 ); // scale * 10
			WRITE_BYTE( 10  ); // framerate
		MESSAGE_END();


		vecSpot = pev->origin + (pev->mins + pev->maxs) * 0.5f;
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSpot );
			WRITE_BYTE( TE_BREAKMODEL);

			// position
			WRITE_COORD( vecSpot.x );
			WRITE_COORD( vecSpot.y );
			WRITE_COORD( vecSpot.z );

			// size
			WRITE_COORD( 800 );
			WRITE_COORD( 800 );
			WRITE_COORD( 132 );

			// velocity
			WRITE_COORD( pev->velocity.x );
			WRITE_COORD( pev->velocity.y );
			WRITE_COORD( pev->velocity.z );

			// randomization
			WRITE_BYTE( 50 );

			// Model
			WRITE_SHORT( m_iBodyGibs );	//model id#

			// # of shards
			WRITE_BYTE( 1 );	// let client decide

			// duration
			WRITE_BYTE( 200 );// 10.0f seconds

			// flags

			WRITE_BYTE( BREAK_FLESH );
		MESSAGE_END();



		// don't stop it we touch a entity
		pev->flags &= ~FL_ONGROUND;
		pev->nextthink = gpGlobals->time + 0.2f;
		return;
	}
	else
	{
		Vector vecSpot = pev->origin + (pev->mins + pev->maxs) * 0.5f;

		// gibs
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSpot );
			WRITE_BYTE( TE_SPRITE );
			WRITE_COORD( vecSpot.x );
			WRITE_COORD( vecSpot.y );
			WRITE_COORD( vecSpot.z + 512 );
			WRITE_SHORT( m_iExplode );
			WRITE_BYTE( 250 ); // scale * 10
			WRITE_BYTE( 255 ); // brightness
		MESSAGE_END();

		// blast circle
		MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_BEAMCYLINDER );
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z);
			WRITE_COORD( pev->origin.x);
			WRITE_COORD( pev->origin.y);
			WRITE_COORD( pev->origin.z + 2000 ); // reach damage radius over .2 seconds
			WRITE_SHORT( m_iSpriteTexture );
			WRITE_BYTE( 0 ); // startframe
			WRITE_BYTE( 0 ); // framerate
			WRITE_BYTE( 4 ); // life
			WRITE_BYTE( 32 );  // width
			WRITE_BYTE( 0 );   // noise
			WRITE_BYTE( 255 );   // r, g, b
			WRITE_BYTE( 255 );   // r, g, b
			WRITE_BYTE( 192 );   // r, g, b
			WRITE_BYTE( 128 ); // brightness
			WRITE_BYTE( 0 );		// speed
		MESSAGE_END();

		EMIT_SOUND(ENT(pev), CHAN_STATIC, "weapons/mortarhit.wav", 1.0f, 0.3f);

		RadiusDamage( pev->origin, pev, pev, 300, CLASS_NONE, DMG_BLAST );

		// gibs
		vecSpot = pev->origin + (pev->mins + pev->maxs) * 0.5f;
		MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, vecSpot );
			WRITE_BYTE( TE_BREAKMODEL);

			// position
			WRITE_COORD( vecSpot.x );
			WRITE_COORD( vecSpot.y );
			WRITE_COORD( vecSpot.z + 64);

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
			WRITE_BYTE( 15 );

			// duration
			WRITE_BYTE( 200 );// 10.0f seconds

			// flags

			WRITE_BYTE( BREAK_FLESH );
		MESSAGE_END();

		if (m_iszDeathTarget)
			FireTargets( STRING( m_iszDeathTarget ), this, this, USE_TOGGLE, 0 );

		UTIL_Remove( this );
	}
}


void CAlienFlyer :: ShowDamage( void )
{
	if (m_iDoLeftSmokePuff > 0 || RANDOM_LONG(0,99) > m_flLeftHealth)
	{
		Vector vecSrc = pev->origin + gpGlobals->v_right * -340;
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSrc );
			WRITE_BYTE( TE_SMOKE );
			WRITE_COORD( vecSrc.x );
			WRITE_COORD( vecSrc.y );
			WRITE_COORD( vecSrc.z );
			WRITE_SHORT( g_sModelIndexSmoke );
			WRITE_BYTE( RANDOM_LONG(0,9) + 20 ); // scale * 10
			WRITE_BYTE( 12 ); // framerate
		MESSAGE_END();
		if (m_iDoLeftSmokePuff > 0)
			m_iDoLeftSmokePuff--;
	}
	if (m_iDoRightSmokePuff > 0 || RANDOM_LONG(0,99) > m_flRightHealth)
	{
		Vector vecSrc = pev->origin + gpGlobals->v_right * 340;
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, vecSrc );
			WRITE_BYTE( TE_SMOKE );
			WRITE_COORD( vecSrc.x );
			WRITE_COORD( vecSrc.y );
			WRITE_COORD( vecSrc.z );
			WRITE_SHORT( g_sModelIndexSmoke );
			WRITE_BYTE( RANDOM_LONG(0,9) + 20 ); // scale * 10
			WRITE_BYTE( 12 ); // framerate
		MESSAGE_END();
		if (m_iDoRightSmokePuff > 0)
			m_iDoRightSmokePuff--;
	}
}


void CAlienFlyer::TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType)
{
	if (m_iNumTracks)
	{
		int iHealthPerTrack = (int)pev->max_health / m_iNumTracks;
		int iTrack = ((int)pev->max_health - (int)pev->health) / iHealthPerTrack;

		if (iTrack != m_iCurTrack)
		{
			m_iCurTrack++;
			if (m_iCurTrack < m_iNumTracks)
			{
				m_fChangedTrack = TRUE;
				m_pGoalEnt = CBaseEntity::Instance( FIND_ENTITY_BY_TARGETNAME ( NULL, STRING( m_iszTrack[m_iCurTrack] ) ) );
				ALERT( at_console, "Alien Flyer Goal Ent: %s\n", STRING( pev->target ) );
				UpdateGoal( TRUE );
			}
		}
	}

	CBaseMonster::TraceAttack( pevAttacker, flDamage, vecDir, ptr, bitsDamageType );
}


CBaseMonster *CAlienFlyer :: MakeGrunt( Vector vecSrc )
{
	for (int i = 0; i < MAX_FLYER_GRUNTS; i++)
	{
		if (m_hGrunt[i] != NULL && m_hGrunt[i]->IsAlive())
			continue;

		if (m_hGrunt[i] != NULL && m_hGrunt[i]->pev->rendermode == kRenderNormal)
		{
			m_hGrunt[i]->SUB_StartFadeOut( );
		}

		edict_t *pent = CREATE_NAMED_ENTITY( MAKE_STRING( "monster_alien_grunt" ) );
		if ( FNullEnt( pent ) )
		{
			ALERT ( at_console, "NULL Ent in AlienFlyer!\n" );
			return NULL;
		}

		entvars_t *pevGrunt = VARS( pent );
		pevGrunt->origin = vecSrc;
		pevGrunt->angles = pev->angles;
		SetBits( pevGrunt->spawnflags, SF_MONSTER_FALL_TO_GROUND );
		DispatchSpawn( ENT( pevGrunt ) );

		CBaseMonster *pGrunt = CBaseEntity::Instance( pent )->MyMonsterPointer( );
		pGrunt->m_vecLastPosition = pev->origin - vecSrc;
		m_hGrunt[i] = pGrunt;
		return pGrunt;
	}
	return NULL;
}


/* registro dos ponteiros de Think/Touch/Use para o save do PS2 */
extern "C" void func_002397D0(const char *name, BASEPTR pfn);
extern "C" void func_002397F0(const char *name, ENTITYFUNCPTR pfn);
extern "C" void func_00239810(const char *name, USEPTR pfn);

extern "C" void func_0012FAA8(void)
{
	func_002397D0("010", static_cast<BASEPTR>(&CAlienFlyer::FlyThink));
	func_002397D0("011", static_cast<BASEPTR>(&CAlienFlyer::AttackThink));
	func_002397D0("012", static_cast<BASEPTR>(&CAlienFlyer::DyingThink));
	func_002397F0("013", static_cast<ENTITYFUNCPTR>(&CAlienFlyer::CrashTouch));
	func_00239810("014", static_cast<USEPTR>(&CAlienFlyer::CommandUse));
}
