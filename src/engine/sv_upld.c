#include "common.h"
#include "engine/server.h"
#include "engine/cvar.h"
#include "engine/cmd.h"

extern cvar_t sv_uploadinterval;
extern float realtime;
extern char sv_upld_flagstr[8];
extern char sv_upld_typestr[32];
int sprintf(char *, const char *, ...);

enum { t_sound = 0, t_skin, t_model, t_decal, t_generic };

void SV_ClearResourceList(resource_t *pList);
extern netadr_t net_from;
qboolean NET_IsLocalAddress(netadr_t adr);
void SV_BeginRedirect(int rd, netadr_t *addr);
void SV_EndRedirect(void);
#define RD_PACKET 1
void func_00288780(client_t *cl);
void func_00281D88(client_t *cl, resource_t *pResource, qboolean bCheck);
void func_00288960(resource_t *pResource, resource_t *pList);
void func_00288A28(resource_t *pResource);
void Mem_Free(void *p);
void func_00281D90(void);
qboolean func_00288B18(void);
int atoi(const char *);
void MSG_WriteByte(sizebuf_t *sb, int c);
void MSG_WriteLong(sizebuf_t *sb, int c);
int func_00243400(void);
int MSG_ReadByte(void);
char *MSG_ReadString(void);
void *memset(void *, int, unsigned int);
char *strcpy(char *, const char *);
extern int cmd_source;

INCLUDE_RODATA("asm/nonmatchings/engine/sv_upld", D_0038FAD0);

void func_002882A8(void)
{
    client_t *cl;
    upload_entry_t *e;

    if (sv_uploadinterval.value == 0)
        return;

    if (sv_uploadinterval.value < 0)
        Cvar_SetValue("sv_uploadinterval", 1);

    cl = host_client;
    if (realtime - cl->unk_869C < sv_uploadinterval.value)
        return;

    cl->unk_869C = realtime;
    e = &cl->unk_86A0[cl->unk_8700 & 7];
    e->unk_0 = 1;
    e->time = realtime;
    e->unk_8 = cl->unk_8694;
    cl->unk_8700++;
}

void func_00288378(void) {
}

void SV_PrintResource(int index, resource_t *pResource)
{
    sprintf(sv_upld_flagstr, "N");

    switch (pResource->type)
    {
    default:
        sprintf(sv_upld_typestr, "unknown");
        break;
    case t_sound:
        sprintf(sv_upld_typestr, "sound");
        break;
    case t_decal:
        sprintf(sv_upld_typestr, "decal");
        break;
    case t_model:
        sprintf(sv_upld_typestr, "model");
        break;
    case t_skin:
        sprintf(sv_upld_typestr, "skin");
        break;
    case t_generic:
        sprintf(sv_upld_typestr, "generic");
        break;
    }

    Con_Printf("%3i %s:%15s %i %s\n", index, sv_upld_typestr, pResource->szFileName, pResource->nIndex, sv_upld_flagstr);
}

void SV_PrintResourceLists(void)
{
    int i;
    resource_t *p;

    i = 1;

    Con_Printf("- Needed -------------------------------------------\n");
    Con_Printf("#   Name                  Size Type Index Fatal\n");
    for (p = host_client->resourcesneeded.pNext; p != &host_client->resourcesneeded; p = p->pNext)
        SV_PrintResource(i++, p);

    Con_Printf("- On hand ------------------------------------------\n");
    Con_Printf("#   Name                  Size Type Index Fatal\n");
    for (p = host_client->resourcesonhand.pNext; p != &host_client->resourcesonhand; p = p->pNext)
        SV_PrintResource(i++, p);

    Con_Printf("--------------------------------------------\n\n");
}

void SV_ClearResourceLists(client_t *cl)
{
    if (!cl)
        Sys_Error("SV_ClearResourceLists with NULL client!");

    SV_ClearResourceList(&cl->resourcesneeded);
    SV_ClearResourceList(&cl->resourcesonhand);
}

