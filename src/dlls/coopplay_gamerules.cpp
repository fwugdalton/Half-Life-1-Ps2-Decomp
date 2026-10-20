/* dlls/coopplay_gamerules.cpp: regras do co-op do Half-Life: Decay
   (CHalfLifeCoopplay). Não existe no HLSDK: é um CHalfLifeMultiplay com as
   regras de equipe do teamplay (todos no mesmo time), as quedas e o
   reaparecimento do singleplay e os respawns presos ao mp_coopweprespawn. */

#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/player.h"
#include "dlls/weapons.h"
#include "dlls/gamerules.h"
#include "include_asm.h"

#define PLAYER_MAX_SAFE_FALL_SPEED  580
#define DAMAGE_FOR_FALL_SPEED       (float)100 / (1024 - PLAYER_MAX_SAFE_FALL_SPEED)

#define ITEM_RESPAWN_TIME       30
#define WEAPON_RESPAWN_TIME     20
#define AMMO_RESPAWN_TIME       20

#define ENTITY_INTOLERANCE      100

#define SF_NORESPAWN            (1 << 30)

typedef struct ps2cvar_s
{
	char	*name;
	char	*string;
	int		flags;
	float	value;
	struct ps2cvar_s *next;
} ps2cvar_t;

extern "C" ps2cvar_t D_0030FF28;	/* mp_weaponstay */
extern "C" ps2cvar_t D_00310048;	/* mp_coopweprespawn */
#define weaponstay D_0030FF28
#define coopweprespawn D_00310048

class CItem : public CBaseEntity
{
};

extern int gmsgGameMode;
extern BOOL g_fGameOver;

class CHalfLifeCoopplay : public CHalfLifeMultiplay
{
public:
	CHalfLifeCoopplay();

	virtual BOOL IsMultiplayer( void );
	virtual BOOL IsCoOp( void );
	virtual void Think( void );
	virtual BOOL ClientCommand( CBasePlayer *pPlayer, const char *pcmd );
	virtual void UpdateGameMode( CBasePlayer *pPlayer );
	virtual void DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor );
	virtual void PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor );
	virtual void Ps2Virt31( CBaseEntity *pVictim, entvars_t *pKiller, entvars_t *pevInflictor );
	virtual BOOL IsTeamplay( void );
	virtual BOOL ShouldAutoAim( CBasePlayer *pPlayer, edict_t *target );
	virtual int IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled );
	virtual int IPointsForMonsterKill( CBasePlayer *pAttacker, CBaseEntity *pKilled );
	virtual float FlPlayerFallDamage( CBasePlayer *pPlayer );
	virtual BOOL FPlayerCanRespawn( CBasePlayer *pPlayer );
	virtual float FlPlayerSpawnTime( CBasePlayer *pPlayer );
	virtual float FlWeaponRespawnTime( CBasePlayerItem *pWeapon );
	virtual float FlWeaponTryRespawn( CBasePlayerItem *pWeapon );
	virtual int WeaponShouldRespawn( CBasePlayerItem *pWeapon );
	virtual int ItemShouldRespawn( CItem *pItem );
	virtual float FlItemRespawnTime( CItem *pItem );
	virtual int AmmoShouldRespawn( CBasePlayerAmmo *pAmmo );
	virtual float FlAmmoRespawnTime( CBasePlayerAmmo *pAmmo );
	virtual float FlHealthChargerRechargeTime( void );
	virtual int DeadPlayerWeapons( CBasePlayer *pPlayer );
	virtual BOOL FAllowMonsters( void );
	virtual const char *GetGameDescription( void ) { return "HL Coopplay"; }

private:
	BOOL m_DisableDeathMessages;        /* 0x1C */
	BOOL m_DisableDeathPenalty;         /* 0x20 */
};

CHalfLifeCoopplay :: CHalfLifeCoopplay()
{
	m_DisableDeathMessages = FALSE;
	m_DisableDeathPenalty = FALSE;

	CVAR_SET_FLOAT( "mp_allowmonsters", 1 );
}

BOOL CHalfLifeCoopplay :: IsMultiplayer( void )
{
	return TRUE;
}

BOOL CHalfLifeCoopplay :: IsCoOp( void )
{
	return TRUE;
}

void CHalfLifeCoopplay :: Think ( void )
{
	if ( g_fGameOver )
		CHalfLifeMultiplay::Think();
}

BOOL CHalfLifeCoopplay :: ClientCommand( CBasePlayer *pPlayer, const char *pcmd )
{
	if ( FStrEq( pcmd, "menuselect" ) )
	{
		if ( CMD_ARGC() < 2 )
			return TRUE;

		return TRUE;
	}

	return FALSE;
}

void CHalfLifeCoopplay :: UpdateGameMode( CBasePlayer *pPlayer )
{
	MESSAGE_BEGIN( MSG_ONE, gmsgGameMode, NULL, pPlayer->edict() );
		WRITE_BYTE( 1 );
	MESSAGE_END();
}

void CHalfLifeCoopplay :: DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor )
{
}

void CHalfLifeCoopplay :: PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
{
	if ( !m_DisableDeathPenalty )
		CHalfLifeMultiplay::PlayerKilled( pVictim, pKiller, pInflictor );
}

