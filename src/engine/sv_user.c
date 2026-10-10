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

INCLUDE_ASM("asm/nonmatchings/engine/sv_user", SV_RunCmd);

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
