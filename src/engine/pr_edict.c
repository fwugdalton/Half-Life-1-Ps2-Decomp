#include "common.h"
#include "engine/mathlib.h"
#include "engine/model.h"
#include "engine/server.h"
#include "engine/cvar.h"

extern char *pr_strings;
extern int D_00332374, D_00332378, D_0033237C, D_00332380, D_00332384;

void Host_Error(const char *error, ...);
void SV_UnlinkEdict(edict_t *ent);
void ReleaseEntityDLLFields(edict_t *pEdict);
void InitEntityDLLFields(edict_t *pEdict);
void FreeEntPrivateData(edict_t *pEdict);
void ED_FreeLeafs(edict_t *pEdict);
void *Mem_DbgCalloc(int num, int size, const char *file, int line);
void Mem_PushTag(void);
void Mem_SetTag(int tag);
void Mem_PopTag(void);
void *Small_Calloc(int num, int size);
void Small_Free(void *p);
void *memset(void *s, int c, unsigned int n);
void *memcpy(void *d, const void *s, unsigned int n);

void ED_ClearEdict(edict_t *e)
{
    memset(&e->v, 0, sizeof(e->v));
    e->free = false;
    ReleaseEntityDLLFields(e);
    InitEntityDLLFields(e);
}

edict_t *ED_Alloc(void)
{
    int i;
    edict_t *e;

    for (i = svs.maxclients + 1; i < sv.num_edicts; i++)
    {
        e = &sv.edicts[i];
        if (e->free && (e->freetime < 2.0f || sv.time - e->freetime > 0.5f))
        {
            ED_ClearEdict(e);
            return e;
        }
    }

    if (i >= sv.max_edicts)
    {
        if (!sv.max_edicts)
            Sys_Error("ED_Alloc: No edicts yet");
        Sys_Error("ED_Alloc: no free edicts");
    }

    sv.num_edicts++;
    e = &sv.edicts[i];

    ED_ClearEdict(e);
    return e;
}

void ED_Free(edict_t *ed)
{
    if (!ed->free)
    {
        SV_UnlinkEdict(ed);
        ED_FreeLeafs(ed);
        FreeEntPrivateData(ed);
        ed->free = true;
        ed->serialnumber++;
        ed->v.flags = 0;
        ed->v.model = 0;
        ed->v.takedamage = 0;
        ed->v.modelindex = 0;
        ed->v.colormap = 0;
        ed->v.skin = 0;
        ed->v.frame = 0;
        ed->v.scale = 0;
        ed->v.gravity = 0;
        VectorCopy(vec3_origin, ed->v.origin);
        VectorCopy(vec3_origin, ed->v.angles);
        ed->v.nextthink = -1;
        ed->v.solid = 0;
        ed->freetime = sv.time;
    }
}

INCLUDE_ASM("asm/nonmatchings/engine/pr_edict", ED_Count);

INCLUDE_ASM("asm/nonmatchings/engine/pr_edict", ED_NewString);

INCLUDE_ASM("asm/nonmatchings/engine/pr_edict", ED_ParseEdict);

INCLUDE_ASM("asm/nonmatchings/engine/pr_edict", ED_LoadFromFile);

void PR_Init(void)
{
}

edict_t *EDICT_NUM(int n)
{
    if (n < 0 || n >= sv.max_edicts)
        Sys_Error("EDICT_NUM: bad number %i", n);
    return &sv.edicts[n];
}

int NUM_FOR_EDICT(const edict_t *e)
{
    int b;

    b = ((byte *)e - (byte *)sv.edicts) / sizeof(edict_t);
    if (b < 0 || b >= sv.num_edicts)
        Sys_Error("NUM_FOR_EDICT: bad pointer");
    return b;
}

INCLUDE_ASM("asm/nonmatchings/engine/pr_edict", SuckOutClassname);

void ReleaseEntityDLLFields(edict_t *pEdict)
{
    ED_FreeLeafs(pEdict);
    FreeEntPrivateData(pEdict);
}

void InitEntityDLLFields(edict_t *pEdict)
{
    pEdict->v.pContainingEntity = pEdict;
}

void *PvAllocEntPrivateData(edict_t *pEdict, int cb)
{
    FreeEntPrivateData(pEdict);

    if (cb <= 0)
        return NULL;

    Mem_PushTag();
    Mem_SetTag(2);
    pEdict->pvPrivateData = Mem_DbgCalloc(1, cb, "engine/Pr_edict.c", 576);
    Mem_PopTag();
    D_00332374++;
    return pEdict->pvPrivateData;
}

