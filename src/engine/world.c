#include "common.h"
#include "engine/mathlib.h"
#include "engine/model.h"
#include "engine/cmd.h"
#include "engine/server.h"
#include "engine/eiface.h"
#include "engine/progdefs.h"

#define CONTENTS_EMPTY -1
#define CONTENTS_SOLID -2
#define fabs(x) ((x) < 0 ? -(x) : (x))

extern hull_t sv_box_hull;
extern hull_t sv_beam_hull;
extern dclipnode_t sv_box_clipnodes[6];
extern mplane_t sv_box_planes[6];
extern mplane_t sv_beam_planes[6];

extern char *pr_strings;
extern areanode_t sv_areanodes[32];
extern int sv_numareanodes;
extern model_t *sv_worldmodel;

#define SOLID_TRIGGER 1
#define SOLID_BSP 4
#define MAX_TOUCHED_LEAFS 1024
#define BOX_ON_PLANE_SIDE(emins, emaxs, p) \
    (((p)->type < 3) ? \
        (((p)->dist <= (emins)[(p)->type]) ? 1 : (((p)->dist >= (emaxs)[(p)->type]) ? 2 : 3)) \
        : BoxOnPlaneSide((emins), (emaxs), (p)))
#define EDICT_FROM_AREA(l) ((edict_t *)((byte *)(l) - 8))

extern short sv_touched_headnode;
extern short sv_num_touched_leafs;
extern short sv_touched_leafnums[MAX_TOUCHED_LEAFS];

int SV_HullPointContents(hull_t *hull, int num, vec3_t p);
#define MOVETYPE_PUSH 7
#define AREA_DEPTH 4

void ClearLink(link_t *l);
void InsertLinkBefore(link_t *l, link_t *before);
void ED_ReserveLeafs(edict_t *ent, int count);
void ED_FreeLeafs(edict_t *ent);
void *memcpy(void *, const void *, unsigned int);
void SV_TouchLinks(edict_t *ent, areanode_t *node);
void SV_FindTouchedLeafs(edict_t *ent, mnode_t *node, int *topnode);
#define SOLID_NOT 0
#define CONTENTS_TRANSLUCENT -15
#define DIST_EPSILON (0.03125f)
#define CONTENTS_WATER -3
#define CONTENTS_CURRENT_0 -9
#define CONTENTS_CURRENT_DOWN -14
#define FL_MONSTERCLIP (1 << 23)
int SV_LinkContents(areanode_t *node, vec3_t pos);
trace_t SV_Move(const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int type, edict_t *passedict, qboolean monsterClipBrush);
void SV_SetGlobalTrace(trace_t *ptrace);
#define MOVETYPE_FOLLOW 12
void RemoveLink(link_t *l);
void *memset(void *, int, unsigned int);
void CrossProduct(const vec3_t v1, const vec3_t v2, vec3_t cross);
float VectorNormalize(vec3_t v);

void SV_InitBoxHull(void)
{
    int i;
    int side;

    sv_box_hull.clipnodes = sv_box_clipnodes;
    sv_box_hull.planes = sv_box_planes;
    sv_box_hull.firstclipnode = 0;
    sv_box_hull.lastclipnode = 5;

    sv_beam_hull = sv_box_hull;
    sv_beam_hull.planes = sv_beam_planes;

    for (i = 0; i < 6; i++)
    {
        sv_box_clipnodes[i].planenum = i;

        side = i & 1;

        sv_box_clipnodes[i].children[side] = CONTENTS_EMPTY;
        if (i != 5)
            sv_box_clipnodes[i].children[side ^ 1] = i + 1;
        else
            sv_box_clipnodes[i].children[side ^ 1] = CONTENTS_SOLID;

        sv_box_planes[i].type = i >> 1;
        sv_box_planes[i].normal[i >> 1] = 1;
        sv_beam_planes[i].type = 5;
    }
}

hull_t *SV_HullForBox(vec3_t mins, vec3_t maxs)
{
    sv_box_planes[0].dist = maxs[0];
    sv_box_planes[1].dist = mins[0];
    sv_box_planes[2].dist = maxs[1];
    sv_box_planes[3].dist = mins[1];
    sv_box_planes[4].dist = maxs[2];
    sv_box_planes[5].dist = mins[2];

    return &sv_box_hull;
}

