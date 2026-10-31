#include "common.h"
#include "stdarg.h"
#include "engine/cmd.h"
#include "engine/cvar.h"
#include "engine/server.h"
#include "engine/eiface.h"
#include "engine/progdefs.h"
#include "engine/client.h"

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

typedef struct modinfo_s
{
    qboolean bIsMod;            /* 0x000 */
    char szInfo[256];           /* 0x004 */
    char szDL[256];             /* 0x104 */
    char szHLVersion[32];       /* 0x204 */
    int version;                /* 0x224 */
    int size;                   /* 0x228 */
    qboolean svonly;            /* 0x22C */
    qboolean cldll;             /* 0x230 */
} modinfo_t;                    /* 0x234 */

typedef struct functiontable_s
{
    unsigned int pFunction;
    char *pFunctionName;
} functiontable_t;

typedef struct extensiondll_s
{
    int lDLLHandle;
    functiontable_t *functionTable;
    int functionCount;
} extensiondll_t;

#define MAX_EXTENSION_DLL 50

extern modinfo_t D_0053DB38;    /* gmodinfo */
extern extensiondll_t D_0053D8E0[MAX_EXTENSION_DLL]; /* g_rgextdll */
extern int D_00334734;          /* g_iextdllMac */
extern int D_00334544;
extern int D_00334548;
extern int D_0033454C;
extern int D_00334510;
extern int D_00334514;
extern int D_00334550;          /* g_engfuncsExportedToDlls */

void Key_SetBinding(int keynum, char *binding);
int strcmp(const char *, const char *);
int atoi(const char *);
void *func_0021D8D0(char *pClassName);
void func_0027D340(void);
int func_00144E98(DLL_FUNCTIONS *pFunctionTable, int interfaceVersion); /* GetEntityAPI */
void GiveFnptrsToDll(void *pengfuncsFromEngine, globalvars_t *pGlobals); /* GiveFnptrsToDll */
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

    memset(&D_0053DB38, 0, sizeof(D_0053DB38));
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

void func_002C63C0(char *unused)
{
    Cbuf_AddText("exec config.cfg\n");
    Cbuf_Execute();
    Key_SetBinding(27, "cancelselect");
}

void func_002C63F8(char *basedir)
{
    memset(D_0053DDF8, 0, sizeof(D_0053DDF8));
    strcpy(D_0053DDF8, basedir);
    D_00334544 = 0;
    func_002C63C0(basedir);
}

void func_002C6458(int x)
{
    D_00334510 = x;
}

void func_002C6468(int x)
{
    D_00334514 = x;
}

void func_002C6478(qboolean paused)
{
    if (paused)
    {
        if (!D_0033454C)
            D_00334548 = sv.paused;
        sv.paused = 1;
        D_0033454C = 1;
    }
    else
    {
        if (D_0033454C)
            sv.paused = D_00334548;
        D_0033454C = 0;
    }
}

int func_002C64D8(void)
{
    return sv.paused;
}

void func_002C64E8(void)
{
    Con_DPrintf("KB Reset\n");
}

/* não usada: só sobraram as strings no binário */
static __inline__ void func_002C64E8_dlls(char *dst)
{
    strcpy(dst, "valve/dlls");
    strcpy(dst, "hl.dll");
    strcpy(dst, "base.dll");
}

int func_002C6508(void)
{
    return 0;
}

char *func_002C6510(extensiondll_t *pDll, unsigned int function)
{
    int i;

    for (i = 0; i < pDll->functionCount; i++)
    {
        if (pDll->functionTable[i].pFunction == function)
            return pDll->functionTable[i].pFunctionName;
    }

    return NULL;
}

unsigned int func_002C6548(extensiondll_t *pDll, const char *pName)
{
    int i;

    for (i = 0; i < pDll->functionCount; i++)
    {
        if (!strcmp(pName, pDll->functionTable[i].pFunctionName))
            return pDll->functionTable[i].pFunction;
    }

    return 0;
}

unsigned int func_002C65D8(const char *pName)
{
    int i;
    unsigned int function;

    for (i = 0; i < D_00334734; i++)
    {
        function = func_002C6548(&D_0053D8E0[i], pName);
        if (function)
            return function;
    }

    Con_Printf("Can't find proc: %s\n", pName);
    return 0;
}

