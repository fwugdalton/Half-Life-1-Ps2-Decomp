#include "common.h"
#include "engine/cmd.h"
#include "engine/cvar.h"
#include "engine/client.h"
#include "engine/render.h"
#include "engine/glquake.h"

extern qboolean gl_mtexable;
extern int TEXTURE0_SGIS;
extern void (*D_003994C4)(int target); /* qglSelectTextureSGIS */
extern int D_003333D0;                 /* oldtarget */
extern int D_00333278[];               /* cnttextures */
void func_0023C430(int target, int texture);
int func_00234F58(void *font, char *str);
int func_002AFE28(int c);

typedef struct
{
    short x;
    short width;
} fontchar_t;

typedef struct
{
    byte pad0[0x4];
    int rows;                   /* 0x04: altura da textura */
    byte pad8[0xC - 0x8];
    int height;                 /* 0x0C */
    fontchar_t chars[256];      /* 0x10 */
    byte data[1];               /* 0x410 */
} font_t;

extern font_t *D_00399634;
extern font_t *font;

void GL_Bind(int texnum)
{
    if (currenttexture == texnum)
        return;
    currenttexture = texnum;
    func_0023C430(0xDE1, texnum);
}

void GL_SelectTexture(int target)
{
    if (!gl_mtexable)
        return;
    D_003994C4(target);
    if (target == D_003333D0)
        return;
    D_00333278[D_003333D0 - TEXTURE0_SGIS] = currenttexture;
    D_003333D0 = target;
    currenttexture = D_00333278[target - TEXTURE0_SGIS];
}

int func_002AD060(char *str)
{
    return func_00234F58(font, str);
}

int func_002AD088(short *widths)
{
    int i;

    if (D_00399634)
    {
        if (widths)
        {
            for (i = 0; i < 256; i++)
                widths[i] = D_00399634->chars[func_002AFE28(i)].width;
        }
        return D_00399634->height;
    }

    return 0;
}

extern int D_00399558, D_00399570;
extern int D_00399648;      /* char_texture */
extern float D_0050EF58[];  /* escala s/t da textura da fonte */
void glEnable(int cap);
void glDisable(int cap);
void glTexEnvf(int target, int pname, float param);
void glBlendFunc(int sfactor, int dfactor);
void glColor4ub(byte r, byte g, byte b, byte a);
void glBegin(int mode);
void glEnd(void);
void glTexCoord2f(float s, float t);
void glVertex2f(float x, float y);
void GL_DisableMultitexture(void);

#ifdef NON_MATCHING
int func_002AD110(int x, int y, int ch, int r, int g, int b)
{
    int c;
    int width, height;
    int code;
    float s, t, s2, t2;

    if (y <= -D_00399634->height)
        return 0;

    c = func_002AFE28(ch & 0xFF);
    height = D_00399634->height;
    width = D_00399634->chars[c].width;
    code = D_00399634->chars[c].x;

    s = ((code & 0xFF) + 0.5f) * D_0050EF58[0];
    t = (((short)code >> 8) + 0.5f) * D_0050EF58[2];
    t2 = t + (height - 1) * D_0050EF58[2];
    s2 = s + (width - 1) * D_0050EF58[0];

    glTexEnvf(0x2300, 0x2200, 0x2100);
    glEnable(0xBC0);
    glEnable(0xBE2);
    glBlendFunc(1, 1);
    glColor4ub(r, g, b, 0xFF);
    GL_DisableMultitexture();
    GL_Bind(D_00399648);

    glBegin(7);
    D_00399570++;
    D_00399558 += 4;
    glTexCoord2f(s, t);
    glVertex2f(x, y);
    glTexCoord2f(s2, t);
    glVertex2f(x + width, y);
    glTexCoord2f(s2, t2);
    glVertex2f(x + width, y + height);
    glTexCoord2f(s, t2);
    glVertex2f(x, y + height);
    glEnd();

    glDisable(0xBE2);
    return width;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002AD110);
#endif

