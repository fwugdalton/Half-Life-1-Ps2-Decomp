#include "common.h"
#include "engine/cmd.h"
#include <stdarg.h>

#define CON_TEXTSIZE 16384
#define MAXPRINTMSG 4096

#define key_console 1
#define key_message 2

typedef struct cvar_s
{
    char *name;
    char *string;
    int flags;
    float value;
    struct cvar_s *next;
} cvar_t;

typedef struct
{
    int width;
    int height;
} viddef_t;

typedef void *FILE_t;

int atoi(const char *);
int sprintf(char *, const char *, ...);
int vsprintf(char *, const char *, va_list);
char *strncpy(char *, const char *, int);
int COM_CheckParm(char *parm);
char *va(char *format, ...);
void *Hunk_AllocName(int size, char *name);
void Cvar_RegisterVariable(cvar_t *variable);
void Sys_Printf(char *fmt, ...);
void SV_FlushRedirect(void);
void SCR_UpdateScreen(void);
void SCR_EndLoadingPlaque(void);
int Draw_Character(int x, int y, int num);
int Draw_String(int x, int y, char *str);
void Draw_ConsoleBackground(int lines);
int func_00234F20(void *font);
int func_00234F58(void *font, char *str);
FILE_t func_002224B8(char *name, char *mode);
void func_002224C8(FILE_t f);
int func_002224D8(void *buf, int size, int count, FILE_t f);

extern int key_dest;
extern int edit_line;
extern int key_linepos;
extern char key_lines[][256];
extern cvar_t developer;
extern float D_003995E0;
extern int D_00399798;
extern int D_00399774;
extern char message_type[32];
extern int chat_bufferlen;
extern char chat_buffer[];
extern viddef_t vid;
extern char com_gamedir[];
extern int sv_redirected;
extern char outputbuf[1400];
extern int scr_disabled_for_loading;
extern int scr_copytop;
extern int clearnotify;
extern void *font;


extern int con_vislines;
extern int con_backscroll;
extern qboolean con_initialized;
extern qboolean con_debuglog;
extern int con_notifylines;
extern int con_current;
extern int con_forcedup;
extern int con_x;
extern int con_totallines;
extern float realtime;
extern int con_cr;
extern char con_debuglog_data[1024];
extern qboolean con_inupdate;

int D_00331C18 = 0;
int con_linewidth = 1;
float con_cursorspeed = 4;
char *con_text = NULL;
cvar_t con_notifytime = { "con_notifytime", "4" };
int con_num_times = 4;
float *con_times = NULL;
int *con_notifypos = NULL;
int con_numrows = 0;
int con_dprintf = 0;

void Con_ToggleConsole_f(void);
void Con_ClearNotify(void);
void Con_CheckResize(void);
void Con_Linefeed(void);
void Con_Print(char *txt);
void Con_DebugLog(char *file, char *fmt, ...);
void Con_DrawInput(void);

void Con_SetTimes_f(void)
{
    int newtimes;

    if (Cmd_Argc() != 2)
    {
        Con_Printf("contimes <n>\nShow <n> overlay lines [4-64].\n%i current overlay lines.\n", con_num_times);
        return;
    }

    newtimes = atoi(Cmd_Argv(1));
    if (newtimes < 4)
        newtimes = 4;
    if (newtimes > 64)
        newtimes = 64;

    if (con_times)
        Mem_Free(con_times);
    if (con_notifypos)
        Mem_Free(con_notifypos);

    con_times = Mem_DbgMalloc(newtimes * sizeof(float), "engine/Console.c", 98);
    con_notifypos = Mem_DbgMalloc(newtimes * sizeof(int), "engine/Console.c", 99);
    if (!con_times || !con_notifypos)
        Sys_Error("Couldn't allocate space for %i console overlays.", newtimes);

    con_num_times = newtimes;
    Con_Printf("%i lines will overlay.\n", newtimes);
}

void Con_HideConsole_f(void)
{
    if (key_dest == key_console)
        Con_ToggleConsole_f();
}

