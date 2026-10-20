#ifndef DLLS_WEAPONS_H
#define DLLS_WEAPONS_H

/* weapons.h do HLSDK (versão antiga, sem previsão no cliente), com os
   campos do PS2 (m_nPlayerIdx do "player_index"). */

#include "dlls/player.h"

#define DLL_GLOBAL

class CBasePlayer;

void DeactivateSatchels(CBasePlayer *pOwner);

#define MAX_WEAPONS         32

#define WEAPON_NOCLIP       -1

#define ITEM_FLAG_SELECTONEMPTY     1
#define ITEM_FLAG_NOAUTORELOAD      2
#define ITEM_FLAG_NOAUTOSWITCHEMPTY 4
#define ITEM_FLAG_LIMITINWORLD      8
#define ITEM_FLAG_EXHAUSTIBLE       16

typedef enum
{
    BULLET_NONE = 0,
    BULLET_PLAYER_9MM,
    BULLET_PLAYER_MP5,
    BULLET_PLAYER_357,
    BULLET_PLAYER_BUCKSHOT,
    BULLET_PLAYER_CROWBAR,

    BULLET_MONSTER_9MM,
    BULLET_MONSTER_MP5,
    BULLET_MONSTER_12MM,
} Bullet;

typedef struct
{
    int iSlot;                  /* 0x00 */
    int iPosition;              /* 0x04 */
    const char *pszAmmo1;       /* 0x08 */
    int iMaxAmmo1;              /* 0x0C */
    const char *pszAmmo2;       /* 0x10 */
    int iMaxAmmo2;              /* 0x14 */
    const char *pszName;        /* 0x18 */
    int iMaxClip;               /* 0x1C */
    int iId;                    /* 0x20 */
    int iFlags;                 /* 0x24 */
    int iWeight;                /* 0x28 */
} ItemInfo;

typedef struct
{
    const char *pszName;
    int iId;
} AmmoInfo;

class CBasePlayerItem : public CBaseAnimating
{
public:
    virtual void KeyValue(KeyValueData *pkvd);
    virtual void SetObjectCollisionBox(void);

    virtual int Save(CSave &save);
    virtual int Restore(CRestore &restore);

    static TYPEDESCRIPTION m_SaveData[];

    virtual int AddToPlayer(CBasePlayer *pPlayer);
    virtual int AddDuplicate(CBasePlayerItem *pItem) { return FALSE; }
    void EXPORT DestroyItem(void);
    void EXPORT DefaultTouch(CBaseEntity *pOther);
    void EXPORT FallThink(void);
    void EXPORT Materialize(void);
    void EXPORT AttemptToMaterialize(void);
    CBaseEntity *Respawn(void);
    void FallInit(void);
    void CheckRespawn(void);
    virtual int GetItemInfo(ItemInfo *p) { return 0; }
    virtual BOOL CanDeploy(void) { return TRUE; }
    virtual BOOL Deploy() { return TRUE; }
    virtual BOOL CanHolster(void) { return TRUE; }
    virtual void Holster(void);
    virtual void UpdateItemInfo(void) { return; }

    virtual void ItemPreFrame(void) { return; }
    virtual void ItemPostFrame(void) { return; }

    virtual void Drop(void);
    virtual void Kill(void);
    virtual void AttachToPlayer(CBasePlayer *pPlayer);

    virtual int PrimaryAmmoIndex() { return -1; }
    virtual int SecondaryAmmoIndex() { return -1; }

    virtual int UpdateClientData(CBasePlayer *pPlayer) { return 0; }

    static ItemInfo ItemInfoArray[MAX_WEAPONS];
    static AmmoInfo AmmoInfoArray[MAX_AMMO_SLOTS];

    CBasePlayer *m_pPlayer;             /* 0x4C */
    CBasePlayerItem *m_pNext;           /* 0x50 */
    int m_iId;                          /* 0x54 */
    int m_nPlayerIdx;                   /* 0x58: PS2 ("player_index") */

    virtual int iItemSlot(void) { return 0; }

