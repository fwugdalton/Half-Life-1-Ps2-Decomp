/* Gbx/GameSave.cpp -- saves de jogo no memory card */

#include "common.h"

extern "C" {
#include "engine/cvar.h"

cvar_t D_00328270 = { "Slot1Available", "0" };
cvar_t D_00328288 = { "Slot2Available", "0" };
int D_0032829C = 0;
int D_003282A0[2] = { 0, 0 };
char D_003282A8[0x64] = { 0 };
unsigned int D_0032830C = 0;

#include "engine/decals.h"

typedef struct client_textmessage_s client_textmessage_t;

struct GameSaveEntry {
    int used;
    int size;
    char unk08[0x70];
    int chapter;
    char unk7C[0x50];
    char name[0x50];
};

struct GameSaveState {
    int card;
    int status;
    int numSaves;
    int unk0C;
    GameSaveEntry *saves;
    int unk14;
    int unk18;
    void *unk1C;
    int unk20;
    int unk24;
};

void Mem_Free(void *ptr);
void func_00225860(void *dst, int c, int n);
void func_00228CF0(char *dst, const char *src);
int func_00228EE0(char *dst, const char *fmt, ...);
int sprintf(char *dst, const char *fmt, ...);
void func_0022B248(const char *name);
void func_0022B2B0(const char *name, const char *newName);
int func_0022B6D0(const char *name);
void func_0022C510(int card);
int func_0022C598(int card);
int func_0022D688(int card);
void func_0022F8D0(GameSaveState *s);
void func_0022F9D8(GameSaveState *s);
int func_002307A8(void);
void func_00230BD0(GameSaveState *s);
client_textmessage_t *func_0028BDE8(const char *name);
void func_00299420(char *comment);
int SaveGameSlot(char *name, char *comment);
void func_0010F5F0(void *ctx, const char *key, const char *value);
void *func_0010F5E8(void *ctx);
void *func_00107EE0(void *obj, const char *name);
float func_00107000(void *var);
int func_0022B8C0(const char *name);
int func_0022D390(int size);
void Pak_Init(pakfile_t *pak);
void Pak_Open(pakfile_t *pak, const char *path);
void Pak_Close(pakfile_t *pak, const char *path);

extern void *D_00301CC0;
extern char D_003A75C8[0x104];
extern char D_003A76D0[8];
extern GameSaveState D_003A76D8;
extern pakfile_t D_003A7700;
}

struct client_textmessage_s {
    char pad[0x28];
    const char *pMessage;
};

