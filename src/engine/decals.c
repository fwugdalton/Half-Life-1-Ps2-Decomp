#include "common.h"
#include "engine/cmd.h"
#include "engine/cvar.h"
#include "engine/decals.h"

typedef void (*cachebuild_t)(void *data);
typedef void (*cachecallback_t)(cache_user_t *c, int mode);

int sprintf(char *, const char *, ...);
char *strncpy(char *, const char *, unsigned int);
int strncmp(const char *, const char *, unsigned int);

void Pak_Open(pakfile_t *pak, char *path);
void Pak_Close(pakfile_t *pak, char *path);
int CD_SectorCount(int size);
void CD_Read(int sector, int count, void *buffer);
void *Cache_Check(cache_user_t *c);
void *Cache_Alloc(cache_user_t *c, int size, char *name);
void COM_FileBase(char *in, char *out);
void W_CleanupName(char *in, char *out);
void GL_BuildDecal(void *data);
int GL_LoadDecalTexture(decalheader_t *header, int type, int width, int height, byte *pixels, byte *palette, int format);
void GL_FreeDecalTexture(decalheader_t *header);
void DecalsWad_Init(void);
void DecalsWad_SetName(int num, char *name);
int DecalsWad_Index(int id);
void *DecalsWad_Texture(int index);
int DecalsWad_Size(int number);
int DecalsWad_IndexFromName(char *name);
void R_DrawVLs_f(void);
void R_DrawDecals_f(void);

extern pakfile_t decal_pak;
extern char decal_names[MAX_DECALS][16];
extern cachebuild_t decal_pfnCacheBuild;
extern cachepic_t *decal_cache;
extern byte *decal_offsets;
extern cachecallback_t cache_callback;
typedef struct server_s
{
    qboolean active;
} server_t;

extern server_t sv;
extern cvar_t violence_hblood;

int Draw_CacheReload(int index, char *clean);
void *Draw_CacheGet(int index);

void Decal_Init(void)
{
    int i;

    Pak_Open(&decal_pak, "VALVE\\DECALS.PAK;1");
    decal_pfnCacheBuild = GL_BuildDecal;
    decal_cache = Mem_DbgMalloc(decal_pak.count * sizeof(cachepic_t), "engine/decals.c", 67);
    memset(decal_cache, 0, decal_pak.count * sizeof(cachepic_t));
    decal_offsets = Mem_DbgMalloc(decal_pak.count, "engine/decals.c", 70);
    memset(decal_offsets, 0, decal_pak.count);

    sv_decalnamecount = decal_pak.count;
    if (sv_decalnamecount > MAX_DECALS)
        Sys_Error("Too many decals: %d / %d\n", sv_decalnamecount, MAX_DECALS);

    for (i = 0; i < sv_decalnamecount; i++)
    {
        memset(&sv_decalnames[i], 0, sizeof(decalname_t));
        strncpy(sv_decalnames[i].name, decal_pak.lumps[i].name, sizeof(sv_decalnames[i].name) - 1);
    }

    DecalsWad_Init();
    Cmd_AddCommand("drawvls", R_DrawVLs_f);
    Cmd_AddCommand("drawdecals", R_DrawDecals_f);
}

void Draw_DecalShutdown(void)
{
    Pak_Close(&decal_pak, "VALVE\\DECALS.PAK;1");
}

void Draw_DecalSetName(int num, char *name)
{
    if (num < decal_pak.count)
    {
        strncpy(decal_names[num], name, sizeof(decal_names[num]) - 1);
        decal_names[num][sizeof(decal_names[num]) - 1] = 0;
    }
    else
    {
        DecalsWad_SetName(num - decal_pak.count, name);
    }
}

