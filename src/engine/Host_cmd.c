#include "common.h"
#include "engine/cmd.h"
#include "engine/cvar.h"
#include "engine/server.h"
#include "engine/client.h"
#include "engine/eiface.h"

/* host_cmd.c: comandos do host (status, map, save/restore, rcon...) */

#define RD_NONE 0
#define RD_CLIENT 1
#define RD_PACKET 2
#define A2C_PRINT 'l'
#define A2M_GET_MOTD 'g'
#define svc_print 8
#define NS_CLIENT 0
#define NS_SERVER 2

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

typedef struct quakeparms_s
{
    char *basedir;
    char *cachedir;
    int argc;
    char **argv;
    void *membase;
    int memsize;
} quakeparms_t;

extern quakeparms_t host_parms;
extern qboolean gfNoMasterServer;
extern master_t *valvemaster_adr;
extern int sv_redirected;
extern char outputbuf[1400];
extern netadr_t sv_redirectto;
extern cvar_t rcon_password;
extern sizebuf_t net_message;
extern int giActive;
extern char D_00392178[];
void Log_Printf(char *fmt, ...);
char *strcat(char *, const char *);
void CL_Disconnect(int player);
void func_0029C330(void);
extern float realtime;
extern char D_00392278[];
int func_0027E140(client_t *cl);
char *func_002C6E08(netadr_t a);
extern int cmd_source;
extern int D_0033473C;
#define noip D_0033473C
extern netadr_t net_local_adr;
extern char gpszVersionString[];
extern vec3_t r_origin;
int build_number(void);
void SV_ClientPrintf(char *fmt, ...);
extern cvar_t deathmatch;
extern cvar_t coop;
extern qboolean (*D_0039979C)(void);
extern int key_dest;
extern char D_00392078[];
extern int D_00332F34;
extern UserMsg *sv_gpUserMsgs;
extern UserMsg *sv_gpNewUserMsgs;
void func_0021C340(void);
void Host_ShutdownServer(qboolean crash);
void func_002AC478(int a0);
void func_0029C388(void);
int SV_SpawnServer(qboolean bIsDemo, char *server, char *startspot);
int func_0029B488(char *level, int a1);
void SV_LoadEntities(void);
void func_00236A10(void);
void SV_ActivateServer(int runPhysics);
char *strcpy(char *, const char *);
void *memcpy(void *, const void *, unsigned int);
extern char D_00392470[];
extern char D_00392070[];
extern cvar_t sv_lan;
int PF_IsMapValid_I(char *mapname);
void func_00245310(void);
void func_002CFB90(char *pszSubString);
extern char D_003924F8[];
extern char D_00392520[];
void func_002CD340(int a0, int a1);
char *Host_SaveGameDirectory(void);
int func_0022B6F0(char *pattern, char *data);
float func_0022B8E0(char *data);
int func_0022B810(int h, char *data);
void func_0022B8A0(int h);
void func_00239488(void);
void func_00228CF0(char *dst, char *src);
void func_00228EE0(char *dst, char *fmt, ...);
extern cvar_t D_003330E8;
#define host_map D_003330E8
extern char D_00392558[];
int Host_Load(char *pName);
int func_00228DC0(const char *s);
int func_00228E40(const char *s1, const char *s2, int n);
void MSG_WriteChar(sizebuf_t *sb, int c);
char *Cmd_Args(void);
char *strncpy(char *, const char *, unsigned int);
void func_00250B08(int player);
char *func_0022E958(void);

typedef struct
{
    int id;                     /* 0x00 */
    edict_t *pent;              /* 0x04 */
    int location;               /* 0x08 */
    int size;                   /* 0x0C */
    int flags;                  /* 0x10 */
    string_t classname;         /* 0x14 */
} ENTITYTABLE;

typedef struct
{
    char mapName[32];           /* 0x00 */
    char landmarkName[32];      /* 0x20 */
    edict_t *pentLandmark;      /* 0x40 */
    vec3_t vecLandmarkOrigin;   /* 0x44 */
} LEVELLIST;

#define MAX_LEVEL_CONNECTIONS 16

typedef struct saverestore_s
{
    char *pBaseData;            /* 0x000 */
    char *pCurrentData;         /* 0x004 */
    int size;                   /* 0x008 */
    int bufferSize;             /* 0x00C */
    int tokenSize;              /* 0x010 */
    int tokenCount;             /* 0x014 */
    char **pTokens;             /* 0x018 */
    int currentIndex;           /* 0x01C */
    int tableCount;             /* 0x020 */
    int connectionCount;        /* 0x024 */
    ENTITYTABLE *pTable;        /* 0x028 */
    LEVELLIST levelList[MAX_LEVEL_CONNECTIONS]; /* 0x02C */
    int fUseLandmark;           /* 0x52C */
    char szLandmarkName[20];    /* 0x530 */
    vec3_t vecLandmarkOffset;   /* 0x544 */
    float time;                 /* 0x550 */
    char szCurrentMapName[32];  /* 0x554 */
} SAVERESTOREDATA;              /* 0x574 */

extern char D_00392628[];
extern int D_00331B00;
extern SAVERESTOREDATA *D_00331AFC;
extern int D_00331B04;
void *Mem_DbgCalloc(int num, int size, const char *file, int line);
void Mem_Free(void *p);
void COM_FixSlashes(char *pname);
void func_0022B248(char *name);
void func_0022B2B0(char *oldName, char *newName);

typedef struct
{
    char mapName[32];           /* 0x00 */
    char comment[80];           /* 0x20 */
    int mapCount;               /* 0x70 */
} GAME_HEADER;

#define SAVEGAME_HEADER (('V' << 24) | ('A' << 16) | ('S' << 8) | 'J')
#define SAVEGAME_VERSION 0x0071

extern char D_00392640[];
extern char D_00392658[];
extern char D_00392668[];
extern char D_00392690[];
extern char D_00392698[];
extern char D_003926A0[];
extern char D_003926B8[];
extern char D_003926C0[];
extern char D_003926D0[];
extern char D_00332F38[];
SAVERESTOREDATA *SaveGamestate(void);
void func_00217560(char *dir, char *mapname);
void func_0022B310(char *name, int a1);
int func_0029C220(char *pPath);
void func_00244138(char *path, char *extension);
int func_0022B118(char *name, char *mode);
int func_0022BD98(void *data, int size, int count, int file);
void func_0029BFC8(char *pPath, int file);
void func_0022B238(int file);
extern char D_003926F8[];
extern char D_00392700[];
extern char D_00392728[];
extern int D_00399704;
void func_00299420(char *pszComment);
void func_00251DC0(char *pMessage, int a1);
char *strstr(const char *, const char *);
int func_0022BE50(void *buf, int size, int count, int file);
extern const char _ctype_[];
#define isdigit(c) ((_ctype_ + 1)[(int)(c)] & 4)
extern char D_00392788[];
extern char D_003927A8[];
extern char D_003927B8[];
extern char D_003927C0[];
extern char D_003927D0[];
extern char D_003927E0[];
extern char D_003927E8[];
extern int giStateInfo;
int atoi(const char *);
char *func_00243F68(char *pName);
int func_0022B590(char *src, char *dst);
void func_0022BC60(char *name);
void func_0029C108(int pFile, int mapCount);
void Cbuf_AddText(char *text);

