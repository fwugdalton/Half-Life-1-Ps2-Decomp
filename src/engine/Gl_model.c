#include "common.h"
#include "engine/cmd.h"
#include "engine/model.h"

typedef void *FileHandle_t;

#define MAX_KNOWN_MODELS 512
#define IDSTUDIOHEADER 0x54534449 /* "IDST" */
#define IDSPRITEHEADER_PS2 0x5A415053 /* "SPAZ" */
#define mod_studio 3

extern model_t *mod_known;
extern model_t *mod_global;
extern model_t *mod_game;
extern model_t *mod_level;
extern int mod_numlevel;
extern int mod_numgame;
extern int mod_numglobal;
extern int mod_numknown;

void *Mem_DbgMalloc(int size, const char *file, int line);
void *memset(void *, int, unsigned int);
char *strcpy(char *, const char *);
void Mod_LoadStudioModel(model_t *mod, void *buffer);
void func_002A01B8(model_t *mod, void *buffer);
void *Cache_Check(cache_user_t *c);
void Cache_Free(cache_user_t *c);
FileHandle_t FS_Open(char *name, char *mode);
void FS_Close(FileHandle_t f);
void func_00222500(FileHandle_t f, char *fmt, ...);

void Mod_InitKnown(void)
{
    mod_known = Mem_DbgMalloc(MAX_KNOWN_MODELS * sizeof(model_t), "engine/Gl_model.c", 61);
    memset(mod_known, 0, MAX_KNOWN_MODELS * sizeof(model_t));
    mod_global = mod_known;
    mod_game = NULL;
    mod_level = NULL;
    mod_numknown = 0;
    mod_numglobal = 0;
    mod_numgame = 0;
    mod_numlevel = 0;
}

void Mod_LoadGlobalModel(char *name, void *buffer)
{
    model_t *mod;

    mod = &mod_global[mod_numglobal++];
    mod_numknown++;
    strcpy(mod->name, name);
    mod->unk_198 = 0;
    mod->needload = 0;

    switch (*(unsigned int *)buffer)
    {
    case IDSTUDIOHEADER:
        Mod_LoadStudioModel(mod, buffer);
        break;
    case 0x50534449:
        break;
    case IDSPRITEHEADER_PS2:
        func_002A01B8(mod, buffer);
        break;
    }
}


void Mod_EndGlobalModels(void)
{
    mod_game = &mod_global[mod_numglobal];
}

void Mod_LoadGameModel(char *name, void *buffer)
{
    model_t *mod;

    mod = &mod_game[mod_numgame++];
    mod_numknown++;
    strcpy(mod->name, name);
    mod->needload = 0;
    mod->unk_198 = 1;

    switch (*(unsigned int *)buffer)
    {
    case IDSTUDIOHEADER:
        Mod_LoadStudioModel(mod, buffer);
        break;
    case 0x50534449:
        break;
    case IDSPRITEHEADER_PS2:
        func_002A01B8(mod, buffer);
        break;
    }
}

void Mod_ClearLevelModels(void)
{
    int i;

    for (i = 0; i < mod_numlevel; i++)
    {
        if (mod_level[i].type == mod_studio)
        {
            if (Cache_Check(&mod_level[i].cache))
                Cache_Free(&mod_level[i].cache);
        }
    }

    memset(mod_game, 0, mod_numgame * sizeof(model_t));
    memset(mod_level, 0, mod_numlevel * sizeof(model_t));
    mod_numknown = mod_numknown - mod_numlevel - mod_numgame;
    mod_numgame = 0;
    mod_numlevel = 0;
    mod_level = NULL;
}

void Mod_EndGameModels(void)
{
    mod_level = &mod_game[mod_numgame];
}

#ifdef NON_MATCHING
void Mod_Report_f(void)
{
    FileHandle_t f;
    int i;

    f = FS_Open("Model Report.txt", "w");
    func_00222500(f, "Models Loaded: [%d]\r\n", mod_numknown);
    func_00222500(f, "       Global: [%d]\r\n", mod_numglobal);
    func_00222500(f, "         Game: [%d]\r\n", mod_numgame);
    func_00222500(f, "        Level: [%d]\r\n", mod_numlevel);

    func_00222500(f, "\r\n\tGlobal Models\r\n");
    for (i = 0; i < mod_numglobal; i++)
        func_00222500(f, "[%s]\r\n", mod_global[i].name);

    func_00222500(f, "\r\n\tGame Models\r\n");
    for (i = 0; i < mod_numgame; i++)
        func_00222500(f, "[%s]\r\n", mod_game[i].name);

    func_00222500(f, "\r\n\tLevel Models\r\n");
    for (i = 0; i < mod_numlevel; i++)
        func_00222500(f, "[%s]\r\n", mod_level[i].name);

    FS_Close(f);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/Gl_model", Mod_Report_f);
#endif

INCLUDE_ASM("asm/nonmatchings/engine/Gl_model", Mod_Extradata);

INCLUDE_ASM("asm/nonmatchings/engine/Gl_model", Mod_PointInLeaf);

INCLUDE_ASM("asm/nonmatchings/engine/Gl_model", func_0029F6A8);

INCLUDE_ASM("asm/nonmatchings/engine/Gl_model", func_0029F760);

INCLUDE_ASM("asm/nonmatchings/engine/Gl_model", func_0029F908);

INCLUDE_ASM("asm/nonmatchings/engine/Gl_model", Mod_ForName);

INCLUDE_ASM("asm/nonmatchings/engine/Gl_model", func_0029FA50);

INCLUDE_ASM("asm/nonmatchings/engine/Gl_model", func_0029FA60);

INCLUDE_ASM("asm/nonmatchings/engine/Gl_model", func_0029FAB0);

INCLUDE_ASM("asm/nonmatchings/engine/Gl_model", func_0029FCF8);

INCLUDE_ASM("asm/nonmatchings/engine/Gl_model", func_0029FE20);

INCLUDE_ASM("asm/nonmatchings/engine/Gl_model", func_0029FED0);

INCLUDE_ASM("asm/nonmatchings/engine/Gl_model", func_002A0028);

INCLUDE_ASM("asm/nonmatchings/engine/Gl_model", func_002A0050);

INCLUDE_ASM("asm/nonmatchings/engine/Gl_model", func_002A0100);

INCLUDE_ASM("asm/nonmatchings/engine/Gl_model", func_002A0180);

INCLUDE_ASM("asm/nonmatchings/engine/Gl_model", func_002A01B8);

INCLUDE_ASM("asm/nonmatchings/engine/Gl_model", func_002A0320);

INCLUDE_ASM("asm/nonmatchings/engine/Gl_model", func_002A03A8);
