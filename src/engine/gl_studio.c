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
studiobatch_t *func_002C3A80(int count, int texnum);
void func_002C3C90(void);

#define STUDIO_BATCHVERTS 74
extern float D_00439580[2][STUDIO_BATCHVERTS][4];
extern float D_00439F00[2][STUDIO_BATCHVERTS][4];
extern float D_0043A880[2][STUDIO_BATCHVERTS][4];
extern void *D_00331030, *D_00331034, *D_00331038;
extern int D_0033103C, D_00331044;
extern int currenttexture;
int func_0023D5F0(void);
void func_0023D618(int);
void func_0023AF68(void);
void GL_Bind(int texnum);
void func_002C3C90(void);

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
extern float lighttransform[MAXSTUDIOBONES][3][4];
extern int r_smodels_total;
extern int chromeage[MAXSTUDIOBONES];
extern vec3_t chromeup[MAXSTUDIOBONES];
extern vec3_t chromeright[MAXSTUDIOBONES];
extern vec3_t vright;
extern int lineargammatable[1024];
extern int screengammatable[1024];
extern float locallightR2[3];
extern int locallinearlight[3][3];
float sqrtf(float);
extern int numlights;
extern float r_blend;
extern int r_ambientlight;
extern float r_shadelight;
extern vec3_t r_plightvec;
extern vec3_t r_blightvec[MAXSTUDIOBONES];
extern vec3_t r_lightcolor;
extern cvar_t v_lambert;
extern int lightgammatable[1024];
extern int D_00399694;
extern float *D_003996BC;
extern int D_0052F778[MAXSTUDIOVERTS][2];
#define kRenderTransAdd 5
typedef int u_long128 __attribute__((mode(TI)));
extern int D_0033102C;
extern int D_0039967C;
extern int D_00399568;
extern float *D_003996B4;
extern int cl_numlocal;
extern float sprite_color[4];
extern float D_003342B0[4];
extern float D_003342C0[4];
extern float D_003342D0[4];
extern float D_003342E0[4];
extern int lightbonecache[MAXSTUDIOBONES];
extern struct dlight_s *locallight[];
extern vec3_t lightbonepos[MAXSTUDIOBONES][3];
extern float D_00515778[2048][3][4];
#define kRenderFxGlowShell 19
studiobatch_t *func_002C3A80(int count, int texnum);
void func_002C3C90(void);
studiohdr_t *func_002C43A0(model_t *pmodel);
void func_002C4460(vec3_t *pstudioverts, float (*pout)[4]);
void func_002C3CE0(mstudiotexture_t *ptexture, short *pskinref, mstudiomesh_t *pmesh, vec3_t *pstudionorms, byte *pnormbone);
void func_002C4B00(float (*light)[4], float *normal, float *src, float *lambert);
void func_002C48D8(int *pchrome, int bone, vec3_t normal);
void func_002C4840(vec3_t in1, float in2[3][4], vec3_t out);
extern cvar_t r_glowshellfreq;
extern int g_NormalIndex[MAXSTUDIOVERTS];
void tri_Color4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

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

