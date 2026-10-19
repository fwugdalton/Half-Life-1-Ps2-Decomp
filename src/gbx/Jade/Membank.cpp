/* Gbx/Jade: alocador de memória (membank) com tags */

#include "common.h"

/* cabeçalho de um bloco do membank (fica antes do ponteiro devolvido) */
typedef struct MemoryTrack {
    int check;                      /* soma de verificação */
    int size;
    struct MemoryTrack *next;       /* blocos vizinhos na memória */
    struct MemoryTrack *prev;
    struct MemoryTrack *nextFree;   /* lista de livres */
    struct MemoryTrack *prevFree;
    char tag;                       /* DONT_CARE, LONG_LIVED, SHORT_LIVED */
    char pad19;
    short tagSize;                  /* nome guardado depois dos dados */
    unsigned char flags;            /* bit 0: do membank, bit 1: livre */
    char pad1D[3];
} MemoryTrack;

/* cabeçalho de uma alocação do sistema (fora do membank) */
typedef struct SystemMemoryTrack {
    int size;
    short tagSize;
    short pad6;
    struct SystemMemoryTrack *next;
    struct SystemMemoryTrack *prev;
    int pad10[3];
    unsigned char flags;
    char pad1D[3];
} SystemMemoryTrack;

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
void *func_00225BD8(int size, const char *tag);
void *Mem_DbgMalloc(int size, const char *file, int line);
void func_00225860(void *dst, char c, int n);
void *func_00225FB0(void *p, int size, const char *tag);
void Mem_Free(void *p);
void Mem_SetTag(int tag);
void Mem_PushTag(void);
void Mem_PopTag(void);
void func_002262D8(void);
void func_00226E98(int f);
void func_00226FC0(int f);
void func_00227300(int f);
int func_00226520(void *p);
int func_00226588(void);
void func_00226600(int poolSize);
void *func_00226788(int size);
void *func_002268C0(int size);
void func_00226930(MemoryTrack *h, MemoryTrack *prevFree, MemoryTrack *nextFree);
void func_00226988(MemoryTrack *h);
void func_00226A18(MemoryTrack *h);
void func_00226A90(void *p);
void func_00226AF0(void *p);
MemoryTrack *func_00226B60(int size, MemoryTrack *h);
void func_00226D00(MemoryTrack *h);
int func_00226E68(void *p);
int func_00227408(MemoryTrack *h);
int func_00227450(int size);
void func_00227490(const char *msg);
void Q_memcpy(void *dst, const void *src, int n);
void func_00228CF0(char *dst, const char *src);
int func_00228DC0(const char *s);
void func_00228FA0(int n, char *buf, int radix);
bool func_00225958(char c);
void func_002292B8(int size);
void *func_00229310(int size);
void func_00229350(void *p);
int FS_Open(const char *name, const char *mode);
void FS_Close(int f);
void func_00222500(int f, const char *fmt, ...);
int sprintf(char *buf, const char *fmt, ...);

extern int D_00328098;              /* semente da soma de verificação */
extern unsigned char D_0032809C;    /* membank iniciado */
extern int D_003280A0;              /* arquivo do relatório */
extern SystemMemoryTrack *D_003280A4; /* alocações do sistema */
extern MemoryTrack *D_003280A8;     /* gMembankFreeListHead */
extern MemoryTrack *D_003280AC;     /* fim da lista de livres */
extern int D_003280B0;              /* gNumSysAllocs */
extern int D_003280B8[32];          /* pilha de tags */
extern int D_00328138;              /* topo da pilha de tags */
extern int D_0032813C;
extern Membank D_003A6410;
extern const char D_003862C0[]; /* "Membank Corrupt" */

}

void *operator new(unsigned int size) {
    return func_00225B40(size);
}

void operator delete(void *p) {
    Mem_Free(p);
}

