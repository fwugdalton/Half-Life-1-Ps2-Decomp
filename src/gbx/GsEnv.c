/* Gbx: ambientes de exibição e desenho do GS (FRAME, DISPLAY, PMODE) */

#include "common.h"
#include "gbx/gl.h"

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

INCLUDE_ASM("asm/nonmatchings/gbx/GsEnv", func_00239DB8);

void func_00239ED0(unsigned long *p, short fbp, short psm, int fbw) {
    unsigned long v;

    v = (fbw << 16) | (long)fbp | ((long)psm << 24);
    p[0] = v | 0xFFFFFFFF00000000;
    p[1] = 0x4C;
    p += 14;
    p[1] = 0x4C;
    p[0] = v;
}

INCLUDE_ASM("asm/nonmatchings/gbx/GsEnv", func_00239F28);

void func_0023A150(GLScreen *s, short w, short h, short psm) {
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

void func_0023A1B0(GLScreen *s, short w, short h, short psm, short zpsm, short a5, short a6) {
    s->x340 = func_00239D88(w) - 1;
    s->x342 = h;
    s->x344 = psm;
    s->x34A = a5;
    func_0023A150(s, w, h, psm);
    func_0023A2F0(s, w, h, psm, s->x346, s->x348);
    func_0023A4E8(s, w, h, psm, s->x346, s->x348, s->x34C, zpsm, a5, a6);
}

void func_0023A2A8(GLScreen *s) {
    func_00239ED0((unsigned long *)s->pad160, s->x346, s->x344, s->x340);
    func_00239ED0((unsigned long *)s->pad2C0, s->x348, s->x344, s->x340);
}

#ifdef NON_MATCHING /* DH e DX: o alvo não estende o sinal e relê DX da memória */
void func_0023A2F0(GLScreen *s, short w, short h, short psm, short fbp0, short fbp1) {
    func_002D0BF0(&s->disp[0], psm, w, h, 0, 0);
    s->disp[0].pmode.ALP = 0x80;
    s->disp[0].display1 = s->disp[0].display2;
    s->disp[0].display2.DH--;
    s->disp[0].dispfb2.FBP = fbp0;
    s->disp[0].dispfb1 = s->disp[0].dispfb2;
    s->disp[0].display2.DX += 0;
    s->disp[0].dispfb2.DBY = 1;
    s->disp[0].pmode.EN1 = 1;
    s->disp[0].pmode.EN2 = 1;
    func_002D0BF0(&s->disp[1], psm, w, h, 0, 0);
    s->disp[1].pmode.ALP = 0x80;
    s->disp[1].display1 = s->disp[1].display2;
    s->disp[1].display2.DH--;
    s->disp[1].dispfb2.FBP = fbp1;
    s->disp[1].dispfb1 = s->disp[1].dispfb2;
    s->disp[1].display2.DX += 0;
    s->disp[1].dispfb2.DBY = 1;
    s->disp[1].pmode.EN1 = 1;
    s->disp[1].pmode.EN2 = 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/GsEnv", func_0023A2F0);
#endif

INCLUDE_ASM("asm/nonmatchings/gbx/GsEnv", func_0023A4E8);

void func_0023A708(GLScreen *s, int buf, int en1) {
    if (buf == 0) {
        if (en1) {
            s->disp[0].pmode.EN1 = 1;
            s->disp[0].pmode.EN2 = 1;
        } else {
            s->disp[0].pmode.EN1 = 0;
            s->disp[0].pmode.EN2 = 1;
        }
        *(volatile unsigned long *)0x12000000 = *(unsigned long *)&s->disp[0].pmode;
        *(volatile unsigned long *)0x12000020 = *(unsigned long *)&s->disp[0].smode2;
        *(volatile unsigned long *)0x12000090 = *(unsigned long *)&s->disp[0].dispfb2;
        *(volatile unsigned long *)0x120000A0 = *(unsigned long *)&s->disp[0].display2;
        *(volatile unsigned long *)0x120000E0 = *(unsigned long *)&s->disp[0].bgcolor;
        *(volatile unsigned long *)0x12000080 = *(unsigned long *)&s->disp[0].display1;
        *(volatile unsigned long *)0x12000070 = *(unsigned long *)&s->disp[0].dispfb1;
    } else {
        if (en1) {
            s->disp[1].pmode.EN1 = 1;
            s->disp[1].pmode.EN2 = 1;
        } else {
            s->disp[1].pmode.EN1 = 0;
            s->disp[1].pmode.EN2 = 1;
        }
        *(volatile unsigned long *)0x12000000 = *(unsigned long *)&s->disp[1].pmode;
        *(volatile unsigned long *)0x12000020 = *(unsigned long *)&s->disp[1].smode2;
        *(volatile unsigned long *)0x12000090 = *(unsigned long *)&s->disp[1].dispfb2;
        *(volatile unsigned long *)0x120000A0 = *(unsigned long *)&s->disp[1].display2;
        *(volatile unsigned long *)0x120000E0 = *(unsigned long *)&s->disp[1].bgcolor;
        *(volatile unsigned long *)0x12000080 = *(unsigned long *)&s->disp[1].display1;
        *(volatile unsigned long *)0x12000070 = *(unsigned long *)&s->disp[1].dispfb1;
    }
}

void func_0023A858(GLScreen *s, int buf, int full) {
    if (full) {
        s->tag0.NLOOP = 0x15;
        s->tag1.NLOOP = 0x15;
    } else {
        s->tag0.NLOOP = 0xD;
        s->tag1.NLOOP = 0xD;
    }
    if (buf == 0) {
        __asm__ volatile("sync.l\n\tnop");
        *(volatile int *)0x1000E010 = 4;
        *(volatile int *)0x1000A020 = s->tag0.NLOOP + 1;
        *(volatile int *)0x1000A010 = (int)&s->tag0 & 0x0FFFFFFF;
        *(volatile int *)0x1000A030 = 0;
        *(volatile int *)0x1000A000 = 0x105;
        __asm__ volatile("sync.l\n\tnop\n\t.set noreorder\n\t.set reorder");
    } else {
        __asm__ volatile("sync.l\n\tnop");
        *(volatile int *)0x1000E010 = 4;
        *(volatile int *)0x1000A020 = s->tag1.NLOOP + 1;
        *(volatile int *)0x1000A010 = (int)&s->tag1 & 0x0FFFFFFF;
        *(volatile int *)0x1000A030 = 0;
        *(volatile int *)0x1000A000 = 0x105;
        __asm__ volatile("sync.l\n\tnop\n\t.set noreorder\n\t.set reorder");
    }
}
