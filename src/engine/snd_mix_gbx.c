#include "common.h"
#include "engine/sound.h"
#include "engine/cvar.h"
#include "engine/client.h"
#include "engine/cl_entity.h"

/* snd_mix_gbx.c: mixagem (snd_mix.c do Quake/HL), dsp e vox */

typedef struct
{
    int left;
    int right;
} portable_samplepair_t;

typedef struct
{
    int volume;
    int pitch;
    int start;
    int end;
    int cbtrim;
    int fKeepCached;
    int samplefrac;
    int timecompress;
    sfx_t *sfx;
} voxword_t;                    /* 0x24 */

extern volatile dma_t *shm;
extern int D_0039984C;          /* paintedtime */
extern cvar_t D_00334D58;       /* volume */
extern portable_samplepair_t D_00542D70[]; /* paintbuffer */
extern voxword_t D_00544E98[][32]; /* rgrgvoxword */

extern int *D_0039981C;         /* snd_p */
extern int D_00399800;          /* snd_linear_count */
extern int D_00399808;          /* snd_vol */
extern short *D_00399814;       /* snd_out */

void func_00238C40(int voice, int);
void func_00225DB8(void *buf);
void func_002C9C68(int ichan);
void func_002C9EA8(channel_t *ch, int clientId);

typedef struct
{
    int cdelaysamplesmax;       /* 0x00 */
    int lp;                     /* 0x04 */
    int idelayinput;            /* 0x08 */
    int idelayoutput;           /* 0x0C */
    int idelayoutputxf;         /* 0x10 */
    int xfade;                  /* 0x14 */
    int delaysamples;           /* 0x18 */
    int delayfeed;              /* 0x1C */
    int lp0, lp1, lp2;          /* 0x20 */
    int mod;                    /* 0x2C */
    int modcur;                 /* 0x30 */
    short *hdelayline;          /* 0x34 */
    short *lpdelayline;         /* 0x38 */
} dly_t;                        /* 0x3C */

extern dly_t D_00544DA8[4];     /* rgsxdly */
extern int D_00544D80[10];      /* rgsxlp */
extern float D_00399840, D_00399824, D_00399804, D_00399828;
extern int D_003348DC;          /* sxhires */
extern int D_003348E0;
extern int D_0039980C, D_00399830, D_00399838, D_00399820; /* sxamod */
extern int D_00399810, D_00399818, D_00399834, D_0039983C; /* sxmod */
extern cvar_t D_003347A0;
extern cvar_t D_003347C0;
extern cvar_t D_003347D8;
extern cvar_t D_003347F0;
extern cvar_t D_00334808;
extern cvar_t D_00334820;
extern cvar_t D_00334838;
extern cvar_t D_00334850;
extern cvar_t D_00334868;
extern cvar_t D_00334880;
extern cvar_t D_00334898;
extern cvar_t D_003348B0;
extern cvar_t D_003348C8;
void func_002C8840(int idelay);
int func_002C86D8(int idelay, float delay);
int RandomLong(int lLow, int lHigh);
int abs(int);
void func_001E82E0(void);
void func_002CB018(void);
extern cl_entity_t *cl_entities[2];

typedef struct
{
    float room_lp;
    float room_mod;
    float room_size;
    float room_refl;
    float room_rvblp;
    float room_delay;
    float room_feedback;
    float room_dlylp;
    float room_left;
} sx_preset_t;                  /* 0x24 */

extern sx_preset_t D_003348E8[29]; /* rgsxpre */
extern char D_00395988[], D_003959A0[], D_003959B8[], D_003959C8[], D_003959E0[];
extern char D_003959F0[], D_00395A00[], D_00395A10[], D_00395A18[];
void Cvar_SetValue(char *var_name, float value);
void func_002C8F28(void);
void func_002C8C20(void);
void func_002C8890(void);
void func_002C96B8(int count);
void func_002C91A8(int count);
void func_002C8D90(int count);
void func_002C89D0(int count);
#define min(a, b) ((a) < (b) ? (a) : (b))
void Con_DPrintf(const char *fmt, ...);
void Con_SafePrintf(const char *fmt, ...);
void Cvar_RegisterVariable(cvar_t *variable);
void *Mem_DbgMalloc(int size, const char *file, int line);
void Mem_Free(void *ptr);
void Mem_PushTag(void);
void Mem_SetTag(int tag);
void Mem_PopTag(void);
void *memset(void *, int, unsigned int);
void func_002C7A20(int end, int fhisound, int clientId);
void func_002C99B0(int count, int, int, int clientId);
extern cvar_t hisound;
extern portable_samplepair_t D_003347B8; /* paintprev */
extern int D_00549698[32][256];  /* snd_scaletable */
void func_002C9C98(channel_t *ch, sfx_t *sfx);
void func_002CB290(channel_t *ch, sfxcache_t *sc);
sfxcache_t *S_LoadSound(sfx_t *s, channel_t *ch);
void Cache_Free(cache_user_t *c);
char *strcpy(char *, const char *);
char *strcat(char *, const char *);
int func_002CFF88(char *name, int *a, int *b, int *c);
byte *func_00225D48(int size, int align, char *file, int line);
byte func_002388A0(int a, int b, byte *c, byte *d);
extern char D_00395968[] __attribute__((aligned(8))); /* "sound/" */
void *memcpy(void *, const void *, unsigned int);
extern char D_00395970[];       /* "engine/snd_mix_gbx.c" */