char *func_002C6668(unsigned int function)
{
    int i;
    char *pName;

    for (i = 0; i < D_00334734; i++)
    {
        pName = func_002C6510(&D_0053D8E0[i], function);
        if (pName)
            return pName;
    }

    Con_Printf("Can't find address: %08lx\n", function);
    return NULL;
}

ENTITYINIT GetEntityInit(char *pClassName)
{
    return (ENTITYINIT)func_0021D8D0(pClassName);
}

int func_002C6718(void)
{
    return func_002C6508();
}

void func_002C6738(modinfo_t *pinfo, char *pkey, char *pvalue)
{
    if (!strcasecmp(pkey, "url_info"))
    {
        pinfo->bIsMod = 1;
        strcpy(pinfo->szInfo, pvalue);
    }
    else if (!strcasecmp(pkey, "url_dl"))
    {
        pinfo->bIsMod = 1;
        strcpy(pinfo->szDL, pvalue);
    }
    else if (!strcasecmp(pkey, "version"))
    {
        pinfo->bIsMod = 1;
        pinfo->version = atoi(pvalue);
    }
    else if (!strcasecmp(pkey, "size"))
    {
        pinfo->bIsMod = 1;
        pinfo->size = atoi(pvalue);
    }
    else if (!strcasecmp(pkey, "svonly"))
    {
        pinfo->bIsMod = 1;
        pinfo->svonly = atoi(pvalue) ? 1 : 0;
    }
    else if (!strcasecmp(pkey, "cldll"))
    {
        pinfo->bIsMod = 1;
        pinfo->cldll = atoi(pvalue) ? 1 : 0;
    }
    else if (!strcasecmp(pkey, "hlversion"))
    {
        strcpy(pinfo->szHLVersion, pvalue);
    }

    Con_Printf("DLL:  %s - %s\n", pkey, pvalue);
}

void func_002C6A20(void);

void func_002C68B0(void)
{
    func_0027D340();

    D_00334734 = 0;
    memset(D_0053D8E0, 0, sizeof(D_0053D8E0));

    func_002C6A20();

    if (!func_00144E98(&gEntityInterface, 0x8A))
    {
        Sys_Error("Invalid DLL version!");
        return;
    }

    Con_Printf("----------------------\n");
    Con_Printf("Dlls loaded for game:\n%s\n", gEntityInterface.pfnGetGameDescription());

    if (D_0053DB38.bIsMod)
    {
        Con_Printf("Modification info:\n");
        Con_Printf("Site:  %s\n", D_0053DB38.szInfo);
        Con_Printf("D/L :  %s\n", D_0053DB38.szDL);
        Con_Printf("Ver.:  %i\n", D_0053DB38.version);
        Con_Printf("Size:  %.2f MB\n", D_0053DB38.size / (1024.0f * 1024.0f));
        if (D_0053DB38.svonly)
            Con_Printf("Server side\n");
        if (D_0053DB38.cldll)
            Con_Printf("New client.dll\n");
    }
    else
    {
        Con_Printf(" -- unmodified game.\n");
    }

    Con_Printf("----------------------\n");
}

void func_002C6A20(void)
{
    extensiondll_t *pextdll;

    GiveFnptrsToDll(&D_00334550, &gGlobalVariables);

    pextdll = &D_0053D8E0[D_00334734++];
    memset(pextdll, 0, sizeof(*pextdll));
    pextdll->lDLLHandle = 1;
}

/* rede do PS2: só existe o loopback (net_ws.c do HL reduzido, no mesmo arquivo:
   a tabela do switch de func_002C6B18 só fica alinhada em 16 contando desde 0x295560) */

#define NA_LOOPBACK 1
#define MAX_LOOPBACK 4

typedef struct loopmsg_s
{
    byte data[0x3720];
    int datalen;                /* 0x3720 */
    int sock;                   /* 0x3724 */
} loopmsg_t;                    /* 0x3728 */

typedef struct loopback_s
{
    loopmsg_t msgs[MAX_LOOPBACK];
    int get;                    /* 0xDCA0 */
    int send;                   /* 0xDCA4 */
} loopback_t;                   /* 0xDCA8 */

