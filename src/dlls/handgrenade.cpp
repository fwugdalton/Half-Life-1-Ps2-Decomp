/* dlls/handgrenade.cpp: a granada de mão (weapon_handgrenade) do HLSDK, na
   versão antiga (sem eventos, gpGlobals->time). O PS2 acrescenta o rumble do
   controle, a munição infinita do jogador e um arremesso mais fraco (força
   máxima 500 e só metade da velocidade do jogador). */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/weapons.h"
#include "dlls/player.h"
#include "include_asm.h"

#define WEAPON_HANDGRENADE          12
#define HANDGRENADE_WEIGHT          5
#define HANDGRENADE_MAX_CARRY       10
#define HANDGRENADE_DEFAULT_GIVE    5

extern "C" void func_002071C8(CBasePlayer *pPlayer, int iUnk, unsigned char intensity, unsigned short duration);

#define	HANDGRENADE_PRIMARY_VOLUME		450

enum handgrenade_e {
	HANDGRENADE_IDLE = 0,
	HANDGRENADE_FIDGET,
	HANDGRENADE_PINPULL,
	HANDGRENADE_THROW1,	// toss
	HANDGRENADE_THROW2,	// medium
	HANDGRENADE_THROW3,	// hard
	HANDGRENADE_HOLSTER,
	HANDGRENADE_DRAW
};

class CHandGrenade : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int iItemSlot( void ) { return 5; }
	int GetItemInfo(ItemInfo *p);

	void PrimaryAttack( void );
	BOOL Deploy( void );
	BOOL CanHolster( void );
	void Holster( void );
	void WeaponIdle( void );
	float m_flStartThrow;
	float m_flReleaseThrow;
};

LINK_ENTITY_TO_CLASS( weapon_handgrenade, CHandGrenade );


void CHandGrenade::Spawn( )
{
	Precache( );
	m_iId = WEAPON_HANDGRENADE;
	SET_MODEL(ENT(pev), "models/w_grenade.dol");

	pev->dmg = gSkillData.plrDmgHandGrenade;

	m_iDefaultAmmo = HANDGRENADE_DEFAULT_GIVE;

	FallInit();// get ready to fall down.
}


void CHandGrenade::Precache( void )
{
	PRECACHE_MODEL("models/w_grenade.dol");
	PRECACHE_MODEL("models/v_grenade.dol");
	PRECACHE_MODEL("models/p_grenade.dol");
}

int CHandGrenade::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "Hand Grenade";
	p->iMaxAmmo1 = HANDGRENADE_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 4;
	p->iPosition = 0;
	p->iId = m_iId = WEAPON_HANDGRENADE;
	p->iWeight = HANDGRENADE_WEIGHT;
	p->iFlags = ITEM_FLAG_LIMITINWORLD | ITEM_FLAG_EXHAUSTIBLE;

	return 1;
}


BOOL CHandGrenade::Deploy( )
{
	m_flReleaseThrow = -1;
	return DefaultDeploy( "models/v_grenade.dol", "models/p_grenade.dol", HANDGRENADE_DRAW, "crowbar" );
}

BOOL CHandGrenade::CanHolster( void )
{
	// can only holster hand grenades when not primed!
	return ( m_flStartThrow == 0 );
}

void CHandGrenade::Holster( )
{
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.5f;

	if ( m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] )
	{
		SendWeaponAnim( HANDGRENADE_HOLSTER );
	}
	else
	{
		// no more grenades!
		m_pPlayer->pev->weapons &= ~(1<<WEAPON_HANDGRENADE);
		SetThink( &CHandGrenade::DestroyItem );
		pev->nextthink = gpGlobals->time + 0.1f;
	}

	EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", 1.0f, ATTN_NORM);
}

void CHandGrenade::PrimaryAttack()
{
	if ( !m_flStartThrow && m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] > 0 )
	{
		m_flStartThrow = gpGlobals->time;
		m_flReleaseThrow = 0;

		SendWeaponAnim( HANDGRENADE_PINPULL );
		func_002071C8( m_pPlayer, 1, 80, 15 );
		m_flTimeWeaponIdle = gpGlobals->time + 0.5f;
	}
}


void CHandGrenade::WeaponIdle( void )
{
	m_pPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );

	if ( m_flReleaseThrow == 0 )
		 m_flReleaseThrow = gpGlobals->time;

	if ( m_flTimeWeaponIdle > gpGlobals->time )
		return;

	if ( m_flStartThrow )
	{
		Vector angThrow = m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle;

		if ( angThrow.x < 0 )
			angThrow.x = -10 + angThrow.x * ( ( 90 - 10 ) / 90.0f );
		else
			angThrow.x = -10 + angThrow.x * ( ( 90 + 10 ) / 90.0f );

		float flVel = ( 90 - angThrow.x ) * 4;
		if ( flVel > 500 )
			flVel = 500;

		UTIL_MakeVectors( angThrow );

		Vector vecSrc = m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_forward * 16;

		Vector vecThrow = gpGlobals->v_forward * flVel + m_pPlayer->pev->velocity * 0.5f;

		// alway explode 3 seconds after the pin was pulled
		float time = m_flStartThrow - gpGlobals->time + 3.0f;
		if (time < 0)
			time = 0;

		func_002071C8( m_pPlayer, 1, 255, 125 );
		CGrenade::ShootTimed( m_pPlayer->pev, vecSrc, vecThrow, time );

		if ( flVel < 500 )
		{
			SendWeaponAnim( HANDGRENADE_THROW1 );
		}
		else if ( flVel < 1000 )
		{
			SendWeaponAnim( HANDGRENADE_THROW2 );
		}
		else
		{
			SendWeaponAnim( HANDGRENADE_THROW3 );
		}

		// player "shoot" animation
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

		m_flStartThrow = 0;
		m_flNextPrimaryAttack = gpGlobals->time + 0.5f;
		m_flTimeWeaponIdle = gpGlobals->time + 0.5f;

		if ( !m_pPlayer->m_ps2InfiniteAmmo )
			m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ]--;

		if ( !m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] )
		{
			// just threw last grenade
			// set attack times in the future, and weapon idle in the future so we can see the whole throw
			// animation, weapon idle will automatically retire the weapon for us.
			m_flTimeWeaponIdle = m_flNextSecondaryAttack = m_flNextPrimaryAttack = gpGlobals->time + 0.5f;// ensure that the animation can finish playing
		}
		return;
	}
	else if ( m_flReleaseThrow > 0 )
	{
		// we've finished the throw, restart.
		m_flStartThrow = 0;

		if ( m_pPlayer->m_rgAmmo[ m_iPrimaryAmmoType ] )
		{
			SendWeaponAnim( HANDGRENADE_DRAW );
		}
		else
		{
			RetireWeapon();
			return;
		}

		m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT( 10, 15 );
		m_flReleaseThrow = -1;
		return;
	}

	if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] )
	{
		int iAnim;
		float flRand = RANDOM_FLOAT( 0, 1 );
		if (flRand <= 0.75f)
		{
			iAnim = HANDGRENADE_IDLE;
			m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT( 10, 15 );// how long till we do this again.
		}
		else 
		{
			iAnim = HANDGRENADE_FIDGET;
			m_flTimeWeaponIdle = gpGlobals->time + 75.0f / 30.0f;
		}

		SendWeaponAnim( iAnim );
	}
}

/* registro do ponteiro de Think para o save do PS2 */
extern "C" void func_002397D0(const char *name, BASEPTR pfn);

extern "C" void func_001810C0(void)
{
	func_002397D0("141", static_cast<BASEPTR>(&CHandGrenade::DestroyItem));
}