void func_002885E8(void)
{
    int i, j;
    client_t *cl;
    customization_t *pCust;

    if (cmd_source == src_command)
    {
        if (!sv.active)
        {
            Cmd_ForwardToServer();
            return;
        }
    }

    if (!NET_IsLocalAddress(net_from))
        SV_BeginRedirect(RD_PACKET, &net_from);

    for (i = 0, cl = svs.clients; i < svs.maxclients; i++, cl++)
    {
        if (!cl->active && !cl->spawned)
            continue;
        if (!cl->customdata.pNext)
            continue;

        Con_DPrintf("SV Customizations:\nPlayer %i:%s\n", i + 1, cl->name);

        j = 1;
        for (pCust = cl->customdata.pNext; pCust; pCust = pCust->pNext)
        {
            if (pCust->bInUse)
                SV_PrintResource(j++, &pCust->resource);
        }

        Con_DPrintf("-----------------\n\n");
    }

    if (!NET_IsLocalAddress(net_from))
        SV_EndRedirect();
}

void func_00288780(client_t *cl) {
}

void func_00288788(void)
{
    client_t *pHost;
    resource_t *pResource;

    pHost = host_client;
    pHost->uploading = false;

    for (pResource = pHost->resourcesonhand.pNext; pResource != &pHost->resourcesonhand; pResource = pResource->pNext)
    {
        func_00288780(pHost);
        func_00281D88(pHost, pResource, true);
    }

    host_client = pHost;
}

void func_00288818(resource_t *pResource)
{
    if (!pResource)
    {
        Con_DPrintf("Null resource passed to SV_MoveToOnHandList\n");
        return;
    }

    switch (pResource->type)
    {
    case t_sound:
    case t_skin:
    case t_model:
    case t_decal:
    case t_generic:
        break;
    default:
        Con_DPrintf("Unknown resource type\n");
        break;
    }

    func_00288A28(pResource);
    func_00288960(pResource, &host_client->resourcesonhand);
}

int func_00288890(resource_t *pList, int *pWorld, int *pModels, int *pDecals, int *pSounds, int *pSkins, int *pGeneric)
{
    resource_t *p;
    int n;
    int one;

    *pModels = 0;
    *pWorld = 0;
    *pDecals = 0;
    *pSounds = 0;
    *pSkins = 0;
    *pGeneric = 0;

    n = 0;
    for (p = pList->pNext; p != pList; p = p->pNext)
    {
        one = 1;
        n++;
        switch (p->type)
        {
        case t_model:
            if (p->nIndex == 1)
                *pWorld = one;
            else
                (*pModels)++;
            break;
        case t_sound:
            (*pSounds)++;
            break;
        case t_decal:
            (*pDecals)++;
            break;
        case t_skin:
            (*pSkins)++;
            break;
        case t_generic:
            (*pGeneric)++;
            break;
        }
    }

    return n;
}

void func_00288960(resource_t *pResource, resource_t *pList)
{
    if (pResource->pPrev || pResource->pNext)
    {
        Con_Printf("Resource already linked\n");
        return;
    }

    pResource->pPrev = pList->pPrev;
    pList->pPrev->pNext = pResource;
    pList->pPrev = pResource;
    pResource->pNext = pList;
}

void SV_ClearResourceList(resource_t *pList)
{
    resource_t *p, *n;

    for (p = pList->pNext; p && p != pList; p = n)
    {
        n = p->pNext;
        func_00288A28(p);
        Mem_Free(p);
    }

    pList->pPrev = pList;
    pList->pNext = pList;
}

void func_00288A28(resource_t *pResource)
{
    pResource->pPrev->pNext = pResource->pNext;
    pResource->pNext->pPrev = pResource->pPrev;
    pResource->pPrev = NULL;
    pResource->pNext = NULL;
}

int func_00288A50(void)
{
    return 0;
}

void func_00288A58(void)
{
    int i;

    host_client = svs.clients;
    for (i = 0; i < svs.maxclients; i++, host_client++)
    {
        if (host_client->active || host_client->spawned)
        {
            while (func_00288B18())
                ;
        }
    }
}

