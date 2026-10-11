#include "common.h"
#include "engine/cmd.h"
#include "engine/cvar.h"
#include "engine/server.h"
#include "engine/client.h"

/* cl_parse.c: interpretação das mensagens do servidor no cliente (um cl[] por jogador local) */

#define MAX_LOCAL 2
#define MAX_QPATH 64
#define MAX_OSPATH 260

/* svc_clientdata */
#define SU_VIEWHEIGHT (1 << 0)
#define SU_IDEALPITCH (1 << 1)
#define SU_PUNCH1 (1 << 2)
#define SU_VELOCITY1 (1 << 5)
#define SU_ITEMS (1 << 8)
#define SU_WEAPONFRAME (1 << 9)
#define SU_ONGROUND (1 << 10)
#define SU_INWATER (1 << 11)
#define SU_FULLYINWATER (1 << 15)
#define DEFAULT_VIEWHEIGHT 22
#define YAW 1
#define MAX_LIGHTSTYLES 64

/* comandos do servidor (protocolo do PS2) */
#define svc_bad 0
#define svc_nop 1
#define svc_disconnect 2
#define svc_updatestat 3
#define svc_version 4
#define svc_setview 5
#define svc_sound 6
#define svc_time 7
#define svc_print 8
#define svc_stufftext 9
#define svc_setangle 10
#define svc_serverinfo 11
#define svc_lightstyle 12
#define svc_updateuserinfo 13
#define svc_clientdata 15
#define svc_stopsound 16
#define svc_particle 18
#define svc_damage 19
#define svc_spawnstatic 20
#define svc_spawnbaseline 22
#define svc_temp_entity 23
#define svc_setpause 24
#define svc_signonnum 25
#define svc_centerprint 26
#define svc_killedmonster 27
#define svc_foundsecret 28
#define svc_spawnstaticsound 29
#define svc_intermission 30
#define svc_finale 31
#define svc_cdtrack 32
#define svc_restore 33
#define svc_cutscene 34
#define svc_weaponanim 35
#define svc_decalname 36
#define svc_roomtype 37
#define svc_addangle 38
#define svc_newusermsg 39
#define svc_packetentities 40
#define svc_deltapacketentities 41
#define svc_deltapacketentities2 42
#define svc_playerinfo 43
#define svc_choke 44
#define svc_resourcelist 45
#define svc_newmovevars 46
#define svc_resourcerequest 47
#define svc_customization 48
#define svc_unk49 49
#define svc_crosshairangle 50
#define svc_soundfade 51
#define svc_clientmaxspeed 52
#define svc_unk53 53
#define SVC_LASTMSG 53
#define MAX_USERMSGS 72

#define SND_FL_VOLUME (1 << 0)
#define SND_FL_ATTENUATION (1 << 1)
#define SND_FL_LARGE_INDEX (1 << 2)
#define SND_FL_PITCH (1 << 3)
#define SND_FL_SENTENCE (1 << 4)
#define PITCH_NORM 100
#define CHAN_STATIC 6

/* resource_t */
#define t_sound 0
#define t_skin 1
#define t_model 2
#define t_decal 3
#define t_generic 4
#define RES_FATALIFMISSING (1 << 0)
#define RES_WASMISSING (1 << 1)
#define RES_CUSTOM (1 << 2)

typedef struct sfx_s sfx_t;

typedef struct
{
    int length;
    char map[64];
} lightstyle_t; /* 0x44 */

/* decallist_t do PS2 (arquivo de decals salvos, cabeçalho "VALV") */
typedef struct
{
    int unk0;                   /* 0x00 */
    short entityIndex;          /* 0x04 */
    byte depth;                 /* 0x06 */
    byte flags;                 /* 0x07 */
    vec3_t position;            /* 0x08 */
    vec3_t impactPlaneNormal;   /* 0x14 */
    char name[16];              /* 0x20 */
} decallist_t; /* 0x30 */
typedef int (*pfnUserMsgHook)(int clientId, const char *pszName, int iSize, void *pbuf);

typedef struct _ClUserMsg
{
    int iMsg;                   /* 0x00 */
    int iSize;                  /* 0x04: -1 = tamanho variável */
    char szName[12];            /* 0x08 */
    pfnUserMsgHook pfn;         /* 0x14 */
} ClUserMsg; /* 0x18 */

extern int gNumUserMsgs[MAX_LOCAL];
extern ClUserMsg gUserMsgs[MAX_LOCAL][MAX_USERMSGS];
extern byte gUserMsgBuf[];
extern cl_entity_t *cl_entities[MAX_LOCAL];
extern sizebuf_t net_message;
extern char *rgpszrawsentence[];
extern cvar_t cl_downloadinterval;
extern float realtime;
extern qboolean noclip_anglehack;
extern int D_0033451C;
extern int cl_updatebackup[MAX_LOCAL];
extern int cl_updatemask[MAX_LOCAL];
extern int cl_parsecountmod[MAX_LOCAL];
extern int D_00409354, D_00409358, D_0040935C; /* últimos 3 comandos */
extern int msg_readcount;
extern int msg_badread;
extern lightstyle_t cl_lightstyle[MAX_LIGHTSTYLES];
extern int D_0053D1D4;
#define vid_recalc_refdef D_0053D1D4
extern float D_004A8B28;
#define sv_maxspeed_value D_004A8B28
extern void (*D_003997DC)(void);
extern void (*D_00399788)(void);
extern float D_00399498[MAX_LOCAL];
extern char com_gamedir[];
extern void (*D_003997C0)(char *gamedir);
extern int protocol_version;
extern int D_00332F2C;
extern char *svc_strings[];
extern int msg_buckets[64];
extern int frame_data[64];
extern int total_data[64];
extern int bitcounts[40];
extern int playerbitcounts[32];
extern int custombitcounts[32];
extern cvar_t r_decals;
extern cvar_t mp_decals;
extern cvar_t cl_skyname;
extern cvar_t gl_zmax;
extern cvar_t gl_wateramp;
extern char D_00409340[8];  /* static local */
extern char D_00409348[12]; /* static local */

