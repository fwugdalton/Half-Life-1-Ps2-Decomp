#include "common.h"
#include "engine/cvar.h"
#include "engine/mathlib.h"
#include "engine/cmd.h"
#include "engine/client.h"
#include "engine/pmove.h"

extern int D_003991B0[2];       /* spec_track */
extern int D_003991B8[2];       /* locked */
extern int D_003991C0[2];       /* autocam */
extern int D_003ACCE8[2];       /* locked */
extern int D_003ACCF0[2];       /* oldbuttons */
extern cvar_t D_00331B28;       /* cl_hightrack */
extern vec3_t D_003ACCD0[2];    /* desired_position */
extern float D_003991A8[2];       /* cam_lastviewtime */
extern float realtime;
extern int cl_updatemask[2];


double func_002D7788(double y, double x); /* atan2 */
double func_002D7DF0(double x);           /* sqrt */
int func_002542F8(int mode);
void MSG_WriteString(sizebuf_t *sb, const char *s);
void MSG_WriteCoord(sizebuf_t *sb, float f);
struct model_s *Mod_ForName(char *name, qboolean crash);
void Mod_TouchModel(struct model_s *mod);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_cam", D_0038A6A8);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_cam", D_0038A6B0);

void func_00252868(vec3_t vec, vec3_t ang)
{
    float forward;
    float yaw, pitch;

    if (vec[1] == 0 && vec[0] == 0)
    {
        yaw = 0;
        if (vec[2] > 0)
            pitch = 90;
        else
            pitch = 270;
    }
    else
    {
        yaw = (int)(func_002D7788(vec[1], vec[0]) * 180 / M_PI);
        if (yaw < 0)
            yaw += 360;

        forward = func_002D7DF0(vec[0] * vec[0] + vec[1] * vec[1]);
        pitch = (int)(func_002D7788(vec[2], forward) * 180 / M_PI);
        if (pitch < 0)
            pitch += 360;
    }

    ang[0] = pitch;
    ang[1] = yaw;
    ang[2] = 0;
}

