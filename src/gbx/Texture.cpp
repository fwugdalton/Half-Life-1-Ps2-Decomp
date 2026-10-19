/* Gbx: texturas do GL no PS2 (upload para a VRAM do GS) */

#include "common.h"
#include "gbx/gl.h"

extern "C" {

extern GLTexture *D_003313D8;   /* tabela de texturas (0x4F9) */
extern int D_003313A8;
int func_0023F3F0(int x);
void func_00240500(GLTexture *t);
void func_002403F8(GLTexture *t);

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

INCLUDE_ASM("asm/nonmatchings/gbx/Texture", func_0023F320);

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

INCLUDE_ASM("asm/nonmatchings/gbx/Texture", func_00240500);

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

INCLUDE_ASM("asm/nonmatchings/gbx/Texture", func_00240658);

INCLUDE_ASM("asm/nonmatchings/gbx/Texture", func_002406C8);

INCLUDE_ASM("asm/nonmatchings/gbx/Texture", func_00240738);

INCLUDE_ASM("asm/nonmatchings/gbx/Texture", func_002407A8);

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
