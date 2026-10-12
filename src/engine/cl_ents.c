#include "common.h"
#include "engine/cvar.h"
#include "engine/cmd.h"
#include "engine/mathlib.h"
#include "engine/client.h"
#include "engine/entity_state.h"
#include "engine/model.h"
#include "engine/pmove.h"

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
extern vec3_t vec3_origin;

typedef struct particle_s
{
    vec3_t org;                 /* 0x00 */
    short color;                /* 0x0C */
    short packedColor;          /* 0x0E */
    struct particle_s *next;    /* 0x10 */
    vec3_t vel;                 /* 0x14 */
    float ramp;                 /* 0x20 */
    float die;                  /* 0x24 */
    int type;                   /* 0x28 */
} particle_t; /* 0x2C */

extern particle_t *D_0039966C;  /* free_particles */
extern particle_t *D_00399660;  /* active_particles */

typedef struct
{
    vec3_t origin;      /* 0x00 */
    float radius;       /* 0x0C */
    color24 color;      /* 0x10 */
    float die;          /* 0x14 */
    float decay;        /* 0x18 */
    float minlight;     /* 0x1C */
    int key;            /* 0x20 */
    qboolean dark;      /* 0x24 */
} dlight_t;

dlight_t *func_002510D0(int key); /* CL_AllocDlight */
void func_002514D0(dlight_t *dl);
void Pmove_Init(void);
float anglemod(float a);
int VectorCompare(const vec3_t v1, const vec3_t v2);
void func_002578C0(vec3_t pos, int clientId);
void func_002B2F08(vec3_t start, vec3_t end, int type, int clientId); /* R_RocketTrail */
extern int cl_numvisedicts[2];
extern int cl_parsecountmod[2];
extern float realtime;
extern int playerbitcounts[32];
extern float D_00399498[2];
extern int D_003994A0;              /* modelindex padrão do jogador */
int func_00243318(int numbits);     /* coordenada em bits */
void func_00243AA0(usercmd_t *to, usercmd_t *from); /* MSG_ReadDeltaUsercmd */
/* máscaras PF_* (ver sv_main.c) */
extern int D_00399354;
extern int D_00399358;
extern int D_00399360;
extern int D_00399364;
extern int D_00399368;
extern int D_0039936C;
extern int D_00399374;
extern int D_00399378;
extern int D_0039937C;
extern int D_00399380;
extern int D_00399384;
extern int D_00399388;
extern int D_0039938C;
extern int D_00399390;
extern int D_00399394;
extern int D_00399398;
extern int D_0039939C;
extern int D_003993A0;
extern int D_003993A4;
extern int D_003993A8;
extern int D_003993AC;
extern int D_003993B0;
extern int D_003993B4;
extern int D_003993B8;
extern int D_003993C0;
extern int D_003993C4;
extern int D_003993CC;
extern int D_003993D0;
extern int D_003993D4;
extern int D_003993D8;
extern int D_003993DC;
extern int D_003993E0;
extern int D_003993E4;
char *func_002447B8(vec3_t v); /* vtos */
extern cl_entity_t *cl_visedicts[2];
extern int D_00399658[2];               /* cl_numbeamentities */
extern cl_entity_t *D_0050EF88[2][64];  /* cl_beamentities */
extern int D_003AC498[2][166];          /* bits de entidades vistas no último frame */
float RandomFloat(float flLow, float flHigh);
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

