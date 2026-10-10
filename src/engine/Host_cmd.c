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

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_00297D10);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Quit_f);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392178);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_00297F08);

INCLUDE_RODATA("asm/nonmatchings/engine/Host_cmd", D_00392278);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Status_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_God_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Notarget_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_00298570);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Noclip_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Fly_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_002988B0);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", Host_Ping_f);

INCLUDE_ASM("asm/nonmatchings/engine/Host_cmd", func_00298A18);

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
