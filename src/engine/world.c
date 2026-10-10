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
hull_t *SV_HullForStudioModel(edict_t *pEdict, const vec3_t mins, const vec3_t maxs, vec3_t offset, int *pNumHulls);
int SV_HitgroupForStudioHull(int index);
void AngleVectorsTranspose(const vec3_t angles, vec3_t forward, vec3_t right, vec3_t up);
#define MOVETYPE_FOLLOW 12
#define MOVE_NOMONSTERS 1
#define MOVE_MISSILE 2
#define MOVETYPE_PUSHSTEP 13
#define kRenderNormal 0
#define FL_MONSTER (1 << 5)
#define FL_WORLDBRUSH (1 << 25)

typedef struct
{
    vec3_t boxmins;             /* 0x00 */
    vec3_t boxmaxs;             /* 0x0C */
    const float *mins;          /* 0x18 */
    const float *maxs;          /* 0x1C */
    vec3_t mins2;               /* 0x20 */
    vec3_t maxs2;               /* 0x2C */
    const float *start;         /* 0x38 */
    const float *end;           /* 0x3C */
    trace_t trace;              /* 0x40 */
    short type;                 /* 0x78 */
    short ignoretrans;          /* 0x7A */
    edict_t *passedict;         /* 0x7C */
    qboolean monsterClipBrush;  /* 0x80 */
} moveclip_t;

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

void SV_SingleClipMoveToEntity(edict_t *ent, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, trace_t *trace, int nostudio)
{
    hull_t *hull;
    vec3_t offset;
    vec3_t start_l, end_l;
    vec3_t forward, right, up;
    vec3_t temp;
    trace_t testtrace;
    vec3_t right2, up2;
    vec3_t temp2;
    int numhulls;
    int rotated;
    int closest;
    int i;
    model_t *pmodel;

    memset(trace, 0, sizeof(trace_t));
    trace->fraction = 1.0f;
    trace->allsolid = true;
    trace->endpos[0] = end[0];
    trace->endpos[1] = end[1];
    trace->endpos[2] = end[2];

    pmodel = sv.models[ent->v.modelindex];
    if (!nostudio && pmodel && pmodel->type == mod_studio)
    {
        hull = SV_HullForStudioModel(ent, mins, maxs, offset, &numhulls);
    }
    else
    {
        hull = SV_HullForEntity(ent, mins, maxs, offset);
        numhulls = 1;
    }

    VectorSubtract(start, offset, start_l);
    VectorSubtract(end, offset, end_l);

    if (ent->v.solid == SOLID_BSP && (ent->v.angles[0] != 0 || ent->v.angles[1] != 0 || ent->v.angles[2] != 0))
        rotated = true;
    else
        rotated = false;

    if (rotated)
    {
        AngleVectors(ent->v.angles, forward, right, up);

        VectorCopy(start_l, temp);
        start_l[0] = DotProduct(temp, forward);
        start_l[1] = -DotProduct(temp, right);
        start_l[2] = DotProduct(temp, up);

        VectorCopy(end_l, temp);
        end_l[0] = DotProduct(temp, forward);
        end_l[1] = -DotProduct(temp, right);
        end_l[2] = DotProduct(temp, up);
    }

    if (numhulls == 1)
    {
        SV_RecursiveHullCheck(hull, hull->firstclipnode, 0, 1, start_l, end_l, trace);
    }
    else
    {
        closest = 0;
        for (i = 0; i < numhulls; i++)
        {
            memset(&testtrace, 0, sizeof(trace_t));
            testtrace.endpos[0] = end[0];
            testtrace.endpos[1] = end[1];
            testtrace.endpos[2] = end[2];
            testtrace.fraction = 1.0f;
            testtrace.allsolid = true;
            SV_RecursiveHullCheck(&hull[i], hull[i].firstclipnode, 0, 1, start_l, end_l, &testtrace);

            if (i == 0 || testtrace.allsolid || testtrace.startsolid || testtrace.fraction < trace->fraction)
            {
                if (trace->startsolid)
                {
                    *trace = testtrace;
                    trace->startsolid = true;
                }
                else
                {
                    *trace = testtrace;
                }
                closest = i;
            }
        }
        trace->hitgroup = SV_HitgroupForStudioHull(closest);
    }

    if (trace->fraction != 1.0f)
    {
        if (rotated)
        {
            AngleVectorsTranspose(ent->v.angles, temp, right2, up2);
            VectorCopy(trace->plane.normal, temp2);
            trace->plane.normal[0] = DotProduct(temp2, temp);
            trace->plane.normal[1] = DotProduct(temp2, right2);
            trace->plane.normal[2] = DotProduct(temp2, up2);
        }

        trace->endpos[0] = start[0] + trace->fraction * (end[0] - start[0]);
        trace->endpos[1] = start[1] + trace->fraction * (end[1] - start[1]);
        trace->endpos[2] = start[2] + trace->fraction * (end[2] - start[2]);
    }

    if (trace->fraction < 1.0f || trace->startsolid)
        trace->ent = ent;
}

