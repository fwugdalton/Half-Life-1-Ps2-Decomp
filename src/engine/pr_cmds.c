#include "common.h"

#include "engine/mathlib.h"
#include "engine/model.h"
#include "engine/server.h"
#include "engine/progdefs.h"
#include "engine/cvar.h"
#include "engine/decals.h"
#include "stdarg.h"

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

extern byte gMsgData[];

vec3_t gHullMins[4] = {
    {0.0f, 0.0f, 0.0f},
    {-16.0f, -16.0f, -36.0f},
    {-32.0f, -32.0f, -32.0f},
    {-16.0f, -16.0f, -18.0f},
};
vec3_t gHullMaxs[4] = {
    {0.0f, 0.0f, 0.0f},
    {16.0f, 16.0f, 36.0f},
    {32.0f, 32.0f, 32.0f},
    {16.0f, 16.0f, 18.0f},
};
sizebuf_t gMsgBuffer = {false, false, gMsgData, 250, 0};
edict_t *gMsgEntity = NULL;
int gMsgDest = 0;
int gMsgType = 0;
qboolean gMsgStarted = false;
int idum = 0;
int iy_ran1 = 0;

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

#define FL_NOTARGET (1 << 7)
#define MOVETYPE_FOLLOW 12

extern byte checkpvs[1024];
extern int c_notvis;
extern int c_invis;

mleaf_t *Mod_PointInLeaf(vec3_t p, model_t *model);
byte *Mod_LeafPVS(mleaf_t *leaf, model_t *model);

int PF_newcheckclient(int check)
{
    int i;
    byte *pvs;
    edict_t *ent;
    mleaf_t *leaf;
    vec3_t org;

    if (check < 1)
        check = 1;
    if (check > svs.maxclients)
        check = svs.maxclients;

    if (check == svs.maxclients)
        i = 1;
    else
        i = check + 1;

    for (;; i++)
    {
        if (i == svs.maxclients + 1)
            i = 1;

        ent = &sv.edicts[i];

        if (i == check)
            break;
        if (ent->free)
            continue;
        if (!ent->pvPrivateData)
            continue;
        if (ent->v.flags & FL_NOTARGET)
            continue;

        break;
    }

    VectorAdd(ent->v.origin, ent->v.view_ofs, org);
    leaf = Mod_PointInLeaf(org, sv.worldmodel);
    pvs = Mod_LeafPVS(leaf, sv.worldmodel);
    memcpy(checkpvs, pvs, (sv.worldmodel->numleafs + 7) >> 3);

    return i;
}

edict_t *PF_checkclient_I(edict_t *pEdict)
{
    edict_t *ent;
    mleaf_t *leaf;
    int l;
    vec3_t view;

    if (sv.time - sv.lastchecktime >= 0.1f)
    {
        sv.lastcheck = PF_newcheckclient(sv.lastcheck);
        sv.lastchecktime = sv.time;
    }

    ent = &sv.edicts[sv.lastcheck];
    if (ent->free || !ent->pvPrivateData)
        return sv.edicts;

    VectorAdd(pEdict->v.origin, pEdict->v.view_ofs, view);
    leaf = Mod_PointInLeaf(view, sv.worldmodel);
    l = (leaf - sv.worldmodel->leafs) - 1;
    if (l < 0 || !(checkpvs[l >> 3] & (1 << (l & 7))))
    {
        c_notvis++;
        return sv.edicts;
    }

    c_invis++;
    return ent;
}

extern short r_visframecount[2];

mnode_t *PVSNode(mnode_t *node, vec_t *emins, vec_t *emaxs, int clientId)
{
    mplane_t *splitplane;
    int sides;
    mnode_t *splitNode;

    if (node->visframe[clientId] != r_visframecount[clientId])
        return NULL;

    if (node->contents < 0)
        return node->contents != CONTENTS_SOLID ? node : NULL;

    splitplane = node->plane;
    if (splitplane->type < 3)
    {
        if (splitplane->dist > emins[splitplane->type])
        {
            if (splitplane->dist >= emaxs[splitplane->type])
                sides = 2;
            else
                sides = 3;
        }
        else
        {
            sides = 1;
        }
    }
    else
    {
        sides = BoxOnPlaneSide(emins, emaxs, splitplane);
    }

    splitNode = NULL;
    if (sides & 1)
        splitNode = PVSNode(node->children[0], emins, emaxs, clientId);

    if (splitNode)
        return splitNode;

    if (sides & 2)
        splitNode = PVSNode(node->children[1], emins, emaxs, clientId);

    return splitNode;
}

void PVSMark(model_t *pmodel, byte *ppvs, int clientId)
{
    int i;
    mnode_t *node;

    r_visframecount[clientId]++;
    for (i = 0; i < pmodel->numleafs; i++)
    {
        if (!((ppvs[i >> 3] >> (i & 7)) & 1))
            continue;

        node = (mnode_t *)&pmodel->leafs[i + 1];
        do
        {
            if (node->visframe[clientId] == r_visframecount[clientId])
                break;
            node->visframe[clientId] = r_visframecount[clientId];
            node = node->parent;
        } while (node);
    }
}

typedef struct
{
    byte pad0[0x13EC];
    model_t *worldmodel;        /* 0x13EC */
    byte pad13F0[0x2128 - 0x13F0];
    int unk_2128;               /* 0x2128 */
} client_state_t;

extern client_state_t cl[2];
extern mleaf_t *r_oldviewleaf;
void R_MarkLeaves(int clientId);

