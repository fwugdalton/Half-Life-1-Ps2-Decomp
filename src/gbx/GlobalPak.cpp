/* Gbx: PAKS\GLOBAL.PAK, sempre na memória (modelos, sprites e sons globais) */

#include "common.h"

extern "C" {
#include "engine/decals.h"

void Mem_PushTag(void);
void Mem_SetTag(int tag);
void Mem_PopTag(void);
void *func_00225D48(int size, int align, const char *file, int line);
void func_00225DB8(void *buf);
int func_00237128(const char *name, int *size, int *lba, int *sectors);
void CD_Read(int lba, int sectors, void *buf);
void Pak_Init(pakfile_t *pak);
void Pak_Load(pakfile_t *pak, const char *name);
int func_00235300(const char *a, const char *b);
char *strstr(const char *, const char *);
void Mod_LoadGlobalModel(char *name, void *buffer, int size);
void Mod_EndGlobalModels(void);
void func_002CB508(char *name, void *data, int size);
void func_002CB5A0(void);
int func_00238398(const char *name, int *size, int mode, void *buf);

extern int D_00328A18;
extern pakfile_t D_003A80A0;
extern int D_003A80BC;

void func_00235688(void) {
    int size;
    int lba;
    int sectors;
    int *h;
    int len;

    func_00237128("PAKS\\GLOBAL.PAK;1", &size, &lba, &sectors);
    Mem_PushTag();
    Mem_SetTag(2);
    h = (int *)func_00225D48(0x800, 0x40, "Gbx/GlobalPak.cpp", 0x45);
    CD_Read(lba, 1, h);
    len = *h;
    func_00225DB8(h);
    Mem_PopTag();
    Pak_Init(&D_003A80A0);
    Mem_PushTag();
    Mem_SetTag(1);
    D_003A80A0.inMemory = 3;
    D_003A80A0.data = (byte *)func_00225D48(len, 0x40, "Gbx/GlobalPak.cpp", 0x5B);
    Mem_PopTag();
    D_00328A18 = 1;
    D_003A80BC = 0;
}

void func_00235770(void) {
    Pak_Load(&D_003A80A0, "PAKS\\GLOBAL.PAK;1");
}

void func_00235798(void) {
    char *s;
    int i;

    for (i = 0; i < D_003A80A0.count; i++) {
        s = strstr(D_003A80A0.lumps[i].name, "models/");
        if (s != 0) {
            Mod_LoadGlobalModel(s, D_003A80A0.data + D_003A80A0.lumps[i].filepos, D_003A80A0.lumps[i].size);
        }
        s = strstr(D_003A80A0.lumps[i].name, "sprites/");
        if (s != 0 && strstr(D_003A80A0.lumps[i].name, ".txt") == 0) {
            Mod_LoadGlobalModel(s, D_003A80A0.data + D_003A80A0.lumps[i].filepos, D_003A80A0.lumps[i].size);
        }
        s = strstr(D_003A80A0.lumps[i].name, "sound/");
        if (s != 0) {
            func_002CB508(s + 6, D_003A80A0.data + D_003A80A0.lumps[i].filepos, D_003A80A0.lumps[i].size);
        }
    }
    Mod_EndGlobalModels();
    func_002CB5A0();
}

int func_00235910(const char *name, byte **data, int *size) {
    int found;
    int i;

    found = 0;
    for (i = 0; i < D_003A80A0.count; i++) {
        if (func_00235300(D_003A80A0.lumps[i].name, name) == 1) {
            found = 1;
            *data = D_003A80A0.data + D_003A80A0.lumps[i].filepos;
            *size = D_003A80A0.lumps[i].size;
            break;
        }
    }
    return found;
}

byte *func_00235A00(void) {
    D_003A80BC = 1;
    return D_003A80A0.data;
}

int func_00235A18(void) {
    int size;
    int ret;

    ret = func_00238398("PAKS\\GRESTORE.PAK;1", &size, 2, D_003A80A0.data);
    D_003A80BC = 0;
    return ret;
}
}
