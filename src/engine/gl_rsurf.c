#include "common.h"
#include "engine/model.h"
#include "engine/mathlib.h"
#include "engine/cl_entity.h"
#include "engine/client.h"
#include "engine/cvar.h"
#include "engine/render.h"
#include "engine/server.h"
#include "engine/progdefs.h"

double pow(double, double);
void Sys_Error(const char *fmt, ...);

#define GL_RGBA 0x1908
#define GL_LUMINANCE 0x1909
#define GL_INTENSITY 0x8049

#define MAX_DLIGHTS 32
#define fabs(x) ((x) < 0 ? -(x) : (x))

typedef struct
{
    unsigned int r, g, b, a;
} colorVec;

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

extern cl_entity_t *currententity;
extern dlight_t cl_dlights[MAX_DLIGHTS];
extern colorVec blocklights[];
extern cvar_t gl_overbright, r_fullbright, gl_monolights, v_lightgamma;
extern int gl_lightmap_format;
extern int d_lightstylevalue[];
extern int lightgammatable[1024];
extern int D_003995AC;
extern int r_framecount;
extern int rtable[20][20];
int RandomLong(int lLow, int lHigh);

#define GL_TEXTURE_2D 0xDE1
void glEnable(int cap);
void glDisable(int cap);
void GL_SelectTexture(int target);
extern qboolean mtexenabled;
extern qboolean gl_mtexable;
extern int TEXTURE0_SGIS, TEXTURE1_SGIS;

typedef struct decal_s
{
    struct decal_s *pnext;      /* 0x00 */
    msurface_t *psurface;       /* 0x04 */
    float dx, dy;               /* 0x08 */
    float scale;                /* 0x10 */
    short texture;              /* 0x14 */
    unsigned short flags;       /* 0x16 */
    short entityIndex;          /* 0x18 */
    byte pad1A[0x1C - 0x1A];
} decal_t;

#define FDECAL_PERMANENT 0x01
#define MAX_OVERLAP_DECALS 4
extern unsigned short gDecalFlags;
extern short gDecalEntity;

typedef struct
{
    int decalIndex;
    float decalVert[(0x74 - 4) / 4];
} decalcache_t;

#define MAX_DECALS 1024
#define DECAL_CACHEENTRIES 256
extern decal_t gDecalPool[MAX_DECALS];
extern int gDecalCount;
extern decalcache_t gDecalCache[DECAL_CACHEENTRIES];
extern int D_00333264, D_00413AD0;
#define MAX_LIGHTMAPS 64
extern int allocated[MAX_LIGHTMAPS][128];
void R_DecalShoot_(int, int, int, int, int, int, int);
void *Hunk_AllocName(int size, char *name);
void func_00225860(void *dst, int c, int n);
void func_0023D520(void);
int AllocBlock(int w, int h, int *x, int *y);
void *Draw_DecalTexture(int index);
float *R_DecalVertsClip(float *pOutVerts, decal_t *pdecal, msurface_t *psurf, texture_t *ptexture, int *pVertCount);
extern qboolean r_drawdecals, r_drawvls;
extern int lightmap_bytes;
extern byte *lightmaps;

extern cvar_t r_decals;
extern float *D_00333258, *D_0033325C;
void func_002A63C8(int count);
float fmodf(float x, float y);
void func_0023D618(int);
extern int D_00399560, D_00399584;
extern float *D_00413620;
extern int D_0033102C, D_0033103C, D_00331044;
extern void *D_00331034;
extern void *D_00331030, *D_00331038;

#define GL_POLYGON 9
void glBegin(int mode);
void glEnd(void);
void glTexCoord2f(float s, float t);
void glVertex3fv(float *v);
extern cvar_t r_dynamic;
void func_0023C808(float s, float t);
void func_002A6BE0(glpoly_t *p);
void func_002A6CA0(glpoly_t *p);
#define MAX_DECALSURFS 500
#define kRenderTransColor 1
extern float D_00333268, D_0032F26C;
void func_002A72F0(msurface_t *surf, byte *dest, int stride, int clientId);
void func_002A7AF8(msurface_t *surf);
void func_002A7238(byte *p);
#define SURF_DRAWSCROLL 0x20
#define MAXLIGHTMAPS 4
extern cl_entity_t *cl_entities[2];
extern int D_00399520, D_00399590;
extern cvar_t r_wateralpha, gl_polyoffset;
void glPolygonOffset(float factor, float units);
void func_0023AFC8(int);
void R_DrawSkyChain(msurface_t *s);
void R_RenderBrushPolyChain(msurface_t *s, int clientId);
extern msurface_t *gDecalSurfs[];
#define GL_FOG 0xB60
#define GL_POLYGON_OFFSET_FILL 0x8037
#define SURF_DRAWTURB 0x10
#define FDECAL_NOCLIP 0x40
qboolean R_CullBox(float *mins, float *maxs);
void R_StoreEfrags(efrag_t **ppefrag, int clientId);
#define SURF_PLANEBACK 2
#define SURF_UNDERWATER 0x80
#define BACKFACE_EPSILON 0.01f
byte *Mod_LeafPVS(mleaf_t *leaf, model_t *model);
extern mleaf_t *r_viewleaf[2], *r_oldviewleaf[2];
extern cvar_t r_novis;
extern short r_visframecount[2];
extern int D_00399580;
void glAlphaFunc(int func, float ref);
void glTexSubImage2D(int target, int level, int xoffset, int yoffset, int width, int height, int format, int type, void *pixels);
void GL_Bind(int texnum);
void func_0023AF68(void);
void func_0023D430(int);
extern cvar_t gl_alphamin;
extern int lightmap_textures;
extern qboolean lightmap_modified[64];
#define GL_ALPHA_TEST 0xBC0
#define GL_GREATER 0x204
#define GL_ONE 1
#define GL_SRC_ALPHA 0x302
#define GL_ONE_MINUS_SRC_ALPHA 0x303
#define GL_ALPHA 0x1906
#define GL_UNSIGNED_BYTE 0x1401
void glColor3f(float r, float g, float b);
void R_ClearSkyBox(void);
void R_RecursiveWorldNode(mnode_t *node, int clientId);
void DrawTextureChains(int clientId);
void func_002CD9F8(int clientId);
void R_DrawDecals(void);
extern vec3_t modelorg;
extern int currenttexture;
extern glpoly_t *lightmap_polys[64];
extern int D_004E77B0[3];
extern int gDecalSurfCount;
void glColor4f(float r, float g, float b, float a);
void glDepthMask(int flag);
void glTexEnvf(int target, int pname, float param);
void glBlendFunc(int sfactor, int dfactor);
void R_BlendLightmaps(void);
extern float r_blend;
#define GL_BLEND 0xBE2
#define GL_TEXTURE_ENV 0x2300
#define GL_TEXTURE_ENV_MODE 0x2200
#define GL_MODULATE 0x2100
#define GL_REPLACE 0x1E01
#define GL_ZERO 0
#define GL_SRC_COLOR 0x300
#define GL_DST_COLOR 0x306
float ScrollOffset(msurface_t *psurface, cl_entity_t *pEntity);
decal_t *R_DecalIntersect(msurface_t *psurf, int *pcount, float x, float y);
extern int rtable2[20][20];
extern float D_00413630[74][4];
void func_002EF8E0(int);