edict_t *PVSFindEntities(edict_t *pplayer)
{
    int i;
    edict_t *pent;
    edict_t *pchain;
    edict_t *pentPVS;
    vec3_t org;
    byte *ppvs;
    mleaf_t *pleaf;

    VectorAdd(pplayer->v.origin, pplayer->v.view_ofs, org);
    pleaf = Mod_PointInLeaf(org, sv.worldmodel);
    ppvs = Mod_LeafPVS(pleaf, sv.worldmodel);
    PVSMark(sv.worldmodel, ppvs, 0);
    pchain = sv.edicts;

    for (i = 1; i < sv.num_edicts; i++)
    {
        pent = &sv.edicts[i];
        if (pent->free)
            continue;

        pentPVS = pent;
        if (pent->v.movetype == MOVETYPE_FOLLOW && pent->v.aiment)
            pentPVS = pent->v.aiment;

        if (PVSNode(sv.worldmodel->nodes, pentPVS->v.absmin, pentPVS->v.absmax, 0))
        {
            pent->v.chain = pchain;
            pchain = pent;
        }
    }

    if (cl[0].worldmodel)
    {
        r_oldviewleaf = NULL;
        R_MarkLeaves(0);
    }

    return pchain;
}

qboolean ValidCmd(const char *pCmd)
{
    int len;

    len = strlen(pCmd);
    if (len && (pCmd[len - 1] == '\n' || pCmd[len - 1] == ';'))
        return true;
    return false;
}

int vsprintf(char *, const char *, va_list);
void Host_ClientCommands(const char *fmt, ...);
void Cbuf_Execute(void);

extern char szOut_stuffcmd[1024];

void PF_stuffcmd_I(edict_t *pEdict, const char *szFmt, ...)
{
    int entnum;
    client_t *old;
    va_list argptr;

    entnum = NUM_FOR_EDICT(pEdict);
    va_start(argptr, szFmt);
    vsprintf(szOut_stuffcmd, szFmt, argptr);
    va_end(argptr);

    if (entnum < 1 || entnum > svs.maxclients)
    {
        Con_Printf("\n!!!\n\nStuffCmd:  Some entity tried to stuff '%s' to console buffer of entity %i when maxclients was set to %i, ignoring\n\n",
                   szOut_stuffcmd, entnum, svs.maxclients);
    }
    else
    {
        if (ValidCmd(szOut_stuffcmd))
        {
            old = host_client;
            host_client = &svs.clients[entnum - 1];
            Host_ClientCommands("%s", szOut_stuffcmd);
            host_client = old;
        }
        else
        {
            Con_Printf("Tried to stuff bad command %s\n", szOut_stuffcmd);
        }
    }
}

void PF_localcmd_I(char *str)
{
    if (ValidCmd(str))
        Cbuf_AddText(str);
    else
        Con_Printf("Error, bad server command %s\n", str);
}

void PF_localexec_I(void)
{
    Cbuf_Execute();
}

edict_t *FindEntityInSphere(edict_t *pEdictStartSearchAfter, const float *org, float rad)
{
    edict_t *ent;
    float eorg;
    float distSquared;
    int i, j, e;

    if (pEdictStartSearchAfter)
        e = NUM_FOR_EDICT(pEdictStartSearchAfter);
    else
        e = 0;

    rad = rad * rad;

    for (i = e + 1; i < sv.num_edicts; i++)
    {
        ent = &sv.edicts[i];
        if (ent->free || !ent->v.classname)
            continue;

        if (i <= svs.maxclients && !svs.clients[i - 1].active)
            continue;

        distSquared = 0;
        for (j = 0; j < 3 && distSquared <= rad; j++)
        {
            if (org[j] < ent->v.absmin[j])
                eorg = org[j] - ent->v.absmin[j];
            else if (org[j] > ent->v.absmax[j])
                eorg = org[j] - ent->v.absmax[j];
            else
                eorg = 0;

            distSquared += eorg * eorg;
        }

        if (distSquared > rad)
            continue;

        return ent;
    }

    return sv.edicts;
}

edict_t *ED_Alloc(void);
void ED_Free(edict_t *ed);

edict_t *PF_Spawn_I(void)
{
    return ED_Alloc();
}


edict_t *CreateNamedEntity(int className)
{
    edict_t *pedict;
    ENTITYINIT pEntityInit;

    if (!className)
        Sys_Error("Spawned a NULL entity!");

    pedict = ED_Alloc();
    pedict->v.classname = className;
    pEntityInit = GetEntityInit(&pr_strings[className]);
    if (pEntityInit)
    {
        pEntityInit(&pedict->v);
    }
    else
    {
        ED_Free(pedict);
        pedict = NULL;
        Con_DPrintf("Can't create entity: %s\n", &pr_strings[className]);
    }

    return pedict;
}

void PF_Remove_I(edict_t *ed)
{
    ED_Free(ed);
}

edict_t *PF_find_Shared(int eStartSearchAfter, int iFieldToMatch, const char *szValueToFind)
{
    int e;
    edict_t *ed;
    char *t;

    for (e = eStartSearchAfter + 1; e < sv.num_edicts; e++)
    {
        ed = &sv.edicts[e];
        if (ed->free)
            continue;

        t = &pr_strings[*(string_t *)((byte *)&ed->v + iFieldToMatch)];
        if (t == NULL || t == pr_strings)
            continue;

        if (!strcmp(t, szValueToFind))
            return ed;
    }

    return sv.edicts;
}

