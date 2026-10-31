/***
*
*	Emissor de foco do Half-Life: Decay (só existe no PS2, não há fonte no HLSDK).
*	Reescrito a partir do asm de SLUS_200.66: o nome do arquivo e das classes,
*	métodos e campos são inventados; o layout é o da ROM.
*
*	item_focusemitter: monstro parado (models/focus_emitter.dol) que abre com "deploy",
*	liga um feixe entre os attachments 1 e 2 e, quando um info_emittertarget o usa,
*	gira os bone controllers até o alvo e dispara um feixe nele.
*	info_emittertarget: ponto que repassa o Use para o emissor e, quando é atingido
*	pelo feixe (value 256), dispara o seu target.
*
****/

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/weapons.h"
#include "dlls/effects.h"
#include "dlls/skill.h"
#include "include_asm.h"
#include "inline_include_asm.h"

#define TE_EXPLOSION		3
#define TE_SMOKE			5
#define TE_EXPLFLAG_NONE	0

extern "C" double fmod( double x, double y );
extern float UTIL_AngleDiff( float destAngle, float srcAngle );
extern void FireTargets( const char *targetName, CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

// value que o emissor e o alvo usam para se reconhecer no Use
#define EMITTER_USE_VALUE	256

//=========================================================
// info_emittertarget
//=========================================================
class CEmitterTarget : public CBaseToggle
{
public:
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
};

LINK_ENTITY_TO_CLASS( info_emittertarget, CEmitterTarget );

void CEmitterTarget::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( value == EMITTER_USE_VALUE )
	{
		// o feixe do emissor chegou
		if ( FClassnameIs( pActivator->pev, "item_focusemitter" ) )
			FireTargets( STRING( pev->target ), this, pCaller, useType, 0 );
	}
	else
	{
		CBaseEntity *pEmitter = UTIL_FindEntityByClassname( NULL, "item_focusemitter" );

		if ( pEmitter )
			pEmitter->Use( this, pCaller, useType, EMITTER_USE_VALUE );
	}
}

//=========================================================
// item_focusemitter
//=========================================================
class CFocusEmitter : public CBaseMonster
{
public:
	void Spawn( void );
	void Precache( void );
	void KeyValue( KeyValueData *pkvd );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType );
	int TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );
	void Killed( entvars_t *pevAttacker, int iGib );

	void SetObjectCollisionBox( void )
	{
		pev->absmin = pev->origin + Vector( -240, -240, 0 );
		pev->absmax = pev->origin + Vector( 240, 240, 480 );
	}

	inline void SetTransparency( int rendermode, int r, int g, int b, int a, int fx )
	{
		pev->rendermode = rendermode;
		pev->rendercolor.x = r;
		pev->rendercolor.y = g;
		pev->rendercolor.z = b;
		pev->renderamt = a;
		pev->renderfx = fx;
	}

	void EXPORT DeployThink( void );
	void EXPORT TrackThink( void );
	void EXPORT FallThink( void );

	float VecToYaw( Vector vecDir );
	void AimAtTarget( void );

	EHANDLE m_hTarget;			// 0x2A0
	CBeam *m_pTargetBeam;		// 0x2A8
	CBeam *m_pBeam;				// 0x2AC
	float m_flIdealYaw;			// 0x2B0
	float m_flYaw;				// 0x2B4
	float m_flYawDelta;			// 0x2B8
	float m_flIdealPitch;		// 0x2BC
	float m_flPitch;			// 0x2C0
	float m_flPitchDelta;		// 0x2C4
	float m_flAimCount;			// 0x2C8
	int m_iUnused2CC;			// 0x2CC
	BOOL m_fDeployed;			// 0x2D0
	BOOL m_fActive;				// 0x2D4
	BOOL m_fPaused;				// 0x2D8
	int m_iszDeployTarget;		// 0x2DC
	int m_iszDeathTarget;		// 0x2E0
	int m_iUnused2E4[5];		// 0x2E4
	int m_iState;				// 0x2F8: 0 inteiro, 1 e 2 quebrado, 3 morrendo
	int m_iExplode;				// 0x2FC
	float m_flNextAim;			// 0x300
};

LINK_ENTITY_TO_CLASS( item_focusemitter, CFocusEmitter );

void CFocusEmitter::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "deploy_target" ) )
	{
		if ( pkvd->szValue && *pkvd->szValue )
			m_iszDeployTarget = ALLOC_STRING( pkvd->szValue );
		else
			m_iszDeployTarget = 0;
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "death_target" ) )
	{
		if ( pkvd->szValue && *pkvd->szValue )
			m_iszDeathTarget = ALLOC_STRING( pkvd->szValue );
		else
			m_iszDeathTarget = 0;
		pkvd->fHandled = TRUE;
	}
	else
		CBaseMonster::KeyValue( pkvd );
}

