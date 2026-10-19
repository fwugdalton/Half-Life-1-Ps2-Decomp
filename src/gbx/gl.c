/* Gbx: implementação do OpenGL do PS2 (pacotes GIF para o GS) */

#include "common.h"

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
    char pad8[0x28];
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

/* estado da tela (FRAME e DISPLAY do GS) */
typedef struct {
    char pad0[0x160];
    char pad160[0x160];
    char pad2C0[0x80];
    short x340;
    short pad342;
    short x344;
    short x346;
    short x348;
    short pad34A;
    short x34C;
} GLScreen;

/* cópia de 16 bytes com lq/sq */
#define QCOPY(d, s) __asm__ volatile("lq $3, 0(%1)\n\tsq $3, 0(%0)" : : "r"(d), "r"(s) : "$3", "memory")

/* cópia de uma matriz 4x4 com lq/sq */
#define MCOPY(d, s) __asm__ volatile( \
    "lq $5, 0x0(%1)\n\tsq $5, 0x0(%0)\n\t" \
    "lq $5, 0x10(%1)\n\tsq $5, 0x10(%0)\n\t" \
    "lq $5, 0x20(%1)\n\tsq $5, 0x20(%0)\n\t" \
    "lq $5, 0x30(%1)\n\tsq $5, 0x30(%0)" : : "r"(d), "r"(s) : "$5", "memory")