INCLUDE_RODATA("asm/nonmatchings/engine/gl_draw", D_00394798);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_draw", D_003947B0);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_draw", D_003947D0);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_draw", D_003947F0);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_draw", D_00394810);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_draw", D_00394820);

typedef struct
{
    char *name;
    int minimize, maximize;
} glmode_t;

extern glmode_t D_003333D8[]; /* modes */
extern int D_003333C8;        /* gl_filter_min */
extern int D_003333CC;        /* gl_filter_max */
void func_0023C600(int target, int pname, float param); /* glTexParameterf */

void func_002AD388(void)
{
    int i;
    gltexture_t *glt;

    if (Cmd_Argc() == 1)
    {
        for (i = 0; i < 6; i++)
        {
            if (D_003333C8 == D_003333D8[i].minimize)
            {
                Con_Printf("%s\n", D_003333D8[i].name);
                return;
            }
        }
        Con_Printf("current filter is unknown???\n");
        return;
    }

    for (i = 0; i < 6; i++)
    {
        if (!Q_strcasecmp(D_003333D8[i].name, Cmd_Argv(1)))
            break;
    }
    if (i == 6)
    {
        Con_Printf("bad filter name\n");
        return;
    }

    D_003333C8 = D_003333D8[i].minimize;
    D_003333CC = D_003333D8[i].maximize;

    for (i = 0, glt = D_004F9DD8; i < D_00399640; i++, glt++)
    {
        if (glt->mipmap)
        {
            GL_Bind(glt->texnum);
            func_0023C600(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, D_003333C8);
            func_0023C600(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, D_003333CC);
        }
    }
}

extern int D_00331C58;
extern byte texgammatable[256];
extern qpic_t *D_003333C0;   /* draw_disc */
extern int D_00399644, D_00399638;
extern font_t *D_0039964C;
void func_002ACFA0(void);
void GL_TextureStats_f(void);
void func_002AD388(void);
int func_00235EA0(char *name, void *out, int *size);
int GL_LoadDecalTexture(char *name, int type, int width, int height, byte *pixels, byte *palette, byte nummips);

void Draw_Init(void)
{
    int i;
    font_t *f;

    D_00331C58 = 1;
    Cmd_AddCommand("gl_texturemode", func_002AD388);
    Cmd_AddCommand("texture_usage", GL_TextureStats_f);
    Cmd_AddCommand("texture_dump", func_002ACFA0);

    for (i = 0; i < 256; i++)
        texgammatable[i] = i;

    func_00235EA0("images/loading.psi", &D_003333C0, &i);
    D_003333C0->texnum = GL_LoadDecalTexture("sys_loading", 0, D_003333C0->width, D_003333C0->height, D_003333C0->data, D_003333C0->palette, 0);

    func_00235EA0("fonts/creditsfont.psf", &D_00399634, &i);
    f = D_00399634;
    D_00399648 = GL_LoadDecalTexture("creditsfont", 0, 256, f->rows, f->data, &f->data[f->rows * 256], 0);
    func_0023C600(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 9729.0f);
    func_0023C600(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 9729.0f);
    D_00399644 = D_00399648;
    font = D_00399634;

    func_00235EA0("fonts/alphafont.psf", &D_0039964C, &i);
    f = D_0039964C;
    D_00399638 = GL_LoadDecalTexture("alphafont", 0, 256, f->rows, f->data, &f->data[f->rows * 256], 0);
    func_0023C600(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 9729.0f);
    func_0023C600(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 9729.0f);

    D_0050EF58[0] = 1.0f / 256;
    D_0050EF58[1] = 1.0f / font->rows;
    D_0050EF58[2] = 1.0f / D_00399634->rows;
}

void glColor4f(float r, float g, float b, float a);