extern "C" {

int func_00225A58(int poolSize) {
    int r;

    if (!D_0032809C) {
        D_0032809C = 1;
        r = 0x81030001;
        D_003A6410.state = 2;
        func_00226600(poolSize);
    } else if (D_003A6410.state == 3) {
        r = 0x41030005;
    } else {
        r = 0x4103000F;
    }
    return r;
}

int func_00225AD0(void) {
    int r;

    if (D_0032809C) {
        D_0032809C = 0;
        D_003A6410.state = 2;
        r = 0x81030001;
        Mem_Free(D_003A6410.start);
        D_003A6410.start = 0;
        D_003A6410.end = 0;
    } else {
        r = 0x41030010;
    }
    return r;
}

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_003861D8);

#ifdef NON_MATCHING /* registrador do tamanho */
void *func_00225B40(int size) {
    void *p;

    p = 0;
    func_002262D8();
    size = func_00227450(size);
    if (size != 0) {
        if (D_0032809C) {
            p = 0;
            if (D_003A6410.state != 0) {
                p = func_00226788(size);
                if (!func_00226520(p)) {
                    func_00227490("Damnit.. returning bad pntr");
                }
            }
        } else {
            p = 0;
        }
    }
    func_002262D8();
    return p;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00225B40);
#endif

#ifdef NON_MATCHING /* o tamanho fica em s0 no alvo */
void *func_00225BD8(int size, const char *tag) {
    void *p;

    p = 0;
    func_002262D8();
    size = func_00227450(size);
    if (size != 0 && D_0032809C) {
        p = func_00226788(size);
        if (!func_00226520(p)) {
            func_00227490("Damnit.. returning bad pntr");
        }
    }
    func_002262D8();
    return p;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00225BD8);
#endif

void *Mem_DbgMalloc(int size, const char *file, int line) {
    return func_00225B40(size);
}

void *func_00225C78(int size, int align) {
    char *p;
    char *q;

    p = (char *)func_00225B40(size + align);
    if ((int)p % align == 0) {
        q = p + align;
    } else {
        q = p + align - (int)p % align;
    }
    q[-1] = q - p;
    return q;
}

void *func_00225CE0(int size, int align, const char *tag) {
    char *p;
    char *q;

    p = (char *)func_00225BD8(size + align, tag);
    if ((int)p % align == 0) {
        q = p + align;
    } else {
        q = p + align - (int)p % align;
    }
    q[-1] = q - p;
    return q;
}

void *func_00225D48(int size, int align, const char *file, int line) {
    char *p;
    char *q;

    p = (char *)Mem_DbgMalloc(size + align, file, line);
    if ((int)p % align == 0) {
        q = p + align;
    } else {
        q = p + align - (int)p % align;
    }
    q[-1] = q - p;
    return q;
}

void func_00225DB8(void *p) {
    Mem_Free((char *)p - ((unsigned char *)p)[-1]);
}

void *func_00225DD8(int n, int size) {
    void *p;
    int total;

    total = n * size;
    p = func_00225B40(total);
    func_00225860(p, 0, total);
    return p;
}

void *func_00225E20(int n, int size, const char *tag) {
    void *p;
    int total;

    total = n * size;
    p = func_00225BD8(total, tag);
    func_00225860(p, 0, total);
    return p;
}

void *Mem_DbgCalloc(int n, int size, const char *file, int line) {
    void *p;
    int total;

    total = n * size;
    p = Mem_DbgMalloc(total, file, line);
    func_00225860(p, 0, total);
    return p;
}

void *func_00225EC0(void *p, int size) {
    MemoryTrack *o;
    MemoryTrack *h;
    char *r;

    size = func_00227450(size);
    if (p == 0) {
        r = (char *)func_00225B40(size);
    } else {
        if (((unsigned char *)p)[-4] & 1) {
            o = (MemoryTrack *)p - 1;
            r = (char *)func_00226788(size + o->tagSize);
            h = (MemoryTrack *)r - 1;
            h->tagSize = o->tagSize;
            h->size -= h->tagSize;
            Q_memcpy((char *)h + (h->size + 0x20), (char *)o + (o->size + 0x20), o->tagSize);
            Q_memcpy(r, p, o->size < h->size ? o->size : h->size);
            ((MemoryTrack *)r)[-1].check = func_00227408(h);
        } else {
            r = 0;
        }
        Mem_Free(p);
    }
    return r;
}

