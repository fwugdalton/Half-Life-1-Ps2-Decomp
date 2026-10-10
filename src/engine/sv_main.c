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
void Log_Open(void);
void Log_Close(void);

void SV_ServerLog_f(void)
{
    if (Cmd_Argc() != 2)
    {
        Con_Printf("usage:  log < on | off >\n");

        if (svs.log.active)
            Con_Printf("currently logging\n");
        else
            Con_Printf("not currently logging\n");
        return;
    }

    if (!strcasecmp(Cmd_Argv(1), "off"))
    {
        if (svs.log.active)
        {
            if (svs.log.file)
                Log_Close();
            Con_Printf("Server logging disabled.\n");
            svs.log.active = false;
        }
    }
    else if (!strcasecmp(Cmd_Argv(1), "on"))
    {
        svs.log.active = true;
        Log_Open();
    }
    else
    {
        Con_Printf("log:  unknown parameter %s, 'on' and 'off' are valid\n", Cmd_Argv(1));
    }
}
void SV_User_f(void)
{
    int i;
    int uid;
    client_t *cl;

    if (!sv.active)
    {
        Con_Printf("Can't 'user', not running a server\n");
        return;
    }

    if (Cmd_Argc() != 2)
    {
        Con_Printf("Usage: user <username / userid>\n");
        return;
    }

    uid = atoi(Cmd_Argv(1));

    for (i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
    {
        if (!cl->active && !cl->spawned && !cl->connected)
            continue;

        if (cl->fakeclient)
            continue;

        if (!cl->name[0])
            continue;

        if (cl->userid == uid || !strcmp(cl->name, Cmd_Argv(1)))
        {
            Info_Print(cl->userinfo);
            return;
        }
    }

    Con_Printf("User not in server.\n");
}
void SV_Users_f(void)
{
    int i;
    int c;
    client_t *cl;

    if (!sv.active)
    {
        Con_Printf("Can't 'users', not running a server\n");
        return;
    }

    Con_Printf("userid : uniqueid : name\n");
    Con_Printf("------ : ---------: ----\n");

    c = 0;
    for (i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
    {
        if (!cl->active && !cl->spawned && !cl->connected)
            continue;

        if (cl->fakeclient)
            continue;

        if (cl->name[0])
        {
            Con_Printf("%6i : %u : %s\n", cl->userid, cl->WONid, cl->name);
            c++;
        }
    }

    Con_Printf("%i users\n", c);
}
void MSG_WriteCoord(sizebuf_t *sb, float f);
void MSG_WriteChar(sizebuf_t *sb, int c);
qboolean SV_BuildSoundMsg(edict_t *entity, int channel, const char *sample, int volume, float attenuation, int fFlags, int pitch, const float *origin, sizebuf_t *buffer);
void SV_Multicast(vec3_t origin, int to, qboolean reliable);
int NUM_FOR_EDICT(edict_t *e);
void MSG_StartBitWriting(sizebuf_t *buf);
void MSG_EndBitWriting(sizebuf_t *buf);
void MSG_WriteBitsUChar(byte *data, int numbits);
void MSG_WriteBitsUShort(unsigned short *data, int numbits);
void func_00242BC0(short *data, int numbits);
int SV_LookupSoundIndex(const char *sample);
qboolean Host_IsSinglePlayerGame(void);

extern int sv_playermodel;

#define svc_particle 18
#define CHAN_STATIC 6
#define SND_STOP (1 << 5)
#define MSG_FL_ALL 0
#define MSG_FL_PAS 2

#define svc_sound 6
#define SND_VOLUME (1 << 0)
#define SND_ATTENUATION (1 << 1)
#define SND_LARGE_INDEX (1 << 2)
#define SND_PITCH (1 << 3)
#define SND_SENTENCE (1 << 4)
#define DEFAULT_SOUND_PACKET_VOLUME 255
#define DEFAULT_SOUND_PACKET_ATTENUATION 1.0f
#define DEFAULT_SOUND_PACKET_PITCH 100
#define CVOXFILESENTENCEMAX 1536

void SV_CountPlayers(int *clients, int *spectators)
{
    int i;
    client_t *cl;

    *clients = 0;
    if (spectators)
        *spectators = 0;

    for (i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
    {
        if (cl->active || cl->spawned || cl->connected)
        {
            (*clients)++;
            if (cl->spectator && spectators)
                (*spectators)++;
        }
    }
}
void SV_FindModelNumbers(void)
{
    int i;

    sv_playermodel = -1;

    for (i = 0; i < 512; i++)
    {
        if (!sv.model_precache[i])
            break;

        /* use case-sensitive names to increase performance */
        if (!Q_strcasecmp(sv.model_precache[i], "models/player.dol"))
            sv_playermodel = i;
    }
}
void SV_StartParticle(const float *org, const float *dir, int color, int count)
{
    int i, v;

    if (sv.datagram.cursize > 4000 - 16)
        return;

    MSG_WriteByte(&sv.datagram, svc_particle);
    MSG_WriteCoord(&sv.datagram, org[0]);
    MSG_WriteCoord(&sv.datagram, org[1]);
    MSG_WriteCoord(&sv.datagram, org[2]);
    for (i = 0; i < 3; i++)
    {
        v = dir[i] * 16;
        if (v > 127)
            v = 127;
        else if (v < -128)
            v = -128;
        MSG_WriteChar(&sv.datagram, v);
    }
    MSG_WriteByte(&sv.datagram, count);
    MSG_WriteByte(&sv.datagram, color);
}
void SV_StartSound(edict_t *entity, int channel, const char *sample, int volume, float attenuation, int fFlags, int pitch)
{
    int i;
    int sendPAS;
    vec3_t origin;

    for (i = 0; i < 3; i++)
        origin[i] = entity->v.origin[i] + (entity->v.mins[i] + entity->v.maxs[i]) * 0.5f;

    if (SV_BuildSoundMsg(entity, channel, sample, volume, attenuation, fFlags, pitch, origin, &sv.multicast))
    {
        if (channel == CHAN_STATIC || (fFlags & SND_STOP))
            sendPAS = false;
        else
            sendPAS = true;
        if (sendPAS)
        {
            SV_Multicast(origin, MSG_FL_PAS, false);
        }
        else
        {
            if (Host_IsSinglePlayerGame())
                SV_Multicast(origin, MSG_FL_ALL, false);
            else
                SV_Multicast(origin, MSG_FL_ALL, true);
        }
    }
}
qboolean SV_BuildSoundMsg(edict_t *entity, int channel, const char *sample, int volume, float attenuation, int fFlags, int pitch, const float *origin, sizebuf_t *buffer)
{
    int i;
    int sound_num;
    int field_mask;
    int ent;
    unsigned short sval;
    byte bval;
    short coord;

    if (volume < 0 || volume > 255)
        Sys_Error("SV_StartSound: volume = %i", volume);
    if (attenuation < 0 || attenuation > 4)
        Sys_Error("SV_StartSound: attenuation = %f", attenuation);
    if (channel < 0 || channel > 7)
        Sys_Error("SV_StartSound: channel = %i", channel);
    if (pitch < 0 || pitch > 255)
        Sys_Error("SV_StartSound: pitch = %i", pitch);

    if (sample && *sample == '!')
    {
        fFlags |= SND_SENTENCE;
        sound_num = atoi(sample + 1);
        if (sound_num >= CVOXFILESENTENCEMAX)
        {
            Con_Printf("invalid sentence number: %s", sample + 1);
            return false;
        }
    }
    else
    {
        sound_num = SV_LookupSoundIndex(sample);
        if (!sound_num || !sv.sound_precache[sound_num])
        {
            Con_Printf("SV_StartSound: %s not precached (%d)\n", sample, sound_num);
            return false;
        }
    }

    ent = NUM_FOR_EDICT(entity);

    field_mask = fFlags;
    if (volume != DEFAULT_SOUND_PACKET_VOLUME)
        field_mask |= SND_VOLUME;
    if (attenuation != DEFAULT_SOUND_PACKET_ATTENUATION)
        field_mask |= SND_ATTENUATION;
    if (pitch != DEFAULT_SOUND_PACKET_PITCH)
        field_mask |= SND_PITCH;
    if (sound_num > 255)
        field_mask |= SND_LARGE_INDEX;

    MSG_WriteByte(buffer, svc_sound);
    MSG_StartBitWriting(buffer);

    sval = field_mask;
    MSG_WriteBitsUShort(&sval, 9);

    if (field_mask & SND_VOLUME)
    {
        bval = volume;
        MSG_WriteBitsUChar(&bval, 8);
    }
    if (field_mask & SND_ATTENUATION)
    {
        bval = (int)(attenuation * 64);
        MSG_WriteBitsUChar(&bval, 8);
    }

    bval = channel;
    MSG_WriteBitsUChar(&bval, 3);

    sval = ent;
    MSG_WriteBitsUShort(&sval, 10);

    if (sound_num > 255)
    {
        sval = sound_num;
        MSG_WriteBitsUShort(&sval, 16);
    }
    else
    {
        bval = sound_num;
        MSG_WriteBitsUChar(&bval, 8);
    }

    for (i = 0; i < 3; i++)
    {
        coord = (int)(origin[i] * 8);
        func_00242BC0(&coord, 16);
    }

    if (field_mask & SND_PITCH)
    {
        bval = pitch;
        MSG_WriteBitsUChar(&bval, 8);
    }

    MSG_EndBitWriting(buffer);
    return true;
}
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", func_0027B640);
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", SV_LookupSoundIndex);
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