#ifdef NON_MATCHING
int Draw_Character(int x, int y, int num)
{
    int c;
    int width, height;
    int code;
    float s, t, s2, t2;

    if (y <= -font->height)
        return 0;

    c = func_002AFE28(num & 0xFF);
    width = font->chars[c].width;

    if (y < 0 || c == ' ')
        return width;

    height = font->height;
    code = font->chars[c].x;

    s = (code & 0xFF) * D_0050EF58[0];
    t = ((short)code >> 8) * D_0050EF58[1];
    s2 = s + width * D_0050EF58[0];
    t2 = t + height * D_0050EF58[1];

    glEnable(GL_BLEND);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_ALPHA_TEST);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_ONE, GL_ONE);
    glColor4f(1, 1, 1, 1);
    GL_DisableMultitexture();
    GL_Bind(D_00399644);

    glBegin(GL_QUADS);
    D_00399570++;
    D_00399558 += 4;
    glTexCoord2f(s, t);
    glVertex2f(x, y);
    glTexCoord2f(s2, t);
    glVertex2f(x + width, y);
    glTexCoord2f(s2, t2);
    glVertex2f(x + width, y + height);
    glTexCoord2f(s, t2);
    glVertex2f(x, y + height);
    glEnd();

    glDisable(GL_ALPHA_TEST);
    return width;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", Draw_Character);
#endif

int Draw_String(int x, int y, char *str)
{
    while (*str)
    {
        x += Draw_Character(x, y, *str);
        str++;
    }
    return x;
}

void func_002ADA70(void)
{
    GL_Bind(D_00399638);
}

#ifdef NON_MATCHING
void func_002ADA98(int ch, int *width, int *height, float *s, float *t, float *s2, float *t2)
{
    int c;
    int code;
    float fs, ft, fs2, ft2;

    c = func_002AFE28(ch & 0xFF);
    *width = D_0039964C->chars[c].width;
    code = D_0039964C->chars[c].x;
    *height = D_0039964C->height;
    fs = (code & 0xFF) * D_0050EF58[0];
    ft = ((short)code >> 8) * D_0050EF58[1];
    fs2 = fs + *width * D_0050EF58[0];
    ft2 = ft + D_0039964C->height * D_0050EF58[1];
    *s = fs;
    *t = ft;
    *s2 = fs2;
    *t2 = ft2;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002ADA98);
#endif

#ifdef NON_MATCHING
int func_002ADB98(int x, int y, int num, float sx, float sy)
{
    int c;
    int w;
    int code;
    float width, height;
    float s, t, s2, t2;

    if (y <= -font->height)
        return 0;

    c = func_002AFE28(num & 0xFF);
    w = font->chars[c].width;

    if (y < 0 || c == ' ')
        return w;

    height = font->height;
    code = font->chars[c].x;
    width = w;

    s = (code & 0xFF) * D_0050EF58[0];
    t = ((short)code >> 8) * D_0050EF58[1];
    s2 = s + width * D_0050EF58[0];
    t2 = t + height * D_0050EF58[1];

    glEnable(GL_BLEND);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_ALPHA_TEST);
    glDisable(GL_DEPTH_TEST);
    width *= sx;
    glBlendFunc(GL_ONE, GL_ONE);
    glColor4f(1, 1, 1, 1);
    GL_DisableMultitexture();
    GL_Bind(D_00399644);

    glBegin(GL_QUADS);
    D_00399570++;
    D_00399558 += 4;
    glTexCoord2f(s, t);
    glVertex2f(x, y);
    glTexCoord2f(s2, t);
    glVertex2f(x + width, y);
    glTexCoord2f(s2, t2);
    glVertex2f(x + width, y + height * sy);
    glTexCoord2f(s, t2);
    glVertex2f(x, y + height * sy);
    glEnd();

    glDisable(GL_ALPHA_TEST);
    return width;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002ADB98);
#endif

typedef struct
{
    int texnum;                 /* 0x00 */
    float sl, tl, sh, th;       /* 0x04 */
} glpic_t;

typedef struct
{
    int width, height;          /* 0x00 */
    glpic_t gl;                 /* 0x08 */
} glqpic_t;