#ifdef NON_MATCHING /* registradores e ordem */
void *func_00225FB0(void *p, int size, const char *tag) {
    MemoryTrack *o;
    MemoryTrack *h;
    char *r;
    int len;
    int n;

    size = func_00227450(size);
    if (p == 0) {
        r = (char *)func_00225BD8(size, tag);
    } else {
        len = 0;
        if (tag) {
            len = func_00228DC0(tag);
            n = func_00227450(len);
        } else {
            n = 0;
        }
        if (((unsigned char *)p)[-4] & 1) {
            o = (MemoryTrack *)p - 1;
            r = (char *)func_00226788(size + n);
            h = (MemoryTrack *)r - 1;
            h->tagSize = len;
            h->size -= (short)len;
            Q_memcpy((char *)h + (h->size + 0x20), tag, len);
            Q_memcpy(r, p, o->size < h->size ? o->size : h->size);
            ((MemoryTrack *)r)[-1].check = func_00227408(h);
        } else {
            r = 0;
        }
        Mem_Free(p);
    }
    return r;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00225FB0);
#endif

#ifdef NON_MATCHING /* laço do nome do arquivo */
void *func_002260D0(void *p, int size, const char *file, int line) {
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
    return func_00225FB0(p, size, tag);
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_002260D0);
#endif

void Mem_Free(void *p) {
    if (p == 0) {
        return;
    }
    if (((unsigned char *)p)[-4] & 1) {
        func_002262D8();
        if (D_0032809C) {
            func_00226A90(p);
        } else {
            func_00227490("Attempted to free memory (which was created inside the membank) after the membank was shutdown");
        }
        func_002262D8();
    } else {
        func_00226AF0(p);
    }
}

int func_00226228(int *out) {
    int r;

    if (out == 0) {
        r = 0x41030003;
    } else if (!D_0032809C) {
        r = 0x41030010;
    } else {
        r = 0x81030001;
        *out = D_003A6410.allocated;
    }
    return r;
}

void Mem_SetTag(int tag) {
    D_003280B8[D_00328138] = tag;
}

void Mem_PushTag(void) {
    int i;

    i = D_00328138;
    D_00328138 = i + 1;
    D_003280B8[i + 1] = D_003280B8[i];
}

void Mem_PopTag(void) {
    D_00328138--;
}

void func_002262D8(void) {
}

void func_002262E0(int on) {
    if (on == 1) {
        D_0032813C = on;
    } else {
        D_0032813C = 0;
    }
}

void func_00226300(void) {
    D_0032813C ^= 1;
}

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_00386258);

#ifdef NON_MATCHING /* ordem dos blocos */
int func_00226318(const char *name) {
    char buf[256];
    int f;
    int r;

    f = FS_Open(name, "w");
    if (f) {
        if (D_0032809C) {
            func_00226FC0(f);
            func_00226E98(f);
        }
        r = 0x81030001;
        FS_Close(f);
    } else {
        r = 0x41030003;
    }
    if (r == 0x81030001) {
        sprintf(buf, "raw_%s", name);
        f = FS_Open(buf, "w");
        if (f) {
            if (D_0032809C) {
                func_00227300(f);
            }
            FS_Close(f);
        } else {
            r = 0x41030003;
        }
    }
    return r;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00226318);
#endif

int func_00226410(void) {
    return sizeof(MemoryTrack);
}