void Q_strlwr(char *s);

int iGetIndex(const char *pszField)
{
    char sz[512];
    entvars_t tempvars;
    void *p;

    strcpy(sz, pszField);
    Q_strlwr(sz);

    if (!strcmp(sz, "classname"))
        p = &tempvars.classname;
    else if (!strcmp(sz, "model"))
        p = &tempvars.model;
    else if (!strcmp(sz, "viewmodel"))
        p = &tempvars.viewmodel;
    else if (!strcmp(sz, "weaponmodel"))
        p = &tempvars.weaponmodel;
    else if (!strcmp(sz, "netname"))
        p = &tempvars.netname;
    else if (!strcmp(sz, "target"))
        p = &tempvars.target;
    else if (!strcmp(sz, "targetname"))
        p = &tempvars.targetname;
    else if (!strcmp(sz, "message"))
        p = &tempvars.message;
    else if (!strcmp(sz, "noise"))
        p = &tempvars.noise;
    else if (!strcmp(sz, "noise1"))
        p = &tempvars.noise1;
    else if (!strcmp(sz, "noise2"))
        p = &tempvars.noise2;
    else if (!strcmp(sz, "noise3"))
        p = &tempvars.noise3;
    else if (!strcmp(sz, "globalname"))
        p = &tempvars.globalname;
    else
        return -1;

    return (int)p - (int)&tempvars;
}


edict_t *FindEntityByString(edict_t *pEdictStartSearchAfter, const char *pszField, const char *pszValue)
{
    int iField;
    int e;

    iField = iGetIndex(pszField);
    if (iField == -1)
        return NULL;

    if (!pszValue)
        return NULL;

    if (pEdictStartSearchAfter)
        e = NUM_FOR_EDICT(pEdictStartSearchAfter);
    else
        e = 0;

    return PF_find_Shared(e, iField, pszValue);
}

typedef struct
{
    int r, g, b, a;
} colorVec;

typedef struct
{
    byte pad0[0xE8];
    colorVec cvFloorColor;      /* 0xE8 */
    byte padF8[0x1A0 - 0xF8];
} cl_entity_t; /* 0x1A0 */

extern cl_entity_t *cl_entities;

int GetEntityIllum(edict_t *pEnt)
{
    int iNumber;
    colorVec cvFloorColor;

    if (!pEnt)
        return -1;

    iNumber = NUM_FOR_EDICT(pEnt);
    if (iNumber <= svs.maxclients)
        return pEnt->v.light_level;

    if (cls[0].state != 3 && cls[0].state != 4 && cls[0].state != 5)
        return 0x80;

    cvFloorColor = cl_entities[iNumber].cvFloorColor;
    return (cvFloorColor.r + cvFloorColor.g + cvFloorColor.b) / 3;
}

void PR_CheckEmptyString(const char *s)
{
    if (s[0] <= ' ')
        Host_Error("Bad string: %s", s);
}

#define ss_loading 1

int PF_precache_sound_I(char *s)
{
    int i;

    if (s && s[0] == '!')
    {
        Host_Error("PF_precache_sound_I: '%s' do not precache sentence names!", s);
        return 0;
    }

    if (sv.state != ss_loading)
    {
        for (i = 0; i < 512; i++)
        {
            if (sv.sound_precache[i] && !strcasecmp(sv.sound_precache[i], s))
                return i;
        }

        Host_Error("PF_precache_sound_I: '%s' Precache can only be done in spawn functions", s);
    }

    PR_CheckEmptyString(s);
    sv.sound_precache_hashedlookup_built = false;

    for (i = 0; i < 512; i++)
    {
        if (!sv.sound_precache[i])
        {
            sv.sound_precache[i] = s;
            return i;
        }

        if (!strcasecmp(sv.sound_precache[i], s))
            return i;
    }

    Host_Error("PF_precache_sound_I: '%s' overflow", s);
    return 0;
}

model_t *Mod_ForName(const char *name, qboolean crash);

int PF_precache_model_I(char *s)
{
    int i;

    if (sv.state != ss_loading)
    {
        for (i = 0; i < 512; i++)
        {
            if (sv.model_precache[i] && !strcasecmp(sv.model_precache[i], s))
                return i;
        }

        Host_Error("PF_precache_model_I: '%s' Precache can only be done in spawn functions", s);
    }

    PR_CheckEmptyString(s);

    for (i = 0; i < 512; i++)
    {
        if (!sv.model_precache[i])
        {
            sv.model_precache[i] = s;
            sv.models[i] = Mod_ForName(s, true);
            return i;
        }

        if (!strcasecmp(sv.model_precache[i], s))
            return i;
    }

    Host_Error("PF_precache_model_I: '%s' overflow", s);
    return 0;
}

int PF_precache_generic_I(char *s)
{
    int i;

    if (sv.state != ss_loading)
    {
        for (i = 0; i < 512; i++)
        {
            if (sv.generic_precache[i] && !strcmp(sv.generic_precache[i], s))
                return i;
        }

        Host_Error("PF_precache_generic_I: '%s' Precache can only be done in spawn functions", s);
    }

    PR_CheckEmptyString(s);

    for (i = 0; i < 512; i++)
    {
        if (!sv.generic_precache[i])
        {
            sv.generic_precache[i] = s;
            return i;
        }

        if (!Q_strcasecmp(sv.generic_precache[i], s))
            return i;
    }

    Host_Error("PF_precache_generic_I: '%s' overflow", s);
    return 0;
}

