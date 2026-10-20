/* dlls/triggers.cpp: entidades de gatilho do HLSDK (func_friction,
   trigger_*, multi_manager, trigger_changelevel, trigger_camera...).
   O PS2 acrescenta multi_kill_manager, trigger_playerfreeze, trigger_random,
   trigger_bit, trigger_bit_counter, trigger_new_hurt, trigger_save_campaign
   e trigger_enddecay (fim das missões do Decay). */

#include "include_asm.h"
#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/player.h"
#include "dlls/saverestore.h"
#include "dlls/gamerules.h"

#define	SF_TRIGGER_PUSH_START_OFF	2//spawnflag that makes trigger_push spawn turned OFF
#define SF_TRIGGER_HURT_TARGETONCE	1// Only fire hurt target once
#define	SF_TRIGGER_HURT_START_OFF	2//spawnflag that makes trigger_push spawn turned OFF
#define	SF_TRIGGER_HURT_NO_CLIENTS	8//spawnflag that makes trigger_push spawn turned OFF
#define SF_TRIGGER_HURT_CLIENTONLYFIRE	16// trigger hurt will only fire its target if it is hurting a client
#define SF_TRIGGER_HURT_CLIENTONLYTOUCH 32// only clients may touch this trigger.

#define MAX_MULTI_TARGETS	16

typedef enum { GLOBAL_OFF = 0, GLOBAL_ON = 1, GLOBAL_DEAD = 2 } GLOBALESTATE;

class CGlobalState
{
public:
	GLOBALESTATE EntityGetState( string_t globalname );
};

extern CGlobalState gGlobalState;

extern "C" void *memcpy(void *dst, const void *src, unsigned int n);

extern BOOL		g_fGameOver;
extern void SetMovedir(entvars_t* pev);
extern Vector VecBModelOrigin( entvars_t* pevBModel );
extern void FireTargets( const char *targetName, CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

class CFrictionModifier : public CBaseEntity
{
public:
	void		Spawn( void );
	void		KeyValue( KeyValueData *pkvd );
	void EXPORT	ChangeFriction( CBaseEntity *pOther );
	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	virtual int	ObjectCaps( void ) { return CBaseEntity :: ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

	static	TYPEDESCRIPTION m_SaveData[];

	float		m_frictionFraction;		// Sorry, couldn't resist this name :)
};

LINK_ENTITY_TO_CLASS( func_friction, CFrictionModifier );

// Global Savedata for changelevel friction modifier
TYPEDESCRIPTION	CFrictionModifier::m_SaveData[] =
{
	DEFINE_FIELD( CFrictionModifier, m_frictionFraction, FIELD_FLOAT ),
};

IMPLEMENT_SAVERESTORE(CFrictionModifier,CBaseEntity);


// Modify an entity's friction
void CFrictionModifier :: Spawn( void )
{
	pev->solid = SOLID_TRIGGER;
	SET_MODEL(ENT(pev), STRING(pev->model));    // set size and link into world
	pev->movetype = MOVETYPE_NONE;
	SetTouch( &CFrictionModifier::ChangeFriction );
}


// Sets toucher's friction to m_frictionFraction (1.0 = normal friction)
void CFrictionModifier :: ChangeFriction( CBaseEntity *pOther )
{
	if ( pOther->pev->movetype != MOVETYPE_BOUNCEMISSILE && pOther->pev->movetype != MOVETYPE_BOUNCE )
		pOther->pev->friction = m_frictionFraction;
}



// Sets toucher's friction to m_frictionFraction (1.0 = normal friction)
void CFrictionModifier :: KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "modifier"))
	{
		m_frictionFraction = atof(pkvd->szValue) / 100.0;
		pkvd->fHandled = TRUE;
	}
	else
		CBaseEntity::KeyValue( pkvd );
}


// This trigger will fire when the level spawns (or respawns if not fire once)
// It will check a global state before firing.  It supports delay and killtargets

#define SF_AUTO_FIREONCE		0x0001

class CAutoTrigger : public CBaseDelay
{
public:
	void KeyValue( KeyValueData *pkvd );
	void Spawn( void );
	void Precache( void );
	void Think( void );

	int ObjectCaps( void ) { return CBaseDelay::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );

	static	TYPEDESCRIPTION m_SaveData[];

private:
	int			m_globalstate;
	USE_TYPE	triggerType;
};
LINK_ENTITY_TO_CLASS( trigger_auto, CAutoTrigger );

TYPEDESCRIPTION	CAutoTrigger::m_SaveData[] =
{
	DEFINE_FIELD( CAutoTrigger, m_globalstate, FIELD_STRING ),
	DEFINE_FIELD( CAutoTrigger, triggerType, FIELD_INTEGER ),
};

IMPLEMENT_SAVERESTORE(CAutoTrigger,CBaseDelay);

void CAutoTrigger::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "globalstate"))
	{
		m_globalstate = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "triggerstate"))
	{
		int type = atoi( pkvd->szValue );
		switch( type )
		{
		case 0:
			triggerType = USE_OFF;
			break;
		case 2:
			triggerType = USE_TOGGLE;
			break;
		default:
			triggerType = USE_ON;
			break;
		}
		pkvd->fHandled = TRUE;
	}
	else
		CBaseDelay::KeyValue( pkvd );
}


void CAutoTrigger::Spawn( void )
{
	Precache();
}


void CAutoTrigger::Precache( void )
{
	pev->nextthink = gpGlobals->time + 0.1f;
}


void CAutoTrigger::Think( void )
{
	if ( !m_globalstate || gGlobalState.EntityGetState( m_globalstate ) == GLOBAL_ON )
	{
		SUB_UseTargets( this, triggerType, 0 );
		if ( pev->spawnflags & SF_AUTO_FIREONCE )
			UTIL_Remove( this );
	}
}



#define SF_RELAY_FIREONCE		0x0001

class CTriggerRelay : public CBaseDelay
{
public:
	void KeyValue( KeyValueData *pkvd );
	void Spawn( void );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	int ObjectCaps( void ) { return CBaseDelay::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );

	static	TYPEDESCRIPTION m_SaveData[];

private:
	USE_TYPE	triggerType;
};
LINK_ENTITY_TO_CLASS( trigger_relay, CTriggerRelay );

TYPEDESCRIPTION	CTriggerRelay::m_SaveData[] =
{
	DEFINE_FIELD( CTriggerRelay, triggerType, FIELD_INTEGER ),
};

IMPLEMENT_SAVERESTORE(CTriggerRelay,CBaseDelay);

void CTriggerRelay::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "triggerstate"))
	{
		int type = atoi( pkvd->szValue );
		switch( type )
		{
		case 0:
			triggerType = USE_OFF;
			break;
		case 2:
			triggerType = USE_TOGGLE;
			break;
		default:
			triggerType = USE_ON;
			break;
		}
		pkvd->fHandled = TRUE;
	}
	else
		CBaseDelay::KeyValue( pkvd );
}


void CTriggerRelay::Spawn( void )
{
}




void CTriggerRelay::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	SUB_UseTargets( this, triggerType, 0 );
	if ( pev->spawnflags & SF_RELAY_FIREONCE )
		UTIL_Remove( this );
}


//**********************************************************
// The Multimanager Entity - when fired, will fire up to 16 targets
// at specified times.
// FLAG:		THREAD (create clones when triggered)
// FLAG:		CLONE (this is a clone for a threaded execution)

#define SF_MULTIMAN_CLONE		0x80000000
#define SF_MULTIMAN_THREAD		0x00000001

class CMultiManager : public CBaseToggle
{
public:
	void KeyValue( KeyValueData *pkvd );
	void Spawn ( void );
	void EXPORT ManagerThink ( void );
	void EXPORT ManagerUse   ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	BOOL		HasTarget( string_t targetname );

	int ObjectCaps( void ) { return CBaseToggle::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );

	static	TYPEDESCRIPTION m_SaveData[];

	int		m_cTargets;	// the total number of targets in this manager's fire list.
	int		m_index;	// Current target
	float	m_startTime;// Time we started firing
	int		m_iTargetName	[ MAX_MULTI_TARGETS ];// list if indexes into global string array
	float	m_flTargetDelay [ MAX_MULTI_TARGETS ];// delay (in seconds) from time of manager fire to target fire

	/* PS2: virtuais para o multi_kill_manager */
	virtual void ManagerFire( void );
	virtual CMultiManager *Clone( void );

protected:
	inline BOOL IsClone( void ) { return (pev->spawnflags & SF_MULTIMAN_CLONE) ? TRUE : FALSE; }
	inline BOOL ShouldClone( void )
	{
		if ( IsClone() )
			return FALSE;

		return (pev->spawnflags & SF_MULTIMAN_THREAD) ? TRUE : FALSE;
	}
};
LINK_ENTITY_TO_CLASS( multi_manager, CMultiManager );

// Global Savedata for multi_manager
TYPEDESCRIPTION	CMultiManager::m_SaveData[] =
{
	DEFINE_FIELD( CMultiManager, m_cTargets, FIELD_INTEGER ),
	DEFINE_FIELD( CMultiManager, m_index, FIELD_INTEGER ),
	DEFINE_FIELD( CMultiManager, m_startTime, FIELD_TIME ),
	DEFINE_ARRAY( CMultiManager, m_iTargetName, FIELD_STRING, MAX_MULTI_TARGETS ),
	DEFINE_ARRAY( CMultiManager, m_flTargetDelay, FIELD_FLOAT, MAX_MULTI_TARGETS ),
};

IMPLEMENT_SAVERESTORE(CMultiManager,CBaseToggle);

void CMultiManager :: KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "wait"))
	{
		m_flWait = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else // add this field to the target list
	{
		// this assumes that additional fields are targetnames and their values are delay values.
		if ( m_cTargets < MAX_MULTI_TARGETS )
		{
			char tmp[128];

			UTIL_StripToken( pkvd->szKeyName, tmp );
			m_iTargetName [ m_cTargets ] = ALLOC_STRING( tmp );
			m_flTargetDelay [ m_cTargets ] = atof (pkvd->szValue);
			m_cTargets++;
			pkvd->fHandled = TRUE;
		}
	}
}


void CMultiManager :: Spawn( void )
{
	pev->solid = SOLID_NOT;
	SetUse ( &CMultiManager::ManagerUse );
	SetThink ( &CMultiManager::ManagerThink);

	// Sort targets
	// Quick and dirty bubble sort
	int swapped = 1;

	while ( swapped )
	{
		swapped = 0;
		for ( int i = 1; i < m_cTargets; i++ )
		{
			if ( m_flTargetDelay[i] < m_flTargetDelay[i-1] )
			{
				// Swap out of order elements
				int name = m_iTargetName[i];
				float delay = m_flTargetDelay[i];
				m_iTargetName[i] = m_iTargetName[i-1];
				m_flTargetDelay[i] = m_flTargetDelay[i-1];
				m_iTargetName[i-1] = name;
				m_flTargetDelay[i-1] = delay;
				swapped = 1;
			}
		}
	}
}


BOOL CMultiManager::HasTarget( string_t targetname )
{
	for ( int i = 0; i < m_cTargets; i++ )
		if ( FStrEq(STRING(targetname), STRING(m_iTargetName[i])) )
			return TRUE;

	return FALSE;
}


// Designers were using this to fire targets that may or may not exist --
// so I changed it to use the standard target fire code, made it a little simpler.
void CMultiManager :: ManagerThink ( void )
{
	ManagerFire();
}


void CMultiManager :: ManagerFire ( void )
{
	float	time;

	time = gpGlobals->time - m_startTime;
	while ( m_index < m_cTargets && m_flTargetDelay[ m_index ] <= time )
	{
		FireTargets( STRING( m_iTargetName[ m_index ] ), m_hActivator, this, USE_TOGGLE, 0 );
		m_index++;
	}

	if ( m_index >= m_cTargets )// have we fired all targets?
	{
		SetThink( NULL );
		if ( IsClone() )
		{
			UTIL_Remove( this );
			return;
		}
		SetUse ( &CMultiManager::ManagerUse );// allow manager re-use
	}
	else
		pev->nextthink = m_startTime + m_flTargetDelay[ m_index ];
}


CMultiManager *CMultiManager::Clone( void )
{
	CMultiManager *pMulti = GetClassPtr( (CMultiManager *)NULL );

	edict_t *pEdict = pMulti->pev->pContainingEntity;
	memcpy( pMulti->pev, pev, sizeof(*pev) );
	pMulti->pev->pContainingEntity = pEdict;

	pMulti->pev->spawnflags |= SF_MULTIMAN_CLONE;
	pMulti->m_cTargets = m_cTargets;
	memcpy( pMulti->m_iTargetName, m_iTargetName, sizeof( m_iTargetName ) );
	memcpy( pMulti->m_flTargetDelay, m_flTargetDelay, sizeof( m_flTargetDelay ) );

	return pMulti;
}


// The USE function builds the time table and starts the entity thinking.
void CMultiManager :: ManagerUse ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	// In multiplayer games, clone the MM and execute in the clone (like a thread)
	// to allow multiple players to trigger the same multimanager
	if ( ShouldClone() )
	{
		CMultiManager *pClone = Clone();
		pClone->ManagerUse( pActivator, pCaller, useType, value );
		return;
	}

	m_hActivator = pActivator;
	m_index = 0;
	m_startTime = gpGlobals->time;

	SetUse( NULL );// disable use until all targets have fired

	SetThink ( &CMultiManager::ManagerThink );
	pev->nextthink = gpGlobals->time;
}