void *memset(void *, int, unsigned int);
char *strcat(char *, const char *);
int sprintf(char *, const char *, ...);
char *va(const char *format, ...);
float Sys_FloatTime(void);
void Hunk_Check(void);
void MSG_WriteString(sizebuf_t *sb, const char *s);
void func_002B0280(int clientId);
void CL_RemoveFromResourceList(resource_t *pResource);
int MSG_ReadShort(void);
void S_BeginPrecaching(void);
struct sfx_s *S_PrecacheSound(char *name);
void S_EndPrecaching(void);
void COM_ExplainDisconnection(qboolean bPrint, char *fmt, ...);
void CL_Disconnect(int clientId);
struct model_s *Mod_ForName(char *name, qboolean crash);
void Draw_DecalSetName(int decal, char *name);
void SCR_EndLoadingPlaque(void);
void SCR_CenterPrint(char *str, int clientId);
void Q_strcpy(char *dest, const char *src);
int Q_strlen(const char *str);
void func_002CD280(int entnum, int entchannel, int clientId);
void func_002B0E28(int clientId);
void func_00258778(int clientId);
void func_00250C50(int clientId);
void func_002490B0(qboolean delta, int clientId);
void func_0024BD88(int clientId);
void func_0024AD40(int clientId);
void func_00251BB0(void);
void func_00239318(int track, qboolean looping);
void *Mem_DbgMalloc(int size, const char *file, int line);
void Mem_Free(void *p);
void Mem_PushTag(void);
void Mem_SetTag(int tag);
void Mem_PopTag(void);
void R_DestroyObjects(int clientId);
void AllocTransObjectsData(int count, int clientId);
int func_00245408(int nMaxClients);
void COM_FileBase(char *in, char *out);
void Host_WriteConfiguration(void);
void func_002CF6F8(char *gamedir);
void Decal_Init(void);
void Draw_Init(void);
void func_0028BD90(void);
void func_00246E78(void);
void func_00247088(int clientId);
void func_00247028(int clientId);
void Cbuf_AddText(char *text);
void func_0024FF90(int unk, int clientId);
void Cvar_SetValue(char *var_name, float value);
int func_00243460(void);
int func_00243370(void);
float MSG_ReadCoord(void);
float MSG_ReadFloat(void);
int COM_CheckParm(char *parm);
int strcmp(const char *, const char *);
void *func_0022B118(char *name, char *mode);
int func_0022BE50(void *buf, int size, int count, void *f);
void func_0022B238(void *f);
int Draw_DecalIndexFromName(char *name);
int Draw_DecalIndex(int id);
void R_DecalShoot(int textureIndex, int entity, int modelIndex, vec3_t position, int flags, int unk);
float func_002437A8(void);
int atoi(const char *);
void Host_Error(const char *error, ...);
void Host_EndGame(const char *message, ...);
char *Info_ValueForKey(const char *s, const char *key);
void COM_ClearCustomizationList(void *pHead, qboolean bCleanDecals);
int MSG_ReadByte(void);
int MSG_ReadLong(void);
char *MSG_ReadString(void);
int MSG_ReadBuf(int iSize, void *pbuf);
void MSG_StartBitReading(sizebuf_t *buf);
void MSG_EndBitReading(sizebuf_t *buf);
int MSG_ReadBits(int numbits);
int MSG_ReadByteBits(int numbits);
int MSG_ReadSBits(int numbits);
void S_StartDynamicSound(int entnum, int entchannel, sfx_t *sfx, vec3_t origin, float fvol, float attenuation, int flags, int pitch, int clientId);
void S_StartStaticSound(int entnum, int entchannel, sfx_t *sfx, vec3_t origin, float fvol, float attenuation, int flags, int pitch, int clientId);

void CL_InitUserMsgs(void)
{
    int i;

    for (i = MAX_LOCAL - 1; i >= 0; i--)
        gNumUserMsgs[i] = 0;

    for (i = 0; i < MAX_LOCAL; i++)
        memset(gUserMsgs[i], 0, sizeof(gUserMsgs[i]));
}

void CL_ClearPlayerCustomization(int slot, int clientId)
{
    COM_ClearCustomizationList(cl[clientId].players[slot].customdata, true);
}

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391120);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391138);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391150);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391160);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391178);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391190);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_003911A8);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_003911B8);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_003911C8);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_003911E0);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_003911F0);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391200);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391218);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391230);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391240);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391250);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391260);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391270);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391280);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391290);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_003912A0);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_003912B0);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_003912C0);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_003912D0);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_003912E8);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391300);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391310);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391328);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391338);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391348);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391358);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391368);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391380);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_003913A0);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_003913B0);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_003913C0);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_003913D0);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_003913E8);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_003913F8);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391408);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391418);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391430);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391440);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391450);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391460);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391470);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391480);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_00391490);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_003914A0);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_003914B0);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_003914C0);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_003914D0);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_003914E0);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_parse", D_003914E8);

cl_entity_t *CL_EntityNum(int number, int clientId)
{
    if (number >= cl[clientId].num_entities)
    {
        if (number >= cl[clientId].max_edicts)
            Host_Error("CL_EntityNum: %i is an invalid number, cl.max_edicts is %i", number, cl[clientId].max_edicts);
        while (cl[clientId].num_entities <= number)
            cl[clientId].num_entities++;
    }

    return &cl_entities[clientId][number];
}

void CL_ParseNewUserMsg(int clientId)
{
    ClUserMsg umsg;
    int i;
    int found;
    int tmp;

    found = 0;

    umsg.iMsg = MSG_ReadByte();
    umsg.iSize = MSG_ReadByte();
    umsg.pfn = NULL;
    if (umsg.iSize == 255)
        umsg.iSize = -1;

    tmp = MSG_ReadLong();
    strncpy(umsg.szName, (char *)&tmp, 4);
    tmp = MSG_ReadLong();
    strncpy(umsg.szName + 4, (char *)&tmp, 4);
    tmp = MSG_ReadLong();
    strncpy(umsg.szName + 8, (char *)&tmp, 4);
    tmp = MSG_ReadLong();
    strncpy(umsg.szName + 12, (char *)&tmp, 4);
    umsg.pfn = NULL;

    for (i = 0; i < gNumUserMsgs[clientId]; i++)
    {
        ClUserMsg *p = &gUserMsgs[clientId][i];
        if (!strcasecmp(p->szName, umsg.szName))
        {
            p->iMsg = umsg.iMsg;
            p->iSize = umsg.iSize;
            found = 1;
        }
    }

    if (!found)
        *(&gUserMsgs[clientId][0] + gNumUserMsgs[clientId]++) = umsg;
}

