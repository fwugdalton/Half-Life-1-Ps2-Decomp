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
#define FL_MONSTER (1 << 5)
#define FL_ONGROUND (1 << 9)
#define FL_ALWAYSTHINK (1 << 21)
#define FL_FLY (1 << 0)
#define FL_SWIM (1 << 1)
#define FL_FLOAT (1 << 15)
#define FL_INWATER (1 << 4)
#define FL_GODMODE (1 << 6)
#define FL_WATERJUMP (1 << 11)
#define FL_IMMUNE_WATER (1 << 17)
#define FL_IMMUNE_SLIME (1 << 18)
#define FL_IMMUNE_LAVA (1 << 19)

#define DEAD_NO 0
#define CHAN_BODY 4
#define ATTN_NORM 0.8f
#define PITCH_NORM 100
#define FL_MONSTERCLIP (1 << 23)

#define CONTENTS_EMPTY -1
#define CONTENTS_SOLID -2
#define CONTENTS_WATER -3
#define CONTENTS_SLIME -4
#define CONTENTS_LAVA -5
#define CONTENTS_CURRENT_0 -9
#define CONTENTS_CURRENT_DOWN -14
#define CONTENTS_TRANSLUCENT -15

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
extern cvar_t sv_friction;
extern cvar_t sv_stopspeed;
float sqrtf(float);
qboolean SV_CheckBottom(edict_t *ent);
void PF_WaterMove(edict_t *pSelf);
extern float host_frametime;
void SV_LinkEdict(edict_t *ent, qboolean touch_triggers);
extern vec3_t *g_moved_from;
extern edict_t **g_moved_edict;
int SV_PushRotate(edict_t *pusher, float movetime);
int SV_PointContents(const vec3_t p);
int RandomLong(int lLow, int lHigh);
void SV_StartSound(edict_t *entity, int channel, const char *sample, int volume, float attenuation, int fFlags, int pitch);
extern vec3_t current_table[];
extern vec3_t g_vecZero;
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

qboolean SV_DoThink(edict_t *ent)
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

