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

/* cabeçalho de 0x5C bytes no início de cada save */
struct GameSaveHeader {
    int unk00;
    char unk04[4];
    int chapter;
    char unk0C[0x50];
};

struct GameSaveEntry {
    int used;
    int size;
    int group;
    char unk0C[0x20];
    char dir[0x44];
    GameSaveHeader hdr;
    char name[0x50];
};

struct GameSaveGroup {
    int loaded;
    int unk04;
    char dir[0x20];
    int unk28;
    int refs;
};

/* objeto de 0x20 bytes montado durante a gravação (funções 0x230F80..0x2311D0) */
struct GameSaveTask {
    char pad[0x20];
};

struct GameSaveState {
    int card;
    int status;
    int numSaves;
    int unk0C;
    GameSaveEntry *saves;
    int unk14;
    int unk18;
    GameSaveGroup *groups;
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
int func_002307A8(int slot);
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
char *strcpy(char *dst, const char *src);
char *strcat(char *dst, const char *src);
int func_0022CF08(int card, const char *pattern, int *handle, char *name);
int func_0022D0F8(int handle, int *found, char *name);
void func_0022D260(int handle);
int func_0022D600(int card, const char *dir, int *err);
void func_002305B8(GameSaveState *s, int n);
extern char D_00328250[16];
extern const char D_003874A0[];
extern const char D_00387618[];
extern const char D_00387610[];
extern const char D_00387620[];
extern const char D_003875A0[];
extern const char D_003875B0[];
void Pak_Load(pakfile_t *pak, const char *path);
void func_00235198(pakfile_t *pak);
int func_0022C7E0(int card, const char *dir);
int func_0022CB50(int card, const char *name, const char *newname);
int func_0022E6A0(int card, const char *dir);
int func_0022F630(int card, const char *name, const void *header);
int func_0022F740(int card, const char *dir, pakfile_t *pak);
void func_002307D8(GameSaveHeader *hdr);
void func_00230F80(GameSaveTask *t);
void func_00230F90(GameSaveTask *t, int mode);
void func_00230FB8(GameSaveTask *t, pakfile_t *pak);
void func_00231070(GameSaveTask *t, int n);
void func_002311D0(GameSaveTask *t);
int func_0022D6D8(int card, const char *name, int *size);
void func_0022AFD0(const char *name, int size);
int func_0022B9D8(void *f, int fd, int size);
int func_0022E838(void);
int func_0022D3B8(void);
int func_00230410(GameSaveGroup *g, GameSaveState *s);
int func_002304C8(GameSaveState *s);
int func_00228C48(const char *a, const char *b);
int func_00228DC0(const char *s);
float powf(float x, float y);
void *func_002260D0(void *ptr, int size, const char *file, int line);
int func_00230678(GameSaveState *s, const char *name);
int func_00230710(GameSaveState *s, const char *name);
extern const char D_00387600[];
extern const char D_00387608[];
extern const char D_00387630[];
int func_0022CC20(int fd, void *buf, int size, int *read);
int func_0022D280(int card, const char *name, int *size);
void func_00230618(GameSaveState *s, int n);
void func_002309F0(GameSaveHeader *hdr, char *out);
int func_0022C638(int card);
int func_0022C668(int card);
int func_0022C680(int card);
int func_0022D6F8(int card, const char *name, int size, const void *buf);
int func_0022FF50(GameSaveState *s);
int func_002300D8(GameSaveState *s);
int func_00230178(GameSaveState *s);
extern const char D_00387590[];
extern const char D_00387598[];
int func_0022C6A0(int card);
int func_0022C8B0(int card, const char *name, int create, int mode, int *fd);
int func_0022C9F0(int fd);
int func_0022CA78(int card, const char *name);
int func_0022CCC0(int fd, const void *buf, int size, int *written);
int func_0022E758(int card, const char *dir);
int func_0022F820(int card, const char *dir, pakfile_t *pak);
void *func_0022B118(const char *name, const char *mode);
void *func_0022BF38(void *f);
void func_0022B238(void *f);
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
    D_003A76D8.groups = 0;
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
    if (D_003A76D8.groups != 0) {
        Mem_Free(D_003A76D8.groups);
    }
    D_003A76D8.groups = 0;
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

int func_0022EAE0(int slot) {
    char buf[0x10];

    D_003A76D8.numSaves = 0;
    D_003A76D8.unk14 = 0;
    D_003A76D8.unk20 = 0;
    D_003A76D8.card = func_002307A8(slot);
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

int func_0022EBA0(int slot) {
    char buf[0x10];

    D_003A76D8.numSaves = 0;
    D_003A76D8.unk14 = 0;
    D_003A76D8.card = func_002307A8(slot);
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
                func_00228EE0(num, "%d", D_003A76D8.saves[idx].hdr.chapter);
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

#ifdef NON_MATCHING
/* ordem de agendamento e alocação de registradores diferentes */
int func_0022F110(int idx) {
    pakfile_t pak;
    GameSaveTask task;
    char path[0x50];
    int ret;
    GameSaveEntry *e;
    GameSaveGroup *g;
    int ok;
    int r;

    e = &D_003A76D8.saves[idx];
    g = &D_003A76D8.groups[e->group];
    D_003282A0[0] = 1;
    ret = 0;
    func_002307D8(&e->hdr);
    ok = 1;
    if (e->used == 1) {
        ok = 0;
        if (func_0022CA78(D_003A76D8.card, e->dir) == 0) {
            e->used = 0;
            ok = 1;
            g->refs--;
        }
    }
    if (ok == 1) {
        if (g->loaded == 0) {
            r = 0;
            Pak_Init(&pak);
            pak.inMemory = ok;
            Pak_Load(&pak, "ICONS\\CGMESAVE.PAK;1");
            if (func_0022C7E0(D_003A76D8.card, g->dir) == 0) {
                func_00230F80(&task);
                func_00230FB8(&task, &pak);
                func_00231070(&task, g->unk28);
                g->loaded = ok;
                r = func_0022F740(D_003A76D8.card, g->dir, &pak);
                if (r == ok) {
                    r = func_0022E6A0(D_003A76D8.card, g->dir);
                }
                func_002311D0(&task);
                func_00230F90(&task, 2);
            }
            func_00235198(&pak);
        } else {
            r = 1;
        }
        if (r == 1) {
            func_00228EE0(path, D_00387590, g->dir, D_00387598, D_003875A0);
            if (func_0022CB50(D_003A76D8.card, path, D_003875B0) == 0) {
                ok = func_0022F630(D_003A76D8.card, e->dir, &e->hdr);
                if (ok == r) {
                    func_00228EE0(path, D_00387590, g->dir, D_00387598, D_003875B0);
                    if (func_0022CB50(D_003A76D8.card, path, D_003875A0) != 0) {
                        ok = 0;
                    }
                    ret = ok;
                }
            }
        }
    }
    return ret;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022F110);
#endif

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387590);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387598);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_003875A0);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_003875B0);

