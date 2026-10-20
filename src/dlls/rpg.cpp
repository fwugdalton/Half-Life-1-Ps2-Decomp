/* dlls/rpg.cpp: o lança-foguetes (weapon_rpg), o ponto do laser
   (laser_spot), o foguete (rpg_rocket) e a munição (ammo_rpgclip) do HLSDK,
   na versão antiga (gpGlobals->time, m_pLauncher como ponteiro). O PS2
   ganhou m_fSpotVisible, m_flNextEmptySound, vibração no disparo e uma cópia
   própria do Explode/Smoke do CGrenade no foguete. */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/weapons.h"
#include "dlls/player.h"
#include "dlls/gamerules.h"
#include "dlls/soundent.h"
#include "dlls/decals.h"
#include "include_asm.h"

#define WEAPON_RPG              8
#define RPG_WEIGHT              20
#define RPG_MAX_CLIP            1
#define RPG_DEFAULT_GIVE        1
#define ROCKET_MAX_CARRY        5
#define AMMO_RPGCLIP_GIVE       RPG_MAX_CLIP

#define AUTOAIM_10DEGREES       0.1736481776669f

#define TE_EXPLOSION            3
#define TE_EXPLFLAG_NONE        0
#define TE_SMOKE                5
#define TE_BEAMFOLLOW           22
#define EF_LIGHT                64
#define GIB_NORMAL              0
#define GIB_NEVER               1

int UTIL_PointContents( const Vector &vec );
void UTIL_Bubbles( Vector mins, Vector maxs, int count );
void UTIL_BubbleTrail( Vector from, Vector to, int count );

extern "C" void func_002071C8(CBasePlayer *pPlayer, int iUnk, unsigned char intensity, unsigned short duration);

enum rpg_e {
	RPG_IDLE = 0,
	RPG_FIDGET,
	RPG_RELOAD,		// to reload
	RPG_FIRE2,		// to empty
	RPG_HOLSTER1,	// loaded
	RPG_DRAW1,		// loaded
	RPG_HOLSTER2,	// unloaded
	RPG_DRAW_UL,	// unloaded
	RPG_IDLE_UL,	// unloaded idle
	RPG_FIDGET_UL,	// unloaded fidget
};

class CLaserSpot : public CBaseEntity
{
public:
	void Spawn( void );
	void Precache( void );

	int	ObjectCaps( void ) { return FCAP_DONT_SAVE; }

	void Suspend( float flSuspendTime );
	void EXPORT Revive( void );

	static CLaserSpot *CreateSpot( void );
};

class CRpg : public CBasePlayerWeapon
{
public:
	int		Save( CSave &save );
	int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	void Spawn( void );
	void Precache( void );
	void Reload( void );
	int iItemSlot( void ) { return 4; }
	int GetItemInfo(ItemInfo *p);
	int AddToPlayer( CBasePlayer *pPlayer );

	BOOL Deploy( void );
	BOOL CanHolster( void );
	void Holster( void );

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void WeaponIdle( void );

	void UpdateSpot( void );
	BOOL ShouldWeaponIdle( void ) { return TRUE; }

	CLaserSpot *m_pSpot;		/* 0x8C */
	int m_fSpotActive;		/* 0x90 */
	int m_fSpotVisible;		/* 0x94 */
	int m_cActiveRockets;		/* 0x98 */
	float m_flNextEmptySound;	/* 0x9C */
};

class CRpgRocket : public CGrenade
{
public:
	int		Save( CSave &save );
	int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];
	void Spawn( void );
	void Precache( void );
	void EXPORT FollowThink( void );
	void EXPORT IgniteThink( void );
	void EXPORT RocketTouch( CBaseEntity *pOther );
	void Explode( TraceResult *pTrace, int bitsDamageType );
	void EXPORT Smoke( void );
	static CRpgRocket *CreateRpgRocket( Vector vecOrigin, Vector vecAngles, CBaseEntity *pOwner, CRpg *pLauncher );

	int m_iTrail;			/* 0x2A8 */
	float m_flIgniteTime;		/* 0x2AC */
	CRpg *m_pLauncher;		/* 0x2B0 */
};

