#include "common.h"
#include "stdarg.h"
#include "engine/cmd.h"
#include "engine/cvar.h"
#include "engine/decals.h"
#include "engine/server.h"

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

typedef struct kbutton_s
{
    int down[2];
    int state;
} kbutton_t;

typedef struct quakeparms_s
{
    char *basedir;
    char *cachedir;
    int argc;
    char **argv;
    void *membase;
    int memsize;
} quakeparms_t;

typedef struct tm_s
{
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
} tm_t;

#define svc_disconnect 2
#define svc_print 8
#define svc_stufftext 9

#define NS_SERVER 2
#define PORT_MASTER 27010

void Sys_Printf(char *fmt, ...);
int vsprintf(char *, const char *, va_list);
int atoi(const char *);
int Q_atoi(char *str);
int COM_CheckParm(char *parm);
char *COM_Parse(char *data);
void COM_FileBase(char *in, char *out);
void *Hunk_AllocName(int size, char *name);
void SCR_EndLoadingPlaque(void);
void SCR_UpdateScreen(void);
void Cvar_CmdInit(void);
void Cvar_WriteVariables(FileHandle_t f, int flags);
void func_00261BD8(FileHandle_t f, int flags);
void Con_Init(void);
void Con_Shutdown(void);
void Mod_Init(void);
void CM_FreePAS(void);
void MSG_WriteString(sizebuf_t *sb, char *s);
void FS_FPrintf(FileHandle_t f, char *fmt, ...);
void func_00222500(FileHandle_t f, char *fmt, ...);
FileHandle_t FS_Open(char *name, char *mode);
void FS_Close(FileHandle_t f);
int setjmp(void *env);
void longjmp(void *env, int val);
float Sys_FloatTime(void);
void Netchan_Setup(int socket, netchan_t *chan, netadr_t adr);
void Netchan_Transmit(netchan_t *chan, int length, byte *data, int unk);
void Netchan_OutOfBandPrint(int sock, netadr_t adr, int unk, char *format, ...);
void NET_SendPacket(int sock, int length, void *data, netadr_t to, int unk);
qboolean NET_CompareAdr(netadr_t a, netadr_t b);
char *NET_AdrToString(netadr_t a);
qboolean NET_StringToAdr(char *s, netadr_t *a);
byte *COM_LoadFile(char *path, int usehunk, int *pLength);
void COM_FixSlashes(char *pname);
tm_t *Sys_LocalTime(void *ltime);
void SV_CountPlayers(int *clients, int *spectators);
void SV_FullClientUpdate(client_t *cl, sizebuf_t *sb);
void CL_Disconnect(int player);
void Key_WriteBindings(FileHandle_t f);
void Key_Init(void);
void func_002CFD08(void *p);
void func_0024FE00(int player);
void SV_ClearEntities(void);
void func_002B05F8(void);
void func_0029F6A8(void);
void func_002885A0(client_t *cl);
void SV_ReadPackets(void);
void func_00287A40(void);
void SV_QueryMovevarsChanged(void);
void SV_SendClientMessages(void);
void func_00288A58(void);
void MSG_BeginReading(void);
int MSG_ReadLong(void);
char *func_002436B0(void);
void func_00297D10(netadr_t *adr);
void func_00247118(int player);
void func_00251710(int player);
void func_0024FB48(int player);
void func_002524F8(int player);
void func_0024C120(int player);
void func_002507A0(int player);
int func_00295718(int player);
void SV_CheckTimeouts(void);
void func_00245848(void);
void func_002C5228(int unk);
void func_0024EFB8(int unk);
void S_Update(float *origin, float *forward, float *right, float *up, int unk);
void func_002CD788(void);
void func_00251330(void);
void func_002C63C0(void *p);
int build_number(void);
void func_00294518(void);
void func_00277B78(int protocol);
void Memory_Init(void *buf, int size);
void func_0028E548(void);
void func_00247660(void);
void COM_Init(char *basedir);
void Host_InitCommands(void);
void NET_Init(void);
void Netchan_Init(void);
void SV_Init(void);
void func_00246E78(void);
qboolean VID_Init(short *palette);
void Draw_Init(void);
void SCR_Init(void);
void R_Init(void);
void S_Init(int unk);
void func_00235798(void);
void CL_Init(int player);
void func_002C5430(void);
void func_00294508(void);
void func_00246FC8(void);
void SV_DeallocateDynamicData(void);
void func_002CBE40(void);
void func_002C54C8(void);
void NET_Shutdown(void);
void func_002502D8(int unk);
void func_002CF640(void);
void func_00294510(void);
void SV_ClearClientStates(void);
void Host_CheckDynamicStructures(void);