#ifdef NON_MATCHING /* ordem dos blocos */
int func_00226418(const char *name) {
    int r;

    if (name != 0) {
        r = 0x41030010;
        if (D_0032809C) {
            Mem_PushTag();
            r = 0x81030001;
            Mem_SetTag(1);
            D_003280A0 = FS_Open(name, "w");
            Mem_PopTag();
        }
    } else {
        r = 0x41030003;
    }
    return r;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00226418);
#endif

#ifdef NON_MATCHING /* ordem dos blocos */
int func_002264A0(void *p, int *out) {
    int r;
    int size;

    if (p != 0) {
        if (((unsigned char *)p)[-4] & 1) {
            if (D_0032809C) {
                size = ((MemoryTrack *)p)[-1].size;
            } else {
                r = 0x41030010;
                func_00227490("attempted to determine the size of a membank pntr, while the membank was not available");
                return r;
            }
        } else {
            size = ((SystemMemoryTrack *)p)[-1].size;
        }
        r = 0x81030001;
        *out = size;
    } else {
        r = 0x81030001;
        *out = 0;
    }
    return r;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_002264A0);
#endif

int func_00226520(void *p) {
    if ((char *)D_003A6410.start < (char *)p && (char *)p < D_003A6410.end) {
        return ((MemoryTrack *)p)[-1].check == func_00227408((MemoryTrack *)p - 1);
    }
    return func_00226E68(p);
}

int func_00226588(void) {
    int r;

    if (D_003280A0) {
        r = 0x81030001;
        if (D_0032809C) {
            func_00226FC0(D_003280A0);
            func_00227300(D_003280A0);
        }
        func_00226E98(D_003280A0);
        FS_Close(D_003280A0);
        D_003280A0 = 0;
    } else {
        r = 0x41030003;
    }
    return r;
}

#ifdef NON_MATCHING /* ordem dos stores do cabeçalho */
void func_00226600(int poolSize) {
    MemoryTrack *h;
    unsigned int all = 0xFFFFFFFF;

    func_002292B8(poolSize);
    D_003A6410.poolSize = poolSize;
    D_003A6410.allocated = 0;
    h = (MemoryTrack *)func_002268C0(poolSize);
    D_003A6410.state = 2;
    D_003A6410.end = (char *)h + poolSize;
    D_003A6410.start = h;
    h->flags = 0xFF;
    h->prev = 0;
    h->next = 0;
    h->flags = all;
    h->tagSize = 0;
    h->size = D_003A6410.poolSize - 0x20;
    h->nextFree = 0;
    h->prevFree = 0;
    D_003280A8 = 0;
    D_003280AC = 0;
    func_00226988(h);
    h->check = func_00227408(h);
    D_003A6410.state = 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00226600);
#endif

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_003862C0);

void func_002266D0(void) {
    MemoryTrack *h;

    D_003A6410.state = 0;
    for (h = D_003A6410.start; h != 0; h = h->next) {
        if (h->check != func_00227408(h)) {
            func_00227490(D_003862C0);
        }
    }
    for (h = D_003280A8; h != 0; h = h->nextFree) {
        if (h->check != func_00227408(h)) {
            func_00227490(D_003862C0);
        }
    }
    D_003A6410.state = 1;
}

#ifdef NON_MATCHING /* laços de busca */
void *func_00226788(int size) {
    MemoryTrack *h;
    char *p;
    int tag;

    p = 0;
    D_003A6410.state = 0;
    tag = D_003280B8[D_00328138];
    if (tag == 0 || tag == 2) {
        for (h = D_003280A8; h != 0; h = h->nextFree) {
            if (h->size >= size) {
                goto found;
            }
        }
    } else {
        for (h = D_003280AC; h != 0; h = h->prevFree) {
            if (h->size >= size) {
                goto found;
            }
        }
    }
    goto done;
found:
    h = func_00226B60(size, h);
    p = (char *)(h + 1);
    h->check = func_00227408(h);
    if (h != 0) {
        D_003A6410.allocated += h->size;
        h->tag = D_003280B8[D_00328138];
    }
done:
    D_003A6410.state = 1;
    if (p == 0) {
        func_00226588();
    }
    return p;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00226788);
