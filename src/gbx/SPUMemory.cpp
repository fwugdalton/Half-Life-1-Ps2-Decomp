/* Gbx: alocador da memória do SPU2 (first-fit, blocos de 64 bytes, lista de livres) */

#include "common.h"

extern "C" {
#include "engine/cmd.h"

typedef void *FileHandle_t;

/* um trecho da memória do SPU2 (struct SPUMemoryTrack, 0x1C bytes) */
struct SPUMemBlock {
    int addr;
    unsigned int size;
    SPUMemBlock *next;
    SPUMemBlock *prev;
    SPUMemBlock *nextFree;
    SPUMemBlock *prevFree;
    unsigned char flags;    /* 1 = em uso, 2 = livre */
};

struct SPUMemBank {
    int size;
    int pad;
    unsigned long allocated;
    SPUMemBlock *first;
    unsigned int start;
    unsigned int end;
    int lock;
};

/* bloco devolvido ao chamador (endereço no SPU2 e o trecho) */
struct SpuBlock {
    unsigned int addr;
    SPUMemBlock *blk;
};

FileHandle_t FS_Open(char *name, char *mode);
void FS_Close(FileHandle_t f);
void func_00222500(FileHandle_t f, char *fmt, ...);
SpuBlock func_00232210(unsigned int size);
int func_00232188(SpuBlock *b);
void func_002321D0(char *name);
void func_002322E0(SpuBlock *b);
void func_00232318(unsigned int size, SPUMemBlock *blk);
void func_002323D8(SPUMemBlock *blk);
void func_002324D0(SPUMemBlock *blk);
void func_002324F0(SPUMemBlock *blk);
void func_00232540(FileHandle_t f);
unsigned int func_00232710(unsigned int size);

extern int D_00328984;
extern SPUMemBlock *D_00328988;
extern SPUMemBank D_003A7F98;

void func_00231FD0(void) {
    SPUMemBlock *blk;
    unsigned int all;

    D_003A7F98.size = 0x100000;
    D_003A7F98.lock = 2;
    D_003A7F98.allocated = 0;
    blk = (SPUMemBlock *)Mem_DbgMalloc(0x1C, "Gbx/SPUMemory.cpp", 0x73);
    D_003A7F98.lock = 2;
    D_003A7F98.start = 0x15040;
    D_003A7F98.end = D_003A7F98.size + 0x15040;
    D_003A7F98.first = blk;
    all = 0xFFFFFFFF;
    blk->flags = 0xFF;
    blk->addr = D_003A7F98.start;
    blk->size = D_003A7F98.size;
    blk->flags = all;
    blk->prev = 0;
    blk->next = 0;
    blk->nextFree = 0;
    blk->prevFree = 0;
    func_002324D0(blk);
    D_003A7F98.lock = 1;
    D_00328984 = 1;
}

void func_00232098(void) {
}

SpuBlock func_002320A0(unsigned int size) {
    SpuBlock r;
    unsigned int n;

    n = func_00232710(size);
    if (n == 0) {
        r.addr = 0;
        r.blk = 0;
    } else {
        r = func_00232210(n);
        if (func_00232188(&r) == 0) {
            func_002321D0("SPUOutOfMemoryReport.txt");
        }
    }
    return r;
}

void func_00232150(SpuBlock *b) {
    if (b->addr != 0 && (b->blk->flags & 1)) {
        func_002322E0(b);
    }
}

int func_00232188(SpuBlock *b) {
    int ret;

    ret = 0;
    if (!(b->addr < D_003A7F98.start)) {
        ret = b->addr < D_003A7F98.end;
    }
    return ret;
}

int func_002321B8(void) {
    return D_003A7F98.allocated;
}

void func_002321D0(char *name) {
    FileHandle_t f;

    f = FS_Open(name, "w");
    func_00232540(f);
    FS_Close(f);
}

#ifdef NON_MATCHING
/* só o agendamento da soma em allocated (o original calcula &D_003A7F98 depois do lwu) */
SpuBlock func_00232210(unsigned int size) {
    SPUMemBlock *blk;
    SpuBlock r;

    D_003A7F98.lock = 0;
    blk = D_00328988;
    r.addr = 0;
    r.blk = 0;
    while (blk != 0) {
        if (blk->size >= size) {
            func_00232318(size, blk);
            r.addr = blk->addr;
            r.blk = blk;
            D_003A7F98.allocated = blk->size + D_003A7F98.allocated;
            break;
        }
        blk = blk->nextFree;
    }
    D_003A7F98.lock = 1;
    return r;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/SPUMemory", func_00232210);
#endif

void func_002322E0(SpuBlock *b) {
    D_003A7F98.lock = 0;
    func_002323D8(b->blk);
    D_003A7F98.lock = 1;
}

void func_00232318(unsigned int size, SPUMemBlock *blk) {
    SPUMemBlock *n;
    unsigned int all;

    if (size < blk->size) {
        n = (SPUMemBlock *)Mem_DbgMalloc(0x1C, "Gbx/SPUMemory.cpp", 0x124);
        all = 0xFFFFFFFF;
        n->flags = all;
        n->addr = blk->addr + size;
        n->size = blk->size - size;
        n->prev = blk;
        n->next = blk->next;
        n->nextFree = 0;
        n->prevFree = 0;
        func_002324D0(n);
        blk->next = n;
        if (n->next != 0) {
            n->next->prev = n;
        }
        blk->size = size;
    }
    blk->flags = 0xFD;
    func_002324F0(blk);
}

void func_002323D8(SPUMemBlock *blk) {
    D_003A7F98.allocated -= blk->size;
    if (blk->prev != 0 && (blk->prev->flags & 2)) {
        func_002324F0(blk->prev);
        blk->prev->next = blk->next;
        if (blk->next != 0) {
            blk->next->prev = blk->prev;
        }
        blk->prev->size += blk->size;
        blk = blk->prev;
    }
    if (blk->next != 0 && (blk->next->flags & 2)) {
        func_002324F0(blk->next);
        blk->size += blk->next->size;
        if (blk->next->next != 0) {
            blk->next->next->prev = blk;
        }
        blk->next = blk->next->next;
    }
    blk->flags |= 2;
    func_002324D0(blk);
}

void func_002324D0(SPUMemBlock *blk) {
    blk->nextFree = D_00328988;
    if (D_00328988 != 0) {
        D_00328988->prevFree = blk;
    }
    D_00328988 = blk;
}

void func_002324F0(SPUMemBlock *blk) {
    if (blk == D_00328988) {
        D_00328988 = blk->nextFree;
        blk->prevFree = 0;
    }
    if (blk->prevFree != 0) {
        blk->prevFree->nextFree = blk->nextFree;
    }
    if (blk->nextFree != 0) {
        blk->nextFree->prevFree = blk->prevFree;
    }
    blk->prevFree = 0;
    blk->nextFree = 0;
}

#ifdef NON_MATCHING
/* registradores trocados (nodes/freeNodes em s6/s7, tamanho em v0 em vez de v1) */
void func_00232540(FileHandle_t f) {
    SPUMemBlock *blk;
    int leaks;
    int freeBytes;
    int leakBytes;
    int freeNodes;
    int accounted;
    int nodes;

    leaks = 0;
    freeNodes = 0;
    leakBytes = 0;
    freeBytes = 0;
    nodes = 0;
    accounted = 0;
    func_00222500(f, "---------------------------------------------------------------------------\r\n");
    func_00222500(f, "BANK:  PoolSize[%d]  Allocated memory[%d]  sizeof(struct SPUMemoryTrack)[%d]\r\n",
                  D_003A7F98.size, D_003A7F98.allocated, sizeof(SPUMemBlock));
    for (blk = D_003A7F98.first; blk != 0; blk = blk->next) {
        nodes++;
        if (!(blk->flags & 2)) {
            func_00222500(f, "  Memory leak:  Size[%7d]", blk->size);
            leaks++;
            accounted += blk->size;
            func_00222500(f, "\r\n");
            leakBytes += blk->size;
        } else {
            func_00222500(f, "  Free Memory:  Size[%7d]\r\n", blk->size);
            accounted += blk->size;
        }
        if (blk->flags & 2) {
            freeBytes += blk->size;
        }
    }
    blk = D_00328988;
    func_00222500(f, "\r\n--Free Memory List (gSPUMembankFreeList)--\r\n");
    for (; blk != 0; blk = blk->nextFree) {
        func_00222500(f, "  Free Memory:  Size[%7d] NextFree[%p]  LastFree[%p]]\r\n", blk->size, blk->nextFree, blk->prevFree);
        freeNodes++;
    }
    func_00222500(f, "----Total for pool [%d] allocations, taking up [%d] bytes.  Unallocated[%d].\r\n", leaks, leakBytes, freeBytes);
    func_00222500(f, "----Cardinality of Free List[%d]  Cardinality of Membank[%d]\r\n", freeNodes, nodes);
    func_00222500(f, "------ACCOUNTED FOR[%d]\r\n\r\n", accounted);
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/SPUMemory", func_00232540);
#endif

unsigned int func_00232710(unsigned int size) {
    if (size != 0) {
        if (size < 0x40) {
            size = 0x40;
        } else if (size % 0x40) {
            size = size + (0x40 - size % 0x40);
        }
    }
    return size;
}
}
