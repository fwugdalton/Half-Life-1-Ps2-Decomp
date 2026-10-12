#include "common.h"
#include "engine/cvar.h"
#include "engine/cmd.h"
#include "engine/client.h"
#include "engine/sound.h"
#include "engine/cl_entity.h"

char *strcpy(char *, const char *);
char *func_002E61B8(char *dst, const char *src, int n); /* strncat */
int Draw_GetFontInfo(short *widths);
typedef int (*pfnUserMsgHook)(const char *pszName, int iSize, void *pbuf);
pfnUserMsgHook HookServerMsg(const char *pszName, pfnUserMsgHook pfn, int clientId);
void Con_Print(char *txt);
sfx_t *S_PrecacheSound(char *name);
void S_StartDynamicSound(int entnum, int entchannel, sfx_t *sfx, vec3_t origin, float fvol, float attenuation, int flags, int pitch, int clientId);
void SCR_CenterPrint(char *str, int clientId);
int Draw_StringLen(char *str);

typedef struct
{
    byte pad0[0x1C];
    int width;                  /* 0x1C */
    int height;                 /* 0x20 */
} cdll_viddef_t;

extern cdll_viddef_t r_viddef;

typedef struct
{
    byte pad0[0xC];
    int height;                 /* 0x0C */
} cdll_font_t;

extern cdll_font_t *font;
extern vec3_t r_origin;
extern char D_00389078[];       /* "invalid sound %s\n" */
/* client_data_t (0x40) */
typedef struct
{
    vec3_t origin;              /* 0x00 */
    float viewheight;           /* 0x0C */
    float maxspeed;             /* 0x10 */
    vec3_t viewangles;          /* 0x14 */
    vec3_t punchangle;          /* 0x20 */
    int iKeyBits;               /* 0x2C */
    int iWeaponBits;            /* 0x30 */
    float fov;                  /* 0x34 */
    float idlescale;            /* 0x38 */
    float mouse_sensitivity;    /* 0x3C */
} client_data_t;

/* cldll_func_t (0x18) */
typedef struct
{
    void (*pInitFunc)(void *pEnginefuncs, int iVersion); /* 0x00 */
    int (*pHudInitFunc)(int clientId); /* 0x04 */
    int (*pHudVidInitFunc)(int clientId); /* 0x08 */
    int (*pHudRedrawFunc)(float flTime, int intermission, int clientId, int unk); /* 0x0C */
    int (*pHudUpdateClientDataFunc)(client_data_t *pcldata, float flTime, int clientId); /* 0x10 */
    int (*pHudResetFunc)(int clientId); /* 0x14 */
} cldll_func_t;

extern cldll_func_t D_0046E4F8; /* cl_funcs */
extern byte D_003315B8[];       /* cl_enginefuncs */
void func_00115D18(void *pEnginefuncs, int iVersion); /* Initialize (DLL do cliente) */
int func_00115E90(int clientId);
int func_00115EC8(int clientId);
int func_00115F00(float flTime, int intermission, int clientId, int unk);
int func_00115F40(client_data_t *pcldata, float flTime, int clientId);
int func_00115F78(int clientId);
int V_ScreenShake(int clientId, const char *pszName, int iSize, void *pbuf);
int V_ScreenFade(int clientId, const char *pszName, int iSize, void *pbuf);
int func_0021C240(int clientId, const char *pszName, int iSize, void *pbuf); /* SetActuator */
void func_00247C00(void);       /* SPR_Shutdown */
void func_00247D18(int clientId); /* SPR_Init */
void Cvar_RemoveHudCvars(void);
void Cmd_RemoveMallocedCmds(void);
void CL_ResetUserMsgs(int clientId);
void *memset(void *, int, unsigned int);
extern cl_entity_t *cl_entities[2];
extern float D_003332E0[2];     /* fov por cliente */
extern cvar_t D_0056A1F8[2];
extern float D_00331844;        /* sensitivity.value */
extern float v_idlescale;
int func_0024E2E0(int bResetState, int clientId); /* CL_ButtonBits */
void func_0024E670(int bits, int clientId); /* CL_ResetButtonBits */
typedef struct
{
    int left, right, top, bottom;
} wrect_t;

/* client_sprite_t (0x98) */
typedef struct
{
    char szName[64];            /* 0x00 */
    char szSprite[64];          /* 0x40 */
    int hspr;                   /* 0x80 */
    int iRes;                   /* 0x84 */
    wrect_t rc;                 /* 0x88 */
} client_sprite_t;

