#ifndef ENGINE_PROGDEFS_H
#define ENGINE_PROGDEFS_H

#include "engine/server.h"

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
    vec3_t v_forward;           /* 0x28 */
    vec3_t v_up;                /* 0x34 */
    vec3_t v_right;             /* 0x40 */
    float trace_allsolid;       /* 0x4C */
    float trace_startsolid;     /* 0x50 */
    float trace_fraction;       /* 0x54 */
    vec3_t trace_endpos;        /* 0x58 */
    vec3_t trace_plane_normal;  /* 0x64 */
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
    char *pStringBase;          /* 0x98 */
    void *pSaveData;            /* 0x9C */
} globalvars_t;

extern globalvars_t gGlobalVariables;

typedef struct KeyValueData_s
{
    char *szClassName;          /* 0x00 */
    char *szKeyName;            /* 0x04 */
    char *szValue;              /* 0x08 */
    int fHandled;               /* 0x0C */
} KeyValueData;

typedef void (*ENTITYINIT)(entvars_t *);
ENTITYINIT GetEntityInit(char *pClassName);

#endif
