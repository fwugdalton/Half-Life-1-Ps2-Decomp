/* dlls/decay_gamerules.cpp: regras do co-op do Half-Life: Decay no PS2
   (CHalfLifeDecay). Não existe no HLSDK e o nome original não aparece no
   jogo: classe, métodos e campos são inventados. É uma cópia do
   CHalfLifeCoopplay (coopplay_gamerules.cpp) com as duas personagens
   (Gina e Colette, ou X-8973 e R-4913 com mp_coopslave): cada jogador ganha
   um índice 1/2 (m_ps2ClientIndex), o parceiro controlado pela IA é um
   cliente falso criado no Think ou pelo "coopsidekick", e o "coopswitch" com
   um controle só troca o jogador de corpo com o parceiro (SwapPlayers). */

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

#define WEAPON_SUIT             31

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
extern "C" ps2cvar_t D_00310060;	/* mp_coopslave: o parceiro é o vortigaunt */
extern "C" ps2cvar_t D_003325D8;	/* cheat_slowmo */
extern "C" ps2cvar_t D_003325F0;	/* cheat_infammo */
#define weaponstay D_0030FF28
#define coopweprespawn D_00310048
#define coopslave D_00310060
#define cheat_slowmo D_003325D8
#define cheat_infammo D_003325F0

extern "C" int cl_numlocal;
extern "C" int D_00317D28;			/* mensagem do InitHUD (cor do HUD) */
extern "C" void func_001CDE70( CBasePlayer *pPlayer );
/* client.cpp: ClientConnect */
extern "C" BOOL func_00147160( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] );
/* inicialização do jogador da IA (arquivo ainda em asm) */
extern "C" void func_00126F80( CBasePlayer *pPlayer );
/* lanterna do CBasePlayer (player.cpp) */
extern "C" BOOL func_001CC800( CBasePlayer *pPlayer );	/* FlashlightIsOn */
extern "C" void func_001CC810( CBasePlayer *pPlayer );	/* FlashlightTurnOn */
extern "C" void func_001CC928( CBasePlayer *pPlayer );	/* FlashlightTurnOff */
extern "C" void func_00154318( void );
extern "C" void func_002397D0( const char *name, BASEPTR pfn );
extern "C" void func_002397F0( const char *name, ENTITYFUNCPTR pfn );
extern int DispatchSpawn( edict_t *pent );

class CItem : public CBaseEntity
{
};

extern int gmsgGameMode;
extern BOOL g_fGameOver;

/* campos do CBasePlayer ainda sem nome no player.h */
#define PLAYER_AI_6CC( p )	( *(int *)( (char *)(p) + 0x6CC ) )
#define PLAYER_AI_6D4( p )	( *(EHANDLE *)( (char *)(p) + 0x6D4 ) )

class CHalfLifeDecay : public CHalfLifeMultiplay
{
public:
	CHalfLifeDecay();

	int AddPlayer( CBasePlayer *pPlayer );
	int FindPlayer( CBasePlayer *pPlayer );
	virtual CBaseEntity *Ps2Virt64( int iPlayerIndex );
	void RemovePlayer( CBasePlayer *pPlayer );
	void CreateAIPlayer( void );
	void SwapPlayers( CBasePlayer *pA, CBasePlayer *pB );
	void CoopSwitch( void );
	BOOL ReplaceAIPlayer( Vector &vecOrigin, Vector &vecAngles, CBasePlayer *pPlayer );

	virtual BOOL Ps2Virt07( void );
	virtual BOOL Ps2Virt08( void );
	virtual BOOL IsMultiplayer( void );
	virtual void RefreshSkillData( void );
	virtual void Think( void );
	virtual BOOL ClientCommand( CBasePlayer *pPlayer, const char *pcmd );
	virtual void PlayerSpawn( CBasePlayer *pPlayer );
	virtual void InitHUD( CBasePlayer *pl );
	virtual BOOL ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] );
	virtual void ClientDisconnected( edict_t *pClient );
	virtual void UpdateGameMode( CBasePlayer *pPlayer );
	virtual void DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor );
	virtual void PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor );
	virtual void Ps2Virt31( CBaseEntity *pVictim, entvars_t *pKiller, entvars_t *pevInflictor );
	virtual BOOL IsTeamplay( void );
	virtual BOOL FPlayerCanTakeDamage( CBasePlayer *pPlayer, CBaseEntity *pAttacker );
	virtual void PlayerThink( CBasePlayer *pPlayer );
	virtual int PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget );
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
	virtual float FlHEVChargerRechargeTime( void );
	virtual int DeadPlayerWeapons( CBasePlayer *pPlayer );
	virtual BOOL FAllowMonsters( void );
	virtual edict_t *GetPlayerSpawnSpot( CBasePlayer *pPlayer );
	virtual BOOL FAllowFlashlight( void );
	virtual const char *GetGameDescription( void ) { return "HL Coopplay"; }
	virtual BOOL PlayTextureSounds( void ) { return TRUE; }

	BOOL m_DisableDeathMessages;        /* 0x1C */
	BOOL m_DisableDeathPenalty;         /* 0x20 */
	unsigned char m_rgPlayers[ 2 ];     /* 0x24: ENTINDEX dos jogadores 1 e 2 */
	int m_iSwitchState;                 /* 0x28: 1/2, alterna a cada coopswitch */
	int m_iNumPlayers;                  /* 0x2C */
	int m_iNumAIPlayers;                /* 0x30 */
	BOOL m_fUseSpawnSpot;               /* 0x34 */
	Vector m_vecSpawnOrigin;            /* 0x38 */
	Vector m_vecSpawnAngles;            /* 0x44 */
	BOOL m_fAIPlayersCreated;           /* 0x50 */
	float m_flAIPlayerTime;             /* 0x54 */
	BOOL m_fUpdateBodies;               /* 0x58 */
};

