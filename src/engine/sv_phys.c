#include "common.h"
#include "engine/mathlib.h"
#include "engine/model.h"
#include "engine/server.h"
#include "engine/cvar.h"
#include "engine/progdefs.h"
#include "engine/eiface.h"

#define FL_KILLME (1 << 30)
#define SOLID_NOT 0
#define SOLID_TRIGGER 1
#define SOLID_SLIDEBOX 3
#define SOLID_BSP 4

#define FL_CLIENT (1 << 3)
#define FL_ONGROUND (1 << 9)
#define FL_MONSTERCLIP (1 << 23)

#define MOVE_NORMAL 0
#define MOVE_NOMONSTERS 1
#define MOVE_MISSILE 2

#define MOVETYPE_NONE 0
#define MOVETYPE_WALK 3
#define MOVETYPE_STEP 4
#define MOVETYPE_FLY 5
#define MOVETYPE_TOSS 6
#define MOVETYPE_PUSH 7
#define MOVETYPE_NOCLIP 8
#define MOVETYPE_FLYMISSILE 9
#define MOVETYPE_BOUNCE 10
#define MOVETYPE_BOUNCEMISSILE 11
#define MOVETYPE_FOLLOW 12
#define MOVETYPE_PUSHSTEP 13

edict_t *SV_TestEntityPosition(edict_t *ent);

extern char *pr_strings;
extern void (*gEntityInterface_pfnTouch)(edict_t *pentTouched, edict_t *pentOther);
void SV_SetGlobalTrace(trace_t *ptrace);
void ED_Free(edict_t *ed);
trace_t SV_Move(const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int type, edict_t *passedict, qboolean monsterClipBrush);
extern cvar_t sv_bounce;
extern cvar_t sv_gravity;
extern float host_frametime;
void SV_LinkEdict(edict_t *ent, qboolean touch_triggers);
extern vec3_t *g_moved_from;
extern edict_t **g_moved_edict;
extern cvar_t sv_maxvelocity;

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F790);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F798);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F7A8);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F7B0);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F7C0);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F7C8);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F7D8);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F7E0);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F7F0);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F7F8);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F808);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F810);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F820);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F828);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F838);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F848);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F860);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F878);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F890);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F8A8);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F8B0);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F8C8);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F8D0);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F8E8);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F900);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F918);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F920);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F928);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F938);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038F940);

void SV_CheckAllEnts(void)
{
    int e;
    edict_t *check;

    for (e = 1; e < sv.num_edicts; e++)
    {
        check = &sv.edicts[e];
        if (check->free)
            continue;
        if (check->v.movetype == MOVETYPE_PUSH
            || check->v.movetype == MOVETYPE_NONE
            || check->v.movetype == MOVETYPE_FOLLOW
            || check->v.movetype == MOVETYPE_NOCLIP)
            continue;

        if (SV_TestEntityPosition(check))
            Con_Printf("entity in invalid position\n");
    }
}

