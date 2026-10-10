#include "common.h"
#include "engine/cmd.h"
#include "engine/cvar.h"
#include "engine/server.h"
#include "engine/client.h"

/* cl_parse.c: interpretação das mensagens do servidor no cliente (um cl[] por jogador local) */

#define MAX_LOCAL 2
#define MAX_QPATH 64
#define SVC_LASTMSG 53
#define MAX_USERMSGS 72

#define SND_FL_VOLUME (1 << 0)
#define SND_FL_ATTENUATION (1 << 1)
#define SND_FL_LARGE_INDEX (1 << 2)
#define SND_FL_PITCH (1 << 3)
#define SND_FL_SENTENCE (1 << 4)
#define PITCH_NORM 100
#define CHAN_STATIC 6

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

void *memset(void *, int, unsigned int);
char *strcat(char *, const char *);
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

INCLUDE_ASM("asm/nonmatchings/engine/cl_parse", func_00295220);

INCLUDE_ASM("asm/nonmatchings/engine/cl_parse", func_00295358);

INCLUDE_ASM("asm/nonmatchings/engine/cl_parse", func_00295380);

INCLUDE_ASM("asm/nonmatchings/engine/cl_parse", func_00295388);

INCLUDE_ASM("asm/nonmatchings/engine/cl_parse", func_002954E0);

INCLUDE_ASM("asm/nonmatchings/engine/cl_parse", func_002955B0);

INCLUDE_ASM("asm/nonmatchings/engine/cl_parse", func_00295628);

INCLUDE_ASM("asm/nonmatchings/engine/cl_parse", func_00295690);

INCLUDE_ASM("asm/nonmatchings/engine/cl_parse", func_00295718);

INCLUDE_ASM("asm/nonmatchings/engine/cl_parse", func_002957E0);

INCLUDE_ASM("asm/nonmatchings/engine/cl_parse", func_002958C8);

INCLUDE_ASM("asm/nonmatchings/engine/cl_parse", func_00295B58);

void func_00295B60(void) {
}

void func_00295B68(void) {
}

INCLUDE_ASM("asm/nonmatchings/engine/cl_parse", func_00295B70);

INCLUDE_ASM("asm/nonmatchings/engine/cl_parse", func_00295BC8);

INCLUDE_ASM("asm/nonmatchings/engine/cl_parse", func_00295D70);

INCLUDE_ASM("asm/nonmatchings/engine/cl_parse", func_00295EA0);

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