void func_002ADE10(int x, int y, glqpic_t *pic)
{
    glpic_t *gl;

    if (!pic)
        return;

    gl = &pic->gl;
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_ALPHA_TEST);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, 7681.0f);
    glColor4f(1, 1, 1, 1);
    GL_Bind(gl->texnum);
    glBegin(GL_QUADS);
    D_00399570++;
    D_00399558 += 4;
    glTexCoord2f(gl->sl, gl->tl);
    glVertex2f(x, y);
    glTexCoord2f(gl->sh, gl->tl);
    glVertex2f(x + pic->width, y);
    glTexCoord2f(gl->sh, gl->th);
    glVertex2f(x + pic->width, y + pic->height);
    glTexCoord2f(gl->sl, gl->th);
    glVertex2f(x, y + pic->height);
    glEnd();
}

void func_002ADFB0(int x, int y, qpic_t *pic, int a)
{
    if (!pic)
        return;

    glEnable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_ALPHA_TEST);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, 7681.0f);
    glColor4f(1, 1, 1, 1);
    GL_Bind(pic->texnum);
    glBegin(GL_QUADS);
    D_00399570++;
    D_00399558 += 4;
    glTexCoord2f(0, 0);
    glVertex2f(x, y);
    glTexCoord2f(1, 0);
    glVertex2f(x + pic->width, y);
    glTexCoord2f(1, 1);
    glVertex2f(x + pic->width, y + pic->height);
    glTexCoord2f(0, 1);
    glVertex2f(x, y + pic->height);
    glEnd();
}

#ifdef NON_MATCHING
void func_002AE158(int x, int y, int srcx, int srcy, int width, int height, glqpic_t *pic, unsigned int *color, int alpha)
{
    glpic_t *gl;
    float newsl, newtl, newsh, newth;
    float oldglwidth, oldglheight;
    float a;

    if (!pic)
        return;

    gl = &pic->gl;
    glBlendFunc(GL_ONE, GL_ONE);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);

    oldglwidth = pic->width;
    oldglheight = pic->height;
    a = alpha * (1.0f / 255.0f);
    newsl = srcx / oldglwidth;
    newsh = newsl + width / oldglwidth;
    newtl = srcy / oldglheight;
    newth = newtl + height / oldglheight;

    glColor4f(a * color[0] / 256.0f, a * color[1] / 256.0f, a * color[2] / 256.0f, 1.0f);
    GL_Bind(gl->texnum);
    glBegin(GL_QUADS);
    D_00399570++;
    D_00399558 += 4;
    glTexCoord2f(newsl, newtl);
    glVertex2f(x, y);
    glTexCoord2f(newsh, newtl);
    glVertex2f(x + width, y);
    glTexCoord2f(newsh, newth);
    glVertex2f(x + width, y + height);
    glTexCoord2f(newsl, newth);
    glVertex2f(x, y + height);
    glEnd();

    glDisable(GL_BLEND);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002AE158);
#endif

void glColor3f(float r, float g, float b);

void func_002AE468(int x, int y, glqpic_t *pic, unsigned int *color, int alpha)
{
    glpic_t *gl;
    float a;

    if (!pic)
        return;

    gl = &pic->gl;
    glBlendFunc(GL_ONE, GL_ONE);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glEnable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);

    a = alpha * (1.0f / 255.0f);
    if (color)
        glColor3f(a * color[0] / 256.0f, a * color[1] / 256.0f, a * color[2] / 256.0f);
    else
        glColor3f(a, a, a);

    GL_Bind(gl->texnum);
    glBegin(GL_QUADS);
    D_00399570++;
    D_00399558 += 4;
    glTexCoord2f(gl->sl, gl->tl);
    glVertex2f(x, y);
    glTexCoord2f(gl->sh, gl->tl);
    glVertex2f(x + pic->width, y);
    glTexCoord2f(gl->sh, gl->th);
    glVertex2f(x + pic->width, y + pic->height);
    glTexCoord2f(gl->sl, gl->th);
    glVertex2f(x, y + pic->height);
    glEnd();

    glDisable(GL_BLEND);
}

