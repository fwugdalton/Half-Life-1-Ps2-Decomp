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

#define MAX_ITEM_TYPES      6
#define MAX_ITEMS           4   /* PS2: 4 (a tabela de save diz 4) */

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

#define CSUITPLAYLIST       4
#define CSUITNOREPEAT       32
#define CBTEXTURENAMEMAX    13
#define TEAM_NAME_LENGTH    16

/* bits do m_iHideHUD */
#define HIDEHUD_WEAPONS         ( 1<<0 )
#define HIDEHUD_FLASHLIGHT      ( 1<<1 )
#define HIDEHUD_ALL             ( 1<<2 )
#define HIDEHUD_HEALTH          ( 1<<3 )
#define HIDEHUD_PS2_NOPICKUP    ( 1<<6 )   /* PS2: modo vortigaunt, não pega armas nem munição (fora do impulse 101) */

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
    edict_t *m_pentSndLast;                     /* 0x2CC */
    float m_flSndRoomtype;                      /* 0x2D0 */
    float m_flSndRange;                         /* 0x2D4 */
    float m_flFallVelocity;                     /* 0x2D8 */
    int m_rgItems[MAX_ITEMS];                   /* 0x2DC */
    int m_fKnownItem;                           /* 0x2EC */
    int m_fNewAmmo;                             /* 0x2F0 */
    unsigned int m_afPhysicsFlags;              /* 0x2F4 */
    float m_fNextSuicideTime;                   /* 0x2F8 */
    float m_flTimeStepSound;                    /* 0x2FC */
    float m_flTimeWeaponIdle;                   /* 0x300 */
    float m_flSwimTime;                         /* 0x304 */
    float m_flDuckTime;                         /* 0x308 */
    float m_flWallJumpTime;                     /* 0x30C */
    float m_flSuitUpdate;                       /* 0x310 */
    int m_rgSuitPlayList[CSUITPLAYLIST];        /* 0x314 */
    int m_iSuitPlayNext;                        /* 0x324 */
    int m_rgiSuitNoRepeat[CSUITNOREPEAT];       /* 0x328 */
    float m_rgflSuitNoRepeatTime[CSUITNOREPEAT];/* 0x3A8 */
    int m_lastDamageAmount;                     /* 0x428 */
    float m_tbdPrev;                            /* 0x42C */
    float m_flgeigerRange;                      /* 0x430 */
    float m_flgeigerDelay;                      /* 0x434 */
    int m_igeigerRangePrev;                     /* 0x438 */
    int m_iStepLeft;                            /* 0x43C */
    char m_szTextureName[CBTEXTURENAMEMAX];     /* 0x440 */
    char m_chTextureType;                       /* 0x44D */
    int m_idrowndmg;                            /* 0x450 */
    int m_idrownrestored;                       /* 0x454 */
    int m_bitsHUDDamage;                        /* 0x458 */
    BOOL m_fInitHUD;                            /* 0x45C */
    BOOL m_fGameHUDInitialized;                 /* 0x460 */
    int m_iTrain;                               /* 0x464 */
    BOOL m_fWeapon;                             /* 0x468 */
    EHANDLE m_pTank;                            /* 0x46C */
    float m_fDeadTime;                          /* 0x474 */
    BOOL m_fNoPlayerSound;                      /* 0x478 */
    BOOL m_fLongJump;                           /* 0x47C */
    float m_tSneaking;                          /* 0x480 */
    int m_iUpdateTime;                          /* 0x484 */
    int m_iClientHealth;                        /* 0x488 */
    int m_iClientBattery;                       /* 0x48C */
    int m_iHideHUD;                             /* 0x490 */
    int m_iClientHideHUD;                       /* 0x494 */
    int m_iFOV;                                 /* 0x498 */
    int m_iClientFOV;                           /* 0x49C */
    CBasePlayerItem *m_rgpPlayerItems[MAX_ITEM_TYPES]; /* 0x4A0 */
    CBasePlayerItem *m_pActiveItem;             /* 0x4B8 */
    CBasePlayerItem *m_pClientActiveItem;       /* 0x4BC */
    CBasePlayerItem *m_pLastItem;               /* 0x4C0 */
    int m_rgAmmo[MAX_AMMO_SLOTS];               /* 0x4C4 */
    int m_rgAmmoLast[MAX_AMMO_SLOTS];           /* 0x544 */
    Vector m_vecAutoAim;                        /* 0x5C4 */
    Vector m_vecAutoAimPrev;                    /* 0x5D0 */
    BOOL m_fOnTarget;                           /* 0x5DC */
    EHANDLE m_hAutoAimTarget;                   /* 0x5E0 */
    int m_iDeaths;                              /* 0x5E8 */
    float m_iRespawnFrames;                     /* 0x5EC */
    int m_ps2ShotsFired;                        /* 0x5F0: estatística do placar do PS2 */
    int m_ps2ShotsHit;                          /* 0x5F4 */
    int m_ps2Stat5F8;                           /* 0x5F8 */
    int m_ps2Unk5FC;                            /* 0x5FC: o Restore põe 1 */
    int m_lastx, m_lasty;                       /* 0x600, 0x604 */
    int m_nCustomSprayFrames;                   /* 0x608 */
    float m_flNextDecalTime;                    /* 0x60C */
    char m_szTeamName[TEAM_NAME_LENGTH];        /* 0x610 */
    int m_iTeamNum;                       /* 0x620: m_iTeamNum na tabela de save; vai no ScoreInfo */
    int m_piSwarmNodeHistory[11];               /* 0x624 */
    int m_iSwarmHistoryIdx;                     /* 0x650 */
    Vector m_vecAITrail[6];                     /* 0x654: últimas posições do dono da IA (rastro que o parceiro segue) */
    int m_iAITrailTail;                         /* 0x69C */
    int m_iAITrailHead;                         /* 0x6A0 */
    Vector m_ps2Vec6A4;                         /* 0x6A4: sem uso conhecido (sem os Vector soltos o g++ inlinaria o construtor) */
    int m_iAITrailIndex;                        /* 0x6B0: ponto do rastro que o parceiro está seguindo */
    Vector m_vecAITrailPos;                     /* 0x6B4 */
    float m_flPs2JumpTime;                      /* 0x6C0: hora do último pulo (pulo duplo) */
    BOOL m_fPs2DoubleJump;                      /* 0x6C4 */
    float m_ps2Unk6C8;
    /* parceiro da IA (aiplayer.cpp, baseado no HPB bot) */
    unsigned int m_iAIState;                    /* 0x6CC */
    unsigned int m_iAIFlags;                    /* 0x6D0 */
    EHANDLE m_hAIUser;                          /* 0x6D4: jogador seguido (pBotUser) */
    Vector m_vecAIPrevOrigin;                   /* 0x6DC */
    float m_flAIMoveSpeed;                      /* 0x6E8: forwardmove do RunPlayerMove */
    float m_flAIMaxSpeed;                       /* 0x6EC: sv_maxspeed */
    float m_flAILadderDir;                      /* 0x6F0: 0 desconhecida, 1 sobe, 2 desce */
    float m_flAIWanderDir;                      /* 0x6F4: 1 ou 2 */
    float m_flAIEndUseLadderTime;                         /* 0x6F8 */
    int m_iAIUnk6FC;                            /* 0x6FC */
    float m_flAIUnk700;                         /* 0x700 */
    int m_iAIUnk704;                            /* 0x704 */
    int m_iAIUnk708;                            /* 0x708 */
    EHANDLE m_hAIPickupItem;                        /* 0x70C */
    float m_flAIUseHealthTime;                         /* 0x714 */
    float m_flAIUseHEVTime;                         /* 0x718 */
    float m_flAIWallOnRight;                     /* 0x71C */
    float m_flAIWallOnLeft;                      /* 0x720 */
    float m_flAIDontAvoidWallTime;                         /* 0x724 */
    int m_iAIUnk728;                            /* 0x728 */
    int m_iAIUnk72C;                            /* 0x72C */
    byte m_bAISkill;                            /* 0x730: mp_aiskill */
    float m_flAIUseTime;                        /* 0x734: última vez que viu o jogador seguido */
    int m_iAIUnk738;                            /* 0x738 */
    int m_iAIUnk73C;                            /* 0x73C */
    float m_flAIUnk740;                         /* 0x740 */
    float m_flAILadderYaw;                      /* 0x744 */
    float m_flAINavigateTime;                         /* 0x748 */
    byte m_ps2AIType;                           /* 0x74C: com 8 o Ps2Look ignora alvos com max_health <= 100 */
    float m_flAIUnk750;                         /* 0x750 */
    float m_flAIUnk754;                         /* 0x754 */
    float m_flAIStuckTime;                      /* 0x758 */
    float m_flAINextAttackSentence;             /* 0x75C */
    Vector m_vecAITarget;                       /* 0x760: ponto do inimigo onde mira */
    float m_flPs2NextLook;                      /* 0x76C: próximo Ps2Look do parceiro da IA */
    BOOL m_fInfiniteAmmo;                     /* 0x770: m_fInfiniteAmmo, recarrega sem gastar munição */
    float m_flDefaultGravity;                         /* 0x774: m_flDefaultGravity, gravidade do trigger_gravity */
    BOOL m_ps2ShowScores;                       /* 0x778: coopswitch alterna +showscores/-showscores */
    EHANDLE m_hTargetedEnemy;                       /* 0x77C: m_hTargetedEnemy */
    Vector m_vecLastAngle;                      /* 0x784 */
    Vector m_vecLastTargetPos;                  /* 0x790 */
    EHANDLE m_ps2hUnk79C;                       /* 0x79C */
    int m_ps2Unk7A4;                            /* 0x7A4: com 1 o Ps2TrackTarget mira no centro do alvo uma vez */
    EHANDLE m_hUseableHighlight;                /* 0x7A8 */
    EHANDLE m_hLastUseableItem;                 /* 0x7B0 */
    float m_flNextUseableCheck;                 /* 0x7B8 */
    char m_szAnimExtention[32];                 /* 0x7BC */

    /* virtuais (a ordem dos novos segue a vtable do PS2, slots 127 em diante) */
    virtual void Spawn(void);
    void Pain(void);
    virtual void Jump(void);                            /* slot 127 */
    virtual void Duck(void);                            /* slot 128 */
    virtual void PreThink(void);                        /* slot 129 */
    virtual void PostThink(void);                       /* slot 130 */
    virtual Vector GetGunPosition(void);
    virtual int TakeHealth(float flHealth, int bitsDamageType);
    virtual void TraceAttack(entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType);
    virtual int TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);
    virtual void Killed(entvars_t *pevAttacker, int iGib);
    virtual int BloodColor(void) { if (m_iHideHUD & HIDEHUD_PS2_NOPICKUP) return BLOOD_COLOR_YELLOW; return m_bloodColor; }
    virtual Vector BodyTarget(const Vector &posSrc) { return Center() + pev->view_ofs * RANDOM_FLOAT(0.5, 1.1); }
    virtual void StartSneaking(void) { m_tSneaking = gpGlobals->time - 1; }
    virtual void StopSneaking(void) { m_tSneaking = gpGlobals->time + 30; }
    virtual BOOL IsSneaking(void) { return m_tSneaking <= gpGlobals->time; }
    virtual BOOL IsAlive(void) { return (pev->deadflag == DEAD_NO) && pev->health > 0; }
    virtual BOOL ShouldFadeOnDeath(void) { return FALSE; }
    virtual BOOL IsPlayer(void) { return TRUE; }
    virtual BOOL IsNetClient(void) { return TRUE; }
    virtual const char *TeamID(void);
    virtual int Save(CSave &save);
    virtual int Restore(CRestore &restore);
    void RenewItems(void);
    void PackDeadPlayerItems(void);
    void RemoveAllItems(BOOL removeSuit);
    BOOL SwitchWeapon(CBasePlayerItem *pWeapon);
    virtual void UpdateClientData(void);                /* slot 131 */
    static TYPEDESCRIPTION m_playerSaveData[];
    virtual int ObjectCaps(void);
    virtual void Precache(void);
    BOOL IsOnLadder(void);
    BOOL FlashlightIsOn(void);
    void FlashlightTurnOn(void);
    void FlashlightTurnOff(void);
    void UpdatePlayerSound(void);
    void DeathSound(void);
    int Classify(void);
    void SetAnimation(PLAYER_ANIM playerAnim);
    virtual void ImpulseCommands(void);                 /* slot 132 */
    void CheatImpulseCommands(int iImpulse);
    void StartDeathCam(void);
    void StartObserver(Vector vecPosition, Vector vecViewAngle);
    void AddPoints(int score, BOOL bAllowNegativeScore);
    void AddPointsToTeam(int score, BOOL bAllowNegativeScore);
    BOOL AddPlayerItem(CBasePlayerItem *pItem);
    BOOL RemovePlayerItem(CBasePlayerItem *pItem);
    void DropPlayerItem(char *pszItemName);
    BOOL HasPlayerItem(CBasePlayerItem *pCheckItem);
    BOOL HasNamedPlayerItem(const char *pszItemName);
    BOOL HasWeapons(void);
    void SelectPrevItem(int iItem);
    void SelectNextItem(int iItem);
    void SelectLastItem(void);
    void SelectItem(const char *pstr);
    void ItemPreFrame(void);
    void ItemPostFrame(void);
    void GiveNamedItem(const char *szName);
    void EnableControl(BOOL fControl);
    int GiveAmmo(int iAmount, char *szName, int iMax);
    void SendAmmoUpdate(void);
    void WaterMove(void);
    void CheckWaterJump(void);                  /* 0x1C58A8: do SDK antigo (Quake) */
    void PlayStepSound(int step, float fvol);   /* 0x1C7AE0 */
    void UpdateStepSound(void);                 /* 0x1C8290 */
    void Ps2Look(int iDistance);                /* 0x1C90C8: cópia do CBaseMonster::Look para o parceiro da IA */
    CBaseEntity *Ps2BestVisibleEnemy(void);     /* 0x1C94B8 */
    BOOL Ps2GetEnemy(void);                     /* 0x1C96A0 */
    void Ps2AcquireTarget(Vector vecSrc, float flDist, float flDot); /* 0x1D1628: trava a mira no alvo da auto-mira (os argumentos não são usados) */
    BOOL Ps2IsSlave(void);                      /* 0x1D1890: a arma ativa é a do vortigaunt (WEAPON_ISLAVE)? */
    void Ps2FindUseable(void);                  /* 0x1C6718: procura o objeto usável à frente (realce) */
    void Ps2HighlightUseable(CBaseEntity *pObject); /* 0x1CECF8 */
    void EXPORT PlayerDeathThink(void);
    void PlayerUse(void);
    void CheckSuitUpdate();
    void SetSuitUpdate(char *name, int fgroup, int iNoRepeat);
    void UpdateGeigerCounter(void);
    void CheckTimeBasedDamage(void);
    BOOL FBecomeProne(void);
    void BarnacleVictimBitten(entvars_t *pevBarnacle);
    void BarnacleVictimReleased(void);
    static int GetAmmoIndex(const char *psz);
    int AmmoInventory(int iAmmoIndex);
    int Illumination(void);
    void ResetAutoaim(void);
    virtual Vector GetAutoaimVector(float flDelta);     /* slot 133 */
    virtual Vector Ps2GetAutoaimVectorOld(float flDelta); /* slot 134: versão do HLSDK (tela dividida) */
    Vector AutoaimDeflection(Vector &vecSrc, float flDist, float flDelta);
    Vector Ps2AutoaimDeflectionOld(Vector &vecSrc, float flDist, float flDelta);
    void Ps2ResetAutoaimOld(void);
    void ForceClientDllUpdate(void);
    /* DeathMessage do HLSDK não existe no PS2 */
    void SetCustomDecalFrames(int nFrames);
    int GetCustomDecalFrames(void);
    void Ps2AISpawnInit(void);                  /* 0x126F80: BotSpawnInit do HPB bot */
    void Ps2AIThink(void);                      /* 0x1270E0: think do parceiro da IA (registro "068") */
    void Ps2AIUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value); /* 0x127500: vazia */
    float Ps2AIChangeYaw(float speed);          /* 0x127508: BotChangeYaw */
    void Ps2AIOnLadder(void);                   /* 0x127700: BotOnLadder */
    BOOL Ps2AIShootAtEnemy(float flMoved);      /* 0x1278F8: BotShootAtEnemy */
    Vector Ps2AIBodyTarget(CBaseEntity *pEnemy); /* 0x127F18: BotBodyTarget */
    BOOL Ps2AIFVisible(const Vector &vecOrigin); /* 0x1284C8: FVisible do HPB bot */
    BOOL Ps2AIFollowUser(float flMoved, float flUnk); /* 0x128568: BotFollowUser */
    BOOL Ps2AICheckWallOnLeft(); /* 0x128998 */
    BOOL Ps2AICheckWallOnRight(); /* 0x128AC0 */
    void Ps2AIFindItem(void); /* 0x128BE8: BotFindItem */
    int Ps2AIInFieldOfView(Vector vecDest); /* 0x12B340: BotInFieldOfView */
    BOOL Ps2AIEntityIsVisible(Vector vecDest); /* 0x12B3E0: BotEntityIsVisible */
    CBasePlayerItem *Ps2AIFindWeapon(int iId); /* 0x12B468 (sem chamadas) */
    void Ps2AIFollowRoute(float flExpected, float flMoved, float flTurn); /* 0x12B5D8 (sem chamadas) */
    void Ps2AIFollowTrail(float flMoved, float flTurn); /* 0x12BAB0: segue o rastro do dono (sem chamadas) */
    BOOL Ps2AIGetTrailPoint(Vector &vecPos);   /* 0x12C2C0 */
    void Ps2AIBackOff(float flMoved);         /* 0x12BD50: anda 100 unidades para trás (sem chamadas) */
    void Ps2AIRecordTrail(void);               /* 0x12C0C8: guarda a posição atual no rastro */
    BOOL Ps2AIFindTrailPoint(Vector &vecPos);  /* 0x12C1D8: primeiro ponto visível do rastro do dono */
    BOOL Ps2AIDodge(Vector vecFrom, float flMoved, float flDist); /* 0x12C358: foge para trás ou para os lados */
    void Ps2AIUseLift(void); /* 0x129618: começo do BotUseLift (sem chamadas) */
    void Ps2AIJumpOutOfWater(void); /* 0x1296C8: sem chamadas */
    BOOL Ps2AICantMoveForward(TraceResult *tr); /* 0x1298E8: BotCantMoveForward (sem chamadas) */
    void Ps2AITurnAtWall(TraceResult *tr); /* 0x12A2D0: BotTurnAtWall (sem chamadas) */
    BOOL Ps2AICanJumpUp(void);                  /* 0x129AF8: BotCanJumpUp */
    BOOL Ps2AICanDuckUnder(void);               /* 0x12A170: BotCanDuckUnder */
    void Ps2AINavigate(float flMoved, Vector vecDest); /* 0x12B4B0 */
    BOOL Ps2AIChangeWeapon(void);               /* 0x12B000: escolhe a melhor arma (BotSelectWeapon) */
    void Ps2AITakeCover(void);                     /* 0x12BE68 */
    BOOL Ps2AIFireWeapon(Vector vecTarget, Vector vecDir, int iType, BOOL fUnk); /* 0x12A5A8: BotFireWeapon */
    void Ps2AITouch(CBaseEntity *pOther);       /* 0x12BFB8: touch do parceiro da IA (registro "069") */
    void PlaySentence(const char *pszSentence, float duration, float volume, float attenuation);   /* 0x12CE30 (arquivo da IA) */
    int ISoundMask(void);                       /* 0x1270D8 (arquivo da IA) */
    void Ps2Unk58(void);
    BOOL Ps2SameTeam(CBaseEntity *pOther);      /* 0x1D0D58 */
    BOOL Ps2SameTeam(entvars_t *pevOther);      /* 0x1D0DB8 */
    void Ps2TrackTarget(int iPlayerIndex, Vector &vecAngles, Vector &vecTargetPos); /* 0x1D0E60: chamado pelo CGameRules::Ps2Virt63 */
    void Ps2AISwitchWeapon(CBasePlayerItem *pWeapon); /* 0x12CDE0 (arquivo da IA) */
    void Ps2AIKilled(void);                     /* 0x12CCD0 (arquivo da IA) */
    int Ps2AITakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType); /* 0x12C878 (arquivo da IA) */
};

extern BOOL gEvilImpulse101;
extern int gmsgCurWeapon;
extern int gmsgWeapPickup;
extern int gmsgScoreInfo;

#endif
