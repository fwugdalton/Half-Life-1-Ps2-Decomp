/* dlls/item_generic.cpp: o item_generic, entidade só do PS2 (do Decay) que
   mostra um modelo parado ou animado em loop. Não existe no HLSDK. */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "include_asm.h"

#define SF_ITEMGENERIC_DROP_TO_FLOOR	0x0001

class CGenericItem : public CBaseAnimating
{
public:
	void Spawn( void );
	void Precache( void );
	void KeyValue( KeyValueData *pkvd );
	void Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	void EXPORT StartItem( void );
	void EXPORT AnimateThink( void );

	void SetRendering( int rendermode, int r, int g, int b, int renderamt, int renderfx );

	float m_lastTime;
	string_t m_iszSequenceName;
};
LINK_ENTITY_TO_CLASS( item_generic, CGenericItem );

void CGenericItem::KeyValue( KeyValueData *pkvd )
{
	if (FStrEq(pkvd->szKeyName, "sequencename"))
	{
		if (pkvd->szValue && *pkvd->szValue && *pkvd->szValue != '0')
			m_iszSequenceName = ALLOC_STRING(pkvd->szValue);
		else
			m_iszSequenceName = 0;
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "skin"))
	{
		pev->skin = (int)atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "body"))
	{
		pev->body = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
		CBaseAnimating::KeyValue( pkvd );
}

void CGenericItem::Spawn( void )
{
	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->effects = 0;
	pev->frame = 0;

	Precache();
	SET_MODEL( ENT(pev), STRING(pev->model) );

	if (m_iszSequenceName || (pev->spawnflags & SF_ITEMGENERIC_DROP_TO_FLOOR))
	{
		SetThink( &CGenericItem::StartItem );
		pev->nextthink = gpGlobals->time + 0.1f;
	}
}

void CGenericItem::Precache( void )
{
	PRECACHE_MODEL( (char *)STRING(pev->model) );
}

void CGenericItem::AnimateThink( void )
{
	DispatchAnimEvents( 0.1f );
	StudioFrameAdvance( 0 );

	if (m_fSequenceFinished && !m_fSequenceLoops)
	{
		pev->frame = 0;
		ResetSequenceInfo();
	}

	pev->nextthink = gpGlobals->time + 0.1f;
	m_lastTime = gpGlobals->time;
}

void CGenericItem::StartItem( void )
{
	if ((pev->spawnflags & SF_ITEMGENERIC_DROP_TO_FLOOR) && !DROP_TO_FLOOR(ENT(pev)))
	{
		ALERT( at_error, "Item %s fell out of level at %f,%f,%f", STRING(pev->classname), pev->origin.x, pev->origin.y, pev->origin.z );
		UTIL_Remove( this );
		return;
	}

	if (m_iszSequenceName)
	{
		pev->effects = 0;
		pev->sequence = LookupSequence( STRING(m_iszSequenceName) );
		if (pev->sequence == -1)
			pev->sequence = 0;
		pev->frame = 0;
		ResetSequenceInfo();
		SetThink( &CGenericItem::AnimateThink );
		pev->nextthink = gpGlobals->time + 0.1f;
		pev->frame = 0;
	}
}

void CGenericItem::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	SetThink( &CGenericItem::SUB_Remove );
	pev->nextthink = gpGlobals->time + 0.1f;
}

extern "C" void func_002397D0(const char *name, BASEPTR pfn);

extern "C" void func_00176DF8(void)
{
	func_002397D0("117", static_cast<BASEPTR>(&CGenericItem::StartItem));
	func_002397D0("118", static_cast<BASEPTR>(&CGenericItem::SUB_Remove));
	func_002397D0("119", static_cast<BASEPTR>(&CGenericItem::AnimateThink));
}

inline void CGenericItem::SetRendering( int rendermode, int r, int g, int b, int renderamt, int renderfx )
{
	pev->rendermode = rendermode;
	pev->rendercolor.x = r;
	pev->rendercolor.y = g;
	pev->rendercolor.z = b;
	pev->renderamt = renderamt;
	pev->renderfx = renderfx;
}