int vsprintf(char *buf, const char *fmt, va_list ap);
int sprintf(char *buf, const char *fmt, ...);
int strlen(const char *s);
void *memset(void *, int, unsigned int);
void *memcpy(void *, const void *, unsigned int);
void func_00222500(void *f, char *fmt, ...);
void Log_Printf(const char *fmt, ...);
int COM_CheckParm(char *parm);
int Q_atoi(char *str);

extern int D_004E2BD4;
extern char D_0043B9C8[];
extern char D_0043BDC8[1024] __attribute__((aligned(8)));
extern char D_0043C1D0[];
extern char D_0043C210[];
extern loopback_t D_0043C250[];
extern int D_00334738;
extern int D_0033473C;
extern byte D_0053DE38[];
extern int gfNoMasterServer;
extern cvar_t developer;
extern netadr_t net_from;
extern sizebuf_t net_message;

void NET_Shutdown(void)
{
    D_004E2BD4 = 0;
}

void func_002C6AA0(void *f, char *fmt, ...)
{
    va_list argptr;

    va_start(argptr, fmt);
    vsprintf(D_0043B9C8, fmt, argptr);
    va_end(argptr);
    func_00222500(f, "%s", D_0043B9C8);
}

void func_002C6B18(int level, char *fmt, ...)
{
    va_list argptr;

    if (level == 5 && svs.maxclients >= 2)
    {
        va_start(argptr, fmt);
        vsprintf(D_0043BDC8, fmt, argptr);
        va_end(argptr);
        Log_Printf("%s", D_0043BDC8);
        return;
    }
    if (developer.value == 0.0f)
        return;
    switch (level)
    {
    case 0:
        strcpy(D_0043BDC8, "NOTE:  ");
        break;
    case 2:
        if (developer.value < 2.0f)
            return;
    case 1:
        D_0043BDC8[0] = 0;
        break;
    case 3:
        strcpy(D_0043BDC8, "WARNING:  ");
        break;
    case 4:
        strcpy(D_0043BDC8, "ERROR:  ");
        break;
    }
    va_start(argptr, fmt);
    vsprintf(D_0043BDC8 + strlen(D_0043BDC8), fmt, argptr);
    va_end(argptr);
    Con_Printf("%s", D_0043BDC8);
}

qboolean NET_CompareAdr(netadr_t a, netadr_t b)
{
    if (a.type != b.type)
        return false;
    return *(int *)a.ip == *(int *)b.ip;
}

