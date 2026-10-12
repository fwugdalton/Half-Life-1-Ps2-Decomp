#ifndef ENGINE_HLFILEIO_H
#define ENGINE_HLFILEIO_H

#include "common.h"

/* sistema de arquivos do HL1 PS2 (HlFileIO.c e HlCdFileIO.c): pacotes .PAK lidos direto do CD */

typedef struct
{
    char name[56];              /* 0x00 */
    int filepos;                /* 0x38 */
    int filelen;                /* 0x3C */
} packfile_t;                   /* 0x40 */

typedef struct pack_s
{
    char filename[260];         /* 0x000 */
    int numfiles;               /* 0x104 */
    packfile_t *files;          /* 0x108 */
    int lba;                    /* 0x10C: setor do .PAK no CD */
} pack_t;

typedef struct searchpath_s
{
    char filename[260];         /* 0x000 */
    char directory[260];        /* 0x104 */
    pack_t *pack;               /* 0x208 */
    int bIsBase;                /* 0x20C: diretório do jogo base (VALVE) */
    struct searchpath_s *next;  /* 0x210 */
} searchpath_t;                 /* 0x214 */

extern searchpath_t *D_00399868;    /* com_searchpaths */

#endif /* ENGINE_HLFILEIO_H */
