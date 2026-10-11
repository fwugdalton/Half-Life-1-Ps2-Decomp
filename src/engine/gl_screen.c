#include "common.h"
#include "engine/cmd.h"

typedef struct
{
    byte pad0[0x18];
    unsigned short width;       /* 0x18 */
    unsigned short height;      /* 0x1A */
} qpic_t;

typedef struct
{
    short texnum;               /* 0x00 */
    short servercount;          /* 0x02 */
    byte pad4[0x48 - 0x4];
} gltexture_t; /* 0x48 */

#define MAX_GLTEXTURES 1200

extern int D_0053D1D4;
extern int D_00399618;
extern int scr_disabled_for_loading;
extern int D_003995C4;
extern int D_003995E8, D_003995DC;
extern float D_003995E0; /* scr_con_current */
extern float D_00399604; /* scr_conlines */
extern int D_00399620, D_00399624;
extern int D_003995D0;
extern int key_dest;
extern int D_00399640; /* numgltextures */
extern gltexture_t D_004F9DD8[];

void SCR_SizeUp_f(void);
void SCR_SizeDown_f(void);
void SCR_UpdateScreen(void);
void Con_ClearNotify(void);
void Con_DrawConsole(int lines, qboolean drawinput);
void Con_DrawNotify(void);
void func_002AF7A8(void);
void func_002ADFB0(int x, int y, qpic_t *pic, int a);

INCLUDE_ASM("asm/nonmatchings/engine/gl_screen", SCR_CenterPrint);

INCLUDE_ASM("asm/nonmatchings/engine/gl_screen", func_002ABB20);

INCLUDE_ASM("asm/nonmatchings/engine/gl_screen", func_002ABD20);

INCLUDE_ASM("asm/nonmatchings/engine/gl_screen", func_002ABDE0);

void func_002AC038(void)
{
    SCR_SizeUp_f();
    D_0053D1D4 = 1;
}

void func_002AC060(void)
{
    SCR_SizeDown_f();
    D_0053D1D4 = 1;
}

INCLUDE_RODATA("asm/nonmatchings/engine/gl_screen", D_00394650);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_screen", D_00394658);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_screen", D_00394670);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_screen", D_00394678);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_screen", D_00394688);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_screen", D_00394690);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_screen", D_003946A0);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_screen", D_003946A8);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_screen", D_003946B8);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_screen", D_003946C0);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_screen", D_003946C8);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_screen", D_003946D8);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_screen", D_003946E0);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_screen", D_003946F0);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_screen", D_003946F8);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_screen", D_00394708);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_screen", D_00394718);

INCLUDE_RODATA("asm/nonmatchings/engine/gl_screen", D_00394728);

INCLUDE_ASM("asm/nonmatchings/engine/gl_screen", SCR_Init);

void func_002AC148(qpic_t *pic, int a)
{
    func_002ADFB0(D_003995E8 / 2 - pic->width / 2, D_003995DC / 2 - pic->height / 2, pic, a);
}

void func_002AC1A8(void) {
}

#ifdef NON_MATCHING
void func_002AC1B0(void)
{
    if (D_00399618)
        func_002AF7A8();
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/gl_screen", func_002AC1B0);
#endif

INCLUDE_ASM("asm/nonmatchings/engine/gl_screen", func_002AC1D8);

void func_002AC400(void)
{
    if (D_003995E0)
    {
        D_00399620 = 1;
        Con_DrawConsole((int)D_003995E0, true);
        D_00399624 = 0;
    }
    else if (key_dest == 0 || key_dest == 2)
    {
        Con_DrawNotify();
    }
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_screen", func_002AC478);

void SCR_EndLoadingPlaque(void)
{
    scr_disabled_for_loading = false;
    D_003995C4 = 0;
    D_00399618 = 0;
    Con_ClearNotify();
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_screen", func_002AC570);

void func_002AC6C8(void)
{
    int i;

    D_003995D0 = 0;
    for (i = 0; i < 20 && D_00399604 != D_003995E0; i++)
        SCR_UpdateScreen();
}

void func_002AC748(void) {
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_screen", SCR_UpdateScreen);

INCLUDE_ASM("asm/nonmatchings/engine/gl_screen", func_002AC9A8);

INCLUDE_ASM("asm/nonmatchings/engine/gl_screen", func_002ACC08);

INCLUDE_ASM("asm/nonmatchings/engine/gl_screen", func_002ACDC0);

void func_002ACF00(void) {
}

void func_002ACF08(void) {
}

void func_002ACF10(void) {
}

void func_002ACF18(void)
{
    int i;
    int world, other, free;
    gltexture_t *glt;

    other = 0;
    world = 0;
    free = 0;
    for (i = 0, glt = D_004F9DD8; i < D_00399640; i++, glt++)
    {
        if (glt->servercount == -1)
            free++;
        else if (!glt->servercount)
            other++;
        else
            world++;
    }
    Con_Printf("World: %d, Other: %d, Free: %d, Unused: %d\n", world, other, free, MAX_GLTEXTURES - D_00399640);
}

void func_002ACFA0(void) {
}