void func_002C72C8(void)
{
    int i;
    int val;

    for (i = 0; i < D_00399800; i += 2)
    {
        val = (D_0039981C[i] * D_00399808) >> 8;
        if (val > 0x7fff)
            D_00399814[i] = 0x7fff;
        else if (val < (short)0x8000)
            D_00399814[i] = (short)0x8000;
        else
            D_00399814[i] = val;

        val = (D_0039981C[i + 1] * D_00399808) >> 8;
        if (val > 0x7fff)
            D_00399814[i + 1] = 0x7fff;
        else if (val < (short)0x8000)
            D_00399814[i + 1] = (short)0x8000;
        else
            D_00399814[i + 1] = val;
    }
}

void func_002C7378(int endtime)
{
    int lpos;
    int lpaintedtime;
    unsigned int *pbuf;

    D_00399808 = D_00334D58.value * 256;

    D_0039981C = (int *)D_00542D70;
    endtime <<= 1;
    lpaintedtime = D_0039984C << 1;

    pbuf = (unsigned int *)shm->buffer;

    while (lpaintedtime < endtime)
    {
        lpos = lpaintedtime & ((shm->samples >> 1) - 1);

        D_00399814 = (short *)pbuf + (lpos << 1);

        D_00399800 = (shm->samples >> 1) - lpos;
        if (endtime - lpaintedtime < D_00399800)
            D_00399800 = endtime - lpaintedtime;

        D_00399800 <<= 1;

        func_002C72C8();

        D_0039981C += D_00399800;
        lpaintedtime += (D_00399800 >> 1);
    }
}

void func_002C74A0(int endtime)
{
    int out_idx;
    int count;
    int out_mask;
    int *p;
    int step;
    int val;
    int snd_vol;
    unsigned int *pbuf;

    if (shm->samplebits == 16 && shm->channels == 2)
    {
        func_002C7378(endtime);
        return;
    }

    p = (int *)D_00542D70;
    count = ((endtime - D_0039984C) * shm->channels) << 1;
    out_mask = shm->samples - 1;
    out_idx = ((D_0039984C << 1) * shm->channels) & out_mask;
    step = 3 - shm->channels;
    snd_vol = D_00334D58.value * 256;

    pbuf = (unsigned int *)shm->buffer;
    if (shm->samplebits == 16)
    {
        short *out = (short *)pbuf;
        while (count--)
        {
            val = (*p * snd_vol) >> 8;
            p += step;
            if (val > 0x7fff)
                val = 0x7fff;
            else if (val < (short)0x8000)
                val = (short)0x8000;
            out[out_idx] = val;
            out_idx = (out_idx + 1) & out_mask;
        }
    }
    else if (shm->samplebits == 8)
    {
        unsigned char *out = (unsigned char *)pbuf;
        while (count--)
        {
            val = (*p * snd_vol) >> 8;
            p += step;
            if (val > 0x7fff)
                val = 0x7fff;
            else if (val < (short)0x8000)
                val = (short)0x8000;
            out[out_idx] = (val >> 8) + 128;
            out_idx = (out_idx + 1) & out_mask;
        }
    }
}

void func_002C7660(channel_t *ch, int clientId)
{
    if (ch->unk_40 == 1)
    {
        func_00238C40(ch->unk_41, 1);
        ch->unk_40 = 0;
    }
    else if (ch->entchannel == CHAN_STREAM)
    {
        func_00225DB8(D_00553DE8[(unsigned int)((char *)ch - (char *)channels) / sizeof(channel_t)].buffer);
    }

    if (ch->entchannel == CHAN_STREAM)
    {
        int i = (unsigned int)((char *)ch - (char *)channels) / sizeof(channel_t);
        ch->sfx->streamed = 0;
        ch->sfx->cache.data = NULL;
        D_00553DE8[i].buffer = NULL;
        func_002C9C68(i);
    }

    if (ch->isentence >= 0)
        D_00544E98[ch->isentence][0].sfx = NULL;

    ch->sfx = NULL;
    ch->isentence = -1;
    func_002C9EA8(ch, clientId);
}

