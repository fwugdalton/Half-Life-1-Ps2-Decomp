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
extern vec3_t r_origin;
extern vec3_t chrome_origin;
extern int g_ForcedFaceFlags;
extern struct model_s *cl_shellchrome;
void tri_SpriteTexture(struct model_s *pSpriteModel, int frame);
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

INCLUDE_ASM("asm/nonmatchings/engine/gl_studio", func_002C3070);

INCLUDE_ASM("asm/nonmatchings/engine/gl_studio", func_002C3A80);

void func_002C3C90(void)
{
    if (D_00439500.count > 0)
    {
        func_0023D520();
        D_003342A0 = !D_003342A0;
    }
    D_00439500.count = 0;
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_studio", func_002C3CE0);

studiohdr_t *func_002C43A0(model_t *pmodel)
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

INCLUDE_ASM("asm/nonmatchings/engine/gl_studio", func_002C4460);

void func_002C4840(vec3_t in1, float in2[3][4], vec3_t out)
{
    out[0] = in1[0] * in2[0][0] + in1[1] * in2[1][0] + in1[2] * in2[2][0];
    out[1] = in1[0] * in2[0][1] + in1[1] * in2[1][1] + in1[2] * in2[2][1];
    out[2] = in1[0] * in2[0][2] + in1[1] * in2[1][2] + in1[2] * in2[2][2];
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_studio", func_002C48D8);

INCLUDE_ASM("asm/nonmatchings/engine/gl_studio", func_002C4B00);
