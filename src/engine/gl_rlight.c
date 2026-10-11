#include "common.h"
#include "engine/model.h"
#include "engine/mathlib.h"
#include "engine/cl_entity.h"
#include "engine/client.h"
#include "engine/render.h"

void *memset(void *, int, int);

#define MAX_LIGHTSTYLES 64
#define MAX_DLIGHTS 32

typedef struct
{
    int length;
    char map[64];
} lightstyle_t; /* 0x44 */

typedef struct
{
    vec3_t origin;      /* 0x00 */
    float radius;       /* 0x0C */
    color24 color;      /* 0x10 */
    float die;          /* 0x14 */
    float decay;        /* 0x18 */
    float minlight;     /* 0x1C */
    int key;            /* 0x20 */
    qboolean dark;      /* 0x24 */
} dlight_t;

extern lightstyle_t cl_lightstyle[MAX_LIGHTSTYLES];
extern int d_lightstylevalue[];
extern dlight_t cl_dlights[MAX_DLIGHTS];
extern int r_framecount;
extern int r_dlightframecount; /* r_dlightframecount */
extern vec3_t lightspot; /* lightspot */
extern mplane_t *lightplane; /* lightplane */

#define SURF_DRAWTILED 0x20
#define MAXLIGHTMAPS 4

typedef struct
{
    unsigned int r, g, b, a;
} colorVec;

void R_MarkLights(dlight_t *light, int bit, mnode_t *node);

void R_AnimateLight(void)
{
    int i, j, k;

    i = (int)(cl[0].time * 10);
    for (j = 0; j < MAX_LIGHTSTYLES; j++)
    {
        if (!cl_lightstyle[j].length)
        {
            d_lightstylevalue[j] = 256;
            continue;
        }
        k = i % cl_lightstyle[j].length;
        k = cl_lightstyle[j].map[k] - 'a';
        k = k * 22;
        d_lightstylevalue[j] = k;
    }
}

void func_002AB188(void) {
}

void R_MarkLights(dlight_t *light, int bit, mnode_t *node)
{
    mplane_t *splitplane;
    float dist;
    msurface_t *surf;
    int i;
    float sl, tl, maxdist;
    int s, t;
    int irad;
    int sd, td;

    if (node->contents < 0)
        return;

    splitplane = node->plane;
    dist = DotProduct(light->origin, splitplane->normal) - splitplane->dist;

    if (dist > light->radius)
    {
        R_MarkLights(light, bit, node->children[0]);
        return;
    }
    if (dist < -light->radius)
    {
        R_MarkLights(light, bit, node->children[1]);
        return;
    }

    surf = cl[0].worldmodel->surfaces + node->firstsurface;
    for (i = 0; i < node->numsurfaces; i++, surf++)
    {
        maxdist = light->radius - __builtin_fabsf(dist);
        if (maxdist < light->minlight)
            continue;
        irad = (int)(maxdist - light->minlight);

        sl = DotProduct(light->origin, surf->vecs[0]) + surf->vecs[0][3] - surf->texturemins[0];
        tl = DotProduct(light->origin, surf->vecs[1]) + surf->vecs[1][3] - surf->texturemins[1];
        s = (int)sl;
        t = (int)tl;
        sd = surf->extents[0]; td = surf->extents[1];

        if (s > -irad && t > -irad && s <= sd + irad && t <= td + irad)
        {
            if (surf->dlightframe != r_dlightframecount)
                surf->dlightframe = r_dlightframecount;
            surf->dlightbits |= bit;
        }
    }

    R_MarkLights(light, bit, node->children[0]);
    R_MarkLights(light, bit, node->children[1]);
}

void R_PushDlights(void)
{
    int i;
    dlight_t *l;

    r_dlightframecount = r_framecount + 1;
    l = cl_dlights;
    for (i = 0; i < MAX_DLIGHTS; i++, l++)
    {
        if (l->die < cl[0].time || !l->radius)
            continue;
        R_MarkLights(l, 1 << i, cl[0].worldmodel->nodes);
    }
}

int RecursiveLightPoint(mnode_t *node, vec3_t start, vec3_t end, colorVec *c)
{
    float front, back, frac;
    int side;
    mplane_t *plane;
    vec3_t mid;
    msurface_t *surf;
    int s, t, ds, dt;
    int i;
    color24 *lightmap;
    unsigned int scale;
    int maps;

    if (node->contents < 0)
        return 0;

    plane = node->plane;
    front = DotProduct(start, plane->normal) - plane->dist;
    back = DotProduct(end, plane->normal) - plane->dist;
    side = front < 0;

    if ((back < 0) == side)
        return RecursiveLightPoint(node->children[side], start, end, c);

    frac = front / (front - back);
    mid[0] = start[0] + (end[0] - start[0]) * frac;
    mid[1] = start[1] + (end[1] - start[1]) * frac;
    mid[2] = start[2] + (end[2] - start[2]) * frac;

    if (RecursiveLightPoint(node->children[side], start, mid, c))
        return 1;

    if ((back < 0) == side)
        return 0;

    VectorCopy(mid, lightspot);
    lightplane = plane;

    surf = cl[0].worldmodel->surfaces + node->firstsurface;
    for (i = 0; i < node->numsurfaces; i++, surf++)
    {
        if (surf->flags & SURF_DRAWTILED)
            continue;

        if (!surf->samples)
            continue;
        lightmap = surf->samples;

        s = (int)(DotProduct(mid, surf->vecs[0]) + surf->vecs[0][3]);
        t = (int)(DotProduct(mid, surf->vecs[1]) + surf->vecs[1][3]);

        if (s < surf->texturemins[0] || t < surf->texturemins[1])
            continue;

        ds = s - surf->texturemins[0];
        dt = t - surf->texturemins[1];

        if (ds > surf->extents[0] || dt > surf->extents[1])
            continue;

        ds >>= 4;
        dt >>= 4;

        lightmap += dt * ((surf->extents[0] >> 4) + 1) + ds;

        for (maps = 0; maps < MAXLIGHTMAPS && surf->styles[maps] != 255; maps++)
        {
            scale = d_lightstylevalue[surf->styles[maps]];
            c->r += lightmap->r * scale;
            c->g += lightmap->g * scale;
            c->b += lightmap->b * scale;
            lightmap += ((surf->extents[0] >> 4) + 1) * ((surf->extents[1] >> 4) + 1);
        }

        c->r >>= 8;
        c->g >>= 8;
        c->b >>= 8;

        if (c->r || c->g || c->b)
            return 1;
    }

    return RecursiveLightPoint(node->children[!side], mid, end, c);
}

colorVec R_LightVec(vec3_t start, vec3_t end)
{
    colorVec c;
    vec3_t dir;

    memset(&c, 0, sizeof(c));

    if (cl[0].worldmodel->lightdata)
    {
        VectorSubtract(start, end, dir);
        RecursiveLightPoint(cl[0].worldmodel->nodes, start, end, &c);

        c.r += r_refdef.ambientlight.r;
        c.g += r_refdef.ambientlight.g;
        c.b += r_refdef.ambientlight.b;

        if (c.r > 255)
            c.r = 255;
        if (c.g > 255)
            c.g = 255;
        if (c.b > 255)
            c.b = 255;
    }
    else
    {
        c.r = c.g = c.b = 255;
    }

    return c;
}

colorVec R_LightPoint(vec3_t p)
{
    vec3_t end;

    end[0] = p[0];
    end[1] = p[1];
    end[2] = p[2] - 8192;

    return R_LightVec(p, end);
}