#ifdef NON_MATCHING
qboolean func_002C6CF8(netadr_t a, netadr_t b)
{
    return NET_CompareAdr(a, b);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/sys_ps2", func_002C6CF8);
#endif

#ifdef NON_MATCHING
qboolean func_002C6D28(netadr_t a, netadr_t b)
{
    return NET_CompareAdr(a, b);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/sys_ps2", func_002C6D28);
#endif

qboolean func_002C6D58(netadr_t adr)
{
    return adr.type == NA_LOOPBACK;
}

#ifdef NON_MATCHING
qboolean func_002C6D78(netadr_t a, netadr_t b)
{
    return NET_CompareAdr(a, b);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/sys_ps2", func_002C6D78);
#endif

INCLUDE_RODATA("asm/nonmatchings/engine/sys_ps2", D_003957B8);

INCLUDE_RODATA("asm/nonmatchings/engine/sys_ps2", D_003957C8);

INCLUDE_RODATA("asm/nonmatchings/engine/sys_ps2", D_003957D8);

INCLUDE_RODATA("asm/nonmatchings/engine/sys_ps2", D_003957E0);

INCLUDE_RODATA("asm/nonmatchings/engine/sys_ps2", D_003957E8);

INCLUDE_RODATA("asm/nonmatchings/engine/sys_ps2", D_003957F8);

INCLUDE_RODATA("asm/nonmatchings/engine/sys_ps2", D_00395800);

char *NET_AdrToString(netadr_t a)
{
    memset(D_0043C1D0, 0, 64);
    if (a.type == NA_LOOPBACK)
        sprintf(D_0043C1D0, "loopback");
    return D_0043C1D0;
}

char *func_002C6E08(netadr_t a)
{
    memset(D_0043C210, 0, 64);
    if (a.type == NA_LOOPBACK)
        sprintf(D_0043C210, "loopback");
    return D_0043C210;
}

qboolean NET_StringToAdr(char *s, netadr_t *a)
{
    memset(a, 0, sizeof(*a));
    a->type = NA_LOOPBACK;
    return true;
}

qboolean NET_IsLocalAddress(netadr_t adr)
{
    return adr.type == NA_LOOPBACK;
}

qboolean func_002C6EC8(int sock, netadr_t *from, sizebuf_t *message)
{
    int i;
    loopback_t *loop;

    loop = &D_0043C250[sock];

    if (loop->send - loop->get > MAX_LOOPBACK)
        loop->get = loop->send - MAX_LOOPBACK;

    if (loop->get >= loop->send)
        return false;

    i = loop->get & (MAX_LOOPBACK - 1);
    loop->get++;

    memcpy(message->data, loop->msgs[i].data, loop->msgs[i].datalen);
    message->cursize = loop->msgs[i].datalen;
    memset(from, 0, sizeof(*from));
    from->type = NA_LOOPBACK;
    *(int *)from->ip = loop->msgs[i].sock;
    return true;
}

void func_002C6FB8(int sock, int length, void *data, netadr_t to, int id)
{
    int i;
    loopback_t *loop;

    loop = &D_0043C250[id];

    i = loop->send & (MAX_LOOPBACK - 1);
    loop->send++;

    memcpy(loop->msgs[i].data, data, length);
    loop->msgs[i].datalen = length;
    loop->msgs[i].sock = sock;
}

qboolean NET_GetPacket(int sock)
{
    return func_002C6EC8(sock, &net_from, &net_message);
}

#ifdef NON_MATCHING
void NET_SendPacket(int sock, int length, void *data, netadr_t to, int id)
{
    func_002C6FB8(sock, length, data, to, id);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/sys_ps2", NET_SendPacket);
#endif

void func_002C70A0(void)
{
}

void func_002C70A8(int multiplayer)
{
    D_0033473C = 1;
    D_00334738 = 1;
}

void func_002C7168(void);
void func_002C7268(void);

#ifdef NON_MATCHING
void NET_Init(void)
{
    Cmd_AddCommand("maxplayers", func_002C7168);
    Cmd_AddCommand("numclients", func_002C7268);

    net_message.data = D_0053DE38;
    net_message.maxsize = 0x3714;

    D_00334738 = 1;
    D_0033473C = 1;

    if (COM_CheckParm("-nomaster"))
    {
        Con_Printf("Master server communication disabled.\n");
        gfNoMasterServer = 1;
    }

    Con_DPrintf("Base networking initialized.\n");
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/sys_ps2", NET_Init);
#endif

/* MaxPlayers_f */
void func_002C7168(void)
{
    int n;

    if (Cmd_Argc() != 2)
    {
        Con_Printf("\"maxplayers\" is \"%u\"\n", svs.maxclients);
        return;
    }

    if (sv.active)
    {
        Con_Printf("maxplayers cannot be changed while a server is running.\n");
        return;
    }

    n = Q_atoi(Cmd_Argv(1));
    if (n < 1)
        n = 1;
    if (n > svs.maxclientslimit)
    {
        n = svs.maxclientslimit;
        Con_Printf("\"maxplayers\" set to \"%u\"\n", n);
    }

    svs.maxclients = n;

    if (n == 1)
        Cvar_Set("deathmatch", "0");
    else
        Cvar_Set("deathmatch", "1");
}

/* NumClients_f */
void func_002C7268(void)
{
    if (Cmd_Argc() == 2)
    {
        cl_numlocal = Q_atoi(Cmd_Argv(1));
        if (cl_numlocal < 1)
            cl_numlocal = 1;
    }
    else
    {
        Con_Printf("\"Num Clients\" is \"%u\"\n", cl_numlocal);
    }
}

/* a rodata do próximo arquivo começa alinhada em 16 */
__asm__(".section .rodata\n.align 4\n.section .text");
