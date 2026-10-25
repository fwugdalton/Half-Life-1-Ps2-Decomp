#ifndef DLLS_GAMERULES_H
#define DLLS_GAMERULES_H

/* CGameRules do PS2 (g_pGameRules em g_pGameRules). O vptr fica em 0x10, então há
   0x10 bytes de campos antes; a ordem das virtuais veio da vtable do CHalfLifeTeamplay (0x3783D0). */

class CBasePlayer;
class CBasePlayerItem;
class CBasePlayerAmmo;
class CItem;
class CBaseEntity;

enum
{
    GR_NONE = 0,
    GR_WEAPON_RESPAWN_YES,
    GR_WEAPON_RESPAWN_NO,
    GR_AMMO_RESPAWN_YES,
    GR_AMMO_RESPAWN_NO,
    GR_ITEM_RESPAWN_YES,
    GR_ITEM_RESPAWN_NO,
};

class CGameRules
{
public:
    int m_ps2Unk00;                     /* 0x00 */
    float m_flSlowmoEnd;                /* 0x04: cheat slowmo (singleplay) */
    float m_flNextSlowmo;               /* 0x08 */
    BOOL m_fInfiniteAmmo;               /* 0x0C: cheat_infammo */

    virtual void RefreshSkillData(void);                                /* slot 1 */
    virtual void Think(void) = 0;
    virtual BOOL IsAllowedToSpawn(CBaseEntity *pEntity) = 0;
    virtual BOOL FAllowFlashlight(void) = 0;
    virtual BOOL FShouldSwitchWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon) = 0;
    virtual BOOL GetNextBestWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pCurrentWeapon); /* slot 6 (0x30) */
    virtual BOOL Ps2Virt07(void);       /* sem item_suit no W_Precache */
    virtual BOOL Ps2Virt08(void);       /* com weaponbox no W_Precache */
    virtual BOOL IsTeamplay(void);      /* slot 9 (0x48): o UTIL_TeamsMatch testa este */
    virtual BOOL IsMultiplayer(void);   /* slot 10 (0x50): escolhe as listas curtas de sons */
    virtual BOOL IsCoOp(void);          /* slot 11 */
    virtual const char *GetGameDescription(void) { return "Half-Life"; } /* slot 12; a string sai no .rodata de cada arquivo */
    virtual BOOL ClientConnected(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128]) = 0;
    virtual void InitHUD(CBasePlayer *pl) = 0;                         /* slot 14 */
    virtual void ClientDisconnected(edict_t *pClient) = 0;
    virtual void UpdateGameMode(CBasePlayer *pPlayer) {}               /* slot 16 */
    virtual float FlPlayerFallDamage(CBasePlayer *pPlayer) = 0;
    virtual BOOL FPlayerCanTakeDamage(CBasePlayer *pPlayer, CBaseEntity *pAttacker); /* slot 18 (0x90) */
    virtual BOOL ShouldAutoAim(CBasePlayer *pPlayer, edict_t *target) { return TRUE; }
    virtual void PlayerSpawn(CBasePlayer *pPlayer) = 0;                /* slot 20 */
    virtual void PlayerThink(CBasePlayer *pPlayer) = 0;
    virtual BOOL FPlayerCanRespawn(CBasePlayer *pPlayer) = 0;
    virtual float FlPlayerSpawnTime(CBasePlayer *pPlayer) = 0;
    virtual edict_t *GetPlayerSpawnSpot(CBasePlayer *pPlayer);
    virtual BOOL AllowAutoTargetCrosshair(void) { return TRUE; }      /* slot 25 */
    virtual BOOL ClientCommand(CBasePlayer *pPlayer, const char *pcmd) { return FALSE; }
    virtual void ClientUserInfoChanged(CBasePlayer *pPlayer, char *infobuffer) {}
    virtual int IPointsForKill(CBasePlayer *pAttacker, CBasePlayer *pKilled) = 0;
    virtual void PlayerKilled(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor) = 0;
    virtual void DeathNotice(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor) = 0; /* slot 30 */
    virtual void Ps2Virt31(CBaseEntity *pVictim, entvars_t *pKiller, entvars_t *pevInflictor); /* slot 31 (0xF8): morte de não jogador */
    virtual BOOL CanHavePlayerItem(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon); /* slot 32 (0x100) */
    virtual void PlayerGotWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon);
    virtual int WeaponShouldRespawn(CBasePlayerItem *pWeapon);
    virtual float FlWeaponRespawnTime(CBasePlayerItem *pWeapon);
    virtual float FlWeaponTryRespawn(CBasePlayerItem *pWeapon);
    virtual Vector VecWeaponRespawnSpot(CBasePlayerItem *pWeapon);
    virtual BOOL CanHaveItem(CBasePlayer *pPlayer, CItem *pItem);
    virtual void PlayerGotItem(CBasePlayer *pPlayer, CItem *pItem);
    virtual int ItemShouldRespawn(CItem *pItem);
    virtual float FlItemRespawnTime(CItem *pItem);
    virtual Vector VecItemRespawnSpot(CItem *pItem);
    virtual BOOL CanHaveAmmo(CBasePlayer *pPlayer, const char *pszAmmoName, int iMaxCarry);
    virtual void PlayerGotAmmo(CBasePlayer *pPlayer, char *szName, int iCount);
    virtual int AmmoShouldRespawn(CBasePlayerAmmo *pAmmo); /* slot 45 (0x168) */
    virtual float FlAmmoRespawnTime(CBasePlayerAmmo *pAmmo);
    virtual Vector VecAmmoRespawnSpot(CBasePlayerAmmo *pAmmo);
    virtual float FlHealthChargerRechargeTime(void);                   /* slot 48 */
    virtual float FlHEVChargerRechargeTime(void);
    virtual int DeadPlayerWeapons(CBasePlayer *pPlayer);
    virtual int DeadPlayerAmmo(CBasePlayer *pPlayer);
    virtual const char *GetTeamID(CBaseEntity *pEntity);               /* slot 52 */
    virtual int PlayerRelationship(CBaseEntity *pPlayer, CBaseEntity *pTarget);
    virtual int GetTeamIndex(const char *pTeamName);
    virtual const char *GetIndexedTeamName(int teamIndex);
    virtual BOOL IsValidTeam(const char *pTeamName);
    virtual void ChangePlayerTeam(CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib);
    virtual const char *SetDefaultPlayerTeam(CBasePlayer *pPlayer);    /* slot 58 */
    virtual BOOL PlayTextureSounds(void);
    virtual BOOL PlayFootstepSounds(CBasePlayer *pl, float fvol);
    virtual BOOL FAllowMonsters(void);
    virtual void EndMultiplayerGame(void);                             /* slot 62 */
    virtual void Ps2Virt63(void);
    virtual CBaseEntity *Ps2Virt64(int iPlayerIndex); /* slot 64 (0x200): jogador pelo player_index */
    virtual void Ps2Virt65(BOOL bOn); /* slot 65 (0x208): FALSE/TRUE em volta do GiveNamedItem do game_player_equip */
    virtual void Ps2Virt66(void);
};

