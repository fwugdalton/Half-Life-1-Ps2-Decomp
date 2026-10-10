#include "common.h"
#include "engine/mathlib.h"
#include "engine/model.h"
#include "engine/server.h"
#include "engine/cvar.h"
#include "engine/cmd.h"
#include "engine/progdefs.h"
#include "engine/eiface.h"
#include "engine/pmove.h"

#define FL_ONGROUND (1 << 9)
#define MAX_FORWARD 6
#define ON_EPSILON 0.1f

extern edict_t *sv_player;
extern vec3_t sv_pmove_mins, sv_pmove_maxs;
int NUM_FOR_EDICT(const edict_t *e);

#define SOLID_NOT 0
#define SOLID_BBOX 2
#define SOLID_SLIDEBOX 3
#define SOLID_BSP 4
#define FL_SPECTATOR (1 << 26)
#define FL_KILLME (1 << 30)
void ED_Free(edict_t *ed);
extern int cmd_source;
void SV_EndRedirect(void);
void SV_BroadcastPrintf(const char *fmt, ...);
void SV_DropClient(client_t *cl, qboolean crash);
char *Info_Serverinfo(void);
void Info_Print(char *s);
int atoi(const char *);
void SV_RunCmd(usercmd_t *ucmd, int clientId);
void func_00289320(float frametime);
qboolean func_00289818(edict_t *ent, float frametime, double time);
void func_002893F8(areanode_t *node, int clientId);
void func_0028A820(void);
float PM_CalcRoll(vec3_t angles, vec3_t velocity, float rollangle, float rollspeed);
void PM_PlayerMove(qboolean server, int clientId);
void PM_CheckVelocity(int clientId);
edict_t *EDICT_NUM(int n);
void SV_Impact(edict_t *e1, edict_t *e2, trace_t *ptrace);
float sqrtf(float);
extern cvar_t sv_maxspeed, sv_rollangle, sv_rollspeed, pm_pushfix;
extern int cl_numlocal;
extern int D_003241F0;
extern float *D_00399428;
extern float *D_00399434;
extern areanode_t sv_areanodes[];

#define FL_CONVEYOR (1 << 2)
#define FL_DUCKING (1 << 14)
#define FL_ONTRAIN (1 << 24)
#define FL_BASEVELOCITY (1 << 22)
#define MOVETYPE_WALK 3
void SV_PreRunCmd(void);
void SV_LinkEdict(edict_t *ent, qboolean touch_triggers);
void func_00288378(void);
void func_00288CE8(void);
void MSG_WriteByte(sizebuf_t *sb, int c);
int MSG_ReadByte(void);
char *MSG_ReadString(void);
float MSG_ReadCoord(void);
void MSG_ReadDeltaUsercmd(usercmd_t *to, usercmd_t *from);
byte COM_BlockSequenceCRCByte(byte *base, int length, int sequence);
extern int msg_badread, msg_readcount;
extern sizebuf_t net_message;
extern int net_drop;
extern int key_dest;
extern cvar_t sv_showcmd;

#define svc_choke 53
#define clc_nop 1
#define clc_move 3
#define clc_stringcmd 4
#define clc_delta 5
#define clc_tmove 6
#define clc_upload 7
#define clc_resourcelist 8
#define key_game 0
#define IN_ATTACK (1 << 0)
#define IN_JUMP (1 << 2)
#define IN_ATTACK2 (1 << 11)
#define FL_FROZEN (1 << 12)
#define FL_MONSTERCLIP (1 << 23)
#define STUDIO_TRACE_HITBOX 0x200
#define EDICT_FROM_AREA(l) ((edict_t *)((byte *)(l) - 8))
extern byte *g_playertouch;
extern cvar_t sv_idealpitchscale;
extern float host_frametime;
void *memset(void *, int, unsigned int);
trace_t SV_Move(const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int type, edict_t *passedict, qboolean monsterClipBrush);

