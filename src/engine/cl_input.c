#include "common.h"
#include "engine/cvar.h"
#include "engine/cmd.h"

/* cl_input.c: botões (+/-comando) por jogador local; Cmd_Argv(1) é o clientId. */

typedef struct kbutton_s
{
    int down[2];
    int state;
} kbutton_t;

extern kbutton_t D_0046EA00[2];
extern kbutton_t in_jlook[2];
extern kbutton_t in_mlook[2];
extern kbutton_t in_leftStickYLook[2];
extern kbutton_t in_leftStickXLook[2];
extern kbutton_t in_rightStickYLook[2];
extern kbutton_t in_rightStickXLook[2];
extern kbutton_t in_leftStickForwardMove[2];
extern kbutton_t in_leftStickSideMove[2];
extern kbutton_t in_rightStickForwardMove[2];
extern kbutton_t in_rightStickSideMove[2];
extern kbutton_t D_0046EBE0[2];
extern kbutton_t D_0046E988[2];
extern kbutton_t D_0046EA78[2];
extern kbutton_t D_0046E9E8[2];
extern kbutton_t D_0046E940[2];
extern kbutton_t D_0046E958[2];
extern kbutton_t D_0046EA60[2];
extern kbutton_t D_0046EB68[2];
extern kbutton_t D_0046EB50[2];
extern kbutton_t D_0046E928[2];
extern kbutton_t D_0046E9B8[2];
extern kbutton_t D_0046EB08[2];
extern kbutton_t D_0046EAA8[2];
extern kbutton_t D_0046E9D0[2];
extern kbutton_t D_0046E910[2];
extern kbutton_t D_0046EA48[2];
extern kbutton_t D_0046EAD8[2];
extern kbutton_t D_0046EB20[2];
extern kbutton_t D_0046EBB0[2];
extern kbutton_t D_0046EAF0[2];
extern kbutton_t D_0046EAC0[2];
extern kbutton_t D_0046EC10[2];
extern kbutton_t D_0046EB80[2];

extern int D_00399150[2];
extern int D_00399160[2];       /* in_impulse */
extern float D_00331824;        /* lookspring.value */

char *Cmd_Argv(int arg);
int atoi(const char *);
int Q_atoi(char *str);
void V_StartPitchDrift(int clientId);
void func_0024C500(kbutton_t *b);
void func_0024C5B0(kbutton_t *b);

void func_0024C500(kbutton_t *b)
{
    char *c;
    int k;

    c = Cmd_Argv(2);
    if (c[0])
        k = atoi(c);
    else
        k = -1;

    if (k == b->down[0] || k == b->down[1])
        return;

    if (!b->down[0])
        b->down[0] = k;
    else if (!b->down[1])
        b->down[1] = k;
    else
    {
        Con_DPrintf("Three keys down for a button '%c' '%c' '%c'!\n", b->down[0], b->down[1], c);
        return;
    }

    if (b->state & 1)
        return;
    b->state |= 1 + 2;
}

void func_0024C5B0(kbutton_t *b)
{
    char *c;
    int k;

    c = Cmd_Argv(2);
    if (c[0])
    {
        k = atoi(c);
    }
    else
    {
        b->down[0] = b->down[1] = 0;
        b->state = 4;
        return;
    }

    if (b->down[0] == k)
        b->down[0] = 0;
    else if (b->down[1] == k)
        b->down[1] = 0;
    else
        return;

    if (b->down[0] || b->down[1])
        return;

    if (!(b->state & 1))
        return;
    b->state &= ~1;
    b->state |= 4;
}

void func_0024C658(void)
{
    func_0024C500(&D_0046EA00[atoi(Cmd_Argv(1))]);
}

void func_0024C698(void)
{
    func_0024C5B0(&D_0046EA00[atoi(Cmd_Argv(1))]);
}

void func_0024C6D8(void)
{
    func_0024C500(&in_jlook[atoi(Cmd_Argv(1))]);
}

void func_0024C718(void)
{
    func_0024C5B0(&in_jlook[atoi(Cmd_Argv(1))]);
}

void func_0024C758(void)
{
    func_0024C500(&in_mlook[atoi(Cmd_Argv(1))]);
}

void func_0024C798(void)
{
    int clientId;

    clientId = atoi(Cmd_Argv(1));
    func_0024C5B0(&in_mlook[clientId]);
    if (!(in_mlook[clientId].state & 1) && D_00331824)
        V_StartPitchDrift(clientId);
}

void func_0024C828(void)
{
    func_0024C500(&in_leftStickYLook[atoi(Cmd_Argv(1))]);
}

void func_0024C868(void)
{
    func_0024C5B0(&in_leftStickYLook[atoi(Cmd_Argv(1))]);
}

void func_0024C8A8(void)
{
    func_0024C500(&in_leftStickXLook[atoi(Cmd_Argv(1))]);
}

