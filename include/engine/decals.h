#ifndef ENGINE_DECALS_H
#define ENGINE_DECALS_H

#include "common.h"

#define MAX_DECALS 512

/* entrada de um .PAK do CD (VALVE\DECALS.PAK etc.) */
typedef struct
{
    char name[56];
    int filepos;  /* 0x38: offset em bytes dentro do pak */
    int size;     /* 0x3C */
} paklump_t;

typedef void (*cachebuild_t)(void *data);

typedef struct
{
    int count;
    paklump_t *lumps;
    int sector;                 /* setor inicial do pak no CD */
    int unk_0C;
    int unk_10;
    byte *data;                 /* 0x14: pak inteiro na memória */
    qboolean inMemory;          /* 0x18 */
} pakfile_t;

typedef struct cachepic_s
{
    char name[64];
    cache_user_t cache;
} cachepic_t;

typedef struct cachewad_s
{
    char *name;
    cachepic_t *cache;
    int cacheCount;
    int cacheMax;
} cachewad_t;

typedef struct decalname_s
{
    char name[16];
    byte ucFlags;
} decalname_t;

/* textura de decal como fica no cache (cabeçalho do pak + paleta + pixels) */
typedef struct
{
    byte unk_00[0x13];
    byte format;            /* 0x13 */
    byte unk_14[4];
    unsigned short width;   /* 0x18 */
    unsigned short height;  /* 0x1A */
    byte unk_1C[4];
} decalheader_t;

typedef struct
{
    byte unk_00[0x24];
    int texture;            /* 0x24 */
    byte unk_28[0x18];
    decalheader_t header;   /* 0x40 */
    byte palette[0x400];    /* 0x60 */
    byte pixels[1];         /* 0x460 */
} decaltex_t;

extern pakfile_t decal_pak;
extern int sv_decalnamecount;
extern decalname_t sv_decalnames[MAX_DECALS];

void Decal_Init(void);
int Draw_CacheIndex(cachewad_t *wad, char *path);

void DecalsWad_Init(void);
void DecalsWad_SetName(int num, char *name);
int DecalsWad_Index(int id);
void *DecalsWad_Texture(int index);
int DecalsWad_Size(int number);
int DecalsWad_IndexFromName(char *name);

void GL_BuildDecal(void *data);

#endif /* ENGINE_DECALS_H */
