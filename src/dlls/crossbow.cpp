/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
/* crossbow.cpp do PS2: versão antiga do SDK (sem eventos de cliente), com
   vibração do controle no FireBolt, estatísticas do placar (m_ps2ShotsFired/Hit)
   e m_fInZoom salvo. */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/weapons.h"
#include "dlls/player.h"
#include "dlls/skill.h"
#include "dlls/gamerules.h"
#include "include_asm.h"

#define WEAPON_CROSSBOW			6
#define CROSSBOW_WEIGHT			10
#define CROSSBOW_MAX_CLIP		5
#define CROSSBOW_DEFAULT_GIVE	5
#define BOLT_MAX_CARRY			50
#define AMMO_CROSSBOWCLIP_GIVE	CROSSBOW_MAX_CLIP

#define TE_EXPLOSION		3
#define TE_EXPLFLAG_NONE	0
#define GIB_NEVER			1
#define AUTOAIM_2DEGREES	0.0348994967025f

extern "C" void func_002071C8(CBasePlayer *pPlayer, int iUnk, unsigned char intensity, unsigned short duration);
extern "C" int cl_numlocal;
extern "C" void func_00156590( void );
extern TraceResult UTIL_GetGlobalTrace( void );
extern int UTIL_PointContents( const Vector &vec );
extern void UTIL_BubbleTrail( Vector from, Vector to, int count );
extern void RadiusDamage( Vector vecSrc, entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, float flRadius, int iClassIgnore, int bitsDamageType );
extern int gmsgScoreInfo;

#define BOLT_AIR_VELOCITY	2000
#define BOLT_WATER_VELOCITY	1000

// UNDONE: Save/restore this?  Don't forget to set classname and LINK_ENTITY_TO_CLASS()
// 
// OVERLOADS SOME ENTVARS:
//
// speed - the ideal magnitude of my velocity
class CCrossbowBolt : public CBaseEntity
{
	/* o registro do PS2 (func_00156590) é amigo para pegar os ponteiros */
	friend void func_00156590( void );
	void Spawn( void );
	void Precache( void );
	int  Classify ( void );
	void EXPORT BubbleThink( void );
	void EXPORT BoltTouch( CBaseEntity *pOther );
	void EXPORT ExplodeThink( void );

	int m_iTrail;

public:
	static CCrossbowBolt *BoltCreate( void );
};
LINK_ENTITY_TO_CLASS( crossbow_bolt, CCrossbowBolt );

CCrossbowBolt *CCrossbowBolt::BoltCreate( void )
{
	// Create a new entity with CCrossbowBolt private data
	CCrossbowBolt *pBolt = GetClassPtr( (CCrossbowBolt *)NULL );
	pBolt->pev->classname = MAKE_STRING("bolt");
	pBolt->Spawn();

	return pBolt;
}

void CCrossbowBolt::Spawn( )
{
	Precache( );
	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_BBOX;

	pev->gravity = 0.5f;

	SET_MODEL(ENT(pev), "models/crossbow_bolt.dol");

	UTIL_SetOrigin( pev, pev->origin );
	UTIL_SetSize(pev, Vector(0, 0, 0), Vector(0, 0, 0));

	SetTouch( &CCrossbowBolt::BoltTouch );
	SetThink( &CCrossbowBolt::BubbleThink );
	pev->nextthink = gpGlobals->time + 0.2f;
}


void CCrossbowBolt::Precache( )
{
	PRECACHE_MODEL ("models/crossbow_bolt.dol");
	PRECACHE_SOUND("weapons/xbow_hitbod1.wav");
	PRECACHE_SOUND("weapons/xbow_hitbod2.wav");
	PRECACHE_SOUND("weapons/xbow_fly1.wav");
	PRECACHE_SOUND("weapons/xbow_hit1.wav");
	PRECACHE_SOUND("fvox/beep.wav");
	m_iTrail = PRECACHE_MODEL("sprites/streak.spz");
}