void func_0024C8E8(void)
{
    func_0024C5B0(&in_leftStickXLook[atoi(Cmd_Argv(1))]);
}

void func_0024C928(void)
{
    func_0024C500(&in_rightStickYLook[atoi(Cmd_Argv(1))]);
}

void func_0024C968(void)
{
    func_0024C5B0(&in_rightStickYLook[atoi(Cmd_Argv(1))]);
}

void func_0024C9A8(void)
{
    func_0024C500(&in_rightStickXLook[atoi(Cmd_Argv(1))]);
}

void func_0024C9E8(void)
{
    func_0024C5B0(&in_rightStickXLook[atoi(Cmd_Argv(1))]);
}

void func_0024CA28(void)
{
    func_0024C500(&in_leftStickForwardMove[atoi(Cmd_Argv(1))]);
}

void func_0024CA68(void)
{
    func_0024C5B0(&in_leftStickForwardMove[atoi(Cmd_Argv(1))]);
}

void func_0024CAA8(void)
{
    func_0024C500(&in_leftStickSideMove[atoi(Cmd_Argv(1))]);
}

void func_0024CAE8(void)
{
    func_0024C5B0(&in_leftStickSideMove[atoi(Cmd_Argv(1))]);
}

void func_0024CB28(void)
{
    func_0024C500(&in_rightStickForwardMove[atoi(Cmd_Argv(1))]);
}

void func_0024CB68(void)
{
    func_0024C5B0(&in_rightStickForwardMove[atoi(Cmd_Argv(1))]);
}

void func_0024CBA8(void)
{
    func_0024C500(&in_rightStickSideMove[atoi(Cmd_Argv(1))]);
}

void func_0024CBE8(void)
{
    func_0024C5B0(&in_rightStickSideMove[atoi(Cmd_Argv(1))]);
}

void func_0024CC28(void)
{
    func_0024C500(&D_0046EBE0[atoi(Cmd_Argv(1))]);
}

void func_0024CC68(void)
{
    func_0024C5B0(&D_0046EBE0[atoi(Cmd_Argv(1))]);
}

void func_0024CCA8(void)
{
    func_0024C500(&D_0046E988[atoi(Cmd_Argv(1))]);
}

void func_0024CCE8(void)
{
    func_0024C5B0(&D_0046E988[atoi(Cmd_Argv(1))]);
}

void func_0024CD28(void)
{
    func_0024C500(&D_0046EA78[atoi(Cmd_Argv(1))]);
}

void func_0024CD68(void)
{
    func_0024C5B0(&D_0046EA78[atoi(Cmd_Argv(1))]);
}

void func_0024CDA8(void)
{
    func_0024C500(&D_0046E9E8[atoi(Cmd_Argv(1))]);
}

void func_0024CDE8(void)
{
    func_0024C5B0(&D_0046E9E8[atoi(Cmd_Argv(1))]);
}

void func_0024CE28(void)
{
    func_0024C500(&D_0046E940[atoi(Cmd_Argv(1))]);
}

void func_0024CE68(void)
{
    func_0024C5B0(&D_0046E940[atoi(Cmd_Argv(1))]);
}

void func_0024CEA8(void)
{
    func_0024C500(&D_0046E958[atoi(Cmd_Argv(1))]);
}

void func_0024CEE8(void)
{
    func_0024C5B0(&D_0046E958[atoi(Cmd_Argv(1))]);
}

void func_0024CF28(void)
{
    func_0024C500(&D_0046EA60[atoi(Cmd_Argv(1))]);
}

void func_0024CF68(void)
{
    func_0024C5B0(&D_0046EA60[atoi(Cmd_Argv(1))]);
}

void func_0024CFA8(void)
{
    func_0024C500(&D_0046EB68[atoi(Cmd_Argv(1))]);
}

void func_0024CFE8(void)
{
    func_0024C5B0(&D_0046EB68[atoi(Cmd_Argv(1))]);
}

void func_0024D028(void)
{
    func_0024C500(&D_0046EB50[atoi(Cmd_Argv(1))]);
}

void func_0024D068(void)
{
    func_0024C5B0(&D_0046EB50[atoi(Cmd_Argv(1))]);
}

void func_0024D0A8(void)
{
    func_0024C500(&D_0046E928[atoi(Cmd_Argv(1))]);
}

void func_0024D0E8(void)
{
    func_0024C5B0(&D_0046E928[atoi(Cmd_Argv(1))]);
}

void func_0024D128(void)
{
    func_0024C500(&D_0046E9B8[atoi(Cmd_Argv(1))]);
}

void func_0024D168(void)
{
    func_0024C5B0(&D_0046E9B8[atoi(Cmd_Argv(1))]);
}

void func_0024D1A8(void)
{
    func_0024C500(&D_0046EB08[atoi(Cmd_Argv(1))]);
}

