#include "common.h"
#include "engine/mathlib.h"
#include "engine/model.h"
#include "engine/cvar.h"
#include "engine/pmove.h"

#define PM_GLASS_IGNORE 4

#define fabs(x) ((x) < 0 ? -(x) : (x))

typedef struct cl_entity_s
{
    byte pad0[0x10];
    int rendermode;             /* 0x10 */
    int renderamt;              /* 0x14 */
    int renderfx;               /* 0x18 */
    int movetype;               /* 0x1C */
    int body;                   /* 0x20 */
    byte pad24[0x2C - 0x24];
    int index;                  /* 0x2C */
    byte pad30[0x34 - 0x30];
    int skin;                   /* 0x34 */
    byte pad38[0x44 - 0x38];
    int aiment;                 /* 0x44 */
    byte pad48[0x50 - 0x48];
    model_t *model;             /* 0x50 */
    byte pad54[0x60 - 0x54];
    float frame;                /* 0x60 */
    byte pad64[0x6C - 0x64];
    float scale;                /* 0x6C */
    byte pad70[0x84 - 0x70];
    vec3_t origin;              /* 0x84 */
} cl_entity_t;

typedef struct
{
    byte pad0[0xF8];
} player_state_t; /* 0xF8 */

typedef struct
{
    byte pad0[0x34];
    player_state_t playerstate[3]; /* 0x34 */
    byte pad31C[0x330 - 0x31C];
} frame_t; /* 0x330 */

typedef struct
{
    byte pad0[0x210];
    int parsecount;             /* 0x210 */
    byte pad214[0x388 - 0x214];
    int unk_388;                /* 0x388 */
    byte pad38C[0x3AC - 0x38C];
    frame_t *frames;            /* 0x3AC */
    byte pad3B0[0x13E0 - 0x3B0];
    int maxclients;             /* 0x13E0 */
    byte pad13E4[0x212C - 0x13E4];
} client_state_t;

extern client_state_t cl[2];
extern int cl_updatemask[2];

typedef struct
{
    unsigned int r, g, b, a;
} colorVec;

typedef struct
{
    float v[4];
} __attribute__((aligned(16))) qvec_t;

typedef struct
{
    byte pad0[0x1C];
    unsigned short width;       /* 0x1C */
    unsigned short height;      /* 0x1E */
} gltexture_t;

typedef struct
{
    int gl_texturenum;          /* 0x00 */
    gltexture_t *texture;       /* 0x04 */
} mspriteframe_t;

typedef struct
{
    byte pad0[4];
    byte type;                  /* 0x04 */
} msprite_t;

#define SPRITE_BUFVERTS 0x4A
#define UNCACHED(p) ((void *)((unsigned int)(p) | 0x30000000))

#define GL_DEPTH_TEST 0x0B71
#define GL_CULL_FACE 0x0B44
#define GL_ALPHA_TEST 0x0BC0
#define GL_BLEND 0x0BE2
#define GL_ONE 1
#define GL_SRC_ALPHA 0x0302
#define GL_ONE_MINUS_SRC_ALPHA 0x0303
#define GL_ALPHA 0x1906
#define GL_REPLACE 0x1E01
#define GL_MODULATE 0x2100
#define GL_TEXTURE_ENV_MODE 0x2200
#define GL_TEXTURE_ENV 0x2300

extern qvec_t sprite_color;
extern qvec_t *sprite_vertptr;
extern qvec_t *sprite_colorptr;
extern int sprite_buffer;
extern int sprite_numverts;
extern int sprite_firsttime;
extern qvec_t sprite_verts[2][SPRITE_BUFVERTS];
extern qvec_t sprite_colors[2][SPRITE_BUFVERTS];
extern qvec_t sprite_st[2][SPRITE_BUFVERTS];
extern int D_00399570;
extern int D_00399558;
extern int D_0033102C;
extern qvec_t *D_00331030;
extern qvec_t *D_00331034;
extern qvec_t *D_00331038;
extern int D_0033103C;
extern int D_00331044;

