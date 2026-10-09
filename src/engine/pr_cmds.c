#include "common.h"

#include "engine/mathlib.h"
#include "engine/model.h"
#include "engine/server.h"

typedef struct
{
    float time;                 /* 0x00 */
    float frametime;            /* 0x04 */
    float force_retouch;        /* 0x08 */
    string_t mapname;           /* 0x0C */
    string_t startspot;         /* 0x10 */
    float deathmatch;           /* 0x14 */
    float coop;                 /* 0x18 */
    float teamplay;             /* 0x1C */
    float serverflags;          /* 0x20 */
    float found_secrets;        /* 0x24 */
    vec3_t v_forward;           /* 0x28 */
    vec3_t v_up;                /* 0x34 */
    vec3_t v_right;             /* 0x40 */
    float trace_allsolid;       /* 0x4C */
    float trace_startsolid;     /* 0x50 */
    float trace_fraction;       /* 0x54 */
    vec3_t trace_endpos;        /* 0x58 */
    vec3_t trace_plane_normal;  /* 0x64 */
    float trace_plane_dist;     /* 0x70 */
    edict_t *trace_ent;         /* 0x74 */
    float trace_inopen;         /* 0x78 */
    float trace_inwater;        /* 0x7C */
    int trace_hitgroup;         /* 0x80 */
    int trace_flags;            /* 0x84 */
} globalvars_t;

typedef struct
{
    int fAllSolid;              /* 0x00 */
    int fStartSolid;            /* 0x04 */
    int fInOpen;                /* 0x08 */
    int fInWater;               /* 0x0C */
    float flFraction;           /* 0x10 */
    vec3_t vecEndPos;           /* 0x14 */
    float flPlaneDist;          /* 0x20 */
    vec3_t vecPlaneNormal;      /* 0x24 */
    edict_t *pHit;              /* 0x30 */
    int iHitgroup;              /* 0x34 */
} TraceResult;

typedef enum
{
    print_console,
    print_center,
    print_chat
} PRINT_TYPE;

#define svc_print 8
#define svc_centerprint 26

extern globalvars_t gGlobalVariables;
extern char *pr_strings;

void Host_Error(const char *error, ...);
float Sys_FloatTime(void);
void SV_LinkEdict(edict_t *ent, qboolean touch_triggers);
int SV_ModelIndex(const char *name);
int ModelFrameCount(model_t *model);
void SV_BroadcastPrintf(const char *fmt, ...);
void MSG_WriteChar(sizebuf_t *sb, int c);
void MSG_WriteString(sizebuf_t *sb, const char *s);
int NUM_FOR_EDICT(const edict_t *e);
void VectorAngles(const float *forward, float *angles);
void SV_StartParticle(const vec3_t org, const vec3_t dir, int color, int count);

void PF_makevectors_I(const float *rgflVector)
{
    AngleVectors(rgflVector, gGlobalVariables.v_forward, gGlobalVariables.v_right, gGlobalVariables.v_up);
}

float PF_Time(void)
{
    return Sys_FloatTime();
}

void PF_setorigin_I(edict_t *e, const float *org)
{
    e->v.origin[0] = org[0];
    e->v.origin[1] = org[1];
    e->v.origin[2] = org[2];
    SV_LinkEdict(e, false);
}

