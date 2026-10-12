#include "common.h"
#include "engine/cvar.h"
#include "engine/cmd.h"
#include "engine/mathlib.h"
#include "engine/client.h"

/* cl_input.c: botões (+/-comando) por jogador local; Cmd_Argv(1) é o clientId. */

typedef struct kbutton_s
{
    int down[2];
    int state;
} kbutton_t;

extern kbutton_t D_0046EA00[2]; /* in_klook */
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
extern kbutton_t D_0046EBE0[2]; /* in_moveup */
extern kbutton_t D_0046E988[2]; /* in_movedown */
extern kbutton_t D_0046EA78[2]; /* in_left */
extern kbutton_t D_0046E9E8[2]; /* in_right */
extern kbutton_t D_0046E940[2]; /* in_forward */
extern kbutton_t D_0046E958[2]; /* in_back */
extern kbutton_t D_0046EA60[2]; /* in_kbforward */
extern kbutton_t D_0046EB68[2]; /* in_kbback */
extern kbutton_t D_0046EB50[2]; /* in_lookup */
extern kbutton_t D_0046E928[2]; /* in_lookdown */
extern kbutton_t D_0046E9B8[2]; /* in_moveleft */
extern kbutton_t D_0046EB08[2]; /* in_moveright */
extern kbutton_t D_0046EAA8[2]; /* in_kbmoveleft */
extern kbutton_t D_0046E9D0[2]; /* in_kbmoveright */
extern kbutton_t D_0046E910[2]; /* in_speed */
extern kbutton_t D_0046EA48[2]; /* in_strafe */
extern kbutton_t D_0046EAD8[2]; /* in_attack */
extern kbutton_t D_0046EB20[2]; /* in_attack2 */
extern kbutton_t D_0046EBB0[2]; /* in_use */
extern kbutton_t D_0046EAF0[2]; /* in_jump */
extern kbutton_t D_0046EAC0[2]; /* in_reload */
extern kbutton_t D_0046EC10[2]; /* in_alt1 */
extern kbutton_t D_0046EB80[2]; /* in_alt2 */

extern kbutton_t D_0046EBC8[2]; /* in_duck */
extern int D_00399150[2];
extern float D_0033282C, D_00332844; /* toggle do duck por jogador */
int func_0024E2E0(int bResetState, int clientId);
float func_0024DA78(kbutton_t *key);
extern float host_frametime;
extern float D_00331774;        /* cl_anglespeedkey.value */
extern cvar_t D_00331738;       /* cl_yawspeed */
extern cvar_t D_00331750;       /* cl_pitchspeed */
extern float D_00331AF4;        /* cl_pitchdown.value */
extern float D_00331ADC;        /* cl_pitchup.value */
float anglemod(float a);
void V_StopPitchDrift(int clientId);
void func_0024DB18(int clientId);
extern cvar_t D_00331708;       /* cl_sidespeed */
extern cvar_t D_003316C0;       /* cl_upspeed */
extern cvar_t cl_forwardspeed;
extern cvar_t D_00331720;       /* cl_movespeedkey */
extern float D_00399148[2];     /* intensidade do analógico lateral */
extern float D_00399158[2];     /* intensidade do analógico frontal */
extern int D_003316B8[2];       /* movimento lateral pelo analógico */
extern int D_003316B0[2];       /* movimento frontal pelo analógico */
void *memset(void *, int, unsigned int);
void func_00225860(void *dst, int c, int n); /* Q_memset */
void func_00246490(void);
void Cmd_AddCommand(char *cmd_name, xcommand_t function);
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

void func_0024D648(void)
{
    int clientId;

    clientId = atoi(Cmd_Argv(1));
    if ((clientId == 0 && D_0033282C > 0) || (clientId == 1 && D_00332844 > 0))
    {
        if (D_0046EBC8[clientId].state & 1)
            func_0024C5B0(&D_0046EBC8[clientId]);
        else
            func_0024C500(&D_0046EBC8[clientId]);
    }
    else
    {
        func_0024C500(&D_0046EBC8[clientId]);
    }
}

void func_0024D710(void)
{
    int clientId;

    clientId = atoi(Cmd_Argv(1));
    if ((clientId == 0 && D_0033282C > 0) || (clientId == 1 && D_00332844 > 0))
        return;
    func_0024C5B0(&D_0046EBC8[clientId]);
}

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

