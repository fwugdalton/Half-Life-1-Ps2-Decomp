/* dlls/hornetgun.cpp: o hivehand (weapon_hornetgun) do HLSDK, na versão antiga
   (sem eventos, gpGlobals->time). O PS2 acrescenta o rumble do controle e dá a
   arma cheia quando o Ps2Virt07 das regras está ligado. */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/weapons.h"
#include "dlls/player.h"
#include "dlls/hornet.h"
#include "dlls/gamerules.h"
#include "include_asm.h"

#define WEAPON_HORNETGUN        11
#define HORNETGUN_WEIGHT        10
#define HORNET_MAX_CARRY        8
#define HIVEHAND_DEFAULT_GIVE   8

extern "C" void func_002071C8(CBasePlayer *pPlayer, int iUnk, unsigned char intensity, unsigned short duration);

enum hgun_e {
	HGUN_IDLE1 = 0,
	HGUN_FIDGETSWAY,
	HGUN_FIDGETSHAKE,
	HGUN_DOWN,
	HGUN_UP,
	HGUN_SHOOT
};

class CHgun : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int iItemSlot( void ) { return 4; }
	int GetItemInfo(ItemInfo *p);
	int AddToPlayer( CBasePlayer *pPlayer );

	void PrimaryAttack( void );
	void SecondaryAttack( void );
	BOOL Deploy( void );
	BOOL IsUseable( void );
	void Holster( void );
	void Reload( void );
	void WeaponIdle( void );
	float m_flNextAnimTime;

	float m_flRechargeTime;

	int m_iFirePhase;// don't save me.
};

LINK_ENTITY_TO_CLASS( weapon_hornetgun, CHgun );

BOOL CHgun::IsUseable( void )
{
	return TRUE;
}

void CHgun::Spawn( )
{
	Precache( );
	m_iId = WEAPON_HORNETGUN;
	SET_MODEL(ENT(pev), "models/w_hgun.dol");

	m_iDefaultAmmo = HIVEHAND_DEFAULT_GIVE;
	m_iFirePhase = 0;

	FallInit();// get ready to fall down.
}


void CHgun::Precache( void )
{
	PRECACHE_MODEL("models/v_hgun.dol");
	PRECACHE_MODEL("models/w_hgun.dol");
	PRECACHE_MODEL("models/p_hgun.dol");

	UTIL_PrecacheOther("hornet");
}

int CHgun::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		if ( g_pGameRules->Ps2Virt07() )
		{
			// in multiplayer, all hivehands come full. 
			pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] = HORNET_MAX_CARRY;
		}

		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
			WRITE_BYTE( m_iId );
		MESSAGE_END();
		return TRUE;
	}
	return FALSE;
}

int CHgun::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "Hornets";
	p->iMaxAmmo1 = HORNET_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 3;
	p->iPosition = 3;
	p->iId = m_iId = WEAPON_HORNETGUN;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY | ITEM_FLAG_NOAUTORELOAD;
	p->iWeight = HORNETGUN_WEIGHT;

	return 1;
}


BOOL CHgun::Deploy( )
{
	return DefaultDeploy( "models/v_hgun.dol", "models/p_hgun.dol", HGUN_UP, "hive" );
}

void CHgun::Holster( )
{
	m_pPlayer->m_flNextAttack = gpGlobals->time + 0.5f;
	SendWeaponAnim( HGUN_DOWN );

	//!!!HACKHACK - can't select hornetgun if it's empty! no way to get ammo for it, either.
	if ( !m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] )
	{
		m_pPlayer->m_rgAmmo[ PrimaryAmmoIndex() ] = 1;
	}
}


void CHgun::PrimaryAttack()
{
	Reload( );

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		return;
	}

	func_002071C8( m_pPlayer, 1, RANDOM_LONG( 50, 200 ), 50 );

	UTIL_MakeVectors( m_pPlayer->pev->v_angle );

	CBaseEntity *pHornet = CBaseEntity::Create( "hornet", m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12, m_pPlayer->pev->v_angle, m_pPlayer->edict() );
	pHornet->pev->velocity = gpGlobals->v_forward * 300;

	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
	m_flRechargeTime = gpGlobals->time + 0.5f;

	m_pPlayer->m_iWeaponVolume = QUIET_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;

	SendWeaponAnim( HGUN_SHOOT );

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	m_flNextPrimaryAttack = m_flNextPrimaryAttack + 0.25f;
	if (m_flNextPrimaryAttack < gpGlobals->time)
	{
		m_flNextPrimaryAttack = gpGlobals->time + 0.25f;
	}

	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );
}



