#ifndef DLLS_PLAYER_H
#define DLLS_PLAYER_H

/* player.h do HLSDK: só os campos de CBasePlayer que o código já usa
   (offsets do PS2; o resto é preenchimento até ser conhecido). */

#ifndef MONSTERS_H
#include "dlls/monsters.h"
#endif

#define PFLAG_ONLADDER      (1 << 0)
#define PFLAG_ONSWING       (1 << 0)
#define PFLAG_ONTRAIN       (1 << 1)
#define PFLAG_ONBARNACLE    (1 << 2)
#define PFLAG_DUCKING       (1 << 3)
#define PFLAG_USING         (1 << 4)
#define PFLAG_OBSERVER      (1 << 5)
#define PFLAG_PS2_NOPICKUP  (1 << 6)   /* PS2: não pega armas nem munição (fora do impulse 101) */

#define MAX_ITEM_TYPES      6
#define MAX_ITEMS           5

#define ITEM_HEALTHKIT      1
#define ITEM_ANTIDOTE       2
#define ITEM_SECURITY       3
#define ITEM_BATTERY        4
#define MAX_AMMO_SLOTS      32

#define WEAPON_IS_ONTARGET  0x40

#define AUTOAIM_10DEGREES   0.1736481776669

typedef enum
{
    PLAYER_IDLE,
    PLAYER_WALK,
    PLAYER_JUMP,
    PLAYER_SUPERJUMP,
    PLAYER_DIE,
    PLAYER_ATTACK1,
    PLAYER_PS2_ATTACK2,     /* PS2: ataque secundário (carga do vortigaunt) */
} PLAYER_ANIM;

class CBasePlayerItem;

class CBasePlayer : public CBaseMonster
{
public:
    int m_iPlayerSound;                         /* 0x2A0 */
    int m_iTargetVolume;                        /* 0x2A4 */
    int m_iWeaponVolume;                        /* 0x2A8 */
    int m_iExtraSoundTypes;                     /* 0x2AC */
    int m_iWeaponFlash;                         /* 0x2B0 */
    float m_flStopExtraSoundTime;               /* 0x2B4 */
    float m_flFlashLightTime;                   /* 0x2B8 */
    int m_iFlashBattery;                        /* 0x2BC */
    int m_afButtonLast;                         /* 0x2C0 */
    int m_afButtonPressed;                      /* 0x2C4 */
    int m_afButtonReleased;                     /* 0x2C8 */
    edict_t *m_pentSndLast; /* 0x2CC */
    float m_flSndRoomtype; /* 0x2D0 */
    float m_flSndRange; /* 0x2D4 */
    float m_flFallVelocity; /* 0x2D8 */
    int m_rgItems[MAX_ITEMS];                   /* 0x2DC */
    byte m_ps2Unk2F0[0x430 - 0x2F0];
    float m_flgeigerRange;                      /* 0x430 */
    byte m_ps2Unk434[0x468 - 0x434];
    BOOL m_fWeapon;                             /* 0x468 */
    EHANDLE m_pTank;                            /* 0x46C */
    byte m_ps2Unk474[0x47C - 0x474];
    BOOL m_fLongJump;                           /* 0x47C */
    byte m_ps2Unk480[0x490 - 0x480];
    union {                                     /* 0x490: no HLSDK é o m_iHideHUD (o func_tank liga o HIDEHUD_WEAPONS aqui) */
        int m_afPhysicsFlags;
        int m_iHideHUD;
    };
    byte m_ps2Unk494[0x498 - 0x494];
    int m_iFOV;                                 /* 0x498 */
    int m_iClientFOV;                           /* 0x49C */
    CBasePlayerItem *m_rgpPlayerItems[MAX_ITEM_TYPES]; /* 0x4A0 */
    CBasePlayerItem *m_pActiveItem;             /* 0x4B8 */
    CBasePlayerItem *m_pClientActiveItem;       /* 0x4BC */
    CBasePlayerItem *m_pLastItem;               /* 0x4C0 */
    int m_rgAmmo[MAX_AMMO_SLOTS];               /* 0x4C4 */
    byte m_ps2Unk544[0x5DC - 0x544];
    BOOL m_fOnTarget;                           /* 0x5DC */
    byte m_ps2Unk5E0[0x5E8 - 0x5E0];
    int m_iDeaths;                              /* 0x5E8 */
    int m_ps2Unk5EC;
    int m_ps2ShotsFired;                        /* 0x5F0: estatística do placar do PS2 */
    int m_ps2ShotsHit;                          /* 0x5F4 */
    int m_ps2Stat5F8;                           /* 0x5F8 */
    byte m_ps2Unk5FC[0x60C - 0x5FC];
    float m_flNextDecalTime;                    /* 0x60C */
    char m_szTeamName[16];                      /* 0x610 */
    int m_ps2ClientIndex;                       /* 0x620: vai no ScoreInfo com um jogador só */
    Vector m_ps2Unk624[6];                      /* 0x624: o construtor do PS2 tem um laço de 6 Vector (posição exata desconhecida) */
    /* Vector soltos (posição desconhecida): sem eles o g++ inlinaria o construtor do CBasePlayer,
       que no PS2 sai fora de linha (__11CBasePlayer) */
    Vector m_ps2Vec66C;
    Vector m_ps2Vec678;
    Vector m_ps2Vec684;
    Vector m_ps2Vec690;
    Vector m_ps2Vec69C;
    Vector m_ps2Vec6A8;
    Vector m_ps2Vec6B4;
    Vector m_ps2Vec6C0;
    Vector m_ps2Vec6CC;
    Vector m_ps2Vec6D8;
    Vector m_ps2Vec6E4;
    Vector m_ps2Vec6F0;
    byte m_ps2Unk6FC[0x770 - 0x6FC];
    BOOL m_ps2InfiniteAmmo;                     /* 0x770: recarrega sem gastar munição */
    float m_ps2Gravity;                         /* 0x774: gravidade do trigger_gravity */
    BOOL m_ps2ShowScores;                       /* 0x778: coopswitch alterna +showscores/-showscores */
    EHANDLE m_ps2hUnk77C;                       /* 0x77C */
    byte m_ps2Unk784[0x79C - 0x784];
    EHANDLE m_ps2hUnk79C;                       /* 0x79C */
    byte m_ps2Unk7A4[0x7BC - 0x7A4];
    char m_szAnimExtention[32];                 /* 0x7BC */

    /* virtuais novos do CBasePlayer a partir do slot 128 (só o que já é usado) */
    virtual void Ps2Virt128(void);
    virtual void Ps2Virt129(void);
    virtual void Ps2Virt130(void);
    virtual void Ps2Virt131(void);
    virtual void Ps2Virt132(void);
    virtual void Ps2Virt133(void);
    virtual Vector GetAutoaimVector(float flDelta);     /* slot 134 */

    void SetAnimation(PLAYER_ANIM playerAnim);
    void SetSuitUpdate(char *name, int fgroup, int iNoRepeat);
    static int GetAmmoIndex(const char *psz);
    BOOL HasPlayerItem(CBasePlayerItem *pCheckItem);
    void GiveNamedItem(const char *szName);
    void EnableControl(BOOL fControl);
    void RemoveAllItems(BOOL removeSuit);
    BOOL IsOnLadder(void);
    BOOL HasNamedPlayerItem(const char *pszItemName);
    BOOL SwitchWeapon(CBasePlayerItem *pWeapon);
};

extern BOOL gEvilImpulse101;
extern int gmsgCurWeapon;
extern int gmsgWeapPickup;
extern int gmsgScoreInfo;

#endif
