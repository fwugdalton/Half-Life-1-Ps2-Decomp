/* Gbx: controles do PS2 (2 portas x 4 slots), botões via KeyStateManager e vibração */

#include "common.h"
#include "gbx/KeyStateManager.h"

extern "C" {
#include "engine/cvar.h"
#include "engine/client.h"

/* um controle (0x1C bytes) */
struct PadSlot {
    int status;                 /* 2 = conectado e estável */
    int handle;                 /* -1 = fechado */
    KeyStateManager keys;
    float x;                    /* analógico para olhar */
    float y;
};

/* um pedido de vibração (0x10 bytes) */
struct Actuator {
    unsigned char port;
    unsigned char motor;
    unsigned char small;
    unsigned char big;
    float duration;
    int state;                  /* 0 = novo, 1 = vibrando, 2 = pausado, 3 = livre */
    float endtime;
};

extern PadSlot D_003A6200[2][4];
extern Actuator D_003A62E0[10];

extern int D_00334FB8;
extern int D_00334FBC;
extern int D_004A96BC;
extern float D_003344EC;
extern float D_00334504;
extern float D_003332A0[];
extern float D_00333298[];
extern float D_00331744;
extern float D_0033175C;
extern float D_00331824;        /* lookspring.value */
extern float D_00331AF4;        /* cl_pitchdown.value */
extern float D_00331ADC;        /* cl_pitchup.value */
extern float D_00399148[2];
extern float D_00399158[2];
extern float host_frametime;
extern cvar_t D_0055A328[2];
extern cvar_t D_0055A3A0[2];
extern cvar_t D_0055A300[2];

typedef struct kbutton_s
{
    int down[2];
    int state;
} kbutton_t;

extern kbutton_t in_leftStickXLook[2];
extern kbutton_t in_leftStickYLook[2];
extern kbutton_t in_rightStickXLook[2];
extern kbutton_t in_rightStickYLook[2];
extern kbutton_t in_leftStickForwardMove[2];
extern kbutton_t in_leftStickSideMove[2];
extern kbutton_t in_rightStickForwardMove[2];
extern kbutton_t in_rightStickSideMove[2];

float Sys_FloatTime(void);
void V_StopPitchDrift(int clientId);
void Mem_PushTag(void);
void Mem_SetTag(int tag);
void Mem_PopTag(void);
void *memset(void *, int, unsigned int);
float fabsf(float);

void func_002178F8(void);
int func_002179D0(int port, int slot);
void func_00217B00(int i);
int func_00217B50(int i);
int func_00217CE8(int i);
unsigned long func_00218090(int i);
unsigned long func_002180B0(void);
void func_00218168(void);
float func_00218E18(int i, int update);
float func_00218E60(int i, int update);
float func_00218EA8(int i, int update);
float func_00218EF0(int i, int update);
float func_00218F38(int i, int update);
float func_00218F80(int i, int update);
float func_00218FC8(int i, int update);
float func_00219010(int i, int update);
float func_00219058(int i, int update);
float func_002190A0(int i, int update);
float func_002190E8(int i, int update);
float func_00219130(int i, int update);
float func_00219178(int i, int update);
float func_002191C0(int i, int update);
float func_00219208(int i, int update);
float func_00219250(int i, int update);
float func_00219298(int i, int update);
float func_002192E0(int i, int update);
float func_00219328(int i, int update);
float func_00219370(int i, int update);
void func_002193B8(int i, unsigned char small, unsigned char big);
void func_00236990(int player);
void func_002369C0(int player);
void func_00236A48(int player);
void func_00236A68(int player);

void func_0021C288(int clientId, unsigned char small, unsigned char big, float duration);
void func_0021C520(void);
int func_0021C780(int port, int slot, int mode);
void func_0021D048(int port, int mode);
void func_0021D0D0(int port, int mode);
void func_0021D158(int handle, int small, int big);
void func_0021D188(KeyStateManager *keys);
int func_0021D778(int key, int port);

void func_0021BC78(void)
{
    int i, j;

    for (i = 0; i < 2; i++)
        for (j = 0; j < 4; j++)
            D_003A6200[i][j].handle = -1;
    for (i = 0; i < 10; i++)
        D_003A62E0[i].state = 3;
    func_002178F8();
}

void func_0021BD00(unsigned char port, unsigned char slot)
{
    if (D_003A6200[port][slot].handle == -1) {
        D_003A6200[port][slot].handle = func_002179D0(port, slot);
        D_003A6200[port][slot].status = func_00217CE8(D_003A6200[port][slot].handle);
        func_0021D188(&D_003A6200[port][slot].keys);
        D_003A6200[port][slot].x = 0;
        D_003A6200[port][slot].y = 0;
    }
}

void func_0021BDB8(unsigned char port, unsigned char slot)
{
    if (D_003A6200[port][slot].handle != -1)
        func_00217B00(D_003A6200[port][slot].handle);
    D_003A6200[port][slot].handle = -1;
}

int func_0021BE28(unsigned char port, unsigned char slot)
{
    return D_003A6200[port][slot].handle != -1;
}

void func_0021BE60(int mode)
{
    int i, j;

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 4; j++) {
            PadSlot *p = &D_003A6200[i][j];

            if (p->handle == -1)
                continue;
            if (p->status == 2) {
                p->status = func_0021C780(i, j, mode);
                if (p->status != 2)
                    func_0021D048(i, mode);
            } else {
                p->status = func_00217CE8(p->handle);
                if (p->status == 2)
                    func_0021D0D0(i, mode);
            }
        }
    }
    if (D_003A6200[0][0].status == 2)
        D_00334FB8 = 1;
    else
        D_00334FB8 = 0;
    if (D_003A6200[1][0].status == 2)
        D_00334FBC = 1;
    else
        D_00334FBC = 0;
    func_0021C520();
}