LINK_ENTITY_TO_CLASS( laser_spot, CLaserSpot );
LINK_ENTITY_TO_CLASS( weapon_rpg, CRpg );

TYPEDESCRIPTION	CRpg::m_SaveData[] =
{
	DEFINE_FIELD( CRpg, m_fSpotActive, FIELD_INTEGER ),
	DEFINE_FIELD( CRpg, m_cActiveRockets, FIELD_INTEGER ),
	DEFINE_FIELD( CRpg, m_flNextEmptySound, FIELD_TIME ),
	DEFINE_FIELD( CRpg, m_fSpotVisible, FIELD_INTEGER ),
};
IMPLEMENT_SAVERESTORE( CRpg, CBasePlayerWeapon );

//=========================================================
//=========================================================
CLaserSpot *CLaserSpot::CreateSpot( void )
{
	CLaserSpot *pSpot = GetClassPtr( (CLaserSpot *)NULL );
	pSpot->Spawn();

	pSpot->pev->classname = MAKE_STRING("laser_spot");

	return pSpot;
}

//=========================================================
//=========================================================
void CLaserSpot::Spawn( void )
{
	Precache( );
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_NOT;

	pev->rendermode = kRenderGlow;
	pev->renderfx = kRenderFxNoDissipation;
	pev->renderamt = 255;

	SET_MODEL(ENT(pev), "sprites/laserdot.spz");
	UTIL_SetOrigin( pev, pev->origin );
};

//=========================================================
// Suspend- make the laser sight invisible.
//=========================================================
void CLaserSpot::Suspend( float flSuspendTime )
{
	pev->effects |= EF_NODRAW;

	SetThink( &CLaserSpot::Revive );
	pev->nextthink = gpGlobals->time + flSuspendTime;
}

//=========================================================
// Revive - bring a suspended laser sight back.
//=========================================================
void CLaserSpot::Revive( void )
{
	pev->effects &= ~EF_NODRAW;

	SetThink( NULL );
}

void CLaserSpot::Precache( void )
{
	PRECACHE_MODEL("sprites/laserdot.spz");
};

LINK_ENTITY_TO_CLASS( rpg_rocket, CRpgRocket );

TYPEDESCRIPTION	CRpgRocket::m_SaveData[] =
{
	DEFINE_FIELD( CRpgRocket, m_flIgniteTime, FIELD_TIME ),
	DEFINE_FIELD( CRpgRocket, m_pLauncher, FIELD_CLASSPTR ),
};
IMPLEMENT_SAVERESTORE( CRpgRocket, CGrenade );

//=========================================================
//=========================================================
CRpgRocket *CRpgRocket::CreateRpgRocket( Vector vecOrigin, Vector vecAngles, CBaseEntity *pOwner, CRpg *pLauncher )
{
	CRpgRocket *pRocket = GetClassPtr( (CRpgRocket *)NULL );

	UTIL_SetOrigin( pRocket->pev, vecOrigin );
	pRocket->pev->angles = vecAngles;
	pRocket->Spawn();
	pRocket->SetTouch( &CRpgRocket::RocketTouch );
	pRocket->m_pLauncher = pLauncher;// remember what RPG fired me.
	pRocket->m_pLauncher->m_cActiveRockets++;// register this missile as active for the launcher
	pRocket->pev->owner = pOwner->edict();

	return pRocket;
}