void func_0024DA18(void)
{
    int clientId;

    clientId = atoi(Cmd_Argv(1));
    if (D_0046EBC8[clientId].state & 1)
        func_0024C5B0(&D_0046EBC8[clientId]);
    else
        func_0024C500(&D_0046EBC8[clientId]);
}

float func_0024DA78(kbutton_t *key)
{
    float val;
    int impulsedown, impulseup, down;

    impulsedown = key->state & 2;
    impulseup = key->state & 4;
    down = key->state & 1;
    val = 0;

    if (impulsedown && !impulseup)
    {
        if (down)
            val = 0.5;
        else
            val = 0;
    }
    if (impulseup && !impulsedown)
    {
        if (down)
            val = 0;
        else
            val = 0;
    }
    if (!impulsedown && !impulseup)
    {
        if (down)
            val = 1.0;
        else
            val = 0;
    }
    if (impulsedown && impulseup)
    {
        if (down)
            val = 0.75;
        else
            val = 0.25;
    }

    key->state &= 1;
    return val;
}

void func_0024DB18(int clientId)
{
    float speed;
    float up, down;

    if (D_0046E910[clientId].state & 1)
        speed = host_frametime * D_00331774;
    else
        speed = host_frametime;

    if (!(D_0046EA48[clientId].state & 1))
    {
        cl[clientId].viewangles[YAW] -= speed * D_00331738.value * func_0024DA78(&D_0046E9E8[clientId]);
        cl[clientId].viewangles[YAW] += speed * D_00331738.value * func_0024DA78(&D_0046EA78[clientId]);
        cl[clientId].viewangles[YAW] = anglemod(cl[clientId].viewangles[YAW]);
    }

    if (D_0046EA00[clientId].state & 1)
    {
        V_StopPitchDrift(clientId);
        cl[clientId].viewangles[PITCH] -= speed * D_00331750.value * func_0024DA78(&D_0046E940[clientId]);
        cl[clientId].viewangles[PITCH] += speed * D_00331750.value * func_0024DA78(&D_0046E958[clientId]);
    }

    up = func_0024DA78(&D_0046EB50[clientId]);
    down = func_0024DA78(&D_0046E928[clientId]);

    cl[clientId].viewangles[PITCH] -= speed * D_00331750.value * up;
    cl[clientId].viewangles[PITCH] += speed * D_00331750.value * down;

    if (up || down)
        V_StopPitchDrift(clientId);

    if (cl[clientId].viewangles[PITCH] > D_00331AF4)
        cl[clientId].viewangles[PITCH] = D_00331AF4;
    if (cl[clientId].viewangles[PITCH] < -D_00331ADC)
        cl[clientId].viewangles[PITCH] = -D_00331ADC;

    if (cl[clientId].viewangles[ROLL] > 50)
        cl[clientId].viewangles[ROLL] = 50;
    if (cl[clientId].viewangles[ROLL] < -50)
        cl[clientId].viewangles[ROLL] = -50;
}