hull_t *SV_HullForBeam(vec3_t start, vec3_t end, vec3_t size)
{
    vec3_t tmp;

    sv_beam_planes[0].normal[0] = end[0] - start[0];
    sv_beam_planes[0].normal[1] = end[1] - start[1];
    sv_beam_planes[0].normal[2] = end[2] - start[2];
    VectorNormalize(sv_beam_planes[0].normal);
    VectorCopy(sv_beam_planes[0].normal, sv_beam_planes[1].normal);

    sv_beam_planes[0].dist = DotProduct(end, sv_beam_planes[0].normal);
    sv_beam_planes[1].dist = DotProduct(start, sv_beam_planes[0].normal);

    if (fabs(sv_beam_planes[0].normal[2]) < 0.9f)
    {
        tmp[0] = tmp[1] = 0;
        tmp[2] = 1;
    }
    else
    {
        tmp[1] = tmp[2] = 0;
        tmp[0] = 1;
    }

    CrossProduct(sv_beam_planes[0].normal, tmp, sv_beam_planes[2].normal);
    VectorNormalize(sv_beam_planes[2].normal);
    VectorCopy(sv_beam_planes[2].normal, sv_beam_planes[3].normal);

    sv_beam_planes[2].dist = (start[0] + sv_beam_planes[2].normal[0]) * sv_beam_planes[2].normal[0]
        + (start[1] + sv_beam_planes[2].normal[1]) * sv_beam_planes[2].normal[1]
        + (start[2] + sv_beam_planes[2].normal[2]) * sv_beam_planes[2].normal[2];
    VectorSubtract(start, sv_beam_planes[2].normal, tmp);
    sv_beam_planes[3].dist = DotProduct(tmp, sv_beam_planes[2].normal);

    CrossProduct(sv_beam_planes[2].normal, sv_beam_planes[0].normal, sv_beam_planes[4].normal);
    VectorNormalize(sv_beam_planes[4].normal);
    VectorCopy(sv_beam_planes[4].normal, sv_beam_planes[5].normal);

    sv_beam_planes[4].dist = (start[0] + sv_beam_planes[4].normal[0]) * sv_beam_planes[4].normal[0]
        + (start[1] + sv_beam_planes[4].normal[1]) * sv_beam_planes[4].normal[1]
        + (start[2] + sv_beam_planes[4].normal[2]) * sv_beam_planes[4].normal[2];
    VectorSubtract(start, sv_beam_planes[4].normal, tmp);
    sv_beam_planes[5].dist = DotProduct(tmp, sv_beam_planes[4].normal);

    sv_beam_planes[0].dist += fabs(sv_beam_planes[0].normal[0] * size[0]) + fabs(sv_beam_planes[0].normal[1] * size[1]) + fabs(sv_beam_planes[0].normal[2] * size[2]);
    sv_beam_planes[1].dist -= fabs(sv_beam_planes[1].normal[0] * size[0]) + fabs(sv_beam_planes[1].normal[1] * size[1]) + fabs(sv_beam_planes[1].normal[2] * size[2]);
    sv_beam_planes[2].dist += fabs(sv_beam_planes[2].normal[0] * size[0]) + fabs(sv_beam_planes[2].normal[1] * size[1]) + fabs(sv_beam_planes[2].normal[2] * size[2]);
    sv_beam_planes[3].dist -= fabs(sv_beam_planes[3].normal[0] * size[0]) + fabs(sv_beam_planes[3].normal[1] * size[1]) + fabs(sv_beam_planes[3].normal[2] * size[2]);
    sv_beam_planes[4].dist += fabs(sv_beam_planes[4].normal[0] * size[0]) + fabs(sv_beam_planes[4].normal[1] * size[1]) + fabs(sv_beam_planes[4].normal[2] * size[2]);
    sv_beam_planes[5].dist -= fabs(sv_beam_planes[5].normal[0] * size[0]) + fabs(sv_beam_planes[5].normal[1] * size[1]) + fabs(sv_beam_planes[5].normal[2] * size[2]);

    return &sv_beam_hull;
}