//=========================================================
//=========================================================
void CRpgRocket :: Spawn( void )
{
	Precache( );
	// motor
	pev->movetype = MOVETYPE_BOUNCE;
	pev->solid = SOLID_BBOX;

	SET_MODEL(ENT(pev), "models/rpgrocket.dol");
	UTIL_SetSize(pev, Vector( 0, 0, 0), Vector(0, 0, 0));
	UTIL_SetOrigin( pev, pev->origin );

	pev->classname = MAKE_STRING("rpg_rocket");

	SetThink( &CRpgRocket::IgniteThink );
	SetTouch( &CRpgRocket::ExplodeTouch );

	pev->angles.x -= 30;
	UTIL_MakeVectors( pev->angles );
	pev->angles.x = -(pev->angles.x + 30);

	pev->velocity = gpGlobals->v_forward * 250;
	pev->gravity = 0.5f;

	pev->nextthink = gpGlobals->time + 0.4f;

	pev->dmg = gSkillData.plrDmgRPG;
}

//=========================================================
//=========================================================
void CRpgRocket :: RocketTouch ( CBaseEntity *pOther )
{
	if ( m_pLauncher )
	{
		// my launcher is still around, tell it I'm dead.
		m_pLauncher->m_cActiveRockets--;
	}

	STOP_SOUND( edict(), CHAN_VOICE, "weapons/rocket1.wav" );
	ExplodeTouch( pOther );
}

#ifdef NON_MATCHING
/* só o agendamento: o alvo carrega o pfnWriteByte depois da conversão
   float→int do (pev->dmg - 50) * 0.6 (mesmo problema do slave_collar) */
//=========================================================
// cópia do CGrenade::Explode no PS2 (sem puxar da parede)
//=========================================================
void CRpgRocket::Explode( TraceResult *pTrace, int bitsDamageType )
{
	float		flRndSound;// sound randomizer

	pev->model = iStringNull;//invisible
	pev->solid = SOLID_NOT;// intangible

	pev->takedamage = DAMAGE_NO;

	int iContents = UTIL_PointContents ( pev->origin );

	MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_EXPLOSION );		// This makes a dynamic light and the explosion sprites/sound
		WRITE_COORD( pev->origin.x );	// Send to PAS because of the sound
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		if (iContents != CONTENTS_WATER)
		{
			WRITE_SHORT( g_sModelIndexFireball );
		}
		else
		{
			WRITE_SHORT( g_sModelIndexWExplosion );
		}
		WRITE_BYTE( (byte)((pev->dmg - 50) * .60f) ); // scale * 10
		WRITE_BYTE( 15  ); // framerate
		WRITE_BYTE( TE_EXPLFLAG_NONE );
	MESSAGE_END();

	CSoundEnt::InsertSound ( bits_SOUND_COMBAT, pev->origin, NORMAL_EXPLOSION_VOLUME, 3.0 );
	entvars_t *pevOwner = VARS( pev->owner );

	pev->owner = NULL; // can't traceline attack owner if this is set

	RadiusDamage ( pev, pevOwner, pev->dmg, CLASS_NONE, bitsDamageType );

	if ( RANDOM_FLOAT( 0 , 1 ) < 0.5f )
	{
		UTIL_DecalTrace( pTrace, DECAL_SCORCH1 );
	}
	else
	{
		UTIL_DecalTrace( pTrace, DECAL_SCORCH2 );
	}

	flRndSound = RANDOM_FLOAT( 0 , 1 );

	switch ( RANDOM_LONG( 0, 2 ) )
	{
		case 0:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/debris1.wav", 0.55f, ATTN_NORM);	break;
		case 1:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/debris2.wav", 0.55f, ATTN_NORM);	break;
		case 2:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/debris3.wav", 0.55f, ATTN_NORM);	break;
	}

	pev->effects |= EF_NODRAW;
	SetThink( &CRpgRocket::Smoke );
	pev->velocity = g_vecZero;
	pev->nextthink = gpGlobals->time + 0.3f;

	if (iContents != CONTENTS_WATER)
	{
		int sparkCount = RANDOM_LONG(0,3);
		for ( int i = 0; i < sparkCount; i++ )
			Create( "spark_shower", pev->origin, pTrace->vecPlaneNormal, NULL );
	}
}
#else
INCLUDE_ASM("asm/nonmatchings/dlls/rpg", Explode__10CRpgRocketP11TraceResulti);
/* a constante do ponteiro para Smoke e a string "spark_shower" vão para o
   .rodata quando o Explode é compilado; esta cópia em seção descartada as
   coloca no mesmo lugar */
