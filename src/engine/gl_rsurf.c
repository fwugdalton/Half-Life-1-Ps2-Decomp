#include "common.h"
#include "engine/model.h"
#include "engine/mathlib.h"
#include "engine/cl_entity.h"
#include "engine/client.h"
#include "engine/cvar.h"

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
    byte pad[0x1C];
} decal_t;

typedef struct
{
    byte pad[0x74];
} decalcache_t;

#define DECAL_CACHEENTRIES 256
extern decal_t gDecalPool[];
extern decalcache_t gDecalCache[DECAL_CACHEENTRIES];
extern int D_00333264, D_00413AD0;
extern byte D_004F0DE8[0x8000];
void R_DecalShoot_(int, int, int, int, int, int, int, int);

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
        rad -= fabs(dist);
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

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A2908);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A29F8);

/* tabela de floats alinhada em 8 no original (até virar C) */
__asm__(".section .rodata\n.align 3\n.section .text");
INCLUDE_RODATA("asm/nonmatchings/engine/gl_rsurf", D_003937E8);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rsurf", D_00393830);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rsurf", D_00393930);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rsurf", D_00393BE8);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rsurf", D_00393FE8);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A2AF0);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A2D80);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", R_DrawBrushModel);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A3468);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A3738);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", R_MarkLeaves);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A3A40);

int func_002A3BC8(int a, int b)
{
    int size = a * b;

    if (size % 4 > 0)
        size += 4 - size % 4;
    return size;
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A3C00);

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

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A3CA0);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A3CF8);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A3D68);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A40B8);

void func_002A40E8(void)
{
    memset(D_004F0DE8, 0, sizeof(D_004F0DE8));
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A4110);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rsurf", D_00394438);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rsurf", D_00394440);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rsurf", D_00394448);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rsurf", D_00394450);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rsurf", D_00394458);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A4180);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A4340);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A43C0);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", R_DrawDecals_f);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rsurf", D_00394488);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_rsurf", D_00394490);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", R_DrawVLs_f);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A47F0);

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

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A48A0);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A48E8);

void func_002A4978(void)
{
    int i;

    for (i = 0; i < 1024; i++)
        ;
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A4990);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A4A50);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A4AC0);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A4FB0);

int func_002A5090(byte *a, byte *b)
{
    if (a[6] > b[6])
        return -1;
    return a[6] < b[6];
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A50B8);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A5230);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", R_DecalShoot_);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A58C8);

void func_002A5948(int a0, int a1, int a2, int a3, int t0, int t1, int t2, int t3)
{
    R_DecalShoot_(a0, ~a1, a2, a3, t0, t1, t2, t3);
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A5968);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A5C50);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A5D30);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A5DE0);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A5EE8);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A6050);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A62F8);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A6380);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A63C8);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A6518);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A65C0);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A67E0);

void func_002A68C0(void)
{
    if (D_00333264 == 1)
        D_00413AD0++;
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A68E8);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A6A88);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A6BE0);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A6CA0);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A6DA0);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A6E78);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A7238);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A72F0);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A7600);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A76F8);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A7A48);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rsurf", func_002A7AF8);
