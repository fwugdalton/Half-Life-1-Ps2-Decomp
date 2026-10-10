#include "common.h"

typedef struct
{
    int rate;
    int width;
    int channels;
    int loopstart;
    int samples;
    int dataofs; /* chunk starts this many bytes from file start */
} wavinfo_t;

extern byte *data_p;
extern byte *iff_end;
extern byte *last_chunk;
extern byte *iff_data;
extern int iff_chunk_len;

void Con_Printf(char *fmt, ...);
void Con_DPrintf(char *fmt, ...);
void Sys_Error(char *error, ...);
int Q_strncmp(char *s1, char *s2, int count);
int strncmp(const char *s1, const char *s2, unsigned int n);
void *memset(void *s, int c, unsigned int n);
void *memcpy(void *d, const void *s, unsigned int n);

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
    int servercount;  /* 0x40 */
    char streamed;    /* 0x44 */
    cache_user_t cache; /* 0x48 */
} sfx_t;

typedef struct
{
    sfx_t *sfx; /* 0x00 */
    byte unk_04[0x40 - 0x04];
    byte unk_40;
    byte unk_41;
    byte unk_42;
    byte unk_43;
    byte unk_44[0x4C - 0x44];
} channel_t;

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
} stream_t;

typedef struct
{
    qboolean gamealive;
    qboolean soundalive;
    qboolean splitbuffer;
    int channels;
    int samples;
    int submission_chunk;
    int samplepos;
    int samplebits;
    int speed; /* 0x20 */
} dma_t;

#include "engine/cvar.h"

extern volatile dma_t *shm;
extern cvar_t hisound;
extern cvar_t loadas8bit;
extern short (*LittleShort)(short l);
extern int com_filesize;
extern channel_t channels[];
extern stream_t D_00553DE8[];
extern int D_00557634;

void *Cache_Check(cache_user_t *c);
void *Cache_Alloc(cache_user_t *c, int size, char *name);
byte *COM_LoadStackFile(char *path, void *buffer, int bufsize);
void Q_strcpy(char *dest, char *src);
void Q_strcat(char *dest, char *src);
char *strcat(char *d, const char *s);
int func_002CFF88(char *name, int *a, int *b, int *c);
byte *func_00225D48(int size, int align, char *file, int line);
byte func_002388A0(int a, int b, byte *c, byte *d);
wavinfo_t GetWavinfo(char *name, byte *wav, int wavlength);
sfxcache_t *S_LoadStreamSound(sfx_t *s, channel_t *ch);

void ResampleSfx(sfx_t *sfx, int inrate, int inwidth, byte *data)
{
    int outcount;
    int srcsample;
    float stepscale;
    int i;
    int sample, samplefrac, fracstep;
    sfxcache_t *sc;

    if (sfx->streamed == 1)
        sc = sfx->cache.data;
    else
    {
        sc = Cache_Check(&sfx->cache);
        if (!sc)
            return;
    }

    stepscale = (float)inrate / shm->speed; /* isto é usado para reamostrar */

    if (stepscale == 2 && hisound.value)
    {
        stepscale = 1;
        outcount = sc->length;
    }
    else
    {
        outcount = sc->length / stepscale;
        sc->length = outcount;
        if (sc->loopstart != -1)
            sc->loopstart = sc->loopstart / stepscale;
        sc->speed = shm->speed;
    }

    if (loadas8bit.value)
        sc->width = 1;
    else
        sc->width = inwidth;
    sc->stereo = 0;

    /* reamostra / decima até a taxa atual */
    if (stepscale == 1 && inwidth == 1 && sc->width == 1)
    {
        /* caso rápido especial */
        for (i = 0; i < outcount; i++)
            ((signed char *)sc->data)[i] = (int)((unsigned char)(data[i]) - 128);
    }
    else
    {
        /* caso geral */
        if (stepscale != 1 && stepscale != 2)
            Con_DPrintf("WARNING! %s is causing runtime sample conversion!\n", sfx->name);
        samplefrac = 0;
        fracstep = stepscale * 256;
        for (i = 0; i < outcount; i++)
        {
            srcsample = samplefrac >> 8;
            samplefrac += fracstep;
            if (inwidth == 2)
                sample = LittleShort(((short *)data)[srcsample]);
            else
                sample = (int)((unsigned char)(data[srcsample]) - 128) << 8;
            if (sc->width == 2)
                ((short *)sc->data)[i] = sample;
            else
                ((signed char *)sc->data)[i] = sample >> 8;
        }
    }
}

