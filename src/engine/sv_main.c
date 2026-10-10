#include "common.h"

#include "common.h"
#include "engine/mathlib.h"
#include "engine/model.h"
#include "engine/server.h"
#include "engine/cvar.h"
#include "engine/cmd.h"

extern edict_t **g_moved_edict;
extern vec3_t *g_moved_from;
extern byte *g_playertouch;
extern char localinfo[];

double atof(const char *s);
int atoi(const char *s);
char *CopyString(char *in);
char *Info_Serverinfo(void);
void Info_SetValueForKey(char *s, char *key, char *value, int maxsize);
void Info_Print(char *s);
char *NET_AdrToString(netadr_t a);
qboolean NET_StringToAdr(char *s, netadr_t *a);
void SV_BroadcastCommand(char *fmt, ...);

#define MAX_SERVERINFO_STRING 512
#define MAX_LOCALINFO 0x8000

void SV_DeallocateDynamicData(void)
{
    if (g_moved_edict)
        Mem_Free(g_moved_edict);
    if (g_moved_from)
        Mem_Free(g_moved_from);
    if (g_playertouch)
        Mem_Free(g_playertouch);

    g_moved_edict = NULL;
    g_moved_from = NULL;
    g_playertouch = NULL;
}
/* rodata não migrada: início */
__asm__(".section .rodata\n"
        ".globl D_0038E628\n"
        "D_0038E628:\n"
        ".byte 0x00\n"
        ".byte 0x00\n"
        ".byte 0x00\n"
        ".byte 0x00\n"
        ".byte 0x00\n"
        ".byte 0x00\n"
        ".byte 0x00\n"
        ".byte 0x00\n"
        ".align 3\n"
        ".globl D_0038E630\n"
        "D_0038E630:\n"
        ".asciz \"0\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E638\n"
        "D_0038E638:\n"
        ".asciz \"sv_minrate\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E648\n"
        "D_0038E648:\n"
        ".asciz \"sv_maxrate\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E658\n"
        "D_0038E658:\n"
        ".asciz \"sv_logrelay\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E668\n"
        "D_0038E668:\n"
        ".asciz \"sv_language\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E678\n"
        "D_0038E678:\n"
        ".asciz \"1\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E680\n"
        "D_0038E680:\n"
        ".asciz \"violence_hblood\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E690\n"
        "D_0038E690:\n"
        ".asciz \"violence_ablood\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E6A0\n"
        "D_0038E6A0:\n"
        ".asciz \"violence_hgibs\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E6B0\n"
        "D_0038E6B0:\n"
        ".asciz \"violence_agibs\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E6C0\n"
        "D_0038E6C0:\n"
        ".asciz \"sv_newunit\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E6D0\n"
        "D_0038E6D0:\n"
        ".asciz \"sv_aim\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E6D8\n"
        "D_0038E6D8:\n"
        ".asciz \"showtriggers\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E6E8\n"
        "D_0038E6E8:\n"
        ".asciz \"laddermode\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E6F8\n"
        "D_0038E6F8:\n"
        ".asciz \"sv_clienttrace\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E708\n"
        "D_0038E708:\n"
        ".asciz \"65\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E710\n"
        "D_0038E710:\n"
        ".asciz \"sv_timeout\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E720\n"
        "D_0038E720:\n"
        ".asciz \"15.0\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E728\n"
        "D_0038E728:\n"
        ".asciz \"sv_challengetime\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E740\n"
        "D_0038E740:\n"
        ".asciz \"1.0\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E748\n"
        "D_0038E748:\n"
        ".asciz \"sv_cheats\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E758\n"
        "D_0038E758:\n"
        ".asciz \"sv_spectator_password\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E770\n"
        "D_0038E770:\n"
        ".asciz \"8\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E778\n"
        "D_0038E778:\n"
        ".asciz \"sv_maxspectators\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E790\n"
        "D_0038E790:\n"
        ".asciz \"sv_spectalk\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E7A0\n"
        "D_0038E7A0:\n"
        ".asciz \"sv_password\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E7B0\n"
        "D_0038E7B0:\n"
        ".asciz \"sv_lan\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E7B8\n"
        "D_0038E7B8:\n"
        ".asciz \"cheat_slowmo\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E7C8\n"
        "D_0038E7C8:\n"
        ".asciz \"cheat_infammo\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E7D8\n"
        "D_0038E7D8:\n"
        ".asciz \"cheat_notarget\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E7E8\n"
        "D_0038E7E8:\n"
        ".asciz \"cheat_godmode\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E7F8\n"
        "D_0038E7F8:\n"
        ".asciz \"-1\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E800\n"
        "D_0038E800:\n"
        ".asciz \"cheat_gravity\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E810\n"
        "D_0038E810:\n"
        ".asciz \"cheat_slavemode\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E820\n"
        "D_0038E820:\n"
        ".asciz \"intf_playerdead\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E830\n"
        "D_0038E830:\n"
        ".asciz \"sv_netsize\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E840\n"
        "D_0038E840:\n"
        ".asciz \"sv_allowdownload\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E858\n"
        "D_0038E858:\n"
        ".asciz \"sv_allowupload\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E868\n"
        "D_0038E868:\n"
        ".asciz \"sv_upload_maxsize\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E880\n"
        "D_0038E880:\n"
        ".asciz \"sv_showcmd\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038E890\n"
        "D_0038E890:\n"
        ".asciz \"sv_smartdelta\"\n"
        ".align 2\n"
        ".align 3\n"
        ".section .text\n");
