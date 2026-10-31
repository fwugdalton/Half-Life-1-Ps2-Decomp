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

//=========================================================
// Generic Monster - purely for scripted sequence work.
//=========================================================
/* dlls/genericmonster.cpp: monster_generic na versão nova do SDK (vira a cabeça para
   quem fala) com os acréscimos do PS2/Decay: alvo de treino que conta acertos do
   jogador e dispara target_trigger/no_target_trigger, e o trigger_resetgeneric que
   reinicia o alvo. */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/schedule.h"
#include "dlls/player.h"
#include "dlls/scripted.h"
#include "include_asm.h"

// For holograms, make them not solid so the player can walk through them
#define	SF_GENERICMONSTER_NOTSOLID					4 
#define	SF_HEAD_CONTROLLER							8
#define	SF_GENERICMONSTER_NEUTRAL					0x20000

void FireTargets( const char *targetName, CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
CBaseEntity *UTIL_FindEntityByTargetname( CBaseEntity *pStartEntity, const char *szName );

//=========================================================
// Monster's Anim Events Go Here
//=========================================================

class CGenericMonster : public CBaseMonster
{
public:
	void Spawn( void );
	void Precache( void );
	void SetYawSpeed( void );
	int  Classify ( void );
	void HandleAnimEvent( MonsterEvent_t *pEvent );
	int ISoundMask ( void );
	void KeyValue( KeyValueData *pkvd );

	int	Save( CSave &save ); 
	int Restore( CRestore &restore );
	static TYPEDESCRIPTION m_SaveData[];

	void IdleHeadTurn( Vector &vecFriend );

	void PlayScriptedSentence( const char *pszSentence, float duration, float volume, float attenuation, BOOL bConcurrent, CBaseEntity *pListener );
	void MonsterThink( void );
	int  TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType );
	void Killed( entvars_t *pevAttacker, int iGib );

	float m_talkTime;
	EHANDLE m_hTalkTarget;
	float m_flIdealYaw;
	float m_flCurrentYaw;
	int m_nNumHits;
	int m_nNumNoHits;
	int m_nTriggerHits;
	string_t m_iszTargetTrigger;
	string_t m_iszNoTargetTrigger;
};
LINK_ENTITY_TO_CLASS( monster_generic, CGenericMonster );

TYPEDESCRIPTION	CGenericMonster::m_SaveData[] = 
{
	DEFINE_FIELD( CGenericMonster, m_talkTime, FIELD_FLOAT ),
	DEFINE_FIELD( CGenericMonster, m_hTalkTarget, FIELD_EHANDLE ),
	DEFINE_FIELD( CGenericMonster, m_flIdealYaw, FIELD_FLOAT ),
	DEFINE_FIELD( CGenericMonster, m_flCurrentYaw, FIELD_FLOAT ),
	DEFINE_FIELD( CGenericMonster, m_nNumHits, FIELD_INTEGER ),
	DEFINE_FIELD( CGenericMonster, m_nNumNoHits, FIELD_INTEGER ),
	DEFINE_FIELD( CGenericMonster, m_nTriggerHits, FIELD_INTEGER ),
	DEFINE_FIELD( CGenericMonster, m_iszTargetTrigger, FIELD_STRING ),
	DEFINE_FIELD( CGenericMonster, m_iszNoTargetTrigger, FIELD_STRING ),
};

IMPLEMENT_SAVERESTORE( CGenericMonster, CBaseMonster );

void CGenericMonster :: KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "target_trigger"))
	{
		if (pkvd->szValue && *pkvd->szValue)
			m_iszTargetTrigger = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "no_target_trigger"))
	{
		if (pkvd->szValue && *pkvd->szValue)
			m_iszNoTargetTrigger = ALLOC_STRING(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "trigger_hits"))
	{
		m_nTriggerHits = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseMonster::KeyValue( pkvd );
}

//=========================================================
// Classify - indicates this monster's place in the 
// relationship table.
//=========================================================
int	CGenericMonster :: Classify ( void )
{
	if ( pev->spawnflags & SF_GENERICMONSTER_NEUTRAL )
		return CLASS_NONE;
	return	CLASS_PLAYER_ALLY;
}