int atoi(const char *);
void *Mem_DbgCalloc(int num, int size, const char *file, int line);
void *COM_LoadFile(char *path, int usehunk, int *pLength);
int func_00235910(char *name, char **pfile, int *plen);
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

typedef struct
{
    int iSize;                  /* 0x00 */
    int iWidth;                 /* 0x04 */
    int iHeight;                /* 0x08 */
    int iFlags;                 /* 0x0C */
    int iCharHeight;            /* 0x10 */
    short charWidths[256];      /* 0x14 */
} SCREENINFO; /* 0x214 */

typedef struct
{
    char *name;                 /* 0x00 */
    short ping;                 /* 0x04 */
    byte thisplayer;            /* 0x06 */
    byte pad7;
    int clientId;               /* 0x08 */
    byte spectator;             /* 0x0C */
    byte packetloss;            /* 0x0D */
    byte padE[2];
    char *model;                /* 0x10 */
    short topcolor;             /* 0x14 */
    short bottomcolor;          /* 0x16 */
} hud_player_info_t;

/* hudGetScreenInfo */
int func_002468C0(SCREENINFO *pscrinfo)
{
    if (!pscrinfo)
        return 0;
    if (pscrinfo->iSize != sizeof(SCREENINFO))
        return 0;
    pscrinfo->iWidth = r_viddef.width;
    pscrinfo->iHeight = r_viddef.height;
    pscrinfo->iFlags = 1;
    pscrinfo->iCharHeight = Draw_GetFontInfo(pscrinfo->charWidths);
    return sizeof(SCREENINFO);
}

/* hudRegisterVariable */
int func_00246930(char *szName, char *szValue, int flags)
{
    cvar_t *cv = Z_Malloc(sizeof(cvar_t));
    cv->name = szName;
    cv->string = szValue;
    cv->flags = flags | 0x10;
    Cvar_RegisterVariable(cv);
    return 1;
}

/* hudGetCvarFloat */
float func_00246990(char *szName)
{
    cvar_t *cv = Cvar_FindVar(szName);
    if (cv)
        return cv->value;
    return 0.0f;
}

/* hudGetCvarString */
char *func_002469B8(char *szName)
{
    cvar_t *cv = Cvar_FindVar(szName);
    if (cv)
        return cv->string;
    return NULL;
}

/* hudAddCommand */
int func_002469E0(char *cmd_name, xcommand_t function)
{
    Cmd_AddMallocCommand(cmd_name, function);
    return 1;
}

/* hudServerCmd */
/* hudServerCmd */
int func_00246A00(char *pszCmdString, int reliable)
{
    char buf[256];

    switch (reliable)
    {
    case 0:
        strcpy(buf, "cmd ");
        break;
    case 1:
        strcpy(buf, "cmd2 ");
        break;
    default:
        strcpy(buf, "cmd ");
        break;
    }
    func_002E61B8(buf, pszCmdString, 250);
    Cmd_TokenizeString(buf);
    switch (reliable)
    {
    case 0:
        Cmd_ForwardToServer();
        break;
    case 1:
        Cmd_ForwardToServerUnreliable();
        break;
    }
    return 1;
}

/* hudClientCmd */
int func_00246AB8(char *pszCmdString)
{
    if (!pszCmdString)
        return 0;
    Cbuf_AddText(pszCmdString);
    Cbuf_AddText("\n");
    return 1;
}

/* hudHookUserMsg */
int func_00246AF0(char *szMsgName, pfnUserMsgHook pfn, int clientId)
{
    return HookServerMsg(szMsgName, pfn, clientId) != NULL;
}

