#include "common.h"
#include "engine/cmd.h"
#include "engine/cvar.h"
#include "engine/client.h"
#include "engine/server.h"
#include "engine/pmove.h"

/* mouse/entrada do PS2 (equivale ao in_win.c do Quake) */

typedef struct kbutton_s
{
    int down[2];
    int state;
} kbutton_t;

#define K_MOUSE1 200

extern int iMouseInUse;
extern int key_dest;
extern qboolean noclip_anglehack;

extern int D_0039972C;          /* mouseinitialized */
extern int D_00399730;          /* mouse_buttons */
extern int D_00399734;          /* mouse_oldbuttonstate */
extern int D_0043B1C0;          /* mouseactive */
extern int D_0043B1C4;          /* mouseactivatetoggle */
extern int D_00399738;          /* mx_accum */
extern int D_0039974C;          /* my_accum */
extern int D_00399720;          /* mouse_x */
extern int D_00399744;          /* mouse_y */
extern int D_00399748;          /* old_mouse_x */
extern int D_00399724;          /* old_mouse_y */

extern cvar_t D_00334360;       /* m_filter */
extern cvar_t D_003344F8;
extern cvar_t D_003344E0;
extern cvar_t D_00556058[], D_0055A378[];
extern cvar_t D_00556030[];     /* m_yaw */
extern cvar_t D_0056A220[];     /* m_side */
extern cvar_t D_0055A328[];     /* m_pitch */
extern cvar_t D_0055A2D8[];     /* m_forward */
extern float D_00331844;        /* sensitivity.value */
extern float D_0033183C;        /* lookstrafe.value */
extern float D_00331AF4;        /* cl_pitchdown.value */
extern float D_00331ADC;        /* cl_pitchup.value */

extern kbutton_t in_mlook[2];
extern kbutton_t D_0046EA48[2]; /* in_strafe */

extern void (*D_0053DDF0)(int, float *, float *);

int COM_CheckParm(char *parm);
int atoi(const char *);
void func_002312D0(void);
void func_00231338(int);
void func_00231768(signed char *dx, signed char *dy);
void func_0021BFC0(int clientId);
void V_StopPitchDrift(int clientId);
void Key_Event(int key, qboolean down);

void func_002C5338(void)
{
    if (!iMouseInUse)
        cl[atoi(Cmd_Argv(1))].viewangles[0] = 0;
}

void func_002C5380(void) {
}

void func_002C5388(void)
{
    D_0043B1C4 = 1;
    if (D_0039972C)
        D_0043B1C0 = 1;
}

void func_002C53B0(void) {
}

void func_002C53B8(void)
{
    D_0043B1C4 = 0;
    if (D_0039972C)
        D_0043B1C0 = 0;
}

void func_002C53E0(void) {
}

INCLUDE_RODATA("asm/nonmatchings/engine/in_ps2", D_003953D8);

INCLUDE_RODATA("asm/nonmatchings/engine/in_ps2", D_003953E0);

INCLUDE_RODATA("asm/nonmatchings/engine/in_ps2", D_003953F0);

INCLUDE_RODATA("asm/nonmatchings/engine/in_ps2", D_00395400);

INCLUDE_RODATA("asm/nonmatchings/engine/in_ps2", D_00395408);

INCLUDE_RODATA("asm/nonmatchings/engine/in_ps2", D_00395418);

INCLUDE_RODATA("asm/nonmatchings/engine/in_ps2", D_00395428);

INCLUDE_RODATA("asm/nonmatchings/engine/in_ps2", D_00395438);

INCLUDE_RODATA("asm/nonmatchings/engine/in_ps2", D_00395448);

INCLUDE_RODATA("asm/nonmatchings/engine/in_ps2", D_00395458);

INCLUDE_RODATA("asm/nonmatchings/engine/in_ps2", D_00395468);

INCLUDE_RODATA("asm/nonmatchings/engine/in_ps2", D_00395478);

INCLUDE_RODATA("asm/nonmatchings/engine/in_ps2", D_00395480);

INCLUDE_RODATA("asm/nonmatchings/engine/in_ps2", D_00395498);

INCLUDE_RODATA("asm/nonmatchings/engine/in_ps2", D_003954B0);

INCLUDE_RODATA("asm/nonmatchings/engine/in_ps2", D_003954B8);

INCLUDE_RODATA("asm/nonmatchings/engine/in_ps2", D_003954D0);

INCLUDE_RODATA("asm/nonmatchings/engine/in_ps2", D_003954E8);

INCLUDE_RODATA("asm/nonmatchings/engine/in_ps2", D_003954F0);

