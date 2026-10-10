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

INCLUDE_ASM("asm/nonmatchings/engine/sv_user", SV_ExecuteClientMessage);

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
