#include "common.h"
#include "engine/cmd.h"
#include "engine/cvar.h"
#include "engine/client.h"
#include "engine/render.h"
#include "engine/server.h"

#include "engine/glquake.h"

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
void FillRGBA(vrect_t *rc, byte *color);
extern int D_00399570, D_00399558;
void glEnable(int cap);
void glDisable(int cap);
void glTexEnvf(int target, int pname, float param);
void glBlendFunc(int sfactor, int dfactor);
void glColor4f(float r, float g, float b, float a);
void glColor3f(float r, float g, float b);
void glBegin(int mode);
void glEnd(void);
void glVertex2f(float x, float y);
extern int D_00399628[]; /* scr_erase_center */
void SCR_SizeUp_f(void);
void SCR_SizeDown_f(void);
void SCR_UpdateScreen(void);
void Con_ClearNotify(void);
void Con_DrawConsole(int lines, qboolean drawinput);
void Con_DrawNotify(void);
void Draw_BeginDisc(void);
void Draw_PsiPic(int x, int y, qpic_t *pic, int a);

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

void SCR_DrawCenterString(int clientId)
{
    char *start;
    int l;
    int j;
    int x, y;
    int remaining;

    if (cl[clientId].intermission)
        remaining = D_00333348.value * (cl[clientId].time - D_00399608[clientId]);
    else
        remaining = 9999;

    D_00399628[clientId] = 0;
    start = D_004F95D8[clientId];

    if (D_003995F0[clientId] <= 4)
        y = D_003995DC * 0.35f;
    else
        y = 48;

    do
    {
        for (l = 0; l < 40; l++)
            if (start[l] == '\n' || !start[l])
                break;
        x = (D_003995E8 - l * 8) / 2;
        for (j = 0; j < l; j++)
        {
            x += Draw_Character(x, y, start[j]);
            if (!remaining--)
                return;
        }

        y += 12;

        while (*start && *start != '\n')
            start++;

        if (!*start)
            break;
        start++;
    } while (1);
}

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

extern float D_003332E0[]; /* scr_fov */
extern int D_003992E8;     /* sb_lines */
extern refdef_t r_refdef;

#ifdef NON_MATCHING
void SCR_CalcRefdef(int clientId)
{
    float size;
    int h;

    D_003995C4 = 0;
    D_0053D1D4 = 0;
    scr_viewsize.value = 120;

    if (D_003332E0[clientId] < 10)
        D_003332E0[clientId] = 10;
    if (D_003332E0[clientId] > 170)
        D_003332E0[clientId] = 170;

    if (cl[clientId].intermission)
        size = 120;
    else
        size = scr_viewsize.value;

    if (size >= 120)
        D_003992E8 = 0;
    else if (size >= 110)
        D_003992E8 = 24;
    else
        D_003992E8 = 48;

    size = scr_viewsize.value < 100 ? scr_viewsize.value : 100;
    if (cl[clientId].intermission)
    {
        D_003992E8 = 0;
        size = 100;
    }
    size *= 0.01f;

    h = D_003995DC - D_003992E8;

    r_refdef.vrect.width = D_003995E8 * size + 0.5f;
    if (r_refdef.vrect.width < 96)
    {
        size = 96.0f / r_refdef.vrect.width;
        r_refdef.vrect.width = 96;
    }

    r_refdef.vrect.height = D_003995DC * size + 0.5f;
    if (r_refdef.vrect.height > D_003995DC - D_003992E8)
        r_refdef.vrect.height = D_003995DC - D_003992E8;

    r_refdef.vrect.x = (D_003995E8 - r_refdef.vrect.width) * 0.5f;
    r_refdef.vrect.y = (h - r_refdef.vrect.height) * 0.5f;

    scr_vrect = r_refdef.vrect;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/gl_screen", SCR_CalcRefdef);
#endif

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
    Draw_PsiPic(D_003995E8 / 2 - pic->width / 2, D_003995DC / 2 - pic->height / 2, pic, a);
}

void func_002AC1A8(void) {
}

