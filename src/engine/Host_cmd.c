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

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Map_f);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392470);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Maps_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Changelevel_f);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003924F8);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392520);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_00299008);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Restart_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Restartdecay_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Reload_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Reconnect_f);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392558);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Connect_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_00299400);

void func_00299420(void) {
}

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_00299428);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_00299568);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_00299638);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_00299828);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392628);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392640);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_00299890);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392658);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392668);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392690);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392698);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003926A0);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003926B8);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003926C0);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003926D0);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Savegame_f);

void Host_AutoSave_f(void) {
}

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_00299CF8);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_00299D38);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_00299FB0);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_003926F8);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392700);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392728);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Loadgame_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_0029A060);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_0029A0A0);

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

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_0029A340);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_0029A958);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392850);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_0029AB00);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_0029AB70);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_0029AEE8);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_0029B150);

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