/* PS2: multi_kill_manager remove os alvos (por targetname) em vez de dispará-los */
class CMultiKillManager : public CMultiManager
{
public:
	void ManagerFire( void );
	CMultiManager *Clone( void );
	void KillTargets( const char *pszName );
};
LINK_ENTITY_TO_CLASS( multi_kill_manager, CMultiKillManager );


void CMultiKillManager :: ManagerFire ( void )
{
	float	time;

	time = gpGlobals->time - m_startTime;
	while ( m_index < m_cTargets && m_flTargetDelay[ m_index ] <= time )
	{
		KillTargets( STRING( m_iTargetName[ m_index ] ) );
		m_index++;
	}

	if ( m_index >= m_cTargets )// have we fired all targets?
	{
		SetThink( NULL );
		if ( IsClone() )
		{
			UTIL_Remove( this );
			return;
		}
		SetUse ( &CMultiManager::ManagerUse );// allow manager re-use
	}
	else
		pev->nextthink = m_startTime + m_flTargetDelay[ m_index ];
}


CMultiManager *CMultiKillManager::Clone( void )
{
	CMultiKillManager *pMulti = GetClassPtr( (CMultiKillManager *)NULL );

	edict_t *pEdict = pMulti->pev->pContainingEntity;
	memcpy( pMulti->pev, pev, sizeof(*pev) );
	pMulti->pev->pContainingEntity = pEdict;

	pMulti->pev->spawnflags |= SF_MULTIMAN_CLONE;
	pMulti->m_cTargets = m_cTargets;

	memcpy( pMulti->m_iTargetName, m_iTargetName, sizeof( m_iTargetName ) );
	memcpy( pMulti->m_flTargetDelay, m_flTargetDelay, sizeof( m_flTargetDelay ) );

	return pMulti;
}


void CMultiKillManager::KillTargets( const char *pszName )
{
	CBaseEntity *pEntity = NULL;

	while ( (pEntity = UTIL_FindEntityByString( pEntity, "targetname", pszName )) != NULL )
		UTIL_Remove( pEntity );
}


//***********************************************************
//
// Render parameters trigger
//
// This entity will copy its render parameters (renderfx, rendermode, rendercolor, renderamt)
// to its targets when triggered.
//


// Flags to indicate masking off various render parameters that are normally copied to the targets
#define SF_RENDER_MASKFX	(1<<0)
#define SF_RENDER_MASKAMT	(1<<1)
#define SF_RENDER_MASKMODE	(1<<2)
#define SF_RENDER_MASKCOLOR	(1<<3)
#define SF_RENDER_NETNAME	(1<<16)	/* PS2: procura os alvos pelo netname */

class CRenderFxManager : public CBaseEntity
{
public:
	void Spawn( void );
	void Use ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
};

/* sem NON_MATCHING o Use é asm: "env_render" vira um símbolo (D_0037A070)
   definido junto dele, e o registro da entidade usa a mesma string */
#ifdef NON_MATCHING
#define ENV_RENDER_CLASSNAME "env_render"
#else
extern "C" const char D_0037A070[];
#define ENV_RENDER_CLASSNAME D_0037A070
#endif
extern "C" void env_render(entvars_t *pev);
void env_render(entvars_t *pev) { GetClassPtr((CRenderFxManager *)pev); }
static CEntityLink env_render_link(ENV_RENDER_CLASSNAME, env_render);

void CRenderFxManager :: Spawn ( void )
{
	pev->solid = SOLID_NOT;
}

#ifdef NON_MATCHING /* &g_engfuncs fica em s4 em vez de s3 (troca de registradores) */
void CRenderFxManager :: Use ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	edict_t* pentTarget	= NULL;

	if ( FBitSet( pev->spawnflags, SF_RENDER_NETNAME ) )
	{
		if (!FStringNull(pev->netname))
		{
			while ( 1 )
			{
				pentTarget = FIND_ENTITY_BY_STRING(pentTarget, "netname", STRING(pev->netname));
				if (FNullEnt(pentTarget))
					break;

				entvars_t *pevTarget = VARS( pentTarget );
				if ( !strncmp( "env_render", STRING(pevTarget->classname), 10 ) )
					continue;

				if ( !FBitSet( pev->spawnflags, SF_RENDER_MASKFX ) )
					pevTarget->renderfx = pev->renderfx;
				if ( !FBitSet( pev->spawnflags, SF_RENDER_MASKAMT ) )
					pevTarget->renderamt = pev->renderamt;
				if ( !FBitSet( pev->spawnflags, SF_RENDER_MASKMODE ) )
					pevTarget->rendermode = pev->rendermode;
				if ( !FBitSet( pev->spawnflags, SF_RENDER_MASKCOLOR ) )
					pevTarget->rendercolor = pev->rendercolor;
			}
		}
	}
	else if (!FStringNull(pev->target))
	{
		while ( 1 )
		{
			pentTarget = FIND_ENTITY_BY_TARGETNAME(pentTarget, STRING(pev->target));
			if (FNullEnt(pentTarget))
				break;

			entvars_t *pevTarget = VARS( pentTarget );
			if ( !FBitSet( pev->spawnflags, SF_RENDER_MASKFX ) )
				pevTarget->renderfx = pev->renderfx;
			if ( !FBitSet( pev->spawnflags, SF_RENDER_MASKAMT ) )
				pevTarget->renderamt = pev->renderamt;
			if ( !FBitSet( pev->spawnflags, SF_RENDER_MASKMODE ) )
				pevTarget->rendermode = pev->rendermode;
			if ( !FBitSet( pev->spawnflags, SF_RENDER_MASKCOLOR ) )
				pevTarget->rendercolor = pev->rendercolor;
		}
	}
}
#else
INCLUDE_ASM("asm/nonmatchings/dlls/triggers", Use__16CRenderFxManagerP11CBaseEntityT18USE_TYPEf);
__asm__(".section .rodata\n.globl D_0037A070\nD_0037A070:\n.asciz \"env_render\"\n.previous\n");
#endif



class CBaseTrigger : public CBaseToggle
{
public:
	void EXPORT TeleportTouch ( CBaseEntity *pOther );
	void KeyValue( KeyValueData *pkvd );
	void EXPORT MultiTouch( CBaseEntity *pOther );
	void EXPORT HurtTouch ( CBaseEntity *pOther );
	void EXPORT CDAudioTouch ( CBaseEntity *pOther );
	void ActivateMultiTrigger( CBaseEntity *pActivator );
	void EXPORT MultiWaitOver( void );
	void EXPORT CounterUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void EXPORT ToggleUse ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void EXPORT ToggleUseState ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void InitTrigger( void );

	virtual int	ObjectCaps( void ) { return CBaseToggle :: ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }

	int m_iPlayerIndex;		/* 0xD0: PS2 ("player_index") */
};

LINK_ENTITY_TO_CLASS( trigger, CBaseTrigger );

/*
================
InitTrigger
================
*/
void CBaseTrigger::InitTrigger( )
{
	// trigger angles are used for one-way touches.  An angle of 0 is assumed
	// to mean no restrictions, so use a yaw of 360 instead.
	if (pev->angles != g_vecZero)
		SetMovedir(pev);
	pev->solid = SOLID_TRIGGER;
	pev->movetype = MOVETYPE_NONE;
	SET_MODEL(ENT(pev), STRING(pev->model));    // set size and link into world
	if ( CVAR_GET_FLOAT("showtriggers") == 0 )
		SetBits( pev->effects, EF_NODRAW );
}


//
// Cache user-entity-field values until spawn is called.
//

void CBaseTrigger :: KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "damage"))
	{
		pev->dmg = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "count"))
	{
		m_cTriggersLeft = (int) atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "damagetype"))
	{
		m_bitsDamageInflict = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "player_index"))
	{
		m_iPlayerIndex = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseToggle::KeyValue( pkvd );
}

class CTriggerHurt : public CBaseTrigger
{
public:
	void Spawn( void );
	void EXPORT RadiationThink( void );
};

LINK_ENTITY_TO_CLASS( trigger_hurt, CTriggerHurt );

//
// trigger_monsterjump
//
class CTriggerMonsterJump : public CBaseTrigger
{
public:
	void Spawn( void );
	void Touch( CBaseEntity *pOther );
	void Think( void );
};

LINK_ENTITY_TO_CLASS( trigger_monsterjump, CTriggerMonsterJump );


void CTriggerMonsterJump :: Spawn ( void )
{
	SetMovedir ( pev );

	InitTrigger ();

	pev->nextthink = 0;
	pev->speed = 200;
	m_flHeight = 150;

	if ( !FStringNull ( pev->targetname ) )
	{// if targetted, spawn turned off
		pev->solid = SOLID_NOT;
		UTIL_SetOrigin( pev, pev->origin ); // Unlink from trigger list
		SetUse( &CTriggerMonsterJump::ToggleUse );
	}
}


void CTriggerMonsterJump :: Think( void )
{
	pev->solid = SOLID_NOT;// kill the trigger for now !!!UNDONE
	UTIL_SetOrigin( pev, pev->origin ); // Unlink from trigger list
	SetThink( NULL );
}

void CTriggerMonsterJump :: Touch( CBaseEntity *pOther )
{
	entvars_t *pevOther = pOther->pev;

	if ( !FBitSet ( pevOther->flags , FL_MONSTER ) )
	{// touched by a non-monster.
		return;
	}

	pevOther->origin.z += 1;

	if ( FBitSet ( pevOther->flags, FL_ONGROUND ) )
	{// clear the onground so physics don't bitch
		pevOther->flags &= ~FL_ONGROUND;
	}

	// toss the monster!
	pevOther->velocity = pev->movedir * pev->speed;
	pevOther->velocity.z += m_flHeight;
	pev->nextthink = gpGlobals->time;
}


//=====================================
//
// trigger_cdaudio - starts/stops cd audio tracks
//
class CTriggerCDAudio : public CBaseTrigger
{
public:
	void Spawn( void );

	virtual void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void PlayTrack( void );
	void Touch ( CBaseEntity *pOther );
};

LINK_ENTITY_TO_CLASS( trigger_cdaudio, CTriggerCDAudio );

//
// Changes tracks or stops CD when player touches
//
// !!!HACK - overloaded HEALTH to avoid adding new field
void CTriggerCDAudio :: Touch ( CBaseEntity *pOther )
{
	if ( !pOther->IsPlayer() )
	{// only clients may trigger these events
		return;
	}

	PlayTrack();
}

void CTriggerCDAudio :: Spawn( void )
{
	InitTrigger();
}

void CTriggerCDAudio::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	PlayTrack();
}

void PlayCDTrack( int iTrack )
{
	edict_t *pClient;

	// manually find the single player.
	pClient = g_engfuncs.pfnPEntityOfEntIndex( 1 );

	// Can't play if the client is not connected!
	if ( !pClient )
		return;

	if ( iTrack < -1 || iTrack > 30 )
	{
		ALERT ( at_console, "TriggerCDAudio - Track %d out of range\n" );
		return;
	}

	if ( iTrack == -1 )
	{
		CLIENT_COMMAND ( pClient, "cd pause\n");
	}
	else
	{
		char string [ 64 ];

		sprintf( string, "cd play %3d\n", iTrack );
		CLIENT_COMMAND ( pClient, string);
	}
}


// only plays for ONE client, so only use in single play!
void CTriggerCDAudio :: PlayTrack( void )
{
	PlayCDTrack( (int)pev->health );

	SetTouch( NULL );
	UTIL_Remove( this );
}


// This plays a CD track when fired or when the player enters it's radius
class CTargetCDAudio : public CPointEntity
{
public:
	void			Spawn( void );
	void			KeyValue( KeyValueData *pkvd );

	virtual void	Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void			Think( void );
	void			Play( void );
};

LINK_ENTITY_TO_CLASS( target_cdaudio, CTargetCDAudio );

void CTargetCDAudio :: KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "radius"))
	{
		pev->scale = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CPointEntity::KeyValue( pkvd );
}

void CTargetCDAudio :: Spawn( void )
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;

	if ( pev->scale > 0 )
		pev->nextthink = gpGlobals->time + 1.0f;
}

void CTargetCDAudio::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	Play();
}

// only plays for ONE client, so only use in single play!
void CTargetCDAudio::Think( void )
{
	edict_t *pClient;

	// manually find the single player.
	pClient = g_engfuncs.pfnPEntityOfEntIndex( 1 );

	// Can't play if the client is not connected!
	if ( !pClient )
		return;

	pev->nextthink = gpGlobals->time + 0.5f;

	if ( (pClient->v.origin - pev->origin).Length() <= pev->scale )
		Play();

}

void CTargetCDAudio::Play( void )
{
	PlayCDTrack( (int)pev->health );
	UTIL_Remove(this);
}

//=====================================

//
// trigger_hurt - hurts anything that touches it. if the trigger has a targetname, firing it will toggle state
//
//int gfToggleState = 0; // used to determine when all radiation trigger hurts have called 'RadiationThink'

void CTriggerHurt :: Spawn( void )
{
	InitTrigger();
	SetTouch ( &CTriggerHurt::HurtTouch );

	if ( !FStringNull ( pev->targetname ) )
	{
		SetUse ( &CTriggerHurt::ToggleUse );
	}
	else
	{
		SetUse ( NULL );
	}

	if (m_bitsDamageInflict & DMG_RADIATION)
	{
		SetThink ( &CTriggerHurt::RadiationThink );
		pev->nextthink = gpGlobals->time + RANDOM_FLOAT(0.0f, 0.5f);
	}

	if ( FBitSet (pev->spawnflags, SF_TRIGGER_HURT_START_OFF) )// if flagged to Start Turned Off, make trigger nonsolid.
		pev->solid = SOLID_NOT;

	UTIL_SetOrigin( pev, pev->origin );		// Link into the list
}