void func_002AE718(int x, int y, glqpic_t *pic, unsigned int *color, int alpha)
{
    glpic_t *gl;

    if (!pic)
        return;

    gl = &pic->gl;
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    if (color)
        glColor4f(color[0] * (1.0f / 255.0f), color[1] * (1.0f / 255.0f), color[2] * (1.0f / 255.0f), alpha * (1.0f / 255.0f));
    else
        glColor4f(1.0f, 1.0f, 1.0f, alpha * (1.0f / 255.0f));

    GL_Bind(gl->texnum);
    glBegin(GL_QUADS);
    D_00399570++;
    D_00399558 += 4;
    glTexCoord2f(gl->sl, gl->tl);
    glVertex2f(x, y);
    glTexCoord2f(gl->sh, gl->tl);
    glVertex2f(x + pic->width, y);
    glTexCoord2f(gl->sh, gl->th);
    glVertex2f(x + pic->width, y + pic->height);
    glTexCoord2f(gl->sl, gl->th);
    glVertex2f(x, y + pic->height);
    glEnd();

    func_0023C600(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, D_003333CC);
}

void func_002AEA00(int x, int y, int w, int h, glqpic_t *pic)
{
    glpic_t *gl;

    if (!pic)
        return;
    h--;

    gl = &pic->gl;
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glEnable(GL_ALPHA_TEST);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, 7681.0f);
    func_0023C600(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, D_003333CC);
    GL_Bind(gl->texnum);
    glBegin(GL_QUADS);
    D_00399570++;
    D_00399558 += 4;
    glTexCoord2f(gl->sl, gl->tl);
    glVertex2f(x, y);
    glTexCoord2f(gl->sh, gl->tl);
    glVertex2f(x + w, y);
    glTexCoord2f(gl->sh, gl->th);
    glVertex2f(x + w, y + h);
    glTexCoord2f(gl->sl, gl->th);
    glVertex2f(x, y + h);
    glEnd();
}


typedef struct
{
    byte pad0[0x1C];
    int width;                  /* 0x1C */
    int height;                 /* 0x20 */
    byte pad24[0x38 - 0x24];
    int conwidth;               /* 0x38 */
} draw_viddef_t;

extern draw_viddef_t r_viddef;
extern int D_00333420, D_00333424, D_00333428, D_0033342C; /* scissor_x, scissor_y, scissor_width, scissor_height */
extern int D_00333430;                                       /* giScissorTest */

#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))

void func_002AEB98(int x, int y, int width, int height)
{
    x = clamp(x, 0, r_viddef.width);
    y = clamp(y, 0, r_viddef.height);
    width = clamp(width, 0, r_viddef.width - x);
    height = clamp(height, 0, r_viddef.height - y);

    D_00333420 = x;
    D_00333428 = width;
    D_00333424 = r_viddef.height - y - height;
    D_0033342C = height;
    D_00333430 = 1;
}

typedef struct
{
    int left, right, top, bottom;
} wrect_t;

typedef struct
{
    byte pad0[0x1C];
    unsigned short width;       /* 0x1C */
    unsigned short height;      /* 0x1E */
} spritepic_t;

typedef struct
{
    int unk0;
    spritepic_t *pic;           /* 0x04 */
} spriteframe_t;

#ifndef max
#define max(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

void func_002AEC60(void)
{
    D_00333420 = 0;
    D_00333428 = 0;
    D_00333424 = 0;
    D_0033342C = 0;
    D_00333430 = 0;
}

qboolean func_002AEC90(wrect_t *prc)
{
    if (!prc)
        return false;

    if (prc->left >= prc->right || prc->top >= prc->bottom)
        return false;

    return true;
}

qboolean func_002AECD0(wrect_t *prc1, wrect_t *prc2, wrect_t *prc)
{
    wrect_t rc;

    if (!prc)
        prc = &rc;

    prc->left = max(prc1->left, prc2->left);
    prc->right = min(prc1->right, prc2->right);

    if (prc->left < prc->right)
    {
        prc->top = max(prc1->top, prc2->top);
        prc->bottom = min(prc1->bottom, prc2->bottom);

        if (prc->top < prc->bottom)
            return true;
    }

    return false;
}