void DispatchUserMsg(int iMsg, int clientId)
{
    int i;
    int found;
    int iSize;
    ClUserMsg *msg;

    found = 0;

    if (iMsg < SVC_LASTMSG + 1 || iMsg > 127)
    {
        Con_DPrintf("Illegal User Msg %d\n", iMsg);
        return;
    }

    for (i = 0; i < gNumUserMsgs[clientId]; i++)
    {
        msg = &gUserMsgs[clientId][i];
        if (msg->iMsg == iMsg)
        {
            iSize = msg->iSize;
            if (!found)
            {
                if (iSize == -1)
                    iSize = MSG_ReadByte();
                MSG_ReadBuf(iSize, gUserMsgBuf);
            }
            found = 1;

            if (!msg->pfn)
                Con_DPrintf("DispatchUserMsg: UserMsgClParse: No pfn %s %d\n", msg->szName, iMsg);
            else
                msg->pfn(clientId, msg->szName, iSize, gUserMsgBuf);
        }
    }

    if (!found)
        Con_DPrintf("UserMsgClParse: Not Present on Client %d\n", iMsg);
}

int DispatchDirectUserMsg(const char *pszName, int iSize, void *pBuf, int clientId)
{
    int i;
    int found;
    int size;
    ClUserMsg *msg;

    found = 0;

    for (i = 0; i < gNumUserMsgs[clientId]; i++)
    {
        msg = &gUserMsgs[clientId][i];
        if (strcasecmp(pszName, msg->szName))
            continue;

        size = msg->iSize;
        found = 1;
        if (size == -1)
            size = iSize;

        if (!msg->pfn)
            Con_DPrintf("DispatchDirectUserMsg: UserMsgClParse: No pfn %s %d\n", msg->szName, msg->iMsg);
        else
            msg->pfn(clientId, msg->szName, size, pBuf);
    }

    return found;
}

pfnUserMsgHook HookServerMsg(const char *pszName, pfnUserMsgHook pfn, int clientId)
{
    ClUserMsg *pList, *pLastMatch;
    pfnUserMsgHook pfnOld;
    int i;

    pfnOld = NULL;
    pLastMatch = NULL;
    pList = NULL;

    for (i = 0; i < gNumUserMsgs[clientId]; i++)
    {
        pList = &gUserMsgs[clientId][i];
        if (!strcasecmp(pszName, pList->szName))
        {
            pLastMatch = pList;
            if (pList->pfn == pfn)
                break;
        }
        pList = NULL;
        pLastMatch = NULL;
    }

    if (pList)
    {
        pfnOld = pList->pfn;
    }
    else
    {
        pList = &gUserMsgs[clientId][gNumUserMsgs[clientId]++];
        memset(pList, 0, sizeof(ClUserMsg));
        if (pLastMatch)
            *pList = *pLastMatch;
        else
            strcpy(pList->szName, pszName);
        pList->pfn = pfn;
    }

    return pfnOld;
}

void CL_ResetUserMsgs(int clientId)
{
    gNumUserMsgs[clientId] = 0;
}

void CL_UserMsgs_f(void)
{
    int i;
    ClUserMsg *msg;

    for (i = 0; i < gNumUserMsgs[0]; i++)
    {
        msg = &gUserMsgs[0][i];
        Con_Printf("%i:%s Sz %i\n", msg->iMsg, msg->szName, msg->iSize);
    }
}

void CL_ParseStartSoundPacket(int clientId)
{
    vec3_t pos;
    char name[80];
    int channel, ent, sound_num, field_mask, pitch;
    float volume, attenuation;
    int i;
    sfx_t *sfx;

    MSG_StartBitReading(&net_message);

    field_mask = MSG_ReadBits(9);

    if (field_mask & SND_FL_VOLUME)
        volume = MSG_ReadByteBits(8) / 255.0f;
    else
        volume = 1.0f;

    if (field_mask & SND_FL_ATTENUATION)
        attenuation = MSG_ReadByteBits(8) / 64.0f;
    else
        attenuation = 1.0f;

    channel = MSG_ReadByteBits(3);
    ent = MSG_ReadBits(10);

    if (field_mask & SND_FL_LARGE_INDEX)
        sound_num = MSG_ReadBits(16);
    else
        sound_num = MSG_ReadByteBits(8);

    for (i = 0; i < 3; i++)
        pos[i] = MSG_ReadSBits(16) * 0.125f;

    if (field_mask & SND_FL_PITCH)
        pitch = MSG_ReadByteBits(8);
    else
        pitch = PITCH_NORM;

    MSG_EndBitReading(&net_message);

    if (ent < cl[clientId].max_edicts)
    {
        if (field_mask & SND_FL_SENTENCE)
        {
            sfx = (sfx_t *)name;
            strcpy(name, "!");
            strcat(name, rgpszrawsentence[sound_num]);
        }
        else
        {
            sfx = cl[clientId].sound_precache[sound_num];
        }

        if (channel == CHAN_STATIC)
            S_StartStaticSound(ent, channel, sfx, pos, volume, attenuation, field_mask, pitch, clientId);
        else
            S_StartDynamicSound(ent, channel, sfx, pos, volume, attenuation, field_mask, pitch, clientId);
    }
    else
    {
        Host_Error("CL_ParseStartSoundPacket: ent = %i", ent);
    }
}

void CL_UpdateUserinfo(int clientId)
{
    int slot;
    player_info_t *player;

    slot = MSG_ReadByte();
    if (slot >= MAX_CLIENTS)
        Host_EndGame("CL_ParseServerMessage: svc_updateuserinfo > MAX_CLIENTS");

    player = &cl[clientId].players[slot];
    player->userid = MSG_ReadLong();
    strncpy(player->userinfo, MSG_ReadString(), MAX_INFO_STRING - 1);
    strncpy(player->name, Info_ValueForKey(player->userinfo, "name"), sizeof(player->name) - 1);
    strncpy(player->model, Info_ValueForKey(player->userinfo, "model"), sizeof(player->model) - 1);
    player->topcolor = atoi(Info_ValueForKey(player->userinfo, "topcolor"));
    player->bottomcolor = atoi(Info_ValueForKey(player->userinfo, "bottomcolor"));

    if (Info_ValueForKey(player->userinfo, "*spectator")[0])
        player->spectator = true;
    else
        player->spectator = false;

    if (!player->userinfo[0] || !player->name[0])
        CL_ClearPlayerCustomization(slot, clientId);
}

int func_00295148(void)
{
    return 1;
}

void func_00295150(void)
{
    downloadtime_t *p;

    if (cl_downloadinterval.value == 0)
        return;

    if (cl_downloadinterval.value < 0)
        Cvar_SetValue("cl_downloadinterval", 1);

    if (realtime - cls[0].dl_fLastStatTime < cl_downloadinterval.value)
        return;

    cls[0].dl_fLastStatTime = realtime;
    p = &cls[0].dl_rgStats[cls[0].dl_nCurStat & 7];
    p->bUsed = 1;
    p->fTime = realtime;
    p->nBytesRemaining = cls[0].dl_nRemainingToTransfer;
    cls[0].dl_nCurStat++;
}

