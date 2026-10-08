#include "common.h"
#include "engine/cmd.h"
#include "engine/cvar.h"


extern int cmd_wait;
extern sizebuf_t cmd_text;
extern cmdalias_t *cmd_alias;
extern cmd_source_t cmd_source;
extern int cmd_argc;
extern char *cmd_argv[MAX_ARGS];
extern char *cmd_null_string;
extern char *cmd_args;
extern cmd_function_t *cmd_functions;

void Cmd_Wait_f(void)
{
    cmd_wait = 1;
}

void Cbuf_Init(void)
{
    SZ_Alloc(&cmd_text, 0x3000);
}

void Cbuf_AddText(char *text)
{
    int l;

    l = Q_strlen(text);

    if (cmd_text.cursize + l >= cmd_text.maxsize)
    {
        Con_Printf("Cbuf_AddText: overflow\n");
        return;
    }

    SZ_Write(&cmd_text, text, Q_strlen(text));
}

void Cbuf_InsertText(char *text)
{
    char *temp;
    int templen;

    templen = cmd_text.cursize;
    if (templen)
    {
        temp = Z_Malloc(templen);
        memcpy(temp, cmd_text.data, templen);
        SZ_Clear(&cmd_text);
    }
    else
    {
        temp = NULL;
    }

    Cbuf_AddText(text);

    if (templen)
    {
        SZ_Write(&cmd_text, temp, templen);
        Z_Free(temp);
    }
}

void Cbuf_InsertTextLines(char *text)
{
    char *temp;
    int templen;

    templen = cmd_text.cursize;
    if (templen)
    {
        temp = Z_Malloc(templen);
        memcpy(temp, cmd_text.data, templen);
        SZ_Clear(&cmd_text);
    }
    else
    {
        temp = NULL;
    }

    Cbuf_AddText("\n");
    Cbuf_AddText(text);
    Cbuf_AddText("\n");

    if (templen)
    {
        SZ_Write(&cmd_text, temp, templen);
        Z_Free(temp);
    }
}

void Cbuf_Execute(void)
{
    int i;
    char *text;
    char line[1024];
    int quotes;

    while (cmd_text.cursize)
    {
        text = (char *)cmd_text.data;

        quotes = 0;
        for (i = 0; i < cmd_text.cursize; i++)
        {
            if (text[i] == '"')
                quotes++;
            if (!(quotes & 1) && text[i] == ';')
                break;
            if (text[i] == '\n')
                break;
        }

        memcpy(line, text, i);
        line[i] = 0;

        if (i == cmd_text.cursize)
        {
            cmd_text.cursize = 0;
        }
        else
        {
            i++;
            cmd_text.cursize -= i;
            memcpy(text, text + i, cmd_text.cursize);
        }

        Cmd_ExecuteString(line, src_command);

        if (cmd_wait)
        {
            cmd_wait = 0;
            break;
        }
    }
}

void Cmd_StuffCmds_f(void)
{
    int i, j;
    int s;
    char *text, *build, c;

    if (Cmd_Argc() != 1)
    {
        Con_Printf("stuffcmds : execute command line parameters\n");
        return;
    }

    s = 0;
    for (i = 1; i < com_argc; i++)
    {
        if (!com_argv[i])
            continue;
        s += Q_strlen(com_argv[i]) + 1;
    }
    if (!s)
        return;

    text = Z_Malloc(s + 1);
    text[0] = 0;
    for (i = 1; i < com_argc; i++)
    {
        if (!com_argv[i])
            continue;
        Q_strcat(text, com_argv[i]);
        if (i != com_argc - 1)
            Q_strcat(text, " ");
    }

    build = Z_Malloc(s + 1);
    build[0] = 0;

    for (i = 0; i < s - 1; i++)
    {
        if (text[i] == '+')
        {
            i++;

            for (j = i; text[j] != '+' && text[j] != '-' && text[j] != 0; j++)
                ;

            c = text[j];
            text[j] = 0;

            Q_strcat(build, text + i);
            Q_strcat(build, "\n");
            text[j] = c;
            i = j - 1;
        }
    }

    if (build[0])
        Cbuf_InsertText(build);

    Z_Free(text);
    Z_Free(build);
}

