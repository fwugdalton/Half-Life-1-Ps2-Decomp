/* Gbx: leituras assíncronas do CD pelo IOP (8 pedidos, até 4 ao mesmo tempo) */

#include "common.h"

/* ee_sema_t */
typedef struct {
    int count;
    int max_count;
    int init_count;
    int wait_threads;
    int attr;
    int option;
} SemaParam;

/* um pedido de leitura (0x1C bytes) */
typedef struct {
    int id;
    int sema;
    int used;
    int state;      /* 1 montando, 2 lendo, 3 pronto, 4 cancelado */
    int size;
    void *buf;
    int freeBuf;
} AsyncSlot;

/* argumentos do RPC 200 */
typedef struct {
    int lba;
    int sectors;
    int id;
    int size;
} AsyncArgs;

int func_002EF660(SemaParam *p);
int func_002EF680(int sema);
int func_002EF6A0(int sema);
void func_002EF8E0(int a);
int func_002F26A0(void *cd, int fno, int mode, void *send, int ssize, void *recv, int rsize, void *end, void *endparam);
int func_002F2890(void *cd);
void func_00225DB8(void *buf);
void func_00229C60(int ms);
int func_00238AD8(int id);
int func_00238F08(void);

extern char D_0032EF40[];       /* cliente RPC do IOP */
extern AsyncArgs D_003A9280;
extern AsyncSlot D_003A9290[8];
extern int D_003A9370;          /* semáforo da tabela */
extern int D_003A9374;          /* leituras em andamento */

void func_002387D0(void) {
    SemaParam p;
    int i;

    p.option = 0;
    p.init_count = 1;
    p.max_count = 1;
    func_002EF8E0(0);
    D_003A9370 = func_002EF660(&p);
    func_002EF6A0(D_003A9370);
    p.init_count = 0;
    func_002EF8E0(0);
    for (i = 0; i < 8; i++) {
        D_003A9290[i].id = i + 100;
        D_003A9290[i].sema = func_002EF660(&p);
        D_003A9290[i].used = 0;
        D_003A9290[i].state = 0;
        D_003A9290[i].size = 0;
        D_003A9290[i].buf = 0;
        D_003A9290[i].freeBuf = 1;
    }
    D_003A9374 = 0;
    func_002EF680(D_003A9370);
}

#ifdef NON_MATCHING /* o gcc calcula o endereço de cada campo separado */
int func_002388A0(int lba, int sectors, int size, void *buf) {
    int ok;
    int id;
    int i;

    while (1) {
        func_002EF6A0(D_003A9370);
        if (D_003A9374 < 4) {
            D_003A9374++;
            i = func_00238F08();
            ok = 1;
            D_003A9280.lba = lba;
            D_003A9280.sectors = sectors;
            D_003A9280.size = size;
            id = D_003A9290[i].id;
            D_003A9290[i].state = 1;
            D_003A9290[i].size = size;
            D_003A9290[i].buf = buf;
            D_003A9290[i].freeBuf = 0;
            D_003A9280.id = D_003A9290[i].id;
            D_003A9290[i].state = 2;
            while (func_002F2890(D_0032EF40) != 0) {
            }
            func_002F26A0(D_0032EF40, 200, 0, &D_003A9280, 0x10, 0, 0, 0, 0);
            while (func_002F2890(D_0032EF40) != 0) {
            }
        } else {
            ok = 0;
            id = -1;
        }
        func_002EF680(D_003A9370);
        if (ok) {
            break;
        }
        func_00229C60(100);
    }
    return id;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/AsyncRead", func_002388A0);
#endif

#ifdef NON_MATCHING /* o laço não é girado nem reduzido como no alvo */
int func_00238A48(int id) {
    int done;
    int i;

    done = 0;
    func_002EF6A0(D_003A9370);
    i = 0;
    while (i < 8) {
        if (D_003A9290[i].id == id) {
            done = D_003A9290[i].state == 3;
            break;
        }
        i++;
    }
    func_002EF680(D_003A9370);
    return done;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/AsyncRead", func_00238A48);
#endif

#ifdef NON_MATCHING /* o laço não é girado nem reduzido como no alvo */
int func_00238AD8(int id) {
    int i;
    int done;

    done = 0;
    func_002EF6A0(D_003A9370);
    for (i = 0; i < 8; i++) {
        if (D_003A9290[i].id == id) {
            done = D_003A9290[i].state == 3;
            break;
        }
    }
    func_002EF680(D_003A9370);
    while (!done) {
        func_002EF6A0(D_003A9290[i].sema);
        func_002EF6A0(D_003A9370);
        if (D_003A9290[i].state == 3) {
            done = 1;
        }
        func_002EF680(D_003A9370);
    }
    return done;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/AsyncRead", func_00238AD8);
#endif

void func_00238BD0(int id) {
    int i;

    func_002EF6A0(D_003A9370);
    for (i = 0; i < 8; i++) {
        if (D_003A9290[i].id == id) {
            D_003A9290[i].size = 0;
            D_003A9290[i].state = 0;
            D_003A9290[i].used = 0;
            break;
        }
    }
    func_002EF680(D_003A9370);
}

#ifdef NON_MATCHING /* as variáveis de indução do laço diferem */
void func_00238C40(int id, int freeBuf) {
    int i;

    func_002EF6A0(D_003A9370);
    for (i = 0; i < 8; i++) {
        if (D_003A9290[i].id == id) {
            if (D_003A9290[i].state == 2) {
                D_003A9290[i].state = 4;
                if (freeBuf == 1) {
                    D_003A9290[i].freeBuf = 1;
                } else {
                    D_003A9290[i].freeBuf = 0;
                }
            }
            if (D_003A9290[i].state == 3) {
                if (freeBuf == 1) {
                    func_00225DB8(D_003A9290[i].buf);
                }
                D_003A9290[i].size = 0;
                D_003A9290[i].state = 0;
                D_003A9290[i].used = 0;
            }
            break;
        }
    }
    func_002EF680(D_003A9370);
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/AsyncRead", func_00238C40);
#endif

void func_00238D50(void) {
    int i;
    int busy;
    int id;

    for (i = 0; i < 8; i++) {
        func_002EF6A0(D_003A9370);
        if (D_003A9290[i].used == 1 && (D_003A9290[i].state == 2 || D_003A9290[i].state == 4)) {
            busy = 1;
            id = D_003A9290[i].id;
        } else {
            busy = 0;
            id = -1;
        }
        func_002EF680(D_003A9370);
        if (busy == 1) {
            func_00238AD8(id);
        }
    }
}

/* o IOP terminou uma leitura */
#ifdef NON_MATCHING /* o alvo guarda i e um ponteiro para state no laço */
void func_00238E20(int *msg) {
    int i;
    AsyncSlot *s;

    func_002EF6A0(D_003A9370);
    for (i = 0; i < 8; i++) {
        s = &D_003A9290[i];
        if (s->id == msg[0]) {
            D_003A9374--;
            switch (s->state) {
            case 2:
                s->state = 3;
                func_002EF680(s->sema);
                break;
            case 4:
                if (s->freeBuf == 1) {
                    func_00225DB8(s->buf);
                }
                s->used = 0;
                s->size = 0;
                s->state = 0;
                break;
            }
            break;
        }
    }
    func_002EF680(D_003A9370);
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/AsyncRead", func_00238E20);
#endif

int func_00238F08(void) {
    int i;

    for (i = 0; i < 8; i++) {
        if (D_003A9290[i].used == 0) {
            D_003A9290[i].used = 1;
            D_003A9290[i].state = 0;
            D_003A9290[i].size = 0;
            break;
        }
    }
    return i;
}