void func_00295218(void) {
}

void CL_PrintResource(int index, resource_t *pResource)
{
    if (pResource->ucFlags & RES_FATALIFMISSING)
        sprintf(D_00409340, "Y");
    else
        sprintf(D_00409340, "N");

    switch (pResource->type)
    {
    default:
        sprintf(D_00409348, "unknown");
        break;
    case t_sound:
        sprintf(D_00409348, "sound");
        break;
    case t_decal:
        sprintf(D_00409348, "decal");
        break;
    case t_model:
        sprintf(D_00409348, "model");
        break;
    case t_skin:
        sprintf(D_00409348, "skin");
        break;
    case t_generic:
        sprintf(D_00409348, "generic");
        break;
    }

    Con_Printf("%3i %s:%15s %i %s\n", index, D_00409348, pResource->szFileName, pResource->nIndex, D_00409340);
}

int func_00295358(resource_t *pList)
{
    int n;
    resource_t *p;

    n = 0;
    for (p = pList->pNext; p != pList; p = p->pNext)
        n++;

    return n;
}

int func_00295380(void *p)
{
    return 1;
}

void CL_RegisterResources(int clientId)
{
    if (cls[clientId].dl_custom)
    {
        cls[clientId].dl_custom = 0;
        return;
    }

    cl[clientId].worldmodel = cl[clientId].model_precache[1];
    cl_entities[clientId][0].model = cl[clientId].worldmodel;
    if (!cl[clientId].worldmodel)
        Sys_Error("Client world model is NULL\n");

    Con_DPrintf("Setting up renderer...\n");
    Sys_FloatTime();
    func_002B0280(clientId);
    Sys_FloatTime();
    Hunk_Check();
    Sys_FloatTime();

    noclip_anglehack = false;

    if (!cls[clientId].demoplayback && !func_00295380(cl[clientId].worldmodel))
        return;

    MSG_WriteByte(&cls[clientId].netchan.message, clc_stringcmd);
    MSG_WriteString(&cls[clientId].netchan.message, va("prespawn %i 0", cl[clientId].servercount));
    Sys_FloatTime();
}

int CL_ResourceBreakdown(resource_t *pList, int *nWorld, int *nModels, int *nDecals, int *nSounds, int *nSkins, int *nGeneric)
{
    int n;
    int world;
    resource_t *p;

    *nModels = 0;
    *nWorld = 0;
    *nDecals = 0;
    *nSounds = 0;
    *nSkins = 0;
    *nGeneric = 0;

    n = 0;
    for (p = pList->pNext; p != pList; p = p->pNext)
    {
        world = 1;
        n++;
        switch (p->type)
        {
        case t_model:
            if (p->nIndex == 1)
                *nWorld = world;
            else
                (*nModels)++;
            break;
        case t_sound:
            (*nSounds)++;
            break;
        case t_decal:
            (*nDecals)++;
            break;
        case t_skin:
            (*nSkins)++;
            break;
        case t_generic:
            (*nGeneric)++;
            break;
        }
    }

    return n;
}

void CL_AddToResourceList(resource_t *pResource, resource_t *pList)
{
    if (pResource->pPrev || pResource->pNext)
    {
        Con_Printf("Resource already linked\n");
        return;
    }

    if (!pList->pPrev || !pList->pNext)
    {
        Sys_Error("Resource list corrupted.\n");
        return;
    }

    pResource->pPrev = pList->pPrev;
    pList->pPrev->pNext = pResource;
    pList->pPrev = pResource;
    pResource->pNext = pList;
}

void CL_ClearResourceList(resource_t *pList)
{
    resource_t *p, *n;

    for (p = pList->pNext; p && p != pList; p = n)
    {
        n = p->pNext;
        CL_RemoveFromResourceList(p);
    }

    pList->pPrev = pList;
    pList->pNext = pList;
}

void CL_RemoveFromResourceList(resource_t *pResource)
{
    if (!pResource->pPrev || !pResource->pNext)
        Sys_Error("Mislinked resource in CL_RemoveFromResourceList");

    if (pResource->pNext == pResource || pResource->pPrev == pResource)
        Sys_Error("Attempt to free last entry in list.");

    pResource->pPrev->pNext = pResource->pNext;
    pResource->pNext->pPrev = pResource->pPrev;
    pResource->pPrev = NULL;
    pResource->pNext = NULL;
}

qboolean CL_RequestMissingResources(int clientId)
{
    if (cls[clientId].unk_7DDC || cls[clientId].dl_unk_7E78)
        return false;

    if (!cls[clientId].dl_custom && cls[clientId].state != ca_uninitialized)
        return false;

    if (cls[clientId].dl_doneregistering)
        return false;

    cls[clientId].unk_7DE0 = 0;
    Con_DPrintf("Resource propagation complete.\n");
    CL_RegisterResources(clientId);
    cls[clientId].dl_doneregistering = true;
    return false;
}

void CL_StartResourceDownloading(char *pszMessage, qboolean bCustom, int clientId)
{
    if (pszMessage)
        Con_DPrintf(pszMessage);

    cls[clientId].dl_nTotalToTransfer = 0;

    if (!bCustom)
    {
        cls[clientId].state = ca_uninitialized;
        cls[clientId].dl_custom = false;
        D_0033451C = 0;
    }
    else
    {
        cls[clientId].dl_custom = true;
    }

    cls[clientId].dl_doneregistering = false;
    cls[clientId].dl_unk_7E78 = 0;
    cls[clientId].dl_fLastStatusUpdate = realtime;
    cls[clientId].dl_fLastStatTime = realtime;
    cls[clientId].dl_nRemainingToTransfer = cls[clientId].dl_nTotalToTransfer;
    memset(cls[clientId].dl_rgStats, 0, sizeof(cls[clientId].dl_rgStats));
    cls[clientId].dl_nCurStat = 0;
}

