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
extern char D_0038A068[];       /* "engine/Cl_main.c" (também usada pelo CL_Init) */
extern char D_0038A080[];       /* "Unable to allocate %i client frames" */
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
#define MAX_ELIGHTS 64
extern dlight_t cl_elights[MAX_ELIGHTS];
extern int D_003995AC;
extern cvar_t D_003317E8;       /* cl_nolerp */
extern float D_00334764;
#define MAX_EFRAGS 640
extern efrag_t D_0046EC58[2][MAX_EFRAGS];
#define cl_efrags D_0046EC58
extern int cl_updatebackup[2];
extern byte cl_lightstyle[0x1100];
void Host_ClearMemory(qboolean bQuiet);
void func_0024FE00(int clientId);
void func_0025AEA8(int clientId);
void func_00251CA8(void);
extern float realtime;

typedef struct
{
    unsigned int buf[4];
    unsigned int bits[2];
    unsigned char in[64];
} MD5Context_t;

void MD5Init(MD5Context_t *ctx);
void MD5Update(MD5Context_t *ctx, unsigned char const *buf, unsigned int len);
void MD5Final(unsigned char digest[16], MD5Context_t *ctx);
char *MD5_Print(unsigned char hash[16]);
extern void (*D_003997A4)(char *key, int *len, qboolean *bDedicated); /* pega a CD key */
typedef struct master_s
{
    qboolean sent;              /* 0x00 */
    float time;                 /* 0x04 */
    int challenge;              /* 0x08 */
    int sequence;               /* 0x0C */
    float last_heartbeat;       /* 0x10 */
    netadr_t adr;               /* 0x14 */
    struct master_s *next;      /* 0x1C */
} master_t;

extern master_t *valvemaster_adr;
extern netadr_t net_from;
extern sizebuf_t net_message;
extern int (*D_003990DC)(short port); /* ntohs */
qboolean func_002C6D78(netadr_t a, netadr_t b); /* NET_CompareAdr */
char *NET_AdrToString(netadr_t a);
void NET_SendPacket(int sock, int length, void *data, netadr_t to, int unused);
char *MSG_ReadString(void);
int MSG_ReadByte(void);
int MSG_ReadShort(void);
int MSG_ReadLong(void);
int sprintf(char *, const char *, ...);
int strcasecmp(const char *, const char *);
extern char D_00389BE8[];
extern float host_frametime;
qboolean NET_CompareAdr(netadr_t a, netadr_t b);
qboolean Netchan_Process(netchan_t *chan);
void CL_ParseServerMessage(int clientId);
void CL_PrintResource(int index, resource_t *pResource);
void func_0024F6E8(int clientId);
qboolean func_0024FB28(int clientId);
void Netchan_Transmit(netchan_t *chan, int length, byte *data, int unk);
void MSG_WriteShort(sizebuf_t *sb, int c);
void *Q_memcpy(void *dest, const void *src, int count);
void func_002CD340(int a0, int a1); /* S_StopAllSounds */
void CL_DeallocateDynamicData(int clientId);
void func_00254838(int clientId);
void func_0024FF90(qboolean bQuiet, int clientId);
extern int cl_updatemask[2];
extern int D_00399160[2];       /* in_impulse */
extern float D_0033192C;        /* cl_nodelta.value */
void func_0024DDF0(usercmd_t *cmd, int clientId); /* CL_BaseMove */
void func_002C5980(usercmd_t *cmd, int clientId); /* IN_Move */
int func_0024E2E0(int bResetState, int clientId); /* CL_ButtonBits */
void func_00253E80(usercmd_t *cmd, int clientId); /* Cam_Track */
void func_002543B0(usercmd_t *cmd, int clientId); /* Cam_FinishMove */
void func_00242800(sizebuf_t *buf, usercmd_t *to, usercmd_t *from); /* MSG_WriteDeltaUsercmd */
byte COM_BlockSequenceCRCByte(byte *base, int length, int sequence);
qboolean NET_StringToAdr(char *s, netadr_t *a);
void COM_ExplainDisconnection(qboolean bPrint, char *fmt, ...);
void func_002504F0(int clientId);
extern cvar_t D_00331968;       /* cl_resend */
extern int protocol_version;
extern int (*D_003997A0)(char *key, int *keysize); /* dados de autenticação */
extern int (*D_003997D0)(void);  /* protocolo de autenticação */
char *func_00241CA0(char *in, int size);
void CRC32_Init(unsigned int *pulCRC);
qboolean CRC_File(unsigned int *crcvalue, char *pszFileName);
void func_00244EE0(char *s, char *key, char *value, int maxsize); /* Info_SetValueForKey */
void func_00250248(int clientId);
extern char D_00389C18[];       /* "cl_resend" */
extern char D_0038A110[];       /* "%s" */
extern char D_0038A118[];       /* "localhost" */
extern char D_0038A128[];       /* "Bad server address\n" */       /* "" */
extern char D_003AC9C8[256];    /* szHashedKeyBuffer */

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