/* a parte de fora do SV_RunThink do HL fica inline nos chamadores */
static inline qboolean SV_RunThink(edict_t *ent)
{
    float thinktime;

    if (!(ent->v.flags & FL_KILLME))
    {
        thinktime = ent->v.nextthink;
        if (thinktime <= 0 || thinktime > sv.time + host_frametime)
            return true;

        return SV_DoThink(ent);
    }

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

    ent->v.velocity[2] -= ent_gravity * sv_gravity.value * 0.5f * host_frametime;
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

#ifdef NON_MATCHING
int SV_PushRotate(edict_t *pusher, float movetime)
{
    int i, e;
    edict_t *check;
    vec3_t move, amove;
    vec3_t entorig, pushorig;
    vec3_t org2, org, temp;
    vec3_t forwardNow, rightNow, upNow;
    vec3_t forward, right, up;
    int num_moved;

    if (!pusher->v.avelocity[0] && !pusher->v.avelocity[1] && !pusher->v.avelocity[2])
    {
        pusher->v.ltime += movetime;
        return 1;
    }

    for (i = 0; i < 3; i++)
        amove[i] = pusher->v.avelocity[i] * movetime;

    amove[0] = 0;
    amove[2] = 0;
    AngleVectors(pusher->v.angles, forward, right, up);
    VectorCopy(pusher->v.angles, pushorig);
    VectorAdd(pusher->v.angles, amove, pusher->v.angles);
    AngleVectorsTranspose(pusher->v.angles, forwardNow, rightNow, upNow);

    pusher->v.ltime += movetime;
    SV_LinkEdict(pusher, false);

    if (pusher->v.solid == SOLID_NOT)
        return 1;

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
            if (check->v.absmin[0] >= pusher->v.absmax[0]
                || check->v.absmin[1] >= pusher->v.absmax[1]
                || check->v.absmin[2] >= pusher->v.absmax[2]
                || check->v.absmax[0] <= pusher->v.absmin[0]
                || check->v.absmax[1] <= pusher->v.absmin[1]
                || check->v.absmax[2] <= pusher->v.absmin[2])
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

        if (num_moved >= sv.max_edicts)
            Sys_Error("Out of edicts in simulator!\n");

        if (check->v.movetype == MOVETYPE_PUSHSTEP)
        {
            org2[0] = (check->v.absmin[0] + check->v.absmax[0]) * 0.5f;
            org2[1] = (check->v.absmin[1] + check->v.absmax[1]) * 0.5f;
            org2[2] = (check->v.absmin[2] + check->v.absmax[2]) * 0.5f;
            VectorSubtract(org2, pusher->v.origin, org);
        }
        else
        {
            VectorSubtract(check->v.origin, pusher->v.origin, org);
        }

        org2[0] = DotProduct(org, forward);
        org2[1] = -DotProduct(org, right);
        org2[2] = DotProduct(org, up);

        temp[0] = DotProduct(org2, forwardNow);
        temp[1] = DotProduct(org2, rightNow);
        temp[2] = DotProduct(org2, upNow);

        VectorSubtract(temp, org, move);

        pusher->v.solid = SOLID_NOT;
        SV_PushEntity(check, move);
        pusher->v.solid = SOLID_BSP;

        if (check->v.movetype != MOVETYPE_PUSHSTEP)
        {
            if (check->v.flags & FL_CLIENT)
            {
                check->v.fixangle = 2;
                check->v.avelocity[1] = amove[1];
            }
            else
            {
                VectorAdd(check->v.angles, amove, check->v.angles);
            }
        }

        if (SV_TestEntityPosition(check))
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

            VectorCopy(pushorig, pusher->v.angles);
            SV_LinkEdict(pusher, false);
            pusher->v.ltime -= movetime;

            gEntityInterface.pfnBlocked(pusher, check);

            for (i = 0; i < num_moved; i++)
            {
                VectorCopy(g_moved_from[i], g_moved_edict[i]->v.origin);
                if (g_moved_edict[i]->v.flags & FL_CLIENT)
                {
                    g_moved_edict[i]->v.avelocity[1] = 0;
                }
                else if (g_moved_edict[i]->v.movetype != MOVETYPE_PUSHSTEP)
                {
                    VectorSubtract(g_moved_edict[i]->v.angles, amove, g_moved_edict[i]->v.angles);
                }
                SV_LinkEdict(g_moved_edict[i], false);
            }
            return 0;
        }
    }

    return 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/sv_phys", SV_PushRotate);
#endif

void SV_Physics_Pusher(edict_t *ent, float movetime)
{
    float thinktime;
    float oldltime;
    float savetime;

    oldltime = ent->v.ltime;
    thinktime = ent->v.nextthink;

    if (ent->v.avelocity[0] != 0 || ent->v.avelocity[1] != 0 || ent->v.avelocity[2] != 0)
    {
        if (ent->v.velocity[0] != 0 || ent->v.velocity[1] != 0 || ent->v.velocity[2] != 0)
        {
            if (SV_PushRotate(ent, movetime))
            {
                savetime = ent->v.ltime;
                ent->v.ltime = oldltime;
                SV_PushMove(ent, movetime);
                if (ent->v.ltime < savetime)
                    ent->v.ltime = savetime;
            }
        }
        else
        {
            SV_PushRotate(ent, movetime);
        }
    }
    else
    {
        SV_PushMove(ent, movetime);
    }

    if (thinktime > oldltime && ((ent->v.flags & FL_ALWAYSTHINK) || thinktime <= ent->v.ltime))
    {
        ent->v.nextthink = 0;
        gGlobalVariables.time = sv.time;
        gEntityInterface.pfnThink(ent);
    }
}

