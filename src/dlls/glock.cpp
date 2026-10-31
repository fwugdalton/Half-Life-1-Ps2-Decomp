/* dlls/glock.cpp: a pistola 9mm (weapon_glock / weapon_9mmhandgun) e o pente
   (ammo_glockclip / ammo_9mmclip) do HLSDK, na versão antiga (sem eventos,
   gpGlobals->time, cápsula e som no servidor). O PS2 tira a pistola com
   silenciador, acrescenta o rumble do controle e a munição infinita do cheat. */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/weapons.h"
#include "dlls/player.h"
#include "include_asm.h"

#define WEAPON_GLOCK            2
#define GLOCK_MAX_CLIP          17
#define GLOCK_DEFAULT_GIVE      17
#define _9MM_MAX_CARRY          250
#define GLOCK_WEIGHT            10
#define AMMO_GLOCKCLIP_GIVE     GLOCK_MAX_CLIP

#define TE_BOUNCE_SHELL         1

extern "C" void func_002071C8(CBasePlayer *pPlayer, int iUnk, unsigned char intensity, unsigned short duration);
extern void EjectBrass(const Vector &vecOrigin, const Vector &vecVelocity, float rotation, int model, int soundtype);

enum glock_e {
	GLOCK_IDLE1 = 0,
	GLOCK_IDLE2,
	GLOCK_IDLE3,
	GLOCK_SHOOT,
	GLOCK_SHOOT_EMPTY,
	GLOCK_RELOAD,
	GLOCK_RELOAD_NOT_EMPTY,
	GLOCK_DRAW,
	GLOCK_HOLSTER,
	GLOCK_ADD_SILENCER
};

class CGlock : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int iItemSlot( void ) { return 2; }
	int GetItemInfo(ItemInfo *p);

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	void GlockFire( float flSpread, float flCycleTime, BOOL fUseAutoAim );
	BOOL Deploy( void );
	void Reload( void );
	void WeaponIdle( void );

private:
	int m_iShell;
};
LINK_ENTITY_TO_CLASS( weapon_glock, CGlock );
LINK_ENTITY_TO_CLASS( weapon_9mmhandgun, CGlock );


void CGlock::Spawn( )
{
	pev->classname = MAKE_STRING("weapon_9mmhandgun"); // hack to allow for old names
	Precache( );
	m_iId = WEAPON_GLOCK;
	SET_MODEL(ENT(pev), "models/w_9mmhandgun.dol");

	m_iDefaultAmmo = GLOCK_DEFAULT_GIVE;

	FallInit();// get ready to fall down.
}


void CGlock::Precache( void )
{
	PRECACHE_MODEL("models/v_9mmhandgun.dol");
	PRECACHE_MODEL("models/w_9mmhandgun.dol");
	PRECACHE_MODEL("models/p_9mmhandgun.dol");

	m_iShell = PRECACHE_MODEL ("models/shell.dol");// brass shell

	PRECACHE_SOUND("items/9mmclip1.wav");
	PRECACHE_SOUND("items/9mmclip2.wav");

	PRECACHE_SOUND ("weapons/pl_gun3.wav");//handgun
}

int CGlock::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "9mm";
	p->iMaxAmmo1 = _9MM_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = GLOCK_MAX_CLIP;
	p->iSlot = 1;
	p->iPosition = 0;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_GLOCK;
	p->iWeight = GLOCK_WEIGHT;

	return 1;
}

BOOL CGlock::Deploy( )
{
	// pev->body = 1;
	return DefaultDeploy( "models/v_9mmhandgun.dol", "models/p_9mmhandgun.dol", GLOCK_DRAW, "onehanded" );
}

void CGlock::SecondaryAttack( void )
{
	GlockFire( 0.1f, 0.2f, FALSE );
}

void CGlock::PrimaryAttack( void )
{
	GlockFire( 0.01f, 0.3f, TRUE );
}

void CGlock::GlockFire( float flSpread , float flCycleTime, BOOL fUseAutoAim )
{
	if (m_iClip <= 0)
	{
		if (m_fFireOnEmpty)
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = gpGlobals->time + 0.2f;
		}

		return;
	}

	m_iClip--;

	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	if (m_iClip != 0)
		SendWeaponAnim( GLOCK_SHOOT );
	else
		SendWeaponAnim( GLOCK_SHOOT_EMPTY );

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
		
	Vector	vecShellVelocity = m_pPlayer->pev->velocity 
							 + gpGlobals->v_right * RANDOM_FLOAT(50,70) 
							 + gpGlobals->v_up * RANDOM_FLOAT(100,150) 
							 + gpGlobals->v_forward * 25;
	EjectBrass ( m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_up * -12 + gpGlobals->v_forward * 32 + gpGlobals->v_right * 6 , vecShellVelocity, pev->angles.y, m_iShell, TE_BOUNCE_SHELL ); 

	// non-silenced
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/pl_gun3.wav", RANDOM_FLOAT(0.92f, 1.0f), ATTN_NORM, 0, 98 + RANDOM_LONG(0,3));

	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming;
	
	if ( fUseAutoAim )
	{
		vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );
	}
	else
	{
		vecAiming = gpGlobals->v_forward;
	}

	func_002071C8( m_pPlayer, 1, 120, 50 );

	m_pPlayer->FireBullets( 1, vecSrc, vecAiming, Vector( flSpread, flSpread, flSpread ), 8192, BULLET_PLAYER_9MM, 0 );

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + flCycleTime;

	if (!m_iClip && !m_pPlayer->m_ps2InfiniteAmmo && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );

	m_pPlayer->pev->punchangle.x -= 2;
}


void CGlock::Reload( void )
{
	int iResult = 0;

	if (m_iClip == 0)
		iResult = DefaultReload( 17, GLOCK_RELOAD, 1.5 );
	else if (m_iClip != 17)
		iResult = DefaultReload( 18, GLOCK_RELOAD_NOT_EMPTY, 1.5 );

	if (iResult)
	{
		m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );
	}
}



void CGlock::WeaponIdle( void )
{
	ResetEmptySound( );

	m_pPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );

	if (m_flTimeWeaponIdle > gpGlobals->time)
		return;

	// only idle if the slid isn't back
	if (m_iClip != 0)
	{
		int iAnim;
		float flRand = RANDOM_FLOAT(0, 1);
		if (flRand <= 0.3f)
		{
			iAnim = GLOCK_IDLE3;
			m_flTimeWeaponIdle = gpGlobals->time + 49.0f / 16;
		}
		else if (flRand <= 0.6f)
		{
			iAnim = GLOCK_IDLE1;
			m_flTimeWeaponIdle = gpGlobals->time + 60.0f / 16.0f;
		}
		else
		{
			iAnim = GLOCK_IDLE2;
			m_flTimeWeaponIdle = gpGlobals->time + 40.0f / 16.0f;
		}
		SendWeaponAnim( iAnim );
	}
}








class CGlockAmmo : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_9mmclip.dol");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_9mmclip.dol");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		if (pOther->GiveAmmo( AMMO_GLOCKCLIP_GIVE, "9mm", _9MM_MAX_CARRY ) != -1)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			return TRUE;
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS( ammo_glockclip, CGlockAmmo );
LINK_ENTITY_TO_CLASS( ammo_9mmclip, CGlockAmmo );