typedef struct
{
    int saveId;                 /* 0x00 */
    int version;                /* 0x04 */
    int skillLevel;             /* 0x08 */
    int entityCount;            /* 0x0C */
    int connectionCount;        /* 0x10 */
    int lightStyleCount;        /* 0x14 */
    float time;                 /* 0x18 */
    char mapName[32];           /* 0x1C */
    char skyName[32];           /* 0x3C */
    int skyColor_r;             /* 0x5C */
    int skyColor_g;             /* 0x60 */
    int skyColor_b;             /* 0x64 */
    float skyVec_x;             /* 0x68 */
    float skyVec_y;             /* 0x6C */
    float skyVec_z;             /* 0x70 */
} SAVE_HEADER;

typedef struct
{
    int index;                  /* 0x00 */
    char style[64];             /* 0x04 */
} SAVELIGHTSTYLE;

#define MAX_LIGHTSTYLES 64
#define FENTTABLE_PLAYER 0x80000000

extern cvar_t skill;
extern cvar_t sv_skyname;
extern cvar_t cl_skycolor_r;
extern cvar_t cl_skycolor_g;
extern cvar_t cl_skycolor_b;
extern cvar_t cl_skyvec_x;
extern cvar_t cl_skyvec_y;
extern cvar_t cl_skyvec_z;
extern char D_003927F0[];
extern char D_00392800[];
extern char D_00392810[];
extern char D_00392820[];
extern char D_00392850[];
extern char D_00332F68[];
extern char D_00333038[];
extern char D_003330C8[];
extern char D_00333078[];
void EntityPatchWrite(SAVERESTOREDATA *pSaveData, char *level);
void CL_Save(char *name);

/* entrada da lista de decals montada por func_002A50B8 */
typedef struct
{
    char *name;                 /* 0x00 */
    short entityIndex;          /* 0x04 */
    byte depth;                 /* 0x06 */
    byte flags;                 /* 0x07 */
    vec3_t position;            /* 0x08 */
} savedecal_t; /* 0x14 */

int func_002A50B8(savedecal_t *pList, int a1);
void Mem_PushTag(void);
void Mem_SetTag(int tag);
void Mem_PopTag(void);
void func_0022BBC0(char *name);
extern char *pr_strings;
void ReleaseEntityDLLFields(edict_t *pEdict);
void InitEntityDLLFields(edict_t *pEdict);
void func_00217678(char *dir, char *level);
extern char D_00392898[];
void *Q_memcpy(void *dest, void *src, int count);
void *Hunk_AllocName(int size, char *name);
#define FENTTABLE_REMOVED 0x40000000
void SV_CountPlayers(int *clients, int *spectators);
int strcasecmp(const char *, const char *);
#include "engine/mathlib.h"
#include "engine/progdefs.h"
#define FL_GODMODE 0x40
#define FL_NOTARGET 0x80
#define MOVETYPE_WALK 3
#define MOVETYPE_FLY 5
#define MOVETYPE_NOCLIP 8
extern int D_00399410;
#define allow_cheats D_00399410
extern edict_t *sv_player;
extern qboolean noclip_anglehack;
edict_t *SV_TestEntityPosition(edict_t *ent);
void SV_ClientPrintf(char *fmt, ...);

int sprintf(char *, const char *, ...);
unsigned int strlen(const char *);
int strcmp(const char *, const char *);
void Cbuf_Execute(void);
void func_002C68B0(char *basedir);
void func_002C70A8(qboolean multiplayer);
char *NET_AdrToString(netadr_t a);
void NET_SendPacket(int sock, int length, void *data, netadr_t to, int unk);
void MSG_WriteLong(sizebuf_t *sb, int c);
void MSG_WriteString(sizebuf_t *sb, const char *s);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00391EE8);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00391EF0);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00391F00);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00391F08);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00391F18);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00391F20);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00391F28);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00391F38);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00391F48);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00391F58);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00391F68);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00391F78);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00391F88);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00391F98);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00391FA0);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00391FA8);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00391FB8);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00391FC8);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00391FD8);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00391FE8);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392000);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392010);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392020);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392030);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392038);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392040);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392050);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392058);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392060);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392068);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392070);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392078);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392080);

void Host_InitializeGameDLL(void)
{
    Cbuf_Execute();

    if (svs.unk_00C)
    {
        Con_DPrintf("Sys_InitializeGameDLL called twice, skipping second call\n");
        return;
    }

    svs.unk_00C = true;
    func_002C68B0(host_parms.basedir);
    gEntityInterface.pfnGameInit();
    Cbuf_Execute();
}

void Host_Motd_f(void)
{
    char szMsg[2048];
    master_t *p;

    if (gfNoMasterServer)
        return;

    if (!valvemaster_adr)
        return;

    func_002C70A8(true);
    sprintf(szMsg, "%c\n", A2M_GET_MOTD);

    p = valvemaster_adr;
    Con_DPrintf("Requesting MOTD from master(s)\n", NET_AdrToString(p->adr));

    for (; p; p = p->next)
        NET_SendPacket(NS_CLIENT, strlen(szMsg), szMsg, p->adr, 2);
}

void SV_InactivateClients(void)
{
    int i;
    client_t *cl;

    for (i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
    {
        if (!cl->active && !cl->connected && !cl->spawned)
            continue;

        cl->active = false;
        cl->connected = true;
        cl->spawned = false;

        if (!cl->fakeclient)
        {
            SZ_Clear(&cl->netchan.message);
            cl->maxspeed = 0;
        }
    }
}

void SV_FlushRedirect(void)
{
    sizebuf_t buf;
    byte data[1425];

    if (sv_redirected == RD_PACKET)
    {
        buf.maxsize = sizeof(data);
        buf.data = data;
        buf.cursize = 0;

        MSG_WriteLong(&buf, -1);
        MSG_WriteByte(&buf, A2C_PRINT);
        MSG_WriteString(&buf, outputbuf);
        MSG_WriteByte(&buf, 0);
        NET_SendPacket(NS_SERVER, buf.cursize, buf.data, sv_redirectto, 0);
    }
    else if (sv_redirected == RD_CLIENT)
    {
        MSG_WriteByte(&host_client->netchan.message, svc_print);
        MSG_WriteString(&host_client->netchan.message, outputbuf);
    }

    outputbuf[0] = 0;
}

void SV_BeginRedirect(int rd, netadr_t *addr)
{
    sv_redirected = rd;
    sv_redirectto = *addr;
    outputbuf[0] = 0;
}

void SV_EndRedirect(void)
{
    SV_FlushRedirect();
    sv_redirected = RD_NONE;
}

int Rcon_Validate(void)
{
    if (!strlen(rcon_password.string))
        return 0;

    if (strcmp(Cmd_Argv(1), rcon_password.string))
        return 0;

    return 1;
}

void SV_Rcon(netadr_t *adr)
{
    char remaining[1024];
    int valid;

    valid = Rcon_Validate();

    if (!valid)
        Con_Printf("Bad rcon from %s:\n%s\n", NET_AdrToString(*adr), net_message.data + 4);
    if (valid == 1)
        Con_Printf("Rcon from %s:\n%s\n", NET_AdrToString(*adr), net_message.data + 4);
    if (!valid)
        Log_Printf("Bad Rcon from \"%s\":\"(%s)\"\n", NET_AdrToString(*adr), net_message.data + 4);
    if (valid == 1)
        Log_Printf("Rcon from \"%s\":\"(%s)\"\n", NET_AdrToString(*adr), net_message.data + 4);

    SV_BeginRedirect(RD_PACKET, adr);

    if (!valid)
    {
        Con_Printf("Bad rcon_password.\n");
    }
    else
    {
        remaining[0] = 0;
        for (valid = 2; valid < Cmd_Argc(); valid++)
        {
            strcat(remaining, Cmd_Argv(valid));
            strcat(remaining, D_00392178);
        }
        Cmd_ExecuteString(remaining, src_command);
    }

    SV_EndRedirect();
}


void Host_Quit_f(void)
{
    int i;

    func_0029C330();
    giActive = 3;

    for (i = 0; i < cl_numlocal; i++)
        CL_Disconnect(i);
}

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392178);

