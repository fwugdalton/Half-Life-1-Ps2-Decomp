/* controles do PS2 via libpad (2 portas, estado em D_003A4B00); nome original desconhecido */

#include "common.h"

/* estado de um controle já convertido: botões em 0/1 e analógicos em -1..1 */
typedef struct {
    float unused;
    float triangle;     /* 0x04 */
    float circle;
    float cross;
    float square;
    float r1;           /* 0x14 */
    float r2;
    float l1;
    float l2;
    float left;         /* 0x24 */
    float right;
    float up;
    float down;
    float l3;           /* 0x34 */
    float r3;
    float lx;           /* 0x3C */
    float ly;
    float rx;
    float ry;
    float start;        /* 0x4C */
    float select;
} PadState;

#define PAD_HISTORY 30

typedef struct {
    int open;
    int slot;
    int port;
    int pad0C;
    unsigned long lastActive;           /* 0x10: tick da última vez que algo foi apertado */
    int actAligned;                     /* 0x18 */
    unsigned char act[2];               /* 0x1C: motores pequeno e grande */
    char pad1E[2];
    PadState cur;                       /* 0x20 */
    unsigned int state;                 /* 0x74: 0 procurando, 1 modo analógico, 2 pronto */
    int histIndex;                      /* 0x78 */
    int histCount;                      /* 0x7C */
    PadState hist[PAD_HISTORY];         /* 0x80 */
    char padA58[0x28];
    unsigned char dmaBuf[256];          /* 0xA80 */
} Pad;

/* libpad */
int func_002D1088(int mode);                                    /* scePadInit */
int func_002D1268(void);                                        /* scePadEnd */
int func_002D12E8(int port, int slot, void *buf);               /* scePadPortOpen */
int func_002D14D0(int port, int slot);                          /* scePadPortClose */
int func_002D1638(int port, int slot, unsigned char *data);     /* scePadRead */
int func_002D16B8(int port, int slot);                          /* scePadGetState */
int func_002D1858(int port, int slot, int act, int term);       /* scePadInfoAct */
int func_002D1A98(int port, int slot, int term, int offs);      /* scePadInfoMode */
int func_002D1BD0(int port, int slot, int offs, int lock);      /* scePadSetMainMode */
int func_002D1C88(int port, int slot, const unsigned char *d);  /* scePadSetActDirect */
int func_002D1D40(int port, int slot, const unsigned char *d);  /* scePadSetActAlign */
void *memset(void *dst, int c, unsigned int n);
int func_002E4560(const void *a, const void *b, unsigned int n); /* memcmp */
unsigned long func_00229B88(void);
unsigned long func_00229C38(unsigned long a, unsigned long b);

void func_00217B00(int i);
int func_00217B50(int i);
int func_00217E98(int i);
int func_00219430(int i);
PadState *func_00219630(Pad *p);
void func_00219670(Pad *p);
PadState func_00219798(unsigned char *data);
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

extern Pad D_003A4B00[2];
extern int D_00325070;                  /* libpad iniciada */
extern unsigned char D_00325078[2];     /* motores para scePadSetActDirect */
extern const unsigned char D_003850D0[]; /* alinhamento dos motores */
extern const unsigned char D_003850D8[]; /* motores desligados */
extern const int D_003850E0[][3];       /* motores esperados (scePadInfoAct) */

void func_002178F8(void) {
    int i;

    func_002D1088(0);
    for (i = 0; i < 2; i++) {
        D_003A4B00[i].open = 0;
        D_003A4B00[i].slot = -1;
        D_003A4B00[i].port = -1;
        D_003A4B00[i].actAligned = 0;
    }
    D_00325070 = 1;
}

void func_00217958(void) {
    Pad *p;
    int i;

    D_00325070 = 0;
    p = D_003A4B00;
    for (i = 0; i < 2; i++, p++) {
        if (p->open == 1) {
            func_00217B00(i);
        }
    }
    func_002D1268();
}

int func_002179D0(int port, int slot) {
    int idx;
    int i;

    idx = -1;
    for (i = 0; i < 2; i++) {
        if (D_003A4B00[i].open == 0) {
            idx = i;
            break;
        }
        if (D_003A4B00[i].slot == slot && D_003A4B00[i].port == port) {
            idx = i;
            break;
        }
    }
    if (i == 2) {
        idx = -1;
    } else if (D_003A4B00[idx].open == 0) {
        D_003A4B00[idx].open = 1;
        D_003A4B00[idx].slot = slot;
        D_003A4B00[idx].port = port;
        D_003A4B00[idx].histIndex = 0;
        D_003A4B00[idx].histCount = 0;
        memset(&D_003A4B00[idx].cur, 0, sizeof(PadState));
        func_002D12E8(port, slot, D_003A4B00[idx].dmaBuf);
        D_003A4B00[idx].state = 0;
    }
    return idx;
}

