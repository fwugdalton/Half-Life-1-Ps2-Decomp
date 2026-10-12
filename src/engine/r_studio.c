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
#include "engine/progdefs.h"

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
extern hull_t cache_hull[MAXSTUDIOBONES];
extern mplane_t cache_planes[MAXSTUDIOBONES * 6];
extern int cache_hull_hitgroup[MAXSTUDIOBONES];
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
void R_FlushStudioCache(void);
void func_002BE2D0(float *out, int bone, vec3_t in);
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
int abs(int x);
void Sys_Error(const char *fmt, ...);
void Con_Printf(const char *fmt, ...);
void GL_Bind(int texnum);

typedef struct alight_s
{
    int ambientlight;           /* 0x00 */
    int shadelight;             /* 0x04 */
    vec3_t color;               /* 0x08 */
    float *plightvec;           /* 0x14 */
} alight_t;

extern int r_ambientlight;
extern float r_shadelight;
extern vec3_t r_plightvec;
extern vec3_t r_blightvec[MAXSTUDIOBONES];
extern int r_colormix[3];
extern vec3_t r_lightcolor;
extern cvar_t v_lambert;
extern int lightgammatable[1024];
extern int lineargammatable[1024];
extern int screengammatable[1024];
extern float locallightR2[3];
extern int locallinearlight[3][3];
float sqrtf(float);
extern int D_00399694;

#define STUDIO_MAXLIGHTS 3
extern int lightbonecache[MAXSTUDIOBONES];
extern int r_smodels_total;
extern int chromeage[MAXSTUDIOBONES];
extern vec3_t chromeup[MAXSTUDIOBONES];
extern vec3_t chromeright[MAXSTUDIOBONES];
extern vec3_t vright;
extern int numlights;
extern cvar_t r_glowshellfreq;
extern int g_NormalIndex[MAXSTUDIOVERTS];
void tri_Color4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
extern float D_0041DE40[MAXSTUDIOBONES][3]; /* pos estático de R_StudioMergeBones */
extern float D_0041E440[MAXSTUDIOBONES][4]; /* q estático de R_StudioMergeBones */
void func_0025C428(cl_entity_t *ent, float transform[3][4]); /* StudioFxTransform */
extern cvar_t sv_clienttrace;
#define FL_CLIENT 8
#define STUDIO_TRACE_HITBOX 0x200
hull_t *R_StudioHull(model_t *pModel, float frame, int sequence, vec3_t angles, vec3_t origin, vec3_t size,
                     byte *pcontroller, byte *pblending, int *pNumHulls);
hull_t *SV_HullForEntity(edict_t *ent, vec3_t mins, vec3_t maxs, vec3_t offset);
extern int boxpnt[6][4];
extern float hullcolor[8][3];
extern int D_00399570, D_00399558;
void glTexCoord2f(float s, float t);
extern struct model_s *D_003991CC;
extern int D_00399574, D_0039954C;
void tri_RenderMode(int mode);
void tri_Color4f(float r, float g, float b, float a);
void tri_Brightness(float brightness);
void glBegin(int mode);
void glEnd(void);
void glVertex3fv(float *v);
typedef struct dlight_s
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

#define MAX_ELIGHTS 64
extern dlight_t cl_elights[MAX_ELIGHTS];
extern struct dlight_s *locallight[];
extern vec3_t lightbonepos[MAXSTUDIOBONES][STUDIO_MAXLIGHTS];
void QuaternionSlerp(float p[4], float q[4], float t, float qt[4]);
float func_002D7F00(float x);

/* variáveis de QuaternionSlerp (no original ficam na .sbss, fora da pilha) */
extern float D_003996C4; /* omega */
extern float D_003996CC; /* cosom */
extern float D_00399688; /* sinom */
extern float D_00399680; /* sclp */
extern float D_003996A0; /* sclq */
qboolean R_CullBox(vec3_t mins, vec3_t maxs);
void R_StudioRotateBBox(vec3_t mins, vec3_t maxs, vec3_t angles);
void R_StudioCalcBoneAdj(float dadt, float *adj, byte *pcontroller1, byte *pcontroller2, byte mouthopen);
void R_StudioCalcBoneQuaternion(int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *adj, float *q);
void R_StudioCalcBonePosition(int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *adj, float *pos);

/* campos do cl_entity_t ainda não mapeados em cl_entity.h */
#define ENT_CONTROLLER(e) ((byte *)(e) + 0x00)
#define ENT_PREVCONTROLLER(e) ((byte *)(e) + 0x08)
#define ENT_MOUTHOPEN(e) (*((byte *)(e) + 0x0E))

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

void R_AddToStudioCache(float frame, int sequence, vec3_t angles, vec3_t origin, vec3_t size, byte *controller,
                        byte *pblending, model_t *pModel, hull_t *pHulls, int numhulls)
{
    r_studiocache_t *pCache;

    if (nCurrentHull + numhulls >= MAXSTUDIOBONES)
        R_FlushStudioCache();

    r_cachecurrent++;
    pCache = &rgStudioCache[r_cachecurrent & STUDIO_CACHEMASK];

    pCache->frame = frame;
    pCache->sequence = sequence;
    VectorCopy(angles, pCache->angles);
    VectorCopy(origin, pCache->origin);
    VectorCopy(size, pCache->size);
    memcpy(pCache->controller, controller, 4);
    memcpy(pCache->blending, pblending, 2);
    pCache->pModel = pModel;
    pCache->nStartHull = nCurrentHull;
    pCache->nStartPlane = nCurrentPlane;

    memcpy(&cache_hull[nCurrentHull], pHulls, numhulls * sizeof(hull_t));
    memcpy(&cache_planes[nCurrentPlane], studio_planes, numhulls * sizeof(mplane_t) * 6);
    memcpy(&cache_hull_hitgroup[nCurrentHull], studio_hull_hitgroup, numhulls * sizeof(int));

    pCache->numhulls = numhulls;
    nCurrentHull += numhulls;
    nCurrentPlane += numhulls * 6;
}

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

