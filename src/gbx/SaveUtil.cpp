/* Gbx: utilidades de save no memory card (nome original desconhecido) */

#include "common.h"

extern "C" {
void func_00228CF0(char *dst, const char *src);
void func_00228DA0(char *dst, const char *src, int n);
void func_00228DE0(char *s);
char *func_00228EC0(const char *s, const char *sub);
int func_00228EE0(char *dst, const char *fmt, ...);
int func_0022C598(int card);
int func_0022C638(int card);
int func_0022C650(int card);
int func_0022C6A0(int card);
int func_0022C8B0(int card, const char *name, int create, int mode, int *fd);
int func_0022C9F0(int fd);
int func_0022CC20(int fd, void *buf, int size, int *read);
int func_0022CCC0(int fd, const void *buf, int size, int *written);
int func_0022D280(int card, const char *name, int *size);

extern char D_00328250[16];
extern int D_00328260;

int func_0022D600(int card, const char *dir, int *err) {
    char path[0x50];
    int size;
    int ok;

    func_00228EE0(path, "%s%s%s", dir, "/", "icon.sys");
    ok = 0;
    if ((*err = func_0022D280(card, path, &size)) == 0) {
        ok = size == 0x3C4;
    }
    return ok;
}

int func_0022D688(int card) {
    int present;

    present = func_0022C638(card);
    if (present != 1) {
        return 0;
    }
    if (func_0022C650(card) != 0) {
        present = 0;
    }
    return present;
}

int func_0022D6D8(int card, const char *name, int *size) {
    return func_0022D280(card, name, size) == 0;
}

int func_0022D6F8(int card, const char *name, int size, const void *buf) {
    int fd;
    int written;
    int ok;

    ok = 0;
    if (func_0022C8B0(card, name, 1, 1, &fd) == 0) {
        if (fd != -1) {
            if (func_0022CCC0(fd, buf, size, &written) == 0) {
                ok = written == size;
            }
            if (func_0022C9F0(fd) != 0) {
                ok = 0;
            }
        }
    }
    return ok;
}

int func_0022D788(int card, const char *name, int size, void *buf) {
    int fd;
    int read;
    int ok;

    ok = 0;
    if (func_0022C8B0(card, name, 0, 0, &fd) == 0) {
        if (fd != -1) {
            if (func_0022CC20(fd, buf, size, &read) == 0) {
                ok = read == size;
            }
            if (func_0022C9F0(fd) != 0) {
                ok = 0;
            }
        }
    }
    return ok;
}

void func_0022D910(char *dst, const char *prefix, const char *id);

void func_0022D818(const char *path) {
    char buf[0x100];
    char *id;

    func_00228CF0(buf, path);
    func_00228DE0(buf);
    if (func_00228EC0(path, ".elf")) {
        func_00228CF0(D_00328250, "BASLUS-20066");
    } else if ((id = func_00228EC0(buf, "slus")) != 0) {
        func_0022D910(D_00328250, "BASLUS-", id);
    } else if ((id = func_00228EC0(buf, "sles")) != 0) {
        func_0022D910(D_00328250, "BESLES-", id);
    } else if ((id = func_00228EC0(buf, "slps")) != 0) {
        func_0022D910(D_00328250, "BISLPS-", id);
    } else {
        func_00228CF0(D_00328250, "BASLUS-20066");
    }
}

void func_0022D910(char *dst, const char *prefix, const char *id) {
    func_00228CF0(dst, prefix);
    func_00228DA0(dst, id + 5, 3);
    func_00228DA0(dst, id + 9, 2);
}

int func_0022D960(int card) {
    int ret;

    if (func_0022C6A0(card) == 0) {
        D_00328260 = 0;
        ret = 0;
    } else {
        D_00328260 = D_00328260 + 1; ret = (D_00328260 < 5) ? 1 : 2;
    }
    return ret;
}
}