int CHalfLifeDecay :: AddPlayer( CBasePlayer *pPlayer )
{
	for ( int i = 0; i < 2; i++ )
	{
		if ( !m_rgPlayers[ i ] )
		{
			m_rgPlayers[ i ] = ENTINDEX( pPlayer->edict() );
			m_iNumPlayers++;
			return i + 1;
		}
	}

	return 0;
}

int CHalfLifeDecay :: FindPlayer( CBasePlayer *pPlayer )
{
	for ( int i = 0; i < 2; i++ )
	{
		if ( m_rgPlayers[ i ] == ENTINDEX( pPlayer->edict() ) )
			return i + 1;
	}

	return 0;
}

CBaseEntity *CHalfLifeDecay :: Ps2Virt64( int iPlayerIndex )
{
	CBasePlayer *pPlayer = NULL;
	int i;

	for ( i = 1; i <= gpGlobals->maxClients; i++ )
	{
		edict_t *pEdict = INDEXENT( i );

		if ( FNullEnt( pEdict ) )
			continue;

		if ( pEdict->free )
			continue;

		pPlayer = (CBasePlayer *)CBaseEntity::Instance( pEdict );
		if ( pPlayer && pPlayer->m_ps2ClientIndex == iPlayerIndex )
			break;
	}

	if ( i > gpGlobals->maxClients )
		return NULL;

	return pPlayer;
}

void CHalfLifeDecay :: RemovePlayer( CBasePlayer *pPlayer )
{
	for ( int i = 0; i < 2; i++ )
	{
		if ( m_rgPlayers[ i ] == ENTINDEX( pPlayer->edict() ) )
		{
			m_rgPlayers[ i ] = 0;
			m_iNumPlayers--;
			return;
		}
	}
}

void CHalfLifeDecay :: CreateAIPlayer( void )
{
	edict_t *pent;

	if ( coopslave.value > 0 )
		pent = (*g_engfuncs.pfnCreateFakeClient)( "R-4913" );
	else
		pent = (*g_engfuncs.pfnCreateFakeClient)( "Colette" );

	if ( FNullEnt( pent ) )
	{
		UTIL_ClientPrintAll( HUD_PRINTNOTIFY, "Max. Players reached.  Can't create AI Player!\n" );
		(*g_engfuncs.pfnIsDedicatedServer)();
		return;
	}

	CBasePlayer *pPlayer = GetClassPtr( (CBasePlayer *)VARS( pent ) );
	char *infobuffer = (*g_engfuncs.pfnGetInfoKeyBuffer)( pPlayer->edict() );
	int index = ENTINDEX( pPlayer->edict() );
	char szRejectReason[ 128 ];

	if ( coopslave.value > 0 )
		(*g_engfuncs.pfnSetClientKeyValue)( index, infobuffer, "model", "dm_slave" );
	else
		(*g_engfuncs.pfnSetClientKeyValue)( index, infobuffer, "model", "ginacol" );

	if ( coopslave.value > 0 )
		func_00147160( pPlayer->edict(), "R-4913", "127.0.0.1", szRejectReason );
	else
		func_00147160( pPlayer->edict(), "Colette", "127.0.0.1", szRejectReason );

	DispatchSpawn( pPlayer->edict() );
	func_00126F80( pPlayer );
	m_iNumAIPlayers++;
}

