#include "common.h"
#include "engine/cmd.h"
#include "engine/server.h"

/* keys.c: entrada de teclado/controle e bindings. No PS2 há 400 códigos de tecla
 * (200 por jogador, com o segundo DualShock em +100). */

#define MAXCMDLINE 256
#define MAX_KEYS 400

#define K_TAB 9
#define K_ENTER 13
#define K_ESCAPE 27
#define K_SPACE 32
#define K_BACKSPACE 127
#define K_UPARROW 128
#define K_DOWNARROW 129
#define K_LEFTARROW 130
#define K_RIGHTARROW 131
#define K_ALT 132
#define K_CTRL 133
#define K_SHIFT 134
#define K_F1 135
#define K_F2 136
#define K_F3 137
#define K_F4 138
#define K_F5 139
#define K_F6 140
#define K_F7 141
#define K_F8 142
#define K_F9 143
#define K_F10 144
#define K_F11 145
#define K_F12 146
#define K_INS 147
#define K_DEL 148
#define K_PGDN 149
#define K_PGUP 150
#define K_HOME 151
#define K_END 152
#define K_MOUSE1 200
#define K_MOUSE2 201
#define K_MOUSE3 202
#define K_JOY1 203
#define K_JOY2 204
#define K_JOY3 205
#define K_JOY4 206
#define K_MWHEELDOWN 207
#define K_MWHEELUP 208
#define K_PAUSE 255

#define K_DS00_DPAD_UP 210
#define K_DS10_DPAD_UP 310

typedef enum
{
    key_game,
    key_console,
    key_message,
    key_menu,
    key_unk4,
    key_unk5
} keydest_t;

typedef struct
{
    char *name;
    int keynum;
} keyname_t;

void Cbuf_Execute(void);
char *Cmd_CompleteCommand(char *partial);
char *Cvar_CompleteVariable(char *partial);
void Con_ToggleConsole_f(void);
void SCR_UpdateScreen(void);
float Sys_FloatTime(void);
void FS_FPrintf(FileHandle_t f, char *fmt, ...);
char *strchr(const char *s, int c);
void func_00225610(int key);

extern int con_numrows;
extern int con_backscroll;
extern int con_totallines;
extern qboolean con_forcedup;
extern unsigned int D_0053D1C8; /* vid.height */
extern int giActive;
extern int giStateInfo;
extern int D_00399798;

/* .bss */
extern char *keybindings[MAX_KEYS];
extern int keyshift[MAX_KEYS];
extern char key_lines[32][MAXCMDLINE];
extern int key_repeats[MAX_KEYS];
extern qboolean consolekeys[MAX_KEYS];
extern qboolean keydown[MAX_KEYS];
extern qboolean menubound[MAX_KEYS];
extern int key_linepos;
extern int key_lastpress;
extern char chat_buffer[120];
extern float key_repeatdelay;
extern char key_tinystr[2];

/* .data */
qboolean shift_down = false;
int edit_line = 0;
int history_line = 0;
keydest_t key_dest = key_console;

