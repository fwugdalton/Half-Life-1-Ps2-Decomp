#include "common.h"
#include "engine/cvar.h"
#include "engine/mathlib.h"
#include "engine/cmd.h"
#include "engine/cl_entity.h"
#include "engine/client.h"
#include "engine/model.h"
#include "engine/server.h"

extern int cmd_source;
extern char D_003ACAC8[2][260]; /* servidor da última conexão (retry) */
extern int D_00399194;          /* contador de snapshots */
extern int D_00399168;          /* gravando filme */
extern cl_entity_t *cl_entities[2];
extern int D_0053D1D4;
extern int D_0033451C;
extern char D_0038A108[];       /* "local" (também usada por funções em asm) */
extern int msg_buckets[64];
extern int total_data[64];
extern int D_005621A4;
extern int D_00331780;
extern int D_003995B4;          /* bits de dlights alteradas */
extern cvar_t D_003319B0;       /* cl_allow_download */
extern cvar_t D_003319C8;       /* cl_allow_upload */
extern cvar_t D_003319E0;       /* tamanho máximo de upload */

#define MAX_DLIGHTS 32

typedef struct
{
    vec3_t origin;      /* 0x00 */
    float radius;       /* 0x0C */
    color24 color;      /* 0x10 */
    float die;          /* 0x14 */
    float decay;        /* 0x18 */
    float minlight;     /* 0x1C */
    int key;            /* 0x20 */
    qboolean dark;      /* 0x24 */
} dlight_t;

extern dlight_t cl_dlights[MAX_DLIGHTS];

qboolean NET_GetPacket(int sock);
void CL_Disconnect(int clientId);
void Host_ShutdownServer(qboolean crash);
void Cbuf_AddText(char *text);
void COM_FileBase(char *in, char *out);
void func_002C5218(char *name);
void func_002C5228(char *name);
int atoi(const char *);
char *strcpy(char *, const char *);
void *memset(void *, int, unsigned int);
char *strncpy(char *, const char *, unsigned int);
int strncasecmp(const char *, const char *, int);
void func_002C70A8(qboolean multiplayer);
void Cache_Report(void);
void SCR_EndLoadingPlaque(void);
void MSG_WriteString(sizebuf_t *sb, const char *s);
int DispatchDirectUserMsg(const char *pszName, int iSize, void *pBuf, int clientId);

INCLUDE_ASM("asm/nonmatchings/engine/Cl_main", func_0024EFB8);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389A80);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389A88);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389A98);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389AA0);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389AB8);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389AC0);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389AD0);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389AD8);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389AF0);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389B00);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389B18);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389B28);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389B38);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389B40);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389B50);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389B60);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389B70);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389B80);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389B90);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389BA0);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389BB0);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389BC0);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389BD8);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389BE8);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389BF0);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389C00);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389C10);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389C18);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389C28);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389C30);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389C48);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389C50);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389C60);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389C78);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389C88);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389C98);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389CA8);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389CC0);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389CD0);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389CE0);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389CE8);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389CF0);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389CF8);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389D00);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389D10);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389D20);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389D28);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389D38);

INCLUDE_ASM("asm/nonmatchings/engine/Cl_main", func_0024F100);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389D78);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389D80);

INCLUDE_ASM("asm/nonmatchings/engine/Cl_main", func_0024F220);

INCLUDE_ASM("asm/nonmatchings/engine/Cl_main", func_0024F388);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389DE8);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389DF8);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389E08);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_00389E38);

INCLUDE_ASM("asm/nonmatchings/engine/Cl_main", func_0024F528);

INCLUDE_ASM("asm/nonmatchings/engine/Cl_main", func_0024F6E8);

qboolean func_0024FB28(int sock)
{
    return NET_GetPacket(sock) != 0;
}

INCLUDE_ASM("asm/nonmatchings/engine/Cl_main", func_0024FB48);

