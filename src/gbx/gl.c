/* Gbx: implementação do OpenGL do PS2 (pacotes GIF para o GS) */

#include "common.h"
#include "gbx/gl.h"

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
extern int D_0032F264;
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
extern GLMatrix D_00331100;
extern int D_0032EF80;          /* largura da tela */
extern int D_0032EF84;          /* altura da tela */
typedef int GLsizei;
float cosf(float x);
extern unsigned int D_0032F188; /* microprograma do VU1 carregado */
extern qword D_002F5980[];      /* microprogramas do VU1 */
extern qword D_002F71B0[];
extern qword D_002F9E50[];
extern qword D_002FC5A0[];
extern qword D_003001C0[];
extern float D_00333804;
extern float D_00333864;
extern float D_00331384;        /* densidade da névoa */
extern int D_003A9C98;
extern int D_003A9C9C;
extern int D_00331390;
extern int D_00331394;
float func_002D8268(float x);
qword *func_0023ECF8(qword *p, int mode);
extern GLVec *D_003A9CA8;       /* cores do desenho atual */
extern GLVec *D_003A9CAC;       /* coordenadas de textura do desenho atual */
extern GLVec *D_003A9CB0;       /* vértices do desenho atual */
extern int D_0032F250;
qword *func_0023E1E8(qword *p);
qword *func_0023D8B8(qword *p);
qword *func_0023DA90(qword *p);
qword *func_0023DCE0(qword *p);
qword *func_0023E468(qword *p);
qword *func_0023E608(qword *p);
qword *func_0023E830(qword *p);
int *func_0023E150(int *p);
qword *func_0023EE68(qword *p, int mode);
void func_0023D2B8(void);
void func_0023D520(void);
qword *func_0023D748(void);
qword *func_0023ECD8(qword *p);
qword *func_0023D3B0(qword *p);
extern int D_00331364;
extern int D_00331350;          /* buffer de tela atual */
extern int D_00331368;
extern GLScreen *D_003A9C90;   /* tela */
extern unsigned short D_0032F1F0;
extern unsigned short D_0032F1F2;
extern unsigned short D_0032F1F4;
extern int D_00331374;
qword *func_0023E9E8(qword *p);
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

void func_0023AFC8(unsigned int mode) {
    qword *p;

    p = 0;
    if (D_0032F188 != mode) {
        D_0033104C = 1;
        D_0032F188 = mode;
        switch (mode) {
        case 0:
            p = D_002F5980;
            break;
        case 1:
            p = D_002F71B0;
            break;
        case 2:
            p = D_002F9E50;
            break;
        case 3:
            p = D_002FC5A0;
            break;
        case 4:
            p = D_003001C0;
            break;
        }
        func_0023AA18();
        __asm__ volatile("sync.l\n\tnop");
        *(volatile int *)0x1000E010 = 2;
        *(volatile int *)0x10009020 = 0;
        *(volatile int *)0x10009010 = 0;
        *(volatile int *)0x10009030 = (int)p & 0x0FFFFFFF;
        *(volatile int *)0x10009000 = 0x145;
        __asm__ volatile("sync.l\n\tnop");
    }
}

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

