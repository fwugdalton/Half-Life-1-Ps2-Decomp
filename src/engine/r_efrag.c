#include "common.h"
#include "engine/model.h"
#include "engine/cl_entity.h"
#include "engine/client.h"
#include "engine/mathlib.h"

#define MAX_VISEDICTS 512

#define BOX_ON_PLANE_SIDE(emins, emaxs, p) \
    (((p)->type < 3) ? \
    ( \
        ((p)->dist <= (emins)[(p)->type]) ? \
            1 \
        : \
        ( \
            ((p)->dist >= (emaxs)[(p)->type]) ? \
                2 \
            : \
                3 \
        ) \
    ) \
    : \
        BoxOnPlaneSide((emins), (emaxs), (p)))

extern client_state_t cl[2];
extern int cl_numvisedicts[2];
extern cl_entity_t *cl_visedicts[2];
extern int r_framecount;
extern vec3_t r_emins;
extern vec3_t r_emaxs;

extern efrag_t **lastlink;
extern cl_entity_t *r_addent;
extern mnode_t *r_pefragtopnode[2];

void R_SplitEntityOnNode(mnode_t *node, int clientId);

void R_RemoveEfrags(cl_entity_t *ent)
{
    efrag_t *ef, *old, *walk, **prev;

    ef = ent->efrag;

    while (ef)
    {
        prev = &ef->leaf->efrags;
        while (1)
        {
            walk = *prev;
            if (!walk)
                break;
            if (walk == ef)
            {
                *prev = ef->leafnext;
                break;
            }
            prev = &walk->leafnext;
        }

        old = ef;
        ef = ef->entnext;

        old->entnext = cl[0].free_efrags;
        cl[0].free_efrags = old;
    }

    ent->efrag = NULL;
}

void R_SplitEntityOnNode(mnode_t *node, int clientId)
{
    efrag_t *ef;
    mplane_t *splitplane;
    mleaf_t *leaf;
    int sides;

    if (node->contents == CONTENTS_SOLID)
        return;

    if (node->contents < 0)
    {
        if (!r_pefragtopnode[clientId])
            r_pefragtopnode[clientId] = node;

        leaf = (mleaf_t *)node;

        ef = cl[clientId].free_efrags;
        if (!ef)
        {
            Con_Printf("Too many efrags!\n");
            return;
        }
        cl[clientId].free_efrags = cl[clientId].free_efrags->entnext;

        ef->entity = r_addent;

        *lastlink = ef;
        lastlink = &ef->entnext;
        ef->entnext = NULL;

        ef->leaf = leaf;
        ef->leafnext = leaf->efrags;
        leaf->efrags = ef;
        return;
    }

    splitplane = node->plane;
    sides = BOX_ON_PLANE_SIDE(r_emins, r_emaxs, splitplane);

    if (sides == 3)
    {
        if (!r_pefragtopnode[clientId])
            r_pefragtopnode[clientId] = node;
    }

    if (sides & 1)
        R_SplitEntityOnNode(node->children[0], clientId);

    if (sides & 2)
        R_SplitEntityOnNode(node->children[1], clientId);
}

void R_AddEfrags(cl_entity_t *ent, int clientId)
{
    model_t *entmodel;
    int i;

    if (!ent->model)
        return;

    r_addent = ent;
    lastlink = &ent->efrag;
    r_pefragtopnode[clientId] = NULL;

    entmodel = ent->model;

    for (i = 0; i < 3; i++)
    {
        r_emins[i] = ent->origin[i] + entmodel->mins[i];
        r_emaxs[i] = ent->origin[i] + entmodel->maxs[i];
    }

    R_SplitEntityOnNode(cl[clientId].worldmodel->nodes, clientId);

    ent->topnode = &r_pefragtopnode[clientId];
}

void R_StoreEfrags(efrag_t **ppefrag, int clientId)
{
    cl_entity_t *pent;
    model_t *clmodel;
    efrag_t *pefrag;

    while ((pefrag = *ppefrag) != NULL)
    {
        pent = pefrag->entity;
        clmodel = pent->model;

        switch ((modtype_t)clmodel->type)
        {
        case mod_alias:
        case mod_brush:
        case mod_sprite:
            pent = pefrag->entity;

            if (pent->visframe != r_framecount && cl_numvisedicts[clientId] < MAX_VISEDICTS)
            {
                cl_visedicts[clientId][cl_numvisedicts[clientId]++] = *pent;
                pent->visframe = r_framecount;
            }

            ppefrag = &pefrag->leafnext;
            break;

        default:
            Sys_Error("R_StoreEfrags: Bad entity type %d\n", clmodel->type);
        }
    }
}
