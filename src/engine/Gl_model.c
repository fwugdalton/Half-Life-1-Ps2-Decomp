#include "common.h"
#include "engine/cmd.h"
#include "engine/model.h"
#include "engine/mathlib.h"
#include "engine/cvar.h"

typedef void *FileHandle_t;

typedef struct
{
    int version;
    int planes;          /* 0x04 */
    int nodes;           /* 0x08 */
    int leafs;           /* 0x0C */
    int edges;           /* 0x10 */
    int surfedges;       /* 0x14 */
    int vertexes;        /* 0x18 */
    int hull0;           /* 0x1C */
    int clipnodes;       /* 0x20 */
    int submodels;       /* 0x24 */
    int surfaces;        /* 0x28 */
    int marksurfaces;    /* 0x2C */
    int visdata;         /* 0x30 */
    int lightdata;       /* 0x34 */
    int textures;        /* 0x38 */
    int entities;        /* 0x3C */
    int unk_40[17];
    int numplanes;       /* 0x84 */
    int numnodes;        /* 0x88 */
    int numleafs;        /* 0x8C */
    int numedges;        /* 0x90 */
    int numsurfedges;    /* 0x94 */
    int numvertexes;     /* 0x98 */
    int unk_9C;
    int numclipnodes;    /* 0xA0 */
    int numsubmodels;    /* 0xA4 */
    int numsurfaces;     /* 0xA8 */
    int nummarksurfaces; /* 0xAC */
    int unk_B0[2];
    int numtextures;     /* 0xB8 */
} bs2header_t;

typedef struct
{
    byte pad0[0x13];
    byte nummips;            /* 0x13 */
    byte pad14[4];
    unsigned short width;    /* 0x18 */
    unsigned short height;   /* 0x1A */
    unsigned short width2;   /* 0x1C */
    unsigned short height2;  /* 0x1E */
    byte palette[0x400];     /* 0x20 */
    byte pixels[1];          /* 0x420 */
} gl_spriteframe_t;

typedef struct
{
    int gl_texturenum;
    gl_spriteframe_t *frame;
} gl_spriteframedesc_t;

typedef struct
{
    byte pad0[6];
    byte flags;                       /* 0x06 */
    byte numframes;                   /* 0x07 */
    gl_spriteframedesc_t frames[1];   /* 0x08 */
} gl_sprite_t;

void func_002A0028(char *buf, model_t *mod, int frame);
void GL_FreeDecalTexture(char *name);
int sprintf(char *, const char *, ...);

bs2header_t *func_002A0450(model_t *mod);
void func_0029FAB0(model_t *mod);
void func_0029FCF8(model_t *mod, bs2header_t *header);
void func_0029FE20(model_t *mod);
int GL_LoadDecalTexture(char *name, int type, int width, int height, byte *pixels, byte *palette, byte nummips);

#define MAX_KNOWN_MODELS 512
#define IDSTUDIOHEADER 0x54534449 /* "IDST" */
#define IDSPRITEHEADER_PS2 0x5A415053 /* "SPAZ" */
#define mod_studio 3
#define fabs(x) ((x) < 0 ? -(x) : (x))

extern model_t *mod_known;
extern model_t *mod_global;
extern model_t *mod_game;
extern model_t *mod_level;
extern int mod_numlevel;
extern int mod_numgame;
extern int mod_numglobal;
extern int mod_numknown;

void *Mem_DbgMalloc(int size, const char *file, int line);
void *memset(void *, int, unsigned int);
char *strcpy(char *, const char *);
void Mod_LoadStudioModel(model_t *mod, void *buffer);
void func_002A01B8(model_t *mod, gl_sprite_t *psprite);
void *COM_LoadFile(char *path, int usehunk, int *pLength);
void func_002AF9A0(int texnum);
void *Cache_Check(cache_user_t *c);
void Cache_Free(cache_user_t *c);
FileHandle_t FS_Open(char *name, char *mode);
void FS_Close(FileHandle_t f);
void func_00222500(FileHandle_t f, char *fmt, ...);
model_t *func_0029F908(model_t *mod, qboolean crash);
model_t *func_0029F760(char *name);
void func_0029FED0(model_t *mod);
void func_002A0100(model_t *mod);
void func_002A0180(model_t *mod);
void Mod_LoadStudioModelCached(model_t *mod);
int strcasecmp(const char *, const char *);
char *strstr(const char *, const char *);
extern model_t *D_003994C0;
extern cvar_t developer;

