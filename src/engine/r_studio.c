#include "common.h"
#include "engine/cmd.h"
#include "engine/cvar.h"
#include "engine/client.h"
#include "engine/render.h"
#include "engine/model.h"
#include "engine/glquake.h"
#include "engine/cl_entity.h"
#include "engine/studio.h"
#include "engine/server.h"

void *Mod_Extradata(model_t *mod);
void *Cache_Check(cache_user_t *c);
void Cache_Free(cache_user_t *c);
void *memset(void *dst, int c, unsigned int n);

/* casco de colisão por osso (SV_InitStudioHull) */
extern hull_t studio_hull[MAXSTUDIOBONES];              /* studio_hull */
extern int studio_hull_hitgroup[MAXSTUDIOBONES];        /* studio_hull_hitgroup */
extern dclipnode_t studio_clipnodes[6];                 /* studio_clipnodes */
extern mplane_t studio_planes[MAXSTUDIOBONES * 6];      /* studio_planes */

#define STUDIO_CACHE_SIZE 16
#define STUDIO_CACHEMASK (STUDIO_CACHE_SIZE - 1)

typedef struct r_studiocache_s
{
    float frame;                /* 0x00 */
    int sequence;               /* 0x04 */
    vec3_t angles;              /* 0x08 */
    vec3_t origin;              /* 0x14 */
    vec3_t size;                /* 0x20 */
    byte controller[4];         /* 0x2C */
    byte blending[2];           /* 0x30 */
    model_t *pModel;            /* 0x34 */
    int nStartHull;             /* 0x38 */
    int nStartPlane;            /* 0x3C */
    int numhulls;               /* 0x40 */
} r_studiocache_t;              /* 0x44 */

extern r_studiocache_t rgStudioCache[STUDIO_CACHE_SIZE];
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
extern vec3_t r_origin;
extern vec3_t chrome_origin;
extern int g_ForcedFaceFlags;
extern struct model_s *cl_shellchrome;
int tri_SpriteTexture(struct model_s *pSpriteModel, int frame);
void R_StudioRenderFinal(void);
int func_002C2AF0(int bodypart);
void *Mem_DbgCalloc(int num, int size, const char *file, int line);
void *COM_LoadFileCached(char *path, cache_user_t *cu, int *pLength);
void Con_DPrintf(const char *fmt, ...);
model_t *Mod_ForName(char *name, qboolean crash);
char *strcpy(char *dst, const char *src);
int strlen(const char *s);

extern float D_0053CF68[3][4];
extern mstudiobodyparts_t *pbodypart;
extern mstudiomodel_t *psubmodel;

extern int m_nCachedBones;
extern char m_nCachedBoneNames[MAXSTUDIOBONES][32];
extern float m_rgCachedBoneTransform[MAXSTUDIOBONES][3][4];
extern float lighttransform[MAXSTUDIOBONES][3][4];
extern float m_rgCachedLightTransform[MAXSTUDIOBONES][3][4];
void MatrixCopy(float in[3][4], float out[3][4]);

void SV_StudioSetupBones(model_t *pModel, float frame, int sequence, vec3_t angles, vec3_t origin,
                         byte *pcontroller, byte *pblending, int iBone);
void *func_00244928(char *name, int *pLength);
void *Cache_Alloc(cache_user_t *c, int size, char *name);
void *memcpy(void *dst, const void *src, unsigned int n);
void Mem_Free(void *p);
int func_002E4560(const void *a, const void *b, int n);
int strcasecmp(const char *a, const char *b);
void Con_Printf(const char *fmt, ...);
void GL_Bind(int texnum);

extern int D_0039967C;
extern int D_00399698;
extern int D_003996C0;

/* no PS2 o número da textura GL fica nos últimos 4 bytes do nome */
#define STUDIO_TEXNUM(ptex) (*(int *)&(ptex)->name[60])
float sinf(float);
float cosf(float);

