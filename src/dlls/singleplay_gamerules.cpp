/* dlls/singleplay_gamerules.cpp: regras do single player (CHalfLifeRules).
   O PS2 trocou o PlayerSpawn do HLSDK (traje e armas) pelos cheats
   (cheat_infammo, cheat_slavemode) e ganhou o cheat_slowmo no Think e no
   ClientCommand, com os tempos guardados no CGameRules. */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/player.h"
#include "dlls/weapons.h"
#include "dlls/gamerules.h"
#include "dlls/skill.h"

#define PLAYER_MAX_SAFE_FALL_SPEED  580
#define DAMAGE_FOR_FALL_SPEED       (float)100 / (1024 - PLAYER_MAX_SAFE_FALL_SPEED)

#define SKILL_EASY      1

typedef struct ps2cvar_s
{
	char	*name;
	char	*string;
	int		flags;
	float	value;
	struct ps2cvar_s *next;
} ps2cvar_t;

extern "C" ps2cvar_t D_003325D8;	/* cheat_slowmo */
extern "C" ps2cvar_t D_003325F0;	/* cheat_infammo */
extern "C" ps2cvar_t D_00332650;	/* cheat_slavemode */
#define cheat_slowmo D_003325D8
#define cheat_infammo D_003325F0
#define cheat_slavemode D_00332650

extern "C" int D_00312C44;
#define g_iSkillLevel D_00312C44

extern "C" int D_00317D28;			/* mensagem do InitHUD (cor do HUD?) */
extern "C" void func_001CDE70(CBasePlayer *pPlayer);

class CItem : public CBaseEntity
{
};

class CHalfLifeRules : public CGameRules
{
public:
	CHalfLifeRules ( void );

	virtual void Think( void );
	virtual BOOL IsAllowedToSpawn( CBaseEntity *pEntity );
	virtual BOOL FAllowFlashlight( void );

	virtual BOOL FShouldSwitchWeapon( CBasePlayer *pPlayer, CBasePlayerItem *pWeapon );
	virtual BOOL GetNextBestWeapon( CBasePlayer *pPlayer, CBasePlayerItem *pCurrentWeapon );

	virtual BOOL Ps2Virt07( void );
	virtual BOOL Ps2Virt08( void );
	virtual BOOL IsMultiplayer( void );

	virtual BOOL ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] );
	virtual void InitHUD( CBasePlayer *pl );
	virtual void ClientDisconnected( edict_t *pClient );

	virtual float FlPlayerFallDamage( CBasePlayer *pPlayer );

	virtual void PlayerSpawn( CBasePlayer *pPlayer );
	virtual void PlayerThink( CBasePlayer *pPlayer );
	virtual BOOL FPlayerCanRespawn( CBasePlayer *pPlayer );
	virtual float FlPlayerSpawnTime( CBasePlayer *pPlayer );

	virtual BOOL AllowAutoTargetCrosshair( void );
	virtual BOOL ClientCommand( CBasePlayer *pPlayer, const char *pcmd );

	virtual int IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled );
	virtual void PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor );
	virtual void DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor );

	virtual void PlayerGotWeapon( CBasePlayer *pPlayer, CBasePlayerItem *pWeapon );

	virtual int WeaponShouldRespawn( CBasePlayerItem *pWeapon );
	virtual float FlWeaponRespawnTime( CBasePlayerItem *pWeapon );
	virtual float FlWeaponTryRespawn( CBasePlayerItem *pWeapon );
	virtual Vector VecWeaponRespawnSpot( CBasePlayerItem *pWeapon );

	virtual BOOL CanHaveItem( CBasePlayer *pPlayer, CItem *pItem );
	virtual void PlayerGotItem( CBasePlayer *pPlayer, CItem *pItem );

	virtual int ItemShouldRespawn( CItem *pItem );
	virtual float FlItemRespawnTime( CItem *pItem );
	virtual Vector VecItemRespawnSpot( CItem *pItem );

	virtual void PlayerGotAmmo( CBasePlayer *pPlayer, char *szName, int iCount );

	virtual int AmmoShouldRespawn( CBasePlayerAmmo *pAmmo );
	virtual float FlAmmoRespawnTime( CBasePlayerAmmo *pAmmo );
	virtual Vector VecAmmoRespawnSpot( CBasePlayerAmmo *pAmmo );

	virtual float FlHealthChargerRechargeTime( void );

	virtual int DeadPlayerWeapons( CBasePlayer *pPlayer );
	virtual int DeadPlayerAmmo( CBasePlayer *pPlayer );

	virtual const char *GetTeamID( CBaseEntity *pEntity );
	virtual int PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget );

	virtual BOOL FAllowMonsters( void );
};

//=========================================================
//=========================================================
CHalfLifeRules::CHalfLifeRules( void )
{
	m_flSlowmoEnd = 0;
	m_flNextSlowmo = 0;

	RefreshSkillData();
}

