#include "common.h"
#include "engine/cmd.h"
#include "engine/cvar.h"
#include "engine/decals.h"
#include "engine/server.h"

void Pak_Init(pakfile_t *pak);
void Pak_Load(pakfile_t *pak, char *path);

extern char gdecal_names[MAX_DECALS][16];
extern pakfile_t gdecal_pak;
extern cachebuild_t gdecal_pfnCacheBuild;
extern cvar_t violence_hblood;

char *gdecal_built = NULL;

int DecalsWad_LumpIndex(char *name);

void DecalsWad_Init(void)
{
    int i;

    memset(gdecal_names, 0, sizeof(gdecal_names));
    Pak_Init(&gdecal_pak);
    gdecal_pak.inMemory = true;
    Pak_Load(&gdecal_pak, "VALVE\\GDECALS.PAK;1");
    gdecal_pfnCacheBuild = GL_BuildDecal;

    if (sv_decalnamecount > MAX_DECALS)
        Sys_Error("Too many decals: %d / %d\n", sv_decalnamecount, MAX_DECALS);

    for (i = 0; i < gdecal_pak.count; i++)
    {
        memset(&sv_decalnames[sv_decalnamecount + i], 0, sizeof(decalname_t));
        strncpy(sv_decalnames[sv_decalnamecount + i].name, gdecal_pak.lumps[i].name,
                sizeof(sv_decalnames[0].name) - 1);
    }

    sv_decalnamecount += gdecal_pak.count;
    gdecal_built = Mem_DbgMalloc(gdecal_pak.count, "engine/DecalsWad.c", 72);
    memset(gdecal_built, 0, gdecal_pak.count);
}

void DecalsWad_SetName(int num, char *name)
{
    strncpy(gdecal_names[num], name, sizeof(gdecal_names[num]) - 1);
    gdecal_names[num][sizeof(gdecal_names[num]) - 1] = 0;
}

int DecalsWad_Index(int id)
{
    char tmp[32];
    char *pName;

    if (!gdecal_names[id][0])
        Sys_Error("Used decal #%d without a name\n", id);

    pName = gdecal_names[id];
    if (!sv.active && violence_hblood.value == 0.0f && !strncmp(pName, "{blood", 6))
    {
        sprintf(tmp, "{yblood%s", pName + 6);
        pName = tmp;
    }

    return DecalsWad_LumpIndex(pName);
}

void *DecalsWad_Texture(int index)
{
    int i;
    byte *dat;

    i = index - decal_pak.count;
    dat = gdecal_pak.data + gdecal_pak.lumps[i].filepos;
    if (!gdecal_built[i])
    {
        gdecal_pfnCacheBuild(dat);
        gdecal_built[i] = true;
    }

    return dat;
}

int DecalsWad_Size(int number)
{
    if (number >= gdecal_pak.count)
        return 0;

    return gdecal_pak.lumps[number].size;
}

void *func_00262C20(int index)
{
    return DecalsWad_Texture(index);
}

int DecalsWad_IndexFromName(char *name)
{
    char tmpName[16];
    int i;

    strcpy(tmpName, name);
    if (tmpName[0] == '}')
        tmpName[0] = '{';

    for (i = 0; i < MAX_DECALS; i++)
    {
        if (gdecal_names[i][0] && !strcmp(tmpName, gdecal_names[i]))
            return i + decal_pak.count;
    }

    return -1;
}

int DecalsWad_LumpIndex(char *name)
{
    int i;

    for (i = 0; i < gdecal_pak.count; i++)
    {
        if (!strcmp(name, gdecal_pak.lumps[i].name))
            break;
    }

    return i + decal_pak.count;
}