int	CCrossbowBolt :: Classify ( void )
{
	return	CLASS_NONE;
}

void CCrossbowBolt::BoltTouch( CBaseEntity *pOther )
{
	SetTouch( NULL );
	SetThink( NULL );

	// PS2: quem atirou, para as estatísticas do placar
	CBasePlayer *pPlayer = (CBasePlayer *)CBaseEntity::Instance( pev->owner );
	if ( !pPlayer->IsPlayer() )
		pPlayer = NULL;

	if (pOther->pev->takedamage)
	{
		TraceResult tr = UTIL_GetGlobalTrace( );
		entvars_t	*pevOwner;

		pevOwner = VARS( pev->owner );

		// UNDONE: this needs to call TraceAttack instead
		ClearMultiDamage( );

		if ( pOther->IsPlayer() )
		{
			pOther->TraceAttack(pevOwner, gSkillData.plrDmgCrossbowClient, pev->velocity.Normalize(), &tr, DMG_NEVERGIB ); 
		}
		else
		{
			pOther->TraceAttack(pevOwner, gSkillData.plrDmgCrossbowMonster, pev->velocity.Normalize(), &tr, DMG_BULLET | DMG_NEVERGIB ); 
		}

		ApplyMultiDamage( pev, pevOwner );

		pev->velocity = Vector( 0, 0, 0 );
		// play body "thwack" sound
		switch( RANDOM_LONG(0,1) )
		{
		case 0:
			EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/xbow_hitbod1.wav", 1, ATTN_NORM); break;
		case 1:
			EMIT_SOUND(ENT(pev), CHAN_WEAPON, "weapons/xbow_hitbod2.wav", 1, ATTN_NORM); break;
		}

		if ( !g_pGameRules->Ps2Virt07() )
		{
			Killed( pev, GIB_NEVER );
		}

		if ( pPlayer && pOther->MyMonsterPointer() && g_pGameRules->FPlayerCanTakeDamage( pPlayer, pOther ) )
			pPlayer->m_ps2ShotsHit++;
	}
	else
	{
		EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, "weapons/xbow_hit1.wav", RANDOM_FLOAT(0.95f, 1.0f), ATTN_NORM, 0, 98 + RANDOM_LONG(0,7));

		SetThink( &CCrossbowBolt::SUB_Remove );
		pev->nextthink = gpGlobals->time;// this will get changed below if the bolt is allowed to stick in what it hit.

		if ( FClassnameIs( pOther->pev, "worldspawn" ) )
		{
			// if what we hit is static architecture, can stay around for a while.
			Vector vecDir = pev->velocity.Normalize( );
			UTIL_SetOrigin( pev, pev->origin - vecDir * 12 );
			pev->angles = UTIL_VecToAngles( vecDir );
			pev->solid = SOLID_NOT;
			pev->movetype = MOVETYPE_FLY;
			pev->velocity = Vector( 0, 0, 0 );
			pev->avelocity.z = 0;
			pev->angles.z = RANDOM_LONG(0,360);
			pev->nextthink = gpGlobals->time + 10.0f;
		}

		if (UTIL_PointContents(pev->origin) != CONTENTS_WATER)
		{
			UTIL_Sparks( pev->origin );
		}
	}

	if ( g_pGameRules->Ps2Virt07() )
	{
		SetThink( &CCrossbowBolt::ExplodeThink );
		pev->nextthink = gpGlobals->time + 0.1f;
	}

	if ( pPlayer && ( g_pGameRules->IsMultiplayer() || g_pGameRules->Ps2Virt07() ) )
	{
		if ( g_pGameRules->IsMultiplayer() && cl_numlocal == 1 )
		{
			MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
				WRITE_BYTE( pPlayer->m_ps2ClientIndex );
				WRITE_SHORT( pPlayer->pev->frags );
				WRITE_SHORT( pPlayer->m_iDeaths );
				WRITE_SHORT( pPlayer->m_ps2Stat5F8 );
				WRITE_SHORT( pPlayer->m_ps2ShotsFired );
				WRITE_SHORT( pPlayer->m_ps2ShotsHit );
			MESSAGE_END();
		}
		else
		{
			MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
				WRITE_BYTE( ENTINDEX( pPlayer->edict() ) );
				WRITE_SHORT( pPlayer->pev->frags );
				WRITE_SHORT( pPlayer->m_iDeaths );
				WRITE_SHORT( pPlayer->m_ps2Stat5F8 );
				WRITE_SHORT( pPlayer->m_ps2ShotsFired );
				WRITE_SHORT( pPlayer->m_ps2ShotsHit );
			MESSAGE_END();
		}
	}
}

