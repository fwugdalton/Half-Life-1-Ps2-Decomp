#ifndef ENGINE_CL_ENTITY_H
#define ENGINE_CL_ENTITY_H

#include "engine/mathlib.h"
#include "engine/entity_state.h"

struct model_s;


/* cl_entity_t do PS2, 0x1A0 bytes. Layout parcial (r_trans.c, pmove.c e pr_cmds.c ainda têm versões locais). */
typedef struct cl_entity_s
{
    byte pad0[0x10];
    int rendermode;             /* 0x10 */
    byte pad14[0x1C - 0x14];
    int movetype;               /* 0x1C */
    int unk_20;                 /* 0x20: body do viewmodel (svc_weaponanim) */
    int sequence;               /* 0x24 */
    byte pad28[0x2C - 0x28];
    int index;                  /* 0x2C */
    int colormap;               /* 0x30 */
    byte pad34[0x38 - 0x34];
    int visframe;               /* 0x38 */
    byte pad3C[0x50 - 0x3C];
    struct model_s *model;      /* 0x50 */
    struct efrag_s *efrag;      /* 0x54 */
    void *topnode;              /* 0x58 */
    float framerate;            /* 0x5C */
    float frame;                /* 0x60 */
    byte pad64[0x68 - 0x64];
    float animtime;             /* 0x68 */
    float scale;                /* 0x6C */
    byte pad70[0x74 - 0x70];
    float prevanimtime;         /* 0x74 */
    byte pad78[0x84 - 0x78];
    vec3_t origin;              /* 0x84 */
    vec3_t angles;              /* 0x90 */
    vec3_t prevorigin;          /* 0x9C */
    vec3_t prevangles;          /* 0xA8 */
    byte padB4[0xE4 - 0xB4];
    color24 rendercolor;        /* 0xE4 */
    byte padE7[0xF8 - 0xE7];
    entity_state_t baseline;    /* 0xF8 */
    byte pad18C[0x1A0 - 0x18C];
} cl_entity_t; /* 0x1A0 */

#endif
