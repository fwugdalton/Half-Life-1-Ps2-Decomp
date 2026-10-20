/* dlls/shotgun.cpp: espingarda (weapon_shotgun) e a caixa de cartuchos
   (ammo_buckshot) do HLSDK, na versão antiga (sem eventos, gpGlobals->time).
   O PS2 acrescenta o rumble do controle e a munição infinita do cheat. */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/weapons.h"
#include "dlls/player.h"
#include "dlls/gamerules.h"

#define WEAPON_SHOTGUN          7
#define SHOTGUN_MAX_CLIP        8
#define SHOTGUN_DEFAULT_GIVE    12
#define BUCKSHOT_MAX_CARRY      125
#define SHOTGUN_WEIGHT          15
#define AMMO_BUCKSHOTBOX_GIVE   12

#define TE_BOUNCE_SHOTSHELL     2
#define AUTOAIM_5DEGREES        0.08715574274766f

#define VECTOR_CONE_10DEGREES   Vector( 0.08716f, 0.08716f, 0.08716f )

// special deathmatch shotgun spreads
#define VECTOR_CONE_DM_SHOTGUN	Vector( 0.08716f, 0.04362f, 0.00f  )// 10 degrees by 5 degrees
#define VECTOR_CONE_DM_DOUBLESHOTGUN Vector( 0.17365f, 0.04362f, 0.00f ) // 20 degrees by 5 degrees

extern "C" void func_002071C8(CBasePlayer *pPlayer, int iUnk, unsigned char intensity, unsigned short duration);
extern "C" void func_00113D08(float flTime, float flDelay, int iPlayer);
extern void EjectBrass(const Vector &vecOrigin, const Vector &vecVelocity, float rotation, int model, int soundtype);

enum shotgun_e {
	SHOTGUN_IDLE = 0,
	SHOTGUN_FIRE,
	SHOTGUN_FIRE2,
	SHOTGUN_RELOAD,
	SHOTGUN_PUMP,
	SHOTGUN_START_RELOAD,
	SHOTGUN_DRAW,
	SHOTGUN_HOLSTER,
	SHOTGUN_IDLE4,
	SHOTGUN_IDLE_DEEP
};

class CShotgun : public CBasePlayerWeapon
{
public:
	int		Save( CSave &save );
	int		Restore( CRestore &restore );
	static	TYPEDESCRIPTION m_SaveData[];

	void Spawn( void );
	void Precache( void );
	int iItemSlot( ) { return 3; }
	int GetItemInfo(ItemInfo *p);
	int AddToPlayer( CBasePlayer *pPlayer );

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	BOOL Deploy( );
	void Reload( void );
	void WeaponIdle( void );
	int m_fInReload;
	float m_flNextReload;
	int m_iShell;
	float m_flPumpTime;
};

LINK_ENTITY_TO_CLASS( weapon_shotgun, CShotgun );

TYPEDESCRIPTION	CShotgun::m_SaveData[] = 
{
	DEFINE_FIELD( CShotgun, m_flNextReload, FIELD_TIME ),
	DEFINE_FIELD( CShotgun, m_fInReload, FIELD_INTEGER ),
	DEFINE_FIELD( CShotgun, m_flPumpTime, FIELD_TIME ),
};
IMPLEMENT_SAVERESTORE( CShotgun, CBasePlayerWeapon );

void CShotgun::Spawn( )
{
	pev->classname = MAKE_STRING("weapon_shotgun"); // hack to allow for old names
	Precache( );
	m_iId = WEAPON_SHOTGUN;
	SET_MODEL(ENT(pev), "models/w_shotgun.dol");

	m_iDefaultAmmo = SHOTGUN_DEFAULT_GIVE;

	FallInit();// get ready to fall
}


void CShotgun::Precache( void )
{
	PRECACHE_MODEL("models/v_shotgun.dol");
	PRECACHE_MODEL("models/w_shotgun.dol");
	PRECACHE_MODEL("models/p_shotgun.dol");

	m_iShell = PRECACHE_MODEL ("models/shotgunshell.dol");// shotgun shell

	PRECACHE_SOUND("items/9mmclip1.wav");              

	PRECACHE_SOUND ("weapons/dbarrel1.wav");//shotgun
	PRECACHE_SOUND ("weapons/sbarrel1.wav");//shotgun

	PRECACHE_SOUND ("weapons/reload1.wav");	// shotgun reload
	PRECACHE_SOUND ("weapons/reload3.wav");	// shotgun reload

	PRECACHE_SOUND ("weapons/357_cock1.wav"); // gun empty sound
	PRECACHE_SOUND ("weapons/scock1.wav");	// cock gun
}

int CShotgun::AddToPlayer( CBasePlayer *pPlayer )
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


