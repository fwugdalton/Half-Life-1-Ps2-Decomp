/* Gbx/HL1Decompress.cpp: refaz o save completo a partir do .HL1 comprimido e do dicionário */

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

/* dados de um save já lidos (0xD8 bytes) */
typedef struct {
    int id;                     /* 0x00 */
    int version;                /* 0x04 */
    short unk08;
    short unk0A;
    int entityCount;            /* 0x0C */
    int unk10;
    ENTITYTABLE *entities;      /* 0x14 */
    int unk18;
    char **tokens;              /* 0x1C */
    int tokenCount;             /* 0x20 */
    int unk24;                  /* 0x24 */
    int unk28;
    int unk2C;                  /* 0x2C */
    short unk30;
    short unk32;
    SAVE_HEADER header;         /* 0x34 */
    int unkA8;
    int unkAC;
    int unkB0;                  /* 0xB0 */
    int dataSize;               /* 0xB4 */
    void *data;                 /* 0xB8 */
    short unkBC;                /* 0xBC */
    short unkBE;
    SAVELIGHTSTYLE *lightStyles; /* 0xC0 */
    int unkC4;
    int unkC8;                  /* 0xC8 */
    int unkCC;
    void *unkD0;                /* 0xD0 */
    int unkD4;
} SaveData;