INCLUDE_ASM("asm/nonmatchings/engine/Cl_main", func_0024FCE8);

void func_0024FDE0(void)
{
    ((customization_t *)cl[0].players[cl[0].playernum].customdata)->pNext = NULL;
}

INCLUDE_ASM("asm/nonmatchings/engine/Cl_main", func_0024FE00);

INCLUDE_ASM("asm/nonmatchings/engine/Cl_main", func_0024FF90);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_0038A068);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_0038A080);

INCLUDE_ASM("asm/nonmatchings/engine/Cl_main", CL_Disconnect);

void func_00250248(int clientId)
{
    CL_Disconnect(clientId);
    if (sv.active)
        Host_ShutdownServer(false);
}

void func_00250280(void)
{
    int clientId;

    if (Cmd_Argc() == 1)
        clientId = 0;
    else
        clientId = atoi(Cmd_Argv(1));
    CL_Disconnect(clientId);
    if (sv.active)
        Host_ShutdownServer(false);
}

INCLUDE_ASM("asm/nonmatchings/engine/Cl_main", func_002502D8);

INCLUDE_ASM("asm/nonmatchings/engine/Cl_main", func_002503E0);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_0038A108);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_0038A110);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_0038A118);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_0038A128);

INCLUDE_ASM("asm/nonmatchings/engine/Cl_main", func_002504F0);

INCLUDE_ASM("asm/nonmatchings/engine/Cl_main", func_002507A0);

void func_00250A98(void)
{
    char szCommand[260];

    if (!D_003ACAC8[0] || !D_003ACAC8[0][0])
    {
        Con_Printf("Can't retry, no previous connection\n");
        return;
    }

    sprintf(szCommand, "connect %s\n", D_003ACAC8[0]);
    Cbuf_AddText(szCommand);
    Con_Printf("Commencing connection retry to %s\n", D_003ACAC8[0]);
}

void func_00250B08(int clientId)
{
    char name[260];
    char *server;

    if (Cmd_Argc() < 2)
    {
        Con_Printf("usage: connect <server>\n");
        return;
    }

    server = Cmd_Args();
    if (!server)
        return;

    strcpy(D_003ACAC8[clientId], server);
    CL_Disconnect(clientId);

    memset(name, 0, sizeof(name));
    strncpy(name, server, sizeof(name));

    memset(msg_buckets, 0, sizeof(msg_buckets));
    memset(total_data, 0, sizeof(total_data));

    strncpy(cls[clientId].servername, name, sizeof(cls[clientId].servername) - 1);
    cls[clientId].state = ca_connecting;
    cls[clientId].connect_time = -99999;
    cls[clientId].connect_retry = 0;
    D_0033451C = 0;

    if (strncasecmp(cls[clientId].servername, D_0038A108, 5))
        func_002C70A8(true);
}

void func_00250C50(int clientId)
{
    char str[8192];

    Con_DPrintf("CL_SignonReply: %i\n", cls[clientId].signon);

    switch (cls[clientId].signon)
    {
    case 1:
        MSG_WriteByte(&cls[clientId].netchan.message, clc_stringcmd);
        sprintf(str, "spawn %i %s", cl[clientId].servercount, cls[clientId].spawnparms);
        MSG_WriteString(&cls[clientId].netchan.message, str);
        break;

    case 2:
        MSG_WriteByte(&cls[clientId].netchan.message, clc_stringcmd);
        MSG_WriteString(&cls[clientId].netchan.message, "begin");
        Cache_Report();
        break;

    case 3:
        SCR_EndLoadingPlaque();
        break;
    }
}