#ifdef NON_MATCHING /* o alvo intercala os lq/sq da cópia */
void glPushMatrix(void) {
    GLMatrix *n;
    GLMatrix *m;

    switch (D_0032F268) {
    case 0x1701:
        m = D_00331140;
        n = m + 1;
        *n = *m;
        D_00331140 = n;
        break;
    case 0x1700:
        m = D_00331144;
        n = m + 1;
        *n = *m;
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

#ifdef NON_MATCHING /* agendamento e registradores das cópias */
qword *func_0023D778(qword *p) {
    *(unsigned long *)p = 0x8000000010000014;
    ((int *)p)[3] = 0x6C0480F1;
    ((int *)p)[2] = 0;
    p++;
    MCOPYT(p, D_00331140);
    p += 4;
    *(unsigned long *)p = 0;
    ((int *)p)[3] = 0x6C0480F5;
    ((int *)p)[2] = 0;
    p++;
    MCOPYT(p, D_00331144);
    p += 4;
    *(unsigned long *)p = 0;
    ((int *)p)[3] = 0x6C0480F9;
    ((int *)p)[2] = 0;
    p++;
    MCOPYT(p, &D_00331100);
    p += 4;
    *(unsigned long *)p = 0;
    ((int *)p)[3] = 0x6C0480FD;
    ((int *)p)[2] = 0;
    p++;
    MCOPYT(p, &D_00331080);
    p += 4;
    ((int *)p)[0] = 0x14000000;
    ((int *)p)[1] = 0;
    ((int *)p)[2] = 0;
    ((int *)p)[3] = 0;
    return p + 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023D778);
#endif

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

#ifdef NON_MATCHING /* a ordem das cargas e dos stores difere */
qword *func_0023E468(qword *p) {
    int prim;
    int n;
    int addr;
    int flag;
    unsigned long tag;

    prim = (D_0032F264 << 9) | (D_0032F204 << 6) | (D_0032F240 << 5) | (D_0032F25C << 4) | (D_0032F200 << 3);
    n = D_0033103C;
    addr = D_00331390;
    flag = (n << 16) | 0x8000;
    tag = n | 0x30000000;
    *(unsigned long *)p = tag | ((unsigned long)(unsigned int)D_003A9CAC << 32);
    ((int *)p)[2] = 0x1000103;
    ((int *)p)[3] = addr | flag | 0x6C000000;
    p++;
    *(unsigned long *)p = tag | ((unsigned long)(unsigned int)D_003A9CA8 << 32);
    ((int *)p)[2] = 0;
    ((int *)p)[3] = (addr + 1) | flag | 0x6C000000;
    p++;
    *(unsigned long *)p = tag | ((unsigned long)(unsigned int)D_003A9CB0 << 32);
    ((int *)p)[2] = 0;
    ((int *)p)[3] = (addr + 2) | flag | 0x6C000000;
    p++;
    *(unsigned long *)p = 0x8000000010000001;
    ((int *)p)[2] = 0x1000101;
    ((int *)p)[3] = D_00331394 | 0x6C018000;
    p++;
    *(unsigned long *)p = (unsigned long)D_0033103C | ((unsigned long)1 << 46) | ((unsigned long)(prim | D_00331044) << 47) | ((unsigned long)3 << 60);
    ((int *)p)[2] = 0x412;
    ((int *)p)[3] = D_00331044;
    return p + 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023E468);
#endif

#ifdef NON_MATCHING /* agendamento e registradores */
qword *func_0023E608(qword *p) {
    int prim;
    int n;
    int t;
    int addr;

    n = D_0033103C;
    t = n * 3;
    addr = D_00331390;
    prim = (D_0032F264 << 9) | (D_0032F204 << 6) | (D_0032F240 << 5) | (D_0032F25C << 4) | (D_0032F200 << 3);
    *(unsigned long *)p = 0x8000000010000000;
    ((int *)p)[2] = 0x20000000;
    ((int *)p)[3] = 0xFFFF5550;
    p++;
    *(unsigned long *)p = (n / 2 + (n & 1) | 0x30000000) | ((unsigned long)(unsigned int)D_003A9CAC << 32);
    ((int *)p)[2] = 0x1000301;
    ((int *)p)[3] = addr | (t << 16) | 0x8000 | 0x74000000;
    p++;
    *(unsigned long *)p = 0x8000000010000000;
    ((int *)p)[2] = 0x20000000;
    ((int *)p)[3] = 0xFFFF5540;
    p++;
    *(unsigned long *)p = (t / 4 + ((t & 3) != 0) | 0x30000000) | ((unsigned long)(unsigned int)D_003A9CB0 << 32);
    ((int *)p)[2] = 0x1000103;
    ((int *)p)[3] = (addr + 2) | (n << 16) | 0x8000 | 0x78000000;
    p++;
    *(unsigned long *)p = 0x8000000010000001;
    ((int *)p)[2] = 0x1000101;
    ((int *)p)[3] = D_00331394 | 0x6C018000;
    p++;
    *(unsigned long *)p = (unsigned long)D_0033103C | ((unsigned long)1 << 46) | ((unsigned long)(prim | D_00331044) << 47) | ((unsigned long)3 << 60);
    ((int *)p)[2] = 0x412;
    ((int *)p)[3] = D_00331044;
    return p + 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023E608);
#endif

#ifdef NON_MATCHING /* agendamento e registradores */
qword *func_0023E830(qword *p) {
    int prim;
    int n;
    int addr;
    unsigned long tag;

    prim = (D_0032F264 << 9) | (D_0032F204 << 6) | (D_0032F240 << 5) | (D_0032F25C << 4) | (D_0032F200 << 3);
    n = D_0033103C;
    addr = D_00331390;
    tag = n | 0x30000000;
    *(unsigned long *)p = 0x8000000010000000;
    ((int *)p)[3] = 0xFFFF5550;
    ((int *)p)[2] = 0x20000000;
    p++;
    *(unsigned long *)p = tag | ((unsigned long)(unsigned int)D_003A9CAC << 32);
    ((int *)p)[2] = 0x1000301;
    ((int *)p)[3] = addr | ((n * 3) << 16) | 0x8000 | 0x7C000000;
    p++;
    *(unsigned long *)p = tag | ((unsigned long)(unsigned int)D_003A9CB0 << 32);
    ((int *)p)[2] = 0x1000103;
    ((int *)p)[3] = (addr + 2) | (n << 16) | 0x8000 | 0x6C000000;
    p++;
    *(unsigned long *)p = 0x8000000010000001;
    ((int *)p)[3] = D_00331394 | 0x6C018000;
    ((int *)p)[2] = 0x1000101;
    p++;
    *(unsigned long *)p = (unsigned long)D_0033103C | ((unsigned long)1 << 46) | ((unsigned long)(prim | D_00331044) << 47) | ((unsigned long)3 << 60);
    ((int *)p)[2] = 0x412;
    ((int *)p)[3] = D_00331044;
    return p + 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023E830);
#endif

#ifdef NON_MATCHING /* a ordem do ld e das constantes da tag difere */
qword *func_0023E9E8(qword *p) {
    int prim;
    int n;
    int qwc;
    float end;
    float start;

    prim = (D_0032F264 << 9) | (D_0032F204 << 6) | (D_0032F240 << 5) | (D_0032F25C << 4) | (D_0032F200 << 3) | 5;
    n = D_0033103C;
    *(unsigned long *)p |= 0x8000;
    p++;
    qwc = D_0032F240 ? 4 : 3;
    *(unsigned long *)p = (unsigned long)qwc | ((unsigned long)7 << 28) | ((unsigned long)0x80000000 << 32);
    ((int *)p)[3] = 0x6C0181BD;
    ((int *)p)[2] = 0x1000101;
    p++;
    ((unsigned long *)p)[1] = 0x412;
    ((unsigned long *)p)[0] = (unsigned long)n | ((unsigned long)1 << 15) | ((unsigned long)1 << 46) | ((unsigned long)prim << 47) | ((unsigned long)0 << 58) | ((unsigned long)3 << 60);
    p++;
    ((int *)p)[0] = 0x1000101;
    ((int *)p)[1] = 0x600181FF;
    ((int *)p)[2] = D_00331040 ? 0x30 : 0x20;
    ((int *)p)[3] = 0;
    p++;
    if (D_0032F240) {
        end = D_0032F258;
        start = D_0032F254;
        ((int *)p)[0] = 0x640180DF;
        ((int *)p)[3] = 0;
        ((float *)p)[2] = end * start * 255.0f / (end - start);
        ((float *)p)[1] = ((end + start) * -255.0f / (end - start) + 255.0f) * 0.5f;
        p++;
    }
    ((int *)p)[0] = 0x17000000;
    ((int *)p)[3] = 0x11000000;
    ((int *)p)[1] = 0;
    ((int *)p)[2] = 0;
    return p + 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023E9E8);
#endif

void func_0023EB88(void) {
    qword *p;
    qword *q;
    float one;
    float k;

    q = func_0023D748();
    *(unsigned long *)q = 0x8000000070000007;
    ((int *)q)[3] = 0x6C0101FE;
    p = q;
    ((int *)q)[2] = 0;
    q++;
    ((unsigned long *)q)[0] = 0x3002C00000008000;
    ((unsigned long *)q)[1] = 0x412;
    q++;
    ((int *)q)[3] = 0x6C0103FE;
    ((int *)q)[0] = 0;
    ((int *)q)[1] = 0;
    ((int *)q)[2] = 0;
    q++;
    ((unsigned long *)q)[0] = 0x3002C00000008000;
    ((unsigned long *)q)[1] = 0x412;
    q++;
    *(unsigned long *)q = 0;
    ((int *)q)[3] = 0x30000000;
    ((int *)q)[2] = 0;
    q++;
    one = 1.0f;
    ((float *)q)[0] = one;
    ((float *)q)[1] = one;
    ((float *)q)[2] = one;
    ((float *)q)[3] = one;
    q++;
    *(unsigned long *)q = 0;
    ((int *)q)[3] = 0x31000000;
    ((int *)q)[2] = 0;
    q++;
    k = 1.3333330154418945f;
    ((float *)q)[0] = k;
    ((float *)q)[1] = k;
    ((float *)q)[2] = k;
    ((float *)q)[3] = k;
    __asm__ volatile("sync.l\n\tnop");
    *(volatile int *)0x1000E010 = 2;
    *(volatile int *)0x10009020 = 0;
    *(volatile int *)0x10009010 = 0;
    *(volatile int *)0x10009030 = (int)p | 0x80000000;
    *(volatile int *)0x10009000 = 0x145;
    __asm__ volatile("sync.l\n\tnop");
}

qword *func_0023ECD8(qword *p) {
    D_0033104C = 0;
    return func_0023D778(p);
}

qword *func_0023ECF8(qword *p, int mode) {
    qword *r;

    r = 0;
    if (D_0033102C) {
        D_003A9CB0 = (GLVec *)D_00331030;
        D_003A9CA8 = (GLVec *)D_00331034;
        D_003A9CAC = (GLVec *)D_00331038;
    } else {
        if (D_00331048) {
            D_003A9CA8 = D_00330180;
            D_003A9CAC = D_00330680;
            D_003A9CB0 = D_00330B80;
        } else {
            D_003A9CA8 = D_0032F280;
            D_003A9CAC = D_0032F780;
            D_003A9CB0 = D_0032FC80;
        }
        D_00331048 = D_00331048 == 0;
    }
    if (D_003313D4->b) {
        r = func_0023E1E8(p);
        D_0032F250 = 0;
    } else {
        if (D_0032F240) {
            p = (qword *)func_0023E150((int *)p);
        }
        switch ((unsigned int)mode) {
        case 0:
            r = func_0023D8B8(p);
            break;
        case 1:
            r = func_0023DA90(p);
            break;
        case 2:
            r = func_0023DCE0(p);
            break;
        }
    }
    return r;
}

qword *func_0023EE68(qword *p, int mode) {
    qword *r;

    if (D_0033102C) {
        D_003A9CB0 = (GLVec *)D_00331030;
        D_003A9CA8 = (GLVec *)D_00331034;
        D_003A9CAC = (GLVec *)D_00331038;
    } else {
        if (D_00331048) {
            D_003A9CA8 = D_00330180;
            D_003A9CAC = D_00330680;
            D_003A9CB0 = D_00330B80;
        } else {
            D_003A9CA8 = D_0032F280;
            D_003A9CAC = D_0032F780;
            D_003A9CB0 = D_0032FC80;
        }
        D_00331048 = D_00331048 == 0;
    }
    r = func_0023E468(p);
    switch ((unsigned int)mode) {
    case 0:
        r = func_0023E468(p);
        break;
    case 1:
        r = func_0023E608(p);
        break;
    case 3:
        r = func_0023E830(p);
        break;
    }
    return r;
}

unsigned int func_0023EFA0(void) {
    if (D_0033138C == 1) {
        D_0033138C = 0;
        return 0x70003000;
    }
    D_0033138C = 1;
    return 0x70002000;
}