extern client_t *host_client;
extern float host_frametime;
extern float oldrealtime;
extern float realtime;
extern float host_time;
extern int host_framecount;
extern int host_hunklevel;
extern short *host_basepal;
extern quakeparms_t host_parms;
extern int host_abortserver[];
extern int giActive;
extern int giStateInfo;
extern int cl_numlocal;
extern int key_dest;
extern int protocol_version;
extern char gpszVersionString[];
extern char com_gamedir[];
extern int com_argc;
extern char **com_argv;
extern char com_token[];
extern cvar_t sv_lan;
extern cvar_t scr_viewsize;
extern cvar_t developer;
extern float D_004A920C;
extern int D_00334540;
extern int D_0033453C;
extern int D_00334544;
extern void (*D_0033452C)(void);
extern int D_00399168;
extern float D_003995E0;
extern int D_00399798;
extern int D_00399704;
extern void (*D_003997D8)(void);
extern void (*D_003997A8)(void);
extern void *D_00333114;
extern int D_00331C18;
extern int scr_disabled_for_loading;
extern netadr_t net_from;
extern byte D_0053DDF8[];
extern void (*gEntityInterface_pfnClientDisconnect)(edict_t *ed);
extern void (*gEntityInterface_pfnSpectatorDisconnect)(edict_t *ed);
extern kbutton_t in_mlook[2];
extern kbutton_t in_jlook[2];
extern kbutton_t in_leftStickXLook[2];
extern kbutton_t in_leftStickYLook[2];
extern kbutton_t in_rightStickXLook[2];
extern kbutton_t in_rightStickYLook[2];
extern kbutton_t in_leftStickForwardMove[2];
extern kbutton_t in_leftStickSideMove[2];
extern kbutton_t in_rightStickForwardMove[2];
extern kbutton_t in_rightStickSideMove[2];
extern float r_origin[3];
extern float vpn[3];
extern float vright[3];
extern float vup[3];
extern float vec3_origin[3];
extern char info_values[4][512];
extern int info_valueindex;
extern float D_00403874;
extern char master_string[2048];
extern char log_string[1024];

cvar_t fps_single = { "fps_single", "72.0" };
float rolling_fps = 0;
qboolean host_initialized = false;
master_t *valvemaster_adr = NULL;
qboolean gfNoMasterServer = false;
int D_00331C84 = 0;
cvar_t host_framerate = { "host_framerate", "0" };
cvar_t host_speeds = { "host_speeds", "0" };
cvar_t mp_logfile = { "mp_logfile", "1", FCVAR_SERVER };
cvar_t mp_logecho = { "mp_logecho", "1", FCVAR_SERVER };
cvar_t developer = { "developer", "0" };
cvar_t skill = { "skill", "0" };
cvar_t deathmatch = { "deathmatch", "0", FCVAR_SERVER };
cvar_t coop = { "coop", "0", FCVAR_SERVER };
cvar_t pausable = { "pausable", "1", FCVAR_SERVER };

void Host_ShutdownServer(qboolean crash);
void Master_Init(void);
void Master_Shutdown(qboolean bVerbose);
void Log_Printf(char *fmt, ...);
void Log_Close(void);

void Host_EndGame(char *message, ...)
{
    va_list argptr;
    char string[1024];

    va_start(argptr, message);
    vsprintf(string, message, argptr);
    va_end(argptr);

    Con_DPrintf("Host_EndGame: %s\n", string);
}

void Host_Error(char *error, ...)
{
    va_list argptr;
    char string[1024];
    static qboolean inerror = false;

    if (inerror)
        Sys_Error("Host_Error: recursively entered");

    inerror = true;
    SCR_EndLoadingPlaque();

    va_start(argptr, error);
    vsprintf(string, error, argptr);
    va_end(argptr);

    Con_Printf("Host_Error: %s\n", string);

    if (sv.active)
        Host_ShutdownServer(false);

    if (cls[0].state == ca_dedicated)
        Sys_Error("Host_Error: %s\n", string);

    CL_Disconnect(0);
    inerror = false;
    longjmp(host_abortserver, 1);
}