void Host_Status_PrintClient(char *msg, qboolean bShowAddress, void (*print)(char *fmt, ...), int j, client_t *client)
{
    int seconds, minutes, hours;

    seconds = (int)(realtime - client->netchan.connect_time);
    minutes = seconds / 60;
    if (minutes)
    {
        seconds -= minutes * 60;
        hours = minutes / 60;
        if (hours)
            minutes -= hours * 60;
    }
    else
    {
        hours = 0;
    }

    print("#%-2u %-12.12s\n", j + 1, client->name);
    print("   frags:  %3i\n", (int)client->edict->v.frags);

    if (hours)
        print("   time :  %2i:%02i:%02i\n", hours, minutes, seconds);
    else
        print("   time :  %02i:%02i\n", minutes, seconds);

    print("   frame rate :  %.2f fps\n", client->netchan.frame_rate);
    print("   frame latency :  %.1f frames\n", client->netchan.frame_latency);
    print("   ping :  %4i\n", func_0027E140(client));
    print("   drop :  %5.2f %%\n", 100.0f * (float)client->netchan.drop_count / (float)client->netchan.incoming_sequence);

    if (msg && msg[0])
        print("   %s\n", msg);

    if (client->spectator)
        print("  (spectator) %s\n", func_002C6E08(client->netchan.remote_address));
    else if (client->fakeclient)
        print("  (fake)\n");
    else if (bShowAddress)
        print("   %s\n", func_002C6E08(client->netchan.remote_address));

    print(D_00392278);
}

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392278);

void Host_Status_f(void)
{
    client_t *client;
    int j;
    int nClients;
    int nSpectators;
    void (*print)(char *fmt, ...);

    if (cmd_source == src_command)
    {
        if (!sv.active)
        {
            Cmd_ForwardToServer();
            return;
        }
        print = (void (*)(char *, ...))Con_Printf;
    }
    else
    {
        print = SV_ClientPrintf;
    }

    print("hostname:  %s\n", Cvar_VariableString("hostname"));
    print("version :  %s\n", gpszVersionString);
    print("build   :  %d\n", build_number());

    if (!noip)
        print("tcp/ip  :  %s\n", NET_AdrToString(net_local_adr));

    print(" map     :  %s\n", sv.name);
    print("at: %d x, %d y, %d z\n", (int)r_origin[0], (int)r_origin[1], (int)r_origin[2]);

    SV_CountPlayers(&nClients, &nSpectators);
    if (nSpectators)
        print(" players: %i active (%i spectators) (%i max)\n\n", nClients, nSpectators, svs.maxclients);
    else
        print(" players: %i active (%i max)\n\n", nClients, svs.maxclients);

    if (Cmd_Argc() == 2 && !strcasecmp(Cmd_Argv(1), "short"))
    {
        for (j = 0, client = svs.clients; j < svs.maxclients; j++, client++)
        {
            if (client->active)
                print("#%i - %s\n", j + 1, client->name);
        }
        return;
    }

    for (j = 0, client = svs.clients; j < svs.maxclients; j++, client++)
    {
        if (!client->active)
        {
            if (client->connected)
                Host_Status_PrintClient("CONNECTING", cmd_source == src_command, print, j, client);
            continue;
        }
        Host_Status_PrintClient(NULL, cmd_source == src_command, print, j, client);
    }
}

void Host_God_f(void)
{
    if (cmd_source == src_command)
    {
        Cmd_ForwardToServer();
        return;
    }

    if (gGlobalVariables.deathmatch != 0 && !host_client->unk_73D0)
        return;

    if (!allow_cheats)
        return;

    sv_player->v.flags ^= FL_GODMODE;
    if (!(sv_player->v.flags & FL_GODMODE))
        SV_ClientPrintf("godmode OFF\n");
    else
        SV_ClientPrintf("godmode ON\n");
}

void Host_Notarget_f(void)
{
    if (cmd_source == src_command)
    {
        Cmd_ForwardToServer();
        return;
    }

    if (gGlobalVariables.deathmatch != 0 && !host_client->unk_73D0)
        return;

    sv_player->v.flags ^= FL_NOTARGET;
    if (!(sv_player->v.flags & FL_NOTARGET))
        SV_ClientPrintf("notarget OFF\n");
    else
        SV_ClientPrintf("notarget ON\n");
}

qboolean FindPassableSpace(edict_t *pEdict, vec_t *direction, float step)
{
    int i;

    for (i = 0; i < 100; i++)
    {
        VectorMA(pEdict->v.origin, step, direction, pEdict->v.origin);
        if (!SV_TestEntityPosition(pEdict))
        {
            VectorCopy(pEdict->v.origin, pEdict->v.oldorigin);
            return true;
        }
    }

    return false;
}

void Host_Noclip_f(void)
{
    vec3_t forward, right, up;

    if (cmd_source == src_command)
    {
        Cmd_ForwardToServer();
        return;
    }

    if (gGlobalVariables.deathmatch != 0 && !host_client->unk_73D0)
        return;

    if (!allow_cheats)
        return;

    if (sv_player->v.movetype != MOVETYPE_NOCLIP)
    {
        noclip_anglehack = true;
        sv_player->v.movetype = MOVETYPE_NOCLIP;
        SV_ClientPrintf("noclip ON\n");
    }
    else
    {
        noclip_anglehack = false;
        sv_player->v.movetype = MOVETYPE_WALK;
        VectorCopy(sv_player->v.origin, sv_player->v.oldorigin);
        SV_ClientPrintf("noclip OFF\n");

        if (SV_TestEntityPosition(sv_player))
        {
            AngleVectors(sv_player->v.v_angle, forward, right, up);
            if (!FindPassableSpace(sv_player, forward, 1.0f) &&
                !FindPassableSpace(sv_player, right, 1.0f) &&
                !FindPassableSpace(sv_player, right, -1.0f) &&
                !FindPassableSpace(sv_player, up, 1.0f) &&
                !FindPassableSpace(sv_player, up, -1.0f) &&
                !FindPassableSpace(sv_player, forward, -1.0f))
            {
                Con_DPrintf("Can't find the world\n");
            }
            VectorCopy(sv_player->v.oldorigin, sv_player->v.origin);
        }
    }
}

void Host_Fly_f(void)
{
    if (cmd_source == src_command)
    {
        Cmd_ForwardToServer();
        return;
    }

    if (gGlobalVariables.deathmatch != 0 && !host_client->unk_73D0)
        return;

    if (sv_player->v.movetype != MOVETYPE_FLY)
    {
        sv_player->v.movetype = MOVETYPE_FLY;
        SV_ClientPrintf("flymode ON\n");
    }
    else
    {
        sv_player->v.movetype = MOVETYPE_WALK;
        SV_ClientPrintf("flymode OFF\n");
    }
}

