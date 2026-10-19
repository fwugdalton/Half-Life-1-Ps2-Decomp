/* Gbx/Jade/Buffer.cpp: cópia de buffers e funções de memória/caracteres */

#include "common.h"

extern "C" {

void *Mem_DbgMalloc(int size, const char *file, int line);
void *memcpy(void *dst, const void *src, unsigned int n);
void *memset(void *dst, int c, unsigned int n);
int func_002E46A4(const void *a, const void *b, int n);
void Q_memcpy(void *dst, const void *src, int n);

extern const unsigned char D_00397D81[]; /* _ctype_ + 1 */

void func_002257D0(const void *src, int size) {
    Q_memcpy(Mem_DbgMalloc(size, "Gbx/Jade/Buffer.cpp", 0x1F), src, size);
}

void Q_memcpy(void *dst, const void *src, int n) {
    memcpy(dst, src, n);
}

int func_00225840(const void *a, const void *b, int n) {
    return func_002E46A4(a, b, n);
}

void func_00225860(void *dst, char c, int n) {
    memset(dst, c, n);
}

char func_00225880(char c) {
    int r;

    r = c;
    if (D_00397D81[c] & 1) {
        r = c + 0x20;
    }
    return r;
}

char func_002258B0(char c) {
    int r;

    r = c;
    if (D_00397D81[c] & 2) {
        r = c - 0x20;
    }
    return r;
}

bool func_002258E0(char c) {
    if (D_00397D81[c] & 7) {
        return true;
    }
    return false;
}

bool func_00225908(char c) {
    if (D_00397D81[c] & 3) {
        return true;
    }
    return false;
}

bool func_00225930(char c) {
    if (D_00397D81[c] & 4) {
        return true;
    }
    return false;
}

bool func_00225958(char c) {
    if (D_00397D81[c] & 0x17) {
        return true;
    }
    return false;
}

bool func_00225980(char c) {
    if (D_00397D81[c] & 8) {
        return true;
    }
    return false;
}

bool func_002259A8(char c) {
    if (D_00397D81[c] & 1) {
        return true;
    }
    return false;
}

bool func_002259C8(char c) {
    if (D_00397D81[c] & 2) {
        return true;
    }
    return false;
}

bool func_002259F0(char c) {
    if (D_00397D81[c] & 0x20) {
        return true;
    }
    return false;
}

}