#ifdef NON_MATCHING
void func_002C3070(void)
{
    int i, j, k;
    byte *pvertbone;
    byte *pnormbone;
    vec3_t *pstudioverts;
    vec3_t *pstudionorms;
    studiohdr_t *ptexturehdr;
    mstudiotexture_t *ptexture;
    short *pskinref;
    mstudiomesh_t *pmesh;
    short *ptricmds;
    int flags;
    int bone;
    int texnum;
    studiobatch_t *batch;
    u_long128 *pverts;
    u_long128 *pcolors;
    float *ptc;
    float lambert[4] __attribute__((aligned(16)));
    float tmp[4] __attribute__((aligned(16)));

    pvertbone = (byte *)pstudiohdr + psubmodel->vertinfoindex;
    pnormbone = (byte *)pstudiohdr + psubmodel->norminfoindex;

    D_0033102C = 1;
    ptexturehdr = func_002C43A0((model_t *)D_0039967C);

    pstudionorms = (vec3_t *)((byte *)pstudiohdr + psubmodel->normindex);
    ptexture = (mstudiotexture_t *)((byte *)ptexturehdr + ptexturehdr->textureindex);
    pmesh = (mstudiomesh_t *)((byte *)pstudiohdr + psubmodel->meshindex);
    pstudioverts = (vec3_t *)((byte *)pstudiohdr + psubmodel->vertindex);
    pskinref = (short *)((byte *)ptexturehdr + ptexturehdr->skinindex);

    if (currententity->skin != 0 && currententity->skin < ptexturehdr->numskinfamilies)
        pskinref += currententity->skin * ptexturehdr->numskinref;

    if (currententity->renderfx == kRenderFxGlowShell)
    {
        func_002C4460(pstudioverts, (float (*)[4])D_003996B4);
    }
    else
    {
        float *pv = D_003996B4;

        for (i = 0; i < psubmodel->numverts; i++, pv += 4)
        {
            tmp[0] = pstudioverts[i][0];
            tmp[1] = pstudioverts[i][1];
            tmp[3] = 1.0f;
            tmp[2] = pstudioverts[i][2];
            __asm__ __volatile__(
                "vmulx.xyzw $vf1, $vf0, $vf0x\n"
                "vadd.xyzw $vf1, $vf1, $vf0\n"
                "lqc2 $vf4, 0x0(%0)\n"
                "lqc2 $vf5, 0x0(%1)\n"
                "vmul.xyzw $vf5, $vf4, $vf5\n"
                "vaddy.x $vf5, $vf5, $vf5y\n"
                "vaddz.x $vf5, $vf5, $vf5z\n"
                "vaddw.x $vf5, $vf5, $vf5w\n"
                "vaddx.x $vf1, $vf0, $vf5x\n"
                "lqc2 $vf5, 0x10(%1)\n"
                "vmul.xyzw $vf5, $vf4, $vf5\n"
                "vaddy.x $vf5, $vf5, $vf5y\n"
                "vaddz.x $vf5, $vf5, $vf5z\n"
                "vaddw.x $vf5, $vf5, $vf5w\n"
                "vaddx.y $vf1, $vf0, $vf5x\n"
                "lqc2 $vf5, 0x20(%1)\n"
                "vmul.xyzw $vf5, $vf4, $vf5\n"
                "vaddy.x $vf5, $vf5, $vf5y\n"
                "vaddz.x $vf5, $vf5, $vf5z\n"
                "vaddw.x $vf5, $vf5, $vf5w\n"
                "vaddx.z $vf1, $vf0, $vf5x\n"
                "sqc2 $vf1, 0x0(%2)\n"
                : : "r"(tmp), "r"(bonetransform[pvertbone[i]]), "r"(pv) : "memory");
        }
    }

    pvertbone = (byte *)pstudiohdr + psubmodel->vertinfoindex;
    pstudioverts = (vec3_t *)((byte *)pstudiohdr + psubmodel->vertindex);

    for (i = 0; i < psubmodel->numverts; i++, pstudioverts++)
    {
        bone = *pvertbone++;

        if (lightbonecache[bone] != r_smodels_total)
        {
            if (numlights > 0 && cl_numlocal == 1)
            {
                for (j = 0; j < numlights; j++)
                {
                    tmp[0] = ((float *)locallight[j])[0] - lighttransform[bone][0][3];
                    tmp[1] = ((float *)locallight[j])[1] - lighttransform[bone][1][3];
                    tmp[2] = ((float *)locallight[j])[2] - lighttransform[bone][2][3];
                    lightbonepos[bone][j][0] = tmp[0] * lighttransform[bone][0][0] + tmp[1] * lighttransform[bone][1][0] + tmp[2] * lighttransform[bone][2][0];
                    lightbonepos[bone][j][1] = tmp[0] * lighttransform[bone][0][1] + tmp[1] * lighttransform[bone][1][1] + tmp[2] * lighttransform[bone][2][1];
                    lightbonepos[bone][j][2] = tmp[0] * lighttransform[bone][0][2] + tmp[1] * lighttransform[bone][1][2] + tmp[2] * lighttransform[bone][2][2];
                }
            }
            lightbonecache[bone] = r_smodels_total;
        }

        if (cl_numlocal == 1)
        {
            for (j = 0; j < numlights; j++)
            {
                D_00515778[i][j][0] = pstudioverts[0][0] - lightbonepos[bone][j][0];
                D_00515778[i][j][1] = pstudioverts[0][1] - lightbonepos[bone][j][1];
                D_00515778[i][j][2] = pstudioverts[0][2] - lightbonepos[bone][j][2];
                D_00515778[i][j][3] = 0;
            }
        }
    }

    func_002C3CE0(ptexture, pskinref, pmesh, pstudionorms, pnormbone);

    pstudionorms = (vec3_t *)((byte *)pstudiohdr + psubmodel->normindex);

    for (j = 0; j < psubmodel->nummesh; j++)
    {
        pmesh = (mstudiomesh_t *)((byte *)pstudiohdr + psubmodel->meshindex) + j;
        ptricmds = (short *)((byte *)pstudiohdr + pmesh->triindex);

        D_00399568 += pmesh->numtris;

        flags = ptexture[pskinref[pmesh->skinref]].flags | g_ForcedFaceFlags;

        tmp[0] = 1.0f / ptexture[pskinref[pmesh->skinref]].width;
        tmp[1] = 1.0f / ptexture[pskinref[pmesh->skinref]].height;
        tmp[2] = 1.0f;
        tmp[3] = 1.0f;

        texnum = 0;
        if (!(g_ForcedFaceFlags & STUDIO_NF_CHROME))
            texnum = *(int *)&((mstudiotexture_t *)((byte *)ptexturehdr + ptexturehdr->textureindex))[pskinref[pmesh->skinref]].name[0x3C];

        if (flags & STUDIO_NF_CHROME)
        {
            tmp[1] *= (1.0f / 1024.0f);
            tmp[0] *= (1.0f / 1024.0f);

            if (g_ForcedFaceFlags & STUDIO_NF_CHROME)
            {
                while ((i = *(ptricmds++)))
                {
                    batch = func_002C3A80(i, texnum);
                    if (i < 0)
                        i = -i;

                    pverts = (u_long128 *)batch->unk10;
                    pcolors = (u_long128 *)batch->unk14;
                    ptc = (float *)batch->unk18;

                    for (; i > 0; i--, ptricmds += 4, ptc += 4)
                    {
                        *pverts++ = ((u_long128 *)D_003996B4)[ptricmds[0]];
                        *pcolors++ = *(u_long128 *)sprite_color;
                        D_003342B0[0] = D_0052F778[g_NormalIndex[ptricmds[0]]][0];
                        D_003342B0[1] = D_0052F778[g_NormalIndex[ptricmds[0]]][1];
                        __asm__ __volatile__(
                            "lqc2 $vf4, 0x0(%0)\n"
                            "lqc2 $vf5, 0x0(%1)\n"
                            "vmul.xyzw $vf5, $vf4, $vf5\n"
                            "sqc2 $vf5, 0x0(%2)\n"
                            : : "r"(D_003342B0), "r"(tmp), "r"(ptc) : "memory");
                    }
                }
            }
            else
            {
                while ((i = *(ptricmds++)))
                {
                    batch = func_002C3A80(i, texnum);
                    if (i < 0)
                        i = -i;

                    pverts = (u_long128 *)batch->unk10;
                    pcolors = (u_long128 *)batch->unk14;
                    ptc = (float *)batch->unk18;

                    for (; i > 0; i--, ptricmds += 4, ptc += 4)
                    {
                        *pverts++ = ((u_long128 *)D_003996B4)[ptricmds[0]];
                        *pcolors++ = ((u_long128 *)D_003996BC)[ptricmds[1]];
                        D_003342C0[0] = D_0052F778[ptricmds[1]][0];
                        D_003342C0[1] = D_0052F778[ptricmds[1]][1];
                        __asm__ __volatile__(
                            "lqc2 $vf4, 0x0(%0)\n"
                            "lqc2 $vf5, 0x0(%1)\n"
                            "vmul.xyzw $vf5, $vf4, $vf5\n"
                            "sqc2 $vf5, 0x0(%2)\n"
                            : : "r"(D_003342C0), "r"(tmp), "r"(ptc) : "memory");
                    }
                }
            }
        }
        else if (numlights > 0 && cl_numlocal == 1)
        {
            while ((i = *(ptricmds++)))
            {
                batch = func_002C3A80(i, texnum);
                if (i < 0)
                    i = -i;

                pverts = (u_long128 *)batch->unk10;
                pcolors = (u_long128 *)batch->unk14;
                ptc = (float *)batch->unk18;

                for (; i > 0; i--, ptricmds += 4, ptc += 4)
                {
                    *pverts++ = ((u_long128 *)D_003996B4)[ptricmds[0]];
                    func_002C4B00(D_00515778[ptricmds[0]], pstudionorms[ptricmds[1]], &D_003996BC[ptricmds[1] * 4], lambert);
                    *pcolors++ = *(u_long128 *)lambert;
                    D_003342D0[0] = ptricmds[2];
                    D_003342D0[1] = ptricmds[3];
                    __asm__ __volatile__(
                        "lqc2 $vf4, 0x0(%0)\n"
                        "lqc2 $vf5, 0x0(%1)\n"
                        "vmul.xyzw $vf5, $vf4, $vf5\n"
                        "sqc2 $vf5, 0x0(%2)\n"
                        : : "r"(D_003342D0), "r"(tmp), "r"(ptc) : "memory");
                }
            }
        }
        else
        {
            while ((i = *(ptricmds++)))
            {
                batch = func_002C3A80(i, texnum);
                if (i < 0)
                    i = -i;

                pverts = (u_long128 *)batch->unk10;
                pcolors = (u_long128 *)batch->unk14;
                ptc = (float *)batch->unk18;

                for (; i > 0; i--, ptricmds += 4, ptc += 4)
                {
                    *pverts++ = ((u_long128 *)D_003996B4)[ptricmds[0]];
                    *pcolors++ = ((u_long128 *)D_003996BC)[ptricmds[1]];
                    D_003342E0[0] = ptricmds[2];
                    D_003342E0[1] = ptricmds[3];
                    __asm__ __volatile__(
                        "lqc2 $vf4, 0x0(%0)\n"
                        "lqc2 $vf5, 0x0(%1)\n"
                        "vmul.xyzw $vf5, $vf4, $vf5\n"
                        "sqc2 $vf5, 0x0(%2)\n"
                        : : "r"(D_003342E0), "r"(tmp), "r"(ptc) : "memory");
                }
            }
        }
    }

    func_002C3C90();
    D_0033102C = 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/gl_studio", func_002C3070);
#endif

#ifdef NON_MATCHING
studiobatch_t *func_002C3A80(int count, int texnum)
{
    int mode;
    int done;
    int n;
    float *p0, *p1, *p2;

    if (count < 0)
    {
        count = -count;
        mode = 5;
    }
    else
    {
        mode = 4;
    }

    done = 0;

    if (D_00439500.count > 0 && D_00439500.texnum == texnum && (n = D_00439500.count + count) < STUDIO_BATCHVERTS)
    {
        p0 = D_00439580[D_003342A0][D_00439500.count];
        p1 = D_0043A880[D_003342A0][D_00439500.count];
        p2 = D_00439F00[D_003342A0][D_00439500.count];
        D_00439500.unk0C++;
        D_00331030 = p0;
        D_00331034 = p2;
        D_00331038 = p1;
        D_00439500.unk10 = (int)p0 | 0x30000000;
        D_00439500.unk14 = (int)p2 | 0x30000000;
        D_00439500.unk18 = (int)p1 | 0x30000000;
        D_0033103C = count;
        D_00439500.count = n;
        D_00331044 = mode;
        if (func_0023D5F0() == 1)
        {
            done = 1;
            func_0023D618(0);
        }
    }

    if (!done)
    {
        func_002C3C90();
        if (currenttexture != texnum)
        {
            GL_Bind(texnum);
            func_0023AF68();
        }
        p0 = D_00439580[D_003342A0][0];
        p1 = D_0043A880[D_003342A0][0];
        p2 = D_00439F00[D_003342A0][0];
        D_00439500.texnum = texnum;
        D_00331044 = mode;
        D_00331030 = p0;
        D_00331034 = p2;
        D_00331038 = p1;
        D_00439500.unk10 = (int)p0 | 0x30000000;
        D_00439500.unk14 = (int)p2 | 0x30000000;
        D_00439500.unk18 = (int)p1 | 0x30000000;
        D_0033103C = count;
        D_00439500.count = count;
        D_00439500.unk0C = 0;
        D_00439500.unk04 = mode;
        func_0023D618(0);
    }

    D_00439500.unk0C++;
    return &D_00439500;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/gl_studio", func_002C3A80);
#endif

void func_002C3C90(void)
{
    if (D_00439500.count > 0)
    {
        func_0023D520();
        D_003342A0 = !D_003342A0;
    }
    D_00439500.count = 0;
}

#ifdef NON_MATCHING
void func_002C3CE0(mstudiotexture_t *ptexture, short *pskinref, mstudiomesh_t *pmesh, vec3_t *pstudionorms, byte *pnormbone)
{
    int i, j;
    int flags;
    float *lv;
    float lv_tmp;
    float illum;
    float lightcos;
    float r;
    int bone;
    int *pchrome;
    float n;
    vec3_t chromeupvec;
    vec3_t chromerightvec;
    vec3_t tmp;

    lv = D_003996BC;

    if (currententity->rendermode == kRenderTransAdd)
    {
        for (j = 0; j < psubmodel->nummesh; j++)
        {
            flags = ptexture[pskinref[pmesh[j].skinref]].flags | g_ForcedFaceFlags;

            for (i = 0; i < pmesh[j].numnorms; i++, lv += 4)
            {
                lv[0] = lv[3] = lv[2] = lv[1] = r_blend * 0.6666665f;
                if (flags & STUDIO_NF_CHROME)
                    func_002C48D8(D_0052F778[i], *pnormbone, (float *)pstudionorms);
            }
        }
        return;
    }

    for (j = 0; j < psubmodel->nummesh; j++)
    {
        flags = ptexture[pskinref[pmesh[j].skinref]].flags | g_ForcedFaceFlags;

        for (i = 0; i < pmesh[j].numnorms; i++, lv += 4, pstudionorms++, pnormbone++)
        {
            illum = r_ambientlight;
            bone = *pnormbone;

            if ((flags & STUDIO_NF_FLATSHADE) && D_00399694 != 1)
            {
                illum += r_shadelight * 0.8f;
            }
            else
            {
                if (bone != -1)
                    lightcos = DotProduct(pstudionorms[0], r_blightvec[bone]);
                else
                    lightcos = DotProduct(pstudionorms[0], r_plightvec);

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
            lv_tmp = illum * 0.0006516779f;

            if (flags & STUDIO_NF_CHROME)
            {
                pchrome = D_0052F778[(float (*)[4])lv - (float (*)[4])D_003996BC];

                if (chromeage[bone] != r_smodels_total)
                {
                    VectorScale(chrome_origin, -1, tmp);
                    tmp[0] += lighttransform[bone][0][3];
                    tmp[1] += lighttransform[bone][1][3];
                    tmp[2] += lighttransform[bone][2][3];
                    VectorNormalize(tmp);
                    CrossProduct(tmp, vright, chromeupvec);
                    VectorNormalize(chromeupvec);
                    CrossProduct(chromeupvec, tmp, chromerightvec);
                    VectorNormalize(chromerightvec);

                    func_002C4840(chromeupvec, lighttransform[bone], chromeup[bone]);
                    func_002C4840(chromerightvec, lighttransform[bone], chromeright[bone]);

                    chromeage[bone] = r_smodels_total;
                }

                n = DotProduct(pstudionorms[0], chromeright[bone]);
                pchrome[0] = (n + 1.0f) * 32768.0f;

                n = DotProduct(pstudionorms[0], chromeup[bone]);
                pchrome[1] = (n + 1.0f) * 32768.0f;
            }

            lv[0] = lv_tmp * r_lightcolor[0];
            lv[1] = lv_tmp * r_lightcolor[1];
            lv[2] = lv_tmp * r_lightcolor[2];
            lv[3] = r_blend;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/gl_studio", func_002C3CE0);
#endif

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

#ifdef NON_MATCHING
void func_002C4460(vec3_t *pstudioverts, float (*pout)[4])
{
    int i, j;
    float scale;
    byte *pvertbone;
    vec3_t *pstudionorms;
    float (*pv)[4];
    mstudiomesh_t *pmesh;
    short *ptricmds;

    scale = currententity->renderamt * 0.05f;
    pstudionorms = (vec3_t *)((byte *)pstudiohdr + psubmodel->normindex);
    pvertbone = (byte *)pstudiohdr + psubmodel->vertinfoindex;

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
        (*pv)[0] = tmp[0] * bonetransform[*pvertbone][0][0] + tmp[1] * bonetransform[*pvertbone][0][1] + tmp[2] * bonetransform[*pvertbone][0][2] + bonetransform[*pvertbone][0][3];
        (*pv)[1] = tmp[0] * bonetransform[*pvertbone][1][0] + tmp[1] * bonetransform[*pvertbone][1][1] + tmp[2] * bonetransform[*pvertbone][1][2] + bonetransform[*pvertbone][1][3];
        (*pv)[2] = tmp[0] * bonetransform[*pvertbone][2][0] + tmp[1] * bonetransform[*pvertbone][2][1] + tmp[2] * bonetransform[*pvertbone][2][2] + bonetransform[*pvertbone][2][3];
        pvertbone++;
    }

    chrome_origin[0] = cosf(cl[0].time * r_glowshellfreq.value) * 4000.0f;
    chrome_origin[1] = sinf(cl[0].time * r_glowshellfreq.value) * 4000.0f;
    chrome_origin[2] = cosf(cl[0].time * r_glowshellfreq.value * 0.33f) * 4000.0f;

    tri_Color4ub(currententity->rendercolor.r, currententity->rendercolor.g, currententity->rendercolor.b, 255);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/gl_studio", func_002C4460);
#endif

void func_002C4840(vec3_t in1, float in2[3][4], vec3_t out)
{
    out[0] = in1[0] * in2[0][0] + in1[1] * in2[1][0] + in1[2] * in2[2][0];
    out[1] = in1[0] * in2[0][1] + in1[1] * in2[1][1] + in1[2] * in2[2][1];
    out[2] = in1[0] * in2[0][2] + in1[1] * in2[1][2] + in1[2] * in2[2][2];
}

void func_002C48D8(int *pchrome, int bone, vec3_t normal)
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

        func_002C4840(chromeupvec, lighttransform[bone], chromeup[bone]);
        func_002C4840(chromerightvec, lighttransform[bone], chromeright[bone]);

        chromeage[bone] = r_smodels_total;
    }

    n = DotProduct(normal, chromeright[bone]);
    pchrome[0] = (n + 1.0f) * 32768.0f;

    n = DotProduct(normal, chromeup[bone]);
    pchrome[1] = (n + 1.0f) * 32768.0f;
}

void func_002C4B00(float (*light)[4], float *normal, float *src, float *lambert)
{
    float lightcos;
    int i;
    int c;
    float r;
    float red, green, blue;

    red = green = blue = 0;

    for (i = 0; i < numlights; i++)
    {
        lightcos = DotProduct(normal, light[i]);
        if (lightcos < 0)
        {
            lightcos = -lightcos;
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

    lambert[3] = r_blend;
    return;
    }

    VectorCopy(src, lambert);
}