void CHalfLifeDecay :: SwapPlayers( CBasePlayer *pA, CBasePlayer *pB )
{
	Vector vecTemp;
	int iTemp;
	float flTemp;
	int i;
	CBasePlayerItem *pItem;

	if ( !pA || !pA->IsAlive() || !pB )
		return;

	vecTemp = pB->pev->origin;
	pB->pev->origin = pA->pev->origin;
	UTIL_SetOrigin( pB->pev, pB->pev->origin );
	pA->pev->origin = vecTemp;
	UTIL_SetOrigin( pA->pev, pA->pev->origin );

	vecTemp = pB->pev->v_angle;
	pB->pev->v_angle = pA->pev->v_angle;
	pA->pev->v_angle = vecTemp;
	pB->pev->fixangle = TRUE;
	pA->pev->fixangle = TRUE;

	vecTemp = pB->pev->angles;
	pB->pev->angles = pA->pev->angles;
	pA->pev->angles = vecTemp;
	pB->pev->ideal_yaw = pB->pev->angles.y;
	pA->pev->ideal_yaw = pA->pev->angles.y;

	vecTemp = pB->pev->velocity;
	pB->pev->velocity = pA->pev->velocity;
	pA->pev->velocity = vecTemp;

	vecTemp = pB->pev->avelocity;
	pB->pev->avelocity = pA->pev->avelocity;
	pA->pev->avelocity = vecTemp;

	{
		char szModel[ 1024 ];
		char *infoB = (*g_engfuncs.pfnGetInfoKeyBuffer)( pB->edict() );
		strcpy( szModel, (*g_engfuncs.pfnInfoKeyValue)( infoB, "model" ) );
		char *infoA = (*g_engfuncs.pfnGetInfoKeyBuffer)( pA->edict() );
		(*g_engfuncs.pfnSetClientKeyValue)( ENTINDEX( pB->edict() ), infoB, "model", (*g_engfuncs.pfnInfoKeyValue)( infoA, "model" ) );
		(*g_engfuncs.pfnSetClientKeyValue)( ENTINDEX( pA->edict() ), infoA, "model", szModel );
	}

	{
		CBasePlayerItem *rgItems[ MAX_ITEM_TYPES ];
		memcpy( rgItems, pB->m_rgpPlayerItems, sizeof( rgItems ) );
		memcpy( pB->m_rgpPlayerItems, pA->m_rgpPlayerItems, sizeof( rgItems ) );
		memcpy( pA->m_rgpPlayerItems, rgItems, sizeof( rgItems ) );
	}

	for ( i = 0; i < MAX_ITEM_TYPES; i++ )
	{
		pItem = pB->m_rgpPlayerItems[ i ];
		while ( pItem )
		{
			pItem->m_pPlayer = pB;
			pItem = pItem->m_pNext;
		}
	}

	for ( i = 0; i < MAX_ITEM_TYPES; i++ )
	{
		pItem = pA->m_rgpPlayerItems[ i ];
		while ( pItem )
		{
			pItem->m_pPlayer = pA;
			pItem = pItem->m_pNext;
		}
	}

	{
		pItem = pB->m_pActiveItem;
		pB->m_pActiveItem = pA->m_pActiveItem;
		pA->m_pActiveItem = pItem;
		pItem = pB->m_pLastItem;
		pB->m_pLastItem = pA->m_pLastItem;
		pA->m_pLastItem = pItem;
	}

	{
		int rgAmmo[ MAX_AMMO_SLOTS ];
		memcpy( rgAmmo, pB->m_rgAmmo, sizeof( rgAmmo ) );
		memcpy( pB->m_rgAmmo, pA->m_rgAmmo, sizeof( rgAmmo ) );
		memcpy( pA->m_rgAmmo, rgAmmo, sizeof( rgAmmo ) );
	}

	pB->SwitchWeapon( pB->m_pActiveItem );
	pA->SwitchWeapon( pA->m_pActiveItem );

	flTemp = pB->pev->health;
	pB->pev->health = pA->pev->health;
	pA->pev->health = flTemp;

	flTemp = pB->pev->armorvalue;
	pB->pev->armorvalue = pA->pev->armorvalue;
	pA->pev->armorvalue = flTemp;

	flTemp = pB->m_flFallVelocity;
	pB->m_flFallVelocity = pA->m_flFallVelocity;
	pA->m_flFallVelocity = flTemp;

	iTemp = pB->pev->weapons;
	pB->pev->weapons = pA->pev->weapons;
	pA->pev->weapons = iTemp;

	iTemp = pB->pev->deadflag;
	pB->pev->deadflag = pA->pev->deadflag;
	pA->pev->deadflag = iTemp;

	vecTemp = pB->pev->view_ofs;
	pB->pev->view_ofs = pA->pev->view_ofs;
	pA->pev->view_ofs = vecTemp;

	iTemp = pB->pev->movetype;
	pB->pev->movetype = pA->pev->movetype;
	pA->pev->movetype = iTemp;

	iTemp = pB->pev->solid;
	pB->pev->solid = pA->pev->solid;
	pA->pev->solid = iTemp;

	iTemp = pB->pev->flags;
	pB->pev->flags = pA->pev->flags;
	pA->pev->flags = iTemp;
	pA->pev->flags |= FL_FAKECLIENT;

	flTemp = pB->pev->takedamage;
	pB->pev->takedamage = pA->pev->takedamage;
	pA->pev->takedamage = flTemp;

	vecTemp = pB->pev->mins;
	pB->pev->mins = pA->pev->mins;
	pA->pev->mins = vecTemp;

	vecTemp = pB->pev->maxs;
	pB->pev->maxs = pA->pev->maxs;
	pA->pev->maxs = vecTemp;

	UTIL_SetSize( pB->pev, pB->pev->mins, pB->pev->maxs );
	UTIL_SetSize( pA->pev, pA->pev->mins, pA->pev->maxs );

	flTemp = pB->m_flFlashLightTime;
	pB->m_flFlashLightTime = pA->m_flFlashLightTime;
	pA->m_flFlashLightTime = flTemp;

	iTemp = pB->m_iFlashBattery;
	pB->m_iFlashBattery = pA->m_iFlashBattery;
	pA->m_iFlashBattery = iTemp;

	iTemp = func_001CC800( pB );
	if ( func_001CC800( pA ) )
		func_001CC810( pB );
	else
		func_001CC928( pB );

	if ( iTemp )
		func_001CC810( pA );
	else
		func_001CC928( pA );

	iTemp = pB->pev->sequence;
	pB->pev->sequence = pA->pev->sequence;
	pA->pev->sequence = iTemp;

	iTemp = pB->pev->gaitsequence;
	pB->pev->gaitsequence = pA->pev->gaitsequence;
	pA->pev->gaitsequence = iTemp;

	flTemp = pB->pev->frame;
	pB->pev->frame = pA->pev->frame;
	pA->pev->frame = flTemp;

	pB->m_hEnemy = NULL;
	pA->m_hEnemy = NULL;

	iTemp = pB->m_ps2ClientIndex;
	pB->m_ps2ClientIndex = pA->m_ps2ClientIndex;
	pA->m_ps2ClientIndex = iTemp;

	flTemp = pB->pev->frags;
	pB->pev->frags = pA->pev->frags;
	pA->pev->frags = flTemp;

	iTemp = pB->m_iDeaths;
	pB->m_iDeaths = pA->m_iDeaths;
	pA->m_iDeaths = iTemp;

	iTemp = pB->m_ps2ShotsFired;
	pB->m_ps2ShotsFired = pA->m_ps2ShotsFired;
	pA->m_ps2ShotsFired = iTemp;

	iTemp = pB->m_ps2ShotsHit;
	pB->m_ps2ShotsHit = pA->m_ps2ShotsHit;
	pA->m_ps2ShotsHit = iTemp;

	iTemp = pB->m_ps2Stat5F8;
	pB->m_ps2Stat5F8 = pA->m_ps2Stat5F8;
	pA->m_ps2Stat5F8 = iTemp;

	if ( coopslave.value <= 0 )
	{
		if ( pB->pev->modelindex > 0 )
		{
			if ( pB->m_ps2ClientIndex == 1 )
			{
				pB->SetBodygroup( 1, 1 );
				pB->pev->skin = 1;
			}
			else
			{
				pB->SetBodygroup( 1, 0 );
				pB->pev->skin = 0;
			}
		}

		if ( pA->pev->modelindex > 0 )
		{
			if ( pA->m_ps2ClientIndex == 1 )
			{
				pA->SetBodygroup( 1, 1 );
				pA->pev->skin = 1;
			}
			else
			{
				pA->SetBodygroup( 1, 0 );
				pA->pev->skin = 0;
			}
		}
	}
}