extern "C" {

int func_00228DC0(const char *s);
void func_00228CF0(char *dst, const char *src);
void func_00225860(void *dst, int c, int n);
void *func_0022B118(const char *name, const char *mode);
void func_0022B238(void *f);
void func_0022B248(const char *name);
void func_0022B310(const char *name, int a1);
void func_0022B590(const char *from, const char *to);
int func_0022BD98(const void *data, int size, int count, void *f);
int func_0022BE50(void *data, int size, int count, void *f);
void func_0022BF78(void *in, void *out, int size);
void *memcpy(void *dst, const void *src, unsigned int n);
int func_0021DA00(const void *buf, int len);

/* índices dos nomes de campo no vetor de tokens do save */
extern short D_003A6380;
extern short D_003A6382;
extern short D_003A6384;
extern short D_003A6386;
extern short D_003A6388;
extern short D_003A638A;
extern short D_003A638C;
extern short D_003A638E;
extern short D_003A6390;
extern short D_003A6392;
extern short D_003A6394;
extern short D_003A6396;
extern short D_003A6398;
extern short D_003A639A;
extern short D_003A639C;
extern short D_003A639E;
extern short D_003A63A0;
extern short D_003A63A2;
extern short D_003A63A4;
extern short D_003A63A6;
extern short D_003A63A8;
extern short D_003A63AA;
extern short D_003A63AC;
extern short D_003A63AE;

extern const char D_00385A18[];     /* "" */

int func_0021F530(void *in, void *out, int dictCount, char **dictTokens, int unused);
void func_0021F738(void *in, void *out, int count, int dictCount, ENTITYTABLE *dict, short token);
void func_0021FBB8(void *in, void *out, short token, SAVE_HEADER *dict);
void func_00220628(void *in, void *out, SaveData dict);
void func_00220748(void *in, void *out, SaveData dict);
void func_00220B10(void *in, void *out, SaveData *dict);
void func_00221018(int index, const char *token);

#ifdef NON_MATCHING /* delay slot vazio depois de apagar o UnzipTmp.sdj */
void func_0021F180(const char *in, SaveData dict, const char *out)
{
    SaveData save;
    void *fin;
    void *fout;

    func_0022B590(in, "UnzipTmp.sdj");
    fin = func_0022B118("UnzipTmp.sdj", "rb");
    if (fin == 0)
        return;
    fout = func_0022B118(out, "wb");

    func_0022BE50(&save.unk2C, 4, 1, fin);
    func_0022BE50(&save.entityCount, 4, 1, fin);
    func_0022BE50(&save.tokenCount, 4, 1, fin);
    func_0022BE50(&save.unk24, 4, 1, fin);
    func_0022BD98(&dict.id, 4, 1, fout);
    func_0022BD98(&dict.version, 4, 1, fout);
    func_0022BD98(&save.unk2C, 4, 1, fout);
    func_0022BD98(&save.entityCount, 4, 1, fout);
    func_0022BD98(&save.tokenCount, 4, 1, fout);
    func_0022BD98(&save.unk24, 4, 1, fout);

    D_003A6380 = -1;
    D_003A6382 = -1;
    D_003A6384 = -1;
    D_003A6386 = -1;
    D_003A6388 = -1;
    D_003A638A = -1;
    D_003A638C = -1;
    D_003A638E = -1;
    D_003A6390 = -1;
    D_003A6392 = -1;
    D_003A6394 = -1;
    D_003A6396 = -1;
    D_003A6398 = -1;
    D_003A639A = -1;
    D_003A639C = -1;
    D_003A639E = -1;
    D_003A63A0 = -1;
    D_003A63A2 = -1;
    D_003A63A4 = -1;
    D_003A63A6 = -1;
    D_003A63A8 = -1;
    D_003A63AA = -1;
    D_003A63AC = -1;
    D_003A63AE = -1;

    func_0021F530(fin, fout, dict.tokenCount, dict.tokens, save.unk24);
    func_0021F738(fin, fout, save.entityCount, dict.entityCount, dict.entities, dict.unk08);
    func_0021FBB8(fin, fout, dict.unk30, &dict.header);
    func_00220628(fin, fout, dict);
    func_00220748(fin, fout, dict);
    func_00220B10(fin, fout, &dict);
    func_0022B238(fin);
    func_0022B238(fout);
    func_0022B248("UnzipTmp.sdj");
    func_0022B310(out, 5);
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/HL1Decompress", func_0021F180);
#endif

INCLUDE_RODATA("asm/nonmatchings/gbx/HL1Decompress", D_00385A18);

int func_0021F530(void *in, void *out, int dictCount, char **dictTokens, int unused)
{
    short count;
    short index;
    short len;
    int i;
    int j;

    func_0022BE50(&count, 2, 1, in);
    i = 0;
    for (j = 0; j < count; j++) {
        func_0022BE50(&index, 2, 1, in);
        func_0022BE50(&len, 2, 1, in);
        for (; i < index; i++) {
            func_0022BD98(dictTokens[i], 1, func_00228DC0(dictTokens[i]) + 1, out);
            func_00221018(i, dictTokens[i]);
        }
        if (len > 0) {
            char buf[256];

            func_0022BE50(buf, 1, len, in);
            buf[len] = 0;
            func_0022BD98(buf, 1, len + 1, out);
            func_00221018(i, buf);
        } else {
            func_0022BD98(D_00385A18, 1, 1, out);
        }
        i++;
    }
    for (; i < dictCount; i++) {
        func_0022BD98(dictTokens[i], 1, func_00228DC0(dictTokens[i]) + 1, out);
        func_00221018(i, dictTokens[i]);
    }
    return i;
}

void func_0021F738(void *in, void *out, int count, int dictCount, ENTITYTABLE *dict, short token)
{
    ENTITYTABLE e;
    char classname[256];
    unsigned char flags;
    short len;
    short size;
    int fields;
    int i;

    for (i = 0; i < count; i++) {
        func_0022BE50(&flags, 1, 1, in);
        if (flags == 0) {
            e.id = dict[i].id;
            e.location = dict[i].location;
            e.size = dict[i].size;
            e.flags = dict[i].flags;
            e.classname = dict[i].classname;
        } else {
            if (flags & 1)
                func_0022BE50(&e.id, 4, 1, in);
            else
                e.id = dict[i].id;
            if (flags & 2)
                func_0022BE50(&e.location, 4, 1, in);
            else
                e.location = dict[i].location;
            if (flags & 4)
                func_0022BE50(&e.size, 4, 1, in);
            else
                e.size = dict[i].size;
            if (flags & 8)
                func_0022BE50(&e.flags, 4, 1, in);
            else
                e.flags = dict[i].flags;
            if (flags & 0x10) {
                func_0022BE50(&len, 2, 1, in);
                func_0022BE50(classname, 1, len, in);
                classname[len] = 0;
                e.classname = classname;
            } else {
                e.classname = dict[i].classname;
            }
        }

        fields = 0;
        if (!func_0021DA00(&e.id, 4))
            fields++;
        if (!func_0021DA00(&e.location, 4))
            fields++;
        if (!func_0021DA00(&e.size, 4))
            fields++;
        if (!func_0021DA00(&e.flags, 4))
            fields++;
        if (func_00228DC0(e.classname))
            fields++;

        size = 4;
        func_0022BD98(&size, 2, 1, out);
        func_0022BD98(&token, 2, 1, out);
        func_0022BD98(&fields, 4, 1, out);
        if (!func_0021DA00(&e.id, 4)) {
            short n = 4;

            func_0022BD98(&n, 2, 1, out);
            func_0022BD98(&D_003A6380, 2, 1, out);
            func_0022BD98(&e.id, n, 1, out);
        }
        if (!func_0021DA00(&e.location, 4)) {
            short n = 4;

            func_0022BD98(&n, 2, 1, out);
            func_0022BD98(&D_003A6382, 2, 1, out);
            func_0022BD98(&e.location, n, 1, out);
        }
        if (!func_0021DA00(&e.size, 4)) {
            short n = 4;

            func_0022BD98(&n, 2, 1, out);
            func_0022BD98(&D_003A6384, 2, 1, out);
            func_0022BD98(&e.size, n, 1, out);
        }
        if (!func_0021DA00(&e.flags, 4)) {
            short n = 4;

            func_0022BD98(&n, 2, 1, out);
            func_0022BD98(&D_003A6386, 2, 1, out);
            func_0022BD98(&e.flags, n, 1, out);
        }
        if (func_00228DC0(e.classname)) {
            short n = func_00228DC0(e.classname) + 1;

            func_0022BD98(&n, 2, 1, out);
            func_0022BD98(&D_003A6388, 2, 1, out);
            func_0022BD98(e.classname, n, 1, out);
        }
    }
}

void func_0021FBB8(void *in, void *out, short token, SAVE_HEADER *dict)
{
    SAVE_HEADER h;
    unsigned short flags;
    short len1;
    short len2;
    short size;
    int fields;

    fields = 0;
    func_0022BE50(&flags, 2, 1, in);
    if (flags & 0x1)
        func_0022BE50(&h.skillLevel, 4, 1, in);
    else
        h.skillLevel = dict->skillLevel;
    if (!func_0021DA00(&h.skillLevel, 4))
        fields++;
    if (flags & 0x2)
        func_0022BE50(&h.entityCount, 4, 1, in);
    else
        h.entityCount = dict->entityCount;
    if (!func_0021DA00(&h.entityCount, 4))
        fields++;
    if (flags & 0x4)
        func_0022BE50(&h.connectionCount, 4, 1, in);
    else
        h.connectionCount = dict->connectionCount;
    if (!func_0021DA00(&h.connectionCount, 4))
        fields++;
    if (flags & 0x8)
        func_0022BE50(&h.lightStyleCount, 4, 1, in);
    else
        h.lightStyleCount = dict->lightStyleCount;
    if (!func_0021DA00(&h.lightStyleCount, 4))
        fields++;
    if (flags & 0x10)
        func_0022BE50(&h.time, 4, 1, in);
    else
        h.time = dict->time;
    if (!func_0021DA00(&h.time, 4))
        fields++;
    if (flags & 0x20) {
        func_00225860(h.mapName, 0, 32);
        func_0022BE50(&len1, 2, 1, in);
        func_0022BE50(h.mapName, len1, 1, in);
    } else {
        func_00225860(h.mapName, 0, 32);
        func_00228CF0(h.mapName, dict->mapName);
    }
    if (!func_0021DA00(h.mapName, 32))
        fields++;
    if (flags & 0x40) {
        func_00225860(h.skyName, 0, 32);
        func_0022BE50(&len2, 2, 1, in);
        func_0022BE50(h.skyName, len2, 1, in);
    } else {
        func_00225860(h.skyName, 0, 32);
        func_00228CF0(h.skyName, dict->skyName);
    }
    if (!func_0021DA00(h.skyName, 32))
        fields++;
    if (flags & 0x80)
        func_0022BE50(&h.skyColor_r, 4, 1, in);
    else
        h.skyColor_r = dict->skyColor_r;
    if (!func_0021DA00(&h.skyColor_r, 4))
        fields++;
    if (flags & 0x100)
        func_0022BE50(&h.skyColor_g, 4, 1, in);
    else
        h.skyColor_g = dict->skyColor_g;
    if (!func_0021DA00(&h.skyColor_g, 4))
        fields++;
    if (flags & 0x200)
        func_0022BE50(&h.skyColor_b, 4, 1, in);
    else
        h.skyColor_b = dict->skyColor_b;
    if (!func_0021DA00(&h.skyColor_b, 4))
        fields++;
    if (flags & 0x400)
        func_0022BE50(&h.skyVec_x, 4, 1, in);
    else
        h.skyVec_x = dict->skyVec_x;
    if (!func_0021DA00(&h.skyVec_x, 4))
        fields++;
    if (flags & 0x800)
        func_0022BE50(&h.skyVec_y, 4, 1, in);
    else
        h.skyVec_y = dict->skyVec_y;
    if (!func_0021DA00(&h.skyVec_y, 4))
        fields++;
    if (flags & 0x1000)
        func_0022BE50(&h.skyVec_z, 4, 1, in);
    else
        h.skyVec_z = dict->skyVec_z;
    if (!func_0021DA00(&h.skyVec_z, 4))
        fields++;

    size = 4;
    func_0022BD98(&size, 2, 1, out);
    func_0022BD98(&token, 2, 1, out);
    func_0022BD98(&fields, 4, 1, out);
    if (!func_0021DA00(&h.skillLevel, 4)) {
        short n = 4;

        func_0022BD98(&n, 2, 1, out);
        func_0022BD98(&D_003A638A, 2, 1, out);
        func_0022BD98(&h.skillLevel, n, 1, out);
    }
    if (!func_0021DA00(&h.entityCount, 4)) {
        short n = 4;

        func_0022BD98(&n, 2, 1, out);
        func_0022BD98(&D_003A638C, 2, 1, out);
        func_0022BD98(&h.entityCount, n, 1, out);
    }
    if (!func_0021DA00(&h.connectionCount, 4)) {
        short n = 4;

        func_0022BD98(&n, 2, 1, out);
        func_0022BD98(&D_003A638E, 2, 1, out);
        func_0022BD98(&h.connectionCount, n, 1, out);
    }
    if (!func_0021DA00(&h.lightStyleCount, 4)) {
        short n = 4;

        func_0022BD98(&n, 2, 1, out);
        func_0022BD98(&D_003A6390, 2, 1, out);
        func_0022BD98(&h.lightStyleCount, n, 1, out);
    }
    if (!func_0021DA00(&h.time, 4)) {
        short n = 4;

        func_0022BD98(&n, 2, 1, out);
        func_0022BD98(&D_003A6392, 2, 1, out);
        func_0022BD98(&h.time, n, 1, out);
    }
    if (!func_0021DA00(h.mapName, 32)) {
        short n = 32;

        func_0022BD98(&n, 2, 1, out);
        func_0022BD98(&D_003A6394, 2, 1, out);
        func_0022BD98(h.mapName, n, 1, out);
    }
    if (!func_0021DA00(h.skyName, 32)) {
        short n = 32;

        func_0022BD98(&n, 2, 1, out);
        func_0022BD98(&D_003A6396, 2, 1, out);
        func_0022BD98(h.skyName, n, 1, out);
    }
    if (!func_0021DA00(&h.skyColor_r, 4)) {
        short n = 4;

        func_0022BD98(&n, 2, 1, out);
        func_0022BD98(&D_003A6398, 2, 1, out);
        func_0022BD98(&h.skyColor_r, n, 1, out);
    }
    if (!func_0021DA00(&h.skyColor_g, 4)) {
        short n = 4;

        func_0022BD98(&n, 2, 1, out);
        func_0022BD98(&D_003A639A, 2, 1, out);
        func_0022BD98(&h.skyColor_g, n, 1, out);
    }
    if (!func_0021DA00(&h.skyColor_b, 4)) {
        short n = 4;

        func_0022BD98(&n, 2, 1, out);
        func_0022BD98(&D_003A639C, 2, 1, out);
        func_0022BD98(&h.skyColor_b, n, 1, out);
    }
    if (!func_0021DA00(&h.skyVec_x, 4)) {
        short n = 4;

        func_0022BD98(&n, 2, 1, out);
        func_0022BD98(&D_003A639E, 2, 1, out);
        func_0022BD98(&h.skyVec_x, n, 1, out);
    }
    if (!func_0021DA00(&h.skyVec_y, 4)) {
        short n = 4;

        func_0022BD98(&n, 2, 1, out);
        func_0022BD98(&D_003A63A0, 2, 1, out);
        func_0022BD98(&h.skyVec_y, n, 1, out);
    }
    if (!func_0021DA00(&h.skyVec_z, 4)) {
        short n = 4;

        func_0022BD98(&n, 2, 1, out);
        func_0022BD98(&D_003A63A2, 2, 1, out);
        func_0022BD98(&h.skyVec_z, n, 1, out);
    }
}

void func_00220628(void *in, void *out, SaveData dict)
{
    int size;

    size = 0;
    func_0022BE50(&size, 4, 1, in);
    func_0022BF78(in, out, size);
}

#ifdef NON_MATCHING /* registradores s3/s4 trocados (in e ls.style) */
void func_00220748(void *in, void *out, SaveData dict)
{
    short count;
    int i;

    func_0022BE50(&count, 2, 1, in);
    for (i = 0; i < count; i++) {
        SAVELIGHTSTYLE ls;
        unsigned char flags;
        unsigned char len;
        short size;
        int fields;

        func_0022BE50(&flags, 1, 1, in);
        if (flags & 1)
            func_0022BE50(&ls.index, 4, 1, in);
        else
            ls.index = dict.lightStyles[i].index;
        if (flags & 2) {
            func_0022BE50(&len, 1, 1, in);
            func_0022BE50(ls.style, len, 1, in);
        } else {
            memcpy(ls.style, dict.lightStyles[i].style, 64);
        }

        fields = 0;
        size = 4;
        if (!func_0021DA00(&ls.index, 4))
            fields++;
        if (!func_0021DA00(ls.style, 64))
            fields++;
        func_0022BD98(&size, 2, 1, out);
        func_0022BD98(&dict.unkBC, 2, 1, out);
        func_0022BD98(&fields, 4, 1, out);
        if (!func_0021DA00(&ls.index, 4)) {
            short n = 4;

            func_0022BD98(&n, 2, 1, out);
            func_0022BD98(&D_003A63AC, 2, 1, out);
            func_0022BD98(&ls.index, n, 1, out);
        }
        if (!func_0021DA00(ls.style, 64)) {
            short n = 64;

            func_0022BD98(&n, 2, 1, out);
            func_0022BD98(&D_003A63AE, 2, 1, out);
            func_0022BD98(ls.style, n, 1, out);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/HL1Decompress", func_00220748);
#endif

INCLUDE_ASM("asm/nonmatchings/gbx/HL1Decompress", func_00220B10);

INCLUDE_ASM("asm/nonmatchings/gbx/HL1Decompress", func_00221018);

}