qboolean Host_ValidGame(void)
{
    if (svs.maxclients > 1)
    {
        if (deathmatch.value != 0 || coop.value != 0)
            return true;
    }
    else
    {
        if (D_0039979C && D_0039979C())
            return true;
    }

    Con_DPrintf("Unable to launch game\n");
    return false;
}

void Host_Ping_f(void)
{
    int i;
    client_t *client;

    if (cmd_source == src_command)
    {
        Cmd_ForwardToServer();
        return;
    }

    SV_ClientPrintf("Client ping times:\n");

    for (i = 0, client = svs.clients; i < svs.maxclients; i++, client++)
    {
        if (!client->active)
            continue;
        SV_ClientPrintf("%4i %s\n", func_0027E140(client), client->name);
    }
}

void Host_Map(qboolean bIsDemo, char *mapstring, char *mapName, qboolean loadGame)
{
    int i, j, k;
    UserMsg *pMsg;

    func_0021C340();
    for (k = 0; k < cl_numlocal; k++)
        CL_Disconnect(k);

    Host_ShutdownServer(false);
    key_dest = 0;
    func_002AC478(0);

    if (!loadGame)
    {
        func_0029C388();
        SV_InactivateClients();
        svs.serverflags = 0;
    }

    for (k = 0; k < cl_numlocal; k++)
        strcpy(cls[k].mapstring, mapstring);

    if (!Host_ValidGame())
        return;

    if (!SV_SpawnServer(bIsDemo, mapName, NULL))
        return;

    if (loadGame)
    {
        if (!func_0029B488(mapName, 1))
            SV_LoadEntities();
        func_00236A10();
        sv.paused = true;
        D_00332F34 = 0;
        sv.loadgame = true;
        SV_ActivateServer(0);
    }
    else
    {
        SV_LoadEntities();
        SV_ActivateServer(1);
        if (!sv.active)
            return;

        for (i = 0; i < cl_numlocal; i++)
        {
            if (cls[i].state)
            {
                memcpy(cls[i].spawnparms, D_00392078, 1);
                for (j = 2; j < Cmd_Argc(); j++)
                {
                    strcat(cls[i].spawnparms, Cmd_Argv(j));
                    strcat(cls[i].spawnparms, D_00392178);
                }
            }
        }
    }

    if (sv_gpNewUserMsgs)
    {
        if (!sv_gpUserMsgs)
        {
            sv_gpUserMsgs = sv_gpNewUserMsgs;
        }
        else
        {
            pMsg = sv_gpUserMsgs;
            while (pMsg->next)
                pMsg = pMsg->next;
            pMsg->next = sv_gpNewUserMsgs;
        }
        sv_gpNewUserMsgs = NULL;
    }

    if (cls[0].state)
        Cmd_ExecuteString("connect local", src_command);
}

void Host_Map_f(void)
{
    int i;
    char mapstring[64];
    char name[64];

    if (cmd_source != src_command)
        return;

    mapstring[0] = 0;
    for (i = 0; i < Cmd_Argc(); i++)
    {
        strcat(mapstring, Cmd_Argv(i));
        strcat(mapstring, D_00392178);
    }
    strcat(mapstring, D_00392278);

    strcpy(name, Cmd_Argv(1));

    if (!svs.unk_00C)
        Host_InitializeGameDLL();

    i = strlen(name);
    if (i > 4 && !Q_strcasecmp(name + i - 4, ".bs2"))
        name[i - 4] = 0;

    if (!PF_IsMapValid_I(name))
    {
        Con_Printf("map change failed: '");
        Con_Printf(name);
        Con_Printf(D_00392470);
        return;
    }

    if (svs.maxclients > 1 && sv_lan.value == 0)
        func_00245310();

    Cvar_Set(D_00392070, name);
    Host_Map(false, mapstring, name, false);
}

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392470);

void Host_Maps_f(void)
{
    char *pszSubString;

    if (Cmd_Argc() != 2)
    {
        Con_Printf("Usage:  maps <substring>\nmaps * for full listing\n");
        return;
    }

    pszSubString = Cmd_Argv(1);
    if (!pszSubString || !*pszSubString)
        return;

    if (*pszSubString == '*')
        pszSubString = NULL;

    func_002CFB90(pszSubString);
}

void Host_Changelevel_f(void)
{
    char level[64];
    char _startspot[64];
    char *startspot;

    if (Cmd_Argc() < 2 || !Host_ValidGame())
    {
        Con_Printf("changelevel <levelname> : continue game on a new level\n");
        return;
    }

    if (!sv.active)
    {
        Con_Printf(D_003924F8);
        return;
    }

    if (!PF_IsMapValid_I(Cmd_Argv(1)))
    {
        Con_Printf(D_00392520);
        Con_Printf(Cmd_Argv(1));
        Con_Printf(D_00392470);
        return;
    }

    func_002AC478(0);
    func_002CD340(1, 0);

    strcpy(level, Cmd_Argv(1));
    if (Cmd_Argc() == 2)
    {
        startspot = NULL;
    }
    else
    {
        strcpy(_startspot, Cmd_Argv(2));
        startspot = _startspot;
    }

    SV_InactivateClients();
    SV_SpawnServer(false, level, startspot);
    SV_LoadEntities();
    SV_ActivateServer(1);
}

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003924F8);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392520);

char *Host_FindRecentSave(char *pszOut)
{
    char data[260];
    float latest;
    float t;
    int found;
    int h;

    found = 0;
    latest = -1.0f;
    sprintf(pszOut, "%s*.sav", Host_SaveGameDirectory());

    h = func_0022B6F0(pszOut, data);
    while (h != -1)
    {
        t = func_0022B8E0(data);
        if (t > 0 && (!found || t > latest))
        {
            latest = t;
            strcpy(pszOut, data);
            found = 1;
        }

        if (!func_0022B810(h, data))
        {
            func_0022B8A0(h);
            h = -1;
        }
    }

    return found ? pszOut : NULL;
}

void Host_Restart_f(void)
{
    char name[260];

    if (!sv.active)
        return;

    if (cmd_source != src_command)
        return;

    func_00239488();
    func_0029C388();
    SV_InactivateClients();
    strcpy(name, sv.name);
    SV_SpawnServer(false, name, NULL);
    SV_LoadEntities();
    SV_ActivateServer(1);
}

void Host_Restartdecay_f(void)
{
    char cmd[100];
    char name[100];

    func_00228CF0(name, sv.name);
    func_00228EE0(cmd, "map %s", name);
    Host_Map(false, cmd, name, false);
}

void Host_Reload_f(void)
{
    char name[260];
    char *pSaveName;

    func_0021C340();

    if (!sv.active)
        return;

    if (cmd_source != src_command)
        return;

    func_0029C388();
    SV_InactivateClients();

    pSaveName = Host_FindRecentSave(name);
    if (pSaveName && Host_Load(pSaveName))
        return;

    func_00239488();

    if (func_00228DC0(host_map.string) >= 4 && !func_00228E40(host_map.string, "c0a0", 4))
        Cvar_Set(D_00392070, "c1a0");

    SV_SpawnServer(false, host_map.string, NULL);
    SV_LoadEntities();
    SV_ActivateServer(1);
}