void glTexEnvf(int target, int pname, float param);
void glBlendFunc(int sfactor, int dfactor);
void glEnable(int cap);
void glDisable(int cap);
void glDepthMask(int flag);
void GL_Bind(int texnum);
void func_0023AF68(void);
void func_0023D520(void);
int func_0023D5F0(void);
void func_0023D618(int);
float *R_GetAttachmentPoint(int entity, int attachment, int clientId);
mspriteframe_t *R_GetSpriteFrame(msprite_t *psprite, int frame);
void R_SpriteColor(colorVec *color, cl_entity_t *ent, int alpha);
void R_GetSpriteAxes(cl_entity_t *ent, int type, vec3_t forward, vec3_t right, vec3_t up);
void R_StudioDrawPlayer(int flags, player_state_t *pplayer, int clientId);
void R_StudioDrawModel(int flags, int clientId, int unk);
void VectorMA(const vec3_t veca, float scale, const vec3_t vecb, vec3_t vecc);

typedef struct
{
    cl_entity_t *pEnt;
    float distance;
} transObjRef;

extern int numTransObjs[2];
extern int maxTransObjs[2];
extern transObjRef *transObjects[2];
extern cl_entity_t *currententity;
extern float r_blend;
extern vec3_t r_origin;
extern vec3_t r_entorigin;
extern cvar_t r_drawentities;
extern cvar_t r_traceglow;
extern model_t *sv_worldmodel;

void Con_Printf(const char *fmt, ...);
void Sys_Error(const char *error, ...);
void *Mem_DbgMalloc(int size, const char *file, int line);
void Mem_Free(void *p);
void *memset(void *s, int c, unsigned int n);
int CL_FxBlend(cl_entity_t *ent);
void R_DrawBrushModel(cl_entity_t *ent, int clientId);
void func_0023AFC8(int);

void AllocTransObjectsData(int nMax, int clientId)
{
    if (transObjects[clientId])
        Con_Printf("Transparent objects reallocate\n");

    transObjects[clientId] = Mem_DbgMalloc(nMax * sizeof(transObjRef), "engine/r_trans.c", 51);
    memset(transObjects[clientId], 0, nMax * sizeof(transObjRef));
    maxTransObjs[clientId] = nMax;
}

void R_DestroyObjects(int clientId)
{
    if (transObjects[clientId])
    {
        Mem_Free(transObjects[clientId]);
        transObjects[clientId] = NULL;
    }
    maxTransObjs[clientId] = 0;
}

