#ifndef STUDIO_H
#define STUDIO_H

/* Formato de modelo studio (IDST versão 10), igual ao do SDK do HL.
   No PS2 os arquivos têm extensão .dol, mas o layout é o mesmo. */

#define MAXSTUDIOTRIANGLES  20000
#define MAXSTUDIOVERTS      2048
#define MAXSTUDIOSEQUENCES  256
#define MAXSTUDIOSKINS      100
#define MAXSTUDIOSRCBONES   512
#define MAXSTUDIOBONES      128
#define MAXSTUDIOMODELS     32
#define MAXSTUDIOBODYPARTS  32
#define MAXSTUDIOGROUPS     16
#define MAXSTUDIOANIMATIONS 512
#define MAXSTUDIOMESHES     256
#define MAXSTUDIOEVENTS     1024
#define MAXSTUDIOPIVOTS     256
#define MAXSTUDIOCONTROLLERS 8

typedef struct
{
    int id;                     /* 0x00 */
    int version;                /* 0x04 */
    char name[64];              /* 0x08 */
    int length;                 /* 0x48 */
    float eyeposition[3];       /* 0x4C */
    float min[3];               /* 0x58 */
    float max[3];               /* 0x64 */
    float bbmin[3];             /* 0x70 */
    float bbmax[3];             /* 0x7C */
    int flags;                  /* 0x88 */
    int numbones;               /* 0x8C */
    int boneindex;              /* 0x90 */
    int numbonecontrollers;     /* 0x94 */
    int bonecontrollerindex;    /* 0x98 */
    int numhitboxes;            /* 0x9C */
    int hitboxindex;            /* 0xA0 */
    int numseq;                 /* 0xA4 */
    int seqindex;               /* 0xA8 */
    int numseqgroups;           /* 0xAC */
    int seqgroupindex;          /* 0xB0 */
    int numtextures;            /* 0xB4 */
    int textureindex;           /* 0xB8 */
    int texturedataindex;       /* 0xBC */
    int numskinref;             /* 0xC0 */
    int numskinfamilies;        /* 0xC4 */
    int skinindex;              /* 0xC8 */
    int numbodyparts;           /* 0xCC */
    int bodypartindex;          /* 0xD0 */
    int numattachments;         /* 0xD4 */
    int attachmentindex;        /* 0xD8 */
    int soundtable;             /* 0xDC */
    int soundindex;             /* 0xE0 */
    int soundgroups;            /* 0xE4 */
    int soundgroupindex;        /* 0xE8 */
    int numtransitions;         /* 0xEC */
    int transitionindex;        /* 0xF0 */
} studiohdr_t;

typedef struct
{
    int id;
    int version;
    char name[64];
    int length;
} studioseqhdr_t;

typedef struct
{
    char name[32];              /* 0x00 */
    int parent;                 /* 0x20 */
    int flags;                  /* 0x24 */
    int bonecontroller[6];      /* 0x28 */
    float value[6];             /* 0x40 */
    float scale[6];             /* 0x58 */
} mstudiobone_t;                /* 0x70 */

typedef struct
{
    int bone;                   /* 0x00 */
    int type;                   /* 0x04 */
    float start;                /* 0x08 */
    float end;                  /* 0x0C */
    int rest;                   /* 0x10 */
    int index;                  /* 0x14 */
} mstudiobonecontroller_t;      /* 0x18 */

typedef struct
{
    int bone;                   /* 0x00 */
    int group;                  /* 0x04 */
    float bbmin[3];             /* 0x08 */
    float bbmax[3];             /* 0x14 */
} mstudiobbox_t;                /* 0x20 */

typedef struct
{
    char label[32];             /* 0x00 */
    char name[64];              /* 0x20 */
    cache_user_t cache;         /* 0x60 */
    int data;                   /* 0x64 */
} mstudioseqgroup_t;            /* 0x68 */