/* cache de texturas de modelo por índice (0x12C bytes) */
typedef struct
{
    int index;                  /* 0x000 */
    int unk004;                 /* 0x004 */
    int unk008;                 /* 0x008 */
    char *unk00C;               /* 0x00C */
    char name[0x104];           /* 0x010 */
    int texindex;               /* 0x114 */
    int unk118;                 /* 0x118 */
    int width;                  /* 0x11C */
    int height;                 /* 0x120 */
    cache_user_t cache;         /* 0x124 */
    int unk128;                 /* 0x128 */
} studiotexcache_t;

studiotexcache_t *func_002BF028(int index);
extern studiotexcache_t *D_0053C2E8[];
extern studiotexcache_t D_005377E8[64];
extern int D_003996A4;

#define kRenderFxGlowShell 19

/* extensão do PS2 no fim do studiohdr_t */
typedef struct
{
    int unk00;
    byte pad04[0x14 - 0x04];
} mstudiounkF4_t;               /* 0x14 */
#define STUDIOHDR_UNKF4(h) (*(int *)((byte *)(h) + 0xF4))
#define STUDIOHDR_UNKF8(h) (*((byte *)(h) + 0xF8))
#define STUDIOHDR_UNKF9(h) (*((byte *)(h) + 0xF9))
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

r_studiocache_t *R_CheckStudioCache(model_t *pModel, float frame, int sequence, vec3_t angles, vec3_t origin,
                                    vec3_t size, byte *controller, byte *blending)
{
    int i;
    r_studiocache_t *pCached;

    for (i = 0; i < STUDIO_CACHE_SIZE; i++)
    {
        pCached = &rgStudioCache[(r_cachecurrent - i) & STUDIO_CACHEMASK];

        if (pCached->pModel == pModel && pCached->frame == frame && pCached->sequence == sequence
            && VectorCompare(pCached->angles, angles) && VectorCompare(pCached->origin, origin)
            && VectorCompare(pCached->size, size)
            && !func_002E4560(pCached->controller, controller, 4)
            && !func_002E4560(pCached->blending, blending, 2))
        {
            return pCached;
        }
    }
    return NULL;
}

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002B9330);

void AngleQuaternion(vec3_t angles, float quaternion[4])
{
    float angle;
    float sr, sp, sy, cr, cp, cy;

    angle = angles[2] * 0.5f;
    sy = sinf(angle);
    cy = cosf(angle);
    angle = angles[1] * 0.5f;
    sp = sinf(angle);
    cp = cosf(angle);
    angle = angles[0] * 0.5f;
    sr = sinf(angle);
    cr = cosf(angle);

    quaternion[0] = sr * cp * cy - cr * sp * sy;
    quaternion[1] = cr * sp * cy + sr * cp * sy;
    quaternion[2] = cr * cp * sy - sr * sp * cy;
    quaternion[3] = cr * cp * cy + sr * sp * sy;
}

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002B9648);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002B98F8);

INCLUDE_RODATA("asm/nonmatchings/engine/r_studio", D_00394FA0);

INCLUDE_RODATA("asm/nonmatchings/engine/r_studio", D_00394FA8);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002B9A60);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002B9CF0);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002B9FE8);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BA1B8);


mstudioanim_t *R_GetAnim(model_t *pmodel, mstudioseqdesc_t *pseqdesc)
{
    mstudioseqgroup_t *pseqgroup;
    cache_user_t *paSequences;

    pseqgroup = (mstudioseqgroup_t *)((byte *)pstudiohdr + pstudiohdr->seqgroupindex) + pseqdesc->seqgroup;

    if (pseqdesc->seqgroup == 0)
        return (mstudioanim_t *)((byte *)pstudiohdr + pseqgroup->data + pseqdesc->animindex);

    paSequences = (cache_user_t *)pmodel->submodels;
    if (paSequences == NULL)
    {
        paSequences = (cache_user_t *)Mem_DbgCalloc(MAXSTUDIOGROUPS + 1, sizeof(cache_user_t), "engine/R_studio.c", 636);
        pmodel->submodels = (void *)paSequences;
    }

    if (!Cache_Check(&paSequences[pseqdesc->seqgroup]))
    {
        Con_DPrintf("Loading sequence [%s]\n", pseqgroup->name);
        COM_LoadFileCached(pseqgroup->name, &paSequences[pseqdesc->seqgroup], NULL);
    }

    return (mstudioanim_t *)((byte *)paSequences[pseqdesc->seqgroup].data + pseqdesc->animindex);
}