/* hudGetPlayerInfo */
/* hudGetPlayerInfo */
/* hudGetPlayerInfo */
#ifdef NON_MATCHING
void func_00246B10(int ent_num, hud_player_info_t *pinfo, int clientId)
{
    if (ent_num > cl[clientId].maxclients || ent_num <= 0
        || !cl[clientId].players[ent_num - 1].name || !cl[clientId].players[ent_num - 1].name[0])
    {
        pinfo->name = NULL;
        return;
    }
    ent_num--;
    pinfo->name = cl[clientId].players[ent_num].name;
    pinfo->ping = cl[clientId].players[ent_num].unk_E8;
    pinfo->spectator = cl[clientId].players[ent_num].spectator;
    pinfo->packetloss = cl[clientId].players[ent_num].unk_EA;
    pinfo->thisplayer = (ent_num == cl[clientId].playernum);
    if (pinfo->thisplayer)
        pinfo->clientId = clientId;
    pinfo->model = cl[clientId].players[ent_num].model;
    pinfo->topcolor = cl[clientId].players[ent_num].topcolor;
    pinfo->bottomcolor = cl[clientId].players[ent_num].bottomcolor;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/cdll_int", func_00246B10);
#endif

/* hudPlaySoundByName */
void func_00246BF0(char *szSound, float volume, int clientId)
{
    sfx_t *sfx;

    volume = max(volume, 0.0f);
    volume = min(volume, 1.0f);
    sfx = S_PrecacheSound(szSound);
    if (!sfx)
    {
        Con_DPrintf(D_00389078, szSound);
        return;
    }
    S_StartDynamicSound(cl[clientId].viewentity, 3, sfx, r_origin, volume, 1.0f, 0, 100, clientId);
}

/* hudPlaySoundVoiceByName */
void func_00246CA8(char *szSound, float volume, int clientId)
{
    sfx_t *sfx;

    volume = max(volume, 0.0f);
    volume = min(volume, 1.0f);
    sfx = S_PrecacheSound(szSound);
    if (!sfx)
    {
        Con_DPrintf(D_00389078, szSound);
        return;
    }
    S_StartDynamicSound(cl[clientId].viewentity, 7, sfx, r_origin, volume, 1.0f, 0, 100, clientId);
}

INCLUDE_RODATA("asm/nonmatchings/engine/cdll_int", D_00389078);

/* hudPlaySoundByIndex */
void func_00246D60(unsigned int iSound, float volume, int clientId)
{
    volume = max(volume, 0.0f);
    volume = min(volume, 1.0f);
    if (iSound > 512)
    {
        Con_DPrintf("invalid sound %i\n", iSound);
        return;
    }
    S_StartDynamicSound(cl[clientId].viewentity, 3, cl[clientId].sound_precache[iSound], r_origin, volume, 1.0f, 0, 100, clientId);
}

/* hudDrawConsoleStringLen */
void func_00246DF0(char *string, int *width, int *height)
{
    *width = Draw_StringLen(string);
    *height = font->height;
}

/* hudConsolePrint */
void func_00246E38(char *string)
{
    Con_Print(string);
}

/* hudCenterPrint */
void func_00246E58(char *string, int clientId)
{
    SCR_CenterPrint(string, clientId);
}

/* ClientDLL_Init */
void func_00246E78(void)
{
    int i;

    D_0046E4F8.pInitFunc = func_00115D18;
    D_0046E4F8.pHudVidInitFunc = func_00115E90;
    D_0046E4F8.pHudInitFunc = func_00115EC8;
    D_0046E4F8.pHudRedrawFunc = func_00115F00;
    D_0046E4F8.pHudUpdateClientDataFunc = func_00115F40;
    D_0046E4F8.pHudResetFunc = func_00115F78;
    for (i = 0; i < 2; i++)
        HookServerMsg("ScreenShake", (pfnUserMsgHook)V_ScreenShake, i);
    for (i = 0; i < 2; i++)
        HookServerMsg("ScreenFade", (pfnUserMsgHook)V_ScreenFade, i);
    for (i = 0; i < 2; i++)
        HookServerMsg("SetActuator", (pfnUserMsgHook)func_0021C240, i);
    D_0046E4F8.pInitFunc(D_003315B8, 6);
}

/* ClientDLL_Shutdown */
void func_00246FC8(void)
{
    int i;

    func_00247C00();
    memset(&D_0046E4F8, 0, sizeof(D_0046E4F8));
    Cvar_RemoveHudCvars();
    Cmd_RemoveMallocedCmds();
    for (i = 0; i < 2; i++)
        CL_ResetUserMsgs(i);
}

/* ClientDLL_HudVidInit */
void func_00247028(int clientId)
{
    if (!D_0046E4F8.pHudVidInitFunc)
        Sys_Error("engine/cdll_int.c, line %d: could not link client DLL for HUD Vid initialization", 262);
    func_00247D18(clientId);
    D_0046E4F8.pHudVidInitFunc(clientId);
}

/* ClientDLL_HudInit */
void func_00247088(int clientId)
{
    if (!D_0046E4F8.pHudInitFunc)
        Sys_Error("engine/cdll_int.c, line %d: could not link client DLL for HUD initialization", 279);
    D_0046E4F8.pHudInitFunc(clientId);
}

/* ClientDLL_HudRedraw */
void func_002470E0(int intermission, int clientId, int unk)
{
    D_0046E4F8.pHudRedrawFunc(cl[clientId].time, intermission, clientId, unk);
}

/* ClientDLL_UpdateClientData */
void func_00247118(int clientId)
{
    client_data_t cdata;
    int iKeyBits;

    if (cl[clientId].spectator)
        return;
    memset(&cdata, 0, sizeof(cdata));
    cdata.viewheight = cl[clientId].viewheight;
    cdata.maxspeed = cl[clientId].maxspeed;
    VectorCopy(cl[clientId].viewangles, cdata.viewangles);
    VectorCopy(cl[clientId].punchangle, cdata.punchangle);
    VectorCopy(cl_entities[clientId][cl[clientId].viewentity].origin, cdata.origin);
    iKeyBits = func_0024E2E0(0, clientId);
    cdata.iKeyBits = iKeyBits;
    cdata.fov = D_003332E0[clientId];
    cdata.idlescale = v_idlescale;
    cdata.mouse_sensitivity = D_0056A1F8[clientId].value;
    cdata.iWeaponBits = cl[clientId].items;
    if (D_0046E4F8.pHudUpdateClientDataFunc(&cdata, cl[clientId].time, clientId))
    {
        cl[clientId].viewheight = cdata.viewheight;
        cl[clientId].maxspeed = cdata.maxspeed;
        VectorCopy(cdata.viewangles, cl[clientId].viewangles);
        VectorCopy(cdata.punchangle, cl[clientId].punchangle);
        D_003332E0[clientId] = cdata.fov;
        D_00331844 = cdata.mouse_sensitivity;
        v_idlescale = cdata.idlescale;
        if (cdata.iKeyBits != iKeyBits && cdata.iKeyBits - iKeyBits != -1)
            Con_DPrintf("HUD changed ikeybits, xor %i diff %i\n", cdata.iKeyBits ^ iKeyBits, cdata.iKeyBits - iKeyBits);
        func_0024E670(cdata.iKeyBits, clientId);
    }
}

/* ClientDLL_UpdateClientData2 */
void func_002473A8(client_data_t *cdata, int clientId)
{
    if (D_0046E4F8.pHudUpdateClientDataFunc(cdata, cl[clientId].time, clientId))
    {
        cl[clientId].viewheight = cdata->viewheight;
        cl[clientId].maxspeed = cdata->maxspeed;
        VectorCopy(cdata->viewangles, cl[clientId].viewangles);
        VectorCopy(cdata->punchangle, cl[clientId].punchangle);
        func_0024E670(cdata->iKeyBits, clientId);
        D_003332E0[clientId] = cdata->fov;
    }
}

/* ClientDLL_Reset */
/* ClientDLL_Reset */
void func_00247490(int clientId)
{
    D_0046E4F8.pHudResetFunc(clientId);
}

/* SPR_GetList */
client_sprite_t *func_002474B8(char *psz, int *piCount)
{
    char *pfile;
    int len;
    int iNumSprites;
    int i;
    client_sprite_t *p;
    client_sprite_t *plist;

    if (!func_00235910(psz, &pfile, &len))
        pfile = COM_LoadFile(psz, 2, NULL);
    pfile = COM_Parse(pfile);
    iNumSprites = atoi(com_token);
    if (!iNumSprites || !(p = Mem_DbgCalloc(iNumSprites * sizeof(client_sprite_t), 1, "engine/cdll_int.c", 446)))
        return NULL;
    plist = p;
    for (i = 0; i < iNumSprites; i++)
    {
        pfile = COM_Parse(pfile);
        strcpy(p->szName, com_token);
        pfile = COM_Parse(pfile);
        p->iRes = atoi(com_token);
        pfile = COM_Parse(pfile);
        strcpy(p->szSprite, com_token);
        pfile = COM_Parse(pfile);
        p->rc.left = atoi(com_token);
        pfile = COM_Parse(pfile);
        p->rc.top = atoi(com_token);
        pfile = COM_Parse(pfile);
        p->rc.right = atoi(com_token) + p->rc.left;
        pfile = COM_Parse(pfile);
        p->rc.bottom = atoi(com_token) + p->rc.top;
        p++;
    }
    if (piCount)
        *piCount = iNumSprites;
    return plist;
}