int func_002D04E8(char *name);

int PF_IsMapValid_I(char *mapname)
{
    char cBuf[260];

    sprintf(cBuf, "maps/%.32s.bs2", mapname);
    return func_002D04E8(cBuf) >= 0;
}

int PF_NumberOfEntities_I(void)
{
    int ent_count;
    int i;

    ent_count = 0;
    for (i = 0; i < sv.num_edicts; i++)
    {
        if (!sv.edicts[i].free)
            ent_count++;
    }

    return ent_count;
}

extern char localinfo[];
char *Info_Serverinfo(void);
char *Info_ValueForKey(char *s, char *key);

char *PF_GetInfoKeyBuffer_I(edict_t *e)
{
    int e1;

    if (!e)
        return localinfo;

    e1 = NUM_FOR_EDICT(e);
    if (!e1)
        return Info_Serverinfo();

    return (e1 < 4) ? svs.clients[e1 - 1].userinfo : "";
}

char *PF_InfoKeyValue_I(char *infobuffer, char *key)
{
    return Info_ValueForKey(infobuffer, key);
}

void Info_SetValueForKey(char *s, char *key, char *value, int maxsize);

void PF_SetKeyValue_I(char *infobuffer, char *key, char *value)
{
    if (infobuffer == localinfo)
        Info_SetValueForKey(infobuffer, key, value, 0x8000);
    else if (infobuffer == Info_Serverinfo())
        Info_SetValueForKey(infobuffer, key, value, 512);
    else
        Sys_Error("Can't set client keys with SetKeyValue");
}

void PF_SetClientKeyValue_I(int clientIndex, char *infobuffer, char *key, char *value)
{
    client_t *pClient;

    if (infobuffer == localinfo || infobuffer == Info_Serverinfo() || clientIndex <= 0 || clientIndex > svs.maxclients)
        return;

    if (strcmp(Info_ValueForKey(infobuffer, key), value))
    {
        Info_SetValueForKey(infobuffer, key, value, MAX_INFO_STRING);
        pClient = svs.clients + clientIndex - 1;
        pClient->sendinfo = true;
        pClient->sendinfo_time = 0;
    }
}

#define FL_FLY (1 << 0)
#define FL_SWIM (1 << 1)
#define FL_ONGROUND (1 << 9)

qboolean SV_movetest(edict_t *ent, vec3_t move, qboolean relink);
qboolean SV_movestep(edict_t *ent, vec3_t move, qboolean relink);

int PF_walkmove_I(edict_t *ent, float yaw, float dist, int iMode)
{
    vec3_t move;
    int ret;

    if (!(ent->v.flags & (FL_FLY | FL_SWIM | FL_ONGROUND)))
        return 0;

    yaw = yaw * M_PI * 2 / 360;

    move[0] = cosf(yaw) * dist;
    move[1] = sinf(yaw) * dist;
    move[2] = 0;

    if (iMode == 1)
        ret = SV_movetest(ent, move, true);
    else if (iMode == 2)
        ret = SV_movestep(ent, move, false);
    else
        ret = SV_movestep(ent, move, true);

    return ret;
}

int PF_droptofloor_I(edict_t *ent)
{
    vec3_t end;
    trace_t trace;
    qboolean monsterClip;

    monsterClip = (ent->v.flags & FL_MONSTERCLIP) > 0;

    VectorCopy(ent->v.origin, end);
    end[2] -= 256;

    trace = SV_Move(ent->v.origin, ent->v.mins, ent->v.maxs, end, 0, ent, monsterClip);
    if (trace.allsolid)
        return -1;

    if (trace.fraction == 1.0f)
        return 0;

    VectorCopy(trace.endpos, ent->v.origin);
    SV_LinkEdict(ent, false);
    ent->v.flags |= FL_ONGROUND;
    ent->v.groundentity = trace.ent;

    return 1;
}

int PF_DecalIndex(const char *name)
{
    int i;

    for (i = 0; i < sv_decalnamecount; i++)
    {
        if (!strcasecmp(sv_decalnames[i].name, name))
            return i;
    }

    return -1;
}

#define ss_active 2
#define svc_lightstyle 12