void func_00217B00(int i) {
    Pad *p;

    p = &D_003A4B00[i];
    func_002D14D0(p->port, p->slot);
    p->open = 0;
    p->slot = -1;
    p->port = -1;
}

int func_00217B50(int i) {
    unsigned char data[32];
    Pad *p;
    int r;

    p = &D_003A4B00[i];
    if (func_002D1A98(p->port, p->slot, 1, -1) != 7) {
        r = 0;
    } else if (func_002D1638(p->port, p->slot, data) == 0) {
        r = 0;
    } else {
        r = 2;
        func_00219670(p);
        p->cur = func_00219798(data);
        if (func_00217E98(i) == 1) {
            p->lastActive = func_00229B88();
        }
    }
    D_003A4B00[i].state = r;
    return r;
}

int func_00217CE8(int i) {
    Pad *p;
    int port;
    int slot;
    int st;
    int n;
    int j;
    int r;

    p = &D_003A4B00[i];
    port = p->port;
    slot = p->slot;
    st = func_002D16B8(port, slot);
    switch (p->state) {
    case 0:
        r = 0;
        if (st == 6) {
            n = func_002D1A98(port, slot, 4, -1);
            for (j = 0; j < n; j++) {
                if (func_002D1A98(port, slot, 4, j) == 7) {
                    while (func_002D16B8(port, slot) != 6) {
                    }
                    func_002D1BD0(port, slot, j, 3);
                    r = 1;
                    break;
                }
            }
        }
        break;
    case 1:
        switch (st) {
        case 6:
            r = 2;
            if (func_00219430(i) != 1) {
                r = 0;
            }
            break;
        case 0:
        case 7:
            r = 0;
            break;
        default:
            r = 1;
            break;
        }
        break;
    case 2:
        r = 2;
        break;
    default:
        r = 0;
        break;
    }
    D_003A4B00[i].state = r;
    return r;
}

int func_00217E98(int i) {
    Pad *p;
    float sum;

    p = &D_003A4B00[i];
    sum = p->cur.triangle;
    sum += p->cur.circle;
    sum += p->cur.cross;
    sum += p->cur.square;
    sum += p->cur.r1;
    sum += p->cur.r2;
    sum += p->cur.l1;
    sum += p->cur.l2;
    sum += p->cur.left;
    sum += p->cur.right;
    sum += p->cur.up;
    sum += p->cur.down;
    sum += p->cur.l3;
    sum += p->cur.r3;
    if (p->cur.lx < -0.33f || p->cur.lx > 0.33f) {
        sum += 1.0f;
    }
    if (p->cur.ly < -0.33f || p->cur.ly > 0.33f) {
        sum += 1.0f;
    }
    if (p->cur.rx < -0.33f || p->cur.rx > 0.33f) {
        sum += 1.0f;
    }
    if (p->cur.ry < -0.33f || p->cur.ry > 0.33f) {
        sum += 1.0f;
    }
    sum += p->cur.start;
    sum += p->cur.select;
    if (sum == 0.0f) {
        return 0;
    }
    return 1;
}

unsigned long func_00218090(int i) {
    Pad *p;

    p = &D_003A4B00[i];
    return p->lastActive;
}

unsigned long func_002180B0(void) {
    unsigned long now;
    unsigned long best;
    unsigned long d;
    int have;
    int i;

    best = 0;
    have = 0;
    now = func_00229B88();
    for (i = 0; i < 2; i++) {
        if (D_003A4B00[i].open == 1) {
            d = func_00229C38(D_003A4B00[i].lastActive, now);
            if (d < best || !have) {
                best = d;
            }
            have = 1;
        }
    }
    return best;
}

void func_00218168(void) {
    unsigned long now;
    int i;

    now = func_00229B88();
    for (i = 0; i < 2; i++) {
        if (D_003A4B00[i].open == 1) {
            D_003A4B00[i].lastActive = now;
        }
    }
}

float func_002181B8(int i, int update) {
    PadState *prev;
    float r;

    prev = func_00219630(&D_003A4B00[i]);
    r = 0.0f;
    if (prev != 0 && prev->triangle <= 0.2f) {
        if (func_00218E18(i, update) > 0.2f) {
            r = 1.0f;
        }
    }
    return r;
}

