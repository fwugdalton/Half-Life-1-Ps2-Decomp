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
    char pad[0xD4];
} GLTexture;

typedef unsigned int GLuint;

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
void func_0023D778(void);
void func_0023F1D8(void);
void func_0023F1E0(void);
void func_0023F230(void);
int func_002F00E8(int a);
void func_002EF360(int a, int b);
extern unsigned int *D_0032F160;
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

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_00239ED0);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_00239F28);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023A150);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023A1B0);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023A2A8);

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

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023B0D0);

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

INCLUDE_ASM("asm/nonmatchings/gbx/gl", glTranslatef);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", glRotatef);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", glLoadIdentity);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", glFrustum);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", glOrtho);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", glViewport);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", glDepthRange);

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

INCLUDE_ASM("asm/nonmatchings/gbx/gl", glBegin);

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

INCLUDE_ASM("asm/nonmatchings/gbx/gl", glTexImage2D);

void glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint width, GLint height, GLenum format, GLenum type, const void *pixels) {
    func_002407A8(D_003313D4, xoffset, yoffset, width, height, pixels);
}

INCLUDE_ASM("asm/nonmatchings/gbx/gl", glTexEnvf);

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

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023C818);

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

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023CA58);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023CBC8);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023CD48);

void func_0023CE68(void) {
    func_002F00E8(2);
    func_002EF360(2, D_003A9C94);
}

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023CE98);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023D068);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023D160);

void func_0023D228(void) {
    D_0033137C = 0x1102;
    D_00331380 = 1;
}

void func_0023D248(void) {
    D_00331380 = 0;
}

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023D258);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023D2B8);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023D3B0);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023D430);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023D520);

int func_0023D5F0(void) {
    int n;

    n = D_0033103C * 3 + 1;
    return n + D_00331378 < 0xDF;
}

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023D618);

unsigned int func_0023D748(void) {
    if (D_00331370 == 1) {
        D_00331370 = 0;
        return 0x70003000;
    }
    D_00331370 = 1;
    return 0x70002000;
}

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023D778);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023D8B8);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023DA90);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023DCE0);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023DF68);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023E150);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023E1E8);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023E468);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023E608);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023E830);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023E9E8);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023EB88);

void func_0023ECD8(void) {
    D_0033104C = 0;
    func_0023D778();
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