#ifdef NON_MATCHING
int Draw_DecalIndex(int id)
{
    char tmp[32];
    char *pName;

    if (id < decal_pak.count)
    {
        pName = decal_names[id];
        if (!pName[0])
            Sys_Error("Used decal #%d without a name\n", id);

        if (!sv.active && violence_hblood.value == 0.0f && !strncmp(pName, "{blood", 6))
        {
            sprintf(tmp, "{yblood%s", pName + 6);
            pName = tmp;
        }

        strcpy(decal_cache[id].name, pName);
        return id;
    }

    return DecalsWad_Index(id - decal_pak.count);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/decals", Draw_DecalIndex);
#endif

#ifdef NON_MATCHING
int Draw_DecalSize(int number)
{
    paklump_t *lump;
    if (number < decal_pak.count)
    {
        lump = &decal_pak.lumps[number];
        return lump->size;
    }
    return DecalsWad_Size(number - decal_pak.count);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/decals", Draw_DecalSize);
#endif

void *Draw_DecalTexture(int index)
{
    if (index < decal_pak.count)
        return Draw_CacheGet(index);

    return DecalsWad_Texture(index);
}

int Draw_CacheByIndex(cachewad_t *wad, int nIndex, int playernum)
{
    int i;
    cachepic_t *pic;
    char szTestName[32];

    sprintf(szTestName, "%03i%02i", playernum, nIndex);

    for (pic = wad->cache, i = 0; i < wad->cacheCount; pic++, i++)
    {
        if (!strcmp(pic->name, szTestName))
            break;
    }

    if (i == wad->cacheCount)
    {
        if (i == wad->cacheMax)
            Sys_Error("Cache wad (%s) out of %d entries", wad->name, i);

        wad->cacheCount++;
        sprintf(pic->name, "%s", szTestName);
    }

    return i;
}

int Draw_DecalIndexFromName(char *name)
{
    char tmpName[16];
    int i;

    i = DecalsWad_IndexFromName(name);
    if (i == -1)
    {
        strcpy(tmpName, name);
        if (tmpName[0] == '}')
            tmpName[0] = '{';

        for (i = 0; i < MAX_DECALS; i++)
        {
            if (decal_names[i][0] && !strcmp(tmpName, decal_names[i]))
                return i;
        }

        return 0;
    }

    return i;
}

void func_00262598(void)
{
}

#ifdef NON_MATCHING
void *Draw_CacheGet(int index)
{
    cachepic_t *pic;
    byte *dat;
    char name[16];
    char clean[16];

    pic = &decal_cache[index];
    dat = NULL;
    if (pic->name[0])
    {
        dat = Cache_Check(&pic->cache);
        if (!dat)
        {
            COM_FileBase(pic->name, name);
            W_CleanupName(name, clean);
            if (!Draw_CacheReload(index, clean))
                Sys_Error("Draw_CacheGet: failed to load %s", pic->name);
            else
                dat = pic->cache.data;
        }
        dat += decal_offsets[index];
    }

    return dat;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/decals", Draw_CacheGet);
#endif

int Draw_CacheIndex(cachewad_t *wad, char *path)
{
    int i;
    cachepic_t *pic;

    for (pic = wad->cache, i = 0; i < wad->cacheCount; pic++, i++)
    {
        if (!strcmp(path, pic->name))
            break;
    }

    if (i == wad->cacheCount)
    {
        if (i == wad->cacheMax)
            Sys_Error("Cache wad (%s) out of %d entries", wad->name, i);

        wad->cacheCount++;
        strcpy(pic->name, path);
    }

    return i;
}

void Draw_DecalCacheCallback(cache_user_t *c, unsigned int mode)
{
    decaltex_t *tex;
    decalheader_t *hdr;

    if (c)
    {
        tex = (decaltex_t *)(((int)c->data + 15) & ~15);
        switch (mode)
        {
            case 0:
                break;
            case 1:
                hdr = &tex->header;
                tex->texture = GL_LoadDecalTexture(hdr, 1, hdr->width, hdr->height, tex->pixels, tex->palette, hdr->format);
                break;
            case 2:
                GL_FreeDecalTexture(&tex->header);
                break;
        }
    }
}

#ifdef NON_MATCHING
int Draw_CacheReload(int index, char *clean)
{
    int count;
    int sector;
    byte *dat;
    byte *buf;

    count = CD_SectorCount(decal_pak.lumps[index].size);
    sector = decal_pak.sector + decal_pak.lumps[index].filepos / 2048;

    cache_callback = Draw_DecalCacheCallback;
    dat = Cache_Alloc(&decal_cache[index].cache, (count << 11) | 0x11, clean);
    cache_callback = NULL;
    if (!dat)
        Sys_Error("Draw_CacheGet: not enough space for %s", clean);

    buf = (byte *)(((int)dat + 15) & ~15);
    decal_offsets[index] = buf - dat;
    CD_Read(sector, count, buf);
    dat[decal_pak.lumps[index].size] = 0;
    decal_pfnCacheBuild(buf);
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/decals", Draw_CacheReload);
#endif
