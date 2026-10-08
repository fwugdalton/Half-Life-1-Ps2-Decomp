#ifndef ENGINE_CMD_H
#define ENGINE_CMD_H

#include "common.h"

typedef void (*xcommand_t)(void);

typedef struct sizebuf_s
{
    const char *buffername;
    unsigned short flags;
    byte *data;
    int maxsize;
    int cursize;
} sizebuf_t;

typedef struct cmd_function_s
{
    struct cmd_function_s *next;
    char *name;
    xcommand_t function;
    int flags;
} cmd_function_t;

#define MAX_ALIAS_NAME 32

typedef struct cmdalias_s
{
    struct cmdalias_s *next;
    char name[MAX_ALIAS_NAME];
    char *value;
} cmdalias_t;

typedef enum
{
    src_client,
    src_command
} cmd_source_t;

#define MAX_ARGS 80

/* common.c */
extern int com_argc;
extern char **com_argv;
extern char com_token[];
int Q_strlen(const char *str);
void Q_strcpy(char *dest, const char *src);
void Q_strcat(char *dest, const char *src);
int Q_strcmp(const char *s1, const char *s2);
int Q_strncmp(const char *s1, const char *s2, int count);
int Q_strncasecmp(const char *s1, const char *s2, int n);
int Q_strcasecmp(const char *s1, const char *s2);
char *COM_Parse(char *data);
byte *COM_LoadHunkFile(char *path);
void SZ_Alloc(sizebuf_t *buf, int startsize);
void SZ_Clear(sizebuf_t *buf);
void SZ_Write(sizebuf_t *buf, const void *data, int length);
void SZ_Print(sizebuf_t *buf, const char *data);
void MSG_WriteByte(sizebuf_t *sb, int c);

/* zone.c */
void *Z_Malloc(int size);
void Z_Free(void *ptr);
int Hunk_LowMark(void);
void Hunk_FreeToLowMark(int mark);

/* Gbx */
void *Mem_DbgMalloc(int size, const char *file, int line);
void Mem_Free(void *ptr);

/* console / sys */
void Con_Printf(const char *fmt, ...);
void Con_DPrintf(const char *fmt, ...);
void Sys_Error(const char *error, ...);


/* libc */
unsigned int strlen(const char *);
char *strcpy(char *, const char *);
char *strcat(char *, const char *);
int strcmp(const char *, const char *);
char *strstr(const char *, const char *);
int strcasecmp(const char *, const char *);
void *memcpy(void *, const void *, unsigned int);
void *memset(void *, int, unsigned int);

/* host */
extern qboolean host_initialized;

#define clc_stringcmd 4

typedef enum
{
    ca_dedicated,
    ca_disconnected,
    ca_connecting,
    ca_connected,
    ca_uninitialized,
    ca_active
} cactive_t;

/* client_static_t (parcial; offsets vistos no binário). Há um por jogador (tela dividida). */
#define MAX_INFO_STRING 196

/* netchan_t (parcial) */
typedef struct netchan_s
{
    byte unk_00[0x50];
    sizebuf_t message;          /* 0x50 */
} netchan_t;

typedef struct client_static_s
{
    cactive_t state;            /* 0x0000 */
    netchan_t netchan;          /* 0x0004 */
    byte unk_0068[0x73C8 - 0x68];
    int signon;                 /* 0x73C8 */
    byte unk_73CC[0x7D14 - 0x73CC];
    char userinfo[MAX_INFO_STRING]; /* 0x7D14 */
    byte unk_7DD8[0x7F18 - 0x7DD8];
} client_static_t;              /* 0x7F18 */

extern client_static_t cls[2];
extern int cls_demoplayback;

/* cmd.c */
void Cbuf_Init(void);
void Cbuf_AddText(char *text);
void Cbuf_InsertText(char *text);
void Cbuf_InsertTextLines(char *text);
void Cbuf_Execute(void);
void Cmd_Init(void);
int Cmd_Argc(void);
char *Cmd_Argv(int arg);
char *Cmd_Args(void);
void Cmd_TokenizeString(char *text);
void Cmd_AddCommand(char *cmd_name, xcommand_t function);
void Cmd_AddMallocCommand(char *cmd_name, xcommand_t function);
void Cmd_RemoveMallocedCmds(void);
qboolean Cmd_Exists(char *cmd_name);
char *Cmd_CompleteCommand(char *partial);
void Cmd_ExecuteString(char *text, cmd_source_t src);
void Cmd_ForwardToServer(void);
void Cmd_ForwardToServerUnreliable(void);
int Cmd_CheckParm(char *parm);
void Cmd_CmdList_f(void);

#endif /* ENGINE_CMD_H */
