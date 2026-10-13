/* Gbx/MemoryCard.cpp -- acesso aos memory cards (libmc) */

#include "common.h"

extern "C" {
#include "engine/cvar.h"

int D_00328218 = 1;
}

struct MCPort {
    int port;
    int slot;
};

struct MCInfo {
    int present;
    int type;
    int format;
    int free;
};

/* entrada de diretório do libmc (sceMcTblGetDir) */
struct MCDirEntry {
    char create[8];
    char modify[8];
    unsigned int size;
    unsigned short attr;
    unsigned short reserve1;
    unsigned int reserve2;
    unsigned int pdaAplNo;
    char name[0x20];
};

enum MCOpenMode {
    MC_READ,
    MC_WRITE,
    MC_READWRITE
};

/* busca de arquivos em andamento */
struct MCFind {
    int free;
    char pattern[0x100];
    MCPort port;
    MCDirEntry *entries;
    int count;
    int index;
};

extern "C" {
void *Mem_DbgMalloc(int size, const char *file, int line);
void *func_00225D48(int size, int align, const char *file, int line);
void func_0022D590(void);
MCPort func_0022D4A8(int card);
MCInfo func_0022D3C0(int port, int slot);
int func_002D57F8(int mode, int *cmd, int *result);
int func_002D5938(int port, int slot, int *type, int *free, int *format);
int func_002D5CC8(int port, int slot);
int func_002D6118(int port, int slot);
int func_002D53F8(int port, int slot, const char *dir);
int func_002D5310(int port, int slot, const char *name, int mode);
int func_002D5430(int fd);
int func_002D5D58(int port, int slot, const char *name);
int func_002D6018(int port, int slot, const char *name, const char *newname);
int func_002D55E0(int fd, void *buf, int size);
int func_002D56B8(int fd, const void *buf, int size);
int func_002D5BE0(int port, int slot, const char *dir, char *cur);
int func_002D5A70(int port, int slot, const char *name, unsigned int mode, int max, MCDirEntry *table);
int func_002D5078(void);
char *func_00228CF0(char *dst, const char *src);
void func_0022D818(int arg);
extern MCDirEntry D_003A7000;

extern MCInfo *D_003A7040;
extern MCFind D_003A7048[5];

void func_0022C4E8(void);
int func_0022C638(int card);

void func_0022C448(void) {
    int i;

    func_0022D590();
    for (i = 0; i < 5; i++) {
        D_003A7048[i].free = 1;
        D_003A7048[i].entries = (MCDirEntry *) func_00225D48(0x280, 0x40, "Gbx/MemoryCard.cpp", 0x53);
    }
    D_003A7040 = (MCInfo *)Mem_DbgMalloc(0x20, "Gbx/MemoryCard.cpp", 0x56);
    D_00328218 = 0;
    func_0022C4E8();
}

void func_0022C510(int card);

void func_0022C4E8(void) {
    func_0022C510(0);
    func_0022C510(1);
}

void func_0022C510(int card) {
    MCPort p;

    p = func_0022D4A8(card);
    D_003A7040[card] = func_0022D3C0(p.port, p.slot);
}

int func_0022C598(int card) {
    int cmd, result;
    MCPort p;

    p = func_0022D4A8(card);
    if (func_002D5938(p.port, p.slot, 0, 0, 0) != 0) {
        return 1;
    }
    func_002D57F8(0, &cmd, &result);
    if (result == 0) {
        return 0;
    }
    if (result >= -9) {
        return 1;
    }
    return func_0022C638(card) != 0;
}

int func_0022C638(int card) {
    return D_003A7040[card].present;
}

int func_0022C650(int card) {
    return D_003A7040[card].type;
}

int func_0022C668(int card) {
    return D_003A7040[card].format;
}

int func_0022C680(int card) {
    return D_003A7040[card].free << 10;
}

int func_0022C6A0(int card) {
    int cmd, result;
    MCPort p;

    p = func_0022D4A8(card);
    if (func_002D5CC8(p.port, p.slot) != 0) {
        return 0x13;
    }
    func_002D57F8(0, &cmd, &result);
    if (result < 0) {
        return 3;
    }
    if (result != 0) {
        return 0x13;
    }
    D_003A7040[card].format = 0;
    return 0;
}

int func_0022C740(int card) {
    int cmd, result;
    MCPort p;
    int ret;

    p = func_0022D4A8(card);
    if (func_002D6118(p.port, p.slot) != 0) {
        ret = 0x13;
    } else {
        func_002D57F8(0, &cmd, &result);
        if (result < 0) {
            ret = 4;
        } else if (result != 0) {
            ret = 0x13;
        } else {
            D_003A7040[card].format = 1;
            ret = 0;
        }
    }
    return ret;
}

int func_0022C7E0(int card, const char *dir) {
    int cmd, result;
    MCPort p;

    p = func_0022D4A8(card);
    if (func_002D53F8(p.port, p.slot, dir) != 0) {
        return 0x13;
    }
    func_002D57F8(0, &cmd, &result);
    if (result < -9) {
        return 1;
    }
    switch (result) {
    case 0:
        return 0;
    case -2:
        return 2;
    case -3:
        return 6;
    case -4:
        return 7;
    }
    return 0x13;
}

int func_0022C8B0(int card, const char *name, int create, MCOpenMode mode, int *fd) {
    int cmd, result;
    MCPort p;
    int flags;

    p = func_0022D4A8(card);
    switch (mode) {
    case MC_WRITE:
        flags = 2;
        break;
    case MC_READWRITE:
        flags = 3;
        break;
    case MC_READ:
    default:
        flags = 1;
        break;
    }
    if (create == 1) {
        flags |= 0x200;
    }
    if (func_002D5310(p.port, p.slot, name, flags) != 0) {
        return 0x13;
    }
    func_002D57F8(0, &cmd, &result);
    if (result >= 0) {
        *fd = result;
        return 0;
    }
    if (result < -9) {
        return 1;
    }
    switch (result) {
    case -2:
        return 2;
    case -3:
        return 6;
    case -4:
        return 9;
    case -5:
        return 0xC;
    case -7:
        return 0xA;
    }
    return 0x13;
}

int func_0022C9F0(int fd) {
    int cmd, result;

    if (func_002D5430(fd) != 0) {
        return 0x13;
    }
    func_002D57F8(0, &cmd, &result);
    if (result < -9) {
        return 1;
    }
    switch (result) {
    case 0:
        return 0;
    case -2:
        return 2;
    case -4:
        return 0xB;
    case -5:
        return 0xB;
    }
    return 0x13;
}

int func_0022CA78(int card, const char *name) {
    int cmd, result;
    MCPort p;

    p = func_0022D4A8(card);
    if (func_002D5D58(p.port, p.slot, name) != 0) {
        return 0x13;
    }
    func_002D57F8(0, &cmd, &result);
    if (result < -9) {
        return 1;
    }
    switch (result) {
    case 0:
        return 0;
    case -2:
        return 2;
    case -4:
        return 9;
    case -5:
        return 0xD;
    case -6:
        return 0xE;
    }
    return 0x13;
}

int func_0022CB50(int card, const char *name, const char *newname) {
    int cmd, result;
    MCPort p;

    p = func_0022D4A8(card);
    if (func_002D6018(p.port, p.slot, name, newname) != 0) {
        return 0x13;
    }
    func_002D57F8(0, &cmd, &result);
    if (result < -9) {
        return 1;
    }
    switch (result) {
    case 0:
        return 0;
    case -2:
        return 2;
    case -4:
        return 9;
    }
    return 0x13;
}

int func_0022CC20(int fd, void *buf, int size, int *read) {
    int cmd, result;

    if (func_002D55E0(fd, buf, size) != 0) {
        return 0x13;
    }
    func_002D57F8(0, &cmd, &result);
    if (result >= 0) {
        *read = result;
        return 0;
    }
    if (result < -9) {
        return 1;
    }
    switch (result) {
    case -2:
        return 2;
    case -4:
        return 0xF;
    case -5:
        return 0x10;
    }
    return 0x13;
}

int func_0022CCC0(int fd, const void *buf, int size, int *written) {
    int cmd, result;

    if (func_002D56B8(fd, buf, size) != 0) {
        return 0x13;
    }
    func_002D57F8(0, &cmd, &result);
    if (result >= 0) {
        *written = result;
        return 0;
    }
    if (result < -9) {
        return 1;
    }
    switch (result) {
    case -1:
        return 5;
    case -2:
        return 2;
    case -3:
        return 6;
    case -4:
        return 0xF;
    case -5:
        return 0x10;
    case -8:
        return 0x11;
    }
    return 0x13;
}

int func_0022CD78(int card, char *cur) {
    int cmd, result;
    MCPort p;

    p = func_0022D4A8(card);
    if (func_002D5BE0(p.port, p.slot, ".", cur) != 0) {
        return 0x13;
    }
    func_002D57F8(0, &cmd, &result);
    if (result < -9) {
        return 1;
    }
    switch (result) {
    case 0:
        return 0;
    case -2:
        return 2;
    case -4:
        return 7;
    }
    return 0x13;
}

int func_0022CE40(int card, const char *dir) {
    int cmd, result;
    MCPort p;

    p = func_0022D4A8(card);
    if (func_002D5BE0(p.port, p.slot, dir, 0) != 0) {
        return 0x13;
    }
    func_002D57F8(0, &cmd, &result);
    if (result < -9) {
        return 1;
    }
    switch (result) {
    case 0:
        return 0;
    case -2:
        return 2;
    case -4:
        return 7;
    }
    return 0x13;
}

int func_0022CF08(int card, const char *pattern, int *handle, char *name) {
    int cmd, result;
    MCPort p;
    int i;
    int found;
    int ret;

    found = 0;
    for (i = 0; i < 5; i++) {
        if (D_003A7048[i].free == 1) {
            found = 1;
            break;
        }
    }
    if (found != 1) {
        ret = 0x12;
    } else {
        p = func_0022D4A8(card);
        if (func_002D5A70(p.port, p.slot, pattern, 0, 10, D_003A7048[i].entries) != 0) {
            ret = 0x13;
        } else {
            func_002D57F8(0, &cmd, &result);
            if (result > 0) {
                D_003A7048[i].free = 0;
                D_003A7048[i].port = p;
                func_00228CF0(D_003A7048[i].pattern, pattern);
                D_003A7048[i].count = result;
                D_003A7048[i].index = 0;
                *handle = i;
                func_00228CF0(name, D_003A7048[i].entries->name);
                ret = 0;
            } else if (result < -9) {
                ret = 1;
            } else {
                switch (result) {
                case 0:
                    *handle = -1;
                    ret = 0;
                    break;
                case -2:
                    ret = 2;
                    break;
                case -4:
                    ret = 7;
                    break;
                default:
                    ret = 0x13;
                    break;
                }
            }
        }
    }
    return ret;
}

int func_0022D0F8(int handle, int *found, char *name) {
    int cmd, result;

    *found = 0;
    D_003A7048[handle].index++;
    if (D_003A7048[handle].index < D_003A7048[handle].count) {
        func_00228CF0(name, D_003A7048[handle].entries[D_003A7048[handle].index].name);
        *found = 1;
        return 0;
    }
    if (D_003A7048[handle].count != 10) {
        return 0;
    }
    if (func_002D5A70(D_003A7048[handle].port.port, D_003A7048[handle].port.slot, D_003A7048[handle].pattern, 1, 10, D_003A7048[handle].entries) != 0) {
        return 0x13;
    }
    func_002D57F8(0, &cmd, &result);
    if (result > 0) {
        D_003A7048[handle].count = result;
        D_003A7048[handle].index = 0;
        func_00228CF0(name, D_003A7048[handle].entries->name);
        *found = 1;
        return 0;
    }
    if (result < -9) {
        return 1;
    }
    switch (result) {
    case 0:
        return 0;
    case -2:
        return 2;
    case -4:
        return 7;
    }
    return 0x13;
}

void func_0022D260(int handle) {
    D_003A7048[handle].free = 1;
}

int func_0022D280(int card, const char *name, int *size) {
    int cmd, result;
    MCPort p;
    int ret;

    p = func_0022D4A8(card);
    if (func_002D5A70(p.port, p.slot, name, 0, 1, &D_003A7000) != 0) {
        ret = 0x13;
    } else {
        func_002D57F8(0, &cmd, &result);
        if (result > 0) {
            ret = 0;
            *size = D_003A7000.size;
        } else {
            *size = 0;
            if (result < -9) {
                ret = 1;
            } else {
                switch (result) {
                case 0:
                    ret = 0;
                    break;
                case -2:
                    ret = 2;
                    break;
                case -4:
                    ret = 7;
                    break;
                default:
                    ret = 0x13;
                    break;
                }
            }
        }
    }
    return ret;
}

int func_0022D390(int size) {
    return ((size + 0x3FF) / 0x400 + 1) * 0x400;
}

int func_0022D3B8(void) {
    return 0x800;
}

int func_0022D510(int type);
int func_0022D570(int format);

MCInfo func_0022D3C0(int port, int slot) {
    MCInfo info;
    int type, free, format;
    int cmd, result;

    if (func_002D5938(port, slot, &type, &free, &format) == 0) {
        func_002D57F8(0, &cmd, &result);
        if (result < -9) {
            info.present = 0;
            info.type = 3;
            info.format = 2;
            info.free = 0;
        } else {
            switch (result) {
            case -1:
                info.present = 1;
                info.type = func_0022D510(type);
                info.format = func_0022D570(format);
                info.free = free;
                break;
            case 0:
                info.present = 1;
                info.type = func_0022D510(type);
                info.format = func_0022D570(format);
                info.free = free;
                break;
            case -2:
                info.present = 1;
                info.type = func_0022D510(type);
                info.format = func_0022D570(format);
                info.free = free;
                break;
            }
        }
    }
    return info;
}

MCPort func_0022D4A8(int card) {
    MCPort p;

    switch (card) {
    case 0:
        p.port = 0;
        p.slot = 0;
        break;
    case 1:
        p.port = 1;
        p.slot = 0;
        break;
    }
    return p;
}

int func_0022D510(int type) {
    switch (type) {
    case 0:
        return 3;
    case 1:
        return 1;
    case 2:
        return 0;
    case 3:
        return 2;
    }
    return 3;
}

int func_0022D570(int format) {
    switch (format) {
    case 0:
        return 1;
    case 1:
        return 0;
    }
    return 1;
}

void func_0022D590(void) {
    func_002D5078();
}

cvar_t D_00328220 = { "memoryCardInSlot1", "0" };
cvar_t D_00328238 = { "memoryCardInSlot2", "0" };
int D_0032824C = 0;

void func_0022D5B0(int arg) {
    func_0022C448();
    Cvar_RegisterVariable(&D_00328220);
    Cvar_RegisterVariable(&D_00328238);
    func_0022D818(arg);
    D_0032824C = 1;
}
}