void func_00289058(void)
{
    float angleval, sinval, cosval;
    trace_t tr;
    vec3_t top, bottom;
    float z[MAX_FORWARD];
    int i, j;
    int step, dir, steps;

    if (!(sv_player->v.flags & FL_ONGROUND))
        return;

    angleval = sv_player->v.angles[YAW] * M_PI * 2 / 360;
    sinval = sinf(angleval);
    cosval = cosf(angleval);

    for (i = 0; i < MAX_FORWARD; i++)
    {
        top[0] = sv_player->v.origin[0] + cosval * (i + 3) * 12;
        top[1] = sv_player->v.origin[1] + sinval * (i + 3) * 12;
        top[2] = sv_player->v.origin[2] + sv_player->v.view_ofs[2];

        bottom[0] = top[0];
        bottom[1] = top[1];
        bottom[2] = top[2] - 160;

        tr = SV_Move(top, vec3_origin, vec3_origin, bottom, 1, sv_player, 0);
        if (tr.allsolid)
            return;

        if (tr.fraction == 1)
            return;

        z[i] = top[2] + tr.fraction * (bottom[2] - top[2]);
    }

    dir = 0;
    steps = 0;
    for (j = 1; j < i; j++)
    {
        step = z[j] - z[j - 1];
        if (step > -ON_EPSILON && step < ON_EPSILON)
            continue;

        if (dir && (step - dir > ON_EPSILON || step - dir < -ON_EPSILON))
            return;

        steps++;
        dir = step;
    }

    if (!dir)
    {
        sv_player->v.idealpitch = 0;
        return;
    }

    if (steps < 2)
        return;

    sv_player->v.idealpitch = -dir * sv_idealpitchscale.value;
}

void func_00289320(float frametime)
{
    float len;

    len = VectorNormalize(sv_player->v.punchangle);
    len -= (10.0f + len * 0.5f) * frametime;
    if (len < 0)
        len = 0;
    VectorScale(sv_player->v.punchangle, len, sv_player->v.punchangle);
}

void SV_PreRunCmd(void)
{
    memset(g_playertouch, 0, (sv.max_edicts + 7) / 8);
}

INCLUDE_RODATA("asm/nonmatchings/engine/sv_user", D_0038FEF0);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_user", D_0038FEF8);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_user", D_0038FF10);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_user", D_0038FF18);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_user", D_0038FF28);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_user", D_0038FF30);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_user", D_0038FF40);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_user", D_0038FF48);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_user", D_0038FF58);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_user", D_0038FF60);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_user", D_0038FF70);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_user", D_0038FF78);

