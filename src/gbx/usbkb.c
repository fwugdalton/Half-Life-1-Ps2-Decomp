/* cliente SIF RPC do módulo IOP de teclado USB (usbkb) */

#include "common.h"

/* sceSifClientData */
typedef struct {
    char pad[0x24];
    void *serve;
    char pad2[0x18];
} SifClient;

void func_002F1E40(int);
int func_002F24D0(SifClient *cd, unsigned int id, int mode);
int func_002F26A0(SifClient *cd, int fno, int mode, void *send, int ssize, void *recv, int rsize, void *end, void *endparam);
int func_002F2890(SifClient *cd);

extern SifClient D_003A7FC0;
extern int D_003A8000[16];

int func_00234B58(void) {
    unsigned int i;

    func_002F1E40(0);
    while (1) {
        if (func_002F24D0(&D_003A7FC0, 0x80000211, 0) < 0) {
            while (1) {
            }
        }
        if (D_003A7FC0.serve != 0) {
            break;
        }
        i = 0x10000;
        while (i--) {
        }
    }
    return 0;
}

int func_00234BE0(void *info) {
    return func_002F26A0(&D_003A7FC0, 1, 1, D_003A8000, 0x20, info, 0x20, 0, 0);
}

int func_00234C28(int port, void *data) {
    D_003A8000[0] = port;
    return func_002F26A0(&D_003A7FC0, 2, 1, D_003A8000, 0x20, data, 0x20, 0, 0);
}

int func_00234C70(int port, void *data) {
    D_003A8000[0] = port;
    return func_002F26A0(&D_003A7FC0, 3, 1, D_003A8000, 0x20, data, 0x20, 0, 0);
}

int func_00234CB8(int mode) {
    switch (mode) {
    case 0:
        while (func_002F2890(&D_003A7FC0) != 0) {
        }
        return 0;
    case 1:
        return func_002F2890(&D_003A7FC0);
    }
    return -1;
}
