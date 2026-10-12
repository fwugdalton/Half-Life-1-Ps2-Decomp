#include "common.h"
#include "stdarg.h"
#include "engine/cmd.h"
#include "engine/cvar.h"

/* sistema do PS2: erros, tempo, init do jogo e carga das "DLLs" (sys_dll.c/sys_dll2.c do HL) */

typedef struct quakeparms_s
{
    char *basedir;
    char *cachedir;
    int argc;
    char **argv;
    void *membase;
    int memsize;
} quakeparms_t;

#define MAX_NUM_ARGVS 50

int vsprintf(char *buf, const char *fmt, va_list ap);
void longjmp(void *env, int val) __attribute__((noreturn));
void *memset(void *, int, unsigned int);

float Q_atof(char *str);
unsigned long func_00229B88(void);
void Host_Shutdown(void);
int Host_Init(quakeparms_t *parms);
void Host_InitializeGameDLL(void);
void SeedRandomNumberGenerator(void);
void func_00244420(int argc, char **argv); /* COM_InitArgv */
void func_002C70A8(int);

extern int host_abortserver[];
extern int giActive;
extern float D_00334520;        /* curtime */
extern float D_00334524;        /* lastcurtime */
extern void (*D_00334528)(char *fmt, ...);
extern int D_0033452C;
extern volatile int D_00334530; /* sys_checksum */
extern int D_00334534;          /* em Sys_Error */
extern char *D_00334538;
extern int D_0033453C;
extern int D_00399754;
extern int D_00399798;
extern float D_0043C1C8;        /* pfreq */
extern int D_0043C1CC;          /* lowshift */
extern char D_0043B1C8[];
extern char D_0043B5C8[];
extern char *D_0053D818[MAX_NUM_ARGVS];
extern byte D_0053DB38[0x234];
extern char D_0053DDF8[0x40];

/* funções vindas do lançador */
extern void *D_003997E0, *D_003997C4, *D_003997B8, *D_00399784, *D_003997A8, *D_00399768;
extern void *D_003997AC, *D_00399778, *D_003997BC, *D_0039976C, *D_00399764, *D_00399790;
extern void *D_00399770, *D_003997D4, *D_00399788, *D_003997DC, *D_003997C0;
extern void (*D_003997B4)(int, char *);
extern int (*D_00399794)(void);
extern void *D_003997D0, *D_003997D8, *D_003997A4, *D_00399780, *D_003997CC, *D_003997B0;
extern void *D_00399758, *D_003997A0, *D_0039977C, *D_0039978C, *D_0039979C;

int func_002C5AE8(void)
{
    return (int)D_003997C4;
}

void func_002C5AF8(void *ptr, int size)
{
    byte *x;
    int m, n;

    x = (byte *)ptr;

    for (n = 0; n < 4; n++)
    {
        for (m = 0; m < (size - 16 * 0x1000); m += 4)
        {
            D_00334530 += *(int *)&x[m];
            D_00334530 += *(int *)&x[m + 16 * 0x1000];
        }
    }
}

void func_002C5B80(void) {
}

void func_002C5B88(void) {
}

void func_002C5B90(void) {
}

void func_002C5B98(void) {
}

void func_002C5BA0(void) {
}

void func_002C5BA8(void) {
}

void func_002C5EC8(void);

void func_002C5BB0(void)
{
    unsigned int lowpart, highpart;

    lowpart = 0x11940000;
    highpart = 0;
    D_0043C1CC = 0;

    while (highpart || (lowpart > 2000000.0f))
    {
        D_0043C1CC++;
        lowpart >>= 1;
        lowpart |= (highpart & 1) << 31;
        highpart >>= 1;
    }

    D_0043C1C8 = 1.0f / (float)lowpart;

    func_002C5EC8();
}

void Sys_Error(const char *error, ...)
{
    va_list argptr;
    char text[1024];

    va_start(argptr, error);
    vsprintf(text, error, argptr);
    va_end(argptr);

    if (D_00334534)
        longjmp(host_abortserver, 2);

    D_00334534 = 1;
}

void func_002C5D20(char *fmt, ...)
{
    va_list argptr;
    char text[1024];

    D_00399794();

    va_start(argptr, fmt);
    vsprintf(text, fmt, argptr);
    va_end(argptr);

    if (cls_demoplayback)
    {
        if (D_00334528)
            D_00334528("WARNING:  %s\n", text);
    }
    else if (D_003997B4)
    {
        D_003997B4(1, text);
    }

    giActive = 2;
}

void Sys_Printf(char *fmt, ...)
{
    va_list argptr;
    char text[1024];

    va_start(argptr, fmt);
    vsprintf(text, fmt, argptr);
    va_end(argptr);
}

void func_002C5E30(void)
{
    D_00399794();
    Host_Shutdown();
    giActive = 3;
    longjmp(host_abortserver, 1);
}

float Sys_FloatTime(void)
{
    return func_00229B88() * 0.001f;
}

void func_002C5EC8(void)
{
    int j;

    Sys_FloatTime();

    j = COM_CheckParm("-starttime");
    if (j)
        D_00334520 = Q_atof(com_argv[j + 1]);
    else
        D_00334520 = 0;

    D_00334524 = D_00334520;
}

void func_002C5F30(void) {
}

void func_002C5F38(void) {
}

int func_002C5F40(void)
{
    return cls[0].state;
}

void func_002C5F50(int state)
{
    D_00399798 = state;
}

void func_002C5F60(int flags)
{
    if (flags & 2)
        func_002C5F50(1);
    else if (flags != 1)
        func_002C5F50(flags);
}