void Cmd_Exec_f(void)
{
    char *f;
    int mark;
    char *pszFileName;

    if (Cmd_Argc() != 2)
    {
        Con_Printf("exec <filename> : execute a script file\n");
        return;
    }

    pszFileName = Cmd_Argv(1);
    if (!pszFileName)
        return;

    mark = Hunk_LowMark();
    f = (char *)COM_LoadHunkFile(pszFileName);
    if (!f)
    {
        if (!strstr(pszFileName, "autoexec.cfg") && !strstr(pszFileName, "hw/opengl.cfg") &&
            !strstr(pszFileName, "joystick.cfg") && !strstr(pszFileName, "game.cfg"))
        {
            Con_Printf("couldn't exec %s\n", pszFileName);
        }
        return;
    }

    Con_DPrintf("execing %s\n", pszFileName);
    Cbuf_InsertTextLines(f);
    Hunk_FreeToLowMark(mark);
}

void Cmd_Echo_f(void)
{
    int i;

    for (i = 1; i < Cmd_Argc(); i++)
        Con_Printf("%s ", Cmd_Argv(i));
    Con_Printf("\n");
}

char *CopyString(char *in)
{
    char *out;

    out = Z_Malloc(strlen(in) + 1);
    strcpy(out, in);
    return out;
}

void Cmd_Alias_f(void)
{
    cmdalias_t *a;
    char cmd[1024];
    int i, c;
    char *s;

    if (Cmd_Argc() == 1)
    {
        Con_Printf("Current alias commands:\n");
        for (a = cmd_alias; a; a = a->next)
            Con_Printf("%s : %s\n", a->name, a->value);
        return;
    }

    s = Cmd_Argv(1);
    if (strlen(s) >= MAX_ALIAS_NAME)
    {
        Con_Printf("Alias name is too long\n");
        return;
    }

    cmd[0] = 0;
    c = Cmd_Argc();
    for (i = 2; i < c; i++)
    {
        strcat(cmd, Cmd_Argv(i));
        if (i != c)
            strcat(cmd, " ");
    }
    strcat(cmd, "\n");

    for (a = cmd_alias; a; a = a->next)
    {
        if (!strcmp(s, a->name))
        {
            if (!strcmp(a->value, cmd))
                return;
            Z_Free(a->value);
            break;
        }
    }

    if (!a)
    {
        a = Z_Malloc(sizeof(cmdalias_t));
        a->next = cmd_alias;
        cmd_alias = a;
    }
    strcpy(a->name, s);

    a->value = CopyString(cmd);
}

char *cmd_null_string = "";
char *cmd_args = NULL;
cmd_function_t *cmd_functions = NULL;

void Cmd_Init(void)
{
    Cmd_AddCommand("stuffcmds", Cmd_StuffCmds_f);
    Cmd_AddCommand("exec", Cmd_Exec_f);
    Cmd_AddCommand("echo", Cmd_Echo_f);
    Cmd_AddCommand("alias", Cmd_Alias_f);
    Cmd_AddCommand("cmd", Cmd_ForwardToServer);
    Cmd_AddCommand("cmd2", Cmd_ForwardToServerUnreliable);
    Cmd_AddCommand("wait", Cmd_Wait_f);
    Cmd_AddCommand("cmdlist", Cmd_CmdList_f);
}

int Cmd_Argc(void)
{
    return cmd_argc;
}

char *Cmd_Argv(int arg)
{
    if ((unsigned)arg >= cmd_argc)
        return cmd_null_string;
    return cmd_argv[arg];
}

char *Cmd_Args(void)
{
    return cmd_args;
}