void CHalfLifeDecay :: CoopSwitch( void )
{
	CBasePlayer *pPlayer = (CBasePlayer *)UTIL_PlayerByIndex( m_rgPlayers[ 0 ] );
	CBasePlayer *pOther = (CBasePlayer *)UTIL_PlayerByIndex( m_rgPlayers[ 1 ] );

	if ( !pPlayer || !pOther )
		return;

	if ( !(pPlayer->pev->flags & FL_FAKECLIENT) && !(pOther->pev->flags & FL_FAKECLIENT) )
		return;

	SwapPlayers( pOther, pPlayer );

	pPlayer->pev->flags &= ~FL_FAKECLIENT;
	if ( (CBaseEntity *)pPlayer->m_ps2hUnk77C )
	{
		((CBaseEntity *)pPlayer->m_ps2hUnk77C)->pev->effects &= ~256;
		pPlayer->m_ps2hUnk77C = NULL;
		UTIL_Remove( pPlayer->m_ps2hUnk79C );
		pPlayer->m_ps2hUnk79C = NULL;
		(*g_engfuncs.pfnPs2Unk1E0)( 1, 0 );
	}

	if ( coopslave.value <= 0 )
	{
		if ( pPlayer->m_ps2ClientIndex == 1 )
		{
			MESSAGE_BEGIN( MSG_ONE, D_00317D28, NULL, pPlayer->edict() );
				WRITE_BYTE( 160 );
				WRITE_BYTE( 160 );
				WRITE_BYTE( 192 );
			MESSAGE_END();
		}
		else
		{
			MESSAGE_BEGIN( MSG_ONE, D_00317D28, NULL, pPlayer->edict() );
				WRITE_BYTE( 255 );
				WRITE_BYTE( 128 );
				WRITE_BYTE( 64 );
			MESSAGE_END();
		}
	}

	if ( pOther )
	{
		pOther->pev->flags |= FL_FAKECLIENT;
		PLAYER_AI_6D4( pOther ) = NULL;
		PLAYER_AI_6CC( pOther ) = 0;
		if ( (CBaseEntity *)pOther->m_ps2hUnk77C )
		{
			((CBaseEntity *)pOther->m_ps2hUnk77C)->pev->effects &= ~256;
			pOther->m_ps2hUnk77C = NULL;
			UTIL_Remove( pOther->m_ps2hUnk79C );
			pOther->m_ps2hUnk79C = NULL;
			(*g_engfuncs.pfnPs2Unk1E0)( 1, 0 );
		}
	}

	m_iSwitchState = ( m_iSwitchState == 1 ) ? 2 : 1;
}

