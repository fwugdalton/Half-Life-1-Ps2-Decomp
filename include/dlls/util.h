#ifndef DLLS_UTIL_H
#define DLLS_UTIL_H

/* util.h e enginecallback.h do HLSDK (só o que o código já usa) */

#include "dlls/extdll.h"

/* o g_vecZero das dlls (0x312C28) é outro símbolo que o do motor (0x3328F8) */
#define g_vecZero g_vecZero_dll
extern Vector g_vecZero;

typedef int EOFFSET;

/* ---- enginecallback.h ---- */
#define PRECACHE_MODEL      (*g_engfuncs.pfnPrecacheModel)
#define PRECACHE_SOUND      (*g_engfuncs.pfnPrecacheSound)
#define SET_MODEL           (*g_engfuncs.pfnSetModel)
#define MODEL_INDEX         (*g_engfuncs.pfnModelIndex)
#define MODEL_FRAMES        (*g_engfuncs.pfnModelFrames)
#define SET_SIZE            (*g_engfuncs.pfnSetSize)
#define CHANGE_LEVEL        (*g_engfuncs.pfnChangeLevel)
#define VEC_TO_YAW          (*g_engfuncs.pfnVecToYaw)
#define VEC_TO_ANGLES       (*g_engfuncs.pfnVecToAngles)
#define MOVE_TO_ORIGIN      (*g_engfuncs.pfnMoveToOrigin)
#define oldCHANGE_YAW       (*g_engfuncs.pfnChangeYaw)
#define CHANGE_PITCH        (*g_engfuncs.pfnChangePitch)
#define MAKE_VECTORS        (*g_engfuncs.pfnMakeVectors)
#define CREATE_ENTITY       (*g_engfuncs.pfnCreateEntity)
#define REMOVE_ENTITY       (*g_engfuncs.pfnRemoveEntity)
#define CREATE_NAMED_ENTITY (*g_engfuncs.pfnCreateNamedEntity)
#define MAKE_STATIC         (*g_engfuncs.pfnMakeStatic)
#define ENT_IS_ON_FLOOR     (*g_engfuncs.pfnEntIsOnFloor)
#define DROP_TO_FLOOR       (*g_engfuncs.pfnDropToFloor)
#define WALK_MOVE           (*g_engfuncs.pfnWalkMove)
#define SET_ORIGIN          (*g_engfuncs.pfnSetOrigin)
#define EMIT_SOUND_DYN1     (*g_engfuncs.pfnEmitSound)
#define BUILD_SOUND_MSG     (*g_engfuncs.pfnBuildSoundMsg)
#define TRACE_LINE          (*g_engfuncs.pfnTraceLine)
#define TRACE_TOSS          (*g_engfuncs.pfnTraceToss)
#define TRACE_MONSTER_HULL  (*g_engfuncs.pfnTraceMonsterHull)
#define TRACE_HULL          (*g_engfuncs.pfnTraceHull)
#define GET_AIM_VECTOR      (*g_engfuncs.pfnGetAimVector)
#define SERVER_COMMAND      (*g_engfuncs.pfnServerCommand)
#define SERVER_EXECUTE      (*g_engfuncs.pfnServerExecute)
#define CLIENT_COMMAND      (*g_engfuncs.pfnClientCommand)
#define PARTICLE_EFFECT     (*g_engfuncs.pfnParticleEffect)
#define LIGHT_STYLE         (*g_engfuncs.pfnLightStyle)
#define DECAL_INDEX         (*g_engfuncs.pfnDecalIndex)
#define POINT_CONTENTS      (*g_engfuncs.pfnPointContents)
#define CRC32_INIT          (*g_engfuncs.pfnCRC32_Init)
#define CRC32_PROCESS_BUFFER (*g_engfuncs.pfnCRC32_ProcessBuffer)
#define CRC32_PROCESS_BYTE  (*g_engfuncs.pfnCRC32_ProcessByte)
#define CRC32_FINAL         (*g_engfuncs.pfnCRC32_Final)
#define RANDOM_LONG         (*g_engfuncs.pfnRandomLong)
#define RANDOM_FLOAT        (*g_engfuncs.pfnRandomFloat)
#define GETPLAYERWONID      (*g_engfuncs.pfnGetPlayerWONId)