void Con_ToggleConsole_f(void)
{
    if (key_dest == key_console)
    {
        if (cls.state == ca_active || cls.state == ca_connected || cls.state == ca_uninitialized)
        {
            key_dest = 0;
            key_lines[edit_line][1] = 0;
            key_linepos = 1;
        }
        else
        {
            D_00399774 = 2;
        }
    }
    else
    {
        if (D_00399798 & 4)
            return;
        if (D_00331C18)
            return;
        key_dest = key_console;
    }

    SCR_EndLoadingPlaque();
    memset(con_times, 0, sizeof(con_times));
    memset(con_notifypos, 0, sizeof(con_notifypos));
}

void Con_Clear_f(void)
{
    if (con_text)
        memset(con_text, ' ', CON_TEXTSIZE);
}

void Con_ClearNotify(void)
{
    int i;

    for (i = 0; i < con_num_times; i++)
        con_times[i] = 0;
}

void Con_MessageMode_f(void)
{
    key_dest = key_message;
    if (Cmd_Argc() == 2)
        strcpy(message_type, Cmd_Argv(1));
    else
        strcpy(message_type, "say");
}

void Con_MessageMode2_f(void)
{
    key_dest = key_message;
    strcpy(message_type, "say_team");
}

void Con_CheckResize(void)
{
    int i, j, width, oldwidth, oldtotal, numlines, numchars;
    char tbuf[CON_TEXTSIZE];

    width = ((vid.width - 16) >> 3) - 2;

    if (width == con_linewidth)
        return;

    if (width < 1)
    {
        width = 38;
        con_linewidth = width;
        con_totallines = CON_TEXTSIZE / con_linewidth;
        memset(con_text, ' ', CON_TEXTSIZE);
    }
    else
    {
        oldwidth = con_linewidth;
        con_linewidth = width;
        oldtotal = con_totallines;
        con_totallines = CON_TEXTSIZE / con_linewidth;
        numlines = oldtotal;

        if (con_totallines < numlines)
            numlines = con_totallines;

        numchars = oldwidth;

        if (con_linewidth < numchars)
            numchars = con_linewidth;

        memcpy(tbuf, con_text, CON_TEXTSIZE);
        memset(con_text, ' ', CON_TEXTSIZE);

        for (i = 0; i < numlines; i++)
        {
            for (j = 0; j < numchars; j++)
            {
                con_text[(con_totallines - 1 - i) * con_linewidth + j] =
                    tbuf[((con_current - i + oldtotal) % oldtotal) * oldwidth + j];
            }
        }

        Con_ClearNotify();
    }

    con_backscroll = 0;
    con_current = con_totallines - 1;
}

void Con_Init(void)
{
#define MAXGAMEDIRLEN 1000
    char temp[MAXGAMEDIRLEN + 1];
    char *t2 = "/qconsole.log";

    con_debuglog = COM_CheckParm("-condebug");

    if (con_debuglog)
    {
        if (strlen(com_gamedir) < (MAXGAMEDIRLEN - strlen(t2)))
            sprintf(temp, "%s%s", com_gamedir, t2);
    }

    con_text = Hunk_AllocName(CON_TEXTSIZE, "context");
    memset(con_text, ' ', CON_TEXTSIZE);
    con_linewidth = -1;

    con_times = Mem_DbgMalloc(con_num_times * sizeof(float), "engine/Console.c", 308);
    con_notifypos = Mem_DbgMalloc(con_num_times * sizeof(int), "engine/Console.c", 309);
    if (!con_times || !con_notifypos)
        Sys_Error("Couldn't allocate space for %i console overlays.", con_num_times);

    Con_CheckResize();

    Con_Printf("Console initialized.\n");

    Cvar_RegisterVariable(&con_notifytime);

    Cmd_AddCommand("contimes", Con_SetTimes_f);
    Cmd_AddCommand("toggleconsole", Con_ToggleConsole_f);
    Cmd_AddCommand("hideconsole", Con_HideConsole_f);
    Cmd_AddCommand("messagemode", Con_MessageMode_f);
    Cmd_AddCommand("messagemode2", Con_MessageMode2_f);
    Cmd_AddCommand("clear", Con_Clear_f);

    con_initialized = true;
}