qboolean func_00288B18(void)
{
    client_t *cl;
    resource_t *p;

    cl = host_client;

    if (cl->unk_85F8 || cl->unk_8688)
        return false;
    if (!cl->uploading)
        return false;
    if (cl->uploaddoneregistering)
        return false;

    p = cl->resourcesneeded.pNext;
    cl->unk_85FC = p;

    if (p == &cl->resourcesneeded)
    {
        func_00288788();
        func_00281D90();
        Con_DPrintf("Custom resource propagation complete.\n");
        host_client->uploaddoneregistering = true;
        return false;
    }

    if (!(p->ucFlags & 2))
    {
        func_00288818(p);
        return true;
    }

    if (p->type == t_decal)
        func_00288818(p);

    return true;
}

void func_00288C00(void)
{
    int index;

    if (Cmd_Argc() != 3)
    {
        Con_Printf("Invalid resource request\n");
        return;
    }

    if (atoi(Cmd_Argv(1)) != svs.spawncount)
    {
        Con_Printf("Resource request with mismatched servercount\n");
        return;
    }

    index = atoi(Cmd_Argv(2));
    if (index < 0)
    {
        Con_Printf("Resource request with bogus starting index %i\n", index);
        return;
    }

    MSG_WriteByte(&host_client->netchan.message, 0x30);
    MSG_WriteLong(&host_client->netchan.message, svs.spawncount);
    MSG_WriteLong(&host_client->netchan.message, index);
}

typedef struct
{
    int world;
    int models;
    int decals;
    int sounds;
    int skins;
    int generic;
} resourcecount_t;

void func_00288CE8(void)
{
    int i, total, start, end;
    resource_t *pResource;
    resourcecount_t rc;

    total = func_00243400();
    start = func_00243400();
    end = func_00243400();

    for (i = start; i < end; i++)
    {
        pResource = Mem_DbgMalloc(sizeof(resource_t), "engine/sv_upld.c", 900);
        memset(pResource, 0, sizeof(resource_t));
        pResource->type = MSG_ReadByte();
        strcpy(pResource->szFileName, MSG_ReadString());
        pResource->nIndex = func_00243400();
        pResource->ucFlags = (byte)MSG_ReadByte() & ~2;
        pResource->pNext = NULL;
        pResource->pPrev = NULL;

        if (i == 0)
        {
            SV_ClearResourceList(&host_client->resourcesneeded);
            SV_ClearResourceList(&host_client->resourcesonhand);
        }

        func_00288960(pResource, &host_client->resourcesneeded);
    }

    if (end >= total)
    {
        Con_DPrintf("Verifying and uploading resources...\n");

        host_client->unk_868C = func_00288890(&host_client->resourcesneeded, &rc.world, &rc.models, &rc.decals, &rc.sounds, &rc.skins, &rc.generic);
        host_client->unk_8690 = func_00288A50();

        if (host_client->unk_868C)
        {
            Con_DPrintf("Custom resources total %.2fK\n", host_client->unk_868C / 1024.0f);
            if (rc.models)
                Con_DPrintf("  Models:  %.2fK\n", rc.models / 1024.0f);
            if (rc.sounds)
                Con_DPrintf("  Sounds:  %.2fK\n", rc.sounds / 1024.0f);
            if (rc.decals)
                Con_DPrintf("  Decals:  %.2fK\n", rc.decals / 1024.0f);
            if (rc.skins)
                Con_DPrintf("  Skins :  %.2fK\n", rc.skins / 1024.0f);
            if (rc.generic)
                Con_DPrintf("  Generic :  %.2fK\n", rc.skins / 1024.0f);
            Con_DPrintf("----------------------\n");

            if (host_client->unk_8690 <= 1024)
                Con_DPrintf("Resources to request: %i bytes\n", host_client->unk_8690);
            else
                Con_DPrintf("Resources to request: %iK\n", host_client->unk_8690 / 1024);
        }

        host_client->uploading = true;
        host_client->unk_8694 = host_client->unk_8690;
        host_client->uploaddoneregistering = false;
        host_client->unk_8688 = 0;
        host_client->unk_8698 = realtime;
        host_client->unk_869C = realtime;
        memset(host_client->unk_86A0, 0, sizeof(host_client->unk_86A0));
        host_client->unk_8700 = 0;
    }
    else
    {
        host_client->uploading = false;
    }
}
