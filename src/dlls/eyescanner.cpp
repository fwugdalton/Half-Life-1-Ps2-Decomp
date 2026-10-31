/***
*
*	Scanner de retina do Half-Life: Decay (só existe no PS2, não há fonte no HLSDK).
*	Reescrito a partir do asm de SLUS_200.66: o nome do arquivo, dos métodos e das
*	flags são inventados; a classe e os campos vêm da tabela de save (CEyeScanner).
*
*	item_eyescanner: o jogador usa, o scanner abre ("ACTIVATE"), pisca a skin por um
*	tempo e fecha ("DEACTIVATE"); dispara locked_target se está trancado (spawnflag 1
*	e master não liberado) ou unlocked_target caso contrário.
*
****/

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/saverestore.h"
#include "dlls/effects.h"
#include "include_asm.h"

extern void FireTargets( const char *targetName, CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

#define SF_EYESCANNER_LOCKED	1

// m_iMoving
#define EYESCANNER_IDLE			0
#define EYESCANNER_OPENING		1
#define EYESCANNER_CLOSING		2

class CEyeScanner : public CBaseToggle
{
public:
	void Spawn( void );
	void Precache( void );
	void KeyValue( KeyValueData *pkvd );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void EXPORT AnimateThink( void );

	virtual int Save( CSave &save );
	virtual int Restore( CRestore &restore );
	static TYPEDESCRIPTION m_SaveData[];

	int ObjectCaps( void ) { return FCAP_IMPULSE_USE; }

	void SetObjectCollisionBox( void )
	{
		if ( pev->angles.y == 0 || pev->angles.y == 180 )
		{
			pev->absmin = pev->origin + Vector( -3, -5, 0 );
			pev->absmax = pev->origin + Vector( 3, 5, 64 );
		}
		else
		{
			pev->absmin = pev->origin + Vector( -5, -3, 0 );
			pev->absmax = pev->origin + Vector( 5, 3, 64 );
		}
	}

	float m_flStopUseTime;			// 0xD0
	CBaseEntity *m_pActivator;		// 0xD4
	int m_iszLockedTarget;			// 0xD8
	int m_iszUnlockedTarget;		// 0xDC
	int m_iMoving;					// 0xE0
	float m_flResetDelay;			// 0xE4
	float m_flNextUseTime;			// 0xE8
};

TYPEDESCRIPTION CEyeScanner::m_SaveData[] =
{
	DEFINE_FIELD( CEyeScanner, m_flStopUseTime, FIELD_TIME ),
	DEFINE_FIELD( CEyeScanner, m_pActivator, FIELD_CLASSPTR ),
	DEFINE_FIELD( CEyeScanner, m_iszLockedTarget, FIELD_STRING ),
	DEFINE_FIELD( CEyeScanner, m_iszUnlockedTarget, FIELD_STRING ),
	DEFINE_FIELD( CEyeScanner, m_iMoving, FIELD_INTEGER ),
	DEFINE_FIELD( CEyeScanner, m_flResetDelay, FIELD_FLOAT ),
	DEFINE_FIELD( CEyeScanner, m_flNextUseTime, FIELD_TIME ),
};

IMPLEMENT_SAVERESTORE( CEyeScanner, CBaseToggle );

LINK_ENTITY_TO_CLASS( item_eyescanner, CEyeScanner );

void CEyeScanner::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "locked_target" ) )
	{
		m_iszLockedTarget = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "unlocked_target" ) )
	{
		m_iszUnlockedTarget = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "reset_delay" ) )
	{
		m_flResetDelay = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseToggle::KeyValue( pkvd );
}

void CEyeScanner::Spawn( void )
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->effects = 0;
	pev->frame = 0;
	pev->skin = 0;

	Precache();
	SET_MODEL( ENT( pev ), "models/eye_scanner.dol" );

	if ( pev->angles.y == 0 || pev->angles.y == 180 )
		UTIL_SetSize( pev, Vector( -3, -5, 0 ), Vector( 3, 5, 64 ) );
	else
		UTIL_SetSize( pev, Vector( -5, -3, 0 ), Vector( 5, 3, 64 ) );
}

void CEyeScanner::Precache( void )
{
	PRECACHE_MODEL( "models/eye_scanner.dol" );
}

void CEyeScanner::AnimateThink( void )
{
	DispatchAnimEvents( 0.1f );
	StudioFrameAdvance( 0 );

	if ( m_fSequenceFinished )
	{
		if ( m_iMoving == EYESCANNER_OPENING )
		{
			pev->sequence = LookupSequence( "idle_OPEN" );
			pev->frame = 0;
			ResetSequenceInfo();
			m_iMoving = EYESCANNER_IDLE;
			m_flStopUseTime = gpGlobals->time + 2.5f;
		}
		else if ( m_iMoving == EYESCANNER_CLOSING )
		{
			pev->sequence = LookupSequence( "idle_CLOSED" );
			pev->frame = 0;
			ResetSequenceInfo();
			m_iMoving = EYESCANNER_IDLE;
			SetThink( NULL );
			pev->nextthink = gpGlobals->time;
			return;
		}
	}

	if ( m_iMoving == EYESCANNER_IDLE )
	{
		// pisca a luz do scanner (skins 1..3)
		if ( pev->skin < 3 )
			pev->skin = pev->skin + 1;
		else
			pev->skin = 1;

		if ( m_flStopUseTime <= gpGlobals->time )
		{
			pev->skin = 0;
			m_pActivator = NULL;
			pev->nextthink = gpGlobals->time + 0.1f;
			pev->sequence = LookupSequence( "DEACTIVATE" );
			pev->frame = 0;
			ResetSequenceInfo();
			m_iMoving = EYESCANNER_CLOSING;
		}
		else
			pev->nextthink = gpGlobals->time + 0.15f;
	}
	else
		pev->nextthink = gpGlobals->time + 0.1f;
}

void CEyeScanner::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( m_pActivator )
		return;

	if ( m_flNextUseTime < gpGlobals->time )
	{
		if ( pActivator )
		{
			Vector vecDelta = pActivator->pev->origin - pev->origin;

			// tem que estar perto do scanner
			if ( vecDelta.Length2D() > 24 )
				return;
		}

		SetThink( &CEyeScanner::AnimateThink );
		pev->nextthink = gpGlobals->time + 0.1f;
		m_pActivator = pActivator;
		m_iMoving = EYESCANNER_OPENING;

		pev->sequence = LookupSequence( "ACTIVATE" );
		pev->frame = 0;
		ResetSequenceInfo();

		m_flNextUseTime = gpGlobals->time + m_flResetDelay;

		if ( ( pev->spawnflags & SF_EYESCANNER_LOCKED ) && m_pActivator && m_pActivator->IsPlayer() &&
			 ( !m_sMaster || !UTIL_IsMasterTriggered( m_sMaster, m_pActivator ) ) )
		{
			if ( m_iszLockedTarget )
				FireTargets( STRING( m_iszLockedTarget ), m_pActivator, this, USE_TOGGLE, 0 );
		}
		else
		{
			if ( m_iszUnlockedTarget )
				FireTargets( STRING( m_iszUnlockedTarget ), m_pActivator, this, USE_TOGGLE, 0 );
		}
	}
}

/* PS2: registra o ponteiro de think para o save */
extern "C" void func_002397D0( const char *name, BASEPTR pfn );
extern "C" void func_00164C68( void )
{
	func_002397D0( "106", static_cast<BASEPTR>( &CEyeScanner::AnimateThink ) );
}
