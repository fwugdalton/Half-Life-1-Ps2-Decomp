/* Gbx: título do icon.sys de um save (texto Shift-JIS com o número do save) */

#include "common.h"

extern "C" {
#include "engine/decals.h"

struct SaveIconTitle {
    char *data;            /* icon.sys inteiro */
    char *title;           /* data + 0xC0 */
    short *lineBreak;      /* data + 6: posição da quebra de linha */
    short savedBreak;
    int len;               /* bytes usados do título */
};

int func_00225930(int c);
int func_00228DC0(const char *s);
int func_00228EE0(char *dst, const char *fmt, ...);
void func_00235218(pakfile_t *pak, const char *name, char **data, int *size);
void __builtin_delete(void *p);
char func_00231208(SaveIconTitle *t, char c);
char func_00231260(SaveIconTitle *t, char c);

SaveIconTitle *func_00230F80(SaveIconTitle *t) {
    t->data = 0;
    return t;
}

void func_00230F90(SaveIconTitle *t, int flags) {
    t->data = 0;
    if (flags & 1) {
        __builtin_delete(t);
    }
}

void func_00230FB8(SaveIconTitle *t, pakfile_t *pak) {
    char *data;
    int size;
    int i;

    func_00235218(pak, "icon.sys", &data, &size);
    t->lineBreak = (short *)(data + 6);
    t->data = data;
    t->savedBreak = *(short *)(data + 6);
    t->len = 0;
    t->title = data + 0xC0;
    for (i = 0; i < 0x43; i += 2) {
        if (t->title[i] == 0 && t->title[i + 1] == 0) {
            t->len = i;
            break;
        }
    }
}

#ifdef NON_MATCHING
/* estrutura igual; o original troca s0/s1/s2 (p, i, t) e soma strlen + (len / 2 + 1) em outra ordem */
void func_00231070(SaveIconTitle *t, int n) {
    char buf[0x20];
    char *p;
    int i;

    if (n > 0 && t->len < 0x3C) {
        i = t->len;
        func_00228EE0(buf, "(%d)", n);
        if (t->savedBreak == 0x20) {
            if (func_00228DC0(buf) + (t->len / 2 + 1) < 0x10) {
                t->title[i] = 0x81;
                t->title[i + 1] = 0x40;
                i += 2;
            } else {
                *t->lineBreak = t->len;
            }
        } else if (func_00228DC0(buf) + (t->len / 2 + 1) < 0x20) {
            t->title[i] = 0x81;
            t->title[i + 1] = 0x40;
            i += 2;
        }
        p = buf;
    loop:
        if (*p != 0 && i < 0x3C) {
            t->title[i] = func_00231208(t, *p);
            t->title[i + 1] = func_00231260(t, *p);
            p++;
            i += 2;
            goto loop;
        }
        t->title[i] = 0;
        t->title[i + 1] = 0;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/SaveIconTitle", func_00231070);
#endif

void func_002311D0(SaveIconTitle *t) {
    int i;

    for (i = t->len; i < 0x44; i++) {
        t->data[i] = 0;
    }
    *t->lineBreak = t->savedBreak;
}

char func_00231208(SaveIconTitle *t, char c) {
    if (func_00225930(c) == 1) {
        return 0x82;
    }
    switch (c) {
    case '(':
        return 0x81;
    case ')':
        return 0x81;
    }
    return 0;
}

char func_00231260(SaveIconTitle *t, char c) {
    if (func_00225930(c) == 1) {
        return c + 0x1F;
    }
    switch (c) {
    case '(':
        return 0x69;
    case ')':
        return 0x6A;
    }
    return 0;
}
}
