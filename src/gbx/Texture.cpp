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
extern int *D_003313E0;          /* linhas de blocos da VRAM em uso */
extern unsigned int *volatile D_003990CC; /* tabela de troca da CLUT */
extern unsigned char D_003313B0[32];
extern const char D_00388A08[]; /* "Gbx/Texture.cpp" */
void *func_00225D48(int size, int align, const char *file, int line);
void func_00225DB8(void *buf);
int func_0023F9C8(int n);
qword *func_0023FC60(qword *p, int tbp, int tbw, int psm, int tw, int th, int cbp, int cpsm,
                     int min, int mag, int mxl, int l, int k, int *mipbp, int *mipbw, int lod);
extern unsigned int D_0032F250; /* máscara de cor do canal */
extern int D_0033134C;
extern int D_0032F264;          /* contexto do GS */
extern unsigned short D_0032F1F4; /* FBP do buffer de desenho */
extern short D_0032F1F6;        /* PSM do buffer */
extern short D_0032F1F8;        /* FBW do buffer */
extern int D_003A9CB8;          /* início da área de texturas na VRAM */
extern int D_003A9CC0[44];      /* registradores do GS guardados */
extern GLTexture *D_003313D4;
void *Mem_DbgMalloc(int size, const char *file, int line);
void func_0023F320(void);
void func_0023FAB0(void);
void func_00241500(const void *src, void *a, void *b, void *c, int n);
qword *func_0023FE10(qword *p, int tbp, int tbw, int psm, int tw, int th, int cbp, int cpsm,
                     int min, int mag, int mxl, int l, int k, int *mipbp, int *mipbw, int lod);
extern int D_003A9CBC;          /* linhas de blocos da VRAM */