#ifdef NON_MATCHING
void func_0024EFB8(int clientId)
{
    float fadePercent;
    float elapsed;

    cls[clientId].soundfade.nClientSoundFadePercent = 0;

    if (realtime >= cls[clientId].soundfade.soundFadeStartTime +
                    (cls[clientId].soundfade.soundFadeOutTime + cls[clientId].soundfade.soundFadeInTime +
                     cls[clientId].soundfade.soundFadeHoldTime))
        return;

    elapsed = realtime - cls[clientId].soundfade.soundFadeStartTime;

    if (cls[clientId].soundfade.soundFadeOutTime && elapsed < cls[clientId].soundfade.soundFadeOutTime)
    {
        fadePercent = elapsed / cls[clientId].soundfade.soundFadeOutTime;
        cls[clientId].soundfade.nClientSoundFadePercent = cls[clientId].soundfade.nStartPercent * fadePercent;
        return;
    }

    if (elapsed < cls[clientId].soundfade.soundFadeOutTime + cls[clientId].soundfade.soundFadeHoldTime)
    {
        cls[clientId].soundfade.nClientSoundFadePercent = cls[clientId].soundfade.nStartPercent;
        return;
    }

    fadePercent = 1.0f - (elapsed - (cls[clientId].soundfade.soundFadeOutTime + cls[clientId].soundfade.soundFadeHoldTime)) / cls[clientId].soundfade.soundFadeOutTime;
    cls[clientId].soundfade.nClientSoundFadePercent = cls[clientId].soundfade.nStartPercent * fadePercent;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/Cl_main", func_0024EFB8);
#endif

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

void func_0024F100(void)
{
    char szText[4096];
    master_t *p;
    char *str;
    char *in;
    char *out;

    if (!valvemaster_adr)
        return;

    for (p = valvemaster_adr; p; p = p->next)
    {
        if (func_002C6D78(net_from, p->adr))
            break;
    }
    if (!p)
        return;

    Con_Printf("---- MOTD -----\n");

    while (1)
    {
        str = MSG_ReadString();
        in = str;
        out = szText;
        while (*in)
        {
            if (*in == '\r' && in - 1 > str && in[-1] != '\n')
            {
                *out++ = '\n';
                *out++ = '\r';
                in++;
            }
            else
            {
                *out++ = *in++;
            }
        }
        *out = 0;

        if (!szText[0])
            break;
        Con_Printf(szText);
    }

    Con_Printf("\n---------------\n");
}

void func_0024F220(void)
{
    char szAddress[128] = "";
    unsigned char ip[4];
    int nNumAddresses;
    int i;
    int j;
    unsigned short port;

    i = 1;
    MSG_ReadByte();
    nNumAddresses = net_message.cursize - 6;
    nNumAddresses /= 6;

    while (nNumAddresses-- > 0)
    {
        memset(szAddress, 0, sizeof(szAddress));
        for (j = 0; j < 4; j++)
            ip[j] = MSG_ReadByte();
        sprintf(szAddress, "%i.%i.%i.%i", ip[0], ip[1], ip[2], ip[3]);
        port = D_003990DC(MSG_ReadShort());
        if (i <= 100)
            Con_Printf("%4i:  %s:%i\n", i, szAddress, port);
        i++;
    }

    Con_Printf("%i total servers\n", i);
}

void func_0024F388(void)
{
    char szLastMod[256];
    char szRequest[128];
    char *str;
    char *s1;
    char *s2;
    int n1;
    int n2;
    int nCount;

    nCount = 0;
    MSG_ReadByte();
    szLastMod[0] = 0;

    while (1)
    {
        str = MSG_ReadString();
        if (!str || !str[0])
            break;
        if (!strcasecmp(str, "end-of-list") || !strcasecmp(str, "more-in-list"))
            break;

        strcpy(szLastMod, str);
        nCount++;
        s1 = MSG_ReadString();
        n1 = atoi(s1);
        s2 = MSG_ReadString();
        n2 = atoi(s2);
        if (s1 && s2)
            Con_Printf("%3i %5i %s\n", n1, n2, szLastMod);
    }

    Con_Printf("%i servers\n", nCount);

    if (str && str[0] && !strcasecmp(str, "more-in-list"))
    {
        sprintf(szRequest, "%c\r\n%s\r\n", 'x', szLastMod);
        Con_DPrintf("Requesting next batch ( >%s ) of mods from %s\n", szLastMod, NET_AdrToString(net_from));
        NET_SendPacket(0, strlen(szRequest) + 1, szRequest, net_from, 2);
    }
    else
    {
        Con_Printf("Done.\n");
    }
}

void func_0024F528(void)
{
    char szAddress[128] = "";
    unsigned char ip[4];
    char data[16];
    int nNumAddresses;
    int nCount;
    int j;
    int nextid;

    nCount = 1;
    MSG_ReadByte();
    nextid = MSG_ReadLong();
    nNumAddresses = net_message.cursize - 6;
    nNumAddresses /= 6;

    while (nNumAddresses-- > 0)
    {
        memset(szAddress, 0, sizeof(szAddress));
        for (j = 0; j < 4; j++)
            ip[j] = MSG_ReadByte();
        sprintf(szAddress, "%i.%i.%i.%i", ip[0], ip[1], ip[2], ip[3]);
        nCount++;
        D_003990DC(MSG_ReadShort());
    }

    Con_Printf("%i servers\n", nCount);

    if (nextid)
    {
        func_002C70A8(true);
        data[0] = 'e';
        *(int *)&data[1] = nextid;
        Con_DPrintf("Requesting next batch ( %i ) server list from %s\n", nextid, NET_AdrToString(net_from));
        NET_SendPacket(0, 5, data, net_from, 2);
    }
    else
    {
        Con_Printf("Done.\n");
    }
}

INCLUDE_ASM("asm/nonmatchings/engine/Cl_main", func_0024F6E8);

qboolean func_0024FB28(int sock)
{
    return NET_GetPacket(sock) != 0;
}

#ifdef NON_MATCHING
void func_0024FB48(int clientId)
{
    cl[clientId].oldtime = cl[clientId].time;
    cl[clientId].time += host_frametime;

    while (func_0024FB28(clientId))
    {
        if (*(int *)net_message.data == -1)
        {
            func_0024F6E8(clientId);
            continue;
        }

        if (cls[clientId].state == ca_disconnected || cls[clientId].state == ca_connecting)
            continue;

        if (net_message.cursize < 8)
        {
            Con_Printf("%s: Runt packet\n", NET_AdrToString(net_from));
            continue;
        }

        if (!NET_CompareAdr(net_from, cls[clientId].netchan.remote_address))
        {
            Con_Printf("%s:sequenced packet without connection\n", NET_AdrToString(net_from));
            continue;
        }

        if (!Netchan_Process(&cls[clientId].netchan))
            continue;

        CL_ParseServerMessage(clientId);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/Cl_main", func_0024FB48);
#endif

void func_0024FCE8(void)
{
    int i;
    int j;
    player_info_t *pl;
    customization_t *pCust;

    if (cls[0].state != ca_active)
    {
        Con_Printf("Can't cl_print_custom, not connected\n");
        return;
    }

    for (i = 0; i < MAX_CLIENTS; i++)
    {
        pl = &cl[0].players[i];
        if (!((customization_t *)pl->customdata)->pNext)
            continue;

        Con_DPrintf("CL Customizations:\nPlayer %i:%s\n", i + 1, pl->name);
        j = 1;
        for (pCust = ((customization_t *)pl->customdata)->pNext; pCust; pCust = pCust->pNext)
        {
            if (pCust->bInUse)
                CL_PrintResource(j++, &pCust->resource);
        }
        Con_DPrintf("-----------------\n\n");
    }
}

void func_0024FDE0(void)
{
    ((customization_t *)cl[0].players[cl[0].playernum].customdata)->pNext = NULL;
}

void func_0024FE00(int clientId)
{
    int i;
    frame_t *frame;
    cl_packet_entities_t *pack;

    if (cl[clientId].frames)
    {
        for (i = 0; i < cl_updatebackup[clientId]; i++)
        {
            frame = &cl[clientId].frames[i];
            pack = &frame->packet_entities;
            if (pack->entities)
                Mem_Free(pack->entities);
            pack->entities = NULL;
            frame->packet_entities.num_entities = 0;
        }
    }

    if (cl[clientId].frames)
        Mem_Free(cl[clientId].frames);
    cl[clientId].frames = NULL;

    memset(&cl[clientId], 0, sizeof(client_state_t));

    cl[clientId].frames = Mem_DbgMalloc(cl_updatebackup[clientId] * sizeof(frame_t), D_0038A068, 841);
    if (!cl[clientId].frames)
        Sys_Error(D_0038A080, cl_updatebackup);
    memset(cl[clientId].frames, 0, cl_updatebackup[clientId] * sizeof(frame_t));

    func_00251CA8();
}

void func_0024FF90(qboolean bQuiet, int clientId)
{
    int i;

    if (!sv.active)
        Host_ClearMemory(bQuiet);

    func_0024FE00(clientId);
    SZ_Clear(&cls[clientId].netchan.message);

    memset(cl_efrags, 0, sizeof(cl_efrags));
    memset(cl_dlights, 0, sizeof(cl_dlights));
    memset(cl_elights, 0, sizeof(cl_elights));
    memset(cl_lightstyle, 0, sizeof(cl_lightstyle));

    func_0025AEA8(clientId);

    cl[clientId].free_efrags = cl_efrags[clientId];
    for (i = 0; i < MAX_EFRAGS - 1; i++)
        cl[clientId].free_efrags[i].entnext = &cl[clientId].free_efrags[i + 1];
    cl[clientId].free_efrags[i].entnext = NULL;
}



INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_0038A068);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_0038A080);

void CL_Disconnect(int clientId)
{
    byte final[32];

    cls[clientId].connect_time = -99999.0f;
    cls[clientId].connect_retry = 0;

    func_002CD340(1, clientId);

    if (cls[clientId].state == ca_active || cls[clientId].state == ca_connected ||
        cls[clientId].state == ca_uninitialized || cls[clientId].state == ca_connecting)
    {
        if (cls[clientId].netchan.remote_address.type)
        {
            final[0] = 4; /* clc_stringcmd */
            strcpy((char *)final + 1, "dropclient\n");
            Netchan_Transmit(&cls[clientId].netchan, 13, final, 2);
            Netchan_Transmit(&cls[clientId].netchan, 13, final, 2);
            Netchan_Transmit(&cls[clientId].netchan, 13, final, 2);
        }

        cls[clientId].state = ca_disconnected;
        if (sv.active)
            Host_ShutdownServer(false);
    }

    cls[clientId].signon = 0;
    func_0024FF90(true, clientId);
    CL_DeallocateDynamicData(clientId);
    func_00254838(clientId);
}

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

void func_002502D8(int clientId)
{
    int i;
    cl_packet_entities_t *pack;

    if (cl[clientId].frames)
    {
        for (i = 0; i < cl_updatebackup[clientId]; i++)
        {
            pack = &cl[clientId].frames[i].packet_entities;
            if (pack->entities)
                Mem_Free(pack->entities);
            pack->entities = NULL;
        }
        Mem_Free(cl[clientId].frames);
    }
    cl[clientId].frames = NULL;
}

char *func_002503E0(void)
{
    char szKeyBuffer[256];
    MD5Context_t ctx;
    unsigned char digest[16];
    int nKeyLength;
    qboolean bDedicated;

    nKeyLength = 13;
    bDedicated = false;
    D_003997A4(szKeyBuffer, &nKeyLength, &bDedicated);

    if (bDedicated)
    {
        Con_Printf("Key has no meaning on dedicated server...\n");
        return D_00389BE8;
    }

    if (nKeyLength <= 0 || nKeyLength >= 256)
    {
        Con_Printf("Bogus key length on CD Key...\n");
        return D_00389BE8;
    }

    szKeyBuffer[nKeyLength] = 0;

    memset(&ctx, 0, sizeof(ctx));
    memset(digest, 0, sizeof(digest));
    MD5Init(&ctx);
    MD5Update(&ctx, (unsigned char *)szKeyBuffer, nKeyLength);
    MD5Final(digest, &ctx);

    memset(D_003AC9C8, 0, sizeof(D_003AC9C8));
    strcpy(D_003AC9C8, MD5_Print(digest));
    return D_003AC9C8;
}

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_0038A108);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_0038A110);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_0038A118);