void Host_Reconnect_f(void)
{
    if (cls[0].state < ca_connected)
        return;

    func_002AC478(0);
    cls[0].state = ca_connected;
    cls[0].signon = 0;
    MSG_WriteChar(&cls[0].netchan.message, clc_stringcmd);
    MSG_WriteString(&cls[0].netchan.message, D_00392558);
}

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392558);

void Host_Connect_f(void)
{
    char name[64];
    int i;

    if (Cmd_Argc() < 2 || !Cmd_Args())
    {
        Con_Printf("Usage:  connect <server>\n");
        return;
    }

    strcpy(name, Cmd_Args());
    strncpy(cls[0].servername, name, sizeof(cls[0].servername) - 1);

    for (i = 0; i < cl_numlocal; i++)
        func_00250B08(i);
}

char *Host_SaveGameDirectory(void)
{
    return func_0022E958();
}

void func_00299420(char *pszComment) {
}

void Host_SaveAgeList(char *pName, int count)
{
    char newName[260];
    char oldName[260];

    sprintf(newName, "%s%s%02d.sav", Host_SaveGameDirectory(), pName, count);
    COM_FixSlashes(newName);
    func_0022B248(newName);

    while (count > 0)
    {
        if (count == 1)
            sprintf(oldName, "%s%s.sav", Host_SaveGameDirectory(), pName);
        else
            sprintf(oldName, "%s%s%02d.sav", Host_SaveGameDirectory(), pName, count - 1);
        COM_FixSlashes(oldName);

        sprintf(newName, "%s%s%02d.sav", Host_SaveGameDirectory(), pName, count);
        COM_FixSlashes(newName);

        func_0022B2B0(oldName, newName);
        count--;
    }
}

int Host_ValidSave(void)
{
    if (cmd_source != src_command)
        return 0;

    if (!sv.active)
    {
        Con_Printf("Not playing a local game.\n");
        return 0;
    }

    if (cl[0].intermission || cls[0].state != ca_active)
    {
        Con_Printf("Can't save in intermission.\n");
        return 0;
    }

    if (svs.maxclients != 1)
    {
        Con_Printf("Can't save multiplayer games.\n");
        return 0;
    }

    if (svs.clients->active && svs.clients->edict->v.health <= 0)
    {
        Con_Printf("Can't savegame with a dead player\n");
        return 0;
    }

    return 1;
}

SAVERESTOREDATA *SaveInit(int size)
{
    int i;
    SAVERESTOREDATA *pSaveData;
    edict_t *pent;

    if (size <= 0)
        size = 0x40000;

    if (D_00331B00 >= (int)(sizeof(SAVERESTOREDATA) + sizeof(ENTITYTABLE) * sv.num_edicts + size))
    {
        pSaveData = D_00331AFC;
        D_00331B04 = 1;
    }
    else
    {
        pSaveData = Mem_DbgCalloc(sizeof(SAVERESTOREDATA) + sizeof(ENTITYTABLE) * sv.num_edicts + size, 1, D_00392628, 1479);
        D_00331B04 = 0;
    }

    pSaveData->pTable = (ENTITYTABLE *)(pSaveData + 1);
    pSaveData->tokenSize = 0;
    pSaveData->tokenCount = 0xFFF;
    pSaveData->pTokens = Mem_DbgCalloc(0xFFF, sizeof(char *), D_00392628, 1488);

    for (i = 0; i < sv.num_edicts; i++)
    {
        pent = sv.edicts + i;
        pSaveData->pTable[i].id = i;
        pSaveData->pTable[i].pent = pent;
        pSaveData->pTable[i].flags = 0;
        pSaveData->pTable[i].location = 0;
        pSaveData->pTable[i].size = 0;
        pSaveData->pTable[i].classname = 0;
    }

    pSaveData->tableCount = sv.num_edicts;
    pSaveData->connectionCount = 0;
    pSaveData->pBaseData = (char *)(pSaveData->pTable + sv.num_edicts);
    pSaveData->pCurrentData = pSaveData->pBaseData;
    pSaveData->size = 0;
    pSaveData->bufferSize = size;
    pSaveData->time = gGlobalVariables.time;
    VectorCopy(vec3_origin, pSaveData->vecLandmarkOffset);
    pSaveData->fUseLandmark = 0;
    gGlobalVariables.pSaveData = pSaveData;

    return pSaveData;
}




void SaveExit(SAVERESTOREDATA *save)
{
    if (save->pTokens)
    {
        Mem_Free(save->pTokens);
        save->pTokens = NULL;
        save->tokenCount = 0;
    }

    if (save)
    {
        if (!D_00331B04)
            Mem_Free(save);
        else
            D_00331B04 = 0;
    }

    gGlobalVariables.pSaveData = NULL;
}

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392628);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392640);

int SaveGameSlot(char *pSaveName, char *pSaveComment)
{
    char name[256];
    char pszFileName[256];
    GAME_HEADER gameHeader;
    char hlPath[256];
    int tag;
    char *pTokenData;
    char *pszToken;
    int i;
    int next;
    int file;
    SAVERESTOREDATA *pSaveData;

    pSaveData = SaveGamestate();
    if (!pSaveData)
        return 0;

    if (0)
    {
overflow:
        Con_Printf(D_00392668);
        pSaveData->size = pSaveData->bufferSize;
        goto done;
    }

    SaveExit(pSaveData);

    sprintf(hlPath, D_00392640, Host_SaveGameDirectory(), sv.name);
    func_00217560(Host_SaveGameDirectory(), sv.name);
    func_0022B310(hlPath, 5);

    pSaveData = SaveInit(0);

    sprintf(name, "%s*.HL?", Host_SaveGameDirectory());
    COM_FixSlashes(name);
    gameHeader.mapCount = func_0029C220(name);
    strcpy(gameHeader.mapName, sv.name);
    strcpy(gameHeader.comment, pSaveComment);

    gEntityInterface.pfnSaveWriteFields(pSaveData, D_00392658, &gameHeader, D_00332F38, 3);
    gEntityInterface.pfnSaveGlobalState(pSaveData);

    pTokenData = pSaveData->pCurrentData;

    if (pSaveData->pTokens)
    {
        for (i = 0; i < pSaveData->tokenCount; i = next)
        {
            if (pSaveData->pTokens[i])
            {
                pszToken = pSaveData->pTokens[i];
                pSaveData->size += strlen(pszToken) + 1;
                if (pSaveData->size > pSaveData->bufferSize)
                    goto overflow;
                next = i + 1;
                while ((*pSaveData->pCurrentData++ = *pszToken++) != 0)
                    ;
            }
            else
            {
                pSaveData->size += 1;
                if (pSaveData->size > pSaveData->bufferSize)
                    goto overflow;
                next = i + 1;
                *pSaveData->pCurrentData++ = 0;
            }
        }
    }
done:
    pSaveData->tokenSize = pSaveData->pCurrentData - pTokenData;
    if (pSaveData->size < pSaveData->bufferSize)
        pSaveData->size -= pSaveData->tokenSize;

    sprintf(pszFileName, D_00392690, Host_SaveGameDirectory(), pSaveName);
    func_00244138(pszFileName, D_00392698);
    COM_FixSlashes(pszFileName);
    Con_DPrintf(D_003926A0, pszFileName);

    if (!strcasecmp(pSaveName, D_003926B8) || !strcasecmp(pSaveName, D_003926C0))
        Host_SaveAgeList(pSaveName, 1);

    file = func_0022B118(pszFileName, D_003926D0);

    tag = SAVEGAME_HEADER;
    func_0022BD98(&tag, sizeof(int), 1, file);
    tag = SAVEGAME_VERSION;
    func_0022BD98(&tag, sizeof(int), 1, file);
    func_0022BD98(&pSaveData->size, sizeof(int), 1, file);
    func_0022BD98(&pSaveData->tokenCount, sizeof(int), 1, file);
    func_0022BD98(&pSaveData->tokenSize, sizeof(int), 1, file);
    func_0022BD98(pTokenData, pSaveData->tokenSize, 1, file);
    func_0022BD98(pSaveData->pBaseData, pSaveData->size, 1, file);
    func_0029BFC8(name, file);
    func_0022B238(file);
    func_0022B310(pszFileName, 9);
    SaveExit(pSaveData);

    return 1;
}

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392658);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392668);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392690);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392698);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003926A0);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003926B8);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003926C0);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003926D0);

