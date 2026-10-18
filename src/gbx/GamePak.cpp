/* Gbx: pak do jogo atual (<jogo>\<jogo>.PAK;1, valve ou um mod) no hunk */

#include "common.h"

extern "C" {
#include "engine/decals.h"
#include "engine/cmd.h"

void *Hunk_AllocName(int size, const char *name);
void Host_InitHunkLevel(void);
void Pak_Init(pakfile_t *pak);
void Pak_Load(pakfile_t *pak, const char *name);
int func_00235300(const char *a, const char *b);
void func_002374D8(const char *name);
char *strstr(const char *, const char *);
void Mod_LoadGameModel(char *name, void *buffer, int size);
void Mod_EndGameModels(void);
void Mod_ClearLevelModels(void);
void func_002CB5C8(char *name, void *data, int size);
void func_002CB660(void);
void func_002CB768(void);
void func_00235E20(void);

extern int D_00328A1C;          /* 1 = vazio, 2 = carregado, 3 = modelos e sons prontos */
extern pakfile_t D_003A80C0;
extern int D_003A80DC;          /* marca do hunk antes do pak */

void func_00235A50(void) {
    Pak_Init(&D_003A80C0);
    Hunk_AllocName(0, "-GAME_PAK-");
    D_003A80DC = Hunk_LowMark();
    D_00328A1C = 1;
}

void func_00235AA0(const char *game) {
    char path[0x70];

    sprintf(path, "%s\\%s.PAK;1", game, game);
    func_002374D8(path);
    D_003A80C0.inMemory = 2;
    Pak_Load(&D_003A80C0, path);
    Host_InitHunkLevel();
    D_00328A1C = 2;
}

void func_00235B08(void) {
    switch (D_00328A1C) {
    case 1:
        break;
    case 2:
        Hunk_FreeToLowMark(D_003A80DC);
        Pak_Init(&D_003A80C0);
        break;
    case 3:
        func_00235E20();
        Hunk_FreeToLowMark(D_003A80DC);
        Pak_Init(&D_003A80C0);
        break;
    }
    D_00328A1C = 1;
}

void func_00235BA0(void) {
    char *s;
    int i;

    for (i = 0; i < D_003A80C0.count; i++) {
        s = strstr(D_003A80C0.lumps[i].name, "models/");
        if (s != 0) {
            Mod_LoadGameModel(s, D_003A80C0.data + D_003A80C0.lumps[i].filepos, D_003A80C0.lumps[i].size);
        }
        s = strstr(D_003A80C0.lumps[i].name, "sprites/");
        if (s != 0 && strstr(D_003A80C0.lumps[i].name, ".txt") == 0) {
            Mod_LoadGameModel(s, D_003A80C0.data + D_003A80C0.lumps[i].filepos, D_003A80C0.lumps[i].size);
        }
        s = strstr(D_003A80C0.lumps[i].name, "sound/");
        if (s != 0) {
            func_002CB5C8(s + 6, D_003A80C0.data + D_003A80C0.lumps[i].filepos, D_003A80C0.lumps[i].size);
        }
    }
    Mod_EndGameModels();
    func_002CB768();
    D_00328A1C = 3;
}

int func_00235D28(const char *name, byte **data, int *size) {
    int found;
    int i;

    found = 0;
    for (i = 0; i < D_003A80C0.count; i++) {
        if (func_00235300(D_003A80C0.lumps[i].name, name) == 1) {
            found = 1;
            *data = D_003A80C0.data + D_003A80C0.lumps[i].filepos;
            *size = D_003A80C0.lumps[i].size;
            break;
        }
    }
    return found;
}

void func_00235E18(void) {
}

void func_00235E20(void) {
    Mod_ClearLevelModels();
    func_002CB660();
}
}
