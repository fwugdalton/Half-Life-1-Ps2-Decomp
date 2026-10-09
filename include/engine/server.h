#ifndef ENGINE_SERVER_H
#define ENGINE_SERVER_H

#include "common.h"
#include "engine/cmd.h"

typedef void *FileHandle_t;

/* parcial: só os campos já identificados */
typedef struct edict_s
{
    qboolean free;
    byte unk_04[0xD8 - 0x04];
    float velocity[3];          /* 0x0D8 (v.velocity) */
    byte unk_E4[0x23C - 0xE4];
    int flags;                  /* 0x23C (v.flags) */
    byte unk_240[0x2A4 - 0x240];
} edict_t; /* 0x2A4 */

#define FL_SPECTATOR (1 << 26)

typedef struct
{
    float normal[3];
    float dist;
} plane_t;

typedef struct
{
    qboolean allsolid;
    qboolean startsolid;
    qboolean inopen;
    qboolean inwater;
    float fraction;
    float endpos[3];
    plane_t plane;
    edict_t *ent;               /* 0x30 */
    int hitgroup;               /* 0x34 */
} trace_t; /* 0x38 */

typedef struct resource_s
{
    byte unk_00[0x48];
    struct resource_s *pNext; /* 0x48 */
    struct resource_s *pPrev; /* 0x4C */
} resource_t; /* 0x50 */

typedef struct client_s
{
    qboolean active;            /* 0x0000 */
    qboolean spawned;           /* 0x0004 */
    qboolean connected;         /* 0x0008 */
    byte unk_000C[0x14 - 0x0C];
    netchan_t netchan;          /* 0x0014 */
    byte unk_73C4[0x73D8 - 0x73C4];
    qboolean spectator;         /* 0x73D8 */
    qboolean fakeclient;        /* 0x73DC */
    byte unk_73E0[0x7444 - 0x73E0];
    int unk_7444;               /* 0x7444 */
    byte unk_7448[0x8408 - 0x7448];
    float connection_started;   /* 0x8408 */
    byte unk_840C[0x8410 - 0x840C];
    edict_t *edict;             /* 0x8410 */
    byte unk_8414[0x8420 - 0x8414];
    char userinfo[MAX_INFO_STRING]; /* 0x8420 */
    byte unk_84E4[0x850D - 0x84E4];
    char name[32];              /* 0x850D */
    byte unk_852D[0x853C - 0x852D];
    void *unk_853C;             /* 0x853C */
    byte unk_8540[0x8558 - 0x8540];
    resource_t resourcesneeded; /* 0x8558 */
    resource_t resourcesonhand; /* 0x85A8 */
    int unk_85F8;               /* 0x85F8 */
    byte unk_85FC[0x8778 - 0x85FC];
} client_t; /* 0x8778 */

typedef struct server_log_s
{
    qboolean active;            /* 0x118 */
    qboolean net_log;           /* 0x11C */
    netadr_t net_address;       /* 0x120 */
    FileHandle_t file;          /* 0x128 */
} server_log_t;

typedef struct server_static_s
{
    int maxclients;             /* 0x000 */
    int maxclientslimit;        /* 0x004 */
    byte unk_008[0x110 - 0x008];
    client_t *clients;          /* 0x110 */
    byte unk_114[0x118 - 0x114];
    server_log_t log;           /* 0x118 */
} server_static_t;

typedef struct server_s
{
    qboolean active;            /* 0x00000 */
    qboolean paused;            /* 0x00004 */
    byte unk_00008[0x18 - 0x08];
    char name[64];              /* 0x00018 */
    byte unk_00058[0x1B9EC - 0x58];
    int num_edicts;             /* 0x1B9EC */
    int max_edicts;             /* 0x1B9F0 */
    edict_t *edicts;            /* 0x1B9F4 */
    byte unk_1B9F8[0x1C9B0 - 0x1B9F8];
    sizebuf_t reliable_datagram; /* 0x1C9B0 */
    byte unk_1C9C4[0x2E784 - 0x1C9C4];
} server_t; /* 0x2E784 */

extern server_t sv;
extern server_static_t svs;
extern client_t *host_client;

#endif /* ENGINE_SERVER_H */