// trigger hurt that causes radiation will do a radius
// check and set the player's geiger counter level
// according to distance from center of trigger

void CTriggerHurt :: RadiationThink( void )
{

	edict_t *pentPlayer;
	CBasePlayer *pPlayer = NULL;
	float flRange;
	entvars_t *pevTarget;
	Vector vecSpot1;
	Vector vecSpot2;
	Vector vecRange;
	Vector origin;
	Vector view_ofs;

	// check to see if a player is in pvs
	// if not, continue

	// set origin to center of trigger so that this check works
	origin = pev->origin;
	view_ofs = pev->view_ofs;

	pev->origin = (pev->absmin + pev->absmax) * 0.5f;
	pev->view_ofs = pev->view_ofs * 0.0f;

	pentPlayer = FIND_CLIENT_IN_PVS(edict());

	pev->origin = origin;
	pev->view_ofs = view_ofs;

	// reset origin

	if (!FNullEnt(pentPlayer))
	{

		pPlayer = GetClassPtr( (CBasePlayer *)VARS(pentPlayer));

		pevTarget = VARS(pentPlayer);

		// get range to player;

		vecSpot1 = (pev->absmin + pev->absmax) * 0.5f;
		vecSpot2 = (pevTarget->absmin + pevTarget->absmax) * 0.5f;

		vecRange = vecSpot1 - vecSpot2;
		flRange = vecRange.Length();

		// if player's current geiger counter range is larger
		// than range to this trigger hurt, reset player's
		// geiger counter range

		if (pPlayer->m_flgeigerRange >= flRange)
			pPlayer->m_flgeigerRange = flRange;
	}

	pev->nextthink = gpGlobals->time + 0.25f;
}

//
// ToggleUse - If this is the USE function for a trigger, its state will toggle every time it's fired
//
void CBaseTrigger :: ToggleUse ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if (pev->solid == SOLID_NOT)
	{// if the trigger is off, turn it on
		pev->solid = SOLID_TRIGGER;

		// Force retouch
		gpGlobals->force_retouch++;
	}
	else
	{// turn the trigger off
		pev->solid = SOLID_NOT;
	}
	UTIL_SetOrigin( pev, pev->origin );
}

/* PS2: como o ToggleUse, mas respeita USE_ON/USE_SET/USE_OFF */
void CBaseTrigger :: ToggleUseState ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( useType == USE_ON || useType == USE_SET )
	{
		pev->solid = SOLID_TRIGGER;
	}
	else if ( useType == USE_OFF )
	{
		pev->solid = SOLID_NOT;
	}
	else if (pev->solid == SOLID_NOT)
	{// if the trigger is off, turn it on
		pev->solid = SOLID_TRIGGER;

		// Force retouch
		gpGlobals->force_retouch++;
	}
	else
	{// turn the trigger off
		pev->solid = SOLID_NOT;
	}
	UTIL_SetOrigin( pev, pev->origin );
}

// When touched, a hurt trigger does DMG points of damage each half-second
void CBaseTrigger :: HurtTouch ( CBaseEntity *pOther )
{
	float fldmg;

	if ( !pOther->pev->takedamage )
		return;

	if ( (pev->spawnflags & SF_TRIGGER_HURT_CLIENTONLYTOUCH) && !pOther->IsPlayer() )
	{
		// this trigger is only allowed to touch clients, and this ain't a client.
		return;
	}

	if ( (pev->spawnflags & SF_TRIGGER_HURT_NO_CLIENTS) && pOther->IsPlayer() )
		return;

	// HACKHACK -- In multiplayer, players touch this based on packet receipt.
	// So the players who send packets later aren't always hurt.  Keep track of
	// how much time has passed and whether or not you've touched that player
	if ( g_pGameRules->Ps2Virt07() )
	{
		if ( pev->dmgtime > gpGlobals->time )
		{
			if ( gpGlobals->time != pev->pain_finished )
			{// too early to hurt again, and not same frame with a different entity
				if ( pOther->IsPlayer() )
				{
					int playerMask = 1 << (pOther->entindex() - 1);

					// If I've already touched this player (this time), then bail out
					if ( pev->impulse & playerMask )
						return;

					// Mark this player as touched
					// BUGBUG - There can be only 32 players!
					pev->impulse |= playerMask;
				}
				else
				{
					return;
				}
			}
		}
		else
		{
			// New clock, "un-touch" all players
			pev->impulse = 0;
			if ( pOther->IsPlayer() )
			{
				int playerMask = 1 << (pOther->entindex() - 1);

				// Mark this player as touched
				// BUGBUG - There can be only 32 players!
				pev->impulse |= playerMask;
			}
		}
	}
	else	// Original code -- single player
	{
		if ( pev->dmgtime > gpGlobals->time && gpGlobals->time != pev->pain_finished )
		{// too early to hurt again, and not same frame with a different entity
			return;
		}
	}



	// If this is time_based damage (poison, radiation), override the pev->dmg with a
	// default for the given damage type.  Monsters only take time-based damage
	// while touching the trigger.  Player continues taking damage for a while after
	// leaving the trigger

	fldmg = pev->dmg * 0.5f;	// 0.5 seconds worth of damage, pev->dmg is damage/second

	if ( fldmg < 0 )
		pOther->TakeHealth( -fldmg, m_bitsDamageInflict );
	else
		pOther->TakeDamage( pev, pev, fldmg, m_bitsDamageInflict );

	// Store pain time so we can get all of the other entities on this frame
	pev->pain_finished = gpGlobals->time;

	// Apply damage every half second
	pev->dmgtime = gpGlobals->time + 0.5f;// half second delay until this trigger can hurt toucher again



	if ( pev->target )
	{
		// trigger has a target it wants to fire.
		if ( pev->spawnflags & SF_TRIGGER_HURT_CLIENTONLYFIRE )
		{
			// if the toucher isn't a client, don't fire the target!
			if ( !pOther->IsPlayer() )
			{
				return;
			}
		}

		SUB_UseTargets( pOther, USE_TOGGLE, 0 );
		if ( pev->spawnflags & SF_TRIGGER_HURT_TARGETONCE )
			pev->target = 0;
	}
}


/*QUAKED trigger_multiple (.5 .5 .5) ? notouch
Variable sized repeatable trigger.  Must be targeted at one or more entities.
If "health" is set, the trigger must be killed to activate each time.
If "delay" is set, the trigger waits some time after activating before firing.
"wait" : Seconds between triggerings. (.2 default)
If notouch is set, the trigger is only fired by other entities, not by touching.
NOTOUCH has been obsoleted by trigger_relay!
*/

#define SF_TRIGGER_ALLOWMONSTERS	1// monsters allowed to fire this trigger
#define SF_TRIGGER_NOCLIENTS		2// players not allowed to fire this trigger
#define SF_TRIGGER_PUSHABLES		4// only pushables can fire this trigger
#define SF_TRIGGER_PLAYERINDEX		0x100	/* PS2: só o jogador do player_index (ou os dois jogadores juntos) */
#define SF_TRIGGER_ALLPLAYERS		0x200	/* PS2: os dois jogadores dentro do gatilho */

#define SPAWNFLAG_NOMESSAGE	1

class CTriggerMultiple : public CBaseTrigger
{
public:
	void Spawn( void );
};

LINK_ENTITY_TO_CLASS( trigger_multiple, CTriggerMultiple );


void CTriggerMultiple :: Spawn( void )
{
	if (m_flWait == 0)
		m_flWait = 0.2f;

	InitTrigger();

	SetTouch( &CTriggerMultiple::MultiTouch );
}


/*QUAKED trigger_once (.5 .5 .5) ? notouch
Variable sized trigger. Triggers once, then removes itself.  You must set the key "target" to the name of another object in the level that has a matching
"targetname".  If "health" is set, the trigger must be killed to activate.
If notouch is set, the trigger is only fired by other entities, not by touching.
if "killtarget" is set, any objects that have a matching "target" will be removed when the trigger is fired.
if "angle" is set, the trigger will only fire when someone is facing the direction of the angle.  Use "360" for an angle of 0.
*/
class CTriggerOnce : public CTriggerMultiple
{
public:
	void Spawn( void );
};

LINK_ENTITY_TO_CLASS( trigger_once, CTriggerOnce );
void CTriggerOnce::Spawn( void )
{
	m_flWait = -1;

	CTriggerMultiple :: Spawn();
}



void CBaseTrigger :: MultiTouch( CBaseEntity *pOther )
{
	entvars_t	*pevToucher;
	CBaseEntity *pList[2];

	pevToucher = pOther->pev;

	if ( pev->spawnflags & SF_TRIGGER_ALLPLAYERS )
	{
		if ( !pOther->IsPlayer() )
			return;
		if ( UTIL_EntitiesInBox( pList, 2, pev->mins, pev->maxs, FL_CLIENT ) >= 2 )
			return;
		if ( g_pGameRules->Ps2Virt64( m_iPlayerIndex ) != pOther )
			return;
		ActivateMultiTrigger( pOther );
	}
	else if ( pev->spawnflags & SF_TRIGGER_PLAYERINDEX )
	{
		if ( !pOther->IsPlayer() )
			return;
		if ( m_iPlayerIndex == 0 )
		{
			if ( UTIL_EntitiesInBox( pList, 2, pev->mins, pev->maxs, FL_CLIENT ) != 2 )
				return;
		}
		else if ( g_pGameRules->Ps2Virt64( m_iPlayerIndex ) != pOther )
			return;
		ActivateMultiTrigger( pOther );
	}
	// Only touch clients, monsters, or pushables (depending on flags)
	else if ( ((pevToucher->flags & FL_CLIENT) && !(pev->spawnflags & SF_TRIGGER_NOCLIENTS)) ||
		 ((pevToucher->flags & FL_MONSTER) && (pev->spawnflags & SF_TRIGGER_ALLOWMONSTERS)) ||
		 (pev->spawnflags & SF_TRIGGER_PUSHABLES) && FClassnameIs(pevToucher,"func_pushable") )
	{
		ActivateMultiTrigger( pOther );
	}
}


//
// the trigger was just touched/killed/used
// self.enemy should be set to the activator so it can be held through a delay
// so wait for the delay time before firing
//
void CBaseTrigger :: ActivateMultiTrigger( CBaseEntity *pActivator )
{
	if (pev->nextthink > gpGlobals->time)
		return;         // still waiting for reset time

	if ( !pActivator )
		ALERT( at_warning, "Activate Multi Trigger activator is NULL!" );
	else if (!UTIL_IsMasterTriggered(m_sMaster,pActivator))
		return;

	if (FClassnameIs(pev, "trigger_secret"))
	{
		if ( pev->enemy == NULL || !FClassnameIs(pev->enemy, "player"))
			return;
		gpGlobals->found_secrets++;
	}

	if (!FStringNull(pev->noise))
		EMIT_SOUND(ENT(pev), CHAN_VOICE, (char*)STRING(pev->noise), 1, ATTN_NORM);

	// don't trigger again until reset
	// pev->takedamage = DAMAGE_NO;

	if ( pActivator )
	{
		m_hActivator = pActivator;
		SUB_UseTargets( m_hActivator, USE_TOGGLE, 0 );
	}

	if ( pev->message && pActivator && pActivator->IsPlayer() )
	{
		UTIL_ShowMessage( STRING(pev->message), pActivator );
	}

	if (m_flWait > 0)
	{
		SetThink( &CBaseTrigger::MultiWaitOver );
		pev->nextthink = gpGlobals->time + m_flWait;
	}
	else
	{
		// we can't just remove (self) here, because this is a touch function
		// called while C code is looping through area links...
		SetTouch( NULL );
		pev->nextthink = gpGlobals->time + 0.1f;
		SetThink(  &CBaseTrigger::SUB_Remove );
	}
}


// the wait time has passed, so set back up for another activation
void CBaseTrigger :: MultiWaitOver( void )
{
	SetThink( NULL );
}


// ========================= COUNTING TRIGGER =====================================

//
// GLOBALS ASSUMED SET:  g_eoActivator
//
void CBaseTrigger::CounterUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	m_cTriggersLeft--;
	m_hActivator = pActivator;

	if (m_cTriggersLeft < 0)
		return;

	BOOL fTellActivator = FALSE;

	if ( pActivator )
		fTellActivator = FClassnameIs(m_hActivator->pev, "player") && !FBitSet(pev->spawnflags, SPAWNFLAG_NOMESSAGE);
	else
		ALERT( at_warning, "Activator for Trigger Counter is NULL!" );

	if (m_cTriggersLeft != 0)
	{
		if (fTellActivator)
		{
			// UNDONE: I don't think we want these Quakesque messages
			switch (m_cTriggersLeft)
			{
			case 1:		ALERT(at_console, "Only 1 more to go...");		break;
			case 2:		ALERT(at_console, "Only 2 more to go...");		break;
			case 3:		ALERT(at_console, "Only 3 more to go...");		break;
			default:	ALERT(at_console, "There are more to go...");	break;
			}
		}
		return;
	}

	// !!!UNDONE: I don't think we want these Quakesque messages
	if (fTellActivator)
		ALERT(at_console, "Sequence completed!");

	ActivateMultiTrigger( m_hActivator );
}


