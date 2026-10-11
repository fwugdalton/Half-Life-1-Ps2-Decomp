#ifndef ENGINE_RENDER_H
#define ENGINE_RENDER_H

#include "engine/mathlib.h"
#include "engine/entity_state.h"

typedef struct
{
    int x, y, width, height;
} vrect_t;

typedef struct
{
    vrect_t vrect;              /* 0x00 */
    byte pad10[0x70 - 0x10];
    vec3_t vieworg;             /* 0x70 */
    vec3_t viewangles;          /* 0x7C */
    color24 ambientlight;       /* 0x88 */
    byte pad8B;
} refdef_t; /* 0x8C */

extern refdef_t r_refdef;

#endif