extern GLMatrix *D_00331140;    /* topo da pilha de projeção */
extern GLMatrix *D_00331144;    /* topo da pilha de modelview */
extern int D_0033104C;          /* matrizes mudaram */
extern GLenum D_0032F268;       /* glMatrixMode */
extern int D_0033137C;
extern float D_00331138;        /* glPolygonOffset */
extern int D_0032F200;          /* glShadeModel */
extern int D_0032F214;          /* escrita de profundidade desligada */
extern int D_0032F218;          /* teste de profundidade */
extern int D_0032F21C;          /* GL_DEPTH_TEST */
extern int D_0032F220;
extern int D_0032F224;
extern int D_00331040;          /* GL_CULL_FACE */
extern int D_0032F204;          /* GL_BLEND */
extern int D_0032F208;          /* GL_ALPHA_TEST */
extern int D_0032F20C;          /* função do teste de alfa */
extern int D_0032F210;          /* referência do teste de alfa */
extern int D_0032F228;          /* ALPHA do GS: A, B, C, D e FIX */
extern int D_0032F22C;
extern int D_0032F230;
extern int D_0032F234;
extern int D_0032F238;
extern volatile int D_0032F23C;          /* GL_POLYGON_OFFSET_FILL */
extern int D_0032F240;          /* GL_FOG */
extern int D_0032F25C;          /* GL_TEXTURE_2D */
extern int D_00331358;
extern int D_0033102C;          /* arrays de vértices ligados */
extern const void *D_00331030;  /* glVertexPointer */
extern const void *D_00331034;  /* glColorPointer */
extern const void *D_00331038;  /* glTexCoordPointer */
extern GLVec D_0032F1E0;        /* coordenada de textura atual */
extern GLVec sprite_color;      /* cor atual */
extern GLVec *D_00331020;       /* próxima cor do buffer */
extern GLVec *D_00331024;       /* próxima coordenada de textura */
extern GLVec *D_00331028;       /* próximo vértice */
extern int D_0033103C;          /* vértices no buffer */
extern float D_0032F254;        /* GL_FOG_START */
extern float D_0032F258;        /* GL_FOG_END */
extern int D_0032F244;          /* cor da névoa */
extern GLTexture *D_003313D4;   /* textura ligada */
extern GLTexture *D_003313D8;
void func_0023D430(int a);
void func_002407D8(GLTexture *tex, const void *table);
void func_002407A8(GLTexture *tex, int x, int y, int w, int h, const void *pixels);
void func_002409F8(GLTexture *t);
void func_00240D60(GLTexture *t);
void func_0023D258(void);
qword *func_0023D778(qword *p);
void func_0023F1D8(void);
void func_0023F1E0(void);
void func_0023F230(void);
int func_002F00E8(int a);
void func_002EF360(int a, int b);
extern unsigned int *D_0032F160;
extern const char *D_0032F18C;  /* GL_VENDOR */
extern const char *D_0032F190;  /* GL_RENDERER */
extern const char *D_0032F194;  /* GL_VERSION */
extern const char *D_0032F198;  /* GL_EXTENSIONS */
extern int D_0032F260;          /* modo do glTexEnv */
extern short D_00331340;        /* cor de limpeza */
extern short D_00331342;
extern short D_00331344;
extern short D_00331346;
extern float D_003337EC;
extern GLMatrix D_00331080;     /* matriz do viewport */
extern int D_0032EF80;          /* largura da tela */
extern int D_0032EF84;          /* altura da tela */
typedef int GLsizei;
float cosf(float x);
extern float D_00333804;
extern float D_00333864;
extern float D_00331384;        /* densidade da névoa */
extern int D_003A9C98;
extern int D_003A9C9C;
extern int D_00331390;
extern int D_00331394;
float func_002D8268(float x);
void func_0023ECF8(qword *p, int mode);
qword *func_0023EE68(qword *p, int mode);
void func_0023D2B8(void);
void func_0023D520(void);
qword *func_0023D748(void);
qword *func_0023ECD8(qword *p);
qword *func_0023D3B0(qword *p);
extern int D_00331364;
extern int D_00331350;          /* buffer de tela atual */
void func_0023A708(int a, int b, int c);
void func_0023A858(int a, int b, int c);
extern int D_00331368;
extern int D_003A9C90;
extern unsigned short D_0032F1F0;
extern unsigned short D_0032F1F2;
extern unsigned short D_0032F1F4;
extern int D_00331374;
void func_0023E9E8(qword *p);
void func_0023AA18(void);
extern qword *D_003A9CA0;       /* fim do pacote atual */
extern qword *D_003A9CA4;       /* início do pacote atual */
int func_002EF690(int sema);
/* registradores do GS guardados (0x32EF88..0x32F00C) */
extern int D_0032EF88;
extern int D_0032EF8C;
extern int D_0032EF90;
extern int D_0032EF94;
extern int D_0032EF98;
extern int D_0032EF9C;
extern int D_0032EFA0;
extern int D_0032EFA4;
extern int D_0032EFA8;
extern int D_0032EFAC;
extern int D_0032EFB0;
extern int D_0032EFB4;
extern int D_0032EFB8;
extern int D_0032EFBC;
extern int D_0032EFC0;
extern int D_0032EFC4;
extern int D_0032EFC8;
extern int D_0032EFCC;
extern int D_0032EFD0;
extern int D_0032EFD4;
extern int D_0032EFD8;
extern int D_0032EFDC;
extern int D_0032EFE0;
extern int D_0032EFE4;
extern int D_0032EFE8;
extern int D_0032EFEC;
extern int D_0032EFF0;
extern int D_0032EFF4;
extern int D_0032EFF8;
extern int D_0032EFFC;
extern int D_0032F000;
extern int D_0032F004;
extern int D_0032F008;
extern int D_0032F00C;
extern int D_00331044;          /* primitiva do GS */
extern int D_00331048;          /* buffer duplo de vértices */
extern GLVec D_0032F280[];      /* buffers de vértices (cor, textura, posição) */
extern GLVec D_0032F780[];
extern GLVec D_0032FC80[];
extern GLVec D_00330180[];
extern GLVec D_00330680[];
extern GLVec D_00330B80[];
void func_002405A8(GLTexture *t, int w, int h);
void func_00240608(GLTexture *t, const void *pixels);
void func_00240658(GLTexture *t, const void *pixels);
void func_002406C8(GLTexture *t, const void *pixels);
void func_00240738(GLTexture *t, int level, int w, int h, const void *pixels);
float sinf(float x);
extern int D_00331360;
extern int D_0033135C;
int func_002EF6A0(int sema);
qword *func_002407F0(GLTexture *t, qword *p);
extern float D_0032F26C;
extern int D_0032F180;
extern int D_003A9C94;
extern int D_00331370;
extern int D_00331378;
extern int D_00331380;
extern int D_0033138C;


int func_00239D88(int n) {
    int i;

    for (i = 0; i < 32; i++) {
        n >>= 1;
        if (n == 0) {
            break;
        }
    }
    return i;
}

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_00239DB8);

void func_00239ED0(unsigned long *p, short fbp, short psm, int fbw) {
    unsigned long v;

    v = (fbw << 16) | (long)fbp | ((long)psm << 24);
    p[0] = v | 0xFFFFFFFF00000000;
    p[1] = 0x4C;
    p += 14;
    p[1] = 0x4C;
    p[0] = v;
}

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_00239F28);

void func_0023A150(GLScreen *s, short w, short h) {
    short x;
    short y;

    if (h == 0x200) {
        if (w != 0x200) {
            return;
        }
        y = 0x80;
        x = 0x100;
    } else if (w != 0x200) {
        if (w != 0x280) {
            return;
        }
        y = 0x8C;
        x = 0x118;
    } else {
        y = 0x70;
        x = 0xE0;
    }
    s->x34C = x;
    s->x348 = y;
    s->x346 = 0;
}

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023A1B0);

