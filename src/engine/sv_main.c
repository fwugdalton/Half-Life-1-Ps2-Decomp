#include "common.h"

#include "common.h"
#include "engine/mathlib.h"
#include "engine/model.h"
#include "engine/server.h"
#include "engine/eiface.h"
#include "engine/cvar.h"
#include "engine/cmd.h"
#include "engine/progdefs.h"
#include "engine/pmove.h"

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
        ".align 3\n"
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
void MSG_WriteBitsShort(short *data, int numbits);
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
        MSG_WriteBitsShort(&coord, 16);
    }

    if (field_mask & SND_PITCH)
    {
        bval = pitch;
        MSG_WriteBitsUChar(&bval, 8);
    }

    MSG_EndBitWriting(buffer);
    return true;
}
extern const char _ctype_[];
#define isupper(c) ((_ctype_ + 1)[(int)(c)] & 1)

extern int hashstrings_collisions;

#define MAX_SOUNDS 512
#define MAX_SOUNDS_HASHLOOKUP_SIZE (MAX_SOUNDS * 2 - 1)

void SV_BuildHashedSoundLookupTable(void);
void SV_AddSampleToHashedLookupTable(const char *pszSample, int iSampleIndex);

static __inline__ int my_tolower(int c)
{
    return isupper(c) ? c - 'A' + 'a' : c;
}

unsigned int SV_HashString(const char *string, int iBounds)
{
    unsigned int hash;
    const char *cc;

    hash = 0;
    for (cc = string; *cc; cc++)
        hash = 2 * hash + my_tolower(*cc);

    return hash % iBounds;
}
int SV_LookupSoundIndex(const char *sample)
{
    int index;
    int starting_index;

    starting_index = SV_HashString(sample, MAX_SOUNDS_HASHLOOKUP_SIZE);
    index = starting_index;

    if (!sv.sound_precache_hashedlookup_built)
    {
        if (sv.state != 1)
        {
            SV_BuildHashedSoundLookupTable();
        }
        else
        {
            for (index = 1; index < MAX_SOUNDS && sv.sound_precache[index]; index++)
            {
                if (!Q_strcasecmp(sample, sv.sound_precache[index]))
                    return index;
            }
            return 0;
        }
    }

    while (sv.sound_precache_hashedlookup[index])
    {
        if (!Q_strcasecmp(sample, sv.sound_precache[sv.sound_precache_hashedlookup[index]]))
            return sv.sound_precache_hashedlookup[index];

        index++;
        if (index >= MAX_SOUNDS_HASHLOOKUP_SIZE)
            index = 0;
        if (index == starting_index)
            return 0;
    }

    return 0;
}
void SV_BuildHashedSoundLookupTable(void)
{
    int i;

    memset(sv.sound_precache_hashedlookup, 0, sizeof(sv.sound_precache_hashedlookup));

    for (i = 0; i < MAX_SOUNDS; i++)
    {
        if (!sv.sound_precache[i])
            break;

        SV_AddSampleToHashedLookupTable(sv.sound_precache[i], i);
    }

    sv.sound_precache_hashedlookup_built = true;
}
void SV_AddSampleToHashedLookupTable(const char *pszSample, int iSampleIndex)
{
    int starting_index;
    int index;

    starting_index = SV_HashString(pszSample, MAX_SOUNDS_HASHLOOKUP_SIZE);
    index = starting_index;
    while (sv.sound_precache_hashedlookup[index])
    {
        index++;
        hashstrings_collisions++;

        if (index >= MAX_SOUNDS_HASHLOOKUP_SIZE)
            index = 0;

        if (index == starting_index)
            Sys_Error("SV_AddSampleToHashedLookupTable: NO FREE SLOTS IN SOUND LOOKUP TABLE");
    }

    sv.sound_precache_hashedlookup[index] = iSampleIndex;
}
extern int cl_maxclients;
byte *CM_LeafPVS(int leafnum);
byte *CM_LeafPAS(int leafnum);
int SV_PointLeafnum(vec_t *p);

#define MSG_FL_PVS 1