void QuaternionSlerp(float p[4], float q[4], float t, float qt[4])
{
    int i;
    float a = 0;
    float b = 0;

    for (i = 0; i < 4; i++)
    {
        a += (p[i] - q[i]) * (p[i] - q[i]);
        b += (p[i] + q[i]) * (p[i] + q[i]);
    }
    if (a > b)
    {
        for (i = 0; i < 4; i++)
            q[i] = -q[i];
    }

    D_003996CC = p[0] * q[0] + p[1] * q[1] + p[2] * q[2] + p[3] * q[3];

    if ((1.0f + D_003996CC) > 0.000001f)
    {
        if ((1.0f - D_003996CC) > 0.000001f)
        {
            D_003996C4 = func_002D7F00(D_003996CC);
            D_00399688 = sinf(D_003996C4);
            D_00399680 = sinf((1.0f - t) * D_003996C4) / D_00399688;
            D_003996A0 = sinf(t * D_003996C4) / D_00399688;
        }
        else
        {
            D_00399680 = 1.0f - t;
            D_003996A0 = t;
        }
        for (i = 0; i < 4; i++)
            qt[i] = D_00399680 * p[i] + D_003996A0 * q[i];
    }
    else
    {
        qt[0] = -q[1];
        qt[1] = q[0];
        qt[2] = -q[3];
        qt[3] = q[2];
        D_00399680 = sinf((1.0f - t) * (0.5f * M_PI));
        D_003996A0 = sinf(t * (0.5f * M_PI));
        for (i = 0; i < 3; i++)
            qt[i] = D_00399680 * p[i] + D_003996A0 * qt[i];
    }
}

void QuaternionMatrix(float quaternion[4], float matrix[3][4])
{
    matrix[0][0] = 1.0f - 2.0f * quaternion[1] * quaternion[1] - 2.0f * quaternion[2] * quaternion[2];
    matrix[1][0] = 2.0f * quaternion[0] * quaternion[1] + 2.0f * quaternion[3] * quaternion[2];
    matrix[2][0] = 2.0f * quaternion[0] * quaternion[2] - 2.0f * quaternion[3] * quaternion[1];

    matrix[0][1] = 2.0f * quaternion[0] * quaternion[1] - 2.0f * quaternion[3] * quaternion[2];
    matrix[1][1] = 1.0f - 2.0f * quaternion[0] * quaternion[0] - 2.0f * quaternion[2] * quaternion[2];
    matrix[2][1] = 2.0f * quaternion[1] * quaternion[2] + 2.0f * quaternion[3] * quaternion[0];

    matrix[0][2] = 2.0f * quaternion[0] * quaternion[2] + 2.0f * quaternion[3] * quaternion[1];
    matrix[1][2] = 2.0f * quaternion[1] * quaternion[2] - 2.0f * quaternion[3] * quaternion[0];
    matrix[2][2] = 1.0f - 2.0f * quaternion[0] * quaternion[0] - 2.0f * quaternion[1] * quaternion[1];
}

INCLUDE_RODATA("asm/nonmatchings/engine/r_studio", D_00394FA0);

INCLUDE_RODATA("asm/nonmatchings/engine/r_studio", D_00394FA8);

void R_StudioCalcBoneAdj(float dadt, float *adj, byte *pcontroller1, byte *pcontroller2, byte mouthopen)
{
    int i, j;
    float value;
    mstudiobonecontroller_t *pbonecontroller;

    pbonecontroller = (mstudiobonecontroller_t *)((byte *)pstudiohdr + pstudiohdr->bonecontrollerindex);

    for (j = 0; j < pstudiohdr->numbonecontrollers; j++)
    {
        i = pbonecontroller[j].index;
        if (i <= 3)
        {
            if (pbonecontroller[j].type & STUDIO_RLOOP)
            {
                if (abs(pcontroller1[i] - pcontroller2[i]) > 128)
                {
                    int a, b;
                    a = (pcontroller1[j] + 128) % 256;
                    b = (pcontroller2[j] + 128) % 256;
                    value = ((a * dadt) + (b * (1.0f - dadt)) - 128) * (360.0f / 256.0f) + pbonecontroller[j].start;
                }
                else
                {
                    value = (pcontroller1[i] * dadt + pcontroller2[i] * (1.0f - dadt)) * (360.0f / 256.0f) + pbonecontroller[j].start;
                }
            }
            else
            {
                value = (pcontroller1[i] * dadt + pcontroller2[i] * (1.0f - dadt)) * (1.0f / 255.0f);
                if (value < 0)
                    value = 0;
                if (value > 1.0f)
                    value = 1.0f;
                value = (1.0f - value) * pbonecontroller[j].start + value * pbonecontroller[j].end;
            }
        }
        else
        {
            value = mouthopen / 64.0f;
            if (value > 1.0f)
                value = 1.0f;
            value = (1.0f - value) * pbonecontroller[j].start + value * pbonecontroller[j].end;
        }

        switch (pbonecontroller[j].type & STUDIO_TYPES)
        {
        case STUDIO_XR:
        case STUDIO_YR:
        case STUDIO_ZR:
            adj[j] = value * (M_PI * (1.0f / 180.0f));
            break;
        case STUDIO_X:
        case STUDIO_Y:
        case STUDIO_Z:
            adj[j] = value;
            break;
        }
    }
}

