/* Gbx: mouse USB (até 8 mouses, botões via MouseStateManager) */

#include "common.h"
#include "gbx/KeyStateManager.h"

extern "C" {
#include "engine/cvar.h"
#include "engine/server.h"

/* um mouse USB (0xC0 bytes) */
struct UsbMouse {
    signed char report[0x80];  /* relatório lido: tamanho, botões, dx, dy, roda */
    float x;
    float y;
    int active;
    signed char wheel;
    unsigned long time;
    MouseStateManager *mgr;
    char pad[0x24];
};

/* informação do driver usbmouse */
struct UsbMouseInfo {
    unsigned char max;
    unsigned char count;
    unsigned char status[0x7E];
};

void Mem_PushTag(void);
void Mem_SetTag(int tag);
void Mem_PopTag(void);
unsigned long func_00229B88(void);
void Key_Event(int key, int down);
void func_00234D18(void);
void func_00234DA0(UsbMouseInfo *info);
int func_00234DE8(int port, UsbMouse *m);
int func_00234E30(int port, unsigned char *info);
void func_00234E78(int mode);
void func_00231848(int port, unsigned char *info, UsbMouse *m);
void func_00231898(MouseStateManager *mgr);

extern int D_00328340;
extern cvar_t D_00328348;
extern UsbMouse D_00328380[8];
extern int D_00334FC4;
extern UsbMouseInfo D_003A7740;
extern unsigned char D_003A77C0[];

#ifdef NON_MATCHING
/* só o registrador do %hi de D_00328380 (v0 em vez de v1) */
void func_002312D0(void) {
    int i;

    func_00234D18();
    for (i = 0; i < 8; i++) {
        D_00328380[i].active = 0;
        D_00328380[i].x = 0;
        D_00328380[i].y = 0;
        D_00328380[i].mgr = 0;
        D_00328380[i].time = 0;
    }
    D_00328340 = 1;
    Cvar_RegisterVariable(&D_00328348);
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Mouse", func_002312D0);
#endif

#ifdef NON_MATCHING
/* fluxo equivalente; agendamento e endereços de D_00328380[i] diferentes */
void func_00231338(int poll) {
    int i;
    int b;

    if (poll == 1) {
        func_00234DA0(&D_003A7740);
        func_00234E78(0);
    }
    for (i = 0; i < D_003A7740.max; i++) {
        if (D_003A7740.status[i] != 0) {
            D_00328380[i].active = 1;
            if (D_00328380[i].mgr == 0) {
                Mem_PushTag();
                Mem_SetTag(1);
                D_00328380[i].mgr = new MouseStateManager;
                D_00328380[i].mgr->Init(5);
                func_00231898(D_00328380[i].mgr);
                Mem_PopTag();
            }
            if (poll == 1) {
                if (func_00234E30(i, D_003A77C0) != 0) {
                    continue;
                }
                func_00234E78(0);
            }
            while (func_00234DE8(i, &D_00328380[i]) == 0) {
                func_00234E78(0);
                if (D_00328380[i].report[0] == 0) {
                    break;
                }
                D_00328380[i].time = func_00229B88();
                if (D_00328348.value != 0.0f) {
                    func_00231848(i, D_003A77C0, &D_00328380[i]);
                }
                if ((unsigned)(D_00328380[i].report[2] + 30) > 60) {
                    D_00328380[i].x += (int)(D_00328380[i].report[2] * 1.1f);
                } else {
                    D_00328380[i].x += D_00328380[i].report[2];
                }
                if ((unsigned)(D_00328380[i].report[3] + 20) > 40) {
                    D_00328380[i].y += (int)(D_00328380[i].report[3] * 1.1f);
                } else {
                    D_00328380[i].y += D_00328380[i].report[3];
                }
                for (b = 0; b < 3; b++) {
                    if ((D_00328380[i].report[1] >> b) & 1) {
                        D_00328380[i].mgr->Press(1 << b);
                    } else {
                        D_00328380[i].mgr->Release(1 << b);
                    }
                }
                if (D_00328380[i].report[0] >= 4) {
                    D_00328380[i].wheel = D_00328380[i].report[4];
                }
            }
        } else {
                if (D_00328380[i].mgr != 0) {
                    D_00328380[i].mgr->Clear();
                }
                D_00328380[i].active = 0;
                D_00328380[i].x = 0;
                D_00328380[i].y = 0;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Mouse", func_00231338);
#endif

void func_00231680(void) {
    UsbMouse *m;
    int i;
    int active;

    D_00334FC4 = 0;
    for (i = 0; i < 8; i++) {
        m = &D_00328380[i];
        active = m->active;
        if (active == 1) {
            D_00334FC4 = active;
            m->mgr->Dispatch();
            if (m->mgr->AnyDown() == active) {
                m->time = func_00229B88();
            }
            if (m->wheel > 0) {
                Key_Event(0xD0, 1);
                Key_Event(0xD0, 0);
            }
            if (m->wheel < 0) {
                Key_Event(0xCF, 1);
                Key_Event(0xCF, 0);
            }
            m->wheel = 0;
        }
    }
}

void func_00231768(unsigned char *dx, unsigned char *dy) {
    UsbMouse *m;
    int i;

    *dx = 0;
    *dy = 0;
    for (i = 0; i < 8; i++) {
        m = &D_00328380[i];
        if (m->active == 1) {
            if (sv.paused == 0) {
                *dx += (int)m->x;
                *dy += (int)m->y;
            }
            m->x = 0;
            m->y = 0;
        }
    }
}

unsigned long func_00231800(void) {
    UsbMouse *m;
    int i;
    unsigned long t;

    t = 0;
    for (i = 0; i < 8; i++) {
        m = &D_00328380[i];
        if (m->active == 1) {
            if (t < m->time) {
                t = m->time;
            }
        }
    }
    return t;
}

/* dump do relatório cru (os printf foram compilados fora) */
void func_00231848(int port, unsigned char *info, UsbMouse *m) {
    int n;
    int k;

    for (n = 0; n < 7 && info[n] != 0; n++) {
    }
    for (k = 5; k <= m->report[0]; k++) {
    }
}

void func_00231898(MouseStateManager *mgr) {
    int b;

    for (b = 0; b < 3; b++) {
        mgr->SetData(1 << b, 200 + b);
    }
}

void func_002318F8(void) {
    __asm__ volatile("break 0");
}
}