#ifdef NON_MATCHING /* o original mantém &cl e clientId*0x212C separados antes do if */
void func_0021BFC0(int clientId)
{
    float x, y;
    client_state_t *pcl;
    float frametime;

    if (D_004A96BC)
        return;

    switch (clientId) {
    case 0:
        x = D_003A6200[0][0].x;
        y = D_003A6200[0][0].y;
        break;
    case 1:
        x = D_003A6200[1][0].x;
        y = D_003A6200[1][0].y;
        break;
    default:
        x = y = 0;
        break;
    }

    pcl = &cl[clientId];
    frametime = host_frametime;
    if (fabsf(x) > D_003344EC)
        pcl->viewangles[1] += x * x * x * (D_003332A0[clientId] * frametime * D_00331744);

    if (fabsf(y) > D_00334504) {
        if (D_0055A328[clientId].value < 0)
            pcl->viewangles[0] -= y * y * y * (D_00333298[clientId] * frametime * D_0033175C);
        else
            pcl->viewangles[0] += y * y * y * (D_00333298[clientId] * frametime * D_0033175C);
        V_StopPitchDrift(clientId);
    } else if (D_00331824 == 0) {
        V_StopPitchDrift(clientId);
    }

    if (cl[clientId].viewangles[0] > D_00331AF4)
        cl[clientId].viewangles[0] = D_00331AF4;
    if (cl[clientId].viewangles[0] < -D_00331ADC)
        cl[clientId].viewangles[0] = -D_00331ADC;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/HL1Input", func_0021BFC0);
#endif

int func_0021C1F0(unsigned long t)
{
    return t >= func_002180B0();
}

void func_0021C220(void)
{
    func_00218168();
}

/* SetActuator */
int func_0021C240(int clientId, const char *pszName, int iSize, void *pbuf)
{
    unsigned char *buf = (unsigned char *)pbuf;

    func_0021C288(clientId, buf[0], buf[1], *(unsigned short *)(buf + 2) * 0.001f);
    return 1;
}

void func_0021C288(int clientId, unsigned char small, unsigned char big, float duration)
{
    int port;
    int motor;
    int i;

    if (D_0055A3A0[clientId].value != 1.0f)
        return;

    switch (clientId) {
    case 0:
        port = 0;
        motor = 0;
        break;
    case 1:
        port = 1;
        motor = 0;
        break;
    default:
        port = 0;
        motor = 0;
        break;
    }

    for (i = 0; i < 10; i++) {
        if (D_003A62E0[i].state == 3) {
            D_003A62E0[i].port = port;
            D_003A62E0[i].motor = motor;
            D_003A62E0[i].small = small;
            D_003A62E0[i].big = big;
            D_003A62E0[i].duration = duration;
            D_003A62E0[i].state = 0;
            return;
        }
    }
}

void func_0021C340(void)
{
    int i;

    for (i = 0; i < 10; i++)
        D_003A62E0[i].state = 3;
    if (D_003A6200[0][0].handle != -1)
        func_0021D158(D_003A6200[0][0].handle, 0, 0);
    if (D_003A6200[1][0].handle != -1)
        func_0021D158(D_003A6200[1][0].handle, 0, 0);
}

void func_0021C3C0(void)
{
    int i;
    float t;

    for (i = 0; i < 10; i++) {
        switch (D_003A62E0[i].state) {
        case 0:
            D_003A62E0[i].state = 2;
            break;
        case 1:
            t = Sys_FloatTime();
            if (D_003A62E0[i].endtime <= t) {
                D_003A62E0[i].state = 3;
            } else {
                D_003A62E0[i].state = 2;
                D_003A62E0[i].duration = D_003A62E0[i].endtime - t;
            }
            break;
        case 2:
            break;
        }
    }
    if (D_003A6200[0][0].handle != -1)
        func_0021D158(D_003A6200[0][0].handle, 0, 0);
    if (D_003A6200[1][0].handle != -1)
        func_0021D158(D_003A6200[1][0].handle, 0, 0);
}

void func_0021C4B0(void)
{
    int i;

    for (i = 0; i < 10; i++) {
        if (D_003A62E0[i].state == 2) {
            float t = Sys_FloatTime();

            D_003A62E0[i].state = 0;
            D_003A62E0[i].endtime = D_003A62E0[i].duration + t;
        }
    }
}

#ifdef NON_MATCHING /* ordem dos testes do switch (pivô no caso 1) e zeros nos casos 2/3 */
void func_0021C520(void)
{
    int small[2][4];
    int big[2][4];
    float t;
    int i;

    memset(small, 0, sizeof(small));
    memset(big, 0, sizeof(big));
    t = Sys_FloatTime();

    for (i = 0; i < 10; i++) {
        int s;
        int b;

        s = 0;
        b = 0;
        switch (D_003A62E0[i].state) {
        case 0:
            D_003A62E0[i].state = 1;
            D_003A62E0[i].endtime = t + D_003A62E0[i].duration;
            s = D_003A62E0[i].small;
            b = D_003A62E0[i].big;
            break;
        case 1:
            if (D_003A62E0[i].endtime <= t) {
                D_003A62E0[i].state = 3;
            } else {
                s = D_003A62E0[i].small;
                b = D_003A62E0[i].big;
            }
            break;
        default:
            s = 0;
            b = 0;
            break;
        }
        small[D_003A62E0[i].port][D_003A62E0[i].motor] += s;
        big[D_003A62E0[i].port][D_003A62E0[i].motor] += b;
    }

    if (D_0055A3A0[0].value != 1.0f) {
        small[0][0] = 0;
        big[0][0] = 0;
    }
    if (D_0055A3A0[1].value != 1.0f) {
        small[1][0] = 0;
        big[1][0] = 0;
    }
    if (D_003A6200[0][0].handle != -1)
        func_0021D158(D_003A6200[0][0].handle, small[0][0], big[0][0]);
    if (D_003A6200[1][0].handle != -1)
        func_0021D158(D_003A6200[1][0].handle, small[1][0], big[1][0]);
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/HL1Input", func_0021C520);
#endif

unsigned long func_0021C6C8(void)
{
    unsigned long max = 0;
    int i, j;

    for (i = 0; i < 2; i++) {
        for (j = 0; j < 4; j++) {
            if (D_003A6200[i][j].handle != -1) {
                unsigned long t = func_00218090(D_003A6200[i][j].handle);

                if (max < t)
                    max = t;
            }
        }
    }
    return max;
}

int func_0021C780(int port, int slot, int mode)
{
    PadSlot *p = &D_003A6200[port][slot];
    KeyStateManager *keys;
    int status;
    int active;
    float x, y, len;

    D_00399158[port] = 1.0f;
    D_00399148[port] = 1.0f;
    p->y = 0;
    p->x = 0;
    status = func_00217B50(p->handle);

    active = 1;
    if (mode == 2 && port == 1)
        active = 0;
    if (mode == 3 && port == 0)
        active = 0;
    if (mode == 4 && port == 1 && cl_numlocal == 1)
        active = 0;

    keys = &p->keys;
    if (status == 2 && active == 1) {
        if (func_002190E8(p->handle, 0) > 0.0f)
            keys->Press(func_0021D778(0, port));
        if (func_00219130(p->handle, 0) > 0.0f)
            keys->Press(func_0021D778(1, port));
        if (func_00219058(p->handle, 0) > 0.0f)
            keys->Press(func_0021D778(2, port));
        if (func_002190A0(p->handle, 0) > 0.0f)
            keys->Press(func_0021D778(3, port));
        if (func_00218E18(p->handle, 0) > 0.0f)
            keys->Press(func_0021D778(4, port));
        if (func_00218EA8(p->handle, 0) > 0.0f)
            keys->Press(func_0021D778(5, port));
        if (func_00218EF0(p->handle, 0) > 0.0f)
            keys->Press(func_0021D778(6, port));
        if (func_00218E60(p->handle, 0) > 0.0f)
            keys->Press(func_0021D778(7, port));
        if (func_00218FC8(p->handle, 0) > 0.0f)
            keys->Press(func_0021D778(8, port));
        if (func_00219010(p->handle, 0) > 0.0f)
            keys->Press(func_0021D778(9, port));
        if (func_00219178(p->handle, 0) > 0.0f)
            keys->Press(func_0021D778(10, port));
        if (func_00218F38(p->handle, 0) > 0.0f)
            keys->Press(func_0021D778(11, port));
        if (func_00218F80(p->handle, 0) > 0.0f)
            keys->Press(func_0021D778(12, port));
        if (func_002191C0(p->handle, 0) > 0.0f)
            keys->Press(func_0021D778(13, port));
        if (func_00219370(p->handle, 0) > 0.0f)
            keys->Press(func_0021D778(14, port));
        if (func_00219328(p->handle, 0) > 0.0f)
            keys->Press(func_0021D778(15, port));

        if (cl[0].intermission)
            goto done;

        x = func_00219208(p->handle, 0);
        y = func_00219250(p->handle, 0);
        if (x > -0.33f && x < 0.33f)
            x = 0;
        if (y > -0.33f && y < 0.33f)
            y = 0;
        len = x * x + y * y;
        if (len > 0.0f && len < D_0055A300[port].value)
            keys->Press(func_0021D778(24, port));
        if (in_leftStickXLook[port].state & 1)
            p->x = x;
        if (in_leftStickYLook[port].state & 1)
            p->y = y;
        if (in_leftStickForwardMove[port].state & 1)
            D_00399158[port] = fabsf(y) * fabsf(y);
        if (in_leftStickSideMove[port].state & 1)
            D_00399148[port] = fabsf(x) * fabsf(x);
        if (x > 0.0f)
            keys->Press(func_0021D778(19, port));
        if (x < 0.0f)
            keys->Press(func_0021D778(18, port));
        if (y > 0.0f)
            keys->Press(func_0021D778(17, port));
        if (y < 0.0f)
            keys->Press(func_0021D778(16, port));

        x = func_00219298(p->handle, 0);
        y = func_002192E0(p->handle, 0);
        if (x > -0.33f && x < 0.33f)
            x = 0;
        if (y > -0.33f && y < 0.33f)
            y = 0;
        len = x * x + y * y;
        if (len > 0.0f && len < D_0055A300[port].value)
            keys->Press(func_0021D778(25, port));
        if (in_rightStickXLook[port].state & 1)
            p->x = x;
        if (in_rightStickYLook[port].state & 1)
            p->y = y;
        if (in_rightStickForwardMove[port].state & 1)
            D_00399158[port] = fabsf(y) * fabsf(y);
        if (in_rightStickSideMove[port].state & 1)
            D_00399148[port] = fabsf(x) * fabsf(x);
        if (x > 0.0f)
            keys->Press(func_0021D778(23, port));
        if (x < 0.0f)
            keys->Press(func_0021D778(22, port));
        if (y > 0.0f)
            keys->Press(func_0021D778(21, port));
        if (y < 0.0f)
            keys->Press(func_0021D778(20, port));
    }
done:
    keys->Dispatch();
    return status;
}

void func_0021D048(int port, int mode)
{
    if (mode == 1) {
        switch (port) {
        case 0:
            break;
        case 1:
            if (cl_numlocal >= 2)
                func_00236A48(1);
            break;
        }
    } else if (mode > 0) {
        if (mode < 5) {
            switch (port) {
            case 0:
                break;
            case 1:
                if (cl_numlocal >= 2)
                    func_00236990(1);
                break;
            }
        }
    }
}

void func_0021D0D0(int port, int mode)
{
    if (mode == 1) {
        switch (port) {
        case 0:
            break;
        case 1:
            if (cl_numlocal >= 2)
                func_00236A68(1);
            break;
        }
    } else if (mode > 0) {
        if (mode < 5) {
            switch (port) {
            case 0:
                break;
            case 1:
                if (cl_numlocal >= 2)
                    func_002369C0(1);
                break;
            }
        }
    }
}

void func_0021D158(int handle, int small, int big)
{
    unsigned char b = big;

    if (big >= 255)
        b = 254;
    func_002193B8(handle, small > 0, b);
}

void func_0021D188(KeyStateManager *keys)
{
    Mem_PushTag();
    Mem_SetTag(1);
    keys->Init(0x36);
    keys->SetData(func_0021D778(0, 0), 0xD2);
    keys->SetData(func_0021D778(1, 0), 0xD3);
    keys->SetData(func_0021D778(2, 0), 0xD4);
    keys->SetData(func_0021D778(3, 0), 0xD5);
    keys->SetData(func_0021D778(4, 0), 0xD6);
    keys->SetData(func_0021D778(5, 0), 0xD7);
    keys->SetData(func_0021D778(6, 0), 0xD8);
    keys->SetData(func_0021D778(7, 0), 0xD9);
    keys->SetData(func_0021D778(8, 0), 0xDA);
    keys->SetData(func_0021D778(9, 0), 0xDB);
    keys->SetData(func_0021D778(10, 0), 0xDC);
    keys->SetData(func_0021D778(11, 0), 0xDD);
    keys->SetData(func_0021D778(12, 0), 0xDE);
    keys->SetData(func_0021D778(13, 0), 0xDF);
    keys->SetData(func_0021D778(14, 0), 0xE0);
    keys->SetData(func_0021D778(15, 0), 0xE1);
    keys->SetData(func_0021D778(16, 0), 0xE2);
    keys->SetData(func_0021D778(17, 0), 0xE3);
    keys->SetData(func_0021D778(18, 0), 0xE4);
    keys->SetData(func_0021D778(19, 0), 0xE5);
    keys->SetData(func_0021D778(20, 0), 0xE6);
    keys->SetData(func_0021D778(21, 0), 0xE7);
    keys->SetData(func_0021D778(22, 0), 0xE8);
    keys->SetData(func_0021D778(23, 0), 0xE9);
    keys->SetData(func_0021D778(24, 0), 0xEA);
    keys->SetData(func_0021D778(25, 0), 0xEB);
    keys->SetData(func_0021D778(0, 1), 0x136);
    keys->SetData(func_0021D778(1, 1), 0x137);
    keys->SetData(func_0021D778(2, 1), 0x138);
    keys->SetData(func_0021D778(3, 1), 0x139);
    keys->SetData(func_0021D778(4, 1), 0x13A);
    keys->SetData(func_0021D778(5, 1), 0x13B);
    keys->SetData(func_0021D778(6, 1), 0x13C);
    keys->SetData(func_0021D778(7, 1), 0x13D);
    keys->SetData(func_0021D778(8, 1), 0x13E);
    keys->SetData(func_0021D778(9, 1), 0x13F);
    keys->SetData(func_0021D778(10, 1), 0x140);
    keys->SetData(func_0021D778(11, 1), 0x141);
    keys->SetData(func_0021D778(12, 1), 0x142);
    keys->SetData(func_0021D778(13, 1), 0x143);
    keys->SetData(func_0021D778(14, 1), 0x144);
    keys->SetData(func_0021D778(15, 1), 0x145);
    keys->SetData(func_0021D778(16, 1), 0x146);
    keys->SetData(func_0021D778(17, 1), 0x147);
    keys->SetData(func_0021D778(18, 1), 0x148);
    keys->SetData(func_0021D778(19, 1), 0x149);
    keys->SetData(func_0021D778(20, 1), 0x14A);
    keys->SetData(func_0021D778(21, 1), 0x14B);
    keys->SetData(func_0021D778(22, 1), 0x14C);
    keys->SetData(func_0021D778(23, 1), 0x14D);
    keys->SetData(func_0021D778(24, 1), 0x14E);
    keys->SetData(func_0021D778(25, 1), 0x14F);
    Mem_PopTag();
}

int func_0021D778(int key, int port)
{
    switch (port) {
    case 0:
        return key;
    case 1:
        return key + 26;
    }
    return key;
}

INCLUDE_ASM("asm/nonmatchings/gbx/HL1Input", func_0021D7A8);

INCLUDE_ASM("asm/nonmatchings/gbx/HL1Input", func_0021D868);

INCLUDE_ASM("asm/nonmatchings/gbx/HL1Input", func_0021D888);

}
