/* Gbx: acesso ao save do sistema por slot (1 ou 2) e arquivo de marca da Gearbox (nome original desconhecido) */

#include "common.h"

extern "C" {
void func_00228EE0(char *dst, const char *fmt, ...);
int func_00228DC0(const char *s);
int func_0022C510(int card);
int func_0022C598(int card);
int func_0022C668(int card);
int func_0022C680(int card);
int func_0022C6A0(int card);
int func_0022C8B0(int card, const char *name, int create, int mode, int *fd);
int func_0022C9F0(int fd);
int func_0022CA78(int card, const char *name);
int func_0022CCC0(int fd, const void *buf, int size, int *written);
int func_0022D280(int card, const char *name, int *size);
int func_0022D390(int size);
int func_0022D600(int card, const char *dir, int *err);
int func_0022D688(int card);
int func_0022D9B8(int card);
int func_0022DAB0(int card);
int func_0022E1C8(int card);
void func_002CBE10(void);

extern char D_00328250[16];

int func_0022E2E8(int slot) {
    char dir[0x20];
    int err;
    int card;
    int i;
    int done;
    int ret;

    card = slot - 1;
    for (i = 0; i < 5; i++) {
        func_0022C510(card);
        if (func_0022D688(card) == 1) {
            if (func_0022C668(card) == 0) {
                func_00228EE0(dir, "%s%s", D_00328250, "SYSTEM");
                done = 0;
                if (func_0022D600(card, dir, &err) == 1) {
                    ret = 3;
                    done = 1;
                } else {
                    switch (err) {
                    case 2:
                        ret = 1;
                        break;
                    case 7:
                        ret = 4;
                        done = 1;
                        break;
                    case 0:
                        done = 1;
                        ret = 4;
                        break;
                    case 1:
                        ret = 2;
                        break;
                    default:
                        ret = 2;
                        break;
                    }
                }
                if (done == 1) {
                    break;
                }
            } else {
                ret = 1;
            }
        } else {
            ret = 2;
        }
    }
    return ret;
}

int func_0022E440(int slot) {
    int ret;

    ret = func_0022E1C8(slot - 1);
    func_002CBE10();
    return ret;
}

int func_0022E470(int slot) {
    char dir[0x20];
    int err;
    int card;
    int i;
    int done;
    int ret;

    card = slot - 1;
    for (i = 0; i < 5; i++) {
        func_0022C510(card);
        if (func_0022D688(card) == 1) {
            if (func_0022C668(card) == 0) {
                func_00228EE0(dir, "%s%s", D_00328250, "SYSTEM");
                done = 0;
                if (func_0022D600(card, dir, &err) == 1) {
                    ret = 5;
                    done = 1;
                } else {
                    switch (err) {
                    case 2:
                        ret = 1;
                        break;
                    case 7:
                        ret = 4;
                        done = 1;
                        break;
                    case 0:
                        done = 1;
                        ret = 4;
                        break;
                    case 1:
                        ret = 2;
                        break;
                    default:
                        ret = 2;
                        break;
                    }
                }
                if (done == 1) {
                    break;
                }
            } else {
                ret = 1;
            }
        } else {
            ret = 2;
        }
    }
    if (ret == 4) {
        if (func_0022C680(card) <= 0xC7FF) {
            ret = 3;
        }
    }
    return ret;
}

int func_0022E5E8(int slot) {
    return func_0022D9B8(slot - 1);
}

int func_0022E608(int slot) {
    return func_0022DAB0(slot - 1);
}

int func_0022E628(int slot) {
    int card;
    int tries;
    int r;

    card = slot - 1;
    tries = 5;
    do {
        r = func_0022C6A0(card);
        tries--;
        if (r == 0) {
            break;
        }
    } while (tries > 0);
    return r == 0;
}

int func_0022E678(int slot) {
    return func_0022C598(slot - 1) != 1;
}

int func_0022E6A0(int card, const char *dir) {
    char path[0x50];
    int fd;
    int written;
    int ok;
    int len;

    func_00228EE0(path, "%s%s%s", dir, "/", dir);
    ok = 0;
    if (func_0022C8B0(card, path, 1, 1, &fd) == 0) {
        len = func_00228DC0("Visit http://www.gearboxsoftware.com");
        if (func_0022CCC0(fd, "Visit http://www.gearboxsoftware.com", len, &written) == 0) {
            ok = written == len;
        }
        func_0022C9F0(fd);
    }
    return ok;
}

int func_0022E758(int card, const char *dir) {
    char path[0x50];

    func_00228EE0(path, "%s%s%s", dir, "/", dir);
    return func_0022CA78(card, path) == 0;
}

int func_0022E7A8(int card, const char *dir, int *found) {
    char path[0x50];
    int size;
    int ret;

    *found = 0;
    func_00228EE0(path, "%s%s%s", dir, "/", dir);
    ret = func_0022D280(card, path, &size);
    if (ret == 0) {
        if (size == func_00228DC0("Visit http://www.gearboxsoftware.com")) {
            *found = 1;
        }
    }
    return ret;
}

int func_0022E838(void) {
    return func_0022D390(func_00228DC0("Visit http://www.gearboxsoftware.com"));
}
}
