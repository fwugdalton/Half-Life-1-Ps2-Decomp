#ifndef DLLS_EXTDLL_H
#define DLLS_EXTDLL_H

/* Tipos básicos e interface com o motor para o código do jogo (dlls do HLSDK).
   No PS2 a DLL do jogo é ligada estaticamente: as estruturas seguem o motor do
   próprio PS2 (ver include/engine/server.h e progdefs.h). */

typedef int BOOL;
typedef unsigned char byte;
typedef unsigned char BYTE;
typedef unsigned short word;
typedef unsigned int ULONG;
typedef int string_t;
typedef float vec_t;
typedef float vec3_t[3];
typedef int qboolean;

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

extern "C" float sqrtf(float);

#include "dlls/vector.h"

struct edict_s;
typedef struct edict_s edict_t;

/* entvars_t do PS2: igual ao do PC sem endpos/startpos/impacttime/starttime e
   sem playerclass..euser4 */
typedef struct entvars_s
{
    string_t classname;         /* 0x000 */
    string_t globalname;        /* 0x004 */
    Vector origin;              /* 0x008 */
    Vector oldorigin;           /* 0x014 */
    Vector velocity;            /* 0x020 */
    Vector basevelocity;        /* 0x02C */
    Vector clbasevelocity;      /* 0x038 */
    Vector movedir;             /* 0x044 */
    Vector angles;              /* 0x050 */
    Vector avelocity;           /* 0x05C */
    Vector punchangle;          /* 0x068 */
    Vector v_angle;             /* 0x074 */
    int fixangle;               /* 0x080 */
    float idealpitch;           /* 0x084 */
    float pitch_speed;          /* 0x088 */
    float ideal_yaw;            /* 0x08C */
    float yaw_speed;            /* 0x090 */
    int modelindex;             /* 0x094 */
    string_t model;             /* 0x098 */
    int viewmodel;              /* 0x09C */
    int weaponmodel;            /* 0x0A0 */
    Vector absmin;              /* 0x0A4 */
    Vector absmax;              /* 0x0B0 */
    Vector mins;                /* 0x0BC */
    Vector maxs;                /* 0x0C8 */
    Vector size;                /* 0x0D4 */
    float ltime;                /* 0x0E0 */
    float nextthink;            /* 0x0E4 */
    int movetype;               /* 0x0E8 */
    int solid;                  /* 0x0EC */
    int skin;                   /* 0x0F0 */
    int body;                   /* 0x0F4 */
    int effects;                /* 0x0F8 */
    float gravity;              /* 0x0FC */
    float friction;             /* 0x100 */
    int light_level;            /* 0x104 */
    int sequence;               /* 0x108 */
    int gaitsequence;           /* 0x10C */
    float frame;                /* 0x110 */
    float animtime;             /* 0x114 */
    float framerate;            /* 0x118 */
    byte controller[4];         /* 0x11C */
    byte blending[2];           /* 0x120 */
    float scale;                /* 0x124 */
    int rendermode;             /* 0x128 */
    float renderamt;            /* 0x12C */
    Vector rendercolor;         /* 0x130 */
    int renderfx;               /* 0x13C */
    float health;               /* 0x140 */
    float frags;                /* 0x144 */
    int weapons;                /* 0x148 */
    float takedamage;           /* 0x14C */
    int deadflag;               /* 0x150 */
    Vector view_ofs;            /* 0x154 */
    int button;                 /* 0x160 */
    int impulse;                /* 0x164 */
    edict_t *chain;             /* 0x168 */
    edict_t *dmg_inflictor;     /* 0x16C */
    edict_t *enemy;             /* 0x170 */
    edict_t *aiment;            /* 0x174 */
    edict_t *owner;             /* 0x178 */
    edict_t *groundentity;      /* 0x17C */
    int spawnflags;             /* 0x180 */
    int flags;                  /* 0x184 */
    int colormap;               /* 0x188 */
    int team;                   /* 0x18C */
    float max_health;           /* 0x190 */
    float teleport_time;        /* 0x194 */
    float armortype;            /* 0x198 */
    float armorvalue;           /* 0x19C */
    int waterlevel;             /* 0x1A0 */
    int watertype;              /* 0x1A4 */
    string_t target;            /* 0x1A8 */
    string_t targetname;        /* 0x1AC */
    string_t netname;           /* 0x1B0 */
    string_t message;           /* 0x1B4 */
    float dmg_take;             /* 0x1B8 */
    float dmg_save;             /* 0x1BC */
    float dmg;                  /* 0x1C0 */
    float dmgtime;              /* 0x1C4 */
    string_t noise;             /* 0x1C8 */
    string_t noise1;            /* 0x1CC */
    string_t noise2;            /* 0x1D0 */
    string_t noise3;            /* 0x1D4 */
    float speed;                /* 0x1D8 */
    float air_finished;         /* 0x1DC */
    float pain_finished;        /* 0x1E0 */
    float radsuit_finished;     /* 0x1E4 */
    edict_t *pContainingEntity; /* 0x1E8 */
} entvars_t; /* 0x1EC */