qboolean Host_IsSinglePlayerGame(void)
{
    if (!sv.active)
        return cl_maxclients == 1;
    else
        return svs.maxclients == 1;
}
#ifdef NON_MATCHING
qboolean SV_ValidClientMulticast(client_t *client, int soundLeaf, int to)
{
    byte *mask;
    int bitNumber;

    mask = NULL;

    if (Host_IsSinglePlayerGame())
        return true;

    switch (to)
    {
    case MSG_FL_ALL:
        break;

    case MSG_FL_PAS:
        mask = CM_LeafPAS(soundLeaf);
        break;

    case MSG_FL_PVS:
        mask = CM_LeafPVS(soundLeaf);
        break;

    default:
        Con_Printf("MULTICAST: Error %d!\n", to);
        return false;
    }

    if (!mask)
        return true;

    bitNumber = SV_PointLeafnum(client->edict->v.origin);
    if (mask[(bitNumber - 1) >> 3] & (1 << ((bitNumber - 1) & 7)))
        return true;

    return false;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", SV_ValidClientMulticast);
#endif
void SV_Multicast(vec3_t origin, int to, qboolean reliable)
{
    client_t *client;
    sizebuf_t *msg;
    int j;

    for (j = 0, client = svs.clients; j < svs.maxclients; j++, client++)
    {
        if (!client->active)
            continue;

        if (reliable)
            msg = &client->netchan.message;
        else
            msg = &client->datagram;

        if (sv.multicast.cursize < msg->maxsize - msg->cursize)
            SZ_Write(msg, sv.multicast.data, sv.multicast.cursize);
    }

    SZ_Clear(&sv.multicast);
}
void MSG_WriteFloat(sizebuf_t *sb, float f);
void MSG_WriteString(sizebuf_t *sb, const char *s);
void SV_SetMoveVars(void);

extern cvar_t sv_friction, sv_stopspeed, sv_gravity, sv_maxvelocity, sv_stepsize, sv_bounce;
extern cvar_t sv_spectatormaxspeed, sv_airaccelerate, sv_wateraccelerate, sv_waterfriction;
extern cvar_t sv_zmax, sv_wateramp, sv_skyname, sv_edgefriction, sv_maxspeed, sv_accelerate;

#define svc_newmovevars 46

void SV_WriteMovevarsToClient(sizebuf_t *message)
{
    MSG_WriteByte(message, svc_newmovevars);
    MSG_WriteFloat(message, movevars.gravity);
    MSG_WriteFloat(message, movevars.stopspeed);
    MSG_WriteFloat(message, movevars.maxspeed);
    MSG_WriteFloat(message, movevars.spectatormaxspeed);
    MSG_WriteFloat(message, movevars.accelerate);
    MSG_WriteFloat(message, movevars.airaccelerate);
    MSG_WriteFloat(message, movevars.wateraccelerate);
    MSG_WriteFloat(message, movevars.friction);
    MSG_WriteFloat(message, movevars.edgefriction);
    MSG_WriteFloat(message, movevars.waterfriction);
    MSG_WriteFloat(message, movevars.entgravity);
    MSG_WriteFloat(message, movevars.bounce);
    MSG_WriteFloat(message, movevars.stepsize);
    MSG_WriteFloat(message, movevars.maxvelocity);
    MSG_WriteFloat(message, movevars.zmax);
    MSG_WriteFloat(message, movevars.waveHeight);
    MSG_WriteString(message, movevars.skyName);
}
void SV_QueryMovevarsChanged(void)
{
    int i;
    client_t *cl;

    if (sv_maxspeed.value != movevars.maxspeed ||
        sv_gravity.value != movevars.gravity ||
        sv_stopspeed.value != movevars.stopspeed ||
        sv_spectatormaxspeed.value != movevars.spectatormaxspeed ||
        sv_accelerate.value != movevars.accelerate ||
        sv_airaccelerate.value != movevars.airaccelerate ||
        sv_wateraccelerate.value != movevars.wateraccelerate ||
        sv_friction.value != movevars.friction ||
        sv_edgefriction.value != movevars.edgefriction ||
        sv_waterfriction.value != movevars.waterfriction ||
        movevars.entgravity != 1.0f ||
        sv_bounce.value != movevars.bounce ||
        sv_stepsize.value != movevars.stepsize ||
        sv_maxvelocity.value != movevars.maxvelocity ||
        sv_zmax.value != movevars.zmax ||
        sv_wateramp.value != movevars.waveHeight ||
        strcmp(sv_skyname.string, movevars.skyName))
    {
        SV_SetMoveVars();

        for (i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
        {
            if (cl->fakeclient)
                continue;

            if (!cl->active && !cl->spawned && !cl->connected)
                continue;

            SV_WriteMovevarsToClient(&cl->netchan.message);
        }
    }
}
void func_0027BF20(void)
{
}
extern cvar_t developer, coop, deathmatch;
extern int protocol_version;
extern char com_gamedir[];
extern char *pr_strings;
extern client_t *host_client;
int build_number(void);
void MSG_WriteLong(sizebuf_t *sb, int c);
void MSG_WriteShort(sizebuf_t *sb, int c);
void COM_FileBase(char *in, char *out);

#define svc_print 8
#define svc_serverinfo 11
#define svc_setview 5
#define svc_cdtrack 32

void SV_SendServerinfo(client_t *client)
{
    char message[2048];
    int playernum;

    if (developer.value || svs.maxclients > 1)
    {
        MSG_WriteByte(&client->netchan.message, svc_print);
        sprintf(message, "%c\nBUILD %d SERVER (%i CRC)\nServer # %i\n", 2, build_number(), 0, svs.spawncount);
        MSG_WriteString(&client->netchan.message, message);
    }

    MSG_WriteByte(&client->netchan.message, svc_serverinfo);
    MSG_WriteLong(&client->netchan.message, protocol_version);
    MSG_WriteLong(&client->netchan.message, svs.spawncount);
    MSG_WriteLong(&client->netchan.message, sv.worldmapCRC);
    MSG_WriteLong(&client->netchan.message, sv.clientdllmd5);
    MSG_WriteByte(&client->netchan.message, svs.maxclients);

    playernum = NUM_FOR_EDICT(client->edict) - 1;
    if (client->spectator)
        playernum |= 128;
    MSG_WriteByte(&client->netchan.message, playernum);

    if (coop.value == 0 && deathmatch.value != 0)
        MSG_WriteByte(&client->netchan.message, 1);
    else
        MSG_WriteByte(&client->netchan.message, 0);

    COM_FileBase(com_gamedir, message);
    MSG_WriteString(&client->netchan.message, message);
    MSG_WriteString(&client->netchan.message, sv.modelname);

    sprintf(message, "%s", pr_strings + sv.edicts->v.message);
    MSG_WriteString(&client->netchan.message, message);

    SV_WriteMovevarsToClient(&host_client->netchan.message);

    MSG_WriteByte(&client->netchan.message, svc_cdtrack);
    MSG_WriteByte(&client->netchan.message, gGlobalVariables.cdAudioTrack);
    MSG_WriteByte(&client->netchan.message, gGlobalVariables.cdAudioTrack);

    MSG_WriteByte(&client->netchan.message, svc_setview);
    MSG_WriteShort(&client->netchan.message, NUM_FOR_EDICT(client->edict));

    MSG_WriteByte(&client->netchan.message, 48);
    MSG_WriteLong(&client->netchan.message, svs.spawncount);
    MSG_WriteLong(&client->netchan.message, 0);

    client->spawned = false;
    client->connected = true;
    client->unk_73D4 = true;

    func_0027BF20();
}
typedef struct saverestore_s
{
    char *pBaseData;            /* 0x00 */
    char *pCurrentData;         /* 0x04 */
    int size;                   /* 0x08 */
    int bufferSize;             /* 0x0C */
    int tokenSize;              /* 0x10 */
    int tokenCount;             /* 0x14 */
    char **pTokens;             /* 0x18 */
    int currentIndex;           /* 0x1C */
} SAVERESTOREDATA;

extern int cmd_source;
extern float realtime;
extern UserMsg *sv_gpUserMsgs;
extern UserMsg *sv_gpNewUserMsgs;
extern cvar_t sv_netsize;
void SV_SendUserReg(sizebuf_t *msg);
void SV_DropClient(client_t *cl, qboolean crash);
char *va(char *format, ...);
void SV_SpectatorSpawn(void);

#define svc_stufftext 9

void SV_New_f(void)
{
    edict_t *ent;
    int sinfo, decals, usr;
    UserMsg *pTemp;
    char szRejectReason[128];
    char szAddress[256];
    char szName[64];

    if (cmd_source == 1)
        return;

    if (host_client->spawned && !host_client->active)
        return;

    ent = host_client->edict;

    host_client->connected = true;
    host_client->connection_started = realtime;

    SZ_Clear(&host_client->netchan.message);

    /* manda os dados do servidor */
    SV_SendServerinfo(host_client);

    sinfo = host_client->netchan.message.cursize;
    if (sv_netsize.value)
        Con_DPrintf("SINFO=%i\n", sinfo);

    decals = host_client->netchan.message.cursize;
    if (sv_netsize.value)
        Con_DPrintf("DECALS=%i\n", decals - sinfo);

    host_client->sendinfo = true;
    if (sv_gpUserMsgs)
    {
        pTemp = sv_gpNewUserMsgs;
        sv_gpNewUserMsgs = sv_gpUserMsgs;
        SV_SendUserReg(&host_client->netchan.message);
        sv_gpNewUserMsgs = pTemp;
    }

    usr = host_client->netchan.message.cursize;
    if (sv_netsize.value)
        Con_DPrintf("USR=%i\n", usr - decals);

    /* se já estava no jogo (mudança de nível), avisa a dll que saiu */
    if ((host_client->active || host_client->spawned) && host_client->edict)
    {
        if (!host_client->edict->free)
        {
            if (!host_client->edict->v.pContainingEntity)
            {
                Con_DPrintf("SV_New_f:Disconnect called with invalid entity\n");
            }
            else if (host_client->spectator)
            {
                gEntityInterface.pfnSpectatorDisconnect(host_client->edict);
            }
            else
            {
                gEntityInterface.pfnClientDisconnect(host_client->edict);
            }
        }
    }

    if (!host_client->spectator)
    {
        sprintf(szName, host_client->name);
        sprintf(szAddress, NET_AdrToString(host_client->netchan.remote_address));
        sprintf(szRejectReason, "Connection rejected by game\n");

        if (!gEntityInterface.pfnClientConnect(ent, szName, szAddress, szRejectReason))
        {
            MSG_WriteByte(&host_client->netchan.message, svc_stufftext);
            MSG_WriteString(&host_client->netchan.message, va("echo %s\n", szRejectReason));
            SV_DropClient(host_client, false);
            return;
        }
    }
    else
    {
        SV_SpectatorSpawn();
        gGlobalVariables.time = sv.time;
        gEntityInterface.pfnParmsNewLevel();
        gEntityInterface.pfnSpectatorConnect(ent);
    }

    if (sv_netsize.value)
        Con_DPrintf("CLSIZE = %i\n", host_client->netchan.message.cursize - usr);

    MSG_WriteByte(&host_client->netchan.message, svc_stufftext);
    MSG_WriteString(&host_client->netchan.message, va("fullserverinfo \"%s\"\n", Info_Serverinfo()));
}
extern sizebuf_t net_message;
extern edict_t *sv_player;
extern vec3_t vec3_origin;
void Cmd_ForwardToServer(void);
void SV_ClientPrintf(char *fmt, ...);
void NET_SendPacket(int sock, int length, void *data, netadr_t to, int unused);
edict_t *EDICT_NUM(int n);

#define S2C_CONNREJECT '9'
#define S2C_BADPASSWORD '8'
#define NS_SERVER 2

void SV_Track_f(void)
{
    int i;

    if (cmd_source == 1)
    {
        Cmd_ForwardToServer();
        return;
    }

    if (Cmd_Argc() != 2)
    {
        host_client->spec_track = 0;
        return;
    }

    i = atoi(Cmd_Argv(1));
    if (i < 0 || i >= svs.maxclients || !svs.clients[i].active || svs.clients[i].spectator)
    {
        SV_ClientPrintf("Invalid client to track\n");
        host_client->spec_track = 0;
        return;
    }

    host_client->spec_track = i + 1;
}
void SV_RejectConnection(netadr_t *adr, char *reason)
{
    SZ_Clear(&net_message);
    MSG_WriteLong(&net_message, -1);
    MSG_WriteByte(&net_message, S2C_CONNREJECT);
    MSG_WriteString(&net_message, reason);
    NET_SendPacket(NS_SERVER, net_message.cursize, net_message.data, *adr, 0);
    SZ_Clear(&net_message);
}
void SV_RejectConnectionForPassword(netadr_t *adr)
{
    SZ_Clear(&net_message);
    MSG_WriteLong(&net_message, -1);
    MSG_WriteByte(&net_message, S2C_BADPASSWORD);
    MSG_WriteString(&net_message, "BADPASSWORD");
    NET_SendPacket(NS_SERVER, net_message.cursize, net_message.data, *adr, 0);
    SZ_Clear(&net_message);
}
void SV_SpectatorSpawn(void)
{
    int i;
    edict_t *e;

    VectorCopy(vec3_origin, sv_player->v.origin);
    VectorCopy(vec3_origin, sv_player->v.view_ofs);
    sv_player->v.view_ofs[2] = 22;

    /* põe o espectador no ponto de partida */
    for (i = 2; i < sv.num_edicts; i++)
    {
        e = EDICT_NUM(i);
        if (!strcmp(pr_strings + e->v.classname, "info_player_start"))
        {
            VectorCopy(e->v.origin, sv_player->v.origin);
            break;
        }
    }
}
extern netadr_t net_from;
extern netadr_t net_local_adr;
extern cvar_t sv_lan;
extern cvar_t sv_password;
extern int D_00334540;
extern int g_userid;
qboolean NET_IsLocalAddress(netadr_t adr);
qboolean func_002C6D78(netadr_t a, netadr_t b);
qboolean func_002C6CF8(netadr_t a, netadr_t b);
qboolean func_002C6D58(netadr_t a);
qboolean NET_CompareAdr(netadr_t a, netadr_t b);
char *Info_ValueForKey(char *s, char *key);
void func_00244AC8(char *s, char *key);
void SV_ClearResourceLists(client_t *cl);
int func_002E60D8(const char *a, const char *b, int n);
void Netchan_Setup(int sock, netchan_t *chan, netadr_t adr);
void Netchan_OutOfBandPrint(int sock, netadr_t adr, int unk, char *format, ...);
void Log_Printf(char *fmt, ...);
void SV_ExtractFromUserinfo(client_t *cl);
void SV_DropClient(client_t *cl, qboolean crash);

extern netadr_t net_from;
extern netadr_t net_local_adr;
extern cvar_t sv_lan;
extern cvar_t sv_password;
extern int D_00334540;
extern int g_userid;
qboolean NET_IsLocalAddress(netadr_t adr);
qboolean func_002C6D78(netadr_t a, netadr_t b);
qboolean func_002C6CF8(netadr_t a, netadr_t b);
qboolean func_002C6D58(netadr_t a);
qboolean NET_CompareAdr(netadr_t a, netadr_t b);
char *Info_ValueForKey(char *s, char *key);
void func_00244AC8(char *s, char *key);
void SV_ClearResourceLists(client_t *cl);
int func_002E60D8(const char *a, const char *b, int n);
void Netchan_Setup(int sock, netchan_t *chan, netadr_t adr);
void Netchan_OutOfBandPrint(int sock, netadr_t adr, int unk, char *format, ...);
void Log_Printf(char *fmt, ...);
void SV_ExtractFromUserinfo(client_t *cl);
void SV_DropClient(client_t *cl, qboolean crash);

extern netadr_t net_from;
extern netadr_t net_local_adr;
extern cvar_t sv_lan;
extern cvar_t sv_password;
extern int D_00334540;
extern int g_userid;
qboolean NET_IsLocalAddress(netadr_t adr);
qboolean func_002C6D78(netadr_t a, netadr_t b);
qboolean func_002C6CF8(netadr_t a, netadr_t b);
qboolean func_002C6D58(netadr_t a);
qboolean NET_CompareAdr(netadr_t a, netadr_t b);
char *Info_ValueForKey(char *s, char *key);
void func_00244AC8(char *s, char *key);
void SV_ClearResourceLists(client_t *cl);
int func_002E60D8(const char *a, const char *b, int n);
void Netchan_Setup(int sock, netchan_t *chan, netadr_t adr);
void Netchan_OutOfBandPrint(int sock, netadr_t adr, int unk, char *format, ...);
void Log_Printf(char *fmt, ...);
void SV_ExtractFromUserinfo(client_t *cl);
void SV_DropClient(client_t *cl, qboolean crash);

extern netadr_t net_from;
extern netadr_t net_local_adr;
extern cvar_t sv_lan;
extern cvar_t sv_password;
extern int D_00334540;
extern int g_userid;
qboolean NET_IsLocalAddress(netadr_t adr);
qboolean func_002C6D78(netadr_t a, netadr_t b);
qboolean func_002C6CF8(netadr_t a, netadr_t b);
qboolean func_002C6D58(netadr_t a);
qboolean NET_CompareAdr(netadr_t a, netadr_t b);
char *Info_ValueForKey(char *s, char *key);
void func_00244AC8(char *s, char *key);
void SV_ClearResourceLists(client_t *cl);
int func_002E60D8(const char *a, const char *b, int n);
void Netchan_Setup(int sock, netchan_t *chan, netadr_t adr);
void Netchan_OutOfBandPrint(int sock, netadr_t adr, int unk, char *format, ...);
void Log_Printf(char *fmt, ...);
void SV_ExtractFromUserinfo(client_t *cl);
void SV_DropClient(client_t *cl, qboolean crash);

extern netadr_t net_from;
extern netadr_t net_local_adr;
extern cvar_t sv_lan;
extern cvar_t sv_password;
extern int D_00334540;
extern int g_userid;
qboolean NET_IsLocalAddress(netadr_t adr);
qboolean func_002C6D78(netadr_t a, netadr_t b);
qboolean func_002C6CF8(netadr_t a, netadr_t b);
qboolean func_002C6D58(netadr_t a);
qboolean NET_CompareAdr(netadr_t a, netadr_t b);
char *Info_ValueForKey(char *s, char *key);
void func_00244AC8(char *s, char *key);
void SV_ClearResourceLists(client_t *cl);
int func_002E60D8(const char *a, const char *b, int n);
void Netchan_Setup(int sock, netchan_t *chan, netadr_t adr);
void Netchan_OutOfBandPrint(int sock, netadr_t adr, int unk, char *format, ...);
void Log_Printf(char *fmt, ...);
void SV_ExtractFromUserinfo(client_t *cl);
void SV_DropClient(client_t *cl, qboolean crash);

extern netadr_t net_from;
extern netadr_t net_local_adr;
extern cvar_t sv_lan;
extern cvar_t sv_password;
extern int D_00334540;
extern int g_userid;
qboolean NET_IsLocalAddress(netadr_t adr);
qboolean func_002C6D78(netadr_t a, netadr_t b);
qboolean func_002C6CF8(netadr_t a, netadr_t b);
qboolean func_002C6D58(netadr_t a);
qboolean NET_CompareAdr(netadr_t a, netadr_t b);
char *Info_ValueForKey(char *s, char *key);
void func_00244AC8(char *s, char *key);
void SV_ClearResourceLists(client_t *cl);
int func_002E60D8(const char *a, const char *b, int n);
void Netchan_Setup(int sock, netchan_t *chan, netadr_t adr);
void Netchan_OutOfBandPrint(int sock, netadr_t adr, int unk, char *format, ...);
void Log_Printf(char *fmt, ...);
void SV_ExtractFromUserinfo(client_t *cl);
void SV_DropClient(client_t *cl, qboolean crash);

extern char D_0038E630[]; /* "0" */

void SV_ConnectClient(void)
{
    netadr_t adr;
    char userinfo[1024];
    char name[128];
    char cdkey[512];
    int nClients;
    int nSpectators;
    edict_t *ent;
    unsigned int authid;
    unsigned int invalidid;
    client_t *client;
    client_t *cl;
    int i;
    int challenge;
    int authprotocol;
    int keylen;
    int count;
    char *s;

    count = 0;
    invalidid = 0xFFFFFFFF;
    adr = net_from;
    client = NULL;

    if (atoi(Cmd_Argv(1)) != protocol_version)
    {
        SV_RejectConnection(&adr, "Bad protocol\n");
        return;
    }

    if (Cmd_Argc() < 7)
    {
        SV_RejectConnection(&adr, "Insufficient connection info\n");
        return;
    }

    challenge = atoi(Cmd_Argv(2));

    if (!NET_IsLocalAddress(adr))
    {
        for (i = 0; i < 16; i++)
        {
            if (func_002C6D78(net_from, svs.challenges[i].adr))
            {
                if (challenge == svs.challenges[i].challenge)
                    break;
                SV_RejectConnection(&adr, "Bad challenge.\n");
                return;
            }
        }
        if (i == 16)
        {
            SV_RejectConnection(&adr, "No challenge for your address.\n");
            return;
        }
    }

    if (D_00334540 || sv_lan.value)
    {
        if (net_from.type == 3 && !func_002C6CF8(net_from, net_local_adr) && !func_002C6D58(net_from))
        {
            SV_RejectConnection(&adr, "LAN servers are restricted to local clients (class C).\n");
            return;
        }
    }

    for (i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
    {
        if (!cl->active && !cl->spawned && !cl->connected)
            continue;

        if (NET_CompareAdr(adr, cl->netchan.remote_address))
        {
            Con_Printf("%s:reconnect\n", NET_AdrToString(adr));
            SV_DropClient(cl, false);
        }
    }

    s = Cmd_Argv(3);
    if (!s || !s[0])
        goto invalid;
    authprotocol = atoi(s);
    if (authprotocol <= 0)
        goto invalid;
    if (authprotocol > 2)
        goto invalid;

    s = Cmd_Argv(4);
    if (!s || !s[0])
        goto invalid;
    authid = atoi(s);

    s = Cmd_Argv(5);
    if (!s || !s[0])
        goto invalid;
    keylen = atoi(s);
    if (keylen <= 0)
        goto invalid;
    if (keylen > 1024)
    {
invalid:
        SV_RejectConnection(&adr, "Invalid connection.\n");
        return;
    }

    s = Cmd_Argv(6);
    if (!s)
    {
        SV_RejectConnection(&adr, "Invalid connection protocol.\n");
        return;
    }
    memset(cdkey, 0, sizeof(cdkey));
    strcpy(cdkey, s);

    s = Cmd_Argv(7);
    strncpy(userinfo, s, sizeof(userinfo) - 1);

    s = Info_ValueForKey(userinfo, "spectator");
    if (s[0] && strcmp(s, D_0038E630))
        ;

    if (!NET_IsLocalAddress(adr) && sv_password.string[0] && strcasecmp(sv_password.string, "none")
        && strcmp(sv_password.string, Info_ValueForKey(userinfo, "password")))
    {
        Con_Printf("%s:  password failed\n", NET_AdrToString(net_from));
        SV_RejectConnectionForPassword(&net_from);
        return;
    }

    if (0)
    {
        Con_Printf("%s:spectator password failed\n", NET_AdrToString(net_from));
        SV_RejectConnection(&adr, "Invalid spectator password.\n");
        Info_ValueForKey(userinfo, "*spectator");
        return;
    }

    func_00244AC8(userinfo, "password");

    s = Info_ValueForKey(userinfo, "name");
    memset(name, 0, sizeof(name));
    if (s)
    {
        strncpy(name, s, sizeof(name));
        name[sizeof(name) - 1] = 0;
    }
    else
    {
        strcpy(name, "unconnected");
    }

    nSpectators = 0;
    nClients = 0;
    SV_CountPlayers(&nClients, &nSpectators);
    nClients -= nSpectators;

    if (0)
    {
        Con_Printf("%s:No space for spectator\n", NET_AdrToString(net_from));
        SV_RejectConnection(&adr, "No more spectators allowed.\n");
        return;
    }

    for (i = 0; i < svs.maxclients; i++)
    {
        client = &svs.clients[i];
        if (!client->active && !client->spawned && !client->connected)
            break;
    }

    if (i == svs.maxclients)
    {
        SV_RejectConnection(&net_from, "Server is full.");
        return;
    }

    ent = EDICT_NUM(i + 1);

    switch (authprotocol)
    {
    default:
    case 1:
        if (invalidid != authid)
        {
            SV_RejectConnection(&adr, "Invalid authentication userid.\n");
            return;
        }
        break;

    case 2:
        if (!D_00334540)
        {
            SV_RejectConnection(&adr, "Invalid authentication message.\n");
            return;
        }
        if (strlen(cdkey) != 32 || keylen != 32)
        {
            SV_RejectConnection(&adr, "Invalid CD Key.\n");
            return;
        }
        for (i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
        {
            if (!cl->active && !cl->spawned && !cl->connected)
                continue;
            if (!func_002E60D8(cdkey, cl->hashedcdkey, 32))
                count++;
        }
        if (count >= 5)
        {
            SV_RejectConnection(&adr, "CD Key already in use.\n");
            return;
        }
        break;
    }

    host_client = client;
    SV_ClearResourceLists(client);
    memset(client, 0, sizeof(client_t));

    client->resourcesneeded.pNext = client->resourcesneeded.pPrev = &client->resourcesneeded;
    client->resourcesonhand.pNext = client->resourcesonhand.pPrev = &client->resourcesonhand;

    Netchan_Setup(NS_SERVER, &client->netchan, adr);
    Netchan_OutOfBandPrint(NS_SERVER, adr, *(int *)adr.ip, "%c0000000000000000", 'B');

    if (!strcasecmp(NET_AdrToString(client->netchan.remote_address), "loopback"))
        Con_DPrintf("Local connection.\n");
    else if (0)
        Con_DPrintf("Spectator %s connected\nAdr: %s\n", name, NET_AdrToString(client->netchan.remote_address));
    else
        Con_DPrintf("Client %s connected\nAdr: %s\n", name, NET_AdrToString(client->netchan.remote_address));

    switch (authprotocol)
    {
    case 1:
        break;
    case 2:
        strncpy(client->hashedcdkey, cdkey, 32);
        client->hashedcdkey[32] = 0;
        break;
    }

    client->active = false;
    client->spawned = false;
    client->connected = true;
    client->uploading = false;
    client->edict = ent;
    client->maxspeed = 0;
    client->userid = g_userid++;
    client->WONid = authid;

    Log_Printf("\"%s<%i><WON:%u>\" connected, address \"%s\"\n", name, client->userid, client->WONid,
               NET_AdrToString(client->netchan.remote_address));

    strncpy(client->userinfo, userinfo, MAX_INFO_STRING);
    SV_ExtractFromUserinfo(client);

    client->datagram.allowoverflow = true;
    client->datagram.data = client->datagram_buf;
    client->datagram.maxsize = sizeof(client->datagram_buf);
    client->spectator = false;
    client->unk_73D0 = 0;

    if (!sv.loadgame)
        gEntityInterface.pfnParmsNewLevel();

    client->sendinfo_time = 0;
}
extern netadr_t net_from;
mleaf_t *Mod_PointInLeaf(vec_t *p, model_t *model);
void ED_LoadFromFile(char *data);
struct packet_entities_s;
int SV_EmitPacketEntities(int unused, client_t *client, struct packet_entities_s *to, sizebuf_t *msg);

typedef struct
{
    qboolean active;            /* 0x000 */
    char unk_004[256];          /* 0x004 */
    char unk_104[256];          /* 0x104 */
    char unk_204[32];           /* 0x204 */
    int unk_224;
    int unk_228;
    int unk_22C;
    int unk_230;
} unk_53DB38_t;

extern unk_53DB38_t D_0053DB38;

void SVC_Ping(void)
{
    char data[6];

    data[0] = 0xFF;
    data[1] = 0xFF;
    data[2] = 0xFF;
    data[3] = 0xFF;
    data[4] = 'j';
    data[5] = 0;

    NET_SendPacket(NS_SERVER, sizeof(data), data, net_from, 0);
}
void func_0027D0F0(void)
{
}
qboolean func_002C6D78(netadr_t a, netadr_t b);
int RandomLong(int lLow, int lHigh);
extern int (*D_003990F0)(int l);
extern int D_00334540;
extern cvar_t sv_lan;
void func_00245310(void);
void Q_memcpy(void *dest, void *src, int count);

#ifdef NON_MATCHING
void SVC_GetChallenge(void)
{
    int i;
    int oldest = 0;
    int oldestTime = 0x7FFFFFFF;
    char data[128];
    int challenge;

    for (i = 0; i < 16; i++)
    {
        if (func_002C6D78(net_from, svs.challenges[i].adr))
            break;
        if (svs.challenges[i].time < oldestTime)
        {
            oldestTime = svs.challenges[i].time;
            oldest = i;
        }
    }

    if (i == 16)
    {
        i = oldest;
        svs.challenges[oldest].challenge = (RandomLong(0, 0xFFFF) << 16) | RandomLong(0, 0xFFFF);
        svs.challenges[oldest].adr = net_from;
        svs.challenges[oldest].time = realtime;
    }

    sprintf(data, "%c%c%c%c%c0000000000000000", 255, 255, 255, 255, 'A');
    challenge = D_003990F0(svs.challenges[i].challenge);
    Q_memcpy(&data[21], &challenge, 4);

    if (sv.active && svs.maxclients > 1 && sv_lan.value == 0.0f)
        func_00245310();
    else
        D_00334540 = 1;

    if (D_00334540)
    {
        Con_DPrintf("Server requiring authentication\n");
        data[25] = 2;
    }
    else
    {
        Con_DPrintf("Server requiring WON authentication\n");
        data[25] = 1;
    }

    NET_SendPacket(NS_SERVER, 26, data, net_from, *(int *)net_from.ip);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", SVC_GetChallenge);
#endif
void func_0027D340(void)
{
    memset(&D_0053DB38, 0, sizeof(D_0053DB38));
    D_0053DB38.unk_224 = 1;
    D_0053DB38.unk_22C = 1;
}
extern char D_0038E628[]; /* "" (ainda emitido pelo bloco de rodata) */

qboolean func_0027D380(char *a0, char *a1, int *a2, int *a3, int *a4, int *a5, char *a6)
{
    if (D_0053DB38.active)
    {
        strcpy(a0, D_0053DB38.unk_004);
        strcpy(a1, D_0053DB38.unk_104);
        strcpy(a6, D_0053DB38.unk_204);
        *a2 = D_0053DB38.unk_224;
        *a3 = D_0053DB38.unk_228;
        *a4 = D_0053DB38.unk_22C;
        *a5 = D_0053DB38.unk_230;
    }
    else
    {
        /* strcpy(x, "") expandido pelo compilador */
        a0[0] = D_0038E628[0];
        a1[0] = D_0038E628[0];
        a6[0] = D_0038E628[0];
        *a2 = 1;
        *a3 = 0;
        *a4 = 1;
        *a5 = 0;
    }
    return D_0053DB38.active;
}
extern int D_0033473C;
extern netadr_t net_local_adr;
extern cvar_t hostname;
extern cvar_t sv_password;
qboolean func_0027D380(char *a0, char *a1, int *a2, int *a3, int *a4, int *a5, char *a6);

void SVC_Info(qboolean bDetailed)
{
    sizebuf_t buf;
    byte data[1400];
    char szURL[512];
    char szDLURL[512];
    char szGameDir[260];
    char szHLVersion[32];
    int nVersion;
    int nSize;
    int bSvOnly;
    int bClDll;
    int i;
    int count;

    buf.maxsize = sizeof(data);
    buf.data = data;
    buf.cursize = 0;

    if (!sv.active)
        return;
    if (svs.maxclients <= 1)
        return;

    count = 0;
    for (i = 0; i < svs.maxclients; i++)
    {
        if (svs.clients[i].active || svs.clients[i].connected || svs.clients[i].spawned)
            count++;
    }

    MSG_WriteLong(&buf, -1);
    if (bDetailed)
        MSG_WriteByte(&buf, 'm');
    else
        MSG_WriteByte(&buf, 'C');

    if (!D_0033473C)
        MSG_WriteString(&buf, NET_AdrToString(net_local_adr));
    else
        MSG_WriteString(&buf, "LOOPBACK");

    MSG_WriteString(&buf, hostname.string);
    MSG_WriteString(&buf, sv.name);

    COM_FileBase(com_gamedir, szGameDir);
    MSG_WriteString(&buf, szGameDir);
    MSG_WriteString(&buf, gEntityInterface.pfnGetGameDescription());

    MSG_WriteByte(&buf, count);
    MSG_WriteByte(&buf, svs.maxclients);
    MSG_WriteByte(&buf, protocol_version);

    if (bDetailed)
    {
        if (!cls[0].state)
            MSG_WriteByte(&buf, 'd');
        else
            MSG_WriteByte(&buf, 'l');
        MSG_WriteByte(&buf, 'l');

        if (strlen(sv_password.string) && strcasecmp(sv_password.string, "none"))
            MSG_WriteByte(&buf, 1);
        else
            MSG_WriteByte(&buf, 0);

        if (func_0027D380(szURL, szDLURL, &nVersion, &nSize, &bSvOnly, &bClDll, szHLVersion))
        {
            MSG_WriteByte(&buf, 1);
            MSG_WriteString(&buf, szURL);
            MSG_WriteString(&buf, szDLURL);
            MSG_WriteString(&buf, szHLVersion);
            MSG_WriteLong(&buf, nVersion);
            MSG_WriteLong(&buf, nSize);
            MSG_WriteByte(&buf, bSvOnly);
            MSG_WriteByte(&buf, bClDll);
        }
        else
        {
            MSG_WriteByte(&buf, 0);
        }
    }

    NET_SendPacket(NS_SERVER, buf.cursize, buf.data, net_from, 0);
}
void SVC_PlayerInfo(void)
{
    sizebuf_t buf;
    byte data[2048];
    int i;
    int count;
    client_t *client;

    buf.maxsize = sizeof(data);
    buf.data = data;
    buf.cursize = 0;

    if (!sv.active)
        return;
    if (svs.maxclients <= 1)
        return;

    MSG_WriteLong(&buf, -1);
    MSG_WriteByte(&buf, 'D');

    count = 0;
    client = svs.clients;
    for (i = 0; i < svs.maxclients; i++)
    {
        if (client[i].active)
            count++;
    }
    MSG_WriteByte(&buf, count);

    count = 0;
    for (i = 0, client = svs.clients; i < svs.maxclients; i++, client++)
    {
        if (!svs.clients[i].active)
            continue;

        count++;
        MSG_WriteByte(&buf, count);
        MSG_WriteString(&buf, client->name);
        MSG_WriteLong(&buf, (int)client->edict->v.frags);
        MSG_WriteFloat(&buf, realtime - client->netchan.connect_time);
    }

    NET_SendPacket(NS_SERVER, buf.cursize, buf.data, net_from, 0);
}
extern char D_0038E630[]; /* "0" (ainda emitido pelo bloco de rodata) */
extern char D_0038E678[]; /* "1" */
int Cvar_CountServerVariables(void);
extern cvar_t *cvar_vars;
void NET_SendPacket(int sock, int length, void *data, netadr_t to, int unk);

#define FCVAR_SERVER 4
#define FCVAR_PROTECTED 0x20

void SVC_RuleInfo(void)
{
    sizebuf_t buf;
    byte data[2048];
    int nNumRules;
    cvar_t *var;

    buf.maxsize = sizeof(data);
    buf.data = data;
    buf.cursize = 0;

    if (!sv.active)
        return;
    if (svs.maxclients <= 1)
        return;

    nNumRules = Cvar_CountServerVariables();
    if (nNumRules <= 0)
        return;

    MSG_WriteLong(&buf, -1);
    MSG_WriteByte(&buf, 'E');
    MSG_WriteShort(&buf, nNumRules);

    for (var = cvar_vars; var; var = var->next)
    {
        if (!(var->flags & FCVAR_SERVER))
            continue;

        MSG_WriteString(&buf, var->name);
        if (var->flags & FCVAR_PROTECTED)
        {
            if (strlen(var->string) > 0 && strcasecmp(var->string, "none"))
                MSG_WriteString(&buf, D_0038E678);
            else
                MSG_WriteString(&buf, D_0038E630);
        }
        else
        {
            MSG_WriteString(&buf, var->string);
        }
    }

    NET_SendPacket(NS_SERVER, buf.cursize, buf.data, net_from, 0);
}
typedef struct master_s
{
    int heartbeat;              /* 0x00 */
    int unk_4;                  /* 0x04 */
    int challenge;              /* 0x08 */
    int unk_C[2];               /* 0x0C */
    netadr_t adr;               /* 0x14 */
    struct master_s *next;      /* 0x1C */
} master_t;

extern master_t *valvemaster_adr;
qboolean NET_CompareAdr(netadr_t a, netadr_t b);
int MSG_ReadLong(void);
void Master_Heartbeat(master_t *p);

void func_0027DA98(void)
{
    master_t *p;

    if (!valvemaster_adr)
        return;

    for (p = valvemaster_adr; p; p = p->next)
    {
        if (!NET_CompareAdr(p->adr, net_from))
            continue;
        p->heartbeat = 0;
        p->challenge = MSG_ReadLong();
        Master_Heartbeat(p);
        break;
    }
}
void MSG_BeginReading(void);
char *MSG_ReadStringLine(void);
int MSG_ReadByte(void);
char *MSG_ReadString(void);
void Cmd_TokenizeString(char *text);
extern cvar_t sv_logrelay;
void func_0027D0F0(void);
void SV_ConnectClient(void);
void func_00297D10(netadr_t *adr);

void SV_ConnectionlessPacket(void)
{
    char *args;
    char *c;
    char *line;

    MSG_BeginReading();
    MSG_ReadLong();

    args = MSG_ReadStringLine();
    line = args;
    Cmd_TokenizeString(args);
    c = Cmd_Argv(0);

    if (!strcmp(c, "ping") || (c[0] == 'i' && (c[1] == 0 || c[1] == '\n')))
    {
        SVC_Ping();
        return;
    }

    if (c[0] == 'l' && (c[1] == 0 || c[1] == '\n'))
    {
        MSG_BeginReading();
        MSG_ReadLong();
        MSG_ReadByte();
        Con_DPrintf(MSG_ReadString());
        return;
    }

    if (c[0] == 'j' && (c[1] == 0 || c[1] == '\n'))
    {
        Con_Printf("A2A_ACK from %s\n", NET_AdrToString(net_from));
        return;
    }

    if (c[0] == 's' && (c[1] == 0 || c[1] == '\n'))
    {
        func_0027DA98();
        return;
    }

    if (!strcasecmp(c, "log"))
    {
        if (sv_logrelay.value && args && strlen(args) > 4)
        {
            args += 4;
            if (args && line[4])
                Con_Printf("%s\n", args);
        }
    }
    else if (!strcmp(c, "status"))
        func_0027D0F0();
    else if (!strcmp(c, "getchallenge"))
        SVC_GetChallenge();
    else if (!strcasecmp(c, "info"))
        SVC_Info(false);
    else if (!strcasecmp(c, "details"))
        SVC_Info(true);
    else if (!strcasecmp(c, "players"))
        SVC_PlayerInfo();
    else if (!strcasecmp(c, "rules"))
        SVC_RuleInfo();
    else if (!strcmp(c, "connect"))
        SV_ConnectClient();
    else if (!strcmp(c, "rcon"))
        func_00297D10(&net_from);
    else
        Con_Printf("bad connectionless packet from %s:\n%s\n", NET_AdrToString(net_from), args);
}
extern cvar_t sv_maxrate;
extern cvar_t sv_minrate;

void SV_CheckRate(client_t *cl)
{
    if (sv_maxrate.value > 0.0f)
    {
        if (cl->netchan.rate > sv_maxrate.value)
            cl->netchan.rate = sv_maxrate.value < 10000.0f ? sv_maxrate.value : 10000.0f;
    }
    if (sv_minrate.value > 0.0f)
    {
        if (cl->netchan.rate < sv_minrate.value)
            cl->netchan.rate = sv_minrate.value > 500.0f ? sv_minrate.value : 500.0f;
    }
}
void func_002879D0(void);
qboolean NET_GetPacket(int sock);
void SV_ConnectionlessPacket(void);
qboolean Netchan_Process(netchan_t *chan);
void SV_ExecuteClientMessage(client_t *cl, int unk);

void SV_ReadPackets(void)
{
    int i;
    client_t *cl;

    func_002879D0();

    while (NET_GetPacket(NS_SERVER))
    {
        if (*(int *)net_message.data == -1)
        {
            SV_ConnectionlessPacket();
            continue;
        }

        for (i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
        {
            if (!cl->connected && !cl->active && !cl->spawned)
                continue;
            if (!NET_CompareAdr(net_from, cl->netchan.remote_address))
                continue;

            SV_CheckRate(cl);
            if (Netchan_Process(&cl->netchan))
            {
                svs.unk_138++;
                cl->send_message = true;
                SV_ExecuteClientMessage(cl, *(int *)net_from.ip);
            }
            break;
        }
    }
}
extern cvar_t sv_timeout;
void SV_BroadcastPrintf(char *fmt, ...);

void SV_CheckTimeouts(void)
{
    int i;
    client_t *cl;
    float droptime;

    droptime = realtime - sv_timeout.value;

    for (i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
    {
        if (cl->fakeclient)
            continue;
        if (!cl->connected && !cl->active && !cl->spawned)
            continue;
        if (cl->netchan.last_received < droptime)
        {
            SV_BroadcastPrintf("%s timed out\n", cl->name);
            SV_DropClient(cl, false);
        }
    }
}
int func_0027E140(client_t *cl)
{
    return 0;
}
int func_0027E148(client_t *cl)
{
    return 0;
}
void Info_RemovePrefixedKeys(char *s, char prefix);

#define svc_updateuserinfo 13

void SV_FullClientUpdate(client_t *cl, sizebuf_t *sb)
{
    char info[MAX_INFO_STRING];
    int i;

    i = cl - svs.clients;

    strcpy(info, cl->userinfo);
    Info_RemovePrefixedKeys(info, '_');

    MSG_WriteByte(sb, svc_updateuserinfo);
    MSG_WriteByte(sb, i);
    MSG_WriteLong(sb, cl->userid);
    MSG_WriteString(sb, info);

    if (cl->maxspeed != 0.0f)
    {
        MSG_WriteByte(sb, 52);
        MSG_WriteByte(sb, i);
        MSG_WriteFloat(sb, cl->maxspeed);
    }
}
void SV_ClearDatagram(void)
{
    SZ_Clear(&sv.datagram);
}
extern int fatbytes;
extern byte fatpvs[];
byte *Mod_LeafPVS(mleaf_t *leaf, model_t *model);

void SV_AddToFatPVS(vec_t *org, mnode_t *node)
{
    int i;
    byte *pvs;
    mplane_t *plane;
    float d;

    while (1)
    {
        if (node->contents < 0)
        {
            if (node->contents != -2)
            {
                pvs = Mod_LeafPVS((mleaf_t *)node, sv.worldmodel);
                for (i = 0; i < fatbytes; i++)
                    fatpvs[i] |= pvs[i];
            }
            return;
        }

        plane = node->plane;
        d = DotProduct(org, plane->normal) - plane->dist;
        if (d > 8)
            node = node->children[0];
        else if (d < -8)
            node = node->children[1];
        else
        {
            SV_AddToFatPVS(org, node->children[0]);
            node = node->children[1];
        }
    }
}
void SV_AddToFatPVS(float *org, mnode_t *node);

byte *SV_FatPVS(float *org)
{
    fatbytes = (sv.worldmodel->numleafs + 31) >> 3;
    memset(fatpvs, 0, fatbytes);
    SV_AddToFatPVS(org, sv.worldmodel->nodes);
    return fatpvs;
}
int SV_PointLeafnum(vec_t *p)
{
    mleaf_t *leaf;

    leaf = Mod_PointInLeaf(p, sv.worldmodel);
    if (leaf)
        return leaf - sv.worldmodel->leafs;

    return 0;
}
void MSG_WriteBitsShort(short *data, int numbits);

void func_0027E480(entity_state_t *from, entity_state_t *to, sizebuf_t *msg, qboolean force)
{
    int bits;
    int i;
    int mode;
    float d;
    byte bval;
    unsigned short sval;
    short coord;

    bits = 0x2101;
    if (to->rendermode != from->rendermode)
        bits |= 0x400;

    mode = to->rendermode & 0xF;
    if (mode == 0 || mode == 1)
    {
        for (i = 0; i < 3; i++)
        {
            d = to->origin[i] - from->origin[i];
            if (d < -0.1f || d > 0.1f)
                bits |= (2 << i);
        }

        if (mode == 0)
        {
            for (i = 0; i < 3; i++)
            {
                d = to->angles[i] - from->angles[i];
                if (d < -0.1f || d > 0.1f)
                    bits |= (0x20 << i);
            }
        }
    }

    if (mode == 1 || mode == 2)
    {
        if (to->sequence != from->sequence || to->skin != from->skin)
            bits |= 0x200;
    }

    if (to->modelindex != from->modelindex)
        bits |= 0x800;
    if (to->scale != from->scale)
        bits |= 0x1000;
    if (to->body != from->body)
        bits |= 0x4000;
    if ((*(int *)&to->rendercolor & 0xFFFFFF) != (*(int *)&from->rendercolor & 0xFFFFFF) || to->renderfx != from->renderfx)
        bits |= 0x10000;
    if (to->renderamt != from->renderamt)
        bits |= 0x20000;
    if (to->frame != from->frame)
        bits |= 0x80000;
    if ((int)to->animtime != (int)from->animtime)
        bits |= 0x40000;

    if (to->number >= 256)
        bits |= 0x8000;
    if (bits >= 0x100)
        bits |= 1;
    if (bits > 0xFFFF)
        bits |= 0x100;
    if (bits > 0xFFFFFF)
        bits |= 0x800000;

    if (!to->number)
        Sys_Error("Unset entity number");

    if (!bits && !force)
        return;

    bval = bits;
    MSG_WriteBitsUChar(&bval, 8);
    if (bits & 1)
    {
        bval = bits >> 8;
        MSG_WriteBitsUChar(&bval, 8);
    }
    if (bits & 0x100)
    {
        bval = bits >> 16;
        MSG_WriteBitsUChar(&bval, 8);
    }
    if (bits & 0x800000)
    {
        bval = bits >> 24;
        MSG_WriteBitsUChar(&bval, 8);
    }

    if (bits & 0x8000)
    {
        sval = to->number;
        MSG_WriteBitsUShort(&sval, 10);
    }
    else
    {
        bval = to->number;
        MSG_WriteBitsUChar(&bval, 8);
    }

    if (bits & 2)
    {
        coord = to->origin[0] * 8;
        MSG_WriteBitsShort(&coord, 16);
    }
    if (bits & 4)
    {
        coord = to->origin[1] * 8;
        MSG_WriteBitsShort(&coord, 16);
    }
    if (bits & 8)
    {
        coord = to->origin[2] * 8;
        MSG_WriteBitsShort(&coord, 16);
    }
    if (bits & 0x20)
    {
        coord = to->angles[0] * 8;
        MSG_WriteBitsShort(&coord, 16);
    }
    if (bits & 0x40)
    {
        coord = to->angles[1] * 8;
        MSG_WriteBitsShort(&coord, 16);
    }
    if (bits & 0x80)
    {
        coord = to->angles[2] * 8;
        MSG_WriteBitsShort(&coord, 16);
    }

    if (bits & 0x200)
    {
        sval = to->sequence;
        MSG_WriteBitsUShort(&sval, 16);
        sval = to->skin;
        MSG_WriteBitsUShort(&sval, 16);
    }
    if (bits & 0x400)
    {
        bval = to->rendermode;
        MSG_WriteBitsUChar(&bval, 8);
    }
    if (bits & 0x800)
    {
        sval = to->modelindex;
        MSG_WriteBitsUShort(&sval, 16);
    }
    if (bits & 0x1000)
    {
        bval = to->scale;
        MSG_WriteBitsUChar(&bval, 8);
    }
    if (bits & 0x4000)
    {
        bval = to->body;
        MSG_WriteBitsUChar(&bval, 8);
    }
    if (bits & 0x10000)
    {
        bval = to->rendercolor.r;
        MSG_WriteBitsUChar(&bval, 8);
        bval = to->rendercolor.g;
        MSG_WriteBitsUChar(&bval, 8);
        bval = to->rendercolor.b;
        MSG_WriteBitsUChar(&bval, 8);
        bval = to->renderfx;
        MSG_WriteBitsUChar(&bval, 8);
    }
    if (bits & 0x20000)
    {
        bval = to->renderamt;
        MSG_WriteBitsUChar(&bval, 8);
    }
    if (bits & 0x80000)
    {
        bval = to->frame;
        MSG_WriteBitsUChar(&bval, 8);
    }
    if (bits & 0x40000)
    {
        bval = to->animtime;
        MSG_WriteBitsUChar(&bval, 8);
    }
}
void func_00242C90(int numbits, float angle);
void func_00242B58(signed char *data, int numbits);

void func_00242C90(int numbits, float angle);
void func_00242B58(signed char *data, int numbits);

void func_00242C90(int numbits, float angle);
void func_00242B58(signed char *data, int numbits);

void SV_WriteDelta(entity_state_t *from, entity_state_t *to, sizebuf_t *msg, qboolean force)
{
    unsigned int bits;
    unsigned int bits2;
    int i;
    float d;
    byte bval;
    unsigned short usval;
    short coord;
    float mineps;
    signed char cval;
    short sbody;
    byte t;

    if (to->entityType)
    {
        func_0027E480(from, to, msg, force);
        return;
    }

    bits = 0;
    bits2 = 0;

    for (i = 0; i < 3; i++)
    {
        d = to->origin[i] - from->origin[i];
        if (d < -0.1f || d > 0.1f)
            bits |= (2 << i);
    }

    if (to->angles[0] != from->angles[0])
        bits |= 0x200;
    if (to->angles[1] != from->angles[1])
        bits |= 0x20;
    if (to->angles[2] != from->angles[2])
        bits |= 0x400;

    if (to->movetype != from->movetype)
        bits |= 0x40;
    if (to->colormap != from->colormap)
        bits |= 0x40000;
    if (*(int *)&to->skin != *(int *)&from->skin)
        bits |= 0x80000;
    if (to->frame != from->frame)
        bits |= 0x80;
    if (to->scale != from->scale)
        bits |= 0x40000000;
    if (to->effects != from->effects)
        bits |= 0x4000;
    if (to->modelindex != from->modelindex)
        bits |= 0x800;
    if (to->animtime != from->animtime || to->sequence != from->sequence)
        bits |= 0x1000;
    if (to->framerate != from->framerate)
        bits |= 0x10000;
    if (to->body != from->body)
        bits |= 0x400000;
    if (to->controller[0] != from->controller[0])
        bits |= 0x1000000;
    if (to->controller[1] != from->controller[1])
        bits |= 0x2000000;
    if (to->controller[2] != from->controller[2])
        bits |= 0x4000000;
    if (to->controller[3] != from->controller[3])
        bits |= 0x8000000;
    if (to->blending[0] != from->blending[0])
        bits |= 0x10000000;
    if (to->blending[1] != from->blending[1])
        bits |= 0x20000000;

    if (to->rendermode != from->rendermode || to->renderamt != from->renderamt || to->renderfx != from->renderfx
        || (*(int *)&to->rendercolor & 0xFFFFFF) != (*(int *)&from->rendercolor & 0xFFFFFF))
        bits |= 0x100000;

    if (to->animtime && !to->velocity[0] && !to->velocity[1] && !to->velocity[2])
        bits |= 0x200000;

    mineps = -0.1f;
    for (i = 0; i < 3; i++)
    {
        d = to->mins[i] - from->mins[i];
        if (d < mineps || d > 0.1f)
            bits2 |= (1 << i);
        d = to->maxs[i] - from->maxs[i];
        if (d < mineps || d > 0.1f)
            bits2 |= (8 << i);
    }

    if (to->aiment != from->aiment)
        bits2 |= 0x40;
    if (to->weaponmodel != from->weaponmodel)
        bits2 |= 0x100;

    if (to->movetype == 12 && to->aiment)
        bits &= ~0xE;
    else if (to->aiment != from->aiment)
        bits |= 0xE;

    if (to->number >= 256)
        bits |= 0x8000;
    if (bits >= 0x100)
        bits |= 1;
    if (bits > 0xFFFF)
        bits |= 0x100;
    if (bits > 0xFFFFFF)
        bits |= 0x800000;

    if (bits2)
        bits |= 0x80800101;

    if (!to->number)
        Sys_Error("Unset entity number");

    if (!bits && !bits2 && !force)
        return;

    bval = bits;
    MSG_WriteBitsUChar(&bval, 8);
    if (bits & 1)
    {
        bval = bits >> 8;
        MSG_WriteBitsUChar(&bval, 8);
    }
    if (bits & 0x100)
    {
        bval = bits >> 16;
        MSG_WriteBitsUChar(&bval, 8);
    }
    if (bits & 0x800000)
    {
        bval = bits >> 24;
        MSG_WriteBitsUChar(&bval, 8);
    }
    if (bits & 0x80000000)
    {
        bval = bits2;
        MSG_WriteBitsUChar(&bval, 8);
    }

    if (bits & 0x8000)
    {
        usval = to->number;
        MSG_WriteBitsUShort(&usval, 10);
    }
    else
    {
        bval = to->number;
        MSG_WriteBitsUChar(&bval, 8);
    }

    if (bits & 0x800)
    {
        if (to->modelindex >= 256)
        {
            bval = 1;
            MSG_WriteBitsUChar(&bval, 1);
            usval = to->modelindex;
            MSG_WriteBitsUShort(&usval, 10);
        }
        else
        {
            bval = 0;
            MSG_WriteBitsUChar(&bval, 1);
            bval = to->modelindex;
            MSG_WriteBitsUChar(&bval, 8);
        }
    }

    if (bits & 0x80)
    {
        bval = to->frame;
        MSG_WriteBitsUChar(&bval, 8);
    }
    if (bits & 0x40)
    {
        bval = to->movetype;
        MSG_WriteBitsUChar(&bval, 4);
    }
    if (bits & 0x40000)
    {
        usval = to->colormap;
        MSG_WriteBitsUShort(&usval, 16);
    }
    if (bits & 0x80000)
    {
        usval = to->skin;
        MSG_WriteBitsUShort(&usval, 10);
        bval = to->solid;
        MSG_WriteBitsUChar(&bval, 3);
    }
    if (bits & 0x40000000)
    {
        usval = to->scale * 256;
        MSG_WriteBitsUShort(&usval, 16);
    }
    if (bits & 0x4000)
    {
        usval = to->effects;
        MSG_WriteBitsUShort(&usval, 10);
    }

    if (bits & 2)
    {
        coord = to->origin[0] * 8;
        MSG_WriteBitsShort(&coord, 16);
    }
    if (bits & 0x200)
        func_00242C90(11, to->angles[0]);
    if (bits & 4)
    {
        coord = to->origin[1] * 8;
        MSG_WriteBitsShort(&coord, 16);
    }
    if (bits & 0x20)
        func_00242C90(13, to->angles[1]);
    if (bits & 8)
    {
        coord = to->origin[2] * 8;
        MSG_WriteBitsShort(&coord, 16);
    }
    if (bits & 0x400)
        func_00242C90(11, to->angles[2]);

    if (bits & 0x1000)
    {
        bval = to->sequence;
        t = to->animtime * 100;
        MSG_WriteBitsUChar(&bval, 8);
        bval = t;
        MSG_WriteBitsUChar(&bval, 8);
    }
    if (bits & 0x10000)
    {
        cval = to->framerate * 16;
        func_00242B58(&cval, 8);
    }

    if (bits & 0x1000000)
    {
        bval = to->controller[0];
        MSG_WriteBitsUChar(&bval, 8);
    }
    if (bits & 0x2000000)
    {
        bval = to->controller[1];
        MSG_WriteBitsUChar(&bval, 8);
    }
    if (bits & 0x4000000)
    {
        bval = to->controller[2];
        MSG_WriteBitsUChar(&bval, 8);
    }
    if (bits & 0x8000000)
    {
        bval = to->controller[3];
        MSG_WriteBitsUChar(&bval, 8);
    }
    if (bits & 0x10000000)
    {
        bval = to->blending[0];
        MSG_WriteBitsUChar(&bval, 8);
    }
    if (bits & 0x20000000)
    {
        bval = to->blending[1];
        MSG_WriteBitsUChar(&bval, 8);
    }

    if (bits & 0x400000)
    {
        bval = to->body;
        sbody = to->body;
        MSG_WriteBitsUShort((unsigned short *)&sbody, 10);
    }

    if (bits & 0x100000)
    {
        bval = to->rendermode;
        MSG_WriteBitsUChar(&bval, 8);
        bval = to->renderamt;
        MSG_WriteBitsUChar(&bval, 8);
        bval = to->rendercolor.r;
        MSG_WriteBitsUChar(&bval, 8);
        bval = to->rendercolor.g;
        MSG_WriteBitsUChar(&bval, 8);
        bval = to->rendercolor.b;
        MSG_WriteBitsUChar(&bval, 8);
        bval = to->renderfx;
        MSG_WriteBitsUChar(&bval, 8);
    }

    if (bits2 & 1)
    {
        coord = to->mins[0] * 8;
        MSG_WriteBitsShort(&coord, 16);
    }
    if (bits2 & 2)
    {
        coord = to->mins[1] * 8;
        MSG_WriteBitsShort(&coord, 16);
    }
    if (bits2 & 4)
    {
        coord = to->mins[2] * 8;
        MSG_WriteBitsShort(&coord, 16);
    }
    if (bits2 & 8)
    {
        coord = to->maxs[0] * 8;
        MSG_WriteBitsShort(&coord, 16);
    }
    if (bits2 & 0x10)
    {
        coord = to->maxs[1] * 8;
        MSG_WriteBitsShort(&coord, 16);
    }
    if (bits2 & 0x20)
    {
        coord = to->maxs[2] * 8;
        MSG_WriteBitsShort(&coord, 16);
    }
    if (bits2 & 0x40)
    {
        coord = to->aiment;
        MSG_WriteBitsUShort((unsigned short *)&coord, 10);
    }
    if (bits2 & 0x100)
    {
        if (to->weaponmodel >= 256)
        {
            bval = 1;
            MSG_WriteBitsUChar(&bval, 1);
            usval = to->weaponmodel;
            MSG_WriteBitsUShort(&usval, 10);
        }
        else
        {
            bval = 0;
            MSG_WriteBitsUChar(&bval, 1);
            bval = to->weaponmodel;
            MSG_WriteBitsUChar(&bval, 8);
        }
    }
}
typedef struct packet_entities_s
{
    int num_entities;
    entity_state_t *entities;
} packet_entities_t;

typedef struct
{
    int num_entities;
    entity_state_t entities[256];
} full_packet_entities_t;

void MSG_WriteBitsUInt(unsigned int *data, int numbits);
void SV_WriteDelta(entity_state_t *from, entity_state_t *to, sizebuf_t *msg, qboolean force);

#define svc_packetentities 41
#define U_REMOVE 0x10

int SV_EmitPacketEntities(int unused, client_t *client, struct packet_entities_s *to, sizebuf_t *msg)
{
    packet_entities_t *from;
    int oldindex, newindex;
    int oldnum, newnum;
    int oldmax;
    edict_t *ent;
    unsigned int bits;
    byte bval;
    unsigned short sval;
    byte nval;
    unsigned int zero;

    from = NULL;
    oldmax = 0;

    MSG_WriteByte(msg, svc_packetentities);
    newindex = 0;
    MSG_WriteShort(msg, to->num_entities);
    MSG_StartBitWriting(msg);

    oldindex = 0;
    while (newindex < to->num_entities || oldindex < oldmax)
    {
        newnum = newindex >= to->num_entities ? 9999 : to->entities[newindex].number;
        oldnum = oldindex >= oldmax ? 9999 : from->entities[oldindex].number;

        if (newnum == oldnum)
        {
            SV_WriteDelta(&from->entities[oldindex], &to->entities[newindex], msg, false);
            oldindex++;
            newindex++;
            continue;
        }

        if (newnum < oldnum)
        {
            ent = EDICT_NUM(newnum);
            SV_WriteDelta(&ent->baseline, &to->entities[newindex], msg, true);
            newindex++;
            continue;
        }

        if (newnum > oldnum)
        {
            bits = U_REMOVE;
            if (oldnum >= 256)
                bits |= 0x8000;
            if (bits >= 0x100)
                bits |= 1;
            if (bits > 0xFFFF)
                bits |= 0x100;
            if (bits > 0xFFFFFF)
                bits |= 0x800000;

            bval = bits;
            MSG_WriteBitsUChar(&bval, 8);
            if (bits & 1)
            {
                bval = bits >> 8;
                MSG_WriteBitsUChar(&bval, 8);
            }
            if (bits & 0x100)
            {
                bval = bits >> 16;
                MSG_WriteBitsUChar(&bval, 8);
            }
            if (bits & 0x800000)
            {
                bval = bits >> 24;
                MSG_WriteBitsUChar(&bval, 8);
            }

            if (bits & 0x8000)
            {
                sval = oldnum;
                MSG_WriteBitsUShort(&sval, 10);
            }
            else
            {
                nval = oldnum;
                MSG_WriteBitsUChar(&nval, 8);
            }
            oldindex++;
        }
    }

    zero = 0;
    MSG_WriteBitsUInt(&zero, 32);
    MSG_EndBitWriting(msg);
    return msg->cursize;
}
void func_0027F910(client_t *client, packet_entities_t *pack, sizebuf_t *msg)
{
    SV_EmitPacketEntities(0, client, pack, msg);
}
extern int host_framecount;

int func_0027F940(int a0)
{
    if (svs.maxclientslimit == 1)
        return host_framecount % 30 == 0;
    if (a0 == host_framecount % svs.maxclientslimit)
        return 1;
    return 0;
}
extern int D_00399354; /* PF_COMMAND */
extern int D_00399358;
extern int D_00399360;
extern int D_00399364;
extern int D_00399368;
extern unsigned char D_0039936C;
extern int D_00399370;
extern int D_00399374;
extern int D_00399378;
extern int D_0039937C;
extern unsigned char D_00399380;
extern int D_00399384;
extern int D_00399388;
extern int D_0039938C;
extern int D_00399390;
extern int D_00399394; /* PF_MSEC */
extern int D_00399398;
extern int D_0039939C;
extern int D_003993A0;
extern unsigned char D_003993A4;
extern int D_003993A8;
extern int D_003993AC;
extern int D_003993B0;
extern unsigned char D_003993B4;
extern int D_003993B8;
extern int D_003993C0;
extern int D_003993C4;
extern int D_003993CC;
extern int D_003993D0;
extern unsigned char D_003993D4;
extern int D_003993D8;
extern int D_003993DC;
extern int D_003993E0;
extern int D_003993E4;
extern float D_00334764;
int func_0027E140(client_t *cl);
int func_0027E148(client_t *cl);
void func_00242C28(int *data, int numbits);
void func_00242508(usercmd_t *from, usercmd_t *to);
int SV_ModelIndex(char *name);

extern int D_00399354; /* PF_COMMAND */
extern int D_00399358;
extern int D_00399360;
extern int D_00399364;
extern int D_00399368;
extern unsigned char D_0039936C;
extern int D_00399370;
extern int D_00399374;
extern int D_00399378;
extern int D_0039937C;
extern unsigned char D_00399380;
extern int D_00399384;
extern int D_00399388;
extern int D_0039938C;
extern int D_00399390;
extern int D_00399394; /* PF_MSEC */
extern int D_00399398;
extern int D_0039939C;
extern int D_003993A0;
extern unsigned char D_003993A4;
extern int D_003993A8;
extern int D_003993AC;
extern int D_003993B0;
extern unsigned char D_003993B4;
extern int D_003993B8;
extern int D_003993C0;
extern int D_003993C4;
extern int D_003993CC;
extern int D_003993D0;
extern unsigned char D_003993D4;
extern int D_003993D8;
extern int D_003993DC;
extern int D_003993E0;
extern int D_003993E4;
extern float D_00334764;
int func_0027E140(client_t *cl);
int func_0027E148(client_t *cl);
void func_00242C28(int *data, int numbits);
void func_00242508(usercmd_t *from, usercmd_t *to);
int SV_ModelIndex(char *name);

extern int D_00399354; /* PF_COMMAND */
extern int D_00399358;
extern int D_00399360;
extern int D_00399364;
extern int D_00399368;
extern unsigned char D_0039936C;
extern int D_00399370;
extern int D_00399374;
extern int D_00399378;
extern int D_0039937C;
extern unsigned char D_00399380;
extern int D_00399384;
extern int D_00399388;
extern int D_0039938C;
extern int D_00399390;
extern int D_00399394; /* PF_MSEC */
extern int D_00399398;
extern int D_0039939C;
extern int D_003993A0;
extern unsigned char D_003993A4;
extern int D_003993A8;
extern int D_003993AC;
extern int D_003993B0;
extern unsigned char D_003993B4;
extern int D_003993B8;
extern int D_003993C0;
extern int D_003993C4;
extern int D_003993CC;
extern int D_003993D0;
extern unsigned char D_003993D4;
extern int D_003993D8;
extern int D_003993DC;
extern int D_003993E0;
extern int D_003993E4;
extern float D_00334764;
int func_0027E140(client_t *cl);
int func_0027E148(client_t *cl);
void func_00242C28(int *data, int numbits);
void func_00242508(usercmd_t *from, usercmd_t *to);
int SV_ModelIndex(char *name);

extern int D_00399354; /* PF_COMMAND */
extern int D_00399358;
extern int D_00399360;
extern int D_00399364;
extern int D_00399368;
extern unsigned char D_0039936C;
extern int D_00399370;
extern int D_00399374;
extern int D_00399378;
extern int D_0039937C;
extern unsigned char D_00399380;
extern int D_00399384;
extern int D_00399388;
extern int D_0039938C;
extern int D_00399390;
extern int D_00399394; /* PF_MSEC */
extern int D_00399398;
extern int D_0039939C;
extern int D_003993A0;
extern unsigned char D_003993A4;
extern int D_003993A8;
extern int D_003993AC;
extern int D_003993B0;
extern unsigned char D_003993B4;
extern int D_003993B8;
extern int D_003993C0;
extern int D_003993C4;
extern int D_003993CC;
extern int D_003993D0;
extern unsigned char D_003993D4;
extern int D_003993D8;
extern int D_003993DC;
extern int D_003993E0;
extern int D_003993E4;
extern float D_00334764;
int func_0027E140(client_t *cl);
int func_0027E148(client_t *cl);
void func_00242C28(int *data, int numbits);
void func_00242508(usercmd_t *from, usercmd_t *to);
int SV_ModelIndex(char *name);

extern int D_00399354; /* PF_COMMAND */
extern int D_00399358;
extern int D_00399360;
extern int D_00399364;
extern int D_00399368;
extern unsigned char D_0039936C;
extern int D_00399370;
extern int D_00399374;
extern int D_00399378;
extern int D_0039937C;
extern unsigned char D_00399380;
extern int D_00399384;
extern int D_00399388;
extern int D_0039938C;
extern int D_00399390;
extern int D_00399394; /* PF_MSEC */
extern int D_00399398;
extern int D_0039939C;
extern int D_003993A0;
extern unsigned char D_003993A4;
extern int D_003993A8;
extern int D_003993AC;
extern int D_003993B0;
extern unsigned char D_003993B4;
extern int D_003993B8;
extern int D_003993C0;
extern int D_003993C4;
extern int D_003993CC;
extern int D_003993D0;
extern unsigned char D_003993D4;
extern int D_003993D8;
extern int D_003993DC;
extern int D_003993E0;
extern int D_003993E4;
extern float D_00334764;
int func_0027E140(client_t *cl);
int func_0027E148(client_t *cl);
void func_00242C28(int *data, int numbits);
void func_00242508(usercmd_t *from, usercmd_t *to);
int SV_ModelIndex(char *name);

void SV_WritePlayersToClient(client_t *client, byte *pvs, sizebuf_t *msg)
{
    usercmd_t cmd;
    byte flags;
    byte bval;
    short sval;
    unsigned short usval;
    int j;
    int ival;
    int i;
    client_t *cl;
    edict_t *clent;
    edict_t *ent;
    int pflags;
    int visible;
    int forced;
    int msec;

    clent = client->edict;

    for (j = 0, cl = svs.clients; j < svs.maxclients; j++, cl++)
    {
        if (!cl->spawned && !cl->active)
            continue;

        ent = cl->edict;
        visible = 1;
        forced = func_0027F940(j);

        if (ent != clent && !(client->spec_track && client->spec_track - 1 == j))
        {
            if (cl->spectator)
            {
                visible = 0;
            }
            else if (ent->leafnums)
            {
                for (i = 0; i < ent->num_leafs; i++)
                {
                    if (pvs[ent->leafnums[i] >> 3] & (1 << (ent->leafnums[i] & 7)))
                        break;
                }
                if (i == ent->num_leafs)
                    visible = 0;
            }
        }

        if (!visible && !forced)
            continue;

        pflags = D_00399394 | D_00399354;

        if (ent->v.modelindex != sv_playermodel)
        {
            if (ent->v.modelindex >= 256)
                pflags |= D_003993CC;
            else
                pflags |= D_0039939C;
        }

        if (ent->v.weaponmodel)
            pflags |= D_003993D8;

        if (ent->v.sequence || ent->v.animtime)
            pflags |= D_00399388;

        if (ent->v.gaitsequence)
            pflags |= D_00399378;

        if (ent->v.velocity[0] || ent->v.velocity[1])
            pflags |= D_00399374;

        if (ent->v.velocity[2])
            pflags |= D_003993A0;

        if (ent->v.effects)
            pflags |= D_00399368;

        if (ent->v.framerate != 1.0f)
            pflags |= D_0039938C;

        if (ent->v.clbasevelocity[0] || ent->v.clbasevelocity[1] || ent->v.clbasevelocity[2])
            pflags |= D_0039937C;

        for (i = 0; i < 4; i++)
        {
            if (ent->v.controller[i])
                pflags |= D_003993B8 << i;
        }

        for (i = 0; i < 2; i++)
        {
            if (ent->v.blending[i])
                pflags |= D_00399364 << i;
        }

        if (ent->v.body)
            pflags |= D_00399360;

        if (ent->v.skin)
            pflags |= D_003993A8;

        if (ent->v.friction != 1.0f)
            pflags |= D_003993B0;

        if (ent->v.rendermode || ent->v.renderamt || ent->v.renderfx
            || ent->v.rendercolor[0] || ent->v.rendercolor[1] || ent->v.rendercolor[2])
            pflags |= D_003993D0;

        if (ent->v.health <= 0)
            pflags |= D_00399370;

        if (ent->v.movetype != 3)
            pflags |= D_003993DC;

        if (cl->spectator)
            pflags &= D_00399374 | D_003993A0;
        else if (ent == clent)
            pflags &= ~D_00399394;

        if (ent == clent && sv.active && cls[0].state == 5 && client->netchan.remote_address.type == 1
            && D_00334764 == 0)
            pflags |= D_00399374 | D_003993A0;

        if (forced)
        {
            if (visible)
                pflags |= D_00399384;
            else
                pflags = D_00399384;
        }

        flags = 0;
        if (ent->v.flags & 0x1000000)
            flags = D_003993D4;
        if (ent->v.flags & 0x4000)
            flags |= D_003993A4;
        if (ent->v.flags & 0x1000)
            flags |= D_003993B4;
        if (ent->v.flags & 0x4000000)
            flags |= D_0039936C;
        if (ent->v.flags & 0x800)
            flags |= D_00399380;

        MSG_WriteByte(msg, 43);

        if (pflags >= 256)
            pflags |= D_00399398;
        if (pflags > 0xFFFF)
            pflags |= D_003993E4;
        if (pflags > 0xFFFFFF)
            pflags |= D_00399358;

        MSG_WriteByte(msg, pflags);
        if (pflags & D_00399398)
            MSG_WriteByte(msg, pflags >> 8);
        if (pflags & D_003993E4)
            MSG_WriteByte(msg, pflags >> 16);
        if (pflags & D_00399358)
            MSG_WriteByte(msg, pflags >> 24);

        MSG_StartBitWriting(msg);

        if (cl->spectator)
            j |= 0x40;
        MSG_WriteBitsUChar((byte *)&j, 6);
        MSG_WriteBitsUChar(&flags, 5);

        for (i = 0; i < 3; i++)
        {
            ival = ent->v.origin[i] * 32;
            func_00242C28(&ival, 18);
        }

        bval = ent->v.frame;
        MSG_WriteBitsUChar(&bval, 8);

        if (pflags & D_00399354)
        {
            usercmd_t nullcmd;

            cmd = cl->lastcmd;

            if (ent->v.health <= 0)
            {
                cmd.viewangles[0] = 0;
                cmd.viewangles[1] = ent->v.angles[1];
                cmd.viewangles[0] = 0;
            }

            cmd.buttons = 0;
            cmd.impulse = 0;

            memset(&nullcmd, 0, sizeof(nullcmd));
            func_00242508(&cmd, &nullcmd);
        }

        if (pflags & D_00399394)
        {
            msec = 1000 * (sv.time - cl->localtime);
            if (msec > 255)
                msec = 255;
            bval = msec;
            MSG_WriteBitsUChar(&bval, 8);
        }

        if (pflags & D_00399374)
        {
            sval = ent->v.velocity[0] * 8;
            MSG_WriteBitsShort(&sval, 14);
            sval = ent->v.velocity[1] * 8;
            MSG_WriteBitsShort(&sval, 14);
        }

        if (pflags & D_003993A0)
        {
            sval = ent->v.velocity[2] * 8;
            MSG_WriteBitsShort(&sval, 14);
        }

        if (pflags & D_0039939C)
        {
            bval = ent->v.modelindex;
            MSG_WriteBitsUChar(&bval, 8);
        }
        else if (pflags & D_003993CC)
        {
            usval = ent->v.modelindex;
            MSG_WriteBitsUShort(&usval, 10);
        }

        if (pflags & D_003993A8)
        {
            usval = ent->v.skin;
            MSG_WriteBitsUShort(&usval, 10);
        }

        if (pflags & D_00399368)
        {
            usval = ent->v.effects;
            MSG_WriteBitsUShort(&usval, 10);
        }

        if ((pflags & D_003993D8) || (pflags & D_003993C0))
        {
            if (pflags & D_003993D8)
            {
                usval = SV_ModelIndex(pr_strings + ent->v.weaponmodel);
                MSG_WriteBitsUShort(&usval, 10);
            }
            else
            {
                bval = SV_ModelIndex(pr_strings + ent->v.weaponmodel);
                MSG_WriteBitsUChar(&bval, 8);
            }
        }

        if (pflags & D_003993DC)
        {
            bval = ent->v.movetype;
            MSG_WriteBitsUChar(&bval, 4);
        }

        if (pflags & D_00399388)
        {
            bval = ent->v.sequence;
            MSG_WriteBitsUChar(&bval, 8);
            bval = ent->v.animtime * 100;
            MSG_WriteBitsUChar(&bval, 8);
        }

        if (pflags & D_003993D0)
        {
            bval = ent->v.rendermode;
            MSG_WriteBitsUChar(&bval, 8);
            bval = ent->v.renderamt;
            MSG_WriteBitsUChar(&bval, 8);
            bval = ent->v.rendercolor[0];
            MSG_WriteBitsUChar(&bval, 8);
            bval = ent->v.rendercolor[1];
            MSG_WriteBitsUChar(&bval, 8);
            bval = ent->v.rendercolor[2];
            MSG_WriteBitsUChar(&bval, 8);
            bval = ent->v.renderfx;
            MSG_WriteBitsUChar(&bval, 8);
        }

        if (pflags & D_0039938C)
        {
            bval = ent->v.framerate * 16;
            MSG_WriteBitsUChar(&bval, 8);
        }

        if (pflags & D_00399360)
        {
            bval = ent->v.body;
            MSG_WriteBitsUChar(&bval, 8);
        }

        if (pflags & D_003993B8)
        {
            bval = ent->v.controller[0];
            MSG_WriteBitsUChar(&bval, 8);
        }
        if (pflags & D_00399390)
        {
            bval = ent->v.controller[1];
            MSG_WriteBitsUChar(&bval, 8);
        }
        if (pflags & D_003993AC)
        {
            bval = ent->v.controller[2];
            MSG_WriteBitsUChar(&bval, 8);
        }
        if (pflags & D_003993E0)
        {
            bval = ent->v.controller[3];
            MSG_WriteBitsUChar(&bval, 8);
        }
        if (pflags & D_00399364)
        {
            bval = ent->v.blending[0];
            MSG_WriteBitsUChar(&bval, 8);
        }
        if (pflags & D_003993C4)
        {
            bval = ent->v.blending[1];
            MSG_WriteBitsUChar(&bval, 8);
        }

        if (pflags & D_0039937C)
        {
            sval = ent->v.clbasevelocity[0] * 8;
            MSG_WriteBitsShort((short *)&usval, 16);
            sval = ent->v.clbasevelocity[1] * 8;
            MSG_WriteBitsShort((short *)&usval, 16);
            sval = ent->v.clbasevelocity[2] * 8;
            MSG_WriteBitsShort((short *)&usval, 16);
        }

        if (pflags & D_003993B0)
        {
            usval = ent->v.friction * 8;
            MSG_WriteBitsShort((short *)&usval, 16);
        }

        if (pflags & D_00399384)
        {
            usval = func_0027E140(cl);
            MSG_WriteBitsUShort(&usval, 12);
            bval = func_0027E148(cl);
            bval = (bval > 63) ? 63 : bval;
            MSG_WriteBitsUChar(&bval, 6);
        }

        if (pflags & D_00399378)
        {
            bval = ent->v.gaitsequence;
            MSG_WriteBitsUChar(&bval, 8);
        }

        MSG_EndBitWriting(msg);
    }
}
extern cvar_t D_00332718;
extern int D_0033272C;
extern int cl_numlocal;
void Cvar_RegisterVariable(cvar_t *variable);
qboolean CM_HeadnodeVisible(mnode_t *node, byte *visbits);

#define FL_CUSTOMENTITY (1 << 29)
#define EF_NODRAW 128

/* rodata não migrada: início */
__asm__(".section .rodata\n"
        ".align 3\n"
        ".globl D_0038F328\n"
        "D_0038F328:\n"
        ".asciz \"expose_problem_entities\"\n"
        ".align 2\n"
        ".align 3\n"
        ".section .text\n");
/* rodata não migrada: fim */
void SV_AddEntitiesToPacket(full_packet_entities_t *pack, int start, byte *pvs)
{
    int e;
    int i;
    int culled;
    edict_t *ent;
    entity_state_t *state;

    if (!D_0033272C)
        Cvar_RegisterVariable(&D_00332718);
    D_0033272C++;

    for (e = start; e < sv.num_edicts; e++)
    {
        ent = &sv.edicts[e];

        if (ent->v.effects == EF_NODRAW)
            continue;
        if (!ent->v.modelindex)
            continue;
        if (!pr_strings[ent->v.model])
            continue;

        if (pvs)
        {
            if (ent->leafnums)
            {
                for (i = 0; i < ent->num_leafs; i++)
                {
                    if (pvs[ent->leafnums[i] >> 3] & (1 << (ent->leafnums[i] & 7)))
                        break;
                }
                if (i == ent->num_leafs)
                    continue;
            }
            else
            {
                if (D_00332718.value == 1.0f)
                {
                    if (!ent->num_leafs)
                        culled = 1;
                    else
                        culled = (D_0033272C / cl_numlocal) & 1;
                }
                else
                {
                    culled = 0;
                    if (!CM_HeadnodeVisible(sv.worldmodel->nodes + ent->headnode, pvs))
                        culled = 1;
                }
                if (culled == 1)
                    continue;
            }
        }

        if (pack->num_entities >= 256)
        {
            Con_DPrintf("Too many entities in visible packet list.\n");
            continue;
        }

        state = &pack->entities[pack->num_entities];
        pack->num_entities++;

        state->number = e;
        state->msg_time = 0;
        state->entityType = 0;
        if (ent->v.flags & FL_CUSTOMENTITY)
            state->entityType = 1;

        VectorCopy(ent->v.origin, state->origin);
        VectorCopy(ent->v.angles, state->angles);
        VectorCopy(ent->v.mins, state->mins);
        VectorCopy(ent->v.maxs, state->maxs);
        VectorCopy(ent->v.velocity, state->velocity);

        state->modelindex = ent->v.modelindex;
        state->frame = ent->v.frame;
        state->colormap = ent->v.colormap;
        state->skin = ent->v.skin;
        state->effects = ent->v.effects;
        state->scale = ent->v.scale;
        state->solid = ent->v.solid;
        state->movetype = ent->v.movetype;
        state->animtime = ent->v.animtime;
        state->sequence = ent->v.sequence;
        state->framerate = ent->v.framerate;
        state->body = ent->v.body;

        for (i = 0; i < 4; i++)
            state->controller[i] = ent->v.controller[i];
        for (i = 0; i < 2; i++)
            state->blending[i] = ent->v.blending[i];

        state->rendermode = ent->v.rendermode;
        state->renderamt = ent->v.renderamt;
        state->renderfx = ent->v.renderfx;
        state->rendercolor.r = ent->v.rendercolor[0];
        state->rendercolor.g = ent->v.rendercolor[1];
        state->rendercolor.b = ent->v.rendercolor[2];

        if (ent->v.aiment)
            state->aiment = NUM_FOR_EDICT(ent->v.aiment);
        else
            state->aiment = 0;

        if (ent->v.weaponmodel)
            state->weaponmodel = SV_ModelIndex(pr_strings + ent->v.weaponmodel);
        else
            state->weaponmodel = 0;
    }
}
byte *SV_FatPVS(float *org);
void SV_WritePlayersToClient(client_t *client, byte *pvs, sizebuf_t *msg);
void SV_AddEntitiesToPacket(full_packet_entities_t *pack, int maxclients, byte *pvs);

void SV_WriteEntitiesToClient(client_t *client, sizebuf_t *msg)
{
    vec3_t org;
    full_packet_entities_t fullpack;
    packet_entities_t pack;
    byte *pvs;
    edict_t *clent;
    edict_t *viewent;

    clent = client->edict;
    viewent = client->pViewEntity;
    if (viewent)
    {
        VectorAdd(viewent->v.origin, viewent->v.view_ofs, org);
    }
    else
    {
        VectorAdd(clent->v.origin, clent->v.view_ofs, org);
    }

    pvs = SV_FatPVS(org);
    SV_WritePlayersToClient(client, pvs, msg);

    fullpack.num_entities = 0;
    SV_AddEntitiesToPacket(&fullpack, svs.maxclients + 1, pvs);

    pack.num_entities = fullpack.num_entities;
    pack.entities = fullpack.entities;
    func_0027F910(client, &pack, msg);
}
void SV_CleanupEnts(void)
{
    int i;
    edict_t *ent;

    for (i = 1; i < sv.num_edicts; i++)
    {
        ent = &sv.edicts[i];
        ent->v.effects &= ~2;
        ent->v.effects &= ~0x20;
    }
}
void func_00289058(void);
void MSG_WriteHangle(sizebuf_t *sb, float f);
void MSG_WriteChar(sizebuf_t *sb, int c);
int SV_ModelIndex(char *name);

#define svc_choke 44
#define svc_addangle 38
#define svc_setangle 10
#define svc_clientdata 15
#define DEFAULT_VIEWHEIGHT 22

void SV_WriteClientdataToMessage(client_t *client, sizebuf_t *msg)
{
    int bits;
    int i;
    edict_t *ent;

    ent = client->edict;

    if (client->chokecount)
    {
        MSG_WriteByte(msg, svc_choke);
        MSG_WriteByte(msg, client->chokecount);
        client->chokecount = 0;
    }

    func_00289058();

    if (ent->v.fixangle)
    {
        if (ent->v.fixangle == 2)
        {
            MSG_WriteByte(msg, svc_addangle);
            MSG_WriteHangle(msg, ent->v.avelocity[1]);
            ent->v.avelocity[1] = 0;
        }
        else
        {
            MSG_WriteByte(msg, svc_setangle);
            for (i = 0; i < 3; i++)
                MSG_WriteHangle(msg, ent->v.angles[i]);
        }
        ent->v.fixangle = 0;
    }

    bits = 0;

    if (ent->v.view_ofs[2] != DEFAULT_VIEWHEIGHT)
        bits |= 1;
    if (ent->v.idealpitch)
        bits |= 2;
    if (ent->v.weapons)
        bits |= 0x100;
    if (ent->v.flags & 0x200)
        bits |= 0x400;
    if (ent->v.waterlevel >= 2)
        bits |= 0x800;
    if (ent->v.waterlevel >= 3)
        bits |= 0x8000;
    if (ent->v.viewmodel)
        bits |= 0x200;

    for (i = 0; i < 3; i++)
    {
        if (ent->v.punchangle[i])
            bits |= (4 << i);
        if (ent->v.velocity[i])
            bits |= (0x20 << i);
    }

    MSG_WriteByte(msg, svc_clientdata);
    MSG_WriteShort(msg, bits);

    if (bits & 1)
        MSG_WriteChar(msg, ent->v.view_ofs[2]);
    if (bits & 2)
        MSG_WriteChar(msg, ent->v.idealpitch);

    for (i = 0; i < 3; i++)
    {
        if (bits & (4 << i))
            MSG_WriteHangle(msg, ent->v.punchangle[i]);
        if (bits & (0x20 << i))
            MSG_WriteChar(msg, ent->v.velocity[i] / 16);
    }

    if (bits & 0x100)
        MSG_WriteLong(msg, ent->v.weapons);
    if (bits & 0x200)
        MSG_WriteShort(msg, SV_ModelIndex(pr_strings + ent->v.viewmodel));

    MSG_WriteShort(msg, ent->v.health);
}
void Netchan_Transmit(netchan_t *chan, int length, byte *data, int unk);
void SV_WriteClientdataToMessage(client_t *client, sizebuf_t *msg);
void SV_WriteEntitiesToClient(client_t *client, sizebuf_t *msg);

#define svc_time 7

qboolean SV_SendClientDatagram(client_t *client, int unk)
{
    byte buf[4000];
    sizebuf_t msg;

    msg.data = buf;
    msg.maxsize = sizeof(buf);
    msg.cursize = 0;
    msg.allowoverflow = true;
    msg.overflowed = false;

    MSG_WriteByte(&msg, svc_time);
    MSG_WriteFloat(&msg, sv.time);

    SV_WriteClientdataToMessage(client, &msg);
    SV_WriteEntitiesToClient(client, &msg);

    if (client->datagram.overflowed)
        Con_Printf("WARNING: datagram overflowed for %s\n", client->name);
    else
        SZ_Write(&msg, client->datagram.data, client->datagram.cursize);
    SZ_Clear(&client->datagram);

    if (msg.overflowed)
    {
        Con_Printf("WARNING: msg overflowed for %s\n", client->name);
        SZ_Clear(&msg);
    }

    Netchan_Transmit(&client->netchan, msg.cursize, buf, unk);
    return true;
}
void SV_ExtractFromUserinfo(client_t *cl);

void SV_UpdateToReliableMessages(void)
{
    int i;
    int j;
    client_t *client;
    UserMsg *pTail;

    for (i = 0, host_client = svs.clients; i < svs.maxclients; i++, host_client++)
    {
        if (!host_client->edict)
            continue;

        if (host_client->sendinfo && host_client->sendinfo_time <= realtime)
        {
            host_client->sendinfo = false;
            host_client->sendinfo_time = realtime + 1.0f;
            SV_ExtractFromUserinfo(host_client);
            SV_FullClientUpdate(host_client, &sv.reliable_datagram);
        }

        if (host_client->fakeclient)
            continue;
        if ((host_client->active || host_client->connected) && sv_gpNewUserMsgs)
            SV_SendUserReg(&host_client->netchan.message);
    }

    if (sv_gpNewUserMsgs)
    {
        if (!sv_gpUserMsgs)
        {
            sv_gpUserMsgs = sv_gpNewUserMsgs;
        }
        else
        {
            pTail = sv_gpUserMsgs;
            while (pTail->next)
                pTail = pTail->next;
            pTail->next = sv_gpNewUserMsgs;
        }
        sv_gpNewUserMsgs = NULL;
    }

    if (sv.datagram.overflowed)
    {
        Con_DPrintf("sv.datagram overflowed!\n");
        SZ_Clear(&sv.datagram);
    }

    for (j = 0, client = svs.clients; j < svs.maxclients; j++, client++)
    {
        if (!client->active || client->fakeclient)
            continue;
        SZ_Write(&client->netchan.message, sv.reliable_datagram.data, sv.reliable_datagram.cursize);
        SZ_Write(&client->datagram, sv.datagram.data, sv.datagram.cursize);
    }

    SZ_Clear(&sv.reliable_datagram);
    SZ_Clear(&sv.datagram);
}
void SV_UpdateToReliableMessages(void);

void SV_SendClientMessages(void)
{
    int i;

    SV_UpdateToReliableMessages();

    for (i = 0, host_client = svs.clients; i < svs.maxclients; i++, host_client++)
    {
        if (!host_client->active && !host_client->connected && !host_client->spawned)
            continue;
        if (host_client->fakeclient)
            continue;
        if (!host_client->send_message)
            continue;

        host_client->send_message = false;
        if (host_client->active)
            SV_SendClientDatagram(host_client, i);
        else
            Netchan_Transmit(&host_client->netchan, 0, NULL, i);
    }

    SV_CleanupEnts();
}
char *Info_ValueForKey(char *s, char *key);

void SV_ExtractFromUserinfo(client_t *cl)
{
    char *val;
    char *p, *q;
    int i;
    char newname[80];

    gEntityInterface.pfnClientUserInfoChanged(cl->edict, cl->userinfo);

    val = Info_ValueForKey(cl->userinfo, "name");

    strncpy(newname, val, sizeof(newname) - 1);
    newname[sizeof(newname) - 1] = 0;

    for (p = newname; *p == ' ' && *p; p++)
        ;
    if (p != newname && *p)
    {
        for (q = newname; *p; *q++ = *p++)
            ;
        *q = 0;
    }

    for (p = newname + strlen(newname) - 1; p != newname && *p == ' '; p--)
        ;
    p[1] = 0;

    if (strcmp(val, newname))
    {
        Info_SetValueForKey(cl->userinfo, "name", newname, MAX_INFO_STRING);
        val = Info_ValueForKey(cl->userinfo, "name");
    }

    if (!val[0] || !strcasecmp(val, "console"))
    {
        Info_SetValueForKey(cl->userinfo, "name", "unnamed", MAX_INFO_STRING);
        val = Info_ValueForKey(cl->userinfo, "name");
    }

    strncpy(cl->name, val, sizeof(cl->name) - 1);

    val = Info_ValueForKey(cl->userinfo, "rate");
    if (strlen(val))
    {
        i = atoi(val);
        if (i < 500)
            i = 500;
        else if (i > 10000)
            i = 10000;
        cl->netchan.rate = i;
    }

    val = Info_ValueForKey(cl->userinfo, "topcolor");
    if (strlen(val))
        cl->topcolor = atoi(val);

    val = Info_ValueForKey(cl->userinfo, "bottomcolor");
    if (strlen(val))
        cl->bottomcolor = atoi(val);
}
int SV_ModelIndex(char *name)
{
    int i;

    if (!name || !name[0])
        return 0;

    for (i = 0; i < 512 && sv.model_precache[i]; i++)
    {
        if (!Q_strcasecmp(sv.model_precache[i], name))
            return i;
    }
    if (i == 512 || !sv.model_precache[i])
        Sys_Error("SV_ModelIndex: model %s not precached", name);
    return i;
}
void SV_FlushSignon(void)
{
    if (sv.signon.cursize < sv.signon.maxsize - 100)
        return;

    if (sv.num_signon_buffers == 15)
        Sys_Error("sv.num_signon_buffers == MAX_SIGNON_BUFFERS-1");

    sv.signon_buffer_size[sv.num_signon_buffers - 1] = sv.signon.cursize;
    sv.signon.cursize = 0;
    sv.signon.data = sv.signon_buffers[sv.num_signon_buffers];
    sv.num_signon_buffers++;
}
void Cmd_ForwardToServer(void);

#define svc_resourcelist 45

void SV_SendResourceListBlock_f(void)
{
    int nStart;
    int n;
    int nSizePos;
    short nCount;

    if (cmd_source == 1)
    {
        Cmd_ForwardToServer();
        return;
    }

    if (!host_client->connected)
    {
        Con_Printf("resourcelist not valid -- already spawned\n");
        return;
    }

    if (atoi(Cmd_Argv(1)) != svs.spawncount)
    {
        Con_Printf("SV_SendResourceListBlock_f from different level\n");
        SV_New_f();
        return;
    }

    nStart = atoi(Cmd_Argv(2));

    MSG_WriteByte(&host_client->netchan.message, svc_resourcelist);
    MSG_WriteShort(&host_client->netchan.message, sv.num_resources);
    MSG_WriteShort(&host_client->netchan.message, nStart);
    nSizePos = host_client->netchan.message.cursize;
    MSG_WriteShort(&host_client->netchan.message, 0);

    for (n = nStart; n < sv.num_resources &&
           host_client->netchan.message.cursize < host_client->netchan.message.maxsize - 512; n++)
    {
        MSG_WriteByte(&host_client->netchan.message, sv.resourcelist[n].type);
        MSG_WriteString(&host_client->netchan.message, sv.resourcelist[n].szFileName);
        MSG_WriteShort(&host_client->netchan.message, sv.resourcelist[n].nIndex);
        MSG_WriteByte(&host_client->netchan.message, sv.resourcelist[n].ucFlags);
    }

    nCount = n;
    Q_memcpy(host_client->netchan.message.data + nSizePos, &nCount, 2);

    if (n < sv.num_resources)
    {
        MSG_WriteByte(&host_client->netchan.message, svc_stufftext);
        MSG_WriteString(&host_client->netchan.message, va("cmd resourcelist %i %i\n", svs.spawncount, n));
    }
}
void SV_AddResource(int type, char *name, int size, int flags, int index)
{
    resource_t *r = &sv.resourcelist[sv.num_resources];

    if (sv.num_resources >= 1280)
        Sys_Error("Too many resources on server.");
    sv.num_resources++;
    r->type = type;
    strcpy(r->szFileName, name);
    r->ucFlags |= flags != 0;
    r->nIndex = index;
}
#include "engine/decals.h"
int Draw_DecalSize(int number);

void SV_CreateResourceList(void)
{
    char **s;
    int i;
    qboolean ffirstsent = false;

    sv.num_resources = 0;

    for (i = 1, s = &sv.sound_precache[1]; *s; s++, i++)
    {
        if (**s != '!')
        {
            SV_AddResource(0, *s, 0, 0, i);
        }
        else if (!ffirstsent)
        {
            ffirstsent = true;
            SV_AddResource(0, "!", 0, 1, i);
        }
    }

    for (s = &sv.model_precache[1], i = 1; *s; s++, i++)
        SV_AddResource(2, *s, 0, 1, i);

    for (i = 0; i < sv_decalnamecount; i++)
        SV_AddResource(3, sv_decalnames[i].name, Draw_DecalSize(i), 0, i);

    for (s = &sv.generic_precache[1], i = 1; *s; s++, i++)
        SV_AddResource(4, *s, 0, 1, i);
}
void func_00281D88(void)
{
}
void func_00281D90(void)
{
}
void MSG_WriteWord(sizebuf_t *sb, int c);
void SV_FlushSignon(void);

#define svc_spawnbaseline 22

void SV_CreateBaseline(void)
{
    int i;
    edict_t *svent;
    int entnum;

    for (entnum = 0; entnum < sv.num_edicts; entnum++)
    {
        svent = &sv.edicts[entnum];
        if (svent->free)
            continue;
        if (entnum > svs.maxclients && !svent->v.modelindex)
            continue;

        svent->baseline.entityType = 0;
        if (svent->v.flags & FL_CUSTOMENTITY)
            svent->baseline.entityType = 1;

        VectorCopy(svent->v.origin, svent->baseline.origin);
        VectorCopy(svent->v.angles, svent->baseline.angles);
        VectorCopy(svent->v.mins, svent->baseline.mins);
        VectorCopy(svent->v.maxs, svent->baseline.maxs);
        svent->baseline.frame = svent->v.frame;
        svent->baseline.scale = svent->v.scale;
        svent->baseline.skin = svent->v.skin;
        svent->baseline.solid = (byte)svent->v.solid;

        if (entnum > 0 && entnum <= svs.maxclients)
        {
            svent->baseline.colormap = entnum;
        }
        else
        {
            svent->baseline.colormap = 0;
            svent->baseline.modelindex = SV_ModelIndex(pr_strings + svent->v.model);
        }

        svent->baseline.rendermode = (byte)svent->v.rendermode;
        svent->baseline.renderamt = (byte)svent->v.renderamt;
        svent->baseline.rendercolor.r = svent->v.rendercolor[0];
        svent->baseline.rendercolor.g = svent->v.rendercolor[1];
        svent->baseline.rendercolor.b = svent->v.rendercolor[2];
        svent->baseline.renderfx = (byte)svent->v.renderfx;

        SV_FlushSignon();

        MSG_WriteByte(&sv.signon, svc_spawnbaseline);
        MSG_WriteShort(&sv.signon, entnum);
        MSG_WriteByte(&sv.signon, svent->baseline.entityType);
        MSG_WriteShort(&sv.signon, svent->baseline.modelindex);

        MSG_WriteByte(&sv.signon, svent->baseline.sequence);
        svent->baseline.sequence = (byte)svent->baseline.sequence;
        MSG_WriteByte(&sv.signon, svent->baseline.frame);
        svent->baseline.frame = (byte)svent->baseline.frame;

        if (svent->baseline.entityType == 0)
            MSG_WriteWord(&sv.signon, svent->baseline.scale * 256);
        else
            MSG_WriteByte(&sv.signon, svent->baseline.scale);

        MSG_WriteWord(&sv.signon, svent->baseline.colormap);
        MSG_WriteShort(&sv.signon, svent->baseline.skin);
        MSG_WriteByte(&sv.signon, svent->baseline.solid);
        svent->baseline.solid = (byte)svent->baseline.solid;

        for (i = 0; i < 3; i++)
        {
            MSG_WriteCoord(&sv.signon, svent->baseline.origin[i]);
            MSG_WriteFloat(&sv.signon, svent->baseline.angles[i]);
            MSG_WriteCoord(&sv.signon, svent->baseline.mins[i]);
            MSG_WriteCoord(&sv.signon, svent->baseline.maxs[i]);
        }

        MSG_WriteByte(&sv.signon, svent->v.rendermode);
        MSG_WriteByte(&sv.signon, svent->v.renderamt);
        MSG_WriteByte(&sv.signon, svent->v.rendercolor[0]);
        MSG_WriteByte(&sv.signon, svent->v.rendercolor[1]);
        MSG_WriteByte(&sv.signon, svent->v.rendercolor[2]);
        MSG_WriteByte(&sv.signon, svent->v.renderfx);
    }
}
#include <stdarg.h>
int vsprintf(char *buf, const char *fmt, va_list ap);

#define svc_stufftext 9

void SV_BroadcastCommand(char *fmt, ...)
{
    va_list argptr;
    char string[1024];
    byte data[128];
    sizebuf_t msg;
    int i;
    client_t *cl;

    msg.data = data;
    msg.maxsize = sizeof(data);
    msg.cursize = 0;

    if (!sv.active)
        return;

    va_start(argptr, fmt);
    vsprintf(string, fmt, argptr);
    va_end(argptr);

    MSG_WriteByte(&msg, svc_stufftext);
    MSG_WriteString(&msg, string);

    for (i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
    {
        if (!cl->active && !cl->connected && !cl->spawned)
            continue;
        if (cl->fakeclient)
            continue;
        SZ_Write(&cl->netchan.message, msg.data, msg.cursize);
    }
}
void SV_ReconnectAllClients(void)
{
    SV_BroadcastCommand("reconnect\n");
}
int NUM_FOR_EDICT(edict_t *e);

void SV_SaveSpawnparms(SAVERESTOREDATA *pSaveData)
{
    int i;

    svs.serverflags = gGlobalVariables.serverflags;
    gEntityInterface.pfnParmsChangeLevel();

    for (i = 0, host_client = svs.clients; i < svs.maxclients; i++, host_client++)
    {
        if (!host_client->active && !host_client->connected)
            continue;

        host_client->unk_8538 = pSaveData->size;
        pSaveData->currentIndex = NUM_FOR_EDICT(host_client->edict);
        gEntityInterface.pfnSave(host_client->edict, pSaveData);
    }
}
extern char D_0038E6C0[]; /* "sv_newunit" */
extern float host_frametime;
void Cvar_Set(char *var_name, char *value);
void SV_Physics(void);
void SV_CreateBaseline(void);

void SV_ActivateServer(int runPhysics)
{
    int i;
    UserMsg *pTemp;

    Cvar_Set(D_0038E6C0, D_0038E630);

    gEntityInterface.pfnServerActivate(sv.edicts, sv.num_edicts, svs.maxclients);

    sv.active = true;
    sv.state = 2;

    if (runPhysics)
    {
        host_frametime = 0.1f;
        SV_Physics();
        SV_Physics();
    }
    else
    {
        host_frametime = 0.001f;
        SV_Physics();
    }

    SV_CreateBaseline();

    sv.signon_buffer_size[sv.num_signon_buffers - 1] = sv.signon.cursize;

    SV_CreateResourceList();

    for (i = 0, host_client = svs.clients; i < svs.maxclients; i++, host_client++)
    {
        if (!host_client->active && !host_client->connected)
            continue;

        if (svs.maxclients <= 1)
            SV_SendServerinfo(host_client);

        if (sv_gpUserMsgs && !host_client->fakeclient)
        {
            pTemp = sv_gpNewUserMsgs;
            sv_gpNewUserMsgs = sv_gpUserMsgs;
            SV_SendUserReg(&host_client->netchan.message);
            sv_gpNewUserMsgs = pTemp;
        }
    }

    if (svs.maxclients > 1)
        Con_DPrintf("%i player server started\n", svs.maxclients);
    else
        Con_DPrintf("Game started\n");
}
extern char D_0038E6F8[]; /* "sv_clienttrace" */
extern cvar_t coop;
extern cvar_t deathmatch;
extern cvar_t skill;
extern int D_003995D0;
extern int D_00332F2C;
extern int D_003994B8;
extern char *D_003323C8;
extern int D_00332F34;
extern int D_00399410;
void SV_ReconnectAllClients(void);
void Log_Open(void);
void Log_Printf(char *fmt, ...);
void Host_PrintServerCvars(void);
void func_002C70A8(qboolean multiplayer);
void Cvar_SetValue(char *var_name, float value);
void Host_ClearMemory(qboolean bQuiet);
int func_00245408(int maxclients);
void *Hunk_AllocName(int size, char *name);
model_t *Mod_ForName(char *name, qboolean crash);
void func_002A47F0(void);
void CRC32_Init(unsigned int *pulCRC);
qboolean CRC_MapFile(unsigned int *crcvalue, char *pszFileName);
qboolean CRC_File(unsigned int *crcvalue, char *pszFileName);
void func_0028F5C8(void);
void func_002A40E8(void);
void func_002A4110(msurface_t *surf);
int func_002A3C00(void *p);
void func_002A4180(void);
void func_002A40B8(int total);
void func_00236108(char *server);
int func_002E0F18(float f);

extern char D_004D7FC0[512][5];
extern cvar_t D_00332548;

#ifdef NON_MATCHING
int SV_SpawnServer(int unused, char *server, char *startspot)
{
    edict_t *ent;
    edict_t *world;
    int i;
    int k;
    int j;
    int total;
    client_t *cl;
    model_t *mod;
    char szDllName[64];

    if (sv.active && svs.maxclients > 1)
    {
        SV_ReconnectAllClients();

        for (i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
        {
            if (!cl->active && !cl->spawned && !cl->connected)
                continue;

            ent = cl->edict;
            if (!ent || ent->free)
                continue;

            if (!ent->pvPrivateData)
            {
                Con_Printf("Skipping reconnect on %s, no pvPrivateData\n", cl->name);
                continue;
            }

            if (cl->spectator)
                gEntityInterface.pfnSpectatorDisconnect(ent);
            else
                gEntityInterface.pfnClientDisconnect(ent);
        }
    }

    Log_Open();
    Log_Printf("Spawning server \"%s\"\n", server);
    Host_PrintServerCvars();
    func_002C70A8(svs.maxclients > 1);

    if (!hostname.string[0])
        Cvar_Set("hostname", "Half-Life");

    D_003995D0 = 0;

    if (startspot)
        Con_DPrintf("Spawn Server %s: [%s]\n", server, startspot);
    else
        Con_DPrintf("Spawn Server %s\n", server);

    D_00332F2C = ++svs.spawncount;

    if (coop.value)
        Cvar_SetValue("deathmatch", 0);

    D_003994B8 = (int)(skill.value + 0.5f);
    if (D_003994B8 < 0)
        D_003994B8 = 0;
    if (D_003994B8 > 3)
        D_003994B8 = 3;
    Cvar_SetValue("skill", (float)D_003994B8);

    Host_ClearMemory(false);

    memset(&sv, 0, sizeof(sv));
    strcpy(sv.name, server);
    if (startspot)
        strcpy(sv.startspot, startspot);
    else
        sv.startspot[0] = 0;

    pr_strings = gGlobalVariables.pStringBase = D_003323C8;

    if (svs.maxclients == 1)
        Cvar_SetValue(D_0038E6F8, 1.0f);

    sv.max_edicts = func_00245408(svs.maxclients);

    SV_DeallocateDynamicData();
    SV_ReallocateDynamicData();

    gGlobalVariables.maxEntities = sv.max_edicts;

    sv.edicts = Hunk_AllocName(sv.max_edicts * sizeof(edict_t), "edicts");

    sv.datagram.maxsize = sizeof(sv.datagram_buf);
    sv.datagram.cursize = 0;
    sv.datagram.data = sv.datagram_buf;

    sv.reliable_datagram.maxsize = sizeof(sv.reliable_datagram_buf);
    sv.reliable_datagram.cursize = 0;
    sv.reliable_datagram.data = sv.reliable_datagram_buf;

    sv.unk_1D964.maxsize = sizeof(sv.unk_1D978);
    sv.unk_1D964.data = sv.unk_1D978;

    sv.multicast.maxsize = sizeof(sv.multicast_buf);
    sv.multicast.data = sv.multicast_buf;

    sv.signon.maxsize = sizeof(sv.signon_buffers[0]);
    sv.signon.cursize = 0;
    sv.signon.data = sv.signon_buffers[0];

    sv.num_signon_buffers = 1;

    sv.num_edicts = svs.maxclients + 1;
    for (i = 0; i < svs.maxclients; i++)
        {
        ent = sv.edicts + i + 1;
        svs.clients[i].edict = ent;
    }

    D_00332F34 = 1;
    gGlobalVariables.maxClients = svs.maxclients;

    sv.state = 1;
    gGlobalVariables.time = 1.0f;
    sv.time = 1.0f;
    sv.paused = false;

    strcpy(sv.name, server);
    sprintf(sv.modelname, "maps/%s.bs2", server);
    sv.worldmodel = Mod_ForName(sv.modelname, false);
    func_002A47F0();

    if (!sv.worldmodel)
    {
        Con_Printf("Couldn't spawn server %s\n", sv.modelname);
        sv.active = false;
        return 0;
    }

    if (svs.maxclients > 1)
    {
        CRC32_Init(&sv.worldmapCRC);
        if (!CRC_MapFile(&sv.worldmapCRC, sv.modelname))
        {
            Con_Printf("Couldn't CRC server map:  %s\n", sv.modelname);
            sv.active = false;
            return 0;
        }

        sprintf(szDllName, "cl_dlls/client.dll");
        CRC32_Init(&sv.clientdllmd5);
        if (!CRC_File(&sv.clientdllmd5, szDllName))
        {
            Con_Printf("Couldn't CRC client side dll:  %s\n", szDllName);
            sv.active = false;
            return 0;
        }
    }
    else
    {
        sv.worldmapCRC = 0;
        sv.clientdllmd5 = 0;
    }

    sv.models[1] = sv.worldmodel;
    func_0028F5C8();

    sv.model_precache[1] = sv.modelname;
    sv.sound_precache[0] = pr_strings;
    sv.model_precache[0] = pr_strings;

    for (i = 1; i < sv.worldmodel->numsubmodels; i++)
    {
        sv.model_precache[1 + i] = D_004D7FC0[i];
        sv.models[i + 1] = Mod_ForName(D_004D7FC0[i], false);
    }

    func_002A40E8();
    total = 0;
    for (k = 1; k < 512 && (mod = sv.models[k]); k++)
    {
        if (mod->name[0] == '*')
            continue;
        for (j = 0; j < mod->numsurfaces; j++)
        {
            func_002A4110(&mod->surfaces[j]);
            if (!(mod->surfaces[j].flags & 0x110))
                total += func_002A3C00(mod->surfaces[j].unk_38);
        }
    }
    func_002A4180();
    func_002A40B8(total);

    func_00236108(server);

    world = sv.edicts;
    memset(&world->v, 0, sizeof(entvars_t));
    world->free = false;
    world->v.model = sv.worldmodel->name - pr_strings;
    world->v.modelindex = 1;
    world->v.solid = 4;
    world->v.movetype = 7;

    gGlobalVariables.deathmatch = 0;
    gGlobalVariables.coop = 0;
    if (coop.value)
        gGlobalVariables.coop = coop.value;
    else
        gGlobalVariables.deathmatch = deathmatch.value;

    gGlobalVariables.serverflags = svs.serverflags;
    gGlobalVariables.mapname = sv.name - pr_strings;
    gGlobalVariables.startspot = sv.startspot - pr_strings;

    D_00399410 = func_002E0F18(D_00332548.value);
    SV_SetMoveVars();

    return 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", SV_SpawnServer);
#endif
void SV_LoadEntities(void)
{
    ED_LoadFromFile(sv.worldmodel->entities);
}
void ReleaseEntityDLLFields(edict_t *pEdict);

void SV_ClearEntities(void)
{
    int i;

    for (i = 0; i < sv.num_edicts; i++)
    {
        edict_t *ent = sv.edicts + i;
        if (!ent->free)
            ReleaseEntityDLLFields(ent);
    }
}
extern int giNextUserMsg;

#ifdef NON_MATCHING
int RegUserMsg(const char *pszName, int iSize)
{
    UserMsg *pUserMsgs;
    qboolean found = false;
    int iMsg;

    if (giNextUserMsg >= 256)
        return 0;
    if (!pszName)
        return 0;
    if (strlen(pszName) > 11)
        return 0;
    if (iSize > 192)
        return 0;

    for (pUserMsgs = sv_gpUserMsgs; pUserMsgs; pUserMsgs = pUserMsgs->next)
    {
        if (!strcmp(pszName, pUserMsgs->szName))
        {
            found = true;
            break;
        }
    }

    if (found)
        iMsg = pUserMsgs->iMsg;
    else
    {
    for (pUserMsgs = sv_gpNewUserMsgs; pUserMsgs; pUserMsgs = pUserMsgs->next)
    {
        if (!strcmp(pszName, pUserMsgs->szName))
        {
            found = true;
            break;
        }
    }

        if (found)
            iMsg = pUserMsgs->iMsg;
        else
        {
    pUserMsgs = Mem_DbgMalloc(sizeof(UserMsg), "engine/Sv_main.c", 5818);
    pUserMsgs->iMsg = giNextUserMsg;
                giNextUserMsg++;
    pUserMsgs->iSize = iSize;
    strcpy(pUserMsgs->szName, pszName);
    if (!pUserMsgs)
        return 0;
    pUserMsgs->next = sv_gpNewUserMsgs;
    sv_gpNewUserMsgs = pUserMsgs;
            iMsg = pUserMsgs->iMsg;
        }
    }
    return iMsg;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/sv_main", RegUserMsg);
#endif
#define svc_newusermsg 39

void SV_SendUserReg(sizebuf_t *msg)
{
    UserMsg *pMsg;

    if (!sv_gpNewUserMsgs)
        return;
    pMsg = sv_gpNewUserMsgs;
    do
    {
        MSG_WriteByte(msg, svc_newusermsg);
        MSG_WriteByte(msg, pMsg->iMsg);
        MSG_WriteByte(msg, pMsg->iSize);
        MSG_WriteLong(msg, *(int *)&pMsg->szName[0]);
        MSG_WriteLong(msg, *(int *)&pMsg->szName[4]);
        MSG_WriteLong(msg, *(int *)&pMsg->szName[8]);
        MSG_WriteLong(msg, *(int *)&pMsg->szName[12]);
        pMsg = pMsg->next;
    } while (pMsg);
}
void func_00283018(void)
{
    char szLine[1024];
    int i;
    client_t *cl;

    if (!sv.active)
    {
        Con_Printf("Not running a server\n");
        return;
    }

    Con_Printf("CD Keys =================\n");
    for (i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
    {
        if (!cl->active && !cl->connected && !cl->spawned)
            continue;
        if (cl->fakeclient)
            continue;
        sprintf(szLine, "%3i %s : %s\n", i + 1, cl->name, cl->hashedcdkey);
        Con_Printf(szLine);
    }
    Con_Printf("==========================\n");
}
void func_00283130(void)
{
    int i;
    client_t *cl;
    customization_t *pCust;

    Con_Printf("Server Rep. of Player Logos ===\n");
    for (i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
    {
        if (!cl->active && !cl->connected && !cl->spawned)
            continue;
        if (cl->fakeclient)
            continue;
        for (pCust = cl->customdata.pNext; pCust; pCust = pCust->pNext)
            ;
    }
    Con_Printf("==========================\n");
}
void Pmove_Init(void);
void func_0028B308(void);
extern cvar_t D_003326F8;
extern cvar_t D_00332968;
extern cvar_t sv_aim;
extern cvar_t D_00332428;
extern cvar_t violence_hblood;
extern cvar_t D_00332458;
extern cvar_t D_00332470;
extern cvar_t D_00332488;
extern cvar_t D_003324A0;
extern cvar_t sv_gravity;
extern cvar_t sv_friction;
extern cvar_t sv_edgefriction;
extern cvar_t sv_stopspeed;
extern cvar_t sv_maxspeed;
extern cvar_t sv_accelerate;
extern cvar_t sv_stepsize;
extern cvar_t D_003327A8;
extern cvar_t sv_bounce;
extern cvar_t D_003327D8;
extern cvar_t sv_airaccelerate;
extern cvar_t sv_wateraccelerate;
extern cvar_t sv_waterfriction;
extern cvar_t D_003327F0;
extern cvar_t D_00332808;
extern cvar_t D_00332820;
extern cvar_t D_00332838;
extern cvar_t D_00332530;
extern cvar_t D_00332500;
extern cvar_t sv_zmax;
extern cvar_t sv_wateramp;
extern cvar_t sv_skyname;
extern cvar_t sv_maxvelocity;
extern cvar_t D_003324D0;
extern cvar_t D_00332560;
extern cvar_t D_00332578;
extern cvar_t D_00332590;
extern cvar_t sv_spectatormaxspeed;
extern cvar_t D_003326E0;
extern cvar_t pm_pushfix;
extern cvar_t D_003326C8;
extern cvar_t D_00332698;
extern cvar_t D_003326B0;
extern cvar_t D_003325D8;
extern cvar_t D_003325F0;
extern cvar_t D_00332608;
extern cvar_t D_00332620;
extern cvar_t D_00332638;
extern cvar_t D_00332650;
extern cvar_t D_00332668;

void SV_Init(void)
{
    int i;
    client_t *cl;

    Cmd_AddCommand("logaddress", SV_SetLogAddress_f);
    Cmd_AddCommand("log", SV_ServerLog_f);
    Cmd_AddCommand("serverinfo", SV_Serverinfo_f);
    Cmd_AddCommand("localinfo", SV_Localinfo_f);
    Cmd_AddCommand("leafdatatrace", func_0027BF20);
    Cmd_AddCommand("showinfo", func_0028B308);
    Cmd_AddCommand("user", SV_User_f);
    Cmd_AddCommand("users", SV_Users_f);

    Cvar_RegisterVariable(&sv_logrelay);
    Cvar_RegisterVariable(&D_003326F8);
    Cvar_RegisterVariable(&sv_lan);
    Cvar_RegisterVariable(&sv_password);
    Cvar_RegisterVariable(&D_00332968);
    Cvar_RegisterVariable(&sv_aim);
    Cvar_RegisterVariable(&D_00332428);
    Cvar_RegisterVariable(&violence_hblood);
    Cvar_RegisterVariable(&D_00332458);
    Cvar_RegisterVariable(&D_00332470);
    Cvar_RegisterVariable(&D_00332488);
    Cvar_RegisterVariable(&D_003324A0);
    Cvar_RegisterVariable(&sv_gravity);
    Cvar_RegisterVariable(&sv_friction);
    Cvar_RegisterVariable(&sv_edgefriction);
    Cvar_RegisterVariable(&sv_stopspeed);
    Cvar_RegisterVariable(&sv_maxspeed);
    Cvar_RegisterVariable(&sv_accelerate);
    Cvar_RegisterVariable(&sv_stepsize);
    Cvar_RegisterVariable(&D_003327A8);
    Cvar_RegisterVariable(&sv_bounce);
    Cvar_RegisterVariable(&D_003327D8);
    Cvar_RegisterVariable(&sv_airaccelerate);
    Cvar_RegisterVariable(&sv_wateraccelerate);
    Cvar_RegisterVariable(&sv_waterfriction);
    Cvar_RegisterVariable(&D_003327F0);
    Cvar_RegisterVariable(&D_00332808);
    Cvar_RegisterVariable(&D_00332820);
    Cvar_RegisterVariable(&D_00332838);
    Cvar_RegisterVariable(&D_00332530);
    Cvar_RegisterVariable(&sv_timeout);
    Cvar_RegisterVariable(&D_00332500);
    Cvar_RegisterVariable(&sv_zmax);
    Cvar_RegisterVariable(&sv_wateramp);
    Cvar_RegisterVariable(&sv_skyname);
    Cvar_RegisterVariable(&sv_maxvelocity);
    Cvar_RegisterVariable(&D_003324D0);
    Cvar_RegisterVariable(&D_00332548);
    Cvar_RegisterVariable(&D_00332560);
    Cvar_RegisterVariable(&D_00332578);
    Cvar_RegisterVariable(&D_00332590);
    Cvar_RegisterVariable(&sv_spectatormaxspeed);
    Cvar_RegisterVariable(&sv_netsize);
    Cvar_RegisterVariable(&D_003326E0);
    Cvar_RegisterVariable(&pm_pushfix);
    Cvar_RegisterVariable(&D_003326C8);
    Cvar_RegisterVariable(&D_00332698);
    Cvar_RegisterVariable(&D_003326B0);
    Cvar_RegisterVariable(&sv_minrate);
    Cvar_RegisterVariable(&sv_maxrate);
    Cvar_RegisterVariable(&D_003325D8);
    Cvar_RegisterVariable(&D_003325F0);
    Cvar_RegisterVariable(&D_00332608);
    Cvar_RegisterVariable(&D_00332620);
    Cvar_RegisterVariable(&D_00332638);
    Cvar_RegisterVariable(&D_00332650);
    Cvar_RegisterVariable(&D_00332668);

    Pmove_Init();

    for (i = 0; i < 512; i++)
        sprintf(D_004D7FC0[i], "*%i", i);

    for (i = 0; i < svs.maxclientslimit; i++)
    {
        cl = &svs.clients[i];
        memset(cl, 0, sizeof(client_t));
        cl->resourcesonhand.pNext = cl->resourcesonhand.pPrev = &cl->resourcesonhand;
        cl->resourcesneeded.pNext = cl->resourcesneeded.pPrev = &cl->resourcesneeded;
    }
}