#define MESSAGE_END         (*g_engfuncs.pfnMessageEnd)
#define WRITE_BYTE          (*g_engfuncs.pfnWriteByte)
#define WRITE_CHAR          (*g_engfuncs.pfnWriteChar)
#define WRITE_SHORT         (*g_engfuncs.pfnWriteShort)
#define WRITE_LONG          (*g_engfuncs.pfnWriteLong)
#define WRITE_ANGLE         (*g_engfuncs.pfnWriteAngle)
#define WRITE_COORD         (*g_engfuncs.pfnWriteCoord)
#define WRITE_STRING        (*g_engfuncs.pfnWriteString)
#define WRITE_ENTITY        (*g_engfuncs.pfnWriteEntity)

#define CVAR_REGISTER       (*g_engfuncs.pfnCVarRegister)
#define CVAR_GET_FLOAT      (*g_engfuncs.pfnCVarGetFloat)
#define CVAR_GET_STRING     (*g_engfuncs.pfnCVarGetString)
#define CVAR_SET_FLOAT      (*g_engfuncs.pfnCVarSetFloat)
#define CVAR_SET_STRING     (*g_engfuncs.pfnCVarSetString)
#define CVAR_GET_POINTER    (*g_engfuncs.pfnCVarGetPointer)
#define ALERT               (*g_engfuncs.pfnAlertMessage)
#define ENGINE_FPRINTF      (*g_engfuncs.pfnEngineFprintf)
#define ALLOC_PRIVATE       (*g_engfuncs.pfnPvAllocEntPrivateData)
inline void *GET_PRIVATE(edict_t *pent)
{
    if (pent)
        return pent->pvPrivateData;
    return NULL;
}
#define FREE_PRIVATE        (*g_engfuncs.pfnFreeEntPrivateData)
#define ALLOC_STRING        (*g_engfuncs.pfnAllocString)
#define FIND_ENTITY_BY_STRING (*g_engfuncs.pfnFindEntityByString)
#define GETENTITYILLUM      (*g_engfuncs.pfnGetEntityIllum)
#define FIND_ENTITY_IN_SPHERE (*g_engfuncs.pfnFindEntityInSphere)
#define FIND_CLIENT_IN_PVS  (*g_engfuncs.pfnFindClientInPVS)
#define EMIT_AMBIENT_SOUND  (*g_engfuncs.pfnEmitAmbientSound)
#define GET_MODEL_PTR       (*g_engfuncs.pfnGetModelPtr)
#define REG_USER_MSG        (*g_engfuncs.pfnRegUserMsg)
#define GET_BONE_POSITION   (*g_engfuncs.pfnGetBonePosition)
#define FUNCTION_FROM_NAME  (*g_engfuncs.pfnFunctionFromName)
#define NAME_FOR_FUNCTION   (*g_engfuncs.pfnNameForFunction)
#define TRACE_TEXTURE       (*g_engfuncs.pfnTraceTexture)
#define CLIENT_PRINTF       (*g_engfuncs.pfnClientPrintf)
#define CMD_ARGS            (*g_engfuncs.pfnCmd_Args)
#define CMD_ARGC            (*g_engfuncs.pfnCmd_Argc)
#define CMD_ARGV            (*g_engfuncs.pfnCmd_Argv)
#define GET_ATTACHMENT      (*g_engfuncs.pfnGetAttachment)
#define SET_VIEW            (*g_engfuncs.pfnSetView)
#define SET_CROSSHAIRANGLE  (*g_engfuncs.pfnCrosshairAngle)
#define LOAD_FILE_FOR_ME    (*g_engfuncs.pfnLoadFileForMe)
#define FREE_FILE           (*g_engfuncs.pfnFreeFile)
#define COMPARE_FILE_TIME   (*g_engfuncs.pfnCompareFileTime)
#define GET_GAME_DIR        (*g_engfuncs.pfnGetGameDir)
#define IS_MAP_VALID        (*g_engfuncs.pfnIsMapValid)
#define NUMBER_OF_ENTITIES  (*g_engfuncs.pfnNumberOfEntities)
#define IS_DEDICATED_SERVER (*g_engfuncs.pfnIsDedicatedServer)
#define PRECACHE_GENERIC    (*g_engfuncs.pfnPrecacheGeneric)
#define GETPLAYERUSERID     (*g_engfuncs.pfnGetPlayerUserId)

