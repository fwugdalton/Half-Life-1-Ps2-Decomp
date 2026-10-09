#ifndef ENGINE_MODEL_H
#define ENGINE_MODEL_H

#include "common.h"

#define CONTENTS_SOLID -2

#define MODEL_MAX_PVS 1024

/* layouts parciais: só os campos usados até agora, nos offsets do binário */
typedef struct mnode_s
{
    int contents;               /* 0x00 */
    struct mnode_s *parent;     /* 0x04 */
    short visframe[2];          /* 0x08: um por jogador local */
    struct mplane_s *plane;     /* 0x0C */
    byte unk_10[0x30 - 0x10];
    struct mnode_s *children[2]; /* 0x30 */
    unsigned short firstsurface; /* 0x38 */
    unsigned short numsurfaces; /* 0x3A */
    byte unk_3C[0x40 - 0x3C];
} mnode_t; /* 0x40 */

typedef struct texture_s
{
    byte unk_00[0x10];
    char name[16];              /* 0x10 */
} texture_t;

/* no PS2 o mtexinfo_t vem embutido no começo da superfície */
typedef struct msurface_s
{
    float vecs[2][4];           /* 0x00 */
    float mipadjust;            /* 0x20 */
    texture_t *texture;         /* 0x24 */
    byte unk_28[0x40 - 0x28];
    short texturemins[2];       /* 0x40 */
    short extents[2];           /* 0x44 */
    byte unk_48[0x90 - 0x48];
} msurface_t; /* 0x90 */

typedef struct mleaf_s
{
    int contents;               /* 0x00 */
    byte unk_04[0x2C];
    byte *compressed_vis;       /* 0x30 */
    byte unk_34[0x0C];
} mleaf_t; /* 0x40 */

typedef enum
{
    mod_brush,
    mod_sprite,
    mod_alias,
    mod_studio
} modtype_t;

typedef struct dclipnode_s
{
    int planenum;
    short children[2];
} dclipnode_t;

typedef struct hull_s
{
    dclipnode_t *clipnodes;
    struct mplane_s *planes;
    int firstclipnode;          /* 0x08 */
    int lastclipnode;
    float clip_mins[3];
    float clip_maxs[3];
} hull_t; /* 0x28 */

typedef struct model_s
{
    char name[64];
    int needload;               /* 0x40 */
    int type;                   /* 0x44 */
    int numframes;              /* 0x48 */
    int synctype;               /* 0x4C */
    float mins[3];              /* 0x50 */
    int unk_5C;
    float maxs[3];              /* 0x60 */
    byte unk_6C[0x80 - 0x6C];
    int flags;                  /* 0x80 */
    byte unk_84[0x9C - 0x84];
    int numleafs;               /* 0x9C */
    mleaf_t *leafs;             /* 0xA0 */
    byte unk_A4[0xB8 - 0xA4];
    mnode_t *nodes;             /* 0xB8 */
    byte unk_BC[0xC0 - 0xBC];
    msurface_t *surfaces;       /* 0xC0 */
    byte unk_C4[0xDC - 0xC4];
    hull_t hulls[4];            /* 0xDC */
} model_t;

#endif /* ENGINE_MODEL_H */