keyname_t keynames[] =
{
    {"TAB", K_TAB},
    {"ENTER", K_ENTER},
    {"ESCAPE", K_ESCAPE},
    {"SPACE", K_SPACE},
    {"BACKSPACE", K_BACKSPACE},
    {"UPARROW", K_UPARROW},
    {"DOWNARROW", K_DOWNARROW},
    {"LEFTARROW", K_LEFTARROW},
    {"RIGHTARROW", K_RIGHTARROW},

    {"ALT", K_ALT},
    {"CTRL", K_CTRL},
    {"SHIFT", K_SHIFT},

    {"F1", K_F1},
    {"F2", K_F2},
    {"F3", K_F3},
    {"F4", K_F4},
    {"F5", K_F5},
    {"F6", K_F6},
    {"F7", K_F7},
    {"F8", K_F8},
    {"F9", K_F9},
    {"F10", K_F10},
    {"F11", K_F11},
    {"F12", K_F12},

    {"INS", K_INS},
    {"DEL", K_DEL},
    {"PGDN", K_PGDN},
    {"PGUP", K_PGUP},
    {"HOME", K_HOME},
    {"END", K_END},

    {"MOUSE1", K_MOUSE1},
    {"MOUSE2", K_MOUSE2},
    {"MOUSE3", K_MOUSE3},

    {"JOY1", K_JOY1},
    {"JOY2", K_JOY2},
    {"JOY3", K_JOY3},
    {"JOY4", K_JOY4},

    {"MWHEELUP", K_MWHEELUP},
    {"MWHEELDOWN", K_MWHEELDOWN},

    {"PAUSE", K_PAUSE},

    {"SEMICOLON", ';'},

    {"DUALSHOCK_00_DPAD_UP", K_DS00_DPAD_UP},
    {"DUALSHOCK_00_DPAD_DOWN", K_DS00_DPAD_UP + 1},
    {"DUALSHOCK_00_DPAD_LEFT", K_DS00_DPAD_UP + 2},
    {"DUALSHOCK_00_DPAD_RIGHT", K_DS00_DPAD_UP + 3},
    {"DUALSHOCK_00_TRIANGLE", K_DS00_DPAD_UP + 4},
    {"DUALSHOCK_00_X", K_DS00_DPAD_UP + 5},
    {"DUALSHOCK_00_SQUARE", K_DS00_DPAD_UP + 6},
    {"DUALSHOCK_00_CIRCLE", K_DS00_DPAD_UP + 7},
    {"DUALSHOCK_00_L1", K_DS00_DPAD_UP + 8},
    {"DUALSHOCK_00_L2", K_DS00_DPAD_UP + 9},
    {"DUALSHOCK_00_L3", K_DS00_DPAD_UP + 10},
    {"DUALSHOCK_00_R1", K_DS00_DPAD_UP + 11},
    {"DUALSHOCK_00_R2", K_DS00_DPAD_UP + 12},
    {"DUALSHOCK_00_R3", K_DS00_DPAD_UP + 13},
    {"DUALSHOCK_00_SELECT", K_DS00_DPAD_UP + 14},
    {"DUALSHOCK_00_START", K_DS00_DPAD_UP + 15},
    {"DUALSHOCK_00_LEFTSTICK_UP", K_DS00_DPAD_UP + 16},
    {"DUALSHOCK_00_LEFTSTICK_DOWN", K_DS00_DPAD_UP + 17},
    {"DUALSHOCK_00_LEFTSTICK_LEFT", K_DS00_DPAD_UP + 18},
    {"DUALSHOCK_00_LEFTSTICK_RIGHT", K_DS00_DPAD_UP + 19},
    {"DUALSHOCK_00_RIGHTSTICK_UP", K_DS00_DPAD_UP + 20},
    {"DUALSHOCK_00_RIGHTSTICK_DOWN", K_DS00_DPAD_UP + 21},
    {"DUALSHOCK_00_RIGHTSTICK_LEFT", K_DS00_DPAD_UP + 22},
    {"DUALSHOCK_00_RIGHTSTICK_RIGHT", K_DS00_DPAD_UP + 23},
    {"DUALSHOCK_00_LEFTSTICK_SMALL_MOVE", K_DS00_DPAD_UP + 24},
    {"DUALSHOCK_00_RIGHTSTICK_SMALL_MOVE", K_DS00_DPAD_UP + 25},

    {"DUALSHOCK_10_DPAD_UP", K_DS10_DPAD_UP},
    {"DUALSHOCK_10_DPAD_DOWN", K_DS10_DPAD_UP + 1},
    {"DUALSHOCK_10_DPAD_LEFT", K_DS10_DPAD_UP + 2},
    {"DUALSHOCK_10_DPAD_RIGHT", K_DS10_DPAD_UP + 3},
    {"DUALSHOCK_10_TRIANGLE", K_DS10_DPAD_UP + 4},
    {"DUALSHOCK_10_X", K_DS10_DPAD_UP + 5},
    {"DUALSHOCK_10_SQUARE", K_DS10_DPAD_UP + 6},
    {"DUALSHOCK_10_CIRCLE", K_DS10_DPAD_UP + 7},
    {"DUALSHOCK_10_L1", K_DS10_DPAD_UP + 8},
    {"DUALSHOCK_10_L2", K_DS10_DPAD_UP + 9},
    {"DUALSHOCK_10_L3", K_DS10_DPAD_UP + 10},
    {"DUALSHOCK_10_R1", K_DS10_DPAD_UP + 11},
    {"DUALSHOCK_10_R2", K_DS10_DPAD_UP + 12},
    {"DUALSHOCK_10_R3", K_DS10_DPAD_UP + 13},
    {"DUALSHOCK_10_SELECT", K_DS10_DPAD_UP + 14},
    {"DUALSHOCK_10_START", K_DS10_DPAD_UP + 15},
    {"DUALSHOCK_10_LEFTSTICK_UP", K_DS10_DPAD_UP + 16},
    {"DUALSHOCK_10_LEFTSTICK_DOWN", K_DS10_DPAD_UP + 17},
    {"DUALSHOCK_10_LEFTSTICK_LEFT", K_DS10_DPAD_UP + 18},
    {"DUALSHOCK_10_LEFTSTICK_RIGHT", K_DS10_DPAD_UP + 19},
    {"DUALSHOCK_10_RIGHTSTICK_UP", K_DS10_DPAD_UP + 20},
    {"DUALSHOCK_10_RIGHTSTICK_DOWN", K_DS10_DPAD_UP + 21},
    {"DUALSHOCK_10_RIGHTSTICK_LEFT", K_DS10_DPAD_UP + 22},
    {"DUALSHOCK_10_RIGHTSTICK_RIGHT", K_DS10_DPAD_UP + 23},
    {"DUALSHOCK_10_LEFTSTICK_SMALL_MOVE", K_DS10_DPAD_UP + 24},
    {"DUALSHOCK_10_RIGHTSTICK_SMALL_MOVE", K_DS10_DPAD_UP + 25},

    {NULL, 0}
};