#ifdef NON_MATCHING
/* o original mantém r em s4 e faz ret = r; aqui o gcc propaga a constante 1 */
int func_0022F360(int idx) {
    GameSaveHeader hdr;
    char path[0x70];
    int size;
    int fd;
    int n;
    GameSaveEntry *e;
    void *f;
    int ret;
    int r;

    e = &D_003A76D8.saves[idx];
    ret = 0;
    r = func_0022D6D8(D_003A76D8.card, e->dir, &size);
    if (r == 1) {
        if (func_0022C8B0(D_003A76D8.card, e->dir, 0, 0, &fd) == 0) {
            if (func_0022CC20(fd, &hdr, 0x5C, &n) == 0 && n == 0x5C) {
                func_00228EE0(path, D_003874A0, func_0022E958(), "LoadGame.sav");
                if (func_0022B6D0(path) == 0) {
                    func_0022AFD0(path, e->size);
                }
                ret = r;
                f = func_0022B118(path, "wb");
                if (func_0022B9D8(f, fd, size - 0x5C) != 1) {
                    ret = 0;
                }
                func_0022B238(f);
            }
            if (func_0022C9F0(fd) != 0) {
                ret = 0;
            }
        }
    }
    return ret;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022F360);
#endif

#ifdef NON_MATCHING
/* registradores: o original guarda a constante 1 em s2 e reusa s0 para g->dir */
int func_0022F4B0(int idx) {
    GameSaveEntry *e;
    GameSaveGroup *g;
    int ret;
    int r;

    e = &D_003A76D8.saves[idx];
    g = &D_003A76D8.groups[e->group];
    ret = 0;
    if (func_0022CA78(D_003A76D8.card, e->dir) == 0) {
        if (--g->refs == 0) {
            if (func_0022F820(D_003A76D8.card, g->dir, &D_003A7700) == 1) {
                r = func_0022E758(D_003A76D8.card, g->dir);
                if (r == 1) {
                    if (func_0022CA78(D_003A76D8.card, g->dir) == 0) {
                        ret = r;
                    }
                }
            }
        } else {
            ret = 1;
        }
    }
    return ret;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022F4B0);