hull_t *SV_HullForBsp(edict_t *ent, vec3_t mins, vec3_t maxs, vec3_t offset)
{
    vec3_t size;
    model_t *model;
    hull_t *hull;

    model = sv.models[ent->v.modelindex];
    if (!model)
        Sys_Error("Hit a %s with no model (%s)", &pr_strings[ent->v.classname], &pr_strings[ent->v.model]);

    if (!model || model->type != mod_brush)
    {
        Sys_Error("Hit a %s with no model (%s)", &pr_strings[ent->v.classname], &pr_strings[ent->v.model]);
        Sys_Error("MOVETYPE_PUSH with a non bsp model");
    }

    VectorSubtract(maxs, mins, size);

    if (size[0] <= 8)
    {
        hull = &model->hulls[0];
        VectorCopy(hull->clip_mins, offset);
    }
    else
    {
        if (size[0] <= 36 && size[2] <= 36)
            hull = &model->hulls[3];
        else if (size[0] <= 36)
            hull = &model->hulls[1];
        else
            hull = &model->hulls[2];

        VectorSubtract(hull->clip_mins, mins, offset);
    }

    VectorAdd(offset, ent->v.origin, offset);
    return hull;
}

hull_t *SV_HullForEntity(edict_t *ent, vec3_t mins, vec3_t maxs, vec3_t offset)
{
    hull_t *hull;
    vec3_t hullmins, hullmaxs;

    if (ent->v.solid == SOLID_BSP)
    {
        if (ent->v.movetype != MOVETYPE_PUSH)
            Sys_Error("SOLID_BSP without MOVETYPE_PUSH");

        return SV_HullForBsp(ent, mins, maxs, offset);
    }

    VectorSubtract(ent->v.mins, maxs, hullmins);
    VectorSubtract(ent->v.maxs, mins, hullmaxs);
    hull = SV_HullForBox(hullmins, hullmaxs);

    VectorCopy(ent->v.origin, offset);
    return hull;
}

areanode_t *SV_CreateAreaNode(int depth, vec3_t mins, vec3_t maxs)
{
    areanode_t *anode;
    vec3_t size;
    vec3_t mins1, maxs1, mins2, maxs2;

    anode = &sv_areanodes[sv_numareanodes];
    sv_numareanodes++;

    ClearLink(&anode->trigger_edicts);
    ClearLink(&anode->solid_edicts);

    if (depth == AREA_DEPTH)
    {
        anode->axis = -1;
        anode->children[0] = anode->children[1] = NULL;
        return anode;
    }

    VectorSubtract(maxs, mins, size);
    if (size[0] > size[1])
        anode->axis = 0;
    else
        anode->axis = 1;

    anode->dist = 0.5f * (maxs[anode->axis] + mins[anode->axis]);
    VectorCopy(mins, mins1);
    VectorCopy(mins, mins2);
    VectorCopy(maxs, maxs1);
    VectorCopy(maxs, maxs2);

    maxs1[anode->axis] = mins2[anode->axis] = anode->dist;

    anode->children[0] = SV_CreateAreaNode(depth + 1, mins2, maxs2);
    anode->children[1] = SV_CreateAreaNode(depth + 1, mins1, maxs1);

    return anode;
}

void SV_ClearWorld(void)
{
    SV_InitBoxHull();

    memset(sv_areanodes, 0, sizeof(sv_areanodes));
    sv_numareanodes = 0;
    SV_CreateAreaNode(0, sv_worldmodel->mins, sv_worldmodel->maxs);
}

void SV_UnlinkEdict(edict_t *ent)
{
    if (!ent->area.prev)
        return;

    RemoveLink(&ent->area);
    ent->area.prev = ent->area.next = NULL;
}

