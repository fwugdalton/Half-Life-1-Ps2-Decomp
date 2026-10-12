#include "common.h"
#include "engine/cvar.h"
#include "engine/mathlib.h"
#include "engine/client.h"
#include "engine/model.h"
#include "engine/server.h"
#include "engine/render.h"

void *memset(void *, int, unsigned int);
float atanf(float x);
qboolean SV_RecursiveHullCheck(hull_t *hull, int num, float p1f, float p2f, vec3_t p1, vec3_t p2, trace_t *trace);

extern cvar_t chase_back;
extern cvar_t chase_up;
extern cvar_t chase_right;
extern cvar_t chase_active;
extern vec3_t D_0046E510;   /* chase_dest */

void func_00247660(void) /* Chase_Init */
{
    Cvar_RegisterVariable(&chase_back);
    Cvar_RegisterVariable(&chase_up);
    Cvar_RegisterVariable(&chase_right);
    Cvar_RegisterVariable(&chase_active);
}

void func_002476A8(void) /* Chase_Reset */
{
}

void func_002476B0(vec3_t start, vec3_t end, vec3_t impact) /* TraceLine */
{
    trace_t trace;

    memset(&trace, 0, sizeof(trace));
    SV_RecursiveHullCheck(cl[0].worldmodel->hulls, 0, 0, 1, start, end, &trace);
    VectorCopy(trace.endpos, impact);
}

void Chase_Update(void)
{
    int i;
    float dist;
    vec3_t forward, up, right;
    vec3_t dest, stop;

    AngleVectors(cl[0].viewangles, forward, right, up);

    for (i = 0; i < 3; i++)
        D_0046E510[i] = r_refdef.vieworg[i] - forward[i] * chase_back.value - right[i] * chase_right.value;
    D_0046E510[2] = r_refdef.vieworg[2] + chase_up.value;

    VectorMA(r_refdef.vieworg, 4096, forward, dest);
    func_002476B0(r_refdef.vieworg, dest, stop);

    VectorSubtract(stop, r_refdef.vieworg, stop);
    dist = DotProduct(stop, forward);
    if (dist < 1)
        dist = 1;
    r_refdef.viewangles[0] = -atanf(stop[2] / dist) * (180 / M_PI);

    VectorCopy(D_0046E510, r_refdef.vieworg);
}

INCLUDE_RODATA("asm/nonmatchings/engine/chase", D_003891C0);

INCLUDE_RODATA("asm/nonmatchings/engine/chase", D_003891C8);

INCLUDE_RODATA("asm/nonmatchings/engine/chase", D_003891D8);

INCLUDE_RODATA("asm/nonmatchings/engine/chase", D_003891E0);

INCLUDE_RODATA("asm/nonmatchings/engine/chase", D_003891F0);

INCLUDE_RODATA("asm/nonmatchings/engine/chase", D_003891F8);

INCLUDE_RODATA("asm/nonmatchings/engine/chase", D_00389208);
