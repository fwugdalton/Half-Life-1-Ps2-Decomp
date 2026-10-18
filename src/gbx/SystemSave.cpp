/* Gbx: save do sistema no memory card (BASLUS-20066SYSTEM; nome original desconhecido) */

#include "common.h"

extern "C" {
#include "engine/decals.h"

void func_00228EE0(char *dst, const char *fmt, ...);
void *func_0022B118(const char *name, const char *mode);
void func_0022B238(void *f);
void func_0022B248(const char *name);
int func_0022B980(const char *name, const char *path, int card);
int func_0022C598(int card);
int func_0022C740(int card);
int func_0022C7E0(int card, const char *dir);
int func_0022CF08(int card, const char *pattern, int *handle, char *name);
void func_0022D260(int handle);
int func_0022D6F8(int card, const char *name, int size, const void *buf);
int func_0022D960(int card);
int func_0022DFA8(int card);
int func_0022E118(int a, int b);
int func_0022E1C8(int card);
int func_0022E6A0(int card, const char *dir);
void Pak_Init(pakfile_t *pak);
void Pak_Load(pakfile_t *pak, const char *path);
void func_00235198(pakfile_t *pak);
void func_00263308(void *f);
void Host_WriteControls(void *f);
void func_002CBE10(void);

extern char D_00328250[16];
extern const char D_00387218[]; /* "%s%s" */
extern const char D_00387220[]; /* "SYSTEM" */
extern unsigned int D_0032826C;
extern int D_003A75C0;

int func_0022DAD0(int card);
int func_0022DB88(int card);

/* "%s%s" e "SYSTEM" são usadas primeiro por func_0022D9B8 (asm) e vêm antes das strings de func_0022DAD0 */
__asm__(".section .rodata\n"
        ".balign 8\n"
        ".globl D_00387218\n"
        "D_00387218: .asciz \"%s%s\"\n"
        ".balign 8\n"
        ".globl D_00387220\n"
        "D_00387220: .asciz \"SYSTEM\"\n"
        ".balign 8\n"
        ".section .text");

#ifdef NON_MATCHING
/* o original guarda a constante 1 em s0 para as comparações */
int func_0022D9B8(int card) {
    char name[0x20];
    char dir[0x20];
    int handle;
    int ok;
    int err;

    func_00228EE0(dir, D_00387218, D_00328250, D_00387220);
    ok = 0;
    err = func_0022CF08(card, dir, &handle, name);
    if (err == 0) {
        if (handle == -1) {
            err = func_0022C7E0(card, dir);
        } else {
            func_0022D260(handle);
        }
        if (err == 0) {
            if (func_0022E6A0(card, dir) == 1) {
                if (func_0022DAD0(card) == 1) {
                    ok = 1;
                    if (func_0022DB88(card) != 1) {
                        ok = 0;
                    }
                }
            }
        }
    }
    return ok;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/SystemSave", func_0022D9B8);
#endif

int func_0022DAB0(int card) {
    return func_0022DAD0(card);
}

int func_0022DAD0(int card) {
    char path[0x50];
    void *f;
    int ok;

    func_00228EE0(path, "%s%s%s%s", D_00328250, D_00387220, "/", "system.cfg");
    f = func_0022B118("system.cfg", "w");
    func_00263308(f);
    Host_WriteControls(f);
    func_0022B238(f);
    ok = func_0022B980("system.cfg", path, card) == 1;
    func_0022B248("system.cfg");
    return ok;
}

#ifdef NON_MATCHING
/* troca v0/v1 ao ler lumps e data do pak */
int func_0022DB88(int card) {
    pakfile_t pak;
    char path[0x50];
    char dir[0x20];
    int ok;
    int i;

    ok = 1;
    i = 0;
    Pak_Init(&pak);
    pak.inMemory = 1;
    Pak_Load(&pak, "ICONS\\SYSSAVE.PAK;1");
    for (; i < pak.count; i++) {
        func_00228EE0(dir, D_00387218, D_00328250, D_00387220);
        func_00228EE0(path, "%s%s%s", dir, "/", pak.lumps[i].name);
        if (!func_0022D6F8(card, path, pak.lumps[i].size, pak.data + pak.lumps[i].filepos)) {
            ok = 0;
            break;
        }
    }
    func_00235198(&pak);
    return ok;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/SystemSave", func_0022DB88);
#endif

void func_0022DCB8(void) {
}

#ifdef NON_MATCHING
/* troca v0/v1 no contador % 10 do início */
int func_0022DCC0(int state) {
    int next;
    int first;
    int r;

    next = 0;
    if (D_0032826C % 10 == 0) {
        switch (state) {
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 11:
        case 12:
        case 26:
            if (func_0022C598(0) == 1) {
                next = 1;
            } else if (func_0022C598(1) == 1) {
                next = 1;
            } else {
                next = 0;
            }
            break;
        case 9:
        case 13:
        case 17:
        case 19:
            next = func_0022C598(0) == 1;
            break;
        case 10:
        case 14:
        case 18:
        case 20:
            next = func_0022C598(1) == 1;
            break;
        case 7:
        case 8:
        case 15:
        case 16:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
            next = 0;
            break;
        }
    }
    D_0032826C++;
    if (next != 0) {
        return next;
    }
    switch (state) {
    case 1:
        next = 2;
        break;
    case 2:
        first = func_0022DFA8(0);
        D_003A75C0 = first;
        next = 3;
        if (first == 5) {
            next = 0x13;
        }
        break;
    case 3:
        r = func_0022DFA8(1);
        if (r == 5) {
            next = 0x14;
        } else {
            next = func_0022E118(D_003A75C0, r);
        }
        break;
    case 34:
        next = 0x14;
        break;
    case 7:
        next = 9;
        break;
    case 8:
        next = 0xA;
        break;
    case 9:
        switch (func_0022D960(0)) {
        case 0:
            next = 0xF;
            break;
        case 1:
            next = 9;
            break;
        case 2:
            next = 0x19;
            break;
        default:
            next = 0;
            break;
        }
        break;
    case 10:
        switch (func_0022D960(1)) {
        case 0:
            next = 0x10;
            break;
        case 1:
            next = 0xA;
            break;
        case 2:
            next = 0x25;
            break;
        default:
            next = 0;
            break;
        }
        break;
    case 0:
        next = 0;
        break;
    case 15:
        next = 0x11;
        break;
    case 16:
        next = 0x12;
        break;
    case 17:
        next = 0x17;
        if (func_0022D9B8(0) == 1) {
            next = 0x15;
        }
        break;
    case 18:
        next = 0x23;
        if (func_0022D9B8(1) == 1) {
            next = 0x16;
        }
        break;
    case 33:
        next = 0x13;
        break;
    case 19:
        if (func_0022E1C8(0) == 1) {
            next = 0x1C;
            func_002CBE10();
        } else {
            next = 0x18;
        }
        break;
    case 20:
        if (func_0022E1C8(1) == 1) {
            next = 0x1C;
            func_002CBE10();
        } else {
            next = 0x24;
        }
        break;
    case 21:
    case 22:
        next = 0x1C;
        break;
    case 4:
    case 5:
    case 6:
    case 11:
    case 12:
    case 13:
    case 14:
    case 23:
    case 24:
    case 25:
    case 26:
    case 27:
    case 28:
    case 35:
    case 36:
    case 37:
        next = state;
        break;
    case 29:
        next = 0x1F;
        break;
    case 30:
        next = 0x20;
        break;
    case 31:
        next = 0x19;
        if (func_0022C740(0) == 0) {
            next = 1;
        }
        break;
    case 32:
        next = 0x25;
        if (func_0022C740(1) == 0) {
            next = 1;
        }
        break;
    }
    return next;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/SystemSave", func_0022DCC0);
#endif
}
