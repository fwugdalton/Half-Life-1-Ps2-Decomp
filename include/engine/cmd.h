#ifndef ENGINE_CMD_H
#define ENGINE_CMD_H

#include "common.h"

typedef void (*xcommand_t)(void);

typedef struct sizebuf_s
{
    qboolean allowoverflow;     /* se falso, estourar é erro fatal */
    qboolean overflowed;        /* setado quando o buffer enche */
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
int strncmp(const char *, const char *, unsigned int);
char *strncpy(char *, const char *, unsigned int);
int sprintf(char *, const char *, ...);
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

typedef struct netadr_s
{
    int type;
    byte ip[4];
} netadr_t;

#define MAX_MSGLEN 14100
#define MAX_LATENT 32
#define MAX_LATENCY 64

typedef struct flowstats_s
{
    int size;
    float time;
} flowstats_t;

typedef struct flow_s
{
    flowstats_t stats[MAX_LATENT];
    int current;                /* 0x100 */
    float nextcompute;          /* 0x104 */
    float kbytespersec;         /* 0x108 */
    float avgkbytespersec;      /* 0x10C */
} flow_t;                       /* 0x110 */

#define FLOW_OUTGOING 0
#define FLOW_INCOMING 1

typedef struct netlatency_s
{
    int latency;
    float rate;
} netlatency_t;

typedef struct netchan_s
{
    netadr_t remote_address;    /* 0x00 */
    int sock;                   /* 0x08 */
    qboolean fatal_error;       /* 0x0C */
    float last_received;        /* 0x10 */
    float connect_time;         /* 0x14 */
    float frame_latency;        /* 0x18 */
    float frame_rate;           /* 0x1C */
    int drop_count;             /* 0x20 */
    int good_count;             /* 0x24 */
    int unk_28;                 /* 0x28 */
    float cleartime;            /* 0x2C */
    float rate;                 /* 0x30 */
    int incoming_sequence;      /* 0x34 */
    int incoming_acknowledged;  /* 0x38 */
    int incoming_reliable_acknowledged; /* 0x3C */
    int incoming_reliable_sequence; /* 0x40 */
    int outgoing_sequence;      /* 0x44 */
    int reliable_sequence;      /* 0x48 */
    int last_reliable_sequence; /* 0x4C */
    sizebuf_t message;          /* 0x50 */
    byte message_buf[MAX_MSGLEN]; /* 0x64 */
    int reliable_length;        /* 0x3778 */
    byte reliable_buf[MAX_MSGLEN]; /* 0x377C */
    int outgoing_size[MAX_LATENT]; /* 0x6E90 */
    float outgoing_time[MAX_LATENT]; /* 0x6F10 */
    netlatency_t latency[MAX_LATENCY]; /* 0x6F90 */
    flow_t flow[2];             /* 0x7190 */
} netchan_t;                    /* 0x73B0 */

typedef struct downloadtime_s
{
    qboolean bUsed;
    float fTime;
    int nBytesRemaining;
} downloadtime_t;

typedef struct soundfade_s
{
    int nStartPercent;          /* 0x00 */
    int nClientSoundFadePercent; /* 0x04 */
    float soundFadeStartTime;   /* 0x08 */
    int soundFadeOutTime;       /* 0x0C */
    int soundFadeHoldTime;      /* 0x10 */
    int soundFadeInTime;        /* 0x14 */
} soundfade_t; /* 0x18 */

typedef struct client_static_s
{
    cactive_t state;            /* 0x0000 */
    netchan_t netchan;          /* 0x0004 */
    byte unk_73B4[0x73C8 - 0x73B4];
    int signon;                 /* 0x73C8 */
    byte unk_73CC[0x74D0 - 0x73CC];
    qboolean demoplayback;      /* 0x74D0 */
    char mapstring[64];         /* 0x74D4 */
    char spawnparms[2048];      /* 0x7514 */
    char userinfo[MAX_INFO_STRING]; /* 0x7D14 */
    byte unk_7DD8[0x7DDC - 0x7DD8];
    int unk_7DDC;               /* 0x7DDC */
    int unk_7DE0;               /* 0x7DE0 */
    qboolean dl_doneregistering; /* 0x7DE4 */
    byte unk_7DE8[0x7E78 - 0x7DE8];
    /* incomingtransfer_t (download de recursos) */
    int dl_unk_7E78;            /* 0x7E78 */
    int dl_unk_7E7C;            /* 0x7E7C */
    int dl_nTotalToTransfer;    /* 0x7E80 */
    int dl_nRemainingToTransfer; /* 0x7E84 */
    float dl_fLastStatusUpdate; /* 0x7E88 */
    float dl_fLastStatTime;     /* 0x7E8C */
    downloadtime_t dl_rgStats[8]; /* 0x7E90 */
    int dl_nCurStat;            /* 0x7EF0 */
    int dl_unk_7EF4;            /* 0x7EF4 */
    qboolean dl_custom;         /* 0x7EF8 */
    float latency;              /* 0x7EFC */
    soundfade_t soundfade;      /* 0x7F00 */
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