void CL_ParseResourceList(int clientId)
{
    resource_t resource;
    int i, total, start, end;

    total = MSG_ReadShort();
    start = MSG_ReadShort();
    end = MSG_ReadShort();

    for (i = start; i < end; i++)
    {
        resource.type = MSG_ReadByte();
        strcpy(resource.szFileName, MSG_ReadString());
        resource.nIndex = MSG_ReadShort();
        resource.ucFlags = (byte)MSG_ReadByte() & ~RES_WASMISSING;
        resource.pNext = NULL;
        resource.pPrev = NULL;

        switch (resource.type)
        {
        case t_decal:
            if (!(resource.ucFlags & RES_CUSTOM))
                Draw_DecalSetName(resource.nIndex, resource.szFileName);
            break;
        case t_sound:
            if (resource.ucFlags & RES_WASMISSING)
            {
                cl[clientId].sound_precache[resource.nIndex] = NULL;
                break;
            }
            S_BeginPrecaching();
            cl[clientId].sound_precache[resource.nIndex] = S_PrecacheSound(resource.szFileName);
            S_EndPrecaching();
            if (!cl[clientId].sound_precache[resource.nIndex])
            {
                if (resource.ucFlags & RES_FATALIFMISSING)
                {
                    COM_ExplainDisconnection(true, "Cannot continue without sound %s, disconnecting\n", resource.szFileName);
                    CL_Disconnect(clientId);
                }
            }
            break;
        case t_skin:
            break;
        case t_model:
            cl[clientId].model_precache[resource.nIndex] = Mod_ForName(resource.szFileName, false);
            if (!cl[clientId].model_precache[resource.nIndex])
            {
                Con_Printf("Model %s not found\n", resource.szFileName);
                if (resource.ucFlags & RES_FATALIFMISSING)
                {
                    COM_ExplainDisconnection(true, "Cannot continue without model %s, disconnecting\n", resource.szFileName);
                    CL_Disconnect(clientId);
                }
            }
            break;
        case t_generic:
            break;
        default:
            Con_DPrintf("Unknown resource type\n");
            break;
        }
    }

    cl[clientId].num_resources += end;
    if (cl[clientId].num_resources >= total)
        CL_StartResourceDownloading("Verifying and downloading resources...\n", false, clientId);
    else
        cls[clientId].state = ca_connected;
}

int func_00295B58(void)
{
    return 0;
}

void func_00295B60(void) {
}

void func_00295B68(void) {
}

void CL_DeallocateDynamicData(int clientId)
{
    if (cl_entities[clientId])
    {
        Mem_Free(cl_entities[clientId]);
        cl_entities[clientId] = NULL;
    }

    R_DestroyObjects(clientId);
}

void CL_ReallocateDynamicData(int nMaxClients, int clientId)
{
    cl[clientId].max_edicts = func_00245408(nMaxClients);

    if (cl[clientId].max_edicts <= 0)
        Sys_Error("CL_ReallocateDynamicData allocating 0 entities");

    if (cl_entities[clientId])
        Con_Printf("CL_Reallocate cl_entities\n");

    Mem_PushTag();
    Mem_SetTag(2);
    cl_entities[clientId] = Mem_DbgMalloc(cl[clientId].max_edicts * sizeof(cl_entity_t), "engine/Cl_parse.c", 1548);
    memset(cl_entities[clientId], 0, cl[clientId].max_edicts * sizeof(cl_entity_t));
    Mem_PopTag();

    AllocTransObjectsData(cl[clientId].max_edicts, clientId);

    cl_updatebackup[clientId] = 8;
    cl_updatemask[clientId] = 7;
    cl_parsecountmod[clientId] &= 7;

    if (cl[clientId].frames)
        Mem_Free(cl[clientId].frames);

    cl[clientId].frames = Mem_DbgMalloc(cl_updatebackup[clientId] * 0x330, "engine/Cl_parse.c", 1571);
    if (!cl[clientId].frames)
        Sys_Error("CL_ReallocateDynamicData failed to allocate %i frames", cl_updatebackup[clientId]);

    memset(cl[clientId].frames, 0, cl_updatebackup[clientId] * 0x330);
}

void CL_CheckGameDirectory(char *gamedir)
{
    char szGameDir[MAX_OSPATH];
    int i;

    if (!gamedir || !gamedir[0])
    {
        Con_Printf("Server didn't specify a gamedir, assuming no change\n");
        return;
    }

    COM_FileBase(com_gamedir, szGameDir);
    if (!strcasecmp(szGameDir, gamedir))
        return;

    Host_WriteConfiguration();
    func_002CF6F8(gamedir);
    if (D_003997C0)
        D_003997C0(gamedir);

    Decal_Init();
    Draw_Init();
    func_0028BD90();
    func_00246E78();

    for (i = 0; i < cl_numlocal; i++)
        func_00247088(i);

    for (i = 0; i < cl_numlocal; i++)
        func_00247028(i);

    Cbuf_AddText("exec config.cfg\n");
    Con_Printf("Changed to game %s\n", gamedir);
}

void CL_ParseServerInfo(int clientId)
{
    char *str;
    char *gamedir;
    int i;

    Con_DPrintf("Serverinfo packet received.\n");

    func_0024FF90(0, clientId);
    func_00247028(clientId);

    i = MSG_ReadLong();
    if (i != protocol_version)
    {
        Con_Printf("Server returned version %i, not %i\n", i, protocol_version);
        return;
    }

    cl[clientId].servercount = MSG_ReadLong();
    cl[clientId].unk_15A4 = MSG_ReadLong();
    cl[clientId].unk_15A8 = MSG_ReadLong();

    cl[clientId].maxclients = MSG_ReadByte();
    if (cl[clientId].maxclients < 1 || cl[clientId].maxclients > MAX_CLIENTS)
    {
        Con_Printf("Bad maxclients (%u) from server\n", cl[clientId].maxclients);
        return;
    }

    if (cl[clientId].maxclients > 1)
    {
        if (mp_decals.value < r_decals.value)
            Cvar_SetValue("r_decals", mp_decals.value);
    }

    CL_DeallocateDynamicData(clientId);
    CL_ReallocateDynamicData(cl[clientId].maxclients, clientId);

    cl[clientId].playernum = MSG_ReadByte();
    if (cl[clientId].playernum & 0x80)
    {
        cl[clientId].paused = true;
        cl[clientId].playernum &= ~0x80;
    }

    cl[clientId].gametype = MSG_ReadByte();

    gamedir = MSG_ReadString();
    if (clientId == 0)
        CL_CheckGameDirectory(gamedir);

    cls[clientId].demoplayback = false;
    str = MSG_ReadString();
    if (str && str[0])
    {
        cls[clientId].demoplayback = true;
        if (!func_00295380(str))
            return;
    }

    str = MSG_ReadString();
    strncpy(cl[clientId].levelname, str, sizeof(cl[clientId].levelname) - 1);
    cl[clientId].num_resources = 0;

    Con_DPrintf("Requesting resourcelist from server.\n");
    MSG_WriteByte(&cls[clientId].netchan.message, clc_stringcmd);
    MSG_WriteString(&cls[clientId].netchan.message, va("resourcelist %i 0", cl[clientId].servercount));
    D_00332F2C = cl[clientId].servercount;
    cls[clientId].state = ca_connected;
}