trace_t SV_ClipMoveToEntity(edict_t *ent, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end)
{
    trace_t goodtrace;

    SV_SingleClipMoveToEntity(ent, start, mins, maxs, end, &goodtrace, 0);
    return goodtrace;
}

trace_t SV_ClipMoveToEntityNoStudio(edict_t *ent, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end)
{
    trace_t goodtrace;

    SV_SingleClipMoveToEntity(ent, start, mins, maxs, end, &goodtrace, 1);
    return goodtrace;
}

void SV_ClipToLinks(areanode_t *node, moveclip_t *clip)
{
    link_t *l, *next;
    edict_t *touch;
    trace_t trace;

    for (l = node->solid_edicts.next; l != &node->solid_edicts; l = next)
    {
        next = l->next;
        touch = EDICT_FROM_AREA(l);

        if (touch->v.solid == SOLID_NOT)
            continue;
        if (touch == clip->passedict)
            continue;
        if (touch->v.solid == SOLID_TRIGGER)
            Sys_Error("Trigger in clipping list");

        if (touch->v.solid == SOLID_BSP)
        {
            if ((touch->v.flags & FL_MONSTERCLIP) && !clip->monsterClipBrush)
                continue;
        }
        else
        {
            if (clip->type == MOVE_NOMONSTERS && touch->v.movetype != MOVETYPE_PUSHSTEP)
                continue;
        }

        if (clip->ignoretrans && touch->v.rendermode != kRenderNormal && !(touch->v.flags & FL_WORLDBRUSH))
            continue;

        if (clip->boxmins[0] > touch->v.absmax[0]
            || clip->boxmins[1] > touch->v.absmax[1]
            || clip->boxmins[2] > touch->v.absmax[2]
            || clip->boxmaxs[0] < touch->v.absmin[0]
            || clip->boxmaxs[1] < touch->v.absmin[1]
            || clip->boxmaxs[2] < touch->v.absmin[2])
            continue;

        if (clip->passedict && clip->passedict->v.size[0] != 0 && touch->v.size[0] == 0)
            continue;

        if (clip->trace.allsolid)
            return;

        if (clip->passedict)
        {
            if (touch->v.owner == clip->passedict)
                continue;
            if (clip->passedict->v.owner == touch)
                continue;
        }

        if (touch->v.flags & FL_MONSTER)
            trace = SV_ClipMoveToEntity(touch, clip->start, clip->mins2, clip->maxs2, clip->end);
        else
            trace = SV_ClipMoveToEntity(touch, clip->start, clip->mins, clip->maxs, clip->end);

        if (trace.allsolid || trace.startsolid || trace.fraction < clip->trace.fraction)
        {
            trace.ent = touch;
            if (clip->trace.startsolid)
            {
                clip->trace = trace;
                clip->trace.startsolid = true;
            }
            else
            {
                clip->trace = trace;
            }
        }
    }

    if (node->axis == -1)
        return;

    if (clip->boxmaxs[node->axis] > node->dist)
        SV_ClipToLinks(node->children[0], clip);
    if (clip->boxmins[node->axis] < node->dist)
        SV_ClipToLinks(node->children[1], clip);
}