void RpgRocket_ExplodeRodata(CRpgRocket *p) __attribute__((section(".rpg_discard")));
void RpgRocket_ExplodeRodata(CRpgRocket *p)
{
	p->SetThink( &CRpgRocket::Smoke );
	CBaseEntity::Create( "spark_shower", p->pev->origin, p->pev->origin, NULL );
}
#endif

#ifdef NON_MATCHING
/* idem, com o (pev->dmg - 50) * 0.8 */
void CRpgRocket::Smoke( void )
{
	if (UTIL_PointContents ( pev->origin ) == CONTENTS_WATER)
	{
		UTIL_Bubbles( pev->origin - Vector( 64, 64, 64 ), pev->origin + Vector( 64, 64, 64 ), 100 );
	}
	else
	{
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
			WRITE_BYTE( TE_SMOKE );
			WRITE_COORD( pev->origin.x );
			WRITE_COORD( pev->origin.y );
			WRITE_COORD( pev->origin.z );
			WRITE_SHORT( g_sModelIndexSmoke );
			WRITE_BYTE( (byte)((pev->dmg - 50) * 0.80f) ); // scale * 10
			WRITE_BYTE( 12  ); // framerate
		MESSAGE_END();
	}
	SetThink( &CBaseEntity::SUB_Remove );
	pev->nextthink = gpGlobals->time + 10;
}
#else
INCLUDE_ASM("asm/nonmatchings/dlls/rpg", Smoke__10CRpgRocket);
/* idem para a constante do ponteiro para SUB_Remove */
void RpgRocket_SmokeRodata(CRpgRocket *p) __attribute__((section(".rpg_discard")));
void RpgRocket_SmokeRodata(CRpgRocket *p)
{
	p->SetThink( &CBaseEntity::SUB_Remove );
}
#endif

//=========================================================
//=========================================================
void CRpgRocket :: Precache( void )
{
	PRECACHE_MODEL("models/rpgrocket.dol");
	m_iTrail = PRECACHE_MODEL("sprites/smoke.spz");
	PRECACHE_SOUND ("weapons/rocket1.wav");
}


void CRpgRocket :: IgniteThink( void  )
{
	// pev->movetype = MOVETYPE_TOSS;

	pev->movetype = MOVETYPE_FLY;
	pev->effects |= EF_LIGHT;

	// make rocket sound
	EMIT_SOUND( ENT(pev), CHAN_VOICE, "weapons/rocket1.wav", 1, 0.5f );

	// rocket trail
	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );

		WRITE_BYTE( TE_BEAMFOLLOW );
		WRITE_SHORT(entindex());	// entity
		WRITE_SHORT(m_iTrail );	// model
		WRITE_BYTE( 40 ); // life
		WRITE_BYTE( 5 );  // width
		WRITE_BYTE( 224 );   // r, g, b
		WRITE_BYTE( 224 );   // r, g, b
		WRITE_BYTE( 255 );   // r, g, b
		WRITE_BYTE( 255 );	// brightness

	MESSAGE_END();  // move PHS/PVS data sending into here (SEND_ALL, SEND_PVS, SEND_PHS)

	m_flIgniteTime = gpGlobals->time;

	// set to follow laser spot
	SetThink( &CRpgRocket::FollowThink );
	pev->nextthink = gpGlobals->time + 0.1f;
}