#endif

int func_0022F598(int slot) {
    int card;
    int i;
    int ret;

    ret = 0;
    card = func_002307A8(slot);
    for (i = 0; i < 5; i++) { if (func_0022C6A0(card) == 0) { ret = 1; break; } ret = 0; }
    return ret;
}

int func_0022F600(void) {
    return func_0022C598(D_003A76D8.card) != 1;
}

int func_0022F630(int card, const char *name, const void *header) {
    int fd;
    int n;
    int size;
    void *f;
    void *buf;
    int ret;

    ret = 0;
    size = func_0022B8C0("TempMCSave.sav");
    f = func_0022B118("TempMCSave.sav", "r");
    buf = func_0022BF38(f);
    func_0022B238(f);
    if (func_0022C8B0(card, name, 1, 1, &fd) == 0) {
        if (func_0022CCC0(fd, header, 0x5C, &n) == 0 && n == 0x5C &&
            func_0022CCC0(fd, buf, size, &n) == 0) {
            ret = n == size;
        }
        func_0022C9F0(fd);
    }
    return ret;
}

int func_0022F740(int card, const char *dir, pakfile_t *pak) {
    char path[0x50];
    int i;
    int ret;

    ret = 1;
    for (i = 0; i < pak->count; i++) {
        func_00228EE0(path, D_00387590, dir, D_00387598, pak->lumps[i].name);
        if (func_0022D6F8(card, path, pak->lumps[i].size, pak->data + pak->lumps[i].filepos) == 0) {
            ret = 0;
            break;
        }
    }
    return ret;
}

int func_0022F820(int card, const char *dir, pakfile_t *pak) {
    char path[0x50];
    int i;
    int ret;

    ret = 1;
    for (i = 0; i < pak->count; i++) {
        func_00228EE0(path, D_00387590, dir, D_00387598, pak->lumps[i].name);
        if (func_0022CA78(card, path) != 0) {
            ret = 0;
            break;
        }
    }
    return ret;
}

