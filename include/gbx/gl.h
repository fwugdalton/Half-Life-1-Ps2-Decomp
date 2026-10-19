#ifndef GBX_GL_H
#define GBX_GL_H

/* tipos do OpenGL do PS2 (Gbx) */

typedef unsigned int GLenum;
typedef int GLint;
typedef float GLfloat;

/* matriz 4x4 alinhada a 16 (copiada com lq/sq) */
typedef struct {
    float m[16];
} __attribute__((aligned(16))) GLMatrix;

typedef int qword __attribute__((mode(TI)));

/* vetor de 4 floats alinhado a 16 */
typedef struct {
    float v[4];
} __attribute__((aligned(16))) GLVec;

/* textura (0xDC bytes) */
typedef struct {
    int a;
    int b;
    char pad8[0x20];
    int x28;        /* filtro de redução */
    int x2C;        /* filtro de ampliação */
    int x30;        /* usa névoa */
    int x34;
    int x38;
    char pad3C[0xA0];
} GLTexture;

typedef unsigned int GLuint;

/* matriz identidade com o VU0 (m é GLMatrix) */
#define MIDENTITY(t) __asm__ volatile( \
    "vsub.xyzw $vf4, $vf0, $vf0\n\t" \
    "vadd.w $vf4, $vf4, $vf0\n\t" \
    "vmr32.xyzw $vf5, $vf4\n\t" \
    "vmr32.xyzw $vf6, $vf5\n\t" \
    "vmr32.xyzw $vf7, $vf6\n\t" \
    "sqc2 $vf4, %0\n\t" \
    "sqc2 $vf5, %1\n\t" \
    "sqc2 $vf6, %2\n\t" \
    "sqc2 $vf7, %3" \
    : "=m"(*(GLVec *)&(t).m[12]), "=m"(*(GLVec *)&(t).m[8]), "=m"(*(GLVec *)&(t).m[4]), "=m"(*(GLVec *)&(t).m[0]))

/* d = d * t com o VU0 */
#define MMUL(d, t) __asm__ volatile( \
    "lqc2 $vf4, 0x0(%0)\n\t" \
    "lqc2 $vf5, 0x10(%0)\n\t" \
    "lqc2 $vf6, 0x20(%0)\n\t" \
    "lqc2 $vf7, 0x30(%0)\n\t" \
    "lqc2 $vf8, %1\n\t" \
    "lqc2 $vf9, %2\n\t" \
    "lqc2 $vf10, %3\n\t" \
    "lqc2 $vf11, %4\n\t" \
    "vmulax.xyzw $ACC, $vf4, $vf8x\n\t" \
    "vmadday.xyzw $ACC, $vf5, $vf8y\n\t" \
    "vmaddaz.xyzw $ACC, $vf6, $vf8z\n\t" \
    "vmaddw.xyzw $vf12, $vf7, $vf8w\n\t" \
    "vmulax.xyzw $ACC, $vf4, $vf9x\n\t" \
    "vmadday.xyzw $ACC, $vf5, $vf9y\n\t" \
    "vmaddaz.xyzw $ACC, $vf6, $vf9z\n\t" \
    "vmaddw.xyzw $vf13, $vf7, $vf9w\n\t" \
    "vmulax.xyzw $ACC, $vf4, $vf10x\n\t" \
    "vmadday.xyzw $ACC, $vf5, $vf10y\n\t" \
    "vmaddaz.xyzw $ACC, $vf6, $vf10z\n\t" \
    "vmaddw.xyzw $vf14, $vf7, $vf10w\n\t" \
    "vmulax.xyzw $ACC, $vf4, $vf11x\n\t" \
    "vmadday.xyzw $ACC, $vf5, $vf11y\n\t" \
    "vmaddaz.xyzw $ACC, $vf6, $vf11z\n\t" \
    "vmaddw.xyzw $vf15, $vf7, $vf11w\n\t" \
    "sqc2 $vf12, 0x0(%0)\n\t" \
    "sqc2 $vf13, 0x10(%0)\n\t" \
    "sqc2 $vf14, 0x20(%0)\n\t" \
    "sqc2 $vf15, 0x30(%0)" \
    : : "r"(d), "m"(*(GLVec *)&(t).m[0]), "m"(*(GLVec *)&(t).m[4]), "m"(*(GLVec *)&(t).m[8]), "m"(*(GLVec *)&(t).m[12]) : "memory")

#define MIDENTITYP(p) __asm__ volatile( \
    "vsub.xyzw $vf4, $vf0, $vf0\n\t" \
    "vadd.w $vf4, $vf4, $vf0\n\t" \
    "vmr32.xyzw $vf5, $vf4\n\t" \
    "vmr32.xyzw $vf6, $vf5\n\t" \
    "vmr32.xyzw $vf7, $vf6\n\t" \
    "sqc2 $vf4, 0x30(%0)\n\t" \
    "sqc2 $vf5, 0x20(%0)\n\t" \
    "sqc2 $vf6, 0x10(%0)\n\t" \
    "sqc2 $vf7, 0x0(%0)" : : "r"(p) : "memory")