void SetMinMaxSize(edict_t *e, const float *min, const float *max, qboolean rotate)
{
    float *angles;
    vec3_t rmin, rmax;
    float bounds[2][3];
    float xvector[2], yvector[2];
    float a;
    vec3_t base, transformed;
    int i, j, k, l;

    for (i = 0; i < 3; i++)
    {
        if (min[i] > max[i])
            Host_Error("backwards mins/maxs");
    }

    rotate = false;

    if (!rotate)
    {
        VectorCopy(min, rmin);
        VectorCopy(max, rmax);
    }
    else
    {
        angles = e->v.angles;

        a = angles[1] / 180 * M_PI;

        xvector[0] = cosf(a);
        xvector[1] = sinf(a);
        yvector[0] = -sinf(a);
        yvector[1] = cosf(a);

        VectorCopy(min, bounds[0]);
        VectorCopy(max, bounds[1]);

        rmin[0] = rmin[1] = rmin[2] = 9999;
        rmax[0] = rmax[1] = rmax[2] = -9999;

        for (i = 0; i <= 1; i++)
        {
            base[0] = bounds[i][0];
            for (j = 0; j <= 1; j++)
            {
                base[1] = bounds[j][1];
                for (k = 0; k <= 1; k++)
                {
                    base[2] = bounds[k][2];

                    transformed[0] = xvector[0] * base[0] + yvector[0] * base[1];
                    transformed[1] = xvector[1] * base[0] + yvector[1] * base[1];
                    transformed[2] = base[2];

                    for (l = 0; l < 3; l++)
                    {
                        if (transformed[l] < rmin[l])
                            rmin[l] = transformed[l];
                        if (transformed[l] > rmax[l])
                            rmax[l] = transformed[l];
                    }
                }
            }
        }
    }

    VectorCopy(rmin, e->v.mins);
    VectorCopy(rmax, e->v.maxs);
    VectorSubtract(max, min, e->v.size);

    SV_LinkEdict(e, false);
}

void PF_setsize_I(edict_t *e, const float *rgflMin, const float *rgflMax)
{
    SetMinMaxSize(e, rgflMin, rgflMax, false);
}

void PF_setmodel_I(edict_t *e, const char *m)
{
    char **check;
    model_t *mod;
    int i;

    i = 0;
    for (check = sv.model_precache; *check; i++, check++)
    {
        if (!strcasecmp(*check, m))
            break;
    }

    if (!*check)
        Host_Error("no precache: %s\n", m);

    e->v.model = m - pr_strings;
    e->v.modelindex = i;

    mod = sv.models[i];
    if (mod)
        SetMinMaxSize(e, mod->mins, mod->maxs, true);
    else
        SetMinMaxSize(e, vec3_origin, vec3_origin, true);
}

int PF_modelindex(const char *pstr)
{
    return SV_ModelIndex(pstr);
}

int ModelFrames(int modelIndex)
{
    if (modelIndex > 0 && modelIndex < 512)
        return ModelFrameCount(sv.models[modelIndex]);

    Con_DPrintf("Bad sprite index!\n");
    return 1;
}

void PF_bprint(char *s)
{
    SV_BroadcastPrintf("%s", s);
}

void PF_sprint(char *s, int entnum)
{
    client_t *client;

    if (entnum <= 0 || entnum > svs.maxclients)
    {
        Con_Printf("tried to sprint to a non-client\n");
        return;
    }

    client = &svs.clients[entnum - 1];
    MSG_WriteChar(&client->netchan.message, svc_print);
    MSG_WriteString(&client->netchan.message, s);
}

void ServerPrint(const char *szMsg)
{
    Con_Printf("%s", szMsg);
}

void ClientPrintf(edict_t *pEdict, PRINT_TYPE ptype, const char *szMsg)
{
    client_t *client;
    int entnum;

    entnum = NUM_FOR_EDICT(pEdict);
    if (entnum <= 0 || entnum > svs.maxclients)
    {
        Con_Printf("tried to sprint to a non-client\n");
        return;
    }

    client = &svs.clients[entnum - 1];

    switch (ptype)
    {
    case print_center:
        MSG_WriteChar(&client->netchan.message, svc_centerprint);
        MSG_WriteString(&client->netchan.message, szMsg);
        break;

    case print_chat:
    case print_console:
        MSG_WriteByte(&client->netchan.message, svc_print);
        MSG_WriteString(&client->netchan.message, szMsg);
        break;

    default:
        Con_Printf("invalid PRINT_TYPE %i\n", ptype);
        break;
    }
}

float PF_vectoyaw_I(const float *rgflVector)
{
    float yaw;

    if (rgflVector[1] == 0 && rgflVector[0] == 0)
    {
        yaw = 0;
    }
    else
    {
        yaw = (int)(atan2f(rgflVector[1], rgflVector[0]) * (180 / M_PI));
        if (yaw < 0)
            yaw += 360;
    }

    return yaw;
}