/*QUAKED trigger_counter (.5 .5 .5) ? nomessage
Acts as an intermediary for an action that takes multiple inputs.
If nomessage is not set, it will print "1 more.. " etc when triggered and
"sequence complete" when finished.  After the counter has been triggered "cTriggersLeft"
times (default 2), it will fire all of it's targets and remove itself.
*/
class CTriggerCounter : public CBaseTrigger
{
public:
	void Spawn( void );
};
LINK_ENTITY_TO_CLASS( trigger_counter, CTriggerCounter );

void CTriggerCounter :: Spawn( void )
{
	// By making the flWait be -1, this counter-trigger will disappear after it's activated
	// (but of course it needs cTriggersLeft "uses" before that happens).
	m_flWait = -1;

	if (m_cTriggersLeft == 0)
		m_cTriggersLeft = 2;
	SetUse( &CTriggerCounter::CounterUse );
}

// ====================== TRIGGER_CHANGELEVEL ================================

class CTriggerVolume : public CPointEntity	// Derive from point entity so this doesn't move across levels
{
public:
	void		Spawn( void );
};

LINK_ENTITY_TO_CLASS( trigger_transition, CTriggerVolume );

// Define space that travels across a level transition
void CTriggerVolume :: Spawn( void )
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	SET_MODEL(ENT(pev), STRING(pev->model));    // set size and link into world
	pev->model = NULL;
	pev->modelindex = 0;
}


// Fires a target after level transition and then dies
class CFireAndDie : public CBaseDelay
{
public:
	void Spawn( void );
	void Precache( void );
	void Think( void );
	int ObjectCaps( void ) { return CBaseDelay::ObjectCaps() | FCAP_FORCE_TRANSITION; }	// Always go across transitions
};
LINK_ENTITY_TO_CLASS( fireanddie, CFireAndDie );

void CFireAndDie::Spawn( void )
{
	pev->classname = MAKE_STRING("fireanddie");
	// Don't call Precache() - it should be called on restore
}


void CFireAndDie::Precache( void )
{
	// This gets called on restore
	pev->nextthink = gpGlobals->time + m_flDelay;
}


void CFireAndDie::Think( void )
{
	SUB_UseTargets( this, USE_TOGGLE, 0 );
	UTIL_Remove( this );
}


#define SF_CHANGELEVEL_USEONLY		0x0002
#define SF_CHANGELEVEL_NOLANDMARK	0x0004	/* PS2: troca de mapa sem landmark (comando "map") */

extern void DispatchSpawn( edict_t *pent );

class CChangeLevel : public CBaseTrigger
{
public:
	void Spawn( void );
	void KeyValue( KeyValueData *pkvd );
	void EXPORT UseChangeLevel ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void EXPORT TriggerChangeLevel( void );
	void EXPORT ExecuteChangeLevel( void );
	void EXPORT TouchChangeLevel( CBaseEntity *pOther );
	void ChangeLevelNow( CBaseEntity *pActivator );

	static edict_t *FindLandmark( const char *pLandmarkName );
	static int ChangeList( LEVELLIST *pLevelList, int maxList );
	static int AddTransitionToList( LEVELLIST *pLevelList, int listCount, const char *pMapName, const char *pLandmarkName, edict_t *pentLandmark );
	static int InTransitionVolume( CBaseEntity *pEntity, char *pVolumeName );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );

	static	TYPEDESCRIPTION m_SaveData[];

	char m_szMapName[cchMapNameMost];		// trigger_changelevel only:  next map
	char m_szLandmarkName[cchMapNameMost];		// trigger_changelevel only:  landmark on next map
	int		m_changeTarget;
	float	m_changeTargetDelay;
};
LINK_ENTITY_TO_CLASS( trigger_changelevel, CChangeLevel );

// Global Savedata for changelevel trigger
TYPEDESCRIPTION	CChangeLevel::m_SaveData[] =
{
	DEFINE_ARRAY( CChangeLevel, m_szMapName, FIELD_CHARACTER, cchMapNameMost ),
	DEFINE_ARRAY( CChangeLevel, m_szLandmarkName, FIELD_CHARACTER, cchMapNameMost ),
	DEFINE_FIELD( CChangeLevel, m_changeTarget, FIELD_STRING ),
	DEFINE_FIELD( CChangeLevel, m_changeTargetDelay, FIELD_FLOAT ),
};

IMPLEMENT_SAVERESTORE(CChangeLevel,CBaseTrigger);

//
// Cache user-entity-field values until spawn is called.
//

void CChangeLevel :: KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "map"))
	{
		if (strlen(pkvd->szValue) >= cchMapNameMost)
			ALERT( at_error, "Map name '%s' too long (32 chars)\n", pkvd->szValue );
		strcpy(m_szMapName, pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "landmark"))
	{
		if (strlen(pkvd->szValue) >= cchMapNameMost)
			ALERT( at_error, "Landmark name '%s' too long (32 chars)\n", pkvd->szValue );
		strcpy(m_szLandmarkName, pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "changetarget"))
	{
		m_changeTarget = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "changedelay"))
	{
		m_changeTargetDelay = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseTrigger::KeyValue( pkvd );
}


/*QUAKED trigger_changelevel (0.5 0.5 0.5) ? NO_INTERMISSION
When the player touches this, he gets sent to the map listed in the "map" variable.  Unless the NO_INTERMISSION flag is set, the view will go to the info_intermission spot and display stats.
*/

void CChangeLevel :: Spawn( void )
{
	if ( FStrEq( m_szMapName, "" ) )
		ALERT( at_console, "a trigger_changelevel doesn't have a map" );

	if ( FStrEq( m_szLandmarkName, "" ) )
		ALERT( at_console, "trigger_changelevel to %s doesn't have a landmark", m_szMapName );

	if (!FStringNull ( pev->targetname ) )
	{
		SetUse ( &CChangeLevel::UseChangeLevel );
	}
	InitTrigger();
	if ( !(pev->spawnflags & SF_CHANGELEVEL_USEONLY) )
		SetTouch( &CChangeLevel::TouchChangeLevel );
}


void CChangeLevel :: ExecuteChangeLevel( void )
{
	MESSAGE_BEGIN( MSG_ALL, SVC_CDTRACK );
		WRITE_BYTE( 3 );
		WRITE_BYTE( 3 );
	MESSAGE_END();

	MESSAGE_BEGIN(MSG_ALL, SVC_INTERMISSION);
	MESSAGE_END();
}


/* ficam no .bss em asm */
extern "C" char D_003A41C0[cchMapNameMost];
extern "C" char D_003A41E0[cchMapNameMost];
#define st_szNextMap D_003A41C0
#define st_szNextSpot D_003A41E0

edict_t *CChangeLevel :: FindLandmark( const char *pLandmarkName )
{
	edict_t	*pentLandmark;

	pentLandmark = FIND_ENTITY_BY_STRING( NULL, "targetname", pLandmarkName );
	while ( !FNullEnt( pentLandmark ) )
	{
		// Found the landmark
		if ( FClassnameIs( pentLandmark, "info_landmark" ) )
			return pentLandmark;
		else
			pentLandmark = FIND_ENTITY_BY_STRING( pentLandmark, "targetname", pLandmarkName );
	}
	ALERT( at_error, "Can't find landmark %s\n", pLandmarkName );
	return NULL;
}


//=========================================================
// CChangeLevel :: Use - allows level transitions to be
// triggered by buttons, etc.
//
//=========================================================
void CChangeLevel :: UseChangeLevel ( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	ChangeLevelNow( pActivator );
}

void CChangeLevel :: ChangeLevelNow( CBaseEntity *pActivator )
{
	edict_t	*pentLandmark;

	// Don't work in deathmatch
	if ( g_pGameRules->Ps2Virt08() )
		return;

	// Some people are firing these multiple times in a frame, disable
	if ( gpGlobals->time == pev->dmgtime )
		return;

	pev->dmgtime = gpGlobals->time;


	CBaseEntity *pPlayer = CBaseEntity::Instance( g_engfuncs.pfnPEntityOfEntIndex( 1 ) );
	if ( !(pev->spawnflags & SF_CHANGELEVEL_NOLANDMARK) )
	{
		if ( !InTransitionVolume( pPlayer, m_szLandmarkName ) )
		{
			ALERT( at_aiconsole, "Player isn't in the transition volume %s, aborting\n", m_szLandmarkName );
			return;
		}

		pPlayer->Ps2Unk58();

		// Create an entity to fire the changetarget
		if ( m_changeTarget )
		{
			CFireAndDie *pFireAndDie = GetClassPtr( (CFireAndDie *)NULL );
			if ( pFireAndDie )
			{
				// Set target and delay
				pFireAndDie->pev->target = m_changeTarget;
				pFireAndDie->m_flDelay = m_changeTargetDelay;
				pFireAndDie->pev->origin = pPlayer->pev->origin;
				// Call spawn
				DispatchSpawn( pFireAndDie->edict() );
			}
		}
	}
	// This object will get removed in the call to CHANGE_LEVEL, copy the params into "safe" memory
	strcpy(st_szNextMap, m_szMapName);

	m_hActivator = pActivator;
	SUB_UseTargets( pActivator, USE_TOGGLE, 0 );
	st_szNextSpot[0] = 0;	// Init landmark to NULL

	// look for a landmark entity
	if ( !(pev->spawnflags & SF_CHANGELEVEL_NOLANDMARK) )
	{
		pentLandmark = FindLandmark( m_szLandmarkName );
		if ( !FNullEnt( pentLandmark ) )
		{
			strcpy(st_szNextSpot, m_szLandmarkName);
			gpGlobals->vecLandmarkOffset = VARS(pentLandmark)->origin;
		}
	}
	ALERT( at_console, "CHANGE LEVEL: %s %s\n", st_szNextMap, st_szNextSpot );
	if ( pev->spawnflags & SF_CHANGELEVEL_NOLANDMARK )
	{
		char szCommand[32];

		sprintf( szCommand, "map %s\n", st_szNextMap );
		SERVER_COMMAND( szCommand );
	}
	else
		CHANGE_LEVEL( st_szNextMap, st_szNextSpot );
}

//
// GLOBALS ASSUMED SET:  st_szNextMap
//
void CChangeLevel :: TouchChangeLevel( CBaseEntity *pOther )
{
	if (!FClassnameIs(pOther->pev, "player"))
		return;

	ChangeLevelNow( pOther );
}


// Add a transition to the list, but ignore duplicates
// (a designer may have placed multiple trigger_changelevels with the same landmark)
int CChangeLevel::AddTransitionToList( LEVELLIST *pLevelList, int listCount, const char *pMapName, const char *pLandmarkName, edict_t *pentLandmark )
{
	int i;

	if ( !pLevelList || !pMapName || !pLandmarkName || !pentLandmark )
		return 0;

	for ( i = 0; i < listCount; i++ )
	{
		if ( pLevelList[i].pentLandmark == pentLandmark && strcmp( pLevelList[i].mapName, pMapName ) == 0 )
			return 0;
	}
	strcpy( pLevelList[listCount].mapName, pMapName );
	strcpy( pLevelList[listCount].landmarkName, pLandmarkName );
	pLevelList[listCount].pentLandmark = pentLandmark;
	pLevelList[listCount].vecLandmarkOrigin = VARS(pentLandmark)->origin;

	return 1;
}

int BuildChangeList( LEVELLIST *pLevelList, int maxList )
{
	return CChangeLevel::ChangeList( pLevelList, maxList );
}


int CChangeLevel::InTransitionVolume( CBaseEntity *pEntity, char *pVolumeName )
{
	edict_t	*pentVolume;


	if ( pEntity->ObjectCaps() & FCAP_FORCE_TRANSITION )
		return 1;

	// If you're following another entity, follow it through the transition (weapons follow the player)
	if ( pEntity->pev->movetype == MOVETYPE_FOLLOW )
	{
		if ( pEntity->pev->aiment != NULL )
			pEntity = CBaseEntity::Instance( pEntity->pev->aiment );
	}

	int inVolume = 1;	// Unless we find a trigger_transition, everything is in the volume

	pentVolume = FIND_ENTITY_BY_TARGETNAME( NULL, pVolumeName );
	while ( !FNullEnt( pentVolume ) )
	{
		CBaseEntity *pVolume = CBaseEntity::Instance( pentVolume );

		if ( pVolume && FClassnameIs( pVolume->pev, "trigger_transition" ) )
		{
			if ( pVolume->Intersects( pEntity ) )	// It touches one, it's in the volume
				return 1;
			else
				inVolume = 0;	// Found a trigger_transition, but I don't intersect it -- if I don't find another, don't go!
		}
		pentVolume = FIND_ENTITY_BY_TARGETNAME( pentVolume, pVolumeName );
	}

	return inVolume;
}


// We can only ever move 512 entities across a transition
#define MAX_ENTITY 512

