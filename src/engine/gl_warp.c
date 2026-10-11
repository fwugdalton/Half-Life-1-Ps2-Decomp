#include "common.h"
#include "engine/cvar.h"
#include "engine/model.h"
#include "engine/mathlib.h"

#define MAX_CLIP_VERTS 64
#define ON_EPSILON 0.1f
#define SIDE_FRONT 0
#define SIDE_BACK 1
#define SIDE_ON 2
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
extern char *suf[6];
extern int skytexturenum[6];
extern int c_sky;
extern float skymins[2][6];
extern float skymaxs[2][6];
extern vec3_t r_origin;
extern int vec_to_st[6][3];
extern vec3_t skyclip[6];
extern int st_to_vec[6][3];
extern float D_004A8B58;
#define sky_min (1.0f / 512.0f)
#define sky_max (511.0f / 512.0f)
void func_0023C048(float s, float t); /* glTexCoord2f */
void func_0023C298(float *v);         /* glVertex3fv */
#define GL_QUADS 7
extern int D_00333220[6]; /* eixo de cada face do céu */
extern int skytexorder[6];
extern vec3_t vpn;
extern int D_00399570;
extern int D_00399558;
extern vec3_t vec3_origin;
void GL_DisableMultitexture(void);
void glDepthMask(int flag);
void func_0023C1D0(float r, float g, float b); /* glColor3f */
void func_0023BF60(int mode);                   /* glBegin */
void func_0023C400(void);                       /* glEnd */
void MakeSkyVec(float s, float t, int axis);

