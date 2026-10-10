#ifndef ENGINE_CLIENT_H
#define ENGINE_CLIENT_H

#include "engine/mathlib.h"
#include "engine/cl_entity.h"

/* client_state_t do PS2: um por jogador local (tela dividida), cl[clientId], 0x212C bytes.
   Layout parcial: só os campos já identificados (outros arquivos ainda têm versões locais). */
typedef struct screenfade_s
{
    float fadeSpeed;            /* 0x00 */
    float fadeEnd;              /* 0x04 */
    float fadeReset;            /* 0x08 */
    byte fader, fadeg, fadeb, fadealpha; /* 0x0C */
    int fadeFlags;              /* 0x10 */
} screenfade_t; /* 0x14 */

#define FFADE_IN 0x0000
#define FFADE_OUT 0x0001
#define FFADE_MODULATE 0x0002
#define FFADE_STAYOUT 0x0004

typedef struct client_state_s
{
    byte pad0[0x224];
    float forwardmove;          /* 0x224: cmd.forwardmove */
    byte pad228[0x238 - 0x228];
    int stats[1];               /* 0x238: stats[STAT_HEALTH] */
    byte pad23C[0x2DC - 0x23C];
    vec3_t viewangles;          /* 0x2DC */
    byte pad2E8[0x328 - 0x2E8];
    vec3_t simorg;              /* 0x328 */
    vec3_t simvel;              /* 0x334 */
    vec3_t simangles;           /* 0x340 */
    float idealpitch;           /* 0x34C */
    float pitchvel;             /* 0x350 */
    qboolean nodrift;           /* 0x354 */
    float driftmove;            /* 0x358 */
    float laststop;             /* 0x35C */
    float unk_360;              /* 0x360 */
    byte pad364[0x368 - 0x364];
    screenfade_t sf;            /* 0x368 */
    byte pad37C[0x380 - 0x37C];
    qboolean onground;          /* 0x380 */
    byte pad384[0x3A0 - 0x384];
    float time;                 /* 0x3A0 */
    float oldtime;              /* 0x3A4 */
    byte pad3A8[0x3B4 - 0x3A8];
    qboolean paused;            /* 0x3B4 */
    byte pad3B8[0x13E8 - 0x3B8];
    int viewentity;             /* 0x13E8 */
    byte pad13EC[0x13FC - 0x13EC];
    cl_entity_t viewent;        /* 0x13FC */
    byte pad159C[0x212C - 0x159C];
} client_state_t; /* 0x212C */

extern client_state_t cl[2];
extern int cl_numlocal;

#endif