void CL_ParseBaseline(cl_entity_t *ent)
{
    int i;

    ent->baseline.entityType = MSG_ReadByte();
    ent->baseline.modelindex = MSG_ReadShort();
    ent->baseline.sequence = MSG_ReadByte();
    ent->baseline.frame = MSG_ReadByte();

    if (!ent->baseline.entityType)
        ent->baseline.scale = func_00243460() * (1.0f / 256.0f);
    else
        ent->baseline.scale = MSG_ReadByte() * 0.1f;

    ent->baseline.colormap = func_00243460();
    ent->baseline.skin = MSG_ReadShort();
    ent->baseline.solid = MSG_ReadByte();

    for (i = 0; i < 3; i++)
    {
        ent->baseline.origin[i] = MSG_ReadCoord();
        ent->baseline.angles[i] = MSG_ReadFloat();
        ent->baseline.mins[i] = MSG_ReadCoord();
        ent->baseline.maxs[i] = MSG_ReadCoord();
    }

    ent->baseline.rendermode = MSG_ReadByte();
    ent->baseline.renderamt = MSG_ReadByte();
    ent->baseline.rendercolor.r = MSG_ReadByte();
    ent->baseline.rendercolor.g = MSG_ReadByte();
    ent->baseline.rendercolor.b = MSG_ReadByte();
    ent->baseline.renderfx = MSG_ReadByte();
}

void CL_ParseClientdata(int bits, int clientId)
{
    int i;
    frame_t *frame;
    float latency;

    i = cls[clientId].netchan.incoming_acknowledged;
    cl[clientId].parsecount = i;
    i &= cl_updatemask[clientId];
    cl_parsecountmod[clientId] = i;
    frame = &cl[clientId].frames[i];
    D_00399498[clientId] = frame->senttime;
    frame->receivedtime = realtime;

    frame->unk_326 = 0;
    frame->unk_328 = 0;
    frame->unk_324 = 0;
    frame->unk_32A = 0;
    frame->unk_32C = 0;

    latency = frame->receivedtime - frame->senttime;
    if (latency < 0 || latency > 1.0f)
    {
    }
    else
    {
        if (latency < cls[clientId].latency)
            cls[clientId].latency = latency;
        else
            cls[clientId].latency += 0.001f;
    }

    if (bits & SU_VIEWHEIGHT)
        cl[clientId].viewheight = func_00243370();
    else
        cl[clientId].viewheight = DEFAULT_VIEWHEIGHT;

    if (bits & SU_IDEALPITCH)
        cl[clientId].idealpitch = func_00243370();
    else
        cl[clientId].idealpitch = 0;

    VectorCopy(cl[clientId].mvelocity[0], cl[clientId].mvelocity[1]);

    for (i = 0; i < 3; i++)
    {
        if (bits & (SU_PUNCH1 << i))
            cl[clientId].punchangle[i] = func_002437A8();
        else
            cl[clientId].punchangle[i] = 0;

        if (bits & (SU_VELOCITY1 << i))
            cl[clientId].mvelocity[0][i] = func_00243370() * 16;
        else
            cl[clientId].mvelocity[0][i] = 0;
    }

    if (bits & SU_ITEMS)
        cl[clientId].items = MSG_ReadLong();

    cl[clientId].onground = (bits & SU_ONGROUND) != 0;
    cl[clientId].inwater = (bits & SU_INWATER) != 0;

    if (cl[clientId].inwater)
    {
        if (bits & SU_FULLYINWATER)
            cl[clientId].waterlevel = 3;
        else
            cl[clientId].waterlevel = 2;
    }
    else
    {
        cl[clientId].waterlevel = 0;
    }

    if (bits & SU_WEAPONFRAME)
        cl[clientId].stats[STAT_WEAPON] = MSG_ReadShort();
    else
        cl[clientId].stats[STAT_WEAPON] = 0;

    i = MSG_ReadShort();
    if (cl[clientId].stats[STAT_HEALTH] != i)
        cl[clientId].stats[STAT_HEALTH] = i;
}

void func_00296658(int clientId) {
}

void CL_ParseStaticSound(int clientId)
{
    vec3_t org;
    char name[80];
    int sound_num, ent, pitch, fFlags;
    float vol, atten;
    int i;
    sfx_t *sfx;

    for (i = 0; i < 3; i++)
        org[i] = MSG_ReadCoord();

    sound_num = MSG_ReadShort();
    vol = MSG_ReadByte() / 255.0f;
    atten = MSG_ReadByte() / 64.0f;
    ent = MSG_ReadShort();
    pitch = MSG_ReadByte();
    fFlags = MSG_ReadByte();

    if (COM_CheckParm("-nosound"))
        return;

    if (fFlags & SND_FL_SENTENCE)
    {
        sfx = (sfx_t *)name;
        strcpy(name, "!");
        strcat(name, rgpszrawsentence[sound_num]);
    }
    else
    {
        sfx = cl[clientId].sound_precache[sound_num];
    }

    S_StartStaticSound(ent, CHAN_STATIC, sfx, org, vol, atten, fFlags, pitch, clientId);
}

void CL_ParseMovevars(int clientId)
{
    movevars.gravity = MSG_ReadFloat();
    movevars.stopspeed = MSG_ReadFloat();
    movevars.maxspeed = MSG_ReadFloat();
    movevars.spectatormaxspeed = MSG_ReadFloat();
    movevars.accelerate = MSG_ReadFloat();
    movevars.airaccelerate = MSG_ReadFloat();
    movevars.wateraccelerate = MSG_ReadFloat();
    movevars.friction = MSG_ReadFloat();
    movevars.edgefriction = MSG_ReadFloat();
    movevars.waterfriction = MSG_ReadFloat();
    movevars.entgravity = MSG_ReadFloat();
    movevars.bounce = MSG_ReadFloat();
    movevars.stepsize = MSG_ReadFloat();
    movevars.maxvelocity = MSG_ReadFloat();
    movevars.zmax = MSG_ReadFloat();
    movevars.waveHeight = MSG_ReadFloat();
    strcpy(movevars.skyName, MSG_ReadString());

    if (strcmp(movevars.skyName, cl_skyname.string))
        Cvar_Set("cl_skyname", movevars.skyName);

    if (movevars.zmax != gl_zmax.value)
        Cvar_SetValue("gl_zmax", movevars.zmax);

    if (movevars.waveHeight != gl_wateramp.value)
        Cvar_SetValue("gl_wateramp", movevars.waveHeight);

    if (cl_entities[clientId])
        cl_entities[clientId][0].scale = gl_wateramp.value;
}