int CShotgun::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "buckshot";
	p->iMaxAmmo1 = BUCKSHOT_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = SHOTGUN_MAX_CLIP;
	p->iSlot = 2;
	p->iPosition = 1;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_SHOTGUN;
	p->iWeight = SHOTGUN_WEIGHT;

	return 1;
}



BOOL CShotgun::Deploy( )
{
	return DefaultDeploy( "models/v_shotgun.dol", "models/p_shotgun.dol", SHOTGUN_DRAW, "shotgun" );
}


void CShotgun::PrimaryAttack()
{
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = gpGlobals->time + 0.15f;
		return;
	}

	if (m_iClip <= 0)
	{
		Reload( );
		if (m_iClip == 0)
			PlayEmptySound( );
		return;
	}

	func_002071C8( m_pPlayer, 1, 180, 95 );

	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	m_iClip--;
	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	SendWeaponAnim( SHOTGUN_FIRE );

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );

	Vector	vecShellVelocity = m_pPlayer->pev->velocity 
							 + gpGlobals->v_right * RANDOM_FLOAT(50,70) 
							 + gpGlobals->v_up * RANDOM_FLOAT(100,150) 
							 + gpGlobals->v_forward * 25;

	EjectBrass ( m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_up * -12 + gpGlobals->v_forward * 20 + gpGlobals->v_right * 4 , vecShellVelocity, pev->angles.y, m_iShell, TE_BOUNCE_SHOTSHELL); 

	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/sbarrel1.wav", RANDOM_FLOAT(0.95f, 1.0f), ATTN_NORM, 0, 93 + RANDOM_LONG(0,0x1f));

	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if ( g_pGameRules->Ps2Virt08() )	/* slot 8: o IsDeathmatch do HLSDK */
	{
		// altered deathmatch spread
		m_pPlayer->FireBullets( 4, vecSrc, vecAiming, VECTOR_CONE_DM_SHOTGUN, 2048, BULLET_PLAYER_BUCKSHOT, 0 );
	}
	else
	{
		// regular old, untouched spread. 
		m_pPlayer->FireBullets( 6, vecSrc, vecAiming, VECTOR_CONE_10DEGREES, 2048, BULLET_PLAYER_BUCKSHOT, 0 );
	}

	if (!m_iClip && !m_pPlayer->m_ps2InfiniteAmmo && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	if (m_iClip != 0)
		m_flPumpTime = gpGlobals->time + 0.5f;

	m_flNextPrimaryAttack = gpGlobals->time + 0.75f;
	m_flNextSecondaryAttack = gpGlobals->time + 0.75f;
	if (m_iClip != 0)
		m_flTimeWeaponIdle = gpGlobals->time + 5.0f;
	else
		m_flTimeWeaponIdle = 0.75f;
	m_fInReload = 0;

	m_pPlayer->pev->punchangle.x -= 5;
}


void CShotgun::SecondaryAttack( void )
{
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound( );
		m_flNextPrimaryAttack = gpGlobals->time + 0.15f;
		return;
	}

	if (m_iClip <= 1)
	{
		Reload( );
		PlayEmptySound( );
		return;
	}

	func_002071C8( m_pPlayer, 1, 255, 125 );

	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	m_iClip -= 2;
	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	SendWeaponAnim( SHOTGUN_FIRE2 );

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );

	Vector	vecShellVelocity = m_pPlayer->pev->velocity 
							 + gpGlobals->v_right * RANDOM_FLOAT(50,70) 
							 + gpGlobals->v_up * RANDOM_FLOAT(100,150) 
							 + gpGlobals->v_forward * 25;

	EjectBrass ( m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_up * -12 + gpGlobals->v_forward * 20 + gpGlobals->v_right * 4 , vecShellVelocity, pev->angles.y, m_iShell, TE_BOUNCE_SHOTSHELL); 

	vecShellVelocity = m_pPlayer->pev->velocity 
							 + gpGlobals->v_right * RANDOM_FLOAT(50,70) 
							 + gpGlobals->v_up * RANDOM_FLOAT(100,150) 
							 + gpGlobals->v_forward * 25;

	EjectBrass ( m_pPlayer->pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_up * -12 + gpGlobals->v_forward * 20 + gpGlobals->v_right * 4 , vecShellVelocity, pev->angles.y, m_iShell, TE_BOUNCE_SHOTSHELL); 

	EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/dbarrel1.wav", RANDOM_FLOAT(0.98f, 1.0f), ATTN_NORM, 0, 85 + RANDOM_LONG(0,0x1f));

	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if ( g_pGameRules->Ps2Virt08() )	/* slot 8: o IsDeathmatch do HLSDK */
	{
		// tuned for deathmatch
		m_pPlayer->FireBullets( 8, vecSrc, vecAiming, VECTOR_CONE_DM_DOUBLESHOTGUN, 2048, BULLET_PLAYER_BUCKSHOT, 0 );
	}
	else
	{
		// untouched default single player
		m_pPlayer->FireBullets( 12, vecSrc, vecAiming, VECTOR_CONE_10DEGREES, 2048, BULLET_PLAYER_BUCKSHOT, 0 );
	}

	if (!m_iClip && !m_pPlayer->m_ps2InfiniteAmmo && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	if (m_iClip != 0)
		m_flPumpTime = gpGlobals->time + 0.95f;

	m_flNextPrimaryAttack = gpGlobals->time + 1.5f;
	m_flNextSecondaryAttack = gpGlobals->time + 1.5f;
	if (m_iClip != 0)
		m_flTimeWeaponIdle = gpGlobals->time + 6.0f;
	else
		m_flTimeWeaponIdle = 1.5f;

	m_fInReload = 0;

	m_pPlayer->pev->punchangle.x -= 10;
}