#ifdef NON_MATCHING
void SV_CheckVelocity(edict_t *ent)
{
    int i;

    for (i = 0; i < 3; i++)
    {
        if (IS_NAN(ent->v.velocity[i]))
        {
            Con_Printf("Got a NaN velocity on %s\n", &pr_strings[ent->v.classname]);
            ent->v.velocity[i] = 0;
        }
        if (IS_NAN(ent->v.origin[i]))
        {
            Con_Printf("Got a NaN origin on %s\n", &pr_strings[ent->v.classname]);
            ent->v.origin[i] = 0;
        }
        if (ent->v.velocity[i] > sv_maxvelocity.value)
        {
            Con_DPrintf("Got a velocity too high on %s\n", &pr_strings[ent->v.classname]);
            ent->v.velocity[i] = sv_maxvelocity.value;
        }
        else if (ent->v.velocity[i] < -sv_maxvelocity.value)
        {
            Con_DPrintf("Got a velocity too low on %s\n", &pr_strings[ent->v.classname]);
            ent->v.velocity[i] = -sv_maxvelocity.value;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/sv_phys", SV_CheckVelocity);
#endif

qboolean SV_RunThink(edict_t *ent)
{
    float thinktime;

    thinktime = ent->v.nextthink;
    if (thinktime < sv.time)
        thinktime = sv.time;

    ent->v.nextthink = 0;
    gGlobalVariables.time = thinktime;
    gEntityInterface.pfnThink(ent);

    if (ent->v.flags & FL_KILLME)
        ED_Free(ent);

    return !ent->free;
}

void SV_Impact(edict_t *e1, edict_t *e2, trace_t *ptrace)
{
    gGlobalVariables.time = sv.time;

    if ((e1->v.flags | e2->v.flags) & FL_KILLME)
        return;

    if (e1->v.solid != SOLID_NOT)
    {
        SV_SetGlobalTrace(ptrace);
        gEntityInterface_pfnTouch(e1, e2);
    }

    if (e2->v.solid != SOLID_NOT)
    {
        SV_SetGlobalTrace(ptrace);
        gEntityInterface_pfnTouch(e2, e1);
    }
}

#define STOP_EPSILON 0.1f

int ClipVelocity(vec_t *in, vec_t *normal, vec_t *out, float overbounce)
{
    float backoff;
    float change;
    int i, blocked;

    blocked = 0;
    if (normal[2] > 0)
        blocked |= 1;
    if (!normal[2])
        blocked |= 2;

    backoff = DotProduct(in, normal) * overbounce;

    for (i = 0; i < 3; i++)
    {
        change = normal[i] * backoff;
        out[i] = in[i] - change;
        if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
            out[i] = 0;
    }

    return blocked;
}

#define MAX_CLIP_PLANES 5

int SV_FlyMove(edict_t *ent, float time, trace_t *steptrace)
{
    int bumpcount, numbumps;
    vec3_t dir;
    float d;
    int numplanes;
    vec3_t planes[MAX_CLIP_PLANES];
    vec3_t primal_velocity, original_velocity, new_velocity;
    int i, j;
    trace_t trace;
    vec3_t end;
    float time_left;
    int blocked;
    qboolean monsterClip;

    numbumps = 4;

    blocked = 0;
    VectorCopy(ent->v.velocity, original_velocity);
    VectorCopy(ent->v.velocity, primal_velocity);
    numplanes = 0;

    time_left = time;

    if (ent->v.flags & FL_MONSTERCLIP)
        monsterClip = 1;
    else
        monsterClip = 0;

    for (bumpcount = 0; bumpcount < numbumps; bumpcount++)
    {
        if (!ent->v.velocity[0] && !ent->v.velocity[1] && !ent->v.velocity[2])
            break;

        for (i = 0; i < 3; i++)
            end[i] = ent->v.origin[i] + time_left * ent->v.velocity[i];

        trace = SV_Move(ent->v.origin, ent->v.mins, ent->v.maxs, end, MOVE_NORMAL, ent, monsterClip);

        if (trace.allsolid)
        {
            VectorCopy(vec3_origin, ent->v.velocity);
            return 4;
        }

        if (trace.fraction > 0)
        {
            VectorCopy(trace.endpos, ent->v.origin);
            VectorCopy(ent->v.velocity, original_velocity);
            numplanes = 0;
        }

        if (trace.fraction == 1)
            break;

        if (!trace.ent)
            Sys_Error("SV_FlyMove: !trace.ent");

        if (trace.plane.normal[2] > 0.7f)
        {
            blocked |= 1;
            if (trace.ent->v.solid == SOLID_BSP
                || trace.ent->v.movetype == MOVETYPE_PUSHSTEP
                || trace.ent->v.solid == SOLID_SLIDEBOX
                || (ent->v.flags & FL_CLIENT))
            {
                ent->v.flags |= FL_ONGROUND;
                ent->v.groundentity = trace.ent;
            }
        }

        if (!trace.plane.normal[2])
        {
            blocked |= 2;
            if (steptrace)
                *steptrace = trace;
        }

        SV_Impact(ent, trace.ent, &trace);
        if (ent->free)
            break;

        time_left -= time_left * trace.fraction;

        if (numplanes >= MAX_CLIP_PLANES)
        {
            VectorCopy(vec3_origin, ent->v.velocity);
            return blocked;
        }

        VectorCopy(trace.plane.normal, planes[numplanes]);
        numplanes++;

        if (ent->v.movetype == MOVETYPE_WALK && (!(ent->v.flags & FL_ONGROUND) || ent->v.friction != 1))
        {
            for (i = 0; i < numplanes; i++)
            {
                if (planes[i][2] > 0.7f)
                {
                    ClipVelocity(original_velocity, planes[i], new_velocity, 1);
                    VectorCopy(new_velocity, original_velocity);
                }
                else
                {
                    ClipVelocity(original_velocity, planes[i], new_velocity, 1.0f + sv_bounce.value * (1 - ent->v.friction));
                }
            }

            VectorCopy(new_velocity, ent->v.velocity);
            VectorCopy(new_velocity, original_velocity);
        }
        else
        {
            for (i = 0; i < numplanes; i++)
            {
                ClipVelocity(original_velocity, planes[i], new_velocity, 1);
                for (j = 0; j < numplanes; j++)
                {
                    if (j != i)
                    {
                        if (DotProduct(new_velocity, planes[j]) < 0)
                            break;
                    }
                }
                if (j == numplanes)
                    break;
            }

            if (i != numplanes)
            {
                VectorCopy(new_velocity, ent->v.velocity);
            }
            else
            {
                if (numplanes != 2)
                    return blocked;

                CrossProduct(planes[0], planes[1], dir);
                d = DotProduct(dir, ent->v.velocity);
                VectorScale(dir, d, ent->v.velocity);
            }

            if (DotProduct(ent->v.velocity, primal_velocity) <= 0)
            {
                VectorCopy(vec3_origin, ent->v.velocity);
                return blocked;
            }
        }
    }

    return blocked;
}

void SV_AddGravity(edict_t *ent)
{
    float ent_gravity;

    if (ent->v.gravity)
        ent_gravity = ent->v.gravity;
    else
        ent_gravity = 1.0f;

    ent->v.velocity[2] -= ent_gravity * sv_gravity.value * host_frametime;
    ent->v.velocity[2] += ent->v.basevelocity[2] * host_frametime;
    ent->v.basevelocity[2] = 0;

    SV_CheckVelocity(ent);
}

void SV_AddCorrectGravity(edict_t *ent)
{
    float ent_gravity;

    if (ent->v.gravity)
        ent_gravity = ent->v.gravity;
    else
        ent_gravity = 1.0f;

    ent->v.velocity[2] -= ent_gravity * sv_gravity.value * (host_frametime * 0.5f);
    ent->v.velocity[2] += ent->v.basevelocity[2] * host_frametime;
    ent->v.basevelocity[2] = 0;

    SV_CheckVelocity(ent);
}

void SV_FixupGravityVelocity(edict_t *ent)
{
    float ent_gravity;

    if (ent->v.gravity)
        ent_gravity = ent->v.gravity;
    else
        ent_gravity = 1.0f;

    ent->v.velocity[2] -= ent_gravity * sv_gravity.value * host_frametime * 0.5f;

    SV_CheckVelocity(ent);
}

trace_t SV_PushEntity(edict_t *ent, vec_t *push)
{
    trace_t trace;
    vec3_t end;
    int moveType;
    qboolean monsterClip;

    if (ent->v.flags & FL_MONSTERCLIP)
        monsterClip = 1;
    else
        monsterClip = 0;

    VectorAdd(ent->v.origin, push, end);

    if (ent->v.movetype == MOVETYPE_FLYMISSILE)
        moveType = MOVE_MISSILE;
    else if (ent->v.solid == SOLID_TRIGGER || ent->v.solid == SOLID_NOT)
        moveType = MOVE_NOMONSTERS;
    else
        moveType = MOVE_NORMAL;

    trace = SV_Move(ent->v.origin, ent->v.mins, ent->v.maxs, end, moveType, ent, monsterClip);

    if (trace.fraction != 0)
        VectorCopy(trace.endpos, ent->v.origin);

    SV_LinkEdict(ent, true);

    if (trace.ent)
        SV_Impact(ent, trace.ent, &trace);

    return trace;
}

void SV_PushMove(edict_t *pusher, float movetime)
{
    int i, e;
    edict_t *check, *block;
    vec3_t mins, maxs, move;
    vec3_t entorig, pushorig;
    int num_moved;

    if (!pusher->v.velocity[0] && !pusher->v.velocity[1] && !pusher->v.velocity[2])
    {
        pusher->v.ltime += movetime;
        return;
    }

    VectorCopy(pusher->v.origin, pushorig);

    for (i = 0; i < 3; i++)
    {
        move[i] = pusher->v.velocity[i] * movetime;
        mins[i] = pusher->v.absmin[i] + move[i];
        maxs[i] = pusher->v.absmax[i] + move[i];
        pusher->v.origin[i] += move[i];
    }

    pusher->v.ltime += movetime;
    SV_LinkEdict(pusher, false);

    if (pusher->v.solid == SOLID_NOT)
        return;

    num_moved = 0;
    for (e = 1; e < sv.num_edicts; e++)
    {
        check = &sv.edicts[e];
        if (check->free)
            continue;

        if (check->v.movetype == MOVETYPE_PUSH
            || check->v.movetype == MOVETYPE_NONE
            || check->v.movetype == MOVETYPE_FOLLOW
            || check->v.movetype == MOVETYPE_NOCLIP)
            continue;

        if (!((check->v.flags & FL_ONGROUND) && check->v.groundentity == pusher))
        {
            if (check->v.absmin[0] >= maxs[0]
                || check->v.absmin[1] >= maxs[1]
                || check->v.absmin[2] >= maxs[2]
                || check->v.absmax[0] <= mins[0]
                || check->v.absmax[1] <= mins[1]
                || check->v.absmax[2] <= mins[2])
                continue;

            if (!SV_TestEntityPosition(check))
                continue;
        }

        if (check->v.movetype != MOVETYPE_WALK)
            check->v.flags &= ~FL_ONGROUND;

        VectorCopy(check->v.origin, entorig);
        VectorCopy(check->v.origin, g_moved_from[num_moved]);
        g_moved_edict[num_moved] = check;
        num_moved++;

        pusher->v.solid = SOLID_NOT;
        SV_PushEntity(check, move);
        pusher->v.solid = SOLID_BSP;

        block = SV_TestEntityPosition(check);
        if (block)
        {
            if (check->v.mins[0] == check->v.maxs[0])
                continue;

            if (check->v.solid == SOLID_NOT || check->v.solid == SOLID_TRIGGER)
            {
                check->v.mins[0] = check->v.mins[1] = 0;
                VectorCopy(check->v.mins, check->v.maxs);
                continue;
            }

            VectorCopy(entorig, check->v.origin);
            SV_LinkEdict(check, true);

            VectorCopy(pushorig, pusher->v.origin);
            SV_LinkEdict(pusher, false);
            pusher->v.ltime -= movetime;

            gEntityInterface.pfnBlocked(pusher, check);

            for (i = 0; i < num_moved; i++)
            {
                VectorCopy(g_moved_from[i], g_moved_edict[i]->v.origin);
                SV_LinkEdict(g_moved_edict[i], false);
            }
            return;
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/engine/sv_phys", SV_PushRotate);

INCLUDE_ASM("asm/nonmatchings/engine/sv_phys", SV_Physics_Pusher);

INCLUDE_ASM("asm/nonmatchings/engine/sv_phys", SV_CheckWater);

INCLUDE_ASM("asm/nonmatchings/engine/sv_phys", SV_RecursiveWaterLevel);

INCLUDE_ASM("asm/nonmatchings/engine/sv_phys", SV_Submerged);

INCLUDE_ASM("asm/nonmatchings/engine/sv_phys", SV_Physics_None);

INCLUDE_ASM("asm/nonmatchings/engine/sv_phys", SV_Physics_Noclip);

INCLUDE_ASM("asm/nonmatchings/engine/sv_phys", SV_CheckWaterTransition);

INCLUDE_ASM("asm/nonmatchings/engine/sv_phys", SV_Physics_Toss);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038FA20);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_phys", D_0038FA38);

INCLUDE_ASM("asm/nonmatchings/engine/sv_phys", PF_WaterMove);

INCLUDE_ASM("asm/nonmatchings/engine/sv_phys", SV_Physics_Step);

INCLUDE_ASM("asm/nonmatchings/engine/sv_phys", func_002879D0);

INCLUDE_ASM("asm/nonmatchings/engine/sv_phys", SV_Physics);

INCLUDE_ASM("asm/nonmatchings/engine/sv_phys", SV_Trace_Toss);

INCLUDE_ASM("asm/nonmatchings/engine/sv_phys", SV_SetMoveVars);

INCLUDE_ASM("asm/nonmatchings/engine/sv_phys", func_002882A0);
