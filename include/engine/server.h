#ifndef ENGINE_SERVER_H
#define ENGINE_SERVER_H

#include "common.h"
#include "engine/cmd.h"

typedef void *FileHandle_t;

typedef int string_t;

typedef struct link_s
{
    struct link_s *prev;
    struct link_s *next;
} link_t;

struct edict_s;

/* entvars_t do PS2: igual ao do PC sem endpos/startpos/impacttime/starttime
   (0x20 bytes a menos antes de mins); depois de flags ainda parcial */
typedef struct entvars_s
{
    string_t classname;         /* 0x000 */
    string_t globalname;        /* 0x004 */
    float origin[3];            /* 0x008 */
    float oldorigin[3];         /* 0x014 */
    float velocity[3];          /* 0x020 */
    float basevelocity[3];      /* 0x02C */
    float clbasevelocity[3];    /* 0x038 */
    float movedir[3];           /* 0x044 */
    float angles[3];            /* 0x050 */
    float avelocity[3];         /* 0x05C */
    float punchangle[3];        /* 0x068 */
    float v_angle[3];           /* 0x074 */
    int fixangle;               /* 0x080 */
    float idealpitch;           /* 0x084 */
    float pitch_speed;          /* 0x088 */
    float ideal_yaw;            /* 0x08C */
    float yaw_speed;            /* 0x090 */
    int modelindex;             /* 0x094 */
    string_t model;             /* 0x098 */
    int viewmodel;              /* 0x09C */
    int weaponmodel;            /* 0x0A0 */
    float absmin[3];            /* 0x0A4 */
    float absmax[3];            /* 0x0B0 */
    float mins[3];              /* 0x0BC */
    float maxs[3];              /* 0x0C8 */
    float size[3];              /* 0x0D4 */
    float ltime;                /* 0x0E0 */
    float nextthink;            /* 0x0E4 */
    int movetype;               /* 0x0E8 */
    int solid;                  /* 0x0EC */
    int skin;                   /* 0x0F0 */
    int body;                   /* 0x0F4 */
    int effects;                /* 0x0F8 */
    float gravity;              /* 0x0FC */
    float friction;             /* 0x100 */
    int light_level;            /* 0x104 */
    int sequence;               /* 0x108 */
    int gaitsequence;           /* 0x10C */
    float frame;                /* 0x110 */
    float animtime;             /* 0x114 */
    float framerate;            /* 0x118 */
    byte controller[4];         /* 0x11C */
    byte blending[2];           /* 0x120 */
    float scale;                /* 0x124 */
    int rendermode;             /* 0x128 */
    float renderamt;            /* 0x12C */
    float rendercolor[3];       /* 0x130 */
    int renderfx;               /* 0x13C */
    float health;               /* 0x140 */
    float frags;                /* 0x144 */
    int weapons;                /* 0x148 */
    float takedamage;           /* 0x14C */
    int deadflag;               /* 0x150 */
    float view_ofs[3];          /* 0x154 */
    int button;                 /* 0x160 */
    int impulse;                /* 0x164 */
    struct edict_s *chain;      /* 0x168 */
    struct edict_s *dmg_inflictor; /* 0x16C */
    struct edict_s *enemy;      /* 0x170 */
    struct edict_s *aiment;     /* 0x174 */
    struct edict_s *owner;      /* 0x178 */
    struct edict_s *groundentity; /* 0x17C */
    int spawnflags;             /* 0x180 */
    int flags;                  /* 0x184 */
    int colormap;               /* 0x188 */
    int team;                   /* 0x18C */
    float max_health;           /* 0x190 */
    float teleport_time;        /* 0x194 */
    float armortype;            /* 0x198 */
    float armorvalue;           /* 0x19C */
    int waterlevel;             /* 0x1A0 */
    int watertype;              /* 0x1A4 */
    string_t target;            /* 0x1A8 */
    string_t targetname;        /* 0x1AC */
    string_t netname;           /* 0x1B0 */
    string_t message;           /* 0x1B4 */
    float dmg_take;             /* 0x1B8 */
    float dmg_save;             /* 0x1BC */
    float dmg;                  /* 0x1C0 */
    float dmgtime;              /* 0x1C4 */
    string_t noise;             /* 0x1C8 */
    string_t noise1;            /* 0x1CC */
    string_t noise2;            /* 0x1D0 */
    string_t noise3;            /* 0x1D4 */
    float speed;                /* 0x1D8 */
    float air_finished;         /* 0x1DC */
    float pain_finished;        /* 0x1E0 */
    float radsuit_finished;     /* 0x1E4 */
    struct edict_s *pContainingEntity; /* 0x1E8 */
} entvars_t; /* 0x1EC: acaba em pContainingEntity (sem playerclass..euser4) */

/* parcial: só os campos já identificados */
typedef struct edict_s
{
    qboolean free;              /* 0x00 */
    int serialnumber;           /* 0x04 */
    link_t area;                /* 0x08 */
    byte unk_10[0xB4 - 0x10];
    void *pvPrivateData;        /* 0xB4 */
    entvars_t v;                /* 0xB8 */
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
    qboolean sendinfo;          /* 0x84E4 */
    float sendinfo_time;        /* 0x84E8 */
    byte unk_84EC[0x850D - 0x84EC];
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
    byte unk_00008[0x0C - 0x08];
    float time;                 /* 0x0000C */
    int lastcheck;              /* 0x00010 */
    float lastchecktime;        /* 0x00014 */
    char name[64];              /* 0x00018 */
    byte unk_00058[0xD8 - 0x58];
    struct model_s *worldmodel; /* 0x000D8 */
    byte unk_000DC[0x190E8 - 0xDC];
    char *model_precache[512];  /* 0x190E8 */
    struct model_s *models[512]; /* 0x198E8 */
    char *sound_precache[512];  /* 0x1A0E8 */
    short sound_precache_hashedlookup[1023]; /* 0x1A8E8 */
    qboolean sound_precache_hashedlookup_built; /* 0x1B0E8 */
    char *generic_precache[512]; /* 0x1B0EC */
    char *lightstyles[64];      /* 0x1B8EC */
    int num_edicts;             /* 0x1B9EC */
    int max_edicts;             /* 0x1B9F0 */
    edict_t *edicts;            /* 0x1B9F4 */
    int state;                  /* 0x1B9F8 */
    sizebuf_t datagram;         /* 0x1B9FC */
    byte datagram_buf[4000];    /* 0x1BA10 */
    sizebuf_t reliable_datagram; /* 0x1C9B0 */
    byte unk_1C9C4[0x1ED2C - 0x1C9C4];
    sizebuf_t signon;           /* 0x1ED2C */
    byte unk_1ED40[0x2E784 - 0x1ED40];
} server_t; /* 0x2E784 */

extern server_t sv;
extern server_static_t svs;
extern client_t *host_client;

#endif /* ENGINE_SERVER_H */