void Con_Shutdown(void)
{
    if (con_times)
        Mem_Free(con_times);
    if (con_notifypos)
        Mem_Free(con_notifypos);

    con_times = NULL;
    con_notifypos = NULL;
    con_initialized = false;
}

void Con_Linefeed(void)
{
    con_x = 0;
    con_current++;
    memset(&con_text[(con_current % con_totallines) * con_linewidth], ' ', con_linewidth);
}

void Con_Print(char *txt)
{
    int y;
    int c, l;
    int fNotify;

    fNotify = developer.value > 0;
    con_backscroll = 0;

    if (txt[0] == 1)
    {
        fNotify = 1;
        txt++;
    }
    else if (txt[0] == 2)
    {
        txt++;
    }

    while ((c = *txt))
    {
        for (l = 0; l < con_linewidth; l++)
            if (txt[l] <= ' ')
                break;

        if (l != con_linewidth && (con_x + l > con_linewidth))
            con_x = 0;

        txt++;

        if (con_cr)
        {
            con_current--;
            con_cr = false;
        }

        if (!con_x)
        {
            Con_Linefeed();
            if (fNotify)
            {
                if (con_current >= 0)
                    con_times[con_current % con_num_times] = realtime;
                con_notifypos[con_current % con_num_times] = con_current % con_totallines;
            }
        }

        switch (c)
        {
        case '\n':
            con_x = 0;
            break;

        case '\r':
            con_x = 0;
            con_cr = 1;
            break;

        default:
            y = con_current % con_totallines;
            con_text[y * con_linewidth + con_x] = c;
            con_x++;
            if (con_x >= con_linewidth)
                con_x = 0;
            break;
        }
    }
}

void Con_DebugLog(char *file, char *fmt, ...)
{
    va_list argptr;
    FILE_t fd;

    va_start(argptr, fmt);
    vsprintf(con_debuglog_data, fmt, argptr);
    va_end(argptr);
    fd = func_002224B8(file, "wa");
    func_002224D8(con_debuglog_data, strlen(con_debuglog_data), 1, fd);
    func_002224C8(fd);
}

void Con_Printf(const char *fmt, ...)
{
    va_list argptr;
    char msg[MAXPRINTMSG];

    va_start(argptr, fmt);
    vsprintf(msg, fmt, argptr);
    va_end(argptr);

    Sys_Printf("%s", msg);

    if (sv_redirected)
    {
        if (strlen(msg) + strlen(outputbuf) > sizeof(outputbuf) - 1)
            SV_FlushRedirect();
        strcat(outputbuf, msg);
        return;
    }

    if (con_debuglog)
        Con_DebugLog(va("%s/qconsole.log", com_gamedir), "%s", msg);

    if (!con_initialized)
        return;

    if (cls.state == ca_dedicated)
        return;

    if ((!D_00331C18 && !(D_00399798 & 4)) || con_dprintf)
        Con_Print(msg);

    if (cls.signon != 3 && !scr_disabled_for_loading)
    {
        if (!con_inupdate)
        {
            con_inupdate = true;
            SCR_UpdateScreen();
            con_inupdate = false;
        }
    }
}

void Con_DPrintf(const char *fmt, ...)
{
    va_list argptr;
    char msg[MAXPRINTMSG];

    if (!developer.value)
        return;

    if (D_003995E0 && cls.state == ca_active)
        return;

    va_start(argptr, fmt);
    vsprintf(msg, fmt, argptr);
    va_end(argptr);

    con_dprintf = 1;
    Con_Printf("%s", msg);
    con_dprintf = 0;
}