INCLUDE_RODATA("asm/nonmatchings/engine/in_ps2", D_00395500);

INCLUDE_RODATA("asm/nonmatchings/engine/in_ps2", D_00395510);

INCLUDE_RODATA("asm/nonmatchings/engine/in_ps2", D_00395520);

void func_002C53E8(void)
{
    if (COM_CheckParm("-nomouse"))
        return;

    func_002312D0();
    D_0039972C = 1;
    D_00399730 = 3;
}

void func_002C5430(void)
{
    Cvar_RegisterVariable(&D_00334360);
    Cvar_RegisterVariable(&D_003344F8);
    Cvar_RegisterVariable(&D_003344E0);
    Cvar_RegisterVariable(&D_00556058[0]);
    Cvar_RegisterVariable(&D_0055A378[0]);
    Cvar_RegisterVariable(&D_00556058[1]);
    Cvar_RegisterVariable(&D_0055A378[1]);
    Cmd_AddCommand("force_centerview", func_002C5338);
    func_002C53E8();
}

void func_002C54C8(void)
{
    func_002C53B8();
    func_002C5380();
}

void func_002C54F0(int mstate)
{
    int i;

    if (D_0043B1C0)
    {
        for (i = 0; i < D_00399730; i++)
        {
            if (((mstate >> i) & 1) && !((D_00399734 >> i) & 1))
                Key_Event(K_MOUSE1 + i, true);

            if (!((mstate >> i) & 1) && ((D_00399734 >> i) & 1))
                Key_Event(K_MOUSE1 + i, false);
        }

        D_00399734 = mstate;
    }
}

void func_002C55B8(void) {
}

void func_002C55C0(usercmd_t *cmd, int clientId)
{
    int mx, my;
    signed char delta[2];

    if (iMouseInUse)
        return;
    if (key_dest == 4)
        return;

    func_00231338(0);
    func_00231768(&delta[0], &delta[1]);

    mx = delta[0] + D_00399738;
    my = delta[1] + D_0039974C;
    D_00399738 = 0;
    D_0039974C = 0;

    if (D_00334360.value)
    {
        D_00399720 = (mx + D_00399748) * 0.5f;
        D_00399744 = (my + D_00399724) * 0.5f;
    }
    else
    {
        D_00399720 = mx;
        D_00399744 = my;
    }

    D_00399748 = mx;
    D_00399724 = my;

    D_00399720 *= D_00331844;
    D_00399744 *= D_00331844;

    if ((D_0046EA48[clientId].state & 1) || (D_0033183C && (in_mlook[clientId].state & 1)))
        cmd->sidemove += D_0056A220[clientId].value * D_00399720;
    else
        cl[clientId].viewangles[1] -= D_00556030[clientId].value * D_00399720;

    if (in_mlook[clientId].state & 1)
        V_StopPitchDrift(clientId);

    if ((in_mlook[clientId].state & 1) && !(D_0046EA48[clientId].state & 1))
    {
        cl[clientId].viewangles[0] += D_0055A328[clientId].value * D_00399744;
        if (cl[clientId].viewangles[0] > D_00331AF4)
            cl[clientId].viewangles[0] = D_00331AF4;
        if (cl[clientId].viewangles[0] < -D_00331ADC)
            cl[clientId].viewangles[0] = -D_00331ADC;
    }
    else
    {
        if ((D_0046EA48[clientId].state & 1) && noclip_anglehack)
            cmd->upmove -= D_0055A2D8[clientId].value * D_00399744;
        else
            cmd->forwardmove -= D_0055A2D8[clientId].value * D_00399744;
    }
}

void func_002C5980(usercmd_t *cmd, int clientId)
{
    if (!iMouseInUse && D_0043B1C0)
        func_002C55C0(cmd, clientId);

    func_0021BFC0(clientId);

    if (cl[clientId].unk2128 > 0 && cl[clientId].unk2128 < sv.num_edicts)
        D_0053DDF0(clientId == 0 ? 1 : 2, cl[clientId].viewangles, cl[clientId].viewattachment);
}

void func_002C5A30(void)
{
    signed char delta[2];

    if (iMouseInUse)
        return;
    if (key_dest == 4)
        return;

    if (D_0043B1C0)
    {
        func_00231338(0);
        func_00231768(&delta[0], &delta[1]);
        D_00399738 += delta[0];
        D_0039974C += delta[1];
    }
}

void func_002C5AB8(void)
{
    if (D_0043B1C0)
    {
        D_00399738 = 0;
        D_0039974C = 0;
        D_00399734 = 0;
    }
}