/* registradores do GS (libgraph) */
typedef struct {
    unsigned long EN1 : 1;
    unsigned long EN2 : 1;
    unsigned long CRTMD : 3;
    unsigned long MMOD : 1;
    unsigned long AMOD : 1;
    unsigned long SLBG : 1;
    unsigned long ALP : 8;
    unsigned long p0 : 48;
} sceGsPmode;

typedef struct {
    unsigned long INT : 1;
    unsigned long FFMD : 1;
    unsigned long DPMS : 2;
    unsigned long p0 : 60;
} sceGsSmode2;

typedef struct {
    unsigned long FBP : 9;
    unsigned long FBW : 6;
    unsigned long PSM : 5;
    unsigned long p0 : 12;
    unsigned long DBX : 11;
    unsigned long DBY : 11;
    unsigned long p1 : 10;
} sceGsDispfb;

typedef struct {
    unsigned long DX : 12;
    unsigned long DY : 11;
    unsigned long MAGH : 4;
    unsigned long MAGV : 2;
    unsigned long p0 : 3;
    unsigned long DW : 12;
    unsigned long DH : 11;
    unsigned long p1 : 9;
} sceGsDisplay;

typedef struct {
    unsigned long R : 8;
    unsigned long G : 8;
    unsigned long B : 8;
    unsigned long p0 : 40;
} sceGsBgcolor;

typedef struct {
    unsigned long NLOOP : 15;
    unsigned long EOP : 1;
    unsigned long pad16 : 16;
    unsigned long id : 14;
    unsigned long PRE : 1;
    unsigned long PRIM : 11;
    unsigned long FLG : 2;
    unsigned long NREG : 4;
    unsigned long REGS : 64;
} __attribute__((aligned(16))) sceGifTag;

/* ambiente de exibição (0x40 bytes) */
typedef struct {
    sceGsPmode pmode;
    sceGsSmode2 smode2;
    sceGsDispfb dispfb2;
    sceGsDisplay display2;
    sceGsBgcolor bgcolor;
    sceGsDispfb dispfb1;
    sceGsDisplay display1;
    unsigned long pad38;
} GLDispEnv;

/* estado da tela: dois ambientes de exibição e dois de desenho */
typedef struct {
    GLDispEnv disp[2];
    sceGifTag tag0;
    char pad90[0xD0];
    char pad160[0x80];
    sceGifTag tag1;
    char pad1F0[0xD0];
    char pad2C0[0x80];
    short x340;
    short x342;
    short x344;
    short x346;
    short x348;
    short x34A;
    short x34C;
} GLScreen;

/* cópia de 16 bytes com lq/sq */
#define QCOPY(d, s) __asm__ volatile("lq $3, 0(%1)\n\tsq $3, 0(%0)" : : "r"(d), "r"(s) : "$3", "memory")

/* cópia de uma matriz 4x4 com lq/sq (registrador temporário escolhido pelo gcc) */
#define MCOPYT(d, s) do { \
    qword _t; \
    __asm__ volatile( \
        "lq %0, 0x0(%2)\n\tsq %0, 0x0(%1)\n\t" \
        "lq %0, 0x10(%2)\n\tsq %0, 0x10(%1)\n\t" \
        "lq %0, 0x20(%2)\n\tsq %0, 0x20(%1)\n\t" \
        "lq %0, 0x30(%2)\n\tsq %0, 0x30(%1)" : "=&r"(_t) : "r"(d), "r"(s) : "memory"); \
} while (0)

/* cópia de uma matriz 4x4 com lq/sq */
#define MCOPY(d, s) __asm__ volatile( \
    "lq $5, 0x0(%1)\n\tsq $5, 0x0(%0)\n\t" \
    "lq $5, 0x10(%1)\n\tsq $5, 0x10(%0)\n\t" \
    "lq $5, 0x20(%1)\n\tsq $5, 0x20(%0)\n\t" \
    "lq $5, 0x30(%1)\n\tsq $5, 0x30(%0)" : : "r"(d), "r"(s) : "$5", "memory")

/* GsEnv.c */
int func_00239D88(int n);
void func_0023A150(GLScreen *s, short w, short h, short psm);
void func_0023A2F0(GLScreen *s, short w, short h, short psm, short fbp0, short fbp1);
void func_0023A4E8(GLScreen *s, short w, short h, short psm, short a, short b, short c, short zpsm, short a5, short a6);
void func_002D0BF0(GLDispEnv *disp, short psm, short w, short h, short dx, short dy);
void func_0023A1B0(GLScreen *s, short w, short h, short psm, short zpsm, short a5, short a6);
void func_0023A2A8(GLScreen *s);
void func_0023A708(GLScreen *s, int buf, int en1);
void func_0023A858(GLScreen *s, int buf, int full);

#endif