float func_00218268(int i, int update) {
    PadState *prev;
    float r;

    prev = func_00219630(&D_003A4B00[i]);
    r = 0.0f;
    if (prev != 0 && prev->circle <= 0.2f) {
        if (func_00218E60(i, update) > 0.2f) {
            r = 1.0f;
        }
    }
    return r;
}

float func_00218318(int i, int update) {
    PadState *prev;
    float r;

    prev = func_00219630(&D_003A4B00[i]);
    r = 0.0f;
    if (prev != 0 && prev->cross <= 0.2f) {
        if (func_00218EA8(i, update) > 0.2f) {
            r = 1.0f;
        }
    }
    return r;
}

float func_002183C8(int i, int update) {
    PadState *prev;
    float r;

    prev = func_00219630(&D_003A4B00[i]);
    r = 0.0f;
    if (prev != 0 && prev->square <= 0.2f) {
        if (func_00218EF0(i, update) > 0.2f) {
            r = 1.0f;
        }
    }
    return r;
}

float func_00218478(int i, int update) {
    PadState *prev;
    float r;

    prev = func_00219630(&D_003A4B00[i]);
    r = 0.0f;
    if (prev != 0 && prev->r1 <= 0.2f) {
        if (func_00218F38(i, update) > 0.2f) {
            r = 1.0f;
        }
    }
    return r;
}

float func_00218528(int i, int update) {
    PadState *prev;
    float r;

    prev = func_00219630(&D_003A4B00[i]);
    r = 0.0f;
    if (prev != 0 && prev->r2 <= 0.2f) {
        if (func_00218F80(i, update) > 0.2f) {
            r = 1.0f;
        }
    }
    return r;
}

float func_002185D8(int i, int update) {
    PadState *prev;
    float r;

    prev = func_00219630(&D_003A4B00[i]);
    r = 0.0f;
    if (prev != 0 && prev->l1 <= 0.2f) {
        if (func_00218FC8(i, update) > 0.2f) {
            r = 1.0f;
        }
    }
    return r;
}

float func_00218688(int i, int update) {
    PadState *prev;
    float r;

    prev = func_00219630(&D_003A4B00[i]);
    r = 0.0f;
    if (prev != 0 && prev->l2 <= 0.2f) {
        if (func_00219010(i, update) > 0.2f) {
            r = 1.0f;
        }
    }
    return r;
}

float func_00218738(int i, int update) {
    PadState *prev;
    int held;
    float r;

    prev = func_00219630(&D_003A4B00[i]);
    held = 0;
    if (prev != 0 && (prev->left > 0.2f || prev->lx < -0.5f)) {
        held = 1;
    }
    r = 0.0f;
    if (!held) {
        if (func_00219058(i, update) > 0.2f || func_00219208(i, update) < -0.5f) {
            r = 1.0f;
        }
    }
    return r;
}

float func_00218840(int i, int update) {
    PadState *prev;
    int held;
    float r;

    prev = func_00219630(&D_003A4B00[i]);
    held = 0;
    if (prev != 0 && (prev->right > 0.2f || prev->lx > 0.5f)) {
        held = 1;
    }
    r = 0.0f;
    if (!held) {
        if (func_002190A0(i, update) > 0.2f || func_00219208(i, update) > 0.5f) {
            r = 1.0f;
        }
    }
    return r;
}

float func_00218948(int i, int update) {
    PadState *prev;
    int held;
    float r;

    prev = func_00219630(&D_003A4B00[i]);
    held = 0;
    if (prev != 0 && (prev->down > 0.2f || prev->ly > 0.5f)) {
        held = 1;
    }
    r = 0.0f;
    if (!held) {
        if (func_00219130(i, update) > 0.2f || func_00219250(i, update) > 0.5f) {
            r = 1.0f;
        }
    }
    return r;
}

float func_00218A50(int i, int update) {
    PadState *prev;
    int held;
    float r;

    prev = func_00219630(&D_003A4B00[i]);
    held = 0;
    if (prev != 0 && (prev->up > 0.2f || prev->ly < -0.5f)) {
        held = 1;
    }
    r = 0.0f;
    if (!held) {
        if (func_002190E8(i, update) > 0.2f || func_00219250(i, update) < -0.5f) {
            r = 1.0f;
        }
    }
    return r;
}