void Host_Savegame_f(void)
{
    char szComment[80];

    if (!Host_ValidSave())
        return;

    if (Cmd_Argc() != 2)
    {
        Con_DPrintf("save <savename> : save a game\n");
        return;
    }

    if (strstr(Cmd_Argv(1), D_003926F8))
    {
        Con_DPrintf(D_00392700);
        return;
    }

    func_00299420(szComment);
    SaveGameSlot(Cmd_Argv(1), szComment);
    func_00251DC0(D_00392728, 0);
}

void Host_AutoSave_f(void) {
}

void func_00299CF8(char *pSaveName, char *pSaveComment)
{
    int old;

    old = D_00399704;
    D_00399704 = 1;
    SaveGameSlot(pSaveName, pSaveComment);
    D_00399704 = old;
}

int SaveReadHeader(int pFile, GAME_HEADER *pHeader, int readGlobalState)
{
    int tag;
    int size;
    int tokenCount;
    int tokenSize;
    char *pszTokenList;
    int i;
    SAVERESTOREDATA *pSaveData;

    func_0022BE50(&tag, sizeof(int), 1, pFile);
    if (tag != SAVEGAME_HEADER)
    {
        func_0022B238(pFile);
        return 0;
    }

    func_0022BE50(&tag, sizeof(int), 1, pFile);
    if (tag != SAVEGAME_VERSION)
    {
        func_0022B238(pFile);
        return 0;
    }

    func_0022BE50(&size, sizeof(int), 1, pFile);
    func_0022BE50(&tokenCount, sizeof(int), 1, pFile);
    func_0022BE50(&tokenSize, sizeof(int), 1, pFile);

    if (D_00331B00 >= (int)(sizeof(SAVERESTOREDATA) + tokenSize + size))
    {
        pSaveData = D_00331AFC;
        D_00331B04 = 1;
    }
    else
    {
        pSaveData = Mem_DbgCalloc(sizeof(SAVERESTOREDATA) + tokenSize + size, 1, D_00392628, 1766);
        D_00331B04 = 0;
    }

    pSaveData->tableCount = 0;
    pSaveData->pTable = NULL;
    pSaveData->connectionCount = 0;

    pszTokenList = (char *)(pSaveData + 1);

    if (tokenSize > 0)
    {
        pSaveData->tokenCount = tokenCount;
        pSaveData->tokenSize = tokenSize;
        func_0022BE50(pszTokenList, tokenSize, 1, pFile);

        if (!pSaveData->pTokens)
            pSaveData->pTokens = Mem_DbgCalloc(tokenCount, sizeof(char *), D_00392628, 1786);

        for (i = 0; i < tokenCount; i++)
        {
            pSaveData->pTokens[i] = *pszTokenList ? pszTokenList : NULL;
            while (*pszTokenList++)
                ;
        }
    }

    pSaveData->size = 0;
    pSaveData->bufferSize = size;
    pSaveData->fUseLandmark = 0;
    pSaveData->time = 0;
    pSaveData->pCurrentData = pszTokenList;
    pSaveData->pBaseData = pszTokenList;

    func_0022BE50(pSaveData->pBaseData, size, 1, pFile);

    gEntityInterface.pfnSaveReadFields(pSaveData, D_00392658, pHeader, D_00332F38, 3);
    if (readGlobalState)
        gEntityInterface.pfnRestoreGlobalState(pSaveData);

    SaveExit(pSaveData);
    return 1;
}

void func_00299FB0(int pFile, char *pName)
{
    GAME_HEADER header;

    if (SaveReadHeader(pFile, &header, 0))
        strcpy(pName, header.comment);
}

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003926F8);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392700);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392728);

void Host_Loadgame_f(void)
{
    if (cmd_source != src_command)
        return;

    if (Cmd_Argc() != 2)
    {
        Con_Printf("load <savename> : load a game\n");
        return;
    }

    if (!Host_Load(Cmd_Argv(1)))
        Con_Printf("Error loading saved game\n");
}

void func_0029A060(char *pName)
{
    int old;

    old = D_00399704;
    D_00399704 = 1;
    Host_Load(pName);
    D_00399704 = old;
}

#ifdef NON_MATCHING
int Host_Load(char *pName)
{
    GAME_HEADER gameHeader;
    char name[256];
    char number[16];
    int pFile;
    int nSlot;
    int result;
    char *p;
    int i;

    func_00239488();

    if (!pName || !pName[0])
        return 0;

    if (pName[0] == '_')
    {
        p = pName + 1;
        i = 0;
        while (*p && isdigit(*p) && i < 5)
            number[i++] = *p++;
        number[i] = 0;

        nSlot = atoi(number);
        if (nSlot < 1 || nSlot > 12)
            return 0;

        sprintf(name, "%sHalf-Life-%i", Host_SaveGameDirectory(), nSlot);
    }
    else
    {
        sprintf(name, D_00392690, Host_SaveGameDirectory(), func_00243F68(pName));
    }

    if (!svs.unk_00C)
        Host_InitializeGameDLL();

    func_00244138(name, D_00392698);
    COM_FixSlashes(name);
    Con_Printf(D_00392788, name);

    result = func_0022B590(name, D_003927A8);
    if (result != 1)
        return 0;

    pFile = func_0022B118(D_003927A8, D_003927B8);
    if (!pFile)
        return 0;

    func_0022BC60(name);
    func_0029C388();

    if (SaveReadHeader(pFile, &gameHeader, 1))
    {

        SV_InactivateClients();
        func_002AC478(0);
        func_0029C108(pFile, gameHeader.mapCount);
        func_0022B238(pFile);
        func_0022B248(D_003927A8);
        Cvar_SetValue(D_003927D0, 0);
        Cvar_SetValue(D_003927E0, 0);
        sprintf(name, D_003927E8, gameHeader.mapName);
        Host_Map(false, name, gameHeader.mapName, true);
        return 1;
    }

    giStateInfo = result;
    Cbuf_AddText(D_003927C0);
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Load);
#endif

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392788);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003927A8);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003927B8);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003927C0);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003927D0);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003927E0);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003927E8);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003927F0);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392800);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392810);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392820);

