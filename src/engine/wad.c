#include "common.h"
#include "engine/cmd.h"

#define TYP_QPIC 0x42

typedef struct
{
    int ident;                  /* "WAD3" */
    int numlumps;
    int infotableofs;
} wadinfo_t;

typedef struct
{
    int filepos;
    int disksize;
    int size;
    char type;
    char compression;
    char pad1, pad2;
    char name[16];
} lumpinfo_t; /* 0x20 */

typedef struct
{
    int width, height;
    byte data[4];
} qpic_t;

extern int wad_numlumps;
extern lumpinfo_t *wad_lumps;
extern byte *wad_base;

extern int (*LittleLong)(int l);

void SwapPic(qpic_t *pic);

void W_CleanupName(char *in, char *out)
{
    int i;
    int c;

    for (i = 0; i < 16; i++)
    {
        c = in[i];
        if (!c)
            break;

        if (c >= 'A' && c <= 'Z')
            c += ('a' - 'A');
        out[i] = c;
    }

    for (; i < 16; i++)
        out[i] = 0;
}

void W_LoadWadFile(char *filename)
{
    lumpinfo_t *lump_p;
    wadinfo_t *header;
    int i;
    int infotableofs;

    wad_base = COM_LoadHunkFile(filename);
    if (!wad_base)
        Sys_Error("W_LoadWadFile: couldn't load %s", filename);

    header = (wadinfo_t *)wad_base;

    if (header->ident != ('W' | ('A' << 8) | ('D' << 16) | ('3' << 24)))
        Sys_Error("Wad file %s doesn't have WAD3 id\n", filename);

    wad_numlumps = LittleLong(header->numlumps);
    infotableofs = LittleLong(header->infotableofs);
    wad_lumps = (lumpinfo_t *)(wad_base + infotableofs);

    for (i = 0, lump_p = wad_lumps; i < wad_numlumps; i++, lump_p++)
    {
        lump_p->filepos = LittleLong(lump_p->filepos);
        lump_p->size = LittleLong(lump_p->size);
        W_CleanupName(lump_p->name, lump_p->name);
        if (lump_p->type == TYP_QPIC)
            SwapPic((qpic_t *)(wad_base + lump_p->filepos));
    }
}

lumpinfo_t *W_GetLumpinfo(char *name, qboolean doerror)
{
    int i;
    lumpinfo_t *lump_p;
    char clean[16];

    W_CleanupName(name, clean);

    for (lump_p = wad_lumps, i = 0; i < wad_numlumps; i++, lump_p++)
    {
        if (!strcmp(clean, lump_p->name))
            return lump_p;
    }

    if (doerror)
        Sys_Error("W_GetLumpinfo: %s not found", name);

    return NULL;
}

void *W_GetLumpName(char *name)
{
    lumpinfo_t *lump;

    lump = W_GetLumpinfo(name, true);

    return (void *)(wad_base + lump->filepos);
}

void *W_GetLumpNum(int num)
{
    lumpinfo_t *lump;

    if (num < 0 || num > wad_numlumps)
        Sys_Error("W_GetLumpNum: bad number: %i", num);

    lump = wad_lumps + num;

    return (void *)(wad_base + lump->filepos);
}

void SwapPic(qpic_t *pic)
{
    pic->width = LittleLong(pic->width);
    pic->height = LittleLong(pic->height);
}