#ifdef NON_MATCHING
int func_002C7788(channel_t *ch, sfxcache_t **psc, int ltime, int ichan, int clientId)
{
    sfxcache_t *sc;
    sfx_t *sfx;
    stream_t *st;
    char name[256];
    int lba;
    int size;

    sc = *psc;

    if (sc->loopstart >= 0)
    {
        ch->pos = sc->loopstart;
        ch->end = ltime + sc->length - ch->pos;
        if (ch->isentence >= 0)
            D_00544E98[ch->isentence][ch->iword].samplefrac = 0;
        return 0;
    }

    if (ch->entchannel == CHAN_STREAM)
    {
        if (D_00553DE8[ichan].pos < D_00553DE8[ichan].info.samples)
        {
            st = &D_00553DE8[ichan];
            if (st->unk_38 == st->unk_30)
            {
                func_002C7660(ch, clientId);
                return 1;
            }
            func_002C9C98(ch, ch->sfx);
            ch->pos = 0;
            ch->end = ltime + sc->length;
            return 0;
        }
    }
    else if (ch->isentence >= 0)
    {
        sfx = D_00544E98[ch->isentence][ch->iword].sfx;
        if (!D_00544E98[ch->isentence][ch->iword].fKeepCached)
            Cache_Free(&sfx->cache);

        ch->sfx = sfx = D_00544E98[ch->isentence][ch->iword + 1].sfx;
        if (sfx)
        {
            if (sfx->servercount == 2)
            {
                memcpy(name, D_00395968, 7);
                strcat(name, sfx->name);
                func_002CFF88(name, &ch->unk_44, &lba, &size);
                if (size > 0)
                {
                    ch->unk_40 = 1;
                    ch->unk_48 = func_00225D48(size << 11, 0x40, D_00395970, 353);
                    ch->unk_41 = func_002388A0(lba, size, ch->unk_48, ch->unk_48);
                }
                ch->pos = 0;
                ch->iword++;
                return 1;
            }

            *psc = sc = S_LoadSound(sfx, ch);
            if (sc && sfx)
            {
                ch->pos = 0;
                ch->end = ltime + sc->length;
                ch->iword++;
                func_002CB290(ch, sc);
                return 0;
            }
        }
    }

    func_002C7660(ch, clientId);
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/snd_mix_gbx", func_002C7788);
#endif

INCLUDE_ASM("asm/nonmatchings/engine/snd_mix_gbx", func_002C7A20);

#ifdef NON_MATCHING
void func_002C7FF8(int endtime, int clientId)
{
    int end;
    int count;
    int count2;
    int i, j;

    while (D_0039984C < endtime)
    {
        end = endtime;
        if (endtime - D_0039984C > 512)
            end = D_0039984C + 512;

        count = end - D_0039984C;
        count2 = count * 2;
        memset(D_00542D70, 0, count * sizeof(portable_samplepair_t));

        func_002C7A20(end, 0, clientId);

        if (hisound.value == 0)
            func_002C99B0(count, 1, 1, clientId);

        for (i = count - 1, j = count2 - 1; i > 0; i--, j -= 2)
        {
            D_00542D70[j] = D_00542D70[i];
            D_00542D70[j - 1].left = (D_00542D70[i].left + D_00542D70[i - 1].left) >> 1;
            D_00542D70[j - 1].right = (D_00542D70[i].right + D_00542D70[i - 1].right) >> 1;
        }

        D_00542D70[1] = D_00542D70[0];
        D_00542D70[0].left = (D_003347B8.left + D_00542D70[0].left) >> 1;
        D_00542D70[0].right = (D_003347B8.right + D_00542D70[0].right) >> 1;
        D_003347B8 = D_00542D70[count2 - 1];

        if (hisound.value > 0)
        {
            func_002C7A20(end, 1, clientId);
            func_002C99B0(count2, 1, 1, clientId);
        }

        func_002C74A0(end);
        D_0039984C = end;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/snd_mix_gbx", func_002C7FF8);
#endif

void func_002C8218(void)
{
    int i, j;

    for (i = 0; i < 32; i++)
        for (j = 0; j < 256; j++)
            D_00549698[i][j] = ((signed char)j) * i * 8;
}

void func_002C8278(channel_t *ch, sfxcache_t *sc, int count)
{
    int data;
    int *lscale, *rscale;
    unsigned char *sfx;
    int i;

    if (ch->leftvol > 255)
        ch->leftvol = 255;
    if (ch->rightvol > 255)
        ch->rightvol = 255;

    lscale = D_00549698[ch->leftvol >> 3];
    rscale = D_00549698[ch->rightvol >> 3];
    sfx = (unsigned char *)sc->data + ch->pos;

    for (i = 0; i < count; i++)
    {
        data = sfx[i];
        D_00542D70[i].left += lscale[data];
        D_00542D70[i].right += rscale[data];
    }

    ch->pos += count;
}

void func_002C8350(channel_t *ch, sfxcache_t *sc, int count, int offset)
{
    int data;
    int *lscale, *rscale;
    unsigned char *sfx;
    int i;
    portable_samplepair_t *pb = &D_00542D70[offset];

    if (ch->leftvol > 255)
        ch->leftvol = 255;
    if (ch->rightvol > 255)
        ch->rightvol = 255;

    lscale = D_00549698[ch->leftvol >> 3];
    rscale = D_00549698[ch->rightvol >> 3];
    sfx = (unsigned char *)sc->data + ch->pos;

    for (i = 0; i < count; i++)
    {
        data = sfx[i];
        pb[i].left += lscale[data];
        pb[i].right += rscale[data];
    }

    ch->pos += count;
}

void func_002C8430(channel_t *ch, sfxcache_t *sc, int count, int offset)
{
    int data;
    int left, right;
    int leftvol, rightvol;
    signed short *sfx;
    int i;
    portable_samplepair_t *pb = &D_00542D70[offset];

    leftvol = ch->leftvol;
    rightvol = ch->rightvol;
    sfx = (signed short *)sc->data + ch->pos;

    for (i = 0; i < count; i++)
    {
        data = sfx[i];
        left = (data * leftvol) >> 8;
        right = (data * rightvol) >> 8;
        pb[i].left += left;
        pb[i].right += right;
    }

    ch->pos += count;
}

INCLUDE_RODATA("asm/nonmatchings/engine/snd_mix_gbx", D_00395950);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_mix_gbx", D_00395958);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_mix_gbx", D_00395968);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_mix_gbx", D_00395970);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_mix_gbx", D_00395988);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_mix_gbx", D_00395998);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_mix_gbx", D_003959A0);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_mix_gbx", D_003959B0);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_mix_gbx", D_003959B8);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_mix_gbx", D_003959C8);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_mix_gbx", D_003959D8);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_mix_gbx", D_003959E0);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_mix_gbx", D_003959F0);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_mix_gbx", D_00395A00);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_mix_gbx", D_00395A10);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_mix_gbx", D_00395A18);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_mix_gbx", D_00395A28);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_mix_gbx", D_00395A38);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_mix_gbx", D_00395A40);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_mix_gbx", D_00395A50);