float func_00218B58(int i, int update) {
    PadState *prev;
    float r;

    prev = func_00219630(&D_003A4B00[i]);
    r = 0.0f;
    if (prev != 0 && prev->l3 <= 0.2f) {
        if (func_00219178(i, update) > 0.2f) {
            r = 1.0f;
        }
    }
    return r;
}

float func_00218C08(int i, int update) {
    PadState *prev;
    float r;

    prev = func_00219630(&D_003A4B00[i]);
    r = 0.0f;
    if (prev != 0 && prev->r3 <= 0.2f) {
        if (func_002191C0(i, update) > 0.2f) {
            r = 1.0f;
        }
    }
    return r;
}

float func_00218CB8(int i, int update) {
    PadState *prev;
    float r;

    prev = func_00219630(&D_003A4B00[i]);
    r = 0.0f;
    if (prev != 0 && prev->start <= 0.2f) {
        if (func_00219328(i, update) > 0.2f) {
            r = 1.0f;
        }
    }
    return r;
}

float func_00218D68(int i, int update) {
    PadState *prev;
    float r;

    prev = func_00219630(&D_003A4B00[i]);
    r = 0.0f;
    if (prev != 0 && prev->select <= 0.2f) {
        if (func_00219370(i, update) > 0.2f) {
            r = 1.0f;
        }
    }
    return r;
}

float func_00218E18(int i, int update) {
    if (update == 1) {
        func_00217B50(i);
    }
    return D_003A4B00[i].cur.triangle;
}

float func_00218E60(int i, int update) {
    if (update == 1) {
        func_00217B50(i);
    }
    return D_003A4B00[i].cur.circle;
}

float func_00218EA8(int i, int update) {
    if (update == 1) {
        func_00217B50(i);
    }
    return D_003A4B00[i].cur.cross;
}

float func_00218EF0(int i, int update) {
    if (update == 1) {
        func_00217B50(i);
    }
    return D_003A4B00[i].cur.square;
}

float func_00218F38(int i, int update) {
    if (update == 1) {
        func_00217B50(i);
    }
    return D_003A4B00[i].cur.r1;
}

float func_00218F80(int i, int update) {
    if (update == 1) {
        func_00217B50(i);
    }
    return D_003A4B00[i].cur.r2;
}

float func_00218FC8(int i, int update) {
    if (update == 1) {
        func_00217B50(i);
    }
    return D_003A4B00[i].cur.l1;
}

float func_00219010(int i, int update) {
    if (update == 1) {
        func_00217B50(i);
    }
    return D_003A4B00[i].cur.l2;
}

float func_00219058(int i, int update) {
    if (update == 1) {
        func_00217B50(i);
    }
    return D_003A4B00[i].cur.left;
}

float func_002190A0(int i, int update) {
    if (update == 1) {
        func_00217B50(i);
    }
    return D_003A4B00[i].cur.right;
}

float func_002190E8(int i, int update) {
    if (update == 1) {
        func_00217B50(i);
    }
    return D_003A4B00[i].cur.up;
}

float func_00219130(int i, int update) {
    if (update == 1) {
        func_00217B50(i);
    }
    return D_003A4B00[i].cur.down;
}

float func_00219178(int i, int update) {
    if (update == 1) {
        func_00217B50(i);
    }
    return D_003A4B00[i].cur.l3;
}

float func_002191C0(int i, int update) {
    if (update == 1) {
        func_00217B50(i);
    }
    return D_003A4B00[i].cur.r3;
}

float func_00219208(int i, int update) {
    if (update == 1) {
        func_00217B50(i);
    }
    return D_003A4B00[i].cur.lx;
}

float func_00219250(int i, int update) {
    if (update == 1) {
        func_00217B50(i);
    }
    return D_003A4B00[i].cur.ly;
}

float func_00219298(int i, int update) {
    if (update == 1) {
        func_00217B50(i);
    }
    return D_003A4B00[i].cur.rx;
}

float func_002192E0(int i, int update) {
    if (update == 1) {
        func_00217B50(i);
    }
    return D_003A4B00[i].cur.ry;
}

float func_00219328(int i, int update) {
    if (update == 1) {
        func_00217B50(i);
    }
    return D_003A4B00[i].cur.start;
}

float func_00219370(int i, int update) {
    if (update == 1) {
        func_00217B50(i);
    }
    return D_003A4B00[i].cur.select;
}

void func_002193B8(int i, unsigned char small, unsigned char big) {
    Pad *p;

    p = &D_003A4B00[i];
    if (p->actAligned == 1 && (p->act[0] != small || p->act[1] != big)) {
        D_00325078[0] = small;
        p->act[1] = big;
        D_00325078[1] = big;
        p->act[0] = small;
        func_002D1C88(p->port, p->slot, D_00325078);
    }
}