typedef struct
{
    char *name;                 /* 0x00 */
    short entityIndex;          /* 0x04 */
    byte depth;                 /* 0x06 */
    byte flags;                 /* 0x07 */
    vec3_t position;            /* 0x08 */
} decalentry_t; /* 0x14 */

#define FDECAL_DONTSAVE 0x04
int R_DecalUnProject(decal_t *pdecal, vec_t *position);
void qsort(void *base, int num, int width, int (*compare)(const void *, const void *));

#define BLOCK_WIDTH 128
#define BLOCK_HEIGHT 128
extern int D_0040A740, D_0040A744;
extern int D_00333254, D_00333260;

/* no PS2 os flags do texinfo são um int em 0x20 (a metade de cima é surf->flags) */
#define TEXINFO_FLAGS(s) (*(int *)&(s)->unk_20)

#ifdef NON_MATCHING
void R_AddDynamicLights(msurface_t *surf)
{
    int lnum;
    int sd, td;
    float dist, rad, minlight;
    vec3_t local, impact;
    int s, t;
    int smax, tmax;
    int brightness;
    colorVec *bl;

    smax = (surf->extents[0] >> 4) + 1;
    tmax = (surf->extents[1] >> 4) + 1;

    for (lnum = 0; lnum < MAX_DLIGHTS; lnum++)
    {
        if (!(surf->dlightbits & (1 << lnum)))
            continue;

        rad = cl_dlights[lnum].radius;
        VectorSubtract(cl_dlights[lnum].origin, currententity->origin, impact);
        dist = DotProduct(impact, surf->plane->normal) - surf->plane->dist;
        rad -= __builtin_fabsf(dist);
        minlight = cl_dlights[lnum].minlight;
        if (rad < minlight)
            continue;
        minlight = rad - minlight;

        local[0] = DotProduct(impact, surf->vecs[0]) + surf->vecs[0][3];
        local[1] = DotProduct(impact, surf->vecs[1]) + surf->vecs[1][3];
        local[0] -= surf->texturemins[0];
        local[1] -= surf->texturemins[1];

        for (t = 0; t < tmax; t++)
        {
            td = local[1] - t * 16;
            if (td < 0)
                td = -td;

            bl = &blocklights[t * smax];
            for (s = 0; s < smax; s++, bl++)
            {
                sd = local[0] - s * 16;
                if (sd < 0)
                    sd = -sd;

                if (sd > td)
                    dist = sd + (td >> 1);
                else
                    dist = td + (sd >> 1);

                if (dist < minlight)
                {
                    brightness = (rad - dist) * 256;
                    bl->r += (brightness * cl_dlights[lnum].color.r) >> 8;
                    bl->g += (brightness * cl_dlights[lnum].color.g) >> 8;
                    bl->b += (brightness * cl_dlights[lnum].color.b) >> 8;
                }
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", R_AddDynamicLights);
#endif

void R_BuildLightMap(msurface_t *surf, byte *dest, int stride, int clientId)
{
    int smax, tmax;
    int t;
    int i, j, size;
    color24 *lightmap;
    unsigned int scale;
    int maps;
    colorVec *bl;
    int shift;
    int k;
    int lum;

    if (gl_overbright.value != 0.0f)
        shift = 256;
    else
        shift = (int)(pow(2.0, 1.0f / v_lightgamma.value) * 256.0 + 0.5);

    surf->dlightbits = surf->cached_dlight = surf->dlightbits & D_003995AC;

    smax = (surf->extents[0] >> 4) + 1;
    tmax = (surf->extents[1] >> 4) + 1;
    size = smax * tmax;
    lightmap = surf->samples;

    if (r_fullbright.value != 0.0f || !cl[clientId].worldmodel->lightdata)
    {
        for (i = 0; i < size; i++)
        {
            blocklights[i].r = 255 * 256;
            blocklights[i].g = 255 * 256;
            blocklights[i].b = 255 * 256;
        }
        goto store;
    }

    for (i = 0; i < size; i++)
    {
        blocklights[i].r = 0;
        blocklights[i].g = 0;
        blocklights[i].b = 0;
    }

    if (lightmap)
    {
        for (maps = 0; maps < 4 && surf->styles[maps] != 255; maps++)
        {
            scale = d_lightstylevalue[surf->styles[maps]];
            surf->cached_light[maps] = scale;
            for (i = 0; i < size; i++)
            {
                blocklights[i].r += lightmap[i].r * scale;
                blocklights[i].g += lightmap[i].g * scale;
                blocklights[i].b += lightmap[i].b * scale;
            }
            lightmap += size;
        }
    }

    if (surf->dlightframe == r_framecount)
        R_AddDynamicLights(surf);

store:
    switch (gl_lightmap_format)
    {
    case GL_RGBA:
        stride -= smax << 2;
        bl = blocklights;
        for (i = 0; i < tmax; i++, dest += stride)
        {
            for (j = 0; j < smax; j++)
            {
                for (k = 0; k < 3; k++)
                {
                    t = ((unsigned int *)bl)[k];
                    t = (unsigned int)(t * shift) >> 14;
                    if (t > 1023)
                        t = 1023;
                    dest[k] = lightgammatable[t] >> 2;
                }
                if (gl_monolights.value != 0.0f)
                {
                    lum = dest[0] * 76 + dest[1] * 149 + dest[2] * 29;
                    dest[0] = 255;
                    dest[2] = 255;
                    dest[1] = 255;
                    dest[3] = 255 - (lum >> 8);
                }
                else
                {
                    dest[3] = 255;
                }
                bl++;
                dest += 4;
            }
        }
        break;

    case GL_LUMINANCE:
    case GL_INTENSITY:
        bl = blocklights;
        for (i = 0; i < tmax; i++, dest += stride)
        {
            for (j = 0; j < smax; j++)
            {
                t = bl->r;
                t = t >> 8;
                if (t > 255)
                    t = 255;
                dest[j] = 255 - t;
                bl++;
            }
        }
        break;

    default:
        Sys_Error("Bad lightmap format");
    }
}

texture_t *R_TextureAnimation(msurface_t *s)
{
    texture_t *base;
    int count, reletive;
    int tx, ty;

    base = s->texture;

    if (!rtable[0][0])
    {
        for (tx = 0; tx < 20; tx++)
        {
            for (ty = 0; ty < 20; ty++)
                rtable[tx][ty] = RandomLong(0, 0x7FFF);
        }
    }

    if (currententity->frame)
    {
        if (base->alternate_anims)
            base = base->alternate_anims;
    }

    if (!base->anim_total)
        return base;

    if (base->name[0] == '-')
    {
        texture_t *tex;

        tx = s->texturemins[0] + (base->srcwidth << 16);
        tx /= base->srcwidth;
        ty = s->texturemins[1] + (base->srcheight << 16);
        ty /= base->srcheight;
        count = 0;
        reletive = rtable[tx % 20][ty % 20] % base->anim_total;
        tex = base;
        while (tex->anim_min > reletive || tex->anim_max <= reletive)
        {
            tex = tex->anim_next;
            if (!tex)
                Sys_Error("R_TextureAnimation: broken cycle");
            if (++count > 100)
                Sys_Error("R_TextureAnimation: infinite cycle");
        }
        base = tex;
    }
    else
    {
        reletive = (int)(cl[0].time * 10) % base->anim_total;
        count = 0;
        while (base->anim_min > reletive || base->anim_max <= reletive)
        {
            base = base->anim_next;
            if (!base)
                Sys_Error("R_TextureAnimation: broken cycle");
            if (++count > 100)
                Sys_Error("R_TextureAnimation: infinite cycle");
        }
    }

    return base;
}

void GL_DisableMultitexture(void)
{
    if (mtexenabled)
    {
        glDisable(GL_TEXTURE_2D);
        GL_SelectTexture(TEXTURE0_SGIS);
        mtexenabled = false;
    }
}

void GL_EnableMultitexture(void)
{
    if (gl_mtexable)
    {
        GL_SelectTexture(TEXTURE1_SGIS);
        glEnable(GL_TEXTURE_2D);
        mtexenabled = true;
    }
}

void DrawGLSolidPoly(glpoly_t *p)
{
    int i;
    float *v;

    v = func_002A3C60((byte *)p);
    glColor4f(currententity->rendercolor.r / 256.0f, currententity->rendercolor.g / 256.0f,
              currententity->rendercolor.b / 256.0f, r_blend);
    glBegin(GL_POLYGON);
    D_00399584++;
    D_00399560 += p->numverts;
    for (i = 0; i < p->numverts; i++, v += 3)
        glVertex3fv(v);
    glEnd();
}

void R_RenderLightmaps(void)
{
    if (r_fullbright.value)
        return;

    glDepthMask(0);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    if (gl_overbright.value)
    {
        glBlendFunc(GL_DST_COLOR, GL_SRC_COLOR);
        glColor4f(2.0f / 3.0f, 2.0f / 3.0f, 2.0f / 3.0f, 1.0f);
    }
    else
    {
        glBlendFunc(GL_ZERO, GL_SRC_COLOR);
    }
    glEnable(GL_BLEND);
    R_BlendLightmaps();
    glDisable(GL_BLEND);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glDepthMask(1);
}

/* tabela de floats alinhada em 8 no original (até virar C) */
__asm__(".section .rodata\n.align 3\n.section .text");
INCLUDE_RODATA("asm/nonmatchings/engine/gl_rsurf", D_003937E8);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rsurf", D_00393830);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rsurf", D_00393930);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rsurf", D_00393BE8);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rsurf", D_00393FE8);

void R_RenderBrushPoly(msurface_t *fa, int clientId)
{
    texture_t *t;
    byte *base;
    int maps;

    if (fa->flags & 0x400)
    {
        if (fa->unk_80)
            func_002A7AF8(fa);
        return;
    }

    if (fa->flags & 0x104)
        return;

    t = R_TextureAnimation(fa);
    GL_Bind(t->gl_texturenum);
    func_0023C808(fa->unk_3C, 1.0f);

    if (currententity->rendermode == kRenderTransColor)
    {
        glDisable(GL_TEXTURE_2D);
        DrawGLSolidPoly(fa->polys);
        glEnable(GL_TEXTURE_2D);
    }
    else if (fa->flags & SURF_DRAWSCROLL)
    {
        DrawGLPolyScroll(fa, currententity);
    }
    else if (r_blend > 0.99f)
    {
        func_002A6BE0(fa->polys);
    }
    else
    {
        func_002A6CA0(fa->polys);
    }
    func_0023C808(0, 0);

    fa->polys->chain = lightmap_polys[fa->lightmaptexturenum];
    lightmap_polys[fa->lightmaptexturenum] = fa->polys;

    if (fa->pdecals)
    {
        gDecalSurfs[gDecalSurfCount] = fa;
        gDecalSurfCount++;
        if (gDecalSurfCount > MAX_DECALSURFS)
            Sys_Error("Too many decal surfaces!\n");
    }

    for (maps = 0; maps < MAXLIGHTMAPS && fa->styles[maps] != 255; maps++)
    {
        if (d_lightstylevalue[fa->styles[maps]] != fa->cached_light[maps])
            goto dynamic;
    }

    if (fa->dlightframe == r_framecount || fa->cached_dlight)
    {
dynamic:
        if (r_dynamic.value)
        {
            lightmap_modified[fa->lightmaptexturenum] = true;
            base = lightmaps + fa->lightmaptexturenum * lightmap_bytes * BLOCK_WIDTH * BLOCK_HEIGHT;
            base += fa->light_t * BLOCK_WIDTH * lightmap_bytes + fa->light_s * lightmap_bytes;
            R_BuildLightMap(fa, base, BLOCK_WIDTH * lightmap_bytes, clientId);
        }
    }
}

void R_SetRenderMode(byte *pEntity)
{
    int mode = *(int *)(pEntity + 0x10);

    if (mode != 0)
    {
        if (mode == 1)
        {
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ALPHA);
        }
        else if (mode == 5)
        {
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glBlendFunc(GL_ONE, GL_ONE);
            glColor4f(r_blend, r_blend, r_blend, 1.0f);
            glDepthMask(0);
        }
        else if (mode == 4)
        {
            glEnable(GL_ALPHA_TEST);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            glDisable(GL_BLEND);
            glAlphaFunc(GL_GREATER, gl_alphamin.value);
            return;
        }
        else
        {
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(1.0f, 1.0f, 1.0f, r_blend);
            glDepthMask(0);
        }
        glEnable(GL_BLEND);
    }
    else
    {
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    }
}

extern float D_00333654;
void func_002AB190(dlight_t *light, int bit, mnode_t *node);
void func_0023B1C8(void);
void func_002A8CB0(cl_entity_t *e);
void func_0023B258(void);

void R_DrawBrushModel(cl_entity_t *e, int clientId)
{
    int i;
    int k;
    vec3_t mins, maxs;
    msurface_t *psurf;
    float dot;
    mplane_t *pplane;
    model_t *clmodel;
    qboolean rotated;
    vec3_t temp;
    vec3_t forward, right, up;

    currenttexture = -1;
    currententity = e;
    clmodel = e->model;

    if (e->angles[0] || e->angles[1] || e->angles[2])
    {
        rotated = 1;
        for (i = 0; i < 3; i++)
        {
            mins[i] = e->origin[i] - clmodel->radius;
            maxs[i] = e->origin[i] + clmodel->radius;
        }
    }
    else
    {
        rotated = 0;
        VectorAdd(e->origin, clmodel->mins, mins);
        VectorAdd(e->origin, clmodel->maxs, maxs);
    }

    if (R_CullBox(mins, maxs))
        return;

    glColor3f(1.0f, 1.0f, 1.0f);
    memset(lightmap_polys, 0, sizeof(lightmap_polys));

    VectorSubtract(r_refdef.vieworg, e->origin, modelorg);
    if (rotated)
    {
        VectorCopy(modelorg, temp);
        AngleVectors(e->angles, forward, right, up);
        modelorg[0] = DotProduct(temp, forward);
        modelorg[1] = -DotProduct(temp, right);
        modelorg[2] = DotProduct(temp, up);
    }

    psurf = &clmodel->surfaces[clmodel->firstmodelsurface];

    if (clmodel->firstmodelsurface != 0 && !D_00333654)
    {
        for (k = 0; k < MAX_DLIGHTS; k++)
        {
            if (cl_dlights[k].die < cl[clientId].time || !cl_dlights[k].radius)
                continue;

            VectorCopy(cl_dlights[k].origin, temp);
            VectorSubtract(cl_dlights[k].origin, currententity->origin, cl_dlights[k].origin);
            func_002AB190(&cl_dlights[k], 1 << k, clmodel->nodes + clmodel->hulls[0].firstclipnode);
            VectorCopy(temp, cl_dlights[k].origin);
        }
    }

    func_0023B1C8();
    func_002A8CB0(e);
    R_SetRenderMode((byte *)e);

    for (i = 0; i < clmodel->nummodelsurfaces; i++, psurf++)
    {
        pplane = psurf->plane;

        if (psurf->flags & SURF_DRAWTURB)
        {
            if (pplane->type != 2)
                continue;
            if (psurf->flags & SURF_PLANEBACK)
                continue;
            if (mins[2] + 1.0f >= pplane->dist)
                continue;
            R_RenderBrushPoly(psurf, clientId);
            continue;
        }

        dot = DotProduct(modelorg, pplane->normal) - pplane->dist;

        if (((psurf->flags & SURF_PLANEBACK) && (dot < -BACKFACE_EPSILON)) ||
            (!(psurf->flags & SURF_PLANEBACK) && (dot > BACKFACE_EPSILON)))
        {
            R_RenderBrushPoly(psurf, clientId);
        }
    }

    if (currententity->rendermode != 0)
    {
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glDisable(GL_BLEND);
    }

    if (currententity->rendermode != 4)
        R_DrawDecals();

    if (currententity->rendermode == 0)
    {
        R_RenderLightmaps();
    }
    else if (currententity->rendermode == 4)
    {
        glDisable(GL_ALPHA_TEST);
        R_RenderLightmaps();
    }

    func_0023B258();
    glDepthMask(1);
    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
    glAlphaFunc(0x205, 0);
}

void R_RecursiveWorldNode(mnode_t *node, int clientId)
{
    int c, side;
    mplane_t *plane;
    msurface_t *surf, **mark;
    mleaf_t *pleaf;
    float dot;

    if (node->contents == CONTENTS_SOLID)
        return;
    if (node->visframe[clientId] != r_visframecount[clientId])
        return;
    if (R_CullBox(node->minmaxs, node->minmaxs + 4))
        return;

    if (node->contents < 0)
    {
        pleaf = (mleaf_t *)node;
        mark = pleaf->firstmarksurface;
        c = pleaf->nummarksurfaces;
        if (c)
        {
            do
            {
                (*mark)->visframe = r_framecount;
                mark++;
            } while (--c);
        }
        if (pleaf->efrags)
            R_StoreEfrags(&pleaf->efrags, clientId);
        return;
    }

    plane = node->plane;
    switch (plane->type)
    {
    case 0:
        dot = modelorg[0] - plane->dist;
        break;
    case 1:
        dot = modelorg[1] - plane->dist;
        break;
    case 2:
        dot = modelorg[2] - plane->dist;
        break;
    default:
        dot = DotProduct(modelorg, plane->normal) - plane->dist;
        break;
    }

    if (dot >= 0)
        side = 0;
    else
        side = 1;

    R_RecursiveWorldNode(node->children[side], clientId);

    c = node->numsurfaces;
    if (c)
    {
        surf = cl[clientId].worldmodel->surfaces + node->firstsurface;

        if (dot < -BACKFACE_EPSILON)
            side = SURF_PLANEBACK;
        else if (dot > BACKFACE_EPSILON)
            side = 0;

        for (; c; c--, surf++)
        {
            if (surf->visframe != r_framecount)
                continue;
            if (surf->flags & 0x100)
                continue;
            if (!(surf->flags & SURF_UNDERWATER) && ((dot < 0) ^ !!(surf->flags & SURF_PLANEBACK)))
                continue;

            surf->texturechain = surf->texture->texturechain;
            surf->texture->texturechain = surf;
        }
    }

    R_RecursiveWorldNode(node->children[!side], clientId);
}

void R_DrawWorld(int clientId)
{
    cl_entity_t ent;

    r_blend = 1.0f;
    memset(&ent, 0, sizeof(ent));
    ent.model = cl[clientId].worldmodel;
    VectorCopy(r_refdef.vieworg, modelorg);
    currententity = &ent;
    ent.rendercolor.r = D_004E77B0[0];
    ent.rendercolor.g = D_004E77B0[1];
    ent.rendercolor.b = D_004E77B0[2];
    currenttexture = -1;
    glColor3f(1.0f, 1.0f, 1.0f);
    memset(lightmap_polys, 0, sizeof(lightmap_polys));
    R_ClearSkyBox();
    R_RecursiveWorldNode(cl[clientId].worldmodel->nodes, clientId);
    gDecalSurfCount = 0;
    DrawTextureChains(clientId);
    func_002CD9F8(clientId);
    R_DrawDecals();
    R_RenderLightmaps();
}


void R_MarkLeaves(int clientId)
{
    byte *vis;
    mnode_t *node;
    int i;
    byte solid[4096];

    if (r_oldviewleaf[clientId] == r_viewleaf[clientId] && !r_novis.value)
        return;
    if (D_00399580)
        return;

    r_visframecount[clientId]++;
    r_oldviewleaf[clientId] = r_viewleaf[clientId];

    if (r_novis.value)
    {
        vis = solid;
        memset(solid, 0xFF, (cl[clientId].worldmodel->numleafs + 7) >> 3);
    }
    else
    {
        vis = Mod_LeafPVS(r_viewleaf[clientId], cl[clientId].worldmodel);
    }

    for (i = 0; i < cl[clientId].worldmodel->numleafs; i++)
    {
        if ((vis[i >> 3] >> (i & 7)) & 1)
        {
            node = (mnode_t *)&cl[clientId].worldmodel->leafs[i + 1];
            do
            {
                if (node->visframe[clientId] == r_visframecount[clientId])
                    break;
                node->visframe[clientId] = r_visframecount[clientId];
                node = node->parent;
            } while (node);
        }
    }
}

#ifdef NON_MATCHING
int AllocBlock(int w, int h, int *x, int *y)
{
    int i, j;
    int best, best2;
    int texnum;

    for (texnum = 0; texnum < MAX_LIGHTMAPS; texnum++)
    {
        best = BLOCK_HEIGHT;

        for (i = 0; i < BLOCK_WIDTH - w; i++)
        {
            best2 = 0;

            for (j = 0; j < w; j++)
            {
                if (allocated[texnum][i + j] >= best)
                    break;
                if (allocated[texnum][i + j] > best2)
                    best2 = allocated[texnum][i + j];
            }
            if (j == w)
            {
                *x = i;
                *y = best = best2;
            }
        }

        if (best + h > BLOCK_HEIGHT)
            continue;

        for (i = 0; i < w; i++)
            allocated[texnum][*x + i] = best + h;

        return texnum;
    }

    Sys_Error("AllocBlock: full");
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", AllocBlock);
#endif

int func_002A3BC8(int a, int b)
{
    int size = a * b;

    if (size % 4 > 0)
        size += 4 - size % 4;
    return size;
}

int func_002A3C00(int w)
{
    int size;

    size = func_002A3BC8(w, 3);
    size += func_002A3BC8(w, 2);
    size += func_002A3BC8(w, 2);
    return size * 4 + 0x10;
}

void *func_002A3C60(byte *p)
{
    return p + 0x10;
}

int *func_002A3C68(byte *p)
{
    int w = *(int *)(p + 8);
    p += 0x10;
    return (int *)p + func_002A3BC8(w, 3);
}

int *func_002A3CA0(byte *p)
{
    byte *q = p + 0x10;

    q += func_002A3BC8(*(int *)(p + 8), 3) * 4;
    return (int *)q + func_002A3BC8(*(int *)(p + 8), 2);
}

void func_002A3CF8(byte *p)
{
    int size;

    size = func_002A3BC8(*(int *)(p + 8), 3);
    size += func_002A3BC8(*(int *)(p + 8), 2);
    size += func_002A3BC8(*(int *)(p + 8), 2);
    func_00225860(func_002A3C60(p), 0, size * 4);
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A3D68);

void func_002A40B8(int size)
{
    D_0040A744 = size;
    D_0040A740 = (int)Hunk_AllocName(size, "SurfaceDisplayList");
}

void func_002A40E8(void)
{
    memset(allocated, 0, sizeof(allocated));
}

void GL_CreateSurfaceLightmap(msurface_t *surf)
{
    int smax, tmax;

    if (surf->flags & 0x114)
        return;
    if ((TEXINFO_FLAGS(surf) & 0x2200000) == 0x2200000)
        return;

    smax = (surf->extents[0] >> 4) + 1;
    tmax = (surf->extents[1] >> 4) + 1;
    surf->lightmaptexturenum = AllocBlock(smax, tmax, &surf->light_s, &surf->light_t);
}

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rsurf", D_00394438);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rsurf", D_00394440);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rsurf", D_00394448);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rsurf", D_00394450);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rsurf", D_00394458);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A4180);