inline void MESSAGE_BEGIN(int msg_dest, int msg_type, const float *pOrigin = NULL, edict_t *ed = NULL)
{
    (*g_engfuncs.pfnMessageBegin)(msg_dest, msg_type, pOrigin, ed);
}

/* ---- util.h ---- */
#define STRING(offset)      (const char *)(gpGlobals->pStringBase + (int)offset)
#define MAKE_STRING(str)    ((int)str - (int)STRING(0))

inline edict_t *FIND_ENTITY_BY_CLASSNAME(edict_t *entStart, const char *pszName)
{
    return FIND_ENTITY_BY_STRING(entStart, "classname", pszName);
}

inline edict_t *FIND_ENTITY_BY_TARGETNAME(edict_t *entStart, const char *pszName)
{
    return FIND_ENTITY_BY_STRING(entStart, "targetname", pszName);
}

inline edict_t *FIND_ENTITY_BY_TARGET(edict_t *entStart, const char *pszName)
{
    return FIND_ENTITY_BY_STRING(entStart, "target", pszName);
}

#define eoNullEntity 0
#define iStringNull 0

inline edict_t *ENT(const entvars_t *pev) { return pev->pContainingEntity; }
inline edict_t *ENT(edict_t *pent) { return pent; }
inline edict_t *ENT(EOFFSET eoffset) { return (*g_engfuncs.pfnPEntityOfEntOffset)(eoffset); }

inline void MESSAGE_BEGIN(int msg_dest, int msg_type, const float *pOrigin, entvars_t *ent)
{
    (*g_engfuncs.pfnMessageBegin)(msg_dest, msg_type, pOrigin, ENT(ent));
}
inline EOFFSET OFFSET(EOFFSET eoffset) { return eoffset; }
inline EOFFSET OFFSET(const edict_t *pent) { return (*g_engfuncs.pfnEntOffsetOfPEntity)(pent); }
inline EOFFSET OFFSET(entvars_t *pev) { return OFFSET(ENT(pev)); }
inline entvars_t *VARS(entvars_t *pev) { return pev; }

inline entvars_t *VARS(edict_t *pent)
{
    if (!pent)
        return NULL;

    return &pent->v;
}

inline entvars_t *VARS(EOFFSET eoffset) { return VARS(ENT(eoffset)); }
inline int ENTINDEX(edict_t *pEdict) { return (*g_engfuncs.pfnIndexOfEdict)(pEdict); }
inline edict_t *INDEXENT(int iEdictNum) { return (*g_engfuncs.pfnPEntityOfEntIndex)(iEdictNum); }

inline BOOL FNullEnt(EOFFSET eoffset) { return eoffset == 0; }
inline BOOL FNullEnt(const edict_t *pent) { return pent == NULL || FNullEnt(OFFSET(pent)); }
inline BOOL FNullEnt(entvars_t *pev) { return pev == NULL || FNullEnt(OFFSET(pev)); }

inline BOOL FStringNull(int iString) { return iString == iStringNull; }

#define cchMapNameMost 32

#define DEAD_NO             0
#define DEAD_DYING          1
#define DEAD_DEAD           2
#define DEAD_RESPAWNABLE    3
#define DEAD_DISCARDBODY    4

#define DAMAGE_NO           0
#define DAMAGE_YES          1
#define DAMAGE_AIM          2

#define MOVETYPE_NONE       0
#define MOVETYPE_WALK       3
#define MOVETYPE_STEP       4
#define MOVETYPE_FLY        5
#define MOVETYPE_TOSS       6
#define MOVETYPE_PUSH       7
#define MOVETYPE_NOCLIP     8
#define MOVETYPE_FLYMISSILE 9
#define MOVETYPE_BOUNCE     10
#define MOVETYPE_BOUNCEMISSILE 11
#define MOVETYPE_FOLLOW     12
#define MOVETYPE_PUSHSTEP   13

#define SOLID_NOT           0
#define SOLID_TRIGGER       1
#define SOLID_BBOX          2
#define SOLID_SLIDEBOX      3
#define SOLID_BSP           4

