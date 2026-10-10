#include "common.h"
#include "engine/cvar.h"
#include "engine/model.h"
#include "engine/mathlib.h"

#define MAX_CLIP_VERTS 64
#define fabs(x) ((x) < 0 ? -(x) : (x))

#define SKY_TEXNUM 1001
#define GL_TEXTURE_2D 0x0DE1
#define GL_UNSIGNED_BYTE 0x1401
#define GL_COLOR_INDEX 0x1900
#define GL_RGBA 0x1908
#define GL_COLOR_FORMAT_PS2 0x8801
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_LINEAR 9729.0f

typedef struct
{
    byte pad0[0x14];
    int type;                /* 0x14 */
    unsigned short width;    /* 0x18 */
    unsigned short height;   /* 0x1A */
    byte pad1C[4];
    byte palette[0x400];     /* 0x20 */
    byte pixels[1];          /* 0x420 */
} psiheader_t;

extern cvar_t cl_skyname;
extern char *D_00333118[6]; /* suf */
#define suf D_00333118
extern int D_004E77C0[6];   /* skytexturenum */
extern int D_003994F8;      /* c_sky */
extern float D_004E7738[2][6]; /* skymins */
extern float D_004E7780[2][6]; /* skymaxs */
extern vec3_t r_origin;
extern int D_003331C0[6][3]; /* vec_to_st */
#define vec_to_st D_003331C0

float *func_002A3C60(glpoly_t *p);
void func_002A13D0(int nump, vec3_t vecs, int stage);
void func_002A1B60(void);

int sprintf(char *, const char *, ...);
int func_002D04E8(char *name);
void *COM_LoadFile(char *path, int usehunk, int *pLength);
void GL_Bind(int texnum);
void func_0023C450(int target, int internalformat, int width, int format, int type, void *table); /* glColorTableEXT */
void glTexImage2D(int target, int level, int internalformat, int width, int height, int border, int format, int type, void *pixels);
void func_0023C600(int target, int pname, float param); /* glTexParameterf */

INCLUDE_RODATA("asm/nonmatchings/engine/gl_warp", D_003936E0);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_warp", D_003936E8);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_warp", D_003936F0);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_warp", D_003936F8);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_warp", D_00393700);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_warp", D_00393708);

#ifdef NON_MATCHING
void func_002A0F68(void)
{
    int i;
    int texnum;
    int length;
    psiheader_t *buf;
    char name[64];

    sprintf(name, "gfx/env/%s%s.psi", cl_skyname.string, suf[0]);
    if (func_002D04E8(name) <= 0)
        Cvar_Set("cl_skyname", "desert");

    for (i = 0; i < 6; i++)
    {
        sprintf(name, "gfx/env/%s%s.psi", cl_skyname.string, suf[i]);
        buf = COM_LoadFile(name, 1, &length);
        if (length > 0)
        {
            texnum = SKY_TEXNUM + i;
            GL_Bind(texnum);

            switch (buf->type)
            {
            case 2:
                func_0023C450(GL_TEXTURE_2D, GL_RGBA, 256, GL_COLOR_FORMAT_PS2, GL_UNSIGNED_BYTE, buf->palette);
                glTexImage2D(GL_TEXTURE_2D, 0, 1, buf->width, buf->height, 0, GL_COLOR_INDEX, GL_UNSIGNED_BYTE, buf->pixels);
                break;
            case 5:
                glTexImage2D(GL_TEXTURE_2D, 0, 1, buf->width, buf->height, 0, GL_COLOR_FORMAT_PS2, GL_UNSIGNED_BYTE, buf->palette);
                break;
            }

            D_004E77C0[i] = texnum;
            func_0023C600(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            func_0023C600(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/gl_warp", func_002A0F68);
#endif

void func_002A1150(int nump, vec3_t vecs)
{
    int i, j;
    vec3_t v, av;
    float s, t, dv;
    int axis;
    float *vp;

    D_003994F8++;

    VectorCopy(vec3_origin, v);
    for (i = 0, vp = vecs; i < nump; i++, vp += 3)
        VectorAdd(vp, v, v);

    av[0] = fabs(v[0]);
    av[1] = fabs(v[1]);
    av[2] = fabs(v[2]);

    if (av[0] > av[1] && av[0] > av[2])
    {
        if (v[0] < 0)
            axis = 1;
        else
            axis = 0;
    }
    else if (av[1] > av[2] && av[1] > av[0])
    {
        if (v[1] < 0)
            axis = 3;
        else
            axis = 2;
    }
    else
    {
        if (v[2] < 0)
            axis = 5;
        else
            axis = 4;
    }

    for (i = 0; i < nump; i++, vecs += 3)
    {
        j = vec_to_st[axis][2];
        if (j > 0)
            dv = vecs[j - 1];
        else
            dv = -vecs[-j - 1];

        j = vec_to_st[axis][0];
        if (j < 0)
            s = -vecs[-j - 1] / dv;
        else
            s = vecs[j - 1] / dv;

        j = vec_to_st[axis][1];
        if (j < 0)
            t = -vecs[-j - 1] / dv;
        else
            t = vecs[j - 1] / dv;

        if (s < D_004E7738[0][axis])
            D_004E7738[0][axis] = s;
        if (t < D_004E7738[1][axis])
            D_004E7738[1][axis] = t;
        if (s > D_004E7780[0][axis])
            D_004E7780[0][axis] = s;
        if (t > D_004E7780[1][axis])
            D_004E7780[1][axis] = t;
    }
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_warp", func_002A13D0);

void func_002A1828(msurface_t *s)
{
    msurface_t *fa;
    glpoly_t *p;
    float *verts;
    int i;
    vec3_t vecs[MAX_CLIP_VERTS];

    D_003994F8 = 0;

    for (fa = s; fa; fa = fa->texturechain)
    {
        for (p = fa->polys; p; p = p->next)
        {
            verts = func_002A3C60(p);
            for (i = 0; i < p->numverts; i++, verts += 3)
                VectorSubtract(verts, r_origin, vecs[i]);

            func_002A13D0(p->numverts, vecs[0], 0);
        }
    }

    func_002A1B60();
}

void func_002A1918(void)
{
    int i;

    for (i = 0; i < 6; i++)
    {
        D_004E7738[0][i] = D_004E7738[1][i] = 9999;
        D_004E7780[0][i] = D_004E7780[1][i] = -9999;
    }
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_warp", func_002A1970);

INCLUDE_ASM("asm/nonmatchings/engine/gl_warp", func_002A1B60);
