#include "common.h"
#include "engine/model.h"
#include "engine/mathlib.h"
#include "engine/cl_entity.h"
#include "engine/client.h"
#include "engine/cvar.h"
#include "engine/render.h"

#define GL_MODELVIEW 0x1700
#define GL_PROJECTION 0x1701
#define GL_TEXTURE_2D 0xDE1
#define GL_DEPTH_TEST 0xB71
#define GL_CULL_FACE 0xB44
#define GL_BLEND 0xBE2
#define GL_ALPHA_TEST 0xBC0
#define GL_POLYGON 9
#define GL_LEQUAL 0x203

typedef struct
{
    int width;
    int height;
} viddef_t;

extern viddef_t vid;

void glEnable(int cap);
void glDisable(int cap);
void glMatrixMode(int mode);
void glPushMatrix(void);
void glPopMatrix(void);
void glLoadIdentity(void);
void glFrustum(float left, float right, float bottom, float top, float zNear, float zFar);
void glOrtho(float left, float right, float bottom, float top, float zNear, float zFar);
void glViewport(int x, int y, int width, int height);
void glDepthRange(float zNear, float zFar);
void glDepthFunc(int func);
void glBegin(int mode);
void glEnd(void);
void glVertex2f(float x, float y);
void glColor3f(float r, float g, float b);
void glColor4f(float r, float g, float b, float a);
float tanf(float x);
float atanf(float x);

extern mplane_t frustum[4];

extern int D_00399564;
extern int D_0032EF68, D_0032EF6C, D_0032EF74;
extern int D_0032EF78, D_0032EF7C, D_0032EF80, D_0032EF84;
extern int D_0032EF90, D_0032EF94, D_0032EF98, D_0032EF9C;
extern int D_0032EFA0, D_0032EFA4, D_0032EFA8, D_0032EFAC;
extern int D_0032EFD0, D_0032EFD4, D_0032EFD8, D_0032EFDC;
extern int D_0032EFE0, D_0032EFE4, D_0032EFE8, D_0032EFEC;
extern float gldepthmax; /* gldepthmax */
extern float gldepthmin; /* gldepthmin */

void func_002A8750(void);

void func_002A8358(int i)
{
    if (D_00399564)
    {
        if (cl_numlocal > 1)
        {
            switch (i)
            {
            case 0:
                glViewport(D_0032EFD0, D_0032EFD4, D_0032EFD8, D_0032EFDC);
                break;
            case 1:
                glViewport(D_0032EFE0, D_0032EFE4, D_0032EFE8, D_0032EFEC);
                break;
            }
        }
        else
        {
            glViewport(D_0032EF7C, D_0032EF78, D_0032EF80, D_0032EF84);
        }
    }
    else
    {
        if (cl_numlocal > 1)
        {
            switch (i)
            {
            case 0:
                glViewport(D_0032EF90, D_0032EF94, D_0032EF98, D_0032EF9C);
                break;
            case 1:
                glViewport(D_0032EFA0, D_0032EFA4, D_0032EFA8, D_0032EFAC);
                break;
            }
        }
        else
        {
            glViewport(D_0032EF7C, D_0032EF78, D_0032EF80, D_0032EF84);
        }
    }
    func_002A8750();
}

void func_002A84C8(void)
{
    glViewport(D_0032EF7C, D_0032EF78, D_0032EF80, D_0032EF84);
}

void func_002A8508(int i, int x, int y)
{
    if (D_00399564)
    {
        switch (i)
        {
        case 0:
            glViewport(D_0032EFD0 + x, D_0032EFD4 + y, D_0032EFD8, D_0032EFDC);
            break;
        case 1:
            glViewport(D_0032EFE0 + x, D_0032EFE4 + y, D_0032EFE8, D_0032EFEC);
            break;
        }
    }
    else
    {
        switch (i)
        {
        case 0:
            glViewport(D_0032EF90 + x, D_0032EF94 + y, D_0032EF98, D_0032EF9C);
            break;
        case 1:
            glViewport(D_0032EFA0 + x, D_0032EFA4 + y, D_0032EFA8, D_0032EFAC);
            break;
        }
    }
}