void PF_lightstyle_I(int style, char *val)
{
    int i;
    client_t *cl;

    sv.lightstyles[style] = val;
    if (sv.state != ss_active)
        return;

    for (i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
    {
        if ((cl->active || cl->spawned) && !cl->fakeclient)
        {
            MSG_WriteChar(&cl->netchan.message, svc_lightstyle);
            MSG_WriteChar(&cl->netchan.message, style);
            MSG_WriteString(&cl->netchan.message, val);
        }
    }
}

int SV_CheckBottom(edict_t *ent);
int SV_PointContents(const vec3_t p);

int PF_checkbottom_I(edict_t *pEdict)
{
    return SV_CheckBottom(pEdict);
}

int PF_pointcontents_I(const float *rgflVector)
{
    return SV_PointContents(rgflVector);
}

#define FL_FAKECLIENT (1 << 13)

extern cvar_t sv_aim;

void PF_aim_I(edict_t *ent, float speed, float *rgflReturn)
{
    vec3_t start, dir, end, bestdir;
    trace_t tr;
    int i, j;
    edict_t *check;
    float dist, bestdist;

    if (!ent || (ent->v.flags & FL_FAKECLIENT))
    {
        VectorCopy(gGlobalVariables.v_forward, rgflReturn);
        return;
    }

    VectorCopy(ent->v.origin, start);
    start[2] += ent->v.view_ofs[2];
    VectorCopy(gGlobalVariables.v_forward, dir);
    VectorMA(start, 2048, dir, end);
    tr = SV_Move(start, vec3_origin, vec3_origin, end, 0, ent, false);

    if (tr.ent && tr.ent->v.takedamage == 2 && (ent->v.team <= 0 || ent->v.team != tr.ent->v.team))
    {
        VectorCopy(gGlobalVariables.v_forward, rgflReturn);
        return;
    }

    VectorCopy(dir, bestdir);
    bestdist = sv_aim.value;

    for (i = 1; i < sv.num_edicts; i++)
    {
        check = &sv.edicts[i];
        if (check->v.takedamage != 2)
            continue;
        if (check->v.flags & FL_FAKECLIENT)
            continue;
        if (check == ent)
            continue;
        if (ent->v.team > 0 && ent->v.team == check->v.team)
            continue;

        for (j = 0; j < 3; j++)
            end[j] = check->v.origin[j] + (check->v.mins[j] + check->v.maxs[j]) * 0.75f;

        VectorSubtract(end, start, dir);
        VectorNormalize(dir);
        dist = DotProduct(dir, gGlobalVariables.v_forward);
        if (dist < bestdist)
            continue;

        tr = SV_Move(start, vec3_origin, vec3_origin, end, 0, ent, false);
        if (tr.ent == check)
        {
            bestdist = dist;
            VectorCopy(dir, bestdir);
        }
    }

    VectorCopy(bestdir, rgflReturn);
}

void PF_changeyaw_I(edict_t *ent)
{
    float ideal, current, move, speed;

    current = anglemod(ent->v.angles[1]);
    ideal = ent->v.ideal_yaw;
    speed = ent->v.yaw_speed;

    if (current == ideal)
        return;

    move = ideal - current;
    if (ideal > current)
    {
        if (move >= 180)
            move = move - 360;
    }
    else
    {
        if (move <= -180)
            move = move + 360;
    }

    if (move > 0)
    {
        if (move > speed)
            move = speed;
    }
    else
    {
        if (move < -speed)
            move = -speed;
    }

    ent->v.angles[1] = anglemod(current + move);
}

void PF_changepitch_I(edict_t *ent)
{
    float ideal, current, move, speed;

    current = anglemod(ent->v.angles[0]);
    ideal = ent->v.idealpitch;
    speed = ent->v.pitch_speed;

    if (current == ideal)
        return;

    move = ideal - current;
    if (ideal > current)
    {
        if (move >= 180)
            move = move - 360;
    }
    else
    {
        if (move <= -180)
            move = move + 360;
    }

    if (move > 0)
    {
        if (move > speed)
            move = speed;
    }
    else
    {
        if (move < -speed)
            move = -speed;
    }

    ent->v.angles[0] = anglemod(current + move);
}

#define svc_setview 5
#define svc_crosshairangle 50

void PF_setview_I(edict_t *clientent, edict_t *viewent)
{
    int clientnum;
    client_t *client;

    clientnum = NUM_FOR_EDICT(clientent);
    if (clientnum < 1 || clientnum > svs.maxclients)
        Host_Error("PF_setview_I: not a client");

    client = &svs.clients[clientnum - 1];
    if (!client->fakeclient)
    {
        client->pViewEntity = viewent;
        MSG_WriteByte(&client->netchan.message, svc_setview);
        MSG_WriteShort(&client->netchan.message, NUM_FOR_EDICT(viewent));
    }
}

void PF_crosshairangle_I(edict_t *clientent, float pitch, float yaw)
{
    int clientnum;
    client_t *client;

    clientnum = NUM_FOR_EDICT(clientent);
    if (clientnum < 1 || clientnum > svs.maxclients)
        return;

    client = &svs.clients[clientnum - 1];
    if (!client->fakeclient)
    {
        if (pitch > 180)
            pitch -= 360;
        if (pitch < -180)
            pitch += 360;
        if (yaw > 180)
            yaw -= 360;
        if (yaw < -180)
            yaw += 360;

        MSG_WriteByte(&client->netchan.message, svc_crosshairangle);
        MSG_WriteChar(&client->netchan.message, pitch * 5);
        MSG_WriteChar(&client->netchan.message, yaw * 5);
    }
}

#define FL_CLIENT (1 << 3)

extern int g_userid;
edict_t *EDICT_NUM(int n);
void SV_ExtractFromUserinfo(client_t *cl);

edict_t *PF_CreateFakeClient_I(const char *netname)
{
    client_t *fakeclient;
    edict_t *ent;
    int i;

    fakeclient = NULL;
    for (i = 0; i < svs.maxclients; i++)
    {
        fakeclient = &svs.clients[i];
        if (!fakeclient->active && !fakeclient->spawned && !fakeclient->connected)
            break;
    }

    if (i == svs.maxclients)
        return NULL;

    ent = EDICT_NUM(i + 1);

    memset(fakeclient, 0, sizeof(client_t));
    fakeclient->resourcesonhand.pPrev = &fakeclient->resourcesonhand;
    fakeclient->resourcesonhand.pNext = &fakeclient->resourcesonhand;
    fakeclient->resourcesneeded.pPrev = &fakeclient->resourcesneeded;
    fakeclient->resourcesneeded.pNext = &fakeclient->resourcesneeded;

    strncpy(fakeclient->name, netname, sizeof(fakeclient->name));

    fakeclient->active = true;
    fakeclient->spawned = true;
    fakeclient->connected = true;
    fakeclient->uploading = false;
    fakeclient->edict = ent;
    fakeclient->fakeclient = true;
    fakeclient->userid = g_userid++;

    ent->v.pContainingEntity = ent;
    ent->v.netname = fakeclient->name - pr_strings;
    ent->v.flags = FL_FAKECLIENT | FL_CLIENT;

    Info_SetValueForKey(fakeclient->userinfo, "name", (char *)netname, MAX_INFO_STRING);
    Info_SetValueForKey(fakeclient->userinfo, "model", "gordon", MAX_INFO_STRING);
    Info_SetValueForKey(fakeclient->userinfo, "topcolor", "1", MAX_INFO_STRING);
    Info_SetValueForKey(fakeclient->userinfo, "bottomcolor", "1", MAX_INFO_STRING);
    fakeclient->sendinfo = true;
    SV_ExtractFromUserinfo(fakeclient);

    return ent;
}

extern edict_t *sv_player;
void SV_PreRunCmd(void);
void SV_RunCmd(usercmd_t *ucmd, int random_seed);

void PF_RunPlayerMove_I(edict_t *fakeclient, const float *viewangles, float forwardmove, float sidemove, float upmove, unsigned short buttons, byte impulse, byte msec)
{
    usercmd_t cmd;
    edict_t *oldclient;
    client_t *old;
    int entnum;

    oldclient = sv_player;
    old = host_client;
    entnum = NUM_FOR_EDICT(fakeclient);
    sv_player = fakeclient;
    host_client = &svs.clients[entnum - 1];

    cmd.lightlevel = 0;
    cmd.viewangles[0] = viewangles[0];
    cmd.viewangles[1] = viewangles[1];
    cmd.viewangles[2] = viewangles[2];
    cmd.forwardmove = forwardmove;
    cmd.sidemove = sidemove;
    cmd.upmove = upmove;
    cmd.buttons = buttons;
    cmd.impulse = impulse;
    cmd.msec = msec;

    SV_PreRunCmd();
    SV_RunCmd(&cmd, 0);
    host_client->lastcmd = cmd;

    sv_player = oldclient;
    host_client = old;
}

#define MSG_BROADCAST 0
#define MSG_ONE 1
#define MSG_ALL 2
#define MSG_INIT 3
#define MSG_PVS 4
#define MSG_PAS 5
#define MSG_PVS_R 6
#define MSG_PAS_R 7

extern edict_t *gMsgEntity;

sizebuf_t *WriteDest_Parm(int dest)
{
    int entnum;

    switch (dest)
    {
    case MSG_BROADCAST:
        return &sv.datagram;
    case MSG_ONE:
        entnum = NUM_FOR_EDICT(gMsgEntity);
        if (entnum <= 0 || entnum > svs.maxclients)
            Host_Error("WriteDest_Parm: not a client");
        return &svs.clients[entnum - 1].netchan.message;
    case MSG_ALL:
        return &sv.reliable_datagram;
    case MSG_INIT:
        return &sv.signon;
    case MSG_PVS:
    case MSG_PAS:
        return &sv.multicast;
    default:
        Host_Error("WriteDest_Parm: bad destination=%d", dest);
        return NULL;
    }
}

extern sizebuf_t gMsgBuffer;
extern qboolean gMsgStarted;
extern int gMsgType;
extern int gMsgDest;
extern vec3_t gMsgOrigin;
qboolean Host_IsSinglePlayerGame(void);

void PF_MessageBegin_I(int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
    if (msg_dest == MSG_ONE)
    {
        if (!ed)
            Sys_Error("MSG_ONE with no target entity\n");
    }
    else
    {
        if (ed)
            Sys_Error("Invalid message;  cannot use broadcast message with a target entity");
    }

    if (gMsgStarted)
        Sys_Error("new message started when msg '%d' has not been sent yet", gMsgType);

    gMsgStarted = true;
    gMsgType = msg_type;
    gMsgEntity = ed;
    gMsgDest = msg_dest;

    if (msg_dest == MSG_PVS || msg_dest == MSG_PAS)
    {
        if (pOrigin)
            VectorCopy(pOrigin, gMsgOrigin);
        Host_IsSinglePlayerGame();
    }

    gMsgBuffer.allowoverflow = false;
    gMsgBuffer.cursize = 0;
    gMsgBuffer.overflowed = false;
}

typedef struct _UserMsg
{
    int iMsg;                   /* 0x00 */
    int iSize;                  /* 0x04 */
    char szName[12];            /* 0x08 */
    struct _UserMsg *next;      /* 0x14 */
} UserMsg;

#define svc_startofusermessages 53
#define MSG_FL_PVS 1
#define MSG_FL_PAS 2

extern UserMsg *sv_gpUserMsgs;
void MSG_WriteBuf(sizebuf_t *sb, int iSize, void *buf);
void SV_Multicast(vec3_t origin, int to, qboolean reliable);

void PF_MessageEnd_I(void)
{
    qboolean isVariableLengthMsg;
    UserMsg *pUserMsg;
    sizebuf_t *pBuffer;

    isVariableLengthMsg = false;

    if (!gMsgStarted)
        Sys_Error("MESSAGE_END called with no active message\n");

    gMsgStarted = false;

    if (gMsgEntity && (gMsgEntity->v.flags & FL_FAKECLIENT))
        return;

    if (gMsgType > svc_startofusermessages)
    {
        pUserMsg = sv_gpUserMsgs;
        while (pUserMsg && pUserMsg->iMsg != gMsgType)
            pUserMsg = pUserMsg->next;

        if (!pUserMsg)
        {
            Con_DPrintf("Illegal User Msg %d\n", gMsgType);
            return;
        }

        if (pUserMsg->iSize == -1)
        {
            isVariableLengthMsg = true;
        }
        else if (pUserMsg->iSize != gMsgBuffer.cursize)
        {
            Sys_Error("User Msg '%s': %d bytes written, expected %d\n", pUserMsg->szName, gMsgBuffer.cursize, pUserMsg->iSize);
            return;
        }
    }

    pBuffer = WriteDest_Parm(gMsgDest);
    if (gMsgDest == MSG_BROADCAST && pBuffer->cursize + gMsgBuffer.cursize > pBuffer->maxsize)
        return;

    if (!pBuffer->data)
    {
        Con_DPrintf("Message sent to uninitialized buffer\n");
        return;
    }

    MSG_WriteByte(WriteDest_Parm(gMsgDest), gMsgType);
    if (isVariableLengthMsg)
        MSG_WriteByte(WriteDest_Parm(gMsgDest), gMsgBuffer.cursize);
    MSG_WriteBuf(WriteDest_Parm(gMsgDest), gMsgBuffer.cursize, gMsgBuffer.data);

    switch (gMsgDest)
    {
    case MSG_PVS:
        SV_Multicast(gMsgOrigin, MSG_FL_PVS, false);
        break;
    case MSG_PAS:
        SV_Multicast(gMsgOrigin, MSG_FL_PAS, false);
        break;
    case MSG_PVS_R:
        SV_Multicast(gMsgOrigin, MSG_FL_PAS, true);
        break;
    case MSG_PAS_R:
        SV_Multicast(gMsgOrigin, MSG_FL_PAS, true);
        break;
    }
}

void MSG_WriteLong(sizebuf_t *sb, int c);
void MSG_WriteAngle(sizebuf_t *sb, float f);

void PF_WriteByte_I(int iValue)
{
    if (!gMsgStarted)
        Sys_Error("WRITE_BYTE called with no active message\n");
    MSG_WriteByte(&gMsgBuffer, iValue);
}

void PF_WriteChar_I(int iValue)
{
    if (!gMsgStarted)
        Sys_Error("WRITE_CHAR called with no active message\n");
    MSG_WriteChar(&gMsgBuffer, iValue);
}

void PF_WriteShort_I(int iValue)
{
    if (!gMsgStarted)
        Sys_Error("WRITE_SHORT called with no active message\n");
    MSG_WriteShort(&gMsgBuffer, iValue);
}

void PF_WriteLong_I(int iValue)
{
    if (!gMsgStarted)
        Sys_Error("PF_WriteLong_I called with no active message\n");
    MSG_WriteLong(&gMsgBuffer, iValue);
}

void PF_WriteAngle_I(float flValue)
{
    if (!gMsgStarted)
        Sys_Error("PF_WriteAngle_I called with no active message\n");
    MSG_WriteAngle(&gMsgBuffer, flValue);
}

void PF_WriteCoord_I(float flValue)
{
    if (!gMsgStarted)
        Sys_Error("PF_WriteCoord_I called with no active message\n");
    MSG_WriteCoord(&gMsgBuffer, flValue);
}

void PF_WriteString_I(const char *sz)
{
    if (!gMsgStarted)
        Sys_Error("PF_WriteString_I called with no active message\n");
    MSG_WriteString(&gMsgBuffer, sz);
}

void PF_WriteEntity_I(int iValue)
{
    if (!gMsgStarted)
        Sys_Error("PF_WriteEntity_I called with no active message\n");
    MSG_WriteShort(&gMsgBuffer, iValue);
}

void func_00276270(void) {
}

char *va(char *format, ...);
void func_002817E0(void);
void MSG_WriteCoord(sizebuf_t *sb, float f);
void MSG_WriteShort(sizebuf_t *sb, int c);
void MSG_WriteChar(sizebuf_t *sb, int c);
void MSG_WriteFloat(sizebuf_t *sb, float f);
void SV_BuildSoundMsg(edict_t *entity, int channel, const char *sample, int volume, float attenuation, int fFlags, int pitch, const float *origin, sizebuf_t *buffer);
extern int last_spawncount_changelevel;
extern int last_spawncount_map;
extern int idum;
extern int iy_ran1;
extern int iv_ran1[32];
extern int cls_demoplayback;

void PF_StaticDecal(const float *origin, int decalIndex, int entityIndex, int modelIndex)
{
    MSG_WriteByte(&sv.signon, 23);
    MSG_WriteByte(&sv.signon, 13);
    MSG_WriteCoord(&sv.signon, origin[0]);
    MSG_WriteCoord(&sv.signon, origin[1]);
    MSG_WriteCoord(&sv.signon, origin[2]);
    MSG_WriteShort(&sv.signon, decalIndex);
    MSG_WriteShort(&sv.signon, entityIndex);
    if (entityIndex)
        MSG_WriteShort(&sv.signon, modelIndex);
    func_002817E0();
}

void func_00276338(edict_t *e)
{
    int i = NUM_FOR_EDICT(e);

    if (i < 1 || i > svs.maxclients)
        Host_Error("Entity is not a client");
}

void PF_changelevel_I(char *s1, char *s2)
{
    if (svs.spawncount == last_spawncount_changelevel)
        return;
    last_spawncount_changelevel = svs.spawncount;

    if (!s2)
        Cbuf_AddText(va("changelevel %s\n", s1));
    else if ((int)gGlobalVariables.serverflags & 0x30)
        Cbuf_AddText(va("changelevel %s %s\n", s1, s2));
    else
        Cbuf_AddText(va("changelevel2 %s %s\n", s1, s2));
}

void func_00276420(char *s1)
{
    if (svs.spawncount == last_spawncount_map)
        return;
    last_spawncount_map = svs.spawncount;
    Cbuf_AddText(va("map %s\n", s1));
}

#define IA 16807
#define IM 2147483647
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1 + (IM - 1) / NTAB)
#define MAX_RANDOM_RANGE 0x7FFFFFFFU
#define AM (1.0 / IM)
#define EPS 1.2e-7
#define RNMX (1.0 - EPS)