void func_00249868(cl_entity_t *ent)
{
    Con_DPrintf("----------------------------\n");
    Con_DPrintf("T %.2f", cl[0].time);
    Con_DPrintf(":Gap %.2f\n", cl[0].time - ent->animtime);
    Con_DPrintf("#%i", ent->index);
    if (ent->model)
        Con_DPrintf(":%s\n", ent->model);
    else
        Con_DPrintf(":?\n");
    Con_DPrintf("AT %4.2f ST %4.2f S %i F %4.1f\n", ent->animtime, ent->sequencetime, ent->sequence, ent->frame);
    Con_DPrintf("PA %4.2f PS %i FR %.1f\n", ent->prevanimtime, ent->prevsequence, ent->framerate);
    Con_DPrintf("C0 %i:%i BL %i:%i\n", ent->controller[0], ent->prevcontroller[0], ent->blending[0], ent->prevblending[0]);
    Con_DPrintf("O : %.0f %.0f %.0f\n", ent->origin[0], ent->origin[1], ent->origin[2]);
    Con_DPrintf("PO: %.0f %.0f %.0f\n", ent->prevorigin[0], ent->prevorigin[1], ent->prevorigin[2]);
    Con_DPrintf("RM:  %i RA: %i RX: %i PF %4.2f\n", ent->rendermode, ent->renderamt, ent->renderfx, ent->prevframe);
    Con_DPrintf("MT: %i:", ent->movetype);
    if (ent->unk194 & 0x20)
        Con_DPrintf("NoInterp ");
    if (ent->unk194 & 0x80)
        Con_DPrintf("NoDraw ");
    if (ent->unk194 & 0x40)
        Con_DPrintf("Light ");
    if (ent->unk194 & 0x2)
        Con_DPrintf("MUZ ");
    if (ent->unk194 & 0x4)
        Con_DPrintf("BLT ");
    if (ent->unk194 & 0x8)
        Con_DPrintf("Dim ");
    if (ent->unk194 & 0x10)
        Con_DPrintf("Inv ");
    if (ent->unk18C)
        Con_DPrintf("F ");
    Con_DPrintf(D_00389290);
}

void func_00249B50(cl_entity_t *ent, entity_state_t *state, qboolean bSync)
{
    int i;

    ent->index = state->number;
    ent->model = cl[0].model_precache[state->modelindex];
    if (ent->model && bSync)
    {
        if (ent->model->synctype == 1)
            ent->syncbase = RandomFloat(0, 1);
        else
            ent->syncbase = 0;
    }

    ent->colormap = state->colormap;
    ent->unk4C = 0;
    ent->skin = state->skin;
    ent->frame = state->frame;
    ent->unk194 = state->effects;
    ent->framerate = state->framerate;
    ent->rendermode = state->rendermode;
    ent->renderamt = state->renderamt;
    ent->rendercolor.r = state->rendercolor.r;
    ent->rendercolor.g = state->rendercolor.g;
    ent->rendercolor.b = state->rendercolor.b;
    ent->renderfx = state->renderfx;

    for (i = 0; i < 4; i++)
        ent->controller[i] = state->controller[i];
    for (i = 0; i < 2; i++)
        ent->blending[i] = state->blending[i];

    VectorCopy(state->origin, ent->origin);
    VectorCopy(state->angles, ent->angles);
    ent->scale = state->scale;
    ent->sequence = state->sequence;
    ent->animtime = state->animtime;
    ent->movetype = state->movetype;
    ent->body = state->body;
    ent->aiment = state->aiment;
}

void func_00249CE8(vec3_t org, int color, float life, int zpos, int zvel)
{
    particle_t *p;

    if (!D_0039966C)
        return;

    p = D_0039966C;
    D_0039966C = p->next;
    p->next = D_00399660;
    D_00399660 = p;

    p->die = cl[0].time + life;
    p->packedColor = 0;
    p->color = color;
    p->type = 0;
    VectorCopy(vec3_origin, p->vel);
    p->vel[2] += zvel;
    VectorCopy(org, p->org);
    p->org[2] += zpos;
}

void func_00249D98(int i, int clientId)
{
    cl_entity_t *ent;
    player_state_t *state;
    dlight_t *dl;
    vec3_t forward;
    pmtrace_t trace;
    vec3_t end;
    float falloff;

    ent = &cl_entities[clientId][i + 1];
    state = &cl[clientId].frames[cl[clientId].parsecount & cl_updatemask[clientId]].playerstate[i];

    if ((ent->unk194 & (4 | 8)) && cl[clientId].worldmodel)
    {
        if (cl[clientId].flashlights[i] && ((dlight_t *)cl[clientId].flashlights[i])->key == i + 1)
            dl = cl[clientId].flashlights[i];
        else
            dl = func_002510D0(i + 1);

        if (ent->unk194 & 4)
        {
            dl->color.r = dl->color.g = dl->color.b = 250;
            dl->radius = 400;
            VectorCopy(ent->origin, dl->origin);
            dl->origin[2] += 16;
        }
        else
        {
            VectorCopy(ent->origin, dl->origin);
            dl->origin[2] += cl[clientId].viewheight;
            AngleVectors(state->viewangles, forward, NULL, NULL);
            VectorMA(dl->origin, 2000, forward, end);

            pmove[clientId].usehull = 2;
            Pmove_Init();
            trace = PM_PlayerTrace(dl->origin, end, 2, clientId);
            if (trace.ent > 0 && pmove[clientId].physents[trace.ent].studiomodel)
                VectorCopy(pmove[clientId].physents[trace.ent].origin, trace.endpos);

            VectorCopy(trace.endpos, dl->origin);

            falloff = trace.fraction * 2000;
            if (falloff < 500)
                falloff = 1.0f;
            else
                falloff = 500.0f / falloff;
            falloff *= falloff;

            dl->radius = 80;
            dl->color.r = dl->color.g = dl->color.b = 255 * falloff;
        }

        cl[clientId].flashlights[i] = dl;
        dl->die = cl[clientId].time + 0.2f;
        func_002514D0(dl);
    }
    else
    {
        if (cl[clientId].flashlights[i] && ((dlight_t *)cl[clientId].flashlights[i])->key == i + 1)
            ((dlight_t *)cl[clientId].flashlights[i])->die = cl[clientId].time;
        cl[clientId].flashlights[i] = NULL;
    }

    if (cl_entities[clientId][cl[clientId].viewentity].unk194 & 2)
        cl[clientId].viewent.unk194 |= 2;
}