void func_0023A2A8(GLScreen *s) {
    func_00239ED0((unsigned long *)s->pad160, s->x346, s->x344, s->x340);
    func_00239ED0((unsigned long *)s->pad2C0, s->x348, s->x344, s->x340);
}

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023A2F0);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023A4E8);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023A708);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023A858);

void func_0023AA00(void) {
    *D_0032F160 |= 6;
}

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023AA18);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023AB00);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023AB28);

void func_0023AD90(void) {
}

void func_0023AD98(void) {
    func_0023D258();
}

INCLUDE_RODATA("asm/nonmatchings/gbx/gl", D_00388940);

INCLUDE_RODATA("asm/nonmatchings/gbx/gl", D_00388958);

INCLUDE_RODATA("asm/nonmatchings/gbx/gl", D_00388970);

INCLUDE_RODATA("asm/nonmatchings/gbx/gl", D_00388978);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023ADB8);

void func_0023AF68(void) {
    if (D_003313D4->b) {
        func_002409F8(D_003313D4);
    } else {
        func_00240D60(D_003313D4);
    }
}

void func_0023AFA8(void) {
    func_0023F1D8();
}

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023AFC8);

const char *func_0023B0D0(GLenum name) {
    const char *s;

    switch (name) {
    case 0x1F00:
        s = D_0032F18C;
        break;
    case 0x1F01:
        s = D_0032F190;
        break;
    case 0x1F02:
        s = D_0032F194;
        break;
    case 0x1F03:
        s = D_0032F198;
        break;
    default:
        s = 0;
        break;
    }
    return s;
}

void glGetFloatv(GLenum pname, GLfloat *params) {
    float *m;
    int i;

    switch (pname) {
    case 0xBA7:
        m = D_00331140->m;
        break;
    case 0xBA6:
        m = D_00331144->m;
        break;
    default:
        return;
    }
    for (i = 0; i < 16; i++) {
        params[i] = m[i];
    }
    if (pname == 0xBA7) {
        params[5] = -params[5];
    }
}

void glHint(GLenum target, GLenum mode) {
    if (target == 0x80F0) {
        D_0033137C = mode;
    }
}

void glMatrixMode(GLenum mode) {
    D_0032F268 = mode;
}

#ifdef NON_MATCHING /* registradores trocados */
void glPushMatrix(void) {
    GLMatrix *n;
    GLMatrix *m;

    switch (D_0032F268) {
    case 0x1701:
        m = D_00331140;
        n = m + 1;
        MCOPY(n, m);
        D_00331140 = n;
        break;
    case 0x1700:
        m = D_00331144;
        n = m + 1;
        MCOPY(n, m);
        D_00331144 = n;
        break;
    }
    D_0033104C = 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/gl", glPushMatrix);
#endif

void glPopMatrix(void) {
    switch (D_0032F268) {
    case 0x1701:
        D_00331140--;
        break;
    case 0x1700:
        D_00331144--;
        break;
    }
    D_0033104C = 1;
}

void glTranslatef(GLfloat x, GLfloat y, GLfloat z) {
    GLMatrix t;

    MIDENTITY(t);
    t.m[12] = x;
    t.m[13] = y;
    t.m[14] = z;
    MMUL(D_00331144, t);
    D_0033104C = 1;
}

void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
    GLMatrix t;
    GLVec v;
    float c;
    float s;
    float k;

    v.v[0] = x;
    v.v[1] = y;
    v.v[2] = z;
    __asm__ volatile(
        "lqc2 $vf4, 0x0(%0)\n\t"
        "vmul.xyz $vf5, $vf4, $vf4\n\t"
        "vaddy.x $vf5, $vf5, $vf5y\n\t"
        "vaddz.x $vf5, $vf5, $vf5z\n\t"
        ".word 0x4A0503BD\n\t"  /* vsqrt Q, $vf5x */
        "vwaitq\n\t"
        "vaddq.x $vf5, $vf0, $Q\n\t"
        "vdiv $Q, $vf0w, $vf5x\n\t"
        "vsub.xyzw $vf6, $vf0, $vf0\n\t"
        "vwaitq\n\t"
        "vmulq.xyz $vf6, $vf4, $Q\n\t"
        "sqc2 $vf6, 0x0(%0)" : : "r"(&v) : "memory");
    angle *= 3.141593f / 180.0f;
    c = cosf(angle);
    s = sinf(angle);
    MIDENTITY(t);
    x = v.v[0];
    y = v.v[1];
    z = v.v[2];
    k = 1.0f - c;
    t.m[0] = x * x * k + c;
    t.m[4] = x * y * k - z * s;
    t.m[8] = x * z * k + y * s;
    t.m[1] = x * y * k + z * s;
    t.m[5] = y * y * k + c;
    t.m[9] = y * z * k - x * s;
    t.m[2] = x * z * k - y * s;
    t.m[6] = y * z * k + x * s;
    t.m[10] = z * z * k + c;
    MMUL(D_00331144, t);
    D_0033104C = 1;
}