BOOL CHalfLifeDecay :: ReplaceAIPlayer( Vector &vecOrigin, Vector &vecAngles, CBasePlayer *pPlayer )
{
	for ( int i = 0; i < 2; i++ )
	{
		CBasePlayer *pAI = (CBasePlayer *)UTIL_PlayerByIndex( m_rgPlayers[ i ] );

		if ( pAI && (pAI->pev->flags & FL_FAKECLIENT) )
		{
			vecOrigin = pAI->pev->origin;

			if ( pPlayer )
				SwapPlayers( pPlayer, pAI );

			pPlayer->pev->flags &= ~FL_FAKECLIENT;
			UTIL_Remove( pAI );
			m_rgPlayers[ i ] = 0;
			m_iNumPlayers--;
			return TRUE;
		}
	}

	return FALSE;
}

CHalfLifeDecay :: CHalfLifeDecay()
{
	m_iSwitchState = 1;
	m_DisableDeathMessages = FALSE;
	m_DisableDeathPenalty = FALSE;
	m_fUseSpawnSpot = FALSE;
	m_fAIPlayersCreated = FALSE;
	m_flAIPlayerTime = 0;

	for ( int i = 0; i < 2; i++ )
		m_rgPlayers[ i ] = 0;

	m_iNumAIPlayers = 0;
	m_iNumPlayers = 0;

	CVAR_SET_FLOAT( "mp_allowmonsters", 1 );

	m_fUpdateBodies = FALSE;
	RefreshSkillData();
	m_flSlowmoEnd = 0;
	m_flNextSlowmo = 0;
}

BOOL CHalfLifeDecay :: Ps2Virt07( void )
{
	return FALSE;
}

BOOL CHalfLifeDecay :: Ps2Virt08( void )
{
	return FALSE;
}

BOOL CHalfLifeDecay :: IsMultiplayer( void )
{
	return TRUE;
}

void CHalfLifeDecay :: RefreshSkillData( void )
{
	CVAR_SET_FLOAT( "skill", 2 );
	CGameRules::RefreshSkillData();
}