sfxcache_t *S_LoadSound(sfx_t *s, channel_t *ch)
{
    char namebuffer[256];
    byte *data;
    wavinfo_t info;
    int len;
    float stepscale;
    sfxcache_t *sc;
    byte stackbuf[1 * 1024]; /* evita a sujeira de sons pequenos */

    if (s->name[0] == '*')
        return S_LoadStreamSound(s, ch);

    /* já está carregado? */
    if (s->servercount == 0 || s->servercount == 1)
        sc = s->cache.data;
    else
        sc = Cache_Check(&s->cache);
    if (sc)
        return sc;

    /* carrega na memória */
    Q_strcpy(namebuffer, "sound");
    if (s->name[0] != '/')
        strcat(namebuffer, "/");
    Q_strcat(namebuffer, s->name);

    data = COM_LoadStackFile(namebuffer, stackbuf, sizeof(stackbuf));

    if (!data)
    {
        Con_DPrintf("Couldn't load %s\n", namebuffer);
        return NULL;
    }

    info = GetWavinfo(s->name, data, com_filesize);
    if (info.channels != 1)
    {
        Con_DPrintf("%s is a stereo sample\n", s->name);
        return NULL;
    }
    if (info.width != 1)
    {
        Con_DPrintf("%s is a 16 bit sample\n", s->name);
        return NULL;
    }

    if (info.rate != shm->speed)
    {
        if (info.rate == shm->speed * 2 && hisound.value > 0)
            stepscale = 1;
        else
            stepscale = (float)info.rate / shm->speed;
    }
    else
        stepscale = 1;
    len = info.samples / stepscale;

    len = len * info.width * info.channels;

    sc = Cache_Alloc(&s->cache, len + sizeof(sfxcache_t), s->name);
    if (!sc)
        return NULL;

    sc->length = info.samples;
    sc->loopstart = info.loopstart;
    sc->speed = info.rate;
    sc->width = info.width;
    sc->stereo = info.channels;

    ResampleSfx(s, sc->speed, sc->width, data + info.dataofs);

    return sc;
}

sfxcache_t *S_LoadSoundFromMemory(sfx_t *s, int unused, byte *data, int len)
{
    wavinfo_t info;
    float stepscale;
    sfxcache_t *sc;

    sc = Cache_Check(&s->cache);
    if (sc)
        return sc;

    info = GetWavinfo(s->name, data, len);
    if (info.channels != 1)
    {
        Con_DPrintf("%s is a stereo sample\n", s->name);
        return NULL;
    }
    if (info.width != 1)
    {
        Con_DPrintf("%s is a 16 bit sample\n", s->name);
        return NULL;
    }

    if (info.rate != shm->speed)
    {
        if (info.rate == shm->speed * 2 && hisound.value > 0)
            stepscale = 1;
        else
            stepscale = (float)info.rate / shm->speed;
    }
    else
        stepscale = 1;
    len = info.samples / stepscale;

    len = len * info.width * info.channels;

    sc = Cache_Alloc(&s->cache, len + sizeof(sfxcache_t), s->name);
    if (!sc)
        return NULL;

    sc->length = info.samples;
    sc->loopstart = info.loopstart;
    sc->speed = info.rate;
    sc->width = info.width;
    sc->stereo = info.channels;

    ResampleSfx(s, sc->speed, sc->width, data + info.dataofs);

    return sc;
}

sfxcache_t *S_LoadStreamChunk(sfx_t *s, int unused, byte *data, int len, int stream)
{
    wavinfo_t info;
    sfxcache_t *sc;

    sc = s->cache.data;
    info = GetWavinfo(s->name, data, len);

    D_00553DE8[stream].len = len - info.dataofs;
    if (info.samples - D_00553DE8[stream].pos < D_00553DE8[stream].len)
        D_00553DE8[stream].len = info.samples - D_00553DE8[stream].pos;
    /* o teste repetido está no binário (provavelmente veio de uma macro) */
    if (info.samples - D_00553DE8[stream].pos < D_00553DE8[stream].len)
        D_00553DE8[stream].len = info.samples - D_00553DE8[stream].pos;

    D_00553DE8[stream].info = info;

    sc->length = D_00553DE8[stream].len;
    sc->loopstart = info.loopstart;
    sc->speed = info.rate;
    sc->width = info.width;
    sc->stereo = info.channels;

    ResampleSfx(s, sc->speed, sc->width, data + info.dataofs);

    return sc;
}