void func_002A8610(void)
{
    glViewport(D_0032EF7C, D_0032EF78, D_0032EF80, D_0032EF84);
    func_002A8750();
}

void func_002A8658(void)
{
    glDisable(GL_TEXTURE_2D);
    glColor3f(0, 0, 0);
    glBegin(GL_POLYGON);
    glVertex2f(254, 0);
    glVertex2f(258, 0);
    glVertex2f(258, 448);
    glVertex2f(254, 448);
    glEnd();
    glEnable(GL_TEXTURE_2D);
}

void func_002A8708(void)
{
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
}

void func_002A8750(void)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(D_0032EF68, D_0032EF68 + vid.width, D_0032EF6C + D_0032EF74, D_0032EF6C, -99999, 99999);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glColor4f(1, 1, 1, 1);
}

void ProjectPointOnPlane(vec3_t dst, vec3_t p, vec3_t normal)
{
    float d;
    vec3_t n;
    float inv_denom;

    inv_denom = 1.0f / DotProduct(normal, normal);

    d = DotProduct(normal, p) * inv_denom;

    n[0] = normal[0] * inv_denom;
    n[1] = normal[1] * inv_denom;
    n[2] = normal[2] * inv_denom;

    dst[0] = p[0] - d * n[0];
    dst[1] = p[1] - d * n[1];
    dst[2] = p[2] - d * n[2];
}

void PerpendicularVector(vec3_t dst, vec3_t src)
{
    int pos;
    int i;
    float minelem = 1.0f;
    vec3_t tempvec;

    for (pos = 0, i = 0; i < 3; i++)
    {
        if (__builtin_fabsf(src[i]) < minelem)
        {
            pos = i;
            minelem = __builtin_fabsf(src[i]);
        }
    }
    tempvec[0] = tempvec[1] = tempvec[2] = 0.0f;
    tempvec[pos] = 1.0f;

    ProjectPointOnPlane(dst, tempvec, src);

    VectorNormalize(dst);
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_rmain", func_002A8990);

qboolean R_CullBox(vec3_t mins, vec3_t maxs)
{
    int i;

    for (i = 0; i < 4; i++)
    {
        if (BoxOnPlaneSide(mins, maxs, &frustum[i]) == 2)
            return true;
    }
    return false;
}

qboolean func_002A8C20(vec3_t mins, vec3_t maxs)
{
    int i;
    qboolean inside = true;

    for (i = 0; i < 4; i++)
    {
        if (BoxOnPlaneSide(mins, maxs, &frustum[i]) != 1)
        {
            inside = false;
            break;
        }
    }
    return inside;
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_rmain", func_002A8CB0);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rmain", func_002A8EB0);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rmain", func_002A9420);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rmain", func_002A9890);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rmain", func_002A9B28);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rmain", func_002A9CA0);

int SignbitsForPlane(mplane_t *out)
{
    int bits, j;

    bits = 0;
    for (j = 0; j < 3; j++)
    {
        if (out->normal[j] < 0)
            bits |= 1 << j;
    }
    return bits;
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_rmain", func_002AA078);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rmain", func_002AA288);

void MYgluPerspective(float fovy, float aspect, float zNear, float zFar)
{
    float xmin, xmax, ymin, ymax;

    ymax = zNear * tanf(fovy * (3.141593f / 360.0f));
    ymin = -ymax;

    xmin = ymin * aspect;
    xmax = ymax * aspect;

    glFrustum(xmin, xmax, ymin, ymax, zNear, zFar);
}

float CalcFov(float fov_x, float width, float height)
{
    float a;
    float x;

    if (fov_x < 1 || fov_x > 179)
        fov_x = 90;

    x = width / tanf(fov_x * (3.141593f / 360.0f));

    a = atanf(height / x);

    a = a * (360.0f / M_PI);

    return a;
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_rmain", func_002AA620);

INCLUDE_ASM("asm/nonmatchings/engine/gl_rmain", func_002AABD8);

void func_002AAD48(void)
{
    gldepthmin = 0;
    gldepthmax = 1;
    glDepthFunc(GL_LEQUAL);
    glDepthRange(gldepthmin, gldepthmax);
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_rmain", R_RenderView);

void func_002AB0B8(int a, int b, int c, ...)
{
}