void func_002A4340(msurface_t *surf, int clientId)
{
    byte *base;

    if (surf->flags & 0x114)
        return;
    if ((TEXINFO_FLAGS(surf) & 0x2200000) == 0x2200000)
        return;

    base = lightmaps + surf->lightmaptexturenum * lightmap_bytes * BLOCK_WIDTH * BLOCK_HEIGHT;
    base += (surf->light_t * BLOCK_WIDTH + surf->light_s) * lightmap_bytes;
    R_BuildLightMap(surf, base, BLOCK_WIDTH * lightmap_bytes, clientId);
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A43C0);

/* "OFF"/"ON" são compartilhadas pelas duas funções; o ee-gcc não junta strings iguais */
__asm__(".section .rodata\n"
        ".align 3\nD_00394470: .asciz \"drawdecals [%s]\\n\"\n"
        ".align 3\nD_00394488: .asciz \"OFF\"\n"
        ".align 3\nD_00394490: .asciz \"ON\"\n"
        ".section .text");
extern char D_00394470[], D_00394488[], D_00394490[];

void R_DrawDecals_f(void)
{
    r_drawdecals = !r_drawdecals;
    Con_Printf(D_00394470, !r_drawdecals ? D_00394488 : D_00394490);
}

void R_DrawVLs_f(void)
{
    r_drawvls = !r_drawvls;
    Con_Printf("drawvls [%s]\n", !r_drawvls ? D_00394488 : D_00394490);
}