#ifdef NON_MATCHING
void func_002893F8(areanode_t *node, int clientId)
{
    link_t *l, *next;
    edict_t *check;
    physent_t *ve;
    int i;
    model_t *pModel;

    for (l = node->solid_edicts.next; l != &node->solid_edicts; l = next)
    {
        next = l->next;
        check = EDICT_FROM_AREA(l);

        if (check->v.owner == sv_player)
            continue;

        if (check->v.solid != SOLID_BSP && check->v.solid != SOLID_BBOX && check->v.solid != SOLID_SLIDEBOX && check->v.solid != SOLID_NOT)
            continue;

        if (check->v.solid == SOLID_NOT && (!check->v.skin || !check->v.modelindex))
            continue;

        if ((check->v.flags & FL_MONSTERCLIP) && check->v.solid == SOLID_BSP)
            continue;

        if (check == sv_player)
            continue;

        for (i = 0; i < 3; i++)
        {
            if (check->v.absmin[i] > sv_pmove_maxs[i] || check->v.absmax[i] < sv_pmove_mins[i])
                break;
        }

        if (i != 3)
            continue;

        if (pmove[clientId].numphysent >= MAX_PHYSENTS)
        {
            Con_DPrintf("Too many physents on server playermove link adding...\n");
            return;
        }

        if (check->v.flags & FL_SPECTATOR)
            continue;

        if (check->v.solid == SOLID_NOT && check->v.skin == -16)
        {
            if (pmove[clientId].nummoveent > MAX_MOVEENTS)
            {
                Con_DPrintf("Too many physents on server playermove link adding...\n");
                return;
            }
            ve = &pmove[clientId].moveents[pmove[clientId].nummoveent++];
        }
        else
        {
            ve = &pmove[clientId].physents[pmove[clientId].numphysent++];
        }

        VectorCopy(check->v.origin, ve->origin);
        VectorCopy(check->v.angles, ve->angles);
        ve->info = NUM_FOR_EDICT(check);
        ve->studiomodel = NULL;
        ve->rendermode = check->v.rendermode;

        if (check->v.solid == SOLID_BSP)
        {
            ve->model = sv.models[check->v.modelindex];
        }
        else if (check->v.solid == SOLID_NOT)
        {
            if (check->v.modelindex)
                ve->model = sv.models[check->v.modelindex];
            else
                ve->model = NULL;
        }
        else
        {
            ve->model = NULL;
            if (check->v.solid == SOLID_BBOX)
            {
                pModel = check->v.modelindex ? sv.models[check->v.modelindex] : NULL;
                if (pModel && (pModel->flags & STUDIO_TRACE_HITBOX))
                    ve->studiomodel = pModel;
            }
            VectorCopy(check->v.mins, ve->mins);
            VectorCopy(check->v.maxs, ve->maxs);
        }

        ve->solid = check->v.solid;
        ve->skin = check->v.skin;
        ve->frame = check->v.frame;
        ve->sequence = check->v.sequence;
        *(int *)ve->controller = *(int *)check->v.controller;
        *(short *)ve->blending = *(short *)check->v.blending;
    }

    if (node->axis == -1)
        return;

    if (sv_pmove_maxs[node->axis] > node->dist)
        func_002893F8(node->children[0], clientId);
    if (sv_pmove_mins[node->axis] < node->dist)
        func_002893F8(node->children[1], clientId);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/sv_user", func_002893F8);
#endif

qboolean func_00289818(edict_t *ent, float frametime, double time)
{
    float thinktime;

    if (!(ent->v.flags & FL_KILLME))
    {
        thinktime = ent->v.nextthink;
        if (thinktime <= 0 || thinktime > time + frametime)
            return true;

        if (thinktime < time)
            thinktime = time;

        ent->v.nextthink = 0;
        gGlobalVariables.time = thinktime;
        gEntityInterface.pfnThink(ent);
    }

    if (ent->v.flags & FL_KILLME)
        ED_Free(ent);

    return !ent->free;
}

#ifdef NON_MATCHING
void SV_RunCmd(usercmd_t *ucmd, int clientId)
{
    static usercmd_t cmd;
    trace_t trace;
    vec3_t oldvel;
    vec3_t vel;
    int i;
    int oldmsec;
    float spd, fmove;
    edict_t *ent;
    pmtrace_t *tr;
    int n;

    cmd = *ucmd;

    if (cmd.msec > 50)
    {
        oldmsec = ucmd->msec;
        cmd.msec = oldmsec / 2;
        SV_RunCmd(&cmd, clientId);
        cmd.msec = oldmsec / 2;
        cmd.impulse = 0;
        SV_RunCmd(&cmd, clientId);
        return;
    }

    pm_frametime = ucmd->msec * 0.001f;
    func_00289320(pm_frametime);

    VectorCopy(vec3_origin, sv_player->v.clbasevelocity);

    if (!sv_player->v.fixangle)
        VectorCopy(ucmd->viewangles, sv_player->v.v_angle);

    sv_player->v.button = ucmd->buttons;
    if (ucmd->impulse)
        sv_player->v.impulse = ucmd->impulse;

    if ((sv_player->v.flags & FL_ONGROUND) && sv_player->v.groundentity && (sv_player->v.groundentity->v.flags & FL_CONVEYOR))
    {
        if (sv_player->v.flags & FL_BASEVELOCITY)
            VectorMA(sv_player->v.basevelocity, sv_player->v.groundentity->v.speed, sv_player->v.groundentity->v.movedir, sv_player->v.basevelocity);
        else
            VectorScale(sv_player->v.groundentity->v.movedir, sv_player->v.groundentity->v.speed, sv_player->v.basevelocity);
        sv_player->v.flags |= FL_BASEVELOCITY;
    }

    if (!(sv_player->v.flags & FL_BASEVELOCITY))
    {
        VectorMA(sv_player->v.velocity, 1.0f + pm_frametime * 0.5f, sv_player->v.basevelocity, sv_player->v.velocity);
        VectorCopy(vec3_origin, sv_player->v.basevelocity);
    }

    sv_player->v.flags &= ~FL_BASEVELOCITY;

    if (!host_client->spectator)
    {
        gGlobalVariables.time = host_client->unk_7410;
        gEntityInterface.pfnPlayerPreThink(sv_player);
        func_00289818(sv_player, pm_frametime, host_client->unk_7410);
    }

    host_client->unk_7410 += pm_frametime;

    pmove[clientId].usehull = 0;
    if (host_client->maxspeed != 0)
        pmove[clientId].maxspeed = host_client->maxspeed < sv_maxspeed.value ? host_client->maxspeed : sv_maxspeed.value;
    else
        pmove[clientId].maxspeed = sv_maxspeed.value;

    spd = sqrtf(cmd.forwardmove * cmd.forwardmove + cmd.sidemove * cmd.sidemove + cmd.upmove * cmd.upmove);
    if (spd != 0 && pmove[clientId].maxspeed < spd)
    {
        fmove = pmove[clientId].maxspeed / spd;
        cmd.forwardmove *= fmove;
        cmd.sidemove *= fmove;
        cmd.upmove *= fmove;
    }

    if (sv_player->v.flags & FL_DUCKING)
    {
        pmove[clientId].usehull = 1;
        cmd.forwardmove *= 0.333f;
        cmd.sidemove *= 0.333f;
        cmd.upmove *= 0.333f;
    }

    if ((sv_player->v.flags & (FL_FROZEN | FL_ONTRAIN)) || D_003241F0)
    {
        cmd.forwardmove = 0;
        cmd.sidemove = 0;
        cmd.upmove = 0;
    }

    if (sv_player->v.basevelocity[0] != 0 || sv_player->v.basevelocity[1] != 0 || sv_player->v.basevelocity[2] != 0)
        VectorCopy(sv_player->v.basevelocity, sv_player->v.clbasevelocity);

    VectorAdd(sv_player->v.v_angle, sv_player->v.punchangle, sv_player->v.v_angle);

    if (sv_player->v.health > 0)
    {
        sv_player->v.angles[ROLL] = PM_CalcRoll(sv_player->v.angles, sv_player->v.velocity, sv_rollangle.value, sv_rollspeed.value) * 4;
        if (!sv_player->v.fixangle)
        {
            if (cl_numlocal == 1)
                sv_player->v.angles[PITCH] = -sv_player->v.v_angle[PITCH] / 3;
            else
                sv_player->v.angles[PITCH] = -sv_player->v.v_angle[PITCH] / 6;
            sv_player->v.angles[YAW] = sv_player->v.v_angle[YAW];
        }
    }

    for (i = 0; i < 3; i++)
        pmove[clientId].origin[i] = sv_player->v.origin[i] + (sv_player->v.mins[i] - player_mins[pmove[clientId].usehull][i]);

    pmove[clientId].server = true;
    pmove[clientId].time = sv.time * 1000.0f;
    pmove[clientId].flDuckTime = host_client->flDuckTime;
    pmove[clientId].bInDuck = host_client->bInDuck;
    VectorCopy(sv_player->v.velocity, pmove[clientId].velocity);
    VectorCopy(sv_player->v.movedir, pmove[clientId].movedir);
    VectorCopy(sv_player->v.v_angle, pmove[clientId].angles);
    VectorCopy(sv_player->v.basevelocity, pmove[clientId].basevelocity);
    VectorCopy(sv_player->v.view_ofs, pmove[clientId].view_ofs);
    pmove[clientId].gravity = sv_player->v.gravity;
    pmove[clientId].friction = sv_player->v.friction;
    pmove[clientId].spectator = host_client->spectator;
    pmove[clientId].waterjumptime = sv_player->v.teleport_time;
    pmove[clientId].cmd = cmd;
    pmove[clientId].dead = sv_player->v.health <= 0;
    pmove[clientId].oldbuttons = host_client->oldbuttons;
    pmove[clientId].movetype = sv_player->v.movetype;
    pmove[clientId].flags = sv_player->v.flags;

    for (i = 0; i < 3; i++)
    {
        sv_pmove_mins[i] = pmove[clientId].origin[i] - 256;
        sv_pmove_maxs[i] = pmove[clientId].origin[i] + 256;
    }

    pmove[clientId].numphysent = 1;
    pmove[clientId].nummoveent = 0;
    pmove[clientId].physents[0].model = sv.worldmodel;
    func_002893F8(sv_areanodes, clientId);

    if (pm_pushfix.value != 0)
        SV_LinkEdict(sv_player, true);

    pmove[clientId].player_index = NUM_FOR_EDICT(sv_player);

    PM_PlayerMove(true, clientId);

    if (pmove[clientId].movetype == MOVETYPE_WALK)
        pmove[clientId].friction = 1.0f;

    host_client->bInDuck = pmove[clientId].bInDuck;
    host_client->flDuckTime = pmove[clientId].flDuckTime;
    host_client->oldbuttons = pmove[clientId].oldbuttons;
    sv_player->v.teleport_time = pmove[clientId].waterjumptime;
    sv_player->v.waterlevel = pm_waterlevel[clientId];
    sv_player->v.watertype = pm_watertype[clientId];
    sv_player->v.flags = pmove[clientId].flags;
    sv_player->v.friction = pmove[clientId].friction;
    if (!pmove[clientId].dead)
        sv_player->v.movetype = pmove[clientId].movetype;

    if (pm_onground[clientId] != -1)
    {
        sv_player->v.flags |= FL_ONGROUND;
        sv_player->v.groundentity = EDICT_NUM(pmove[clientId].physents[pm_onground[clientId]].info);
    }
    else
    {
        sv_player->v.flags &= ~FL_ONGROUND;
    }

    VectorCopy(pmove[clientId].view_ofs, sv_player->v.view_ofs);
    pm_onground[clientId] = pm_onground[clientId] != -1;

    for (i = 0; i < 3; i++)
        sv_player->v.origin[i] = pmove[clientId].origin[i] - (sv_player->v.mins[i] - player_mins[pmove[clientId].usehull][i]);

    if (pm_pushfix.value == 0)
        VectorCopy(pmove[clientId].velocity, sv_player->v.velocity);

    VectorCopy(pmove[clientId].basevelocity, sv_player->v.basevelocity);

    D_00399428 = sv_player->v.origin;
    D_00399434 = sv_player->v.velocity;

    if (pm_pushfix.value != 0)
    {
        if (!host_client->spectator)
        {
            VectorCopy(vec3_origin, vel);
            for (i = 0; i < pmove[clientId].numtouch; i++)
            {
                tr = &pmove[clientId].touchindex[i];
                VectorAdd(vel, tr->deltavelocity, vel);
            }

            PM_CheckVelocity(clientId);
            VectorAdd(pmove[clientId].velocity, vel, sv_player->v.velocity);
            SV_LinkEdict(sv_player, true);
        }
    }
    else if (!host_client->spectator)
    {
        SV_LinkEdict(sv_player, true);

        VectorCopy(sv_player->v.velocity, oldvel);

        for (i = 0; i < pmove[clientId].numtouch; i++)
        {
            tr = &pmove[clientId].touchindex[i];
            n = pmove[clientId].physents[tr->ent].info;
            ent = EDICT_NUM(n);

            if (g_playertouch[n / 8] & (1 << (n % 8)))
                continue;

            if (ent->v.flags & FL_SPECTATOR)
                continue;

            trace.allsolid = tr->allsolid;
            trace.startsolid = tr->startsolid;
            trace.inopen = tr->inopen;
            trace.inwater = tr->inwater;
            trace.fraction = tr->fraction;
            VectorCopy(tr->endpos, trace.endpos);
            VectorCopy(tr->plane.normal, trace.plane.normal);
            trace.plane.dist = tr->plane.dist;
            trace.hitgroup = tr->hitgroup;
            trace.ent = ent;

            VectorCopy(tr->deltavelocity, sv_player->v.velocity);
            SV_Impact(ent, sv_player, &trace);
        }

        VectorCopy(oldvel, sv_player->v.velocity);
    }

    func_0028A820();
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/sv_user", SV_RunCmd);
#endif

void func_0028A820(void)
{
    if (!host_client->spectator)
    {
        gGlobalVariables.time = sv.time;
        gEntityInterface.pfnPlayerPostThink(sv_player);
    }
    else
    {
        gGlobalVariables.time = sv.time;
        gEntityInterface.pfnSpectatorThink(sv_player);
    }
}

#ifdef NON_MATCHING
void SV_ExecuteClientMessage(client_t *cl, int clientId)
{
    int c;
    char *s;
    usercmd_t oldest, oldcmd, newcmd;
    usercmd_t nullcmd;
    vec3_t o;
    qboolean move_issued = false;
    int checksumIndex;
    byte checksum, calculatedChecksum;
    int ret;

    if (cl->netchan.incoming_sequence >= cl->netchan.outgoing_sequence)
    {
        cl->netchan.unk_28 += cl->netchan.incoming_sequence - cl->netchan.outgoing_sequence;
        while (cl->netchan.unk_28-- > 0)
        {
            MSG_WriteByte(&cl->netchan.message, svc_choke);
            MSG_WriteByte(&cl->netchan.message, cl->netchan.outgoing_sequence + cl->netchan.unk_28);
        }
        cl->netchan.unk_28 = 0;
        cl->netchan.outgoing_sequence = cl->netchan.incoming_sequence;
    }
    else
    {
        cl->send_message = false;
    }

    host_client = cl;
    sv_player = cl->edict;
    cl->localtime = sv.time;
    cl->delta_sequence = -1;

    while (1)
    {
        if (msg_badread)
        {
            Con_Printf("SV_ReadClientMessage: badread\n");
            SV_DropClient(cl, false);
            return;
        }

        c = MSG_ReadByte();
        if (c == -1)
            break;

        switch (c)
        {
        default:
            Con_Printf("SV_ReadClientMessage: unknown command char\n");
            SV_DropClient(cl, false);
            return;

        case clc_nop:
            break;

        case clc_delta:
            cl->delta_sequence = MSG_ReadByte();
            break;

        case clc_move:
            if (move_issued)
                return;
            move_issued = true;

            memset(&nullcmd, 0, sizeof(nullcmd));

            checksumIndex = msg_readcount;
            checksum = (byte)MSG_ReadByte();

            MSG_ReadDeltaUsercmd(&oldest, &nullcmd);
            MSG_ReadDeltaUsercmd(&oldcmd, &oldest);
            MSG_ReadDeltaUsercmd(&newcmd, &oldcmd);

            if (!cl->active && !cl->spawned)
                break;

            if (!sv.active)
                break;

            calculatedChecksum = COM_BlockSequenceCRCByte(net_message.data + checksumIndex + 1, msg_readcount - checksumIndex - 1, cl->netchan.incoming_sequence);
            if (calculatedChecksum != checksum)
            {
                Con_DPrintf("Failed command checksum for %s (%d != %d)/%d\n", cl->name, calculatedChecksum, checksum, cl->netchan.incoming_sequence);
                return;
            }

            if (sv_showcmd.value != 0)
            {
                Con_Printf("MSEC %i IMP %i Buttons:  %i:%i:%i FWD %i\n", newcmd.msec, newcmd.impulse,
                    newcmd.buttons & IN_ATTACK ? 1 : 0, newcmd.buttons & IN_ATTACK2 ? 1 : 0, newcmd.buttons & IN_JUMP ? 1 : 0,
                    (int)newcmd.forwardmove);
            }

            if (sv.paused || (svs.maxclients < 2 && key_dest != key_game) || (sv_player->v.flags & FL_FROZEN))
            {
                newcmd.msec = 0;
                newcmd.forwardmove = 0;
                newcmd.sidemove = 0;
                newcmd.upmove = 0;
                newcmd.buttons = 0;
                if (sv_player->v.flags & FL_FROZEN)
                    newcmd.impulse = 0;

                oldest = newcmd;
                oldcmd = newcmd;

                if (!(sv_player->v.flags & FL_FROZEN))
                    VectorCopy(host_client->edict->v.v_angle, newcmd.viewangles);
            }
            else
            {
                VectorCopy(newcmd.viewangles, host_client->edict->v.v_angle);
            }

            host_client->edict->v.button = newcmd.buttons;
            if (newcmd.impulse)
                host_client->edict->v.impulse = newcmd.impulse;
            host_client->edict->v.light_level = newcmd.lightlevel;

            SV_PreRunCmd();

            if (net_drop < 16)
            {
                while (net_drop > 2)
                {
                    SV_RunCmd(&cl->lastcmd, clientId);
                    net_drop--;
                }
                if (net_drop > 1)
                    SV_RunCmd(&oldest, clientId);
                if (net_drop > 0)
                    SV_RunCmd(&oldcmd, clientId);
            }
            SV_RunCmd(&newcmd, clientId);

            cl->lastcmd = newcmd;
            break;

        case clc_stringcmd:
            s = MSG_ReadString();
            ret = host_client->unk_73D0 ? 2 : 0;
            if (!Q_strncasecmp(s, "status", 6) ||
                !Q_strncasecmp(s, "god", 3) ||
                !Q_strncasecmp(s, "customrsrclist", 14) ||
                !Q_strncasecmp(s, "notarget", 8) ||
                !Q_strncasecmp(s, "fly", 3) ||
                !Q_strncasecmp(s, "name", 4) ||
                !Q_strncasecmp(s, "noclip", 6) ||
                !Q_strncasecmp(s, "tell", 4) ||
                !Q_strncasecmp(s, "color", 5) ||
                (!Q_strncasecmp(s, "kill", 4) && Q_strncasecmp(s, "killserver", 10)) ||
                !Q_strncasecmp(s, "pause", 5) ||
                !Q_strncasecmp(s, "spawn", 5) ||
                !Q_strncasecmp(s, "new", 3) ||
                !Q_strncasecmp(s, "dropclient", 10) ||
                !Q_strncasecmp(s, "begin", 5) ||
                !Q_strncasecmp(s, "prespawn", 8) ||
                !Q_strncasecmp(s, "kick", 4) ||
                !Q_strncasecmp(s, "ping", 4) ||
                !Q_strncasecmp(s, "download", 8) ||
                !Q_strncasecmp(s, "resourcelist", 11) ||
                !Q_strncasecmp(s, "nextdl", 6) ||
                !Q_strncasecmp(s, "sv_print_custom", 15) ||
                !Q_strncasecmp(s, "ban", 3) ||
                !Q_strncasecmp(s, "ptrack", 6) ||
                !Q_strncasecmp(s, "setinfo", 7))
            {
                ret = 1;
            }
            else if (!Q_strncasecmp(s, "showinfo", 8))
            {
                ret = 1;
            }

            if (ret == 2)
                Cbuf_InsertText(s);
            else if (ret == 1)
                Cmd_ExecuteString(s, src_client);
            else
            {
                Cmd_TokenizeString(s);
                gEntityInterface.pfnClientCommand(host_client->edict);
            }
            break;

        case clc_tmove:
            o[0] = MSG_ReadCoord();
            o[1] = MSG_ReadCoord();
            o[2] = MSG_ReadCoord();
            if (host_client->spectator)
            {
                VectorCopy(o, sv_player->v.origin);
                SV_LinkEdict(sv_player, false);
            }
            break;

        case clc_upload:
            func_00288378();
            break;

        case clc_resourcelist:
            func_00288CE8();
            break;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/sv_user", SV_ExecuteClientMessage);
#endif

void func_0028B188(void)
{
    if (cmd_source == src_command)
    {
        Cmd_ForwardToServer();
        return;
    }

    SV_EndRedirect();

    if (!host_client->spectator)
        SV_BroadcastPrintf("%s dropped\n", host_client->name);

    SV_DropClient(host_client, false);
}

qboolean func_0028B200(int idnum)
{
    client_t *cl;
    int i;

    for (i = 0, cl = svs.clients; i < 3; i++, cl++)
    {
        if (!cl->spawned || !cl->active || !cl->connected)
            continue;

        if (cl->userid == idnum)
        {
            host_client = cl;
            sv_player = cl->edict;
            return true;
        }
    }

    Con_Printf("Userid %i is not on the server\n", idnum);
    return false;
}

void func_0028B2A0(void)
{
    if (Cmd_Argc() != 2)
    {
        Con_Printf("Usage: info <userid>\n");
        return;
    }

    if (!func_0028B200(atoi(Cmd_Argv(1))))
        return;

    Info_Print(host_client->userinfo);
}

void func_0028B308(void)
{
    if (cmd_source == src_command)
    {
        Cmd_ForwardToServer();
        return;
    }

    Info_Print(Info_Serverinfo());
}
