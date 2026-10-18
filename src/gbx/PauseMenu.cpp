/* Gbx: menu de pausa (GUI carregada de um pak, desenhada sobre a tela escurecida) */

#include "common.h"

extern "C" {
#include "engine/decals.h"
#include "engine/cvar.h"

typedef struct {
    int width;
    int height;
} viddef_t;

/* nome e valor padrão da cvar WhichPlayerPaused (usados pelo .data em asm) */
__asm__(".section .rodata\n"
        ".balign 8\n"
        ".globl D_00388648\n"
        "D_00388648: .asciz \"WhichPlayerPaused\"\n"
        ".balign 8\n"
        ".globl D_00388660\n"
        "D_00388660: .asciz \"0\"\n"
        ".section .text");

void *__builtin_new(int size);
void *func_0010EC68(void *gui);
void func_0010EC78(void *gui, int flags);
void func_0010EE50(void *gui, pakfile_t *pak, const char *name, int a);
void func_0010F2B8(void *gui);
void func_0010F3A0(void *gui, const char *s);
void func_00225438(int a);
void func_00225458(void);
void func_00225490(void);
void func_00225590(void *gui);
void func_002255B0(void);
void func_002255E8(void);
void func_002262D8(void);
void func_002262E0(int a);
void func_00228CF0(char *dst, const char *src);
int func_00235D28(const char *name, byte **data, int *size);
void func_00235038(pakfile_t *pak, byte *data, int size);
void Pak_Init(pakfile_t *pak);
void func_002365C0(const char *s);
void func_002365E8(const char *s);
void func_00236610(const char *s);
void glMatrixMode(int mode);
void glPushMatrix(void);
void glPopMatrix(void);
void glLoadIdentity(void);
void glOrtho(float left, float right, float bottom, float top, float zNear, float zFar);
void glViewport(int x, int y, int width, int height);
void glEnable(int cap);
void glDisable(int cap);
void glBlendFunc(int s, int d);
void glColor4f(float r, float g, float b, float a);
void glBegin(int mode);
void glEnd(void);
void glVertex2f(float x, float y);
void GL_Bind(int texnum);

extern const char D_00388648[];
extern const char D_00388660[];
extern cvar_t D_00328A28;
extern char D_00328A40[0x38];   /* pak da GUI */
extern char D_00328A78[0x38];   /* arquivo da GUI dentro do pak */
extern char D_00328AB0[0x64];   /* texto extra */
extern int D_00328B14;
extern void *D_003A8118;
extern pakfile_t D_003A8120;
extern viddef_t vid;
extern int D_0032EF68, D_0032EF6C, D_0032EF74;
extern int D_0032EF78, D_0032EF7C, D_0032EF80, D_0032EF84;
extern int D_00399644;

void func_002363A0(int player) {
    byte *data;
    int size;

    if (D_00328B14 == 1) {
        D_00328B14 = 0;
        Cvar_RegisterVariable(&D_00328A28);
    }
    func_00225458();
    if (D_003A8118 != 0) {
        return;
    }
    switch (player) {
    case 0:
        func_00225438(2);
        Cvar_Set((char *)D_00388648, "1");
        break;
    case 1:
        func_00225438(3);
        Cvar_Set((char *)D_00388648, "2");
        break;
    }
    func_00235D28(D_00328A40, &data, &size);
    Pak_Init(&D_003A8120);
    func_00235038(&D_003A8120, data, size);
    func_002262E0(0);
    D_003A8118 = func_0010EC68(__builtin_new(0x64));
    func_0010EE50(D_003A8118, &D_003A8120, D_00328A78, 0);
    if (D_00328AB0[0] != 0) {
        func_0010F3A0(D_003A8118, D_00328AB0);
    }
    func_002262E0(1);
    func_002262D8();
    func_002255B0();
    func_00225590(D_003A8118);
}

void func_00236500(void) {
    if (D_003A8118 != 0) {
        func_002255E8();
        func_00225490();
        func_002262E0(0);
        if (D_003A8118 != 0) {
            func_0010EC78(D_003A8118, 3);
        }
        D_003A8118 = 0;
        func_002262E0(1);
        func_002262D8();
    }
    Cvar_Set((char *)D_00388648, (char *)D_00388660);
}

void func_00236580(const char *pak, const char *file, const char *text) {
    func_002365C0(pak);
    func_002365E8(file);
    func_00236610(text);
}

void func_002365C0(const char *s) {
    func_00228CF0(D_00328A40, s);
}

void func_002365E8(const char *s) {
    func_00228CF0(D_00328A78, s);
}

void func_00236610(const char *s) {
    if (s != 0) {
        func_00228CF0(D_00328AB0, s);
    } else {
        func_00228CF0(D_00328AB0, "");
    }
}

void func_00236658(void) {
    if (D_003A8118 == 0) {
        return;
    }
    glViewport(D_0032EF7C, D_0032EF78, D_0032EF80, D_0032EF84);
    glMatrixMode(0x1701);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(D_0032EF68, D_0032EF68 + vid.width, D_0032EF6C + D_0032EF74, D_0032EF6C, -99999, 99999);
    glMatrixMode(0x1700);
    glPushMatrix();
    glLoadIdentity();
    glDisable(0xBC0);
    glDisable(0xB71);
    glDisable(0xDE1);
    glEnable(0xBE2);
    GL_Bind(D_00399644);
    glBlendFunc(0x302, 0x303);
    glColor4f(0, 0, 0, 0.5f);
    glBegin(9);
    glVertex2f(0, 0);
    glVertex2f(vid.width, 0);
    glVertex2f(vid.width, D_0032EF74);
    glVertex2f(0, D_0032EF74);
    glEnd();
    glDisable(0xBE2);
    glEnable(0xDE1);
    glEnable(0xB71);
    glEnable(0xBC0);
    glMatrixMode(0x1701);
    glPopMatrix();
    glMatrixMode(0x1700);
    glPopMatrix();
    func_0010F2B8(D_003A8118);
}
}