SAVERESTOREDATA *SaveGamestate(void)
{
    char name[256];
    SAVE_HEADER header;
    SAVELIGHTSTYLE light;
    int i;
    char *pTableData;
    char *pTokenData;
    char *pszToken;
    int pFile;
    int dataSize;
    int tableSize;
    edict_t *pent;
    SAVERESTOREDATA *pSaveData;

    if (!gEntityInterface.pfnParmsChangeLevel)
        return NULL;

    pSaveData = SaveInit(0);
    sprintf(name, D_00392640, Host_SaveGameDirectory(), sv.name);
    COM_FixSlashes(name);
    gEntityInterface.pfnParmsChangeLevel();

    header.version = build_number();
    header.skillLevel = (int)skill.value;
    header.entityCount = pSaveData->tableCount;
    header.connectionCount = pSaveData->connectionCount;
    header.time = sv.time;
    strcpy(header.skyName, sv_skyname.string);
    header.skyColor_r = (int)cl_skycolor_r.value;
    header.skyColor_g = (int)cl_skycolor_g.value;
    header.skyColor_b = (int)cl_skycolor_b.value;
    header.skyVec_x = cl_skyvec_x.value;
    header.skyVec_y = cl_skyvec_y.value;
    header.skyVec_z = cl_skyvec_z.value;
    pSaveData->time = 0;
    strcpy(header.mapName, sv.name);

    header.lightStyleCount = 0;
    for (i = 0; i < MAX_LIGHTSTYLES; i++)
    {
        if (sv.lightstyles[i])
            header.lightStyleCount++;
    }

    gEntityInterface.pfnSaveWriteFields(pSaveData, D_003927F0, &header, D_00332F68, 13);
    pSaveData->time = header.time;

    for (i = 0; i < pSaveData->connectionCount; i++)
        gEntityInterface.pfnSaveWriteFields(pSaveData, D_00392800, &pSaveData->levelList[i], D_00333038, 4);

    for (i = 0; i < MAX_LIGHTSTYLES; i++)
    {
        if (sv.lightstyles[i])
        {
            light.index = i;
            strcpy(light.style, sv.lightstyles[i]);
            gEntityInterface.pfnSaveWriteFields(pSaveData, D_00392810, &light, D_003330C8, 2);
        }
    }

    for (i = 0; i < sv.num_edicts; i++)
    {
        pent = &sv.edicts[i];
        pSaveData->currentIndex = i;
        pSaveData->pTable[i].location = pSaveData->size;
        pSaveData->pTable[i].size = 0;

        if (pent->free)
            continue;

        gEntityInterface.pfnSave(pent, pSaveData);

        if (i > 0 && i < svs.maxclients + 1)
            pSaveData->pTable[i].flags |= FENTTABLE_PLAYER;
    }

    dataSize = pSaveData->size;
    pTableData = pSaveData->pCurrentData;

    for (i = 0; i < sv.num_edicts; i++)
        gEntityInterface.pfnSaveWriteFields(pSaveData, D_00392820, &pSaveData->pTable[i], D_00333078, 5);

    tableSize = pSaveData->size - dataSize;
    pTokenData = pSaveData->pCurrentData;

    if (pSaveData->pTokens)
    {
        for (i = 0; i < pSaveData->tokenCount; i++)
        {
            if (pSaveData->pTokens[i])
            {
                pszToken = pSaveData->pTokens[i];
                while ((*pSaveData->pCurrentData++ = *pszToken++) != 0)
                    ;
            }
            else
            {
                *pSaveData->pCurrentData++ = 0;
            }
        }
    }

    pSaveData->tokenSize = pSaveData->pCurrentData - pTokenData;

    pFile = func_0022B118(name, D_003926D0);
    if (!pFile)
    {
        Con_Printf("Unable to open save game file %s.", name);
        return NULL;
    }

    i = (('V' << 24) | ('L' << 16) | ('A' << 8) | 'V');
    func_0022BD98(&i, sizeof(int), 1, pFile);
    i = SAVEGAME_VERSION;
    func_0022BD98(&i, sizeof(int), 1, pFile);
    func_0022BD98(&pSaveData->size, sizeof(int), 1, pFile);
    func_0022BD98(&pSaveData->tableCount, sizeof(int), 1, pFile);
    func_0022BD98(&pSaveData->tokenCount, sizeof(int), 1, pFile);
    func_0022BD98(&pSaveData->tokenSize, sizeof(int), 1, pFile);
    func_0022BD98(pTokenData, pSaveData->tokenSize, 1, pFile);
    func_0022BD98(pTableData, tableSize, 1, pFile);
    func_0022BD98(pSaveData->pBaseData, dataSize, 1, pFile);
    func_0022B238(pFile);

    EntityPatchWrite(pSaveData, sv.name);

    sprintf(name, D_00392850, Host_SaveGameDirectory(), sv.name);
    COM_FixSlashes(name);
    CL_Save(name);

    return pSaveData;
}

void CL_Save(char *name)
{
    savedecal_t decalList[1024];
    int tag;
    int number;
    int i;
    int pFile;

    number = func_002A50B8(decalList, 0);

    Mem_PushTag();
    Mem_SetTag(2);

    pFile = func_0022B118(name, D_003926D0);
    if (pFile)
    {
        tag = (('V' << 24) | ('L' << 16) | ('A' << 8) | 'V');
        func_0022BD98(&tag, sizeof(int), 1, pFile);
        tag = SAVEGAME_VERSION;
        func_0022BD98(&tag, sizeof(int), 1, pFile);
        func_0022BD98(&number, sizeof(int), 1, pFile);

        for (i = 0; i < number; i++)
        {
            func_0022BD98(decalList[i].name, 1, 16, pFile);
            func_0022BD98(&decalList[i].entityIndex, sizeof(short), 1, pFile);
            func_0022BD98(&decalList[i].depth, sizeof(byte), 1, pFile);
            func_0022BD98(&decalList[i].flags, sizeof(byte), 1, pFile);
            func_0022BD98(decalList[i].position, sizeof(vec3_t), 1, pFile);
        }

        func_0022B238(pFile);
    }

    Mem_PopTag();
    func_0022BBC0(name);
}

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392850);

void EntityInit(edict_t *pEdict, string_t className)
{
    ENTITYINIT pEntityInit;

    if (!className)
        Sys_Error("Bad class!!\n");

    ReleaseEntityDLLFields(pEdict);
    InitEntityDLLFields(pEdict);
    pEdict->v.classname = className;

    pEntityInit = GetEntityInit(pr_strings + className);
    if (pEntityInit)
        pEntityInit(&pEdict->v);
}