void PF_vectoangles_I(const float *rgflVectorIn, float *rgflVectorOut)
{
    VectorAngles(rgflVectorIn, rgflVectorOut);
}

void PF_particle_I(const float *org, const float *dir, float color, float count)
{
    SV_StartParticle(org, dir, color, count);
}

#define SND_SENTENCE (1 << 4)
#define SND_SPAWNING (1 << 8)
#define svc_spawnstaticsound 29
#define CVOXFILESENTENCEMAX 1536

int atoi(const char *s);
void MSG_WriteShort(sizebuf_t *sb, int c);
void MSG_WriteCoord(sizebuf_t *sb, float f);

void PF_ambientsound_I(edict_t *entity, float *pos, const char *samp, float vol, float attenuation, int fFlags, int pitch)
{
    char **check;
    int i;
    int soundnum;
    int ent;
    sizebuf_t *pout;

    if (samp[0] == '!')
    {
        fFlags |= SND_SENTENCE;
        soundnum = atoi(samp + 1);
        if (soundnum >= CVOXFILESENTENCEMAX)
        {
            Con_Printf("invalid sentence number: %s", samp + 1);
            return;
        }
    }
    else
    {
        soundnum = 0;
        for (check = sv.sound_precache; *check; check++, soundnum++)
        {
            if (!strcasecmp(*check, samp))
                break;
        }

        if (!*check)
        {
            Con_Printf("no precache: %s\n", samp);
            return;
        }
    }

    ent = NUM_FOR_EDICT(entity);

    if (fFlags & SND_SPAWNING)
        pout = &sv.signon;
    else
        pout = &sv.datagram;

    MSG_WriteByte(pout, svc_spawnstaticsound);
    for (i = 0; i < 3; i++)
        MSG_WriteCoord(pout, pos[i]);

    MSG_WriteShort(pout, soundnum);
    MSG_WriteByte(pout, vol * 255);
    MSG_WriteByte(pout, attenuation * 64);
    MSG_WriteShort(pout, ent);
    MSG_WriteByte(pout, pitch);
    MSG_WriteByte(pout, (byte)fFlags);
}

void SV_StartSound(edict_t *entity, int channel, const char *sample, int volume, float attenuation, int fFlags, int pitch);

void PF_sound_I(edict_t *entity, int channel, const char *sample, float volume, float attenuation, int fFlags, int pitch)
{
    if (volume < 0 || volume > 255)
        Sys_Error("SV_StartSound: volume = %i", volume);
    if (attenuation < 0 || attenuation > 4)
        Sys_Error("SV_StartSound: attenuation = %f", attenuation);
    if (channel < 0 || channel > 7)
        Sys_Error("SV_StartSound: channel = %i", channel);
    if (pitch < 0 || pitch > 255)
        Sys_Error("SV_StartSound: pitch = %i", pitch);

    SV_StartSound(entity, channel, sample, volume * 255, attenuation, fFlags, pitch);
}

trace_t SV_Move(const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int type, edict_t *passedict, qboolean monsterClipBrush);
trace_t SV_MoveNoEnts(const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int type, edict_t *passedict, qboolean monsterClipBrush);
void SV_SetGlobalTrace(trace_t *ptrace);

void PF_traceline_Shared(const float *v1, const float *v2, int nomonsters, edict_t *ent, qboolean noents)
{
    trace_t trace;

    if (noents)
        trace = SV_MoveNoEnts(v1, vec3_origin, vec3_origin, v2, nomonsters, ent, false);
    else
        trace = SV_Move(v1, vec3_origin, vec3_origin, v2, nomonsters, ent, false);

    gGlobalVariables.trace_flags = 0;
    SV_SetGlobalTrace(&trace);
}