void CHalfLifeDecay :: Think ( void )
{
	int i;

	if ( cheat_slowmo.value > 0 && m_flSlowmoEnd <= gpGlobals->time )
		SERVER_COMMAND( "host_framerate 0\n" );

	if ( g_fGameOver )
	{
		CHalfLifeMultiplay::Think();
		return;
	}

	if ( m_fUpdateBodies )
	{
		for ( i = 1; i <= gpGlobals->maxClients; i++ )
		{
			CBasePlayer *pPlayer = (CBasePlayer *)UTIL_PlayerByIndex( i );

			if ( pPlayer )
			{
				if ( pPlayer->m_ps2ClientIndex == 1 )
				{
					pPlayer->SetBodygroup( 1, 1 );
					pPlayer->pev->skin = 1;
				}
				else
				{
					pPlayer->SetBodygroup( 1, 0 );
					pPlayer->pev->skin = 0;
				}
			}
		}

		m_fUpdateBodies = FALSE;
	}

	if ( cl_numlocal < 2 && !m_fAIPlayersCreated && m_iNumPlayers > 0 && m_flAIPlayerTime != 0 && m_flAIPlayerTime < gpGlobals->time )
	{
		int iCount = 2 - m_iNumPlayers;

		if ( iCount > 0 )
		{
			for ( i = 0; i < iCount; i++ )
				CreateAIPlayer();
		}

		m_fAIPlayersCreated = TRUE;
	}
}

BOOL CHalfLifeDecay :: ClientCommand( CBasePlayer *pPlayer, const char *pcmd )
{
	if ( FStrEq( pcmd, "menuselect" ) )
	{
		if ( CMD_ARGC() < 2 )
			return TRUE;

		return TRUE;
	}
	else if ( FStrEq( pcmd, "coopsidekick" ) )
	{
		CreateAIPlayer();
		return TRUE;
	}
	else if ( FStrEq( pcmd, "coopswitch" ) )
	{
		if ( cl_numlocal == 1 )
		{
			CoopSwitch();
			return TRUE;
		}

		const char *pszMap = STRING( gpGlobals->mapname );
		if ( !strncmp( "outro", pszMap + ( strlen( pszMap ) - 5 ), 5 ) )
			return TRUE;

		if ( CMD_ARGC() < 2 )
			return FALSE;

		char szCommand[ 16 ];
		int iClient = atoi( CMD_ARGV( 1 ) );
		CBasePlayer *plr = (CBasePlayer *)UTIL_PlayerByIndex( iClient + 1 );

		plr->m_ps2ShowScores = !plr->m_ps2ShowScores;
		if ( plr->m_ps2ShowScores )
			sprintf( szCommand, "+showscores %d\n", iClient );
		else
			sprintf( szCommand, "-showscores %d\n", iClient );

		CLIENT_COMMAND( plr->edict(), szCommand );
		return TRUE;
	}
	else if ( FStrEq( pcmd, "slowmo" ) )
	{
		if ( cheat_slowmo.value > 0 && m_flNextSlowmo <= gpGlobals->time )
		{
			m_flSlowmoEnd = gpGlobals->time + 4;
			m_flNextSlowmo = gpGlobals->time + 25;
			if ( cl_numlocal > 1 )
				SERVER_COMMAND( "host_framerate 0.007\n" );
			else
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

void CHalfLifeDecay :: PlayerSpawn( CBasePlayer *pPlayer )
{
	CBaseEntity	*pWeaponEntity = NULL;

	pPlayer->pev->weapons |= (1<<WEAPON_SUIT);

	while ( pWeaponEntity = UTIL_FindEntityByClassname( pWeaponEntity, "game_player_equip" ))
	{
		pWeaponEntity->Touch( pPlayer );
	}

	if ( m_iNumAIPlayers > 0 && m_iNumPlayers == 2 )
		ReplaceAIPlayer( m_vecSpawnOrigin, m_vecSpawnAngles, pPlayer );

	pPlayer->m_ps2ClientIndex = AddPlayer( pPlayer );
	GetPlayerSpawnSpot( pPlayer );
	m_flAIPlayerTime = gpGlobals->time + 1;

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
}

void CHalfLifeDecay :: InitHUD( CBasePlayer *pl )
{
	CHalfLifeMultiplay::InitHUD( pl );

	if ( coopslave.value > 0 )
	{
		(*g_engfuncs.pfnSetClientKeyValue)( ENTINDEX( pl->edict() ), (*g_engfuncs.pfnGetInfoKeyBuffer)( pl->edict() ), "model", "dm_slave" );
		if ( pl->m_ps2ClientIndex == 1 )
			(*g_engfuncs.pfnSetClientKeyValue)( ENTINDEX( pl->edict() ), (*g_engfuncs.pfnGetInfoKeyBuffer)( pl->edict() ), "name", "X-8973" );
		else
			(*g_engfuncs.pfnSetClientKeyValue)( ENTINDEX( pl->edict() ), (*g_engfuncs.pfnGetInfoKeyBuffer)( pl->edict() ), "name", "R-4913" );
	}
	else
	{
		(*g_engfuncs.pfnSetClientKeyValue)( ENTINDEX( pl->edict() ), (*g_engfuncs.pfnGetInfoKeyBuffer)( pl->edict() ), "model", "ginacol" );
		if ( pl->m_ps2ClientIndex == 1 )
			(*g_engfuncs.pfnSetClientKeyValue)( ENTINDEX( pl->edict() ), (*g_engfuncs.pfnGetInfoKeyBuffer)( pl->edict() ), "name", "Gina" );
		else
			(*g_engfuncs.pfnSetClientKeyValue)( ENTINDEX( pl->edict() ), (*g_engfuncs.pfnGetInfoKeyBuffer)( pl->edict() ), "name", "Colette" );
	}

	if ( pl->m_ps2ClientIndex == 1 )
	{
		MESSAGE_BEGIN( MSG_ONE, D_00317D28, NULL, pl->edict() );
			WRITE_BYTE( 160 );
			WRITE_BYTE( 160 );
			WRITE_BYTE( 192 );
		MESSAGE_END();
	}
	else
	{
		MESSAGE_BEGIN( MSG_ONE, D_00317D28, NULL, pl->edict() );
			WRITE_BYTE( 255 );
			WRITE_BYTE( 128 );
			WRITE_BYTE( 64 );
		MESSAGE_END();
	}

	if ( m_fInfiniteAmmo )
		func_001CDE70( pl );

	m_fUpdateBodies = TRUE;
}

BOOL CHalfLifeDecay :: ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] )
{
	return TRUE;
}

void CHalfLifeDecay :: ClientDisconnected( edict_t *pClient )
{
}

void CHalfLifeDecay :: UpdateGameMode( CBasePlayer *pPlayer )
{
	MESSAGE_BEGIN( MSG_ONE, gmsgGameMode, NULL, pPlayer->edict() );
		WRITE_BYTE( 1 );
	MESSAGE_END();
}

void CHalfLifeDecay :: DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor )
{
}