#ifdef NON_MATCHING
/* o original materializa a constante 1 em três registradores diferentes (s18, s19, s17) */
void func_0022F8D0(GameSaveState *s) {
    int i;
    int r;
    int t;

    for (i = 0; i < 5; i++) {
        func_0022C510(s->card);
        r = func_0022C638(s->card);
        if (r == 1) {
            t = func_0022D688(s->card);
            if (t == r) {
                if (func_0022C668(s->card) == 0) {
                    s->unk20 = func_0022C680(s->card);
                    if (func_0022FF50(s) != 1) {
                        s->status = 6;
                    } else if (func_002300D8(s) != 1) {
                        s->status = 6;
                    } else if (func_00230178(s) != 1) {
                        s->status = 6;
                    } else {
                        s->status = 3;
                    }
                    break;
                } else {
                    s->status = t;
                }
            } else {
                s->status = 2;
            }
        } else {
            s->status = 2;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022F8D0);
#endif

#ifdef NON_MATCHING
/* fluxo equivalente; o original iça os endereços de "save" e ".sav" para s8/s7 e usa outros registradores */
void func_0022F9D8(GameSaveState *s) {
    char buf[0x10];
    int found;
    int i;
    int need;
    int n;

    found = 0;
    for (i = 0; i < s->unk14; i++) {
        if (s->groups[i].unk04 == 1 && s->groups[i].refs < 13) {
            found = 1;
            need = func_0022D390(func_0022B8C0("TempMCSave.sav") + 0x50);
            sprintf(buf, "%d", 400);
            func_0010F5F0(D_00301CC0, "SpaceRequired", buf);
            if (need < func_0022C680(s->card)) {
                n = func_00230410(&s->groups[i], s);
                func_00230618(s, 1);
                func_00228EE0(s->saves[s->numSaves].dir, "%s%s%s%d%s", s->groups[i].dir, D_00387598, D_00387600, n, D_00387608);
                func_00228EE0(s->saves[s->numSaves].unk0C, D_00387610, D_00387600, n, D_00387608);
                s->saves[s->numSaves].group = i;
                s->saves[s->numSaves].size = 0;
                s->saves[s->numSaves].used = 0;
                s->saves[s->numSaves].hdr.unk00 = 99999;
                s->saves[s->numSaves].hdr.unk04[0] = 'c';
                s->saves[s->numSaves].hdr.unk04[1] = 'c';
                s->saves[s->numSaves].hdr.unk04[2] = 'c';
                s->saves[s->numSaves].hdr.unk04[3] = 'c';
                s->saves[s->numSaves].hdr.chapter = 0;
                func_00228CF0(s->saves[s->numSaves].name, D_003282A8);
                s->numSaves++;
            }
        }
    }
    if (found == 0) {
        int total;
        int base;
        int j;

        total = 0;
        j = 0;
        base = func_0022B8C0("TempMCSave.sav") + 0x50;
        for (; j < D_003A7700.count; j++) {
            total += func_0022D390(D_003A7700.lumps[j].size);
        }
        total += func_0022D390(base);
        total += func_0022E838();
        total += func_0022D3B8();
        sprintf(buf, "%d", 400);
        func_0010F5F0(D_00301CC0, "SpaceRequired", buf);
        if (total < func_0022C680(s->card)) {
            n = func_002304C8(s);
            func_002305B8(s, 1);
            func_00228EE0(s->groups[s->unk14].dir, D_00387618, D_00328250, D_003A76D0, n);
            s->groups[s->unk14].refs = 0;
            s->groups[s->unk14].loaded = 0;
            s->groups[s->unk14].unk28 = n;
            s->unk14++;
            func_00230618(s, 1);
            func_00228EE0(s->saves[s->numSaves].dir, D_00387620, D_00328250, D_003A76D0, n, D_00387598, D_00387600, 0, D_00387608);
            func_00228EE0(s->saves[s->numSaves].unk0C, D_00387610, D_00387600, 0, D_00387608);
            s->saves[s->numSaves].group = s->unk14 - 1;
            s->saves[s->numSaves].size = 0;
            s->saves[s->numSaves].used = 0;
            s->saves[s->numSaves].hdr.unk00 = 99999;
            s->saves[s->numSaves].hdr.unk04[0] = 'c';
            s->saves[s->numSaves].hdr.unk04[1] = 'c';
            s->saves[s->numSaves].hdr.unk04[2] = 'c';
            s->saves[s->numSaves].hdr.unk04[3] = 'c';
            s->saves[s->numSaves].hdr.chapter = 0;
            func_00228CF0(s->saves[s->numSaves].name, D_003282A8);
            s->numSaves++;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022F9D8);
#endif

#ifdef NON_MATCHING
/* alocação de registradores (ret em s20) e o retorno duplicado no fim */
int func_0022FF50(GameSaveState *s) {
    char pattern[0x50];
    char name[0x20];
    int handle;
    int found;
    int ret;

    strcpy(pattern, D_00328250);
    strcat(pattern, D_003A76D0);
    strcat(pattern, "*");
    s->unk14 = 0;
    ret = func_0022CF08(s->card, pattern, &handle, name);
    if (ret == 0) {
        ret = 1;
        if (handle != -1) {
            do {
                func_002305B8(s, 5);
                strcpy(s->groups[s->unk14].dir, name);
                s->groups[s->unk14].refs = 0;
                s->groups[s->unk14].loaded = 1;
                s->groups[s->unk14].unk04 = 0;
                s->unk14++;
                if (func_0022D0F8(handle, &found, name) == 0) {
                    if (found == 0) {
                        func_0022D260(handle);
                        ret = 1;
                        handle = -1;
                    }
                } else {
                    func_0022D260(handle);
                    ret = 0;
                }
            } while (handle != -1);
        }
    }
    return ret;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022FF50);
#endif

int func_002300D8(GameSaveState *s) {
    int err;
    int i;
    int r;

    for (i = 0; i < s->unk14; i++) {
        r = func_0022D600(s->card, s->groups[i].dir, &err);
        if (r == 1) {
            s->groups[i].unk04 = r;
        } else {
            s->groups[i].unk04 = 0;
        }
    }
    return 1;
}

#ifdef NON_MATCHING
/* fluxo equivalente; o original guarda o offset do grupo na pilha (0x84) e ordena os blocos de outro jeito */
int func_00230178(GameSaveState *s) {
    char pattern[0x50];
    char name[0x20];
    int handle;
    int size;
    int fd;
    int n;
    int found;
    GameSaveEntry *e;
    int i;
    int ret;

    ret = 1;
    s->numSaves = 0;
    for (i = 0; i < s->unk14; i++) {
        if (s->groups[i].unk04 == 1) {
            strcpy(pattern, s->groups[i].dir);
            strcat(pattern, D_00387598);
            strcat(pattern, D_00387600);
            strcat(pattern, D_00387630);
            strcat(pattern, D_00387608);
            if (func_0022CF08(s->card, pattern, &handle, name) != 0) {
                ret = 0;
                break;
            }
            if (handle != -1) {
                do {
                    func_00230618(s, 5);
                    e = &s->saves[s->numSaves];
                    strcpy(e->dir, s->groups[i].dir);
                    strcat(e->dir, D_00387598);
                    strcat(e->dir, name);
                    strcpy(e->unk0C, name);
                    e->used = 1;
                    if (func_0022D280(s->card, e->dir, &size) != 0) {
                        ret = 0;
                    } else {
                        e->group = i;
                        e->size = func_0022D390(size);
                        if (func_0022C8B0(s->card, e->dir, 0, 0, &fd) != 0) {
                            ret = 0;
                        } else {
                            if (func_0022CC20(fd, &e->hdr, 0x5C, &n) != 0) {
                                ret = 0;
                            } else {
                                func_002309F0(&e->hdr, e->name);
                                s->groups[i].refs++;
                                s->numSaves++;
                            }
                            if (func_0022C9F0(fd) != 0) {
                                ret = 0;
                            }
                        }
                    }
                    if (ret == 1) {
                        if (func_0022D0F8(handle, &found, name) != 0) {
                            ret = 0;
                        } else if (found == 0) {
                            func_0022D260(handle);
                            handle = -1;
                        }
                    }
                } while (ret != 0 && handle != -1);
            }
            if (ret == 0) {
                func_0022D260(handle);
                return ret;
            }
        }
    }
    return ret;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_00230178);
#endif

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387600);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387608);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387610);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387618);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387620);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387630);