void CShotgun::Reload( void )
{
	if ((!m_pPlayer->m_ps2InfiniteAmmo && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0) || m_iClip == SHOTGUN_MAX_CLIP)
		return;

	if (m_flNextReload > gpGlobals->time)
		return;

	// don't reload until recoil is done
	if (m_flNextPrimaryAttack > gpGlobals->time)
		return;

	// check to see if we're ready to reload
	if (m_fInReload == 0)
	{
		SendWeaponAnim( SHOTGUN_START_RELOAD );
		m_fInReload = 1;
		m_pPlayer->m_flNextAttack = gpGlobals->time + 0.6f;
		m_flTimeWeaponIdle = gpGlobals->time + 0.6f;
		m_flNextPrimaryAttack = gpGlobals->time + 1.0f;
		m_flNextSecondaryAttack = gpGlobals->time + 1.0f;
		return;
	}
	else if (m_fInReload == 1)
	{
		if (m_flTimeWeaponIdle > gpGlobals->time)
			return;
		// was waiting for gun to move to side
		m_fInReload = 2;

		if (RANDOM_LONG(0,1))
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/reload1.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG(0,0x1f));
		else
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/reload3.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG(0,0x1f));

		SendWeaponAnim( SHOTGUN_RELOAD );

		globalvars_t *pGlobals = gpGlobals;
		m_flNextReload = pGlobals->time + 0.5f;
		m_flTimeWeaponIdle = pGlobals->time + 0.5f;
		func_00113D08( pGlobals->time, 0.5f, ENTINDEX(ENT(m_pPlayer->pev)) - 1 );
	}
	else
	{
		// Add them to the clip
		m_iClip += 1;
		if (!m_pPlayer->m_ps2InfiniteAmmo)
			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 1;
		m_fInReload = 1;
	}
}


void CShotgun::WeaponIdle( void )
{
	ResetEmptySound( );

	m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if ( m_flPumpTime && m_flPumpTime < gpGlobals->time )
	{
		// play pumping sound
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/scock1.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,0x1f));
		m_flPumpTime = 0;
	}

	if (m_flTimeWeaponIdle < gpGlobals->time)
	{
		if (m_iClip == 0 && m_fInReload == 0 && (m_pPlayer->m_ps2InfiniteAmmo || m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]))
		{
			Reload( );
		}
		else if (m_fInReload != 0)
		{
			if (m_iClip != 8 && (m_pPlayer->m_ps2InfiniteAmmo || m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]))
			{
				Reload( );
			}
			else
			{
				// reload debounce has timed out
				SendWeaponAnim( SHOTGUN_PUMP );
				
				// play cocking sound
				EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/scock1.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,0x1f));
				m_fInReload = 0;
				m_flTimeWeaponIdle = gpGlobals->time + 1.5f;
			}
		}
		else
		{
			int iAnim;
			float flRand = RANDOM_FLOAT(0, 1);
			if (flRand <= 0.8f)
			{
				iAnim = SHOTGUN_IDLE_DEEP;
				m_flTimeWeaponIdle = gpGlobals->time + (60.0f/12.0f);// * RANDOM_LONG(2, 5);
			}
			else if (flRand <= 0.95f)
			{
				iAnim = SHOTGUN_IDLE;
				m_flTimeWeaponIdle = gpGlobals->time + (20.0f/9.0f);
			}
			else
			{
				iAnim = SHOTGUN_IDLE4;
				m_flTimeWeaponIdle = gpGlobals->time + (20.0f/9.0f);
			}
			SendWeaponAnim( iAnim );
		}
	}
}



class CShotgunAmmo : public CBasePlayerAmmo
{
	void Spawn( void )
	{ 
		Precache( );
		SET_MODEL(ENT(pev), "models/w_shotbox.dol");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_shotbox.dol");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther ) 
	{ 
		if (pOther->GiveAmmo( AMMO_BUCKSHOTBOX_GIVE, "buckshot", BUCKSHOT_MAX_CARRY ) != -1)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			return TRUE;
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS( ammo_buckshot, CShotgunAmmo );