struct edict_s
{
    int free;                   /* 0x00 */
    int serialnumber;           /* 0x04 */
    byte unk08[0xAC];           /* area, leafs, baseline, freetime */
    void *pvPrivateData;        /* 0xB4 */
    entvars_t v;                /* 0xB8 */
}; /* 0x2A4 */

typedef struct
{
    float time;                 /* 0x00 */
    float frametime;            /* 0x04 */
    float force_retouch;        /* 0x08 */
    string_t mapname;           /* 0x0C */
    string_t startspot;         /* 0x10 */
    float deathmatch;           /* 0x14 */
    float coop;                 /* 0x18 */
    float teamplay;             /* 0x1C */
    float serverflags;          /* 0x20 */
    float found_secrets;        /* 0x24 */
    Vector v_forward;           /* 0x28 */
    Vector v_up;                /* 0x34 */
    Vector v_right;             /* 0x40 */
    float trace_allsolid;       /* 0x4C */
    float trace_startsolid;     /* 0x50 */
    float trace_fraction;       /* 0x54 */
    Vector trace_endpos;        /* 0x58 */
    Vector trace_plane_normal;  /* 0x64 */
    float trace_plane_dist;     /* 0x70 */
    edict_t *trace_ent;         /* 0x74 */
    float trace_inopen;         /* 0x78 */
    float trace_inwater;        /* 0x7C */
    int trace_hitgroup;         /* 0x80 */
    int trace_flags;            /* 0x84 */
    int msg_entity;             /* 0x88 */
    int cdAudioTrack;           /* 0x8C */
    int maxClients;             /* 0x90 */
    int maxEntities;            /* 0x94 */
    const char *pStringBase;    /* 0x98 */
    void *pSaveData;            /* 0x9C */
} globalvars_t;

typedef struct KeyValueData_s
{
    char *szClassName;          /* 0x00 */
    char *szKeyName;            /* 0x04 */
    char *szValue;              /* 0x08 */
    int fHandled;               /* 0x0C */
} KeyValueData;

typedef struct
{
    int id;                     /* 0x00 */
    edict_t *pent;              /* 0x04 */
    int location;               /* 0x08 */
    int size;                   /* 0x0C */
    int flags;                  /* 0x10 */
    string_t classname;         /* 0x14 */
} ENTITYTABLE;

#define FENTTABLE_PLAYER        0x80000000
#define FENTTABLE_REMOVED       0x40000000
#define FENTTABLE_MOVEABLE      0x20000000
#define FENTTABLE_GLOBAL        0x10000000

typedef struct
{
    char mapName[32];           /* 0x00 */
    char landmarkName[32];      /* 0x20 */
    edict_t *pentLandmark;      /* 0x40 */
    Vector vecLandmarkOrigin;   /* 0x44 */
} LEVELLIST;

#define MAX_LEVEL_CONNECTIONS   16

typedef struct saverestore_s
{
    char *pBaseData;            /* 0x000 */
    char *pCurrentData;         /* 0x004 */
    int size;                   /* 0x008 */
    int bufferSize;             /* 0x00C */
    int tokenSize;              /* 0x010 */
    int tokenCount;             /* 0x014 */
    char **pTokens;             /* 0x018 */
    int currentIndex;           /* 0x01C */
    int tableCount;             /* 0x020 */
    int connectionCount;        /* 0x024 */
    ENTITYTABLE *pTable;        /* 0x028 */
    LEVELLIST levelList[MAX_LEVEL_CONNECTIONS]; /* 0x02C */
    int fUseLandmark;           /* 0x52C */
    char szLandmarkName[20];    /* 0x530 */
    Vector vecLandmarkOffset;   /* 0x544 */
    float time;                 /* 0x550 */
    char szCurrentMapName[32];  /* 0x554 */
} SAVERESTOREDATA;              /* 0x574 */