#define FL_FLY              (1 << 0)
#define FL_SWIM             (1 << 1)
#define FL_CONVEYOR         (1 << 2)
#define FL_CLIENT           (1 << 3)
#define FL_INWATER          (1 << 4)
#define FL_MONSTER          (1 << 5)
#define FL_GODMODE          (1 << 6)
#define FL_NOTARGET         (1 << 7)
#define FL_SKIPLOCALHOST    (1 << 8)
#define FL_ONGROUND         (1 << 9)
#define FL_PARTIALGROUND    (1 << 10)
#define FL_WATERJUMP        (1 << 11)
#define FL_FROZEN           (1 << 12)
#define FL_FAKECLIENT       (1 << 13)
#define FL_DUCKING          (1 << 14)
#define FL_FLOAT            (1 << 15)
#define FL_GRAPHED          (1 << 16)

#define CHAN_AUTO           0
#define CHAN_WEAPON         1
#define CHAN_VOICE          2
#define CHAN_ITEM           3
#define CHAN_BODY           4
#define CHAN_STREAM         5
#define CHAN_STATIC         6

#define ATTN_NONE           0
#define ATTN_NORM           (float)0.8
#define ATTN_IDLE           (float)2
#define ATTN_STATIC         (float)1.25

#define PITCH_NORM          100
#define PITCH_LOW           95
#define PITCH_HIGH          120

#define VOL_NORM            1.0

#define SND_SPAWNING        (1 << 8)
#define SND_STOP            (1 << 5)
#define SND_CHANGE_VOL      (1 << 6)
#define SND_CHANGE_PITCH    (1 << 7)

#define DONT_BLEED          -1
#define BLOOD_COLOR_RED     (BYTE)247
#define BLOOD_COLOR_YELLOW  (BYTE)195
#define BLOOD_COLOR_GREEN   BLOOD_COLOR_YELLOW

#define VEC_HULL_MIN        Vector(-16, -16, -36)
#define VEC_HULL_MAX        Vector(16, 16, 36)
#define VEC_HUMAN_HULL_MIN  Vector(-16, -16, 0)
#define VEC_HUMAN_HULL_MAX  Vector(16, 16, 72)
#define VEC_HUMAN_HULL_DUCK Vector(16, 16, 36)

#define VEC_VIEW            Vector(0, 0, 28)

#define VEC_DUCK_HULL_MIN   Vector(-16, -16, -18)
#define VEC_DUCK_HULL_MAX   Vector(16, 16, 18)
#define VEC_DUCK_VIEW       Vector(0, 0, 12)

#define EF_NODRAW           128

#define VIEW_FIELD_FULL         (float)-1.0
#define VIEW_FIELD_WIDE         (float)-0.7
#define VIEW_FIELD_NARROW       (float)0.7
#define VIEW_FIELD_ULTRA_NARROW (float)0.9

enum
{
    kRenderNormal,
    kRenderTransColor,
    kRenderTransTexture,
    kRenderGlow,
    kRenderTransAlpha,
    kRenderTransAdd,
};

enum
{
    kRenderFxNone = 0,
    kRenderFxPulseSlow,
    kRenderFxPulseFast,
    kRenderFxPulseSlowWide,
    kRenderFxPulseFastWide,
    kRenderFxFadeSlow,
    kRenderFxFadeFast,
    kRenderFxSolidSlow,
    kRenderFxSolidFast,
    kRenderFxStrobeSlow,
    kRenderFxStrobeFast,
    kRenderFxStrobeFaster,
    kRenderFxFlickerSlow,
    kRenderFxFlickerFast,
    kRenderFxNoDissipation,
    kRenderFxDistort,
    kRenderFxHologram,
    kRenderFxDeadPlayer,
    kRenderFxExplode,
    kRenderFxGlowShell,
    kRenderFxClampMinScale,
};

#define ARRAYSIZE(p)        (sizeof(p) / sizeof(p[0]))

typedef enum
{
    TS_AT_TOP,
    TS_AT_BOTTOM,
    TS_GOING_UP,
    TS_GOING_DOWN
} TOGGLE_STATE;

#define CLASS_NONE              0
#define CLASS_MACHINE           1
#define CLASS_PLAYER            2
#define CLASS_HUMAN_PASSIVE     3
#define CLASS_HUMAN_MILITARY    4
#define CLASS_ALIEN_MILITARY    5
#define CLASS_ALIEN_PASSIVE     6
#define CLASS_ALIEN_MONSTER     7
#define CLASS_ALIEN_PREY        8
#define CLASS_ALIEN_PREDATOR    9
#define CLASS_INSECT            10
#define CLASS_PLAYER_ALLY       11
#define CLASS_PLAYER_BIOWEAPON  12
#define CLASS_ALIEN_BIOWEAPON   13
#define CLASS_BARNACLE          99

