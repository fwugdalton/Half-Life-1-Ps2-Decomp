/* dlls/aievent.cpp: info_aievent, entidade nova do PS2 (não existe no HLSDK).
   É um aiscripted_sequence (CCineAI) com tipo de evento, ângulo, repetições, chance e um alvo
   que o CBaseMonster::Ps2Unk127 (monsters.cpp) aciona. Nomes de arquivo, classe e
   campos são inventados; os nomes das chaves vêm da .rodata. */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/scripted.h"

extern "C" void func_00126E20( void );

class CAIEvent : public CCineAI
{
public:
	void Spawn( void );
	void KeyValue( KeyValueData *pkvd );
	void SequenceDone( CBaseMonster *pMonster );

	int m_iEventType;           /* 0x2D4 */
	float m_flAngleRange;       /* 0x2D8 */
	int m_iAnimCount;           /* 0x2DC */
	int m_iCount;               /* 0x2E0 */
	int m_iProbability;         /* 0x2E4 */
	string_t m_iszUseTarget;    /* 0x2E8 */
	BYTE m_fActive;             /* 0x2EC: o Ps2Unk127 zera */
};

LINK_ENTITY_TO_CLASS( info_aievent, CAIEvent );


void CAIEvent :: KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "eventtype"))
	{
		m_iEventType = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "anglerange"))
	{
		m_flAngleRange = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "animcount"))
	{
		m_iAnimCount = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "probability"))
	{
		m_iProbability = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "usetarget"))
	{
		m_iszUseTarget = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CCineMonster::KeyValue( pkvd );
}


void CAIEvent :: Spawn( void )
{
	pev->classname = MAKE_STRING( "info_aievent" );
}


void CAIEvent :: SequenceDone( CBaseMonster *pMonster )
{
	if ( m_iEventType == 2 )
	{
		if ( ++m_iCount < m_iAnimCount )
			return;
	}

	if ( !( pev->spawnflags & SF_SCRIPT_REPEATABLE ) )
	{
		SetThink( &CAIEvent::SUB_Remove );
		pev->nextthink = gpGlobals->time + 0.1f;
	}

	pMonster->CineCleanup();
	FixScriptMonsterSchedule( pMonster );
	SUB_UseTargets( NULL, USE_TOGGLE, 0 );
}


/* registro dos ponteiros de Think para o save do PS2 */
extern "C" void func_002397D0( const char *name, BASEPTR pfn );

extern "C" void func_00126E20( void )
{
	func_002397D0("006", static_cast<BASEPTR>(&CAIEvent::SUB_Remove));
}