void Con_SafePrintf(const char *fmt, ...)
{
    va_list argptr;
    char msg[1024];
    int temp;

    va_start(argptr, fmt);
    vsprintf(msg, fmt, argptr);
    va_end(argptr);

    temp = scr_disabled_for_loading;
    scr_disabled_for_loading = true;
    con_dprintf = 1;
    Con_Printf("%s", msg);
    scr_disabled_for_loading = temp;
    con_dprintf = 0;
}

int Con_TextTop(int lines)
{
    int y, h;

    if (lines > vid.height - 24)
        lines = vid.height - 24;

    h = func_00234F20(font);
    y = lines - 4;
    y -= h;
    if (y < 28)
        y = 28;
    return y;
}

void Con_DrawInput(void)
{
    int x, y;
    int i;
    char *text;

    if (key_dest != key_console && !con_forcedup)
        return;

    text = key_lines[edit_line];

    text[key_linepos] = 10 + ((int)(realtime * con_cursorspeed) & 1);

    for (i = key_linepos + 1; i < con_linewidth; i++)
        text[i] = ' ';

    if (key_linepos >= con_linewidth)
        text += 1 + key_linepos - con_linewidth;

    y = Con_TextTop(con_vislines);
    x = func_00234F58(font, "]") + 8;

    for (i = 0; i < con_linewidth; i++)
        x += Draw_Character(x, y, text[i]);

    key_lines[edit_line][key_linepos] = 0;
}

void Con_GetInput(char *buf, int size)
{
    strncpy(buf, key_lines[edit_line], size < con_linewidth ? size : con_linewidth);
    buf[(size < con_linewidth ? size : con_linewidth) - 1] = 0;
}

#ifdef NON_MATCHING
void Con_DrawNotify(void)
{
    int x, v;
    char *text;
    int i, j;
    float time;

    v = 28;
    for (i = con_current - con_num_times + 1; i <= con_current; i++)
    {
        if (i < 0)
            continue;
        time = con_times[i % con_num_times];
        if (time == 0)
            continue;
        time = realtime - time;
        if (time > con_notifytime.value)
            continue;
        text = con_text + con_notifypos[i % con_num_times] * con_linewidth;

        clearnotify = 0;
        scr_copytop = 1;

        x = 16;
        for (j = 0; j < con_linewidth; j++)
            x += Draw_Character(x, v, text[j]);

        v += func_00234F20(font);
    }

    if (key_dest == key_message)
    {
        int maxchars, offset;

        scr_copytop = 1;
        clearnotify = 0;

        offset = 0;
        maxchars = vid.width / 10;
        if (chat_bufferlen > maxchars)
            offset = chat_bufferlen - maxchars;

        x = Draw_String(16, v, message_type);
        x = Draw_String(x, v, ": ");
        while (chat_buffer[offset])
        {
            x += Draw_Character(x, v, chat_buffer[offset]);
            offset++;
        }
        Draw_Character((offset + 5) << 3, v, 10 + ((int)(realtime * con_cursorspeed) & 1));
        v += func_00234F20(font);
    }

    if (v > con_notifylines)
        con_notifylines = v;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/Console", Con_DrawNotify);
#endif

void Con_DrawConsole(int lines, qboolean drawinput)
{
    int i, x, y;
    int rows;
    char *text;
    int j, k;

    if (lines <= 0)
        return;

    Draw_ConsoleBackground(lines);

    if (D_00399798 & 4)
        return;
    if (D_00331C18)
        return;

    con_vislines = lines;
    lines -= 28;
    rows = lines / func_00234F20(font) - 1;
    con_numrows = rows;
    y = Con_TextTop(con_vislines) - rows * func_00234F20(font);

    for (i = con_current - rows + 1; i <= con_current; i++, y += func_00234F20(font))
    {
        if (y < 29)
            continue;

        j = i - con_backscroll;
        if (j < 0)
            j = 0;
        text = con_text + (j % con_totallines) * con_linewidth;

        x = 16;
        for (k = 0; k < con_linewidth; k++)
            x += Draw_Character(x, y, text[k]);
    }

    if (drawinput)
        Con_DrawInput();
}