void func_0024DDF0(usercmd_t *cmd, int clientId)
{
    float k1, k2;
    float spd, fmov, fscale;

    if (cls[clientId].signon != 3)
        return;

    func_0024DB18(clientId);

    memset(cmd, 0, sizeof(*cmd));

    if (D_0046EA48[clientId].state & 1)
    {
        cmd->sidemove += D_00331708.value * func_0024DA78(&D_0046E9E8[clientId]);
        cmd->sidemove -= D_00331708.value * func_0024DA78(&D_0046EA78[clientId]);
    }

    k1 = func_0024DA78(&D_0046E9D0[clientId]);
    k2 = func_0024DA78(&D_0046EB08[clientId]);
    if (k2 != 0)
    {
        D_003316B8[clientId] = 1;
        cmd->sidemove += D_00331708.value * k2 * D_00399148[clientId];
    }
    else
    {
        D_003316B8[clientId] = 0;
        cmd->sidemove += D_00331708.value * k1;
    }

    k1 = func_0024DA78(&D_0046EAA8[clientId]);
    k2 = func_0024DA78(&D_0046E9B8[clientId]);
    if (k2 != 0)
    {
        D_003316B8[clientId] = 1;
        cmd->sidemove -= D_00331708.value * k2 * D_00399148[clientId];
    }
    else
    {
        D_003316B8[clientId] = 0;
        cmd->sidemove -= D_00331708.value * k1;
    }

    cmd->upmove += D_003316C0.value * func_0024DA78(&D_0046EBE0[clientId]);
    cmd->upmove -= D_003316C0.value * func_0024DA78(&D_0046E988[clientId]);

    if (!(D_0046EA00[clientId].state & 1))
    {
        k1 = func_0024DA78(&D_0046EA60[clientId]);
        k2 = func_0024DA78(&D_0046E940[clientId]);
        if (k2 != 0)
        {
            D_003316B0[clientId] = 1;
            cmd->forwardmove += cl_forwardspeed.value * k2 * D_00399158[clientId];
        }
        else
        {
            D_003316B0[clientId] = 0;
            cmd->forwardmove += cl_forwardspeed.value * k1;
        }

        k1 = func_0024DA78(&D_0046EB68[clientId]);
        k2 = func_0024DA78(&D_0046E958[clientId]);
        if (k2 != 0)
        {
            D_003316B0[clientId] = 1;
            cmd->forwardmove -= cl_forwardspeed.value * k2 * D_00399158[clientId];
        }
        else
        {
            D_003316B0[clientId] = 0;
            cmd->forwardmove -= cl_forwardspeed.value * k1;
        }
    }

    if (D_0046E910[clientId].state & 1)
    {
        cmd->forwardmove *= D_00331720.value;
        cmd->sidemove *= D_00331720.value;
        cmd->upmove *= D_00331720.value;
    }

    spd = cl[clientId].maxspeed;
    if (spd != 0)
    {
        fmov = sqrtf(cmd->forwardmove * cmd->forwardmove + cmd->sidemove * cmd->sidemove + cmd->upmove * cmd->upmove);
        if (fmov > spd)
        {
            fscale = spd / fmov;
            cmd->forwardmove *= fscale;
            cmd->sidemove *= fscale;
            cmd->upmove *= fscale;
        }
    }

    cmd->lightlevel = cl[clientId].light_level;
}

int func_0024E2E0(int bResetState, int clientId)
{
    int bits;

    bits = 0;

    if (D_0046EAD8[clientId].state & 3)
        bits |= IN_ATTACK;
    if (D_0046EBC8[clientId].state & 3)
        bits |= IN_DUCK;
    if (D_0046EAF0[clientId].state & 3)
        bits |= IN_JUMP;
    if (D_0046E940[clientId].state & 3)
        bits |= IN_FORWARD;
    if (D_0046EA60[clientId].state & 3)
        bits |= IN_FORWARD;
    if (D_0046E958[clientId].state & 3)
        bits |= IN_BACK;
    if (D_0046EB68[clientId].state & 3)
        bits |= IN_BACK;
    if (D_0046EBB0[clientId].state & 3)
        bits |= IN_USE;
    if (D_00399150[clientId])
        bits |= IN_CANCEL;
    if (D_0046EA78[clientId].state & 3)
        bits |= IN_LEFT;
    if (D_0046E9E8[clientId].state & 3)
        bits |= IN_RIGHT;
    if (D_0046E9B8[clientId].state & 3)
        bits |= IN_MOVELEFT;
    if (D_0046EAA8[clientId].state & 3)
        bits |= IN_MOVELEFT;
    if (D_0046EB08[clientId].state & 3)
        bits |= IN_MOVERIGHT;
    if (D_0046E9D0[clientId].state & 3)
        bits |= IN_MOVERIGHT;
    if (D_0046EB20[clientId].state & 3)
        bits |= IN_ATTACK2;
    if (D_0046EAC0[clientId].state & 3)
        bits |= IN_RELOAD;
    if (D_0046EC10[clientId].state & 3)
        bits |= IN_ALT1;
    if (D_0046EB80[clientId].state & 3)
        bits |= IN_SCORE;

    if (bResetState)
    {
        D_0046EAD8[clientId].state &= ~2;
        D_0046EBC8[clientId].state &= ~2;
        D_0046EAF0[clientId].state &= ~2;
        D_0046E940[clientId].state &= ~2;
        D_0046E958[clientId].state &= ~2;
        D_0046EBB0[clientId].state &= ~2;
        D_0046EA78[clientId].state &= ~2;
        D_0046E9E8[clientId].state &= ~2;
        D_0046E9B8[clientId].state &= ~2;
        D_0046EB08[clientId].state &= ~2;
        D_0046EB20[clientId].state &= ~2;
        D_0046EAC0[clientId].state &= ~2;
        D_0046EC10[clientId].state &= ~2;
        D_0046EB80[clientId].state &= ~2;
    }

    return bits;
}

