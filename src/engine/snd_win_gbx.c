#include "common.h"
#include "engine/cmd.h"
#include "engine/sound.h"

/* saída de som do PS2 (snd_win.c do HL adaptado: "wave" = blocos enviados ao IOP/SPU2) */

typedef struct wavehdr_s
{
    char *lpData;
    int dwFlags;
} wavehdr_t;

/* bloco de transferência para o SPU (0x38 bytes, lista circular) */
typedef struct sndblk_s
{
    unsigned int addr;          /* 0x00 */
    int state;                  /* 0x04 */
    int wrapped;                /* 0x08 */
    unsigned int start;         /* 0x0C */
    unsigned int end;           /* 0x10 */
    int unk_14;
    unsigned long time;         /* 0x18 */
    byte unk_20[0x28 - 0x20];
    byte id;                    /* 0x28 */
    byte inuse;                 /* 0x29 */
    struct sndblk_s *next;      /* 0x2C */
    wavehdr_t *hdr;             /* 0x30 */
    int unk_34;
} sndblk_t;

/* cabeçalho de cada bloco de 0x800 bytes enviado ao IOP */
typedef struct sndhdr_s
{
    unsigned int addr;          /* 0x00 */
    int idx;                    /* 0x04 */
    int size;                   /* 0x08 */
    int last;                   /* 0x0C */
} sndhdr_t;

/* buffer de stream (0xDC; dois em D_00465898, alternados) */
typedef struct sndstream_s
{
    char *buf;                  /* 0x00 */
    char *data[8];              /* 0x04 */
    short *left[16];            /* 0x24 */
    short *right[16];           /* 0x64 */
    short *curleft;             /* 0xA4 */
    short *curright;            /* 0xA8 */
    int samples;                /* 0xAC */
    int half;                   /* 0xB0 */
    int flag;                   /* 0xB4 */
    int count;                  /* 0xB8 */
    sndhdr_t *hdr[8];           /* 0xBC */
} sndstream_t;

#define WAV_BUFFERS 64
#define WAV_BUFFER_SIZE 0x400

void *memset(void *, int, unsigned int);
void *memcpy(void *, const void *, unsigned int);
byte *func_00225D48(int size, int align, char *file, int line);
int func_002F3B08(int size);
int func_002F2890(void *cd);
int func_002F26A0(void *cd, int fno, int mode, void *send, int ssize, void *recv, int rsize, void *end, void *endparam);
int COM_CheckParm(char *parm);
unsigned long func_00229B88(void);
int func_002D63A0(int, int, int, ...);
int func_002CED18(unsigned int start, unsigned int end, unsigned int pos);
void func_002CEDD8(unsigned int pos);
void func_002CE938(void);
void func_002EF8E0(int);
void func_002CD450(void);
void func_002CBE40(void);
void func_002CBBB0(void);
void func_002CF378(int);
void func_002CF380(int vol);
void func_002CF3C8(void);
void func_002CF080(void);

extern dma_t *shm;
extern dma_t D_00551730;        /* sn */
extern int D_00334D28;          /* snd_blocked */
extern signed char D_00334E9C;  /* wav_init */
extern signed char D_00334E9D;  /* snd_firsttime */
extern signed char D_00334E9E;  /* snd_isdirect? */
extern signed char D_00334E9F;  /* snd_iswave */
extern int D_00334EA4;          /* snd_buffer_count */
extern int D_00399850;          /* hWaveOut */
extern wavehdr_t *D_00399854;   /* lpWaveHdr */
extern unsigned int D_00399858; /* gSndBufSize */
extern wavehdr_t *D_0039985C;
extern char *D_00399860;        /* lpData */
extern char *D_00399864;
extern int D_00465A58;          /* sample16 */
extern int D_00465A5C;          /* snd_sent */
extern int D_00465A60;          /* snd_completed */
extern sndblk_t *D_00465A68;
extern int D_00465A70;
extern int D_00334EA8;
extern unsigned short D_00334EAC;
extern unsigned short D_00334EAE;
extern sndblk_t *D_00465A64;
extern sndblk_t *D_00465A6C;
extern sndstream_t *D_00465A50;
extern int D_00465A54;
extern sndstream_t D_00465898[2];
extern char D_0032EF40[];       /* SifRpcClientData do som */
extern char D_00396208[];      /* "engine/snd_win_gbx.c" */