void CL_ParseSoundFade(int clientId)
{
    int percent, holdTime, inTime, outTime;

    percent = MSG_ReadByte();
    holdTime = MSG_ReadByte();
    inTime = MSG_ReadByte();
    outTime = MSG_ReadByte();

    cls[clientId].soundfade.soundFadeStartTime = realtime;
    cls[clientId].soundfade.nStartPercent = percent;
    cls[clientId].soundfade.soundFadeOutTime = outTime;
    cls[clientId].soundfade.soundFadeHoldTime = holdTime;
    cls[clientId].soundfade.soundFadeInTime = inTime;
}

void CL_LoadDecals(char *filename)
{
    void *f;
    decallist_t decal;
    int id, i, count;
    int n;

    f = func_0022B118(filename, "rb");
    if (f)
    {
        func_0022BE50(&id, 4, 1, f);
        func_0022BE50(&i, 4, 1, f);

        if (id == ('V' | ('A' << 8) | ('L' << 16) | ('V' << 24)))
        {
            func_0022BE50(&count, 4, 1, f);
            for (i = 0; i < count; i++)
            {
                func_0022BE50(decal.name, 1, 16, f);
                func_0022BE50(&decal.entityIndex, 2, 1, f);
                func_0022BE50(&decal.depth, 1, 1, f);
                func_0022BE50(&decal.flags, 1, 1, f);
                func_0022BE50(decal.position, 12, 1, f);

                if (r_decals.value != 0)
                {
                    id = Draw_DecalIndexFromName(decal.name);
                    R_DecalShoot(Draw_DecalIndex(id), decal.entityIndex, 0, decal.position, decal.flags, 0);
                }
            }
        }

        func_0022B238(f);
    }

    n = MSG_ReadByte();
    for (i = 0; i < n; i++)
        Con_Printf("Loading decals from %s\n", MSG_ReadString());
}

void CL_DumpMessageLoad_f(void)
{
    int i;
    int total;

    total = 0;
    Con_Printf("-------- Message Load ---------\n");

    for (i = 0; i < SVC_LASTMSG + 10; i++)
    {
        Con_Printf("%i:%s: %i msgs:%.2fK\n", i, (i < SVC_LASTMSG + 1) ? svc_strings[i] : "bogus #", msg_buckets[i], total_data[i] / 1024.0f);
        total += msg_buckets[i];
    }

    Con_Printf("User messages:  %i:%.2fK\n", msg_buckets[SVC_LASTMSG + 10], total_data[SVC_LASTMSG + 10] / 1024.0f);
    Con_Printf("------ End:  %i Total----\n", total + msg_buckets[SVC_LASTMSG + 10]);
}

void CL_BitCounts_f(void)
{
    int i;

    Con_Printf("------- Bit Counts -------\n");
    Con_Printf("Bit    Delta   Player  Custom\n");

    for (i = 0; i < 40; i++)
    {
        if (i < 32)
            Con_Printf("(1<<%2i) %6.6i  %6.6i  %6.6i\n", i, bitcounts[i], playerbitcounts[i], custombitcounts[i]);
        else
            Con_Printf("(1<<%2i) %6.6i\n", i, bitcounts[i]);
    }

    Con_Printf("--------------------------\n");
}

void CL_AccumulateMessageLoad(void)
{
    int i;

    for (i = 0; i < 64; i++)
        total_data[i] += frame_data[i];
}

