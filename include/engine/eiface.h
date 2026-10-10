#ifndef ENGINE_EIFACE_H
#define ENGINE_EIFACE_H

#include "engine/server.h"

struct saverestore_s;

/* interface com a DLL do jogo (mesma ordem do HL: gEntityInterface fica em 0x0053DD70) */
typedef struct
{
    void (*pfnGameInit)(void);                                  /* 0x00 */
    int (*pfnSpawn)(edict_t *pent);                             /* 0x04 */
    void (*pfnThink)(edict_t *pent);                            /* 0x08 */
    void (*pfnUse)(edict_t *pentUsed, edict_t *pentOther);      /* 0x0C */
    void (*pfnTouch)(edict_t *pentTouched, edict_t *pentOther); /* 0x10 */
    void (*pfnBlocked)(edict_t *pentBlocked, edict_t *pentOther); /* 0x14 */
    void (*pfnKeyValue)(edict_t *pentKeyvalue, void *pkvd);     /* 0x18 */
    void (*pfnSave)(edict_t *pent, struct saverestore_s *pSaveData); /* 0x1C */
    int (*pfnRestore)(edict_t *pent, struct saverestore_s *pSaveData, int globalEntity); /* 0x20 */
    void (*pfnSetAbsBox)(edict_t *pent);                        /* 0x24 */
    void *pfnSaveWriteFields;                                   /* 0x28 */
    void *pfnSaveReadFields;                                    /* 0x2C */
    void *pfnSaveGlobalState;                                   /* 0x30 */
    void *pfnRestoreGlobalState;                                /* 0x34 */
    void (*pfnResetGlobalState)(void);                          /* 0x38 */
    int (*pfnClientConnect)(edict_t *pEntity, const char *pszName, const char *pszAddress, char *szRejectReason); /* 0x3C */
    void (*pfnClientDisconnect)(edict_t *pEntity);              /* 0x40 */
    void (*pfnClientKill)(edict_t *pEntity);                    /* 0x44 */
    void (*pfnClientPutInServer)(edict_t *pEntity);             /* 0x48 */
    void (*pfnClientCommand)(edict_t *pEntity);                 /* 0x4C */
    void (*pfnClientUserInfoChanged)(edict_t *pEntity, char *infobuffer); /* 0x50 */
    void (*pfnServerActivate)(edict_t *pEdictList, int edictCount, int clientMax); /* 0x54 */
    /* o PS2 não tem pfnServerDeactivate: daqui em diante tudo sobe 4 bytes */
    void (*pfnPlayerPreThink)(edict_t *pEntity);                /* 0x58 */
    void (*pfnPlayerPostThink)(edict_t *pEntity);               /* 0x5C */
    void (*pfnStartFrame)(void);                                /* 0x60 */
    void (*pfnParmsNewLevel)(void);                             /* 0x64 */
    void (*pfnParmsChangeLevel)(void);                          /* 0x68 */
    const char *(*pfnGetGameDescription)(void);                 /* 0x6C */
    void *pfnPlayerCustomization;                               /* 0x70 */
    void (*pfnSpectatorConnect)(edict_t *pEntity);              /* 0x74 */
    void (*pfnSpectatorDisconnect)(edict_t *pEntity);           /* 0x78 */
} DLL_FUNCTIONS;

extern DLL_FUNCTIONS gEntityInterface;

#endif