void func_002CE108(void)
{
    if (D_00334E9F == 1)
        D_00334D28++;
}

void func_002CE130(void)
{
    if (D_00334E9F == 1)
        D_00334D28--;
}

void func_002CE158(void)
{
    if (D_00399850)
    {
        if (D_00399854)
            Mem_Free(D_00399854);
        if (D_00399860)
            Mem_Free(D_00399860);
    }
    D_00399850 = 0;
    D_00399860 = NULL;
    D_00399854 = NULL;
    D_00399864 = NULL;
    D_0039985C = NULL;
    D_00334E9C = 0;
}

void func_002CE1E8(void)
{
    if (D_00334E9E == 1)
    {
        if (--D_00334EA4 == 0)
        {
            func_002CD450();
            func_002CBE40();
        }
    }
}

void func_002CE230(void)
{
    if (D_00334E9E == 1)
    {
        if (++D_00334EA4 == 1)
            func_002CBBB0();
    }
}

signed char func_002CEEE0(void);

int func_002CE270(void)
{
    D_00334E9C = 0;

    if (D_00334E9D == 1 || D_00334E9F == 1)
    {
        D_00334E9F = func_002CEEE0();
        if (D_00334E9F == 1)
        {
            if (D_00334E9D == 1)
                Con_DPrintf("Wave sound initialized\n");
        }
        else
        {
            Con_DPrintf("Wave sound failed to init\n");
        }
    }

    D_00334E9D = 0;
    D_00334EA4 = 1;

    if (!D_00334E9C)
    {
        if (D_00334E9D == 1)
            Con_DPrintf("No sound device initialized\n");
        return 0;
    }
    return 1;
}

int func_002CE338(void)
{
    int s;

    s = 0;
    if (D_00334E9C == 1)
        s = D_00465A5C * WAV_BUFFER_SIZE;

    s >>= D_00465A58;
    s &= (shm->samples - 1);
    return s;
}