// This has grown into a complicated beast
// Can we make this more elegant?
// This builds the list of all transitions on this level and which entities are in their PVS's and can / should
// be moved across.
int CChangeLevel::ChangeList( LEVELLIST *pLevelList, int maxList )
{
	edict_t	*pentChangelevel, *pentLandmark;
	int			i, count;

	count = 0;

	// Find all of the possible level changes on this BSP
	pentChangelevel = FIND_ENTITY_BY_STRING( NULL, "classname", "trigger_changelevel" );
	if ( FNullEnt( pentChangelevel ) )
		return 0;
	while ( !FNullEnt( pentChangelevel ) )
	{
		CChangeLevel *pTrigger;

		pTrigger = GetClassPtr((CChangeLevel *)VARS(pentChangelevel));
		if ( pTrigger )
		{
			// Find the corresponding landmark
			pentLandmark = FindLandmark( pTrigger->m_szLandmarkName );
			if ( pentLandmark )
			{
				// Build a list of unique transitions
				if ( AddTransitionToList( pLevelList, count, pTrigger->m_szMapName, pTrigger->m_szLandmarkName, pentLandmark ) )
				{
					count++;
					if ( count >= maxList )		// FULL!!
						break;
				}
			}
		}
		pentChangelevel = FIND_ENTITY_BY_STRING( pentChangelevel, "classname", "trigger_changelevel" );
	}

	if ( gpGlobals->pSaveData && ((SAVERESTOREDATA *)gpGlobals->pSaveData)->pTable )
	{
		CSave saveHelper( (SAVERESTOREDATA *)gpGlobals->pSaveData );

		for ( i = 0; i < count; i++ )
		{
			int j, entityCount = 0;
			CBaseEntity *pEntList[ MAX_ENTITY ];
			int			 entityFlags[ MAX_ENTITY ];

			// Follow the linked list of entities in the PVS of the transition landmark
			edict_t *pent = UTIL_EntitiesInPVS( pLevelList[i].pentLandmark );

			// Build a list of valid entities in this linked list (we're going to use pent->v.chain again)
			while ( !FNullEnt( pent ) )
			{
				CBaseEntity *pEntity = CBaseEntity::Instance(pent);
				if ( pEntity )
				{
					int caps = pEntity->ObjectCaps();
					if ( !(caps & FCAP_DONT_SAVE) )
					{
						int flags = 0;

						// If this entity can be moved or is global, mark it
						if ( caps & FCAP_ACROSS_TRANSITION )
							flags |= FENTTABLE_MOVEABLE;
						if ( pEntity->pev->globalname && !pEntity->IsDormant() )
							flags |= FENTTABLE_GLOBAL;
						if ( flags )
						{
							pEntList[ entityCount ] = pEntity;
							entityFlags[ entityCount ] = flags;
							entityCount++;
							if ( entityCount > MAX_ENTITY )
								ALERT( at_error, "Too many entities across a transition!" );
						}
					}
				}
				pent = pent->v.chain;
			}

			for ( j = 0; j < entityCount; j++ )
			{
				// Check to make sure the entity isn't screened out by a trigger_transition
				if ( entityFlags[j] && InTransitionVolume( pEntList[j], pLevelList[i].landmarkName ) )
				{
					// Mark entity table with 1<<i
					int index = saveHelper.EntityIndex( pEntList[j] );
					// Flag it with the level number
					saveHelper.EntityFlagsSet( index, entityFlags[j] | (1<<i) );
				}
			}
		}
	}

	return count;
}

/*
go to the next level for deathmatch
only called if a time or frag limit has expired
*/
void NextLevel( void )
{
	edict_t* pent;
	CChangeLevel *pChange;

	// find a trigger_changelevel
	pent = FIND_ENTITY_BY_CLASSNAME(NULL, "trigger_changelevel");

	// go back to start if no trigger_changelevel
	if (FNullEnt(pent))
	{
		gpGlobals->mapname = ALLOC_STRING("start");
		pChange = GetClassPtr( (CChangeLevel *)NULL );
		strcpy(pChange->m_szMapName, "start");
	}
	else
		pChange = GetClassPtr( (CChangeLevel *)VARS(pent));

	strcpy(st_szNextMap, pChange->m_szMapName);
	g_fGameOver = TRUE;

	if (pChange->pev->nextthink < gpGlobals->time)
	{
		pChange->SetThink( &CChangeLevel::ExecuteChangeLevel );
		pChange->pev->nextthink = gpGlobals->time + 0.1f;
	}
}


#define CONTENTS_LADDER		-16
#define FL_BASEVELOCITY		(1<<22)
#define SF_TRIG_PUSH_ONCE	1

class CLadder : public CBaseTrigger
{
public:
	void KeyValue( KeyValueData *pkvd );
	void Spawn( void );
	void Precache( void );
};
LINK_ENTITY_TO_CLASS( func_ladder, CLadder );


void CLadder :: KeyValue( KeyValueData *pkvd )
{
	CBaseTrigger::KeyValue( pkvd );
}


//=========================================================
// func_ladder - makes an area vertically negotiable
//=========================================================
void CLadder :: Precache( void )
{
	// Do all of this in here because we need to 'convert' old saved games
	pev->solid = SOLID_NOT;
	pev->skin = CONTENTS_LADDER;
	if ( CVAR_GET_FLOAT("showtriggers") == 0 )
	{
		pev->rendermode = kRenderTransTexture;
		pev->renderamt = 0;
	}
	pev->effects &= ~EF_NODRAW;
}


void CLadder :: Spawn( void )
{
	Precache();

	SET_MODEL(ENT(pev), STRING(pev->model));    // set size and link into world
	pev->movetype = MOVETYPE_PUSH;
}


// ========================== A TRIGGER THAT PUSHES YOU ===============================

class CTriggerPush : public CBaseTrigger
{
public:
	void Spawn( void );
	void KeyValue( KeyValueData *pkvd );
	void Touch( CBaseEntity *pOther );
};
LINK_ENTITY_TO_CLASS( trigger_push, CTriggerPush );


void CTriggerPush :: KeyValue( KeyValueData *pkvd )
{
	CBaseTrigger::KeyValue( pkvd );
}


/*QUAKED trigger_push (.5 .5 .5) ? TRIG_PUSH_ONCE
Pushes the player
*/

void CTriggerPush :: Spawn( )
{
	if ( pev->angles == g_vecZero )
		pev->angles.y = 360;
	InitTrigger();

	if (pev->speed == 0)
		pev->speed = 100;

	if ( FBitSet (pev->spawnflags, SF_TRIGGER_PUSH_START_OFF) )// if flagged to Start Turned Off, make trigger nonsolid.
		pev->solid = SOLID_NOT;

	SetUse( &CTriggerPush::ToggleUse );

	UTIL_SetOrigin( pev, pev->origin );		// Link into the list
}


void CTriggerPush :: Touch( CBaseEntity *pOther )
{
	entvars_t* pevToucher = pOther->pev;

	// UNDONE: Is there a better way than health to detect things that have physics? (clients/monsters)
	switch( pevToucher->movetype )
	{
	case MOVETYPE_NONE:
	case MOVETYPE_PUSH:
	case MOVETYPE_NOCLIP:
	case MOVETYPE_FOLLOW:
		return;
	}

	if ( pevToucher->solid != SOLID_NOT && pevToucher->solid != SOLID_BSP )
	{
		// Instant trigger, just transfer velocity and remove
		if (FBitSet(pev->spawnflags, SF_TRIG_PUSH_ONCE))
		{
			pevToucher->velocity = pevToucher->velocity + (pev->speed * pev->movedir);
			if ( pevToucher->velocity.z > 0 )
				pevToucher->flags &= ~FL_ONGROUND;
			UTIL_Remove( this );
		}
		else
		{	// Push field, transfer to base velocity
			Vector vecPush = (pev->speed * pev->movedir);
			if ( pevToucher->flags & FL_BASEVELOCITY )
				vecPush = vecPush +  pevToucher->basevelocity;

			pevToucher->basevelocity = vecPush;

			pevToucher->flags |= FL_BASEVELOCITY;
//			ALERT( at_console, "Vel %f, base %f\n", pevToucher->velocity.z, pevToucher->basevelocity.z );
		}
	}
}


//======================================
// teleport trigger
//
//

void CBaseTrigger :: TeleportTouch( CBaseEntity *pOther )
{
	entvars_t* pevToucher;
	edict_t	*pentTarget = NULL;

	if ( !pOther )
		return;

	if ( pev->spawnflags & SF_TRIGGER_PLAYERINDEX )
	{
		if ( pOther->IsPlayer() && m_iPlayerIndex )
		{
			if ( g_pGameRules->Ps2Virt64( m_iPlayerIndex ) != pOther )
				return;
		}
	}

	pevToucher = pOther->pev;

	// Only teleport monsters or clients
	if ( !FBitSet( pevToucher->flags, FL_CLIENT|FL_MONSTER ) )
		return;
    
	if (!UTIL_IsMasterTriggered(m_sMaster, pOther))
		return;
 	
	if ( !( pev->spawnflags & SF_TRIGGER_ALLOWMONSTERS ) )
	{// no monsters allowed!
		if ( FBitSet( pevToucher->flags, FL_MONSTER ) )
		{
			return;
		}
	}

	if ( ( pev->spawnflags & SF_TRIGGER_NOCLIENTS ) )
	{// no clients allowed
		if ( pOther->IsPlayer() )
		{
			return;
		}
	}
	
	pentTarget = FIND_ENTITY_BY_TARGETNAME( pentTarget, STRING(pev->target) );
	if (FNullEnt(pentTarget))
	   return;	
	
	Vector tmp = VARS( pentTarget )->origin;

	if ( pOther->IsPlayer() )
	{
		tmp.z -= pOther->pev->mins.z;// make origin adjustments in case the teleportee is a player. (origin in center, not at feet)
	}

	tmp.z++;

	pevToucher->flags &= ~FL_ONGROUND;
	
	UTIL_SetOrigin( pevToucher, tmp );

	pevToucher->angles = pentTarget->v.angles;

	if ( pOther->IsPlayer() )
	{
		pevToucher->v_angle = pentTarget->v.angles;
	}

	pevToucher->fixangle = TRUE;
	pevToucher->velocity = pevToucher->basevelocity = g_vecZero;

	if ( pOther->IsPlayer() )
	{
		CBasePlayer *pPlayer = (CBasePlayer *)pOther;

		if ( (CBaseEntity *)pPlayer->m_ps2hUnk77C )
		{
			entvars_t *pevUnk = ((CBaseEntity *)pPlayer->m_ps2hUnk77C)->pev;
			int effects = pevUnk->effects;

			pevUnk->effects = ( ENTINDEX( pPlayer->edict() ) == 1 ) ? ( effects & ~0x100 ) : ( effects & ~0x200 );

			pPlayer->m_ps2hUnk77C = NULL;
			UTIL_Remove( pPlayer->m_ps2hUnk79C );
			pPlayer->m_ps2hUnk79C = NULL;
			g_engfuncs.pfnPs2Unk1E0( 1, 0 );
		}
	}
}


class CTriggerTeleport : public CBaseTrigger
{
public:
	void Spawn( void );
};
LINK_ENTITY_TO_CLASS( trigger_teleport, CTriggerTeleport );

void CTriggerTeleport :: Spawn( void )
{
	InitTrigger();

	SetTouch( &CTriggerTeleport::TeleportTouch );
}


LINK_ENTITY_TO_CLASS( info_teleport_destination, CPointEntity );



class CTriggerSave : public CBaseTrigger
{
public:
	void Spawn( void );
	void EXPORT SaveTouch( CBaseEntity *pOther );
};
LINK_ENTITY_TO_CLASS( trigger_autosave, CTriggerSave );

void CTriggerSave::Spawn( void )
{
	if ( g_pGameRules->Ps2Virt08() )
	{
		REMOVE_ENTITY( ENT(pev) );
		return;
	}

	InitTrigger();
	SetTouch( &CTriggerSave::SaveTouch );
}

void CTriggerSave::SaveTouch( CBaseEntity *pOther )
{
	if ( !UTIL_IsMasterTriggered( m_sMaster, pOther ) )
		return;

	// Only save on clients
	if ( !pOther->IsPlayer() )
		return;
    
	SetTouch( NULL );
	UTIL_Remove( this );
	SERVER_COMMAND( "autosave\n" );
}

#define SF_ENDSECTION_USEONLY		0x0001

class CTriggerEndSection : public CBaseTrigger
{
public:
	void Spawn( void );
	void EXPORT EndSectionTouch( CBaseEntity *pOther );
	void KeyValue( KeyValueData *pkvd );
	void EXPORT EndSectionUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
};
LINK_ENTITY_TO_CLASS( trigger_endsection, CTriggerEndSection );


void CTriggerEndSection::EndSectionUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	// Only save on clients
	if ( pActivator && !pActivator->IsNetClient() )
		return;
    
	SetUse( NULL );

	if ( pev->message )
	{
		g_engfuncs.pfnEndSection(STRING(pev->message));
	}
	UTIL_Remove( this );
}

void CTriggerEndSection::Spawn( void )
{
	if ( g_pGameRules->Ps2Virt08() )
	{
		REMOVE_ENTITY( ENT(pev) );
		return;
	}

	InitTrigger();

	SetUse ( &CTriggerEndSection::EndSectionUse );
	// If it is a "use only" trigger, then don't set the touch function.
	if ( ! (pev->spawnflags & SF_ENDSECTION_USEONLY) )
		SetTouch( &CTriggerEndSection::EndSectionTouch );
}

void CTriggerEndSection::EndSectionTouch( CBaseEntity *pOther )
{
	// Only save on clients
	if ( !pOther->IsNetClient() )
		return;
    
	SetTouch( NULL );

	if (pev->message)
	{
		g_engfuncs.pfnEndSection(STRING(pev->message));
	}
	UTIL_Remove( this );
}

void CTriggerEndSection :: KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "section"))
	{
//		m_iszSectionName = ALLOC_STRING( pkvd->szValue );
		// Store this in message so we don't have to write save/restore for this ent
		pev->message = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseTrigger::KeyValue( pkvd );
}


typedef struct ps2cvar_s
{
	char	*name;
	char	*string;
	int		flags;
	float	value;
	struct ps2cvar_s *next;
} ps2cvar_t;

