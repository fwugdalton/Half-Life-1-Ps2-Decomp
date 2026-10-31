/* dlls/gamerules.cpp: CGameRules (a base das regras) e o InstallGameRules.
   Diferenças do PS2: o RefreshSkillData usa skill 2 no multiplayer e lê os cvars do
   Decay (sk_plr_islave, sk_flyer_*, sk_focus_health); o CanHavePlayerItem não olha o
   deadflag; o InstallGameRules escolhe as regras do coop (mp_coop > 0) e não mexe no
   g_teamplay; as inline do gamerules.h saem aqui fora de linha, na ordem da vtable. */

#define DLLS_GAMERULES_CPP
#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/player.h"
#include "dlls/weapons.h"
#include "dlls/gamerules.h"
#include "dlls/teamplay_gamerules.h"

/* coopplay_gamerules.h do PS2: só a string da GetGameDescription inline sai aqui */
class CHalfLifeCoopplay : public CHalfLifeMultiplay
{
public:
	CHalfLifeCoopplay();
	virtual const char *GetGameDescription( void ) { return "HL Coopplay"; }
};
#include "dlls/skill.h"

typedef struct ps2cvar_s
{
	char	*name;
	char	*string;
	int		flags;
	float	value;
	struct ps2cvar_s *next;
} ps2cvar_t;

extern ps2cvar_t teamplay;
extern int g_iSkillLevel;

float GetSkillCvar( char *pName );
extern "C" edict_t *func_001CA720( CBaseEntity *pPlayer );			/* EntSelectSpawnPoint */
extern "C" int func_001CDDD0( CBasePlayer *pPlayer, int iAmmoIndex );	/* CBasePlayer::AmmoInventory */
extern "C" void func_001D0E60( CBaseEntity *pPlayer, int iPlayerIndex, int a2, int a3 );
extern "C" CGameRules *func_00152E20( void *pMem );					/* construtor das regras do coop (0x5C bytes) */

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

DLL_GLOBAL CGameRules *g_pGameRules = NULL;

//=========================================================
//=========================================================
BOOL CGameRules::CanHaveAmmo( CBasePlayer *pPlayer, const char *pszAmmoName, int iMaxCarry )
{
	int iAmmoIndex;

	if ( pszAmmoName )
	{
		iAmmoIndex = pPlayer->GetAmmoIndex( pszAmmoName );

		if ( iAmmoIndex > -1 )
		{
			if ( func_001CDDD0( pPlayer, iAmmoIndex ) < iMaxCarry )
			{
				// player has room for more of this type of ammo
				return TRUE;
			}
		}
	}

	return FALSE;
}

//=========================================================
//=========================================================
edict_t *CGameRules :: GetPlayerSpawnSpot( CBasePlayer *pPlayer )
{
	edict_t *pentSpawnSpot = func_001CA720( pPlayer );

	pPlayer->pev->origin = VARS(pentSpawnSpot)->origin + Vector(0,0,1);
	pPlayer->pev->v_angle  = g_vecZero;
	pPlayer->pev->velocity = g_vecZero;
	pPlayer->pev->angles = VARS(pentSpawnSpot)->angles;
	pPlayer->pev->punchangle = g_vecZero;
	pPlayer->pev->fixangle = TRUE;

	return pentSpawnSpot;
}

//=========================================================
//=========================================================
BOOL CGameRules::CanHavePlayerItem( CBasePlayer *pPlayer, CBasePlayerItem *pWeapon )
{
	if ( pWeapon->pszAmmo1() )
	{
		if ( !CanHaveAmmo( pPlayer, pWeapon->pszAmmo1(), pWeapon->iMaxAmmo1() ) )
		{
			// we can't carry anymore ammo for this gun. We can only
			// have the gun if we aren't already carrying one of this type
			if ( pPlayer->HasPlayerItem( pWeapon ) )
			{
				return FALSE;
			}
		}
	}
	else
	{
		// weapon doesn't use ammo, don't take another if you already have it.
		if ( pPlayer->HasPlayerItem( pWeapon ) )
		{
			return FALSE;
		}
	}

	// note: will fall through to here if GetItemInfo doesn't fill the struct!
	return TRUE;
}