void SeedRandomNumberGenerator(void)
{
    idum = -2249;
}

int ran1(void)
{
    int j;
    int k;

    if (idum <= 0 || !iy_ran1)
    {
        if (-(idum) < 1)
            idum = 1;
        else
            idum = -(idum);

        for (j = NTAB + 7; j >= 0; j--)
        {
            k = (idum) / IQ;
            idum = IA * (idum - k * IQ) - IR * k;
            if (idum < 0)
                idum += IM;
            if (j < NTAB)
                iv_ran1[j] = idum;
        }
        iy_ran1 = iv_ran1[0];
    }
    k = (idum) / IQ;
    idum = IA * (idum - k * IQ) - IR * k;
    if (idum < 0)
        idum += IM;
    j = iy_ran1 / NDIV;
    iy_ran1 = iv_ran1[j];
    iv_ran1[j] = idum;

    return iy_ran1;
}

float fran1(void)
{
    float temp = (float)AM * ran1();
    if (temp > (float)RNMX)
        return (float)RNMX;
    else
        return temp;
}

float RandomFloat(float flLow, float flHigh)
{
    float fl = fran1();
    return (fl * (flHigh - flLow)) + flLow;
}

int RandomLong(int lLow, int lHigh)
{
    unsigned int maxAcceptable;
    unsigned int x = lHigh - lLow + 1;
    unsigned int n;

    if (x <= 0 || MAX_RANDOM_RANGE < x - 1)
        return lLow;

    maxAcceptable = MAX_RANDOM_RANGE - ((MAX_RANDOM_RANGE + 1) % x);
    do
    {
        n = ran1();
    } while (n > maxAcceptable);

    return lLow + (n % x);
}