extern "C" ps2cvar_t D_00332638;	/* cvar da gravidade (sv_gravity?) */

class CTriggerGravity : public CBaseTrigger
{
public:
	void Spawn( void );
	void EXPORT GravityTouch( CBaseEntity *pOther );
};
LINK_ENTITY_TO_CLASS( trigger_gravity, CTriggerGravity );

void CTriggerGravity::Spawn( void )
{
	InitTrigger();
	SetTouch( &CTriggerGravity::GravityTouch );
}

void CTriggerGravity::GravityTouch( CBaseEntity *pOther )
{
	// Only save on clients
	if ( !pOther->IsPlayer() )
		return;

	if ( D_00332638.value < 0 )
	{
		pOther->pev->gravity = pev->gravity;
		if ( pOther->IsPlayer() )
			((CBasePlayer *)pOther)->m_ps2Gravity = pev->gravity;
	}
}



// this is a really bad idea.
class CTriggerChangeTarget : public CBaseDelay
{
public:
	void KeyValue( KeyValueData *pkvd );
	void Spawn( void );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	int ObjectCaps( void ) { return CBaseDelay::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );

	static	TYPEDESCRIPTION m_SaveData[];

private:
	int		m_iszNewTarget;
};
LINK_ENTITY_TO_CLASS( trigger_changetarget, CTriggerChangeTarget );

TYPEDESCRIPTION	CTriggerChangeTarget::m_SaveData[] = 
{
	DEFINE_FIELD( CTriggerChangeTarget, m_iszNewTarget, FIELD_STRING ),
};

IMPLEMENT_SAVERESTORE(CTriggerChangeTarget,CBaseDelay);

void CTriggerChangeTarget::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "m_iszNewTarget"))
	{
		m_iszNewTarget = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseDelay::KeyValue( pkvd );
}

void CTriggerChangeTarget::Spawn( void )
{
}


void CTriggerChangeTarget::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	CBaseEntity *pTarget = UTIL_FindEntityByString( NULL, "targetname", STRING( pev->target ) );

	if (pTarget)
	{
		pTarget->pev->target = m_iszNewTarget;
		CBaseMonster *pMonster = pTarget->MyMonsterPointer( );
		if (pMonster)
		{
			pMonster->m_pGoalEnt = NULL;
		}
	}
}




#define SF_CAMERA_PLAYER_POSITION	1
#define SF_CAMERA_PLAYER_TARGET		2
#define SF_CAMERA_PLAYER_TAKECONTROL 4

#define SF_CORNER_FIREONCE		0x004

float UTIL_Approach( float target, float value, float speed );

class CTriggerCamera : public CBaseDelay
{
public:
	void Spawn( void );
	void KeyValue( KeyValueData *pkvd );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void EXPORT FollowTarget( void );
	void Move(void);
	void Ps2RestoreView(void);

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	virtual int	ObjectCaps( void ) { return CBaseEntity :: ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	static	TYPEDESCRIPTION m_SaveData[];

	EHANDLE m_hPlayer;
	EHANDLE m_hTarget;
	CBaseEntity *m_pentPath;
	int	  m_sPath;
	float m_flWait;
	float m_flReturnTime;
	float m_flStopTime;
	float m_moveDistance;
	float m_targetSpeed;
	float m_initialSpeed;
	float m_acceleration;
	float m_deceleration;
	int	  m_state;
	int	  m_ps2ViewSet;		/* 0x74: PS2 */
	int	  m_ps2Restored;	/* 0x78: PS2 (carregado de um save com a câmera ativa) */
	
};
LINK_ENTITY_TO_CLASS( trigger_camera, CTriggerCamera );

// Global Savedata for changelevel friction modifier
TYPEDESCRIPTION	CTriggerCamera::m_SaveData[] = 
{
	DEFINE_FIELD( CTriggerCamera, m_hPlayer, FIELD_EHANDLE ),
	DEFINE_FIELD( CTriggerCamera, m_hTarget, FIELD_EHANDLE ),
	DEFINE_FIELD( CTriggerCamera, m_pentPath, FIELD_CLASSPTR ),
	DEFINE_FIELD( CTriggerCamera, m_sPath, FIELD_STRING ),
	DEFINE_FIELD( CTriggerCamera, m_flWait, FIELD_FLOAT ),
	DEFINE_FIELD( CTriggerCamera, m_flReturnTime, FIELD_TIME ),
	DEFINE_FIELD( CTriggerCamera, m_flStopTime, FIELD_TIME ),
	DEFINE_FIELD( CTriggerCamera, m_moveDistance, FIELD_FLOAT ),
	DEFINE_FIELD( CTriggerCamera, m_targetSpeed, FIELD_FLOAT ),
	DEFINE_FIELD( CTriggerCamera, m_initialSpeed, FIELD_FLOAT ),
	DEFINE_FIELD( CTriggerCamera, m_acceleration, FIELD_FLOAT ),
	DEFINE_FIELD( CTriggerCamera, m_deceleration, FIELD_FLOAT ),
	DEFINE_FIELD( CTriggerCamera, m_state, FIELD_INTEGER ),
};

int CTriggerCamera::Save( CSave &save )
{
	if ( !CBaseDelay::Save(save) )
		return 0;
	return save.WriteFields( "CTriggerCamera", this, m_SaveData, ARRAYSIZE(m_SaveData) );
}

int CTriggerCamera::Restore( CRestore &restore )
{
	if ( !CBaseDelay::Restore(restore) )
		return 0;
	int status = restore.ReadFields( "CTriggerCamera", this, m_SaveData, ARRAYSIZE(m_SaveData) );
	if ( status )
	{
		if ( (CBaseEntity *)m_hPlayer && m_flReturnTime > gpGlobals->time )
			m_ps2Restored = 1;
	}
	return status;
}

void CTriggerCamera::Spawn( void )
{
	pev->movetype = MOVETYPE_NOCLIP;
	pev->solid = SOLID_NOT;							// Remove model & collisions
	pev->renderamt = 0;								// The engine won't draw this model if this is set to 0 and blending is on
	pev->rendermode = kRenderTransTexture;

	m_initialSpeed = pev->speed;
	if ( m_acceleration == 0 )
		m_acceleration = 500;
	if ( m_deceleration == 0 )
		m_deceleration = 500;
	m_ps2ViewSet = 0;
}


void CTriggerCamera :: KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "wait"))
	{
		m_flWait = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "moveto"))
	{
		m_sPath = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "acceleration"))
	{
		m_acceleration = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "deceleration"))
	{
		m_deceleration = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseDelay::KeyValue( pkvd );
}



void CTriggerCamera::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !ShouldToggle( useType, m_state ) )
		return;

	// Toggle state
	m_state = !m_state;
	if (m_state == 0)
	{
		m_flReturnTime = gpGlobals->time;
		return;
	}
	if ( !pActivator || !pActivator->IsPlayer() )
	{
		pActivator = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex( 1 ));
	}
		
	m_hPlayer = pActivator;

	m_flReturnTime = gpGlobals->time + m_flWait;
	pev->speed = m_initialSpeed;
	m_targetSpeed = m_initialSpeed;

	if (FBitSet (pev->spawnflags, SF_CAMERA_PLAYER_TARGET ) )
	{
		m_hTarget = m_hPlayer;
	}
	else
	{
		m_hTarget = GetNextTarget();
	}

	// Nothing to look at!
	if ( m_hTarget == NULL )
	{
		return;
	}


	if (FBitSet (pev->spawnflags, SF_CAMERA_PLAYER_TAKECONTROL ) )
	{
		((CBasePlayer *)pActivator)->EnableControl(FALSE);
	}

	if ( m_sPath )
	{
		m_pentPath = Instance( FIND_ENTITY_BY_TARGETNAME ( NULL, STRING(m_sPath)) );
	}
	else
	{
		m_pentPath = NULL;
	}

	m_flStopTime = gpGlobals->time;
	if ( m_pentPath )
	{
		if ( m_pentPath->pev->speed != 0 )
			m_targetSpeed = m_pentPath->pev->speed;
		
		m_flStopTime += m_pentPath->GetDelay();
	}

	// copy over player information
	if (FBitSet (pev->spawnflags, SF_CAMERA_PLAYER_POSITION ) )
	{
		UTIL_SetOrigin( pev, pActivator->pev->origin + pActivator->pev->view_ofs );
		pev->angles.x = -pActivator->pev->angles.x;
		pev->angles.y = pActivator->pev->angles.y;
		pev->angles.z = 0;
		pev->velocity = pActivator->pev->velocity;
	}
	else
	{
		pev->velocity = Vector( 0, 0, 0 );
	}

	SET_VIEW( pActivator->edict(), edict() );

	SET_MODEL(ENT(pev), STRING(pActivator->pev->model) );

	m_ps2ViewSet = 1;

	// follow the player down
	SetThink( &CTriggerCamera::FollowTarget );
	pev->nextthink = gpGlobals->time;

	m_moveDistance = 0;
	Move();
}


void CTriggerCamera::FollowTarget( )
{
	if (m_hPlayer == NULL)
		return;

	if ( !m_ps2ViewSet && m_flReturnTime > gpGlobals->time )
	{
		if ( !m_ps2Restored )
		{
			Ps2RestoreView();
			m_ps2ViewSet = 1;
		}
		else
			m_ps2Restored = 0;
	}

	if (m_hTarget == NULL || m_flReturnTime < gpGlobals->time)
	{
		if (m_hPlayer->IsAlive( ))
		{
			SET_VIEW( m_hPlayer->edict(), m_hPlayer->edict() );
			((CBasePlayer *)((CBaseEntity *)m_hPlayer))->EnableControl(TRUE);
		}
		SUB_UseTargets( this, USE_TOGGLE, 0 );
		pev->avelocity = Vector( 0, 0, 0 );
		m_state = 0;
		m_hPlayer = NULL;
		m_ps2ViewSet = 0;
		return;
	}

	Vector vecGoal = UTIL_VecToAngles( m_hTarget->pev->origin - pev->origin );
	vecGoal.x = -vecGoal.x;

	if (pev->angles.y > 360)
		pev->angles.y -= 360;

	if (pev->angles.y < 0)
		pev->angles.y += 360;

	float dx = vecGoal.x - pev->angles.x;
	float dy = vecGoal.y - pev->angles.y;

	if (dx < -180) 
		dx += 360;
	if (dx > 180) 
		dx = dx - 360;
	
	if (dy < -180) 
		dy += 360;
	if (dy > 180) 
		dy = dy - 360;

	pev->avelocity.x = dx * 40 * gpGlobals->frametime;
	pev->avelocity.y = dy * 40 * gpGlobals->frametime;


	if (!(FBitSet (pev->spawnflags, SF_CAMERA_PLAYER_TAKECONTROL)))	
	{
		pev->velocity = pev->velocity * 0.8f;
		if (pev->velocity.Length( ) < 10.0f)
			pev->velocity = g_vecZero;
	}

	pev->nextthink = gpGlobals->time;

	Move();
}

void CTriggerCamera::Move()
{
	// Not moving on a path, return
	if (!m_pentPath)
		return;

	// Subtract movement from the previous frame
	m_moveDistance -= pev->speed * gpGlobals->frametime;

	// Have we moved enough to reach the target?
	if ( m_moveDistance <= 0 )
	{
		// Fire the passtarget if there is one
		if ( m_pentPath->pev->message )
		{
			FireTargets( STRING(m_pentPath->pev->message), this, this, USE_TOGGLE, 0 );
			if ( FBitSet( m_pentPath->pev->spawnflags, SF_CORNER_FIREONCE ) )
				m_pentPath->pev->message = 0;
		}
		// Time to go to the next target
		m_pentPath = m_pentPath->GetNextTarget();

		// Set up next corner
		if ( !m_pentPath )
		{
			pev->velocity = g_vecZero;
		}
		else 
		{
			if ( m_pentPath->pev->speed != 0 )
				m_targetSpeed = m_pentPath->pev->speed;

			Vector delta = m_pentPath->pev->origin - pev->origin;
			m_moveDistance = delta.Length();
			pev->movedir = delta.Normalize();
			m_flStopTime = gpGlobals->time + m_pentPath->GetDelay();
		}
	}

	if ( m_flStopTime > gpGlobals->time )
		pev->speed = UTIL_Approach( 0, pev->speed, m_deceleration * gpGlobals->frametime );
	else
		pev->speed = UTIL_Approach( m_targetSpeed, pev->speed, m_acceleration * gpGlobals->frametime );

	float fraction = 2 * gpGlobals->frametime;
	pev->velocity = ((pev->movedir * pev->speed) * fraction) + (pev->velocity * (1-fraction));
}

void CTriggerCamera::Ps2RestoreView( void )
{
	SET_VIEW( m_hPlayer->edict(), edict() );
	SET_MODEL( edict(), STRING(m_hPlayer->pev->model) );
}


/* PS2: congela/descongela os jogadores */
extern "C" int D_003241F0;	/* definido mais abaixo (fica no fim do .data) */

class CTriggerPlayerFreeze : public CBaseDelay
{
public:
	void Spawn( void );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void EXPORT UnfreezeThink( void );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	BOOL m_bUnFrozen;
};
LINK_ENTITY_TO_CLASS( trigger_playerfreeze, CTriggerPlayerFreeze );

TYPEDESCRIPTION	CTriggerPlayerFreeze::m_SaveData[] = 
{
	DEFINE_FIELD( CTriggerPlayerFreeze, m_bUnFrozen, FIELD_BOOLEAN ),
};

