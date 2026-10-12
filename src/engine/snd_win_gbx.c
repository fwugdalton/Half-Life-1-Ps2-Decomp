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
    byte unk_14[0x28 - 0x14];
    byte id;                    /* 0x28 */
    byte inuse;                 /* 0x29 */
    struct sndblk_s *next;      /* 0x2C */
    wavehdr_t *hdr;             /* 0x30 */
    int unk_34;
} sndblk_t;

#define WAV_BUFFERS 64
#define WAV_BUFFER_SIZE 0x400

void *memset(void *, int, unsigned int);
int COM_CheckParm(char *parm);
unsigned long func_00229B88(void);
void func_002D63A0(int, int, int, int, ...);
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

INCLUDE_ASM("asm/nonmatchings/engine/snd_win_gbx", func_002CE380);

INCLUDE_ASM("asm/nonmatchings/engine/snd_win_gbx", func_002CE938);

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

INCLUDE_ASM("asm/nonmatchings/engine/snd_win_gbx", func_002CF080);

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