void CFocusEmitter::Spawn( void )
{
	Precache();

	SET_MODEL( ENT( pev ), "models/focus_emitter.dol" );
	UTIL_SetSize( pev, Vector( -64, -64, 0 ), Vector( 64, 64, 128 ) );

	pev->solid = SOLID_BBOX;
	pev->movetype = MOVETYPE_NONE;
	pev->effects = 0;
	pev->frame = 0;
	pev->health = pev->max_health = gSkillData.focusHealth;
	pev->takedamage = DAMAGE_YES;
	m_bloodColor = DONT_BLEED;
	pev->deadflag = DEAD_NO;
	m_IdealMonsterState = MONSTERSTATE_IDLE;
	m_IdealActivity = ACT_IDLE;
	pev->flags |= FL_MONSTER;

	InitBoneControllers();

	SetThink( &CFocusEmitter::FallThink );
	pev->nextthink = gpGlobals->time + 0.1f;

	m_flYaw = m_flIdealYaw = 0;
	SetBoneController( 0, 0 );
	m_flPitch = m_flIdealPitch = 0;
	SetBoneController( 1, 0 );

	m_fPaused = 0;
	m_fActive = 0;
	m_fDeployed = 0;
	m_MonsterState = MONSTERSTATE_NONE;
	m_iState = 0;
}

void CFocusEmitter::Precache( void )
{
	PRECACHE_MODEL( "models/focus_emitter.dol" );
	PRECACHE_MODEL( "sprites/laserbeam.spz" );
	PRECACHE_SOUND( "debris/beamstart5.wav" );
	m_iExplode = PRECACHE_MODEL( "sprites/fexplo.spz" );
}

void CFocusEmitter::DeployThink( void )
{
	DispatchAnimEvents( 0.1f );
	StudioFrameAdvance( 0 );

	if ( m_fSequenceFinished )
	{
		if ( m_iState == 3 )
		{
			SetThink( NULL );
			FireTargets( STRING( m_iszDeathTarget ), this, this, USE_TOGGLE, 0 );
			return;
		}

		pev->sequence = LookupSequence( "idle_open" );
		pev->frame = 0;
		ResetSequenceInfo();

		m_fDeployed = m_fActive = TRUE;

		m_pBeam = CBeam::BeamCreate( "sprites/laserbeam.spz", 30 );
		m_pBeam->EntsInit( entindex(), entindex() );
		m_pBeam->SetStartAttachment( 1 );
		m_pBeam->SetEndAttachment( 2 );
		m_pBeam->SetColor( 255, 255, 255 );
		m_pBeam->SetBrightness( 192 );
		m_pBeam->SetScrollRate( 20 );

		FireTargets( STRING( m_iszDeployTarget ), this, this, USE_TOGGLE, 0 );

		SetThink( &CFocusEmitter::TrackThink );
		m_flNextAim = 0;
		pev->nextthink = gpGlobals->time + 0.1f;
		SetBodygroup( 1, 1 );
		return;
	}

	if ( m_iState == 3 )
	{
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_EXPLOSION );
			WRITE_COORD( pev->origin.x + RANDOM_FLOAT( -150, 150 ) );
			WRITE_COORD( pev->origin.y + RANDOM_FLOAT( -150, 150 ) );
			WRITE_COORD( pev->origin.z + RANDOM_FLOAT( 0, 150 ) );
			WRITE_SHORT( g_sModelIndexFireball );
			WRITE_BYTE( RANDOM_LONG( 0, 29 ) + 30 );
			WRITE_BYTE( 12 );
			WRITE_BYTE( TE_EXPLFLAG_NONE );
		MESSAGE_END();

		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_SMOKE );
			WRITE_COORD( pev->origin.x + RANDOM_FLOAT( -150, 150 ) );
			WRITE_COORD( pev->origin.y + RANDOM_FLOAT( -150, 150 ) );
			WRITE_COORD( pev->origin.z + RANDOM_FLOAT( 0, 150 ) );
			WRITE_SHORT( g_sModelIndexSmoke );
			WRITE_BYTE( 100 );
			WRITE_BYTE( 10 );
		MESSAGE_END();
	}

	pev->nextthink = gpGlobals->time + 0.1f;
}

float CFocusEmitter::VecToYaw( Vector vecDir )
{
	if ( vecDir.x == 0 && vecDir.y == 0 && vecDir.z == 0 )
		return pev->angles.y;

	return UTIL_VecToYaw( vecDir );
}

