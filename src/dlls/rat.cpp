/* dlls/rat.cpp: o rato (monster_rat) do HLSDK. No PS2 o Spawn também liga o
   takedamage depois do MonsterInit. */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/schedule.h"

//=========================================================
// Monster's Anim Events Go Here
//=========================================================

class CRat : public CBaseMonster
{
public:
	void Spawn( void );
	void Precache( void );
	void SetYawSpeed( void );
	int  Classify ( void );
};
LINK_ENTITY_TO_CLASS( monster_rat, CRat );

//=========================================================
// Classify - indicates this monster's place in the
// relationship table.
//=========================================================
int	CRat :: Classify ( void )
{
	return	CLASS_INSECT;
}

//=========================================================
// SetYawSpeed - allows each sequence to have a different
// turn rate associated with it.
//=========================================================
void CRat :: SetYawSpeed ( void )
{
	int ys;

	switch ( m_Activity )
	{
	case ACT_IDLE:
	default:
		ys = 45;
		break;
	}

	pev->yaw_speed = ys;
}

//=========================================================
// Spawn
//=========================================================
void CRat :: Spawn()
{
	Precache( );

	SET_MODEL(ENT(pev), "models/bigrat.dol");
	UTIL_SetSize( pev, Vector( 0, 0, 0 ), Vector( 0, 0, 0 ) );

	pev->solid			= SOLID_SLIDEBOX;
	pev->movetype		= MOVETYPE_STEP;
	m_bloodColor		= BLOOD_COLOR_RED;
	pev->health			= 8;
	pev->view_ofs		= Vector ( 0, 0, 6 );// position of the eyes relative to monster's origin.
	m_flFieldOfView		= 0.5;// indicates the width of this monster's forward view cone ( as a dotproduct result )
	m_MonsterState		= MONSTERSTATE_NONE;

	MonsterInit();
	pev->takedamage		= DAMAGE_YES;
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void CRat :: Precache()
{
	PRECACHE_MODEL("models/bigrat.dol");
}