char message_type[32] = "say";
int chat_bufferlen = 0;

void Key_Message(int key);

qboolean CheckForCommand(void)
{
    char command[128];
    char *cmd, *s;
    int i;

    s = key_lines[edit_line] + 1;

    for (i = 0; i < 127; i++)
    {
        if (s[i] <= ' ')
            break;
        else
            command[i] = s[i];
    }
    command[i] = 0;

    cmd = Cmd_CompleteCommand(command);
    if (!cmd || strcmp(cmd, command))
        cmd = Cvar_CompleteVariable(command);
    if (!cmd || strcmp(cmd, command))
        return false;

    return true;
}

void CompleteCommand(void)
{
    char *cmd, *s;

    s = key_lines[edit_line] + 1;
    if (*s == '\\' || *s == '/')
        s++;

    cmd = Cmd_CompleteCommand(s);
    if (!cmd)
        cmd = Cvar_CompleteVariable(s);
    if (cmd)
    {
        key_lines[edit_line][1] = '/';
        Q_strcpy(key_lines[edit_line] + 2, cmd);
        key_linepos = Q_strlen(cmd) + 2;
        key_lines[edit_line][key_linepos] = ' ';
        key_linepos++;
        key_lines[edit_line][key_linepos] = 0;
        return;
    }
}

void Key_HistoryUp(void)
{
    do
    {
        history_line = (history_line - 1) & 31;
    } while (history_line != edit_line && !key_lines[history_line][1]);

    if (history_line == edit_line)
        history_line = (edit_line + 1) & 31;

    Q_strcpy(key_lines[edit_line], key_lines[history_line]);
    key_linepos = Q_strlen(key_lines[edit_line]);
}

void Key_Console(int key)
{
    if (key == K_ENTER)
    {
        if (key_lines[edit_line][1] == '\\' || key_lines[edit_line][1] == '/')
            Cbuf_AddText(key_lines[edit_line] + 2);
        else if (CheckForCommand())
            Cbuf_AddText(key_lines[edit_line] + 1);
        else
            Cbuf_AddText(key_lines[edit_line] + 1);

        Cbuf_AddText("\n");
        Con_Printf("%s\n", key_lines[edit_line]);
        edit_line = (edit_line + 1) & 31;
        history_line = edit_line;
        key_lines[edit_line][0] = ']';
        key_linepos = 1;
        if (cls[0].state == ca_disconnected)
            SCR_UpdateScreen();
        return;
    }

    if (key == K_TAB)
    {
        CompleteCommand();
        return;
    }

    if (key == K_BACKSPACE || key == K_LEFTARROW)
    {
        if (key_linepos > 1)
            key_linepos--;
        return;
    }

    if (key == K_UPARROW)
    {
        do
        {
            history_line = (history_line - 1) & 31;
        } while (history_line != edit_line && !key_lines[history_line][1]);

        if (history_line == edit_line)
            history_line = (edit_line + 1) & 31;

        Q_strcpy(key_lines[edit_line], key_lines[history_line]);
        key_linepos = Q_strlen(key_lines[edit_line]);
        return;
    }

    if (key == K_DOWNARROW)
    {
        if (history_line == edit_line)
            return;

        do
        {
            history_line = (history_line + 1) & 31;
        } while (history_line != edit_line && !key_lines[history_line][1]);

        if (history_line == edit_line)
        {
            key_lines[edit_line][0] = ']';
            key_linepos = 1;
        }
        else
        {
            Q_strcpy(key_lines[edit_line], key_lines[history_line]);
            key_linepos = Q_strlen(key_lines[edit_line]);
        }
        return;
    }

    if (key == K_PGUP)
    {
        con_backscroll += (con_numrows / 2 > 3) ? con_numrows / 2 : 4;
        if (con_backscroll > con_totallines - (int)(D_0053D1C8 >> 3) - 1)
            con_backscroll = con_totallines - (int)(D_0053D1C8 >> 3) - 1;
        return;
    }

    if (key == K_PGDN)
    {
        con_backscroll -= (con_numrows / 2 > 3) ? con_numrows / 2 : 4;
        if (con_backscroll < 0)
            con_backscroll = 0;
        return;
    }

    if (key == K_HOME)
    {
        con_backscroll = con_totallines - (D_0053D1C8 >> 3) - 1;
        return;
    }

    if (key == K_END)
    {
        con_backscroll = 0;
        return;
    }

    if (key < 32 || key > 127)
        return;

    if (key_linepos < MAXCMDLINE - 1)
    {
        key_lines[edit_line][key_linepos] = key;
        key_linepos++;
        key_lines[edit_line][key_linepos] = 0;
    }
}

