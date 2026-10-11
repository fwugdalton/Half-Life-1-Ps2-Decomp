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
    float minmaxs[8];           /* 0x10: mins em 0x10, maxs em 0x20 */
    struct mnode_s *children[2]; /* 0x30 */
    unsigned short firstsurface; /* 0x38 */
    unsigned short numsurfaces; /* 0x3A */
    byte unk_3C[0x40 - 0x3C];
} mnode_t; /* 0x40 */

typedef struct texture_s
{
    byte *pixels;               /* 0x00 */
    byte *palette;              /* 0x04 */
    unsigned short srcwidth;    /* 0x08 */
    unsigned short srcheight;   /* 0x0A */
    unsigned short width;       /* 0x0C */
    unsigned short height;      /* 0x0E */
    char name[16];              /* 0x10 */
    byte unk_20[0x23 - 0x20];
    byte nummips;               /* 0x23 */
    int gl_texturenum;          /* 0x24 */
    struct msurface_s *texturechain; /* 0x28 */
    int anim_total;             /* 0x2C */
    int anim_min, anim_max;     /* 0x30 */
    struct texture_s *anim_next; /* 0x38 */
    struct texture_s *alternate_anims; /* 0x3C */
} texture_t; /* 0x40 */

typedef struct glpoly_s
{
    struct glpoly_s *next;      /* 0x00 */
    struct glpoly_s *chain;     /* 0x04 */
    int numverts;               /* 0x08 */
    int flags;                  /* 0x0C */
} glpoly_t;

/* no PS2 o mtexinfo_t vem embutido no começo da superfície */
typedef struct msurface_s
{
    float vecs[2][4];           /* 0x00 */
    short unk_20;               /* 0x20 */
    unsigned short flags;       /* 0x22 */
    texture_t *texture;         /* 0x24 */
    struct color24_s *samples;  /* 0x28 */
    struct mplane_s *plane;     /* 0x2C */
    int visframe;               /* 0x30 */
    int firstedge;              /* 0x34 */
    int numedges;               /* 0x38 */
    float unk_3C;               /* 0x3C */
    short texturemins[2];       /* 0x40 */
    short extents[2];           /* 0x44 */
    glpoly_t *polys;            /* 0x48 */
    struct msurface_s *texturechain; /* 0x4C */
    int lightmaptexturenum;     /* 0x50 */
    int light_s, light_t;       /* 0x54 */
    int dlightframe;            /* 0x5C */
    int dlightbits;             /* 0x60 */
    byte styles[4];             /* 0x64 */
    int cached_light[4];        /* 0x68 */
    int cached_dlight;          /* 0x78: -1 força refazer o lightmap */
    struct decal_s *pdecals;    /* 0x7C */
    void *unk_80;               /* 0x80 */
    byte unk_84[0x90 - 0x84];
} msurface_t; /* 0x90 */

typedef struct efrag_s
{
    struct mleaf_s *leaf;       /* 0x00 */
    struct efrag_s *leafnext;   /* 0x04 */
    struct cl_entity_s *entity; /* 0x08 */
    struct efrag_s *entnext;    /* 0x0C */
} efrag_t;

typedef struct mleaf_s
{
    int contents;               /* 0x00 */
    byte unk_04[0x0C - 0x04];
    efrag_t *efrags;            /* 0x0C */
    byte unk_10[0x30 - 0x10];
    byte *compressed_vis;       /* 0x30 */
    struct msurface_s **firstmarksurface; /* 0x34 */
    unsigned short nummarksurfaces; /* 0x38 */
    byte unk_3A[0x40 - 0x3A];
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

typedef struct dmodel_s
{
    float mins[3];              /* 0x00 (vetores alinhados em 16 no PS2) */
    int pad_0C;
    float maxs[3];              /* 0x10 */
    int pad_1C;
    float origin[3];            /* 0x20 */
    int pad_2C;
    int headnode[4];            /* 0x30 */
    int visleafs;               /* 0x40 */
    int firstface;              /* 0x44 */
    int numfaces;               /* 0x48 */
    int pad_4C;
} dmodel_t; /* 0x50 */

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
    byte unk_6C[0x70 - 0x6C];
    float radius;               /* 0x70 */
    byte unk_74[0x80 - 0x74];
    int flags;                  /* 0x80 */
    int firstmodelsurface;      /* 0x84 */
    int nummodelsurfaces;       /* 0x88 */
    int numsubmodels;           /* 0x8C */
    struct dmodel_s *submodels; /* 0x90 */
    int numplanes;              /* 0x94 */
    struct mplane_s *planes;    /* 0x98 */
    int numleafs;               /* 0x9C */
    mleaf_t *leafs;             /* 0xA0 */
    int numvertexes;            /* 0xA4 */
    void *vertexes;             /* 0xA8 */
    int numedges;               /* 0xAC */
    void *edges;                /* 0xB0 */
    int numnodes;               /* 0xB4 */
    mnode_t *nodes;             /* 0xB8 */
    int numsurfaces;            /* 0xBC */
    msurface_t *surfaces;       /* 0xC0 */
    int numsurfedges;           /* 0xC4 */
    int *surfedges;             /* 0xC8 */
    int numclipnodes;           /* 0xCC */
    dclipnode_t *clipnodes;     /* 0xD0 */
    int nummarksurfaces;        /* 0xD4 */
    msurface_t **marksurfaces;  /* 0xD8 */
    hull_t hulls[4];            /* 0xDC */
    int numtextures;            /* 0x17C */
    texture_t *textures;        /* 0x180 */
    byte *visdata;              /* 0x184 */
    void *lightdata;            /* 0x188 */
    char *entities;             /* 0x18C */
    byte unk_190[0x194 - 0x190];
    cache_user_t cache;         /* 0x194 */
    int unk_198;                /* 0x198 */
} model_t;

#endif /* ENGINE_MODEL_H */