INCLUDE_RODATA("asm/nonmatchings/engine/pad_ps2", D_003850D0);

INCLUDE_RODATA("asm/nonmatchings/engine/pad_ps2", D_003850D8);

INCLUDE_RODATA("asm/nonmatchings/engine/pad_ps2", D_003850E0);

int func_00219430(int i) {
    const int *tbl;
    int port;
    int slot;
    int n;
    int j;
    int match;
    int ok;
    int a[3];
    int st;

    port = D_003A4B00[i].port;
    slot = D_003A4B00[i].slot;
    ok = 1;
    n = func_002D1858(port, slot, -1, 0);
    if (n == 2) {
        D_003A4B00[i].act[0] = 0;
        D_003A4B00[i].act[1] = 0;
        tbl = D_003850E0[0];
        match = 1;
        for (j = 0; j < n; j++, tbl += 3) {
            a[0] = func_002D1858(port, slot, j, 1);
            a[1] = func_002D1858(port, slot, j, 2);
            a[2] = func_002D1858(port, slot, j, 3);
            if (a[0] != tbl[0] || a[1] != tbl[1] || a[2] != tbl[2]) {
                match = 0;
                break;
            }
        }
        if (match == 1) {
            D_003A4B00[i].actAligned = match;
            if (func_002D1D40(port, slot, D_003850D0) != 1) {
                ok = 0;
            } else {
                do {
                    st = func_002D16B8(port, slot);
                } while (st != 0 && st != 6 && st != 7);
                if (st != 6) {
                    ok = 0;
                } else {
                    ok = func_002D1C88(port, slot, D_003850D8) == 1;
                }
            }
        } else {
            D_003A4B00[i].actAligned = 0;
        }
    } else {
        D_003A4B00[i].actAligned = 0;
    }
    return ok;
}

int func_00219628(unsigned int i) {
    return i < 2;
}

PadState *func_00219630(Pad *p) {
    int idx;

    if (p->histCount != 0) {
        idx = p->histIndex - 1;
        if (idx < 0) {
            idx = PAD_HISTORY - 1;
        }
        return &p->hist[idx];
    }
    return 0;
}

void func_00219670(Pad *p) {
    func_00219630(p);
    p->hist[p->histIndex] = p->cur;
    p->histIndex = (p->histIndex + 1) % PAD_HISTORY;
    if (p->histCount < PAD_HISTORY) {
        p->histCount++;
    }
}

PadState func_00219798(unsigned char *data) {
    PadState s;

    s.unused = 0.0f;
    s.triangle = (~data[3] & 0x10) ? 1.0f : 0.0f;
    s.circle = (~data[3] & 0x20) ? 1.0f : 0.0f;
    s.cross = (~data[3] & 0x40) ? 1.0f : 0.0f;
    s.square = (~data[3] & 0x80) ? 1.0f : 0.0f;
    s.r1 = (~data[3] & 0x08) ? 1.0f : 0.0f;
    s.r2 = (~data[3] & 0x02) ? 1.0f : 0.0f;
    s.l1 = (~data[3] & 0x04) ? 1.0f : 0.0f;
    s.l2 = (~data[3] & 0x01) ? 1.0f : 0.0f;
    s.up = (~data[2] & 0x10) ? 1.0f : 0.0f;
    s.down = (~data[2] & 0x40) ? 1.0f : 0.0f;
    s.left = (~data[2] & 0x80) ? 1.0f : 0.0f;
    s.right = (~data[2] & 0x20) ? 1.0f : 0.0f;
    s.l3 = (~data[2] & 0x02) ? 1.0f : 0.0f;
    s.r3 = (~data[2] & 0x04) ? 1.0f : 0.0f;
    s.lx = (data[6] - 127.0f) * (1.0f / 127.0f);
    s.ly = (data[7] - 127.0f) * (1.0f / 127.0f);
    s.rx = (data[4] - 127.0f) * (1.0f / 127.0f);
    s.ry = (data[5] - 127.0f) * (1.0f / 127.0f);
    s.start = (~data[2] & 0x08) ? 1.0f : 0.0f;
    s.select = (~data[2] & 0x01) ? 1.0f : 0.0f;
    return s;
}

int func_00219B00(const PadState *a, const PadState *b) {
    return func_002E4560(a, b, sizeof(PadState)) == 0;
}