void Cmd_TokenizeString(char *text)
{
    int i;

    for (i = 0; i < cmd_argc; i++)
        Z_Free(cmd_argv[i]);

    cmd_argc = 0;
    cmd_args = NULL;

    while (1)
    {
        while (*text && *text <= ' ' && *text != '\n')
            text++;

        if (*text == '\n')
            return;

        if (!*text)
            return;

        if (cmd_argc == 1)
            cmd_args = text;

        text = COM_Parse(text);
        if (!text)
            return;

        if (cmd_argc < MAX_ARGS)
        {
            cmd_argv[cmd_argc] = Z_Malloc(Q_strlen(com_token) + 1);
            Q_strcpy(cmd_argv[cmd_argc], com_token);
            cmd_argc++;
        }
    }
}

void Cmd_AddCommand(char *cmd_name, xcommand_t function)
{
    cmd_function_t *cmd;

    if (host_initialized)
        Sys_Error("Cmd_AddCommand after host_initialized [%s]", cmd_name);

    if (Cvar_VariableString(cmd_name)[0])
    {
        Con_Printf("Cmd_AddCommand: %s already defined as a var\n", cmd_name);
        return;
    }

    for (cmd = cmd_functions; cmd; cmd = cmd->next)
    {
        if (!Q_strcmp(cmd_name, cmd->name))
        {
            Con_Printf("Cmd_AddCommand: %s already defined\n", cmd_name);
            return;
        }
    }

    cmd = Z_Malloc(sizeof(cmd_function_t));
    cmd->name = cmd_name;
    cmd->function = function;
    cmd->next = cmd_functions;
    cmd->flags = 0;
    cmd_functions = cmd;
}

void Cmd_AddMallocCommand(char *cmd_name, xcommand_t function)
{
    cmd_function_t *cmd;

    if (Cvar_VariableString(cmd_name)[0])
    {
        Con_Printf("Cmd_AddCommand: %s already defined as a var\n", cmd_name);
        return;
    }

    for (cmd = cmd_functions; cmd; cmd = cmd->next)
    {
        if (!Q_strcmp(cmd_name, cmd->name))
        {
            Con_Printf("Cmd_AddCommand: %s already defined\n", cmd_name);
            return;
        }
    }

    cmd = Mem_DbgMalloc(sizeof(cmd_function_t), "engine/Cmd.c", 692);
    cmd->name = cmd_name;
    cmd->function = function;
    cmd->flags = 1;
    cmd->next = cmd_functions;
    cmd_functions = cmd;
}

void Cmd_RemoveMallocedCmds(void)
{
    cmd_function_t *c, *next, *list;

    list = NULL;
    for (c = cmd_functions; c; c = next)
    {
        next = c->next;
        if (c->flags)
        {
            Mem_Free(c);
        }
        else
        {
            c->next = list;
            list = c;
        }
    }
    cmd_functions = list;
}

qboolean Cmd_Exists(char *cmd_name)
{
    cmd_function_t *cmd;

    for (cmd = cmd_functions; cmd; cmd = cmd->next)
    {
        if (!Q_strcmp(cmd_name, cmd->name))
            return 1;
    }
    return 0;
}

char *Cmd_CompleteCommand(char *partial)
{
    cmd_function_t *cmd;
    int len;
    char *best;

    best = NULL;
    len = Q_strlen(partial);
    if (!len)
        return NULL;

    for (cmd = cmd_functions; cmd; cmd = cmd->next)
    {
        if (!Q_strncmp(partial, cmd->name, len))
        {
            if (Q_strlen(cmd->name) == len)
                return cmd->name;
            best = cmd->name;
        }
    }
    return best;
}

void Cmd_ExecuteString(char *text, cmd_source_t src)
{
    cmd_function_t *cmd;
    cmdalias_t *a;

    cmd_source = src;
    Cmd_TokenizeString(text);

    if (!Cmd_Argc())
        return;

    for (cmd = cmd_functions; cmd; cmd = cmd->next)
    {
        if (!Q_strcasecmp(cmd_argv[0], cmd->name))
        {
            cmd->function();
            return;
        }
    }

    for (a = cmd_alias; a; a = a->next)
    {
        if (!Q_strcasecmp(cmd_argv[0], a->name))
        {
            Cbuf_InsertText(a->value);
            return;
        }
    }

    if (Cvar_Command())
        return;

    if (cls[0].state == ca_active || cls[0].state == ca_connected || cls[0].state == ca_uninitialized)
        Cmd_ForwardToServer();
}