void PF_traceline_DLL(const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr)
{
    PF_traceline_Shared(v1, v2, fNoMonsters, pentToSkip ? pentToSkip : sv.edicts, false);
    ptr->fAllSolid = (int)gGlobalVariables.trace_allsolid;
    ptr->fStartSolid = (int)gGlobalVariables.trace_startsolid;
    ptr->fInOpen = (int)gGlobalVariables.trace_inopen;
    ptr->fInWater = (int)gGlobalVariables.trace_inwater;
    ptr->flFraction = gGlobalVariables.trace_fraction;
    ptr->flPlaneDist = gGlobalVariables.trace_plane_dist;
    ptr->pHit = gGlobalVariables.trace_ent;
    ptr->vecEndPos[0] = gGlobalVariables.trace_endpos[0];
    ptr->vecEndPos[1] = gGlobalVariables.trace_endpos[1];
    ptr->vecEndPos[2] = gGlobalVariables.trace_endpos[2];
    ptr->vecPlaneNormal[0] = gGlobalVariables.trace_plane_normal[0];
    ptr->vecPlaneNormal[1] = gGlobalVariables.trace_plane_normal[1];
    ptr->vecPlaneNormal[2] = gGlobalVariables.trace_plane_normal[2];
    ptr->iHitgroup = gGlobalVariables.trace_hitgroup;
}

void PF_tracelineNoEnts_DLL(const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr)
{
    PF_traceline_Shared(v1, v2, fNoMonsters, pentToSkip ? pentToSkip : sv.edicts, true);
    ptr->fAllSolid = (int)gGlobalVariables.trace_allsolid;
    ptr->fStartSolid = (int)gGlobalVariables.trace_startsolid;
    ptr->fInOpen = (int)gGlobalVariables.trace_inopen;
    ptr->fInWater = (int)gGlobalVariables.trace_inwater;
    ptr->flFraction = gGlobalVariables.trace_fraction;
    ptr->flPlaneDist = gGlobalVariables.trace_plane_dist;
    ptr->pHit = gGlobalVariables.trace_ent;
    ptr->vecEndPos[0] = gGlobalVariables.trace_endpos[0];
    ptr->vecEndPos[1] = gGlobalVariables.trace_endpos[1];
    ptr->vecEndPos[2] = gGlobalVariables.trace_endpos[2];
    ptr->vecPlaneNormal[0] = gGlobalVariables.trace_plane_normal[0];
    ptr->vecPlaneNormal[1] = gGlobalVariables.trace_plane_normal[1];
    ptr->vecPlaneNormal[2] = gGlobalVariables.trace_plane_normal[2];
    ptr->iHitgroup = gGlobalVariables.trace_hitgroup;
}

extern vec3_t gHullMins[4];
extern vec3_t gHullMaxs[4];

void TraceHull(const float *v1, const float *v2, int fNoMonsters, int hullNumber, edict_t *pentToSkip, TraceResult *ptr)
{
    trace_t trace;

    if (hullNumber < 0 || hullNumber > 3)
        hullNumber = 0;

    trace = SV_Move(v1, gHullMins[hullNumber], gHullMaxs[hullNumber], v2, fNoMonsters, pentToSkip, false);

    ptr->fAllSolid = trace.allsolid;
    ptr->fStartSolid = trace.startsolid;
    ptr->fInOpen = trace.inopen;
    ptr->fInWater = trace.inwater;
    ptr->flFraction = trace.fraction;
    ptr->flPlaneDist = trace.plane.dist;
    ptr->pHit = trace.ent;
    ptr->iHitgroup = trace.hitgroup;
    ptr->vecEndPos[0] = trace.endpos[0];
    ptr->vecEndPos[1] = trace.endpos[1];
    ptr->vecEndPos[2] = trace.endpos[2];
    ptr->vecPlaneNormal[0] = trace.plane.normal[0];
    ptr->vecPlaneNormal[1] = trace.plane.normal[1];
    ptr->vecPlaneNormal[2] = trace.plane.normal[2];
}

void TraceSphere(const float *v1, const float *v2, int fNoMonsters, float radius, edict_t *pentToSkip, TraceResult *ptr)
{
    Sys_Error("TraceSphere not yet implemented!\n");
}

#define SOLID_BSP 4
#define MOVETYPE_PUSH 7

trace_t SV_ClipMoveToEntity(edict_t *ent, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end);