void CCrossbowBolt::BubbleThink( void )
{
	pev->nextthink = gpGlobals->time + 0.1f;

	if (pev->waterlevel == 0)
		return;

	UTIL_BubbleTrail( pev->origin - pev->velocity * 0.1f, pev->origin, 1 );
}

void CCrossbowBolt::ExplodeThink( void )
{
	int iContents = UTIL_PointContents ( pev->origin );
	int iScale;
	
	pev->dmg = 40;
	iScale = 10;

	MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_EXPLOSION);		
		WRITE_COORD( pev->origin.x );
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
		WRITE_BYTE( iScale  ); // scale * 10
		WRITE_BYTE( 15  ); // framerate
		WRITE_BYTE( TE_EXPLFLAG_NONE );
	MESSAGE_END();

	entvars_t *pevOwner;

	if ( pev->owner )
		pevOwner = VARS( pev->owner );
	else
		pevOwner = NULL;

	pev->owner = NULL; // can't traceline attack owner if this is set

	::RadiusDamage( pev->origin, pev, pevOwner, pev->dmg, 128, CLASS_NONE, DMG_BLAST | DMG_ALWAYSGIB );

	UTIL_Remove(this);
}

enum crossbow_e {
	CROSSBOW_IDLE1 = 0,	// full
	CROSSBOW_IDLE2,		// empty
	CROSSBOW_FIDGET1,	// full
	CROSSBOW_FIDGET2,	// empty
	CROSSBOW_FIRE1,		// full
	CROSSBOW_FIRE2,		// reload
	CROSSBOW_FIRE3,		// empty
	CROSSBOW_RELOAD,	// from empty
	CROSSBOW_DRAW1,		// full
	CROSSBOW_DRAW2,		// empty
	CROSSBOW_HOLSTER1,	// full
	CROSSBOW_HOLSTER2,	// empty
};

class CCrossbow : public CBasePlayerWeapon
{
public:
	int		Save( CSave &save );
	int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	void Spawn( void );
	void Precache( void );
	int iItemSlot( ) { return 3; }
	int GetItemInfo(ItemInfo *p);

	void FireBolt( void );
	void FireSniperBolt( void );
	void PrimaryAttack( void );
	void SecondaryAttack( void );
	int AddToPlayer( CBasePlayer *pPlayer );
	BOOL Deploy( );
	void Holster( void );
	void Reload( void );
	void WeaponIdle( void );

	int m_fInZoom; // don't save this
};

LINK_ENTITY_TO_CLASS( weapon_crossbow, CCrossbow );

TYPEDESCRIPTION	CCrossbow::m_SaveData[] = 
{
	DEFINE_FIELD( CCrossbow, m_fInZoom, FIELD_INTEGER ),
};
IMPLEMENT_SAVERESTORE( CCrossbow, CBasePlayerWeapon );

void CCrossbow::Spawn( )
{
	Precache( );
	pev->classname = MAKE_STRING("weapon_crossbow"); // hack to allow for old names
	m_iId = WEAPON_CROSSBOW;
	SET_MODEL(ENT(pev), "models/w_crossbow.dol");

	m_iDefaultAmmo = CROSSBOW_DEFAULT_GIVE;

	FallInit();// get ready to fall down.
}

int CCrossbow::AddToPlayer( CBasePlayer *pPlayer )
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

