/* Gbx: laço principal e pilha de estados do jogo */

#include "common.h"

void func_0021BC78(void);
void func_0021BD00(int a, int b);
void func_0021BE60(int state);
unsigned int func_0021C6C8(void);
void func_0022A280(void);
void func_0022A2E0(int a);
unsigned int func_0022A4B0(void);
void func_00231338(int a);
void func_00231680(void);
unsigned int func_00231800(void);
void func_00236990(int a);
void func_002369C0(int a);
int func_0010F390(void *p);
void func_0010F310(void *p, int key);
int memcmp(void);
int func_002E4480(void);
int func_002256E0(int key);
void func_002256F0(int state);

extern int D_0032808C;          /* jogo iniciado */
extern unsigned int D_00328090; /* contador de quadros */
extern int D_003A63B0[10];      /* pilha de estados */
extern int D_003A63D8;          /* topo da pilha de estados */
extern void *D_003A63E0[10];    /* pilha de quem recebe as teclas */
extern int D_003A6408;          /* topo da pilha de teclas */
extern int D_00334FB8;
extern int D_00334FC0;
extern int D_00334FC4;

void func_002253C8(void) {
    D_003A63D8 = 0;
    D_003A63B0[0] = 0;
    D_003A6408 = 0;
    D_003A63E0[0] = 0;
    func_0021BC78();
    func_0021BD00(0, 0);
    func_0021BD00(1, 0);
    func_0022A280();
    D_0032808C = 1;
}

void func_00225430(void) {
}

void func_00225438(int state) {
    D_003A63B0[D_003A63D8] = state;
}

void func_00225458(void) {
    int i;

    i = D_003A63D8;
    D_003A63D8 = i + 1;
    D_003A63B0[i + 1] = D_003A63B0[i];
}

void func_00225490(void) {
    D_003A63D8--;
}

void func_002254A8(void) {
    int tick;

    func_0021BE60(D_003A63B0[D_003A63D8]);
    tick = D_00328090++ % 10 == 0;
    func_0022A2E0(tick);
    func_00231338(tick);
    func_00231680();
    func_002256F0(D_003A63B0[D_003A63D8]);
}

unsigned int func_00225548(void) {
    unsigned int a;
    unsigned int b;
    unsigned int c;

    a = func_0021C6C8();
    b = func_0022A4B0();
    c = func_00231800();
    if (c < b) {
        c = b;
    }
    if (c < a) {
        c = a;
    }
    return c;
}

void func_00225590(void *p) {
    D_003A63E0[D_003A6408] = p;
}

void func_002255B0(void) {
    int i;

    i = D_003A6408;
    D_003A6408 = i + 1;
    D_003A63E0[i + 1] = D_003A63E0[i];
}

void func_002255E8(void) {
    int i;

    i = D_003A6408;
    D_003A6408 = i - 1;
    D_003A63E0[i] = 0;
}

void func_00225610(int key) {
    int ok;
    void *p;

    switch (D_003A63B0[D_003A63D8]) {
    case 2:
        ok = func_002256E0(key) == 0;
        break;
    case 3:
        ok = func_002256E0(key) == 1;
        break;
    default:
        ok = 1;
        break;
    }
    if (ok == 1) {
        p = D_003A63E0[D_003A6408];
        if (p != 0 && func_0010F390(p) == 0) {
            func_0010F310(p, key);
        }
    }
}

int func_002256E0(int key) {
    return key >= 0x136 && key < 0x150;
}

void func_002256F0(int state) {
    if (state == 4 || state == 2 || state == 3) {
        if (D_00334FB8 == 0 && D_00334FC0 == 0 && D_00334FC4 == 0) {
            func_00236990(0);
        }
    }
    if (D_00334FB8 == 1 || D_00334FC0 == 1 || D_00334FC4 == 1) {
        func_002369C0(0);
    }
}

int func_00225790(void) {
    return memcmp();
}

int func_002257B0(void) {
    return func_002E4480();
}
