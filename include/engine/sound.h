#ifndef ENGINE_SOUND_H
#define ENGINE_SOUND_H

#include "common.h"
#include "engine/mathlib.h"

/* estruturas de som do HL1 PS2 (snd_dma_gbx.c, snd_mem.c, snd_mix_gbx.c, snd_win_gbx.c) */

typedef struct
{
    int rate;
    int width;
    int channels;
    int loopstart;
    int samples;
    int dataofs; /* chunk starts this many bytes from file start */
} wavinfo_t;

typedef struct
{
    int length;
    int loopstart;
    int speed;
    int width;
    int stereo;
    byte data[1]; /* tamanho variável */
} sfxcache_t;

typedef struct sfx_s
{
    char name[64];
    int servercount;            /* 0x40: 0 = global, 1 = jogo, 2 = fase */
    char streamed;              /* 0x44 */
    cache_user_t cache;         /* 0x48 */
    int unk_4C;                 /* servercount do cliente quando usado */
} sfx_t;                        /* 0x50 */

typedef struct
{
    sfx_t *sfx;                 /* 0x00 */
    int leftvol;                /* 0x04 */
    int rightvol;               /* 0x08 */
    int end;                    /* 0x0C */
    int pos;                    /* 0x10 */
    int looping;                /* 0x14 */
    int entnum;                 /* 0x18 */
    int entchannel;             /* 0x1C */
    vec3_t origin;              /* 0x20 */
    float dist_mult;            /* 0x2C */
    int master_vol;             /* 0x30 */
    int isentence;              /* 0x34 */
    int iword;                  /* 0x38 */
    int pitch;                  /* 0x3C */
    char unk_40;                /* 1 = stream */
    byte unk_41;
    byte unk_42;
    byte unk_43;
    byte unk_44[0x4C - 0x44];
} channel_t;                    /* 0x4C */

typedef struct
{
    int pos;          /* 0x00 */
    int len;          /* 0x04 */
    wavinfo_t info;   /* 0x08 */
    int unk_20;
    int unk_24;
    byte *buffer;     /* 0x28 */
    int unk_2C;
    int unk_30;
    int unk_34;
    int unk_38;
    int unk_3C;
} stream_t;                     /* 0x40 */

typedef struct dma_s
{
    qboolean gamealive;         /* 0x00 */
    qboolean soundalive;        /* 0x04 */
    qboolean splitbuffer;       /* 0x08 */
    int channels;               /* 0x0C */
    int samples;                /* 0x10 */
    int submission_chunk;       /* 0x14 */
    int samplepos;              /* 0x18 */
    int samplebits;             /* 0x1C */
    int speed;                  /* 0x20 */
    int dmaspeed;               /* 0x24 */
    unsigned char *buffer;      /* 0x28 */
} dma_t;

#define MAX_SFX 512
#define MAX_CHANNELS 128
#define MAX_DYNAMIC_CHANNELS 12
#define NUM_AMBIENTS 4

#define CHAN_AUTO 0
#define CHAN_VOICE 2
#define CHAN_STREAM 5

extern channel_t channels[MAX_CHANNELS];
extern stream_t D_00553DE8[];

#endif /* ENGINE_SOUND_H */