#ifdef NON_MATCHING
void func_002C84C0(void)
{
    memset(D_00544DA8, 0, sizeof(D_00544DA8));
    memset(D_00544D80, 0, sizeof(D_00544D80));

    D_00399824 = -1.0f;
    D_00399804 = -1.0f;
    D_00399828 = -1.0f;
    D_00399840 = -1.0f;

    D_003348DC = hisound.value ? 1 : 0;
    D_003348E0 = D_003348DC;
    D_00399838 = 255;
    D_00399830 = 255;
    D_00399820 = 255;
    D_0039980C = 255;

    D_00399810 = 350 * (shm->speed / 11025);
    D_00399834 = 450 * (shm->speed / 11025);
    D_00399818 = D_00399810;
    D_0039983C = D_00399834;

    Con_DPrintf("\nFX Processor Initialization\n");

    Cvar_RegisterVariable(&D_003347A0);
    Cvar_RegisterVariable(&D_003347C0);
    Cvar_RegisterVariable(&D_003347D8);
    Cvar_RegisterVariable(&D_003347F0);
    Cvar_RegisterVariable(&D_00334808);
    Cvar_RegisterVariable(&D_00334820);
    Cvar_RegisterVariable(&D_00334838);
    Cvar_RegisterVariable(&D_00334850);
    Cvar_RegisterVariable(&D_00334868);
    Cvar_RegisterVariable(&D_00334880);
    Cvar_RegisterVariable(&D_00334898);
    Cvar_RegisterVariable(&D_003348B0);
    Cvar_RegisterVariable(&D_003348C8);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/snd_mix_gbx", func_002C84C0);
#endif

void func_002C8688(void)
{
    int i;

    func_002C8840(0);
    for (i = 0; i < 2; i++)
        func_002C8840(1 + i);
    func_002C8840(3);
}

int func_002C86D8(int idelay, float delay)
{
    dly_t *cur;
    short *buf;
    int size;

    cur = &D_00544DA8[idelay];

    if (delay > 0.4f)
        delay = 0.4f;

    if (cur->lpdelayline)
    {
        Mem_Free(cur->hdelayline);
        cur->hdelayline = NULL;
        cur->lpdelayline = NULL;
    }

    if (delay == 0.0f)
        return 1;

    cur->cdelaysamplesmax = ((int)(shm->speed * delay) << D_003348DC) + 1;
    size = cur->cdelaysamplesmax * 2;

    Mem_PushTag();
    Mem_SetTag(1);
    buf = Mem_DbgMalloc(size, D_00395970, 1070);
    Mem_PopTag();

    if (!buf)
    {
        Con_SafePrintf("Sound FX: Out of memory.\n");
        return 0;
    }

    memset(buf, 0, size);
    cur->hdelayline = buf;
    cur->idelayinput = 0;
    cur->xfade = 0;
    cur->mod = 0;
    cur->lpdelayline = buf;
    if (!cur->lpdelayline)
    {
        Con_SafePrintf("Sound FX: Failed to lock.\n");
        return 0;
    }
    cur->modcur = 0;
    cur->lp = 1;
    cur->idelayoutput = cur->cdelaysamplesmax - cur->delaysamples;
    cur->lp2 = 0;
    cur->lp1 = 0;
    cur->lp0 = 0;
    return 1;
}

void func_002C8840(int idelay)
{
    dly_t *cur = &D_00544DA8[idelay];

    if (cur->lpdelayline)
    {
        Mem_Free(cur->hdelayline);
        cur->hdelayline = NULL;
        cur->lpdelayline = NULL;
    }
}

void func_002C8890(void)
{
    float delay;
    dly_t *dly;
    int delaysamples;

    dly = &D_00544DA8[3];
    delay = D_00334850.value;

    if (delay == D_00399828)
        return;

    if (delay == 0)
    {
        func_002C8840(3);
        D_00399828 = 0;
        return;
    }

    delay = min(delay, 0.1f);
    delay *= shm->speed;
    delaysamples = (int)delay;
    delaysamples <<= D_003348DC;

    if (!dly->lpdelayline)
    {
        dly->delaysamples = delaysamples;
        func_002C86D8(3, 0.1f);
    }

    if (delaysamples != dly->delaysamples)
    {
        dly->idelayoutputxf = dly->idelayinput - delaysamples;
        if (dly->idelayoutputxf < 0)
            dly->idelayoutputxf += dly->cdelaysamplesmax;
        dly->xfade = 128;
    }

    D_00399828 = D_00334850.value;
    dly->mod = 0;
    dly->modcur = 0;

    if (dly->delaysamples == 0)
        func_002C8840(3);
}

void func_002C89D0(int count)
{
    short delay, samplexf;
    portable_samplepair_t *paint;
    int countr;
    int val;

    if (!D_00544DA8[3].lpdelayline)
        return;

    paint = D_00542D70;
    countr = count;
    while (countr--)
    {
        if (D_00544DA8[3].mod && (--D_00544DA8[3].modcur < 0))
            D_00544DA8[3].modcur = D_00544DA8[3].mod;

        val = paint->left;
        delay = D_00544DA8[3].lpdelayline[D_00544DA8[3].idelayoutput];

        if (D_00544DA8[3].xfade || delay || val)
        {
            if (!D_00544DA8[3].xfade && !D_00544DA8[3].modcur && D_00544DA8[3].mod)
            {
                D_00544DA8[3].idelayoutputxf = D_00544DA8[3].idelayoutput + ((RandomLong(0, 0xFF) * D_00544DA8[3].delaysamples) >> 9);

                if (D_00544DA8[3].idelayoutputxf >= D_00544DA8[3].cdelaysamplesmax)
                    D_00544DA8[3].idelayoutputxf -= D_00544DA8[3].cdelaysamplesmax;

                D_00544DA8[3].xfade = 128;
            }

            if (D_00544DA8[3].xfade)
            {
                samplexf = (D_00544DA8[3].lpdelayline[D_00544DA8[3].idelayoutputxf] * (128 - D_00544DA8[3].xfade)) >> 7;
                delay = samplexf + ((delay * D_00544DA8[3].xfade) >> 7);

                if (++D_00544DA8[3].idelayoutputxf >= D_00544DA8[3].cdelaysamplesmax)
                    D_00544DA8[3].idelayoutputxf = 0;

                if (--D_00544DA8[3].xfade == 0)
                    D_00544DA8[3].idelayoutput = D_00544DA8[3].idelayoutputxf;
            }

            if (val > 0x7fff)
                val = 0x7fff;
            else if (val < (short)0x8000)
                val = (short)0x8000;
            D_00544DA8[3].lpdelayline[D_00544DA8[3].idelayinput] = val;

            paint->left = delay;
        }
        else
        {
            D_00544DA8[3].lpdelayline[D_00544DA8[3].idelayinput] = 0;
        }

        if (++D_00544DA8[3].idelayinput >= D_00544DA8[3].cdelaysamplesmax)
            D_00544DA8[3].idelayinput = 0;

        if (++D_00544DA8[3].idelayoutput >= D_00544DA8[3].cdelaysamplesmax)
            D_00544DA8[3].idelayoutput = 0;
        paint++;
    }
}

void func_002C8C20(void)
{
    float delay;
    dly_t *dly;

    dly = &D_00544DA8[0];
    delay = D_003347C0.value;

    if (delay != D_00399824)
    {
        if (delay == 0)
        {
            func_002C8840(0);
            D_00399824 = D_003347C0.value;
        }
        else
        {
            delay = min(delay, 0.4f);
            delay *= shm->speed;
            dly->delaysamples = (int)delay << D_003348DC;

            if (!dly->lpdelayline)
                func_002C86D8(0, 0.4f);

            if (dly->lpdelayline)
            {
                memset(dly->lpdelayline, 0, dly->cdelaysamplesmax * 2);
                dly->lp2 = dly->lp1 = dly->lp0 = 0;
            }

            dly->idelayinput = 0;
            dly->idelayoutput = dly->cdelaysamplesmax - dly->delaysamples;
            D_00399824 = D_003347C0.value;

            if (!dly->delaysamples)
                func_002C8840(0);
        }
    }

    dly->lp = (int)D_003347F0.value;
    dly->delayfeed = (int)(D_003347D8.value * 255);
}

void func_002C8D90(int count)
{
    int val;
    int valt;
    int left, right;
    short sampledly;
    portable_samplepair_t *pbuf;
    int countr;

    if (D_00544DA8[0].lpdelayline)
    {
        pbuf = D_00542D70;
        countr = count;

        while (countr--)
        {
            sampledly = D_00544DA8[0].lpdelayline[D_00544DA8[0].idelayoutput];
            left = pbuf->left;
            right = pbuf->right;

            if (sampledly || left || right)
            {
                val = ((left + right) >> 1) + ((D_00544DA8[0].delayfeed * sampledly) >> 8);
                if (val > 0x7fff)
                    val = 0x7fff;
                else if (val < (short)0x8000)
                    val = (short)0x8000;

                if (D_00544DA8[0].lp)
                {
                    valt = (D_00544DA8[0].lp0 + D_00544DA8[0].lp1 + (val << 1)) >> 2;
                    D_00544DA8[0].lp0 = D_00544DA8[0].lp1;
                    D_00544DA8[0].lp1 = val;
                }
                else
                {
                    valt = val;
                }

                D_00544DA8[0].lpdelayline[D_00544DA8[0].idelayinput] = valt;

                valt >>= 2;
                left += valt;
                right += valt;

                if (left > 0x7fff)
                    left = 0x7fff;
                else if (left < (short)0x8000)
                    left = (short)0x8000;
                if (right > 0x7fff)
                    right = 0x7fff;
                else if (right < (short)0x8000)
                    right = (short)0x8000;

                pbuf->left = left;
                pbuf->right = right;
            }
            else
            {
                D_00544DA8[0].lp0 = D_00544DA8[0].lp1 = 0;
                D_00544DA8[0].lpdelayline[D_00544DA8[0].idelayinput] = 0;
            }

            if (++D_00544DA8[0].idelayinput >= D_00544DA8[0].cdelaysamplesmax)
                D_00544DA8[0].idelayinput = 0;

            if (++D_00544DA8[0].idelayoutput >= D_00544DA8[0].cdelaysamplesmax)
                D_00544DA8[0].idelayoutput = 0;
            pbuf++;
        }
    }
}

#ifdef NON_MATCHING
void func_002C8F28(void)
{
    dly_t *dly;
    int delaysamples = 0;
    int i, mod;

    if (D_00334808.value != D_00399804)
    {
        D_00399804 = D_00334808.value;

        if (D_00334808.value == 0)
        {
            func_002C8840(1);
            func_002C8840(2);
        }
        else
        {
            for (i = 1, dly = &D_00544DA8[1]; i < 3; i++, dly++)
            {
                switch (i)
                {
                case 1:
                    delaysamples = (int)(min(D_00334808.value, 0.1f) * shm->speed);
                    dly->mod = (500 * (shm->speed / 11025)) << D_003348DC;
                    break;
                case 2:
                    delaysamples = (int)(min(D_00334808.value * 0.71f, 0.1f) * shm->speed);
                    dly->mod = (700 * (shm->speed / 11025)) << D_003348DC;
                    break;
                }

                delaysamples <<= D_003348DC;
                mod = dly->mod;

                if (!dly->lpdelayline)
                {
                    dly->delaysamples = delaysamples;
                    func_002C86D8(i, 0.1f);
                }

                dly->modcur = dly->mod = mod;

                if (delaysamples != dly->delaysamples)
                {
                    dly->idelayoutputxf = dly->idelayinput - delaysamples;
                    if (dly->idelayoutputxf < 0)
                        dly->idelayoutputxf += dly->cdelaysamplesmax;
                    dly->xfade = 32;
                }

                if (!dly->delaysamples)
                    func_002C8840(i);
            }
        }
    }

    D_00544DA8[1].lp = D_00544DA8[2].lp = (int)D_00334838.value;
    D_00544DA8[1].delayfeed = D_00544DA8[2].delayfeed = (int)(D_00334820.value * 255);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/snd_mix_gbx", func_002C8F28);
#endif

INCLUDE_ASM("asm/nonmatchings/engine/snd_mix_gbx", func_002C91A8);

INCLUDE_ASM("asm/nonmatchings/engine/snd_mix_gbx", func_002C96B8);

#ifdef NON_MATCHING
void func_002C9980(void)
{
    D_00399840 = -1.0f;
    D_00399824 = -1.0f;
    D_00399804 = -1.0f;
    D_00399828 = -1.0f;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/snd_mix_gbx", func_002C9980);
#endif

#ifdef NON_MATCHING
void func_002C99B0(int count, int fFilter, int fTimefx, int clientId)
{
    int fReset;
    int i;
    float roomType;

    if (D_003348C8.value != 0.0f)
        return;

    D_003348DC = hisound.value ? 1 : 0;
    if (D_003348DC != D_003348E0)
    {
        func_002C9980();
        D_003348E0 = D_003348DC;
    }

    fReset = 0;

    if (cl[clientId].waterlevel > 2)
        roomType = D_003348B0.value;
    else
        roomType = D_00334898.value;

    if (roomType != D_00399840)
    {
        i = (int)roomType;
        D_00399840 = roomType;
        if ((unsigned int)i < 29)
        {
            Cvar_SetValue(D_00395A10, D_003348E8[i].room_lp);
            Cvar_SetValue(D_00395A18, D_003348E8[i].room_mod);
            Cvar_SetValue(D_003959C8, D_003348E8[i].room_size);
            Cvar_SetValue(D_003959E0, D_003348E8[i].room_refl);
            Cvar_SetValue(D_003959F0, D_003348E8[i].room_rvblp);
            Cvar_SetValue(D_00395988, D_003348E8[i].room_delay);
            Cvar_SetValue(D_003959A0, D_003348E8[i].room_feedback);
            Cvar_SetValue(D_003959B8, D_003348E8[i].room_dlylp);
            Cvar_SetValue(D_00395A00, D_003348E8[i].room_left);
        }

        func_002C8F28();
        func_002C8C20();
        func_002C8890();
        fReset = 1;
    }

    if (fReset || roomType != 0.0f)
    {
        func_002C8F28();
        func_002C8C20();
        func_002C8890();

        if (fFilter)
            func_002C96B8(count);

        if (fTimefx)
        {
            func_002C91A8(count);
            func_002C8D90(count);
            func_002C89D0(count);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/snd_mix_gbx", func_002C99B0);
#endif

int func_002C9C10(void)
{
    int i;

    for (i = 0; i < MAX_CHANNELS; i++)
        memset(&D_00553DE8[i], 0, sizeof(stream_t));
    return 1;
}

void func_002C9C68(int ichan)
{
    memset(&D_00553DE8[ichan], 0, sizeof(stream_t));
}

#ifdef NON_MATCHING
void func_002C9C98(channel_t *ch, sfx_t *sfx)
{
    int i;
    sfxcache_t *sc;
    int lba;
    int size;
    int bytes;

    i = (unsigned int)((char *)ch - (char *)channels) / sizeof(channel_t);
    sc = sfx->cache.data;

    if (ch->unk_40 == 1)
    {
        func_00238C40(ch->unk_41, 0);
        ch->unk_40 = 0;
    }

    D_00553DE8[i].unk_20 = D_00553DE8[i].pos + D_00553DE8[i].info.dataofs;
    lba = D_00553DE8[i].unk_2C + D_00553DE8[i].unk_38;

    if (D_00553DE8[i].unk_38 + 16 >= D_00553DE8[i].unk_30)
    {
        size = D_00553DE8[i].unk_30 - D_00553DE8[i].unk_38;
        if (size > 0)
            bytes = D_00553DE8[i].unk_34 % 2048 - 2048 + size * 2048;
        else
            bytes = 0;
    }
    else
    {
        size = 16;
        bytes = 0x8000;
    }

    ch->unk_41 = func_002388A0(lba, size, sc->data, D_00553DE8[i].buffer);
    ch->unk_40 = 1;

    D_00553DE8[i].unk_38 += size;
    D_00553DE8[i].unk_3C += bytes;
    D_00553DE8[i].len = bytes;
    if (D_00553DE8[i].info.samples - D_00553DE8[i].pos < bytes)
        D_00553DE8[i].len = D_00553DE8[i].info.samples - D_00553DE8[i].pos;

    sc->length = D_00553DE8[i].len;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/snd_mix_gbx", func_002C9C98);
#endif

void func_002C9E48(int entnum, int entchannel, int clientId)
{
    if ((entchannel == CHAN_VOICE || entchannel == CHAN_STREAM) && entnum > 0)
    {
        cl_entities[clientId][entnum].mouthopen = 0;
        cl_entities[clientId][entnum].sndavg = 0;
        cl_entities[clientId][entnum].sndcount = 0;
    }
}

void func_002C9EA8(channel_t *ch, int clientId)
{
    if (ch->entnum > 0 && (ch->entchannel == CHAN_VOICE || ch->entchannel == CHAN_STREAM))
        cl_entities[clientId][ch->entnum].mouthopen = 0;
}

void func_002C9EF8(channel_t *ch, sfxcache_t *sc, int count, int clientId)
{
    int data;
    char *pdata;
    int i;
    int savg;
    int scount;
    cl_entity_t *pent;

    pent = &cl_entities[clientId][ch->entnum];
    pdata = (char *)sc->data + ch->pos;

    i = 0;
    savg = 0;
    scount = pent->sndcount;

    while (i < count && scount < 10)
    {
        data = pdata[i];
        savg += abs(data);
        i += 80 + ((byte)data & 0x1F);
        scount++;
    }

    pent->sndavg += savg;
    pent->sndcount = (byte)scount;

    if (pent->sndcount >= 10)
    {
        pent->mouthopen = pent->sndavg / 10;
        pent->sndavg = 0;
        pent->sndcount = 0;
    }

    if (cl_numlocal >= 2)
    {
        pent = &cl_entities[1][ch->entnum];
        pent->sndavg += savg;
        pent->sndcount = (byte)scount;

        if (pent->sndcount >= 10)
        {
            pent->mouthopen = pent->sndavg / 10;
            pent->sndavg = 0;
            pent->sndcount = 0;
        }
    }
}

void func_002CA018(void)
{
    func_001E82E0();
    memset(D_00544E98, 0, sizeof(voxword_t) * 32 * 16);
    func_002CB018();
}

INCLUDE_ASM("asm/nonmatchings/engine/snd_mix_gbx", func_002CA050);

INCLUDE_ASM("asm/nonmatchings/engine/snd_mix_gbx", func_002CA250);

INCLUDE_ASM("asm/nonmatchings/engine/snd_mix_gbx", func_002CA320);

INCLUDE_ASM("asm/nonmatchings/engine/snd_mix_gbx", func_002CA3C8);

INCLUDE_ASM("asm/nonmatchings/engine/snd_mix_gbx", func_002CA698);

INCLUDE_ASM("asm/nonmatchings/engine/snd_mix_gbx", func_002CA700);

INCLUDE_ASM("asm/nonmatchings/engine/snd_mix_gbx", func_002CA7E8);

INCLUDE_ASM("asm/nonmatchings/engine/snd_mix_gbx", func_002CAB28);

INCLUDE_ASM("asm/nonmatchings/engine/snd_mix_gbx", func_002CB018);

INCLUDE_ASM("asm/nonmatchings/engine/snd_mix_gbx", func_002CB1E8);

INCLUDE_ASM("asm/nonmatchings/engine/snd_mix_gbx", func_002CB290);