float *func_002A3C60(glpoly_t *p);
void ClipSkyPolygon(int nump, vec3_t vecs, int stage);
void R_DrawSkyBox(void);

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
void R_LoadSkys(void)
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

            skytexturenum[i] = texnum;
            func_0023C600(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            func_0023C600(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/gl_warp", R_LoadSkys);
#endif

void DrawSkyPolygon(int nump, vec3_t vecs)
{
    int i, j;
    vec3_t v, av;
    float s, t, dv;
    int axis;
    float *vp;

    c_sky++;

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

        if (s < skymins[0][axis])
            skymins[0][axis] = s;
        if (t < skymins[1][axis])
            skymins[1][axis] = t;
        if (s > skymaxs[0][axis])
            skymaxs[0][axis] = s;
        if (t > skymaxs[1][axis])
            skymaxs[1][axis] = t;
    }
}

void ClipSkyPolygon(int nump, vec3_t vecs, int stage)
{
    float *norm;
    float *v;
    qboolean front, back;
    float d, e;
    float dists[MAX_CLIP_VERTS];
    int sides[MAX_CLIP_VERTS];
    vec3_t newv[2][MAX_CLIP_VERTS];
    int newc[2];
    int i, j;

    if (nump > MAX_CLIP_VERTS - 2)
        Sys_Error("ClipSkyPolygon: MAX_CLIP_VERTS");

    if (stage == 6)
    {
        DrawSkyPolygon(nump, vecs);
        return;
    }

    front = back = false;
    norm = skyclip[stage];
    for (i = 0, v = vecs; i < nump; i++, v += 3)
    {
        d = DotProduct(v, norm);
        if (d > ON_EPSILON)
        {
            front = true;
            sides[i] = SIDE_FRONT;
        }
        else if (d < ON_EPSILON)
        {
            back = true;
            sides[i] = SIDE_BACK;
        }
        else
            sides[i] = SIDE_ON;
        dists[i] = d;
    }

    if (!front || !back)
    {
        ClipSkyPolygon(nump, vecs, stage + 1);
        return;
    }

    sides[i] = sides[0];
    dists[i] = dists[0];
    VectorCopy(vecs, (vecs + (i * 3)));
    newc[0] = newc[1] = 0;

    for (i = 0, v = vecs; i < nump; i++, v += 3)
    {
        switch (sides[i])
        {
        case SIDE_FRONT:
            VectorCopy(v, newv[0][newc[0]]);
            newc[0]++;
            break;
        case SIDE_BACK:
            VectorCopy(v, newv[1][newc[1]]);
            newc[1]++;
            break;
        case SIDE_ON:
            VectorCopy(v, newv[0][newc[0]]);
            newc[0]++;
            VectorCopy(v, newv[1][newc[1]]);
            newc[1]++;
            break;
        }

        if (sides[i] == SIDE_ON || sides[i + 1] == SIDE_ON || sides[i + 1] == sides[i])
            continue;

        d = dists[i] / (dists[i] - dists[i + 1]);
        for (j = 0; j < 3; j++)
        {
            e = v[j] + d * (v[j + 3] - v[j]);
            newv[0][newc[0]][j] = e;
            newv[1][newc[1]][j] = e;
        }
        newc[0]++;
        newc[1]++;
    }

    ClipSkyPolygon(newc[0], newv[0][0], stage + 1);
    ClipSkyPolygon(newc[1], newv[1][0], stage + 1);
}

void R_DrawSkyChain(msurface_t *s)
{
    msurface_t *fa;
    glpoly_t *p;
    float *verts;
    int i;
    vec3_t vecs[MAX_CLIP_VERTS];

    c_sky = 0;

    for (fa = s; fa; fa = fa->texturechain)
    {
        for (p = fa->polys; p; p = p->next)
        {
            verts = func_002A3C60(p);
            for (i = 0; i < p->numverts; i++, verts += 3)
                VectorSubtract(verts, r_origin, vecs[i]);

            ClipSkyPolygon(p->numverts, vecs[0], 0);
        }
    }

    R_DrawSkyBox();
}

void R_ClearSkyBox(void)
{
    int i;

    for (i = 0; i < 6; i++)
    {
        skymins[0][i] = skymins[1][i] = 9999;
        skymaxs[0][i] = skymaxs[1][i] = -9999;
    }
}

void MakeSkyVec(float s, float t, int axis)
{
    vec3_t v, b;
    int j, k;
    float dist;

    dist = D_004A8B58 * 0.57735f;

    if (s < -1)
        s = -1;
    else if (s > 1)
        s = 1;

    if (t < -1)
        t = -1;
    else if (t > 1)
        t = 1;

    b[0] = s * dist;
    b[1] = t * dist;
    b[2] = dist;

    for (j = 0; j < 3; j++)
    {
        k = st_to_vec[axis][j];
        if (k < 0)
            v[j] = -b[-k - 1];
        else
            v[j] = b[k - 1];
        v[j] += r_origin[j];
    }

    s = (s + 1) * 0.5f;
    t = (t + 1) * 0.5f;

    if (s < sky_min)
        s = sky_min;
    else if (s > sky_max)
        s = sky_max;

    if (t < sky_min)
        t = sky_min;
    else if (t > sky_max)
        t = sky_max;

    t = 1.0f - t;
    func_0023C048(s, t);
    func_0023C298(v);
}

void R_DrawSkyBox(void)
{
    int i;
    vec3_t v;

    GL_DisableMultitexture();
    glDepthMask(0);

    for (i = 0; i < 6; i++)
    {
        if (skymins[0][i] >= skymaxs[0][i] || skymins[1][i] >= skymaxs[1][i])
            continue;

        VectorCopy(vec3_origin, v);
        switch (D_00333220[i])
        {
        case 1:
            v[0] = 1;
            break;
        case -1:
            v[0] = -1;
            break;
        case 2:
            v[1] = 1;
            break;
        case -2:
            v[1] = -1;
            break;
        case 3:
            v[2] = 1;
            break;
        case -3:
            v[2] = -1;
            break;
        }

        if (DotProduct(vpn, v) < -0.29289322f)
            continue;

        GL_Bind(skytexturenum[skytexorder[i]]);
        func_0023C1D0(1, 1, 1);
        func_0023BF60(GL_QUADS);
        D_00399570++;
        D_00399558 += 4;
        MakeSkyVec(skymins[0][i], skymins[1][i], i);
        MakeSkyVec(skymins[0][i], skymaxs[1][i], i);
        MakeSkyVec(skymaxs[0][i], skymaxs[1][i], i);
        MakeSkyVec(skymaxs[0][i], skymins[1][i], i);
        func_0023C400();
    }

    glDepthMask(1);
}