void glLoadIdentity(void) {
    GLMatrix *m;

    switch (D_0032F268) {
    case 0x1701:
        m = D_00331140;
        break;
    case 0x1700:
        m = D_00331144;
        break;
    default:
        goto end;
    }
    __asm__ volatile(
        "vsub.xyzw $vf4, $vf0, $vf0\n\t"
        "vadd.w $vf4, $vf4, $vf0\n\t"
        "vmr32.xyzw $vf5, $vf4\n\t"
        "vmr32.xyzw $vf6, $vf5\n\t"
        "vmr32.xyzw $vf7, $vf6\n\t"
        "sqc2 $vf4, 0x30(%0)\n\t"
        "sqc2 $vf5, 0x20(%0)\n\t"
        "sqc2 $vf6, 0x10(%0)\n\t"
        "sqc2 $vf7, 0x0(%0)" : : "r"(m) : "$2", "memory");
end:
    D_0033104C = 1;
}

void glFrustum(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar) {
    GLMatrix t;

    MIDENTITY(t);
    t.m[0] = (zNear + zNear) / (right - left);
    t.m[5] = -((zNear + zNear) / (top - bottom));
    t.m[10] = -(zFar + zNear) / (zFar - zNear);
    t.m[11] = -1.0f;
    t.m[14] = zFar * (zNear * -2.0f) / (zFar - zNear);
    t.m[15] = 0;
    MMUL(D_00331140, t);
    D_0033104C = 1;
}

void glOrtho(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar) {
    GLMatrix t;

    MIDENTITY(t);
    t.m[0] = 2.0f / (right - left);
    t.m[5] = -(2.0f / (top - bottom));
    t.m[10] = -2.0f / (zFar - zNear);
    t.m[12] = -1.0f;
    t.m[13] = -1.0f;
    MMUL(D_00331140, t);
    D_0033104C = 1;
}

void glViewport(GLint x, GLint y, GLsizei width, GLsizei height) {
    GLMatrix *m;
    int w;
    int h;

    m = &D_00331080;
    MIDENTITYP(m);
    w = width / 2;
    h = height / 2;
    m->m[0] = w;
    m->m[5] = h;
    m->m[10] = -32767.0f;
    m->m[14] = 32768.0f;
    m->m[12] = 2048.0f - (D_0032EF80 / 2 - (x + w));
    m->m[13] = 2048.0f - (D_0032EF84 / 2 - (y + h));
    m->m[15] = 1.0f;
    D_0033104C = 1;
}

void glDepthRange(GLfloat zNear, GLfloat zFar) {
    float f;
    float n;

    f = (1.0f - zFar) * 65534.0f + 1.0f;
    n = (1.0f - zNear) * 65534.0f + 1.0f;
    D_0033104C = 1;
    D_00331080.m[10] = (n - f) * -0.5f;
    D_00331080.m[14] = (n + f) * 0.5f;
}

void func_0023B930(void) {
}

void func_0023B938(void) {
}

void glPolygonOffset(GLfloat factor, GLfloat units) {
    D_00331138 = units;
    D_0033104C = 1;
}

void glEnable(GLenum cap) {
    switch (cap) {
    case 0xB44:
        D_00331040 = 1;
        break;
    case 0xBC0:
        D_0032F208 = 1;
        break;
    case 0xB71:
        D_0032F21C = 1;
        D_0032F220 = D_0032F214;
        D_0032F224 = D_0032F218;
        break;
    case 0xDE1:
        D_0032F25C = 1;
        break;
    case 0xB60:
        D_0032F240 = 1;
        break;
    case 0xBE2:
        D_0032F204 = 1;
        break;
    case 0x8037:
        D_0032F23C = 1;
        break;
    case 0xC11:
        break;
    }
}

void glDisable(GLenum cap) {
    switch (cap) {
    case 0xB44:
        D_00331040 = 0;
        break;
    case 0xBC0:
        D_0032F208 = 0;
        break;
    case 0xB71:
        D_0032F21C = 0;
        D_0032F220 = 1;
        D_0032F224 = 1;
        break;
    case 0xDE1:
        D_0032F25C = 0;
        break;
    case 0xB60:
        D_0032F240 = 0;
        break;
    case 0xBE2:
        D_0032F204 = 0;
        D_00331358 = 0;
        break;
    case 0x8037:
        D_0032F23C = 0;
        glPolygonOffset(0, 0);
        break;
    case 0xC11:
        break;
    }
}

