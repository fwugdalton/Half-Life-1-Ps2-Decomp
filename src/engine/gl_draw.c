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
int GL_LoadDecalTexture(char *name, int type, int width, int height, byte *pixels, byte *palette, int nummips);

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

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002ADB98);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002ADE10);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002ADFB0);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002AE158);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002AE468);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002AE718);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002AEA00);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002AEB98);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002AEC60);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002AEC90);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002AECD0);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002AED58);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002AEEC0);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002AF0A8);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002AF0D0);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002AF170);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", Draw_ConsoleBackground);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002AF498);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002AF668);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002AF6A0);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002AF7A8);

void func_002AF7D8(void) {
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002AF7E0);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_draw", D_00394938);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002AF838);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", GL_FreeDecalTexture);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002AF9A0);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002AFA30);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", GL_LoadDecalTexture);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", GL_BuildDecal);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002AFE28);
