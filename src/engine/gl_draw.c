#include "common.h"
#include "engine/cmd.h"
#include "engine/cvar.h"
#include "engine/client.h"
#include "engine/render.h"

extern int currenttexture;
extern qboolean gl_mtexable;
extern int TEXTURE0_SGIS;
extern void (*D_003994C4)(int target); /* qglSelectTextureSGIS */
extern int D_003333D0;                 /* oldtarget */
extern int D_00333278[];               /* cnttextures */
extern void *font;
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
    byte pad0[0xC];
    int height;                 /* 0x0C */
    fontchar_t chars[256];      /* 0x10 */
} font_t;

extern font_t *D_00399634;

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
    t = ((code >> 8) + 0.5f) * D_0050EF58[2];
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

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002AD388);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", Draw_Init);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", Draw_Character);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", Draw_String);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002ADA70);

INCLUDE_ASM("asm/nonmatchings/engine/gl_draw", func_002ADA98);

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
