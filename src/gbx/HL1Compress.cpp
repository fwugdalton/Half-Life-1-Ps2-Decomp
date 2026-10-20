/* Gbx/HL1Compress.cpp: grava o save (.HL1) só com as diferenças em relação ao dicionário */

#include "common.h"

/* nó da tabela hash (Jade/HashTable) */
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

typedef struct {
    short delta;
    short index;
    void *pfn;
} VtblEntry;

typedef struct {
    VtblEntry rtti;
    VtblEntry dtor;
    VtblEntry allocNode;
    VtblEntry hash;
} HashTableVtbl;

typedef struct HashTable {
    HashSlot slots[100];
    int numSlots;
    int numElements;
    HashTableVtbl *vptr;
} HashTable;

/* tabela hash com nós num vetor fixo (objeto global D_00325088) */
typedef struct {
    HashTable base;
    HashNode nodes[293];        /* 0x330 */
    int numNodes;               /* 0x1EA8 */
} FixedHashTable;

typedef struct {
    int id;
    int location;
    int size;
    int flags;
    char *classname;
} ENTITYTABLE;

typedef struct {
    int saveId;
    int version;
    int skillLevel;
    int entityCount;
    int connectionCount;
    int lightStyleCount;
    float time;
    char mapName[32];
    char skyName[32];
    int skyColor_r;
    int skyColor_g;
    int skyColor_b;
    float skyVec_x;
    float skyVec_y;
    float skyVec_z;
} SAVE_HEADER;

typedef struct {
    int index;
    char style[64];
} SAVELIGHTSTYLE;

/* dados de um save já lidos (0xD8 bytes) */
typedef struct {
    int id;                     /* 0x00 */
    int version;                /* 0x04 */
    int unk08;
    int entityCount;            /* 0x0C */
    int unk10;
    ENTITYTABLE *entities;      /* 0x14 */
    int unk18;
    char **tokens;              /* 0x1C */
    int tokenCount;             /* 0x20 */
    int unk24;                  /* 0x24 */
    int unk28;
    int unk2C;                  /* 0x2C */
    int unk30;
    SAVE_HEADER header;         /* 0x34 */
    int unkA8;
    int unkAC;
    int unkB0;                  /* 0xB0 */
    int dataSize;               /* 0xB4 */
    void *data;                 /* 0xB8 */
    int unkBC;
    SAVELIGHTSTYLE *lightStyles; /* 0xC0 */
    int unkC4;
    int unkC8;                  /* 0xC8 */
    int unkCC;
    void *unkD0;                /* 0xD0 */
    int unkD4;
} SaveData;