void func_002AED58(spriteframe_t *pFrame, float *pfLeft, float *pfRight, float *pfTop, float *pfBottom, int *pw, int *ph, wrect_t *prcSubRect)
{
    wrect_t rc;
    float f;
    spritepic_t *pic;

    if (!func_002AEC90(prcSubRect))
        return;

    rc.top = rc.left = 0;
    rc.right = *pw;
    rc.bottom = *ph;

    if (!func_002AECD0(prcSubRect, &rc, &rc))
        return;

    *pw = rc.right - rc.left;
    *ph = rc.bottom - rc.top;

    pic = pFrame->pic;
    f = 1.0f / (float)pic->width;
    *pfLeft = ((float)rc.left + 0.5f) * f;
    *pfRight = ((float)rc.right - 0.5f) * f;

    f = 1.0f / (float)pic->height;
    *pfTop = ((float)rc.top + 0.5f) * f;
    *pfBottom = ((float)rc.bottom - 0.5f) * f;
}

extern cvar_t D_003336D8; /* gl_spriteblend */
void glDepthMask(int flag);

void func_002AEEC0(spriteframe_t *pFrame, int ix, int iy, wrect_t *prcSubRect)
{
    float fLeft = 0;
    float fRight = 1;
    float fTop = 0;
    float fBottom = 1;
    int iWidth = pFrame->pic->width;
    int iHeight = pFrame->pic->height;
    float x, y;

    if (D_00333430)
        Con_Printf("ERROR: Scissoring Enabled\n");

    if (prcSubRect)
        func_002AED58(pFrame, &fLeft, &fRight, &fTop, &fBottom, &iWidth, &iHeight, prcSubRect);

    x = ix;
    y = iy;

    glDepthMask(0);
    GL_Bind(pFrame->unk0);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBegin(GL_QUADS);
    D_00399570++;
    D_00399558 += 4;
    glTexCoord2f(fLeft, fTop);
    glVertex2f(x, y);
    glTexCoord2f(fRight, fTop);
    glVertex2f(ix + iWidth, y);
    glTexCoord2f(fRight, fBottom);
    glVertex2f(ix + iWidth, iy + iHeight);
    glTexCoord2f(fLeft, fBottom);
    glVertex2f(x, iy + iHeight);
    glEnd();
    glDepthMask(1);
    glDisable(0xC11);
}

void func_002AF0A8(spriteframe_t *pFrame, void *pPalette, int x, int y, wrect_t *prcSubRect)
{
    func_002AEEC0(pFrame, x, y, prcSubRect);
}

void func_002AF0D0(spriteframe_t *pFrame, void *pPalette, int x, int y, wrect_t *prcSubRect)
{
    glEnable(GL_ALPHA_TEST);
    if (D_003336D8.value)
    {
        glBlendFunc(GL_SRC_ALPHA, 0x303);
        glEnable(GL_BLEND);
    }
    func_002AEEC0(pFrame, x, y, prcSubRect);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
}

void func_002AF170(spriteframe_t *pFrame, void *pPalette, int x, int y, wrect_t *prcSubRect)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    func_002AEEC0(pFrame, x, y, prcSubRect);
    glDisable(GL_BLEND);
}

extern int D_003995E8, D_003995DC;
typedef struct
{
    int width;
    int height;
} viddef_t;

extern viddef_t vid;
extern int D_0032EF68, D_0032EF6C, D_0032EF74;
extern int D_0032EF78, D_0032EF7C, D_0032EF80, D_0032EF84;
extern int D_00331C18;
extern int D_00399798;
extern char D_00394938[];
void glMatrixMode(int mode);
void glPushMatrix(void);
void glPopMatrix(void);
void glLoadIdentity(void);
void glOrtho(float left, float right, float bottom, float top, float zNear, float zFar);
void glViewport(int x, int y, int width, int height);
void func_002AF7A8(int a);
int sprintf(char *buf, const char *fmt, ...);