void CRpgRocket :: FollowThink( void  )
{
	CBaseEntity *pOther = NULL;
	Vector vecTarget;
	Vector vecDir;
	float flDist, flMax, flDot;
	TraceResult tr;

	UTIL_MakeAimVectors( pev->angles );

	vecTarget = gpGlobals->v_forward;
	flMax = 4096;

	// Examine all entities within a reasonable radius
	while ((pOther = UTIL_FindEntityByClassname( pOther, "laser_spot" )) != NULL)
	{
		UTIL_TraceLine ( pev->origin, pOther->pev->origin, dont_ignore_monsters, ENT(pev), &tr );
		// ALERT( at_console, "%f\n", tr.flFraction );
		if (tr.flFraction >= 0.90f)
		{
			vecDir = pOther->pev->origin - pev->origin;
			flDist = vecDir.Length( );
			vecDir = vecDir.Normalize( );
			flDot = DotProduct( gpGlobals->v_forward, vecDir );
			if ((flDot > 0) && (flDist * (1 - flDot) < flMax))
			{
				flMax = flDist * (1 - flDot);
				vecTarget = vecDir;
			}
		}
	}

	pev->angles = UTIL_VecToAngles( vecTarget );

	// this acceleration and turning math is totally wrong, but it seems to respond well so don't change it.
	float flSpeed = pev->velocity.Length();
	if (gpGlobals->time - m_flIgniteTime < 1.0f)
	{
		pev->velocity = pev->velocity * 0.2f + vecTarget * (flSpeed * 0.8f + 400);
		if (pev->waterlevel == 3)
		{
			// go slow underwater
			if (pev->velocity.Length() > 300)
			{
				pev->velocity = pev->velocity.Normalize() * 300;
			}
			UTIL_BubbleTrail( pev->origin - pev->velocity * 0.1f, pev->origin, 4 );
		}
		else
		{
			if (pev->velocity.Length() > 2000)
			{
				pev->velocity = pev->velocity.Normalize() * 2000;
			}
		}
	}
	else
	{
		if (pev->effects & EF_LIGHT)
		{
			pev->effects = 0;
			STOP_SOUND( ENT(pev), CHAN_VOICE, "weapons/rocket1.wav" );
		}
		pev->velocity = pev->velocity * 0.2f + vecTarget * flSpeed * 0.798f;
		if (pev->waterlevel == 0 && pev->velocity.Length() < 1500)
		{
			Detonate( );
			if ( m_pLauncher )
			{
				// my launcher is still around, tell it I'm dead.
				m_pLauncher->m_cActiveRockets--;
			}
		}
	}

	pev->nextthink = gpGlobals->time + 0.1f;
}



void CRpg::Reload( void )
{
	int iResult = 0;

	if ( m_iClip == 1 )
	{
		// don't bother with any of this if don't need to reload.
		return;
	}

	// because the RPG waits to autoreload when no missiles are active while  the LTD is on, the
	// weapons code is constantly calling into this function, but is often denied because
	// a) missiles are in flight, but the LTD is on
	// or
	// b) player is totally out of ammo and has nothing to switch to, and should be allowed to
	//    shine the designator around
	//
	// Set the next attack time into the future so that WeaponIdle will get called more often
	// than reload, allowing the RPG LTD to be updated

	m_flNextPrimaryAttack = gpGlobals->time + 0.5f;

	if ( m_cActiveRockets && m_fSpotActive )
	{
		// no reloading when there are active missiles tracking the designator.
		// ward off future autoreload attempts by setting next attack time into the future for a bit.
		return;
	}

	if (m_pSpot && m_fSpotActive)
	{
		m_pSpot->Suspend( 2.1f );
		m_flNextSecondaryAttack = gpGlobals->time + 2.1f;
	}

	if (m_iClip == 0)
	{
		iResult = DefaultReload( RPG_MAX_CLIP, RPG_RELOAD, 2 );
	}

	if (iResult)
	{
		m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );
	}
}

void CRpg::Spawn( )
{
	pev->classname = MAKE_STRING("weapon_rpg");
	Precache( );
	m_iId = WEAPON_RPG;

	SET_MODEL(ENT(pev), "models/w_rpg.dol");
	m_fSpotActive = 1;

	if ( g_pGameRules->Ps2Virt07() )
	{
		// more default ammo in multiplay.
		m_iDefaultAmmo = RPG_DEFAULT_GIVE * 2;
	}
	else
	{
		m_iDefaultAmmo = RPG_DEFAULT_GIVE;
	}

	FallInit();// get ready to fall down.
	m_flNextEmptySound = 0;
}