extern "C" {

void *Mem_DbgMalloc(int size, const char *file, int line);
void Mem_Free(void *p);
void func_00225860(void *dst, int c, int n);
int func_00225790(const void *a, const void *b, int n);
int func_00228DC0(const char *s);
int func_00228E00(const char *a, const char *b);
void *func_0022B118(const char *name, const char *mode);
void func_0022B238(void *f);
void func_0022B310(const char *name, int a1);
int func_0022BD98(const void *data, int size, int count, void *f);
HashTable *func_00229378(HashTable *t);
void func_00229398(HashTable *t, int flags);
void func_002293C8(HashTable *t, const char *key, long value);
long func_002294D0(HashTable *t, const char *key);

extern FixedHashTable D_00325088;
extern HashTableVtbl D_003859B0;
extern const char D_003859E0[];     /* "Gbx/HL1Compress.cpp" */

FixedHashTable *func_0021D900(FixedHashTable *t);
void func_0021D938(FixedHashTable *t, int flags);
int func_0021DA30(const void *a, const void *b, int n);
int func_0021DD50(void *f, int count, char **tokens, int dictCount, char **dictTokens);
int func_0021DF50(void *f, int count, ENTITYTABLE *tab, int dictCount, ENTITYTABLE *dict);
int func_0021E190(void *f, SAVE_HEADER *h, SAVE_HEADER *dict);
int func_0021E5F0(void *f, int size, void *data, int size2, int unk, int dictSize, int dictUnk);
int func_0021E650(void *f, int count, SAVELIGHTSTYLE *ls, int dictCount, SAVELIGHTSTYLE *dict);
int func_0021E880(void *f, int count, void *a, int dictCount, void *b);

void func_0021D8A8(const char *key, long value)
{
    func_002293C8(&D_00325088.base, key, value);
}

int func_0021D8D0(const char *key)
{
    return func_002294D0(&D_00325088.base, key);
}

FixedHashTable *func_0021D900(FixedHashTable *t)
{
    func_00229378(&t->base);
    t->numNodes = 0;
    t->base.vptr = &D_003859B0;
    return t;
}

void func_0021D938(FixedHashTable *t, int flags)
{
    t->base.vptr = &D_003859B0;
    func_00229398(&t->base, flags);
}

HashNode *func_0021D960(FixedHashTable *t)
{
    return &t->nodes[t->numNodes++];
}

void func_0021D980(int initialize, int priority)
{
    if (priority == 0xFFFF) {
        if (initialize)
            func_0021D900(&D_00325088);
        else
            func_0021D938(&D_00325088, 2);
    }
}

void func_0021D9C0(void)
{
    func_0021D980(1, 0xFFFF);
}

void func_0021D9E0(void)
{
    func_0021D980(0, 0xFFFF);
}

int func_0021DA00(const char *buf, int len)
{
    int empty = 1;
    int i;

    for (i = 0; i < len; i++) {
        if (buf[i])
            empty = 0;
    }
    return empty;
}

int func_0021DA30(const void *a, const void *b, int n)
{
    return func_00225790(a, b, n) == 0;
}

INCLUDE_RODATA("asm/nonmatchings/gbx/HL1Compress", D_003859B0);

void func_0021DA50(SaveData in, SaveData dict, const char *out)
{
    void *f;

    if (in.id != dict.id)
        return;
    if (in.version != dict.version)
        return;

    f = func_0022B118(out, "wb");
    func_0022BD98(&in.unk2C, 4, 1, f);
    func_0022BD98(&in.entityCount, 4, 1, f);
    func_0022BD98(&in.tokenCount, 4, 1, f);
    func_0022BD98(&in.unk24, 4, 1, f);
    func_0021DD50(f, in.tokenCount, in.tokens, dict.tokenCount, dict.tokens);
    func_0021DF50(f, in.entityCount, in.entities, dict.entityCount, dict.entities);
    func_0021E190(f, &in.header, &dict.header);
    func_0021E5F0(f, in.dataSize, in.data, in.dataSize, in.unkB0, dict.dataSize, dict.unkB0);
    func_0021E650(f, in.header.lightStyleCount, in.lightStyles, dict.header.lightStyleCount, dict.lightStyles);
    func_0021E880(f, in.unkC8, in.unkD0, dict.unkC8, dict.unkD0);
    func_0022B238(f);
    func_0022B310(out, 9);
}

int func_0021DD50(void *f, int count, char **tokens, int dictCount, char **dictTokens)
{
    short changed;
    short i;
    short len;
    int *same;
    int size;

    changed = 0;
    same = (int *)Mem_DbgMalloc(count * 4, D_003859E0, 0x83);
    func_00225860(same, 0, count * 4);
    for (i = 0; i < count; i++) {
        if (func_00228E00(dictTokens[i], tokens[i]) == 0)
            same[i] = 1;
        else
            changed++;
    }

    func_0022BD98(&changed, 2, 1, f);
    size = 2;
    for (i = 0; i < count; i++) {
        if (same[i] == 0) {
            if (i < count)
                len = func_00228DC0(tokens[i]);
            else
                len = 0;
            func_0022BD98(&i, 2, 1, f);
            func_0022BD98(&len, 2, 1, f);
            if (len > 0)
                func_0022BD98(tokens[i], 1, len, f);
            size += 4 + len;
        }
    }
    Mem_Free(same);
    return size;
}

int func_0021DF50(void *f, int count, ENTITYTABLE *tab, int dictCount, ENTITYTABLE *dict)
{
    unsigned char flags;
    short len;
    int size;
    int i;

    size = 0;
    for (i = 0; i < count; i++) {
        if (i < dictCount) {
            flags = 0;
            if (tab[i].id != dict[i].id)
                flags = 1;
            if (tab[i].location != dict[i].location)
                flags |= 2;
            if (tab[i].size != dict[i].size)
                flags |= 4;
            if (tab[i].flags != dict[i].flags)
                flags |= 8;
            if (func_00228E00(tab[i].classname, dict[i].classname))
                flags |= 0x10;
        } else {
            flags = 0xFF;
        }

        func_0022BD98(&flags, 1, 1, f);
        size++;
        if (flags & 1) {
            func_0022BD98(&tab[i].id, 4, 1, f);
            size += 4;
        }
        if (flags & 2) {
            func_0022BD98(&tab[i].location, 4, 1, f);
            size += 4;
        }
        if (flags & 4) {
            func_0022BD98(&tab[i].size, 4, 1, f);
            size += 4;
        }
        if (flags & 8) {
            func_0022BD98(&tab[i].flags, 4, 1, f);
            size += 4;
        }
        if (flags & 0x10) {
            size += 3;
            len = func_00228DC0(tab[i].classname);
            func_0022BD98(&len, 2, 1, f);
            func_0022BD98(tab[i].classname, 1, len, f);
        }
    }
    return size;
}

int func_0021E190(void *f, SAVE_HEADER *h, SAVE_HEADER *dict)
{
    unsigned short flags;
    short len1;
    short len2;
    int size;

    flags = 0;
    if (h->skillLevel != dict->skillLevel)
        flags = 1;
    if (h->entityCount != dict->entityCount)
        flags |= 2;
    if (h->connectionCount != dict->connectionCount)
        flags |= 4;
    if (h->lightStyleCount != dict->lightStyleCount)
        flags |= 8;
    if (!func_0021DA30(&h->time, &dict->time, 4))
        flags |= 0x10;
    if (func_00228E00(h->mapName, dict->mapName))
        flags |= 0x20;
    if (func_00228E00(h->skyName, dict->skyName))
        flags |= 0x40;
    if (h->skyColor_r != dict->skyColor_r)
        flags |= 0x80;
    if (h->skyColor_g != dict->skyColor_g)
        flags |= 0x100;
    if (h->skyColor_b != dict->skyColor_b)
        flags |= 0x200;
    if (!func_0021DA30(&h->skyVec_x, &dict->skyVec_x, 4))
        flags |= 0x400;
    if (!func_0021DA30(&h->skyVec_y, &dict->skyVec_y, 4))
        flags |= 0x800;
    if (!func_0021DA30(&h->skyVec_z, &dict->skyVec_z, 4))
        flags |= 0x1000;

    func_0022BD98(&flags, 2, 1, f);
    size = 2;
    if (flags & 1) {
        func_0022BD98(&h->skillLevel, 4, 1, f);
        size = 6;
    }
    if (flags & 2) {
        func_0022BD98(&h->entityCount, 4, 1, f);
        size += 4;
    }
    if (flags & 4) {
        func_0022BD98(&h->connectionCount, 4, 1, f);
        size += 4;
    }
    if (flags & 8) {
        func_0022BD98(&h->lightStyleCount, 4, 1, f);
        size += 4;
    }
    if (flags & 0x10) {
        func_0022BD98(&h->time, 4, 1, f);
        size += 4;
    }
    if (flags & 0x20) {
        len1 = func_00228DC0(h->mapName) + 1;
        func_0022BD98(&len1, 2, 1, f);
        func_0022BD98(h->mapName, 1, len1, f);
        size += 2 + len1;
    }
    if (flags & 0x40) {
        len2 = func_00228DC0(h->skyName) + 1;
        func_0022BD98(&len2, 2, 1, f);
        func_0022BD98(h->skyName, 1, len2, f);
        size += 2 + len2;
    }
    if (flags & 0x80) {
        func_0022BD98(&h->skyColor_r, 4, 1, f);
        size += 4;
    }
    if (flags & 0x100) {
        func_0022BD98(&h->skyColor_g, 4, 1, f);
        size += 4;
    }
    if (flags & 0x200) {
        func_0022BD98(&h->skyColor_b, 4, 1, f);
        size += 4;
    }
    if (flags & 0x400) {
        func_0022BD98(&h->skyVec_x, 4, 1, f);
        size += 4;
    }
    if (flags & 0x800) {
        func_0022BD98(&h->skyVec_y, 4, 1, f);
        size += 4;
    }
    if (flags & 0x1000) {
        func_0022BD98(&h->skyVec_z, 4, 1, f);
        size += 4;
    }
    return size;
}

int func_0021E5F0(void *f, int size, void *data, int size2, int unk, int dictSize, int dictUnk)
{
    func_0022BD98(&size, 4, 1, f);
    func_0022BD98(data, size, 1, f);
    return size + 4;
}

int func_0021E650(void *f, int count, SAVELIGHTSTYLE *ls, int dictCount, SAVELIGHTSTYLE *dict)
{
    short n;
    short i;
    int size;

    n = count;
    func_0022BD98(&n, 2, 1, f);
    size = 2;
    for (i = 0; i < count; i++) {
        if (i < dictCount) {
            unsigned char flags;
            unsigned char len;

            flags = 0;
            if (ls[i].index != dict[i].index)
                flags = 1;
            if (func_00228E00(ls[i].style, dict[i].style))
                flags |= 2;
            func_0022BD98(&flags, 1, 1, f);
            size++;
            if (ls[i].index != dict[i].index) {
                func_0022BD98(&ls[i].index, 4, 1, f);
                size += 4;
            }
            if (func_00228E00(ls[i].style, dict[i].style)) {
                len = func_00228DC0(ls[i].style) + 1;
                func_0022BD98(&len, 1, 1, f);
                func_0022BD98(ls[i].style, 1, len, f);
                size += 1 + len;
            }
        } else {
            unsigned char flags;
            unsigned char len;

            flags = 3;
            func_0022BD98(&flags, 1, 1, f);
            size++;
            func_0022BD98(&ls[i].index, 4, 1, f);
            size += 4;
            len = func_00228DC0(ls[i].style) + 1;
            func_0022BD98(&len, 1, 1, f);
            func_0022BD98(ls[i].style, 1, len, f);
            size += 1 + len;
        }
    }
    return size;
}

INCLUDE_ASM("asm/nonmatchings/gbx/HL1Compress", func_0021E880);

INCLUDE_RODATA("asm/nonmatchings/gbx/HL1Compress", D_003859E0);

}
