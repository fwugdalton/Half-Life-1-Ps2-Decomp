/* dlls/controller_hud.cpp: entidade ps2controller_hud (CControllerHud), só do PS2.
   Não existe no HLSDK e o nome do arquivo é inventado: quando usada, manda ao
   jogador 1 uma mensagem com os spawnflags e o tempo de exibição (holdtime) para o
   HUD mostrar a figura do controle (sprites/controller.spz). */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/gamerules.h"
#include "dlls/cbase.h"
#include "dlls/saverestore.h"
#include "include_asm.h"

extern "C" int D_00317D20;			/* mensagem do HUD do controle */
extern "C" void func_00151A48( void );
extern "C" void func_00239810( const char *name, USEPTR pfn );

class CControllerHud : public CPointEntity
{
	friend void func_00151A48( void );
public:
	void Spawn( void );
	void Precache( void );
	void KeyValue( KeyValueData *pkvd );
	void EXPORT HudUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	virtual int Save( CSave &save );
	virtual int Restore( CRestore &restore );

	static TYPEDESCRIPTION m_SaveData[];

	int m_ps2Unk30;			/* 0x30: sem uso aqui (o GetClassPtr aloca 0x38) */
	int m_iHoldTime;		/* 0x34 */
};

LINK_ENTITY_TO_CLASS( ps2controller_hud, CControllerHud );

TYPEDESCRIPTION CControllerHud::m_SaveData[] =
{
	DEFINE_FIELD( CControllerHud, m_iHoldTime, FIELD_INTEGER ),
};

IMPLEMENT_SAVERESTORE( CControllerHud, CBaseEntity );

void CControllerHud :: Precache( void )
{
	PRECACHE_MODEL( "sprites/controller.spz" );
}

void CControllerHud :: Spawn( void )
{
	Precache();

	pev->solid = SOLID_NOT;
	pev->movetype = MOVETYPE_NONE;
	pev->effects = EF_NODRAW;

	SetUse( &CControllerHud::HudUse );
}

void CControllerHud :: KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "holdtime" ) )
	{
		m_iHoldTime = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		pkvd->fHandled = FALSE;
}

void CControllerHud :: HudUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	CBaseEntity *pPlayer = CBaseEntity::Instance( INDEXENT( 1 ) );

	if ( pPlayer )
	{
		MESSAGE_BEGIN( MSG_ONE, D_00317D20, NULL, pPlayer->edict() );
			WRITE_SHORT( pev->spawnflags );
			WRITE_SHORT( m_iHoldTime );
		MESSAGE_END();
	}
}

extern "C" void func_00151A48( void )
{
	func_00239810( "067", static_cast<USEPTR>( &CControllerHud::HudUse ) );
}
