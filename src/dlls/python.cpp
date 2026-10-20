/* dlls/python.cpp: revólver .357 (weapon_python / weapon_357) e a caixa de
   munição (ammo_357) do HLSDK, na versão antiga (sem eventos, gpGlobals->time).
   O PS2 acrescenta o rumble do controle e a munição infinita do cheat. */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/weapons.h"
#include "dlls/player.h"
#include "dlls/gamerules.h"
#include "include_asm.h"

#define WEAPON_PYTHON           3
#define PYTHON_MAX_CLIP         6
#define PYTHON_DEFAULT_GIVE     6
#define _357_MAX_CARRY          36
#define PYTHON_WEIGHT           15
#define AMMO_357BOX_GIVE        PYTHON_MAX_CLIP

extern "C" void func_002071C8(CBasePlayer *pPlayer, int iUnk, unsigned char intensity, unsigned short duration);

enum python_e {
	PYTHON_IDLE1 = 0,
	PYTHON_FIDGET,
	PYTHON_FIRE1,
	PYTHON_RELOAD,
	PYTHON_HOLSTER,
	PYTHON_DRAW,
	PYTHON_IDLE2,
	PYTHON_IDLE3
};

class CPython : public CBasePlayerWeapon
{
public:
	void Spawn( void );
	void Precache( void );
	int iItemSlot( void ) { return 2; }
	int GetItemInfo(ItemInfo *p);
	int AddToPlayer( CBasePlayer *pPlayer );
	void PrimaryAttack( void );
	void SecondaryAttack( void );
	BOOL Deploy( void );
	void Holster( void );
	void Reload( void );
	void WeaponIdle( void );
	float m_flSoundDelay;

	BOOL m_fInZoom;// don't save this.
};

LINK_ENTITY_TO_CLASS( weapon_python, CPython );
LINK_ENTITY_TO_CLASS( weapon_357, CPython );

int CPython::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "357";
	p->iMaxAmmo1 = _357_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = PYTHON_MAX_CLIP;
	p->iFlags = 0;
	p->iSlot = 1;
	p->iPosition = 1;
	p->iId = m_iId = WEAPON_PYTHON;
	p->iWeight = PYTHON_WEIGHT;

	return 1;
}

int CPython::AddToPlayer( CBasePlayer *pPlayer )
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

void CPython::Spawn( )
{
	pev->classname = MAKE_STRING("weapon_357"); // hack to allow for old names
	Precache( );
	m_iId = WEAPON_PYTHON;
	SET_MODEL(ENT(pev), "models/w_357.dol");

	m_iDefaultAmmo = PYTHON_DEFAULT_GIVE;

	FallInit();// get ready to fall down.
}


void CPython::Precache( void )
{
	PRECACHE_MODEL("models/v_357.dol");
	PRECACHE_MODEL("models/w_357.dol");
	PRECACHE_MODEL("models/p_357.dol");

	PRECACHE_MODEL("models/w_357ammobox.dol");
	PRECACHE_SOUND("items/9mmclip1.wav");

	PRECACHE_SOUND ("weapons/357_reload1.wav");
	PRECACHE_SOUND ("weapons/357_cock1.wav");
	PRECACHE_SOUND ("weapons/357_shot1.wav");
	PRECACHE_SOUND ("weapons/357_shot2.wav");
}

BOOL CPython::Deploy( )
{
	if ( g_pGameRules->Ps2Virt07() )
	{
		// enable laser sight geometry.
		pev->body = 1;
	}
	else
	{
		pev->body = 0;
	}

	return DefaultDeploy( "models/v_357.dol", "models/p_357.dol", PYTHON_DRAW, "python" );
}


void CPython::Holster( )
{
	m_fInReload = FALSE;// cancel any reload in progress.

	if ( m_fInZoom )
	{
		SecondaryAttack();
	}

	m_pPlayer->m_flNextAttack = gpGlobals->time + 1.0f;
	m_flTimeWeaponIdle = gpGlobals->time + 10 + RANDOM_FLOAT ( 0, 5 );
	SendWeaponAnim( PYTHON_HOLSTER );
}

