#include "common.h"
#include "engine/cmd.h"
#include "engine/model.h"

void *memset(void *, int, unsigned int);
char *strcpy(char *, const char *);
int strlen(const char *);
int strcasecmp(const char *, const char *);
void Mem_PushTag(void);
void Mem_SetTag(int tag);
void Mem_PopTag(void);
model_t *Mod_ForName(char *name, qboolean crash);
int ModelFrameCount(model_t *model);
void Mod_FreeSpriteDecals(model_t *mod);
void glColor4f(float r, float g, float b, float a);
extern qboolean host_initialized;

typedef struct
{
    int left, right, top, bottom;
} wrect_t;

typedef struct
{
    byte pad0[0x1C];
    unsigned short width;       /* 0x1C */
    unsigned short height;      /* 0x1E */
} spritepic_t;

typedef struct
{
    int unk0;
    spritepic_t *pic;           /* 0x04 */
} spriteframe_t;

typedef struct msprite_s msprite_t;

/* SPRITELIST (0xC) */
typedef struct
{
    model_t *pSprite;           /* 0x00 */
    char *pName;                /* 0x04 */
    int frames;                 /* 0x08 */
} spritelist_t;

typedef struct
{
    byte pad0[0x1C];
    int width;                  /* 0x1C */
    int height;                 /* 0x20 */
} draw_viddef_t;

extern draw_viddef_t r_viddef;

spriteframe_t *R_GetSpriteFrame(msprite_t *pSprite, int frame);
void Draw_SpriteFrameNormal(spriteframe_t *pFrame, void *pPalette, int x, int y, wrect_t *prcSubRect);
void Draw_SpriteFrameHoles(spriteframe_t *pFrame, void *pPalette, int x, int y, wrect_t *prcSubRect);
void Draw_SpriteFrameAdditive(spriteframe_t *pFrame, void *pPalette, int x, int y, wrect_t *prcSubRect);
void EnableScissorTest(int x, int y, int width, int height);
void DisableScissorTest(void);

extern int D_00399138[2];           /* hspr da mira */
extern int D_00399130[2];           /* r da mira */
extern int D_00399128[2];           /* g da mira */
extern int D_00399140[2];           /* b da mira */
extern wrect_t D_0046E750[2];       /* retângulo da mira */
extern spritelist_t *D_0033169C;    /* gSpriteList */
extern int D_003316A0;              /* gSpriteCount */
extern msprite_t *D_003316A4;       /* gpSprite */
extern int D_003316A8;              /* gSpriteMipMap */
extern byte D_0046E550[];           /* paleta dos sprites */

void func_00247E80(int hspr, int r, int g, int b);
extern char D_00389218[];          /* "engine/cl_draw.c" */
void func_00248070(int frame, int x, int y, wrect_t *prc);

/* SetCrosshair */
void func_002478F8(int hspr, wrect_t rc, int r, int g, int b, int clientId)
{
    D_00399138[clientId] = hspr;
    D_0046E750[clientId] = rc;
    D_00399130[clientId] = r;
    D_00399128[clientId] = g;
    D_00399140[clientId] = b;
}

/* DrawCrosshair */
void func_00247998(int x, int y, int clientId)
{
    if (D_00399138[clientId])
    {
        func_00247E80(D_00399138[clientId], D_00399130[clientId], D_00399128[clientId], D_00399140[clientId]);
        func_00248070(0, x - (D_0046E750[clientId].right - D_0046E750[clientId].left) / 2,
                      y - (D_0046E750[clientId].bottom - D_0046E750[clientId].top) / 2, &D_0046E750[clientId]);
    }
}

INCLUDE_RODATA("asm/nonmatchings/engine/cl_draw", D_00389218);

