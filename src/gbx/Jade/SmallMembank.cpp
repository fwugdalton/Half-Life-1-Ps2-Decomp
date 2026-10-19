/* Gbx/Jade: alocador de blocos pequenos (Small_*), cópia do membank com trava */

#include "common.h"

/* mesmo cabeçalho do Membank.cpp */
typedef struct MemoryTrack {
    int check;
    int size;
    struct MemoryTrack *next;
    struct MemoryTrack *prev;
    struct MemoryTrack *nextFree;
    struct MemoryTrack *prevFree;
    char tag;
    char pad19;
    short tagSize;
    unsigned char flags;            /* bit 0: do membank, bit 1: livre */
    char pad1D[3];
} MemoryTrack;

typedef struct {
    int poolSize;
    int x4;
    long allocated;
    MemoryTrack *start;
    char *end;
    int state;                      /* 0 ocupado, 1 pronto, 2 iniciando */
} Membank;

extern "C" {

void *func_00225B40(int size);
void func_00225860(void *dst, char c, int n);
void Mem_Free(void *p);
void func_002262D8(void);
int func_00226520(void *p);
void *func_00227580(int size);
void *func_00227630(int size, const char *tag);
void *func_002276E8(int size, const char *file, int line);
void *func_00227A50(void *p, int size, const char *tag);
void Small_Free(void *p);
void func_00227F58(int poolSize);
void *func_002280D8(int size);
void *func_002281F8(int size);
void func_00228218(MemoryTrack *h);
void func_002282A8(MemoryTrack *h);
void func_00228320(void *p);
MemoryTrack *func_00228380(int size, MemoryTrack *h);
void func_00228540(MemoryTrack *h);
void func_002286D8(int f);
void func_00228A18(int f);
int func_00228B20(MemoryTrack *h);
int func_00228B68(int size);
void func_00228BA8(const char *msg);
void Q_memcpy(void *dst, const void *src, int n);
void func_00228CF0(char *dst, const char *src);
int func_00228DC0(const char *s);
void func_00228FA0(int n, char *buf, int radix);
bool func_00225958(char c);
int FS_Open(const char *name, const char *mode);
void FS_Close(int f);
void func_00222500(int f, const char *fmt, ...);
int sprintf(char *buf, const char *fmt, ...);

extern int D_00328140;              /* semente da soma de verificação */
extern unsigned char D_00328144;    /* iniciado */
extern MemoryTrack *D_00328148;     /* início da lista de livres */
extern MemoryTrack *D_0032814C;     /* fim da lista de livres */
extern int D_00328150[32];          /* pilha de tags */
extern int D_003281D0;              /* topo da pilha de tags */
extern int D_003281D4;              /* trava ligada */
extern Membank D_003A6430;
extern const char D_003867D8[];     /* "Membank Corrupt" */

int func_00227498(int poolSize) {
    int r;

    if (!D_00328144) {
        D_00328144 = 1;
        r = 0x81030001;
        D_003A6430.state = 2;
        func_00227F58(poolSize);
    } else if (D_003A6430.state == 3) {
        r = 0x41030005;
    } else {
        r = 0x4103000F;
    }
    return r;
}

int func_00227510(void) {
    int r;

    if (D_00328144) {
        D_00328144 = 0;
        D_003A6430.state = 2;
        r = 0x81030001;
        Mem_Free(D_003A6430.start);
        D_003A6430.start = 0;
        D_003A6430.end = 0;
    } else {
        r = 0x41030010;
    }
    return r;
}

#ifdef NON_MATCHING /* estrutura dos desvios */
void *func_00227580(int size) {
    void *p;

    if (D_003281D4 == 1) {
        func_002262D8();
    }
    p = 0;
    size = func_00228B68(size);
    if (size != 0) {
        if (D_00328144) {
            p = 0;
            if (D_003A6430.state != 0) {
                p = func_002280D8(size);
            }
        } else {
            p = 0;
        }
    }
    if (D_003281D4 == 1) {
        func_002262D8();
    }
    return p;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Jade/SmallMembank", func_00227580);
#endif

void *func_00227630(int size, const char *tag) {
    void *p;

    if (D_003281D4 == 1) {
        func_002262D8();
    }
    p = 0;
    size = func_00228B68(size);
    if (size != 0 && D_00328144) {
        p = func_002280D8(size);
        if (!func_00226520(p)) {
            func_00228BA8("Damnit.. returning bad pntr");
        }
    }
    if (D_003281D4 == 1) {
        func_002262D8();
    }
    return p;
}

void *func_002276E8(int size, const char *file, int line) {
    return func_00225B40(size);
}

void *func_00227708(int size, int align) {
    char *p;
    char *q;

    p = (char *)func_00227580(size + align);
    if ((int)p % align == 0) {
        q = p + align;
    } else {
        q = p + align - (int)p % align;
    }
    q[-1] = q - p;
    return q;
}

void *func_00227770(int size, int align, const char *tag) {
    char *p;
    char *q;

    p = (char *)func_00227630(size + align, tag);
    if ((int)p % align == 0) {
        q = p + align;
    } else {
        q = p + align - (int)p % align;
    }
    q[-1] = q - p;
    return q;
}

void *func_002277D8(int size, int align, const char *file, int line) {
    char *p;
    char *q;

    p = (char *)func_002276E8(size + align, file, line);
    if ((int)p % align == 0) {
        q = p + align;
    } else {
        q = p + align - (int)p % align;
    }
    q[-1] = q - p;
    return q;
}

void func_00227848(void *p) {
    Small_Free((char *)p - ((unsigned char *)p)[-1]);
}

void *Small_Calloc(int n, int size) {
    void *p;
    int total;

    total = n * size;
    p = func_00227580(total);
    if (p) {
        func_00225860(p, 0, total);
    }
    return p;
}

void *func_002278B8(int n, int size, const char *tag) {
    void *p;
    int total;

    total = n * size;
    p = func_00227630(total, tag);
    if (p) {
        func_00225860(p, 0, total);
    }
    return p;
}

void *func_00227908(int n, int size, const char *file, int line) {
    void *p;
    int total;

    total = n * size;
    p = func_002276E8(total, file, line);
    if (p) {
        func_00225860(p, 0, total);
    }
    return p;
}

void *func_00227960(void *p, int size) {
    MemoryTrack *o;
    MemoryTrack *h;
    char *r;

    size = func_00228B68(size);
    if (p == 0) {
        r = (char *)func_00227580(size);
    } else {
        if (((unsigned char *)p)[-4] & 1) {
            o = (MemoryTrack *)p - 1;
            r = (char *)func_002280D8(size + o->tagSize);
            h = (MemoryTrack *)r - 1;
            h->tagSize = o->tagSize;
            h->size -= h->tagSize;
            Q_memcpy((char *)h + (h->size + 0x20), (char *)o + (o->size + 0x20), o->tagSize);
            Q_memcpy(r, p, o->size < h->size ? o->size : h->size);
            ((MemoryTrack *)r)[-1].check = func_00228B20(h);
        } else {
            r = 0;
        }
        Small_Free(p);
    }
    return r;
}

#ifdef NON_MATCHING /* registradores e ordem */
void *func_00227A50(void *p, int size, const char *tag) {
    MemoryTrack *o;
    MemoryTrack *h;
    char *r;
    int len;
    int n;

    size = func_00228B68(size);
    if (p == 0) {
        r = (char *)func_00227630(size, tag);
    } else {
        len = 0;
        if (tag) {
            len = func_00228DC0(tag);
            n = func_00228B68(len);
        } else {
            n = 0;
        }
        if (((unsigned char *)p)[-4] & 1) {
            o = (MemoryTrack *)p - 1;
            r = (char *)func_002280D8(size + n);
            h = (MemoryTrack *)r - 1;
            h->tagSize = len;
            h->size -= (short)len;
            Q_memcpy((char *)h + (h->size + 0x20), tag, len);
            Q_memcpy(r, p, o->size < h->size ? o->size : h->size);
            ((MemoryTrack *)r)[-1].check = func_00228B20(h);
        } else {
            r = 0;
        }
        Small_Free(p);
    }
    return r;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Jade/SmallMembank", func_00227A50);
#endif

#ifdef NON_MATCHING /* laço do nome do arquivo */
void *func_00227B70(void *p, int size, const char *file, int line) {
    char num[16];
    char tag[1000];
    int i;
    int start;

    func_00228FA0(line, num, 10);
    i = 0;
    start = 0;
    if (file) {
        while (file[i]) {
            tag[i - start] = file[i];
            if (file[i] == '\\' || file[i] == '/') {
                start = i + 1;
            }
            i++;
        }
    }
    func_00228CF0(&tag[i - start], num);
    return func_00227A50(p, size, tag);
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Jade/SmallMembank", func_00227B70);
#endif

void Small_Free(void *p) {
    if (p == 0) {
        return;
    }
    if (((unsigned char *)p)[-4] & 1) {
        if (D_003281D4 == 1) {
            func_002262D8();
        }
        if (D_00328144) {
            func_00228320(p);
        } else {
            func_00228BA8("Attempted to free memory (which was created inside the membank) after the membank was shutdown");
        }
        if (D_003281D4 == 1) {
            func_002262D8();
        }
    }
}

int func_00227CE0(int *out) {
    int r;

    if (out == 0) {
        r = 0x41030003;
    } else if (!D_00328144) {
        r = 0x41030010;
    } else {
        r = 0x81030001;
        *out = D_003A6430.allocated;
    }
    return r;
}

void func_00227D20(int tag) {
    D_00328150[D_003281D0] = tag;
}

void func_00227D40(void) {
    int i;

    i = D_003281D0;
    D_003281D0 = i + 1;
    D_00328150[i + 1] = D_00328150[i];
}

void func_00227D78(void) {
    D_003281D0--;
}

void func_00227D90(void) {
}

void func_00227D98(void) {
    D_003281D4 ^= 1;
}

int func_00227DB0(const char *name) {
    char buf[256];
    int f;
    int f2;
    int r;

    f = FS_Open(name, "w");
    if (f) {
        r = 0x81030001;
        if (D_00328144) {
            sprintf(buf, "raw_%s", name);
            f2 = FS_Open(buf, "w");
            func_002286D8(f);
            func_00228A18(f2);
            FS_Close(f2);
        }
        FS_Close(f);
    } else {
        r = 0x41030003;
    }
    return r;
}

#ifdef NON_MATCHING /* endereço do cabeçalho */
int func_00227E70(void *p, int *out) {
    int r;

    if (p == 0) {
        r = 0x81030001;
        *out = 0;
    } else if (((unsigned char *)p)[-4] & 1) {
        if (D_00328144) {
            r = 0x81030001;
            *out = ((MemoryTrack *)p)[-1].size;
        } else {
            r = 0x41030010;
            func_00228BA8("attempted to determine the size of a membank pntr, while the membank was not available");
            return r;
        }
    } else {
        r = 0x41030003;
    }
    return r;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Jade/SmallMembank", func_00227E70);
#endif

int func_00227EF8(void *p) {
    if ((char *)D_003A6430.start < (char *)p && (char *)p < D_003A6430.end) {
        return ((MemoryTrack *)p)[-1].check == func_00228B20((MemoryTrack *)p - 1);
    }
    return 0;
}

#ifdef NON_MATCHING /* ordem dos stores do cabeçalho */
void func_00227F58(int poolSize) {
    MemoryTrack *h;
    unsigned int all = 0xFFFFFFFF;

    D_003A6430.poolSize = poolSize;
    D_003A6430.allocated = 0;
    h = (MemoryTrack *)func_002281F8(poolSize);
    D_003A6430.state = 2;
    D_003A6430.end = (char *)h + poolSize;
    D_003A6430.start = h;
    h->flags = 0xFF;
    h->prev = 0;
    h->next = 0;
    h->flags = all;
    h->tagSize = 0;
    h->size = D_003A6430.poolSize - 0x20;
    h->nextFree = 0;
    h->prevFree = 0;
    D_00328148 = 0;
    D_0032814C = 0;
    func_00228218(h);
    h->check = func_00228B20(h);
    D_003A6430.state = 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Jade/SmallMembank", func_00227F58);
#endif

void func_00228020(void) {
    MemoryTrack *h;

    D_003A6430.state = 0;
    for (h = D_003A6430.start; h != 0; h = h->next) {
        if (h->check != func_00228B20(h)) {
            func_00228BA8(D_003867D8);
        }
    }
    for (h = D_00328148; h != 0; h = h->nextFree) {
        if (h->check != func_00228B20(h)) {
            func_00228BA8(D_003867D8);
        }
    }
    D_003A6430.state = 1;
}

#ifdef NON_MATCHING /* laços de busca */
void *func_002280D8(int size) {
    MemoryTrack *h;
    char *p;
    int tag;

    p = 0;
    D_003A6430.state = 0;
    tag = D_00328150[D_003281D0];
    if (tag == 0 || tag == 2) {
        h = D_00328148;
    next1:
        if (h == 0) {
            goto done;
        }
        if (h->size < size) {
            h = h->nextFree;
            goto next1;
        }
    } else {
        h = D_0032814C;
    next2:
        if (h == 0) {
            goto done;
        }
        if (h->size < size) {
            h = h->prevFree;
            goto next2;
        }
    }
    h = func_00228380(size, h);
    h->check = func_00228B20(h);
    p = (char *)(h + 1);
    if (h != 0) {
        D_003A6430.allocated += h->size;
        h->tag = D_00328150[D_003281D0];
    }
done:
    D_003A6430.state = 1;
    return p;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Jade/SmallMembank", func_002280D8);
#endif

void *func_002281F8(int size) {
    return func_00225B40(size);
}

void func_00228218(MemoryTrack *h) {
    MemoryTrack *p;

    for (p = h->prev; p != 0; p = p->prev) {
        if (p->flags & 2) {
            h->prevFree = p;
            p->nextFree = h;
            break;
        }
    }
    if (h < D_00328148 || D_00328148 == 0) {
        D_00328148 = h;
    }
    for (p = h->next; p != 0; p = p->next) {
        if (p->flags & 2) {
            h->nextFree = p;
            p->prevFree = h;
            break;
        }
    }
    if (D_0032814C < h || D_0032814C == 0) {
        D_0032814C = h;
    }
}

void func_002282A8(MemoryTrack *h) {
    if (h == D_00328148) {
        D_00328148 = h->nextFree;
        if (D_00328148) {
            D_00328148->prevFree = 0;
        }
    }
    if (h == D_0032814C) {
        D_0032814C = h->prevFree;
        if (D_0032814C) {
            D_0032814C->nextFree = 0;
        }
    }
    if (h->prevFree) {
        h->prevFree->nextFree = h->nextFree;
    }
    if (h->nextFree) {
        h->nextFree->prevFree = h->prevFree;
    }
    h->prevFree = 0;
    h->nextFree = 0;
}

void func_00228320(void *p) {
    MemoryTrack *h;

    h = (MemoryTrack *)p - 1;
    D_003A6430.state = 0;
    func_00228540(h);
    ((MemoryTrack *)p)[-1].check = func_00228B20(h);
    D_003A6430.state = 1;
}

#ifdef NON_MATCHING /* ordem dos stores */
MemoryTrack *func_00228380(int size, MemoryTrack *h) {
    MemoryTrack *r;
    MemoryTrack *n;
    int tag;
    unsigned int all = 0xFFFFFFFF;

    if (D_003281D4 == 1) {
        func_002262D8();
    }
    if (size + 0x20 < h->size) {
        tag = D_00328150[D_003281D0];
        if (tag == 0 || tag == 2) {
            n = (MemoryTrack *)((char *)h + size + 0x20);
            n->tagSize = 0;
            n->size = h->size - size - 0x20;
            n->prev = h;
            n->next = h->next;
            if (h->next) {
                h->next->prev = n;
            }
            r = h;
            n->nextFree = 0;
            n->flags = all;
            n->prevFree = 0;
            r->size = size;
            r->next = n;
            r->flags = 0xFD;
            func_002282A8(r);
            func_00228218(n);
            r->check = func_00228B20(r);
            n->check = func_00228B20(n);
            goto done;
        }
        h->size = h->size - size - 0x20;
        h->check = func_00228B20(h);
        r = (MemoryTrack *)((char *)h + (h->size + 0x20));
        r->tagSize = 0;
        r->prev = h;
        r->next = h->next;
        if (h->next) {
            h->next->prev = r;
        }
        r->nextFree = 0;
        r->prevFree = 0;
        h->next = r;
        r->size = size;
        r->flags = 0xFD;
    } else {
        r = h;
        r->flags = (r->flags | 1) & ~2;
        func_002282A8(r);
        r->tagSize = 0;
    }
    r->check = func_00228B20(r);
done:
    if (D_003281D4 == 1) {
        func_002262D8();
    }
    return r;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Jade/SmallMembank", func_00228380);
#endif

#ifdef NON_MATCHING /* junção de blocos livres */
void func_00228540(MemoryTrack *h) {
    MemoryTrack *p;

    if (D_003281D4 == 1) {
        func_002262D8();
    }
    if (h->check != func_00228B20(h)) {
        func_00228BA8(D_003867D8);
    }
    D_003A6430.allocated -= h->size + h->tagSize;
    p = h->prev;
    if (p != 0 && (p->flags & 2)) {
        func_002282A8(p);
        h->prev->next = h->next;
        if (h->next) {
            h->next->prev = h->prev;
        }
        h->prev->size += h->size + h->tagSize + 0x20;
        h = h->prev;
    }
    p = h->next;
    if (p != 0 && (p->flags & 2)) {
        func_002282A8(p);
        h->size += p->size + p->tagSize + 0x20;
        if (p->next) {
            p->next->prev = h;
        }
        h->next = h->next->next;
    }
    h->flags |= 2;
    h->check = func_00228B20(h);
    func_00228218(h);
    h->size += h->tagSize;
    h->tagSize = 0;
    h->check = func_00228B20(h);
    if (D_003281D4 == 1) {
        func_002262D8();
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Jade/SmallMembank", func_00228540);
#endif

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/SmallMembank", D_003867D8);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/SmallMembank", func_002286D8);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/SmallMembank", D_00386888);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/SmallMembank", D_00386898);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/SmallMembank", D_003868A8);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/SmallMembank", D_003868B8);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/SmallMembank", D_003868C8);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/SmallMembank", D_00386920);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/SmallMembank", D_00386928);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/SmallMembank", D_00386930);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/SmallMembank", D_00386938);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/SmallMembank", D_00386940);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/SmallMembank", D_003869A0);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/SmallMembank", D_003869D0);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/SmallMembank", D_00386A38);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/SmallMembank", D_00386AA8);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/SmallMembank", D_00386AE8);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/SmallMembank", func_00228A18);

#ifdef NON_MATCHING /* registradores da soma */
int func_00228B20(MemoryTrack *h) {
    int c;

    c = D_00328140 - 0x20; c -= (int)h; c -= h->flags; c -= h->size; c -= h->tagSize;
    if (h->tagSize > 0) {
        c -= ((char *)h)[h->size + 0x20];
    }
    return c;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Jade/SmallMembank", func_00228B20);
#endif

int func_00228B68(int size) {
    if (size > 0) {
        if (size < 16) {
            size = 16;
        } else if (size & 0xF) {
            size = size / 16 * 16 + 16;
        }
    }
    return size;
}

void func_00228BA8(const char *msg) {
}

}
