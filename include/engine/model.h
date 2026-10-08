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

typedef struct model_s
{
    byte unk_00[0x9C];
    int numleafs;               /* 0x9C */
    mleaf_t *leafs;             /* 0xA0 */
} model_t;

#endif /* ENGINE_MODEL_H */
