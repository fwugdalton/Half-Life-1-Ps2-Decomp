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
extern char D_00389290[];       /* "\n" */

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

void func_002482A8(entity_state_t *from, entity_state_t *to, int bits, int morebits, int number)
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

INCLUDE_ASM("asm/nonmatchings/engine/cl_ents", func_00248710);

INCLUDE_ASM("asm/nonmatchings/engine/cl_ents", func_00248F40);

void func_00249070(cl_packet_entities_t *pack)
{
    if (pack->entities)
        Mem_Free(pack->entities);
    pack->entities = NULL;
    pack->num_entities = 0;
}

INCLUDE_RODATA("asm/nonmatchings/engine/cl_ents", D_003892C0);

INCLUDE_ASM("asm/nonmatchings/engine/cl_ents", func_002490B0);

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