void CCrossbow::Precache( void )
{
	PRECACHE_MODEL("models/w_crossbow.dol");
	PRECACHE_MODEL("models/v_crossbow.dol");
	PRECACHE_MODEL("models/p_crossbow.dol");

	PRECACHE_SOUND("weapons/xbow_fire1.wav");
	PRECACHE_SOUND("weapons/xbow_reload1.wav");

	UTIL_PrecacheOther( "crossbow_bolt" );
}


int CCrossbow::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "bolts";
	p->iMaxAmmo1 = BOLT_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = CROSSBOW_MAX_CLIP;
	p->iSlot = 2;
	p->iPosition = 2;
	p->iId = WEAPON_CROSSBOW;
	p->iFlags = 0;
	p->iWeight = CROSSBOW_WEIGHT;
	return 1;
}


BOOL CCrossbow::Deploy( )
{
	if (m_iClip)
		return DefaultDeploy( "models/v_crossbow.dol", "models/p_crossbow.dol", CROSSBOW_DRAW1, "bow" );
	return DefaultDeploy( "models/v_crossbow.dol", "models/p_crossbow.dol", CROSSBOW_DRAW2, "bow" );
}

void CCrossbow::Holster( )
{
	m_fInReload = FALSE;// cancel any reload in progress.

	if ( m_fInZoom )
	{
		SecondaryAttack( );
	}

	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.5f;
	if (m_iClip)
		SendWeaponAnim( CROSSBOW_HOLSTER1 );
	else
		SendWeaponAnim( CROSSBOW_HOLSTER2 );
}

void CCrossbow::PrimaryAttack( void )
{
	m_pPlayer->m_ps2ShotsFired++;

	if ( m_fInZoom && g_pGameRules->Ps2Virt07() )
	{
		FireSniperBolt();
		return;
	}

	FireBolt();
}