void R_ClearDecals(void)
{
    int i;

    memset(gDecalPool, 0, sizeof(gDecalPool));
    gDecalCount = 0;
    for (i = DECAL_CACHEENTRIES - 1; i >= 0; i--)
        gDecalCache[i].decalIndex = -1;
}

int R_DecalIndex(decal_t *pdecal)
{
    return pdecal - gDecalPool;
}

int R_DecalCacheIndex(int index)
{
    return index & (DECAL_CACHEENTRIES - 1);
}

decalcache_t *R_DecalCacheSlot(int decalIndex)
{
    int cacheIndex;

    cacheIndex = R_DecalCacheIndex(decalIndex);
    return &gDecalCache[cacheIndex];
}

void R_DecalCacheClear(decal_t *pdecal)
{
    int index;
    decalcache_t *pCache;

    index = R_DecalIndex(pdecal);
    pCache = R_DecalCacheSlot(index);
    if (pCache->decalIndex == index)
        pCache->decalIndex = -1;
}

void R_DecalUnlink(decal_t *pdecal)
{
    decal_t *tmp;

    R_DecalCacheClear(pdecal);
    if (pdecal->psurface)
    {
        if (pdecal->psurface->pdecals == pdecal)
        {
            pdecal->psurface->pdecals = pdecal->pnext;
        }
        else
        {
            tmp = pdecal->psurface->pdecals;
            if (!tmp)
                Sys_Error("Bad decal list");
            while (tmp->pnext)
            {
                if (tmp->pnext == pdecal)
                {
                    tmp->pnext = pdecal->pnext;
                    break;
                }
                tmp = tmp->pnext;
            }
        }
    }
    pdecal->psurface = NULL;
}