SAVERESTOREDATA *LoadSaveData(char *level)
{
    char name[260];
    int i;
    int size;
    int tableCount;
    int tokenCount;
    int tokenSize;
    int pFile;
    int result;
    char *pszTokenList;
    SAVERESTOREDATA *pSaveData;

    sprintf(name, D_00392640, Host_SaveGameDirectory(), level);
    COM_FixSlashes(name);
    Con_Printf(D_00392788, name);

    pFile = func_0022B118(name, D_003927B8);
    if (!pFile)
    {
        func_00217678(Host_SaveGameDirectory(), level);
        pFile = func_0022B118(name, D_003927B8);
        if (!pFile)
        {
            Con_Printf("ERROR: couldn't open.\n");
            return NULL;
        }
    }

    func_0022B238(pFile);

    result = func_0022B590(name, D_003927A8);
    if (result != 1)
    {
        Con_Printf("ERROR: couldn't open.\n");
        return NULL;
    }

    pFile = func_0022B118(D_003927A8, D_003927B8);
    if (!pFile)
    {
        Con_Printf("ERROR: couldn't open.\n");
        return NULL;
    }

    func_0022BE50(&i, sizeof(int), 1, pFile);
    if (i != (('V' << 24) | ('L' << 16) | ('A' << 8) | 'V'))
        return NULL;

    func_0022BE50(&i, sizeof(int), 1, pFile);
    if (i > SAVEGAME_VERSION)
        return NULL;

    func_0022BE50(&size, sizeof(int), 1, pFile);
    func_0022BE50(&tableCount, sizeof(int), 1, pFile);
    func_0022BE50(&tokenCount, sizeof(int), 1, pFile);
    func_0022BE50(&tokenSize, sizeof(int), 1, pFile);

    if (D_00331B00 >= (int)(tokenSize + sizeof(SAVERESTOREDATA) + sizeof(ENTITYTABLE) * tableCount + size))
    {
        D_00331B04 = result;
        pSaveData = D_00331AFC;
    }
    else
    {
        pSaveData = Mem_DbgCalloc(tokenSize + sizeof(SAVERESTOREDATA) + sizeof(ENTITYTABLE) * tableCount + size, 1, D_00392628, 2258);
        D_00331B04 = 0;
    }

    pSaveData->tableCount = tableCount;
    pSaveData->tokenCount = tokenCount;
    pSaveData->tokenSize = tokenSize;
    strcpy(pSaveData->szCurrentMapName, level);

    pszTokenList = (char *)(pSaveData + 1);

    if (tokenSize > 0)
    {
        func_0022BE50(pszTokenList, pSaveData->tokenSize, 1, pFile);

        if (!pSaveData->pTokens)
            pSaveData->pTokens = Mem_DbgCalloc(tokenCount, sizeof(char *), D_00392628, 2276);

        for (i = 0; i < tokenCount; i++)
        {
            pSaveData->pTokens[i] = *pszTokenList ? pszTokenList : NULL;
            while (*pszTokenList++)
                ;
        }
    }

    pSaveData->pTable = (ENTITYTABLE *)pszTokenList;
    pSaveData->connectionCount = 0;
    pSaveData->size = 0;
    pSaveData->pBaseData = (char *)(pSaveData->pTable + pSaveData->tableCount);
    pSaveData->pCurrentData = pSaveData->pBaseData;
    pSaveData->bufferSize = size;
    pSaveData->fUseLandmark = 1;
    pSaveData->time = 0;
    VectorCopy(vec3_origin, pSaveData->vecLandmarkOffset);
    gGlobalVariables.pSaveData = pSaveData;

    func_0022BE50(pSaveData->pBaseData, size, 1, pFile);
    func_0022B238(pFile);
    func_0022B248(D_003927A8);

    return pSaveData;
}

void ParseSaveTables(SAVERESTOREDATA *pSaveData, SAVE_HEADER *pHeader, int updateGlobals)
{
    SAVELIGHTSTYLE light;
    edict_t *pNull;
    int i;

    for (i = 0; i < pSaveData->tableCount; i++)
    {
        gEntityInterface.pfnSaveReadFields(pSaveData, D_00392820, &pSaveData->pTable[i], D_00333078, 5);
        pNull = NULL;
        Q_memcpy(&pSaveData->pTable[i].pent, &pNull, sizeof(edict_t *));
    }

    pSaveData->size = 0;
    pSaveData->pBaseData = pSaveData->pCurrentData;

    gEntityInterface.pfnSaveReadFields(pSaveData, D_003927F0, pHeader, D_00332F68, 13);

    pSaveData->connectionCount = pHeader->connectionCount;
    pSaveData->time = pHeader->time;
    pSaveData->fUseLandmark = 1;
    VectorCopy(vec3_origin, pSaveData->vecLandmarkOffset);

    for (i = 0; i < pSaveData->connectionCount; i++)
        gEntityInterface.pfnSaveReadFields(pSaveData, D_00392800, &pSaveData->levelList[i], D_00333038, 4);

    if (updateGlobals)
    {
        for (i = MAX_LIGHTSTYLES - 1; i >= 0; i--)
            sv.lightstyles[i] = NULL;
    }

    for (i = 0; i < pHeader->lightStyleCount; i++)
    {
        gEntityInterface.pfnSaveReadFields(pSaveData, D_00392810, &light, D_003330C8, 2);
        if (updateGlobals)
        {
            sv.lightstyles[light.index] = Hunk_AllocName(strlen(light.style) + 1, "LightStyle");
            strcpy(sv.lightstyles[light.index], light.style);
        }
    }
}

void EntityPatchWrite(SAVERESTOREDATA *pSaveData, char *level)
{
    char name[260];
    ENTITYTABLE entity;
    int size;
    int i;
    int pFile;

    sprintf(name, D_00392898, Host_SaveGameDirectory(), level);
    COM_FixSlashes(name);

    Mem_PushTag();
    Mem_SetTag(2);

    pFile = func_0022B118(name, D_003926D0);
    if (pFile)
    {
        size = 0;
        for (i = 0; i < pSaveData->tableCount; i++)
        {
            entity = pSaveData->pTable[i];
            if (entity.flags & FENTTABLE_REMOVED)
                size++;
        }

        func_0022BD98(&size, sizeof(int), 1, pFile);

        for (i = 0; i < pSaveData->tableCount; i++)
        {
            entity = pSaveData->pTable[i];
            if (entity.flags & FENTTABLE_REMOVED)
                func_0022BD98(&i, sizeof(int), 1, pFile);
        }

        func_0022B238(pFile);
    }

    Mem_PopTag();
    func_0022BBC0(name);
}

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_0029B328);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392898);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_0029B488);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_0029B818);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_0029B8A8);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_0029B970);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392920);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_0029BA10);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_0029BDB8);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_0029BFC8);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_0029C108);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_0029C220);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392988);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392990);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_0029C2B8);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_0029C330);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_0029C388);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Changelevel2_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Version_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_FullInfo_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_SetInfo_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_0029C948);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Say_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Say_Team_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Tell_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Kill_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Pause_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_PreSpawn_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Spawn_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Begin_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Kick_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_0029D8F0);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Viewmodel_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Viewframe_f);

void func_0029DAB8(void) {
}

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Viewnext_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Viewprev_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Interp_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Nextdl_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_0029DDB8);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_AllowDownload_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_AllowUpload_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_0029DF60);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Download_f);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392E30);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_0029E2D0);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_WC_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_SoundFade_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_KillServer_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Crc_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Logos_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Protocol_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_GetSv_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_BGetSv_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_BGetMod_f);

void Host_PingSv_f(void) {
}

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Notify_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_GetCertificate_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_InitCommands);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393230);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393240);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393248);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393250);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393258);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393260);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393268);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393270);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393280);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393288);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393290);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003932A0);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003932B0);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003932B8);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003932C8);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003932D0);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003932D8);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003932E8);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003932F0);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003932F8);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393300);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393308);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393310);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393320);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393328);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393330);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393338);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393340);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393348);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393358);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393360);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393370);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393378);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393388);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393390);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393398);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003933A8);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003933B8);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003933C8);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003933D8);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003933E0);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003933E8);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003933F8);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393408);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393410);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393420);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393430);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393438);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393450);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393460);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393470);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393480);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00393488);