float func_00252A18(vec3_t v)
{
    return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

void func_00252A40(int clientId)
{
    if (func_002542F8(D_003991C0[clientId]))
    {
        MSG_WriteByte(&cls[clientId].netchan.message, clc_stringcmd);
        MSG_WriteString(&cls[clientId].netchan.message, "ptrack");
        D_003ACCE8[clientId] = 0;
    }
}

void func_00252AD0(int playernum, int clientId)
{
    char st[40];

    sprintf(st, "ptrack %i", playernum);
    MSG_WriteByte(&cls[clientId].netchan.message, clc_stringcmd);
    MSG_WriteString(&cls[clientId].netchan.message, st);
    D_003991B0[clientId] = playernum;
    D_003991B8[clientId] = 1;
    D_003ACCE8[clientId] = 0;
}

pmtrace_t func_00252B80(vec3_t vec1, vec3_t vec2)
{
    VectorCopy(vec1, pmove[0].origin);
    return PM_PlayerTrace(pmove[0].origin, vec2, 0, 0);
}

float func_00252BD8(player_state_t *self, player_state_t *player, vec3_t vec, int checkvis, int clientId)
{
    vec3_t v;
    pmtrace_t trace;
    float len;
    float maxlen;

    maxlen = 800;
    if (D_003991C0[clientId] == 2)
        maxlen = 32;

    func_00252868(vec, v);
    VectorCopy(v, pmove[0].angles);
    VectorNormalize(vec);
    VectorMA(player->origin, maxlen, vec, v);
    trace = func_00252B80(player->origin, v);
    if (trace.inwater)
        return 9999;
    VectorCopy(trace.endpos, vec);
    VectorSubtract(trace.endpos, player->origin, v);
    len = sqrtf(DotProduct(v, v));
    if (len < 8 || len > maxlen)
        return 9999;
    if (checkvis)
    {
        VectorSubtract(trace.endpos, self->origin, v);
        len = sqrtf(DotProduct(v, v));

        trace = func_00252B80(self->origin, vec);
        if (trace.fraction != 1 || trace.inwater)
            return 9999;
    }
    return len;
}

int func_00252DD8(player_state_t *player, vec3_t vec)
{
    pmtrace_t trace;
    vec3_t v;
    float d;

    trace = func_00252B80(player->origin, vec);
    if (trace.fraction != 1 || trace.inwater)
        return 0;
    VectorSubtract(player->origin, vec, v);
    d = func_00252A18(v);
    if (d < 8)
        return 0;
    return 1;
}

int func_00252E98(player_state_t *self, player_state_t *player, int checkvis, int clientId)
{
    float f, max;
    vec3_t vec, vec2;
    vec3_t forward, right, up;

    VectorCopy(player->viewangles, vec);
    vec[0] = 0;
    AngleVectors(vec, forward, right, up);

    max = 1000;
    if (D_003991C0[clientId] != 2 && D_003991C0[clientId] != 3)
    {
        VectorAdd(forward, up, vec2);
        VectorAdd(vec2, right, vec2);
        if ((f = func_00252BD8(self, player, vec2, checkvis, clientId)) < max)
        {
            max = f;
            VectorCopy(vec2, vec);
        }
        VectorAdd(forward, up, vec2);
        VectorSubtract(vec2, right, vec2);
        if ((f = func_00252BD8(self, player, vec2, checkvis, clientId)) < max)
        {
            max = f;
            VectorCopy(vec2, vec);
        }
        VectorAdd(forward, right, vec2);
        if ((f = func_00252BD8(self, player, vec2, checkvis, clientId)) < max)
        {
            max = f;
            VectorCopy(vec2, vec);
        }
        VectorSubtract(forward, right, vec2);
        if ((f = func_00252BD8(self, player, vec2, checkvis, clientId)) < max)
        {
            max = f;
            VectorCopy(vec2, vec);
        }
        VectorAdd(forward, up, vec2);
        if ((f = func_00252BD8(self, player, vec2, checkvis, clientId)) < max)
        {
            max = f;
            VectorCopy(vec2, vec);
        }
        VectorSubtract(forward, up, vec2);
        if ((f = func_00252BD8(self, player, vec2, checkvis, clientId)) < max)
        {
            max = f;
            VectorCopy(vec2, vec);
        }
        VectorAdd(up, right, vec2);
        VectorSubtract(vec2, forward, vec2);
        if ((f = func_00252BD8(self, player, vec2, checkvis, clientId)) < max)
        {
            max = f;
            VectorCopy(vec2, vec);
        }
        VectorSubtract(up, right, vec2);
        VectorSubtract(vec2, forward, vec2);
        if ((f = func_00252BD8(self, player, vec2, checkvis, clientId)) < max)
        {
            max = f;
            VectorCopy(vec2, vec);
        }
        VectorSubtract(vec3_origin, forward, vec2);
        if ((f = func_00252BD8(self, player, vec2, checkvis, clientId)) < max)
        {
            max = f;
            VectorCopy(vec2, vec);
        }
        VectorCopy(forward, vec2);
        if ((f = func_00252BD8(self, player, vec2, checkvis, clientId)) < max)
        {
            max = f;
            VectorCopy(vec2, vec);
        }
        VectorSubtract(vec3_origin, right, vec2);
        if ((f = func_00252BD8(self, player, vec2, checkvis, clientId)) < max)
        {
            max = f;
            VectorCopy(vec2, vec);
        }
        VectorCopy(right, vec2);
        if ((f = func_00252BD8(self, player, vec2, checkvis, clientId)) < max)
        {
            max = f;
            VectorCopy(vec2, vec);
        }
    }

    if (max >= 1000)
    {
        func_00252A40(clientId);
        return 0;
    }
    D_003ACCE8[clientId] = 1;
    VectorCopy(vec, D_003ACCD0[clientId]);
    return 1;
}

void func_00253570(int clientId)
{
    int i, j;
    player_info_t *s;

    j = -1;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        s = &cl[clientId].players[i];
        if (s->name[0] && !s->spectator)
            j = i;
    }
    if (j >= 0)
    {
        if (!D_003ACCE8)
            func_00252AD0(j, clientId);
    }
    else
        func_00252A40(clientId);
}

