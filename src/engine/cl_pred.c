#include "common.h"
#include "engine/cvar.h"
#include "engine/mathlib.h"
#include "engine/cmd.h"
#include "engine/client.h"

extern int D_003991B0[2];       /* spec_track (cl_cam.c) */
extern int D_003991C0[2];       /* autocam (cl_cam.c) */
extern int cl_updatemask[2];
extern int cl_updatebackup[2];
extern cvar_t pm_nostudio;
extern cvar_t pm_nocomplex;
extern cvar_t pm_worldonly;
extern cvar_t pm_nostucktouch;

void func_00253680(vec3_t in, vec3_t out);

/* CL_PredictMove */
void func_002524F8(int clientId)
{
    frame_t *from;
    vec3_t vec;

    if (cl[clientId].intermission)
        return;

    if (!cl[clientId].validsequence)
        return;

    if (cls[clientId].netchan.outgoing_sequence - cls[clientId].netchan.incoming_sequence >= cl_updatebackup[clientId] - 1)
        return;

    VectorCopy(cl[clientId].viewangles, cl[clientId].simangles);

    from = &cl[clientId].frames[cls[clientId].netchan.incoming_sequence & cl_updatemask[clientId]];

    if (cl[clientId].spectator)
    {
        if (D_003991C0[clientId] == 2)
        {
            VectorCopy(from->playerstate[D_003991B0[clientId]].velocity, cl[clientId].simvel);
            VectorCopy(from->playerstate[D_003991B0[clientId]].origin, cl[clientId].simorg);
            return;
        }
        if (D_003991C0[clientId] == 3)
        {
            func_00253680(from->playerstate[D_003991B0[clientId]].origin, vec);
            VectorCopy(from->playerstate[D_003991B0[clientId]].velocity, cl[clientId].simvel);
            VectorCopy(vec, cl[clientId].simorg);
            return;
        }
    }

    VectorCopy(from->playerstate[cl[clientId].playernum].velocity, cl[clientId].simvel);
    VectorCopy(from->playerstate[cl[clientId].playernum].origin, cl[clientId].simorg);
}

/* CL_InitPrediction */
void func_002527D0(void)
{
    Cvar_RegisterVariable(&pm_nostudio);
    Cvar_RegisterVariable(&pm_nocomplex);
    Cvar_RegisterVariable(&pm_worldonly);
    Cvar_RegisterVariable(&pm_nostucktouch);
}