void SV_ClipToLinksNoStudio(areanode_t *node, moveclip_t *clip)
{
    link_t *l, *next;
    edict_t *touch;
    trace_t trace;

    for (l = node->solid_edicts.next; l != &node->solid_edicts; l = next)
    {
        next = l->next;
        touch = EDICT_FROM_AREA(l);

        if (touch->v.solid == SOLID_NOT)
            continue;
        if (touch == clip->passedict)
            continue;
        if (touch->v.solid == SOLID_TRIGGER)
            Sys_Error("Trigger in clipping list");

        if (touch->v.solid == SOLID_BSP)
        {
            if ((touch->v.flags & FL_MONSTERCLIP) && !clip->monsterClipBrush)
                continue;
        }
        else
        {
            if (clip->type == MOVE_NOMONSTERS && touch->v.movetype != MOVETYPE_PUSHSTEP)
                continue;
        }

        if (clip->ignoretrans && touch->v.rendermode != kRenderNormal && !(touch->v.flags & FL_WORLDBRUSH))
            continue;

        if (clip->boxmins[0] > touch->v.absmax[0]
            || clip->boxmins[1] > touch->v.absmax[1]
            || clip->boxmins[2] > touch->v.absmax[2]
            || clip->boxmaxs[0] < touch->v.absmin[0]
            || clip->boxmaxs[1] < touch->v.absmin[1]
            || clip->boxmaxs[2] < touch->v.absmin[2])
            continue;

        if (clip->passedict && clip->passedict->v.size[0] != 0 && touch->v.size[0] == 0)
            continue;

        if (clip->trace.allsolid)
            return;

        if (clip->passedict)
        {
            if (touch->v.owner == clip->passedict)
                continue;
            if (clip->passedict->v.owner == touch)
                continue;
        }

        if (touch->v.flags & FL_MONSTER)
            trace = SV_ClipMoveToEntityNoStudio(touch, clip->start, clip->mins2, clip->maxs2, clip->end);
        else
            trace = SV_ClipMoveToEntityNoStudio(touch, clip->start, clip->mins, clip->maxs, clip->end);

        if (trace.allsolid || trace.startsolid || trace.fraction < clip->trace.fraction)
        {
            trace.ent = touch;
            if (clip->trace.startsolid)
            {
                clip->trace = trace;
                clip->trace.startsolid = true;
            }
            else
            {
                clip->trace = trace;
            }
        }
    }

    if (node->axis == -1)
        return;

    if (clip->boxmaxs[node->axis] > node->dist)
        SV_ClipToLinksNoStudio(node->children[0], clip);
    if (clip->boxmins[node->axis] < node->dist)
        SV_ClipToLinksNoStudio(node->children[1], clip);
}

