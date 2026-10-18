/* Gbx/SystemSaveInitialize.cpp -- procura e carrega o save do sistema (system.cfg) */

#include "common.h"

extern "C" {
void func_00228EE0(char *dst, const char *fmt, ...);
void *func_00225D48(int size, int align, const char *file, int line);
void func_00225DB8(void *ptr);
void Mem_PushTag(void);
void Mem_SetTag(int tag);
void Mem_PopTag(void);
void Cbuf_AddText(char *text);
void Cbuf_Execute(void);
void func_0022C510(int card);
int func_0022C668(int card);
int func_0022C680(int card);
int func_0022C8B0(int card, const char *name, int create, int mode, int *fd);
int func_0022C9F0(int fd);
int func_0022CC20(int fd, void *buf, int size, int *read);
int func_0022D280(int card, const char *name, int *size);
int func_0022D600(int card, const char *dir, int *err);
int func_0022D688(int card);

extern char D_00328250[16];

int func_0022DFA8(int card) {
    char dir[0x20];
    int err;
    int i;
    int done;
    int ret;

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
                        ret = 2;
                        break;
                    case 7:
                        done = 1;
                        ret = (func_0022C680(card) > 0xC7FF) ? 3 : 4;
                        break;
                    case 0:
                        done = 1;
                        ret = 3;
                        break;
                    case 1:
                        ret = 1;
                        break;
                    default:
                        ret = 1;
                        break;
                    }
                }
                if (done == 1) {
                    break;
                }
            } else {
                ret = 2;
            }
        } else {
            ret = 1;
        }
    }
    return ret;
}

#ifdef NON_MATCHING
/* o original desvia todos os casos para um único jr ra no fim */
int func_0022E118(int a, int b) {
    int r;

    switch (a) {
    case 4:
        switch (b) {
        case 2:
            r = 6;
            break;
        case 3:
            r = 0xE;
            break;
        default:
            r = 0xB;
            break;
        }
        break;
    case 3:
        r = 0xD;
        break;
    case 2:
        r = 5;
        if (b == 3) {
            r = 0xE;
        }
        break;
    case 1:
        switch (b) {
        case 2:
            r = 6;
            break;
        case 3:
            r = 0xE;
            break;
        case 4:
            r = 0xC;
            break;
        default:
            r = 4;
            break;
        }
        break;
    default:
        r = 2;
        break;
    }
    return r;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/SystemSaveInitialize", func_0022E118);
#endif

int func_0022E1C8(int card) {
    char path[0x50];
    int size;
    int fd;
    int read;
    int ok;
    char *buf;

    func_00228EE0(path, "%s%s%s%s", D_00328250, "SYSTEM", "/", "system.cfg");
    ok = 0;
    if (func_0022D280(card, path, &size) == 0) {
        if (func_0022C8B0(card, path, 0, 0, &fd) == 0) {
            Mem_PushTag();
            Mem_SetTag(2);
            buf = (char *)func_00225D48(size + 1, 0x40, "Gbx/SystemSaveInitialize.cpp", 0x287);
            Mem_PopTag();
            if (func_0022CC20(fd, buf, size, &read) == 0 && read == size) {
                buf[read] = 0;
                Cbuf_AddText(buf);
                ok = 1;
                Cbuf_Execute();
            }
            func_00225DB8(buf);
            func_0022C9F0(fd);
        }
    }
    return ok;
}
}