void Draw_ConsoleBackground(int lines)
{
    char ver[100];
    int y;
    int x;
    int h, w, x0;

    x0 = 0;
    y = lines - D_003995DC;
    w = D_003995E8;
    h = D_003995DC + 1;
    glViewport(D_0032EF7C, D_0032EF78, D_0032EF80, D_0032EF84);
    glMatrixMode(0x1701);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(D_0032EF68, D_0032EF68 + vid.width, D_0032EF6C + D_0032EF74, D_0032EF6C, -99999, 99999);
    glMatrixMode(0x1700);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    GL_Bind(D_00399644);
    glBlendFunc(GL_SRC_ALPHA, 0x303);
    glColor4f(0, 0, 0, 1);
    glBegin(9);
    glVertex2f(x0, y);
    glVertex2f(x0 + w, y);
    glVertex2f(x0 + w, y + h);
    glVertex2f(x0, y + h);
    glEnd();
    glDisable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_ALPHA_TEST);
    glMatrixMode(0x1701);
    glPopMatrix();
    glMatrixMode(0x1700);
    glPopMatrix();

    if (D_00331C18 && lines == r_viddef.height)
        func_002AF7A8(0);

    sprintf(ver, D_00394938);
    x = r_viddef.conwidth - func_002AD060(ver);
    if (!D_00331C18 && !(D_00399798 & 4))
        Draw_String(x, 24, ver);
}