//=========================================================
// SetYawSpeed - allows each sequence to have a different
// turn rate associated with it.
//=========================================================
void CGenericMonster :: SetYawSpeed ( void )
{
	int ys;

	switch ( m_Activity )
	{
	case ACT_IDLE:
	default:
		ys = 90;
	}

	pev->yaw_speed = ys;
}

//=========================================================
// HandleAnimEvent - catches the monster-specific messages
// that occur when tagged animation frames are played.
//=========================================================
void CGenericMonster :: HandleAnimEvent( MonsterEvent_t *pEvent )
{
	switch( pEvent->event )
	{
	case 0:
	default:
		CBaseMonster::HandleAnimEvent( pEvent );
		break;
	}
}

//=========================================================
// ISoundMask - generic monster can't hear.
//=========================================================
int CGenericMonster :: ISoundMask ( void )
{
	return	NULL;
}

//=========================================================
// Spawn
//=========================================================
void CGenericMonster :: Spawn()
{
	Precache();

	SET_MODEL( ENT(pev), STRING(pev->model) );

	if ( FStrEq( STRING(pev->model), "models/player.dol" ) || FStrEq( STRING(pev->model), "models/holo.dol" ) )
		UTIL_SetSize(pev, VEC_HULL_MIN, VEC_HULL_MAX);
	else
		UTIL_SetSize(pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX);

	pev->solid			= SOLID_SLIDEBOX;
	pev->movetype		= MOVETYPE_STEP;
	if ( m_iszTargetTrigger || m_iszNoTargetTrigger )
		m_bloodColor	= DONT_BLEED;
	else
		m_bloodColor	= BLOOD_COLOR_RED;
	if ( pev->max_health > 0 )
		pev->health		= pev->max_health;
	else
		pev->health		= 8;
	m_flFieldOfView		= 0.5;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_MonsterState		= MONSTERSTATE_NONE;

	MonsterInit();

	if ( pev->spawnflags & SF_GENERICMONSTER_NOTSOLID )
	{
		pev->solid = SOLID_NOT;
		pev->takedamage = DAMAGE_NO;
	}

	if ( pev->spawnflags & SF_HEAD_CONTROLLER )
	{
		m_afCapability = bits_CAP_TURN_HEAD;
	}

	m_flIdealYaw = m_flCurrentYaw = 0;
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CGenericMonster :: Precache()
{
	PRECACHE_MODEL( (char *)STRING(pev->model) );
}	

//=========================================================
// AI Schedules Specific to this monster
//=========================================================

void CGenericMonster::PlayScriptedSentence( const char *pszSentence, float duration, float volume, float attenuation, BOOL bConcurrent, CBaseEntity *pListener )
{
	m_talkTime = gpGlobals->time + duration;
	PlaySentence( pszSentence, duration, volume, attenuation );

	m_hTalkTarget = pListener;
}

void CGenericMonster::IdleHeadTurn( Vector &vecFriend )
{
	 // turn head in desired direction only if ent has a turnable head
	if (m_afCapability & bits_CAP_TURN_HEAD)
	{
		float yaw = VecToYaw( vecFriend - pev->origin ) - pev->angles.y;

		if (yaw > 180) yaw -= 360;
		if (yaw < -180) yaw += 360;

		m_flIdealYaw = yaw;
	}
}

void CGenericMonster::MonsterThink()
{
	if (m_afCapability & bits_CAP_TURN_HEAD)
	{
		if ((CBaseEntity *)m_hTalkTarget)
		{
			if (gpGlobals->time > m_talkTime)
			{
				m_flIdealYaw = 0;
				m_hTalkTarget = NULL;
			}
			else
			{
				IdleHeadTurn(((CBaseEntity *)m_hTalkTarget)->pev->origin);
			}
		}

		if (m_flIdealYaw != m_flCurrentYaw)
		{
			if (m_flIdealYaw < m_flCurrentYaw)
			{
				m_flCurrentYaw -= min(m_flCurrentYaw - m_flIdealYaw, 20.0f);
			}
			else
			{
				m_flCurrentYaw += min(m_flIdealYaw - m_flCurrentYaw, 20.0f);
			}
			SetBoneController(0, m_flCurrentYaw);
		}
	}

	CBaseMonster::MonsterThink();
}

// PS2: alvo de treino; conta os acertos seguidos do jogador
int CGenericMonster::TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType )
{
	CBaseEntity *pAttacker = CBaseEntity::Instance( pevAttacker );

	if ( m_iszTargetTrigger && pAttacker->IsPlayer() )
	{
		if ( ((CBasePlayer *)pAttacker)->m_ps2hUnk77C == this )
		{
			m_nNumNoHits = 0;
			m_nNumHits++;
		}
		else
		{
			m_nNumHits = 0;
			m_nNumNoHits++;
		}

		if ( m_nNumHits >= m_nTriggerHits )
		{
			FireTargets( STRING( m_iszTargetTrigger ), this, this, USE_TOGGLE, 0 );
			m_nNumNoHits = 0;
			m_nNumHits = 0;
		}
		else if ( m_nNumNoHits >= m_nTriggerHits )
		{
			FireTargets( STRING( m_iszNoTargetTrigger ), this, this, USE_TOGGLE, 0 );
			m_nNumNoHits = 0;
			m_nNumHits = 0;
		}
	}

	return CBaseMonster::TakeDamage( pevInflictor, pevAttacker, flDamage, bitsDamageType );
}