INCLUDE_RODATA("asm/nonmatchings/engine/Cl_main", D_0038A128);

void func_002504F0(int clientId)
{
    netadr_t adr;
    char data[2048];
    char szServerName[260];
    char cdkey[1024];
    char key[1024];
    char szModelCRC[32];
    int keysize;
    unsigned int crc;
    unsigned int authprotocol;

    authprotocol = -1;

    strncpy(szServerName, cls[clientId].servername, sizeof(szServerName));
    if (!strcasecmp(cls[clientId].servername, D_0038A108))
        sprintf(szServerName, D_0038A110, D_0038A118);

    if (!NET_StringToAdr(szServerName, &adr))
    {
        Con_Printf(D_0038A128);
        cls[clientId].connect_time = -99999.0f;
        cls[clientId].connect_retry = 0;
        cls[clientId].state = ca_disconnected;
        return;
    }

    if (cls[clientId].unk_73C0 != 1)
    {
        if (cls[clientId].unk_73C0 != 2)
            cls[clientId].unk_73C0 = 2;
        keysize = strlen(func_002503E0());
        strcpy(cdkey, func_002503E0());
    }
    else
    {
        if (!D_003997A0(key, &keysize))
        {
            func_00250248(clientId);
            Con_Printf("Error retrieving connection auth data\n");
            return;
        }
        authprotocol = D_003997D0();
        strcpy(cdkey, func_00241CA0(key, keysize));
        keysize *= 2;
    }

    CRC32_Init(&crc);
    if (!CRC_File(&crc, "models/player/gordon/gordon.dol"))
    {
        func_00250248(clientId);
        Con_Printf("could not find models/player/gordon/gordon.dol\n");
        return;
    }

    sprintf(szModelCRC, "%d", crc);
    func_00244EE0(cls[clientId].userinfo, "*modelcrc", szModelCRC, MAX_INFO_STRING);

    sprintf(data, "%c%c%c%cconnect %i %i %i %i %i \"%s\" \"%s\"\n", 255, 255, 255, 255, protocol_version,
            cls[clientId].challenge, cls[clientId].unk_73C0, authprotocol, keysize, cdkey, cls[clientId].userinfo);
    NET_SendPacket(clientId, strlen(data), data, adr, 2);
}