void func_002AF498(int x, int y, int width, int height, int r, int g, int b, int a)
{
    float fx = x;
    float fy = y;

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4f(r * (1.0f / 255.0f), g * (1.0f / 255.0f), b * (1.0f / 255.0f), a * (1.0f / 255.0f));
    glBegin(GL_QUADS);
    D_00399570++;
    D_00399558 += 4;
    glVertex2f(fx, fy);
    glVertex2f(x + width, fy);
    glVertex2f(x + width, y + height);
    glVertex2f(fx, y + height);
    glEnd();
    glColor3f(1, 1, 1);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

extern unsigned short *host_basepal;
void func_002AC148(qpic_t *pic, int a);
void func_0023C7C8(int n, int *textures); /* glDeleteTextures */
void *memset(void *s, int c, unsigned int n);

void func_002AF668(int x, int y, int w, int h, int c)
{
    func_002AF498(x, y, w, h, host_basepal[c * 4], host_basepal[c * 4 + 1], host_basepal[c * 4 + 2], 255);
}

void func_002AF6A0(void)
{
    glEnable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
    glColor4f(0, 0, 0, 0.8f);
    glBegin(GL_QUADS);
    D_00399570++;
    D_00399558 += 4;
    glVertex2f(0, 0);
    glVertex2f(D_003995E8, 0);
    glVertex2f(D_003995E8, D_003995DC);
    glVertex2f(0, D_003995DC);
    glEnd();
    glColor4f(1, 1, 1, 1);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void func_002AF7A8(int a)
{
    if (D_003333C0)
        func_002AC148(D_003333C0, a);
}

void func_002AF7D8(void) {
}

void func_002AF7E0(gltexture_t *glt)
{
    int texnum;

    texnum = glt->texnum;
    func_0023C7C8(1, &texnum);
    memset(glt, 0, sizeof(gltexture_t));
    glt->servercount = -1;
    glt->texnum = texnum;
}

/* D_00394938 é alinhado em 8 no original */
__asm__(".section .rodata\n.align 3\n.section .text");
INCLUDE_RODATA("asm/nonmatchings/engine/gl_draw", D_00394938);

extern int D_00332F2C; /* servercount */
extern void (*qglColorTableEXT)(int target, int internalformat, int width, int format, int type, void *table);
int strcmp(const char *s1, const char *s2);

void func_002AF838(void)
{
    int i;
    int count = 0;
    gltexture_t *glt;

    GL_Bind(D_00399644);
    for (i = 0, glt = D_004F9DD8; i < D_00399640; i++, glt++)
    {
        if (glt->servercount > 0 && glt->servercount != D_00332F2C)
        {
            func_002AF7E0(glt);
            count++;
        }
    }
    Con_DPrintf("GL_UnloadTextures: Unloaded %d textures\n", count);
}

void GL_FreeDecalTexture(char *identifier)
{
    int i;
    gltexture_t *glt;

    GL_Bind(D_00399644);
    for (i = 0, glt = D_004F9DD8; i < D_00399640; i++, glt++)
    {
        if (!strcmp(identifier, glt->identifier))
        {
            if (glt->servercount >= 0)
                func_002AF7E0(glt);
            return;
        }
    }
    Con_Printf("WARNING: Texture not found [%s]\n", identifier);
}

void func_002AF9A0(int texnum)
{
    int i;
    gltexture_t *glt;

    GL_Bind(D_00399644);
    for (i = 0, glt = D_004F9DD8; i < D_00399640; i++, glt++)
    {
        if (glt->texnum == texnum)
        {
            if (glt->servercount >= 0)
                func_002AF7E0(glt);
            return;
        }
    }
    Con_Printf("WARNING: Texture not found [%d]\n", texnum);
}

void func_002AFA30(byte *palette)
{
    if (qglColorTableEXT)
        qglColorTableEXT(GL_TEXTURE_2D, 0x1908, 256, 0x8801, 0x1401, palette);
}

extern int texture_extension_number;
void glTexImage2D(int target, int level, int internalformat, int width, int height, int border, int format, int type, void *pixels);
char *strcpy(char *dst, const char *src);

#ifdef NON_MATCHING
int GL_LoadDecalTexture(char *identifier, int textureType, int width, int height, byte *data, byte *palette, byte nummips)
{
    int i;
    gltexture_t *glt;
    gltexture_t *slot;
    byte mipcount;
    byte *pdata;
    int w, h;

    slot = NULL;

retry:
    mipcount = nummips + 1;
    if (identifier[0])
    {
        for (i = 0, glt = D_004F9DD8; i < D_00399640; i++, glt++)
        {
            if (glt->servercount < 0)
            {
                if (!slot)
                    slot = glt;
                continue;
            }

            if (!strcmp(identifier, glt->identifier))
            {
                if (width != glt->width || height != glt->height)
                {
                    identifier[3]++;
                    goto retry;
                }

                if (glt->servercount > 0)
                    glt->servercount = D_00332F2C;
                slot = glt;
                break;
            }
        }
    }
    else
    {
        Con_DPrintf("NULL Texture\n");
    }

    glt = slot;
    if (!glt)
    {
        glt = &D_004F9DD8[D_00399640++];
        if (D_00399640 >= MAX_GLTEXTURES)
        {
            func_002ACFA0();
            Sys_Error("Texture Overflow: MAX_GLTEXTURES");
        }
        glt->texnum = texture_extension_number++;
    }

    strcpy(glt->identifier, identifier);
    glt->mipmap = nummips;
    glt->width = width;
    glt->height = height;

    if (textureType == 4)
        glt->servercount = D_00332F2C;
    else
        glt->servercount = 0;

    GL_Bind(glt->texnum);
    func_002AFA30(palette);

    if (identifier[0] == '@')
        func_0023C600(GL_TEXTURE_2D, 0x2805, 1.0f);

    glTexImage2D(GL_TEXTURE_2D, 0, 1, width, height, 0, 0x1900, 0x1401, data);

    pdata = data + width * height;
    for (i = 1; i < mipcount; i++)
    {
        w = width >> i;
        h = height >> i;
        glTexImage2D(GL_TEXTURE_2D, i, 1, w, h, 0, 0x1900, 0x1401, pdata);
        pdata += w * h;
    }

    if (mipcount)
    {
        func_0023C600(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, D_003333C8);
        func_0023C600(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, D_003333CC);
    }
    else
    {
        func_0023C600(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, D_003333CC);
        func_0023C600(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, D_003333CC);
    }

    return glt->texnum;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", GL_LoadDecalTexture);
#endif

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", GL_BuildDecal);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002AFE28);