void Key_Message(int key)
{
    if (key == K_ENTER)
    {
        Cbuf_AddText(message_type);
        Cbuf_AddText(" \"");
        Cbuf_AddText(chat_buffer);
        Cbuf_AddText("\"\n");

        key_dest = key_game;
        strcpy(message_type, "say");
        chat_bufferlen = 0;
        chat_buffer[0] = 0;
        return;
    }

    if (key == K_ESCAPE)
    {
        key_dest = key_game;
        chat_bufferlen = 0;
        chat_buffer[0] = 0;
        return;
    }

    if (key < 32 || key > 127)
        return;

    if (key == K_BACKSPACE)
    {
        if (chat_bufferlen)
        {
            chat_bufferlen--;
            chat_buffer[chat_bufferlen] = 0;
        }
        return;
    }

    if (chat_bufferlen == 120)
        return;

    chat_buffer[chat_bufferlen++] = key;
    chat_buffer[chat_bufferlen] = 0;
}

int Key_StringToKeynum(char *str)
{
    keyname_t *kn;

    if (!str || !str[0])
        return -1;
    if (!str[1])
        return str[0];

    for (kn = keynames; kn->name; kn++)
    {
        if (!Q_strcasecmp(str, kn->name))
            return kn->keynum;
    }
    return -1;
}

char *Key_KeynumToString(int keynum)
{
    keyname_t *kn;

    if (keynum == -1)
        return "<KEY NOT FOUND>";
    if (keynum > 32 && keynum < 127)
    {
        key_tinystr[0] = keynum;
        key_tinystr[1] = 0;
        return key_tinystr;
    }

    for (kn = keynames; kn->name; kn++)
    {
        if (keynum == kn->keynum)
            return kn->name;
    }

    return "<UNKNOWN KEYNUM>";
}

void Key_ClearBinding(int keynum)
{
    if (keynum == -1)
        return;

    if (keybindings[keynum])
    {
        Z_Free(keybindings[keynum]);
        keybindings[keynum] = NULL;
    }
}

void Key_SetBinding(int keynum, char *binding)
{
    char *new;
    int l;

    if (keynum == -1)
        return;

    if (keybindings[keynum])
    {
        if (!strcmp(keybindings[keynum], binding))
            return;

        Z_Free(keybindings[keynum]);
        keybindings[keynum] = NULL;
    }

    l = Q_strlen(binding);
    new = Z_Malloc(l + 1);
    Q_strcpy(new, binding);
    new[l] = 0;
    keybindings[keynum] = new;
}

qboolean Key_FindBinding(char *binding, int max, int skip, int *count, int *keys)
{
    int i;
    int found;
    qboolean ret;

    ret = true;
    found = 0;
    *count = 0;

    for (i = 0; i < MAX_KEYS; i++)
    {
        if (!keybindings[i])
            continue;
        if (strcasecmp(keybindings[i], binding))
            continue;

        found++;
        if (found < skip)
            continue;

        if (*count == max)
        {
            ret = false;
            break;
        }

        keys[*count] = i;
        (*count)++;
    }

    return ret;
}