void R_StudioLoadSeqGroup(model_t *pmodel, int seqgroup)
{
    studiohdr_t *phdr;
    mstudioseqgroup_t *pseqgroup;
    cache_user_t *paSequences;

    if (!seqgroup)
        return;

    phdr = (studiohdr_t *)Mod_Extradata(pmodel);
    if (seqgroup >= phdr->numseqgroups)
        return;

    pseqgroup = (mstudioseqgroup_t *)((byte *)phdr + phdr->seqgroupindex) + seqgroup;

    paSequences = (cache_user_t *)pmodel->submodels;
    if (paSequences == NULL)
    {
        paSequences = (cache_user_t *)Mem_DbgCalloc(MAXSTUDIOGROUPS + 1, sizeof(cache_user_t), "engine/R_studio.c", 675);
        pmodel->submodels = (void *)paSequences;
    }

    if (!Cache_Check(&paSequences[seqgroup]))
        COM_LoadFileCached(pseqgroup->name, &paSequences[seqgroup], NULL);
}

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", SV_StudioSetupBones);

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

void R_StudioPlayerBlend(mstudioseqdesc_t *pseqdesc, int *pBlend, float *pPitch)
{
    *pBlend = (*pPitch * 3);
    if (*pBlend < pseqdesc->blendstart[0])
    {
        *pPitch -= pseqdesc->blendstart[0] * (1.0f / 3.0f);
        *pBlend = 0;
    }
    else if (*pBlend > pseqdesc->blendend[0])
    {
        *pPitch -= pseqdesc->blendend[0] * (1.0f / 3.0f);
        *pBlend = 255;
    }
    else
    {
        if (pseqdesc->blendend[0] - pseqdesc->blendstart[0] < 0.1f)
            *pBlend = 127;
        else
            *pBlend = 255 * (*pBlend - pseqdesc->blendstart[0]) / (pseqdesc->blendend[0] - pseqdesc->blendstart[0]);
        *pPitch = 0;
    }
}

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", SV_HullForStudioModel);

void func_002BB4F8(void) {
}

void GetBonePosition(edict_t *pEdict, int iBone, float *rgflOrigin, float *rgflAngles)
{
    pstudiohdr = (studiohdr_t *)Mod_Extradata(sv.models[pEdict->v.modelindex]);

    SV_StudioSetupBones(sv.models[pEdict->v.modelindex], pEdict->v.frame, pEdict->v.sequence,
                        pEdict->v.angles, pEdict->v.origin, pEdict->v.controller, pEdict->v.blending, iBone);

    if (rgflOrigin)
    {
        rgflOrigin[0] = bonetransform[iBone][0][3];
        rgflOrigin[1] = bonetransform[iBone][1][3];
        rgflOrigin[2] = bonetransform[iBone][2][3];
    }
}

void GetAttachment(edict_t *pEdict, int iAttachment, float *rgflOrigin, float *rgflAngles)
{
    mstudioattachment_t *pattachment;
    vec3_t angles;

    pstudiohdr = (studiohdr_t *)Mod_Extradata(sv.models[pEdict->v.modelindex]);
    pattachment = (mstudioattachment_t *)((byte *)pstudiohdr + pstudiohdr->attachmentindex) + iAttachment;

    angles[2] = pEdict->v.angles[2];
    angles[0] = -pEdict->v.angles[0];
    angles[1] = pEdict->v.angles[1];

    SV_StudioSetupBones(sv.models[pEdict->v.modelindex], pEdict->v.frame, pEdict->v.sequence,
                        angles, pEdict->v.origin, pEdict->v.controller, pEdict->v.blending, pattachment->bone);

    if (rgflOrigin)
        VectorTransform(pattachment->org, bonetransform[pattachment->bone], rgflOrigin);
}

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