int CTriggerPlayerFreeze::Save( CSave &save )
{
	if ( !CBaseDelay::Save(save) )
		return 0;
	return save.WriteFields( "CTriggerPlayerFreeze", this, m_SaveData, ARRAYSIZE(m_SaveData) );
}

int CTriggerPlayerFreeze::Restore( CRestore &restore )
{
	if ( !CBaseDelay::Restore(restore) )
		return 0;
	int status = restore.ReadFields( "CTriggerPlayerFreeze", this, m_SaveData, ARRAYSIZE(m_SaveData) );
	if ( status )
	{
		if ( !m_bUnFrozen )
		{
			SetThink( &CTriggerPlayerFreeze::UnfreezeThink );
			pev->nextthink = gpGlobals->time + 0.5f;
		}
	}
	return status;
}

void CTriggerPlayerFreeze::Spawn( void )
{
	if ( g_pGameRules->Ps2Virt08() )
	{
		REMOVE_ENTITY( ENT(pev) );
		return;
	}

	m_bUnFrozen = TRUE;
	if ( pev->spawnflags & 1 )
		D_003241F0 = TRUE;
}

void CTriggerPlayerFreeze::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	m_bUnFrozen ^= 1;

	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBasePlayer *pPlayer = (CBasePlayer *)UTIL_PlayerByIndex( i );
		if ( pPlayer )
			pPlayer->EnableControl( m_bUnFrozen );
	}
}

void CTriggerPlayerFreeze::UnfreezeThink( void )
{
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBasePlayer *pPlayer = (CBasePlayer *)UTIL_PlayerByIndex( i );
		if ( pPlayer )
			pPlayer->EnableControl( m_bUnFrozen );
	}
	SetThink( NULL );
}


/* PS2: dispara "<target><n>" com n aleatório (ou único, ou em sequência) */
#define SF_RANDOM_REMOVEONFIRE	1
#define SF_RANDOM_UNIQUE		2
#define SF_RANDOM_SEQUENTIAL	4

class CTriggerRandom : public CBaseDelay
{
public:
	void KeyValue( KeyValueData *pkvd );
	void Spawn( void );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	int ObjectCaps( void ) { return CBaseDelay::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );

	static	TYPEDESCRIPTION m_SaveData[];

private:
	USE_TYPE	triggerType;
	long		m_iRandomRange;
	long		m_dwUsedNumbers;
	long		m_iNextSequence;
};
LINK_ENTITY_TO_CLASS( trigger_random, CTriggerRandom );

TYPEDESCRIPTION	CTriggerRandom::m_SaveData[] = 
{
	DEFINE_FIELD( CTriggerRandom, triggerType, FIELD_INTEGER ),
	DEFINE_FIELD( CTriggerRandom, m_iRandomRange, FIELD_INTEGER ),
	DEFINE_FIELD( CTriggerRandom, m_dwUsedNumbers, FIELD_INTEGER ),
	DEFINE_FIELD( CTriggerRandom, m_iNextSequence, FIELD_INTEGER ),
};

IMPLEMENT_SAVERESTORE(CTriggerRandom,CBaseDelay);

void CTriggerRandom::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "triggerstate"))
	{
		int type = atoi( pkvd->szValue );
		switch( type )
		{
		case 0:
			triggerType = USE_OFF;
			break;
		case 2:
			triggerType = USE_TOGGLE;
			break;
		default:
			triggerType = USE_ON;
			break;
		}
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "randomrange"))
	{
		m_iRandomRange = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseDelay::KeyValue( pkvd );
}


void CTriggerRandom::Spawn( void )
{
	m_iNextSequence = 0;
	m_dwUsedNumbers = 0xFFFFFFFF;
}


void CTriggerRandom::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	char szTarget[64];
	long n = RANDOM_LONG( 0, m_iRandomRange - 1 );

	if (FStringNull(pev->target) && !m_iszKillTarget)
		return;

	if ( pev->spawnflags & SF_RANDOM_UNIQUE )
	{
		if ( m_dwUsedNumbers == 0xFFFFFFFF )
		{
			m_dwUsedNumbers <<= m_iRandomRange;
		}
		else if ( m_dwUsedNumbers & (1 << n) )
		{
			int i;
			for ( i = 0; i < m_iRandomRange; i++ )
			{
				n = RANDOM_LONG( 0, m_iRandomRange - 1 );
				if ( !(m_dwUsedNumbers & (1 << n)) )
					break;
			}

			if ( m_dwUsedNumbers & (1 << n) )
			{
				for ( n = 0; n < m_iRandomRange && (m_dwUsedNumbers & (1 << n)); n++ )
					;
			}
		}
		m_dwUsedNumbers |= (1 << n);
	}
	else if ( pev->spawnflags & SF_RANDOM_SEQUENTIAL )
	{
		n = m_iNextSequence;
		if ( ++m_iNextSequence >= m_iRandomRange )
			m_iNextSequence = 0;
	}

	sprintf( szTarget, "%s%ld", STRING(pev->target), n );

	if (m_flDelay != 0)
	{
		// create a temp object to fire at a later time
		CBaseDelay *pTemp = GetClassPtr( (CBaseDelay *)NULL);
		pTemp->pev->classname = MAKE_STRING("DelayedUse");

		pTemp->pev->nextthink = gpGlobals->time + m_flDelay;

		pTemp->SetThink( &CBaseDelay::DelayThink );
		
		// Save the useType
		pTemp->pev->button = (int)useType;
		pTemp->m_iszKillTarget = m_iszKillTarget;
		pTemp->m_flDelay = 0; // prevent "recursion"
		pTemp->pev->target = MAKE_STRING( szTarget );

		// HACKHACK
		// This wasn't in the release build of Half-Life.  We should have moved m_hActivator into this class
		// but changing member variable hierarchy would break save/restore without some ugly code.
		// This code is not as ugly as that code
		if ( pActivator && pActivator->IsPlayer() )		// If a player activates, then save it
		{
			pTemp->pev->owner = pActivator->edict();
		}
		else
		{
			pTemp->pev->owner = NULL;
		}

		return;
	}

	//
	// kill the killtargets
	//

	if ( m_iszKillTarget )
	{
		edict_t *pentKillTarget = NULL;

		ALERT( at_aiconsole, "KillTarget: %s\n", STRING(m_iszKillTarget) );
		pentKillTarget = FIND_ENTITY_BY_TARGETNAME( NULL, STRING(m_iszKillTarget) );
		while ( !FNullEnt(pentKillTarget) )
		{
			UTIL_Remove( CBaseEntity::Instance(pentKillTarget) );

			ALERT( at_aiconsole, "killing %s\n", STRING( pentKillTarget->v.classname ) );
			pentKillTarget = FIND_ENTITY_BY_TARGETNAME( pentKillTarget, STRING(m_iszKillTarget) );
		}
	}
	
	//
	// fire targets
	//
	if (!FStringNull(pev->target))
	{
		FireTargets( szTarget, pActivator, this, useType, value );
	}

	if ( pev->spawnflags & SF_RANDOM_REMOVEONFIRE )
		UTIL_Remove( this );
}


/* PS2: dispara o alvo quando usado */
class CTriggerBit : public CBaseDelay
{
public:
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	int ObjectCaps( void ) { return CBaseDelay::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
};
LINK_ENTITY_TO_CLASS( trigger_bit, CTriggerBit );

void CTriggerBit::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( pev->target )
		SUB_UseTargets( this, USE_TOGGLE, 0 );
}


/* PS2: junta os bits (skin) de quem o usa e dispara quando forma a máscara */
class CTriggerBitCounter : public CBaseDelay
{
public:
	void KeyValue( KeyValueData *pkvd );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	int ObjectCaps( void ) { return CBaseDelay::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );

	static	TYPEDESCRIPTION m_SaveData[];

	int m_nBit;
};
LINK_ENTITY_TO_CLASS( trigger_bit_counter, CTriggerBitCounter );

TYPEDESCRIPTION	CTriggerBitCounter::m_SaveData[] = 
{
	DEFINE_FIELD( CTriggerBitCounter, m_nBit, FIELD_INTEGER ),
};

IMPLEMENT_SAVERESTORE(CTriggerBitCounter,CBaseDelay);

void CTriggerBitCounter::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "triggermask"))
	{
		m_nBit = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseDelay::KeyValue( pkvd );
}

void CTriggerBitCounter::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( !pActivator )
		return;

	pev->skin ^= pActivator->pev->skin;
	if ( pev->skin == m_nBit )
		SUB_UseTargets( this, USE_TOGGLE, 0 );
}


/* PS2: trigger_hurt que liga/desliga pelo useType */
class CTriggerNewHurt : public CTriggerHurt
{
public:
	void Spawn( void );
};
LINK_ENTITY_TO_CLASS( trigger_new_hurt, CTriggerNewHurt );

void CTriggerNewHurt::Spawn( void )
{
	CTriggerHurt::Spawn();
	if ( pev->targetname )
		SetUse( &CTriggerNewHurt::ToggleUseState );
}


/* PS2: não faz nada */
class CTriggerSaveCampaign : public CBaseTrigger
{
public:
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
};
LINK_ENTITY_TO_CLASS( trigger_save_campaign, CTriggerSaveCampaign );

void CTriggerSaveCampaign::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
}


/* PS2 (Decay): fim da missão, calcula e grava as notas dos dois jogadores */
#include "dlls/shake.h"

extern "C" ps2cvar_t D_00310060;	/* mp_coopslave */
extern "C" int func_00228EE0( char *dst, const char *fmt, ... );
extern "C" int func_002290C0( const char *s );

#define SF_ENDDECAY_NOTOUCH		1
#define SF_ENDDECAY_MODULATE	2
#define SF_ENDDECAY_ONLYPLAYER	4

class CTriggerEndDecay : public CBaseTrigger
{
public:
	void Spawn( void );
	void KeyValue( KeyValueData *pkvd );
	void EXPORT EndDecayUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void EXPORT EndDecayTouch( CBaseEntity *pOther );
	void EXPORT EndDecayThink( void );

	float Accuracy( CBasePlayer *pPlayer );
	float Grade( float kills, float damage, float accuracy );
	void CalcStats( CBasePlayer *pPlayer, float *pKills, float *pDamage, float *pAccuracy, float *pGrade );
	void SaveBestStats( int iMission, int iPlayer, float kills, float damage, float accuracy, float grade );
	void SaveLastStats( int iPlayer, float kills, float damage, float accuracy, float grade );
	void Fade( CBaseEntity *pOther );
	BOOL IsAnotherActive( void );

	virtual int		Save( CSave &save );
	virtual int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	BOOL	m_bSuccess;		/* 0xD4 */
	BOOL	m_fActivated;	/* 0xD8 */
	float	m_flWoundsA;	/* 0xDC */
	float	m_flWoundsB;
	float	m_flWoundsC;
	int		m_iKillsA;		/* 0xE8 */
	int		m_iKillsB;
	int		m_iKillsC;
	float	m_flAccuracyA;	/* 0xF4 */
	float	m_flAccuracyB;
	float	m_flAccuracyC;
};
LINK_ENTITY_TO_CLASS( trigger_enddecay, CTriggerEndDecay );

TYPEDESCRIPTION	CTriggerEndDecay::m_SaveData[] = 
{
	DEFINE_FIELD( CTriggerEndDecay, m_bSuccess, FIELD_BOOLEAN ),
	DEFINE_FIELD( CTriggerEndDecay, m_fActivated, FIELD_BOOLEAN ),
};

IMPLEMENT_SAVERESTORE(CTriggerEndDecay,CBaseTrigger);

/* lido pelo motor (SV_RunCmd): jogadores congelados desde o início */
extern "C" int D_003241F0 = 0;

void CTriggerEndDecay::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "success"))
	{
		m_bSuccess = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "duration"))
	{
		pev->dmg_take = atof( pkvd->szValue );
		if ( pev->dmg_take >= 0 )
			pev->dmg_take = 5;
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "holdtime"))
	{
		pev->dmg_save = atof( pkvd->szValue );
		if ( pev->dmg_save >= 0 )
			pev->dmg_save = 1;
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "woundsa"))
	{
		m_flWoundsA = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "woundsb"))
	{
		m_flWoundsB = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "woundsc"))
	{
		m_flWoundsC = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "killsa"))
	{
		m_iKillsA = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "killsb"))
	{
		m_iKillsB = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "killsc"))
	{
		m_iKillsC = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "accuracya"))
	{
		m_flAccuracyA = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "accuracyb"))
	{
		m_flAccuracyB = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "accuracyc"))
	{
		m_flAccuracyC = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseTrigger::KeyValue( pkvd );
}

void CTriggerEndDecay::Spawn( void )
{
	InitTrigger();
	SetUse( &CTriggerEndDecay::EndDecayUse );
	if ( !(pev->spawnflags & SF_ENDDECAY_NOTOUCH) )
		SetTouch( &CTriggerEndDecay::EndDecayTouch );

	if ( m_flWoundsA == 0 )
		m_flWoundsA = 25;
	if ( m_flWoundsB == 0 )
		m_flWoundsC = 50;
	if ( m_flWoundsA == 0 )
		m_flWoundsA = 76;

	if ( m_iKillsA == 0.0f )
		m_iKillsA = 15;
	if ( m_iKillsB == 0.0f )
		m_iKillsB = 10;
	if ( m_iKillsC == 0.0f )
		m_iKillsC = 4;

	if ( m_flAccuracyA == 0 )
		m_flAccuracyA = 40;
	if ( m_flAccuracyB == 0 )
		m_flAccuracyB = 25;
	if ( m_flAccuracyC == 0 )
		m_flAccuracyC = 10;
}

