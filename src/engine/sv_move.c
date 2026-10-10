#include "common.h"
#include "engine/mathlib.h"
#include "engine/model.h"
#include "engine/server.h"
#include "engine/cvar.h"

#define FL_MONSTERCLIP 0x800000

extern cvar_t sv_stepsize;

trace_t SV_Move(const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int type, edict_t *passedict, qboolean monsterClipBrush);
int SV_PointContents(const vec3_t p);

extern int c_yes;
extern int c_no;


extern cvar_t sv_stepsize;

trace_t SV_Move(const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int type, edict_t *passedict, qboolean monsterClipBrush);
int SV_PointContents(const vec3_t p);

extern int c_yes;
extern int c_no;


extern cvar_t sv_stepsize;

trace_t SV_Move(const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int type, edict_t *passedict, qboolean monsterClipBrush);
int SV_PointContents(const vec3_t p);

extern int c_yes;
extern int c_no;

#ifdef NON_MATCHING
qboolean SV_CheckBottom(edict_t *ent)
{
    vec3_t mins, maxs, start, stop;
    trace_t trace;
    int x, y;
    float mid, bottom;
    qboolean monsterClip = (ent->v.flags & FL_MONSTERCLIP) > 0;

    VectorAdd(ent->v.origin, ent->v.mins, mins);
    VectorAdd(ent->v.origin, ent->v.maxs, maxs);

    start[2] = mins[2] - 1;
    for (x = 0; x <= 1; x++)
    {
        for (y = 0; y <= 1; y++)
        {
            start[0] = x ? maxs[0] : mins[0];
            start[1] = y ? maxs[1] : mins[1];
            if (SV_PointContents(start) != CONTENTS_SOLID)
                goto realcheck;
        }
    }

    c_yes++;
    return true;

realcheck:
    c_no++;

    start[2] = mins[2] + sv_stepsize.value;

    start[0] = stop[0] = (mins[0] + maxs[0]) * 0.5f;
    start[1] = stop[1] = (mins[1] + maxs[1]) * 0.5f;
    stop[2] = start[2] - 2 * sv_stepsize.value;
    trace = SV_Move(start, vec3_origin, vec3_origin, stop, true, ent, monsterClip);

    if (trace.fraction == 1.0f)
        return false;
    mid = bottom = trace.endpos[2];

    for (x = 0; x <= 1; x++)
    {
        for (y = 0; y <= 1; y++)
        {
            start[0] = stop[0] = x ? maxs[0] : mins[0];
            start[1] = stop[1] = y ? maxs[1] : mins[1];

            trace = SV_Move(start, vec3_origin, vec3_origin, stop, true, ent, monsterClip);

            if (trace.fraction != 1.0f && trace.endpos[2] > bottom)
                bottom = trace.endpos[2];
            if (trace.fraction == 1.0f || mid - trace.endpos[2] > sv_stepsize.value)
                return false;
        }
    }

    c_yes++;
    return true;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/sv_move", SV_CheckBottom);
#endif
INCLUDE_ASM("asm/nonmatchings/engine/sv_move", SV_movetest);
INCLUDE_ASM("asm/nonmatchings/engine/sv_move", SV_movestep);
INCLUDE_ASM("asm/nonmatchings/engine/sv_move", func_00283F98);
INCLUDE_ASM("asm/nonmatchings/engine/sv_move", func_00284060);
INCLUDE_ASM("asm/nonmatchings/engine/sv_move", func_002840C8);
INCLUDE_ASM("asm/nonmatchings/engine/sv_move", func_002840D8);
INCLUDE_ASM("asm/nonmatchings/engine/sv_move", func_00284538);
INCLUDE_ASM("asm/nonmatchings/engine/sv_move", func_002845A0);
INCLUDE_ASM("asm/nonmatchings/engine/sv_move", func_002845F8);