void CHalfLifeDecay :: PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
{
	edict_t *pent;

	if ( pVictim->m_ps2ClientIndex == 1 )
		pent = FIND_ENTITY_BY_TARGETNAME( NULL, "decay_player1_dead" );
	else
		pent = FIND_ENTITY_BY_TARGETNAME( NULL, "decay_player2_dead" );

	if ( FNullEnt( pent ) )
	{
		ALERT( at_error, "Could not find trigger_enddecay for player death!\n" );
		return;
	}

	CBaseEntity *pEnd = CBaseEntity::Instance( pent );
	if ( pEnd )
		pEnd->Use( pVictim, pVictim, USE_TOGGLE, 0 );
}

void CHalfLifeDecay :: Ps2Virt31( CBaseEntity *pVictim, entvars_t *pKiller, entvars_t *pevInflictor )
{
	CBaseEntity *ktmp = CBaseEntity::Instance( pKiller );

	if ( ktmp && ktmp->IsPlayer() && !pVictim->IsPlayer() )
	{
		CBasePlayer *pPlayer = (CBasePlayer *)ktmp;

		pKiller->frags += IPointsForMonsterKill( pPlayer, pVictim );

		if ( cl_numlocal == 1 )
		{
			MESSAGE_BEGIN( MSG_ALL, gmsgScoreInfo );
				WRITE_BYTE( pPlayer->m_ps2ClientIndex );
				WRITE_SHORT( (short)pPlayer->pev->frags );
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
				WRITE_SHORT( (short)pPlayer->pev->frags );
				WRITE_SHORT( pPlayer->m_iDeaths );
				WRITE_SHORT( pPlayer->m_ps2Stat5F8 );
				WRITE_SHORT( pPlayer->m_ps2ShotsFired );
				WRITE_SHORT( pPlayer->m_ps2ShotsHit );
			MESSAGE_END();
		}
	}
}

BOOL CHalfLifeDecay :: IsTeamplay( void )
{
	return TRUE;
}

BOOL CHalfLifeDecay :: FPlayerCanTakeDamage( CBasePlayer *pPlayer, CBaseEntity *pAttacker )
{
	if ( pPlayer != pAttacker && pAttacker && pAttacker->IsPlayer() )
		return FALSE;

	return CHalfLifeMultiplay::FPlayerCanTakeDamage( pPlayer, pAttacker );
}

void CHalfLifeDecay :: PlayerThink( CBasePlayer *pPlayer )
{
}

int CHalfLifeDecay :: PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget )
{
	return GR_TEAMMATE;
}

BOOL CHalfLifeDecay :: ShouldAutoAim( CBasePlayer *pPlayer, edict_t *target )
{
	CBaseEntity *pTgt = CBaseEntity::Instance( target );
	if ( pTgt && pTgt->IsPlayer() )
	{
		if ( PlayerRelationship( pPlayer, pTgt ) == GR_TEAMMATE )
			return FALSE; // don't autoaim at teammates
	}

	return TRUE;
}