#ifdef NON_MATCHING
void func_002AC1B0(int clientId)
{
    if (D_00399618)
        Draw_BeginDisc();
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/gl_screen", func_002AC1B0);
#endif

typedef struct
{
    byte pad0[0x20];
    unsigned int height;        /* 0x20 */
    byte pad24[0x28 - 0x24];
    int numpages;               /* 0x28 */
    int recalc_refdef;          /* 0x2C */
} scr_viddef_t;
extern scr_viddef_t r_viddef;
extern int clearnotify, con_notifylines, con_forcedup;
void Con_CheckResize(void);

void SCR_SetUpToDrawConsole(int clientId)
{
    Con_CheckResize();
    if (D_00399618)
        return;

    con_forcedup = !cl[clientId].worldmodel || cls[clientId].signon != 3;

    if (con_forcedup)
    {
        D_00399604 = r_viddef.height;
        D_003995E0 = D_00399604;
    }
    else if (key_dest == 1)
        D_00399604 = r_viddef.height / 2;
    else
        D_00399604 = 0;

    if (D_00399604 < D_003995E0)
    {
        D_003995E0 -= D_003332E8.value * host_frametime;
        if (D_00399604 > D_003995E0)
            D_003995E0 = D_00399604;
    }
    else if (D_00399604 > D_003995E0)
    {
        D_003995E0 += D_003332E8.value * host_frametime;
        if (D_00399604 < D_003995E0)
            D_003995E0 = D_00399604;
    }

    if (D_00399624++ < r_viddef.numpages)
        ;
    else if (clearnotify++ < r_viddef.numpages)
        ;
    else
        con_notifylines = 0;
}

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

qboolean V_CheckGamma(void);
void V_UpdatePalette(void);
void func_002A68C0(void);
void func_002C50C8(int *x, int *y, int *width, int *height);
void func_002C5130(void);
void func_00236658(void);
void func_002A8610(void);
void func_002A8708(void);
void SCR_UpdateClientScreen(int clientId);
extern int D_0033453C, D_00399704, D_003342F0;
extern qboolean con_initialized;
extern int D_003995C0, D_0039961C;
extern int D_0039956C, D_0039957C, D_00399548, D_0039955C, D_0039953C, D_00399578, D_003995A8, D_00399538, D_00399594;
extern int D_0039958C, D_00399584, D_00399560, D_00399574, D_0039954C, D_00399568, D_00399534, D_00399598;

void SCR_UpdateScreen(void)
{
    int i;

    if (key_dest == 4)
        return;

    V_CheckGamma();
    func_002A68C0();

    if (D_0033453C)
        return;
    if (D_00399704)
        return;

    if (D_003342F0)
    {
        D_003342F0 = 0;
        return;
    }

    scr_copytop = 0;
    D_00399620 = 0;

    if (scr_disabled_for_loading)
    {
        if (realtime - D_00414158 > 60)
        {
            scr_disabled_for_loading = 0;
            Con_Printf("load failed.\n");
        }
        else
            return;
    }

    if (!cls[0].state)
        return;
    if (!D_003995D8)
        return;
    if (!con_initialized)
        return;

    V_UpdatePalette();
    func_002C50C8(&D_003995C0, &D_0039961C, &D_003995E8, &D_003995DC);

    D_0039956C = 0;
    D_0039957C = 0;
    D_00399548 = 0;
    D_0039955C = 0;
    D_0039953C = 0;
    D_00399578 = 0;
    D_003995A8 = 0;
    D_00399538 = 0;
    D_00399594 = 0;
    D_0039958C = 0;
    D_00399584 = 0;
    D_00399560 = 0;
    D_00399574 = 0;
    D_0039954C = 0;
    D_00399568 = 0;
    D_00399534 = 0;
    D_00399598 = 0;
    D_00399570 = 0;
    D_00399558 = 0;

    for (i = 0; i < cl_numlocal; i++)
        SCR_UpdateClientScreen(i);

    func_002A8610();
    if (sv.paused)
        func_00236658();
    SCR_DrawConsole();
    func_002A8708();
    func_002C5130();
}

void SCR_DrawNotifyString(void);
void SCR_DrawConnectMessage(void);
void func_002ACF08(void);
void func_002ACF10(void);
extern int D_003995E4;
void V_RenderView(int clientId);
void func_002A8358(int clientId);
void func_00266188(int clientId);
void Draw_FadeScreen(void);
void func_002A8610(void);
void func_002470E0(int intermission, int nClient, int flags);
void func_002A8708(void);
void func_002A8508(int nClient, int x, int y);
void GL_Bind(int texnum);
void func_002943F8();

void SCR_UpdateClientScreen(int clientId)
{
    if (r_viddef.recalc_refdef)
        SCR_CalcRefdef(clientId);

    if (key_dest != 4 || D_003332B0.value != 0)
    {
        SCR_SetUpToDrawConsole(clientId);
        V_RenderView(clientId);
        func_002A8358(clientId);

        if (D_003995E4)
        {
            func_00266188(clientId);
            Draw_FadeScreen();
            SCR_DrawNotifyString();
            D_00399620 = 1;
        }
        else if (D_00399618)
        {
            func_002AC1B0(clientId);
            func_00266188(clientId);
        }
        else if (cl[clientId].intermission == 1 && key_dest == 0)
        {
            if (clientId == 1)
            {
                func_002A8610();
                func_002470E0(1, 1, 0x80);
                func_002A8708();
                func_002A8508(1, 0, 0);
            }
        }
        else if (cl[clientId].intermission == 2 && key_dest == 0)
        {
            SCR_CheckDrawCenterString(clientId);
        }
        else
        {
            GL_Bind(0);
            if (D_003995E0 < r_viddef.height)
                func_00266188(clientId);
            SCR_CheckDrawCenterString(clientId);
        }

        func_002ACF08();
        SCR_DrawConnectMessage();
        func_002ACF10();
        func_002A8708();
    }

    if (key_dest == 4)
        func_002943F8(D_003332B0.value != 0);
}

void FillRGBA(vrect_t *rc, byte *color)
{
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glColor4f(color[0] * (1.0f / 255.0f), color[1] * (1.0f / 255.0f), color[2] * (1.0f / 255.0f), 1.0f);
    glDisable(GL_DEPTH_TEST);

    glBegin(GL_QUADS);
    D_00399570++;
    D_00399558 += 4;
    glVertex2f(rc->x, rc->y);
    glVertex2f(rc->x + rc->width, rc->y);
    glVertex2f(rc->x + rc->width, rc->y + rc->height);
    glVertex2f(rc->x, rc->y + rc->height);
    glEnd();

    glEnable(GL_DEPTH_TEST);
    glColor3f(1, 1, 1);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

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
    FillRGBA(&rcFill, color);

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
