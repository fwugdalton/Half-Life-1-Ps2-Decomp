#include "common.h"
#include "engine/cmd.h"
#include "engine/cvar.h"
#include "engine/client.h"
#include "engine/render.h"
#include "engine/model.h"
#include "engine/glquake.h"
#include "engine/cl_entity.h"
#include "engine/studio.h"

void *Mod_Extradata(model_t *mod);
void *Cache_Check(cache_user_t *c);
void Cache_Free(cache_user_t *c);
void *memset(void *dst, int c, unsigned int n);

/* casco de colisão por osso (SV_InitStudioHull) */
extern hull_t studio_hull[MAXSTUDIOBONES];              /* studio_hull */
extern int studio_hull_hitgroup[MAXSTUDIOBONES];        /* studio_hull_hitgroup */
extern dclipnode_t studio_clipnodes[6];                 /* studio_clipnodes */
extern mplane_t studio_planes[MAXSTUDIOBONES * 6];      /* studio_planes */

extern byte rgStudioCache[0x440];
extern int r_cachecurrent;
extern int nCurrentHull;
extern int nCurrentPlane;

/* sprite do PS2 (ver gl_sprite_t em Gl_model.c) */
typedef struct
{
    byte pad0[7];
    byte numframes;             /* 0x07 */
} msprite_t;

typedef struct
{
    float m[3][4];
} matrix3x4_t;

int R_StudioBodyVariations(model_t *model);
void func_00228CF0(char *dst, char *src);
void func_0023D520(void);
int strcmp(const char *a, const char *b);

extern int r_dointerp;
extern int D_003342A0;

/* lote de triângulos do studio */
typedef struct
{
    int texnum;                 /* 0x00 */
    unsigned int unk04;         /* 0x04 */
    int count;                  /* 0x08 */
    int unk0C;
    int unk10;
    int unk14;
    int unk18;
} studiobatch_t;
extern studiobatch_t D_00439500;

typedef struct
{
    char name[0x104];           /* 0x000 */
    char modelname[0x104];      /* 0x104 */
    int unk208;                 /* 0x208 */
} playermodel_t;                /* 0x20C */
extern playermodel_t r_playermodels[3];
extern studiohdr_t *pstudiohdr;
extern float bonetransform[MAXSTUDIOBONES][3][4];
extern cl_entity_t *currententity;

void SV_InitStudioHull(void)
{
    int i;
    int side;

    if (studio_hull[0].planes != NULL)
        return;

    for (i = 0; i < 6; i++)
    {
        studio_clipnodes[i].planenum = i;
        side = i & 1;
        studio_clipnodes[i].children[side] = CONTENTS_EMPTY;
        if (i != 5)
            studio_clipnodes[i].children[side ^ 1] = i + 1;
        else
            studio_clipnodes[i].children[side ^ 1] = CONTENTS_SOLID;
    }

    for (i = 0; i < MAXSTUDIOBONES; i++)
    {
        studio_hull[i].clipnodes = studio_clipnodes;
        studio_hull[i].planes = &studio_planes[i * 6];
        studio_hull[i].firstclipnode = 0;
        studio_hull[i].lastclipnode = 5;
    }
}

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002B91D8);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002B9330);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002B9538);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002B9648);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002B98F8);

INCLUDE_RODATA("asm/nonmatchings/engine/r_studio", D_00394FA0);

INCLUDE_RODATA("asm/nonmatchings/engine/r_studio", D_00394FA8);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002B9A60);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002B9CF0);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002B9FE8);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BA1B8);

INCLUDE_RODATA("asm/nonmatchings/engine/r_studio", D_00395040);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BA320);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BA420);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BA4D8);

void SV_SetStudioHullPlane(mplane_t *pl, int iBone, int k, float dist)
{
    pl->type = 5;
    pl->normal[0] = bonetransform[iBone][0][k];
    pl->normal[1] = bonetransform[iBone][1][k];
    pl->normal[2] = bonetransform[iBone][2][k];
    pl->dist = bonetransform[iBone][0][3] * pl->normal[0]
             + bonetransform[iBone][1][3] * pl->normal[1]
             + bonetransform[iBone][2][3] * pl->normal[2]
             + dist;
}

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", R_StudioHull);

int SV_HitgroupForStudioHull(int index)
{
    return studio_hull_hitgroup[index];
}

void R_ResetStudio(void)
{
    memset(rgStudioCache, 0, sizeof(rgStudioCache));
    r_cachecurrent = 0;
    nCurrentHull = 0;
    nCurrentPlane = 0;
}

void R_FlushStudioCache(void)
{
    R_ResetStudio();
}