//=========================================================
// load the SkillData struct with the proper values based on the skill level.
//=========================================================
void CGameRules::RefreshSkillData ( void )
{
	int	iSkill;

	if ( IsMultiplayer() )
		iSkill = 2;
	else
		iSkill = (int)CVAR_GET_FLOAT("skill");
	g_iSkillLevel = iSkill;

	if ( iSkill < 1 )
	{
		iSkill = 1;
	}
	else if ( iSkill > 3 )
	{
		iSkill = 3;
	}

	gSkillData.iSkillLevel = iSkill;

	ALERT ( at_console, "\nGAME SKILL LEVEL:%d\n",iSkill );

	gSkillData.agruntHealth = GetSkillCvar( "sk_agrunt_health" );
	gSkillData.agruntDmgPunch = GetSkillCvar( "sk_agrunt_dmg_punch" );
	gSkillData.apacheHealth = GetSkillCvar( "sk_apache_health" );
	gSkillData.barneyHealth = GetSkillCvar( "sk_barney_health" );
	gSkillData.bigmommaHealthFactor = GetSkillCvar( "sk_bigmomma_health_factor" );
	gSkillData.bigmommaDmgSlash = GetSkillCvar( "sk_bigmomma_dmg_slash" );
	gSkillData.bigmommaDmgBlast = GetSkillCvar( "sk_bigmomma_dmg_blast" );
	gSkillData.bigmommaRadiusBlast = GetSkillCvar( "sk_bigmomma_radius_blast" );
	gSkillData.bullsquidHealth = GetSkillCvar( "sk_bullsquid_health" );
	gSkillData.bullsquidDmgBite = GetSkillCvar( "sk_bullsquid_dmg_bite" );
	gSkillData.bullsquidDmgWhip = GetSkillCvar( "sk_bullsquid_dmg_whip" );
	gSkillData.bullsquidDmgSpit = GetSkillCvar( "sk_bullsquid_dmg_spit" );
	gSkillData.gargantuaHealth = GetSkillCvar( "sk_gargantua_health" );
	gSkillData.gargantuaDmgSlash = GetSkillCvar( "sk_gargantua_dmg_slash" );
	gSkillData.gargantuaDmgFire = GetSkillCvar( "sk_gargantua_dmg_fire" );
	gSkillData.gargantuaDmgStomp = GetSkillCvar( "sk_gargantua_dmg_stomp" );
	gSkillData.hassassinHealth = GetSkillCvar( "sk_hassassin_health" );
	gSkillData.headcrabHealth = GetSkillCvar( "sk_headcrab_health" );
	gSkillData.headcrabDmgBite = GetSkillCvar( "sk_headcrab_dmg_bite" );
	gSkillData.hgruntHealth = GetSkillCvar( "sk_hgrunt_health" );
	gSkillData.hgruntDmgKick = GetSkillCvar( "sk_hgrunt_kick" );
	gSkillData.hgruntShotgunPellets = GetSkillCvar( "sk_hgrunt_pellets" );
	gSkillData.hgruntGrenadeSpeed = GetSkillCvar( "sk_hgrunt_gspeed" );
	gSkillData.houndeyeHealth = GetSkillCvar( "sk_houndeye_health" );
	gSkillData.houndeyeDmgBlast = GetSkillCvar( "sk_houndeye_dmg_blast" );
	gSkillData.slaveHealth = GetSkillCvar( "sk_islave_health" );
	gSkillData.slaveDmgClaw = GetSkillCvar( "sk_islave_dmg_claw" );
	gSkillData.slaveDmgClawrake = GetSkillCvar( "sk_islave_dmg_clawrake" );
	gSkillData.slaveDmgZap = GetSkillCvar( "sk_islave_dmg_zap" );
	gSkillData.ichthyosaurHealth = GetSkillCvar( "sk_ichthyosaur_health" );
	gSkillData.ichthyosaurDmgShake = GetSkillCvar( "sk_ichthyosaur_shake" );
	gSkillData.leechHealth = GetSkillCvar( "sk_leech_health" );
	gSkillData.leechDmgBite = GetSkillCvar( "sk_leech_dmg_bite" );
	gSkillData.controllerHealth = GetSkillCvar( "sk_controller_health" );
	gSkillData.controllerDmgZap = GetSkillCvar( "sk_controller_dmgzap" );
	gSkillData.controllerSpeedBall = GetSkillCvar( "sk_controller_speedball" );
	gSkillData.controllerDmgBall = GetSkillCvar( "sk_controller_dmgball" );
	gSkillData.nihilanthHealth = GetSkillCvar( "sk_nihilanth_health" );
	gSkillData.nihilanthZap = GetSkillCvar( "sk_nihilanth_zap" );
	gSkillData.scientistHealth = GetSkillCvar( "sk_scientist_health" );
	gSkillData.snarkHealth = GetSkillCvar( "sk_snark_health" );
	gSkillData.snarkDmgBite = GetSkillCvar( "sk_snark_dmg_bite" );
	gSkillData.snarkDmgPop = GetSkillCvar( "sk_snark_dmg_pop" );
	gSkillData.zombieHealth = GetSkillCvar( "sk_zombie_health" );
	gSkillData.zombieDmgOneSlash = GetSkillCvar( "sk_zombie_dmg_one_slash" );
	gSkillData.zombieDmgBothSlash = GetSkillCvar( "sk_zombie_dmg_both_slash" );
	gSkillData.turretHealth = GetSkillCvar( "sk_turret_health" );
	gSkillData.miniturretHealth = GetSkillCvar( "sk_miniturret_health" );
	gSkillData.sentryHealth = GetSkillCvar( "sk_sentry_health" );
	gSkillData.plrDmgCrowbar = GetSkillCvar( "sk_plr_crowbar" );
	gSkillData.plrDmgISlave = GetSkillCvar( "sk_plr_islave" );
	gSkillData.plrDmgISlaveZap = GetSkillCvar( "sk_plr_islavezap" );
	gSkillData.plrDmg9MM = GetSkillCvar( "sk_plr_9mm_bullet" );
	gSkillData.plrDmg357 = GetSkillCvar( "sk_plr_357_bullet" );
	gSkillData.plrDmgMP5 = GetSkillCvar( "sk_plr_9mmAR_bullet" );
	gSkillData.plrDmgM203Grenade = GetSkillCvar( "sk_plr_9mmAR_grenade" );
	gSkillData.plrDmgBuckshot = GetSkillCvar( "sk_plr_buckshot" );
	gSkillData.plrDmgCrossbowClient = GetSkillCvar( "sk_plr_xbow_bolt_client" );
	gSkillData.plrDmgCrossbowMonster = GetSkillCvar( "sk_plr_xbow_bolt_monster" );
	gSkillData.plrDmgRPG = GetSkillCvar( "sk_plr_rpg" );
	gSkillData.plrDmgGauss = GetSkillCvar( "sk_plr_gauss" );
	gSkillData.plrDmgEgonNarrow = GetSkillCvar( "sk_plr_egon_narrow" );
	gSkillData.plrDmgEgonWide = GetSkillCvar( "sk_plr_egon_wide" );
	gSkillData.plrDmgHandGrenade = GetSkillCvar( "sk_plr_hand_grenade" );
	gSkillData.plrDmgSatchel = GetSkillCvar( "sk_plr_satchel" );
	gSkillData.plrDmgTripmine = GetSkillCvar( "sk_plr_tripmine" );
	gSkillData.monDmg12MM = GetSkillCvar( "sk_12mm_bullet" );
	gSkillData.monDmgMP5 = GetSkillCvar( "sk_9mmAR_bullet" );
	gSkillData.monDmg9MM = GetSkillCvar( "sk_9mm_bullet" );
	gSkillData.monDmgHornet = GetSkillCvar( "sk_hornet_dmg" );
	gSkillData.plrDmgHornet = 7;
	gSkillData.suitchargerCapacity = GetSkillCvar( "sk_suitcharger" );
	gSkillData.batteryCapacity = GetSkillCvar( "sk_battery" );
	gSkillData.healthchargerCapacity = GetSkillCvar( "sk_healthcharger" );
	gSkillData.healthkitCapacity = GetSkillCvar( "sk_healthkit" );
	gSkillData.scientistHeal = GetSkillCvar( "sk_scientist_heal" );
	gSkillData.monHead = GetSkillCvar( "sk_monster_head" );
	gSkillData.monChest = GetSkillCvar( "sk_monster_chest" );
	gSkillData.monStomach = GetSkillCvar( "sk_monster_stomach" );
	gSkillData.monLeg = GetSkillCvar( "sk_monster_leg" );
	gSkillData.monArm = GetSkillCvar( "sk_monster_arm" );
	gSkillData.plrHead = GetSkillCvar( "sk_player_head" );
	gSkillData.plrChest = GetSkillCvar( "sk_player_chest" );
	gSkillData.plrStomach = GetSkillCvar( "sk_player_stomach" );
	gSkillData.plrLeg = GetSkillCvar( "sk_player_leg" );
	gSkillData.plrArm = GetSkillCvar( "sk_player_arm" );
	gSkillData.flyerHealth = GetSkillCvar( "sk_flyer_health" );
	gSkillData.flyerDamage = GetSkillCvar( "sk_flyer_damage" );
	gSkillData.focusHealth = GetSkillCvar( "sk_focus_health" );
}

//=========================================================
// instantiate the proper game rules object
//=========================================================

CGameRules *InstallGameRules( void )
{
	SERVER_COMMAND( "exec game.cfg\n" );
	SERVER_EXECUTE( );

	if ( !gpGlobals->deathmatch )
	{
		if ( gpGlobals->coop > 0 )
			return func_00152E20( operator new( 0x5C ) );

		// generic half-life
		return new CHalfLifeRules;
	}
	else
	{
		if ( gpGlobals->coop > 0 )
			return func_00152E20( operator new( 0x5C ) );

		if ( teamplay.value > 0 )
		{
			// teamplay
			return new CHalfLifeTeamplay;
		}
		if ((int)gpGlobals->deathmatch == 1)
		{
			// vanilla deathmatch
			return new CHalfLifeMultiplay;
		}
		else
		{
			// vanilla deathmatch??
			return new CHalfLifeMultiplay;
		}
	}
}

void CGameRules::Ps2Virt63( int iPlayerIndex, int a2, int a3 )
{
	CBaseEntity *pPlayer = UTIL_PlayerByIndex( iPlayerIndex );

	if ( pPlayer )
		func_001D0E60( pPlayer, iPlayerIndex, a2, a3 );
}
