#include "common.h"
#include "engine/cvar.h"
#include "engine/cmd.h"
#include "engine/mathlib.h"
#include "engine/client.h"
#include "engine/entity_state.h"

int sprintf(char *, const char *, ...);
char *strcat(char *, const char *);
void func_00244880(char *pszFile, char *fmt, ...); /* COM_Log */
int MSG_ReadBits(int numbits);
int MSG_ReadByteBits(int numbits);
int MSG_ReadSBits(int numbits);
void Host_Error(const char *error, ...);
extern int custombitcounts[32];
extern int bitcounts[40];
float func_00242D60(int numbits); /* MSG_ReadBitAngle */
int func_00243278(int numbits);
int func_00243190(int numbits); /* MSG_PeekBits */
int func_00243120(int numbits);
void MSG_StartBitReading(sizebuf_t *buf);
void MSG_EndBitReading(sizebuf_t *buf);
void Host_EndGame(const char *message, ...);
extern sizebuf_t net_message;
extern int msg_badread;
extern int cl_updatemask[2];
void *memset(void *, int, unsigned int);
extern char D_003892C0[];       /* "msg_badread in packetentities\n" */
extern char D_00389290[];       /* "\n" */
extern int cl_updatebackup[2];
extern cl_entity_t *cl_entities[2];
int MSG_ReadShort(void);
int MSG_ReadByte(void);
void func_00250C50(int clientId); /* CL_SignonReply */
cl_entity_t *CL_EntityNum(int number, int clientId);
void func_002B5228(int deadEntity, int clientId);

void func_00248148(char *pszFile, usercmd_t *cmd)
{
    char szLine[1024];

    sprintf(szLine, "t %i", cmd->msec);
    if (cmd->buttons & IN_DUCK)
        strcat(szLine, " duck");
    strcat(szLine, D_00389290);
    func_00244880(pszFile, szLine);
}

int func_002481C0(int *pbits, int *pmorebits)
{
    int bits;
    int morebits;
    int num;

    morebits = 0;
    bits = MSG_ReadByteBits(8);
    if (bits & 0x1)
        bits |= MSG_ReadByteBits(8) << 8;
    if (bits & 0x100)
        bits |= MSG_ReadByteBits(8) << 16;
    if (bits & 0x800000)
        bits |= MSG_ReadByteBits(8) << 24;
    if (bits & 0x80000000)
        morebits = MSG_ReadByteBits(8);

    if (bits & 0x8000)
        num = MSG_ReadBits(10);
    else
        num = MSG_ReadByteBits(8);

    *pbits = bits;
    *pmorebits = morebits;
    return num;
}

INCLUDE_RODATA("asm/nonmatchings/engine/cl_ents", D_00389290);

void func_002482A8(entity_state_t *from, entity_state_t *to, int bits, int morebits, int number, int unused)
{
    int i;

    *to = *from;
    to->entityType = 1;
    to->number = number;

    for (i = 0; i < 32; i++)
    {
        if ((bits >> i) & 1)
            custombitcounts[i]++;
    }

    to->flags = bits;

    if (bits & 0x2)
        to->origin[0] = MSG_ReadSBits(16) * 0.125f;
    if (bits & 0x4)
        to->origin[1] = MSG_ReadSBits(16) * 0.125f;
    if (bits & 0x8)
        to->origin[2] = MSG_ReadSBits(16) * 0.125f;
    if (bits & 0x20)
        to->angles[0] = MSG_ReadSBits(16) * 0.125f;
    if (bits & 0x40)
        to->angles[1] = MSG_ReadSBits(16) * 0.125f;
    if (bits & 0x80)
        to->angles[2] = MSG_ReadSBits(16) * 0.125f;

    if (bits & 0x200)
    {
        to->sequence = MSG_ReadBits(16);
        to->skin = MSG_ReadBits(16);
    }

    if (bits & 0x400)
        to->rendermode = MSG_ReadByteBits(8);

    if (bits & 0x800)
    {
        to->modelindex = MSG_ReadBits(16);
        if (to->modelindex >= 512)
            Host_Error("CL_ParseCustomEntity: bad model number");
    }

    if (bits & 0x1000)
        to->scale = MSG_ReadByteBits(8) * 0.1f;

    if (bits & 0x4000)
        to->body = MSG_ReadByteBits(8);

    if (bits & 0x10000)
    {
        to->rendercolor.r = MSG_ReadByteBits(8);
        to->rendercolor.g = MSG_ReadByteBits(8);
        to->rendercolor.b = MSG_ReadByteBits(8);
        to->renderfx = MSG_ReadByteBits(8);
    }

    if (bits & 0x20000)
        to->renderamt = MSG_ReadByteBits(8);

    if (bits & 0x80000)
        to->frame = MSG_ReadByteBits(8);

    if (bits & 0x40000)
        to->animtime = MSG_ReadByteBits(8) * 0.1f;
}