void CGenericMonster::Killed( entvars_t *pevAttacker, int iGib )
{
	if ( pev->max_health > 0 )
		iGib = TRUE;

	CBaseEntity *pAttacker = CBaseEntity::Instance( pevAttacker );

	if ( m_iszTargetTrigger && pAttacker->IsPlayer() && ((CBasePlayer *)pAttacker)->m_ps2hUnk77C == this )
		FireTargets( STRING( m_iszTargetTrigger ), this, this, USE_TOGGLE, 0 );

	if ( m_iszNoTargetTrigger && pAttacker->IsPlayer() && ((CBasePlayer *)pAttacker)->m_ps2hUnk77C != this )
		FireTargets( STRING( m_iszNoTargetTrigger ), this, this, USE_TOGGLE, 0 );

	CBaseMonster::Killed( pevAttacker, iGib );
}

// PS2: trigger_resetgeneric reinicia o monster_generic (alvo de treino) apontado por target
class CResetGeneric : public CBaseToggle
{
public:
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
};
LINK_ENTITY_TO_CLASS( trigger_resetgeneric, CResetGeneric );

void CResetGeneric::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	CBaseEntity *pTarget = UTIL_FindEntityByTargetname( NULL, STRING(pev->target) );

	if ( !pTarget )
		return;

	if ( strcmp( "monster_generic", STRING(pTarget->pev->classname) ) )
		return;

	CGenericMonster *pGeneric = (CGenericMonster *)pTarget;

	if ( pGeneric->pev->max_health > 0 )
		pGeneric->pev->health = pGeneric->pev->max_health;
	else
		pGeneric->pev->health = 8;
	pGeneric->m_MonsterState = MONSTERSTATE_NONE;
	pGeneric->MonsterInit();

	pGeneric->m_talkTime = pGeneric->m_flIdealYaw = pGeneric->m_flCurrentYaw = 0;
	pGeneric->m_hTalkTarget = NULL;
	pGeneric->m_nNumHits = 0;
	pGeneric->m_nNumNoHits = 0;

	if ( pGeneric->m_pCine )
	{
		pGeneric->m_pCine->m_hTargetEnt = NULL;
		pGeneric->m_pCine = NULL;
	}

	CBasePlayer *pPlayer = (CBasePlayer *)UTIL_PlayerByIndex( 1 );

	if ( pPlayer && (CBaseEntity *)pPlayer->m_ps2hUnk77C )
	{
		((CBaseEntity *)pPlayer->m_ps2hUnk77C)->pev->effects &= ~0x100;
		pPlayer->m_ps2hUnk77C = NULL;
		UTIL_Remove( pPlayer->m_ps2hUnk79C );
		pPlayer->m_ps2hUnk79C = NULL;
		(*g_engfuncs.pfnPs2Unk1E0)( 1, 0 );
	}
}