#ifdef NON_MATCHING
void func_0024A160(int clientId)
{
    cl_entity_t old;
    vec3_t oldorg;
    cl_packet_entities_t *pack;
    entity_state_t *s1;
    cl_entity_t *ent;
    model_t *model;
    dlight_t *dl;
    int pnum;
    int bitset;
    float autorotate;
    float time;

    pack = &cl[clientId].frames[cls[clientId].netchan.incoming_sequence & cl_updatemask[clientId]].packet_entities;
    autorotate = anglemod(100 * cl[clientId].time);
    time = cl[clientId].time;

    for (pnum = 0; pnum < pack->num_entities; pnum++)
    {
        s1 = &pack->entities[pnum];

        if (!s1->modelindex)
            continue;
        if (cl_numvisedicts[clientId] >= 512)
            break;
        if (s1->effects & 0x80)
            continue;

        ent = &cl_visedicts[clientId][cl_numvisedicts[clientId]++];
        *ent = cl_entities[clientId][s1->number];
        old = cl_entities[clientId][s1->number];
        bitset = D_003AC498[clientId][ent->index / 8] & (1 << (ent->index % 8));
        func_00249B50(ent, s1, 1);

        model = ent->model;
        if (s1->entityType)
        {
            ent->movetype = s1->modelindex;
            if (model && model->type != 1)
                Sys_Error("Bad model on beam");
            ent->prevsequence = ent->sequence;
            VectorCopy(ent->origin, ent->prevorigin);
            VectorCopy(ent->angles, ent->prevangles);
            cl_entities[clientId][ent->index] = *ent;
            cl_numvisedicts[clientId]--;
            if (D_00399658[clientId] < 64)
                D_0050EF88[clientId][D_00399658[clientId]++] = &cl_entities[clientId][ent->index];
            else
                Con_DPrintf("Overflow beam entity list!\n");
            continue;
        }

        if (old.animtime != ent->animtime)
        {
            if (old.sequence != ent->sequence)
            {
                ent->sequencetime = ent->animtime;
                ent->prevsequence = old.sequence;
                ent->prevseqblending[0] = old.blending[0];
                ent->prevseqblending[1] = old.blending[1];
            }
            ent->prevanimtime = old.animtime;
            ent->prevcontroller[0] = old.controller[0];
            ent->prevcontroller[1] = old.controller[1];
            ent->prevcontroller[2] = old.controller[2];
            ent->prevcontroller[3] = old.controller[3];
            ent->prevblending[0] = old.blending[0];
            ent->prevblending[1] = old.blending[1];
            VectorCopy(old.origin, ent->prevorigin);
            VectorCopy(old.angles, ent->prevangles);
        }
        else if (!VectorCompare(ent->origin, old.origin))
        {
            ent->unk70 = time + 0.2f;
            VectorCopy(ent->origin, ent->prevorigin);
            VectorCopy(ent->angles, ent->prevangles);
        }

        if ((s1->flags & 0x200000) && ent->unk70 < cl[clientId].time)
            ent->movetype = 4;
        else
            ent->movetype = 0;

        if (ent->aiment)
            ent->movetype = 12;

        if (!bitset)
            ent->unk18C = 1;
        if (!bitset || (ent->unk194 & 0x20))
        {
            ent->prevsequence = ent->sequence;
            VectorCopy(ent->origin, ent->prevorigin);
            VectorCopy(ent->angles, ent->prevangles);
            ent->animtime = time;
            ent->prevanimtime = time;
        }

        bitset = ent->index;
        if (bitset != 1)
        {
            if (ent->unk194 & 4)
            {
                dl = func_002510D0(bitset);
                VectorCopy(ent->origin, dl->origin);
                dl->origin[2] += 16;
                dl->color.r = dl->color.g = dl->color.b = 250;
                dl->radius = RandomFloat(400, 431);
                dl->die = cl[clientId].time + 0.001f;
            }
            if (ent->unk194 & 8)
            {
                dl = func_002510D0(bitset);
                VectorCopy(ent->origin, dl->origin);
                dl->color.r = dl->color.g = dl->color.b = 100;
                dl->radius = RandomFloat(200, 231);
                dl->die = cl[clientId].time + 0.001f;
            }
        }

        if (ent->unk194 & 0x40)
        {
            func_002578C0(ent->origin, clientId);
            if (cl_numlocal < 2)
            {
                dl = func_002510D0(bitset);
                VectorCopy(ent->origin, dl->origin);
                dl->radius = 200;
                dl->color.r = dl->color.g = dl->color.b = 100;
                dl->die = cl[clientId].time + 0.001f;
            }
        }

        if (model)
        {
            if (ent->model->flags & 4)
                func_002B2F08(oldorg, ent->origin, 2, clientId);
            else if (ent->model->flags & 0x20)
                func_002B2F08(oldorg, ent->origin, 4, clientId);
            else if (ent->model->flags & 0x10)
                func_002B2F08(oldorg, ent->origin, 3, clientId);
            else if (ent->model->flags & 0x40)
                func_002B2F08(oldorg, ent->origin, 5, clientId);
            else if (ent->model->flags & 1)
            {
                func_002B2F08(oldorg, ent->origin, 0, clientId);
                dl = func_002510D0(bitset);
                VectorCopy(ent->origin, dl->origin);
                dl->radius = 200;
                dl->color.r = dl->color.g = dl->color.b = 200;
                dl->die = cl[clientId].time + 0.01f;
            }
            else if (ent->model->flags & 2)
                func_002B2F08(oldorg, ent->origin, 1, clientId);
            else if (ent->model->flags & 0x80)
                func_002B2F08(oldorg, ent->origin, 6, clientId);
        }

        ent->weaponmodel = s1->weaponmodel;
        cl_entities[clientId][ent->index] = *ent;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/cl_ents", func_0024A160);
#endif

void func_0024ABF8(player_state_t *state, int pnum, int clientId)
{
    player_state_t *prev;
    int i;

    for (i = 0; i < cl_updatebackup[clientId] - 1; i++)
    {
        prev = &cl[clientId].frames[(cl_parsecountmod[clientId] - i) & cl_updatemask[clientId]].playerstate[pnum];
        if (prev == state)
            Con_Printf("*");
        Con_Printf("%s", func_002447B8(prev->origin));
        Con_Printf(" pred %s\n", func_002447B8(prev->predorigin));
    }
}

#ifdef NON_MATCHING
void func_0024AD40(int clientId)
{
    vec3_t oldorigin;
    vec3_t oldangles;
    vec3_t origin;
    usercmd_t nullcmd;
    int flags;
    int num;
    int spec;
    int pmflags;
    int i;
    player_info_t *info;
    player_state_t *state;
    cl_entity_t *ent;
    float animtime;
    int seq;
    unsigned int x;

    spec = 0;

    flags = MSG_ReadByte();
    if (flags & D_00399398)
        flags |= MSG_ReadByte() << 8;
    if (flags & D_003993E4)
        flags |= MSG_ReadByte() << 16;
    if (flags & D_00399358)
        flags |= MSG_ReadByte() << 24;

    MSG_StartBitReading(&net_message);

    num = MSG_ReadByteBits(6);
    if (num & 0x40)
    {
        spec = 1;
        num &= ~0x40;
    }
    if (num > 3)
        Sys_Error("CL_ParsePlayerinfo: bad num");

    info = &cl[clientId].players[num];
    info->spectator = spec;

    state = &cl[clientId].frames[cl_parsecountmod[clientId]].playerstate[num];
    state->msg_time = realtime;
    state->number = num;
    state->messagenum = cl[clientId].parsecount;
    state->flags = flags;

    pmflags = MSG_ReadByteBits(5);
    state->pmflags = 8;
    if (pmflags & D_003993D4)
        state->pmflags = 0x1000008;
    if (pmflags & D_003993A4)
        state->pmflags |= 0x4000;
    if (pmflags & D_003993B4)
        state->pmflags |= 0x1000;
    if (pmflags & D_0039936C)
        state->pmflags |= 0x4000000;
    if (pmflags & D_00399380)
        state->pmflags |= 0x800;

    origin[0] = func_00243318(18) * 0.03125f;
    origin[1] = func_00243318(18) * 0.03125f;
    origin[2] = func_00243318(18) * 0.03125f;
    VectorCopy(origin, state->origin);
    VectorSubtract(state->origin, state->predorigin, state->deltaorigin);

    state->frame = MSG_ReadByteBits(8);

    for (i = 0; i < 32; i++)
    {
        if (flags & (1 << i))
            playerbitcounts[i]++;
    }

    if (flags & D_00399354)
    {
        memset(&nullcmd, 0, sizeof(nullcmd));
        func_00243AA0(&state->command, &nullcmd);
        VectorCopy(state->command.viewangles, state->viewangles);
    }
    else
        VectorCopy(vec3_origin, state->viewangles);

    if (flags & D_00399394)
        state->state_time = D_00399498[clientId] - MSG_ReadByteBits(8) * 0.001f;
    else
        state->state_time = D_00399498[clientId];

    if (flags & D_00399374)
    {
        state->velocity[0] = MSG_ReadSBits(14) * 0.125f;
        state->velocity[1] = MSG_ReadSBits(14) * 0.125f;
    }
    else
    {
        state->velocity[0] = 0;
        state->velocity[1] = 0;
    }

    if (flags & D_003993A0)
        state->velocity[2] = MSG_ReadSBits(14) * 0.125f;
    else
        state->velocity[2] = 0;

    if ((flags & D_003993CC) || (flags & D_0039939C))
    {
        if (flags & D_003993CC)
            state->modelindex = MSG_ReadBits(10);
        else
            state->modelindex = MSG_ReadByteBits(8);
    }
    else
        state->modelindex = D_003994A0;

    if (flags & D_003993A8)
        state->skin = MSG_ReadBits(10);
    else
        state->skin = 0;

    if (flags & D_00399368)
        state->effects = MSG_ReadBits(10);
    else
        state->effects = 0;

    if ((flags & D_003993D8) || (flags & D_003993C0))
    {
        if (flags & D_003993D8)
            state->weaponmodel = MSG_ReadBits(10);
        else
            state->weaponmodel = MSG_ReadByteBits(8);
    }
    else
        state->weaponmodel = 0;

    if (flags & D_003993DC)
        state->movetype = MSG_ReadByteBits(4);
    else
        state->movetype = 3;

    if (flags & D_00399388)
    {
        seq = MSG_ReadByteBits(8);
        animtime = (int)(cl[clientId].time * 100.0f);
        animtime = (animtime - ((int)animtime & 0xFF)) * 0.01f;
        animtime += MSG_ReadByteBits(8) * 0.01f;
        while (animtime > cl[clientId].time + 0.1f)
            animtime -= 2.56f;
        state->sequence = seq;
        state->animtime = animtime;
    }

    if (flags & D_003993D0)
    {
        state->rendermode = MSG_ReadByteBits(8);
        state->renderamt = MSG_ReadByteBits(8);
        state->rendercolor.r = MSG_ReadByteBits(8);
        state->rendercolor.g = MSG_ReadByteBits(8);
        state->rendercolor.b = MSG_ReadByteBits(8);
        state->renderfx = MSG_ReadByteBits(8);
    }
    else
    {
        state->rendermode = 0;
        state->renderamt = 0;
        state->rendercolor.r = 0;
        state->rendercolor.g = 0;
        state->rendercolor.b = 0;
        state->renderfx = 0;
    }

    if (flags & D_0039938C)
        state->framerate = MSG_ReadByteBits(8) * 0.0625f;
    else
        state->framerate = 1.0f;

    if (flags & D_00399360)
        state->body = MSG_ReadByteBits(8);
    else
        state->body = 0;

    if (flags & D_003993B8)
        state->controller[0] = MSG_ReadByteBits(8);
    else
        state->controller[0] = 0;
    if (flags & D_00399390)
        state->controller[1] = MSG_ReadByteBits(8);
    else
        state->controller[1] = 0;
    if (flags & D_003993AC)
        state->controller[2] = MSG_ReadByteBits(8);
    else
        state->controller[2] = 0;
    if (flags & D_003993E0)
        state->controller[3] = MSG_ReadByteBits(8);
    else
        state->controller[3] = 0;
    if (flags & D_00399364)
        state->blending[0] = MSG_ReadByteBits(8);
    else
        state->blending[0] = 0;
    if (flags & D_003993C4)
        state->blending[1] = MSG_ReadByteBits(8);
    else
        state->blending[1] = 0;

    if (flags & D_0039937C)
    {
        state->basevelocity[0] = MSG_ReadSBits(16) * 0.125f;
        state->basevelocity[1] = MSG_ReadSBits(16) * 0.125f;
        state->basevelocity[2] = MSG_ReadSBits(16) * 0.125f;
    }
    else
        VectorCopy(vec3_origin, state->basevelocity);

    if (flags & D_003993B0)
        state->friction = MSG_ReadSBits(16) * 0.125f;
    else
        state->friction = 1.0f;

    if (flags & D_00399384)
    {
        info->unk_E8 = MSG_ReadBits(12);
        info->unk_EA = x = MSG_ReadByteBits(6);
        if (x > 100)
            info->unk_EA = 100;
        else
            info->unk_EA = x;
    }

    if (flags & D_00399378)
        state->gaitsequence = MSG_ReadByteBits(8);
    else
        state->gaitsequence = 0;

    MSG_EndBitReading(&net_message);

    ent = &cl_entities[clientId][num + 1];
    VectorCopy(ent->origin, oldorigin);
    VectorCopy(ent->angles, oldangles);
    VectorCopy(state->origin, ent->origin);
    VectorCopy(state->viewangles, ent->angles);
    if (cl_numlocal == 1)
        ent->angles[0] = -ent->angles[0] * 0.33333334f;
    else
        ent->angles[0] = -ent->angles[0] * 0.16666667f;

    ent->frame = state->frame;
    ent->movetype = state->movetype;
    ent->unk194 = state->effects;
    ent->model = cl[clientId].model_precache[state->modelindex];
    ent->skin = state->skin;
    ent->body = state->body;

    if (ent->animtime != state->animtime)
    {
        if (state->sequence != ent->sequence)
        {
            ent->sequencetime = state->animtime;
            ent->prevsequence = ent->sequence;
            ent->prevseqblending[0] = ent->blending[0];
            ent->prevseqblending[1] = ent->blending[1];
            ent->sequence = state->sequence;
        }
        ent->prevanimtime = ent->animtime;
        ent->prevcontroller[0] = ent->controller[0];
        ent->prevcontroller[1] = ent->controller[1];
        ent->prevcontroller[2] = ent->controller[2];
        ent->prevcontroller[3] = ent->controller[3];
        ent->prevblending[0] = ent->blending[0];
        ent->prevblending[1] = ent->blending[1];
        VectorCopy(oldorigin, ent->prevorigin);
        VectorCopy(oldangles, ent->prevangles);
        ent->animtime = state->animtime;
    }

    ent->rendermode = state->rendermode;
    ent->renderamt = state->renderamt;
    ent->renderfx = state->renderfx;
    ent->rendercolor.r = state->rendercolor.r;
    ent->rendercolor.g = state->rendercolor.g;
    ent->rendercolor.b = state->rendercolor.b;
    ent->framerate = state->framerate;

    for (i = 0; i < 4; i++)
        ent->controller[i] = state->controller[i];
    for (i = 0; i < 2; i++)
        ent->blending[i] = state->blending[i];
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/cl_ents", func_0024AD40);
#endif

INCLUDE_ASM("asm/nonmatchings/engine/cl_ents", func_0024B920);

INCLUDE_ASM("asm/nonmatchings/engine/cl_ents", func_0024BD88);

INCLUDE_ASM("asm/nonmatchings/engine/cl_ents", func_0024C060);

INCLUDE_ASM("asm/nonmatchings/engine/cl_ents", func_0024C120);