void TraceModel(const float *v1, const float *v2, int hullNumber, edict_t *pent, TraceResult *ptr)
{
    int oldMovetype, oldSolid;
    model_t *pmodel;
    trace_t trace;

    oldSolid = 0;
    oldMovetype = 0;

    if (hullNumber < 0 || hullNumber > 3)
        hullNumber = 0;

    pmodel = sv.models[pent->v.modelindex];
    if (pmodel && pmodel->type == mod_brush)
    {
        oldSolid = pent->v.solid;
        oldMovetype = pent->v.movetype;
        pent->v.solid = SOLID_BSP;
        pent->v.movetype = MOVETYPE_PUSH;
    }

    trace = SV_ClipMoveToEntity(pent, v1, gHullMins[hullNumber], gHullMaxs[hullNumber], v2);

    if (pmodel && pmodel->type == mod_brush)
    {
        pent->v.solid = oldSolid;
        pent->v.movetype = oldMovetype;
    }

    ptr->fAllSolid = trace.allsolid;
    ptr->fStartSolid = trace.startsolid;
    ptr->fInOpen = trace.inopen;
    ptr->fInWater = trace.inwater;
    ptr->flFraction = trace.fraction;
    ptr->flPlaneDist = trace.plane.dist;
    ptr->pHit = trace.ent;
    ptr->iHitgroup = trace.hitgroup;
    ptr->vecEndPos[0] = trace.endpos[0];
    ptr->vecEndPos[1] = trace.endpos[1];
    ptr->vecEndPos[2] = trace.endpos[2];
    ptr->vecPlaneNormal[0] = trace.plane.normal[0];
    ptr->vecPlaneNormal[1] = trace.plane.normal[1];
    ptr->vecPlaneNormal[2] = trace.plane.normal[2];
}

msurface_t *SurfaceAtPoint(model_t *pModel, mnode_t *node, vec_t *start, vec_t *end)
{
    mplane_t *plane;
    int side;
    msurface_t *surf;
    int s, t, ds, dt;
    vec3_t mid;
    float back, front, frac;
    int i;

    if (node->contents < 0)
        return NULL;

    plane = node->plane;
    front = DotProduct(start, plane->normal) - plane->dist;
    back = DotProduct(end, plane->normal) - plane->dist;
    side = front < 0;

    if ((back < 0) == side)
        return SurfaceAtPoint(pModel, node->children[side], start, end);

    frac = front / (front - back);
    mid[0] = start[0] + (end[0] - start[0]) * frac;
    mid[1] = start[1] + (end[1] - start[1]) * frac;
    mid[2] = start[2] + (end[2] - start[2]) * frac;

    surf = SurfaceAtPoint(pModel, node->children[side], start, mid);
    if (surf)
        return surf;

    if ((back < 0) == side)
        return NULL;

    surf = pModel->surfaces + node->firstsurface;
    for (i = 0; i < node->numsurfaces; i++, surf++)
    {
        s = DotProduct(mid, surf->vecs[0]) + surf->vecs[0][3];
        t = DotProduct(mid, surf->vecs[1]) + surf->vecs[1][3];

        if (s < surf->texturemins[0] || t < surf->texturemins[1])
            continue;

        ds = s - surf->texturemins[0];
        dt = t - surf->texturemins[1];

        if (ds > surf->extents[0] || dt > surf->extents[1])
            continue;

        return surf;
    }

    return SurfaceAtPoint(pModel, node->children[!side], mid, end);
}

hull_t *SV_HullForBsp(edict_t *ent, const vec3_t mins, const vec3_t maxs, vec3_t offset);