void PF_FadeVolume(const edict_t *clientent, int fadePercent, int fadeOutSeconds, int holdTime, int fadeInSeconds)
{
    int entnum;
    client_t *client;

    entnum = NUM_FOR_EDICT(clientent);
    if (entnum < 1 || entnum > svs.maxclients)
    {
        Con_Printf("tried to PF_FadeVolume a non-client\n");
        return;
    }

    client = &svs.clients[entnum - 1];
    if (client->fakeclient)
        return;

    MSG_WriteChar(&client->netchan.message, 51);
    MSG_WriteByte(&client->netchan.message, (byte)fadePercent);
    MSG_WriteByte(&client->netchan.message, (byte)holdTime);
    MSG_WriteByte(&client->netchan.message, (byte)fadeOutSeconds);
    MSG_WriteByte(&client->netchan.message, (byte)fadeInSeconds);
}

void PF_SetClientMaxspeed(const edict_t *clientent, float fNewMaxspeed)
{
    int entnum;
    client_t *client;

    entnum = NUM_FOR_EDICT(clientent);
    if (entnum < 1 || entnum > svs.maxclients)
    {
        Con_Printf("tried to PF_SetClientMaxspeed a non-client\n");
        return;
    }

    client = &svs.clients[entnum - 1];
    client->maxspeed = fNewMaxspeed;
    if (client->fakeclient)
        return;

    MSG_WriteChar(&sv.reliable_datagram, 52);
    MSG_WriteByte(&sv.reliable_datagram, (byte)(entnum - 1));
    MSG_WriteFloat(&sv.reliable_datagram, fNewMaxspeed);
}

int PF_GetPlayerUserId(edict_t *e)
{
    client_t *cl;
    int i;

    if (!sv.active || !e)
        return -1;

    for (i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
    {
        if (cl->edict == e)
            return cl->userid;
    }

    return -1;
}

unsigned int PF_GetPlayerWONId(edict_t *e)
{
    client_t *cl;
    int i;

    if (!sv.active)
        return -1;
    if (!e)
        return -1;
    for (i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
    {
        if (cl->edict == e)
            return cl->WONid;
    }

    return -1;
}

void PF_BuildSoundMsg_I(edict_t *entity, int channel, const char *sample, float volume, float attenuation, int fFlags, int pitch, int msg_dest, int msg_type, const float *pOrigin, edict_t *ed)
{
    PF_MessageBegin_I(msg_dest, msg_type, pOrigin, ed);
    SV_BuildSoundMsg(entity, channel, sample, (int)volume, attenuation, fFlags, pitch, pOrigin, &gMsgBuffer);
    PF_MessageEnd_I();
}

int PF_IsDemoPlayback(void)
{
    return cls_demoplayback;
}

void func_00276A68(int nClient, int value)
{
    cl[nClient != 1].unk_2128 = value;
}
