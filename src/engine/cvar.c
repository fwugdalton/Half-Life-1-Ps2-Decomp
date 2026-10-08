#include "common.h"
#include "engine/cmd.h"
#include "engine/cvar.h"

typedef void *FileHandle_t;

float Q_atof(char *str);
int Q_atoi(char *str);
int sprintf(char *, const char *, ...);
char *va(char *format, ...);
char *func_00241578(void);
void func_00245170(char *s, char *key, char *value, int maxsize);
void func_00282170(char *fmt, ...);
void Log_Printf(char *fmt, ...);
void SV_BroadcastPrintf(char *fmt, ...);
void FS_FPrintf(FileHandle_t f, char *fmt, ...);
void func_00222500(FileHandle_t f, char *fmt, ...);
FileHandle_t FS_Open(char *name, char *mode);
void FS_Close(FileHandle_t f);
qboolean Cmd_Exists(char *cmd_name);
void Cmd_CvarList_f(void);

extern int cl_maxclients;

char *cvar_null_string = "";

cvar_t *Cvar_FindVar(char *var_name)
{
    cvar_t *var;

    for (var = cvar_vars; var; var = var->next)
    {
        if (!Q_strcmp(var_name, var->name))
            return var;
    }

    return NULL;
}

float Cvar_VariableValue(char *var_name)
{
    cvar_t *var;

    var = Cvar_FindVar(var_name);
    if (!var)
        return 0;

    return Q_atof(var->string);
}

int Cvar_VariableInt(char *var_name)
{
    cvar_t *var;

    var = Cvar_FindVar(var_name);
    if (!var)
        return 0;

    return Q_atoi(var->string);
}

char *Cvar_VariableString(char *var_name)
{
    cvar_t *var;

    var = Cvar_FindVar(var_name);
    if (!var)
        return cvar_null_string;

    return var->string;
}

char *Cvar_CompleteVariable(char *partial)
{
    cvar_t *cvar;
    int len;
    char *last;

    last = NULL;
    len = Q_strlen(partial);

    if (!len)
        return NULL;

    for (cvar = cvar_vars; cvar; cvar = cvar->next)
    {
        if (!Q_strncmp(partial, cvar->name, len))
        {
            if (Q_strlen(cvar->name) == len)
                return cvar->name;
            last = cvar->name;
        }
    }

    return last;
}

void Cvar_Set(char *var_name, char *value)
{
    cvar_t *var;
    qboolean changed;
    int player;
    char szNew[1024];
    char szName[32];

    var = Cvar_FindVar(var_name);
    if (!var)
    {
        Con_DPrintf("Cvar_Set: variable %s not found\n", var_name);
        return;
    }

    if (var->flags & FCVAR_PRINTABLEONLY)
    {
        char *pS, *pD;

        szNew[0] = 0;
        pS = value;
        pD = szNew;

        while (*pS)
        {
            if (*pS < 32)
            {
                pS++;
                continue;
            }
            *pD++ = *pS++;
        }
        *pD = 0;

        if (!strlen(szNew))
            strcpy(szNew, "empty");

        value = szNew;
    }

    changed = Q_strcmp(var->string, value);

    if (var->flags & FCVAR_PLAYER)
        player = (var->flags >> 16) & 0xFF;
    else
        player = 0;

    if (var->flags & FCVAR_USERINFO)
    {
        if (cls[player].state == ca_dedicated)
        {
            func_00245170(func_00241578(), var_name, value, 512);
            func_00282170("fullserverinfo \"%s\"\n", func_00241578());
        }

        if (cls[player].state != ca_dedicated)
        {
            int len;

            len = strlen(var_name);
            strcpy(szName, var_name);
            if (szName[len - 2] == '_')
                szName[len - 2] = 0;

            func_00245170(cls[player].userinfo, szName, value, MAX_INFO_STRING);

            if (changed && cls[player].state >= ca_connected)
            {
                MSG_WriteByte(&cls[player].netchan.message, clc_stringcmd);
                SZ_Print(&cls[player].netchan.message, va("setinfo %d \"%s\" \"%s\"\n", player, var_name, value));
            }
        }
    }

    if ((var->flags & FCVAR_SERVER) && changed)
    {
        Log_Printf("\"%s\" = \"%s\"\n", var_name, value);
        SV_BroadcastPrintf("\"%s\" changed to \"%s\"\n", var_name, value);
    }

    Z_Free(var->string);
    var->string = Z_Malloc(Q_strlen(value) + 1);
    Q_strcpy(var->string, value);
    var->value = Q_atof(var->string);
}

void Cvar_SetValue(char *var_name, float value)
{
    char val[32];

    sprintf(val, "%f", value);
    Cvar_Set(var_name, val);
}

void Cvar_RegisterVariable(cvar_t *variable)
{
    char *oldstr;

    if (Cvar_FindVar(variable->name))
    {
        Con_Printf("Can't register variable %s, already defined\n", variable->name);
        return;
    }

    if (Cmd_Exists(variable->name))
    {
        Con_Printf("Cvar_RegisterVariable: %s is a command\n", variable->name);
        return;
    }

    oldstr = variable->string;
    variable->string = Z_Malloc(Q_strlen(variable->string) + 1);
    Q_strcpy(variable->string, oldstr);
    variable->value = Q_atof(variable->string);

    variable->next = cvar_vars;
    cvar_vars = variable;
}