#define GR_NOTTEAMMATE  0
#define GR_TEAMMATE     1
#define GR_ENEMY        2
#define GR_ALLY         3
#define GR_NEUTRAL      4

/* CHalfLifeMultiplay (multiplay_gamerules.cpp, 0x1AB410-0x1ACFE0): as virtuais que ele sobrescreve */
class CHalfLifeMultiplay : public CGameRules
{
public:
    CHalfLifeMultiplay();

    virtual void RefreshSkillData(void);
    virtual void Think(void);
    virtual BOOL IsAllowedToSpawn(CBaseEntity *pEntity);
    virtual BOOL FAllowFlashlight(void);
    virtual BOOL FShouldSwitchWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon);
    virtual BOOL GetNextBestWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pCurrentWeapon);
    virtual BOOL Ps2Virt07(void);
    virtual BOOL Ps2Virt08(void);
    virtual BOOL IsMultiplayer(void);
    virtual BOOL ClientConnected(edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[128]);
    virtual void UpdateGameMode(CBasePlayer *pPlayer);
    virtual void InitHUD(CBasePlayer *pl);
    virtual void ClientDisconnected(edict_t *pClient);
    virtual float FlPlayerFallDamage(CBasePlayer *pPlayer);
    virtual BOOL FPlayerCanTakeDamage(CBasePlayer *pPlayer, CBaseEntity *pAttacker);
    virtual void PlayerSpawn(CBasePlayer *pPlayer);
    virtual void PlayerThink(CBasePlayer *pPlayer);
    virtual BOOL FPlayerCanRespawn(CBasePlayer *pPlayer);
    virtual float FlPlayerSpawnTime(CBasePlayer *pPlayer);
    virtual edict_t *GetPlayerSpawnSpot(CBasePlayer *pPlayer);
    virtual BOOL AllowAutoTargetCrosshair(void);
    virtual BOOL ClientCommand(CBasePlayer *pPlayer, const char *pcmd);
    virtual int IPointsForKill(CBasePlayer *pAttacker, CBasePlayer *pKilled);
    virtual void PlayerKilled(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor);
    virtual void DeathNotice(CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor);
    virtual BOOL CanHavePlayerItem(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon);
    virtual void PlayerGotWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon);
    virtual int WeaponShouldRespawn(CBasePlayerItem *pWeapon);
    virtual float FlWeaponRespawnTime(CBasePlayerItem *pWeapon);
    virtual float FlWeaponTryRespawn(CBasePlayerItem *pWeapon);
    virtual Vector VecWeaponRespawnSpot(CBasePlayerItem *pWeapon);
    virtual BOOL CanHaveItem(CBasePlayer *pPlayer, CItem *pItem);
    virtual void PlayerGotItem(CBasePlayer *pPlayer, CItem *pItem);
    virtual int ItemShouldRespawn(CItem *pItem);
    virtual float FlItemRespawnTime(CItem *pItem);
    virtual Vector VecItemRespawnSpot(CItem *pItem);
    virtual void PlayerGotAmmo(CBasePlayer *pPlayer, char *szName, int iCount);
    virtual int AmmoShouldRespawn(CBasePlayerAmmo *pAmmo);
    virtual float FlAmmoRespawnTime(CBasePlayerAmmo *pAmmo);
    virtual Vector VecAmmoRespawnSpot(CBasePlayerAmmo *pAmmo);
    virtual float FlHealthChargerRechargeTime(void);
    virtual float FlHEVChargerRechargeTime(void);
    virtual int DeadPlayerWeapons(CBasePlayer *pPlayer);
    virtual int DeadPlayerAmmo(CBasePlayer *pPlayer);
    virtual const char *GetTeamID(CBaseEntity *pEntity);
    virtual int PlayerRelationship(CBaseEntity *pPlayer, CBaseEntity *pTarget);
    virtual BOOL PlayTextureSounds(void);
    virtual BOOL PlayFootstepSounds(CBasePlayer *pl, float fvol);
    virtual BOOL FAllowMonsters(void);
    virtual void EndMultiplayerGame(void);

    virtual void ChangeLevel(void);                                    /* slot 67 */
    virtual void GoToIntermission(void);                               /* slot 68 (0x220) */

    void SendMOTDToClient(edict_t *client);

    float m_flIntermissionEndTime;      /* 0x14 */
    BOOL m_iEndIntermissionButtonHit;   /* 0x18 */
};

extern CGameRules *g_pGameRules;

#endif