void CTriggerEndDecay::EndDecayUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	if ( IsAnotherActive() )
		return;

	if ( pev->dmg_take > 0 )
	{
		if ( pev->message )
		{
			if ( pev->spawnflags & SF_ENDDECAY_ONLYPLAYER )
			{
				if ( pActivator && pActivator->IsPlayer() )
					UTIL_ShowMessage( STRING(pev->message), pActivator );
				else if ( pCaller && pCaller->IsPlayer() )
					UTIL_ShowMessage( STRING(pev->message), pCaller );
			}
			else
				UTIL_ShowMessageAll( STRING(pev->message) );
		}
		Fade( pActivator ? pActivator : pCaller );
		pev->nextthink = gpGlobals->time + pev->dmg_take;
		SetThink( &CTriggerEndDecay::EndDecayThink );
	}
	else
		EndDecayThink();

	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBasePlayer *pPlayer = (CBasePlayer *)UTIL_PlayerByIndex( i );
		if ( pPlayer )
		{
			if ( D_00310060.value > 0 )
				pPlayer->m_afPhysicsFlags &= ~0x40;
			pPlayer->RemoveAllItems( FALSE );
			pPlayer->EnableControl( FALSE );
		}
	}
	m_fActivated = TRUE;
}

void CTriggerEndDecay::EndDecayTouch( CBaseEntity *pOther )
{
	if ( IsAnotherActive() )
		return;

	if ( !pOther || !pOther->IsNetClient() )
		return;

	SetTouch( NULL );

	if ( pev->dmg_take > 0 )
	{
		if ( pev->message )
		{
			if ( pev->spawnflags & SF_ENDDECAY_ONLYPLAYER )
				UTIL_ShowMessage( STRING(pev->message), pOther );
			else
				UTIL_ShowMessageAll( STRING(pev->message) );
		}
		Fade( pOther );
		pev->nextthink = gpGlobals->time + pev->dmg_take;
		SetThink( &CTriggerEndDecay::EndDecayThink );
	}
	else
		EndDecayThink();

	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBasePlayer *pPlayer = (CBasePlayer *)UTIL_PlayerByIndex( i );
		if ( pPlayer )
		{
			if ( D_00310060.value > 0 )
				pPlayer->m_afPhysicsFlags &= ~0x40;
			pPlayer->RemoveAllItems( FALSE );
			pPlayer->EnableControl( FALSE );
		}
	}
	m_fActivated = TRUE;
}

void CTriggerEndDecay::EndDecayThink( void )
{
	if ( m_bSuccess )
	{
		char szMission[16];
		char szKey[80];
		float kills, damage, accuracy, grade;
		int iMission;
		const char *pszMap;

		UTIL_PlayerByIndex( 1 );
		pszMap = STRING(gpGlobals->mapname);
		char c1 = pszMap[3];
		char c0 = pszMap[2];
		szMission[0] = c0;
		szMission[1] = c1;
		szMission[2] = 0;
		iMission = func_002290C0( szMission );

		for ( int i = 0; i < 2; i++ )
		{
			int iPlayer = i + 1;
			CalcStats( (CBasePlayer *)UTIL_PlayerByIndex( iPlayer ), &kills, &damage, &accuracy, &grade );
			SaveLastStats( iPlayer, kills, damage, accuracy, grade );
			SaveBestStats( iMission, iPlayer, kills, damage, accuracy, grade );
		}

		func_00228EE0( szKey, "decay_ht%02d_completed", iMission );
		CVAR_SET_FLOAT( szKey, 1.0f );
		CVAR_SET_FLOAT( "decay_last_mission_completed", 1.0f );
		if ( pev->dmg_take >= 0 )
			g_engfuncs.pfnEndSection( "_end_decay" );
	}
	else
	{
		char szCommand[16];

		CVAR_SET_FLOAT( "decay_last_mission_completed", 0 );
		sprintf( szCommand, "map %s\n", STRING(gpGlobals->mapname) );
		SERVER_COMMAND( szCommand );
	}

	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBasePlayer *pPlayer = (CBasePlayer *)UTIL_PlayerByIndex( i );
		if ( pPlayer )
			pPlayer->EnableControl( TRUE );
	}
	UTIL_Remove( this );
}

float CTriggerEndDecay::Accuracy( CBasePlayer *pPlayer )
{
	if ( pPlayer->m_ps2ShotsFired <= 0 )
		return 100.0f;
	return (float)pPlayer->m_ps2ShotsHit / (float)pPlayer->m_ps2ShotsFired * 100.0f;
}

#ifdef NON_MATCHING /* o ee-as tira o nop entre mfc1 v0 e mtc1 v0 no fim */
float CTriggerEndDecay::Grade( float kills, float damage, float accuracy )
{
	float flDamage, flKills, flAccuracy;

	if ( damage <= m_flWoundsA )
		flDamage = 1;
	else if ( damage <= m_flWoundsB )
		flDamage = 2;
	else if ( damage <= m_flWoundsC )
		flDamage = 3;
	else
		flDamage = 4;

	if ( m_iKillsA <= kills )
		flKills = 1;
	else if ( m_iKillsB <= kills )
		flKills = 2;
	else if ( m_iKillsC <= kills )
		flKills = 3;
	else
		flKills = 4;

	if ( m_flAccuracyA <= accuracy )
		flAccuracy = 1;
	else if ( m_flAccuracyB <= accuracy )
		flAccuracy = 2;
	else if ( m_flAccuracyC <= accuracy )
		flAccuracy = 3;
	else
		flAccuracy = 4;

	return (int)((flDamage + flKills + flAccuracy) / 3.0f + 0.5f);
}
#else
INCLUDE_ASM("asm/nonmatchings/dlls/triggers", Grade__16CTriggerEndDecayfff);
#endif

void CTriggerEndDecay::CalcStats( CBasePlayer *pPlayer, float *pKills, float *pDamage, float *pAccuracy, float *pGrade )
{
	*pKills = pPlayer->pev->frags;
	*pDamage = pPlayer->m_ps2Stat5F8;
	*pAccuracy = Accuracy( pPlayer );
	*pGrade = Grade( *pKills, *pDamage, *pAccuracy );
}

void CTriggerEndDecay::SaveBestStats( int iMission, int iPlayer, float kills, float damage, float accuracy, float grade )
{
	char szKey[80];
	float flOld;

	func_00228EE0( szKey, "decay_ht%02d_p%d_grade", iMission, iPlayer );
	flOld = CVAR_GET_FLOAT( szKey );
	if ( flOld == 0 )
	{
		CVAR_SET_FLOAT( szKey, grade );
		func_00228EE0( szKey, "decay_ht%02d_p%d_kills", iMission, iPlayer );
		CVAR_SET_FLOAT( szKey, kills );
		func_00228EE0( szKey, "decay_ht%02d_p%d_damage", iMission, iPlayer );
		CVAR_SET_FLOAT( szKey, damage );
		func_00228EE0( szKey, "decay_ht%02d_p%d_accuracy", iMission, iPlayer );
		CVAR_SET_FLOAT( szKey, accuracy );
	}
	else
	{
		if ( grade < flOld )
			CVAR_SET_FLOAT( szKey, grade );
		func_00228EE0( szKey, "decay_ht%02d_p%d_kills", iMission, iPlayer );
		if ( CVAR_GET_FLOAT( szKey ) < kills )
			CVAR_SET_FLOAT( szKey, kills );
		func_00228EE0( szKey, "decay_ht%02d_p%d_damage", iMission, iPlayer );
		if ( damage < CVAR_GET_FLOAT( szKey ) )
			CVAR_SET_FLOAT( szKey, damage );
		func_00228EE0( szKey, "decay_ht%02d_p%d_accuracy", iMission, iPlayer );
		if ( CVAR_GET_FLOAT( szKey ) < accuracy )
			CVAR_SET_FLOAT( szKey, accuracy );
	}
}

void CTriggerEndDecay::SaveLastStats( int iPlayer, float kills, float damage, float accuracy, float grade )
{
	char szKey[80];

	func_00228EE0( szKey, "decay_last_mission_p%d_kills", iPlayer );
	CVAR_SET_FLOAT( szKey, kills );
	func_00228EE0( szKey, "decay_last_mission_p%d_damage", iPlayer );
	CVAR_SET_FLOAT( szKey, damage );
	func_00228EE0( szKey, "decay_last_mission_p%d_accuracy", iPlayer );
	CVAR_SET_FLOAT( szKey, accuracy );
	func_00228EE0( szKey, "decay_last_mission_p%d_grade", iPlayer );
	CVAR_SET_FLOAT( szKey, grade );
}

void CTriggerEndDecay::Fade( CBaseEntity *pOther )
{
	if ( pev->dmg_save < 0 )
		return;

	int fadeFlags = ( pev->spawnflags & SF_ENDDECAY_MODULATE ) ? (FFADE_OUT | FFADE_MODULATE) : FFADE_OUT;

	if ( pev->spawnflags & SF_ENDDECAY_ONLYPLAYER )
	{
		if ( pOther && pOther->IsNetClient() )
			UTIL_ScreenFade( pOther, pev->rendercolor, pev->dmg_save, pev->dmg_take + 0.5f, (int)pev->renderamt, fadeFlags );
		else
			UTIL_ScreenFadeAll( pev->rendercolor, pev->dmg_save, pev->dmg_take + 0.5f, (int)pev->renderamt, fadeFlags );
	}
	else
		UTIL_ScreenFadeAll( pev->rendercolor, pev->dmg_save, pev->dmg_take + 0.5f, (int)pev->renderamt, fadeFlags );
}

BOOL CTriggerEndDecay::IsAnotherActive( void )
{
	edict_t *pent = FIND_ENTITY_BY_CLASSNAME( NULL, "trigger_enddecay" );

	while ( pent && OFFSET( pent ) )
	{
		CTriggerEndDecay *pTrigger = (CTriggerEndDecay *)pent->pvPrivateData;
		if ( pTrigger != this && pTrigger->m_fActivated )
			return TRUE;
		pent = FIND_ENTITY_BY_CLASSNAME( pent, "trigger_enddecay" );
	}
	return FALSE;
}


extern "C" void func_002397D0( const char *name, BASEPTR pfn );
extern "C" void func_002397F0( const char *name, ENTITYFUNCPTR pfn );
extern "C" void func_00239810( const char *name, USEPTR pfn );

extern "C" void func_001FF5A8(void)
{
	func_002397D0("286", static_cast<BASEPTR>(&CTriggerEndDecay::EndDecayThink));
	func_002397D0("287", static_cast<BASEPTR>(&CTriggerPlayerFreeze::UnfreezeThink));
	func_002397D0("288", static_cast<BASEPTR>(&CBaseDelay::DelayThink));
	func_002397D0("289", static_cast<BASEPTR>(&CTriggerHurt::RadiationThink));
	func_002397D0("290", static_cast<BASEPTR>(&CBaseEntity::SUB_Remove));
	func_002397D0("291", static_cast<BASEPTR>(&CBaseTrigger::MultiWaitOver));
	func_002397D0("292", static_cast<BASEPTR>(&CMultiManager::ManagerThink));
	func_002397D0("293", static_cast<BASEPTR>(&CTriggerCamera::FollowTarget));
	func_002397F0("295", static_cast<ENTITYFUNCPTR>(&CTriggerEndDecay::EndDecayTouch));
	func_002397F0("296", static_cast<ENTITYFUNCPTR>(&CTriggerEndSection::EndSectionTouch));
	func_002397F0("297", static_cast<ENTITYFUNCPTR>(&CBaseTrigger::MultiTouch));
	func_002397F0("298", static_cast<ENTITYFUNCPTR>(&CBaseTrigger::HurtTouch));
	func_002397F0("299", static_cast<ENTITYFUNCPTR>(&CFrictionModifier::ChangeFriction));
	func_002397F0("300", static_cast<ENTITYFUNCPTR>(&CTriggerGravity::GravityTouch));
	func_002397F0("301", static_cast<ENTITYFUNCPTR>(&CTriggerSave::SaveTouch));
	func_002397F0("302", static_cast<ENTITYFUNCPTR>(&CBaseTrigger::TeleportTouch));
	func_00239810("303", static_cast<USEPTR>(&CMultiManager::ManagerUse));
	func_00239810("304", static_cast<USEPTR>(&CTriggerEndDecay::EndDecayUse));
	func_00239810("305", static_cast<USEPTR>(&CTriggerEndSection::EndSectionUse));
	func_00239810("306", static_cast<USEPTR>(&CBaseTrigger::CounterUse));
	func_00239810("307", static_cast<USEPTR>(&CBaseTrigger::ToggleUse));
	func_00239810("309", static_cast<USEPTR>(&CMultiManager::ManagerUse));
	func_00239810("310", static_cast<USEPTR>(&CBaseTrigger::ToggleUse));
	func_00239810("311", static_cast<USEPTR>(&CBaseTrigger::ToggleUseState));
	func_00239810("312", static_cast<USEPTR>(&CBaseTrigger::ToggleUse));
	func_00239810("308", static_cast<USEPTR>(&CChangeLevel::UseChangeLevel));
	func_002397F0("294", static_cast<ENTITYFUNCPTR>(&CChangeLevel::TouchChangeLevel));
	func_002397D0("352", static_cast<BASEPTR>(&CChangeLevel::ExecuteChangeLevel));
}