void func_0024E670(int bits, int clientId)
{
    int bitsNew;

    bitsNew = func_0024E2E0(0, clientId) ^ bits;
    if (bitsNew & 1)
    {
        if (bits & 1)
            func_0024C500(&D_0046EAD8[clientId]);
        else
            D_0046EAD8[clientId].state &= ~7;
    }
}

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

void func_0024E700(void)
{
    Cmd_AddCommand("+moveup", func_0024CC28);
    Cmd_AddCommand("-moveup", func_0024CC68);
    Cmd_AddCommand("+movedown", func_0024CCA8);
    Cmd_AddCommand("-movedown", func_0024CCE8);
    Cmd_AddCommand("+left", func_0024CD28);
    Cmd_AddCommand("-left", func_0024CD68);
    Cmd_AddCommand("+right", func_0024CDA8);
    Cmd_AddCommand("-right", func_0024CDE8);
    Cmd_AddCommand("+forward", func_0024CE28);
    Cmd_AddCommand("-forward", func_0024CE68);
    Cmd_AddCommand("+back", func_0024CEA8);
    Cmd_AddCommand("-back", func_0024CEE8);
    Cmd_AddCommand("+kbforward", func_0024CF28);
    Cmd_AddCommand("-kbforward", func_0024CF68);
    Cmd_AddCommand("+kbback", func_0024CFA8);
    Cmd_AddCommand("-kbback", func_0024CFE8);
    Cmd_AddCommand("+lookup", func_0024D028);
    Cmd_AddCommand("-lookup", func_0024D068);
    Cmd_AddCommand("+lookdown", func_0024D0A8);
    Cmd_AddCommand("-lookdown", func_0024D0E8);
    Cmd_AddCommand("+strafe", func_0024D3A8);
    Cmd_AddCommand("-strafe", func_0024D3E8);
    Cmd_AddCommand("+moveleft", func_0024D128);
    Cmd_AddCommand("-moveleft", func_0024D168);
    Cmd_AddCommand("+moveright", func_0024D1A8);
    Cmd_AddCommand("-moveright", func_0024D1E8);
    Cmd_AddCommand("+kbmoveleft", func_0024D228);
    Cmd_AddCommand("-kbmoveleft", func_0024D268);
    Cmd_AddCommand("+kbmoveright", func_0024D2A8);
    Cmd_AddCommand("-kbmoveright", func_0024D2E8);
    Cmd_AddCommand("+speed", func_0024D328);
    Cmd_AddCommand("-speed", func_0024D368);
    Cmd_AddCommand("tspeed", func_0024D9B8);
    Cmd_AddCommand("+attack", func_0024D428);
    Cmd_AddCommand("-attack", func_0024D468);
    Cmd_AddCommand("+attack2", func_0024D4C8);
    Cmd_AddCommand("-attack2", func_0024D508);
    Cmd_AddCommand("+use", func_0024D548);
    Cmd_AddCommand("-use", func_0024D588);
    Cmd_AddCommand("+jump", func_0024D5C8);
    Cmd_AddCommand("-jump", func_0024D608);
    Cmd_AddCommand("impulse", func_0024D7E0);
    Cmd_AddCommand("+klook", func_0024C658);
    Cmd_AddCommand("-klook", func_0024C698);
    Cmd_AddCommand("+mlook", func_0024C758);
    Cmd_AddCommand("-mlook", func_0024C798);
    Cmd_AddCommand("+jlook", func_0024C6D8);
    Cmd_AddCommand("-jlook", func_0024C718);
    Cmd_AddCommand("+duck", func_0024D648);
    Cmd_AddCommand("-duck", func_0024D710);
    Cmd_AddCommand("tduck", func_0024DA18);
    Cmd_AddCommand("+reload", func_0024D838);
    Cmd_AddCommand("-reload", func_0024D878);
    Cmd_AddCommand("+alt1", func_0024D8B8);
    Cmd_AddCommand("-alt1", func_0024D8F8);
    Cmd_AddCommand("+alt2", func_0024D938);
    Cmd_AddCommand("-alt2", func_0024D978);
    Cmd_AddCommand("+leftStickYLook", func_0024C828);
    Cmd_AddCommand("-leftStickYLook", func_0024C868);
    Cmd_AddCommand("+leftStickXLook", func_0024C8A8);
    Cmd_AddCommand("-leftStickXLook", func_0024C8E8);
    Cmd_AddCommand("+rightStickYLook", func_0024C928);
    Cmd_AddCommand("-rightStickYLook", func_0024C968);
    Cmd_AddCommand("+rightStickXLook", func_0024C9A8);
    Cmd_AddCommand("-rightStickXLook", func_0024C9E8);
    Cmd_AddCommand("+leftStickForwardMove", func_0024CA28);
    Cmd_AddCommand("-leftStickForwardMove", func_0024CA68);
    Cmd_AddCommand("+leftStickSideMove", func_0024CAA8);
    Cmd_AddCommand("-leftStickSideMove", func_0024CAE8);
    Cmd_AddCommand("+rightStickForwardMove", func_0024CB28);
    Cmd_AddCommand("-rightStickForwardMove", func_0024CB68);
    Cmd_AddCommand("+rightStickSideMove", func_0024CBA8);
    Cmd_AddCommand("-rightStickSideMove", func_0024CBE8);
    func_00225860(in_leftStickYLook, 0, sizeof(in_leftStickYLook));
    func_00225860(in_leftStickXLook, 0, sizeof(in_leftStickXLook));
    func_00225860(in_rightStickYLook, 0, sizeof(in_rightStickYLook));
    func_00225860(in_rightStickXLook, 0, sizeof(in_rightStickXLook));
    func_00225860(in_leftStickForwardMove, 0, sizeof(in_leftStickForwardMove));
    func_00225860(in_leftStickSideMove, 0, sizeof(in_leftStickSideMove));
    func_00225860(in_rightStickForwardMove, 0, sizeof(in_rightStickForwardMove));
    func_00225860(in_rightStickSideMove, 0, sizeof(in_rightStickSideMove));
    func_00225860(in_mlook, 0, sizeof(in_mlook));
    func_00225860(D_0046EA00, 0, sizeof(D_0046EA00));
    func_00225860(in_jlook, 0, sizeof(in_jlook));
    func_00225860(D_0046EA78, 0, sizeof(D_0046EA78));
    func_00225860(D_0046E9E8, 0, sizeof(D_0046E9E8));
    func_00225860(D_0046E940, 0, sizeof(D_0046E940));
    func_00225860(D_0046E958, 0, sizeof(D_0046E958));
    func_00225860(D_0046EAA8, 0, sizeof(D_0046EAA8));
    func_00225860(D_0046E9D0, 0, sizeof(D_0046E9D0));
    func_00225860(D_0046EA60, 0, sizeof(D_0046EA60));
    func_00225860(D_0046EB68, 0, sizeof(D_0046EB68));
    func_00225860(D_0046EB50, 0, sizeof(D_0046EB50));
    func_00225860(D_0046E928, 0, sizeof(D_0046E928));
    func_00225860(D_0046E9B8, 0, sizeof(D_0046E9B8));
    func_00225860(D_0046EB08, 0, sizeof(D_0046EB08));
    func_00225860(D_0046EA48, 0, sizeof(D_0046EA48));
    func_00225860(D_0046E910, 0, sizeof(D_0046E910));
    func_00225860(D_0046EBB0, 0, sizeof(D_0046EBB0));
    func_00225860(D_0046EAF0, 0, sizeof(D_0046EAF0));
    func_00225860(D_0046EAD8, 0, sizeof(D_0046EAD8));
    func_00225860(D_0046EB20, 0, sizeof(D_0046EB20));
    func_00225860(D_0046EBE0, 0, sizeof(D_0046EBE0));
    func_00225860(D_0046E988, 0, sizeof(D_0046E988));
    func_00225860(D_0046EC10, 0, sizeof(D_0046EC10));
    func_00225860(D_0046EB80, 0, sizeof(D_0046EB80));
    func_00225860(D_0046EBC8, 0, sizeof(D_0046EBC8));
    func_00225860(D_0046EAC0, 0, sizeof(D_0046EAC0));
    func_00225860(D_00399160, 0, sizeof(D_00399160));
    func_00225860(D_00399150, 0, sizeof(D_00399150));
    func_00246490();
}