/* SPR_Load */
#ifdef NON_MATCHING
int func_00247A80(char *pTextureName)
{
    int i;

    if (!pTextureName || !D_0033169C || D_003316A0 <= 0)
        return 0;

    for (i = 0; i < D_003316A0; i++)
    {
        if (!D_0033169C[i].pSprite)
        {
            Mem_PushTag();
            Mem_SetTag(1);
            D_0033169C[i].pName = Mem_DbgMalloc(strlen(pTextureName) + 1, D_00389218, 116);
            Mem_PopTag();
            strcpy(D_0033169C[i].pName, pTextureName);
        }

        if (!strcasecmp(pTextureName, D_0033169C[i].pName))
        {
            D_003316A8 = 0;
            D_0033169C[i].pSprite = Mod_ForName(pTextureName, false);
            D_003316A8 = 1;
            if (!D_0033169C[i].pSprite)
                return 0;
            D_0033169C[i].frames = ModelFrameCount(D_0033169C[i].pSprite);
            return i + 1;
        }
    }

    Sys_Error("cannot allocate more than %d HUD sprites\n", 128);
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/cl_draw", func_00247A80);
#endif

/* SPR_Shutdown */
void func_00247C00(void)
{
    int i;

    if (!host_initialized)
        return;

    if (D_0033169C)
    {
        for (i = 0; i < D_003316A0; i++)
        {
            if (D_0033169C[i].pSprite)
                Mod_FreeSpriteDecals(D_0033169C[i].pSprite);
            if (D_0033169C[i].pName)
                Mem_Free(D_0033169C[i].pName);
        }
        Mem_Free(D_0033169C);
    }

    D_0033169C = NULL;
    D_003316A0 = 0;
    D_003316A4 = NULL;
}

/* SPR_Get */
spritelist_t *func_00247CD0(int hSprite)
{
    int i;

    i = hSprite - 1;
    if (i < 0 || i >= D_003316A0)
        return NULL;
    return &D_0033169C[i];
}

/* SPR_Pointer */
msprite_t *func_00247D08(spritelist_t *pList)
{
    return (msprite_t *)pList->pSprite->cache.data;
}

/* SPR_Init */
void func_00247D18(int clientId)
{
    D_00399138[clientId] = 0;

    if (!D_0033169C && !clientId)
    {
        D_003316A0 = 128;
        Mem_PushTag();
        Mem_SetTag(1);
        D_0033169C = Mem_DbgMalloc(128 * sizeof(spritelist_t), D_00389218, 217);
        Mem_PopTag();
        memset(D_0033169C, 0, 128 * sizeof(spritelist_t));
        D_003316A4 = NULL;
    }
}

/* SPR_Frames */
int func_00247DB0(int hSprite)
{
    spritelist_t *p;

    p = func_00247CD0(hSprite);
    if (p)
        return p->frames;
    return 0;
}

/* SPR_Width */
int func_00247DD8(int hSprite, int frame)
{
    spritelist_t *p;
    spriteframe_t *pFrame;

    p = func_00247CD0(hSprite);
    if (!p)
        return 0;
    pFrame = R_GetSpriteFrame(func_00247D08(p), frame);
    if (!pFrame)
        return 0;
    return pFrame->pic->width;
}

/* SPR_Height */
int func_00247E28(int hSprite, int frame)
{
    spritelist_t *p;
    spriteframe_t *pFrame;

    p = func_00247CD0(hSprite);
    if (!p)
        return 0;
    pFrame = R_GetSpriteFrame(func_00247D08(p), frame);
    if (!pFrame)
        return 0;
    return pFrame->pic->height;
}

void func_00247E78(void)
{
}

/* SPR_Set */
void func_00247E80(int hspr, int r, int g, int b)
{
    spritelist_t *p;

    p = func_00247CD0(hspr);
    if (!p)
        return;
    D_003316A4 = func_00247D08(p);
    if (D_003316A4)
        glColor4f(r * (1.0f / 255.0f), g * (1.0f / 255.0f), b * (1.0f / 255.0f), 1.0f);
}

/* SPR_Draw */
void func_00247F20(int frame, int x, int y, wrect_t *prc)
{
    spriteframe_t *pFrame;

    if (!D_003316A4)
        return;
    if (x >= r_viddef.width || y >= r_viddef.height)
        return;

    pFrame = R_GetSpriteFrame(D_003316A4, frame);
    if (!pFrame)
        Sys_Error("SPR_Draw error:  invalid frame");
    Draw_SpriteFrameNormal(pFrame, D_0046E550, x, y, prc);
}

/* SPR_DrawHoles */
void func_00247FD8(int frame, int x, int y, wrect_t *prc)
{
    if (!D_003316A4)
        return;
    if (x >= r_viddef.width || y >= r_viddef.height)
        return;

    Draw_SpriteFrameHoles(R_GetSpriteFrame(D_003316A4, frame), D_0046E550, x, y, prc);
}

/* SPR_DrawAdditive */
void func_00248070(int frame, int x, int y, wrect_t *prc)
{
    if (!D_003316A4)
        return;
    if (x >= r_viddef.width || y >= r_viddef.height)
        return;

    Draw_SpriteFrameAdditive(R_GetSpriteFrame(D_003316A4, frame), D_0046E550, x, y, prc);
}

/* SPR_EnableScissor */
void func_00248108(int x, int y, int width, int height)
{
    EnableScissorTest(x, y, width, height);
}

/* SPR_DisableScissor */
void func_00248128(void)
{
    DisableScissorTest();
}