void func_00250D80(void)
{
    cl_entity_t *ent;
    int i;
    int clientId;

    clientId = atoi(Cmd_Argv(1));
    ent = cl_entities[clientId];
    for (i = 0; i < cl[clientId].num_entities; i++, ent++)
    {
        Con_Printf("%3i:", i);
        if (!ent->model)
        {
            Con_Printf("EMPTY\n");
            continue;
        }
        Con_Printf("%s:%2i  (%5.1f,%5.1f,%5.1f) [%5.1f %5.1f %5.1f]\n", ent->model->name, ent->frame,
                   ent->origin[0], ent->origin[1], ent->origin[2], ent->angles[0], ent->angles[1], ent->angles[2]);
    }
}

void func_00250EE8(void)
{
    char base[64];
    char filename[64];

    if (cl[0].num_entities && cl_entities[0]->model)
        COM_FileBase(cl_entities[0]->model->name, base);
    else
        strcpy(base, "Snapshot");

    sprintf(filename, "%s%04d.bmp", base, D_00399194++);
    func_002C5218(filename);
}

void func_00250F80(void)
{
    if (cmd_source != src_command)
        return;

    if (Cmd_Argc() != 2)
    {
        Con_Printf("startmovie <filename>\n");
        return;
    }

    D_00399168 = true;
    func_002C5228(Cmd_Argv(1));
    Con_Printf("Started recording movie...\n");
}

void func_00250FF8(void)
{
    if (!D_00399168)
    {
        Con_Printf("No movie started.\n");
        return;
    }

    D_00399168 = false;
    Con_Printf("Stopped recording movie...\n");
}

void func_00251040(void)
{
    int i;

    if (Cmd_Argc() != 2)
    {
        Con_Printf("cl_view entity#\nCurrent %i\n", cl[0].viewentity);
        return;
    }

    i = atoi(Cmd_Argv(1));
    if (!i)
        return;
    if (i >= cl[0].num_entities)
        return;

    D_0053D1D4 = 1;
    cl[0].viewentity = i;
    Con_Printf("View entity set to %i\n", i);
}

void func_002510C8(void) {
}

INCLUDE_ASM("asm/nonmatchings/engine/Cl_main", func_002510D0);

INCLUDE_ASM("asm/nonmatchings/engine/Cl_main", func_00251250);

INCLUDE_ASM("asm/nonmatchings/engine/Cl_main", func_00251330);

void func_002514D0(dlight_t *dl)
{
    int i;

    i = dl - cl_dlights;
    if ((unsigned int)i < MAX_DLIGHTS)
        D_003995B4 |= 1 << i;
}

INCLUDE_ASM("asm/nonmatchings/engine/Cl_main", func_00251518);

INCLUDE_ASM("asm/nonmatchings/engine/Cl_main", func_00251710);

INCLUDE_ASM("asm/nonmatchings/engine/Cl_main", func_00251BB0);

int func_00251CA0(void)
{
    return 0;
}

void func_00251CA8(void) {
}

void func_00251CB0(void)
{
    if (D_005621A4)
        D_00331780 = 1;
}

void func_00251CD0(void)
{
    D_003319B0.value = D_003319B0.value ? 0 : 1;
    if (D_003319B0.value)
        Con_Printf("Client downloading enabled.\n");
    else
        Con_Printf("Client downloading disabled.\n");
}

void func_00251D40(void)
{
    D_003319C8.value = D_003319C8.value ? 0 : 1;
    if (D_003319C8.value)
        Con_Printf("Client uploading enabled.\nMax. upload size is %i", (int)D_003319E0.value);
    else
        Con_Printf("Client uploading disabled.\n");
}

void func_00251DC0(char *pMessage, int clientId)
{
    DispatchDirectUserMsg("HudText", strlen(pMessage), pMessage, clientId);
}

void func_00251E18(void) {
}

void func_00251E20(void)
{
    if (Cmd_Argc() != 2)
    {
        Con_Printf("usage: fullserverinfo <complete info string>\n");
        return;
    }

    strcpy(cl[0].serverinfo, Cmd_Argv(1));
}

INCLUDE_ASM("asm/nonmatchings/engine/Cl_main", CL_Init);
