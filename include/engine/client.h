#ifndef ENGINE_CLIENT_H
#define ENGINE_CLIENT_H

#include "engine/mathlib.h"
#include "engine/cl_entity.h"

/* client_state_t do PS2: um por jogador local (tela dividida), cl[clientId], 0x212C bytes.
   Layout parcial: só os campos já identificados (outros arquivos ainda têm versões locais). */
typedef struct screenfade_s
{
    float fadeSpeed;            /* 0x00 */
    float fadeEnd;              /* 0x04 */
    float fadeReset;            /* 0x08 */
    byte fader, fadeg, fadeb, fadealpha; /* 0x0C */
    int fadeFlags;              /* 0x10 */
} screenfade_t; /* 0x14 */

#define FFADE_IN 0x0000
#define FFADE_OUT 0x0001
#define FFADE_MODULATE 0x0002
#define FFADE_STAYOUT 0x0004

/* estado de jogador guardado no frame (layout parcial) */
typedef struct player_state_s
{
    byte pad0[0x08];
    float msg_time;             /* 0x08 */
    byte pad0C[0x30 - 0x0C];
    byte number;                /* 0x30 */
    byte pad31[0x4C - 0x31];
    vec3_t origin;              /* 0x4C */
    vec3_t viewangles;          /* 0x58 */
    vec3_t velocity;            /* 0x64 */
    byte pad70[0x98 - 0x70];
    int renderframe;            /* 0x98 */
    byte pad9C[0xC4 - 0x9C];
    int weaponmodel;            /* 0xC4 */
    int gaitsequence;           /* 0xC8 */
    byte padCC[0xF8 - 0xCC];
} player_state_t; /* 0xF8 */

/* frame_t do PS2 (0x330), cl.frames[cl_updatebackup] */
typedef struct frame_s
{
    byte pad0[0x24];
    float senttime;             /* 0x24 */
    byte pad28[0x2C - 0x28];
    float receivedtime;         /* 0x2C */
    byte pad30[0x34 - 0x30];
    player_state_t playerstate[3]; /* 0x34 (MAX_CLIENTS) */
    byte pad31C[0x324 - 0x31C];
    short unk_324;              /* 0x324 */
    short unk_326;              /* 0x326 */
    short unk_328;              /* 0x328 */
    short unk_32A;              /* 0x32A */
    short unk_32C;              /* 0x32C */
    short pad32E;
} frame_t; /* 0x330 */

typedef struct player_info_s
{
    int userid;                 /* 0x000 */
    char userinfo[196];         /* 0x004 */
    char name[32];              /* 0x0C8 */
    int unk_E8;                 /* 0x0E8 */
    int spectator;              /* 0x0EC */
    char model[64];             /* 0x0F0 */
    int topcolor;               /* 0x130 */
    int bottomcolor;            /* 0x134 */
    byte pad138[0x340 - 0x138];
    float maxspeed;             /* 0x340 */
    byte customdata[0x3B0 - 0x344]; /* 0x344: customization_t (cabeça da lista) */
    int gaitsequence;           /* 0x3B0 */
    float gaitframe;            /* 0x3B4 */
    float gaityaw;              /* 0x3B8 */
    vec3_t prevgaitorigin;      /* 0x3BC */
} player_info_t; /* 0x3C8 */

#define MAX_CLIENTS 3
#define MAX_CL_STATS 32
#define STAT_HEALTH 0
#define STAT_WEAPON 2

typedef struct client_state_s
{
    int max_edicts;             /* 0x000 */
    int num_resources;          /* 0x004 */
    byte pad8[0x208 - 0x8];
    int servercount;            /* 0x208 */
    int validsequence;          /* 0x20C */
    int parsecount;             /* 0x210 */
    byte pad214[0x224 - 0x214];
    float forwardmove;          /* 0x224: cmd.forwardmove */
    byte pad228[0x238 - 0x228];
    int stats[MAX_CL_STATS];    /* 0x238 */
    int items;                  /* 0x2B8 */
    byte pad2BC[0x2DC - 0x2BC];
    vec3_t viewangles;          /* 0x2DC */
    vec3_t mvelocity[2];        /* 0x2E8: [0] atual, [1] anterior */
    byte pad300[0x30C - 0x300];
    vec3_t punchangle;          /* 0x30C */
    vec3_t crosshairangle;      /* 0x318 */
    byte pad324[0x328 - 0x324];
    vec3_t simorg;              /* 0x328 */
    vec3_t simvel;              /* 0x334 */
    vec3_t simangles;           /* 0x340 */
    float idealpitch;           /* 0x34C */
    float pitchvel;             /* 0x350 */
    qboolean nodrift;           /* 0x354 */
    float driftmove;            /* 0x358 */
    float laststop;             /* 0x35C */
    float viewheight;           /* 0x360 */
    byte pad364[0x368 - 0x364];
    screenfade_t sf;            /* 0x368 */
    qboolean unk_37C;           /* 0x37C: congela a câmera (V_RenderView) */
    qboolean onground;          /* 0x380 */
    qboolean inwater;           /* 0x384 */
    int waterlevel;             /* 0x388 */
    int light_level;            /* 0x38C */
    int intermission;           /* 0x390 */
    int completed_time;         /* 0x394 */
    float mtime[2];             /* 0x398 */
    float time;                 /* 0x3A0 */
    float oldtime;              /* 0x3A4 */
    byte pad3A8[0x3AC - 0x3A8];
    frame_t *frames;     /* 0x3AC: frame_t[cl_updatebackup] (0x330 cada) */
    int playernum;              /* 0x3B0 */
    qboolean spectator;         /* 0x3B4 */
    struct model_s *model_precache[512]; /* 0x3B8 */
    struct sfx_s *sound_precache[512]; /* 0xBB8 */
    char levelname[40];         /* 0x13B8 */
    int maxclients;             /* 0x13E0 */
    int gametype;               /* 0x13E4 */
    int viewentity;             /* 0x13E8 */
    struct model_s *worldmodel; /* 0x13EC */
    struct efrag_s *free_efrags; /* 0x13F0 */
    int num_entities;           /* 0x13F4 */
    byte pad13F8[0x13FC - 0x13F8];
    cl_entity_t viewent;        /* 0x13FC */
    int cdtrack;                /* 0x159C */
    int looptrack;              /* 0x15A0 */
    int unk_15A4;               /* 0x15A4 */
    int unk_15A8;               /* 0x15A8 */
    float weaponstarttime;      /* 0x15AC */
    int weaponsequence;         /* 0x15B0 */
    byte pad15B4[0x15C4 - 0x15B4];
    player_info_t players[MAX_CLIENTS]; /* 0x15C4 */
    vec3_t viewattachment;      /* 0x211C: último attachment do viewmodel */
    int unk2128;                /* 0x2128: entidade de câmera (comparada com sv.num_edicts) */
} client_state_t; /* 0x212C */

extern client_state_t cl[2];
extern int cl_numlocal;

#endif