qboolean SV_CheckWater(edict_t *ent)
{
    vec3_t point;
    int cont;
    int truecont;

    point[0] = (ent->v.absmin[0] + ent->v.absmax[0]) * 0.5f;
    point[1] = (ent->v.absmin[1] + ent->v.absmax[1]) * 0.5f;
    point[2] = ent->v.absmin[2] + 1;

    ent->v.waterlevel = 0;
    ent->v.watertype = CONTENTS_EMPTY;

    cont = SV_PointContents(point);
    if (cont <= CONTENTS_WATER && cont > CONTENTS_TRANSLUCENT)
    {
        truecont = cont;
        ent->v.watertype = cont;
        ent->v.waterlevel = 1;

        if (ent->v.absmin[2] == ent->v.absmax[2])
        {
            ent->v.waterlevel = 3;
        }
        else
        {
            point[2] = (ent->v.absmin[2] + ent->v.absmax[2]) * 0.5f;
            cont = SV_PointContents(point);
            if (cont <= CONTENTS_WATER && cont > CONTENTS_TRANSLUCENT)
            {
                ent->v.waterlevel = 2;
                point[2] += ent->v.view_ofs[2];
                cont = SV_PointContents(point);
                if (cont <= CONTENTS_WATER && cont > CONTENTS_TRANSLUCENT)
                    ent->v.waterlevel = 3;
            }
        }

        if (truecont <= CONTENTS_CURRENT_0 && truecont >= CONTENTS_CURRENT_DOWN)
            VectorMA(ent->v.basevelocity, 50.0f * ent->v.waterlevel, current_table[CONTENTS_CURRENT_0 - truecont], ent->v.basevelocity);
    }

    return ent->v.waterlevel > 1;
}


float SV_RecursiveWaterLevel(vec_t *center, float out, float in, int count)
{
    vec3_t test;
    float offset;

    count++;
    offset = in + (out - in) * 0.5f;
    if (count > 5)
        return offset;

    VectorCopy(center, test);
    test[2] += offset;

    if (SV_PointContents(test) == CONTENTS_WATER)
        return SV_RecursiveWaterLevel(center, out, offset, count);
    else
        return SV_RecursiveWaterLevel(center, offset, in, count);
}

float SV_Submerged(edict_t *ent)
{
    vec3_t center;
    vec3_t end;
    float start, bottom;

    center[0] = (ent->v.absmin[0] + ent->v.absmax[0]) * 0.5f;
    center[1] = (ent->v.absmin[1] + ent->v.absmax[1]) * 0.5f;
    center[2] = (ent->v.absmin[2] + ent->v.absmax[2]) * 0.5f;
    start = ent->v.absmin[2] - center[2];

    switch (ent->v.waterlevel)
    {
    case 1:
        return SV_RecursiveWaterLevel(center, 0, start, 0) - start;
    case 3:
        end[0] = center[0];
        end[1] = center[1];
        end[2] = ent->v.absmax[2];
        if (SV_PointContents(end) == CONTENTS_WATER)
            return ent->v.maxs[2] - ent->v.mins[2];
    case 2:
        bottom = ent->v.absmax[2] - center[2];
        return SV_RecursiveWaterLevel(center, bottom, 0, 0) - start;
    }

    return 0;
}

void SV_Physics_None(edict_t *ent)
{
    SV_RunThink(ent);
}

void SV_Physics_Noclip(edict_t *ent)
{
    if (!SV_RunThink(ent))
        return;

    VectorMA(ent->v.angles, host_frametime, ent->v.avelocity, ent->v.angles);
    VectorMA(ent->v.origin, host_frametime, ent->v.velocity, ent->v.origin);

    SV_LinkEdict(ent, false);
}

void SV_CheckWaterTransition(edict_t *ent)
{
    vec3_t point;
    int cont;

    point[0] = (ent->v.absmin[0] + ent->v.absmax[0]) * 0.5f;
    point[1] = (ent->v.absmin[1] + ent->v.absmax[1]) * 0.5f;
    point[2] = ent->v.absmin[2] + 1.0f;

    cont = SV_PointContents(point);

    if (!ent->v.watertype)
    {
        ent->v.watertype = cont;
        ent->v.waterlevel = 1;
        return;
    }

    if (cont <= CONTENTS_WATER && cont > CONTENTS_TRANSLUCENT)
    {
        if (ent->v.watertype == CONTENTS_EMPTY)
        {
            SV_StartSound(ent, 0, "player/pl_wade1.wav", 255, 1.0f, 0, 100);
            ent->v.velocity[2] *= 0.5f;
        }

        ent->v.watertype = cont;
        ent->v.waterlevel = 1;

        if (ent->v.absmin[2] == ent->v.absmax[2])
        {
            ent->v.waterlevel = 3;
        }
        else
        {
            point[2] = (ent->v.absmin[2] + ent->v.absmax[2]) * 0.5f;
            cont = SV_PointContents(point);
            if (cont <= CONTENTS_WATER && cont > CONTENTS_TRANSLUCENT)
            {
                ent->v.waterlevel = 2;
                point[2] += ent->v.view_ofs[2];
                cont = SV_PointContents(point);
                if (cont <= CONTENTS_WATER && cont > CONTENTS_TRANSLUCENT)
                    ent->v.waterlevel = 3;
            }
        }
    }
    else
    {
        if (ent->v.watertype != CONTENTS_EMPTY)
            SV_StartSound(ent, 0, "player/pl_wade2.wav", 255, 1.0f, 0, 100);

        ent->v.watertype = CONTENTS_EMPTY;
        ent->v.waterlevel = 0;
    }
}

