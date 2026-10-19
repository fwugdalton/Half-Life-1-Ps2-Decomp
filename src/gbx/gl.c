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
extern float D_0032F1E0[4];     /* coordenada de textura atual */
extern float sprite_color[4] __attribute__((aligned(16)));


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

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023AA00);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023AA18);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023AB00);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023AB28);

void func_0023AD90(void) {
}

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023AD98);

INCLUDE_RODATA("asm/nonmatchings/gbx/gl", D_00388940);

INCLUDE_RODATA("asm/nonmatchings/gbx/gl", D_00388958);

INCLUDE_RODATA("asm/nonmatchings/gbx/gl", D_00388970);

INCLUDE_RODATA("asm/nonmatchings/gbx/gl", D_00388978);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023ADB8);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023AF68);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023AFA8);

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

#ifdef NON_MATCHING /* o alvo intercala os lq/sq da cópia */
void glPushMatrix(void) {
    GLMatrix *m;
    GLMatrix *n;

    switch (D_0032F268) {
    case 0x1701:
        m = D_00331140;
        n = m + 1;
        ((qword *)n)[0] = ((qword *)m)[0];
        ((qword *)n)[1] = ((qword *)m)[1];
        ((qword *)n)[2] = ((qword *)m)[2];
        ((qword *)n)[3] = ((qword *)m)[3];
        D_00331140 = n;
        break;
    case 0x1700:
        m = D_00331144;
        n = m + 1;
        ((qword *)n)[0] = ((qword *)m)[0];
        ((qword *)n)[1] = ((qword *)m)[1];
        ((qword *)n)[2] = ((qword *)m)[2];
        ((qword *)n)[3] = ((qword *)m)[3];
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
    D_0032F1E0[0] = s;
    D_0032F1E0[1] = t;
    D_0032F1E0[2] = 1.0f;
    D_0032F1E0[3] = 0;
}

void func_0023C070(const unsigned char *v) {
    sprite_color[0] = v[0] * (1.0f / 255.0f);
    sprite_color[1] = v[1] * (1.0f / 255.0f);
    sprite_color[2] = v[2] * (1.0f / 255.0f);
    sprite_color[3] = 1.0f;
}

INCLUDE_ASM("asm/nonmatchings/gbx/gl", glColor4ub);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", glColor4ubv);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", glColor3f);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", glColor4f);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023C218);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", glVertex3fv);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", glVertex2f);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023C3A0);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023C3D8);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", glEnd);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023C430);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023C450);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", glTexImage2D);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", glTexSubImage2D);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", glTexEnvf);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023C600);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023C7B0);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023C7C8);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023C7E8);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023C808);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023C818);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023C880);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023CA00);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023CA28);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023CA58);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023CBC8);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023CD48);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023CE68);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023CE98);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023D068);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023D160);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023D228);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023D248);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023D258);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023D2B8);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023D3B0);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023D430);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023D520);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023D5F0);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023D618);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023D748);

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

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023ECD8);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023ECF8);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023EE68);

INCLUDE_ASM("asm/nonmatchings/gbx/gl", func_0023EFA0);
