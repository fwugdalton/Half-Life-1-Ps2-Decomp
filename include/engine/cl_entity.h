#ifndef ENGINE_CL_ENTITY_H
#define ENGINE_CL_ENTITY_H

#include "engine/mathlib.h"
#include "engine/entity_state.h"

struct model_s;


/* cl_entity_t do PS2, 0x1A0 bytes. Layout parcial (r_trans.c, pmove.c e pr_cmds.c ainda têm versões locais). */
typedef struct cl_entity_s
{
    byte pad0[0x2C];
    int index;                  /* 0x2C */
    int unk_30;                 /* 0x30 */
    byte pad34[0x50 - 0x34];
    struct model_s *model;      /* 0x50 */
    byte pad54[0x60 - 0x54];
    float frame;                /* 0x60 */
    byte pad64[0x6C - 0x64];
    float scale;                /* 0x6C */
    byte pad70[0x84 - 0x70];
    vec3_t origin;              /* 0x84 */
    vec3_t angles;              /* 0x90 */
    byte pad9C[0xF8 - 0x9C];
    entity_state_t baseline;    /* 0xF8 */
    byte pad18C[0x1A0 - 0x18C];
} cl_entity_t; /* 0x1A0 */

#endif
