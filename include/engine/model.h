#ifndef ENGINE_MODEL_H
#define ENGINE_MODEL_H

#include "common.h"

#define CONTENTS_SOLID -2

#define MODEL_MAX_PVS 1024

/* layouts parciais: só os campos usados até agora, nos offsets do binário */
typedef struct mnode_s
{
    int contents;               /* 0x00 */
    byte unk_04[0x2C];
    struct mnode_s *children[2]; /* 0x30 */
} mnode_t;

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
    byte unk_6C[0x9C - 0x6C];
    int numleafs;               /* 0x9C */
    mleaf_t *leafs;             /* 0xA0 */
    byte unk_A4[0xDC - 0xA4];
    hull_t hulls[4];            /* 0xDC */
} model_t;

#endif /* ENGINE_MODEL_H */