void func_002507A0(int clientId)
{
    netadr_t adr;
    char data[2048];
    char szServerName[260];

    if (cls[clientId].state == ca_disconnected && sv.active)
    {
        D_0033451C = 0;
        cls[clientId].state = ca_connecting;
        strncpy(cls[clientId].servername, D_0038A118, sizeof(cls[clientId].servername) - 1);
        cls[clientId].unk_73C0 = 2;
        func_002504F0(clientId);
        return;
    }

    if (cls[clientId].state != ca_connecting)
        return;

    if (D_00331968.value < 1.5f)
        Cvar_SetValue(D_00389C18, 1.5f);
    else if (D_00331968.value > 20.0f)
        Cvar_SetValue(D_00389C18, 20.0f);

    if (realtime - cls[clientId].connect_time < D_00331968.value)
        return;

    strncpy(szServerName, cls[clientId].servername, sizeof(szServerName));

    if (!strcasecmp(cls[clientId].servername, D_0038A108))
        sprintf(szServerName, D_0038A110, D_0038A118);

    if (!NET_StringToAdr(szServerName, &adr))
    {
        Con_Printf(D_0038A128);
        cls[clientId].state = ca_disconnected;
        return;
    }

    if (cls[clientId].connect_retry >= 4)
    {
        COM_ExplainDisconnection(true, "Connection failed after %i retries.\n", 4);
        cls[clientId].connect_time = -99999.0f;
        cls[clientId].connect_retry = 0;
        cls[clientId].state = ca_disconnected;
        return;
    }

    cls[clientId].connect_time = realtime;

    if (strcasecmp(szServerName, D_0038A118))
    {
        if (cls[clientId].connect_retry == 0)
            Con_Printf("Connecting to %s...\n", szServerName);
        else
            Con_Printf("Retrying %s...\n", szServerName);
    }

    cls[clientId].connect_retry++;

    sprintf(data, "%c%c%c%cgetchallenge\n", 255, 255, 255, 255);
    NET_SendPacket(clientId, strlen(data), data, adr, 2);
}

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