void R_StudioCalcBoneQuaternion(int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *adj, float *q)
{
    int j, k;
    float q1[4], q2[4];
    vec3_t angle1, angle2;
    mstudioanimvalue_t *panimvalue;

    for (j = 0; j < 3; j++)
    {
        if (panim->offset[j + 3] == 0)
        {
            angle2[j] = angle1[j] = pbone->value[j + 3];
        }
        else
        {
            panimvalue = (mstudioanimvalue_t *)((byte *)panim + panim->offset[j + 3]);
            k = frame;
            if (panimvalue->num.total < panimvalue->num.valid)
                k = 0;
            while (panimvalue->num.total <= k)
            {
                k -= panimvalue->num.total;
                panimvalue += panimvalue->num.valid + 1;
                if (panimvalue->num.total < panimvalue->num.valid)
                    k = 0;
            }

            if (panimvalue->num.valid > k)
            {
                angle1[j] = panimvalue[k + 1].value;

                if (panimvalue->num.valid > k + 1)
                {
                    angle2[j] = panimvalue[k + 2].value;
                }
                else
                {
                    if (panimvalue->num.total > k + 1)
                        angle2[j] = angle1[j];
                    else
                        angle2[j] = panimvalue[panimvalue->num.valid + 2].value;
                }
            }
            else
            {
                angle1[j] = panimvalue[panimvalue->num.valid].value;
                if (panimvalue->num.total > k + 1)
                    angle2[j] = angle1[j];
                else
                    angle2[j] = panimvalue[panimvalue->num.valid + 2].value;
            }
            angle1[j] = pbone->value[j + 3] + angle1[j] * pbone->scale[j + 3];
            angle2[j] = pbone->value[j + 3] + angle2[j] * pbone->scale[j + 3];
        }

        if (pbone->bonecontroller[j + 3] != -1)
        {
            angle1[j] += adj[pbone->bonecontroller[j + 3]];
            angle2[j] += adj[pbone->bonecontroller[j + 3]];
        }
    }

    if (!VectorCompare(angle1, angle2))
    {
        AngleQuaternion(angle1, q1);
        AngleQuaternion(angle2, q2);
        QuaternionSlerp(q1, q2, s, q);
    }
    else
    {
        AngleQuaternion(angle1, q);
    }
}

void R_StudioCalcBonePosition(int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *adj, float *pos)
{
    int j, k;
    mstudioanimvalue_t *panimvalue;

    for (j = 0; j < 3; j++)
    {
        pos[j] = pbone->value[j];
        if (panim->offset[j] != 0)
        {
            panimvalue = (mstudioanimvalue_t *)((byte *)panim + panim->offset[j]);

            k = frame;
            if (panimvalue->num.total < panimvalue->num.valid)
                k = 0;

            while (panimvalue->num.total <= k)
            {
                k -= panimvalue->num.total;
                panimvalue += panimvalue->num.valid + 1;
                if (panimvalue->num.total < panimvalue->num.valid)
                    k = 0;
            }

            if (panimvalue->num.valid > k)
            {
                if (panimvalue->num.valid > k + 1)
                    pos[j] += (panimvalue[k + 1].value * (1.0f - s) + s * panimvalue[k + 2].value) * pbone->scale[j];
                else
                    pos[j] += panimvalue[k + 1].value * pbone->scale[j];
            }
            else
            {
                if (panimvalue->num.total <= k + 1)
                    pos[j] += (panimvalue[panimvalue->num.valid].value * (1.0f - s) + s * panimvalue[panimvalue->num.valid + 2].value) * pbone->scale[j];
                else
                    pos[j] += panimvalue[panimvalue->num.valid].value * pbone->scale[j];
            }
        }
        if (pbone->bonecontroller[j] != -1)
            pos[j] += adj[pbone->bonecontroller[j]];
    }
}

void R_StudioSlerpBones(float q1[][4], float pos1[][3], float q2[][4], float pos2[][3], float s)
{
    int i;
    float q3[4];
    float s1;

    if (s < 0)
        s = 0;
    else if (s > 1.0f)
        s = 1.0f;

    s1 = 1.0f - s;

    for (i = 0; i < pstudiohdr->numbones; i++)
    {
        QuaternionSlerp(q1[i], q2[i], s, q3);
        q1[i][0] = q3[0];
        q1[i][1] = q3[1];
        q1[i][2] = q3[2];
        q1[i][3] = q3[3];
        pos1[i][0] = pos1[i][0] * s1 + pos2[i][0] * s;
        pos1[i][1] = pos1[i][1] * s1 + pos2[i][1] * s;
        pos1[i][2] = pos1[i][2] * s1 + pos2[i][2] * s;
    }
}


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