// this function only gets called in multiplayer
void CCrossbow::FireSniperBolt()
{
	m_flNextPrimaryAttack = gpGlobals->time + 0.75f;

	if (m_iClip == 0)
	{
		PlayEmptySound( );
		return;
	}

	TraceResult tr;

	m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;
	m_iClip--;

	// make twang sound
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/xbow_fire1.wav", RANDOM_FLOAT(0.95f, 1.0f), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));

	if (m_iClip)
	{
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/xbow_reload1.wav", RANDOM_FLOAT(0.95f, 1.0f), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		SendWeaponAnim( CROSSBOW_FIRE1 );
	}
	else if ( !m_pPlayer->m_ps2InfiniteAmmo && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] == 0 )
	{
		SendWeaponAnim( CROSSBOW_FIRE3 );
	}

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
	
	Vector anglesAim = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
	UTIL_MakeVectors( anglesAim );
	Vector vecSrc = m_pPlayer->GetGunPosition( ) - gpGlobals->v_up * 2;
	Vector vecDir = gpGlobals->v_forward;

	UTIL_TraceLine(vecSrc, vecSrc + vecDir * 8192, dont_ignore_monsters, m_pPlayer->edict(), &tr);

	if ( tr.pHit->v.takedamage )
	{
		switch( RANDOM_LONG(0,1) )
		{
		case 0:
			EMIT_SOUND( tr.pHit, CHAN_BODY, "weapons/xbow_hitbod1.wav", 1, ATTN_NORM); break;
		case 1:
			EMIT_SOUND( tr.pHit, CHAN_BODY, "weapons/xbow_hitbod2.wav", 1, ATTN_NORM); break;
		}

		ClearMultiDamage( );
		CBaseEntity::Instance(tr.pHit)->TraceAttack(m_pPlayer->pev, 120, vecDir, &tr, DMG_BULLET | DMG_NEVERGIB ); 
		ApplyMultiDamage( pev, m_pPlayer->pev );

		CBaseEntity *pHit = CBaseEntity::Instance(tr.pHit);
		if ( pHit->MyMonsterPointer() && g_pGameRules->FPlayerCanTakeDamage( m_pPlayer, pHit ) )
			m_pPlayer->m_ps2ShotsHit++;
	}
	else
	{
		// create a bolt
		CCrossbowBolt *pBolt = CCrossbowBolt::BoltCreate();
		pBolt->pev->origin = tr.vecEndPos - vecDir * 10;
		pBolt->pev->angles = UTIL_VecToAngles( vecDir );
		pBolt->pev->solid = SOLID_NOT;
		pBolt->SetTouch( NULL );
		pBolt->SetThink( &CCrossbowBolt::SUB_Remove );

		EMIT_SOUND( pBolt->edict(), CHAN_WEAPON, "weapons/xbow_hit1.wav", RANDOM_FLOAT(0.95f, 1.0f), ATTN_NORM );

		if (UTIL_PointContents(tr.vecEndPos) != CONTENTS_WATER)
		{
			UTIL_Sparks( tr.vecEndPos );
		}

		if ( FClassnameIs( tr.pHit, "worldspawn" ) )
		{
			// let the bolt sit around for a while if it hit static architecture
			pBolt->pev->nextthink = gpGlobals->time + 5.0f;
		}
		else
		{
			pBolt->pev->nextthink = gpGlobals->time;
		}
	}

	if ( g_pGameRules->IsMultiplayer() || g_pGameRules->Ps2Virt07() )
	{
		if ( g_pGameRules->IsMultiplayer() && cl_numlocal == 1 )
		{
			MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
				WRITE_BYTE( m_pPlayer->m_ps2ClientIndex );
				WRITE_SHORT( m_pPlayer->pev->frags );
				WRITE_SHORT( m_pPlayer->m_iDeaths );
				WRITE_SHORT( m_pPlayer->m_ps2Stat5F8 );
				WRITE_SHORT( m_pPlayer->m_ps2ShotsFired );
				WRITE_SHORT( m_pPlayer->m_ps2ShotsHit );
			MESSAGE_END();
		}
		else
		{
			MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
				WRITE_BYTE( ENTINDEX( m_pPlayer->edict() ) );
				WRITE_SHORT( m_pPlayer->pev->frags );
				WRITE_SHORT( m_pPlayer->m_iDeaths );
				WRITE_SHORT( m_pPlayer->m_ps2Stat5F8 );
				WRITE_SHORT( m_pPlayer->m_ps2ShotsFired );
				WRITE_SHORT( m_pPlayer->m_ps2ShotsHit );
			MESSAGE_END();
		}
	}
}

void CCrossbow::FireBolt()
{
	if (m_iClip == 0)
	{
		PlayEmptySound( );
		return;
	}

	// PS2: vibração do controle
	func_002071C8( m_pPlayer, 1, RANDOM_LONG(50,200), 50 );

	m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;

	m_iClip--;

	// make twang sound
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/xbow_fire1.wav", RANDOM_FLOAT(0.95f, 1.0f), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));

	if (m_iClip)
	{
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/xbow_reload1.wav", RANDOM_FLOAT(0.95f, 1.0f), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
		SendWeaponAnim( CROSSBOW_FIRE1 );
	}
	else if ( !m_pPlayer->m_ps2InfiniteAmmo && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] == 0 )
	{
		SendWeaponAnim( CROSSBOW_FIRE3 );
	}

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	Vector anglesAim = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;
	UTIL_MakeVectors( anglesAim );
	
	anglesAim.x		= -anglesAim.x;
	Vector vecSrc	 = m_pPlayer->GetGunPosition( ) - gpGlobals->v_up * 2;
	Vector vecDir	 = gpGlobals->v_forward;

	CCrossbowBolt *pBolt = CCrossbowBolt::BoltCreate();
	UTIL_SetOrigin( pBolt->pev, vecSrc );
	pBolt->pev->angles = anglesAim;
	pBolt->pev->owner = m_pPlayer->edict();

	if (m_pPlayer->pev->waterlevel == 3)
	{
		pBolt->pev->velocity = vecDir * BOLT_WATER_VELOCITY;
		pBolt->pev->speed = BOLT_WATER_VELOCITY;
	}
	else
	{
		pBolt->pev->velocity = vecDir * BOLT_AIR_VELOCITY;
		pBolt->pev->speed = BOLT_AIR_VELOCITY;
	}
	pBolt->pev->avelocity.z = 10;

	if (!m_iClip && !m_pPlayer->m_ps2InfiniteAmmo && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	m_flNextPrimaryAttack = gpGlobals->time + 0.75f;

	m_flNextSecondaryAttack = gpGlobals->time + 0.75f;

	if (m_iClip != 0)
		m_flTimeWeaponIdle = gpGlobals->time + 5.0f;
	else
		m_flTimeWeaponIdle = 0.75f;

	m_pPlayer->pev->punchangle.x -= 2;
}