void glDepthMask(unsigned char flag) {
    int v;

    v = flag == 0;
    D_0032F214 = v;
    if (D_0032F21C) {
        D_0032F220 = v;
    }
}

void glDepthFunc(GLenum func) {
    switch (func) {
    case 0x200:
        D_0032F218 = 0;
        break;
    case 0x207:
        D_0032F218 = 1;
        break;
    case 0x204:
        D_0032F218 = 3;
        break;
    case 0x206:
        D_0032F218 = 2;
        break;
    default:
        D_0032F218 = 2;
        break;
    }
    if (D_0032F21C) {
        D_0032F224 = D_0032F218;
    }
}

void glAlphaFunc(GLenum func, GLfloat ref) {
    D_0032F210 = ref * 128.0f;
    switch (func) {
    case 0x200:
        D_0032F20C = 0;
        break;
    case 0x207:
        D_0032F20C = 1;
        break;
    case 0x201:
        D_0032F20C = 2;
        break;
    case 0x203:
        D_0032F20C = 3;
        break;
    case 0x202:
        D_0032F20C = 4;
        break;
    case 0x206:
        D_0032F20C = 5;
        break;
    case 0x204:
        D_0032F20C = 6;
        break;
    case 0x205:
        D_0032F20C = 7;
        break;
    }
}

void glBlendFunc(GLenum sfactor, GLenum dfactor) {
    D_00331358 = 0;
    if (sfactor == 0x302 && dfactor == 1) {
        D_0032F228 = 0;
        D_0032F22C = 2;
        D_0032F230 = 0;
        D_0032F234 = 1;
        D_0032F238 = 0;
    } else if (sfactor == 1 && dfactor == 1) {
        D_0032F228 = 0;
        D_0032F22C = 2;
        D_0032F230 = 2;
        D_0032F234 = 1;
        D_0032F238 = 0x80;
    } else if (sfactor == 0x302 && dfactor == 0x303) {
        D_0032F228 = 0;
        D_0032F22C = 1;
        D_0032F230 = 0;
        D_0032F234 = 1;
        D_0032F238 = 0;
    } else if (sfactor == 0x306 && dfactor == 0x300) {
        D_00331358 = 1;
        D_0032F228 = 1;
        D_0032F22C = 2;
        D_0032F230 = 0;
        D_0032F234 = 2;
        D_0032F238 = 0;
    } else if (sfactor == 0 && dfactor == 0x300) {
        D_0032F228 = 0;
        D_0032F22C = 2;
        D_0032F230 = 2;
        D_0032F234 = 1;
        D_0032F238 = 0x80;
    }
}

void glShadeModel(GLenum mode) {
    switch (mode) {
    case 0x1D00:
        D_0032F200 = 0;
        break;
    case 0x1D01:
        D_0032F200 = 1;
        break;
    }
}

void func_0023BEA8(GLenum array) {
    switch (array) {
    case 0x8074:
    case 0x8076:
    case 0x8078:
        D_0033102C = 1;
        break;
    }
}

void func_0023BEF0(GLenum array) {
    switch (array) {
    case 0x8074:
    case 0x8076:
    case 0x8078:
        D_0033102C = 0;
        break;
    }
}

void func_0023BF30(GLint size, GLenum type, GLint stride, const void *ptr) {
    D_00331030 = ptr;
}

void func_0023BF40(GLint size, GLenum type, GLint stride, const void *ptr) {
    D_00331034 = ptr;
}

void func_0023BF50(GLint size, GLenum type, GLint stride, const void *ptr) {
    D_00331038 = ptr;
}

void glBegin(GLenum mode) {
    func_0023AF68();
    D_0033103C = 0;
    switch (mode) {
    case 5:
        D_00331044 = 4;
        break;
    case 6:
    case 7:
    case 9:
        D_00331044 = 5;
        break;
    }
    if (D_00331048) {
        D_00331020 = (GLVec *)((unsigned int)D_00330180 | 0x30000000);
        D_00331024 = (GLVec *)((unsigned int)D_00330680 | 0x30000000);
        D_00331028 = (GLVec *)((unsigned int)D_00330B80 | 0x30000000);
    } else {
        D_00331020 = (GLVec *)((unsigned int)D_0032F280 | 0x30000000);
        D_00331024 = (GLVec *)((unsigned int)D_0032F780 | 0x30000000);
        D_00331028 = (GLVec *)((unsigned int)D_0032FC80 | 0x30000000);
    }
}