void func_00248710(entity_state_t *from, entity_state_t *to, int bits, int morebits, int number, int unused)
{
    int i;
    int seq;
    float animtime;

    *to = *from;
    to->number = number;
    to->entityType = 0;

    for (i = 0; i < 32; i++)
    {
        if ((bits >> i) & 1)
            bitcounts[i]++;
    }
    for (i = 0; i < 8; i++)
    {
        if ((morebits >> i) & 1)
            bitcounts[32 + i]++;
    }

    to->flags = bits;

    if (bits & 0x800)
    {
        if (MSG_ReadByteBits(1) == 1)
            to->modelindex = MSG_ReadBits(10);
        else
            to->modelindex = MSG_ReadByteBits(8);
    }

    if (bits & 0x80)
        to->frame = MSG_ReadByteBits(8);

    if (bits & 0x40)
        to->movetype = MSG_ReadByteBits(4);

    if (bits & 0x40000)
        to->colormap = MSG_ReadBits(16);

    if (bits & 0x80000)
    {
        to->skin = MSG_ReadBits(10);
        to->solid = MSG_ReadByteBits(3);
    }

    if (bits & 0x40000000)
        to->scale = MSG_ReadBits(16) * (1.0f / 256.0f);

    if (bits & 0x4000)
        to->effects = MSG_ReadBits(10);

    if (bits & 0x2)
        to->origin[0] = MSG_ReadSBits(16) * 0.125f;
    if (bits & 0x200)
        to->angles[0] = func_00242D60(11);
    if (bits & 0x4)
        to->origin[1] = MSG_ReadSBits(16) * 0.125f;
    if (bits & 0x20)
        to->angles[1] = func_00242D60(13);
    if (bits & 0x8)
        to->origin[2] = MSG_ReadSBits(16) * 0.125f;
    if (bits & 0x400)
        to->angles[2] = func_00242D60(11);

    if (bits & 0x1000)
    {
        seq = MSG_ReadByteBits(8);
        animtime = (int)(cl[0].time * 100.0f);
        animtime = (animtime - ((int)animtime & 0xFF)) * 0.01f;
        animtime += MSG_ReadByteBits(8) * 0.01f;
        while (animtime > cl[0].time + 0.1f)
            animtime -= 2.56f;
        to->sequence = seq;
        to->animtime = animtime;
    }

    if (bits & 0x10000)
        to->framerate = func_00243278(8) * 0.0625f;

    if (bits & 0x1000000)
        to->controller[0] = MSG_ReadByteBits(8);
    if (bits & 0x2000000)
        to->controller[1] = MSG_ReadByteBits(8);
    if (bits & 0x4000000)
        to->controller[2] = MSG_ReadByteBits(8);
    if (bits & 0x8000000)
        to->controller[3] = MSG_ReadByteBits(8);
    if (bits & 0x10000000)
        to->blending[0] = MSG_ReadByteBits(8);
    if (bits & 0x20000000)
        to->blending[1] = MSG_ReadByteBits(8);

    if (bits & 0x400000)
        to->body = MSG_ReadBits(10);

    if (bits & 0x100000)
    {
        to->rendermode = MSG_ReadByteBits(8);
        to->renderamt = MSG_ReadByteBits(8);
        to->rendercolor.r = MSG_ReadByteBits(8);
        to->rendercolor.g = MSG_ReadByteBits(8);
        to->rendercolor.b = MSG_ReadByteBits(8);
        to->renderfx = MSG_ReadByteBits(8);
    }

    if (morebits & 0x1)
        to->mins[0] = MSG_ReadSBits(16) * 0.125f;
    if (morebits & 0x2)
        to->mins[1] = MSG_ReadSBits(16) * 0.125f;
    if (morebits & 0x4)
        to->mins[2] = MSG_ReadSBits(16) * 0.125f;
    if (morebits & 0x8)
        to->maxs[0] = MSG_ReadSBits(16) * 0.125f;
    if (morebits & 0x10)
        to->maxs[1] = MSG_ReadSBits(16) * 0.125f;
    if (morebits & 0x20)
        to->maxs[2] = MSG_ReadSBits(16) * 0.125f;

    if (morebits & 0x40)
        to->aiment = MSG_ReadBits(10);

    if (morebits & 0x100)
    {
        if (MSG_ReadByteBits(1) == 1)
            to->weaponmodel = MSG_ReadBits(10);
        else
            to->weaponmodel = MSG_ReadByteBits(8);
    }
}