#ifdef NON_MATCHING
void SV_TouchLinks(edict_t *ent, areanode_t *node)
{
    link_t *l, *next;
    edict_t *touch;
    model_t *pModel;

    for (l = node->trigger_edicts.next; l != &node->trigger_edicts && l; l = next)
    {
        next = l->next;
        touch = EDICT_FROM_AREA(l);

        if (touch == ent)
            continue;

        if (touch->v.solid != SOLID_TRIGGER)
            continue;

        if (ent->v.absmin[0] > touch->v.absmax[0]
            || ent->v.absmin[1] > touch->v.absmax[1]
            || ent->v.absmin[2] > touch->v.absmax[2]
            || ent->v.absmax[0] < touch->v.absmin[0]
            || ent->v.absmax[1] < touch->v.absmin[1]
            || ent->v.absmax[2] < touch->v.absmin[2])
            continue;

        pModel = sv.models[touch->v.modelindex];
        if (pModel && pModel->type == mod_brush)
        {
            vec3_t offset, localPosition;
            hull_t *hull;

            hull = SV_HullForBsp(touch, ent->v.mins, ent->v.maxs, offset);
            VectorSubtract(ent->v.origin, offset, localPosition);
            if (SV_HullPointContents(hull, hull->firstclipnode, localPosition) != CONTENTS_SOLID)
                continue;
        }

        gGlobalVariables.time = sv.time;
        gEntityInterface.pfnTouch(touch, ent);
    }

    if (node->axis == -1)
        return;

    if (ent->v.absmax[node->axis] > node->dist)
        SV_TouchLinks(ent, node->children[0]);
    if (ent->v.absmin[node->axis] < node->dist)
        SV_TouchLinks(ent, node->children[1]);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/world", SV_TouchLinks);
#endif

void SV_FindTouchedLeafs(edict_t *ent, mnode_t *node, int *topnode)
{
    mplane_t *splitplane;
    mleaf_t *leaf;
    int sides;
    int leafnum;

    if (node->contents == CONTENTS_SOLID)
        return;

    if (node->contents < 0)
    {
        if ((unsigned short)sv_num_touched_leafs >= MAX_TOUCHED_LEAFS)
        {
            sv_num_touched_leafs = -1;
            return;
        }

        leaf = (mleaf_t *)node;
        leafnum = leaf - sv_worldmodel->leafs - 1;

        sv_touched_leafnums[sv_num_touched_leafs] = leafnum;
        sv_num_touched_leafs++;
        if (!sv_touched_headnode)
            sv_touched_headnode = leafnum;
        return;
    }

    splitplane = node->plane;
    sides = BOX_ON_PLANE_SIDE(ent->v.absmin, ent->v.absmax, splitplane);

    if (sides == 3 && *topnode == -1)
        *topnode = node - sv_worldmodel->nodes;

    if (sides & 1)
        SV_FindTouchedLeafs(ent, node->children[0], topnode);
    if (sides & 2)
        SV_FindTouchedLeafs(ent, node->children[1], topnode);
}

void SV_LinkEdict(edict_t *ent, qboolean touch_triggers)
{
    areanode_t *node;
    int topnode;

    if (ent->area.prev)
        SV_UnlinkEdict(ent);

    if (ent == sv.edicts)
        return;

    if (ent->free)
        return;

    gEntityInterface.pfnSetAbsBox(ent);

    ED_FreeLeafs(ent);

    if (ent->v.movetype == MOVETYPE_FOLLOW && ent->v.aiment)
    {
        if (ent->v.aiment->leafnums && ent->v.aiment->num_leafs > 0)
        {
            ED_ReserveLeafs(ent, ent->v.aiment->num_leafs);
            if (ent->leafnums)
            {
                ent->num_leafs = ent->v.aiment->num_leafs;
                ent->headnode = ent->v.aiment->headnode;
                memcpy(ent->leafnums, ent->v.aiment->leafnums, ent->v.aiment->num_leafs * sizeof(short));
            }
        }
    }
    else
    {
        topnode = -1;

        if (ent->v.modelindex)
        {
            sv_num_touched_leafs = 0;
            sv_touched_headnode = 0;
            SV_FindTouchedLeafs(ent, sv.worldmodel->nodes, &topnode);
            ent->headnode = sv_touched_headnode;

            if (sv_num_touched_leafs > 0)
            {
                ED_ReserveLeafs(ent, sv_num_touched_leafs);
                if (ent->leafnums)
                {
                    memcpy(ent->leafnums, sv_touched_leafnums, sv_num_touched_leafs * sizeof(short));
                    ent->num_leafs = sv_num_touched_leafs;
                }
            }
            else
            {
                ED_FreeLeafs(ent);
            }
        }
    }

    if (ent->v.solid == SOLID_NOT && ent->v.skin >= -1)
        return;

    if (ent->v.solid == SOLID_BSP && !sv.models[ent->v.modelindex] && !strlen(&pr_strings[ent->v.model]))
    {
        Con_DPrintf("Inserted %s with no model\n", &pr_strings[ent->v.classname]);
        return;
    }

    node = sv_areanodes;
    while (1)
    {
        if (node->axis == -1)
            break;
        if (ent->v.absmin[node->axis] > node->dist)
            node = node->children[0];
        else if (ent->v.absmax[node->axis] < node->dist)
            node = node->children[1];
        else
            break;
    }

    if (ent->v.solid == SOLID_TRIGGER)
        InsertLinkBefore(&ent->area, &node->trigger_edicts);
    else
        InsertLinkBefore(&ent->area, &node->solid_edicts);

    if (touch_triggers)
        SV_TouchLinks(ent, sv_areanodes);
}

int SV_HullPointContents(hull_t *hull, int num, vec3_t p)
{
    float d;
    dclipnode_t *node;
    mplane_t *plane;

    while (num >= 0)
    {
        if (num < hull->firstclipnode || num > hull->lastclipnode)
            Sys_Error("SV_HullPointContents: bad node number");

        node = hull->clipnodes + num;
        plane = hull->planes + node->planenum;

        if (plane->type < 3)
            d = p[plane->type] - plane->dist;
        else
            d = DotProduct(plane->normal, p) - plane->dist;

        if (d < 0)
            num = node->children[1];
        else
            num = node->children[0];
    }

    return num;
}

#ifdef NON_MATCHING
int SV_LinkContents(areanode_t *node, vec3_t pos)
{
    link_t *l, *next;
    edict_t *touch;
    model_t *pModel;
    hull_t *hull;
    vec3_t offset, localPosition;

    for (l = node->solid_edicts.next; l != &node->solid_edicts; l = next)
    {
        next = l->next;
        touch = EDICT_FROM_AREA(l);

        if (touch->v.solid != SOLID_NOT)
            continue;

        pModel = sv.models[touch->v.modelindex];
        if (!pModel || pModel->type != mod_brush)
            continue;

        if (pos[0] > touch->v.absmax[0]
            || pos[1] > touch->v.absmax[1]
            || pos[2] > touch->v.absmax[2]
            || pos[0] < touch->v.absmin[0]
            || pos[1] < touch->v.absmin[1]
            || pos[2] < touch->v.absmin[2])
            continue;

        if (touch->v.skin < -100 || touch->v.skin > 100)
            Con_DPrintf("Invalid contents on trigger field: %s\n", &pr_strings[touch->v.classname]);

        hull = SV_HullForBsp(touch, vec3_origin, vec3_origin, offset);
        VectorSubtract(pos, offset, localPosition);
        if (SV_HullPointContents(hull, hull->firstclipnode, localPosition) != CONTENTS_EMPTY)
            return touch->v.skin;
    }

    if (node->axis == -1)
        return CONTENTS_EMPTY;

    if (pos[node->axis] > node->dist)
        return SV_LinkContents(node->children[0], pos);
    if (pos[node->axis] < node->dist)
        return SV_LinkContents(node->children[1], pos);

    return CONTENTS_EMPTY;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/world", SV_LinkContents);
#endif

int SV_PointContents(vec3_t p)
{
    int cont;
    int entityContents;

    cont = SV_HullPointContents(&sv_worldmodel->hulls[0], 0, p);
    if (cont <= CONTENTS_CURRENT_0 && cont >= CONTENTS_CURRENT_DOWN)
        cont = CONTENTS_WATER;

    if (cont != CONTENTS_SOLID)
    {
        entityContents = SV_LinkContents(&sv_areanodes[0], p);
        if (entityContents != CONTENTS_EMPTY)
            return entityContents;
    }

    return cont;
}

int SV_TruePointContents(vec3_t p)
{
    return SV_HullPointContents(&sv_worldmodel->hulls[0], 0, p);
}

edict_t *SV_TestEntityPosition(edict_t *ent)
{
    trace_t trace;

    trace = SV_Move(ent->v.origin, ent->v.mins, ent->v.maxs, ent->v.origin, 0, ent, (ent->v.flags & FL_MONSTERCLIP) > 0);
    if (trace.startsolid)
    {
        SV_SetGlobalTrace(&trace);
        return trace.ent;
    }

    return NULL;
}

qboolean SV_RecursiveHullCheck(hull_t *hull, int num, float p1f, float p2f, vec3_t p1, vec3_t p2, trace_t *trace)
{
    dclipnode_t *node;
    mplane_t *plane;
    float t1, t2;
    float frac;
    int i;
    vec3_t mid;
    int side;
    float midf;

    if (num < 0)
    {
        if (num != CONTENTS_SOLID)
        {
            trace->allsolid = false;
            if (num == CONTENTS_EMPTY)
                trace->inopen = true;
            else if (num != CONTENTS_TRANSLUCENT)
                trace->inwater = true;
        }
        else
        {
            trace->startsolid = true;
        }
        return true;
    }

    if (num < hull->firstclipnode || num > hull->lastclipnode || !hull->planes)
        Sys_Error("SV_RecursiveHullCheck: bad node number");

    node = hull->clipnodes + num;
    plane = hull->planes + node->planenum;

    if (plane->type < 3)
    {
        t1 = p1[plane->type] - plane->dist;
        t2 = p2[plane->type] - plane->dist;
    }
    else
    {
        t1 = DotProduct(plane->normal, p1) - plane->dist;
        t2 = DotProduct(plane->normal, p2) - plane->dist;
    }

    if (t1 >= 0 && t2 >= 0)
        return SV_RecursiveHullCheck(hull, node->children[0], p1f, p2f, p1, p2, trace);
    if (t1 < 0 && t2 < 0)
        return SV_RecursiveHullCheck(hull, node->children[1], p1f, p2f, p1, p2, trace);

    if (t1 < 0)
        frac = (t1 + DIST_EPSILON) / (t1 - t2);
    else
        frac = (t1 - DIST_EPSILON) / (t1 - t2);
    if (frac < 0)
        frac = 0;
    if (frac > 1)
        frac = 1;

    midf = p1f + (p2f - p1f) * frac;
    for (i = 0; i < 3; i++)
        mid[i] = p1[i] + frac * (p2[i] - p1[i]);

    side = (t1 < 0);

    if (!SV_RecursiveHullCheck(hull, node->children[side], p1f, midf, p1, mid, trace))
        return false;

    if (SV_HullPointContents(hull, node->children[side ^ 1], mid) != CONTENTS_SOLID)
        return SV_RecursiveHullCheck(hull, node->children[side ^ 1], midf, p2f, mid, p2, trace);

    if (trace->allsolid)
        return false;

    if (!side)
    {
        VectorCopy(plane->normal, trace->plane.normal);
        trace->plane.dist = plane->dist;
    }
    else
    {
        VectorSubtract(vec3_origin, plane->normal, trace->plane.normal);
        trace->plane.dist = -plane->dist;
    }

    while (SV_HullPointContents(hull, hull->firstclipnode, mid) == CONTENTS_SOLID)
    {
        frac -= 0.1f;
        if (frac < 0)
        {
            trace->fraction = midf;
            VectorCopy(mid, trace->endpos);
            Con_DPrintf("backup past 0\n");
            return false;
        }
        midf = p1f + (p2f - p1f) * frac;
        for (i = 0; i < 3; i++)
            mid[i] = p1[i] + frac * (p2[i] - p1[i]);
    }

    trace->fraction = midf;
    VectorCopy(mid, trace->endpos);

    return false;
}

INCLUDE_ASM("asm/nonmatchings/engine/world", func_00290628);

INCLUDE_ASM("asm/nonmatchings/engine/world", SV_ClipMoveToEntity);

INCLUDE_ASM("asm/nonmatchings/engine/world", func_00290CF8);

INCLUDE_ASM("asm/nonmatchings/engine/world", func_00290DB0);

INCLUDE_ASM("asm/nonmatchings/engine/world", func_00291198);

INCLUDE_ASM("asm/nonmatchings/engine/world", func_00291580);

INCLUDE_ASM("asm/nonmatchings/engine/world", SV_MoveBounds);

INCLUDE_ASM("asm/nonmatchings/engine/world", func_00291900);

INCLUDE_ASM("asm/nonmatchings/engine/world", SV_Move);

INCLUDE_ASM("asm/nonmatchings/engine/world", SV_MoveNoEnts);

INCLUDE_RODATA("asm/nonmatchings/engine/world", D_00390780);