void glTexCoord2f(GLfloat s, GLfloat t) {
    D_0032F1E0.v[0] = s;
    D_0032F1E0.v[1] = t;
    D_0032F1E0.v[2] = 1.0f;
    D_0032F1E0.v[3] = 0;
}

void func_0023C070(const unsigned char *v) {
    sprite_color.v[0] = v[0] * (1.0f / 255.0f);
    sprite_color.v[1] = v[1] * (1.0f / 255.0f);
    sprite_color.v[2] = v[2] * (1.0f / 255.0f);
    sprite_color.v[3] = 1.0f;
}

void glColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    sprite_color.v[0] = r * (1.0f / 255.0f);
    sprite_color.v[1] = g * (1.0f / 255.0f);
    sprite_color.v[2] = b * (1.0f / 255.0f);
    sprite_color.v[3] = a * (1.0f / 255.0f);
}

void glColor4ubv(const unsigned char *v) {
    sprite_color.v[0] = v[0] * (1.0f / 255.0f);
    sprite_color.v[1] = v[1] * (1.0f / 255.0f);
    sprite_color.v[2] = v[2] * (1.0f / 255.0f);
    sprite_color.v[3] = v[3] * (1.0f / 255.0f);
}

void glColor3f(GLfloat r, GLfloat g, GLfloat b) {
    sprite_color.v[0] = r;
    sprite_color.v[1] = g;
    sprite_color.v[2] = b;
    sprite_color.v[3] = 1.0f;
}

void glColor4f(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    sprite_color.v[0] = r;
    sprite_color.v[1] = g;
    sprite_color.v[2] = b;
    sprite_color.v[3] = a;
}

void func_0023C218(GLfloat x, GLfloat y, GLfloat z) {
    GLVec *c;
    GLVec *t;
    GLVec *v;

    c = D_00331020;
    QCOPY(c, &sprite_color);
    t = D_00331024;
    QCOPY(t, &D_0032F1E0);
    v = D_00331028;
    v->v[0] = x;
    v->v[1] = y;
    v->v[2] = z;
    v->v[3] = 1.0f;
    D_00331020 = c + 1;
    D_00331024 = t + 1;
    D_00331028 = v + 1;
    D_0033103C++;
}

void glVertex3fv(const GLfloat *p) {
    GLVec *c;
    GLVec *t;
    GLVec *v;

    c = D_00331020;
    QCOPY(c, &sprite_color);
    t = D_00331024;
    QCOPY(t, &D_0032F1E0);
    v = D_00331028;
    v->v[0] = p[0];
    v->v[1] = p[1];
    v->v[2] = p[2];
    v->v[3] = 1.0f;
    D_00331020 = c + 1;
    D_00331024 = t + 1;
    D_00331028 = v + 1;
    D_0033103C++;
}

void glVertex2f(GLfloat x, GLfloat y) {
    GLVec *c;
    GLVec *t;
    GLVec *v;

    c = D_00331020;
    QCOPY(c, &sprite_color);
    t = D_00331024;
    QCOPY(t, &D_0032F1E0);
    v = D_00331028;
    v->v[0] = x;
    v->v[1] = y;
    v->v[2] = 0;
    v->v[3] = 1.0f;
    D_00331020 = c + 1;
    D_00331024 = t + 1;
    D_00331028 = v + 1;
    D_0033103C++;
}

void func_0023C3A0(GLenum pname, GLfloat param) {
    switch (pname) {
    case 0xB63:
        D_0032F254 = param;
        break;
    case 0xB64:
        D_0032F258 = param;
        break;
    case 0xB65:
        break;
    }
}

void func_0023C3D8(unsigned char r, unsigned char g, unsigned char b) {
    D_0032F244 = r | (g << 8) | (b << 16);
}

void glEnd(void) {
    if (D_0033103C >= 3) {
        func_0023D430(0);
    }
}

void func_0023C430(GLenum target, GLuint texture) {
    D_003313D4 = &D_003313D8[texture];
}

void func_0023C450(GLenum target, GLenum internalformat, GLint width, GLenum format, GLenum type, const void *table) {
    if (format == 0x8801) {
        func_002407D8(D_003313D4, table);
    }
}

void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels) {
    if (level == 0) {
        func_002405A8(D_003313D4, width, height);
        switch (format) {
        case 0x8801:
            func_002406C8(D_003313D4, pixels);
            break;
        case 0x1908:
            func_00240658(D_003313D4, pixels);
            break;
        case 0x1900:
            func_00240608(D_003313D4, pixels);
            break;
        }
    } else if (format == 0x1900) {
        func_00240738(D_003313D4, level - 1, width, height, pixels);
    }
}

void glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint width, GLint height, GLenum format, GLenum type, const void *pixels) {
    func_002407A8(D_003313D4, xoffset, yoffset, width, height, pixels);
}

void glTexEnvf(GLenum target, GLenum pname, GLfloat param) {
    switch ((int)param) {
    case 0x2100:
        D_0032F260 = 0;
        break;
    case 0x1E01:
        D_0032F260 = 1;
        break;
    case 0x1906:
        *(volatile int *)&D_0032F260 = 2;
        break;
    case 0xBE2:
    case 0x2101:
        break;
    }
}

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023C600);

void func_0023C7B0(void) {
    ((volatile GLTexture *)D_003313D4)->b = 1;
}

void func_0023C7C8(void) {
    func_0023F230();
}

void func_0023C7E8(void) {
    func_0023F1E0();
}

void func_0023C808(GLfloat f) {
    D_0032F26C = f;
}

void func_0023C818(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    D_00331340 = r * 255.0f;
    D_00331342 = g * 255.0f;
    D_00331344 = b * 255.0f;
    D_00331346 = a * 128.0f;
}

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023C880);

unsigned int func_0023CA00(void) {
    if (D_0032F180 == 1) {
        D_0032F180 = 0;
        return 0x70001000;
    }
    D_0032F180 = 1;
    return 0x70000000;
}

void func_0023CA28(int a, int b, int c, ...) {
}

void func_0023CA58(void) {
    D_0032EF88 = 0x80;
    D_0032EF8C = 0x80;
    D_0032EF90 = 0x80;
    D_0032EF94 = 0;
    D_0032EF98 = 0x100;
    D_0032EF9C = 0x100;
    D_0032EFA0 = 0x80;
    D_0032EFA4 = 0x100;
    D_0032EFA8 = 0x100;
    D_0032EFAC = 0x100;
    D_0032EFB0 = 0;
    D_0032EFB4 = 0;
    D_0032EFB8 = 0x200;
    D_0032EFBC = 0x100;
    D_0032EFC0 = 0;
    D_0032EFC4 = 0x100;
    D_0032EFC8 = 0x200;
    D_0032EFCC = 0x100;
    D_0032EFD0 = 0;
    D_0032EFD4 = 0x80;
    D_0032EFD8 = 0x100;
    D_0032EFDC = 0x100;
    D_0032EFE0 = 0x100;
    D_0032EFE4 = 0x80;
    D_0032EFE8 = 0x100;
    D_0032EFEC = 0x100;
    D_0032EFF0 = 0;
    D_0032EFF4 = 0;
    D_0032EFF8 = 0x100;
    D_0032EFFC = 0x200;
    D_0032F000 = 0x100;
    D_0032F004 = 0;
    D_0032F008 = 0x100;
    D_0032F00C = 0x200;
}

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023CBC8);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023CD48);

void func_0023CE68(void) {
    func_002F00E8(2);
    func_002EF360(2, D_003A9C94);
}

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023CE98);

void func_0023D068(void) {
    if (D_003337EC != 0) {
        D_00331360 = 1;
        D_00331364 = 1;
        D_00331368 = D_00333804 != 0;
    } else {
        D_00331350 = D_00331350 == 0;
        func_0023A708(D_003A9C90, D_00331350 ^ 1, 1);
        if (D_00333804 != 0) {
            D_00331350 = D_00331350 == 0;
        }
        func_0023A858(D_003A9C90, D_00331350, 1);
        D_0032F1F4 = D_00331350 ? D_0032F1F2 : D_0032F1F0;
    }
}

int func_0023D160(void) {
    if (D_00331364 == 1) {
        D_00331364 = 0;
        D_00331350 = D_00331350 == 0;
        func_0023A708(D_003A9C90, D_00331350 ^ 1, 1);
        if (D_00331368) {
            D_00331350 = D_00331350 == 0;
        }
        func_0023A858(D_003A9C90, D_00331350, 1);
        D_0032F1F4 = D_00331350 ? D_0032F1F2 : D_0032F1F0;
        func_002EF690(D_0033135C);
    }
    __asm__ volatile("sync.l\n\tei");
    return 0;
}

void func_0023D228(void) {
    D_0033137C = 0x1102;
    D_00331380 = 1;
}

void func_0023D248(void) {
    D_00331380 = 0;
}

void func_0023D258(void) {
    if (D_003337EC != 0 && D_00331360 == 1) {
        D_00331360 = 0;
        func_002EF6A0(D_0033135C);
    }
    D_00331380 = 1;
}