void *PvEntPrivateData(edict_t *pEdict)
{
    if (pEdict)
        return pEdict->pvPrivateData;
    return NULL;
}

void FreeEntPrivateData(edict_t *pEdict)
{
    if (pEdict->pvPrivateData)
    {
        Mem_Free(pEdict->pvPrivateData);
        D_00332378++;
    }
    pEdict->pvPrivateData = NULL;
}

#ifdef NON_MATCHING
void ED_ReserveLeafs(edict_t *pEdict, int count)
{
    if (!pEdict->leafnums)
    {
        pEdict->leafnums = Small_Calloc(1, count * 2);
        if (pEdict->leafnums)
        {
            D_0033237C++;
            pEdict->max_leafs = count;
            pEdict->num_leafs = 0;
            pEdict->unk_14 = 0;
        }
        else
        {
            ED_FreeLeafs(pEdict);
        }
    }
    else if (pEdict->max_leafs < count)
    {
        short *p = Small_Calloc(1, count * 2);
        if (p)
        {
            D_0033237C++;
            D_00332384++;
            memcpy(p, pEdict->leafnums, pEdict->num_leafs * 2);
            pEdict->max_leafs = count;
            Small_Free(pEdict->leafnums);
            D_00332380++;
            pEdict->leafnums = p;
        }
        else
        {
            ED_FreeLeafs(pEdict);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/pr_edict", ED_ReserveLeafs);
#endif

void ED_FreeLeafs(edict_t *pEdict)
{
    if (pEdict->leafnums)
    {
        Small_Free(pEdict->leafnums);
        D_00332380++;
    }
    pEdict->leafnums = NULL;
    pEdict->num_leafs = 0;
    pEdict->max_leafs = 0;
    pEdict->unk_14 = 0;
}

edict_t *PEntityOfEntOffset(int iEntOffset)
{
    return (edict_t *)((char *)sv.edicts + iEntOffset);
}

int EntOffsetOfPEntity(const edict_t *pEdict)
{
    return (char *)pEdict - (char *)sv.edicts;
}

int IndexOfEdict(const edict_t *pEdict)
{
    int index;

    if (!pEdict)
        return 0;

    index = pEdict - sv.edicts;
    if (index < 0 || index > sv.max_edicts)
        Sys_Error("Bad entity in IndexOfEdict()");
    return index;
}

#ifdef NON_MATCHING
edict_t *PEntityOfEntIndex(int iEntIndex)
{
    edict_t *pEdict;

    if (iEntIndex < 0 || iEntIndex >= sv.max_edicts)
        return NULL;

    pEdict = EDICT_NUM(iEntIndex);

    if (pEdict->free || !pEdict->pvPrivateData)
    {
        if (iEntIndex >= svs.maxclients || pEdict->free)
            return NULL;
    }
    return pEdict;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/pr_edict", PEntityOfEntIndex);
#endif

const char *SzFromIndex(int iString)
{
    return pr_strings + iString;
}

entvars_t *GetVarsOfEnt(edict_t *pEdict)
{
    return &pEdict->v;
}

edict_t *FindEntityByVars(entvars_t *pvars)
{
    int i;
    edict_t *pEdict;

    for (i = 0; i < sv.num_edicts; i++)
    {
        pEdict = &sv.edicts[i];
        if (&pEdict->v == pvars)
            return pEdict;
    }
    return NULL;
}

INCLUDE_ASM("asm/nonmatchings/engine/pr_edict", CVarGetFloat);

INCLUDE_ASM("asm/nonmatchings/engine/pr_edict", CVarGetString);

INCLUDE_ASM("asm/nonmatchings/engine/pr_edict", CVarGetPointer);

INCLUDE_ASM("asm/nonmatchings/engine/pr_edict", CVarSetFloat);

INCLUDE_ASM("asm/nonmatchings/engine/pr_edict", CVarSetString);

INCLUDE_ASM("asm/nonmatchings/engine/pr_edict", CVarRegister);

INCLUDE_ASM("asm/nonmatchings/engine/pr_edict", AllocEngineString);

INCLUDE_ASM("asm/nonmatchings/engine/pr_edict", SaveSpawnParms);

INCLUDE_ASM("asm/nonmatchings/engine/pr_edict", GetModelPtr);

