#include "common.h"
#include "engine/cmd.h"
#include "engine/cvar.h"
#include "engine/client.h"
#include "engine/render.h"

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
extern float D_003995D0[]; /* scr_centertime_off */
extern int key_dest;
extern int D_00399640; /* numgltextures */
extern gltexture_t D_004F9DD8[];

extern char D_004F95D8[][1024]; /* scr_centerstring */
extern cvar_t D_00333300; /* scr_centertime */
extern float D_00399608[]; /* scr_centertime_start */
extern int D_003995F0[]; /* scr_center_lines */
extern int D_00399610[]; /* scr_erase_lines */
extern int scr_copytop;
extern float host_frametime;
extern cvar_t scr_viewsize, D_00333330, D_003332E8, D_00333348, D_00333318, D_003332B0, D_00333378, D_00333390, D_003333A8;
extern int D_003995D8; /* scr_initialized */
void SCR_DrawCenterString(int clientId);
void SCR_SizeUp(void);
void SCR_SizeDown(void);
extern char *D_003995C8; /* scr_notifystring */
extern float realtime;
extern float D_00414158; /* scr_disabled_time */
extern vrect_t scr_vrect;
void func_002CD340(int a, int clientId);
int Draw_Character(int x, int y, int num);
int Draw_String(int x, int y, char *str);
void func_002ACC08(vrect_t *rc, byte *color);
void SCR_SizeUp_f(void);
void SCR_SizeDown_f(void);
void SCR_UpdateScreen(void);
void Con_ClearNotify(void);
void Con_DrawConsole(int lines, qboolean drawinput);
void Con_DrawNotify(void);
void func_002AF7A8(void);
void func_002ADFB0(int x, int y, qpic_t *pic, int a);

void SCR_CenterPrint(char *str, int clientId)
{
    strncpy(D_004F95D8[clientId], str, sizeof(D_004F95D8[0]) - 1);
    D_003995D0[clientId] = D_00333300.value;
    D_00399608[clientId] = cl[clientId].time;

    D_003995F0[clientId] = 1;
    while (*str)
    {
        if (*str == '\n')
            D_003995F0[clientId]++;
        str++;
    }
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_screen", SCR_DrawCenterString);

void SCR_CheckDrawCenterString(int clientId)
{
    scr_copytop = 1;
    if (D_003995F0[clientId] > D_00399610[clientId])
        D_00399610[clientId] = D_003995F0[clientId];

    D_003995D0[clientId] -= host_frametime;

    if (D_003995D0[clientId] <= 0 && !cl[clientId].intermission)
        return;
    if (key_dest != 0)
        return;

    SCR_DrawCenterString(clientId);
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_screen", func_002ABDE0);

void SCR_SizeUp(void)
{
    SCR_SizeUp_f();
    D_0053D1D4 = 1;
}

void SCR_SizeDown(void)
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

void SCR_Init(void)
{
    Cvar_RegisterVariable(&scr_viewsize);
    Cvar_RegisterVariable(&D_00333330);
    Cvar_RegisterVariable(&D_003332E8);
    Cvar_RegisterVariable(&D_00333300);
    Cvar_RegisterVariable(&D_00333348);
    Cvar_RegisterVariable(&D_00333318);
    Cvar_RegisterVariable(&D_003332B0);
    Cvar_RegisterVariable(&D_00333378);
    Cvar_RegisterVariable(&D_00333390);
    Cvar_RegisterVariable(&D_003333A8);

    Cmd_AddCommand("sizeup", SCR_SizeUp);
    Cmd_AddCommand("sizedown", SCR_SizeDown);

    D_003995D8 = 1;
}

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

void SCR_DrawConsole(void)
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

void SCR_BeginLoadingPlaque(int clientId)
{
    func_002CD340(1, clientId);

    if (cls[clientId].state < ca_connected || cls[clientId].state > ca_active)
        return;
    if (cls[clientId].signon != 3)
        return;

    Con_ClearNotify();
    D_003995D0[clientId] = 0;
    D_003995E0 = 0;

    D_00399618 = 1;
    D_003995C4 = 0;
    SCR_UpdateScreen();
    SCR_UpdateScreen();

    scr_disabled_for_loading = true;
    D_00414158 = realtime;
    D_003995C4 = 0;
}

void SCR_EndLoadingPlaque(void)
{
    scr_disabled_for_loading = false;
    D_003995C4 = 0;
    D_00399618 = 0;
    Con_ClearNotify();
}

void SCR_DrawNotifyString(void)
{
    char *start;
    int l;
    int j;
    int x, y;

    start = D_003995C8;

    y = D_003995DC * 0.35f;

    do
    {
        for (l = 0; l < 40; l++)
            if (start[l] == '\n' || !start[l])
                break;
        x = (D_003995E8 - l * 8) / 2;
        for (j = 0; j < l; j++, x += 8)
            Draw_Character(x, y, start[j]);

        y += 8;

        while (*start && *start != '\n')
            start++;

        if (!*start)
            break;
        start++;
    } while (1);
}

void func_002AC6C8(void)
{
    int i;

    D_003995D0[0] = 0;
    for (i = 0; i < 20 && D_00399604 != D_003995E0; i++)
        SCR_UpdateScreen();
}

void func_002AC748(void) {
}

INCLUDE_ASM("asm/nonmatchings/engine/gl_screen", SCR_UpdateScreen);

INCLUDE_ASM("asm/nonmatchings/engine/gl_screen", func_002AC9A8);

INCLUDE_ASM("asm/nonmatchings/engine/gl_screen", func_002ACC08);

#ifdef NON_MATCHING
void SCR_DrawConnectMessage(void)
{
    vrect_t rcFill;
    int pad[4];
    byte color[3];
    int nWidth, nHeight;
    int xpos, ypos;

    if (D_00333378.string[0] == '0')
        return;

    nWidth = 250;
    if (scr_vrect.width <= nWidth)
        nWidth = scr_vrect.width - 2;
    nHeight = 10;
    if (scr_vrect.height <= nHeight)
        nHeight = scr_vrect.height - 2;

    xpos = scr_vrect.x + (scr_vrect.width - nWidth) / 2;
    ypos = scr_vrect.height - 5 * nHeight - nHeight / 2;

    rcFill.x = xpos - 4;
    rcFill.y = ypos - nHeight / 2;
    rcFill.width = nWidth + 10;
    rcFill.height = 4 * nHeight;

    color[0] = color[1] = color[2] = 0;
    func_002ACC08(&rcFill, color);

    Draw_String(xpos + 1, ypos, D_00333378.string);

    if (D_00333390.string[0] != '0')
    {
        ypos += nHeight;
        Draw_String(xpos + 1, ypos, D_00333390.string);
    }

    if (D_003333A8.string[0] != '0')
    {
        Draw_String(xpos + 1, ypos + nHeight, D_003333A8.string);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/gl_screen", SCR_DrawConnectMessage);
#endif

void func_002ACF00(void) {
}

void func_002ACF08(void) {
}

void func_002ACF10(void) {
}

void GL_TextureStats_f(void)
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