void func_002510C8(int i)
{
}

dlight_t *func_002510D0(int key)
{
    int i;
    dlight_t *dl;

    if (key)
    {
        dl = cl_dlights;
        for (i = 0; i < MAX_DLIGHTS; i++, dl++)
        {
            if (dl->key == key)
            {
                memset(dl, 0, sizeof(*dl));
                dl->key = key;
                D_003995B4 |= 1 << i;
                D_003995AC |= 1 << i;
                return dl;
            }
        }
    }

    dl = cl_dlights;
    for (i = 0; i < MAX_DLIGHTS; i++, dl++)
    {
        if (dl->die < cl[0].time)
        {
            memset(dl, 0, sizeof(*dl));
            dl->key = key;
            D_003995B4 |= 1 << i;
            D_003995AC |= 1 << i;
            return dl;
        }
    }

    dl = &cl_dlights[0];
    memset(dl, 0, sizeof(*dl));
    dl->key = key;
    D_003995B4 |= 1;
    D_003995AC |= 1;
    return dl;
}

dlight_t *func_00251250(int key)
{
    int i;
    dlight_t *dl;

    if (key)
    {
        dl = cl_elights;
        for (i = 0; i < MAX_ELIGHTS; i++, dl++)
        {
            if (dl->key == key)
            {
                memset(dl, 0, sizeof(*dl));
                dl->key = key;
                return dl;
            }
        }
    }

    dl = cl_elights;
    for (i = 0; i < MAX_ELIGHTS; i++, dl++)
    {
        if ((dl->radius <= 0 || dl->die != 0) && dl->die < cl[0].time)
        {
            memset(dl, 0, sizeof(*dl));
            dl->key = key;
            return dl;
        }
    }

    dl = &cl_elights[0];
    memset(dl, 0, sizeof(*dl));
    dl->key = key;
    return dl;
}