#ifdef NON_MATCHING
void SV_SetMaxclients(void)
{
    int i;
    int j;
    client_t *cl;

    svs.maxclients = 1;
    cl_numlocal = 1;

    i = COM_CheckParm("-maxplayers");
    if (i)
        svs.maxclients = Q_atoi(com_argv[i + 1]);
    else if (cls_demoplayback)
        svs.maxclients = 6;

    if (cls_demoplayback)
    {
        for (j = 1; j >= 0; j--)
            cls[j].state = ca_dedicated;
    }
    else
    {
        for (j = 1; j >= 0; j--)
            cls[j].state = ca_disconnected;
    }

    if (svs.maxclients < 1)
        svs.maxclients = 6;
    else if (svs.maxclients > 3)
        svs.maxclients = 3;

    if (!cls_demoplayback && host_parms.memsize <= 0x1000000)
        svs.maxclientslimit = 4;
    else
        svs.maxclientslimit = 3;

    svs.clients = Hunk_AllocName(svs.maxclientslimit * sizeof(client_t), "clients");

    for (i = 0, cl = svs.clients; i < svs.maxclientslimit; cl++, i++)
    {
        memset(cl, 0, sizeof(client_t));
        cl->resourcesonhand.pNext = cl->resourcesonhand.pPrev = &cl->resourcesonhand;
        cl->resourcesneeded.pNext = cl->resourcesneeded.pPrev = &cl->resourcesneeded;
    }

    if (svs.maxclients > 1)
        Cvar_SetValue("deathmatch", 1.0f);
    else
        Cvar_SetValue("deathmatch", 0.0f);

    if (svs.maxclients > svs.maxclientslimit)
        svs.maxclients = svs.maxclientslimit;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/Host", SV_SetMaxclients);
#endif

void Host_InitLocal(void)
{
    Host_InitCommands();
    Cvar_RegisterVariable(&fps_single);
    Cvar_RegisterVariable(&host_framerate);
    Cvar_RegisterVariable(&host_speeds);
    Cvar_RegisterVariable(&mp_logfile);
    Cvar_RegisterVariable(&mp_logecho);
    Cvar_RegisterVariable(&developer);
    Cvar_RegisterVariable(&deathmatch);
    Cvar_RegisterVariable(&coop);
    Cvar_RegisterVariable(&pausable);
    Cvar_RegisterVariable(&skill);
    SV_SetMaxclients();
    host_time = 1.0f;
}

void Info_WriteVars(FileHandle_t fp)
{
    char pkey[512];
    char *o;
    char *s;
    cvar_t *pcvar;

    info_valueindex = (info_valueindex + 1) % 4;
    s = cls[0].userinfo;
    if (*s == '\\')
        s++;

    while (1)
    {
        o = pkey;
        while (*s != '\\')
        {
            if (!*s)
                return;
            *o++ = *s++;
        }
        *o = 0;
        s++;

        o = info_values[info_valueindex];
        while (*s != '\\' && *s)
        {
            if (!*s)
                return;
            *o++ = *s++;
        }
        *o = 0;

        pcvar = Cvar_FindVar(pkey);
        if (!pcvar && pkey[0] != '*')
            FS_FPrintf(fp, "setinfo 0 \"%s\" \"%s\"\r\n", pkey, info_values[info_valueindex]);

        if (!*s)
            return;
        s++;
    }
}

void Host_WriteConfiguration(void)
{
}

void func_00263308(FileHandle_t f)
{
    Cvar_WriteVariables(f, 0x400);
}

void func_00263328(FileHandle_t f)
{
    func_00261BD8(f, 0x400);
}

#ifdef NON_MATCHING
void Host_WriteControls(FileHandle_t f)
{
    int i;

    FS_FPrintf(f, "unbindall\n");
    Key_WriteBindings(f);
    Cvar_WriteVariables(f, 0x200);

    for (i = 0; i < 2; i++)
    {
        if (in_mlook[i].state & 1)
            FS_FPrintf(f, "+mlook %d\r\n", i);
        else
            FS_FPrintf(f, "-mlook %d\r\n", i);

        if (in_jlook[i].state & 1)
            FS_FPrintf(f, "+jlook %d\r\n", i);
        else
            FS_FPrintf(f, "-jlook %d\r\n", i);

        if (in_leftStickXLook[i].state & 1)
            FS_FPrintf(f, "+leftStickXLook %d\r\n", i);
        else
            FS_FPrintf(f, "-leftStickXLook %d\r\n", i);

        if (in_leftStickYLook[i].state & 1)
            FS_FPrintf(f, "+leftStickYLook %d\r\n", i);
        else
            FS_FPrintf(f, "-leftStickYLook %d\r\n", i);

        if (in_rightStickXLook[i].state & 1)
            FS_FPrintf(f, "+rightStickXLook %d\r\n", i);
        else
            FS_FPrintf(f, "-rightStickXLook %d\r\n", i);

        if (in_rightStickYLook[i].state & 1)
            FS_FPrintf(f, "+rightStickYLook %d\r\n", i);
        else
            FS_FPrintf(f, "-rightStickYLook %d\r\n", i);

        if (in_leftStickForwardMove[i].state & 1)
            FS_FPrintf(f, "+leftStickForwardMove %d\r\n", i);
        else
            FS_FPrintf(f, "-leftStickForwardMove %d\r\n", i);

        if (in_leftStickSideMove[i].state & 1)
            FS_FPrintf(f, "+leftStickSideMove %d\r\n", i);
        else
            FS_FPrintf(f, "-leftStickSideMove %d\r\n", i);

        if (in_rightStickForwardMove[i].state & 1)
            FS_FPrintf(f, "+rightStickForwardMove %d\r\n", i);
        else
            FS_FPrintf(f, "-rightStickForwardMove %d\r\n", i);

        if (in_rightStickSideMove[i].state & 1)
            FS_FPrintf(f, "+rightStickSideMove %d\r\n", i);
        else
            FS_FPrintf(f, "-rightStickSideMove %d\r\n", i);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/Host", Host_WriteControls);
#endif

void SV_ClientPrintf(char *fmt, ...)
{
    va_list argptr;
    char string[1024];

    if (host_client->fakeclient)
        return;

    va_start(argptr, fmt);
    vsprintf(string, fmt, argptr);
    va_end(argptr);

    MSG_WriteByte(&host_client->netchan.message, svc_print);
    MSG_WriteString(&host_client->netchan.message, string);
}

void SV_BroadcastPrintf(char *fmt, ...)
{
    va_list argptr;
    char string[1024];
    int i;

    va_start(argptr, fmt);
    vsprintf(string, fmt, argptr);
    va_end(argptr);

    for (i = 0; i < svs.maxclients; i++)
    {
        if ((svs.clients[i].active || svs.clients[i].spawned) && !svs.clients[i].fakeclient)
        {
            MSG_WriteByte(&svs.clients[i].netchan.message, svc_print);
            MSG_WriteString(&svs.clients[i].netchan.message, string);
        }
    }
}

void Host_ClientCommands(char *fmt, ...)
{
    va_list argptr;
    char string[1024];

    va_start(argptr, fmt);
    vsprintf(string, fmt, argptr);
    va_end(argptr);

    MSG_WriteByte(&host_client->netchan.message, svc_stufftext);
    MSG_WriteString(&host_client->netchan.message, string);
}

void SV_DropClient(client_t *cl, qboolean crash)
{
    int i;
    byte final[32];

    i = 0;
    if (!crash)
    {
        if (!cl->fakeclient)
        {
            MSG_WriteByte(&cl->netchan.message, svc_disconnect);
            final[0] = svc_disconnect;
            i = 1;
        }

        if (cl->edict && cl->spawned)
        {
            if (cl->spectator)
                gEntityInterface_pfnSpectatorDisconnect(cl->edict);
            else
                gEntityInterface_pfnClientDisconnect(cl->edict);
        }

        if (cl->unk_853C)
        {
            func_002CFD08(cl->unk_853C);
            cl->unk_853C = NULL;
        }

        if (cl->unk_85F8)
            cl->unk_85F8 = 0;

        if (cl->spectator)
            Sys_Printf("Dropped %s (spectator) from server\n", cl->name);
        else
            Sys_Printf("Dropped %s from server\n", cl->name);

        if (!cl->fakeclient)
            Netchan_Transmit(&cl->netchan, i, final, 0);
    }

    cl->active = false;
    cl->connected = false;
    cl->fakeclient = false;
    cl->spawned = false;
    cl->name[0] = 0;
    cl->connection_started = realtime;
    cl->maxspeed = 0;
    cl->edict = NULL;
    memset(cl->userinfo, 0, sizeof(cl->userinfo));
    SV_FullClientUpdate(cl, &sv.reliable_datagram);
}

void Host_ClearClients(qboolean bFramesOnly)
{
    int i;
    netadr_t save;

    host_client = svs.clients;
    for (i = 0; i < svs.maxclients; i++)
    {
        if (host_client->netchan.remote_address.type)
        {
            save = host_client->netchan.remote_address;
            memset(&host_client->netchan, 0, sizeof(netchan_t));
            Netchan_Setup(NS_SERVER, &host_client->netchan, save);
        }
        host_client++;
    }

    if (!bFramesOnly)
        memset(svs.clients, 0, svs.maxclientslimit * sizeof(client_t));
}

void Host_ShutdownServer(qboolean crash)
{
    int i;

    if (!sv.active)
        return;

    sv.active = false;

    if (cls[0].state == ca_active || cls[0].state == ca_connected || cls[0].state == ca_uninitialized ||
        cls[0].state == ca_connecting)
        CL_Disconnect(0);

    host_client = svs.clients;
    for (i = 0; i < svs.maxclients; i++, host_client++)
    {
        if ((host_client->active || host_client->connected) && !host_client->fakeclient)
            SV_DropClient(host_client, crash);
    }

    SV_ClearEntities();
    memset(&sv, 0, sizeof(sv));
    func_0024FE00(0);
    SV_ClearClientStates();
    Host_ClearClients(false);
    memset(svs.clients, 0, svs.maxclientslimit * sizeof(client_t));
    Master_Shutdown(false);
    Log_Printf("Server shutdown.\n");
    Log_Close();
}

void SV_ClearClientStates(void)
{
    int i;
    client_t *pcl;

    for (i = 0, pcl = svs.clients; i < svs.maxclients; i++, pcl++)
        func_002885A0(pcl);
}

void Host_CheckDynamicStructures(void)
{
}

void Host_ClearMemory(qboolean bQuiet)
{
    int i;
    int j;

    CM_FreePAS();
    SV_ClearEntities();

    if (!bQuiet)
        Con_DPrintf("Clearing memory\n");

    func_002B05F8();
    func_0029F6A8();

    if (host_hunklevel)
    {
        Host_CheckDynamicStructures();
        Hunk_FreeToLowMark(host_hunklevel);
    }

    for (j = 0; j < cl_numlocal; j++)
        cls[j].signon = 0;

    memset(&sv, 0, sizeof(sv));

    for (i = 0; i < cl_numlocal; i++)
        func_0024FE00(i);

    SV_ClearClientStates();
}

qboolean Host_FilterTime(float time)
{
    float fps;

    realtime += time;

    fps = fps_single.value;
    if (fps < 0.1f)
        fps = 0.1f;
    else if (fps > 72.0f)
        fps = 72.0f;

    if (realtime - oldrealtime < 1.0f / fps)
        return false;

    host_frametime = realtime - oldrealtime;
    oldrealtime = realtime;

    if (host_framerate.value > 0)
    {
        host_frametime = host_framerate.value;
    }
    else
    {
        if (host_frametime > 0.1f)
            host_frametime = 0.1f;
        if (host_frametime < 0.001f)
            host_frametime = 0.001f;
    }

    return true;
}

qboolean func_00263F68(float time)
{
    static float accum = 0;

    accum += host_frametime;
    if (time - D_00403874 < 0.1f)
        return false;

    D_00403874 = time;
    host_frametime = accum;
    accum = 0;
    return true;
}

qboolean func_00263FC8(float time)
{
    static float last = 0;

    if (time - last < 1.0f / 30.0f)
        return false;

    last = time;
    return true;
}

void Host_ServerFrame(void)
{
    float t[5];

    if (host_speeds.value != 0)
        t[0] = Sys_FloatTime();

    D_004A920C = host_frametime;
    SV_ReadPackets();

    if (host_speeds.value != 0)
        t[1] = Sys_FloatTime();

    if (!sv.paused && (svs.maxclients > 1 || (key_dest == 0 && (cls[0].state == ca_active || cls[0].state == ca_dedicated))))
        func_00287A40();

    if (host_speeds.value != 0)
        t[2] = Sys_FloatTime();

    SV_QueryMovevarsChanged();
    SV_SendClientMessages();

    if (host_speeds.value != 0)
        t[3] = Sys_FloatTime();

    func_00288A58();

    if (host_speeds.value != 0)
        t[4] = Sys_FloatTime();
}

void Master_Heartbeat(master_t *p)
{
    char szGD[260];
    int active;

    if (D_00334540)
        return;

    if (!p)
        return;

    if (p->sent)
        return;

    if (realtime - p->time >= 15.0f)
        return;

    Master_Init();
    SV_CountPlayers(&active, NULL);

    p->sequence++;
    memset(master_string, 0, sizeof(master_string));
    COM_FileBase(com_gamedir, szGD);
    sprintf(master_string, "%c\n%i\n%i\n%i\n%i\n%s\n", 'a', p->challenge, p->sequence, active, protocol_version, szGD);
    NET_SendPacket(NS_SERVER, strlen(master_string), master_string, p->adr, 0);
}

#ifdef NON_MATCHING
void Master_RequestHeartbeats(void)
{
    master_t *p;
    char c;

    if (gfNoMasterServer)
        return;

    if (D_00334540)
        return;

    if (sv_lan.value != 0)
        return;

    if (svs.maxclients < 2)
        return;

    if (!sv.active)
        return;

    Master_Init();

    for (p = valvemaster_adr; p; p = p->next)
    {
        if (realtime - p->last_heartbeat < 300.0f)
            continue;

        if (p->sent && realtime - p->time < 15.0f)
            continue;

        p->sent = true;
        p->last_heartbeat = realtime;
        p->time = realtime;
        c = 'q';
        NET_SendPacket(NS_SERVER, 1, &c, p->adr, 0);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/Host", Master_RequestHeartbeats);
#endif

void Master_Shutdown(qboolean bVerbose)
{
    char string[2048];
    master_t *p, *pnext;

    if (gfNoMasterServer)
        return;

    if (D_00334540)
        return;

    if (sv_lan.value != 0)
        return;

    if (svs.maxclients < 2)
        return;

    Master_Init();
    sprintf(string, "%c\n", 'b');

    for (p = valvemaster_adr; p; p = p->next)
    {
        if (bVerbose)
            Con_DPrintf("Sending shutdown to %s\n", NET_AdrToString(p->adr));

        NET_SendPacket(NS_SERVER, strlen(string), string, p->adr, 0);
    }

    if (bVerbose)
    {
        for (p = valvemaster_adr; p; p = pnext)
        {
            pnext = p->next;
            Mem_Free(p);
        }
        valvemaster_adr = NULL;
    }
}

void Master_AddServer(netadr_t *adr)
{
    master_t *n;

    Master_Init();

    for (n = valvemaster_adr; n != NULL; n = n->next)
    {
        if (NET_CompareAdr(n->adr, *adr))
            break;
    }

    if (n)
        return;

    n = (master_t *)Mem_DbgMalloc(sizeof(master_t), "engine/Host.c", 1262);
    if (!n)
        Sys_Error("Error allocating %i bytes for master address.", sizeof(master_t));

    memset(n, 0, sizeof(master_t));
    n->adr = *adr;
    n->last_heartbeat = -99999.0f;
    n->next = valvemaster_adr;
    valvemaster_adr = n;
}

#ifdef NON_MATCHING
void Master_SetMaster_f(void)
{
    char szMasterAddress[128];
    char szPort[32];
    netadr_t adr;
    int argc;
    int i;
    int port;
    char *pszCmd;
    char *pszIP;
    char *pszPort;
    master_t *p, *pprev;

    port = PORT_MASTER;
    sprintf(szPort, "%i", PORT_MASTER);

    argc = Cmd_Argc();
    if (argc < 2 || argc > 4)
    {
        Con_Printf("Usage:\nSetmaster <add | remove | enable | disable> <IP:port>\n");
        p = valvemaster_adr;
        if (!p)
        {
            Con_Printf("Current:  None\n");
        }
        else
        {
            Con_Printf("Current:\n");
            i = 1;
            while (p)
            {
                Con_Printf("  %i:  %s\n", i++, NET_AdrToString(p->adr));
                p = p->next;
            }
        }
        return;
    }

    pszCmd = Cmd_Argv(1);
    if (!pszCmd || !pszCmd[0])
        return;

    if (!strcasecmp(pszCmd, "disable"))
    {
        gfNoMasterServer = true;
        return;
    }

    if (!strcasecmp(pszCmd, "enable"))
    {
        gfNoMasterServer = false;
        return;
    }

    if (strcasecmp(pszCmd, "add") && strcasecmp(pszCmd, "remove"))
    {
        Con_Printf("Setmaster:  Unknown command %s\n", pszCmd);
        return;
    }

    pszIP = Cmd_Argv(2);
    if (argc == 4)
    {
        pszPort = Cmd_Argv(3);
        if (pszPort && pszPort[0])
        {
            port = atoi(pszPort);
            if (!port)
                port = PORT_MASTER;
        }
    }

    sprintf(szMasterAddress, "%s:%i", pszIP, port);

    if (!NET_StringToAdr(szMasterAddress, &adr))
    {
        Con_Printf(" Invalid address \"%s\", setmaster command ignored\n", szMasterAddress);
        return;
    }

    if (!strcasecmp(pszCmd, "add"))
    {
        Master_Init();
        Master_AddServer(&adr);
        gfNoMasterServer = false;
        Con_Printf("Adding master at %s\n", szMasterAddress);
    }
    else
    {
        Master_Init();

        if (!valvemaster_adr)
        {
            Con_Printf("Can't remove master, list is empty\n");
            return;
        }

        for (p = valvemaster_adr; p; p = p->next)
        {
            if (NET_CompareAdr(p->adr, adr))
                break;
        }

        if (!p)
        {
            Con_Printf("Can't remove master %s, not in list\n", szMasterAddress);
            return;
        }

        if (p == valvemaster_adr)
        {
            valvemaster_adr = p->next;
            Mem_Free(p);
            return;
        }

        for (pprev = valvemaster_adr; pprev && pprev->next != p; pprev = pprev->next)
            ;

        if (pprev)
        {
            pprev->next = p->next;
            Mem_Free(p);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/Host", Master_SetMaster_f);
/* strings que o splat não levou para o .s (a primeira é compartilhada com Master_Init) */
__asm__(".section .rodata\n"
        ".align 3\n.globl D_0038C550\nD_0038C550: .asciz \"%s:%i\"\n"
        ".align 3\n.globl D_0038C558\nD_0038C558: .asciz \" Invalid address \\\"%s\\\", setmaster command ignored\\n\"\n"
        ".align 3\n.globl D_0038C590\nD_0038C590: .asciz \"Adding master at %s\\n\"\n"
        ".align 3\n.globl D_0038C5A8\nD_0038C5A8: .asciz \"Can't remove master, list is empty\\n\"\n"
        ".align 3\n.globl D_0038C5D0\nD_0038C5D0: .asciz \"Can't remove master %s, not in list\\n\"\n"
        ".section .text\n");
#endif

void Master_ResetHeartbeats(void)
{
    master_t *p;

    for (p = valvemaster_adr; p; p = p->next)
        p->last_heartbeat = -9999.0f;
}

void Master_UseDefault(void)
{
    char szMaster[256];
    netadr_t adr;

    sprintf(szMaster, "half-life.east.won.net:27010");
    if (NET_StringToAdr(szMaster, &adr))
        Master_AddServer(&adr);
}

#ifdef NON_MATCHING
void Master_Init(void)
{
    static qboolean bInitialized = false;
    char szDefault[256];
    char szFile[256];
    char szPort[16];
    netadr_t adr;
    char szAdr[64];
    char szValue[256];
    char *pData;
    char *pStart;
    int nCount;
    int nDefaultPort;
    int nPort;
    int i;
    qboolean bIgnore;

    nCount = 0;
    nDefaultPort = 0;

    if (gfNoMasterServer)
        return;

    if (D_00334540)
        return;

    if (sv_lan.value != 0)
        return;

    if (svs.maxclients < 2)
        return;

    if (bInitialized)
        return;

    bInitialized = true;

    sprintf(szDefault, "half-life.east.won.net:27010");
    sprintf(szFile, "%s", "woncomm.lst");

    i = COM_CheckParm("-comm");
    if (i && i < com_argc - 1)
        strcpy(szFile, com_argv[i + 1]);

    pData = (char *)COM_LoadFile(szFile, 5, NULL);
    if (!pData)
    {
        Con_Printf("Couldn't load woncomm.lst\nUsing default master\n");
        Master_UseDefault();
        return;
    }

    pStart = pData;
    while (1)
    {
        pStart = COM_Parse(pStart);
        if (!strlen(com_token))
            break;

        bIgnore = true;
        if (!strcasecmp(com_token, "Master"))
        {
            bIgnore = false;
            nDefaultPort = PORT_MASTER;
        }

        pStart = COM_Parse(pStart);
        if (!strlen(com_token))
            break;

        if (strcasecmp(com_token, "{"))
            break;

        while (1)
        {
            pStart = COM_Parse(pStart);
            if (!strlen(com_token))
                break;

            if (!strcasecmp(com_token, "}"))
                break;

            sprintf(szValue, "%s", com_token);

            pStart = COM_Parse(pStart);
            if (!strlen(com_token))
                break;

            if (strcasecmp(com_token, ":"))
                break;

            pStart = COM_Parse(pStart);
            if (!strlen(com_token))
                break;

            nPort = atoi(com_token);
            if (!nPort)
                nPort = nDefaultPort;

            sprintf(szAdr, "%s:%i", szValue, nPort);

            if (!NET_StringToAdr(szAdr, &adr))
                bIgnore = true;

            if (!bIgnore)
            {
                nCount++;
                Con_Printf("Adding master server %s\n", NET_AdrToString(adr));
                Master_AddServer(&adr);
            }
        }
    }

    if (!nCount)
    {
        Con_Printf("No masters parsed from woncomm.lst\nUsing default master\n");
        Master_UseDefault();
    }

    Mem_Free(pData);
}
#else
qboolean bInitialized = false;
__asm__(".section .rodata\n.align 3\n.globl D_0038C618\nD_0038C618: .asciz \"%s\"\n.section .text\n");
INCLUDE_ASM("asm/nonmatchings/engine/Host", Master_Init);
#endif

void Host_SetRollingFPS(float fps)
{
    rolling_fps = fps;
}

void Host_GetHostInfo(float *fps, int *nActive, int *nSpectators, int *nMaxPlayers, char *pszMap)
{
    int clients;
    int spectators;

    spectators = 0;
    clients = 0;

    *fps = rolling_fps;
    SV_CountPlayers(&clients, &spectators);
    clients -= spectators;
    *nActive = clients;
    *nSpectators = spectators;

    if (pszMap)
    {
        if (sv.name && sv.name[0])
            strcpy(pszMap, sv.name);
        else
            *pszMap = 0;
    }

    *nMaxPlayers = svs.maxclients;
}

void Host_Speeds(float time1, float time2, float time3, float time4)
{
    float pass1, pass2, pass3;
    float fps;
    int i, ent_count;

    pass1 = (time2 - time1) * 1000.0f;
    pass2 = (time3 - time2) * 1000.0f;
    pass3 = (time4 - time3) * 1000.0f;

    fps = pass1 + pass2 + pass3;
    if (fps < 1.0f)
        fps = 100.0f;
    else
        fps = 1000.0f / fps;

    Host_SetRollingFPS(fps);

    if (host_speeds.value != 0)
    {
        ent_count = 0;
        for (i = 0; i < sv.num_edicts; i++)
        {
            if (!sv.edicts[i].free)
                ent_count++;
        }

        Con_DPrintf("%3i fps -- sv(%3.0f) gfx(%3.0f) snd(%3.0f) ents(%d)\n", (int)fps, pass1, pass2, pass3, ent_count);
    }
}

void Host_ConnectionlessPacket(void)
{
    MSG_BeginReading();
    MSG_ReadLong();
    Cmd_TokenizeString(func_002436B0());

    if (!strcmp(Cmd_Argv(0), "rcon"))
        func_00297D10(&net_from);
}

void _Host_Frame(float time)
{
    static float time1 = 0;
    static float time2 = 0;
    static float time3 = 0;
    static float time4 = 0;
    int i;
    int j;

    if (!Host_FilterTime(time))
        return;

    for (j = 0; j < cl_numlocal; j++)
    {
        if ((cls[j].state == ca_connected || cls[j].state == ca_uninitialized) && cls[j].signon == 3)
            cls[j].state = ca_active;
    }

    Cbuf_Execute();

    for (i = 0; i < cl_numlocal; i++)
    {
        if (cls[i].state == ca_active)
            func_00247118(i);
    }

    if (sv.active)
    {
        for (i = 0; i < cl_numlocal; i++)
            func_00251710(i);

        Host_ServerFrame();
    }

    for (i = 0; i < cl_numlocal; i++)
        func_0024FB48(i);

    if (!sv.active)
    {
        for (i = 0; i < cl_numlocal; i++)
            func_00251710(i);
    }

    for (i = 0; i < cl_numlocal; i++)
    {
        if (cls[i].state == ca_active)
            func_002524F8(i);
    }

    for (i = 0; i < cl_numlocal; i++)
        func_0024C120(i);

    for (i = 0; i < cl_numlocal; i++)
        func_002507A0(i);

    for (i = 0; i < cl_numlocal; i++)
    {
        while (func_00295718(i))
            ;
    }

    SV_CheckTimeouts();
    host_time += host_frametime;
    func_00245848();

    time2 = Sys_FloatTime();

    if (!D_0033453C)
    {
        SCR_UpdateScreen();
        if (D_00399168 && D_003995E0 == 0)
            func_002C5228(0);
    }

    time3 = Sys_FloatTime();
    func_0024EFB8(0);

    if (!D_0033453C)
    {
        if (cls[0].signon == 3)
        {
            if (cl_numlocal == 1)
                S_Update(r_origin, vpn, vright, vup, 0);
            else
                func_002CD788();

            func_00251330();
        }
        else
        {
            S_Update(vec3_origin, vec3_origin, vec3_origin, vec3_origin, 0);
        }
    }

    if (D_0033452C)
        D_0033452C();

    time1 = time4;
    time4 = Sys_FloatTime();
    Host_Speeds(time1, time2, time3, time4);

    if ((D_00399798 & 4) && cls[0].state == ca_disconnected)
        giActive = 2;

    host_framecount++;
}

int Host_Frame(float time, int iState, int *stateInfo)
{
    if (setjmp(host_abortserver))
        return giActive;

    if (cls[0].state == ca_active && D_00334544)
    {
        func_002C63C0(D_0053DDF8);
        D_00334544 = 0;
        memset(D_0053DDF8, 0, 0x40);
    }

    giActive = iState;
    *stateInfo = 0;
    _Host_Frame(time);

    if (giStateInfo)
    {
        *stateInfo = giStateInfo;
        giStateInfo = 0;
        Cbuf_Execute();
    }

    return giActive;
}

void Host_CheckGore(void)
{
    char szBuffer[128];

    memset(szBuffer, 0, sizeof(szBuffer));
    Cvar_SetValue("violence_hblood", 1.0f);
    Cvar_SetValue("violence_hgibs", 1.0f);
    Cvar_SetValue("violence_ablood", 1.0f);
    Cvar_SetValue("violence_agibs", 1.0f);
}

void Host_PrintServerCvars(void)
{
    cvar_t *var;

    if (svs.log.active)
    {
        Log_Printf("server cvars start\n");
        for (var = cvar_vars; var; var = var->next)
        {
            if (var->flags & FCVAR_SERVER)
                Log_Printf("\"%s\" = \"%s\"\n", var->name, var->string);
        }
        Log_Printf("server cvars end\n");
    }
}

#ifdef NON_MATCHING
#define STR_S "%s"
#else
/* emitida junto com Master_Init (INCLUDE_ASM); usar o literal geraria uma cópia nova */
extern char D_0038C618[];
#define STR_S D_0038C618
#endif

void Log_Printf(char *fmt, ...)
{
    va_list argptr;
    int ltime;
    int len;
    tm_t *today;

    if (!svs.log.active)
        return;

    if (svs.maxclients < 2)
        return;

    today = Sys_LocalTime(&ltime);
    sprintf(log_string, "L %02i/%02i/%04i - %02i:%02i:%02i: ", today->tm_mon + 1, today->tm_mday,
            today->tm_year + 1900, today->tm_hour, today->tm_min, today->tm_sec);

    len = strlen(log_string);
    va_start(argptr, fmt);
    vsprintf(&log_string[len], fmt, argptr);
    va_end(argptr);

    if (mp_logecho.value != 0)
        Con_Printf(STR_S, log_string);

    if (svs.log.file && mp_logfile.value != 0)
        func_00222500(svs.log.file, STR_S, log_string);

    if (svs.log.net_log)
        Netchan_OutOfBandPrint(NS_SERVER, svs.log.net_address, 0, "log %s", log_string);
}

void Log_Close(void)
{
    if (svs.log.file)
    {
        Log_Printf("Log closed.\n");
        FS_Close(svs.log.file);
    }
    svs.log.file = NULL;
}

void Log_Open(void)
{
    char szFileBase[260];
    char szTestFile[260];
    int ltime;
    tm_t *today;
    int i;
    FileHandle_t fp;

    if (!svs.log.active)
        return;

    Log_Close();
    today = Sys_LocalTime(&ltime);
    sprintf(szFileBase, "%s/logs/L%02i%02i", com_gamedir, today->tm_mon + 1, today->tm_mday);

    for (i = 0; i < 1000; i++)
    {
        sprintf(szTestFile, "%s%03i.log", szFileBase, i);
        COM_FixSlashes(szTestFile);
        fp = FS_Open(szTestFile, "r");
        if (!fp)
        {
            fp = FS_Open(szTestFile, "wt");
            if (!fp)
            {
                i = 1000;
            }
            else
            {
                Con_Printf("Server logging data to file %s\n", szTestFile);
            }
            break;
        }
        FS_Close(fp);
    }

    if (i == 1000)
    {
        Con_Printf("Unable to open logfiles under %s\nLogging disabled\n", szFileBase);
        svs.log.active = false;
        return;
    }

    if (fp)
        svs.log.file = fp;

    Log_Printf("Log file started.\n");
}

void Host_Version(void)
{
    strcpy(gpszVersionString, "Playstation 2");
    Con_DPrintf("Protocol version: %i\nExe version: %s\n", protocol_version, gpszVersionString);
    Con_DPrintf("Exe build: 19:13:34 Oct 19 2001 (%i)\n", build_number());
}

int Host_Init(quakeparms_t *parms)
{
    int i;
    int j;
    int k;
    byte *pb;
    byte *pal;
    short *pw;

    host_parms = *parms;
    com_argc = parms->argc;
    com_argv = parms->argv;
    realtime = 0;

    if (COM_CheckParm("-console") || COM_CheckParm("-toconsole") || COM_CheckParm("-dev"))
        D_00331C18 = 0;
    else
        D_00331C18 = 1;

    i = COM_CheckParm("-protocol");
    if (i)
    {
        i = Q_atoi(com_argv[i + 1]);
        if (i == 35)
            protocol_version = i;
    }

    func_00294518();
    func_00277B78(protocol_version);
    Memory_Init(parms->membase, parms->memsize);
    Cbuf_Init();
    Cmd_Init();
    Cvar_CmdInit();
    func_0028E548();
    func_00247660();
    COM_Init(parms->basedir);
    Host_InitLocal();
    Key_Init();
    Con_Init();
    Decal_Init();
    Mod_Init();
    NET_Init();
    Netchan_Init();
    SV_Init();
    Host_Version();

    Con_DPrintf("%4.1f megabyte heap\n", parms->memsize / (1024.0f * 1024.0f));

    if (cls[0].state != ca_dedicated)
    {
        pal = COM_LoadHunkFile("gfx/palette.lmp");
        if (!pal)
            Sys_Error("Couldn't load gfx/palette.lmp");

        host_basepal = Hunk_AllocName(256 * 4 * sizeof(short), "palette.lmp");
        pw = host_basepal;
        pb = pal;
        for (k = 0; k < 256; k++)
        {
            pw[0] = (char)pb[2];
            pw[1] = (char)pb[1];
            pw[2] = (char)pb[0];
            pw[3] = 0;
            pw += 4;
            pb += 3;
        }

        func_00246E78();
        if (!VID_Init(host_basepal))
            return 0;

        Draw_Init();
        SCR_Init();
        R_Init();
        S_Init(0);
        func_00235798();

        for (j = 0; j < 2; j++)
            CL_Init(j);

        func_002C5430();
        D_003997D8();
        func_00294508();
    }

    Cbuf_InsertText("exec valve.rc\n");
    host_hunklevel = 0;
    giActive = 1;
    D_00399704 = 0;

    if (COM_CheckParm("-dev"))
    {
        Cvar_SetValue("sv_cheats", 1.0f);
        Cvar_SetValue("developer", 1.0f);
    }

    Host_CheckGore();
    host_initialized = true;
    return 1;
}

void Host_InitHunkLevel(void)
{
    Hunk_AllocName(0, "-HOST_HUNKLEVEL-");
    host_hunklevel = Hunk_LowMark();
}

void Host_Shutdown(void)
{
    static qboolean isdown = false;

    if (isdown)
        return;

    isdown = true;

    if (host_initialized)
        Host_WriteConfiguration();

    host_initialized = false;

    if (cls[0].state != ca_dedicated)
        func_00246FC8();

    SV_DeallocateDynamicData();
    Master_Shutdown(true);
    scr_disabled_for_loading = true;
    func_002CBE40();
    func_002C54C8();
    Con_Shutdown();

    if (cls[0].state != ca_dedicated)
        D_003997A8();

    NET_Shutdown();
    func_002502D8(0);
    CM_FreePAS();

    if (D_00333114)
        Mem_Free(D_00333114);
    D_00333114 = NULL;

    Log_Printf("Server shutdown.\n");
    Log_Close();
    func_002CF640();

    if (cls[0].state != ca_dedicated)
        func_00294510();
}

void SCR_SizeUp_f(void)
{
    if (scr_viewsize.value < 120)
        Cvar_SetValue("viewsize", scr_viewsize.value + 10);
}

void SCR_SizeDown_f(void)
{
    if (scr_viewsize.value > 30)
        Cvar_SetValue("viewsize", scr_viewsize.value - 10);
}
