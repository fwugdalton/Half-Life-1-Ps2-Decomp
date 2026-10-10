#ifndef ENGINE_CL_ENTITY_H
#define ENGINE_CL_ENTITY_H

#include "engine/mathlib.h"

struct model_s;

/* cl_entity_t do PS2, 0x1A0 bytes. Layout parcial (r_trans.c, pmove.c e pr_cmds.c ainda têm versões locais). */
typedef struct cl_entity_s
{
    byte pad0[0x50];
    struct model_s *model;      /* 0x50 */
    byte pad54[0x84 - 0x54];
    vec3_t origin;              /* 0x84 */
    vec3_t angles;              /* 0x90 */
    byte pad9C[0x1A0 - 0x9C];
} cl_entity_t; /* 0x1A0 */

#endif