void Cvar_RemoveHudCvars(void)
{
    cvar_t *pVar, *pNext, *pList;

    pList = NULL;
    for (pVar = cvar_vars; pVar; pVar = pNext)
    {
        pNext = pVar->next;
        if (pVar->flags & FCVAR_CLIENTDLL)
        {
            Z_Free(pVar->string);
            Z_Free(pVar);
        }
        else
        {
            pVar->next = pList;
            pList = pVar;
        }
    }

    cvar_vars = pList;
}

qboolean Cvar_Command(void)
{
    cvar_t *v;

    v = Cvar_FindVar(Cmd_Argv(0));
    if (!v)
        return false;

    if (Cmd_Argc() == 1)
    {
        Con_Printf("\"%s\" is \"%s\"\n", v->name, v->string);
        return true;
    }

    if ((v->flags & FCVAR_SPONLY) && cls[0].state >= ca_connecting && cl_maxclients > 1)
    {
        Con_Printf("Can't set %s in multiplayer\n", v->name);
        return true;
    }

    Cvar_Set(v->name, Cmd_Argv(1));
    return true;
}

void Cvar_WriteVariables(FileHandle_t f, int flags)
{
    cvar_t *var;

    for (var = cvar_vars; var; var = var->next)
    {
        if ((var->flags & flags) == flags)
            FS_FPrintf(f, "%s \"%s\"\r\n", var->name, var->string);
    }
}

void func_00261BD8(FileHandle_t f, int flags)
{
    cvar_t *var;

    for (var = cvar_vars; var; var = var->next)
    {
        if ((var->flags & flags) == flags)
            FS_FPrintf(f, "%s", var->string);
    }
}

void Cvar_PrintCvar(cvar_t *var, FileHandle_t f)
{
    char szOutstr[256];

    if (var->value == (float)(int)var->value)
        sprintf(szOutstr, "%-15s : %8i", var->name, (int)var->value);
    else
        sprintf(szOutstr, "%-15s : %8.3f", var->name, var->value);

    if (var->flags & FCVAR_ARCHIVE)
        strcat(szOutstr, ", a");
    if (var->flags & FCVAR_SERVER)
        strcat(szOutstr, ", sv");
    if (var->flags & FCVAR_USERINFO)
        strcat(szOutstr, ", i");

    strcat(szOutstr, "\n");

    Con_Printf("%s", szOutstr);

    if (f)
        func_00222500(f, "%s", szOutstr);
}

void Cmd_CvarList_f(void)
{
    cvar_t *var;
    int iCvars;
    int iArgs;
    char *partial;
    int ipLen;
    qboolean bAOnly;
    qboolean bSOnly;
    char szTemp[256];
    FileHandle_t f;
    qboolean bLogging;

    iCvars = 0;
    partial = NULL;
    bAOnly = false;
    bSOnly = false;
    f = NULL;
    bLogging = false;
    ipLen = 0;

    iArgs = Cmd_Argc();
    if (iArgs > 1)
    {
        if (!Q_strcasecmp(Cmd_Argv(1), "?"))
        {
            Con_Printf("CvarList           : List all cvars\nCvarList [Partial] : List cvars starting with 'Partial'\nCvarList log logfile [Partial] : Logs cvars to file c:\\logfile.\nNOTE:  No relative paths allowed!");
            return;
        }

        if (!Q_strcasecmp(Cmd_Argv(1), "log"))
        {
            sprintf(szTemp, "c:\\%s", Cmd_Argv(2));
            f = FS_Open(szTemp, "wt");
            if (f)
            {
                bLogging = true;
            }
            else
            {
                Con_Printf("Couldn't open [%s] for writing!\n", Cmd_Argv(2));
                FS_Close(f);
            }

            if (iArgs == 4)
            {
                partial = Cmd_Argv(3);
                ipLen = strlen(partial);
            }
        }
        else if (!strcasecmp(Cmd_Argv(1), "-a"))
        {
            bAOnly = true;
        }
        else if (!strcasecmp(Cmd_Argv(1), "-s"))
        {
            bSOnly = true;
        }
        else
        {
            partial = Cmd_Argv(1);
            ipLen = strlen(partial);
        }
    }

    Con_Printf("CVar List\n--------------\n");

    for (var = cvar_vars; var; var = var->next)
    {
        if (bAOnly && !(var->flags & FCVAR_ARCHIVE))
            continue;
        if (bSOnly && !(var->flags & FCVAR_SERVER))
            continue;
        if (partial)
        {
            if (!Q_strncasecmp(var->name, partial, ipLen))
            {
                Cvar_PrintCvar(var, f);
                iCvars++;
            }
        }
        else
        {
            Cvar_PrintCvar(var, f);
            iCvars++;
        }
    }

    if (iArgs == 2 && partial && *partial)
        Con_Printf("--------------\n%3i CVars for [%s]\nCvarList ? for syntax\n", iCvars, partial);
    else
        Con_Printf("--------------\n%3i Total CVars\nCvarList ? for syntax\n", iCvars);

    if (bLogging)
        FS_Close(f);
}

int Cvar_CountServerVariables(void)
{
    int i;
    cvar_t *var;

    for (i = 0, var = cvar_vars; var; var = var->next)
    {
        if (var->flags & FCVAR_SERVER)
            ++i;
    }

    return i;
}

void Cvar_UnlinkExternals(void)
{
    cvar_t *pVar;
    cvar_t **pList;

    pVar = cvar_vars;
    pList = &cvar_vars;

    while (*pList)
    {
        if (pVar->flags & FCVAR_EXTDLL)
            *pList = pVar->next;
        else
            pList = &pVar->next;

        pVar = pVar->next;
    }
}

void Cvar_CmdInit(void)
{
    Cmd_AddCommand("cvarlist", Cmd_CvarList_f);
}