void func_002A4978(void)
{
    int i;

    for (i = 0; i < 1024; i++)
        ;
}

decal_t *R_DecalAlloc(decal_t *pdecal)
{
    int limit = MAX_DECALS;
    int count;

    if (r_decals.value < limit)
        limit = r_decals.value;

    if (!limit)
        return NULL;

    if (!pdecal)
    {
        count = 0;
        do
        {
            gDecalCount++;
            if (gDecalCount >= limit)
                gDecalCount = 0;
            pdecal = gDecalPool + gDecalCount;
            count++;
        } while ((pdecal->flags & FDECAL_PERMANENT) && count < limit);
    }

    R_DecalUnlink(pdecal);
    return pdecal;
}

void R_DecalRemoveAll(int textureIndex)
{
    int i;
    decal_t *pDecal;

    for (i = 0; i < MAX_DECALS; i++)
    {
        pDecal = &gDecalPool[i];
        if (pDecal->texture == textureIndex)
        {
            R_DecalUnlink(pDecal);
            memset(pDecal, 0, sizeof(decal_t));
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A4AC0);

int func_002A4FB0(decalentry_t *list, int count)
{
    int i;
    vec3_t tmp;
    decalentry_t *pnew = &list[count];

    for (i = 0; i < count; i++)
    {
        if (pnew->name == list[i].name && pnew->entityIndex == list[i].entityIndex)
        {
            VectorSubtract(pnew->position, list[i].position, tmp);
            if (Length(tmp) < 2.0f)
                return count;
        }
    }
    return count + 1;
}

int func_002A5090(byte *a, byte *b)
{
    if (a[6] > b[6])
        return -1;
    return a[6] < b[6];
}

int DecalListCreate(decalentry_t *pList, int clientId)
{
    int total = 0;
    int i, depth;
    decal_t *pdecals;

    if (cl[clientId].worldmodel)
    {
        for (i = 0; i < MAX_DECALS; i++)
        {
            if (!gDecalPool[i].psurface || (gDecalPool[i].flags & FDECAL_DONTSAVE))
                continue;

            depth = 0;
            pdecals = gDecalPool[i].psurface->pdecals;
            while (pdecals && pdecals != &gDecalPool[i])
            {
                depth++;
                pdecals = pdecals->pnext;
            }

            pList[total].depth = depth;
            pList[total].flags = gDecalPool[i].flags;
            pList[total].entityIndex = R_DecalUnProject(&gDecalPool[i], pList[total].position);
            pList[total].name = ((texture_t *)Draw_DecalTexture(gDecalPool[i].texture))->name;
            total = func_002A4FB0(pList, total);
        }
    }

    qsort(pList, total, sizeof(decalentry_t), (int (*)(const void *, const void *))func_002A5090);
    return total;
}

int R_DecalUnProject(decal_t *pdecal, vec_t *position)
{
    texture_t *ptexture;
    texture_t *pdtex;
    float scale, inverseScale;
    float x, y;
    edict_t *pEdict;
    model_t *pModel;
    int entityIndex;
    vec3_t temp;
    vec3_t forward, right, up;
    hull_t *phull;

    if (!pdecal || !pdecal->psurface)
        return -1;

    ptexture = pdecal->psurface->texture;
    x = pdecal->dx;
    y = pdecal->dy;
    x = x * ptexture->srcwidth - pdecal->psurface->texturemins[0];
    y = y * ptexture->srcheight - pdecal->psurface->texturemins[1];

    scale = Length(pdecal->psurface->vecs[0]) * 0.5f;
    pdtex = Draw_DecalTexture(pdecal->texture);
    x = x + pdtex->srcwidth * scale + pdecal->psurface->texturemins[0] - pdecal->psurface->vecs[0][3];
    y = y + pdtex->srcheight * scale + pdecal->psurface->texturemins[1] - pdecal->psurface->vecs[1][3];

    inverseScale = 0.0f;
    scale = __builtin_fabsf(Length(pdecal->psurface->vecs[0]));
    if (scale != 0.0f)
    {
        inverseScale = 1.0f / scale;
        inverseScale *= inverseScale;
    }

    VectorScale(pdecal->psurface->vecs[0], inverseScale * x, position);
    VectorMA(position, inverseScale * y, pdecal->psurface->vecs[1], position);
    VectorMA(position, pdecal->psurface->plane->dist, pdecal->psurface->plane->normal, position);

    entityIndex = pdecal->entityIndex;
    if (entityIndex)
    {
        pEdict = &sv.edicts[entityIndex];
        pModel = pEdict->v.modelindex ? sv.models[pEdict->v.modelindex] : NULL;
        if (!pModel || pModel->type != mod_brush)
            return 0;
        phull = &pModel->hulls[0];

        if (pEdict->v.angles[0] || pEdict->v.angles[1] || pEdict->v.angles[2])
        {
            AngleVectorsTranspose(pEdict->v.angles, forward, right, up);
            VectorCopy(position, temp);
            position[0] = DotProduct(temp, forward);
            position[1] = DotProduct(temp, right);
            position[2] = DotProduct(temp, up);
        }

        if (pModel->firstmodelsurface)
        {
            VectorAdd(position, phull->clip_mins, position);
            VectorAdd(position, pEdict->v.origin, position);
        }
    }

    return entityIndex;
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", R_DecalShoot_);

void R_DecalShoot(int textureIndex, int entity, int modelIndex, int position, int flags, int unk)
{
    R_DecalShoot_((int)Draw_DecalTexture(textureIndex), textureIndex, entity, modelIndex, position, flags, unk);
}

void func_002A5948(int a0, int a1, int a2, int a3, int t0, int t1, int t2)
{
    R_DecalShoot_(a0, ~a1, a2, a3, t0, t1, t2);
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", R_DecalIntersect);

void R_DecalCreate(msurface_t *psurface, int textureIndex, float scale, float x, float y)
{
    decal_t *pdecal, *pold;
    int count;

    pold = R_DecalIntersect(psurface, &count, x, y);
    if (count < MAX_OVERLAP_DECALS)
        pold = NULL;

    pdecal = R_DecalAlloc(pold);
    pdecal->flags = gDecalFlags;
    pdecal->dx = x;
    pdecal->dy = y;
    pdecal->texture = textureIndex;
    pdecal->pnext = NULL;

    pold = psurface->pdecals;
    if (!pold)
    {
        psurface->pdecals = pdecal;
    }
    else
    {
        while (pold->pnext)
            pold = pold->pnext;
        pold->pnext = pdecal;
    }

    pdecal->psurface = psurface;
    pdecal->scale = scale;
    pdecal->entityIndex = gDecalEntity;
}



int Inside(float *vert, int edge)
{
    switch (edge)
    {
    case 0:
        if (vert[3] > 0.0f)
            return 1;
        return 0;
    case 1:
        if (vert[3] < 1.0f)
            return 1;
        return 0;
    case 2:
        if (vert[4] > 0.0f)
            return 1;
        return 0;
    case 3:
        if (vert[4] < 1.0f)
            return 1;
        return 0;
    }
    return 0;
}

void Intersect(float *one, float *two, int edge, float *out)
{
    float t;

    if (edge < 2)
    {
        if (edge == 0)
        {
            t = one[3] / (one[3] - two[3]);
            out[3] = 0;
        }
        else
        {
            t = (one[3] - 1.0f) / (one[3] - two[3]);
            out[3] = 1.0f;
        }
        out[4] = one[4] + (two[4] - one[4]) * t;
    }
    else
    {
        if (edge == 2)
        {
            t = one[4] / (one[4] - two[4]);
            out[4] = 0;
        }
        else
        {
            t = (one[4] - 1.0f) / (one[4] - two[4]);
            out[4] = 1.0f;
        }
        out[3] = one[3] + (two[3] - one[3]) * t;
    }
    out[0] = one[0] + (two[0] - one[0]) * t;
    out[1] = one[1] + (two[1] - one[1]) * t;
    out[2] = one[2] + (two[2] - one[2]) * t;
}

int SHClip(float *vert, int vertCount, float *out, int edge)
{
    int j, outCount;
    float *s, *p;

    outCount = 0;
    s = &vert[(vertCount - 1) * 7];
    for (j = 0; j < vertCount; j++)
    {
        p = &vert[j * 7];
        if (Inside(p, edge))
        {
            if (Inside(s, edge))
            {
                memcpy(out, p, sizeof(float) * 7);
                outCount++;
                out += 7;
            }
            else
            {
                Intersect(s, p, edge, out);
                out += 7;
                outCount++;
                memcpy(out, p, sizeof(float) * 7);
                outCount++;
                out += 7;
            }
        }
        else
        {
            if (Inside(s, edge))
            {
                Intersect(p, s, edge, out);
                out += 7;
                outCount++;
            }
        }
        s = p;
    }
    return outCount;
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", R_DecalVertsClip);

float *R_DecalCacheVerts(decal_t *pdecal, msurface_t *psurf, texture_t *ptexture)
{
    int decalIndex;
    int outCount;
    decalcache_t *pCache;

    decalIndex = R_DecalIndex(pdecal);
    pCache = R_DecalCacheSlot(decalIndex);
    if (pCache->decalIndex == decalIndex)
        return pCache->decalVert;

    pCache->decalIndex = decalIndex;
    return R_DecalVertsClip(pCache->decalVert, pdecal, psurf, ptexture, &outCount);
}

void func_002A6380(void)
{
    if (D_00333260 > 0)
    {
        func_0023D520();
        D_00333260 = 0;
        D_00333254 = !D_00333254;
    }
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A63C8);

void func_002A6518(float *v, int count)
{
    float *pv, *pt;

    func_002A63C8(count);
    for (; count; count--, v += 7)
    {
        pv = D_00333258;
        pt = D_0033325C;
        pv[0] = v[0];
        pv[1] = v[1];
        pv[2] = v[2];
        pv[3] = 1.0f;
        D_00333258 = pv + 4;
        pt[0] = v[3];
        pt[1] = v[4];
        pt[2] = 1.0f;
        pt[3] = 0;
        D_0033325C = pt + 4;
    }
}

void R_DrawDecals(void)
{
    decal_t *plist;
    int i, outCount;
    texture_t *ptexture;
    msurface_t *psurf;
    int lasttex;
    float *v;

    lasttex = -1;
    if (!r_drawdecals)
        return;
    if (!gDecalSurfCount)
        return;

    glEnable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glDepthMask(0);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, gl_polyoffset.value);

    psurf = gDecalSurfs[0];
    plist = psurf->pdecals;
    Draw_DecalTexture(plist->texture);

    D_00331044 = 5;
    D_0033102C = 1;
    for (i = 0; i < gDecalSurfCount; i++)
    {
        psurf = gDecalSurfs[i];
        plist = psurf->pdecals;
        while (plist)
        {
            ptexture = Draw_DecalTexture(plist->texture);
            if (!ptexture)
            {
                plist = plist->pnext;
                continue;
            }
            if (!(plist->flags & FDECAL_NOCLIP))
                v = R_DecalVertsClip(NULL, plist, psurf, ptexture, &outCount);
            if (plist->flags & FDECAL_NOCLIP)
            {
                v = R_DecalCacheVerts(plist, psurf, ptexture);
                outCount = 4;
            }
            if (outCount)
            {
                if (lasttex != ptexture->gl_texturenum)
                {
                    func_002A6380();
                    lasttex = ptexture->gl_texturenum;
                    GL_Bind(lasttex);
                    func_0023AF68();
                }
                func_002A6518(v, outCount);
            }
            plist = plist->pnext;
        }
    }

    func_002A6380();
    D_0033102C = 0;
    glDisable(GL_POLYGON_OFFSET_FILL);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
    glDepthMask(1);
    gDecalSurfCount = 0;
}

void func_002A67E0(void)
{
    int i, j, k;

    for (i = 0; i < 20; i++)
    {
        for (j = 0; j < 20; j++)
            rtable2[i][j] = RandomLong(0, 0x7FFF);
    }

    D_00413620 = D_00413630[0];
    for (k = 0; k < 74; k++)
    {
        D_00413630[k][0] = 1.0f;
        D_00413630[k][1] = 1.0f;
        D_00413630[k][2] = 1.0f;
        D_00413630[k][3] = 1.0f;
    }

    func_002EF8E0(0);
    D_00413AD0 = 0;
    D_00333264 = 1;
}

void func_002A68C0(void)
{
    if (D_00333264 == 1)
        D_00413AD0++;
}

void DrawTextureChains(int clientId)
{
    int i;
    msurface_t *s;
    texture_t *t;
    int count = 100;

    currententity = cl_entities[clientId];
    for (i = 0; i < cl[clientId].worldmodel->numtextures; i++)
    {
        t = &cl[clientId].worldmodel->textures[i];
        if (!t)
            continue;
        s = t->texturechain;
        if (!s)
            continue;

        if (i == D_00399520)
        {
            if (D_00399590)
            {
                glDisable(GL_FOG);
                func_0023AFC8(1);
            }
            R_DrawSkyChain(s);
            if (D_00399590)
            {
                glEnable(GL_FOG);
                func_0023AFC8(3);
            }
        }
        else
        {
            if ((s->flags & SURF_DRAWTURB) && r_wateralpha.value != 1.0f)
                continue;
            R_RenderBrushPolyChain(s, clientId);
        }
        t->texturechain = NULL;

        if (--count == 0)
        {
            func_002CD9F8(clientId);
            count = 100;
        }
    }
}

#ifdef NON_MATCHING
void R_BlendLightmaps(void)
{
    int i;
    glpoly_t *p;
    qboolean *pmod;
    byte *base;

    D_00331044 = 5;
    D_0033102C = 1;
    for (i = 0; i < MAX_LIGHTMAPS; i++)
    {
        p = lightmap_polys[i];
        if (!p)
            continue;

        pmod = &lightmap_modified[i];
        base = lightmaps + i * BLOCK_WIDTH * BLOCK_HEIGHT * 4;
        GL_Bind(lightmap_textures + i);
        if (*pmod)
        {
            *pmod = false;
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, BLOCK_WIDTH, BLOCK_HEIGHT, gl_lightmap_format, GL_UNSIGNED_BYTE, base);
        }
        func_0023AF68();
        for (; p; p = p->chain)
        {
            D_00331030 = func_002A3C60((byte *)p);
            D_00331038 = func_002A3CA0((byte *)p);
            D_0033103C = p->numverts;
            func_0023D430(1);
        }
    }
    D_0033102C = 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", R_BlendLightmaps);
#endif

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A6BE0);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A6CA0);

void DrawGLPolyScroll(msurface_t *psurface, cl_entity_t *pEntity)
{
    glpoly_t *p;
    float *v, *st;
    int i;
    float sOffset;

    sOffset = ScrollOffset(psurface, pEntity);
    p = psurface->polys;
    if (!p)
        return;

    v = func_002A3C60((byte *)p);
    st = (float *)func_002A3C68((byte *)p);
    glBegin(GL_POLYGON);
    D_00399584++;
    D_00399560 += p->numverts;
    for (i = 0; i < p->numverts; i++, v += 3, st += 2)
    {
        glTexCoord2f(st[0] + sOffset, st[1]);
        glVertex3fv(v);
    }
    glEnd();
}

#ifdef NON_MATCHING
void R_RenderBrushPolyChain(msurface_t *s, int clientId)
{
    texture_t *t;
    texture_t *lasttex = NULL;
    msurface_t *scrollchain = NULL;
    msurface_t *next;
    int special;
    int is_dynamic;
    int maps;
    byte *base;

    for (; s && !(s->flags & 0x104); s = next)
    {
        t = s->texture;
        if (currententity->frame)
        {
            if (t->alternate_anims)
                t = t->alternate_anims;
        }
        if (t->anim_total)
            t = R_TextureAnimation2(s, t);

        if (!(TEXINFO_FLAGS(s) & 0x4200000))
        {
            if (lasttex != t || D_00333268 != s->unk_3C)
            {
                func_0023D520();
                lasttex = t;
                D_00333268 = s->unk_3C;
                GL_Bind(t->gl_texturenum);
                func_0023AF68();
            }
        }

        special = 0;
        if (s->flags & 0x400)
        {
            special = 1;
        }
        else
        {
            if (s->flags & SURF_DRAWSCROLL)
            {
                special = 1;
            }
            else
            {
                D_0032F26C = s->unk_3C;
                func_002A7238((byte *)s->polys);
                D_0032F26C = 0;
            }

            s->polys->chain = lightmap_polys[s->lightmaptexturenum];
            lightmap_polys[s->lightmaptexturenum] = s->polys;

            if (s->pdecals)
            {
                gDecalSurfs[gDecalSurfCount] = s;
                gDecalSurfCount++;
            }

            is_dynamic = 0;
            if (s->dlightframe == r_framecount)
            {
                is_dynamic = 1;
            }
            else if (s->cached_dlight)
            {
                is_dynamic = 1;
            }
            else
            {
                for (maps = 0; maps < MAXLIGHTMAPS && s->styles[maps] != 255; maps++)
                {
                    if (d_lightstylevalue[s->styles[maps]] != s->cached_light[maps])
                    {
                        is_dynamic = 1;
                        break;
                    }
                }
            }

            if (is_dynamic == 1)
            {
                lightmap_modified[s->lightmaptexturenum] = true;
                base = lightmaps + s->lightmaptexturenum * lightmap_bytes * BLOCK_WIDTH * BLOCK_HEIGHT;
                base += s->light_t * BLOCK_WIDTH * lightmap_bytes + s->light_s * lightmap_bytes;
                func_002A72F0(s, base, BLOCK_WIDTH * lightmap_bytes, clientId);
            }
        }

        next = s->texturechain;
        if (special == 1)
        {
            s->texturechain = scrollchain;
            scrollchain = s;
        }
    }

    func_0023D520();
    D_00333268 = 0;

    for (s = scrollchain; s && !(s->flags & 0x104); s = s->texturechain)
    {
        t = s->texture;
        if (currententity->frame)
        {
            if (t->alternate_anims)
                t = t->alternate_anims;
        }
        if (t->anim_total)
            t = R_TextureAnimation2(s, t);

        if (lasttex != t)
        {
            lasttex = t;
            GL_Bind(t->gl_texturenum);
            func_0023AF68();
        }

        if (s->flags & 0x400)
        {
            func_002A7AF8(s);
        }
        else if (s->flags & SURF_DRAWSCROLL)
        {
            D_0032F26C = s->unk_3C;
            DrawGLPolyScroll(s, currententity);
            D_0032F26C = 0;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", R_RenderBrushPolyChain);
#endif

#ifdef NON_MATCHING
void func_002A7238(byte *p)
{
    int *active = &D_0033102C;
    void **a = &D_00331030;
    void **b = &D_00331038;

    D_00399560 += *(int *)(p + 8);
    D_00399584++;
    *a = func_002A3C60(p);
    D_00331034 = D_00413620;
    *b = func_002A3C68(p);
    D_00331044 = 5;
    *active = 1;
    D_0033103C = *(int *)(p + 8);
    func_0023D618(1);
    *active = 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A7238);
#endif

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A72F0);

texture_t *R_TextureAnimation2(msurface_t *s, texture_t *base)
{
    int reletive;
    int tx, ty;

    if (base->name[0] == '-' && base->anim_total)
    {
        tx = s->texturemins[0] + (base->srcwidth << 16);
        tx /= base->srcwidth;
        ty = s->texturemins[1] + (base->srcheight << 16);
        ty /= base->srcheight;
        reletive = rtable2[tx % 20][ty % 20] % base->anim_total;
    }
    else
    {
        reletive = (int)(cl[0].time * 10) % base->anim_total;
    }

    while (base->anim_min > reletive || base->anim_max <= reletive)
        base = base->anim_next;

    return base;
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A76F8);

float ScrollOffset(msurface_t *psurface, cl_entity_t *pEntity)
{
    float speed, sOffset;

    speed = (float)((pEntity->rendercolor.g << 8) + pEntity->rendercolor.b) / 16.0f;
    if (pEntity->rendercolor.r == 0)
        speed = -speed;

    sOffset = speed * cl[0].time * (1.0f / (float)psurface->texture->srcwidth);
    if (sOffset < 0)
        sOffset = fmodf(sOffset, -1.0f);
    else
        sOffset = fmodf(sOffset, 1.0f);
    return sOffset;
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A7AF8);