    int iItemPosition(void) { return ItemInfoArray[m_iId].iPosition; }
    const char *pszAmmo1(void) { return ItemInfoArray[m_iId].pszAmmo1; }
    int iMaxAmmo1(void) { return ItemInfoArray[m_iId].iMaxAmmo1; }
    const char *pszAmmo2(void) { return ItemInfoArray[m_iId].pszAmmo2; }
    int iMaxAmmo2(void) { return ItemInfoArray[m_iId].iMaxAmmo2; }
    const char *pszName(void) { return ItemInfoArray[m_iId].pszName; }
    int iMaxClip(void) { return ItemInfoArray[m_iId].iMaxClip; }
    int iWeight(void) { return ItemInfoArray[m_iId].iWeight; }
    int iFlags(void) { return ItemInfoArray[m_iId].iFlags; }
};

class CBasePlayerWeapon : public CBasePlayerItem
{
public:
    virtual int Save(CSave &save);
    virtual int Restore(CRestore &restore);

    static TYPEDESCRIPTION m_SaveData[];

    virtual int AddToPlayer(CBasePlayer *pPlayer);
    virtual int AddDuplicate(CBasePlayerItem *pItem);

    virtual int ExtractAmmo(CBasePlayerWeapon *pWeapon);
    virtual int ExtractClipAmmo(CBasePlayerWeapon *pWeapon);

    virtual int AddWeapon(void) { ExtractAmmo(this); return TRUE; }

    BOOL AddPrimaryAmmo(int iCount, char *szName, int iMaxClip, int iMaxCarry);
    BOOL AddSecondaryAmmo(int iCount, char *szName, int iMaxCarry);

    virtual void UpdateItemInfo(void) {}

    int m_iPlayEmptySound;              /* 0x5C */
    int m_fFireOnEmpty;                 /* 0x60 */
    virtual BOOL PlayEmptySound(void);
    virtual void ResetEmptySound(void);

    virtual void SendWeaponAnim(int iAnim);

    virtual BOOL CanDeploy(void);
    virtual BOOL IsUseable(void);
    BOOL DefaultDeploy(char *szViewModel, char *szWeaponModel, int iAnim, char *szAnimExt);
    int DefaultReload(int iClipSize, int iAnim, float fDelay);

    virtual void ItemPostFrame(void);
    virtual void PrimaryAttack(void) { return; }
    virtual void SecondaryAttack(void) { return; }
    virtual void Reload(void) { return; }
    virtual void WeaponIdle(void) { return; }
    virtual int UpdateClientData(CBasePlayer *pPlayer);
    virtual void RetireWeapon(void);
    virtual BOOL ShouldWeaponIdle(void) { return FALSE; }
    virtual void Holster(void);

    int PrimaryAmmoIndex();
    int SecondaryAmmoIndex();

    float m_flNextPrimaryAttack;        /* 0x64 */
    float m_flNextSecondaryAttack;      /* 0x68 */
    float m_flTimeWeaponIdle;           /* 0x6C */
    int m_iPrimaryAmmoType;             /* 0x70 */
    int m_iSecondaryAmmoType;           /* 0x74 */
    int m_iClip;                        /* 0x78 */
    int m_iClientClip;                  /* 0x7C */
    int m_iClientWeaponState;           /* 0x80 */
    int m_fInReload;                    /* 0x84 */
    int m_iDefaultAmmo;                 /* 0x88 */
};

class CBasePlayerAmmo : public CBaseEntity
{
public:
    virtual void Spawn(void);
    void EXPORT DefaultTouch(CBaseEntity *pOther);
    virtual BOOL AddAmmo(CBaseEntity *pOther) { return TRUE; }

    CBaseEntity *Respawn(void);
    void EXPORT Materialize(void);
};

class CGrenade : public CBaseMonster
{
public:
    void Spawn(void);

    typedef enum { SATCHEL_DETONATE = 0, SATCHEL_RELEASE } SATCHELCODE;

    static CGrenade *ShootTimed(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity, float time);
    static CGrenade *ShootContact(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity);
    static CGrenade *ShootSatchelCharge(entvars_t *pevOwner, Vector vecStart, Vector vecVelocity);
    static void UseSatchelCharges(entvars_t *pevOwner, SATCHELCODE code);

    void Explode(Vector vecSrc, Vector vecAim);
    virtual void Explode(TraceResult *pTrace, int bitsDamageType);  /* virtual no PS2 (slot 128) */
    void EXPORT Smoke(void);

