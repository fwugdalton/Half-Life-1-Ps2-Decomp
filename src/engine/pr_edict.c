#include "common.h"
#include "engine/mathlib.h"
#include "engine/model.h"
#include "engine/server.h"
#include "engine/cvar.h"
#include "engine/progdefs.h"

extern char *pr_strings;
extern cvar_t deathmatch;
extern int (*gEntityInterface_pfnSpawn)(edict_t *pent);
extern void (*gEntityInterface_pfnKeyValue)(edict_t *pentKeyvalue, KeyValueData *pkvd);

#define SF_NOTINDEATHMATCH 0x800
#define FL_KILLME 0x40000000

double atof(const char *s);
int sprintf(char *s, const char *fmt, ...);
char *strcpy(char *d, const char *s);
int strcmp(const char *a, const char *b);
void func_002817E0(void);
void SuckOutClassname(char *szInputStream, edict_t *pEdict);
int ed_privdata_allocs = 0;
int ed_privdata_frees = 0;
int ed_leaf_allocs = 0;
int ed_leaf_frees = 0;
int ed_leaf_reallocs = 0;

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
int strlen(const char *s);
void *Hunk_SubAllocName(int size, char *name);
void *Mod_Extradata(model_t *mod);

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

void ED_Count(void)
{
    int i;
    edict_t *ent;
    int active, models, solid, step;

    active = models = solid = step = 0;
    for (i = 0; i < sv.num_edicts; i++)
    {
        ent = &sv.edicts[i];
        if (ent->free)
            continue;
        active++;
        if (ent->v.solid)
            solid++;
        if (ent->v.model)
            models++;
        if (ent->v.movetype == 4)
            step++;
    }

    Con_Printf("num_edicts:%3i\n", sv.num_edicts);
    Con_Printf("active    :%3i\n", active);
    Con_Printf("view      :%3i\n", models);
    Con_Printf("touch     :%3i\n", solid);
    Con_Printf("step      :%3i\n", step);
}

char *ED_NewString(char *string)
{
    char *new_s, *new_p;
    int i, l;

    l = strlen(string) + 1;
    new_s = Hunk_SubAllocName(l, "ED_NewString");
    new_p = new_s;

    for (i = 0; i < l; i++)
    {
        if (string[i] == '\\' && i < l - 1)
        {
            i++;
            if (string[i] == 'n')
                *new_p++ = '\n';
            else
                *new_p++ = '\\';
        }
        else
            *new_p++ = string[i];
    }

    return new_s;
}

char *ED_ParseEdict(char *data, edict_t *ent)
{
    qboolean init;
    char keyname[256];
    int n;
    ENTITYINIT pEntityInit;
    KeyValueData kvd;
    float v;

    if (ent != sv.edicts)
        memset(&ent->v, 0, sizeof(ent->v));

    InitEntityDLLFields(ent);
    SuckOutClassname(data, ent);

    init = false;
    pEntityInit = GetEntityInit(pr_strings + ent->v.classname);
    if (pEntityInit)
    {
        pEntityInit(&ent->v);
        init = true;
    }
    else
    {
        pEntityInit = GetEntityInit("custom");
        if (pEntityInit)
        {
            pEntityInit(&ent->v);
            init = true;
            kvd.szClassName = "custom";
            kvd.szKeyName = "customclass";
            kvd.szValue = pr_strings + ent->v.classname;
            kvd.fHandled = false;
            gEntityInterface_pfnKeyValue(ent, &kvd);
        }
        else
        {
            Con_DPrintf("Can't init %s\n", pr_strings + ent->v.classname);
        }
    }

    while (1)
    {
        data = COM_Parse(data);
        if (com_token[0] == '}')
            break;
        if (!data)
            Sys_Error("ED_ParseEntity: EOF without closing brace");

        strcpy(keyname, com_token);

        n = strlen(keyname);
        while (n && keyname[n - 1] == ' ')
        {
            keyname[n - 1] = 0;
            n--;
        }

        data = COM_Parse(data);
        if (!data)
            Sys_Error("ED_ParseEntity: EOF without closing brace");

        if (com_token[0] == '}')
            Sys_Error("ED_ParseEntity: closing brace without data");

        if (pr_strings + ent->v.classname && !strcmp(pr_strings + ent->v.classname, com_token))
            continue;

        if (!strcmp(keyname, "angle"))
        {
            v = atof(com_token);
            if (v >= 0)
                sprintf(com_token, "%f %f %f", ent->v.angles[0], v, ent->v.angles[2]);
            else if ((int)v == -1)
                sprintf(com_token, "-90 0 0");
            else
                sprintf(com_token, "90 0 0");
            strcpy(keyname, "angles");
        }

        kvd.szClassName = pr_strings + ent->v.classname;
        kvd.szKeyName = keyname;
        kvd.szValue = com_token;
        kvd.fHandled = false;
        gEntityInterface_pfnKeyValue(ent, &kvd);
    }

    if (!init)
    {
        ent->free = true;
        ent->serialnumber++;
    }
    return data;
}