void func_00251330(void)
{
    int i;
    dlight_t *dl;
    float time;

    time = cl[0].time - cl[0].oldtime;

    D_003995B4 = 0;
    D_003995AC = 0;

    dl = cl_dlights;
    for (i = 0; i < MAX_DLIGHTS; i++, dl++)
    {
        if (!dl->radius)
            continue;

        if (dl->die && dl->die < cl[0].time)
        {
            D_003995B4 |= 1 << i;
            dl->radius = 0;
        }
        else if (dl->decay)
        {
            D_003995B4 |= 1 << i;
            dl->radius -= time * dl->decay;
            if (dl->radius < 0)
                dl->radius = 0;
        }

        if (dl->radius)
            D_003995AC |= 1 << i;
    }

    dl = cl_elights;
    for (i = 0; i < MAX_ELIGHTS; i++, dl++)
    {
        if (!dl->radius)
            continue;

        if (dl->die && dl->die < cl[0].time)
        {
            dl->radius = 0;
            continue;
        }

        dl->radius -= time * dl->decay;
        if (dl->radius < 0)
            dl->radius = 0;
    }
}

void func_002514D0(dlight_t *dl)
{
    int i;

    i = dl - cl_dlights;
    if ((unsigned int)i < MAX_DLIGHTS)
        D_003995B4 |= 1 << i;
}

float func_00251518(int clientId)
{
    float f, frac;

    f = cl[clientId].mtime[0] - cl[clientId].mtime[1];

    if (!f || D_003317E8.value || (sv.active && !D_00334764))
    {
        cl[clientId].time = cl[clientId].mtime[0];
        return 1;
    }

    if (f > 0.1f)
    {
        cl[clientId].mtime[1] = cl[clientId].mtime[0] - 0.1f;
        f = 0.1f;
    }

    frac = (cl[clientId].time - cl[clientId].mtime[1]) / f;
    if (frac < 0)
    {
        if (frac < -0.01f)
        {
            func_002510C8(1);
            cl[clientId].time = cl[clientId].mtime[1];
        }
        frac = 0;
    }
    else if (frac > 1)
    {
        if (frac > 1.01f)
        {
            func_002510C8(2);
            cl[clientId].time = cl[clientId].mtime[0];
        }
        frac = 1;
    }
    else
        func_002510C8(0);

    return frac;
}