/* rodata não migrada: fim */
void SV_ReallocateDynamicData(void)
{
    int nSize;

    if (!sv.max_edicts)
    {
        Con_DPrintf("SV_ReallocateDynamicData with sv.max_edicts == 0");
        return;
    }

    nSize = sv.max_edicts;

    if (g_moved_edict)
        Con_Printf("Reallocate on moved_edict\n");
    g_moved_edict = Mem_DbgMalloc(nSize * sizeof(edict_t *), "engine/Sv_main.c", 166);
    memset(g_moved_edict, 0, nSize * sizeof(edict_t *));

    if (g_moved_from)
        Con_Printf("Reallocate on moved_from\n");
    g_moved_from = Mem_DbgMalloc(nSize * sizeof(vec3_t), "engine/Sv_main.c", 172);
    memset(g_moved_from, 0, nSize * sizeof(vec3_t));

    if (g_playertouch)
        Con_Printf("Reallocate on playertouch\n");
    g_playertouch = Mem_DbgMalloc((nSize + 7) / 8, "engine/Sv_main.c", 178);
    memset(g_playertouch, 0, (nSize + 7) / 8);
}
void SV_Serverinfo_f(void)
{
    cvar_t *var;

    if (Cmd_Argc() == 1)
    {
        Con_Printf("Server info settings:\n");
        Info_Print(Info_Serverinfo());
        return;
    }

    if (Cmd_Argc() != 3)
    {
        Con_Printf("usage: serverinfo [ <key> <value> ]\n");
        return;
    }

    if (Cmd_Argv(1)[0] == '*')
    {
        Con_Printf("Star variables cannot be changed.\n");
        return;
    }

    Info_SetValueForKey(Info_Serverinfo(), Cmd_Argv(1), Cmd_Argv(2), MAX_SERVERINFO_STRING);

    /* se for uma cvar, muda ela também */
    var = Cvar_FindVar(Cmd_Argv(1));
    if (var)
    {
        Z_Free(var->string); /* libera a string antiga */
        var->string = CopyString(Cmd_Argv(2));
        var->value = atof(var->string);
    }

    SV_BroadcastCommand("fullserverinfo \"%s\"\n", Info_Serverinfo());
}
void SV_Localinfo_f(void)
{
    if (Cmd_Argc() == 1)
    {
        Con_Printf("Local info settings:\n");
        Info_Print(localinfo);
        return;
    }

    if (Cmd_Argc() != 3)
    {
        Con_Printf("usage: localinfo [ <key> <value> ]\n");
        return;
    }

    if (Cmd_Argv(1)[0] == '*')
    {
        Con_Printf("Star variables cannot be changed.\n");
        return;
    }

    Info_SetValueForKey(localinfo, Cmd_Argv(1), Cmd_Argv(2), MAX_LOCALINFO);
}
void SV_SetLogAddress_f(void)
{
    char *s;
    int nPort;
    char szAdr[260];
    netadr_t adr;

    if (Cmd_Argc() != 3)
    {
        Con_Printf("logaddress:  usage\nlogaddress ip port\n");
        if (svs.log.net_log)
            Con_Printf("current:  %s\n", NET_AdrToString(svs.log.net_address));
        return;
    }

    s = Cmd_Argv(1);
    nPort = atoi(Cmd_Argv(2));
    if (!nPort)
    {
        Con_Printf("logaddress:  must specify a valid port\n");
        return;
    }

    if (!s || *s == '\0')
    {
        Con_Printf("logaddress:  unparseable address\n");
        return;
    }

    sprintf(szAdr, "%s:%i", s, nPort);

    if (NET_StringToAdr(szAdr, &adr))
    {
        svs.log.net_log = true;
        svs.log.net_address = adr;
        Con_Printf("logaddress:  %s\n", NET_AdrToString(adr));
    }
    else
        Con_Printf("logaddress:  unable to resolve %s\n", szAdr);
}
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", SV_ServerLog_f);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", SV_User_f);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", SV_Users_f);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", SV_CountPlayers);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027B030);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", SV_StartParticle);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", SV_StartSound);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", SV_BuildSoundMsg);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027B640);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027B6B0);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027B820);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027B8D8);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", Host_IsSinglePlayerGame);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027B9F8);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", SV_Multicast);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027BBC8);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027BCD0);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027BF20);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027BF28);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027C1B8);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027C500);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027C5C8);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027C658);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027C6E0);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027C7F8);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027D098);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027D0F0);
/* rodata não migrada: início */
__asm__(".section .rodata\n"
        ".globl D_0038EFD0\n"
        "D_0038EFD0:\n"
        ".asciz \"none\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038EFD8\n"
        "D_0038EFD8:\n"
        ".asciz \"password\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038EFE8\n"
        "D_0038EFE8:\n"
        ".asciz \"%s:  password failed\\n\"\n"
        ".align 2\n"
        ".align 3\n"
        ".asciz \"%s:spectator password failed\\n\"\n"
        ".align 2\n"
        ".align 3\n"
        ".asciz \"Invalid spectator password.\\n\"\n"
        ".align 2\n"
        ".align 3\n"
        ".asciz \"*spectator\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038F050\n"
        "D_0038F050:\n"
        ".asciz \"name\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038F058\n"
        "D_0038F058:\n"
        ".asciz \"unconnected\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".asciz \"%s:No space for spectator\\n\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".asciz \"No more spectators allowed.\\n\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038F0A8\n"
        "D_0038F0A8:\n"
        ".asciz \"Server is full.\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038F0B8\n"
        "D_0038F0B8:\n"
        ".asciz \"Invalid authentication userid.\\n\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038F0D8\n"
        "D_0038F0D8:\n"
        ".asciz \"Invalid authentication message.\\n\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038F100\n"
        "D_0038F100:\n"
        ".asciz \"Invalid CD Key.\\n\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038F118\n"
        "D_0038F118:\n"
        ".asciz \"CD Key already in use.\\n\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038F130\n"
        "D_0038F130:\n"
        ".asciz \"%c0000000000000000\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038F148\n"
        "D_0038F148:\n"
        ".asciz \"loopback\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038F158\n"
        "D_0038F158:\n"
        ".asciz \"Local connection.\\n\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".asciz \"Spectator %s connected\\nAdr: %s\\n\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038F190\n"
        "D_0038F190:\n"
        ".asciz \"Client %s connected\\nAdr: %s\\n\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038F1B0\n"
        "D_0038F1B0:\n"
        ".asciz \"\\\"%s<%i><WON:%u>\\\" connected, address \\\"%s\\\"\\n\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".section .text\n");
/* rodata não migrada: fim */
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027D0F8);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027D340);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027D380);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027D480);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027D778);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027D938);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027DA98);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027DB10);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027DE58);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027DEE8);
/* rodata não migrada: início */
__asm__(".section .rodata\n"
        ".globl D_0038F280\n"
        "D_0038F280:\n"
        ".asciz \"log\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038F288\n"
        "D_0038F288:\n"
        ".asciz \"%s\\n\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038F290\n"
        "D_0038F290:\n"
        ".asciz \"status\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038F298\n"
        "D_0038F298:\n"
        ".asciz \"getchallenge\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038F2A8\n"
        "D_0038F2A8:\n"
        ".asciz \"info\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038F2B0\n"
        "D_0038F2B0:\n"
        ".asciz \"details\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038F2B8\n"
        "D_0038F2B8:\n"
        ".asciz \"players\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038F2C0\n"
        "D_0038F2C0:\n"
        ".asciz \"rules\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038F2C8\n"
        "D_0038F2C8:\n"
        ".asciz \"connect\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038F2D0\n"
        "D_0038F2D0:\n"
        ".asciz \"rcon\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038F2D8\n"
        "D_0038F2D8:\n"
        ".asciz \"bad connectionless packet from %s:\\n%s\\n\"\n"
        ".align 2\n"
        ".align 3\n"
        ".section .text\n");
