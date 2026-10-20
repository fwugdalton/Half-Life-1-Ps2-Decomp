/* Gbx/HL1Read.cpp */

#include "common.h"

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

typedef struct {
    short id;
    short len;
    void *data;
    int owned;
} SaveEntry;

typedef struct SaveRecord {
    short token;
    short unk02;
    int unk04;
    unsigned char count;        /* 0x08 */
    SaveEntry *entries;         /* 0x0C */
} SaveRecord;

/* dados de um save já lidos (0xD8 bytes) */
typedef struct {
    int id;                     /* 0x00 */
    int version;                /* 0x04 */
    short unk08;
    short unk0A;
    int entityCount;            /* 0x0C */
    int unk10;
    ENTITYTABLE *entities;      /* 0x14 */
    char *tokenData;            /* 0x18 */
    char **tokens;              /* 0x1C */
    int tokenCount;             /* 0x20 */
    int tokenDataSize;          /* 0x24 */
    char *body;                 /* 0x28 */
    int unk2C;                  /* 0x2C */
    short unk30;
    short unk32;
    SAVE_HEADER header;         /* 0x34 */
    int unkA8;
    int unkAC;
    void *unkB0;                /* 0xB0 */
    int dataSize;               /* 0xB4 */
    void *data;                 /* 0xB8 */
    short unkBC;                /* 0xBC */
    short unkBE;
    SAVELIGHTSTYLE *lightStyles; /* 0xC0 */
    int unkC4;
    int unkC8;                  /* 0xC8 */
    void *unkCC;
    struct SaveRecord *records; /* 0xD0 */
    int unkD4;
} SaveData;

extern "C" {

void *func_0022B118(const char *name, const char *mode);
void func_0022B238(void *f);
int func_0022BE50(void *data, int size, int count, void *f);
char *func_0022BF38(void *f);
void *Mem_DbgMalloc(int size, const char *file, int line);
void Mem_Free(void *ptr);
extern const char D_00385B78[];     /* "Gbx/HL1Read.cpp" */

void func_00221898(char *p, char **tokens, int count);
char *func_002218E8(char *p, ENTITYTABLE *entities, short *a2, int count, char **tokens);
char *func_00221AA8(char *p, short *a1, SAVE_HEADER *header, char **tokens, int *a4);
char *func_00221DF0(char *p, int *a1, void *buf, int count, char **tokens, int *a5, void **a6);
char *func_00222018(char *p, short *a1, SAVELIGHTSTYLE *buf, int count, char **tokens, int *a5);
char *func_002221E0(char *p, SaveRecord *buf, int count, int *a3, int total, char **tokens);
char *func_002223E8(char *p, int *size);

int func_00221470(const char *name, SaveData *s)
{
    void *f;
    int r;
    int total;
    char *p;
    char *q;
    char *data;
    int size;

    f = func_0022B118(name, "rb");
    if (f != 0) {
    func_0022BE50(&s->id, 4, 1, f);
    if (s->id != 0x564C4156) {
        r = 0;
    } else {
        func_0022BE50(&s->version, 4, 1, f);
        func_0022BE50(&s->unk2C, 4, 1, f);
        func_0022BE50(&s->entityCount, 4, 1, f);
        func_0022BE50(&s->tokenCount, 4, 1, f);
        func_0022BE50(&s->tokenDataSize, 4, 1, f);
        s->tokenData = (char *)Mem_DbgMalloc(s->tokenDataSize, D_00385B78, 0x3A);
        s->tokens = (char **)Mem_DbgMalloc(s->tokenCount * 4, D_00385B78, 0x3B);
        func_0022BE50(s->tokenData, s->tokenDataSize, 1, f);
        func_00221898(s->tokenData, s->tokens, s->tokenCount);
        s->body = func_0022BF38(f);
        s->entities = (ENTITYTABLE *)Mem_DbgMalloc(s->entityCount * 0x14, D_00385B78, 0x43);
        data = s->body;
        p = func_002218E8(data, s->entities, &s->unk08, s->entityCount, s->tokens);
        s->unk10 = p - data;
        p = func_00221AA8(p, &s->unk30, &s->header, s->tokens, &s->unkA8);
        s->unkB0 = Mem_DbgMalloc(s->header.connectionCount * 0x50, D_00385B78, 0x4D);
        p = func_00221DF0(p, &s->unkAC, s->unkB0, s->header.connectionCount, s->tokens, &s->dataSize, &s->data);
        s->lightStyles = (SAVELIGHTSTYLE *)Mem_DbgMalloc(s->header.lightStyleCount * 0x44, D_00385B78, 0x52);
        p = func_00222018(p, &s->unkBC, s->lightStyles, s->header.lightStyleCount, s->tokens, &s->unkC4);
        s->unkC8 = 0;
        s->unkCC = 0;
        total = 0;
        q = p;
        while (q - s->body < s->unk2C) {
            q = func_002223E8(q, &size);
            s->unkC8++;
            total += size;
        }
        s->records = (SaveRecord *)Mem_DbgMalloc(s->unkC8 * 16, D_00385B78, 0x65);
        data = p;
        s->unkD4 = func_002221E0(data, s->records, s->unkC8, (int *)&s->unkCC, total, s->tokens) - data;
        r = 1;
    }
    func_0022B238(f);
    } else {
        r = 0;
    }
    return r;
}

void func_00221768(SaveData s)
{
    Mem_Free(s.tokenData);
    Mem_Free(s.tokens);
    Mem_Free(s.entities);
    Mem_Free(s.unkB0);
    Mem_Free(s.lightStyles);
    if (s.unkCC)
        Mem_Free(s.unkCC);
    if (s.records)
        Mem_Free(s.records);
}

INCLUDE_RODATA("asm/nonmatchings/gbx/HL1Read", D_00385B78);

INCLUDE_RODATA("asm/nonmatchings/gbx/HL1Read", D_00385B88);

void func_00221898(char *p, char **tokens, int count)
{
    int i;

    for (i = 0; i < count; i++) {
        tokens[i] = p;
        while (*p++)
            ;
    }
}

INCLUDE_ASM("asm/nonmatchings/gbx/HL1Read", func_002218E8);

INCLUDE_ASM("asm/nonmatchings/gbx/HL1Read", func_00221AA8);

INCLUDE_ASM("asm/nonmatchings/gbx/HL1Read", func_00221DF0);

INCLUDE_ASM("asm/nonmatchings/gbx/HL1Read", func_00222018);

INCLUDE_ASM("asm/nonmatchings/gbx/HL1Read", func_002221E0);

INCLUDE_ASM("asm/nonmatchings/gbx/HL1Read", func_002223E8);

}