#ifdef NON_MATCHING
int R_RecursiveTestLine(mnode_t *node, vec3_t start, vec3_t end)
{
    float front, back, frac;
    int side, r;
    vec3_t mid;
    mplane_t *plane;

    if (node->contents < 0)
        return node->contents;

    plane = node->plane;
    if (!plane)
        return -2;

    front = DotProduct(start, plane->normal) - plane->dist;
    back = DotProduct(end, plane->normal) - plane->dist;

    if (front >= -0.1f && back >= -0.1f)
    {
        if (!node->children[0])
            return node->contents;
        return R_RecursiveTestLine(node->children[0], start, end);
    }
    if (front < 0.1f && back < 0.1f)
    {
        if (!node->children[1])
            return node->contents;
        return R_RecursiveTestLine(node->children[1], start, end);
    }

    side = front < 0;
    frac = front / (front - back);
    mid[0] = start[0] + (end[0] - start[0]) * frac;
    mid[1] = start[1] + (end[1] - start[1]) * frac;
    mid[2] = start[2] + (end[2] - start[2]) * frac;

    r = R_RecursiveTestLine(node->children[side], start, mid);
    if (r != -1)
        return r;
    return R_RecursiveTestLine(node->children[!side], mid, end);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/r_trans", R_RecursiveTestLine);
#endif

int R_TestLine(vec3_t start, vec3_t end)
{
    return R_RecursiveTestLine(sv_worldmodel->nodes, start, end);
}

float GlowBlend(cl_entity_t *entity)
{
    vec3_t tmp;
    float dist, brightness;
    pmtrace_t trace;

    VectorSubtract(r_entorigin, r_origin, tmp);
    dist = Length(tmp);

    pmove[0].usehull = 2;
    if (R_TestLine(r_origin, r_entorigin) == -2)
        return 0;

    if (r_traceglow.value)
        trace = PM_PlayerTrace(r_origin, r_entorigin, PM_GLASS_IGNORE, 0);
    else
        trace = PM_PlayerTrace(r_origin, r_entorigin, PM_GLASS_IGNORE | PM_STUDIO_IGNORE, 0);

    if ((1.0f - trace.fraction) * dist > 8)
        return 0;

    if (entity->renderfx == 14 || entity->renderfx == 21 || entity->renderfx == 22)
        return entity->renderamt * (1.0f / 255.0f);

    brightness = 19000.0f / (dist * dist);
    if (brightness < 0.05f)
        brightness = 0.05f;
    if (brightness > 1.0f)
        brightness = 1.0f;

    entity->scale = dist * (1.0f / 200.0f);
    return brightness;
}

void R_RotatedBBoxNoAngles(vec3_t mins, vec3_t maxs, vec3_t angles, vec3_t outmins, vec3_t outmaxs)
{
    int i;
    float v, max;

    if (angles[0] == 0 && angles[1] == 0 && angles[2] == 0)
    {
        VectorCopy(mins, outmins);
        VectorCopy(maxs, outmaxs);
        return;
    }

    max = 0;
    for (i = 0; i < 3; i++)
    {
        v = fabs(mins[i]);
        if (v > max)
            max = v;
        v = fabs(maxs[i]);
        if (v > max)
            max = v;
    }
    for (i = 0; i < 3; i++)
    {
        outmins[i] = -max;
        outmaxs[i] = max;
    }
}

void AddTEntity(cl_entity_t *pEnt, int clientId)
{
    int i;
    float dist = 1000000000.0f;
    vec3_t v;

    if (numTransObjs[clientId] >= maxTransObjs[clientId])
        Sys_Error("AddTentity: Too many objects");

    if (!pEnt->model || pEnt->model->type != 0 || pEnt->rendermode != 4)
    {
        VectorAdd(pEnt->model->mins, pEnt->model->maxs, v);
        VectorScale(v, 0.5f, v);
        VectorAdd(v, pEnt->origin, v);
        VectorSubtract(r_origin, v, v);
        dist = DotProduct(v, v);
    }

    for (i = numTransObjs[clientId]; i > 0; i--)
    {
        if (transObjects[clientId][i - 1].distance < dist)
        {
        transObjects[clientId][i].pEnt = transObjects[clientId][i - 1].pEnt;
        transObjects[clientId][i].distance = transObjects[clientId][i - 1].distance;
        }
        else
            break;
    }

    transObjects[clientId][i].pEnt = pEnt;
    transObjects[clientId][i].distance = dist;
    numTransObjs[clientId]++;
}

void AppendTEntity(cl_entity_t *pEnt, int clientId)
{
    vec3_t v;
    float dist;

    if (numTransObjs[clientId] >= maxTransObjs[clientId])
        Sys_Error("AddTentity: Too many objects");

    VectorAdd(pEnt->model->mins, pEnt->model->maxs, v);
    VectorScale(v, 0.5f, v);
    VectorAdd(v, pEnt->origin, v);
    VectorSubtract(r_origin, v, v);
    dist = DotProduct(v, v);

    transObjects[clientId][numTransObjs[clientId]].pEnt = pEnt;
    transObjects[clientId][numTransObjs[clientId]].distance = dist;
    numTransObjs[clientId]++;
}

void R_DrawTEntitiesOnList(int clientId)
{
    int i;

    if (r_drawentities.value == 0)
        return;

    if (cl[clientId].unk_388 < 3)
        func_0023AFC8(1);

    for (i = 0; i < numTransObjs[clientId]; i++)
    {
        currententity = transObjects[clientId][i].pEnt;
        r_blend = CL_FxBlend(currententity);
        if (r_blend <= 0)
            continue;

        r_blend = r_blend * (1.0f / 255.0f);

        if (currententity->rendermode == 3 && currententity->model->type != 1)
            Con_Printf("Non-sprite set to glow!\n");

        if (currententity->model->type == 0)
        {
            if (cl[clientId].unk_388 >= 3)
                R_DrawBrushModel(currententity, clientId);
            else
                R_DrawBrushModel(currententity, clientId);
        }
    }

    r_blend = 1.0f;
}

/* constante usada só por função ainda em asm (o splat não a leva para o .s) */
__asm__(".section .rodata\n"
        ".align 3\n.globl D_0038E498\nD_0038E498: .word 0xFF, 0xFF, 0xFF, 0xFF\n"
        ".section .text\n");

#ifdef NON_MATCHING
void R_SpriteSetColor(colorVec *color)
{
    int i;
    qvec_t *p;

    sprite_color.v[0] = color->r * (1.0f / 255.0f);
    sprite_color.v[1] = color->g * (1.0f / 255.0f);
    sprite_color.v[2] = color->b * (1.0f / 255.0f);
    sprite_color.v[3] = 1.0f;

    p = sprite_colorptr;
    for (i = 0; i < 4; i++)
        *p++ = sprite_color;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/r_trans", R_SpriteSetColor);
#endif

int R_SpriteSetRenderMode(cl_entity_t *ent, int *pMode)
{
    int oldmode;

    if (ent->rendermode == 0)
        r_blend = 1.0f;

    oldmode = *pMode;
    if (ent->rendermode == 1)
    {
        if (oldmode != 1)
        {
            *pMode = 1;
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ALPHA);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(1);
        }
    }
    else if (ent->rendermode == 5)
    {
        if (oldmode != 5)
        {
            *pMode = 5;
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glBlendFunc(GL_ONE, GL_ONE);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(0);
        }
    }
    else if (ent->rendermode == 3)
    {
        if (oldmode != 3)
        {
            *pMode = 3;
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glBlendFunc(GL_ONE, GL_ONE);
            glDisable(GL_DEPTH_TEST);
            glDepthMask(0);
        }
    }
    else if (ent->rendermode == 4)
    {
        if (oldmode != 4)
        {
            *pMode = 4;
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(0);
        }
    }
    else
    {
        if (oldmode != -1)
        {
            *pMode = -1;
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_DEPTH_TEST);
            glDepthMask(1);
        }
    }

    return oldmode != *pMode;
}