hull_t *SV_HullForStudioModel(edict_t *pEdict, vec3_t mins, vec3_t maxs, vec3_t offset, int *pNumHulls)
{
    qboolean useComplexHull = false;
    vec3_t size;
    float factor = 0.5f;

    VectorSubtract(maxs, mins, size);
    if (VectorCompare(vec3_origin, size))
    {
        if (!(gGlobalVariables.trace_flags & 1))
        {
            useComplexHull = true;
            if (pEdict->v.flags & FL_CLIENT)
            {
                if (sv_clienttrace.value != 0.0f)
                {
                    size[2] = 1.0f;
                    size[1] = 1.0f;
                    size[0] = 1.0f;
                    factor = sv_clienttrace.value * factor;
                }
                else
                {
                    useComplexHull = false;
                }
            }
        }
    }

    if ((sv.models[pEdict->v.modelindex]->flags & STUDIO_TRACE_HITBOX) || useComplexHull)
    {
        VectorScale(size, factor, size);
        VectorCopy(vec3_origin, offset);

        if (pEdict->v.flags & FL_CLIENT)
        {
            mstudioseqdesc_t *pseqdesc;
            vec3_t angles;
            int iBlend;
            byte blending[2];
            byte controller[4];

            pstudiohdr = (studiohdr_t *)Mod_Extradata(sv.models[pEdict->v.modelindex]);
            pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex) + pEdict->v.sequence;

            VectorCopy(pEdict->v.angles, angles);
            R_StudioPlayerBlend(pseqdesc, &iBlend, angles);

            blending[0] = iBlend;
            blending[1] = 0;
            controller[0] = 0x7F;
            controller[1] = 0x7F;
            controller[2] = 0x7F;
            controller[3] = 0x7F;

            return R_StudioHull(sv.models[pEdict->v.modelindex], pEdict->v.frame, pEdict->v.sequence, angles,
                                pEdict->v.origin, size, controller, blending, pNumHulls);
        }
        else
        {
            return R_StudioHull(sv.models[pEdict->v.modelindex], pEdict->v.frame, pEdict->v.sequence,
                                pEdict->v.angles, pEdict->v.origin, size, pEdict->v.controller,
                                pEdict->v.blending, pNumHulls);
        }
    }

    *pNumHulls = 1;
    return SV_HullForEntity(pEdict, mins, maxs, offset);
}

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

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", R_StudioRotateBBox);

int R_StudioCheckBBox(void)
{
    mstudioseqdesc_t *pseqdesc;
    vec3_t mins, maxs;

    pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex) + currententity->sequence;

    VectorCopy(pseqdesc->bbmin, mins);
    VectorCopy(pseqdesc->bbmax, maxs);

    R_StudioRotateBBox(mins, maxs, currententity->angles);

    VectorAdd(currententity->origin, mins, mins);
    VectorAdd(currententity->origin, maxs, maxs);

    if (currententity->model && !func_002E4560(currententity->model->name + 7, "barnacle", 8))
        mins[2] -= 1024.0f;

    return !R_CullBox(mins, maxs);
}

void R_StudioTransform(vec3_t in, vec3_t out)
{
    out[0] = in[0] * D_0053CF68[0][0] + in[1] * D_0053CF68[0][1] + in[2] * D_0053CF68[0][2] + D_0053CF68[0][3];
    out[1] = in[0] * D_0053CF68[1][0] + in[1] * D_0053CF68[1][1] + in[2] * D_0053CF68[1][2] + D_0053CF68[1][3];
    out[2] = in[0] * D_0053CF68[2][0] + in[1] * D_0053CF68[2][1] + in[2] * D_0053CF68[2][2] + D_0053CF68[2][3];
}

void R_StudioSetupGlowShell(vec3_t *pstudioverts, float (*pout)[4])
{
    int i, j;
    float scale;
    byte *pvertbone;
    vec3_t *pstudionorms;
    float (*pv)[4];
    mstudiomesh_t *pmesh;
    short *ptricmds;

    scale = currententity->renderamt * 0.05f;
    pvertbone = (byte *)pstudiohdr + psubmodel->vertinfoindex;
    pstudionorms = (vec3_t *)((byte *)pstudiohdr + psubmodel->normindex);

    for (i = 0; i < psubmodel->numverts; i++)
        g_NormalIndex[i] = -1;

    for (i = 0; i < pstudiohdr->numbones; i++)
        chromeage[i] = 0;

    for (j = 0; j < psubmodel->nummesh; j++)
    {
        pmesh = (mstudiomesh_t *)((byte *)pstudiohdr + psubmodel->meshindex) + j;
        ptricmds = (short *)((byte *)pstudiohdr + pmesh->triindex);

        while ((i = *(ptricmds++)))
        {
            if (i < 0)
                i = -i;

            for (; i > 0; i--, ptricmds += 4)
            {
                if (g_NormalIndex[ptricmds[0]] < 0)
                    g_NormalIndex[ptricmds[0]] = ptricmds[1];
            }
        }
    }

    pv = pout;
    for (i = 0; i < psubmodel->numverts; i++, pstudioverts++, pv++)
    {
        vec3_t tmp;

        VectorMA(pstudioverts[0], scale, pstudionorms[g_NormalIndex[i]], tmp);
        func_002BE2D0(*pv, pvertbone[i], tmp);
    }

    chrome_origin[0] = cosf(cl[0].time * r_glowshellfreq.value) * 4000.0f;
    chrome_origin[1] = sinf(cl[0].time * r_glowshellfreq.value) * 4000.0f;
    chrome_origin[2] = cosf(cl[0].time * r_glowshellfreq.value * 0.33f) * 4000.0f;

    tri_Color4ub(currententity->rendercolor.r, currententity->rendercolor.g, currententity->rendercolor.b, 255);
}