const char *TraceTexture(edict_t *pTextureEntity, const float *v1, const float *v2)
{
    int firstnode;
    model_t *pmodel;
    hull_t *phull;
    msurface_t *psurf;
    vec3_t start, end;
    vec3_t offset;
    vec3_t forward, right, up;
    vec3_t temp;

    firstnode = 0;
    if (pTextureEntity)
    {
        pmodel = sv.models[pTextureEntity->v.modelindex];
        if (!pmodel || pmodel->type != mod_brush)
            return NULL;

        phull = SV_HullForBsp(pTextureEntity, vec3_origin, vec3_origin, offset);
        VectorSubtract(v1, offset, start);
        VectorSubtract(v2, offset, end);
        firstnode = phull->firstclipnode;

        if (pTextureEntity->v.angles[0] || pTextureEntity->v.angles[1] || pTextureEntity->v.angles[2])
        {
            AngleVectors(pTextureEntity->v.angles, forward, right, up);

            VectorCopy(start, temp);
            start[0] = DotProduct(temp, forward);
            start[1] = -DotProduct(temp, right);
            start[2] = DotProduct(temp, up);

            VectorCopy(end, temp);
            end[0] = DotProduct(temp, forward);
            end[1] = -DotProduct(temp, right);
            end[2] = DotProduct(temp, up);
        }
    }
    else
    {
        pmodel = sv.worldmodel;
        VectorCopy(v1, start);
        VectorCopy(v2, end);
    }

    if (!pmodel || pmodel->type != mod_brush || !pmodel->nodes)
        return NULL;

    psurf = SurfaceAtPoint(pmodel, &pmodel->nodes[firstnode], start, end);
    if (!psurf)
        return NULL;

    return psurf->texture->name;
}

trace_t SV_Trace_Toss(edict_t *ent, edict_t *ignore);

void PF_TraceToss_Shared(edict_t *ent, edict_t *ignore)
{
    trace_t trace;

    trace = SV_Trace_Toss(ent, ignore);
    SV_SetGlobalTrace(&trace);
}

void SV_SetGlobalTrace(trace_t *ptrace)
{
    gGlobalVariables.trace_allsolid = (unsigned int)ptrace->allsolid;
    gGlobalVariables.trace_startsolid = (unsigned int)ptrace->startsolid;
    gGlobalVariables.trace_fraction = ptrace->fraction;
    gGlobalVariables.trace_inwater = (unsigned int)ptrace->inwater;
    gGlobalVariables.trace_inopen = (unsigned int)ptrace->inopen;
    VectorCopy(ptrace->endpos, gGlobalVariables.trace_endpos);
    VectorCopy(ptrace->plane.normal, gGlobalVariables.trace_plane_normal);
    gGlobalVariables.trace_plane_dist = ptrace->plane.dist;
    if (ptrace->ent)
        gGlobalVariables.trace_ent = ptrace->ent;
    else
        gGlobalVariables.trace_ent = sv.edicts;
    gGlobalVariables.trace_hitgroup = ptrace->hitgroup;
}

void PF_TraceToss_DLL(edict_t *pent, edict_t *pentToIgnore, TraceResult *ptr)
{
    PF_TraceToss_Shared(pent, pentToIgnore ? pentToIgnore : sv.edicts);
    ptr->fAllSolid = (int)gGlobalVariables.trace_allsolid;
    ptr->fStartSolid = (int)gGlobalVariables.trace_startsolid;
    ptr->fInOpen = (int)gGlobalVariables.trace_inopen;
    ptr->fInWater = (int)gGlobalVariables.trace_inwater;
    ptr->flFraction = gGlobalVariables.trace_fraction;
    ptr->flPlaneDist = gGlobalVariables.trace_plane_dist;
    ptr->pHit = gGlobalVariables.trace_ent;
    ptr->vecEndPos[0] = gGlobalVariables.trace_endpos[0];
    ptr->vecEndPos[1] = gGlobalVariables.trace_endpos[1];
    ptr->vecEndPos[2] = gGlobalVariables.trace_endpos[2];
    ptr->vecPlaneNormal[0] = gGlobalVariables.trace_plane_normal[0];
    ptr->vecPlaneNormal[1] = gGlobalVariables.trace_plane_normal[1];
    ptr->vecPlaneNormal[2] = gGlobalVariables.trace_plane_normal[2];
    ptr->iHitgroup = gGlobalVariables.trace_hitgroup;
}

#define FL_MONSTERCLIP (1 << 23)

