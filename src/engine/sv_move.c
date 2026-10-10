#include "common.h"
#include "engine/mathlib.h"
#include "engine/model.h"
#include "engine/server.h"
#include "engine/cvar.h"

#define FL_FLY (1 << 0)
#define FL_SWIM (1 << 1)
#define FL_MONSTERCLIP 0x800000
#define FL_ONGROUND (1 << 9)
#define FL_PARTIALGROUND (1 << 10)

extern cvar_t sv_stepsize;

trace_t SV_Move(const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int type, edict_t *passedict, qboolean monsterClipBrush);
int SV_PointContents(const vec3_t p);

extern int c_yes;
extern int c_no;

qboolean SV_CheckBottom(edict_t *ent)
{
    vec3_t mins, maxs, start, stop;
    trace_t trace;
    int x, y;
    float mid, bottom;
    char monsterClip = (ent->v.flags & FL_MONSTERCLIP) > 0;

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
trace_t func_00291900(const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int type, edict_t *passedict);
void SV_LinkEdict(edict_t *ent, qboolean touch_triggers);

qboolean SV_movetest(edict_t *ent, vec_t *move, qboolean relink)
{
    vec3_t oldorg, neworg, end;
    trace_t trace;

    VectorCopy(ent->v.origin, oldorg);
    VectorAdd(ent->v.origin, move, neworg);

    neworg[2] += sv_stepsize.value;
    VectorCopy(neworg, end);
    end[2] -= sv_stepsize.value * 2;

    trace = func_00291900(neworg, ent->v.mins, ent->v.maxs, end, false, ent);
    if (trace.allsolid)
        return false;

    if (trace.startsolid)
    {
        neworg[2] -= sv_stepsize.value;
        trace = func_00291900(neworg, ent->v.mins, ent->v.maxs, end, false, ent);
        if (trace.allsolid || trace.startsolid)
            return false;
    }

    if (trace.fraction == 1.0f)
    {
        if (ent->v.flags & FL_PARTIALGROUND)
        {
            VectorAdd(ent->v.origin, move, ent->v.origin);
            if (relink)
                SV_LinkEdict(ent, true);
            ent->v.flags &= ~FL_ONGROUND;
            return true;
        }
        return false;
    }

    VectorCopy(trace.endpos, ent->v.origin);

    if (!SV_CheckBottom(ent))
    {
        if (ent->v.flags & FL_PARTIALGROUND)
        {
            if (relink)
                SV_LinkEdict(ent, true);
            return true;
        }
        VectorCopy(oldorg, ent->v.origin);
        return false;
    }

    if (ent->v.flags & FL_PARTIALGROUND)
        ent->v.flags &= ~FL_PARTIALGROUND;

    ent->v.groundentity = trace.ent;

    if (relink)
        SV_LinkEdict(ent, true);
    return true;
}
#ifdef NON_MATCHING
qboolean SV_movestep(edict_t *ent, vec_t *move, qboolean relink)
{
    float dz;
    vec3_t oldorg, neworg, end;
    trace_t trace;
    int i;
    edict_t *enemy;
    char monsterClip = (ent->v.flags & FL_MONSTERCLIP) > 0;

    VectorCopy(ent->v.origin, oldorg);
    VectorAdd(ent->v.origin, move, neworg);

    if (ent->v.flags & (FL_SWIM | FL_FLY))
    {
        for (i = 0; i < 2; i++)
        {
            VectorAdd(ent->v.origin, move, neworg);
            enemy = ent->v.enemy;
            if (i == 0 && enemy)
            {
                dz = ent->v.origin[2] - enemy->v.origin[2];
                if (dz > 40)
                    neworg[2] -= 8;
                if (dz < 30)
                    neworg[2] += 8;
            }
            trace = SV_Move(ent->v.origin, ent->v.mins, ent->v.maxs, neworg, false, ent, monsterClip);

            if (trace.fraction == 1.0f)
            {
                if ((ent->v.flags & FL_SWIM) && SV_PointContents(trace.endpos) == CONTENTS_EMPTY)
                    return false;

                VectorCopy(trace.endpos, ent->v.origin);
                if (relink)
                    SV_LinkEdict(ent, true);
                return true;
            }

            if (!enemy)
                break;
        }

        return false;
    }

    neworg[2] += sv_stepsize.value;
    VectorCopy(neworg, end);
    end[2] -= sv_stepsize.value * 2;

    trace = SV_Move(neworg, ent->v.mins, ent->v.maxs, end, false, ent, monsterClip);
    if (trace.allsolid)
        return false;

    if (trace.startsolid)
    {
        neworg[2] -= sv_stepsize.value;
        trace = SV_Move(neworg, ent->v.mins, ent->v.maxs, end, false, ent, monsterClip);
        if (trace.allsolid || trace.startsolid)
            return false;
    }

    if (trace.fraction == 1.0f)
    {
        if (ent->v.flags & FL_PARTIALGROUND)
        {
            VectorAdd(ent->v.origin, move, ent->v.origin);
            if (relink)
                SV_LinkEdict(ent, true);
            ent->v.flags &= ~FL_ONGROUND;
            return true;
        }
        return false;
    }

    VectorCopy(trace.endpos, ent->v.origin);

    if (!SV_CheckBottom(ent))
    {
        if (ent->v.flags & FL_PARTIALGROUND)
        {
            if (relink)
                SV_LinkEdict(ent, true);
            return true;
        }
        VectorCopy(oldorg, ent->v.origin);
        return false;
    }

    if (ent->v.flags & FL_PARTIALGROUND)
        ent->v.flags &= ~FL_PARTIALGROUND;

    ent->v.groundentity = trace.ent;

    if (relink)
        SV_LinkEdict(ent, true);
    return true;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/sv_move", SV_movestep);
#endif
float cosf(float x);
float sinf(float x);

qboolean SV_StepDirection(edict_t *ent, float yaw, float dist)
{
    vec3_t move, oldorigin;

    yaw = yaw * M_PI * 2 / 360;
    move[0] = cosf(yaw) * dist;
    move[1] = sinf(yaw) * dist;
    move[2] = 0;

    VectorCopy(ent->v.origin, oldorigin);
    if (SV_movestep(ent, move, false))
    {
        SV_LinkEdict(ent, true);
        return true;
    }
    SV_LinkEdict(ent, true);
    return false;
}
qboolean SV_FlyDirection(edict_t *ent, vec_t *direction)
{
    vec3_t oldorigin;

    VectorCopy(ent->v.origin, oldorigin);
    if (SV_movestep(ent, direction, false))
    {
        SV_LinkEdict(ent, true);
        return true;
    }
    SV_LinkEdict(ent, true);
    return false;
}
void SV_FixCheckBottom(edict_t *ent)
{
    ent->v.flags |= FL_PARTIALGROUND;
}
#define DI_NODIR -1

int RandomLong(int lLow, int lHigh);
int abs(int);

void SV_NewChaseDir(edict_t *actor, edict_t *enemy, float dist)
{
    float deltax, deltay;
    float d[3];
    float tdir, olddir, turnaround;

    olddir = anglemod((int)(actor->v.ideal_yaw / 45.0f) * 45);
    turnaround = anglemod(olddir - 180);

    deltax = enemy->v.origin[0] - actor->v.origin[0];
    deltay = enemy->v.origin[1] - actor->v.origin[1];
    if (deltax > 10)
        d[1] = 0;
    else if (deltax < -10)
        d[1] = 180;
    else
        d[1] = DI_NODIR;
    if (deltay < -10)
        d[2] = 270;
    else if (deltay > 10)
        d[2] = 90;
    else
        d[2] = DI_NODIR;

    if (d[1] != DI_NODIR && d[2] != DI_NODIR)
    {
        if (d[1] == 0)
            tdir = d[2] == 90 ? 45 : 315;
        else
            tdir = d[2] == 90 ? 135 : 215;

        if (tdir != turnaround && SV_StepDirection(actor, tdir, dist))
            return;
    }

    if (RandomLong(0, 1) || abs((int)deltay) > abs((int)deltax))
    {
        tdir = d[1];
        d[1] = d[2];
        d[2] = tdir;
    }

    if (d[1] != DI_NODIR && d[1] != turnaround && SV_StepDirection(actor, d[1], dist))
        return;

    if (d[2] != DI_NODIR && d[2] != turnaround && SV_StepDirection(actor, d[2], dist))
        return;

    if (olddir != DI_NODIR && SV_StepDirection(actor, olddir, dist))
        return;

    if (RandomLong(0, 1))
    {
        for (tdir = 0; tdir <= 315; tdir += 45)
        {
            if (tdir != turnaround && SV_StepDirection(actor, tdir, dist))
                return;
        }
    }
    else
    {
        for (tdir = 315; tdir >= 0; tdir -= 45)
        {
            if (tdir != turnaround && SV_StepDirection(actor, tdir, dist))
                return;
        }
    }

    if (turnaround != DI_NODIR && SV_StepDirection(actor, turnaround, dist))
        return;

    actor->v.ideal_yaw = olddir;

    if (!SV_CheckBottom(actor))
        SV_FixCheckBottom(actor);
}
qboolean SV_CloseEnough(edict_t *ent, edict_t *goal, float dist)
{
    int i;

    for (i = 0; i < 3; i++)
    {
        if (goal->v.absmin[i] > ent->v.absmax[i] + dist)
            return false;
        if (goal->v.absmax[i] < ent->v.absmin[i] - dist)
            return false;
    }
    return true;
}
qboolean SV_ReachedGoal(edict_t *ent, vec_t *vecGoal, float flDist)
{
    int i;

    for (i = 0; i < 3; i++)
    {
        if (vecGoal[i] > ent->v.absmax[i] + flDist)
            return false;
        if (vecGoal[i] < ent->v.absmin[i] - flDist)
            return false;
    }
    return true;
}
void SV_NewChaseDir2(edict_t *actor, vec_t *vecGoal, float dist)
{
    float deltax, deltay;
    float d[3];
    float tdir, olddir, turnaround;

    olddir = anglemod((int)(actor->v.ideal_yaw / 45.0f) * 45);
    turnaround = anglemod(olddir - 180);

    deltax = vecGoal[0] - actor->v.origin[0];
    deltay = vecGoal[1] - actor->v.origin[1];
    if (deltax > 10)
        d[1] = 0;
    else if (deltax < -10)
        d[1] = 180;
    else
        d[1] = DI_NODIR;
    if (deltay < -10)
        d[2] = 270;
    else if (deltay > 10)
        d[2] = 90;
    else
        d[2] = DI_NODIR;

    if (d[1] != DI_NODIR && d[2] != DI_NODIR)
    {
        if (d[1] == 0)
            tdir = d[2] == 90 ? 45 : 315;
        else
            tdir = d[2] == 90 ? 135 : 215;

        if (tdir != turnaround && SV_StepDirection(actor, tdir, dist))
            return;
    }

    if (RandomLong(0, 1) || abs((int)deltay) > abs((int)deltax))
    {
        tdir = d[1];
        d[1] = d[2];
        d[2] = tdir;
    }

    if (d[1] != DI_NODIR && d[1] != turnaround && SV_StepDirection(actor, d[1], dist))
        return;

    if (d[2] != DI_NODIR && d[2] != turnaround && SV_StepDirection(actor, d[2], dist))
        return;

    if (olddir != DI_NODIR && SV_StepDirection(actor, olddir, dist))
        return;

    if (RandomLong(0, 1))
    {
        for (tdir = 0; tdir <= 315; tdir += 45)
        {
            if (tdir != turnaround && SV_StepDirection(actor, tdir, dist))
                return;
        }
    }
    else
    {
        for (tdir = 315; tdir >= 0; tdir -= 45)
        {
            if (tdir != turnaround && SV_StepDirection(actor, tdir, dist))
                return;
        }
    }

    if (turnaround != DI_NODIR && SV_StepDirection(actor, turnaround, dist))
        return;

    actor->v.ideal_yaw = olddir;

    if (!SV_CheckBottom(actor))
        SV_FixCheckBottom(actor);
}