void R_StudioSetUpTransform(int trivial_accept, int clientId)
{
    int i;
    vec3_t angles;
    vec3_t modelpos;

    for (i = 0; i < 3; i++)
        modelpos[i] = currententity->origin[i];

    angles[2] = currententity->angles[2];
    angles[0] = currententity->angles[0];
    angles[1] = currententity->angles[1];

    if (currententity->movetype == 4)
    {
        float f = 0;
        float d;

        if ((cl[clientId].time < currententity->animtime + 1.0f) &&
            (currententity->animtime != currententity->prevanimtime))
        {
            f = (cl[clientId].time - currententity->animtime) / (currententity->animtime - currententity->prevanimtime);
        }

        if (r_dointerp)
            f = f - 1.0f;
        else
            f = 0;

        for (i = 0; i < 3; i++)
            modelpos[i] += (currententity->origin[i] - currententity->prevorigin[i]) * f;

        if (pstudiohdr->numattachments == 0 && (currententity->unk194 & 0x300))
        {
            if (currententity->unk194 & 0x100)
                VectorCopy(modelpos, cl[0].viewattachment);
            if (currententity->unk194 & 0x200)
                VectorCopy(modelpos, cl[1].viewattachment);
        }

        for (i = 0; i < 3; i++)
        {
            float ang1, ang2;

            ang1 = currententity->angles[i];
            ang2 = currententity->prevangles[i];

            d = ang1 - ang2;
            if (d > 180)
                d -= 360;
            else if (d < -180)
                d += 360;

            angles[i] += d * f;
        }
    }
    else
    {
        if (pstudiohdr->numattachments == 0 && (currententity->unk194 & 0x300))
        {
            if (currententity->unk194 & 0x100)
                VectorCopy(modelpos, cl[0].viewattachment);
            if (currententity->unk194 & 0x200)
                VectorCopy(modelpos, cl[1].viewattachment);
        }
    }


    angles[0] = -angles[0];
    AngleMatrix(angles, D_0053CF68);

    D_0053CF68[0][3] = modelpos[0];
    D_0053CF68[1][3] = modelpos[1];
    D_0053CF68[2][3] = modelpos[2];
}

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
        if (dadt > 2.0f)
            return 2.0f;
    }
    else
    {
        dadt = 1.0f;
    }
    return dadt;
}

void R_StudioCalcRotations(float pos[][3], float q[][4], mstudioseqdesc_t *pseqdesc, mstudioanim_t *panim, float f, int clientId)
{
    int i;
    int frame;
    mstudiobone_t *pbone;
    float s;
    float adj[MAXSTUDIOCONTROLLERS];
    float dadt;

    if (f > pseqdesc->numframes - 1)
        f = 0;

    frame = (int)f;
    dadt = R_StudioEstimateInterpolant(clientId);
    s = f - frame;

    pbone = (mstudiobone_t *)((byte *)pstudiohdr + pstudiohdr->boneindex);
    R_StudioCalcBoneAdj(dadt, adj, ENT_CONTROLLER(currententity), ENT_PREVCONTROLLER(currententity), ENT_MOUTHOPEN(currententity));

    for (i = 0; i < pstudiohdr->numbones; i++, pbone++, panim++)
    {
        R_StudioCalcBoneQuaternion(frame, s, pbone, panim, adj, q[i]);
        R_StudioCalcBonePosition(frame, s, pbone, panim, adj, pos[i]);
    }
}

float R_StudioEstimateFrame(mstudioseqdesc_t *pseqdesc, int clientId)
{
    float dfdt, f;

    if (r_dointerp)
        dfdt = (cl[clientId].time - currententity->animtime) * currententity->framerate * pseqdesc->fps;
    else
        dfdt = 0;

    if (pseqdesc->numframes <= 1)
        f = 0;
    else
        f = (currententity->frame * (pseqdesc->numframes - 1)) / 256.0f;

    f += dfdt;

    if (pseqdesc->flags & STUDIO_LOOPING)
    {
        if (pseqdesc->numframes > 1)
            f -= (int)(f / (pseqdesc->numframes - 1)) * (pseqdesc->numframes - 1);
        if (f < 0)
            f += (pseqdesc->numframes - 1);
    }
    else
    {
        if (f >= pseqdesc->numframes - 1.001f)
            f = pseqdesc->numframes - 1.001f;
        if (f < 0.0f)
            f = 0.0f;
    }
    return f;
}

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