#ifdef NON_MATCHING /* o alvo carrega D_003313D4 depois da conversão */
void func_0023D2B8(void) {
    float d;
    float f;
    int i;

    if (D_00333864 != 0 || D_0032F26C == 0) {
        d = D_00331384;
    } else {
        if (1.0f < D_0032F26C) {
            D_0032F26C = 1.0f;
        }
        d = D_00331384 / D_0032F26C;
    }
    f = -(func_002D8268(d) * 1.442695f);
    if (f < 0) {
        f += 256.0f;
    }
    i = f * 16.0f;
    D_003A9C9C = i;
    D_003A9C98 = 0;
    D_003313D4->x38 = i;
    D_003313D4->x34 = 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023D2B8);
#endif

qword *func_0023D3B0(qword *p) {
    qword *start;
    int n;

    start = p;
    p = func_002407F0(D_003313D4, start + 1);
    n = p - start - 1;
    if (n >= 2) {
        *(unsigned long *)start = n | 0x10000000 | 0x8000000000000000;
        ((int *)start)[3] = n | 0x51000000;
        ((int *)start)[2] = 0;
    } else {
        p = start;
    }
    return p;
}

void func_0023D430(int mode) {
    qword *p;
    qword *q;

    if (D_00331380 == 1) {
        if (D_003313D4->x30) {
            func_0023D2B8();
        }
        p = func_0023D748();
        q = p;
        if (D_0033104C) {
            q = func_0023ECD8(p);
        }
        q = func_0023D3B0(q);
        func_0023ECF8(q, mode);
        func_0023AA18();
        __asm__ volatile("sync.l\n\tnop");
        *(volatile int *)0x1000E010 = 2;
        *(volatile int *)0x10009020 = 0;
        *(volatile int *)0x10009010 = 0;
        *(volatile int *)0x10009030 = (int)p | 0x80000000;
        *(volatile int *)0x10009000 = 0x145;
        __asm__ volatile("sync.l\n\tnop");
    }
}

void func_0023D520(void) {
    if (D_00331374 == 0 && D_00331380 == 1) {
        func_0023E9E8(D_003A9CA0 - 1);
        func_0023AA18();
        __asm__ volatile("sync.l\n\tnop");
        *(volatile int *)0x1000E010 = 2;
        *(volatile int *)0x10009020 = 0;
        *(volatile int *)0x10009010 = 0;
        *(volatile int *)0x10009030 = (int)D_003A9CA4 | 0x80000000;
        *(volatile int *)0x10009000 = 0x145;
        __asm__ volatile("sync.l\n\tnop");
    }
    D_00331374 = 1;
    D_00331378 = 0;
}

int func_0023D5F0(void) {
    int n;

    n = D_0033103C * 3 + 1;
    return n + D_00331378 < 0xDF;
}

void func_0023D618(int mode) {
    int n;
    qword *p;

    n = D_0033103C * 3 + 1;
    if (n + D_00331378 >= 0xDF) {
        func_0023D520();
    }
    if (D_00331374) {
        D_00331390 = 0;
        D_00331394 = 0xDE;
        D_00331374 = 0;
        if (D_003313D4->x30) {
            func_0023D2B8();
        }
        p = func_0023D748();
        D_003A9CA0 = p;
        D_003A9CA4 = p;
        if (D_0033104C) {
            D_003A9CA0 = func_0023ECD8(p);
        }
        D_003A9CA0 = func_0023D3B0(D_003A9CA0);
    }
    D_003A9CA0 = func_0023EE68(D_003A9CA0, mode);
    D_00331394 += n;
    D_00331390 += n - 1;
    D_00331378 += n;
}

qword *func_0023D748(void) {
    if (D_00331370 == 1) {
        D_00331370 = 0;
        return (qword *)0x70003000;
    }
    D_00331370 = 1;
    return (qword *)0x70002000;
}

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023D778);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023D8B8);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023DA90);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023DCE0);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023DF68);

int *func_0023E150(int *p) {
    float end;
    float start;

    end = D_0032F258;
    start = D_0032F254;
    *(unsigned long *)p = 0x8000000010000001;
    p[2] = 0x1000101;
    p[3] = 0;
    p += 4;
    p[0] = 0x640180DF;
    p[3] = 0;
    ((float *)p)[2] = end * start * 255.0f / (end - start);
    ((float *)p)[1] = ((end + start) * -255.0f / (end - start) + 255.0f) * 0.5f;
    return p + 4;
}

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023E1E8);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023E468);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023E608);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023E830);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023E9E8);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023EB88);

qword *func_0023ECD8(qword *p) {
    D_0033104C = 0;
    return func_0023D778(p);
}

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023ECF8);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023EE68);

unsigned int func_0023EFA0(void) {
    if (D_0033138C == 1) {
        D_0033138C = 0;
        return 0x70003000;
    }
    D_0033138C = 1;
    return 0x70002000;
}