void Mod_InitKnown(void)
{
    mod_known = Mem_DbgMalloc(MAX_KNOWN_MODELS * sizeof(model_t), "engine/Gl_model.c", 61);
    memset(mod_known, 0, MAX_KNOWN_MODELS * sizeof(model_t));
    mod_global = mod_known;
    mod_game = NULL;
    mod_level = NULL;
    mod_numknown = 0;
    mod_numglobal = 0;
    mod_numgame = 0;
    mod_numlevel = 0;
}

void Mod_LoadGlobalModel(char *name, void *buffer)
{
    model_t *mod;

    mod = &mod_global[mod_numglobal++];
    mod_numknown++;
    strcpy(mod->name, name);
    mod->unk_198 = 0;
    mod->needload = 0;

    switch (*(unsigned int *)buffer)
    {
    case IDSTUDIOHEADER:
        Mod_LoadStudioModel(mod, buffer);
        break;
    case 0x50534449:
        break;
    case IDSPRITEHEADER_PS2:
        func_002A01B8(mod, buffer);
        break;
    }
}


void Mod_EndGlobalModels(void)
{
    mod_game = &mod_global[mod_numglobal];
}

void Mod_LoadGameModel(char *name, void *buffer)
{
    model_t *mod;

    mod = &mod_game[mod_numgame++];
    mod_numknown++;
    strcpy(mod->name, name);
    mod->needload = 0;
    mod->unk_198 = 1;

    switch (*(unsigned int *)buffer)
    {
    case IDSTUDIOHEADER:
        Mod_LoadStudioModel(mod, buffer);
        break;
    case 0x50534449:
        break;
    case IDSPRITEHEADER_PS2:
        func_002A01B8(mod, buffer);
        break;
    }
}

void Mod_ClearLevelModels(void)
{
    int i;

    for (i = 0; i < mod_numlevel; i++)
    {
        if (mod_level[i].type == mod_studio)
        {
            if (Cache_Check(&mod_level[i].cache))
                Cache_Free(&mod_level[i].cache);
        }
    }

    memset(mod_game, 0, mod_numgame * sizeof(model_t));
    memset(mod_level, 0, mod_numlevel * sizeof(model_t));
    mod_numknown = mod_numknown - mod_numlevel - mod_numgame;
    mod_numgame = 0;
    mod_numlevel = 0;
    mod_level = NULL;
}

void Mod_EndGameModels(void)
{
    mod_level = &mod_game[mod_numgame];
}