/* SNDDMA_Submit: reamostra os blocos de 22050 Hz para o SPU e envia */
#ifdef NON_MATCHING
void func_002CE380(void)
{
    short left[600];
    short right[600];
    int addr;
    int n;
    unsigned long time;
    int i;
    int k;
    int count;
    int idx;
    float frac;
    wavehdr_t *h;
    short *src;
    short *data;
    short *lp;
    short *rp;
    short *lp2;
    short *rp2;

    if (!D_00334E9C)
        return;

    if (D_00334EA8 == 1)
    {
        addr = func_002D63A0(1, 0x8100, 0) & 0xFFFFFF;
        func_002CEDD8(addr);
    }
    else
        addr = D_00465A64->start;

    while (1)
    {
        if (D_00465A60 == D_00465A5C)
            break;
        if (!(D_0039985C[D_00465A60 & 63].dwFlags & 1))
            break;
        D_00465A60++;
    }

    n = D_00465A5C - 16;
    n = D_00465A60 - n;
    for (i = 0; i < n; i++)
    {
        idx = D_00465A5C & 63;
        h = &D_0039985C[idx];
        D_00465A5C++;
        data = (short *)h->lpData;
        h->dwFlags = 0;

        count = 0;
        frac = 0.0f;
        lp = left;
        rp = right;
        src = data;
        for (k = 0; k < 510; k += 2)
        {
            frac += 0.17687f;
            count++;
            *lp++ = src[0];
            *rp++ = src[1];
            if (frac >= 1.0f)
            {
                frac -= 1.0f;
                count += 2;
                frac += 0.17687f;
                *lp++ = src[0] + (src[2] - src[0]) / 3;
                *rp++ = src[1] + (src[3] - src[1]) / 3;
                *lp++ = src[0] + (src[2] - src[0]) / 3 * 2;
                *rp++ = src[1] + (src[3] - src[1]) / 3 * 2;
            }
            else
            {
                count++;
                *lp++ = (src[0] + src[2]) / 2;
                *rp++ = (src[1] + src[3]) / 2;
            }
            src += 2;
        }
        count += 2;
        lp[0] = data[0x1FE];
        rp[0] = data[0x1FF];
        lp[1] = data[0x1FE];
        rp[1] = data[0x1FF];

        time = func_00229B88();

        lp2 = left;
        rp2 = right;
        for (k = 0; k < count; lp2++, rp2++, k++)
        {
            *D_00465A50->curleft = *lp2;
            *D_00465A50->curright = *rp2;
            D_00334EAC++;
            D_00334EAE++;
            D_00465A50->curleft++;
            D_00465A50->curright++;
            if (++D_00465A50->samples == 0x100)
            {
                D_00465A50->samples = 0;
                if (++D_00465A50->flag == 2)
                {
                    D_00465A50->flag = 0;
                    if (!D_00465A6C->inuse)
                    {
                        D_00465A50->hdr[D_00465A50->count]->addr = D_00465A6C->start;
                        D_00465A50->hdr[D_00465A50->count]->idx = idx;
                        D_00465A50->hdr[D_00465A50->count]->size = 0x800;
                        D_00465A50->hdr[D_00465A50->count]->last = 0;
                        D_00465A50->count++;
                        D_00465A6C->inuse = 1;
                        D_00465A6C->hdr = h;
                        D_00465A6C->time = time;
                        D_00465A6C->addr = addr;
                        D_00465A6C->state = func_002CED18(D_00465A6C->start, D_00465A6C->end, addr);
                        if (D_00465A6C->state == 2)
                            D_00465A6C->state = 0;
                        D_00465A6C->wrapped = 0;
                        D_00465A6C = D_00465A6C->next;
                    }
                    else
                    {
                        D_00465A50->hdr[D_00465A50->count]->addr = 0;
                        D_00465A50->hdr[D_00465A50->count]->idx = idx;
                        D_00465A50->hdr[D_00465A50->count]->size = 0x800;
                        memset(D_00465A50->data[D_00465A50->count], 0, 0x800);
                        D_00465A50->count++;
                    }

                    if (D_00465A50->count == 8)
                        func_002CE938();
                    else
                    {
                        D_00465A50->half++;
                        D_00465A50->curleft = D_00465A50->left[D_00465A50->half];
                        D_00465A50->curright = D_00465A50->right[D_00465A50->half];
                    }
                }
                else
                {
                    D_00465A50->half++;
                    D_00465A50->curleft = D_00465A50->left[D_00465A50->half];
                    D_00465A50->curright = D_00465A50->right[D_00465A50->half];
                }
            }
        }
    }

    func_002CE938();
    if (!D_00334EA8)
    {
        D_00334EA8 = 1;
        func_002CF380(0x3FFF);
        func_002CF3C8();
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/snd_win_gbx", func_002CE380);
#endif

/* func_002CE938: envia o buffer de stream cheio ao IOP e troca para o outro */
void func_002CE938(void)
{
    sndstream_t *next;
    int size;

    if (D_00465A50->count <= 0)
        return;

    D_00465A50->hdr[D_00465A50->count - 1]->last = 1;
    D_00465A54 = (D_00465A54 + 1) % 2;
    next = &D_00465898[D_00465A54];

    if (D_00465A50->flag > 0)
    {
        memcpy(next->left[next->half], D_00465A50->left[D_00465A50->half - 1], 0x200);
        memcpy(next->right[next->half], D_00465A50->right[D_00465A50->half - 1], 0x200);
        next->half++;
    }
    next->flag = D_00465A50->flag;

    if (D_00465A50->samples > 0)
    {
        memcpy(next->left[next->half], D_00465A50->left[D_00465A50->half], D_00465A50->samples * 2);
        memcpy(next->right[next->half], D_00465A50->right[D_00465A50->half], D_00465A50->samples * 2);
        next->samples = D_00465A50->samples;
        next->curleft = next->left[next->half] + D_00465A50->samples;
        next->curright = next->right[next->half] + D_00465A50->samples;
    }
    else
    {
        next->curleft = next->left[next->half];
        next->curright = next->right[next->half];
    }

    size = (D_00465A50->count << 11) + (D_00465A50->count << 4);
    func_002EF8E0(0);
    while (func_002F2890(D_0032EF40))
        ;
    func_002F26A0(D_0032EF40, 0x1F5, 1, D_00465A50->buf, size, 0, 0, 0, 0);

    D_00465A50->curleft = D_00465A50->left[0];
    D_00465A50->curright = D_00465A50->right[0];
    D_00465A50->half = 0;
    D_00465A50->samples = 0;
    D_00465A50->count = 0;
    D_00465A50->flag = 0;
    D_00465A50 = next;
}

void func_002CECC0(void)
{
    func_002CE158();
}

void func_002CECE0(void)
{
    if (!COM_CheckParm("-nosound"))
        func_002CF378(0x21800);
}

INCLUDE_RODATA("asm/nonmatchings/engine/snd_win_gbx", D_00396208);

int func_002CED18(unsigned int start, unsigned int end, unsigned int pos)
{
    int r;

    if (pos <= start)
        r = 0;
    else if (pos >= end)
        r = 1;
    else
        r = 2;
    return r;
}

int func_002CED40(sndblk_t *blk, unsigned int pos)
{
    int r;
    int result;

    result = 0;
    r = func_002CED18(blk->start, blk->end, pos);
    if (blk->state != 1)
    {
        if (blk->state == 0)
        {
            if (r == 1)
                result = 1;
            else if (r == 0 && blk->wrapped == 1)
                result = 1;
        }
    }
    else if (r == 1 && blk->wrapped == 1)
        result = 1;
    return result;
}

void func_002CEDD8(unsigned int pos)
{
    sndblk_t *blk;
    int r;

    for (blk = D_00465A68; blk->inuse == 1; blk = blk->next)
    {
        if (pos < blk->addr)
            blk->wrapped = 1;
    }

    blk = D_00465A68;
    while (blk->inuse == 1)
    {
        r = func_002CED40(blk, pos);
        if (!r && blk->next->inuse == 1 && func_002CED40(blk->next, pos) == 1)
            r = 1;
        if (r != 1)
        {
            D_00465A68 = blk;
            return;
        }
        blk->hdr->dwFlags |= 1;
        blk->hdr = NULL;
        blk->inuse = 0;
        func_00229B88();
        blk = blk->next;
    }
    D_00465A68 = blk;
}

#ifdef NON_MATCHING
signed char func_002CEEE0(void)
{
    int i;

    shm = &D_00551730;
    shm->channels = 2;
    shm->samplebits = 16;
    shm->speed = 11025;
    shm->dmaspeed = 22050;
    D_00465A5C = 0;
    D_00465A60 = 0;

    D_00399858 = WAV_BUFFERS * WAV_BUFFER_SIZE;

    D_00399860 = Mem_DbgMalloc(D_00399858, D_00396208, 899);
    D_00399864 = D_00399860;
    memset(D_00399864, 0, D_00399858);

    D_00399854 = Mem_DbgMalloc(sizeof(wavehdr_t) * WAV_BUFFERS, D_00396208, 908);
    D_0039985C = D_00399854;
    memset(D_0039985C, 0, sizeof(wavehdr_t) * WAV_BUFFERS);

    for (i = 0; i < WAV_BUFFERS; i++)
    {
        D_0039985C[i].lpData = D_00399864 + i * WAV_BUFFER_SIZE;
        D_0039985C[i].dwFlags = 0;
    }

    shm->soundalive = 1;
    shm->splitbuffer = 0;
    shm->samples = D_00399858 / (shm->samplebits / 8);
    shm->samplepos = 0;
    shm->submission_chunk = 1;
    shm->buffer = D_00399864;
    D_00334E9C = 1;
    D_00465A58 = (shm->samplebits / 8) - 1;
    func_002CF080();
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/snd_win_gbx", func_002CEEE0);
#endif

/* func_002CF080: aloca os blocos do SPU e os buffers de stream */
#ifdef NON_MATCHING
void func_002CF080(void)
{
    int args[4];
    int i;
    int n;
    int j;
    char *p;

    func_002CF380(0);
    D_00465A70 = func_002F3B08(0x21800);
    func_002CF378(0x21800);
    D_00465A64 = Mem_DbgMalloc(67 * sizeof(sndblk_t), D_00396208, 943);

    for (n = 0; n < 67; n++)
    {
        D_00465A64[n].id = n;
        D_00465A64[n].inuse = 0;
        D_00465A64[n].hdr = NULL;
        D_00465A64[n].start = D_00465A70 + n * 0x800;
        D_00465A64[n].end = D_00465A70 + n * 0x800 + 0x800;
        D_00465A64[n].addr = 0;
        if (n < 66)
            D_00465A64[n].next = &D_00465A64[n + 1];
        else
            D_00465A64[n].next = D_00465A64;
    }

    D_00465A6C = D_00465A64;
    D_00465A68 = D_00465A64;

    args[0] = D_00465A70;
    args[1] = 0x21800;
    func_002EF8E0(0);
    while (func_002F2890(D_0032EF40))
        ;
    func_002F26A0(D_0032EF40, 0x1F4, 0, args, 16, 0, 0, 0, 0);
    while (func_002F2890(D_0032EF40))
        ;

    D_00465A54 = 0;
    D_00465A50 = D_00465898;

    for (i = 0; i < 2; i++)
    {
        p = func_00225D48(0x4080, 0x40, D_00396208, 996);
        memset(p, 0, 0x4080);
        D_00465898[i].buf = p;
        for (j = 0; j < 8; j++)
        {
            D_00465898[i].data[j] = p + j * 0x800 + (j + 1) * 16;
            D_00465898[i].hdr[j] = (sndhdr_t *)(p + j * 0x800 + j * 16);
            D_00465898[i].left[j * 2] = (short *)D_00465898[i].data[j];
            D_00465898[i].right[j * 2] = D_00465898[i].left[j * 2] + 0x100;
            D_00465898[i].right[j * 2 + 1] = D_00465898[i].left[j * 2] + 0x300;
            D_00465898[i].left[j * 2 + 1] = D_00465898[i].left[j * 2] + 0x200;
        }
        D_00465898[i].curleft = D_00465898[i].left[0];
        D_00465898[i].curright = D_00465898[i].right[0];
        D_00465898[i].samples = 0;
        D_00465898[i].half = 0;
        D_00465898[i].flag = 0;
        D_00465898[i].count = 0;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/snd_win_gbx", func_002CF080);
#endif

void func_002CF378(int unused)
{
}

void func_002CF380(int vol)
{
    vol &= 0xFFFF;
    func_002D63A0(1, 0x8010, 0xF80, vol);
    func_002D63A0(1, 0x8010, 0x1080, vol);
}

void func_002CF3C8(void)
{
    func_002EF8E0(0);
    func_002D63A0(0, 0x80E0, 0, 0x10, D_00465A70, 0x21800);
}