#ifdef NON_MATCHING /* contador do laço e ordem dos stores */
void func_0023EFD0(void) {
    int i;

    if (D_003313D8 == 0) {
        D_003313D8 = (GLTexture *)Mem_DbgMalloc(0x445FC, D_00388A08, 0x92);
        memset(D_003313D8, 0, 0x445FC);
    }
    if (D_003313DC == 0) {
        D_003313DC = (GLTexture *(*)[32])Mem_DbgMalloc(0x7400, D_00388A08, 0x98);
        memset(D_003313DC, 0, 0x7400);
    }
    if (D_003313E0 == 0) {
        D_003313E0 = (int *)Mem_DbgMalloc(0x3A0, D_00388A08, 0x9D);
        memset(D_003313E0, 0, 0x3A0);
    }
    if (D_0033134C) {
        D_003A9CB8 = 0xA0000;
        D_003A9CBC = 0xC0;
    } else {
        D_003A9CB8 = 0x8C000;
        D_003A9CBC = 0xE8;
    }
    D_003313D4 = D_003313D8;
    for (i = 0x4F8; i >= 0; i--) {
        D_003313D8[i].x8 = i;
    }
    func_0023FAB0();
    D_003A9CC0[0] = -1;
    D_003A9CC0[1] = -1;
    D_003A9CC0[2] = -1;
    D_003A9CC0[3] = -1;
    D_003A9CC0[4] = -1;
    D_003A9CC0[5] = -1;
    D_003A9CC0[6] = -1;
    D_003A9CC0[7] = -1;
    D_003A9CC0[8] = -1;
    D_003A9CC0[9] = -1;
    D_003A9CC0[10] = 0;
    D_003A9CC0[11] = 0;
    D_003A9CC0[12] = 0;
    D_003A9CC0[21] = 0;
    D_003A9CC0[20] = 0;
    D_003A9CC0[13] = -1;
    D_003A9CC0[14] = -1;
    D_003A9CC0[15] = -1;
    D_003A9CC0[16] = -1;
    D_003A9CC0[17] = -1;
    D_003A9CC0[18] = -1;
    D_003A9CC0[19] = -1;
    D_003A9CC0[22] = -1;
    D_003A9CC0[23] = -1;
    D_003A9CC0[24] = -1;
    D_003A9CC0[25] = -1;
    D_003A9CC0[26] = -1;
    D_003A9CC0[27] = -1;
    D_003A9CC0[28] = -1;
    D_003A9CC0[29] = -1;
    D_003A9CC0[30] = -1;
    D_003A9CC0[31] = -1;
    D_003A9CC0[32] = 0;
    D_003A9CC0[33] = 0;
    D_003A9CC0[41] = -1;
    D_003A9CC0[34] = 0;
    D_003A9CC0[43] = 0;
    D_003A9CC0[42] = 0;
    D_003A9CC0[35] = -1;
    D_003A9CC0[36] = -1;
    D_003A9CC0[37] = -1;
    D_003A9CC0[38] = -1;
    D_003A9CC0[39] = -1;
    D_003A9CC0[40] = -1;
    func_0023F320();
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Texture", func_0023EFD0);
#endif

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

void func_0023F230(int n, const GLuint *ids) {
    GLTexture *t;
    int i;

    for (i = 0; i < n; i++) {
        t = &D_003313D8[ids[i]];
        if (t->a) {
            t->a = 0;
            t->b = 0;
            t->xB0 = 0;
            t->xC4[0] = 0;
            t->xC4[1] = 0;
            t->xC4[2] = 0;
            t->xC4[3] = 0;
            t->xC4[4] = 0;
            t->xC4[5] = 0;
            t->xB4 = 0;
            if (t->xB8) {
                func_00225DB8(t->xB8);
            }
            if (t->xBC) {
                func_00225DB8(t->xBC);
            }
            if (t->xC0) {
                func_00225DB8(t->xC0);
            }
            t->xB8 = 0;
            t->xBC = 0;
            t->xC0 = 0;
            func_00240500(t);
        }
    }
}

void func_0023F320(void) {
    GLTexture *t;
    int i;

    D_003313D0 = 0;
    D_003990C8 = 0;
    t = D_003313D8;
    for (i = 0; i < 0x4F9; i++, t++) {
        t->x84 = 0;
        t->x88 = 0;
        t->x98[0] = 0;
        t->x98[1] = 0;
        t->x98[2] = 0;
        t->x98[3] = 0;
        t->x98[4] = 0;
        t->x98[5] = 0;
        t->x8C[0] = 0;
        t->x8C[1] = 0;
        t->x8C[2] = 0;
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

int func_0023F948(int w, int h, int psm8) {
    int n;

    if (psm8) {
        w = (w + 127) / 128;
        n = (h + 63) / 64;
    } else {
        w = (w + 63) / 64;
        n = (h + 31) / 32;
    }
    return func_0023F9C8(w * n);
}

int func_0023F9C8(int n) {
    int start;
    int *p;
    GLTexture **q;
    int j;

    if (D_003A9CBC < D_003313D0 + n) {
        D_003313D0 = 0;
    }
    start = D_003313D0;
    D_003313D0 = start + n;
    p = &D_003313E0[start];
    q = D_003313DC[start];
    for (; n != 0; n--) {
        *p = 0;
        for (j = 0; j < 32; j++) {
            if (q[j]) {
                func_00240500(q[j]);
                q[j] = 0;
            }
        }
        p++;
        q += 32;
    }
    return start;
}

#ifdef NON_MATCHING /* registradores da soma do endereço */
void func_0023FAB0(void) {
    unsigned int i;
    unsigned int h;

    D_003990CC = (unsigned int *)func_00225D48(0x400, 0x80, D_00388A08, 0x300);
    for (i = 0; i < 0x100; i++) {
        h = i >> 1;
        D_003990CC[D_003313B0[i & 0x1F] + (i & ~0x1F)] = (h << 24) + (h << 16) + (h << 8) + h;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Texture", func_0023FAB0);
#endif

qword *func_0023FB58(qword *p, int dbp, int dbw, int psm, int w, int h, int src) {
    int n;

    if (psm) {
        n = w * h / 16;
    } else {
        n = w * h / 4;
    }
    *(unsigned long *)p = 0x8000000010000006;
    p++;
    ((unsigned long *)p)[0] = 0x1000000000008004;
    ((unsigned long *)p)[1] = 0xE;
    p++;
    ((unsigned long *)p)[0] = ((unsigned long)dbp << 32) | ((unsigned long)dbw << 48) | ((unsigned long)psm << 56);
    ((unsigned long *)p)[1] = 0x50;
    p++;
    ((unsigned long *)p)[0] = 0;
    ((unsigned long *)p)[1] = 0x51;
    p++;
    ((unsigned long *)p)[0] = w | ((unsigned long)h << 32);
    ((unsigned long *)p)[1] = 0x52;
    p++;
    ((unsigned long *)p)[0] = 0;
    ((unsigned long *)p)[1] = 0x53;
    p++;
    *(unsigned long *)p = (n | 0x8000) | ((unsigned long)3 << 59);
    p++;
    *(unsigned long *)p = (n | 0x30000000) | ((unsigned long)src << 32);
    return p + 1;
}

#ifdef NON_MATCHING /* agendamento dos stores do pacote */
qword *func_0023FC60(qword *p, int tbp, int tbw, int psm, int tw, int th, int cbp, int cld,
                     int mmin, int mmag, int mxl, int l, int k, int *mipbp, int *mipbw, int eop) {
    int ctx;
    int *c;
    unsigned long *q;

    ctx = D_0032F264;
    c = &D_003A9CC0[ctx * 22];
    q = (unsigned long *)p;
    q[1] = 0xE;
    q[0] = ((unsigned long)eop << 15) | 0x1000000000000004;
    c[13] = tbp;
    c[14] = tbw;
    c[15] = psm;
    c[16] = tw;
    c[17] = th;
    c[18] = cbp;
    q[3] = 0x3F;
    q[4] = ((unsigned long)mxl << 2) | ((unsigned long)mmag << 5) | ((unsigned long)mmin << 6) |
           ((unsigned long)l << 19) | ((unsigned long)k << 32);
    q[5] = ctx + 0x14;
    q[6] = tbp | ((unsigned long)tbw << 14) | ((unsigned long)psm << 20) | ((unsigned long)tw << 26) |
           ((unsigned long)th << 30) | ((unsigned long)1 << 34) | ((unsigned long)cbp << 37) |
           ((unsigned long)cld << 61);
    q[7] = ctx + 6;
    q[8] = (short)D_0032F1F4 | ((unsigned long)D_0032F1F8 << 16) | ((unsigned long)D_0032F1F6 << 24) |
           ((unsigned long)D_0032F250 << 32);
    c[20] = (short)D_0032F1F4;
    c[21] = D_0032F250;
    q[9] = ctx + 0x4C;
    return p + 5;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Texture", func_0023FC60);
#endif

INCLUDE_ASM("asm/nonmatchings/gbx/Texture", func_0023FE10);

void func_002403F8(GLTexture *t) {
    void *a;
    void *b;
    void *c;

    if (t->xB8 == 0) {
        t->xB8 = func_00225D48(t->x24, 0x80, D_00388A08, 0x467);
        t->xBC = func_00225D48(t->x24, 0x80, D_00388A08, 0x468);
        t->xC0 = func_00225D48(t->x24, 0x80, D_00388A08, 0x469);
    }
    if (t->x8 == 0x4F8) {
        D_003990C8 = 0;
    } else {
        t->x84 = 0;
        t->x88 = 0;
        t->x98[0] = 0;
        t->x98[1] = 0;
        t->x98[2] = 0;
        t->x98[3] = 0;
        t->x98[4] = 0;
        t->x98[5] = 0;
        t->x8C[0] = 0;
        t->x8C[1] = 0;
        t->x8C[2] = 0;
    }
    a = (void *)((int)t->xB8 | 0x30000000);
    b = (void *)((int)t->xBC | 0x30000000);
    c = (void *)((int)t->xC0 | 0x30000000);
    func_00241500(t->xB0, c, b, a, t->x10 * t->x14);
    __asm__ volatile("sync.l\n\tnop");
}

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
    t->x98[0] = 0;
    t->x98[1] = 0;
    t->x98[2] = 0;
    t->x98[3] = 0;
    t->x98[4] = 0;
    t->x98[5] = 0;
    t->x8C[0] = 0;
    t->x8C[1] = 0;
    t->x8C[2] = 0;
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

#ifdef NON_MATCHING /* o gcc junta os stores dos argumentos na pilha */
qword *func_002407F0(GLTexture *t, qword *p) {
    qword *r;

    if (t->b) {
        r = func_0023FE10(p, t->x8C[0], t->x20, t->xC, t->x18, t->x1C, D_003990C8, 4,
                          t->x28, t->x2C, 0, 0, 0, 0, 0, 1);
    } else if (t->xC == 0) {
        r = func_0023FE10(p, t->x84, t->x20, 0, t->x18, t->x1C, 0, 0,
                          t->x28, t->x2C, 0, 0, 0, 0, 0, 1);
    } else {
        r = func_0023FE10(p, t->x84, t->x20, t->xC, t->x18, t->x1C, t->x88, 4,
                          t->x28, t->x2C, t->x30, t->x34, t->x38, t->x98, t->x6C, 1);
    }
    return r;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Texture", func_002407F0);
#endif

qword *func_00240928(GLTexture *t, qword *p, int ch) {
    switch (ch) {
    case 0:
        D_0032F250 = 0xFF00FFFF;
        break;
    case 1:
        D_0032F250 = 0xFFFF00FF;
        break;
    case 2:
        D_0032F250 = 0xFFFFFF00;
        break;
    }
    return func_0023FC60(p, t->x8C[ch], t->x20, t->xC, t->x18, t->x1C, D_003990C8, 4,
                         t->x28, t->x2C, 0, 0, 0, 0, 0, 0);
}

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