void Key_Unbind_f(void)
{
    int b;

    if (Cmd_Argc() != 2)
    {
        Con_Printf("unbind <key> : remove commands from a key\n");
        return;
    }

    b = Key_StringToKeynum(Cmd_Argv(1));
    if (b == -1)
    {
        Con_Printf("\"%s\" isn't a valid key\n", Cmd_Argv(1));
        return;
    }

    if (b == K_ESCAPE)
    {
        Con_Printf("Can't unbind ESCAPE key\n");
        return;
    }

    Key_SetBinding(b, "");
}

void Key_Unbindall_f(void)
{
    int i;

    for (i = 0; i < MAX_KEYS; i++)
    {
        if (keybindings[i])
        {
            if (i != K_ESCAPE)
                Key_SetBinding(i, "");
        }
    }
}

void Key_Escape_f(void)
{
    if (D_00399798 & 0x10)
    {
        Cbuf_AddText("disconnect\n");
        giActive = 2;
        giStateInfo = 2;
        D_00399798 = 1;
        Cbuf_Execute();
        return;
    }

    switch (key_dest)
    {
        case key_message:
            Key_Message(K_ESCAPE);
            break;
        case key_console:
            Con_ToggleConsole_f();
            break;
        case key_game:
        case key_unk4:
        case key_unk5:
            giActive = 2;
            break;
        default:
            Sys_Error("Bad key_dest");
    }
}

void Key_Bind_f(void)
{
    int i, c, b;
    char cmd[1024];

    c = Cmd_Argc();

    if (c != 2 && c != 3)
    {
        Con_Printf("bind <key> [command] : attach a command to a key\n");
        return;
    }

    b = Key_StringToKeynum(Cmd_Argv(1));
    if (b == -1)
    {
        Con_Printf("\"%s\" isn't a valid key\n", Cmd_Argv(1));
        return;
    }

    if (c == 2)
    {
        if (keybindings[b])
            Con_Printf("\"%s\" = \"%s\"\n", Cmd_Argv(1), keybindings[b]);
        else
            Con_Printf("\"%s\" is not bound\n", Cmd_Argv(1));
        return;
    }

    if (b == K_ESCAPE)
    {
        strcpy(cmd, "cancelselect");
    }
    else
    {
        cmd[0] = 0;
        for (i = 2; i < c; i++)
        {
            if (i > 2)
                strcat(cmd, " ");
            strcat(cmd, Cmd_Argv(i));
        }
    }

    Key_SetBinding(b, cmd);
}

int Key_CountBindings(void)
{
    int i;
    int n;

    n = 0;
    for (i = 0; i < MAX_KEYS; i++)
    {
        if (keybindings[i] && keybindings[i][0])
            n++;
    }

    return n;
}

void Key_WriteBindings(FileHandle_t f)
{
    int i;

    for (i = 0; i < MAX_KEYS; i++)
    {
        if (keybindings[i])
        {
            if (*keybindings[i])
                FS_FPrintf(f, "bind \"%s\" \"%s\"\r\n", Key_KeynumToString(i), keybindings[i]);
        }
    }
}

void Key_Init(void)
{
    int i;

    for (i = 0; i < 32; i++)
    {
        key_lines[i][0] = ']';
        key_lines[i][1] = 0;
    }
    key_linepos = 1;

    for (i = 32; i < 128; i++)
        consolekeys[i] = true;
    consolekeys[K_ENTER] = true;
    consolekeys[K_TAB] = true;
    consolekeys[K_LEFTARROW] = true;
    consolekeys[K_RIGHTARROW] = true;
    consolekeys[K_UPARROW] = true;
    consolekeys[K_DOWNARROW] = true;
    consolekeys[K_BACKSPACE] = true;
    consolekeys[K_PGUP] = true;
    consolekeys[K_PGDN] = true;
    consolekeys[K_HOME] = true;
    consolekeys[K_END] = true;
    consolekeys[K_SHIFT] = true;
    consolekeys[K_ALT] = true;
    consolekeys[K_SPACE] = true;
    consolekeys[K_CTRL] = true;
    consolekeys['`'] = false;
    consolekeys['~'] = false;

    for (i = 0; i < MAX_KEYS; i++)
        keyshift[i] = i;
    for (i = 'a'; i <= 'z'; i++)
        keyshift[i] = i - 'a' + 'A';
    keyshift['1'] = '!';
    keyshift['2'] = '@';
    keyshift['3'] = '#';
    keyshift['4'] = '$';
    keyshift['5'] = '%';
    keyshift['6'] = '^';
    keyshift['7'] = '&';
    keyshift['8'] = '*';
    keyshift['9'] = '(';
    keyshift['0'] = ')';
    keyshift['-'] = '_';
    keyshift['='] = '+';
    keyshift[','] = '<';
    keyshift['.'] = '>';
    keyshift['/'] = '?';
    keyshift[';'] = ':';
    keyshift['\''] = '"';
    keyshift['['] = '{';
    keyshift[']'] = '}';
    keyshift['`'] = '~';
    keyshift['\\'] = '|';

    menubound[K_ESCAPE] = true;
    for (i = 0; i < 12; i++)
        menubound[K_F1 + i] = true;

    Cmd_AddCommand("bind", Key_Bind_f);
    Cmd_AddCommand("unbind", Key_Unbind_f);
    Cmd_AddCommand("unbindall", Key_Unbindall_f);
    Cmd_AddCommand("escape", Key_Escape_f);
}