/* dano */
#define DMG_GENERIC         0
#define DMG_CRUSH           (1 << 0)
#define DMG_BULLET          (1 << 1)
#define DMG_SLASH           (1 << 2)
#define DMG_BURN            (1 << 3)
#define DMG_FREEZE          (1 << 4)
#define DMG_FALL            (1 << 5)
#define DMG_BLAST           (1 << 6)
#define DMG_CLUB            (1 << 7)
#define DMG_SHOCK           (1 << 8)
#define DMG_SONIC           (1 << 9)
#define DMG_ENERGYBEAM      (1 << 10)
#define DMG_NEVERGIB        (1 << 12)
#define DMG_ALWAYSGIB       (1 << 13)
#define DMG_DROWN           (1 << 14)
#define DMG_PARALYZE        (1 << 15)
#define DMG_NERVEGAS        (1 << 16)
#define DMG_POISON          (1 << 17)
#define DMG_RADIATION       (1 << 18)
#define DMG_DROWNRECOVER    (1 << 19)
#define DMG_ACID            (1 << 20)
#define DMG_SLOWBURN        (1 << 21)
#define DMG_SLOWFREEZE      (1 << 22)
#define DMG_MORTAR          (1 << 23)

void UTIL_SetSize(entvars_t *pev, const Vector &vecMin, const Vector &vecMax);
void UTIL_SetOrigin(entvars_t *pev, const Vector &vecOrigin);
void UTIL_MakeVectors(const Vector &vecAngles);
float UTIL_VecToYaw(const Vector &vec);
Vector UTIL_VecToAngles(const Vector &vec);
void UTIL_TraceLine(const Vector &vecStart, const Vector &vecEnd, int igmon, edict_t *pentIgnore, TraceResult *ptr);
void UTIL_TraceHull(const Vector &vecStart, const Vector &vecEnd, int igmon, int hullNumber, edict_t *pentIgnore, TraceResult *ptr);

enum { ignore_monsters = 1, dont_ignore_monsters = 0, missile = 2 };
enum { point_hull = 0, human_hull = 1, large_hull = 2, head_hull = 3 };
class CBaseEntity;
void UTIL_Remove(CBaseEntity *pEntity);
int UTIL_EntitiesInBox(CBaseEntity **pList, int listMax, const Vector &mins, const Vector &maxs, int flagMask);

inline void UTIL_MakeVectorsPrivate(const Vector &vecAngles, float *p_vForward, float *p_vRight, float *p_vUp)
{
    g_engfuncs.pfnAngleVectors(vecAngles, p_vForward, p_vRight, p_vUp);
}

extern "C" int strcmp(const char *s1, const char *s2);
inline BOOL FStrEq(const char *sz1, const char *sz2) { return (strcmp(sz1, sz2) == 0); }

inline BOOL FClassnameIs(entvars_t *pev, const char *szClassname)
{
    return FStrEq(STRING(pev->classname), szClassname);
}

void EMIT_SOUND_DYN(edict_t *entity, int channel, const char *sample, float volume, float attenuation, int flags, int pitch);

inline void EMIT_SOUND(edict_t *entity, int channel, const char *sample, float volume, float attenuation)
{
    EMIT_SOUND_DYN(entity, channel, sample, volume, attenuation, 0, PITCH_NORM);
}

inline void STOP_SOUND(edict_t *entity, int channel, const char *sample)
{
    EMIT_SOUND_DYN(entity, channel, sample, 0, 0, SND_STOP, PITCH_NORM);
}

#define PRECACHE_SOUND_ARRAY(a) \
    { for (int i = 0; i < ARRAYSIZE(a); i++) PRECACHE_SOUND((char *)a[i]); }

#define EMIT_SOUND_ARRAY_DYN(chan, array) \
    EMIT_SOUND_DYN(ENT(pev), chan, array[RANDOM_LONG(0, ARRAYSIZE(array) - 1)], 1.0, ATTN_NORM, 0, RANDOM_LONG(95, 105));

/* usados por weapons.cpp */
#define MSG_BROADCAST       0
#define MSG_ONE             1
#define MSG_ALL             2
#define MSG_INIT            3
#define MSG_PVS             4
#define MSG_PAS             5

