#include "common.h"
#include "engine/model.h"

#define GL_BLEND 0x0BE2
#define GL_FLAT 0x1D00
#define GL_SMOOTH 0x1D01
#define GL_ONE 1
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303

enum
{
    kRenderNormal,
    kRenderTransColor,
    kRenderTransTexture,
    kRenderGlow,
    kRenderTransAlpha,
    kRenderTransAdd
};

typedef struct
{
    int gl_texturenum;
} mspriteframe_t;

void glColor4f(float r, float g, float b, float a); /* glColor4f */
void func_0023BE70(int mode);                            /* glShadeModel */
void glEnable(int cap);
void glDisable(int cap);
void glDepthMask(int flag);
void glBlendFunc(int sfactor, int dfactor);
void GL_Bind(int texnum);
mspriteframe_t *R_GetSpriteFrame(void *psprite, int frame);

extern float D_003994E0; /* r */
extern float D_003994E4; /* a */
extern float D_003994E8; /* g */
extern float D_003994EC; /* b */
extern int D_00332D18[4];
extern int D_004E77B0[3];

void tri_Color4f(float r, float g, float b, float a)
{
    glColor4f(r * a, g * a, b * a, 1.0f);
    D_003994E0 = r;
    D_003994E8 = g;
    D_003994EC = b;
    D_003994E4 = a;
}

void tri_Color4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    D_003994E0 = r / 255.0f;
    D_003994E8 = g / 255.0f;
    D_003994EC = b / 255.0f;
    D_003994E4 = a / 255.0f;
    glColor4f(D_003994E0, D_003994E8, D_003994EC, 1.0f);
}

void tri_Brightness(float brightness)
{
    float r, g, b;

    r = D_003994E0 * brightness;
    g = D_003994E8 * brightness;
    b = D_003994EC * brightness;
    glColor4f(r * D_003994E4, g * D_003994E4, b * D_003994E4, 1.0f);
}

void tri_RenderMode(int mode)
{
    switch (mode)
    {
    case kRenderNormal:
        glDisable(GL_BLEND);
        glDepthMask(1);
        func_0023BE70(GL_FLAT);
        break;
    case kRenderTransColor:
    case kRenderTransTexture:
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
        func_0023BE70(GL_SMOOTH);
        break;
    case kRenderTransAdd:
        glBlendFunc(GL_ONE, GL_ONE);
        glEnable(GL_BLEND);
        glDepthMask(0);
        func_0023BE70(GL_SMOOTH);
        break;
    }
}

void func_002A0EE0(void)
{
}

int tri_SpriteTexture(model_t *pSpriteModel, int frame)
{
    mspriteframe_t *pframe;

    pframe = R_GetSpriteFrame(pSpriteModel->cache.data, frame);
    if (!pframe)
        return 0;

    GL_Bind(pframe->gl_texturenum);
    return 1;
}

void func_002A0F18(int a, int b, int c, int d)
{
    D_004E77B0[0] = a;
    D_004E77B0[1] = b;
    D_004E77B0[2] = c;
    D_00332D18[0] = a;
    D_00332D18[1] = b;
    D_00332D18[2] = c;
    D_00332D18[3] = d;
}

short func_002A0F48(byte *p)
{
    return p[0] + (p[1] << 8);
}
