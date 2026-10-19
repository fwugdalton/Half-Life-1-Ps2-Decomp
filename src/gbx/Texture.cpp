/* Gbx: texturas do GL no PS2 (upload para a VRAM do GS) */

#include "common.h"
#include "gbx/gl.h"

extern "C" {

extern GLTexture *D_003313D8;   /* tabela de texturas (0x4F9) */
extern int D_003313A8;
int func_0023F3F0(int x);
void func_00240500(GLTexture *t);
void func_002403F8(GLTexture *t);
void *memset(void *p, int c, unsigned int n);
extern int D_003313D0;
extern int D_003990C8;
extern GLTexture *(*D_003313DC)[32]; /* blocos da VRAM */
extern void *D_003313E0;
extern int D_003A9CBC;          /* linhas de blocos da VRAM */

INCLUDE_ASM("asm/nonmatchings/gbx/Texture", func_0023EFD0);

void func_0023F1D8(void) {
}

void func_0023F1E0(int n, GLuint *ids) {
    GLTexture *t;
    int i;

    t = D_003313D8 + 1;
    for (i = 1; i < 0x4F9; i++, t++) {
        if (t->a == 0) {
            *ids++ = t->x8;
            if (--n == 0) {
                break;
            }
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/gbx/Texture", func_0023F230);

void func_0023F320(void) {
    GLTexture *t;
    int i;

    D_003313D0 = 0;
    D_003990C8 = 0;
    t = D_003313D8;
    for (i = 0; i < 0x4F9; i++, t++) {
        t->x84 = 0;
        t->x88 = 0;
        t->x98 = 0;
        t->x9C = 0;
        t->xA0 = 0;
        t->xA4 = 0;
        t->xA8 = 0;
        t->xAC = 0;
        t->x8C = 0;
        t->x90 = 0;
        t->x94 = 0;
    }
    memset(D_003313DC, 0, 0x7400);
    memset(D_003313E0, 0, 0x3A0);
}

int func_0023F3B8(int x) {
    int b;

    for (b = 0x400; b != 0; b >>= 1) {
        if (b & x) {
            if (b == x) {
                return b;
            }
            return b << 1;
        }
    }
    return 0x800;
}

int func_0023F3F0(int x) {
    int n;

    for (n = 0; n < 32 && (x >>= 1) != 0; n++) {
    }
    return n;
}

INCLUDE_ASM("asm/nonmatchings/gbx/Texture", func_0023F420);

INCLUDE_ASM("asm/nonmatchings/gbx/Texture", func_0023F948);

INCLUDE_ASM("asm/nonmatchings/gbx/Texture", func_0023F9C8);

INCLUDE_ASM("asm/nonmatchings/gbx/Texture", func_0023FAB0);

INCLUDE_ASM("asm/nonmatchings/gbx/Texture", func_0023FB58);

INCLUDE_ASM("asm/nonmatchings/gbx/Texture", func_0023FC60);

INCLUDE_ASM("asm/nonmatchings/gbx/Texture", func_0023FE10);

INCLUDE_ASM("asm/nonmatchings/gbx/Texture", func_002403F8);

#ifdef NON_MATCHING /* o alvo guarda o limite do laço num registrador */
void func_00240500(GLTexture *t) {
    GLTexture **p;
    GLTexture **q;
    int n;
    int i;
    int j;

    if (t->x8 == 0x4F8) {
        D_003990C8 = 0;
        return;
    }
    t->x84 = 0;
    t->x88 = 0;
    t->x98 = 0;
    t->x9C = 0;
    t->xA0 = 0;
    t->xA4 = 0;
    t->xA8 = 0;
    t->xAC = 0;
    t->x8C = 0;
    t->x90 = 0;
    t->x94 = 0;
    p = D_003313DC[0];
    for (i = 0; i < D_003A9CBC; i++) {
        q = p;
        p += 32;
        for (j = 0; j < 32; j++) {
            if (*q == t) {
                *q = 0;
            }
            q++;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Texture", func_00240500);
#endif

void func_002405A8(GLTexture *t, int w, int h) {
    int n;

    t->x14 = h;
    t->x10 = w;
    t->x18 = func_0023F3F0(w);
    t->x1C = func_0023F3F0(t->x14);
    t->x30 = 0;
    n = t->x10 >> 6;
    t->x20 = n ? n : 1;
    func_00240500(t);
}

void func_00240608(GLTexture *t, const void *pixels) {
    t->a = 1;
    t->xC = 0x13;
    t->x24 = t->x10 * t->x14;
    if (((int)pixels & 0xF) == 0) {
        t->xB0 = pixels;
    }
    if (t->x20 & 1) {
        t->x20++;
    }
}

void func_00240658(GLTexture *t, const void *pixels) {
    t->a = 1;
    t->xC = 0;
    t->x24 = t->x10 * t->x14 * 4;
    if (((int)pixels & 0xF) == 0) {
        t->xB0 = pixels;
    }
    if (t->b) {
        t->xC = 0x13;
        t->x24 = t->x10 * t->x14;
        func_002403F8(t);
    }
}

void func_002406C8(GLTexture *t, const void *pixels) {
    t->a = 1;
    t->xC = 0;
    t->x24 = t->x10 * t->x14 * 4;
    if (((int)pixels & 0xF) == 0) {
        t->xB0 = pixels;
    }
    if (t->b) {
        t->xC = 0x13;
        t->x24 = t->x10 * t->x14;
        func_002403F8(t);
    }
}

void func_00240738(GLTexture *t, int level, int w, int h, const void *pixels) {
    int n;

    t->x3C[level] = w;
    t->x54[level] = h;
    n = w >> 6;
    t->x6C[level] = n ? n : 1;
    if (t->x6C[level] & 1) {
        t->x6C[level]++;
    }
    t->x30 = level + 1 < t->x30 ? t->x30 : level + 1;
    if (((int)pixels & 0xF) == 0) {
        t->xC4[level] = pixels;
    }
}

void func_002407A8(GLTexture *t, int x, int y, int w, int h, const void *pixels) {
    t->xB0 = pixels;
    if (t->b) {
        func_002403F8(t);
    }
}

void func_002407D8(GLTexture *t, const void *p) {
    if (((int)p & 0xF) == 0) {
        t->xB4 = p;
    }
}

INCLUDE_ASM("asm/nonmatchings/gbx/Texture", func_002407F0);

INCLUDE_ASM("asm/nonmatchings/gbx/Texture", func_00240928);

INCLUDE_ASM("asm/nonmatchings/gbx/Texture", func_002409F8);

INCLUDE_ASM("asm/nonmatchings/gbx/Texture", func_00240D60);

unsigned int func_00241408(void) {
    if (D_003313A8 == 1) {
        D_003313A8 = 0;
        return 0x70001000;
    }
    D_003313A8 = 1;
    return 0x70000000;
}

INCLUDE_RODATA("asm/nonmatchings/gbx/Texture", D_00388A08);

INCLUDE_RODATA("asm/nonmatchings/gbx/Texture", D_00388A18);

INCLUDE_RODATA("asm/nonmatchings/gbx/Texture", D_00388A28);
}