/* rodata não migrada: fim */
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027E048);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027E140);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027E148);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", SV_FullClientUpdate);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027E238);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027E258);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027E3B0);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027E430);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027E480);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027EA70);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027F670);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027F910);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027F940);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027F9A0);
/* rodata não migrada: início */
__asm__(".section .rodata\n"
        ".globl D_0038F310\n"
        "D_0038F310:\n"
        ".asciz \"Unset entity number\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".globl D_0038F328\n"
        "D_0038F328:\n"
        ".asciz \"expose_problem_entities\"\n"
        ".align 2\n"
        ".align 3\n"
        ".section .text\n");
/* rodata não migrada: fim */
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_00280700);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_00280B30);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_00280C48);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_00280CB8);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_00280FC8);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_002810D8);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_00281328);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", SV_ExtractFromUserinfo);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", SV_ModelIndex);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_002817E0);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_00281888);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_00281B08);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_00281BC8);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_00281D88);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_00281D90);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_00281D98);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", SV_BroadcastCommand);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_00282290);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_002822B0);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_002823C8);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_002825C8);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_00282D90);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_00282DB8);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_00282E40);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_00282F80);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_00283018);
/* rodata não migrada: início */
__asm__(".section .rodata\n"
        ".globl D_0038F6E0\n"
        "D_0038F6E0:\n"
        ".asciz \"==========================\\n\"\n"
        ".align 2\n"
        ".asciz \"\"\n"
        ".align 2\n"
        ".align 3\n"
        ".section .text\n");
/* rodata não migrada: fim */
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_00283130);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", SV_Init);