int func_00230410(GameSaveGroup *g, GameSaveState *s) {
    char prefix[0x50];
    char name[0x50];
    int i;

    strcpy(prefix, g->dir);
    strcat(prefix, D_00387598);
    for (i = 0; i < 13; i++) {
        sprintf(name, "%s%s%d%s", prefix, D_00387600, i, D_00387608);
        if (func_00230678(s, name) == 0) {
            break;
        }
    }
    return i;
}

int func_002304C8(GameSaveState *s) {
    char prefix[0x20];
    char name[0x20];
    int max;
    int i;

    i = 0;
    max = 8 - func_00228DC0(D_003A76D0);
    max = (int)powf(10.0f, (float)max);
    sprintf(prefix, D_003874A0, D_00328250, D_003A76D0);
    max--;
    for (; i < max; i++) {
        sprintf(name, D_00387618, D_00328250, D_003A76D0, i);
        if (func_00230710(s, name) == 0) {
            break;
        }
    }
    return i;
}

void func_002305B8(GameSaveState *s, int n) {
    if (s->unk14 == s->unk18) {
        s->unk18 = s->unk14 + n;
        s->groups = (GameSaveGroup *)func_002260D0(s->groups, s->unk18 * sizeof(GameSaveGroup), "Gbx/GameSave.cpp", 0x546);
    }
}

void func_00230618(GameSaveState *s, int n) {
    if (s->numSaves == s->unk0C) {
        s->unk0C = s->numSaves + n;
        s->saves = (GameSaveEntry *)func_002260D0(s->saves, s->unk0C * sizeof(GameSaveEntry), "Gbx/GameSave.cpp", 0x556);
    }
}

int func_00230678(GameSaveState *s, const char *name) {
    int i;
    int found;

    found = 0;
    for (i = 0; i < s->numSaves; i++) {
        if (func_00228C48(s->saves[i].dir, name) == 1) {
            found = 1;
            break;
        }
    }
    return found;
}

int func_00230710(GameSaveState *s, const char *name) {
    int i;
    int found;

    found = 0;
    for (i = 0; i < s->unk14; i++) {
        if (func_00228C48(s->groups[i].dir, name) == 1) {
            found = 1;
            break;
        }
    }
    return found;
}

int func_002307A8(int slot) {
    switch (slot) {
    case 1:
        return 0;
    case 2:
        return 1;
    }
    return 1;
}

}