void CFocusEmitter::AimAtTarget( void )
{
	if ( (CBaseEntity *)m_hTarget == NULL )
		return;

	Vector vecDir;
	Vector vecSrc, vecAngles;
	Vector vecEnd, vecEndAngles;

	GetAttachment( 1, vecSrc, vecAngles );
	GetAttachment( 2, vecEnd, vecEndAngles );
	vecSrc.z = vecEnd.z;

	vecDir = ((CBaseEntity *)m_hTarget)->pev->origin - vecSrc;

	float yaw = VecToYaw( vecDir );
	if ( yaw > 360 )
		yaw -= 360;
	else if ( yaw < 0 )
		yaw += 360;

	m_flIdealYaw = yaw;
	m_flYawDelta = UTIL_AngleDiff( yaw, m_flYaw );

	vecAngles = UTIL_VecToAngles( vecDir );
	m_flIdealPitch = -vecAngles.x;
	m_flPitchDelta = UTIL_AngleDiff( m_flIdealPitch, m_flPitch );
}

void CFocusEmitter::TrackThink( void )
{
	pev->nextthink = gpGlobals->time + 0.1f;

	DispatchAnimEvents( 0.1f );
	StudioFrameAdvance( 0 );

	if ( m_fSequenceFinished )
	{
		if ( m_iState == 3 )
		{
			SetThink( NULL );
			FireTargets( STRING( m_iszDeathTarget ), this, this, USE_TOGGLE, 0 );
		}
	}
	else
	{
		if ( m_iState == 3 )
		{
			MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
				WRITE_BYTE( TE_EXPLOSION );
				WRITE_COORD( pev->origin.x + RANDOM_FLOAT( -150, 150 ) );
				WRITE_COORD( pev->origin.y + RANDOM_FLOAT( -150, 150 ) );
				WRITE_COORD( pev->origin.z + RANDOM_FLOAT( 0, 150 ) );
				WRITE_SHORT( g_sModelIndexFireball );
				WRITE_BYTE( RANDOM_LONG( 0, 29 ) + 30 );
				WRITE_BYTE( 12 );
				WRITE_BYTE( TE_EXPLFLAG_NONE );
			MESSAGE_END();

			MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
				WRITE_BYTE( TE_SMOKE );
				WRITE_COORD( pev->origin.x + RANDOM_FLOAT( -150, 150 ) );
				WRITE_COORD( pev->origin.y + RANDOM_FLOAT( -150, 150 ) );
				WRITE_COORD( pev->origin.z + RANDOM_FLOAT( 0, 150 ) );
				WRITE_SHORT( g_sModelIndexSmoke );
				WRITE_BYTE( 100 );
				WRITE_BYTE( 10 );
			MESSAGE_END();
		}

		pev->nextthink = gpGlobals->time + 0.1f;
	}

	if ( gpGlobals->time < m_flNextAim )
		return;

	m_flNextAim = gpGlobals->time + 0.1f;

	if ( m_flIdealYaw == m_flYaw )
	{
		// já está mirando: dispara o feixe no alvo
		if ( (CBaseEntity *)m_hTarget != NULL )
		{
			m_pTargetBeam = CBeam::BeamCreate( "sprites/laserbeam.spz", 30 );
			m_pTargetBeam->PointEntInit( ((CBaseEntity *)m_hTarget)->pev->origin, entindex() );
			m_pTargetBeam->SetEndAttachment( 3 );
			m_pTargetBeam->SetColor( 255, 255, 255 );
			m_pTargetBeam->SetBrightness( 192 );
			m_pTargetBeam->LiveForTime( 1.0f );
			m_pTargetBeam->SetScrollRate( 20 );

			EMIT_SOUND( ENT( pev ), CHAN_VOICE, "debris/beamstart5.wav", 1.0f, 0.2f );

			m_flNextAim = gpGlobals->time + 1.0f;
			FireTargets( STRING( pev->target ), this, this, USE_TOGGLE, 0 );
			((CBaseEntity *)m_hTarget)->Use( this, this, USE_TOGGLE, EMITTER_USE_VALUE );
			m_hTarget = NULL;
		}
		return;
	}

	if ( m_flIdealYaw != m_flYaw )
	{
		float flStep;

		if ( m_flYawDelta < 0 )
			flStep = max( m_flYawDelta, -5.0f );
		else
			flStep = min( m_flYawDelta, 5.0f );

		m_flYaw += flStep;
		m_flYawDelta -= flStep;

		if ( m_flYaw > 360 )
			m_flYaw -= 360;
		else if ( m_flYaw < 0 )
			m_flYaw += 360;

		float yaw = m_flYaw - pev->angles.y;
		if ( yaw > 360 )
			yaw = fmod( yaw, 360 );

		SetBoneController( 0, yaw );
	}

	if ( m_flIdealPitch != m_flPitch )
	{
		float flStep;

		if ( m_flPitchDelta < 0 )
			flStep = max( m_flPitchDelta, -5.0f );
		else
			flStep = min( m_flPitchDelta, 5.0f );

		m_flPitch += flStep;
		m_flPitchDelta -= flStep;

		if ( m_flPitch > 360 )
			m_flPitch -= 360;
		else if ( m_flPitch < 0 )
			m_flPitch += 360;

		SetBoneController( 1, m_flPitch );
	}

	m_flAimCount += 1.0f;
}

