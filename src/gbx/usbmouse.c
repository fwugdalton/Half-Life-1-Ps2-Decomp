/* cliente SIF RPC do módulo IOP de mouse USB (usbmouse) */

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

extern SifClient D_003A8040;
extern int D_003A8080[16];

int func_00234D18(void) {
    unsigned int i;

    func_002F1E40(0);
    while (1) {
        if (func_002F24D0(&D_003A8040, 0x80000210, 0) < 0) {
            while (1) {
            }
        }
        if (D_003A8040.serve != 0) {
            break;
        }
        i = 0x10000;
        while (i--) {
        }
    }
    return 0;
}

int func_00234DA0(void *info) {
    return func_002F26A0(&D_003A8040, 1, 1, D_003A8080, 0x20, info, 0x20, 0, 0);
}

int func_00234DE8(int port, void *data) {
    D_003A8080[0] = port;
    return func_002F26A0(&D_003A8040, 2, 1, D_003A8080, 0x20, data, 0x20, 0, 0);
}

int func_00234E30(int port, void *data) {
    D_003A8080[0] = port;
    return func_002F26A0(&D_003A8040, 3, 1, D_003A8080, 0x20, data, 0x20, 0, 0);
}

int func_00234E78(int mode) {
    switch (mode) {
    case 0:
        while (func_002F2890(&D_003A8040) != 0) {
        }
        return 0;
    case 1:
        return func_002F2890(&D_003A8040);
    }
    return -1;
}