void Cmd_ForwardToServer(void)
{
    if (cls[0].state != ca_connected && cls[0].state != ca_uninitialized && cls[0].state != ca_active)
    {
        if (strcasecmp(Cmd_Argv(0), "setinfo"))
            Con_Printf("Can't \"%s\", not connected\n", Cmd_Argv(0));
        return;
    }

    if (cls_demoplayback)
        return;

    MSG_WriteByte(&cls[0].netchan.message, clc_stringcmd);
    if (Q_strcasecmp(Cmd_Argv(0), "cmd"))
    {
        SZ_Print(&cls[0].netchan.message, Cmd_Argv(0));
        SZ_Print(&cls[0].netchan.message, " ");
    }
    if (Cmd_Argc() > 1)
        SZ_Print(&cls[0].netchan.message, Cmd_Args());
    else
        SZ_Print(&cls[0].netchan.message, "\n");
}

void Cmd_ForwardToServerUnreliable(void)
{
    if (cls[1].state != ca_connected && cls[1].state != ca_uninitialized && cls[1].state != ca_active)
    {
        if (strcasecmp(Cmd_Argv(0), "setinfo"))
            Con_Printf("Can't \"%s\", not connected\n", Cmd_Argv(0));
        return;
    }

    if (cls_demoplayback)
        return;

    MSG_WriteByte(&cls[1].netchan.message, clc_stringcmd);
    if (Q_strcasecmp(Cmd_Argv(0), "cmd2"))
    {
        SZ_Print(&cls[1].netchan.message, Cmd_Argv(0));
        SZ_Print(&cls[1].netchan.message, " ");
    }
    if (Cmd_Argc() > 1)
        SZ_Print(&cls[1].netchan.message, Cmd_Args());
    else
        SZ_Print(&cls[1].netchan.message, "\n");
}

int Cmd_CheckParm(char *parm)
{
    int i;

    if (!parm)
        Sys_Error("Cmd_CheckParm: NULL");

    for (i = 1; i < Cmd_Argc(); i++)
    {
        if (!Q_strcasecmp(parm, Cmd_Argv(i)))
            return i;
    }

    return 0;
}

void Cmd_CmdList_f(void)
{
    cmd_function_t *cmd;
    int iCmds;
    int iArgs;
    char *partial;
    int ipLen;

    iCmds = 0;
    partial = NULL;
    ipLen = 0;

    iArgs = Cmd_Argc();
    if (iArgs > 1)
    {
        if (!Q_strcasecmp(Cmd_Argv(1), "?"))
        {
            Con_Printf("CmdList           : List all commands\nCmdList [Partial] : List commands starting with 'Partial'\nCmdList log logfile [Partial] : Logs commands to file c:\\logfile.\nNOTE:  No relative paths allowed!");
            return;
        }

        partial = Cmd_Argv(1);
        ipLen = strlen(partial);
    }

    Con_Printf("Command List\n--------------\n");

    for (cmd = cmd_functions; cmd; cmd = cmd->next)
    {
        if (partial)
        {
            if (Q_strncasecmp(cmd->name, partial, ipLen))
                continue;
            Con_Printf("%-16.16s\n", cmd->name);
            iCmds++;
        }
        else
        {
            Con_Printf("%-16.16s\n", cmd->name);
            iCmds++;
        }
    }

    if (iArgs == 2 && partial && *partial)
        Con_Printf("--------------\n%3i Commands for [%s]\nCmdList ? for syntax\n", iCmds, partial);
    else
        Con_Printf("--------------\n%3i Total Commands\nCmdList ? for syntax\n", iCmds);
}
