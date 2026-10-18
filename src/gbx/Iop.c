/* Gbx: comunicação com o IOP (comandos SIF, servidor RPC 0x24CE e clientes) */

#include "common.h"

/* ee_thread_t */
typedef struct {
    int status;
    void (*entry)(void);
    void *stack;
    int stackSize;
    void *gpReg;
    int initPriority;
    int currentPriority;
    int attr;
    int option;
} ThreadParam;

/* sceSifClientData (só o campo usado) */
typedef struct {
    int pad[9];
    void *server;
} RpcClient;

/* desliga as interrupções e espera a CPU confirmar */
#define DI()                                                     \
    do {                                                         \
        int stat_;                                               \
        do {                                                     \
            __asm__ volatile("di");                              \
            __asm__ volatile("sync.p");                          \
            __asm__ volatile("mfc0 %0, $12" : "=r"(stat_));      \
        } while (stat_ & 0x10000);                               \
    } while (0)
#define EI() __asm__ volatile("ei")

int func_002EF460(ThreadParam *t);
int func_002EF480(int thread, void *arg);
int func_002EF550(void);
void func_002F1A30(void *buf, int n);
void func_002F1A60(int cmd, void *handler, void *arg);
int func_002F24D0(RpcClient *cd, int id, int mode);
int func_002F26A0(void *cd, int fno, int mode, void *send, int ssize, void *recv, int rsize, void *end, void *endparam);
int func_002F2890(void *cd);
void func_002F28D0(void *q, int thread);
void func_002F2960(void *s, int id, void *(*func)(int, void *, int), void *buf, void *cfunc, void *cbuf, void *q);
void func_002F2D28(void *q);
void func_00236BF8(void);
void func_00236C78(void *data);
void func_00236EF0(void);
void func_00236F90(void);
void func_00236FD8(const char *path);
void func_00238358(void);
void func_002387D0(void);
void func_00238E20(void *data);
void func_002390B0(void);
void *func_00239108(int fno, void *data, int size);
void func_00239248(RpcClient *cd, int id);
void func_002392B0(RpcClient *cd);

extern RpcClient D_0032EF40;    /* cliente RPC do IOP */
extern int D_00328B28;
extern int D_00328B30;
extern int D_00328B34;
extern char D_00328B40[];       /* pilha da thread do servidor */
extern char D_003A0F70[];       /* _gp */
extern char D_003A9378[];
extern int D_003A93F8[16];
extern int D_003A9440;
extern char D_003A9480[];

#ifdef NON_MATCHING /* o addiu de 60 e o li 15 saem trocados */
void func_00238F50(void) {
    int i;

    for (i = 15; i >= 0; i--) {
        D_003A93F8[i] = 0;
    }
    DI();
    func_002F1A30(D_003A9378, 0x10);
    EI();
    D_00328B28 = 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Iop", func_00238F50);
#endif

#ifdef NON_MATCHING /* o lui 0x10000 do DI() fica depois dos sq */
void func_00238FC8(int cmd, void *handler, void *arg) {
    DI();
    func_002F1A60(cmd, handler, arg);
    EI();
    D_003A93F8[cmd] = 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Iop", func_00238FC8);
#endif

void func_00239030(void) {
    ThreadParam t;

    t.entry = func_002390B0;
    t.stack = D_00328B40;
    t.stackSize = 0x6400;
    t.gpReg = D_003A0F70;
    t.initPriority = 1;
    t.option = 0;
    func_00236BF8();
    D_00328B30 = func_002EF550();
    D_00328B34 = func_002EF460(&t);
    func_002EF480(D_00328B34, 0);
}

void func_002390B0(void) {
    char q[0x20];
    char s[0x50];

    func_002F28D0(q, func_002EF550());
    func_002F2960(s, 0x24CE, func_00239108, D_003A9480, 0, 0, q);
    func_002F2D28(q);
}

#ifdef NON_MATCHING /* o alvo copia a1 para a0 antes do switch */
void *func_00239108(int fno, void *data, int size) {
    D_003A9440 = 0;
    if (fno == 0) {
        func_00238E20(data);
    } else if (fno == 100) {
        func_00236C78(data);
    }
    return &D_003A9440;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Iop", func_00239108);
#endif

void func_00239170(void) {
    func_00239248(&D_0032EF40, 0x8C9);
    func_002392B0(&D_0032EF40);
    func_00236EF0();
    func_00236F90();
    func_00236FD8("");
    func_00238358();
    func_002387D0();
}

void func_002391D8(void) {
    while (func_002F2890(&D_0032EF40) != 0) {
    }
    func_002F26A0(&D_0032EF40, 1, 0, 0, 0, 0, 0, 0, 0);
    while (func_002F2890(&D_0032EF40) != 0) {
    }
}

void func_00239248(RpcClient *cd, int id) {
    int i;

    while (func_002F24D0(cd, id, 0), cd->server == 0) {
        i = 0xFFFF;
        while (i--) {
        }
    }
}

void func_002392B0(RpcClient *cd) {
    while (func_002F2890(cd) != 0) {
    }
    func_002F26A0(cd, 0, 0, 0, 0, 0, 0, 0, 0);
    while (func_002F2890(cd) != 0) {
    }
}