#ifdef NON_MATCHING
void R_SpriteInitST(void)
{
    int i, j;

    for (i = 0; i < 2; i++)
    {
        for (j = 0; j + 4 < SPRITE_BUFVERTS; j += 4)
        {
            sprite_st[i][j + 0].v[0] = 0;
            sprite_st[i][j + 0].v[1] = 0;
            sprite_st[i][j + 0].v[2] = 1.0f;
            sprite_st[i][j + 0].v[3] = 0;
            sprite_st[i][j + 1].v[0] = 1.0f;
            sprite_st[i][j + 1].v[1] = 0;
            sprite_st[i][j + 1].v[2] = 1.0f;
            sprite_st[i][j + 1].v[3] = 0;
            sprite_st[i][j + 2].v[0] = 1.0f;
            sprite_st[i][j + 2].v[1] = 1.0f;
            sprite_st[i][j + 2].v[2] = 1.0f;
            sprite_st[i][j + 2].v[3] = 0;
            sprite_st[i][j + 3].v[0] = 0;
            sprite_st[i][j + 3].v[1] = 1.0f;
            sprite_st[i][j + 3].v[2] = 1.0f;
            sprite_st[i][j + 3].v[3] = 0;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/r_trans", R_SpriteInitST);
#endif

void R_SpriteFlush(void)
{
    if (sprite_numverts > 0)
    {
        D_00399570++;
        D_00399558 += sprite_numverts;
        sprite_numverts = 0;
        func_0023D520();
        sprite_buffer = !sprite_buffer;
    }
}

#ifdef NON_MATCHING
void R_SpriteBeginQuad(void)
{
    int done = 0;
    int n;

    D_00331044 = 5;
    if (sprite_numverts > 0 && (n = sprite_numverts + 4) < SPRITE_BUFVERTS)
    {
        D_00331038 = &sprite_st[sprite_buffer][sprite_numverts];
        sprite_vertptr = UNCACHED(&sprite_verts[sprite_buffer][sprite_numverts]);
        sprite_colorptr = UNCACHED(&sprite_colors[sprite_buffer][sprite_numverts]);
        D_0033103C = 4;
        D_00331030 = &sprite_verts[sprite_buffer][sprite_numverts];
        D_00331034 = &sprite_colors[sprite_buffer][sprite_numverts];
        sprite_numverts = n;
        D_0033102C = 1;
        if (func_0023D5F0() == 1)
        {
            done = 1;
            func_0023D618(0);
        }
        D_0033102C = 0;
    }

    if (!done)
    {
        R_SpriteFlush();
        D_00331038 = &sprite_st[sprite_buffer][0];
        sprite_vertptr = UNCACHED(&sprite_verts[sprite_buffer][0]);
        sprite_colorptr = UNCACHED(&sprite_colors[sprite_buffer][0]);
        sprite_numverts = 4;
        D_0033102C = 1;
        D_00331030 = &sprite_verts[sprite_buffer][0];
        D_00331034 = &sprite_colors[sprite_buffer][0];
        D_0033103C = 4;
        func_0023D618(0);
        D_0033102C = 0;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/r_trans", R_SpriteBeginQuad);
#endif

#define SPRITE_VERTEX(p) \
    { \
        sprite_vertptr->v[0] = (p)[0]; \
        sprite_vertptr->v[1] = (p)[1]; \
        sprite_vertptr->v[2] = (p)[2]; \
        sprite_vertptr->v[3] = 1.0f; \
        sprite_vertptr++; \
    }

#ifdef NON_MATCHING
void R_DrawSpriteTEntities(int clientId)
{
    colorVec color = {255, 255, 255, 255};
    vec3_t point, forward, right, up;
    int i;
    int rendermode;
    int lasttexture;
    msprite_t *psprite;
    mspriteframe_t *frame;
    float fup, fdown, fleft, fright, scale;

    if (sprite_firsttime == 1)
        R_SpriteInitST();

    lasttexture = -1;
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    rendermode = -2;

    for (i = 0; i < numTransObjs[clientId]; i++)
    {
        currententity = transObjects[clientId][i].pEnt;
        r_blend = CL_FxBlend(currententity);
        if (r_blend <= 0)
            continue;

        r_blend = r_blend * (1.0f / 255.0f);

        if (currententity->model->type != 1)
            continue;

        if (currententity->body)
        {
            VectorCopy(R_GetAttachmentPoint(currententity->skin, currententity->body, clientId), r_entorigin);
        }
        else
        {
            VectorCopy(currententity->origin, r_entorigin);
        }

        if (currententity->rendermode == 3)
            r_blend *= GlowBlend(currententity);

        if (r_blend == 0)
            continue;

        if (currententity->renderfx == 21 && clientId)
            continue;
        if (currententity->renderfx == 22 && !clientId)
            continue;

        psprite = currententity->model->cache.data;
        frame = R_GetSpriteFrame(psprite, currententity->frame);

        if (R_SpriteSetRenderMode(currententity, &rendermode))
            R_SpriteFlush();

        if (frame->gl_texturenum != lasttexture)
        {
            R_SpriteFlush();
            lasttexture = frame->gl_texturenum;
            GL_Bind(lasttexture);
            func_0023AF68();
        }

        R_SpriteBeginQuad();
        R_SpriteColor(&color, currententity, r_blend * 255);
        R_SpriteSetColor(&color);
        R_GetSpriteAxes(currententity, psprite->type, forward, right, up);

        fleft = -(frame->texture->width >> 1);
        fdown = -(frame->texture->height >> 1);
        fright = frame->texture->width >> 1;
        fup = frame->texture->height >> 1;

        if (currententity->renderfx == 21 || currententity->renderfx == 22)
        {
            fup = 1;
            fleft = -1;
            fright = fup;
            fdown = fleft;
        }

        scale = currententity->scale;
        if (scale > 0)
        {
            fright *= scale;
            fup *= scale;
            fdown *= scale;
            fleft *= scale;
        }

        VectorMA(r_entorigin, fup, up, point);
        VectorMA(point, fleft, right, point);
        SPRITE_VERTEX(point);

        VectorMA(r_entorigin, fup, up, point);
        VectorMA(point, fright, right, point);
        SPRITE_VERTEX(point);

        VectorMA(r_entorigin, fdown, up, point);
        VectorMA(point, fright, right, point);
        SPRITE_VERTEX(point);

        VectorMA(r_entorigin, fdown, up, point);
        VectorMA(point, fleft, right, point);
        SPRITE_VERTEX(point);
    }

    R_SpriteFlush();
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glDepthMask(1);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/r_trans", R_DrawSpriteTEntities);
#endif

#ifdef NON_MATCHING
void R_DrawStudioTEntities(int clientId)
{
    int i, j;

    if (r_drawentities.value == 0)
        return;

    func_0023AFC8(2);
    R_DrawSpriteTEntities(clientId);

    for (i = 0; i < numTransObjs[clientId]; i++)
    {
        currententity = transObjects[clientId][i].pEnt;
        r_blend = CL_FxBlend(currententity);
        if (r_blend <= 0)
            continue;

        r_blend = r_blend * (1.0f / 255.0f);

        if (currententity->rendermode == 3 && currententity->model->type != 1)
            Con_Printf("Non-sprite set to glow!\n");

        if (currententity->model->type == 2)
            continue;
        if (currententity->model->type != 3)
            continue;

        if (currententity->renderfx == 16 || currententity->renderfx == 23)
        {
            glEnable(GL_CULL_FACE);
            func_0023AFC8(4);
        }

        if (currententity->index > 0 && currententity->index <= cl[clientId].maxclients)
        {
            R_StudioDrawPlayer(3, &cl[clientId].frames[cl[clientId].parsecount & cl_updatemask[clientId]].playerstate[currententity->index - 1], clientId);
        }
        else if (currententity->movetype == 12)
        {
            for (j = 0; j < numTransObjs[clientId]; j++)
            {
                if (transObjects[clientId][j].pEnt->index == currententity->aiment)
                {
                    currententity = transObjects[clientId][j].pEnt;
                    if (currententity->index > 0 && currententity->index <= cl[clientId].maxclients)
                        R_StudioDrawPlayer(0, &cl[clientId].frames[cl[clientId].parsecount & cl_updatemask[clientId]].playerstate[currententity->index - 1], clientId);
                    else
                        R_StudioDrawModel(0, clientId, 1);

                    currententity = transObjects[clientId][i].pEnt;
                    R_StudioDrawModel(3, clientId, 1);
                    break;
                }
            }
        }
        else
        {
            R_StudioDrawModel(3, clientId, 1);
        }

        if (currententity->renderfx == 16 || currententity->renderfx == 23)
        {
            glDisable(GL_CULL_FACE);
            func_0023AFC8(2);
        }
    }

    r_blend = 1.0f;
    numTransObjs[clientId] = 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/r_trans", R_DrawStudioTEntities);
#endif