extern "C" {
void func_0022E860(void) {
    client_textmessage_t *msg;

    msg = func_0028BDE8("GAMESAVE_UNUSED_DATA");
    if (msg != 0) {
        func_00228CF0(D_003282A8, msg->pMessage);
    } else {
        func_00228CF0(D_003282A8, "-- Unused Data --");
    }
    D_003A76D8.unk0C = 0;
    D_003A76D8.numSaves = 0;
    D_003A76D8.saves = 0;
    D_003A76D8.unk18 = 0;
    D_003A76D8.unk14 = 0;
    D_003A76D8.unk1C = 0;
    D_003A76D8.unk20 = 0;
}

void func_0022E8D0(void) {
    D_003A76D8.unk0C = 0;
    D_003A76D8.numSaves = 0;
    if (D_003A76D8.saves != 0) {
        Mem_Free(D_003A76D8.saves);
    }
    D_003A76D8.saves = 0;
    D_003A76D8.unk18 = 0;
    D_003A76D8.unk14 = 0;
    if (D_003A76D8.unk1C != 0) {
        Mem_Free(D_003A76D8.unk1C);
    }
    D_003A76D8.unk1C = 0;
}

void func_0022E930(const char *name) {
    func_00228CF0(D_003A76D0, name);
}

char *func_0022E958(void) {
    func_00225860(D_003A75C8, 0, 0x104);
    func_00228EE0(D_003A75C8, "/%s/SAVE/", D_003A76D0);
    return D_003A75C8;
}

void func_0022E9A8(void) {
    char path[0x100];
    char comment[0x50];

    if (func_0022B6D0("TempMCSave.sav") == 0) {
        sprintf(path, "%s%s", func_0022E958(), "TempMCSave.sav");
        func_00299420(comment);
        SaveGameSlot("TempMCSave.sav", comment);
        func_0022B2B0(path, "TempMCSave.sav");
        D_003282A0[0] = 0;
    }
}

void func_0022EA30(void) {
    char path[0xD0];

    if (func_0022B6D0("TempMCSave.sav") == 1) {
        if (D_003282A0[0] == 1) {
            func_00228EE0(path, "%s%s", func_0022E958(), "LatestMCSave.sav");
            if (func_0022B6D0(path) == 1) {
                func_0022B248(path);
            }
            func_0022B2B0("TempMCSave.sav", path);
        } else {
            func_0022B248("TempMCSave.sav");
        }
    }
}

int func_0022EAE0(void) {
    char buf[0x10];

    D_003A76D8.numSaves = 0;
    D_003A76D8.unk14 = 0;
    D_003A76D8.unk20 = 0;
    D_003A76D8.card = func_002307A8();
    D_003A76D8.status = 0;
    func_0022F8D0(&D_003A76D8);
    if (D_003A76D8.status == 3) {
        func_0022F9D8(&D_003A76D8);
        func_00230BD0(&D_003A76D8);
        if (D_003A76D8.numSaves == 0) {
            D_003A76D8.status = 5;
        }
        func_00228EE0(buf, "%d", D_003A76D8.numSaves);
        func_0010F5F0(D_00301CC0, "NumSaves", buf);
    }
    return D_003A76D8.status;
}

int func_0022EBA0(void) {
    char buf[0x10];

    D_003A76D8.numSaves = 0;
    D_003A76D8.unk14 = 0;
    D_003A76D8.card = func_002307A8();
    D_003A76D8.status = 0;
    func_0022F8D0(&D_003A76D8);
    if (D_003A76D8.status == 3) {
        func_00230BD0(&D_003A76D8);
        func_00228EE0(buf, "%d", D_003A76D8.numSaves);
        func_0010F5F0(D_00301CC0, "NumSaves", buf);
        if (D_003A76D8.numSaves == 0) {
            D_003A76D8.status = 4;
        }
    }
    return D_003A76D8.status;
}

void func_0022EC50(void) {
    if (D_0032829C == 0) {
        Pak_Init(&D_003A7700);
        Pak_Open(&D_003A7700, "ICONS\\UGMESAVE.PAK;1");
        D_0032829C = 1;
    } else {
        Pak_Close(&D_003A7700, "ICONS\\UGMESAVE.PAK;1");
    }
}

void func_0022ECC0(void) {
    if (D_0032830C % 10 == 0) {
        if (func_0022C598(0) == 1) {
            func_0022C510(0);
            if (func_0022D688(0) == 1) {
                func_0010F5F0(D_00301CC0, "Slot1Available", "1");
            } else {
                func_0010F5F0(D_00301CC0, "Slot1Available", "0");
            }
        }
        if (func_0022C598(1) == 1) {
            func_0022C510(1);
            if (func_0022D688(1) == 1) {
                func_0010F5F0(D_00301CC0, "Slot2Available", "1");
            } else {
                func_0010F5F0(D_00301CC0, "Slot2Available", "0");
            }
        }
    }
    D_0032830C++;
}

void func_0022EDE8(int first, int count) {
    char key[0x70];
    char num[0x10];
    int n;
    int i;
    int idx;

    n = (int)func_00107000(func_00107EE0(func_0010F5E8(D_00301CC0), "NumSavePositions"));
    for (i = 0; i < n; i++) {
        func_00228EE0(key, "Save%dName", i + 1);
        idx = first + (i + 1 - count);
        if (idx >= 0 && idx < D_003A76D8.numSaves) {
            func_0010F5F0(D_00301CC0, key, D_003A76D8.saves[idx].name);
            if (i == count - 1) {
                func_00228EE0(num, "%d", D_003A76D8.saves[idx].chapter);
                func_0010F5F0(D_00301CC0, "ChapterNumber", num);
            }
            if (D_003A76D8.saves[idx].used == 0) {
                func_00228EE0(num, "%d", i + 1);
                func_0010F5F0(D_00301CC0, "EmptySlot", num);
            }
        } else {
            func_0010F5F0(D_00301CC0, key, "");
        }
    }
    if (n < D_003A76D8.numSaves) {
        if (first + 1 == count) {
            func_0010F5F0(D_00301CC0, "DrawUpArrow", "0.0");
        } else {
            func_0010F5F0(D_00301CC0, "DrawUpArrow", "1.0");
        }
        if (first + (n - count) == D_003A76D8.numSaves - 1) {
            func_0010F5F0(D_00301CC0, "DrawDownArrow", "0.0");
        } else {
            func_0010F5F0(D_00301CC0, "DrawDownArrow", "1.0");
        }
    } else {
        func_0010F5F0(D_00301CC0, "DrawUpArrow", "0.0");
        func_0010F5F0(D_00301CC0, "DrawDownArrow", "0.0");
    }
}

int func_0022F070(int idx) {
    int used;
    int need;

    used = D_003A76D8.saves[idx].used;
    if (used != 1) {
        return 1;
    }
    need = D_003A76D8.saves[idx].size + D_003A76D8.unk20;
    if (func_0022D390(func_0022B8C0("TempMCSave.sav")) >= need) {
        used = 0;
    }
    return used;
}

int func_0022F0E8(int idx) {
    return D_003A76D8.saves[idx].used == 1;
}

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022F110);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387590);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387598);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_003875A0);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_003875B0);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022F360);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022F4B0);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022F598);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022F600);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022F630);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022F740);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022F820);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022F8D0);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022F9D8);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022FF50);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_002300D8);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_00230178);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387600);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387608);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387610);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387618);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387620);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387630);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_00230410);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_002304C8);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_002305B8);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_00230618);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_00230678);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_00230710);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_002307A8);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387648);
}