void func_002535F8(vec3_t ang)
{
    vec3_t src, dst, dir;

    VectorCopy(vec3_origin, src);
    VectorCopy(vec3_origin, dst);
    dst[2] += 800;
    VectorSubtract(src, dst, dir);
    func_00252868(dir, ang);
    ang[0] = -ang[0];
}

void func_00253680(vec3_t in, vec3_t out)
{
    vec3_t v;
    pmtrace_t trace;

    v[0] = in[0];
    v[1] = in[1];
    v[2] = in[2] + 800;
    trace = func_00252B80(in, v);
    if (trace.fraction < 1)
        trace.endpos[2] -= 1;
    VectorSubtract(trace.endpos, in, v);
    VectorScale(v, 0.5f, v);
    VectorAdd(in, v, out);
    if (sqrtf(DotProduct(v, v)) < 32)
    {
        out[0] = in[0];
        out[1] = in[1];
        out[2] = in[2] + 32;
    }
}

#ifdef NON_MATCHING
void func_002537C0(vec3_t viewangles, int clientId)
{
    frame_t *frame, *oldframe;
    player_state_t *player, *oldplayer;
    float f, len, d;
    vec3_t result, vec, vec2;
    int i;

    frame = &cl[clientId].frames[cls[clientId].netchan.incoming_sequence & cl_updatemask[clientId]];
    oldframe = &cl[clientId].frames[(cls[clientId].netchan.incoming_sequence - 1) & cl_updatemask[clientId]];
    player = &frame->playerstate[D_003991B0[clientId]];
    oldplayer = &oldframe->playerstate[D_003991B0[clientId]];

    len = player->msg_time - oldplayer->msg_time;
    if (len == 0 || oldframe->receivedtime == -1 || frame->receivedtime == -1)
    {
        VectorCopy(player->viewangles, viewangles);
        VectorCopy(player->viewangles, viewangles);
        return;
    }

    f = realtime - player->msg_time;
    if (f < 0)
        f = 0;
    f = f / len;
    if (f > 1)
        f = 1;

    VectorCopy(player->viewangles, vec);
    VectorCopy(oldplayer->viewangles, vec2);
    for (i = 0; i < 3; i++)
    {
        if (vec[i] < 0)
            vec[i] += 360;
        if (vec2[i] < 0)
            vec2[i] += 360;
        d = vec[i] - vec2[i];
        while (d > 180)
            d -= 360;
        while (d <= -180)
            d += 360;
        result[i] = vec[i] + f * d;
        cl[clientId].simorg[i] = player->origin[i] + f * (player->origin[i] - oldplayer->origin[i]);
    }
    VectorCopy(result, viewangles);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/cl_cam", func_002537C0);
#endif

void func_00253AA0(usercmd_t *cmd, int clientId)
{
    frame_t *frame;
    player_state_t *player, *self;

    if (!func_002542F8(D_003991C0[clientId]))
        return;
    if (cls[clientId].state != ca_active)
        return;

    if (!cl[clientId].players[D_003991B0[clientId]].name[0] || cl[clientId].players[D_003991B0[clientId]].spectator)
    {
        D_003ACCE8[clientId] = 0;
        if (D_00331B28.value)
            func_00253570(clientId);
        else
            func_00252A40(clientId);
        return;
    }

    frame = &cl[clientId].frames[cls[clientId].netchan.incoming_sequence & cl_updatemask[clientId]];
    player = &frame->playerstate[D_003991B0[clientId]];
    self = &frame->playerstate[cl[clientId].playernum];

    VectorCopy(player->origin, D_003ACCD0[clientId]);
    VectorCopy(D_003ACCD0[clientId], self->origin);

    MSG_WriteByte(&cls[clientId].netchan.message, 6);
    MSG_WriteCoord(&cls[clientId].netchan.message, D_003ACCD0[clientId][0]);
    MSG_WriteCoord(&cls[clientId].netchan.message, D_003ACCD0[clientId][1]);
    MSG_WriteCoord(&cls[clientId].netchan.message, D_003ACCD0[clientId][2]);
}

void func_00253C80(usercmd_t *cmd, int clientId)
{
    frame_t *frame;
    player_state_t *player, *self;
    vec3_t vec;

    if (!func_002542F8(D_003991C0[clientId]))
        return;
    if (cls[clientId].state != ca_active)
        return;

    if (!cl[clientId].players[D_003991B0[clientId]].name[0] || cl[clientId].players[D_003991B0[clientId]].spectator)
    {
        D_003ACCE8[clientId] = 0;
        if (D_00331B28.value)
            func_00253570(clientId);
        else
            func_00252A40(clientId);
        return;
    }

    frame = &cl[clientId].frames[cls[clientId].netchan.incoming_sequence & cl_updatemask[clientId]];
    player = &frame->playerstate[D_003991B0[clientId]];
    self = &frame->playerstate[cl[clientId].playernum];

    VectorCopy(player->origin, vec);
    func_00253680(vec, D_003ACCD0[clientId]);
    VectorCopy(D_003ACCD0[clientId], self->origin);

    MSG_WriteByte(&cls[clientId].netchan.message, 6);
    MSG_WriteCoord(&cls[clientId].netchan.message, D_003ACCD0[clientId][0]);
    MSG_WriteCoord(&cls[clientId].netchan.message, D_003ACCD0[clientId][1]);
    MSG_WriteCoord(&cls[clientId].netchan.message, D_003ACCD0[clientId][2]);
}

void func_00253E80(usercmd_t *cmd, int clientId)
{
    player_state_t *player, *self;
    frame_t *frame;
    vec3_t vec;
    float len;

    if (!cl[clientId].spectator)
        return;

    if (D_003991C0[clientId] == 2)
    {
        func_00253AA0(cmd, clientId);
        return;
    }
    if (D_003991C0[clientId] == 3)
    {
        func_00253C80(cmd, clientId);
        return;
    }

    if (D_00331B28.value && !D_003ACCE8[clientId])
        func_00253570(clientId);

    if (!func_002542F8(D_003991C0[clientId]) || cls[clientId].state != ca_active)
        return;

    if (!cl[clientId].players[D_003991B0[clientId]].name[0] || cl[clientId].players[D_003991B0[clientId]].spectator)
    {
        D_003ACCE8[clientId] = 0;
        if (D_00331B28.value)
            func_00253570(clientId);
        else
            func_00252A40(clientId);
        return;
    }

    frame = &cl[clientId].frames[cls[clientId].netchan.incoming_sequence & cl_updatemask[clientId]];
    player = &frame->playerstate[D_003991B0[clientId]];
    self = &frame->playerstate[cl[clientId].playernum];

    if (!D_003ACCE8[clientId] || !func_00252DD8(player, D_003ACCD0[clientId]))
    {
        if (!D_003ACCE8[clientId] || realtime - D_003991A8[clientId] > 0.1f)
        {
            if (!func_00252E98(self, player, 1, clientId))
                func_00252E98(self, player, 0, clientId);
            D_003991A8[clientId] = realtime;
        }
    }
    else
        D_003991A8[clientId] = realtime;

    if (!func_002542F8(D_003991C0[clientId]) || !D_003ACCE8[clientId])
        return;

    VectorSubtract(D_003ACCD0[clientId], self->origin, vec);
    len = func_00252A18(vec);
    cmd->forwardmove = cmd->sidemove = cmd->upmove = 0;
    if (len > 16)
    {
        MSG_WriteByte(&cls[clientId].netchan.message, 6);
        MSG_WriteCoord(&cls[clientId].netchan.message, D_003ACCD0[clientId][0]);
        MSG_WriteCoord(&cls[clientId].netchan.message, D_003ACCD0[clientId][1]);
        MSG_WriteCoord(&cls[clientId].netchan.message, D_003ACCD0[clientId][2]);
    }

    VectorCopy(D_003ACCD0[clientId], self->origin);

    VectorSubtract(player->origin, D_003ACCD0[clientId], vec);
    func_00252868(vec, cl[clientId].viewangles);
    cl[clientId].viewangles[0] = -cl[clientId].viewangles[0];
    VectorCopy(cl[clientId].viewangles, cl[clientId].simangles);
}

int func_002542F8(int mode)
{
    switch (mode)
    {
    case 0:
        return 0;
    case 1:
        return 1;
    case 2:
        return 1;
    case 3:
        return 1;
    }
    return 0;
}

char *func_00254340(int mode)
{
    switch (mode)
    {
    case 0:
        return "No camera";
    case 1:
        return "Tracking";
    case 2:
        return "1st Person";
    case 3:
        return "Top Down";
    }
    return "Unknown";
}

void func_002543B0(usercmd_t *cmd, int clientId)
{
    int i;
    player_info_t *s;
    int end;

    if (cls[clientId].state != ca_active)
        return;

    if (!cl[clientId].spectator)
        return;

    if (cmd->buttons & IN_ATTACK)
    {
        if (D_003ACCF0[clientId] & IN_ATTACK)
            return;
        D_003ACCF0[clientId] |= IN_ATTACK;
        D_003991C0[clientId] = (D_003991C0[clientId] + 1) % 4;
        Con_Printf("Specator mode set to %s\n", func_00254340(D_003991C0[clientId]));
        if (!func_002542F8(D_003991C0[clientId]))
        {
            func_00252A40(clientId);
            VectorCopy(cl[clientId].viewangles, cmd->viewangles);
            return;
        }
    }
    else
    {
        D_003ACCF0[clientId] &= ~IN_ATTACK;
        if (!func_002542F8(D_003991C0[clientId]))
            return;
    }

    if (cmd->buttons & IN_ATTACK2)
    {
        if (D_003ACCF0[clientId] & IN_ATTACK2)
            return;
        D_003ACCF0[clientId] |= IN_ATTACK2;
        D_003991C0[clientId] = (D_003991C0[clientId] - 1) % 4;
        Con_Printf("Specator mode set to %s\n", func_00254340(D_003991C0[clientId]));
        if (!func_002542F8(D_003991C0[clientId]))
        {
            func_00252A40(clientId);
            VectorCopy(cl[clientId].viewangles, cmd->viewangles);
            return;
        }
    }
    else
    {
        D_003ACCF0[clientId] &= ~IN_ATTACK2;
        if (!func_002542F8(D_003991C0[clientId]))
            return;
    }

    if (func_002542F8(D_003991C0[clientId]) && D_00331B28.value)
    {
        func_00253570(clientId);
        return;
    }

    if (D_003ACCE8[clientId])
    {
        if ((cmd->buttons & IN_JUMP) && (D_003ACCF0[clientId] & IN_JUMP))
            return;

        if (!(cmd->buttons & IN_JUMP))
        {
            D_003ACCF0[clientId] &= ~IN_JUMP;
            return;
        }
        D_003ACCF0[clientId] |= IN_JUMP;
    }

    if (D_003ACCE8[clientId] && func_002542F8(D_003991C0[clientId]))
        end = (D_003991B0[clientId] + 1) % MAX_CLIENTS;
    else
        end = D_003991B0[clientId];
    i = end;
    do
    {
        s = &cl[clientId].players[i];
        if (s->name[0] && !s->spectator)
        {
            func_00252AD0(i, clientId);
            return;
        }
        i = (i + 1) % MAX_CLIENTS;
    } while (i != end);

    i = D_003991B0[clientId];
    s = &cl[clientId].players[i];
    if (s->name[0] && !s->spectator)
    {
        func_00252AD0(i, clientId);
        return;
    }
    Con_Printf("No target found ...\n");
    D_003991C0[clientId] = D_003ACCE8[clientId] = 0;
}

void func_00254838(int clientId)
{
    D_003991C0[clientId] = 0;
    D_003991B0[clientId] = 0;
}

void func_00254860(void)
{
    Cvar_RegisterVariable(&D_00331B28);
}

struct model_s *func_00254880(char *name)
{
    struct model_s *m;

    m = Mod_ForName(name, 1);
    Mod_TouchModel(m);
    return m;
}