#ifdef NON_MATCHING
void R_StudioMergeBones(model_t *m_pSubModel, int clientId)
{
    int i, j;
    float f;
    mstudiobone_t *pbones;
    mstudioseqdesc_t *pseqdesc;
    mstudioanim_t *panim;
    float bonematrix[3][4];

    if (currententity->sequence >= pstudiohdr->numseq)
        currententity->sequence = 0;

    pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex) + currententity->sequence;

    f = R_StudioEstimateFrame(pseqdesc, clientId);

    R_StudioCalcRotations(D_0041DE40, D_0041E440, pseqdesc, R_GetAnim(m_pSubModel, pseqdesc), f, clientId);

    pbones = (mstudiobone_t *)((byte *)pstudiohdr + pstudiohdr->boneindex);

    for (i = 0; i < pstudiohdr->numbones; i++)
    {
        for (j = 0; j < m_nCachedBones; j++)
        {
            if (strcasecmp(pbones[i].name, m_nCachedBoneNames[j]) == 0)
            {
                MatrixCopy(m_rgCachedBoneTransform[j], bonetransform[i]);
                MatrixCopy(m_rgCachedLightTransform[j], lighttransform[i]);
                break;
            }
        }
        if (j >= m_nCachedBones)
        {
            QuaternionMatrix(D_0041E440[i], bonematrix);

            bonematrix[0][3] = D_0041DE40[i][0];
            bonematrix[1][3] = D_0041DE40[i][1];
            bonematrix[2][3] = D_0041DE40[i][2];

            if (pbones[i].parent == -1)
            {
                R_ConcatTransforms(D_0053CF68, bonematrix, bonetransform[i]);
                R_ConcatTransforms(D_0053CF68, bonematrix, lighttransform[i]);
                func_0025C428(currententity, bonetransform[i]);
            }
            else
            {
                R_ConcatTransforms(bonetransform[pbones[i].parent], bonematrix, bonetransform[i]);
                R_ConcatTransforms(lighttransform[pbones[i].parent], bonematrix, lighttransform[i]);
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/r_studio", R_StudioMergeBones);
#endif

#ifdef NON_MATCHING
void R_StudioDrawHulls(void)
{
    vec3_t tmp;
    vec3_t p[8];
    float lv;
    int i, j;
    mstudiobbox_t *pbboxes;

    pbboxes = (mstudiobbox_t *)((byte *)pstudiohdr + pstudiohdr->hitboxindex);
    tri_SpriteTexture(D_003991CC, 0);

    for (i = 0; i < pstudiohdr->numhitboxes; i++)
    {
        for (j = 0; j < 8; j++)
        {
            tmp[0] = (j & 1) ? pbboxes[i].bbmin[0] : pbboxes[i].bbmax[0];
            tmp[1] = (j & 2) ? pbboxes[i].bbmin[1] : pbboxes[i].bbmax[1];
            tmp[2] = (j & 4) ? pbboxes[i].bbmin[2] : pbboxes[i].bbmax[2];

            VectorTransform(tmp, lighttransform[pbboxes[i].bone], p[j]);
        }

        j = pbboxes[i].group % 8;
        tri_Color4f(hullcolor[j][0], hullcolor[j][1], hullcolor[j][2], 1.0f);
        glTexCoord2f(0, 0);

        for (j = 0; j < 6; j++)
        {
            tmp[0] = tmp[1] = tmp[2] = 0;
            tmp[j % 3] = (j < 3) ? 1.0f : -1.0f;
            R_StudioLighting(&lv, pbboxes[i].bone, 0, tmp);
            tri_Brightness(lv);

            glBegin(6);
            D_00399570++;
            D_00399558 += 4;
            glVertex3fv(p[boxpnt[j][0]]);
            glVertex3fv(p[boxpnt[j][1]]);
            glVertex3fv(p[boxpnt[j][2]]);
            glVertex3fv(p[boxpnt[j][3]]);
            glEnd();
        }
        glEnd();
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/r_studio", R_StudioDrawHulls);
#endif

#ifdef NON_MATCHING
void R_StudioDrawAbsBBox(void)
{
    vec3_t tmp;
    vec3_t p[8];
    float lv;
    int i;
    mstudioseqdesc_t *pseqdesc;

    pseqdesc = (mstudioseqdesc_t *)((byte *)pstudiohdr + pstudiohdr->seqindex) + currententity->sequence;

    tri_RenderMode(5);
    tri_SpriteTexture(D_003991CC, 0);

    for (i = 0; i < 8; i++)
    {
        p[i][0] = (i & 1) ? pseqdesc->bbmin[0] : pseqdesc->bbmax[0];
        p[i][1] = (i & 2) ? pseqdesc->bbmin[1] : pseqdesc->bbmax[1];
        p[i][2] = (i & 4) ? pseqdesc->bbmin[2] : pseqdesc->bbmax[2];
        VectorAdd(p[i], currententity->origin, p[i]);
    }

    tri_Color4f(0.5f, 0.5f, 1.0f, 1.0f);

    for (i = 0; i < 6; i++)
    {
        tmp[0] = tmp[1] = tmp[2] = 0;
        tmp[i % 3] = (i < 3) ? 1.0f : -1.0f;
        R_StudioLighting(&lv, -1, 0, tmp);
        tri_Brightness(lv);

        glBegin(6);
        D_00399574++;
        D_0039954C += 4;
        glVertex3fv(p[boxpnt[i][0]]);
        glVertex3fv(p[boxpnt[i][1]]);
        glVertex3fv(p[boxpnt[i][2]]);
        glVertex3fv(p[boxpnt[i][3]]);
        glEnd();
    }

    tri_RenderMode(0);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/r_studio", R_StudioDrawAbsBBox);
#endif

INCLUDE_ASM("asm/nonmatchings/engine/r_studio", func_002BDDC0);

void func_002BE2D0(float *out, int bone, vec3_t in)
{
    out[0] = DotProduct(in, bonetransform[bone][0]) + bonetransform[bone][0][3];
    out[1] = DotProduct(in, bonetransform[bone][1]) + bonetransform[bone][1][3];
    out[2] = DotProduct(in, bonetransform[bone][2]) + bonetransform[bone][2][3];
    out[3] = 1.0f;
}

void R_StudioLighting(float *lv, int bone, int flags, vec3_t normal)
{
    float illum;
    float lightcos;
    float r;

    illum = r_ambientlight;

    if ((flags & STUDIO_NF_FLATSHADE) && D_00399694 != 1)
    {
        illum += r_shadelight * 0.8f;
    }
    else
    {
        if (bone != -1)
            lightcos = DotProduct(normal, r_blightvec[bone]);
        else
            lightcos = DotProduct(normal, r_plightvec);

        if (lightcos > 1.0f)
            lightcos = 1.0f;

        r = v_lambert.value;
        if (r >= 1.0f)
        {
            lightcos = (lightcos + (r - 1.0f)) / r;
            illum += r_shadelight;
            if (lightcos > 0.0f)
                illum -= r_shadelight * lightcos;
        }
        else
        {
            lightcos = (r - lightcos) / (r + 1.0f);
            if (lightcos > 0.0f)
                illum += r_shadelight * lightcos;
        }

        if (illum <= 0)
            illum = 0;
    }

    if (illum > 255)
        illum = 255;

    illum = lightgammatable[(int)(illum * 4)];
    *lv = illum * 0.0006516779f;
}

void R_LightStrength(int bone, float *vert, float (*light)[4])
{
    int i;
    vec3_t lpos;

    if (lightbonecache[bone] != r_smodels_total)
    {
        for (i = 0; i < numlights; i++)
        {
            lpos[0] = ((float *)locallight[i])[0] - lighttransform[bone][0][3];
            lpos[1] = ((float *)locallight[i])[1] - lighttransform[bone][1][3];
            lpos[2] = ((float *)locallight[i])[2] - lighttransform[bone][2][3];
            VectorIRotate(lpos, lighttransform[bone], lightbonepos[bone][i]);
        }
        lightbonecache[bone] = r_smodels_total;
    }

    for (i = 0; i < numlights; i++)
    {
        VectorSubtract(vert, lightbonepos[bone][i], light[i]);
        light[i][3] = 0;
    }
}

void R_LightLambert(float (*light)[4], float *normal, float *src, float *lambert)
{
    float lightcos;
    int i;
    int c;
    float r;
    float red, green, blue;

    if (numlights)
    {
        red = green = blue = 0;

        for (i = 0; i < numlights; i++)
        {
            lightcos = -DotProduct(normal, light[i]);
            if (lightcos > 0)
            {
                if (light[i][3] == 0)
                {
                    r = DotProduct(light[i], light[i]);
                    if (r > 0)
                        light[i][3] = locallightR2[i] / (r * sqrtf(r));
                    else
                        light[i][3] = 1;
                }

                lightcos *= light[i][3];
                red += locallinearlight[i][0] * lightcos;
                green += locallinearlight[i][1] * lightcos;
                blue += locallinearlight[i][2] * lightcos;
            }
        }

        if (red != 0 || green != 0 || blue != 0)
        {
            c = lineargammatable[(int)(src[0] * 1023)] + red;
            if (c > 1023)
                lambert[0] = 1.0f;
            else
                lambert[0] = screengammatable[c] * (1.0f / 1023.0f);

            c = lineargammatable[(int)(src[1] * 1023)] + green;
            if (c > 1023)
                lambert[1] = 1.0f;
            else
                lambert[1] = screengammatable[c] * (1.0f / 1023.0f);

            c = lineargammatable[(int)(src[2] * 1023)] + blue;
            if (c > 1023)
                lambert[2] = 1.0f;
            else
                lambert[2] = screengammatable[c] * (1.0f / 1023.0f);
            return;
        }
    }

    VectorCopy(lambert, src);
}

void R_StudioChrome(int *pchrome, int bone, vec3_t normal)
{
    float n;

    if (chromeage[bone] != r_smodels_total)
    {
        vec3_t chromeupvec;
        vec3_t chromerightvec;
        vec3_t tmp;

        VectorScale(chrome_origin, -1, tmp);
        tmp[0] += lighttransform[bone][0][3];
        tmp[1] += lighttransform[bone][1][3];
        tmp[2] += lighttransform[bone][2][3];
        VectorNormalize(tmp);
        CrossProduct(tmp, vright, chromeupvec);
        VectorNormalize(chromeupvec);
        CrossProduct(chromeupvec, tmp, chromerightvec);
        VectorNormalize(chromerightvec);

        VectorIRotate(chromeupvec, lighttransform[bone], chromeup[bone]);
        VectorIRotate(chromerightvec, lighttransform[bone], chromeright[bone]);

        chromeage[bone] = r_smodels_total;
    }

    n = DotProduct(normal, chromeright[bone]);
    pchrome[0] = (n + 1.0f) * 32768.0f;

    n = DotProduct(normal, chromeup[bone]);
    pchrome[1] = (n + 1.0f) * 32768.0f;
}

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

#define HUE_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define HUE_MIN(a, b) (((a) < (b)) ? (a) : (b))

void PaletteHueReplace(byte *palSrc, int newHue, int Start, int end)
{
    int i;
    float r, b, g;
    float maxcol, mincol;
    float hue, val, sat;

    hue = (float)(newHue * (360.0f / 255.0f));

    for (i = Start; i <= end; i++)
    {
        r = palSrc[i * 3 + 0];
        g = palSrc[i * 3 + 1];
        b = palSrc[i * 3 + 2];

        maxcol = HUE_MAX(HUE_MAX(r, g), b) / 255.0f;
        mincol = HUE_MIN(HUE_MIN(r, g), b) / 255.0f;

        val = maxcol;
        sat = (maxcol - mincol) / maxcol;

        mincol = val * (1.0f - sat);

        if (hue <= 120.0f)
        {
            b = mincol;
            if (hue < 60.0f)
            {
                r = val;
                g = mincol + hue * (val - mincol) / (120.0f - hue);
            }
            else
            {
                g = val;
                r = mincol + (120.0f - hue) * (val - mincol) / hue;
            }
        }
        else if (hue <= 240.0f)
        {
            r = mincol;
            if (hue < 180.0f)
            {
                g = val;
                b = mincol + (hue - 120.0f) * (val - mincol) / (240.0f - hue);
            }
            else
            {
                b = val;
                g = mincol + (240.0f - hue) * (val - mincol) / (hue - 120.0f);
            }
        }
        else
        {
            g = mincol;
            if (hue < 300.0f)
            {
                b = val;
                r = mincol + (hue - 240.0f) * (val - mincol) / (360.0f - hue);
            }
            else
            {
                r = val;
                b = mincol + (360.0f - hue) * (val - mincol) / (hue - 240.0f);
            }
        }

        palSrc[i * 3 + 0] = (byte)(r * 255.0f);
        palSrc[i * 3 + 1] = (byte)(g * 255.0f);
        palSrc[i * 3 + 2] = (byte)(b * 255.0f);
    }
}

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

void R_StudioSetupLighting(alight_t *plighting)
{
    int i;

    r_ambientlight = plighting->ambientlight;
    r_shadelight = plighting->shadelight;
    VectorCopy(plighting->plightvec, r_plightvec);

    for (i = 0; i < pstudiohdr->numbones; i++)
        VectorIRotate(r_plightvec, lighttransform[i], r_blightvec[i]);

    r_colormix[0] = (int)(plighting->color[0] * 49407.0f) & 0xFF00;
    r_colormix[1] = (int)(plighting->color[1] * 49407.0f) & 0xFF00;
    r_colormix[2] = (int)(plighting->color[2] * 49407.0f) & 0xFF00;

    VectorCopy(plighting->color, r_lightcolor);
}

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

#ifdef NON_MATCHING
void R_StudioEntityLight(alight_t *plight, int clientId)
{
    int i, j, k;
    float minstrength, strength;
    dlight_t *dl;
    vec3_t end;
    vec3_t dist;
    float lstrength[STUDIO_MAXLIGHTS];
    float r2;
    float d;

    VectorCopy(currententity->origin, end);

    for (i = 0; i < STUDIO_MAXLIGHTS; i++)
        lstrength[i] = 0;

    numlights = 0;
    minstrength = 1000000;

    dl = cl_elights;
    for (i = 0; i < MAX_ELIGHTS; i++, dl++)
    {
        if ((dl->die == 0 || cl[clientId].time < dl->die) && dl->radius > 0)
        {
            if ((dl->key & 0xFFF) == currententity->index)
            {
                int att = (dl->key >> 12) & 0xF;

                if (att == 0)
                {
                    VectorCopy(currententity->origin, dl->origin);
                }
                else
                {
                    VectorCopy(currententity->attachment[att - 1], dl->origin);
                }
            }

            VectorSubtract(end, dl->origin, dist);
            r2 = dl->radius * dl->radius;
            d = DotProduct(dist, dist);
            strength = 1.0f;
            if (d > r2)
                strength = r2 / d;

            if (strength > 0.004f)
            {
                if (numlights < STUDIO_MAXLIGHTS)
                {
                    k = numlights;
                }
                else
                {
                    k = -1;
                    for (j = 0; j < numlights; j++)
                    {
                        if (lstrength[j] < minstrength && lstrength[j] < strength)
                        {
                            minstrength = lstrength[j];
                            k = j;
                        }
                    }
                }

                if (k != -1)
                {
                    lstrength[k] = strength;
                    locallight[k] = dl;
                    locallightR2[k] = dl->radius * dl->radius;
                    locallinearlight[k][0] = lineargammatable[dl->color.r * 4];
                    locallinearlight[k][1] = lineargammatable[dl->color.g * 4];
                    locallinearlight[k][2] = lineargammatable[dl->color.b * 4];
                    if (k >= numlights)
                        numlights = k + 1;
                }
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/r_studio", R_StudioEntityLight);
#endif

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

void R_StudioCalcAttachments(void)
{
    int i;
    mstudioattachment_t *pattachment;

    if (pstudiohdr->numattachments > 4)
        Sys_Error("Too many attachments on %s\n", currententity->model->name);

    pattachment = (mstudioattachment_t *)((byte *)pstudiohdr + pstudiohdr->attachmentindex);
    for (i = 0; i < pstudiohdr->numattachments; i++)
        VectorTransform(pattachment[i].org, lighttransform[pattachment[i].bone], currententity->attachment[i]);

    if (pstudiohdr->numattachments > 0 && (currententity->unk194 & 0x300))
    {
        if (currententity->unk194 & 0x100)
            VectorCopy(currententity->attachment[pstudiohdr->numattachments - 1], cl[0].viewattachment);
        if (currententity->unk194 & 0x200)
            VectorCopy(currententity->attachment[pstudiohdr->numattachments - 1], cl[1].viewattachment);
    }
}

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