void CRpg::Precache( void )
{
	PRECACHE_MODEL("models/w_rpg.dol");
	PRECACHE_MODEL("models/v_rpg.dol");
	PRECACHE_MODEL("models/p_rpg.dol");

	PRECACHE_SOUND("items/9mmclip1.wav");

	UTIL_PrecacheOther( "laser_spot" );
	UTIL_PrecacheOther( "rpg_rocket" );

	PRECACHE_SOUND("weapons/rocketfire1.wav");
	PRECACHE_SOUND("weapons/glauncher.wav"); // alternative fire sound
}


int CRpg::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "rockets";
	p->iMaxAmmo1 = ROCKET_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = RPG_MAX_CLIP;
	p->iSlot = 3;
	p->iPosition = 0;
	p->iId = m_iId = WEAPON_RPG;
	p->iFlags = 0;
	p->iWeight = RPG_WEIGHT;

	return 1;
}

int CRpg::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
		MESSAGE_END();
		return TRUE;
	}
	return FALSE;
}

BOOL CRpg::Deploy( )
{
	m_fSpotVisible = 1;

	if ( m_iClip == 0 )
	{
		return DefaultDeploy( "models/v_rpg.dol", "models/p_rpg.dol", RPG_DRAW_UL, "rpg" );
	}

	return DefaultDeploy( "models/v_rpg.dol", "models/p_rpg.dol", RPG_DRAW1, "rpg" );
}


BOOL CRpg::CanHolster( void )
{
	if ( m_fSpotActive && m_cActiveRockets )
	{
		// can't put away while guiding a missile.
		return FALSE;
	}

	return TRUE;
}

void CRpg::Holster( void )
{
	m_fInReload = FALSE;// cancel any reload in progress.

	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.5f;

	SendWeaponAnim( RPG_HOLSTER1 );
	if (m_pSpot)
	{
		m_pSpot->pev->effects |= EF_NODRAW;
		m_pSpot->Killed( NULL, GIB_NEVER );
		m_pSpot = NULL;
		m_fSpotVisible = 0;
	}
}



void CRpg::PrimaryAttack()
{
	if (m_iClip)
	{
		m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

		SendWeaponAnim( RPG_FIRE2 );

		// player "shoot" animation
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

		UTIL_MakeVectors( m_pPlayer->pev->v_angle );
		Vector vecSrc = m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -8;

		CRpgRocket *pRocket = CRpgRocket::CreateRpgRocket( vecSrc, m_pPlayer->pev->v_angle, m_pPlayer, this );
		func_002071C8( m_pPlayer, 1, 255, 125 );

		UTIL_MakeVectors( m_pPlayer->pev->v_angle );// RpgRocket::Create stomps on globals, so remake.
		pRocket->pev->velocity = pRocket->pev->velocity + gpGlobals->v_forward * DotProduct( m_pPlayer->pev->velocity, gpGlobals->v_forward );

		// firing RPG no longer turns on the designator. ALT fire is a toggle switch for the LTD.
		// Ken signed up for this as a global change (sjb)

		EMIT_SOUND(m_pPlayer->edict(), CHAN_WEAPON, "weapons/rocketfire1.wav", 0.9f, ATTN_NORM);
		EMIT_SOUND(m_pPlayer->edict(), CHAN_ITEM, "weapons/glauncher.wav", 0.7f, ATTN_NORM);

		m_iClip--;

		m_flNextPrimaryAttack = gpGlobals->time + 1.5f;
		m_flTimeWeaponIdle = gpGlobals->time + 1.5f;
		m_pPlayer->pev->punchangle.x -= 5;
	}
	else if (m_flNextEmptySound <= gpGlobals->time)
	{
		m_flNextEmptySound = gpGlobals->time + 1.5f;
		PlayEmptySound( );
	}
	UpdateSpot( );
}