#endif

void *func_002268C0(int size) {
    SystemMemoryTrack *h;

    h = (SystemMemoryTrack *)func_00229310(size + 0x20);
    h->size = size;
    h->flags = 0xFC;
    h->tagSize = 0;
    h->prev = 0;
    h->next = D_003280A4;
    if (D_003280A4) {
        D_003280A4->prev = h;
    }
    D_003280A4 = h;
    D_003280B0++;
    return h + 1;
}

void func_00226930(MemoryTrack *h, MemoryTrack *prevFree, MemoryTrack *nextFree) {
    h->prevFree = prevFree;
    if (prevFree) {
        prevFree->nextFree = h;
    }
    if (h < D_003280A8 || D_003280A8 == 0) {
        D_003280A8 = h;
    }
    h->nextFree = nextFree;
    if (nextFree) {
        nextFree->prevFree = h;
    }
    if (D_003280AC < h || D_003280AC == 0) {
        D_003280AC = h;
    }
}

void func_00226988(MemoryTrack *h) {
    MemoryTrack *p;

    for (p = h->prev; p != 0; p = p->prev) {
        if (p->flags & 2) {
            h->prevFree = p;
            p->nextFree = h;
            break;
        }
    }
    if (h < D_003280A8 || D_003280A8 == 0) {
        D_003280A8 = h;
    }
    for (p = h->next; p != 0; p = p->next) {
        if (p->flags & 2) {
            h->nextFree = p;
            p->prevFree = h;
            break;
        }
    }
    if (D_003280AC < h || D_003280AC == 0) {
        D_003280AC = h;
    }
}