#ifdef NON_MATCHING
void SV_ClipToWorldbrush(areanode_t *node, moveclip_t *clip)
{
    link_t *l;
    link_t *next;
    edict_t *touch;
    trace_t trace;
    link_t *head;

    head = &node->solid_edicts;
    for (l = node->solid_edicts.next; l != head; l = next)
    {
        next = l->next;
        touch = EDICT_FROM_AREA(l);

        if (touch->v.solid != SOLID_BSP || touch == clip->passedict || !(touch->v.flags & FL_WORLDBRUSH))
            continue;

        if (clip->boxmins[0] > touch->v.absmax[0]
            || clip->boxmins[1] > touch->v.absmax[1]
            || clip->boxmins[2] > touch->v.absmax[2]
            || clip->boxmaxs[0] < touch->v.absmin[0]
            || clip->boxmaxs[1] < touch->v.absmin[1]
            || clip->boxmaxs[2] < touch->v.absmin[2])
            continue;

        if (clip->trace.allsolid)
            return;

        trace = SV_ClipMoveToEntity(touch, clip->start, clip->mins, clip->maxs, clip->end);

        if (trace.allsolid || trace.startsolid || trace.fraction < clip->trace.fraction)
        {
            trace.ent = touch;
            if (clip->trace.startsolid)
            {
                clip->trace = trace;
                clip->trace.startsolid = true;
            }
            else
            {
                clip->trace = trace;
            }
        }
    }

    if (node->axis == -1)
        return;

    if (clip->boxmaxs[node->axis] > node->dist)
        SV_ClipToWorldbrush(node->children[0], clip);
    if (clip->boxmins[node->axis] < node->dist)
        SV_ClipToWorldbrush(node->children[1], clip);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/world", SV_ClipToWorldbrush);
#endif

void SV_MoveBounds(const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, vec3_t boxmins, vec3_t boxmaxs)
{
    int i;

    for (i = 0; i < 3; i++)
    {
        if (end[i] > start[i])
        {
            boxmins[i] = start[i] + mins[i] - 1;
            boxmaxs[i] = end[i] + maxs[i] + 1;
        }
        else
        {
            boxmins[i] = end[i] + mins[i] - 1;
            boxmaxs[i] = start[i] + maxs[i] + 1;
        }
    }
}

trace_t SV_MoveNoEnts(const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int type, edict_t *passedict)
{
    moveclip_t clip;
    vec3_t worldEndPoint;
    float worldFraction;

    memset(&clip, 0, sizeof(moveclip_t));
    clip.trace = SV_ClipMoveToEntity(sv.edicts, start, mins, maxs, end);

    if (clip.trace.fraction == 0)
        return clip.trace;

    VectorCopy(clip.trace.endpos, worldEndPoint);
    worldFraction = clip.trace.fraction;
    clip.trace.fraction = 1.0f;
    clip.end = worldEndPoint;
    clip.type = type & 0xFF;
    clip.ignoretrans = type >> 8;
    clip.passedict = passedict;
    clip.start = start;
    clip.mins = mins;
    VectorCopy(mins, clip.mins2);
    clip.maxs = maxs;
    VectorCopy(maxs, clip.maxs2);
    clip.monsterClipBrush = false;

    SV_MoveBounds(start, clip.mins2, clip.maxs2, worldEndPoint, clip.boxmins, clip.boxmaxs);
    SV_ClipToWorldbrush(sv_areanodes, &clip);

    clip.trace.fraction *= worldFraction;
    if (clip.trace.ent)
        gGlobalVariables.trace_ent = clip.trace.ent;
    else
        gGlobalVariables.trace_ent = NULL;


    return clip.trace;
}

trace_t SV_Move(const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int type, edict_t *passedict, qboolean monsterClipBrush)
{
    moveclip_t clip;
    vec3_t worldEndPoint;
    float worldFraction;
    int i;

    memset(&clip, 0, sizeof(moveclip_t));
    clip.trace = SV_ClipMoveToEntity(sv.edicts, start, mins, maxs, end);

    if (clip.trace.fraction == 0)
        return clip.trace;

    VectorCopy(clip.trace.endpos, worldEndPoint);
    worldFraction = clip.trace.fraction;
    clip.trace.fraction = 1.0f;
    clip.type = type & 0xFF;
    clip.ignoretrans = type >> 8;
    clip.passedict = passedict;
    clip.monsterClipBrush = monsterClipBrush;
    clip.end = worldEndPoint;
    clip.start = start;
    clip.mins = mins;
    clip.maxs = maxs;

    if (type == MOVE_MISSILE)
    {
        for (i = 0; i < 3; i++)
        {
            clip.mins2[i] = -15;
            clip.maxs2[i] = 15;
        }
    }
    else
    {
        VectorCopy(mins, clip.mins2);
        VectorCopy(maxs, clip.maxs2);
    }

    SV_MoveBounds(start, clip.mins2, clip.maxs2, worldEndPoint, clip.boxmins, clip.boxmaxs);
    SV_ClipToLinks(sv_areanodes, &clip);

    clip.trace.fraction *= worldFraction;
    if (clip.trace.ent)
        gGlobalVariables.trace_ent = clip.trace.ent;
    else
        gGlobalVariables.trace_ent = NULL;


    return clip.trace;
}

trace_t SV_MoveNoStudio(const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int type, edict_t *passedict, qboolean monsterClipBrush)
{
    moveclip_t clip;
    vec3_t worldEndPoint;
    float worldFraction;
    int i;

    memset(&clip, 0, sizeof(moveclip_t));
    clip.trace = SV_ClipMoveToEntityNoStudio(sv.edicts, start, mins, maxs, end);

    if (clip.trace.fraction == 0)
        return clip.trace;

    VectorCopy(clip.trace.endpos, worldEndPoint);
    worldFraction = clip.trace.fraction;
    clip.trace.fraction = 1.0f;
    clip.type = type & 0xFF;
    clip.ignoretrans = type >> 8;
    clip.passedict = passedict;
    clip.monsterClipBrush = monsterClipBrush;
    clip.end = worldEndPoint;
    clip.start = start;
    clip.mins = mins;
    clip.maxs = maxs;

    if (type == MOVE_MISSILE)
    {
        for (i = 0; i < 3; i++)
        {
            clip.mins2[i] = -15;
            clip.maxs2[i] = 15;
        }
    }
    else
    {
        VectorCopy(mins, clip.mins2);
        VectorCopy(maxs, clip.maxs2);
    }

    SV_MoveBounds(start, clip.mins2, clip.maxs2, worldEndPoint, clip.boxmins, clip.boxmaxs);
    SV_ClipToLinksNoStudio(sv_areanodes, &clip);

    clip.trace.fraction *= worldFraction;
    if (clip.trace.ent)
        gGlobalVariables.trace_ent = clip.trace.ent;
    else
        gGlobalVariables.trace_ent = NULL;


    return clip.trace;
}

