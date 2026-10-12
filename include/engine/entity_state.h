#ifndef ENGINE_ENTITY_STATE_H
#define ENGINE_ENTITY_STATE_H

#include "engine/mathlib.h"

typedef struct color24_s
{
    byte r, g, b;
} color24;

/* entity_state_t do PS2: o do PC até weaponmodel, sem messagenum */
typedef struct entity_state_s
{
    int entityType;             /* 0x00 */
    int number;                 /* 0x04 */
    int flags;                  /* 0x08: bits do último delta (QW) */
    vec3_t origin;              /* 0x0C */
    vec3_t angles;              /* 0x18 */
    int modelindex;             /* 0x24 */
    int sequence;               /* 0x28 */
    float frame;                /* 0x2C */
    int colormap;               /* 0x30 */
    short skin;                 /* 0x34 */
    short solid;                /* 0x36 */
    int effects;                /* 0x38 */
    float scale;                /* 0x3C */
    int rendermode;             /* 0x40 */
    int renderamt;              /* 0x44 */
    color24 rendercolor;        /* 0x48 */
    int renderfx;               /* 0x4C */
    int movetype;               /* 0x50 */
    float animtime;             /* 0x54 */
    float framerate;            /* 0x58 */
    int body;                   /* 0x5C */
    byte controller[4];         /* 0x60 */
    byte blending[4];           /* 0x64 */
    vec3_t velocity;            /* 0x68 */
    vec3_t mins;                /* 0x74 */
    vec3_t maxs;                /* 0x80 */
    int aiment;                 /* 0x8C */
    int weaponmodel;            /* 0x90 */
} entity_state_t; /* 0x94 */

#endif