void func_00226A18(MemoryTrack *h) {
    if (h == D_003280A8) {
        D_003280A8 = h->nextFree;
        if (D_003280A8) {
            D_003280A8->prevFree = 0;
        }
    }
    if (h == D_003280AC) {
        D_003280AC = h->prevFree;
        if (D_003280AC) {
            D_003280AC->nextFree = 0;
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

void func_00226A90(void *p) {
    MemoryTrack *h;

    h = (MemoryTrack *)p - 1;
    D_003A6410.state = 0;
    func_00226D00(h);
    ((MemoryTrack *)p)[-1].check = func_00227408(h);
    D_003A6410.state = 1;
}

void func_00226AF0(void *p) {
    SystemMemoryTrack *h;

    h = (SystemMemoryTrack *)p - 1;
    if (h == D_003280A4) {
        D_003280A4 = h->next;
    }
    if (h->prev) {
        h->prev->next = h->next;
    }
    if (h->next) {
        h->next->prev = h->prev;
    }
    func_00229350(h);
    D_003280B0--;
}

#ifdef NON_MATCHING /* ordem dos stores */
MemoryTrack *func_00226B60(int size, MemoryTrack *h) {
    MemoryTrack *r;
    MemoryTrack *n;
    MemoryTrack *prevFree;
    MemoryTrack *nextFree;
    int tag;
    unsigned int all = 0xFFFFFFFF;

    func_002262D8();
    if (size + 0x20 < h->size) {
        tag = D_003280B8[D_00328138];
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
            prevFree = r->prevFree;
            nextFree = r->nextFree;
            func_00226A18(r);
            func_00226930(n, prevFree, nextFree);
            r->check = func_00227408(r);
            n->check = func_00227408(n);
            func_002262D8();
            return r;
        }
        h->size = h->size - size - 0x20;
        h->check = func_00227408(h);
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
        func_00226A18(r);
        r->tagSize = 0;
    }
    r->check = func_00227408(r);
    func_002262D8();
    return r;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00226B60);
#endif

#ifdef NON_MATCHING /* junção de blocos livres */
void func_00226D00(MemoryTrack *h) {
    MemoryTrack *p;

    func_002262D8();
    if (h->check != func_00227408(h)) {
        func_00227490("Membank Corrupt");
    }
    D_003A6410.allocated -= h->size + h->tagSize;
    p = h->prev;
    if (p != 0 && (p->flags & 2)) {
        func_00226A18(p);
        h->prev->next = h->next;
        if (h->next) {
            h->next->prev = h->prev;
        }
        h->prev->size += h->size + h->tagSize + 0x20;
        h = h->prev;
    }
    p = h->next;
    if (p != 0 && (p->flags & 2)) {
        func_00226A18(p);
        h->size += p->size + p->tagSize + 0x20;
        if (p->next) {
            p->next->prev = h;
        }
        h->next = h->next->next;
    }
    h->flags |= 2;
    h->check = func_00227408(h);
    func_00226988(h);
    h->size += h->tagSize;
    h->tagSize = 0;
    h->check = func_00227408(h);
    func_002262D8();
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00226D00);
#endif

int func_00226E68(void *p) {
    SystemMemoryTrack *h;
    int found;

    found = 0;
    for (h = D_003280A4; h != 0; h = h->next) {
        if ((void *)(h + 1) == p) {
            found = 1;
            break;
        }
    }
    return found;
}

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_003862D0);

#ifdef NON_MATCHING /* laço do relatório */
void func_00226E98(int f) {
    char buf[1000];
    SystemMemoryTrack *h;
    int total;
    int size;

    h = D_003280A4;
    total = 0;
    func_00222500(f, "---------------------------------------------------------------------------\r\n");
    func_00222500(f, "SYSTEM:  gNumSysAllocs[%d]  sizeof(struct SystemMemoryTrack)[%d]\r\n", D_003280B0,
                  sizeof(SystemMemoryTrack));
    for (; h != 0; h = h->next) {
        if (h->tagSize > 0) {
            Q_memcpy(buf, (char *)h + (h->size + 0x20), h->tagSize);
            buf[h->tagSize] = 0;
            size = h->size + h->tagSize + 0x20;
            total += size;
            func_00222500(f, "System Level Memory leak:  Size[%7d]  Tag[%s]  TotalTrackSize[%7d]\r\n", h->size, buf,
                          size);
        } else {
            size = h->size + h->tagSize + 0x20;
            total += size;
            func_00222500(f, "System Level Memory leak:  Size[%7d]  Undescribed  TotalTrackSize[%7d]\r\n", h->size,
                          size);
        }
    }
    func_00222500(f, "------ACCOUNTED FOR[%d]\r\n\r\n", total);
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00226E98);
#endif

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_00386400);