    void EXPORT BounceTouch(CBaseEntity *pOther);
    void EXPORT SlideTouch(CBaseEntity *pOther);
    void EXPORT ExplodeTouch(CBaseEntity *pOther);
    void EXPORT DangerSoundThink(void);
    void EXPORT PreDetonate(void);
    void EXPORT Detonate(void);
    void EXPORT DetonateUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
    void EXPORT TumbleThink(void);

    virtual void BounceSound(void);
    virtual int BloodColor(void);       /* fora de linha no PS2 (ggrenade.cpp) */
    virtual void Killed(entvars_t *pevAttacker, int iGib);

    BOOL m_fRegisteredSound;            /* 0x2A0 */
    int m_iPs2Unk2A4;                   /* 0x2A4 */
};

extern DLL_GLOBAL short g_sModelIndexLaser;
extern DLL_GLOBAL const char *g_pModelNameLaser;

extern DLL_GLOBAL short g_sModelIndexLaserDot;
extern DLL_GLOBAL short g_sModelIndexFireball;
extern DLL_GLOBAL short g_sModelIndexSmoke;
extern DLL_GLOBAL short g_sModelIndexWExplosion;
extern DLL_GLOBAL short g_sModelIndexBubbles;
extern DLL_GLOBAL short g_sModelIndexBloodDrop;
extern DLL_GLOBAL short g_sModelIndexBloodSpray;

extern void ClearMultiDamage(void);
extern void ApplyMultiDamage(entvars_t *pevInflictor, entvars_t *pevAttacker);
extern void AddMultiDamage(entvars_t *pevInflictor, CBaseEntity *pEntity, float flDamage, int bitsDamageType);

extern void DecalGunshot(TraceResult *pTrace, int iBulletType);
extern void SpawnBlood(Vector vecSpot, int bloodColor, float flDamage);
extern int DamageDecal(CBaseEntity *pEntity, int bitsDamageType);

typedef struct
{
    CBaseEntity *pEntity;
    float amount;
    int type;
} MULTIDAMAGE;

extern MULTIDAMAGE gMultiDamage;

#define LOUD_GUN_VOLUME         1000
#define NORMAL_GUN_VOLUME       600
#define QUIET_GUN_VOLUME        200

#define BRIGHT_GUN_FLASH        512
#define NORMAL_GUN_FLASH        256
#define DIM_GUN_FLASH           128

#define BIG_EXPLOSION_VOLUME    2048
#define NORMAL_EXPLOSION_VOLUME 1024
#define SMALL_EXPLOSION_VOLUME  512

#define WEAPON_ACTIVITY_VOLUME  64

#define VECTOR_CONE_1DEGREES    Vector(0.00873, 0.00873, 0.00873)
#define VECTOR_CONE_5DEGREES    Vector(0.04362, 0.04362, 0.04362)
#define VECTOR_CONE_15DEGREES   Vector(0.13053, 0.13053, 0.13053)

class CWeaponBox : public CBaseEntity
{
    void Precache(void);
    void Spawn(void);
    void Touch(CBaseEntity *pOther);
    void KeyValue(KeyValueData *pkvd);
    BOOL IsEmpty(void);
    int GiveAmmo(int iCount, char *szName, int iMax, int *pIndex = NULL);
    void SetObjectCollisionBox(void);

public:
    void EXPORT Kill(void);
    int Save(CSave &save);
    int Restore(CRestore &restore);
    static TYPEDESCRIPTION m_SaveData[];

    BOOL HasWeapon(CBasePlayerItem *pCheckItem);
    BOOL PackWeapon(CBasePlayerItem *pWeapon);
    BOOL PackAmmo(int iszName, int iCount);

    CBasePlayerItem *m_rgpPlayerItems[MAX_ITEM_TYPES]; /* 0x30 */
    int m_rgiszAmmo[MAX_AMMO_SLOTS];    /* 0x48 */
    int m_rgAmmo[MAX_AMMO_SLOTS];       /* 0xC8 */
    int m_cAmmoTypes;                   /* 0x148 */
    int m_nPlayerIdx;                   /* 0x14C: PS2 ("player_index") */
    int m_iNumBatteries;                /* 0x150: PS2 ("batteries") */
};

#endif