typedef struct
{
    int fAllSolid;              /* 0x00 */
    int fStartSolid;            /* 0x04 */
    int fInOpen;                /* 0x08 */
    int fInWater;               /* 0x0C */
    float flFraction;           /* 0x10 */
    Vector vecEndPos;           /* 0x14 */
    float flPlaneDist;          /* 0x20 */
    Vector vecPlaneNormal;      /* 0x24 */
    edict_t *pHit;              /* 0x30 */
    int iHitgroup;              /* 0x34 */
} TraceResult;

typedef enum { at_notice, at_console, at_aiconsole, at_warning, at_error, at_logged } ALERT_TYPE;

/* tabela de funções do motor (g_engfuncsExportedToDlls em 0x00334550; cópia da DLL em g_engfuncs).
   Igual à do HLSDK com duas entradas a mais (0x1C e 0x84) */
typedef struct enginefuncs_s
{
    int (*pfnPrecacheModel)(char *s);                                   /* 0x000 */
    int (*pfnPrecacheSound)(char *s);                                   /* 0x004 */
    void (*pfnSetModel)(edict_t *e, const char *m);                     /* 0x008 */
    int (*pfnModelIndex)(const char *m);                                /* 0x00C */
    int (*pfnModelFrames)(int modelIndex);                              /* 0x010 */
    void (*pfnSetSize)(edict_t *e, const float *rgflMin, const float *rgflMax); /* 0x014 */
    void (*pfnChangeLevel)(char *s1, char *s2);                         /* 0x018 */
    void (*pfnUnk1C)(void);                                             /* 0x01C */
    void (*pfnGetSpawnParms)(edict_t *ent);                             /* 0x020 */
    void (*pfnSaveSpawnParms)(edict_t *ent);                            /* 0x024 */
    float (*pfnVecToYaw)(const float *rgflVector);                      /* 0x028 */
    void (*pfnVecToAngles)(const float *rgflVectorIn, float *rgflVectorOut); /* 0x02C */
    void (*pfnMoveToOrigin)(edict_t *ent, const float *pflGoal, float dist, int iMoveType); /* 0x030 */
    void (*pfnChangeYaw)(edict_t *ent);                                 /* 0x034 */
    void (*pfnChangePitch)(edict_t *ent);                               /* 0x038 */
    edict_t *(*pfnFindEntityByString)(edict_t *pEdictStartSearchAfter, const char *pszField, const char *pszValue); /* 0x03C */
    int (*pfnGetEntityIllum)(edict_t *pEnt);                            /* 0x040 */
    edict_t *(*pfnFindEntityInSphere)(edict_t *pEdictStartSearchAfter, const float *org, float rad); /* 0x044 */
    edict_t *(*pfnFindClientInPVS)(edict_t *pEdict);                    /* 0x048 */
    edict_t *(*pfnEntitiesInPVS)(edict_t *pplayer);                     /* 0x04C */
    void (*pfnMakeVectors)(const float *rgflVector);                    /* 0x050 */
    void (*pfnAngleVectors)(const float *rgflVector, float *forward, float *right, float *up); /* 0x054 */
    edict_t *(*pfnCreateEntity)(void);                                  /* 0x058 */
    void (*pfnRemoveEntity)(edict_t *e);                                /* 0x05C */
    edict_t *(*pfnCreateNamedEntity)(int className);                    /* 0x060 */
    void (*pfnMakeStatic)(edict_t *ent);                                /* 0x064 */
    int (*pfnEntIsOnFloor)(edict_t *e);                                 /* 0x068 */
    int (*pfnDropToFloor)(edict_t *e);                                  /* 0x06C */
    int (*pfnWalkMove)(edict_t *ent, float yaw, float dist, int iMode); /* 0x070 */
    void (*pfnSetOrigin)(edict_t *e, const float *rgflOrigin);          /* 0x074 */
    void (*pfnEmitSound)(edict_t *entity, int channel, const char *sample, float volume, float attenuation, int fFlags, int pitch); /* 0x078 */
    void (*pfnEmitAmbientSound)(edict_t *entity, float *pos, const char *samp, float vol, float attenuation, int fFlags, int pitch); /* 0x07C */
    void (*pfnTraceLine)(const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr); /* 0x080 */
    void (*pfnTraceLineNoEnts)(const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr); /* 0x084 */
    void (*pfnTraceToss)(edict_t *pent, edict_t *pentToIgnore, TraceResult *ptr); /* 0x088 */
    int (*pfnTraceMonsterHull)(edict_t *pEdict, const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr); /* 0x08C */
    void (*pfnTraceHull)(const float *v1, const float *v2, int fNoMonsters, int hullNumber, edict_t *pentToSkip, TraceResult *ptr); /* 0x090 */
    void (*pfnTraceModel)(const float *v1, const float *v2, int hullNumber, edict_t *pent, TraceResult *ptr); /* 0x094 */
    const char *(*pfnTraceTexture)(edict_t *pTextureEntity, const float *v1, const float *v2); /* 0x098 */
    void (*pfnTraceSphere)(const float *v1, const float *v2, int fNoMonsters, float radius, edict_t *pentToSkip, TraceResult *ptr); /* 0x09C */
    void (*pfnGetAimVector)(edict_t *ent, float speed, float *rgflReturn); /* 0x0A0 */
    void (*pfnServerCommand)(char *str);                                /* 0x0A4 */
    void (*pfnServerExecute)(void);                                     /* 0x0A8 */
    void (*pfnClientCommand)(edict_t *pEdict, char *szFmt, ...);        /* 0x0AC */
    void (*pfnParticleEffect)(const float *org, const float *dir, float color, float count); /* 0x0B0 */
    void (*pfnLightStyle)(int style, char *val);                        /* 0x0B4 */
    int (*pfnDecalIndex)(const char *name);                             /* 0x0B8 */
    int (*pfnPointContents)(const float *rgflVector);                   /* 0x0BC */
    void (*pfnMessageBegin)(int msg_dest, int msg_type, const float *pOrigin, edict_t *ed); /* 0x0C0 */
    void (*pfnMessageEnd)(void);                                        /* 0x0C4 */
    void (*pfnWriteByte)(int iValue);                                   /* 0x0C8 */
    void (*pfnWriteChar)(int iValue);                                   /* 0x0CC */
    void (*pfnWriteShort)(int iValue);                                  /* 0x0D0 */
    void (*pfnWriteLong)(int iValue);                                   /* 0x0D4 */
    void (*pfnWriteAngle)(float flValue);                               /* 0x0D8 */
    void (*pfnWriteCoord)(float flValue);                               /* 0x0DC */
    void (*pfnWriteString)(const char *sz);                             /* 0x0E0 */
    void (*pfnWriteEntity)(int iValue);                                 /* 0x0E4 */
    void (*pfnCVarRegister)(void *pCvar);                               /* 0x0E8 */
    float (*pfnCVarGetFloat)(const char *szVarName);                    /* 0x0EC */
    const char *(*pfnCVarGetString)(const char *szVarName);             /* 0x0F0 */
    void (*pfnCVarSetFloat)(const char *szVarName, float flValue);      /* 0x0F4 */
    void (*pfnCVarSetString)(const char *szVarName, const char *szValue); /* 0x0F8 */
    void (*pfnAlertMessage)(ALERT_TYPE atype, char *szFmt, ...);        /* 0x0FC */
    void (*pfnEngineFprintf)(void *pfile, char *szFmt, ...);            /* 0x100 */
    void *(*pfnPvAllocEntPrivateData)(edict_t *pEdict, int cb);        /* 0x104 */
    void *(*pfnPvEntPrivateData)(edict_t *pEdict);                      /* 0x108 */
    void (*pfnFreeEntPrivateData)(edict_t *pEdict);                     /* 0x10C */
    const char *(*pfnSzFromIndex)(int iString);                         /* 0x110 */
    int (*pfnAllocString)(const char *szValue);                         /* 0x114 */
    entvars_t *(*pfnGetVarsOfEnt)(edict_t *pEdict);                     /* 0x118 */
    edict_t *(*pfnPEntityOfEntOffset)(int iEntOffset);                  /* 0x11C */
    int (*pfnEntOffsetOfPEntity)(const edict_t *pEdict);                /* 0x120 */
    int (*pfnIndexOfEdict)(const edict_t *pEdict);                      /* 0x124 */
    edict_t *(*pfnPEntityOfEntIndex)(int iEntIndex);                    /* 0x128 */
    edict_t *(*pfnFindEntityByVars)(entvars_t *pvars);                  /* 0x12C */
    void *(*pfnGetModelPtr)(edict_t *pEdict);                           /* 0x130 */
    int (*pfnRegUserMsg)(const char *pszName, int iSize);               /* 0x134 */
    void (*pfnAnimationAutomove)(const edict_t *pEdict, float flTime);  /* 0x138 */
    void (*pfnGetBonePosition)(const edict_t *pEdict, int iBone, float *rgflOrigin, float *rgflAngles); /* 0x13C */
    unsigned int (*pfnFunctionFromName)(const char *pName);            /* 0x140 */
    const char *(*pfnNameForFunction)(unsigned int function);          /* 0x144 */
    void (*pfnClientPrintf)(edict_t *pEdict, int ptype, const char *szMsg); /* 0x148 */
    void (*pfnServerPrint)(const char *szMsg);                          /* 0x14C */
    const char *(*pfnCmd_Args)(void);                                   /* 0x150 */
    const char *(*pfnCmd_Argv)(int argc);                               /* 0x154 */
    int (*pfnCmd_Argc)(void);                                           /* 0x158 */
    void (*pfnGetAttachment)(const edict_t *pEdict, int iAttachment, float *rgflOrigin, float *rgflAngles); /* 0x15C */
    void (*pfnCRC32_Init)(unsigned int *pulCRC);                       /* 0x160 */
    void (*pfnCRC32_ProcessBuffer)(unsigned int *pulCRC, void *p, int len); /* 0x164 */
    void (*pfnCRC32_ProcessByte)(unsigned int *pulCRC, unsigned char ch); /* 0x168 */
    unsigned int (*pfnCRC32_Final)(unsigned int pulCRC);              /* 0x16C */
    int (*pfnRandomLong)(int lLow, int lHigh);                       /* 0x170 */
    float (*pfnRandomFloat)(float flLow, float flHigh);                 /* 0x174 */
    void (*pfnSetView)(const edict_t *pClient, const edict_t *pViewent); /* 0x178 */
    float (*pfnTime)(void);                                             /* 0x17C */
    void (*pfnCrosshairAngle)(const edict_t *pClient, float pitch, float yaw); /* 0x180 */
    byte *(*pfnLoadFileForMe)(char *filename, int *pLength);            /* 0x184 */
    void (*pfnFreeFile)(void *buffer);                                  /* 0x188 */
    void (*pfnEndSection)(const char *pszSectionName);                  /* 0x18C */
    int (*pfnCompareFileTime)(char *filename1, char *filename2, int *iCompare); /* 0x190 */
    void (*pfnGetGameDir)(char *szGetGameDir);                          /* 0x194 */
    void (*pfnCvar_RegisterVariable)(void *variable);                   /* 0x198 */
    void (*pfnFadeClientVolume)(const edict_t *pEdict, int fadePercent, int fadeOutSeconds, int holdTime, int fadeInSeconds); /* 0x19C */
    void (*pfnSetClientMaxspeed)(const edict_t *pEdict, float fNewMaxspeed); /* 0x1A0 */
    edict_t *(*pfnCreateFakeClient)(const char *netname);               /* 0x1A4 */
    void (*pfnRunPlayerMove)(edict_t *fakeclient, const float *viewangles, float forwardmove, float sidemove, float upmove, unsigned short buttons, byte impulse, byte msec); /* 0x1A8 */
    int (*pfnNumberOfEntities)(void);                                   /* 0x1AC */
    char *(*pfnGetInfoKeyBuffer)(edict_t *e);                           /* 0x1B0 */
    char *(*pfnInfoKeyValue)(char *infobuffer, char *key);              /* 0x1B4 */
    void (*pfnSetKeyValue)(char *infobuffer, char *key, char *value);   /* 0x1B8 */
    void (*pfnSetClientKeyValue)(int clientIndex, char *infobuffer, char *key, char *value); /* 0x1BC */
    int (*pfnIsMapValid)(char *filename);                               /* 0x1C0 */
    void (*pfnStaticDecal)(const float *origin, int decalIndex, int entityIndex, int modelIndex); /* 0x1C4 */
    int (*pfnPrecacheGeneric)(char *s);                                 /* 0x1C8 */
    int (*pfnGetPlayerUserId)(edict_t *e);                              /* 0x1CC */
    void (*pfnBuildSoundMsg)(edict_t *entity, int channel, const char *sample, float volume, float attenuation, int fFlags, int pitch, int msg_dest, int msg_type, const float *pOrigin, edict_t *ed); /* 0x1D0 */
    int (*pfnIsDedicatedServer)(void);                                  /* 0x1D4 */
    void *(*pfnCVarGetPointer)(const char *szVarName);                  /* 0x1D8 */
    unsigned int (*pfnGetPlayerWONId)(edict_t *e);                      /* 0x1DC */
    void (*pfnUnk1E0)(void);                                            /* 0x1E0 */
} enginefuncs_t;

extern enginefuncs_t g_engfuncs;        /* g_engfuncs */
extern globalvars_t *gpGlobals;         /* gpGlobals */

#endif