#ifdef NON_MATCHING
void CL_ParseServerMessage(int clientId)
{
    int cmd;
    int i, j;
    int bufStart;
    int bufSize;
    float f;
    frame_t *frame;

    cl[clientId].onground = 0;
    memset(frame_data, 0, sizeof(frame_data));

    for (;;)
    {
        if (msg_badread)
            Host_Error("CL_ParseServerMessage: Bad server message");

        bufStart = msg_readcount;
        cmd = MSG_ReadByte();
        if (cmd == -1)
            break;

        if (cmd > SVC_LASTMSG)
        {
            msg_buckets[SVC_LASTMSG + 10]++;
            DispatchUserMsg(cmd, clientId);
            frame_data[SVC_LASTMSG + 10] += msg_readcount - bufStart;
            continue;
        }

        D_0040935C = D_00409358;
        D_00409358 = D_00409354;
        D_00409354 = cmd;

        if ((unsigned)cmd < 64)
            msg_buckets[cmd]++;

        switch (cmd)
        {
        default:
            Con_DPrintf("Last 3 messages parsed.\n");
            Con_DPrintf("%s\n", svc_strings[D_0040935C]);
            Con_DPrintf("%s\n", svc_strings[D_00409358]);
            Con_DPrintf("%s\n", svc_strings[D_00409354]);
            Con_DPrintf("BAD:  %3i:%s\n", msg_readcount - 1, svc_strings[cmd]);
            Host_Error("CL_ParseServerMessage: Illegible server message\n");
            break;

        case svc_time:
            cl[clientId].mtime[1] = cl[clientId].mtime[0];
            cl[clientId].mtime[0] = MSG_ReadFloat();
            break;

        case svc_clientdata:
            CL_ParseClientdata(MSG_ReadShort(), clientId);
            break;

        case svc_version:
            i = MSG_ReadLong();
            if (i != protocol_version)
                Host_Error("CL_ParseServerMessage: Server is protocol %i instead of %i\n", i, protocol_version);
            break;

        case svc_disconnect:
            SCR_EndLoadingPlaque();
            Host_EndGame("Server disconnected\n");
            break;

        case svc_print:
            Con_Printf("%s", MSG_ReadString());
            break;

        case svc_centerprint:
            SCR_CenterPrint(MSG_ReadString(), clientId);
            break;

        case svc_stufftext:
            Cbuf_AddText(MSG_ReadString());
            break;

        case svc_serverinfo:
            CL_ParseServerInfo(clientId);
            vid_recalc_refdef = true;
            break;

        case svc_setangle:
            for (i = 0; i < 3; i++)
                cl[clientId].viewangles[i] = func_002437A8();
            break;

        case svc_addangle:
            cl[clientId].viewangles[YAW] += func_002437A8();
            break;

        case svc_setview:
            cl[clientId].viewentity = MSG_ReadShort();
            break;

        case svc_crosshairangle:
            cl[clientId].crosshairangle[0] = func_00243370() * 0.2f;
            cl[clientId].crosshairangle[1] = func_00243370() * 0.2f;
            break;

        case svc_lightstyle:
            i = MSG_ReadByte();
            if (i >= MAX_LIGHTSTYLES)
                Sys_Error("svc_lightstyle > MAX_LIGHTSTYLES");
            Q_strcpy(cl_lightstyle[i].map, MSG_ReadString());
            cl_lightstyle[i].length = Q_strlen(cl_lightstyle[i].map);
            break;

        case svc_sound:
            CL_ParseStartSoundPacket(clientId);
            break;

        case svc_stopsound:
            i = MSG_ReadShort();
            func_002CD280(i >> 3, i & 7, clientId);
            break;

        case svc_particle:
            func_002B0E28(clientId);
            break;

        case svc_spawnbaseline:
            CL_ParseBaseline(CL_EntityNum(MSG_ReadShort(), clientId));
            break;

        case svc_spawnstatic:
            func_00296658(clientId);
            break;

        case svc_temp_entity:
            func_00258778(clientId);
            break;

        case svc_setpause:
            cl[clientId].unk_37C = MSG_ReadByte();
            if (cl[clientId].unk_37C)
                D_003997DC();
            else
                D_00399788();
            break;

        case svc_signonnum:
            i = MSG_ReadByte();
            if (i <= cls[clientId].signon)
                Host_Error("Received signon %i when at %i", i, cls[clientId].signon);
            cls[clientId].signon = i;
            func_00250C50(clientId);
            break;

        case svc_spawnstaticsound:
            CL_ParseStaticSound(clientId);
            break;

        case svc_cdtrack:
            cl[clientId].cdtrack = MSG_ReadByte();
            cl[clientId].looptrack = MSG_ReadByte();
            func_00239318(cl[clientId].cdtrack, true);
            break;

        case svc_restore:
            CL_LoadDecals(MSG_ReadString());
            break;

        case svc_weaponanim:
            cl[clientId].weaponstarttime = 0;
            cl[clientId].weaponsequence = MSG_ReadByte();
            cl[clientId].viewent.unk_20 = MSG_ReadByte();
            break;

        case svc_decalname:
            i = MSG_ReadByte();
            Draw_DecalSetName(i, MSG_ReadString());
            break;

        case svc_roomtype:
            Cvar_SetValue("room_type", MSG_ReadShort());
            break;

        case svc_newusermsg:
            CL_ParseNewUserMsg(clientId);
            break;

        case svc_updateuserinfo:
            CL_UpdateUserinfo(clientId);
            break;

        case svc_updatestat:
            i = MSG_ReadByte();
            if ((unsigned)i >= MAX_CL_STATS)
                Sys_Error("svc_updatestat: %i is invalid", i);
            cl[clientId].stats[i] = MSG_ReadLong();
            break;

        case svc_intermission:
            vid_recalc_refdef = true;
            cl[clientId].intermission = 1;
            cl[clientId].completed_time = cl[clientId].time;
            break;

        case svc_finale:
            vid_recalc_refdef = true;
            cl[clientId].intermission = 2;
            cl[clientId].completed_time = cl[clientId].time;
            SCR_CenterPrint(MSG_ReadString(), clientId);
            break;

        case svc_cutscene:
            vid_recalc_refdef = true;
            cl[clientId].intermission = 3;
            cl[clientId].completed_time = cl[clientId].time;
            SCR_CenterPrint(MSG_ReadString(), clientId);
            break;

        case svc_packetentities:
            func_00295218();
            break;

        case svc_deltapacketentities:
            func_002490B0(false, clientId);
            func_0024BD88(clientId);
            break;

        case svc_deltapacketentities2:
            func_002490B0(true, clientId);
            func_0024BD88(clientId);
            break;

        case svc_playerinfo:
            func_0024AD40(clientId);
            break;

        case svc_choke:
            i = MSG_ReadByte();
            for (j = 0; j < i; j++)
                cl[clientId].frames[(cls[clientId].netchan.incoming_acknowledged - 1 - j) & cl_updatemask[clientId]].receivedtime = -2;
            break;

        case svc_unk53:
            i = MSG_ReadByte();
            if (cl[clientId].frames[i & cl_updatemask[clientId]].receivedtime == -1 || cl[clientId].frames[i & cl_updatemask[clientId]].receivedtime == -2)
                cl[clientId].frames[i & cl_updatemask[clientId]].receivedtime = -3;
            break;

        case svc_resourcelist:
            CL_ParseResourceList(clientId);
            break;

        case svc_newmovevars:
            CL_ParseMovevars(clientId);
            break;

        case svc_customization:
            func_00251BB0();
            break;

        case svc_unk49:
            func_00295B68();
            break;

        case svc_soundfade:
            CL_ParseSoundFade(clientId);
            break;

        case svc_clientmaxspeed:
            i = MSG_ReadByte();
            if (i >= cl[clientId].maxclients)
                Host_Error("CL_ParseServerMessage: svc_clientmaxspeed >= cl[clientId].maxclients");
            f = MSG_ReadFloat();
            cl[clientId].players[i].maxspeed = f;
            if (f > sv_maxspeed_value)
                cl[clientId].players[i].maxspeed = sv_maxspeed_value;
            break;

        case svc_nop:
            break;

        case svc_damage:
            break;

        case svc_killedmonster:
            break;

        case svc_foundsecret:
            break;

        case svc_resourcerequest:
            break;
        }

        bufSize = msg_readcount - bufStart;
        frame_data[cmd] += bufSize;

        if (cmd == svc_deltapacketentities || cmd == svc_deltapacketentities2)
            cl[clientId].frames[cl_parsecountmod[clientId]].unk_326 += bufSize;
        else if (cmd == svc_playerinfo)
            cl[clientId].frames[cl_parsecountmod[clientId]].unk_324 += bufSize;
        else if (cmd == svc_temp_entity)
            cl[clientId].frames[cl_parsecountmod[clientId]].unk_328 += bufSize;
        else if (cmd == svc_sound)
            cl[clientId].frames[cl_parsecountmod[clientId]].unk_32A += bufSize;
    }

    cl[clientId].frames[cl_parsecountmod[clientId]].unk_32C += net_message.cursize;
    CL_AccumulateMessageLoad();
    func_0024BD88(clientId);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/cl_parse", CL_ParseServerMessage);
#endif