void CPython::SecondaryAttack( void )
{
	if ( !g_pGameRules->Ps2Virt07() )
	{
		return;
	}

	if ( m_fInZoom )
	{
		m_fInZoom = FALSE;
		m_pPlayer->m_iFOV = 0;  // 0 means reset to default fov
	}
	else
	{
		m_fInZoom = TRUE;
		m_pPlayer->m_iFOV = 40;
	}

	m_flNextSecondaryAttack = gpGlobals->time + 0.5f;
}

#ifdef NON_MATCHING
/* só difere no escalonamento: o gcc daqui sobe o lui de g_engfuncs para logo
   depois do rumble em vez de deixá-lo junto do efeito do clarão */
void CPython::PrimaryAttack()
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
		if (!m_fFireOnEmpty)
			Reload( );
		else
		{
			EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/357_cock1.wav", 0.8f, ATTN_NORM);
			m_flNextPrimaryAttack = gpGlobals->time + 0.15f;
		}

		return;
	}

	func_002071C8( m_pPlayer, 1, 230, 95 );

	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	m_iClip--;

	SendWeaponAnim( PYTHON_FIRE1 );

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	m_pPlayer->pev->effects = (int)(float)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	switch(RANDOM_LONG(0,1))
	{
	case 0:
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/357_shot1.wav", RANDOM_FLOAT(0.8f, 0.9f), ATTN_NORM);
		break;
	case 1:
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/357_shot2.wav", RANDOM_FLOAT(0.8f, 0.9f), ATTN_NORM);
		break;
	}

	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );

	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );

	m_pPlayer->FireBullets( 1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 8192, BULLET_PLAYER_357, 0 );

	if (!m_iClip && !m_pPlayer->m_ps2InfiniteAmmo && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", FALSE, 0);

	m_flNextPrimaryAttack = gpGlobals->time + 0.75f;
	m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT( 10, 15 );

	m_pPlayer->pev->punchangle.x -= 10;
}
#else
INCLUDE_ASM("asm/nonmatchings/dlls/python", PrimaryAttack__7CPython);

#endif


void CPython::Reload( void )
{
	if ( m_fInZoom )
	{
		m_fInZoom = FALSE;
		m_pPlayer->m_iFOV = 0;  // 0 means reset to default fov
	}

	if (DefaultReload( 6, PYTHON_RELOAD, 2.0f ))
	{
		m_flSoundDelay = gpGlobals->time + 1.5f;
	}
}


void CPython::WeaponIdle( void )
{
	ResetEmptySound( );

	m_pPlayer->GetAutoaimVector( AUTOAIM_10DEGREES );

	// ALERT( at_console, "%.2f\n", gpGlobals->time - m_flSoundDelay );
	if (m_flSoundDelay != 0 && m_flSoundDelay <= gpGlobals->time)
	{
		EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "weapons/357_reload1.wav", RANDOM_FLOAT(0.8f, 0.9f), ATTN_NORM);
		m_flSoundDelay = 0;
	}

	if (m_flTimeWeaponIdle > gpGlobals->time)
		return;

	int iAnim;
	float flRand = RANDOM_FLOAT(0, 1);
	if (flRand <= 0.5f)
	{
		iAnim = PYTHON_IDLE1;
		m_flTimeWeaponIdle = gpGlobals->time + (70.0f/30.0f);
	}
	else if (flRand <= 0.7f)
	{
		iAnim = PYTHON_IDLE2;
		m_flTimeWeaponIdle = gpGlobals->time + (60.0f/30.0f);
	}
	else if (flRand <= 0.9f)
	{
		iAnim = PYTHON_IDLE3;
		m_flTimeWeaponIdle = gpGlobals->time + (88.0f/30.0f);
	}
	else
	{
		iAnim = PYTHON_FIDGET;
		m_flTimeWeaponIdle = gpGlobals->time + (170.0f/30.0f);
	}
	SendWeaponAnim( iAnim );
}



class CPythonAmmo : public CBasePlayerAmmo
{
	void Spawn( void )
	{
		Precache( );
		SET_MODEL(ENT(pev), "models/w_357ammobox.dol");
		CBasePlayerAmmo::Spawn( );
	}
	void Precache( void )
	{
		PRECACHE_MODEL ("models/w_357ammobox.dol");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	BOOL AddAmmo( CBaseEntity *pOther )
	{
		if (pOther->GiveAmmo( AMMO_357BOX_GIVE, "357", _357_MAX_CARRY ) != -1)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			return TRUE;
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS( ammo_357, CPythonAmmo );