void ED_LoadFromFile(char *data)
{
    edict_t *ent;
    int inhibit;

    gGlobalVariables.time = sv.time;

    ent = NULL;
    inhibit = 0;
    while (1)
    {
        data = COM_Parse(data);
        if (!data)
            break;
        if (com_token[0] != '{')
            Sys_Error("ED_LoadFromFile: found %s when expecting {", com_token);

        if (!ent)
        {
            ent = sv.edicts;
            ReleaseEntityDLLFields(ent);
            InitEntityDLLFields(ent);
        }
        else
        {
            ent = ED_Alloc();
        }

        data = ED_ParseEdict(data, ent);
        if (ent->free)
            continue;

        if (deathmatch.value && (ent->v.spawnflags & SF_NOTINDEATHMATCH))
        {
            ED_Free(ent);
            inhibit++;
            continue;
        }

        if (!ent->v.classname)
        {
            Con_Printf("No classname for:\n");
            ED_Free(ent);
            continue;
        }

        if (gEntityInterface_pfnSpawn(ent) < 0 || (ent->v.flags & FL_KILLME))
            ED_Free(ent);
        func_002817E0();
    }

    Con_DPrintf("%i entities inhibited\n", inhibit);
}

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

void SuckOutClassname(char *szInputStream, edict_t *pEdict)
{
    char szKeyName[256];
    KeyValueData kvd;

    while (1)
    {
        szInputStream = COM_Parse(szInputStream);
        if (com_token[0] == '}')
            return;

        strcpy(szKeyName, com_token);
        szInputStream = COM_Parse(szInputStream);

        if (!strcmp(szKeyName, "classname"))
            break;
    }

    kvd.szClassName = NULL;
    kvd.szKeyName = szKeyName;
    kvd.szValue = com_token;
    kvd.fHandled = false;
    gEntityInterface_pfnKeyValue(pEdict, &kvd);
    if (!kvd.fHandled)
        Host_Error("SuckOutClassname: parse error");
}

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
    ed_privdata_allocs++;
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
        ed_privdata_frees++;
    }
    pEdict->pvPrivateData = NULL;
}

void ED_ReserveLeafs(edict_t *pEdict, int count)
{
    if (!pEdict->leafnums)
    {
        pEdict->leafnums = Small_Calloc(1, count * 2);
        if (pEdict->leafnums)
        {
            ed_leaf_allocs++;
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
            ed_leaf_allocs++;
            ed_leaf_reallocs++;
            pEdict->max_leafs = count;
            memcpy(p, pEdict->leafnums, pEdict->num_leafs * 2);
            Small_Free(pEdict->leafnums);
            ed_leaf_frees++;
            pEdict->leafnums = p;
        }
        else
        {
            ED_FreeLeafs(pEdict);
        }
    }
}

void ED_FreeLeafs(edict_t *pEdict)
{
    if (pEdict->leafnums)
    {
        Small_Free(pEdict->leafnums);
        ed_leaf_frees++;
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

edict_t *PEntityOfEntIndex(int iEntIndex)
{
    edict_t *pEdict;

    if (iEntIndex < 0 || iEntIndex >= sv.max_edicts)
        return NULL;

    pEdict = EDICT_NUM(iEntIndex);

    if (!pEdict->free && pEdict->pvPrivateData)
        return pEdict;
    if (iEntIndex < svs.maxclients && !pEdict->free)
        return pEdict;
    return NULL;
}

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

float CVarGetFloat(char *szVarName)
{
    return Cvar_VariableValue(szVarName);
}

char *CVarGetString(char *szVarName)
{
    return Cvar_VariableString(szVarName);
}

cvar_t *CVarGetPointer(char *szVarName)
{
    return Cvar_FindVar(szVarName);
}

void CVarSetFloat(char *szVarName, float flValue)
{
    Cvar_SetValue(szVarName, flValue);
}

void CVarSetString(char *szVarName, char *szValue)
{
    Cvar_Set(szVarName, szValue);
}

void CVarRegister(cvar_t *pCvar)
{
    if (pCvar)
    {
        pCvar->flags |= 8;
        Cvar_RegisterVariable(pCvar);
    }
}

int AllocEngineString(char *szValue)
{
    return ED_NewString(szValue) - pr_strings;
}

void SaveSpawnParms(edict_t *pEdict)
{
    int eoffset = NUM_FOR_EDICT(pEdict);

    if (eoffset < 1 || eoffset > svs.maxclients)
        Host_Error("Entity is not a client");
}

void *GetModelPtr(edict_t *pEdict)
{
    return Mod_Extradata(sv.models[pEdict->v.modelindex]);
}