#ifdef NON_MATCHING
void SV_Physics_Toss(edict_t *ent)
{
    trace_t trace;
    vec3_t move;
    float backoff;
    float vel;

    SV_CheckWater(ent);

    if (!SV_RunThink(ent))
        return;

    if (ent->v.velocity[2] > 0 || !ent->v.groundentity || (ent->v.groundentity->v.flags & (FL_MONSTER | FL_CLIENT)))
        ent->v.flags &= ~FL_ONGROUND;

    if ((ent->v.flags & FL_ONGROUND) && VectorCompare(ent->v.velocity, g_vecZero))
    {
        VectorCopy(vec3_origin, ent->v.avelocity);
        if (VectorCompare(ent->v.basevelocity, g_vecZero))
            return;
    }

    SV_CheckVelocity(ent);

    if (ent->v.movetype != MOVETYPE_FLY && ent->v.movetype != MOVETYPE_BOUNCEMISSILE && ent->v.movetype != MOVETYPE_FLYMISSILE)
        SV_AddGravity(ent);

    VectorMA(ent->v.angles, host_frametime, ent->v.avelocity, ent->v.angles);
    VectorAdd(ent->v.velocity, ent->v.basevelocity, ent->v.velocity);
    SV_CheckVelocity(ent);
    VectorScale(ent->v.velocity, host_frametime, move);
    VectorSubtract(ent->v.velocity, ent->v.basevelocity, ent->v.velocity);

    trace = SV_PushEntity(ent, move);
    SV_CheckVelocity(ent);

    if (trace.allsolid)
    {
        VectorCopy(vec3_origin, ent->v.velocity);
        VectorCopy(vec3_origin, ent->v.avelocity);
        return;
    }

    if (trace.fraction == 1.0f)
    {
        SV_CheckWaterTransition(ent);
        return;
    }

    if (ent->free)
        return;

    if (ent->v.movetype == MOVETYPE_BOUNCE)
        backoff = 2.0f - ent->v.friction;
    else if (ent->v.movetype == MOVETYPE_BOUNCEMISSILE)
        backoff = 2.0f;
    else
        backoff = 1.0f;

    ClipVelocity(ent->v.velocity, trace.plane.normal, ent->v.velocity, backoff);

    if (trace.plane.normal[2] > 0.7f)
    {
        VectorAdd(ent->v.velocity, ent->v.basevelocity, move);
        vel = DotProduct(move, move);

        if (move[2] < sv_gravity.value * host_frametime)
        {
            ent->v.flags |= FL_ONGROUND;
            ent->v.velocity[2] = 0;
            ent->v.groundentity = trace.ent;
        }

        if (vel < 900 || (ent->v.movetype != MOVETYPE_BOUNCE && ent->v.movetype != MOVETYPE_BOUNCEMISSILE))
        {
            ent->v.flags |= FL_ONGROUND;
            ent->v.groundentity = trace.ent;
            VectorCopy(vec3_origin, ent->v.velocity);
            VectorCopy(vec3_origin, ent->v.avelocity);
        }
        else
        {
            VectorScale(ent->v.velocity, (1.0f - trace.fraction) * host_frametime * 0.9f, move);
            VectorMA(move, (1.0f - trace.fraction) * host_frametime * 0.9f, ent->v.basevelocity, move);
            trace = SV_PushEntity(ent, move);
        }
    }

    SV_CheckWaterTransition(ent);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/sv_phys", SV_Physics_Toss);
#endif

#ifdef NON_MATCHING
void PF_WaterMove(edict_t *pSelf)
{
    int flags;
    int waterlevel;
    int watertype;
    float drownlevel;

    if (pSelf->v.movetype == MOVETYPE_NOCLIP)
    {
        pSelf->v.air_finished = sv.time + 12;
        return;
    }

    if (pSelf->v.health < 0)
        return;

    if (pSelf->v.deadflag == DEAD_NO)
        drownlevel = 3.0f;
    else
        drownlevel = 1.0f;

    flags = pSelf->v.flags;
    waterlevel = pSelf->v.waterlevel;
    watertype = pSelf->v.watertype;

    if (!(flags & (FL_IMMUNE_WATER | FL_GODMODE)))
    {
        if (((flags & FL_SWIM) && waterlevel < drownlevel) || waterlevel >= drownlevel)
        {
            if (pSelf->v.air_finished < sv.time && pSelf->v.pain_finished < sv.time)
            {
                pSelf->v.dmg += 2;
                if (pSelf->v.dmg > 15)
                    pSelf->v.dmg = 10;
                pSelf->v.pain_finished = sv.time + 1.0f;
            }
        }
        else if (pSelf->v.air_finished < sv.time)
        {
        }
        else
        {
            pSelf->v.air_finished = sv.time + 12;
            pSelf->v.dmg = 2;
        }
    }

    if (!waterlevel)
    {
        if (flags & FL_INWATER)
        {
            switch (RandomLong(0, 3))
            {
            case 0:
                SV_StartSound(pSelf, CHAN_BODY, "player/pl_wade1.wav", 255, ATTN_NORM, 0, PITCH_NORM);
                break;
            case 1:
                SV_StartSound(pSelf, CHAN_BODY, "player/pl_wade2.wav", 255, ATTN_NORM, 0, PITCH_NORM);
                break;
            case 2:
                SV_StartSound(pSelf, CHAN_BODY, "player/pl_wade3.wav", 255, ATTN_NORM, 0, PITCH_NORM);
                break;
            case 3:
                SV_StartSound(pSelf, CHAN_BODY, "player/pl_wade4.wav", 255, ATTN_NORM, 0, PITCH_NORM);
                break;
            }

            pSelf->v.flags = flags & ~FL_INWATER;
        }

        pSelf->v.air_finished = sv.time + 12;
        return;
    }

    if (watertype == CONTENTS_LAVA)
    {
        if (!(flags & (FL_IMMUNE_LAVA | FL_GODMODE)) && pSelf->v.dmgtime < sv.time)
        {
            if (pSelf->v.radsuit_finished < sv.time)
                pSelf->v.dmgtime = sv.time + 0.2f;
            else
                pSelf->v.dmgtime = sv.time + 1.0f;
        }
    }
    else if (watertype == CONTENTS_SLIME)
    {
        if (!(flags & (FL_IMMUNE_SLIME | FL_GODMODE)) && pSelf->v.dmgtime < sv.time && pSelf->v.radsuit_finished < sv.time)
            pSelf->v.dmgtime = sv.time + 1.0f;
    }

    if (!(flags & FL_INWATER))
    {
        if (watertype == CONTENTS_WATER)
        {
            switch (RandomLong(0, 3))
            {
            case 0:
                SV_StartSound(pSelf, CHAN_BODY, "player/pl_wade1.wav", 255, ATTN_NORM, 0, PITCH_NORM);
                break;
            case 1:
                SV_StartSound(pSelf, CHAN_BODY, "player/pl_wade2.wav", 255, ATTN_NORM, 0, PITCH_NORM);
                break;
            case 2:
                SV_StartSound(pSelf, CHAN_BODY, "player/pl_wade3.wav", 255, ATTN_NORM, 0, PITCH_NORM);
                break;
            case 3:
                SV_StartSound(pSelf, CHAN_BODY, "player/pl_wade4.wav", 255, ATTN_NORM, 0, PITCH_NORM);
                break;
            }
        }

        pSelf->v.dmgtime = 0;
        pSelf->v.flags = flags | FL_INWATER;
    }

    if (!(flags & FL_WATERJUMP))
        VectorMA(pSelf->v.velocity, -0.8f * pSelf->v.waterlevel * host_frametime, pSelf->v.velocity, pSelf->v.velocity);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/sv_phys", PF_WaterMove);
#endif

#ifdef NON_MATCHING
void SV_Physics_Step(edict_t *ent)
{
    qboolean wasonground;
    qboolean inwater;
    float *vel;
    float speed, newspeed, control;
    float friction;

    PF_WaterMove(ent);
    SV_CheckVelocity(ent);

    wasonground = ent->v.flags & FL_ONGROUND;
    inwater = SV_CheckWater(ent);

    if ((ent->v.flags & FL_FLOAT) && ent->v.waterlevel > 0)
    {
        float buoyancy = SV_Submerged(ent) * ent->v.skin * host_frametime;

        SV_AddGravity(ent);
        ent->v.velocity[2] += buoyancy;
    }

    if (!wasonground && !(ent->v.flags & FL_FLY) && (!(ent->v.flags & FL_SWIM) || ent->v.waterlevel <= 0))
    {
        if (!inwater)
            SV_AddGravity(ent);
    }

    if (!VectorCompare(ent->v.velocity, g_vecZero) || !VectorCompare(ent->v.basevelocity, g_vecZero))
    {
        ent->v.flags &= ~FL_ONGROUND;

        if (wasonground && (ent->v.health > 0 || SV_CheckBottom(ent)))
        {
            vel = ent->v.velocity;
            speed = sqrtf(vel[0] * vel[0] + vel[1] * vel[1]);
            if (speed)
            {
                friction = sv_friction.value * ent->v.friction;
                ent->v.friction = 1.0f;

                control = (speed < sv_stopspeed.value) ? sv_stopspeed.value : speed;
                newspeed = speed - host_frametime * control * friction;
                if (newspeed < 0)
                    newspeed = 0;
                newspeed /= speed;

                vel[0] *= newspeed;
                vel[1] *= newspeed;
            }
        }

        VectorAdd(ent->v.velocity, ent->v.basevelocity, ent->v.velocity);
        SV_CheckVelocity(ent);
        SV_FlyMove(ent, host_frametime, NULL);
        SV_CheckVelocity(ent);
        VectorSubtract(ent->v.velocity, ent->v.basevelocity, ent->v.velocity);
        SV_CheckVelocity(ent);

        {
            vec3_t mins, maxs, point;
            int x, y;

            VectorAdd(ent->v.origin, ent->v.mins, mins);
            VectorAdd(ent->v.origin, ent->v.maxs, maxs);

            point[2] = mins[2] - 1;
            for (x = 0; x <= 1; x++)
            {
                for (y = 0; y <= 1; y++)
                {
                    point[0] = x ? maxs[0] : mins[0];
                    point[1] = y ? maxs[1] : mins[1];
                    if (SV_PointContents(point) == CONTENTS_SOLID)
                    {
                        ent->v.flags |= FL_ONGROUND;
                        break;
                    }
                }
            }
        }

        SV_LinkEdict(ent, true);
    }
    else
    {
        if (gGlobalVariables.force_retouch != 0)
        {
            trace_t trace = SV_Move(ent->v.origin, ent->v.mins, ent->v.maxs, ent->v.origin, MOVE_NORMAL, ent, (ent->v.flags & FL_MONSTERCLIP) ? 1 : 0);

            if ((trace.fraction < 1.0f || trace.startsolid) && trace.ent)
                SV_Impact(ent, trace.ent, &trace);
        }
    }

    SV_RunThink(ent);
    SV_CheckWaterTransition(ent);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/sv_phys", SV_Physics_Step);
#endif

INCLUDE_ASM("asm/nonmatchings/engine/sv_phys", func_002879D0);

INCLUDE_ASM("asm/nonmatchings/engine/sv_phys", SV_Physics);

INCLUDE_ASM("asm/nonmatchings/engine/sv_phys", SV_Trace_Toss);

INCLUDE_ASM("asm/nonmatchings/engine/sv_phys", SV_SetMoveVars);

INCLUDE_ASM("asm/nonmatchings/engine/sv_phys", func_002882A0);