void func_002BB740(float m[4][4], float angle)
{
    float s, c;
    float a0, a1, a2, a3, b0, b1, b2, b3;

    angle = angle * (M_PI * (1.0f / 180.0f));
    s = sinf(angle);
    c = cosf(angle);

    a0 = m[0][1] * c + m[0][2] * s;
    a1 = m[1][1] * c + m[1][2] * s;
    a2 = m[2][1] * c + m[2][2] * s;
    a3 = m[3][1] * c + m[3][2] * s;
    b0 = m[0][1] * -s + m[0][2] * c;
    b1 = m[1][1] * -s + m[1][2] * c;
    b2 = m[2][1] * -s + m[2][2] * c;
    b3 = m[3][1] * -s + m[3][2] * c;

    m[0][1] = a0;
    m[1][1] = a1;
    m[2][1] = a2;
    m[3][1] = a3;
    m[0][2] = b0;
    m[1][2] = b1;
    m[2][2] = b2;
    m[3][2] = b3;
}

void func_002BB838(float m[4][4], float angle)
{
    float s, c;
    float a0, a1, a2, a3, b0, b1, b2, b3;

    angle = angle * (M_PI * (1.0f / 180.0f));
    s = sinf(angle);
    c = cosf(angle);

    a0 = m[0][0] * c + m[0][2] * -s;
    a1 = m[1][0] * c + m[1][2] * -s;
    a2 = m[2][0] * c + m[2][2] * -s;
    a3 = m[3][0] * c + m[3][2] * -s;
    b0 = m[0][0] * s + m[0][2] * c;
    b1 = m[1][0] * s + m[1][2] * c;
    b2 = m[2][0] * s + m[2][2] * c;
    b3 = m[3][0] * s + m[3][2] * c;

    m[0][0] = a0;
    m[1][0] = a1;
    m[2][0] = a2;
    m[3][0] = a3;
    m[0][2] = b0;
    m[1][2] = b1;
    m[2][2] = b2;
    m[3][2] = b3;
}

void func_002BB930(float m[4][4], float angle)
{
    float s, c;
    float a0, a1, a2, a3, b0, b1, b2, b3;

    angle = angle * (M_PI * (1.0f / 180.0f));
    s = sinf(angle);
    c = cosf(angle);

    a0 = m[0][0] * c + m[0][1] * s;
    a1 = m[1][0] * c + m[1][1] * s;
    a2 = m[2][0] * c + m[2][1] * s;
    a3 = m[3][0] * c + m[3][1] * s;
    b0 = m[0][0] * -s + m[0][1] * c;
    b1 = m[1][0] * -s + m[1][1] * c;
    b2 = m[2][0] * -s + m[2][1] * c;
    b3 = m[3][0] * -s + m[3][1] * c;

    m[0][0] = a0;
    m[1][0] = a1;
    m[2][0] = a2;
    m[3][0] = a3;
    m[0][1] = b0;
    m[1][1] = b1;
    m[2][1] = b2;
    m[3][1] = b3;
}

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

void R_StudioTransform(vec3_t in, vec3_t out)
{
    out[0] = in[0] * D_0053CF68[0][0] + in[1] * D_0053CF68[0][1] + in[2] * D_0053CF68[0][2] + D_0053CF68[0][3];
    out[1] = in[0] * D_0053CF68[1][0] + in[1] * D_0053CF68[1][1] + in[2] * D_0053CF68[1][2] + D_0053CF68[1][3];
    out[2] = in[0] * D_0053CF68[2][0] + in[1] * D_0053CF68[2][1] + in[2] * D_0053CF68[2][2] + D_0053CF68[2][3];
}

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BC240);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BC520);

void VectorIRotate(vec3_t in1, float in2[3][4], vec3_t out)
{
    out[0] = in1[0] * in2[0][0] + in1[1] * in2[1][0] + in1[2] * in2[2][0];
    out[1] = in1[0] * in2[0][1] + in1[1] * in2[1][1] + in1[2] * in2[2][1];
    out[2] = in1[0] * in2[0][2] + in1[1] * in2[1][2] + in1[2] * in2[2][2];
}

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