#ifdef NON_MATCHING
void func_00251710(int clientId)
{
    sizebuf_t buf;
    byte data[128];
    int i;
    int j;
    usercmd_t *cmd;
    usercmd_t *oldcmd;
    int checksumIndex;
    usercmd_t nullcmd;
    int ms;

    if (cls[clientId].state < ca_connected)
        return;
    if (!cl[clientId].frames)
        return;

    i = cls[clientId].netchan.outgoing_sequence & cl_updatemask[clientId];
    cmd = &cl[clientId].frames[i].cmd;
    cl[clientId].frames[i].senttime = realtime;
    cl[clientId].frames[i].receivedtime = -1;
    memset(cmd, 0, sizeof(*cmd));

    if (cls[clientId].signon == 3)
    {
        func_0024DDF0(cmd, clientId);
        func_002C5980(cmd, clientId);
    }

    buf.maxsize = 128;
    buf.cursize = 0;
    buf.data = data;

    MSG_WriteByte(&buf, 3); /* clc_move */
    checksumIndex = buf.cursize;
    MSG_WriteByte(&buf, 0);

    for (i = 0; i < 3; i++)
        cmd->viewangles[i] = cl[clientId].viewangles[i];

    ms = host_frametime * 1000;
    if (ms > 250)
        ms = 100;
    cmd->msec = ms;

    cmd->buttons = func_0024E2E0(1, clientId);
    cmd->impulse = D_00399160[clientId];

    if (cl[clientId].spectator)
    {
        func_00253E80(cmd, clientId);
        func_002543B0(cmd, clientId);
    }

    memset(&nullcmd, 0, sizeof(nullcmd));

    i = (cls[clientId].netchan.outgoing_sequence - 2) & cl_updatemask[clientId];
    func_00242800(&buf, &cl[clientId].frames[i].cmd, &nullcmd);

    j = i;
    i = (cls[clientId].netchan.outgoing_sequence - 1) & cl_updatemask[clientId];
    func_00242800(&buf, &cl[clientId].frames[i].cmd, &cl[clientId].frames[j].cmd);

    func_00242800(&buf, cmd, &cl[clientId].frames[i].cmd);

    buf.data[checksumIndex] = COM_BlockSequenceCRCByte(buf.data + checksumIndex + 1, buf.cursize - checksumIndex - 1,
                                                       cls[clientId].netchan.outgoing_sequence);

    cl[clientId].cmd = *cmd;

    D_00399160[clientId] = 0;

    if (cls[clientId].netchan.outgoing_sequence - cl[clientId].validsequence >= cl_updatebackup[clientId] - 1)
        cl[clientId].validsequence = 0;

    if (cl[clientId].validsequence && !D_0033192C && cls[clientId].state == ca_active)
    {
        cl[clientId].frames[cls[clientId].netchan.outgoing_sequence & cl_updatemask[clientId]].delta_sequence = cl[clientId].validsequence;
        MSG_WriteByte(&buf, 5); /* clc_delta */
        MSG_WriteByte(&buf, cl[clientId].validsequence & 255);
    }
    else
    {
        cl[clientId].frames[cls[clientId].netchan.outgoing_sequence & cl_updatemask[clientId]].delta_sequence = -1;
    }

    Netchan_Transmit(&cls[clientId].netchan, buf.cursize, buf.data, 2);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/Cl_main", func_00251710);
#endif

void func_00251BB0(void)
{
    int nStartPos;
    short nSize;

    if (cls[0].state == ca_dedicated || cls[0].state == ca_disconnected)
    {
        Con_Printf("custom resource list not valid -- not connected\n");
        return;
    }

    if (MSG_ReadLong() != cl[0].servercount)
    {
        Con_Printf("CL_SendResourceListBlock_f from different level\n");
        return;
    }

    if (MSG_ReadLong() != 0)
    {
        Con_Printf("custom resource list request out of range\n");
        return;
    }

    MSG_WriteByte(&cls[0].netchan.message, 8);
    MSG_WriteShort(&cls[0].netchan.message, 0);
    MSG_WriteShort(&cls[0].netchan.message, 0);
    nStartPos = cls[0].netchan.message.cursize;
    MSG_WriteShort(&cls[0].netchan.message, 0);
    nSize = 0;
    Q_memcpy(cls[0].netchan.message.data + nStartPos, &nSize, sizeof(nSize));
}

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