void CFocusEmitter::FallThink( void )
{
	if ( DROP_TO_FLOOR( ENT( pev ) ) == 0 )
	{
		ALERT( at_error, "Item %s fell out of level at %f,%f,%f", STRING( pev->classname ), pev->origin.x, pev->origin.y, pev->origin.z );
		UTIL_Remove( this );
	}
}

void CFocusEmitter::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( value == EMITTER_USE_VALUE )
	{
		if ( FClassnameIs( pActivator->pev, "info_emittertarget" ) )
		{
			m_hTarget = pActivator;
			AimAtTarget();
			m_flAimCount = 5.0f;
		}
		return;
	}

	if ( !m_fDeployed )
	{
		pev->sequence = LookupSequence( "deploy" );
		pev->frame = 0;
		ResetSequenceInfo();
		SetThink( &CFocusEmitter::DeployThink );
		pev->nextthink = gpGlobals->time + 0.1f;
	}
	else if ( !m_fActive )
	{
		SetThink( &CFocusEmitter::TrackThink );
		m_flNextAim = 0;
		pev->nextthink = gpGlobals->time + 0.1f;
		m_fActive = TRUE;
		m_fPaused = FALSE;
	}
	else
		m_fPaused = TRUE;
}

void CFocusEmitter::TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType )
{
#ifndef NON_MATCHING
	/* não casa (a ROM lê m_fActive à toa no delay slot do teste do pevAttacker e aloca
	   os registradores de outro jeito); o corpo vem do asm */
	INLINE_INCLUDE_ASM( "asm/nonmatchings/dlls/focus_emitter", TraceAttack__13CFocusEmitterP9entvars_sfG6VectorP11TraceResulti );
#endif
	UTIL_Ricochet( ptr->vecEndPos, RANDOM_FLOAT( 0.5f, 1.5f ) );
	pev->dmgtime = gpGlobals->time;

	// só o alienflyer machuca o emissor
	if ( pevAttacker && !FStrEq( STRING( pevAttacker->classname ), "monster_alienflyer" ) )
		flDamage = 0;
	else
		flDamage = 1;

	CBaseMonster::TraceAttack( pevAttacker, flDamage, vecDir, ptr, bitsDamageType );
}

int CFocusEmitter::TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	if ( pevAttacker && FStrEq( STRING( pevAttacker->classname ), "monster_alienflyer" ) &&
		 pevInflictor && FStrEq( STRING( pevInflictor->classname ), "monster_alienflyer" ) && bitsDamageType == 0 )
		flDamage = 1;
	else
		flDamage = 0;

	if ( pev->health - flDamage < pev->max_health / 3 && m_iState < 2 )
	{
		pev->sequence = LookupSequence( "broken2" );
		pev->frame = 0;
		ResetSequenceInfo();
		m_iState = 2;
	}
	else if ( pev->health - flDamage < pev->max_health / 3 * 2 && m_iState <= 0 )
	{
		pev->sequence = LookupSequence( "broken1" );
		pev->frame = 0;
		ResetSequenceInfo();
		m_iState = 1;
	}

	return CBaseMonster::TakeDamage( pevInflictor, pevAttacker, flDamage, bitsDamageType );
}

void CFocusEmitter::Killed( entvars_t *pevAttacker, int iGib )
{
	pev->sequence = LookupSequence( "death" );
	pev->frame = 0;
	ResetSequenceInfo();

	SetThink( &CFocusEmitter::DeployThink );
	pev->nextthink = gpGlobals->time + 0.1f;
	m_iState = 3;
	pev->takedamage = DAMAGE_NO;
}

/* PS2: registra os ponteiros de think para o save */
extern "C" void func_002397D0( const char *name, BASEPTR pfn );
extern "C" void func_001672C8( void )
{
	func_002397D0( "107", static_cast<BASEPTR>( &CFocusEmitter::DeployThink ) );
	func_002397D0( "108", static_cast<BASEPTR>( &CFocusEmitter::FallThink ) );
	func_002397D0( "110", static_cast<BASEPTR>( &CFocusEmitter::TrackThink ) );
}