#ifdef NON_MATCHING /* contadores do relatório */
void func_00226FC0(int f) {
    char buf[1000];
    int numAllocs;
    int allocBytes;
    int tagBytes;
    int numFree;
    int numTracks;
    int unallocated;
    int numTags;
    int total;
    int size;
    int i;
    char c;
    const char *hint;
    MemoryTrack *h;

    unallocated = 0;
    numTags = 0;
    total = 0;
    func_00222500(f, "---------------------------------------------------------------------------\r\n");
    numAllocs = 0;
    func_00222500(f, "BANK:  PoolSize[%d]  Allocated memory[%d]  sizeof(struct MemoryTrack)[%d]\r\n",
                  D_003A6410.poolSize, D_003A6410.allocated, sizeof(MemoryTrack));
    allocBytes = 0;
    h = D_003A6410.start;
    tagBytes = 0;
    numFree = 0;
    numTracks = 0;
    for (; h != 0; h = h->next) {
        numTracks++;
        if (!(h->flags & 2)) {
            numAllocs++;
            if (h->tagSize > 0) {
                numTags++;
                Q_memcpy(buf, (char *)h + (h->size + 0x20), h->tagSize);
                buf[h->tagSize] = 0;
                tagBytes += h->tagSize;
            } else {
                func_00228CF0(buf, "UNDESCRIBED");
            }
            size = h->size + h->tagSize + 0x20;
            if (h->tag == 0) {
                hint = "DONT_CARE";
            } else if (h->tag == 2) {
                hint = "SHORT_LIVED";
            } else {
                hint = "LONG_LIVED";
            }
            func_00222500(f, "  Memory leak:  Size[%7d]  TagSize[%7d]  Tag[%25s]  TotalTrackSize[%7d]  Hint[%s]",
                          h->size, h->tagSize, buf, size, hint);
            total += size;
            if (h->size < 100) {
                func_00222500(f, "  Data[");
                for (i = 0; i < h->size - 1; i++) {
                    c = ((char *)(h + 1))[i];
                    if (!func_00225958(c) && c != ' ') {
                        break;
                    }
                    func_00222500(f, "%c", c);
                }
                func_00222500(f, "]");
            }
            func_00222500(f, "\r\n");
            allocBytes += h->size;
        } else {
            size = h->size + h->tagSize + 0x20;
            total += size;
            func_00222500(f,
                          "  Free Memory:  Size[%7d]  TagSize[%7d]                                  "
                          "TotalTrackSize[%7d]\r\n",
                          h->size, h->tagSize, size);
        }
        if (h->flags & 2) {
            unallocated += h->size;
        }
    }
    h = D_003280A8;
    func_00222500(f, "\r\n--Free Memory List (gMembankFreeListHead)--\r\n");
    for (; h != 0; h = h->nextFree) {
        numFree++;
        func_00222500(f,
                      "  Free Memory:  Size[%7d]  TagSize[%7d]  This[%p]  NextFree[%p]  LastFree[%p]  "
                      "TotalTrackSize[%7d]\r\n",
                      h->size, h->tagSize, h, h->nextFree, h->prevFree, h->size + h->tagSize + 0x20);
    }
    func_00222500(f,
                  "----Total for pool [%d] allocations, taking up [%d] bytes.  Unallocated[%d].  Tags [%d] took "
                  "up [%d] bytes\r\n",
                  numAllocs, allocBytes, unallocated, numTags, tagBytes);
    func_00222500(f, "----Cardinality of Free List[%d]  Cardinality of Membank[%d]\r\n", numFree, numTracks);
    func_00222500(f, "------ACCOUNTED FOR[%d]\r\n\r\n", total);
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00226FC0);
#endif

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_00386470);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_00386480);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_00386490);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_003864A0);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_003864B0);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_00386508);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_00386510);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_00386518);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_00386520);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_00386528);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_00386588);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_003865B8);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_00386620);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_00386690);

#ifdef NON_MATCHING /* laço do relatório */
void func_00227300(int f) {
    char buf[1000];
    MemoryTrack *h;

    for (h = D_003A6410.start; h != 0; h = h->next) {
        if (!(h->flags & 2)) {
            if (h->tagSize > 0) {
                Q_memcpy(buf, (char *)h + (h->size + 0x20), h->tagSize);
                buf[h->tagSize] = 0;
            } else {
                func_00228CF0(buf, "UNDESCRIBED");
            }
            func_00222500(f, "1\t%d\t%s\t%d", h->size, buf, h->size + h->tagSize + 0x20);
            func_00222500(f, "\r\n");
        } else {
            func_00222500(f, "0\t%d\t\t%d\r\n", h->size, h->size + h->tagSize + 0x20);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00227300);
#endif

#ifdef NON_MATCHING /* registradores da soma */
int func_00227408(MemoryTrack *h) {
    int c;

    c = D_00328098 - 0x20; c -= (int)h; c -= h->flags; c -= h->size; c -= h->tagSize;
    if (h->tagSize > 0) {
        c -= ((char *)h)[h->size + 0x20];
    }
    return c;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00227408);
#endif

int func_00227450(int size) {
    if (size > 0) {
        if (size < 16) {
            size = 16;
        } else if (size & 0xF) {
            size = size / 16 * 16 + 16;
        }
    }
    return size;
}

void func_00227490(const char *msg) {
}

}