//=========================================================
//=========================================================
void CHalfLifeRules::Think ( void )
{
	if ( cheat_slowmo.value > 0 && m_flSlowmoEnd <= gpGlobals->time )
		SERVER_COMMAND( "host_framerate 0\n" );
}

BOOL CHalfLifeRules::Ps2Virt07( void )
{
	return FALSE;
}

BOOL CHalfLifeRules::Ps2Virt08( void )
{
	return FALSE;
}

//=========================================================
//=========================================================
BOOL CHalfLifeRules::IsMultiplayer( void )
{
	return FALSE;
}

//=========================================================
//=========================================================
BOOL CHalfLifeRules :: FShouldSwitchWeapon ( CBasePlayer *pPlayer, CBasePlayerItem *pWeapon )
{
	if ( !pPlayer->m_pActiveItem )
	{
		// player doesn't have an active item!
		return TRUE;
	}

	if ( !pPlayer->m_pActiveItem->CanHolster() )
	{
		return FALSE;
	}

	return TRUE;
}

//=========================================================
//=========================================================
BOOL CHalfLifeRules :: GetNextBestWeapon( CBasePlayer *pPlayer, CBasePlayerItem *pCurrentWeapon )
{
	return FALSE;
}

//=========================================================
//=========================================================
BOOL CHalfLifeRules :: ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] )
{
	return TRUE;
}

void CHalfLifeRules :: InitHUD( CBasePlayer *pl )
{
	MESSAGE_BEGIN( MSG_ONE, D_00317D28, NULL, pl->edict() );
		WRITE_BYTE( 255 );
		WRITE_BYTE( 160 );
		WRITE_BYTE( 0 );
	MESSAGE_END();

	if ( m_fInfiniteAmmo )
		func_001CDE70( pl );
}

//=========================================================
//=========================================================
void CHalfLifeRules :: ClientDisconnected( edict_t *pClient )
{
}

//=========================================================
//=========================================================
float CHalfLifeRules::FlPlayerFallDamage( CBasePlayer *pPlayer )
{
	// subtract off the speed at which a player is allowed to fall without being hurt,
	// so damage will be based on speed beyond that, not the entire fall
	pPlayer->m_flFallVelocity -= PLAYER_MAX_SAFE_FALL_SPEED;
	return pPlayer->m_flFallVelocity * DAMAGE_FOR_FALL_SPEED;
}

//=========================================================
//=========================================================
BOOL CHalfLifeRules :: ClientCommand( CBasePlayer *pPlayer, const char *pcmd )
{
	if ( FStrEq( pcmd, "coopswitch" ) )
		return TRUE;

	if ( FStrEq( pcmd, "slavemode" ) )
	{
		pPlayer->GiveNamedItem( "weapon_islave" );
		return TRUE;
	}

	if ( FStrEq( pcmd, "slowmo" ) )
	{
		if ( cheat_slowmo.value > 0 && m_flNextSlowmo <= gpGlobals->time )
		{
			m_flSlowmoEnd = gpGlobals->time + 4;
			m_flNextSlowmo = gpGlobals->time + 25;
			SERVER_COMMAND( "host_framerate 0.006\n" );
		}
		return TRUE;
	}

	return FALSE;
}

#define FILL_AMMO( name, amount ) \
	{ \
		int i = CBasePlayer::GetAmmoIndex( name ); \
		if ( i >= 0 && i < MAX_AMMO_SLOTS && pPlayer->m_rgAmmo[i] < amount ) \
			pPlayer->m_rgAmmo[i] = amount; \
	}

//=========================================================
//=========================================================
void CHalfLifeRules :: PlayerSpawn( CBasePlayer *pPlayer )
{
	m_fInfiniteAmmo = cheat_infammo.value > 0;
	pPlayer->m_ps2InfiniteAmmo = m_fInfiniteAmmo;

	if ( m_fInfiniteAmmo )
	{
		ALERT( at_console, "Infinite Ammo.\n" );

		FILL_AMMO( "9mm", 250 );
		FILL_AMMO( "ARgrenades", 10 );
		FILL_AMMO( "buckshot", 125 );
		FILL_AMMO( "357", 36 );
		FILL_AMMO( "rockets", 5 );
		FILL_AMMO( "bolts", 50 );
		FILL_AMMO( "uranium", 100 );
	}

	if ( cheat_slavemode.value > 0 )
		pPlayer->GiveNamedItem( "weapon_islave" );
}

//=========================================================
//=========================================================
BOOL CHalfLifeRules :: AllowAutoTargetCrosshair( void )
{
	return ( g_iSkillLevel == SKILL_EASY );
}

//=========================================================
//=========================================================
void CHalfLifeRules :: PlayerThink( CBasePlayer *pPlayer )
{
}


//=========================================================
//=========================================================
BOOL CHalfLifeRules :: FPlayerCanRespawn( CBasePlayer *pPlayer )
{
	return TRUE;
}