void func_002C5FA0(int active)
{
    giActive = active;
}

void func_002C5FB0(int x)
{
    D_0033453C = x;
}

#ifdef NON_MATCHING
int func_002C5FC0(char *lpCmdLine, void *membase, int memsize, void **funcs, char *basedir, int dedicated)
{
    char *cmdline;
    quakeparms_t parms;

    memset(D_0053DB38, 0, sizeof(D_0053DB38));
    strcpy(D_0043B5C8, lpCmdLine);
    cmdline = D_0043B5C8;
    host_initialized = 0;
    memset(D_0053DDF8, 0, sizeof(D_0053DDF8));
    strcpy(D_0053DDF8, basedir);

    D_003997E0 = funcs[0];
    D_003997C4 = funcs[1];
    D_003997B8 = funcs[2];
    D_00399784 = funcs[3];
    D_003997A8 = funcs[4];
    D_00399768 = funcs[5];
    D_003997AC = funcs[6];
    D_00399794 = funcs[7];
    D_00399778 = funcs[8];
    D_003997BC = funcs[9];
    D_0039976C = funcs[10];
    D_00399790 = funcs[11];
    D_00399770 = funcs[12];
    D_003997D4 = funcs[13];
    D_003997DC = funcs[15];
    D_00399788 = funcs[16];
    D_0033452C = (int)funcs[17];
    D_003997D8 = funcs[18];
    D_003997B4 = funcs[19];
    D_00399764 = funcs[20];
    D_00334528 = funcs[21];
    D_003997A4 = funcs[22];
    D_00399780 = funcs[25];
    D_003997CC = funcs[26];
    D_003997B0 = funcs[27];
    D_00399758 = funcs[28];
    D_003997A0 = funcs[29];
    D_0039977C = funcs[30];
    D_0039978C = funcs[31];
    D_0039979C = funcs[32];
    D_003997C0 = funcs[33];
    D_003997D0 = funcs[34];

    if (dedicated)
        D_0039979C = 0;

    D_00399754 = 1;
    SeedRandomNumberGenerator();
    func_002C5BA8();

    strcpy(D_0043B1C8, "");
    if (D_0043B1C8[Q_strlen(D_0043B1C8) - 1] == '/')
        D_0043B1C8[Q_strlen(D_0043B1C8) - 1] = 0;

    parms.basedir = D_0043B1C8;
    parms.cachedir = NULL;
    parms.argc = 1;
    D_0053D818[0] = D_00334538;

    if (*cmdline)
    {
        while (*cmdline && (parms.argc < MAX_NUM_ARGVS))
        {
            while (*cmdline && ((*cmdline <= 32) || (*cmdline > 126)))
                cmdline++;

            if (*cmdline)
            {
                D_0053D818[parms.argc] = cmdline;
                parms.argc++;

                while (*cmdline && ((*cmdline > 32) && (*cmdline <= 126)))
                    cmdline++;

                if (*cmdline)
                {
                    *cmdline = 0;
                    cmdline++;
                }
            }
        }
    }

    parms.argv = D_0053D818;
    func_00244420(parms.argc, parms.argv);
    parms.argc = com_argc;
    parms.argv = com_argv;
    parms.membase = membase;
    parms.memsize = memsize;

    cls_demoplayback = dedicated;
    func_002C5BB0();
    Sys_Printf("Host_Init\n");

    if (!Host_Init(&parms))
        return 0;

    Host_InitializeGameDLL();

    if (cls_demoplayback)
    {
        Host_InitializeGameDLL();
        func_002C70A8(1);
    }

    return 1;
}
#else
__asm__(".section .rodata\n.align 3\n.section .text");
INCLUDE_RODATA("asm/nonmatchings/engine/sys_ps2", D_00395580);

INCLUDE_ASM("asm/nonmatchings/engine/sys_ps2", func_002C5FC0);
#endif

INCLUDE_ASM("asm/nonmatchings/engine/sys_ps2", func_002C63C0);

INCLUDE_ASM("asm/nonmatchings/engine/sys_ps2", func_002C63F8);

INCLUDE_ASM("asm/nonmatchings/engine/sys_ps2", func_002C6458);

INCLUDE_ASM("asm/nonmatchings/engine/sys_ps2", func_002C6468);

INCLUDE_ASM("asm/nonmatchings/engine/sys_ps2", func_002C6478);

INCLUDE_ASM("asm/nonmatchings/engine/sys_ps2", func_002C64D8);

INCLUDE_ASM("asm/nonmatchings/engine/sys_ps2", func_002C64E8);

INCLUDE_ASM("asm/nonmatchings/engine/sys_ps2", func_002C6508);

INCLUDE_ASM("asm/nonmatchings/engine/sys_ps2", func_002C6510);

INCLUDE_ASM("asm/nonmatchings/engine/sys_ps2", func_002C6548);

INCLUDE_ASM("asm/nonmatchings/engine/sys_ps2", func_002C65D8);

INCLUDE_ASM("asm/nonmatchings/engine/sys_ps2", func_002C6668);

INCLUDE_ASM("asm/nonmatchings/engine/sys_ps2", GetEntityInit);

INCLUDE_ASM("asm/nonmatchings/engine/sys_ps2", func_002C6718);

INCLUDE_ASM("asm/nonmatchings/engine/sys_ps2", func_002C6738);

INCLUDE_ASM("asm/nonmatchings/engine/sys_ps2", func_002C68B0);

INCLUDE_ASM("asm/nonmatchings/engine/sys_ps2", func_002C6A20);