#ifdef NON_MATCHING
void Mod_Report_f(void)
{
    FileHandle_t f;
    int i;

    f = FS_Open("Model Report.txt", "w");
    func_00222500(f, "Models Loaded: [%d]\r\n", mod_numknown);
    func_00222500(f, "       Global: [%d]\r\n", mod_numglobal);
    func_00222500(f, "         Game: [%d]\r\n", mod_numgame);
    func_00222500(f, "        Level: [%d]\r\n", mod_numlevel);

    func_00222500(f, "\r\n\tGlobal Models\r\n");
    for (i = 0; i < mod_numglobal; i++)
        func_00222500(f, "[%s]\r\n", mod_global[i].name);

    func_00222500(f, "\r\n\tGame Models\r\n");
    for (i = 0; i < mod_numgame; i++)
        func_00222500(f, "[%s]\r\n", mod_game[i].name);

    func_00222500(f, "\r\n\tLevel Models\r\n");
    for (i = 0; i < mod_numlevel; i++)
        func_00222500(f, "[%s]\r\n", mod_level[i].name);

    FS_Close(f);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/Gl_model", Mod_Report_f);
#endif

void *Mod_Extradata(model_t *mod)
{
    if (mod->unk_198 == 2)
    {
        if (!Cache_Check(&mod->cache))
            func_0029F908(mod, true);

        if (!mod->cache.data)
            Sys_Error("Mod_Extradata: caching failed");
    }

    return mod->cache.data;
}

mleaf_t *Mod_PointInLeaf(vec3_t p, model_t *model)
{
    mnode_t *node;
    float d;
    mplane_t *plane;

    if (!model || !model->nodes)
        Sys_Error("Mod_PointInLeaf: bad model");

    node = model->nodes;
    while (1)
    {
        if (node->contents < 0)
            return (mleaf_t *)node;

        plane = node->plane;
        d = DotProduct(p, plane->normal) - plane->dist;
        if (d > 0)
            node = node->children[0];
        else
            node = node->children[1];
    }
}

void func_0029F6A8(void)
{
    int i;
    model_t *mod;

    for (i = 0, mod = mod_level; i < mod_numlevel; i++, mod++)
    {
        if (mod->type == mod_sprite && !mod->needload)
            func_002A0100(mod);

        if (mod->type != mod_alias && mod->needload != 3)
            mod->needload = 1;
    }
}

#ifdef NON_MATCHING
model_t *func_0029F760(char *name)
{
    int i;
    model_t *mod;
    qboolean found;

    found = false;
    mod = NULL;

    for (i = 0; i < mod_numglobal; i++)
    {
        if (!strcasecmp(mod_global[i].name, name))
        {
            found = true;
            mod = &mod_global[i];
            break;
        }
    }

    if (found)
        return mod;

    for (i = 0; i < mod_numgame; i++)
    {
        if (!strcasecmp(mod_game[i].name, name))
        {
            found = true;
            mod = &mod_game[i];
            break;
        }
    }

    if (found)
        return mod;

    for (i = 0; i < mod_numlevel; i++)
    {
        if (!strcasecmp(mod_level[i].name, name))
        {
            found = true;
            mod = &mod_level[i];
            break;
        }
    }

    if (found)
        return mod;

    mod = &mod_level[i];
    memset(mod, 0, sizeof(model_t));
    strcpy(mod->name, name);
    mod->unk_198 = 2;
    mod->needload = 1;
    mod_numknown++;
    mod_numlevel++;
    return mod;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/Gl_model", func_0029F760);
#endif

model_t *func_0029F908(model_t *mod, qboolean crash)
{
    if (!strstr(mod->name, ".bs2"))
    {
        if (mod->type == mod_alias || mod->type == mod_studio)
        {
            if ((unsigned int)mod->unk_198 < 2)
                return mod;

            if (Cache_Check(&mod->cache))
            {
                mod->needload = 0;
                return mod;
            }
        }
        else
        {
            if (!mod->needload)
                return mod;
            if (mod->needload == 3)
                return mod;
        }

        if (developer.value > 1)
            Con_DPrintf("loading %s\n", mod->name);

        mod->needload = 0;

        if (strstr(mod->name, ".dol"))
        {
            Mod_LoadStudioModelCached(mod);
            return mod;
        }

        if (strstr(mod->name, ".spz"))
            func_002A0180(mod);

        return mod;
    }

    if (mod->needload)
    {
        D_003994C0 = mod;
        func_0029FED0(mod);
    }

    return mod;
}

model_t *Mod_ForName(char *name, qboolean crash)
{
    model_t *mod;

    mod = func_0029F760(name);
    return func_0029F908(mod, crash);
}

void func_0029FA50(model_t *mod)
{
    mod->needload = 3;
}

float func_0029FA60(vec3_t mins, vec3_t maxs)
{
    int i;
    vec3_t corner;

    for (i = 0; i < 3; i++)
        corner[i] = fabs(mins[i]) > fabs(maxs[i]) ? fabs(mins[i]) : fabs(maxs[i]);

    return Length(corner);
}

#ifdef NON_MATCHING
void func_0029FAB0(model_t *mod)
{
    unsigned int i, j;
    unsigned int numsubmodels;
    dmodel_t *bm;
    char name[10];

    numsubmodels = mod->numsubmodels;

    for (i = 0; i < numsubmodels; i++)
    {
        bm = &mod->submodels[i];

        mod->hulls[0].firstclipnode = bm->headnode[0];
        for (j = 1; j < 4; j++)
        {
            mod->hulls[j].firstclipnode = bm->headnode[j];
            mod->hulls[j].lastclipnode = mod->numclipnodes - 1;
        }

        mod->firstmodelsurface = bm->firstface;
        mod->nummodelsurfaces = bm->numfaces;

        VectorCopy(bm->maxs, mod->maxs);
        VectorCopy(bm->mins, mod->mins);

        mod->radius = func_0029FA60(mod->mins, mod->maxs);
        mod->numleafs = bm->visleafs;

        if (i < mod->numsubmodels - 1)
        {
            sprintf(name, "*%i", i + 1);
            D_003994C0 = func_0029F760(name);
            *D_003994C0 = *mod;
            Q_strcpy(D_003994C0->name, name);
            mod = D_003994C0;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/Gl_model", func_0029FAB0);
#endif

void func_0029FCF8(model_t *mod, bs2header_t *header)
{
    hull_t *hull;

    hull = &D_003994C0->hulls[0];
    hull->clipnodes = (dclipnode_t *)((byte *)header + header->hull0);
    hull->firstclipnode = 0;
    hull->lastclipnode = mod->numnodes - 1;
    hull->planes = D_003994C0->planes;

    hull = &D_003994C0->hulls[1];
    hull->clipnodes = mod->clipnodes;
    hull->firstclipnode = 0;
    hull->lastclipnode = mod->numclipnodes - 1;
    hull->planes = D_003994C0->planes;
    hull->clip_mins[0] = -16;
    hull->clip_mins[1] = -16;
    hull->clip_mins[2] = -36;
    hull->clip_maxs[0] = 16;
    hull->clip_maxs[1] = 16;
    hull->clip_maxs[2] = 36;

    hull = &D_003994C0->hulls[2];
    hull->clipnodes = mod->clipnodes;
    hull->firstclipnode = 0;
    hull->lastclipnode = mod->numclipnodes - 1;
    hull->planes = D_003994C0->planes;
    hull->clip_mins[0] = -32;
    hull->clip_mins[1] = -32;
    hull->clip_mins[2] = -32;
    hull->clip_maxs[0] = 32;
    hull->clip_maxs[1] = 32;
    hull->clip_maxs[2] = 32;

    hull = &D_003994C0->hulls[3];
    hull->clipnodes = mod->clipnodes;
    hull->firstclipnode = 0;
    hull->lastclipnode = mod->numclipnodes - 1;
    hull->planes = D_003994C0->planes;
    hull->clip_mins[0] = -16;
    hull->clip_mins[1] = -16;
    hull->clip_mins[2] = -18;
    hull->clip_maxs[0] = 16;
    hull->clip_maxs[1] = 16;
    hull->clip_maxs[2] = 18;
}

void func_0029FE20(model_t *mod)
{
    unsigned int i;
    unsigned int num;
    texture_t *tx;

    num = mod->numtextures;
    for (i = 0; i < num; i++)
    {
        tx = &mod->textures[i];
        if (Q_strncmp(tx->name, "sky", 3))
            tx->gl_texturenum = GL_LoadDecalTexture(tx->name, 4, tx->width, tx->height, tx->pixels, tx->palette, tx->nummips);
    }
}

void func_0029FED0(model_t *mod)
{
    bs2header_t *header;

    header = func_002A0450(mod);
    mod->type = mod_brush;
    mod->needload = 0;

    mod->numsubmodels = header->numsubmodels;
    mod->numplanes = header->numplanes;
    mod->numleafs = header->numleafs;
    mod->numvertexes = header->numvertexes;
    mod->numedges = header->numedges;
    mod->numnodes = header->numnodes;
    mod->numsurfaces = header->numsurfaces;
    mod->numsurfedges = header->numsurfedges;
    mod->numclipnodes = header->numclipnodes;
    mod->numtextures = header->numtextures;
    mod->nummarksurfaces = header->nummarksurfaces;

    mod->submodels = (void *)((byte *)header + header->submodels);
    mod->planes = (void *)((byte *)header + header->planes);
    mod->leafs = (void *)((byte *)header + header->leafs);
    mod->vertexes = (void *)((byte *)header + header->vertexes);
    mod->edges = (void *)((byte *)header + header->edges);
    mod->nodes = (void *)((byte *)header + header->nodes);
    mod->surfaces = (void *)((byte *)header + header->surfaces);
    mod->surfedges = (void *)((byte *)header + header->surfedges);
    mod->clipnodes = (void *)((byte *)header + header->clipnodes);
    mod->textures = (void *)((byte *)header + header->textures);
    mod->marksurfaces = (void *)((byte *)header + header->marksurfaces);
    mod->visdata = (void *)((byte *)header + header->visdata);
    mod->lightdata = (void *)((byte *)header + header->lightdata);
    mod->entities = (char *)header + header->entities;
    mod->flags = 0;
    mod->numframes = 2;

    func_0029FCF8(mod, header);
    func_0029FAB0(mod);
    func_0029FE20(mod);
}

void func_002A0028(char *buf, model_t *mod, int frame)
{
    sprintf(buf, "%s_%i", mod->name, frame);
}

void func_002A0050(model_t *mod)
{
    int i;
    char name[256];
    gl_sprite_t *psprite;

    if (mod->type != mod_sprite)
        return;

    mod->needload = 1;
    psprite = (gl_sprite_t *)mod->cache.data;
    if (!psprite)
        return;

    {
        int *p = &psprite->frames[0].gl_texturenum;
        for (i = 0; i < psprite->numframes; i++)
            *p = 0;
    }

    for (i = 0; i < psprite->numframes; i++)
    {
        func_002A0028(name, mod, i);
        GL_FreeDecalTexture(name);
    }
}

void func_002A0100(model_t *mod)
{
    int i;
    gl_sprite_t *psprite;
    gl_spriteframedesc_t *pdesc;

    if (mod->type != mod_sprite)
        return;

    mod->needload = 1;
    psprite = (gl_sprite_t *)mod->cache.data;
    if (!psprite)
        return;

    {
        gl_spriteframedesc_t *frames = psprite->frames;
        for (i = 0; i < psprite->numframes; i++)
        {
            func_002AF9A0(frames[i].gl_texturenum);
            frames[i].gl_texturenum = 0;
        }
    }
}

void func_002A0180(model_t *mod)
{
    int length;

    func_002A01B8(mod, COM_LoadFile(mod->name, 1, &length));
}

void func_002A01B8(model_t *mod, gl_sprite_t *psprite)
{
    int i;
    char name[256];
    gl_spriteframe_t *frame;
    gl_spriteframedesc_t *frames;
    byte *pixels, *palette;

    mod->cache.data = psprite;

    frames = psprite->frames;
    for (i = 0; i < psprite->numframes; i++)
    {
        if (!(psprite->flags & 1))
            frames[i].frame = (gl_spriteframe_t *)((int)frames[i].frame + (int)psprite);
        frame = frames[i].frame;
        palette = frame->palette;
        pixels = frame->pixels;
        func_002A0028(name, mod, i);
        frames[i].gl_texturenum = GL_LoadDecalTexture(name, 5, frame->width, frame->height, pixels, palette, frame->nummips);
    }

    psprite->flags |= 1;
    frame = psprite->frames[0].frame;
    mod->mins[0] = mod->mins[1] = -frame->width2 / 2;
    mod->maxs[0] = mod->maxs[1] = frame->width2 / 2;
    mod->mins[2] = -frame->height2 / 2;
    mod->maxs[2] = frame->height2 / 2;
    mod->type = mod_sprite;
    mod->synctype = 0;
}

INCLUDE_ASM("asm/nonmatchings/engine/Gl_model", func_002A0320);

INCLUDE_ASM("asm/nonmatchings/engine/Gl_model", func_002A03A8);