void MatrixCopy(float in[3][4], float out[3][4])
{
    *(matrix3x4_t *)out = *(matrix3x4_t *)in;
}

void R_StudioSaveBones(void)
{
    int i;
    mstudiobone_t *pbones;

    pbones = (mstudiobone_t *)((byte *)pstudiohdr + pstudiohdr->boneindex);
    m_nCachedBones = pstudiohdr->numbones;

    for (i = 0; i < pstudiohdr->numbones; i++)
    {
        strcpy(m_nCachedBoneNames[i], pbones[i].name);
        MatrixCopy(bonetransform[i], m_rgCachedBoneTransform[i]);
        MatrixCopy(lighttransform[i], m_rgCachedLightTransform[i]);
    }
}

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BD568);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BD828);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BDB30);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BDDC0);

void func_002BE2D0(float *out, int bone, vec3_t in)
{
    out[0] = DotProduct(in, bonetransform[bone][0]) + bonetransform[bone][0][3];
    out[1] = DotProduct(in, bonetransform[bone][1]) + bonetransform[bone][1][3];
    out[2] = DotProduct(in, bonetransform[bone][2]) + bonetransform[bone][2][3];
    out[3] = 1.0f;
}

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BE3C0);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BE5A0);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BE780);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BEA90);

studiohdr_t *R_LoadTextures(model_t *pmodel)
{
    char modelname[260];
    model_t *ptexturemodel;

    if (pstudiohdr->textureindex)
        return pstudiohdr;

    ptexturemodel = *(model_t **)((byte *)pmodel + 0x190);
    if (ptexturemodel && ptexturemodel->cache.data)
        return (studiohdr_t *)ptexturemodel->cache.data;

    strcpy(modelname, pmodel->name);
    strcpy(&modelname[strlen(modelname) - 4], "T.dol");

    ptexturemodel = Mod_ForName(modelname, true);
    *(model_t **)((byte *)pmodel + 0x190) = ptexturemodel;
    strcpy(((studiohdr_t *)ptexturemodel->cache.data)->name, modelname);
    return (studiohdr_t *)ptexturemodel->cache.data;
}

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

studiotexcache_t *func_002BF028(int index)
{
    studiotexcache_t **ppcache;
    studiotexcache_t *pcache;

    ppcache = &D_0053C2E8[index];
    pcache = *ppcache;
    if (pcache && pcache->index == index)
        return pcache;

    pcache = &D_005377E8[D_003996A4];
    D_003996A4 = (D_003996A4 + 1) % 64;
    *ppcache = pcache;
    pcache->index = index;
    pcache->unk004 = -1;
    pcache->unk008 = -1;
    if (Cache_Check(&pcache->cache))
        Cache_Free(&pcache->cache);
    return pcache;
}

void func_002BF0E8(char *name, int texindex, studiotexcache_t *pcache)
{
    studiohdr_t *phdr;
    mstudiotexture_t *ptexture;
    int size;

    if (Cache_Check(&pcache->cache))
    {
        if (pcache->unk00C == name)
            return;
        Cache_Free(&pcache->cache);
    }

    pcache->texindex = texindex;
    phdr = (studiohdr_t *)func_00244928(name, NULL);
    ptexture = (mstudiotexture_t *)((byte *)phdr + phdr->textureindex) + pcache->texindex;
    pcache->width = ptexture->width;
    pcache->height = ptexture->height;
    size = ptexture->width * ptexture->height + 0x300;
    Cache_Alloc(&pcache->cache, size, pcache->name);
    memcpy(pcache->cache.data, (byte *)phdr + ptexture->index, size);
    Mem_Free(phdr);
}