void CHgun::SecondaryAttack( void )
{
	Reload();

	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		return;
	}

	func_002071C8( m_pPlayer, 1, RANDOM_LONG( 50, 180 ), 25 );

	//Wouldn't be a bad idea to completely predict these, since they fly so fast...
	CBaseEntity *pHornet;
	Vector vecSrc;

	UTIL_MakeVectors( m_pPlayer->pev->v_angle );

	vecSrc = m_pPlayer->GetGunPosition( ) + gpGlobals->v_forward * 16 + gpGlobals->v_right * 8 + gpGlobals->v_up * -12;

	m_iFirePhase++;
	switch ( m_iFirePhase )
	{
	case 1:
		vecSrc = vecSrc + gpGlobals->v_up * 8;
		break;
	case 2:
		vecSrc = vecSrc + gpGlobals->v_up * 8;
		vecSrc = vecSrc + gpGlobals->v_right * 8;
		break;
	case 3:
		vecSrc = vecSrc + gpGlobals->v_right * 8;
		break;
	case 4:
		vecSrc = vecSrc + gpGlobals->v_up * -8;
		vecSrc = vecSrc + gpGlobals->v_right * 8;
		break;
	case 5:
		vecSrc = vecSrc + gpGlobals->v_up * -8;
		break;
	case 6:
		vecSrc = vecSrc + gpGlobals->v_up * -8;
		vecSrc = vecSrc + gpGlobals->v_right * -8;
		break;
	case 7:
		vecSrc = vecSrc + gpGlobals->v_right * -8;
		break;
	case 8:
		vecSrc = vecSrc + gpGlobals->v_up * 8;
		vecSrc = vecSrc + gpGlobals->v_right * -8;
		m_iFirePhase = 0;
		break;
	}

	pHornet = CBaseEntity::Create( "hornet", vecSrc, m_pPlayer->pev->v_angle, m_pPlayer->edict() );
	pHornet->pev->velocity = gpGlobals->v_forward * 1200;
	pHornet->pev->angles = UTIL_VecToAngles( pHornet->pev->velocity );

	pHornet->SetThink( &CHornet::StartDart );

	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = DIM_GUN_FLASH;

	m_flRechargeTime = gpGlobals->time + 0.5f;

	SendWeaponAnim( HGUN_SHOOT );

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = gpGlobals->time + 0.1f;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT ( 10, 15 );

	m_pPlayer->pev->punchangle.x = RANDOM_FLOAT( 0, 2 );
}


void CHgun::Reload( void )
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] >= HORNET_MAX_CARRY)
		return;

	while (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] < HORNET_MAX_CARRY && m_flRechargeTime < gpGlobals->time)
	{
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]++;
		m_flRechargeTime += 0.5f;
	}
}


void CHgun::WeaponIdle( void )
{
	m_pPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );

	Reload( );

	if (m_flTimeWeaponIdle > gpGlobals->time)
		return;

	int iAnim;
	float flRand = RANDOM_FLOAT(0, 1);
	if (flRand <= 0.75f)
	{
		iAnim = HGUN_IDLE1;
		m_flTimeWeaponIdle = gpGlobals->time + 30.0f / 16 * (2);
	}
	else if (flRand <= 0.875f)
	{
		iAnim = HGUN_FIDGETSWAY;
		m_flTimeWeaponIdle = gpGlobals->time + 40.0f / 16.0f;
	}
	else
	{
		iAnim = HGUN_FIDGETSHAKE;
		m_flTimeWeaponIdle = gpGlobals->time + 35.0f / 16.0f;
	}
	SendWeaponAnim( iAnim );
}

/* registro do ponteiro de Think para o save do PS2 */
extern "C" void func_002397D0(const char *name, BASEPTR pfn);

extern "C" void func_0018FDD8(void)
{
	func_002397D0("350", static_cast<BASEPTR>(&CHornet::StartDart));
}