#ifdef NON_MATCHING
void Key_Event(int key, qboolean down)
{
    char *kb, *p, *semi;
    char cmd[1024];
    static float lastrepeat = 0;

    keydown[key] = down;

    if (!down)
        key_repeats[key] = 0;

    key_lastpress = key;

    if (down)
    {
        key_repeats[key]++;
        if (key_dest != key_unk4 && key_dest != key_unk5 && key != K_BACKSPACE && key != K_PAUSE && key_repeats[key] > 1)
            return;

        if (key_repeats[key] == 1)
        {
            key_repeatdelay = 0.5f;
            lastrepeat = Sys_FloatTime();
        }
        else
        {
            if (Sys_FloatTime() < lastrepeat + key_repeatdelay)
            {
                key_repeats[key]--;
                return;
            }
            key_repeatdelay = 0.2f;
            lastrepeat = Sys_FloatTime();
        }
    }

    if (key == K_SHIFT)
        shift_down = down;

    if (!down)
    {
        kb = keybindings[key];
        if (kb)
        {
            do
            {
                p = kb;
                semi = strchr(p, ';');
                if (semi)
                {
                    *semi = 0;
                    kb = semi + 1;
                }
                else
                {
                    kb = NULL;
                }

                if (*p == '+')
                {
                    sprintf(cmd, "-%s %i\n", p + 1, key);
                    Cbuf_AddText(cmd);
                }

                if (semi)
                    *semi = ';';
            } while (kb);
        }

        if (keyshift[key] != key)
        {
            kb = keybindings[keyshift[key]];
            if (kb && kb[0] == '+')
            {
                sprintf(cmd, "-%s %i\n", kb + 1, key);
                Cbuf_AddText(cmd);
            }
        }
        return;
    }

    if ((D_00399798 & 0x10) && key != K_ESCAPE)
        return;

    if ((key_dest == key_menu && menubound[key])
        || (key_dest == key_console && !consolekeys[key])
        || (key_dest == key_game && (!con_forcedup || !consolekeys[key])))
    {
        kb = keybindings[key];
        if (kb)
        {
            do
            {
                p = kb;
                semi = strchr(p, ';');
                if (semi)
                {
                    *semi = 0;
                    kb = semi + 1;
                }
                else
                {
                    kb = NULL;
                }

                if (*p == '+')
                {
                    sprintf(cmd, "%s %i\n", p, key);
                    Cbuf_AddText(cmd);
                }
                else
                {
                    Cbuf_AddText(p);
                    Cbuf_AddText("\n");
                }

                if (semi)
                    *semi = ';';
            } while (kb);
        }
        return;
    }

    if (!down)
        return;

    if (shift_down)
        key = keyshift[key];

    switch (key_dest)
    {
        case key_message:
            Key_Message(key);
            break;
        case key_game:
        case key_console:
            Key_Console(key);
            break;
        case key_unk4:
        case key_unk5:
            func_00225610(key);
            break;
        default:
            Sys_Error("Bad key_dest");
    }
}
#else
float key_lastrepeat = 0;
INCLUDE_ASM("asm/nonmatchings/engine/keys", Key_Event);
#endif

void Key_ClearStates(void)
{
    int i;

    for (i = 0; i < MAX_KEYS; i++)
    {
        keydown[i] = false;
        key_repeats[i] = 0;
    }
}
