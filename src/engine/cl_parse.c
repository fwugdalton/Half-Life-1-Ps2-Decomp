#include "common.h"
#include "engine/cmd.h"
#include "engine/cvar.h"
#include "engine/server.h"
#include "engine/client.h"

/* cl_parse.c: interpretação das mensagens do servidor no cliente (um cl[] por jogador local) */

#define MAX_LOCAL 2
#define MAX_QPATH 64
#define MAX_OSPATH 260
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
extern int D_00399490[MAX_LOCAL];
extern char com_gamedir[];
extern void (*D_003997C0)(char *gamedir);
extern int protocol_version;
extern int D_00332F2C;
extern float D_003334D4; /* r_decals.value */
extern float D_003334EC; /* mp_decals.value */
#define r_decals_value D_003334D4
#define mp_decals_value D_003334EC
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

INCLUDE_ASM("asm/nonmatchings/engine/cl_parse", func_00295150);

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
    D_00399490[clientId] &= 7;

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
        if (mp_decals_value < r_decals_value)
            Cvar_SetValue("r_decals", mp_decals_value);
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

INCLUDE_ASM("asm/nonmatchings/engine/cl_parse", func_00296138);

INCLUDE_ASM("asm/nonmatchings/engine/cl_parse", func_002962A0);

void func_00296658(void) {
}

INCLUDE_ASM("asm/nonmatchings/engine/cl_parse", func_00296660);

INCLUDE_ASM("asm/nonmatchings/engine/cl_parse", func_002967C8);

INCLUDE_ASM("asm/nonmatchings/engine/cl_parse", func_00296930);

INCLUDE_ASM("asm/nonmatchings/engine/cl_parse", func_002969C0);

INCLUDE_ASM("asm/nonmatchings/engine/cl_parse", func_00296B90);

INCLUDE_ASM("asm/nonmatchings/engine/cl_parse", func_00296CE0);

INCLUDE_ASM("asm/nonmatchings/engine/cl_parse", func_00296DC0);

INCLUDE_ASM("asm/nonmatchings/engine/cl_parse", func_00296E00);