void func_0024D1E8(void)
{
    func_0024C5B0(&D_0046EB08[atoi(Cmd_Argv(1))]);
}

void func_0024D228(void)
{
    func_0024C500(&D_0046EAA8[atoi(Cmd_Argv(1))]);
}

void func_0024D268(void)
{
    func_0024C5B0(&D_0046EAA8[atoi(Cmd_Argv(1))]);
}

void func_0024D2A8(void)
{
    func_0024C500(&D_0046E9D0[atoi(Cmd_Argv(1))]);
}

void func_0024D2E8(void)
{
    func_0024C5B0(&D_0046E9D0[atoi(Cmd_Argv(1))]);
}

void func_0024D328(void)
{
    func_0024C500(&D_0046E910[atoi(Cmd_Argv(1))]);
}

void func_0024D368(void)
{
    func_0024C5B0(&D_0046E910[atoi(Cmd_Argv(1))]);
}

void func_0024D3A8(void)
{
    func_0024C500(&D_0046EA48[atoi(Cmd_Argv(1))]);
}

void func_0024D3E8(void)
{
    func_0024C5B0(&D_0046EA48[atoi(Cmd_Argv(1))]);
}

void func_0024D428(void)
{
    func_0024C500(&D_0046EAD8[atoi(Cmd_Argv(1))]);
}

void func_0024D468(void)
{
    int clientId;

    clientId = atoi(Cmd_Argv(1));
    func_0024C5B0(&D_0046EAD8[clientId]);
    D_00399150[clientId] = 0;
}

void func_0024D4C8(void)
{
    func_0024C500(&D_0046EB20[atoi(Cmd_Argv(1))]);
}

void func_0024D508(void)
{
    func_0024C5B0(&D_0046EB20[atoi(Cmd_Argv(1))]);
}

void func_0024D548(void)
{
    func_0024C500(&D_0046EBB0[atoi(Cmd_Argv(1))]);
}

void func_0024D588(void)
{
    func_0024C5B0(&D_0046EBB0[atoi(Cmd_Argv(1))]);
}

void func_0024D5C8(void)
{
    func_0024C500(&D_0046EAF0[atoi(Cmd_Argv(1))]);
}

void func_0024D608(void)
{
    func_0024C5B0(&D_0046EAF0[atoi(Cmd_Argv(1))]);
}

INCLUDE_ASM("asm/nonmatchings/engine/cl_input", func_0024D648);

INCLUDE_ASM("asm/nonmatchings/engine/cl_input", func_0024D710);

void func_0024D7A0(void)
{
    D_00399150[atoi(Cmd_Argv(1))] = 1;
}

void func_0024D7E0(void)
{
    int clientId;

    clientId = atoi(Cmd_Argv(1));
    D_00399160[clientId] = Q_atoi(Cmd_Argv(2));
}

void func_0024D838(void)
{
    func_0024C500(&D_0046EAC0[atoi(Cmd_Argv(1))]);
}

void func_0024D878(void)
{
    func_0024C5B0(&D_0046EAC0[atoi(Cmd_Argv(1))]);
}

void func_0024D8B8(void)
{
    func_0024C500(&D_0046EC10[atoi(Cmd_Argv(1))]);
}

void func_0024D8F8(void)
{
    func_0024C5B0(&D_0046EC10[atoi(Cmd_Argv(1))]);
}

void func_0024D938(void)
{
    func_0024C500(&D_0046EB80[atoi(Cmd_Argv(1))]);
}

void func_0024D978(void)
{
    func_0024C5B0(&D_0046EB80[atoi(Cmd_Argv(1))]);
}

void func_0024D9B8(void)
{
    int clientId;

    clientId = atoi(Cmd_Argv(1));
    if (D_0046E910[clientId].state & 1)
        func_0024C5B0(&D_0046E910[clientId]);
    else
        func_0024C500(&D_0046E910[clientId]);
}

INCLUDE_ASM("asm/nonmatchings/engine/cl_input", func_0024DA18);

INCLUDE_ASM("asm/nonmatchings/engine/cl_input", func_0024DA78);

INCLUDE_ASM("asm/nonmatchings/engine/cl_input", func_0024DB18);

INCLUDE_ASM("asm/nonmatchings/engine/cl_input", func_0024DDF0);

INCLUDE_ASM("asm/nonmatchings/engine/cl_input", func_0024E2E0);

INCLUDE_ASM("asm/nonmatchings/engine/cl_input", func_0024E670);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_input", D_00389610);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_input", D_00389618);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_input", D_00389628);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_input", D_00389630);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_input", D_00389640);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_input", D_00389650);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_input", D_00389660);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_input", D_00389668);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_input", D_00389678);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_input", D_00389680);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_input", D_00389690);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_input", D_00389698);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_input", D_003896A8);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_input", D_003896B0);

INCLUDE_ASM("asm/nonmatchings/engine/cl_input", func_0024E700);