void R_StudioSetupSkin(studiohdr_t *ptexturehdr, int index)
{
    mstudiotexture_t *ptexture;
    studiotexcache_t *pcache;

    if (g_ForcedFaceFlags & STUDIO_NF_CHROME)
        return;

    ptexture = (mstudiotexture_t *)((byte *)ptexturehdr + ptexturehdr->textureindex) + index;

    if (currententity->index > 0 && !strcasecmp(ptexture->name, "DM_Base.bmp"))
    {
        pcache = func_002BF028(currententity->index);
        if (pcache->unk00C != (char *)D_0039967C || pcache->unk004 != D_00399698 || pcache->unk008 != D_003996C0)
            Con_Printf("If you just died, blame Mike\n");

        if (pcache->unk128)
        {
            GL_Bind(pcache->unk128);
            return;
        }
    }

    GL_Bind(STUDIO_TEXNUM(ptexture));
}

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BF2B0);

void R_StudioSetupModel(int bodypart)
{
    int index;

    if (bodypart > pstudiohdr->numbodyparts)
    {
        Con_DPrintf("R_StudioSetupModel: no such bodypart %d\n", bodypart);
        bodypart = 0;
    }

    pbodypart = (mstudiobodyparts_t *)((byte *)pstudiohdr + pstudiohdr->bodypartindex) + bodypart;

    index = currententity->body / pbodypart->base;
    index = index % pbodypart->nummodels;

    psubmodel = (mstudiomodel_t *)((byte *)pstudiohdr + pbodypart->modelindex) + index;
}

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", R_StudioDrawModel);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BFB68);

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BFEA8);

void func_002C0258(void)
{
    studiotexcache_t *pcache;

    pcache = func_002BF028(currententity->index);
    pcache->unk004 = -1;
    pcache->unk008 = -1;
    if (Cache_Check(&pcache->cache))
        Cache_Free(&pcache->cache);
}

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

void R_StudioRenderModel(void)
{
    int fx;

    VectorCopy(r_origin, chrome_origin);
    g_ForcedFaceFlags = 0;

    fx = currententity->renderfx;
    if (fx == kRenderFxGlowShell)
    {
        currententity->renderfx = 0;
        R_StudioRenderFinal();

        g_ForcedFaceFlags = STUDIO_NF_CHROME;
        tri_SpriteTexture(cl_shellchrome, 0);
        currententity->renderfx = fx;
        R_StudioRenderFinal();
    }
    else
    {
        R_StudioRenderFinal();
    }
}

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", R_StudioRenderFinal);

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

int func_002C2B68(int bodypart)
{
    int model;
    int total;
    int i;
    mstudiounkF4_t *p;

    model = func_002C2AF0(bodypart);
    total = 0;
    p = (mstudiounkF4_t *)((byte *)pstudiohdr + STUDIOHDR_UNKF4(pstudiohdr));
    for (i = 0; i < STUDIOHDR_UNKF9(pstudiohdr); i++)
    {
        total++;
        total += p[bodypart * STUDIOHDR_UNKF8(pstudiohdr) + i].unk00;
        if (model < total)
        {
            model = i;
            break;
        }
    }
    return model;
}

void func_002C2BF8(int bodypart, int submodel, int offset)
{
    mstudiobodyparts_t *pbodypart;
    mstudiounkF4_t *p;
    int i;
    int total;
    int body;

    pbodypart = (mstudiobodyparts_t *)((byte *)pstudiohdr + pstudiohdr->bodypartindex) + bodypart;
    p = (mstudiounkF4_t *)((byte *)pstudiohdr + STUDIOHDR_UNKF4(pstudiohdr));
    total = 0;
    for (i = 0; i < STUDIOHDR_UNKF9(pstudiohdr) && i != submodel; i++)
        total += 1 + p[bodypart * STUDIOHDR_UNKF8(pstudiohdr) + i].unk00;
    total += offset;

    if (bodypart <= pstudiohdr->numbodyparts && submodel < pbodypart->nummodels)
    {
        body = currententity->body;
        currententity->body = body - (body / pbodypart->base) % pbodypart->nummodels * pbodypart->base
                            + total * pbodypart->base;
    }
}

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002C2CE0);