#define SVC_TEMPENTITY      23
#define TE_DLIGHT           27
#define SVC_WEAPONANIM      35

#define TE_MODEL            106

#define EF_MUZZLEFLASH      2

#define IN_ATTACK           (1 << 0)
#define IN_ATTACK2          (1 << 11)
#define IN_RELOAD           (1 << 13)

#define TE_BSPDECAL         13
#define TE_SPARKS           9
#define TE_TEXTMESSAGE      29
#define TE_BLOODSTREAM      101
#define TE_BLOOD            103
#define TE_DECAL            104
#define TE_GUNSHOTDECAL     109
#define TE_ARMOR_RICOCHET   111
#define TE_PLAYERDECAL      112
#define TE_BUBBLES          113
#define TE_BUBBLETRAIL      114
#define TE_BLOODSPRITE      115
#define TE_WORLDDECAL       116
#define TE_WORLDDECALHIGH   117
#define TE_DECALHIGH        118

#define CONTENTS_EMPTY      -1
#define CONTENTS_SOLID      -2
#define CONTENTS_WATER      -3
#define CONTENTS_SLIME      -4
#define CONTENTS_LAVA       -5
#define CONTENTS_SKY        -6

#define HUD_PRINTNOTIFY     1
#define HUD_PRINTCONSOLE    2
#define HUD_PRINTTALK       3
#define HUD_PRINTCENTER     4

#define LANGUAGE_ENGLISH    0
#define LANGUAGE_GERMAN     1
#define LANGUAGE_FRENCH     2
#define LANGUAGE_BRITISH    3

extern int g_Language;

extern int gmsgTextMsg;
extern int gmsgHudText;
extern int gmsgSayText;

#define FL_KILLME           (1 << 30)

#define ASSERT(f)

typedef struct hudtextparms_s
{
    float x;
    float y;
    int effect;
    byte r1, g1, b1, a1;
    byte r2, g2, b2, a2;
    float fadeinTime;
    float fadeoutTime;
    float holdTime;
    float fxTime;
    int channel;
} hudtextparms_t;

class CBaseEntity;
void UTIL_HudMessageAll(const hudtextparms_t &textparms, const char *pMessage);
void UTIL_HudMessage(CBaseEntity *pEntity, const hudtextparms_t &textparms, const char *pMessage);

char *UTIL_dtos1(int d);
char *UTIL_dtos2(int d);
char *UTIL_dtos3(int d);
char *UTIL_dtos4(int d);

void UTIL_ClientPrintAll(int msg_dest, const char *msg_name, const char *param1 = NULL, const char *param2 = NULL, const char *param3 = NULL, const char *param4 = NULL);
void ClientPrint(entvars_t *client, int msg_dest, const char *msg_name, const char *param1 = NULL, const char *param2 = NULL, const char *param3 = NULL, const char *param4 = NULL);

int SENTENCEG_Lookup(const char *sample, char *sentencenum);

extern "C" int sprintf(char *buf, const char *fmt, ...);
extern "C" int vsprintf(char *buf, const char *fmt, char *ap);
extern "C" char *strncpy(char *dst, const char *src, unsigned int n);

void UTIL_BloodDrips(const Vector &origin, const Vector &direction, int color, int amount);
void UTIL_DecalTrace(TraceResult *pTrace, int decalNumber);
void UTIL_GunshotDecalTrace(TraceResult *pTrace, int decalNumber);
BOOL UTIL_IsValidEntity(edict_t *pent);
void UTIL_PrecacheOther(const char *szClassname);

extern "C" int strncmp(const char *s1, const char *s2, unsigned int n);
extern "C" int strcasecmp(const char *s1, const char *s2);
extern "C" char *strcpy(char *dst, const char *src);
extern "C" int atoi(const char *s);
extern "C" double atof(const char *s);
extern "C" void *memset(void *dst, int c, unsigned int n);
#define stricmp strcasecmp

#ifndef min
#define min(a, b)  (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a, b)  (((a) > (b)) ? (a) : (b))
#endif

#define SetBits(flBitVector, bits)      ((flBitVector) = (int)(flBitVector) | (bits))
#define ClearBits(flBitVector, bits)    ((flBitVector) = (int)(flBitVector) & ~(bits))
#define FBitSet(flBitVector, bit)       ((int)(flBitVector) & (bit))

#endif