void CCrossbow::SecondaryAttack()
{
	if (m_fInZoom)
	{
		m_pPlayer->m_iFOV = 0;  // 0 means reset to default fov
		m_fInZoom = 0;
	}
	else
	{
		m_pPlayer->m_iFOV = 20;
		m_fInZoom = 1;
	}
	
	pev->nextthink = gpGlobals->time + 0.1f;
	m_flNextSecondaryAttack = gpGlobals->time + 1.0f;
}


void CCrossbow::Reload( void )
{
	if ( m_fInZoom )
	{
		SecondaryAttack();
	}

	if ( DefaultReload( 5, CROSSBOW_RELOAD, 4.5f ) )
	{
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/xbow_reload1.wav", RANDOM_FLOAT(0.95f, 1.0f), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0xF));
	}
}


void CCrossbow::WeaponIdle( void )
{
	ResetEmptySound( );

	m_pPlayer->GetAutoaimVector( AUTOAIM_2DEGREES );  // get the autoaim vector but ignore it;  used for autoaim crosshair in DM
	
	if ( m_flTimeWeaponIdle < gpGlobals->time )
	{
		float flRand = RANDOM_FLOAT(0, 1);
		if (flRand <= 0.75f)
		{
			if (m_iClip)
			{
				SendWeaponAnim( CROSSBOW_IDLE1 );
			}
			else
			{
				SendWeaponAnim( CROSSBOW_IDLE2 );
			}
			m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT( 10, 15 );
		}
		else
		{
			if (m_iClip)
			{
				SendWeaponAnim( CROSSBOW_FIDGET1 );
				m_flTimeWeaponIdle = gpGlobals->time + 90.0f / 30.0f;
			}
			else
			{
				SendWeaponAnim( CROSSBOW_FIDGET2 );
				m_flTimeWeaponIdle = gpGlobals->time + 80.0f / 30.0f;
			}
		}
	}
}



class CCrossbowAmmo : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_crossbow_clip.dol");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_crossbow_clip.dol");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		if (pOther->GiveAmmo( AMMO_CROSSBOWCLIP_GIVE, "bolts", BOLT_MAX_CARRY ) != -1)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			return TRUE;
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS( ammo_crossbow, CCrossbowAmmo );

/* registro dos ponteiros de função do PS2 (MethodPointerTable) */
extern "C" void func_002397D0( const char *name, BASEPTR pfn );
extern "C" void func_002397F0( const char *name, ENTITYFUNCPTR pfn );
extern "C" void func_00156590( void )
{
	func_002397D0( "070", static_cast<BASEPTR>( &CCrossbowBolt::SUB_Remove ) );
	func_002397D0( "071", static_cast<BASEPTR>( &CCrossbowBolt::BubbleThink ) );
	func_002397D0( "072", static_cast<BASEPTR>( &CCrossbowBolt::ExplodeThink ) );
	func_002397D0( "073", static_cast<BASEPTR>( &CCrossbowBolt::SUB_Remove ) );
	func_002397F0( "074", static_cast<ENTITYFUNCPTR>( &CCrossbowBolt::BoltTouch ) );
}