void CRpg::SecondaryAttack()
{
	m_fSpotActive = ! m_fSpotActive;

	if (!m_fSpotActive && m_pSpot)
	{
		m_pSpot->Killed( NULL, GIB_NORMAL );
		m_pSpot = NULL;
	}

	m_flNextSecondaryAttack = gpGlobals->time + 0.2f;
}


void CRpg::WeaponIdle( void )
{
	m_pPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );

	UpdateSpot( );

	ResetEmptySound( );

	if (m_flTimeWeaponIdle > gpGlobals->time)
		return;

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
	{
		int iAnim;
		float flRand = RANDOM_FLOAT(0, 1);
		if (flRand <= 0.75f || m_fSpotActive)
		{
			if ( m_iClip == 0 )
				iAnim = RPG_IDLE_UL;
			else
				iAnim = RPG_IDLE;

			m_flTimeWeaponIdle = gpGlobals->time + 6.0000005f;
		}
		else
		{
			if ( m_iClip == 0 )
				iAnim = RPG_FIDGET_UL;
			else
				iAnim = RPG_FIDGET;

			m_flTimeWeaponIdle = gpGlobals->time + 3.0f;
		}

		SendWeaponAnim( iAnim );
	}
	else
	{
		m_flTimeWeaponIdle = gpGlobals->time + 1;
	}
}



void CRpg::UpdateSpot( void )
{
	if (m_fSpotActive && m_fSpotVisible)
	{
		if (!m_pSpot)
		{
			m_pSpot = CLaserSpot::CreateSpot();
		}

		UTIL_MakeVectors( m_pPlayer->pev->v_angle );
		Vector vecSrc = m_pPlayer->GetGunPosition( );;
		Vector vecAiming = gpGlobals->v_forward;

		TraceResult tr;
		UTIL_TraceLine ( vecSrc, vecSrc + vecAiming * 8192, dont_ignore_monsters, ENT(m_pPlayer->pev), &tr );

		UTIL_SetOrigin( m_pSpot->pev, tr.vecEndPos );
	}
}


class CRpgAmmo : public CBasePlayerAmmo
{
	void Spawn( void )
	{
		Precache( );
		SET_MODEL(ENT(pev), "models/w_rpgammo.dol");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_rpgammo.dol");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther )
	{
		int iGive;

		if ( g_pGameRules->Ps2Virt07() )
		{
			// hand out more ammo per rocket in multiplayer.
			iGive = AMMO_RPGCLIP_GIVE * 2;
		}
		else
		{
			iGive = AMMO_RPGCLIP_GIVE;
		}

		if (pOther->GiveAmmo( iGive, "rockets", ROCKET_MAX_CARRY ) != -1)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			return TRUE;
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS( ammo_rpgclip, CRpgAmmo );

/* registro dos ponteiros de método usados neste arquivo (chamado por
   func_00239870 no MethodPointerTable.cpp) */
extern "C" void func_002397D0(const char *name, BASEPTR pfn);
extern "C" void func_002397F0(const char *name, ENTITYFUNCPTR pfn);

extern "C" void func_001D9630(void)
{
	func_002397D0("244", static_cast<BASEPTR>(&CLaserSpot::Revive));
	func_002397D0("245", static_cast<BASEPTR>(&CRpgRocket::FollowThink));
	func_002397D0("246", static_cast<BASEPTR>(&CRpgRocket::IgniteThink));
	func_002397F0("247", static_cast<ENTITYFUNCPTR>(&CRpgRocket::ExplodeTouch));
	func_002397D0("346", static_cast<BASEPTR>(&CRpgRocket::Smoke));
	func_002397D0("347", static_cast<BASEPTR>(&CRpgRocket::SUB_Remove));
	func_002397F0("348", static_cast<ENTITYFUNCPTR>(&CRpgRocket::RocketTouch));
}