#ifdef NON_MATCHING
/* só a alocação: o alvo carrega o pfnWriteShort do frags depois da conversão (em v0) */
void CHalfLifeCoopplay :: Ps2Virt31( CBaseEntity *pVictim, entvars_t *pKiller, entvars_t *pevInflictor )
{
	CBaseEntity *ktmp = CBaseEntity::Instance( pKiller );

	if ( ktmp && ktmp->IsPlayer() && !pVictim->IsPlayer() )
	{
		CBasePlayer *pPlayer = (CBasePlayer *)ktmp;

		pKiller->frags += IPointsForMonsterKill( pPlayer, pVictim );

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
#else
INCLUDE_ASM("asm/nonmatchings/dlls/coopplay_gamerules", Ps2Virt31__17CHalfLifeCoopplayP11CBaseEntityP9entvars_sT2);
#endif

BOOL CHalfLifeCoopplay :: IsTeamplay( void )
{
	return TRUE;
}

BOOL CHalfLifeCoopplay :: ShouldAutoAim( CBasePlayer *pPlayer, edict_t *target )
{
	CBaseEntity *pTgt = CBaseEntity::Instance( target );
	if ( pTgt && pTgt->IsPlayer() )
	{
		if ( PlayerRelationship( pPlayer, pTgt ) == GR_TEAMMATE )
			return FALSE; // don't autoaim at teammates
	}

	return TRUE;
}

int CHalfLifeCoopplay :: IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled )
{
	if ( !pKilled )
		return 0;

	if ( !pAttacker )
		return 1;

	if ( pAttacker != pKilled && PlayerRelationship( pAttacker, pKilled ) == GR_TEAMMATE )
		return -1;

	return 1;
}

int CHalfLifeCoopplay :: IPointsForMonsterKill( CBasePlayer *pAttacker, CBaseEntity *pKilled )
{
	return pKilled != NULL;
}

float CHalfLifeCoopplay :: FlPlayerFallDamage( CBasePlayer *pPlayer )
{
	pPlayer->m_flFallVelocity -= PLAYER_MAX_SAFE_FALL_SPEED;
	return pPlayer->m_flFallVelocity * DAMAGE_FOR_FALL_SPEED;
}

BOOL CHalfLifeCoopplay :: FPlayerCanRespawn( CBasePlayer *pPlayer )
{
	return TRUE;
}

float CHalfLifeCoopplay :: FlPlayerSpawnTime( CBasePlayer *pPlayer )
{
	return gpGlobals->time;
}

float CHalfLifeCoopplay :: FlWeaponRespawnTime( CBasePlayerItem *pWeapon )
{
	if ( coopweprespawn.value > 0 )
	{
		if ( weaponstay.value > 0 )
		{
			// make sure it's only certain weapons
			if ( !(pWeapon->iFlags() & ITEM_FLAG_LIMITINWORLD) )
			{
				return gpGlobals->time + 0;		// weapon respawns almost instantly
			}
		}

		return gpGlobals->time + WEAPON_RESPAWN_TIME;
	}

	return -1;
}

float CHalfLifeCoopplay :: FlWeaponTryRespawn( CBasePlayerItem *pWeapon )
{
	if ( coopweprespawn.value > 0 )
	{
		if ( pWeapon && pWeapon->m_iId && (pWeapon->iFlags() & ITEM_FLAG_LIMITINWORLD) )
		{
			if ( NUMBER_OF_ENTITIES() < (gpGlobals->maxEntities - ENTITY_INTOLERANCE) )
				return 0;

			// we're past the entity tolerance level,  so delay the respawn
			return FlWeaponRespawnTime( pWeapon );
		}
	}

	return 0;
}

int CHalfLifeCoopplay :: WeaponShouldRespawn( CBasePlayerItem *pWeapon )
{
	if ( coopweprespawn.value > 0 )
	{
		if ( pWeapon->pev->spawnflags & SF_NORESPAWN )
			return GR_WEAPON_RESPAWN_NO;

		return GR_WEAPON_RESPAWN_YES;
	}

	return GR_WEAPON_RESPAWN_NO;
}

int CHalfLifeCoopplay :: ItemShouldRespawn( CItem *pItem )
{
	if ( coopweprespawn.value > 0 )
	{
		if ( pItem->pev->spawnflags & SF_NORESPAWN )
			return GR_ITEM_RESPAWN_NO;

		return GR_ITEM_RESPAWN_YES;
	}

	return GR_ITEM_RESPAWN_NO;
}

float CHalfLifeCoopplay :: FlItemRespawnTime( CItem *pItem )
{
	if ( coopweprespawn.value > 0 )
		return gpGlobals->time + ITEM_RESPAWN_TIME;

	return -1;
}

int CHalfLifeCoopplay :: AmmoShouldRespawn( CBasePlayerAmmo *pAmmo )
{
	if ( coopweprespawn.value > 0 )
	{
		if ( pAmmo->pev->spawnflags & SF_NORESPAWN )
			return GR_AMMO_RESPAWN_NO;

		return GR_AMMO_RESPAWN_YES;
	}

	return GR_AMMO_RESPAWN_NO;
}

float CHalfLifeCoopplay :: FlAmmoRespawnTime( CBasePlayerAmmo *pAmmo )
{
	if ( coopweprespawn.value > 0 )
		return gpGlobals->time + AMMO_RESPAWN_TIME;

	return -1;
}

float CHalfLifeCoopplay :: FlHealthChargerRechargeTime( void )
{
	if ( coopweprespawn.value > 0 )
		return 60;

	return 0;
}

int CHalfLifeCoopplay :: DeadPlayerWeapons( CBasePlayer *pPlayer )
{
	return 9;
}

BOOL CHalfLifeCoopplay :: FAllowMonsters( void )
{
	return TRUE;
}