sfxcache_t *S_LoadStreamSound(sfx_t *s, channel_t *ch)
{
    char namebuffer[256];
    char name[64];
    int i;
    unsigned int size;
    int flags;
    byte *buf;
    sfxcache_t *sc;

    if (!D_00557634)
    {
        i = ((int)ch - (int)channels) / sizeof(channel_t);
        if (!D_00553DE8[i].buffer)
        {
            s->streamed = 1;
            Q_strcpy(name, s->name + 1);
            Q_strcpy(namebuffer, "sound");
            if (name[0] != '/')
                strcat(namebuffer, "/");
            Q_strcat(namebuffer, name);

            func_002CFF88(namebuffer, &D_00553DE8[i].unk_34, &D_00553DE8[i].unk_2C, &D_00553DE8[i].unk_30);
            if (D_00553DE8[i].unk_30 > 16)
            {
                size = 16;
                flags = 0x8000;
            }
            else
            {
                size = D_00553DE8[i].unk_30;
                flags = D_00553DE8[i].unk_34;
            }

            buf = func_00225D48(size * 0x800 + 0x58, 0x40, "engine/Snd_mem.c", 417);
            D_00553DE8[i].buffer = buf;
            sc = (sfxcache_t *)(buf + 0x2C);
            s->cache.data = sc;
            ch->unk_41 = func_002388A0(D_00553DE8[i].unk_2C, size, buf + 0x40, D_00553DE8[i].buffer);
            ch->unk_40 = 1;
            ch->unk_43 = 1;
            D_00553DE8[i].unk_38 = size;
            D_00553DE8[i].unk_3C = flags;
        }
        else
            sc = ch->sfx->cache.data;
    }
    else
        sc = NULL;

    return sc;
}

short GetLittleShort(void)
{
    short val = 0;
    val = *data_p;
    val = val + (*(data_p + 1) << 8);
    data_p += 2;
    return val;
}

int GetLittleLong(void)
{
    int val = 0;
    val = *data_p;
    val = val + (*(data_p + 1) << 8);
    val = val + (*(data_p + 2) << 16);
    val = val + (*(data_p + 3) << 24);
    data_p += 4;
    return val;
}

void FindNextChunk(char *name)
{
    while (1)
    {
        data_p = last_chunk;

        if (data_p >= iff_end)
        {
            /* não achou o chunk */
            data_p = NULL;
            return;
        }

        data_p += 4;
        iff_chunk_len = GetLittleLong();
        if (iff_chunk_len < 0)
        {
            data_p = NULL;
            return;
        }
        data_p -= 8;
        last_chunk = data_p + 8 + ((iff_chunk_len + 1) & ~1);
        if (!Q_strncmp((char *)data_p, name, 4))
            return;
    }
}

void FindChunk(char *name)
{
    last_chunk = iff_data;
    FindNextChunk(name);
}

void DumpChunks(void)
{
    char str[5];

    str[4] = 0;
    data_p = iff_data;
    do
    {
        memcpy(str, data_p, 4);
        data_p += 4;
        iff_chunk_len = GetLittleLong();
        Con_Printf("0x%x : %s (%d)\n", (int)(data_p - 4), str, iff_chunk_len);
        data_p += (iff_chunk_len + 1) & ~1;
    } while (data_p < iff_end);
}

wavinfo_t GetWavinfo(char *name, byte *wav, int wavlength)
{
    wavinfo_t info;
    int i;
    int format;
    int samples;

    memset(&info, 0, sizeof(info));

    if (!wav)
        return info;

    iff_data = wav;
    iff_end = wav + wavlength;

    /* procura "RIFF", "WAVE" */
    FindChunk("RIFF");
    if (!(data_p && !Q_strncmp((char *)data_p + 8, "WAVE", 4)))
    {
        Con_Printf("Missing RIFF/WAVE chunks\n");
        return info;
    }

    /* pega as informações do formato */
    iff_data = data_p + 12;

    FindChunk("fmt ");
    if (!data_p)
    {
        Con_Printf("Missing fmt chunk\n");
        return info;
    }
    data_p += 8;
    format = GetLittleShort();
    if (format != 1)
    {
        Con_Printf("Microsoft PCM format only\n");
        return info;
    }

    info.channels = GetLittleShort();
    info.rate = GetLittleLong();
    data_p += 4 + 2;
    info.width = GetLittleShort() / 8;

    /* pega o início do loop */
    FindChunk("cue ");
    if (data_p)
    {
        data_p += 32;
        info.loopstart = GetLittleLong();

        /* se houver um chunk "mark" seguinte, ele diz o tamanho do loop */
        FindNextChunk("LIST");
        if (data_p)
        {
            if (!strncmp((char *)data_p + 28, "mark", 4))
            {
                /* não é um parse correto, mas funciona com o cooledit... */
                data_p += 24;
                i = GetLittleLong(); /* amostras no loop */
                info.samples = info.loopstart + i;
            }
        }
    }
    else
        info.loopstart = -1;

    /* acha o tamanho dos dados */
    FindChunk("data");
    if (!data_p)
    {
        Con_Printf("Missing data chunk\n");
        return info;
    }

    data_p += 4;
    samples = GetLittleLong() / info.width;

    if (info.samples)
    {
        if (samples < info.samples)
            Sys_Error("Sound %s has a bad loop length", name);
    }
    else
        info.samples = samples;

    info.dataofs = data_p - wav;

    return info;
}
