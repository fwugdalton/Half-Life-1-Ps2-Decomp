#ifndef DLLS_GAMERULES_H
#define DLLS_GAMERULES_H

/* CGameRules do PS2 (g_pGameRules em g_pGameRules). O vptr fica em 0x10, então há
   0x10 bytes de campos antes; a ordem das virtuais ainda não foi conferida. */

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
    int m_ps2Unk00[4];                  /* 0x00 */

    virtual void Ps2Virt01(void);
    virtual void Ps2Virt02(void);
    virtual void Ps2Virt03(void);
    virtual void Ps2Virt04(void);
    virtual void Ps2Virt05(void);
    virtual BOOL GetNextBestWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pCurrentWeapon); /* slot 6 (0x30) */
    virtual BOOL Ps2Virt07(void);       /* sem item_suit no W_Precache */
    virtual BOOL Ps2Virt08(void);       /* com weaponbox no W_Precache */
    virtual BOOL Ps2Virt09(void);       /* slot 9 (0x48): o UTIL_TeamsMatch testa este, não o IsTeamplay */
    virtual BOOL IsMultiplayer(void);   /* slot 10 (0x50): escolhe as listas curtas de sons */
    virtual BOOL IsDeathmatch(void);
    virtual BOOL IsTeamplay(void);
    virtual BOOL IsCoOp(void);
    virtual const char *GetGameDescription(void) { return "Half-Life"; } /* a string sai no .rodata de cada arquivo */
    virtual void Ps2Virt15(void);
    virtual void Ps2Virt16(void);
    virtual void Ps2Virt17(void);
    virtual BOOL Ps2Virt18(CBasePlayer *pPlayer, CBaseEntity *pTarget); /* conta o acerto nas estatísticas */
    virtual void Ps2Virt19(void);
    virtual void Ps2Virt20(void);
    virtual void Ps2Virt21(void);
    virtual void Ps2Virt22(void);
    virtual void Ps2Virt23(void);
    virtual void Ps2Virt24(void);
    virtual void Ps2Virt25(void);
    virtual void Ps2Virt26(void);
    virtual void Ps2Virt27(void);
    virtual void Ps2Virt28(void);
    virtual void Ps2Virt29(void);
    virtual void Ps2Virt30(void);
    virtual void Ps2Virt31(void);
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
    virtual void Ps2Virt48(void);
    virtual void Ps2Virt49(void);
    virtual void Ps2Virt50(void);
    virtual void Ps2Virt51(void);
    virtual void Ps2Virt52(void);
    virtual void Ps2Virt53(void);
    virtual void Ps2Virt54(void);
    virtual void Ps2Virt55(void);
    virtual void Ps2Virt56(void);
    virtual void Ps2Virt57(void);
    virtual void Ps2Virt58(void);
    virtual void Ps2Virt59(void);
    virtual void Ps2Virt60(void);
    virtual void Ps2Virt61(void);
    virtual void Ps2Virt62(void);
    virtual void Ps2Virt63(void);
    virtual CBaseEntity *Ps2Virt64(int iPlayerIndex); /* slot 64 (0x200): jogador pelo player_index */
};

extern CGameRules *g_pGameRules;

#endif