int CHalfLifeDecay :: IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled )
{
	if ( !pKilled )
		return 0;

	if ( !pAttacker )
		return 1;

	if ( pAttacker != pKilled && PlayerRelationship( pAttacker, pKilled ) == GR_TEAMMATE )
		return -1;

	return 1;
}

int CHalfLifeDecay :: IPointsForMonsterKill( CBasePlayer *pAttacker, CBaseEntity *pKilled )
{
	return pKilled != NULL;
}

float CHalfLifeDecay :: FlPlayerFallDamage( CBasePlayer *pPlayer )
{
	pPlayer->m_flFallVelocity -= PLAYER_MAX_SAFE_FALL_SPEED;
	return pPlayer->m_flFallVelocity * DAMAGE_FOR_FALL_SPEED;
}

BOOL CHalfLifeDecay :: FPlayerCanRespawn( CBasePlayer *pPlayer )
{
	return FALSE;
}

float CHalfLifeDecay :: FlPlayerSpawnTime( CBasePlayer *pPlayer )
{
	return gpGlobals->time;
}

float CHalfLifeDecay :: FlWeaponRespawnTime( CBasePlayerItem *pWeapon )
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

float CHalfLifeDecay :: FlWeaponTryRespawn( CBasePlayerItem *pWeapon )
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

int CHalfLifeDecay :: WeaponShouldRespawn( CBasePlayerItem *pWeapon )
{
	if ( coopweprespawn.value > 0 )
	{
		if ( pWeapon->pev->spawnflags & SF_NORESPAWN )
			return GR_WEAPON_RESPAWN_NO;

		return GR_WEAPON_RESPAWN_YES;
	}

	return GR_WEAPON_RESPAWN_NO;
}

int CHalfLifeDecay :: ItemShouldRespawn( CItem *pItem )
{
	if ( coopweprespawn.value > 0 )
	{
		if ( pItem->pev->spawnflags & SF_NORESPAWN )
			return GR_ITEM_RESPAWN_NO;

		return GR_ITEM_RESPAWN_YES;
	}

	return GR_ITEM_RESPAWN_NO;
}

float CHalfLifeDecay :: FlItemRespawnTime( CItem *pItem )
{
	if ( coopweprespawn.value > 0 )
		return gpGlobals->time + ITEM_RESPAWN_TIME;

	return -1;
}

int CHalfLifeDecay :: AmmoShouldRespawn( CBasePlayerAmmo *pAmmo )
{
	if ( coopweprespawn.value > 0 )
	{
		if ( pAmmo->pev->spawnflags & SF_NORESPAWN )
			return GR_AMMO_RESPAWN_NO;

		return GR_AMMO_RESPAWN_YES;
	}

	return GR_AMMO_RESPAWN_NO;
}

float CHalfLifeDecay :: FlAmmoRespawnTime( CBasePlayerAmmo *pAmmo )
{
	if ( coopweprespawn.value > 0 )
		return gpGlobals->time + AMMO_RESPAWN_TIME;

	return -1;
}

float CHalfLifeDecay :: FlHealthChargerRechargeTime( void )
{
	if ( coopweprespawn.value > 0 )
		return 60;

	return 0;
}

float CHalfLifeDecay :: FlHEVChargerRechargeTime( void )
{
	if ( coopweprespawn.value > 0 )
		return 60;

	return 0;
}

int CHalfLifeDecay :: DeadPlayerWeapons( CBasePlayer *pPlayer )
{
	return 9;
}

BOOL CHalfLifeDecay :: FAllowMonsters( void )
{
	return TRUE;
}

edict_t *CHalfLifeDecay :: GetPlayerSpawnSpot( CBasePlayer *pPlayer )
{
	if ( !m_fUseSpawnSpot )
		return CHalfLifeMultiplay::GetPlayerSpawnSpot( pPlayer );

	pPlayer->pev->origin = m_vecSpawnOrigin + Vector( 0, 0, 1 );
	pPlayer->pev->v_angle = g_vecZero;
	pPlayer->pev->velocity = g_vecZero;
	pPlayer->pev->angles = m_vecSpawnAngles;
	pPlayer->pev->punchangle = g_vecZero;
	pPlayer->pev->fixangle = TRUE;
	m_fUseSpawnSpot = FALSE;
	return NULL;
}

BOOL CHalfLifeDecay :: FAllowFlashlight( void )
{
	return FALSE;
}

extern "C" void func_00154318( void )
{
	func_002397D0( "068", static_cast<BASEPTR>( &CBasePlayer::Ps2AIThink ) );
	func_002397F0( "069", static_cast<ENTITYFUNCPTR>( &CBasePlayer::Ps2AITouch ) );
}