int R_StudioBodyVariations(model_t *model)
{
    studiohdr_t *shdr;
    mstudiobodyparts_t *pbodypart;
    int i, count;

    if (model->type != mod_studio)
        return 0;

    shdr = (studiohdr_t *)Mod_Extradata(model);
    if (!shdr)
        return 0;

    count = 1;
    pbodypart = (mstudiobodyparts_t *)((byte *)shdr + shdr->bodypartindex);
    for (i = 0; i < shdr->numbodyparts; i++)
        count = count * pbodypart[i].nummodels;

    return count;
}

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BB168);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", SV_HullForStudioModel);

void func_002BB4F8(void) {
}

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BB500);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BB5E0);

int ModelFrameCount(model_t *model)
{
    int count;

    count = 1;
    if (!model)
        return 1;

    if (model->type == mod_sprite)
        count = ((msprite_t *)model->cache.data)->numframes;
    else if (model->type == mod_studio)
        count = R_StudioBodyVariations(model);

    if (count < 1)
        count = 1;
    return count;
}

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BB740);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BB838);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BB930);

void R_StudioIdentityMatrix(float m[4][4])
{
    m[0][0] = 1.0f;
    m[1][0] = 0.0f;
    m[2][0] = 0.0f;
    m[3][0] = 0.0f;
    m[0][1] = 0.0f;
    m[1][1] = 1.0f;
    m[2][1] = 0.0f;
    m[3][1] = 0.0f;
    m[0][2] = 0.0f;
    m[1][2] = 0.0f;
    m[2][2] = 1.0f;
    m[3][2] = 0.0f;
    m[0][3] = 0.0f;
    m[1][3] = 0.0f;
    m[2][3] = 0.0f;
    m[3][3] = 1.0f;
}

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BBA78);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BC048);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BC188);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BC240);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BC520);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BC808);

float R_StudioEstimateInterpolant(int clientId)
{
    float dadt;

    if (r_dointerp && currententity->animtime >= currententity->prevanimtime + 0.01f)
    {
        dadt = (cl[clientId].time - currententity->animtime) * 10.0f;
        if (!(dadt > 2.0f))
            return dadt;
        return 2.0f;
    }
    return 1.0f;
}

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BC930);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BCA88);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BCBE8);

void MatrixCopy(matrix3x4_t *in, matrix3x4_t *out)
{
    *out = *in;
}

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BD468);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BD568);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BD828);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BDB30);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BDDC0);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BE2D0);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BE3C0);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BE5A0);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BE780);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BEA90);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BECB8);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BED78);

void func_002BEF98(void)
{
    int i;

    for (i = 0; i < 3; i++)
    {
        func_00228CF0(r_playermodels[i].name, "");
        func_00228CF0(r_playermodels[i].modelname, "");
        r_playermodels[i].unk208 = 0;
    }
}

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BF028);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BF0E8);


INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BF1C8);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BF2B0);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BF430);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", R_StudioDrawModel);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BFB68);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BFEA8);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002C0258);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", R_StudioDrawPlayer);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002C0A50);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002C14C8);

struct sfx_s *func_002C1800(char *name, int clientId)
{
    int i;
    struct sfx_s *sfx;

    for (i = 0; i < 512; i++)
    {
        sfx = cl[clientId].sound_precache[i];
        if (sfx && !strcmp(name, (char *)sfx))
            return sfx;
    }
    return NULL;
}

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002C1890);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002C1A68);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002C1E10);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002C1EC0);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002C2260);

int func_002C2AF0(int bodypart)
{
    mstudiobodyparts_t *pbodypart;

    pbodypart = (mstudiobodyparts_t *)((byte *)pstudiohdr + pstudiohdr->bodypartindex) + bodypart;
    if (bodypart > pstudiohdr->numbodyparts)
        return 0;
    if (pbodypart->nummodels < 2)
        return 0;
    return (currententity->body / pbodypart->base) % pbodypart->nummodels;
}

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002C2B68);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002C2BF8);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002C2CE0);

void func_002C3038(void)
{
    D_00439500.texnum = -1;
    D_00439500.unk04 = 0xFFFFFFFF;
    D_00439500.unk10 = 0;
    D_00439500.unk14 = 0;
    D_00439500.unk18 = 0;
    D_00439500.count = 0;
    D_00439500.unk0C = 0;
}

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002C3070);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002C3A80);

void func_002C3C90(void)
{
    if (D_00439500.count > 0)
    {
        func_0023D520();
        D_003342A0 = !D_003342A0;
    }
    D_00439500.count = 0;
}

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002C3CE0);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002C43A0);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002C4460);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002C4840);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002C48D8);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002C4B00);