int TraceMonsterHull(edict_t *pEdict, const float *v1, const float *v2, int fNoMonsters, edict_t *pentToSkip, TraceResult *ptr)
{
    qboolean monsterClip;
    trace_t trace;

    monsterClip = (pEdict->v.flags & FL_MONSTERCLIP) > 0;
    trace = SV_Move(v1, pEdict->v.mins, pEdict->v.maxs, v2, fNoMonsters, pentToSkip, monsterClip);
    if (ptr)
    {
        ptr->fAllSolid = trace.allsolid;
        ptr->fStartSolid = trace.startsolid;
        ptr->fInOpen = trace.inopen;
        ptr->fInWater = trace.inwater;
        ptr->flFraction = trace.fraction;
        ptr->flPlaneDist = trace.plane.dist;
        ptr->pHit = trace.ent;
        ptr->iHitgroup = trace.hitgroup;
        ptr->vecEndPos[0] = trace.endpos[0];
        ptr->vecEndPos[1] = trace.endpos[1];
        ptr->vecEndPos[2] = trace.endpos[2];
        ptr->vecPlaneNormal[0] = trace.plane.normal[0];
        ptr->vecPlaneNormal[1] = trace.plane.normal[1];
        ptr->vecPlaneNormal[2] = trace.plane.normal[2];
    }

    if (trace.allsolid || trace.fraction != 1.0f)
        return true;
    return false;
}

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", PF_newcheckclient);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", PF_checkclient_I);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", PVSNode);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", PVSMark);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", PVSFindEntities);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", ValidCmd);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", PF_stuffcmd_I);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", PF_localcmd_I);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", PF_localexec_I);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", FindEntityInSphere);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00274288);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_002742A8);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00274340);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00274360);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00274468);

/* strings de func_00274468 compartilhadas com func_00275898: o splat não as migra */
__asm__(".section .rodata\n"
        ".align 3\n.globl D_0038DB10\nD_0038DB10: .asciz \"model\"\n"
        ".align 3\n.globl D_0038DB18\nD_0038DB18: .asciz \"viewmodel\"\n"
        ".align 3\n.globl D_0038DB28\nD_0038DB28: .asciz \"weaponmodel\"\n"
        ".align 3\n.globl D_0038DB38\nD_0038DB38: .asciz \"netname\"\n"
        ".align 3\n.globl D_0038DB40\nD_0038DB40: .asciz \"target\"\n"
        ".align 3\n.globl D_0038DB48\nD_0038DB48: .asciz \"targetname\"\n"
        ".align 3\n.globl D_0038DB58\nD_0038DB58: .asciz \"message\"\n"
        ".align 3\n.globl D_0038DB60\nD_0038DB60: .asciz \"noise\"\n"
        ".align 3\n.globl D_0038DB68\nD_0038DB68: .asciz \"noise1\"\n"
        ".align 3\n.globl D_0038DB70\nD_0038DB70: .asciz \"noise2\"\n"
        ".align 3\n.globl D_0038DB78\nD_0038DB78: .asciz \"noise3\"\n"
        ".align 3\n.globl D_0038DB80\nD_0038DB80: .asciz \"globalname\"\n"
        ".section .text\n");

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00274608);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00274680);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00274750);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00274788);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_002748D8);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_002749F8);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00274B08);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00274B40);

__asm__(".section .rodata\n.align 3\n.globl D_0038DD48\nD_0038DD48: .asciz \"\"\n.section .text\n");

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00274B88);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00274C00);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00274C20);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00274CA8);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00274D80);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00274E70);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00274F50);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00274FD8);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_002750E0);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00275100);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00275120);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_002754A8);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_002755A0);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00275698);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00275748);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00275898);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00275A98);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00275BF8);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00275CD8);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00275DF0);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00276030);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00276078);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_002760C0);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00276108);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00276150);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00276198);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_002761E0);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00276228);

void func_00276270(void) {
}

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00276278);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00276338);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00276380);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00276420);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00276468);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00276478);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_002765E0);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", RandomFloat);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", RandomLong);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00276710);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_002767F0);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_002768A8);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00276918);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00276998);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00276A58);

INCLUDE_ASM("asm/nonmatchings/engine/pr_cmds", func_00276A68);