void func_00248F40(qboolean bBitReading)
{
    entity_state_t from;
    entity_state_t to;
    int bits;
    int morebits;
    int num;

    memset(&from, 0, sizeof(from));

    cl[0].validsequence = 0;
    cl[0].frames[cls[0].netchan.incoming_sequence & cl_updatemask[0]].invalid = true;

    if (bBitReading)
        MSG_StartBitReading(&net_message);

    while (1)
    {
        if (!func_00243190(32))
            num = func_00243120(32);
        else
            num = func_002481C0(&bits, &morebits);

        if (msg_badread)
        {
            Host_EndGame(D_003892C0);
            break;
        }

        if (!num)
            break;

        if (bits & 0x10)
            continue;

        if (bits & 0x2000)
            func_002482A8(&from, &to, bits, morebits, num, 0);
        else
            func_00248710(&from, &to, bits, morebits, num, 0);
    }

    if (bBitReading)
        MSG_EndBitReading(&net_message);
}

void func_00249070(cl_packet_entities_t *pack)
{
    if (pack->entities)
        Mem_Free(pack->entities);
    pack->entities = NULL;
    pack->num_entities = 0;
}

INCLUDE_RODATA("asm/nonmatchings/engine/cl_ents", D_003892C0);

#ifdef NON_MATCHING
void func_002490B0(qboolean delta, int clientId)
{
    cl_entity_t *ent;
    cl_packet_entities_t dummy;
    cl_packet_entities_t *oldp;
    cl_packet_entities_t *newp;
    int bits;
    int morebits;
    int full;
    int error;
    int oldpacket;
    int newpacket;
    frame_t *newframe;
    int oldindex;
    int newindex;
    int newnum;
    int oldnum;
    int from;
    int count;
    int num;

    error = 0;

    if (cls[clientId].signon == 2)
    {
        cls[clientId].signon = 3;
        func_00250C50(clientId);
    }

    newpacket = cls[clientId].netchan.incoming_sequence & cl_updatemask[clientId];
    newframe = &cl[clientId].frames[newpacket];
    newframe->invalid = 0;
    newp = &newframe->packet_entities;

    count = MSG_ReadShort();
    if (!count)
        count = 1;

    if (newp->entities)
        Mem_Free(newp->entities);
    newp->entities = Mem_DbgMalloc(count * sizeof(entity_state_t), "engine/cl_ents.c", 618);
    if (!newp->entities)
        Sys_Error("CL_ParsePacketEntities:  Failed to allocate space for %i entities.\n", count);
    memset(newp->entities, 0, count * sizeof(entity_state_t));
    newframe->packet_entities.num_entities = count;

    if (delta)
    {
        from = MSG_ReadByte();
        oldpacket = cl[clientId].frames[newpacket].delta_sequence;
        if ((from & cl_updatemask[clientId]) != (oldpacket & cl_updatemask[clientId]))
            Con_DPrintf("WARNING: from mismatch\n");
    }
    else
        oldpacket = -1;

    full = 0;
    if (oldpacket != -1)
    {
        if (cls[clientId].netchan.outgoing_sequence - oldpacket >= cl_updatebackup[clientId] - 1)
        {
            func_00248F40(1);
            func_00249070(newp);
            return;
        }
        cl[clientId].validsequence = cls[clientId].netchan.incoming_sequence;
        oldp = &cl[clientId].frames[oldpacket & cl_updatemask[clientId]].packet_entities;
    }
    else
    {
        memset(&dummy, 0, sizeof(dummy));
        dummy.num_entities = 0;
        oldp = &dummy;
        full = 1;
        cl[clientId].validsequence = cls[clientId].netchan.incoming_sequence;
    }

    newp->num_entities = 0;
    MSG_StartBitReading(&net_message);

    oldindex = 0;
    newindex = 0;

    while (1)
    {
        if (!func_00243190(32))
            newnum = func_00243120(32);
        else
            newnum = func_002481C0(&bits, &morebits);

        if (!newnum)
        {
            while (oldindex < oldp->num_entities)
            {
                if (newindex >= 256)
                    Host_EndGame("CL_ParsePacketEntities: newindex == MAX_PACKET_ENTITIES");
                newp->entities[newindex] = oldp->entities[oldindex];
                newindex++;
                oldindex++;
            }
            break;
        }

        num = newnum;
        oldnum = oldindex >= oldp->num_entities ? 9999 : oldp->entities[oldindex].number;

        while (newnum > oldnum)
        {
            if (full)
            {
                Con_Printf("WARNING: oldcopy on full update");
                error = 1;
                func_00248F40(0);
                func_00249070(newp);
                break;
            }
            if (newindex >= 256)
                Host_EndGame("CL_ParsePacketEntities: newindex == MAX_PACKET_ENTITIES");
            newp->entities[newindex] = oldp->entities[oldindex];
            newindex++;
            oldindex++;
            oldnum = oldindex >= oldp->num_entities ? 9999 : oldp->entities[oldindex].number;
        }

        if (newnum < oldnum)
        {
            if (bits & 0x10)
            {
                if (full)
                {
                    cl[clientId].validsequence = 0;
                    Con_Printf("WARNING: U_REMOVE on full update\n");
                    error = 1;
                    func_00248F40(0);
                    func_00249070(newp);
                    break;
                }
                continue;
            }

            if (newindex >= 256)
                Host_EndGame("CL_ParsePacketEntities: newindex == MAX_PACKET_ENTITIES");

            CL_EntityNum(newnum, clientId);
            ent = &cl_entities[clientId][num];
            if (bits & 0x2000)
                func_002482A8(&ent->baseline, &newp->entities[newindex], bits, morebits, newnum, 1);
            else
                func_00248710(&ent->baseline, &newp->entities[newindex], bits, morebits, newnum, 1);
            newindex++;
            continue;
        }

        if (newnum == oldnum)
        {
            if (full)
            {
                cl[clientId].validsequence = 0;
                Con_Printf("WARNING: delta on full update");
            }
            if (bits & 0x10)
            {
                func_002B5228(newnum, clientId);
                oldindex++;
                continue;
            }
            if (bits & 0x2000)
                func_002482A8(&oldp->entities[oldindex], &newp->entities[newindex], bits, morebits, newnum, 0);
            else
                func_00248710(&oldp->entities[oldindex], &newp->entities[newindex], bits, morebits, newnum, 0);
            newindex++;
            oldindex++;
        }
    }

    if (!error)
        newp->num_entities = newindex;

    MSG_EndBitReading(&net_message);

    if (msg_badread)
        Host_EndGame(D_003892C0);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/cl_ents", func_002490B0);
#endif

INCLUDE_ASM("asm/nonmatchings/engine/cl_ents", func_00249868);

INCLUDE_ASM("asm/nonmatchings/engine/cl_ents", func_00249B50);

INCLUDE_ASM("asm/nonmatchings/engine/cl_ents", func_00249CE8);

INCLUDE_ASM("asm/nonmatchings/engine/cl_ents", func_00249D98);

INCLUDE_ASM("asm/nonmatchings/engine/cl_ents", func_0024A160);

INCLUDE_ASM("asm/nonmatchings/engine/cl_ents", func_0024ABF8);

INCLUDE_ASM("asm/nonmatchings/engine/cl_ents", func_0024AD40);

INCLUDE_ASM("asm/nonmatchings/engine/cl_ents", func_0024B920);

INCLUDE_ASM("asm/nonmatchings/engine/cl_ents", func_0024BD88);

INCLUDE_ASM("asm/nonmatchings/engine/cl_ents", func_0024C060);

INCLUDE_ASM("asm/nonmatchings/engine/cl_ents", func_0024C120);