//=========================================================
//=========================================================
float CHalfLifeRules :: FlPlayerSpawnTime( CBasePlayer *pPlayer )
{
	return gpGlobals->time;//now!
}

//=========================================================
// IPointsForKill - how many points awarded to anyone
// that kills this player?
//=========================================================
int CHalfLifeRules :: IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled )
{
	return 1;
}

//=========================================================
// PlayerKilled - someone/something killed this player
//=========================================================
void CHalfLifeRules :: PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
{
}

//=========================================================
// Deathnotice
//=========================================================
void CHalfLifeRules::DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
{
}

//=========================================================
// PlayerGotWeapon - player has grabbed a weapon that was
// sitting in the world
//=========================================================
void CHalfLifeRules :: PlayerGotWeapon( CBasePlayer *pPlayer, CBasePlayerItem *pWeapon )
{
}

//=========================================================
// FlWeaponRespawnTime - what is the time in the future
// at which this weapon may spawn?
//=========================================================
float CHalfLifeRules :: FlWeaponRespawnTime( CBasePlayerItem *pWeapon )
{
	return -1;
}

//=========================================================
// FlWeaponRespawnTime - Returns 0 if the weapon can respawn 
// now,  otherwise it returns the time at which it can try
// to spawn again.
//=========================================================
float CHalfLifeRules :: FlWeaponTryRespawn( CBasePlayerItem *pWeapon )
{
	return 0;
}

//=========================================================
// VecWeaponRespawnSpot - where should this weapon spawn?
// Some game variations may choose to randomize spawn locations
//=========================================================
Vector CHalfLifeRules :: VecWeaponRespawnSpot( CBasePlayerItem *pWeapon )
{
	return pWeapon->pev->origin;
}

//=========================================================
// WeaponShouldRespawn - any conditions inhibiting the
// respawning of this weapon?
//=========================================================
int CHalfLifeRules :: WeaponShouldRespawn( CBasePlayerItem *pWeapon )
{
	return GR_WEAPON_RESPAWN_NO;
}

//=========================================================
//=========================================================
BOOL CHalfLifeRules::CanHaveItem( CBasePlayer *pPlayer, CItem *pItem )
{
	return TRUE;
}

//=========================================================
//=========================================================
void CHalfLifeRules::PlayerGotItem( CBasePlayer *pPlayer, CItem *pItem )
{
}

//=========================================================
//=========================================================
int CHalfLifeRules::ItemShouldRespawn( CItem *pItem )
{
	return GR_ITEM_RESPAWN_NO;
}


//=========================================================
// At what time in the future may this Item respawn?
//=========================================================
float CHalfLifeRules::FlItemRespawnTime( CItem *pItem )
{
	return -1;
}

//=========================================================
// Where should this item respawn?
// Some game variations may choose to randomize spawn locations
//=========================================================
Vector CHalfLifeRules::VecItemRespawnSpot( CItem *pItem )
{
	return pItem->pev->origin;
}

//=========================================================
//=========================================================
BOOL CHalfLifeRules::IsAllowedToSpawn( CBaseEntity *pEntity )
{
	return TRUE;
}

//=========================================================
//=========================================================
void CHalfLifeRules::PlayerGotAmmo( CBasePlayer *pPlayer, char *szName, int iCount )
{
}

//=========================================================
//=========================================================
int CHalfLifeRules::AmmoShouldRespawn( CBasePlayerAmmo *pAmmo )
{
	return GR_AMMO_RESPAWN_NO;
}

//=========================================================
//=========================================================
float CHalfLifeRules::FlAmmoRespawnTime( CBasePlayerAmmo *pAmmo )
{
	return -1;
}

//=========================================================
//=========================================================
Vector CHalfLifeRules::VecAmmoRespawnSpot( CBasePlayerAmmo *pAmmo )
{
	return pAmmo->pev->origin;
}

//=========================================================
//=========================================================
float CHalfLifeRules::FlHealthChargerRechargeTime( void )
{
	return 0;// don't recharge
}

//=========================================================
//=========================================================
int CHalfLifeRules::DeadPlayerWeapons( CBasePlayer *pPlayer )
{
	return 9;
}

//=========================================================
//=========================================================
int CHalfLifeRules::DeadPlayerAmmo( CBasePlayer *pPlayer )
{
	return 12;
}

//=========================================================
//=========================================================
int CHalfLifeRules::PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget )
{
	// why would a single player in half life need this? 
	return GR_NOTTEAMMATE;
}

//=========================================================
//=========================================================
BOOL CHalfLifeRules :: FAllowMonsters( void )
{
	return TRUE;
}

//=========================================================
//=========================================================
BOOL CHalfLifeRules :: FAllowFlashlight( void )
{
	return TRUE;
}

//=========================================================
//=========================================================
const char *CHalfLifeRules :: GetTeamID( CBaseEntity *pEntity )
{
	return "";
}