typedef struct
{
    char label[32];             /* 0x00 */
    float fps;                  /* 0x20 */
    int flags;                  /* 0x24 */
    int activity;               /* 0x28 */
    int actweight;              /* 0x2C */
    int numevents;              /* 0x30 */
    int eventindex;             /* 0x34 */
    int numframes;              /* 0x38 */
    int numpivots;              /* 0x3C */
    int pivotindex;             /* 0x40 */
    int motiontype;             /* 0x44 */
    int motionbone;             /* 0x48 */
    float linearmovement[3];    /* 0x4C */
    int automoveposindex;       /* 0x58 */
    int automoveangleindex;     /* 0x5C */
    float bbmin[3];             /* 0x60 */
    float bbmax[3];             /* 0x6C */
    int numblends;              /* 0x78 */
    int animindex;              /* 0x7C */
    int blendtype[2];           /* 0x80 */
    float blendstart[2];        /* 0x88 */
    float blendend[2];          /* 0x90 */
    int blendparent;            /* 0x98 */
    int seqgroup;               /* 0x9C */
    int entrynode;              /* 0xA0 */
    int exitnode;               /* 0xA4 */
    int nodeflags;              /* 0xA8 */
    int nextseq;                /* 0xAC */
} mstudioseqdesc_t;             /* 0xB0 */

typedef struct
{
    int frame;
    int event;
    int type;
    char options[64];
} mstudioevent_t;               /* 0x4C */

typedef struct
{
    float org[3];
    int start, end;
} mstudiopivot_t;

typedef struct
{
    char name[32];              /* 0x00 */
    int type;                   /* 0x20 */
    int bone;                   /* 0x24 */
    float org[3];               /* 0x28 */
    float vectors[3][3];        /* 0x34 */
} mstudioattachment_t;          /* 0x58 */

typedef struct
{
    unsigned short offset[6];
} mstudioanim_t;

typedef union
{
    struct {
        byte valid;
        byte total;
    } num;
    short value;
} mstudioanimvalue_t;

typedef struct
{
    char name[64];              /* 0x00 */
    int nummodels;              /* 0x40 */
    int base;                   /* 0x44 */
    int modelindex;             /* 0x48 */
} mstudiobodyparts_t;           /* 0x4C */

typedef struct
{
    char name[64];              /* 0x00 */
    int flags;                  /* 0x40 */
    int width;                  /* 0x44 */
    int height;                 /* 0x48 */
    int index;                  /* 0x4C */
} mstudiotexture_t;             /* 0x50 */

typedef struct
{
    char name[64];              /* 0x00 */
    int type;                   /* 0x40 */
    float boundingradius;       /* 0x44 */
    int nummesh;                /* 0x48 */
    int meshindex;              /* 0x4C */
    int numverts;               /* 0x50 */
    int vertinfoindex;          /* 0x54 */
    int vertindex;              /* 0x58 */
    int numnorms;               /* 0x5C */
    int norminfoindex;          /* 0x60 */
    int normindex;              /* 0x64 */
    int numgroups;              /* 0x68 */
    int groupindex;             /* 0x6C */
} mstudiomodel_t;               /* 0x70 */

typedef struct
{
    int numtris;                /* 0x00 */
    int triindex;               /* 0x04 */
    int skinref;                /* 0x08 */
    int numnorms;               /* 0x0C */
    int normindex;              /* 0x10 */
} mstudiomesh_t;                /* 0x14 */

#define STUDIO_X        0x0001
#define STUDIO_Y        0x0002
#define STUDIO_Z        0x0004
#define STUDIO_XR       0x0008
#define STUDIO_YR       0x0010
#define STUDIO_ZR       0x0020
#define STUDIO_LX       0x0040
#define STUDIO_LY       0x0080
#define STUDIO_LZ       0x0100
#define STUDIO_AX       0x0200
#define STUDIO_AY       0x0400
#define STUDIO_AZ       0x0800
#define STUDIO_AXR      0x1000
#define STUDIO_AYR      0x2000
#define STUDIO_AZR      0x4000
#define STUDIO_TYPES    0x7FFF
#define STUDIO_RLOOP    0x8000

#define STUDIO_LOOPING  0x0001

#define STUDIO_NF_FLATSHADE  0x0001
#define STUDIO_NF_CHROME     0x0002
#define STUDIO_NF_FULLBRIGHT 0x0004
#define STUDIO_NF_ADDITIVE   0x0020
#define STUDIO_NF_TRANSPARENT 0x0040

#define STUDIO_HAS_NORMALS  0x0001
#define STUDIO_HAS_VERTICES 0x0002
#define STUDIO_HAS_BBOX     0x0004
#define STUDIO_HAS_CHROME   0x0008

#endif /* STUDIO_H */
