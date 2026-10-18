/* Gbx: estado de pausa de cada jogador (tela dividida) */

#include "common.h"

extern "C" {
#include "engine/cmd.h"
#include "engine/client.h"

int func_00236AF8(int player);
void func_00236880(void);
void func_00236990(int player);

extern int D_004A96BC;
extern int D_00332F34;
extern int D_00328B18;
extern int D_003A8140[2];
extern int D_003A8148[2];       /* 1 = jogando, 2 = pediu pausa, 3 = pausado */

void func_00236858(void) {
    func_00236880();
    D_00328B18 = 1;
}

void func_00236880(void) {
    int i;

    for (i = 0; i < 2; i++) {
        D_003A8148[i] = 1;
        D_003A8140[i] = 1;
    }
}

void func_002368B8(void) {
    char buf[0x20];
    int i;

    if (D_004A96BC != 0) {
        return;
    }
    for (i = 0; i < cl_numlocal; i++) {
        if (D_003A8148[i] == 2 && func_00236AF8(i) == 1) {
            sprintf(buf, "pause %d\n", i);
            Cbuf_AddText(buf);
            D_003A8148[i] = 3;
            return;
        }
    }
}

void func_00236990(int player) {
    if (D_003A8148[player] != 3) {
        D_003A8148[player] = 2;
    }
}

void func_002369C0(int player) {
    if (D_003A8148[player] == 2) {
        D_003A8148[player] = 1;
    }
}

void func_002369F0(int player) {
    D_003A8148[player] = 1;
}

void func_00236A10(void) {
    int i;

    for (i = 0; i < 2; i++) {
        if (D_003A8148[i] == 3) {
            D_003A8148[i] = 2;
        }
    }
}

void func_00236A48(int player) {
    D_003A8140[player] = 2;
}

void func_00236A68(int player) {
    D_003A8140[player] = 1;
}

void func_00236A88(void) {
    int i;

    for (i = 0; i < 2; i++) {
        if (D_003A8140[i] == 2) {
            D_003A8140[i] = 1;
            func_00236990(i);
        }
    }
}

#ifdef NON_MATCHING
/* o original não propaga o 1 de D_00332F34 (devolve o valor lido, com movn) */
int func_00236AF8(int player) {
    int ret;

    ret = 0;
    if (D_00332F34 == 1) {
        ret = D_00332F34;
        if ((unsigned)(cls[player].state - 3) > 2) {
            ret = 0;
        }
    }
    return ret;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/PauseState", func_00236AF8);
#endif
}
