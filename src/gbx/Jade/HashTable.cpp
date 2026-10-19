/* Gbx/Jade: tabela hash de strings (classe C++ com vtable, escrita à mão) */

#include "common.h"

typedef struct HashNode {
    int hash;
    const char *key;
    long value;
    struct HashNode *next;
} HashNode;

typedef struct {
    int count;
    HashNode *head;
} HashSlot;

struct HashTable;

/* entrada de vtable do gcc 2.95: ajuste do this + ponteiro da função */
typedef struct {
    short delta;
    short index;
    void *pfn;
} VtblEntry;

typedef struct {
    VtblEntry rtti;
    VtblEntry dtor;
    VtblEntry allocNode;            /* virtual pura */
    VtblEntry hash;
} HashTableVtbl;

typedef struct HashTable {
    HashSlot slots[100];
    int numSlots;
    int numElements;
    HashTableVtbl *vptr;
} HashTable;

#define VCALL(t, e) ((char *)(t) + (t)->vptr->e.delta)

extern "C" {

void __builtin_delete(void *p);
int func_00228DC0(const char *s);
int func_00228E60(const char *a, const char *b);
int func_002296E0(HashTable *t, int len, const char *s);
float func_00229708(HashTable *t);
int FS_Open(const char *name, const char *mode);
void FS_Close(int f);
void func_00222500(int f, const char *fmt, ...);

extern HashTableVtbl D_00386C38;

HashTable *func_00229378(HashTable *t) {
    t->vptr = &D_00386C38;
    t->numSlots = 100;
    t->numElements = 0;
    return t;
}

void func_00229398(HashTable *t, int flags) {
    t->vptr = &D_00386C38;
    if (flags & 1) {
        __builtin_delete(t);
    }
}

#ifdef NON_MATCHING /* registradores */
void func_002293C8(HashTable *t, const char *key, long value) {
    HashNode *n;
    HashNode *m;
    int sum;
    int idx;

    sum = func_002296E0(t, func_00228DC0(key), key);
    idx = ((int (*)(void *, int))t->vptr->hash.pfn)(VCALL(t, hash), sum);
    for (n = t->slots[idx].head; n != 0; n = n->next) {
        if (func_00228E60(n->key, key) == 0) {
            break;
        }
    }
    if (n != 0) {
        n->value = value;
    } else {
        m = ((HashNode * (*)(void *)) t->vptr->allocNode.pfn)(VCALL(t, allocNode));
        m->key = key;
        m->hash = sum;
        m->value = value;
        m->next = t->slots[idx].head;
        t->slots[idx].head = m;
        t->slots[idx].count++;
        t->numElements++;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Jade/HashTable", func_002293C8);
#endif

long func_002294D0(HashTable *t, const char *key) {
    HashNode *n;
    long r;
    int idx;

    r = 0;
    idx = func_002296E0(t, func_00228DC0(key), key);
    idx = ((int (*)(void *, int))t->vptr->hash.pfn)(VCALL(t, hash), idx);
    for (n = t->slots[idx].head; n != 0; n = n->next) {
        if (func_00228E60(n->key, key) == 0) {
            r = n->value;
            break;
        }
    }
    return r;
}

#ifdef NON_MATCHING /* ordem dos blocos */
const char *func_00229570(HashTable *t, long value) {
    HashNode *n;
    const char *r;
    int i;

    r = 0;
    for (i = 0; i < t->numSlots; i++) {
        for (n = t->slots[i].head; n != 0; n = n->next) {
            if (n->value == value) {
                break;
            }
        }
        if (n != 0) {
            r = n->key;
            break;
        }
    }
    return r;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Jade/HashTable", func_00229570);
#endif

#ifdef NON_MATCHING /* registrador do limite do laço */
void func_002295C8(HashTable *t, const char *name) {
    int f;
    int i;

    i = 0;
    f = FS_Open(name, "w");
    func_00222500(f, "Num Elements: [%5d]\r\n", t->numElements);
    func_00222500(f, "Num Slots   : [%5d]\r\n", t->numSlots);
    func_00222500(f, "Load Factor : [%f]\r\n", func_00229708(t));
    func_00222500(f, "\r\n");
    for (; i < t->numSlots; i++) {
        func_00222500(f, "Slot [%5d] -> [%5d] elements\r\n", i, t->slots[i].count);
    }
    FS_Close(f);
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/Jade/HashTable", func_002295C8);
#endif

unsigned int func_002296C0(HashTable *t, unsigned int key) {
    return key % t->numSlots;
}

int func_002296E0(HashTable *t, int len, const char *s) {
    int r;

    r = 0;
    while (len != 0) {
        r += *(unsigned char *)s;
        len--;
        s++;
    }
    return r;
}

float func_00229708(HashTable *t) {
    return (float)t->numElements / (float)t->numSlots;
}

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/HashTable", D_00386C38);

}
