#include "common.h"
#include "engine/cmd.h"

/* snd_dma.c do HL (versão PS2: três listas de sfx, global/jogo/fase) */

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

typedef struct sfx_s
{
    char name[64];
    int servercount;            /* 0x40: 0 = global, 1 = jogo */
    int unk_44;
    cache_user_t cache;         /* 0x48 */
    int unk_4C;
} sfx_t;                        /* 0x50 */

#define MAX_SFX 512

typedef int FileHandle_t;

char *strcpy(char *, const char *);
void *memset(void *, int, unsigned int);
int COM_CheckParm(char *parm);
void *Cache_Check(cache_user_t *c);
void Cache_Free(cache_user_t *c);
FileHandle_t FS_Open(char *name, char *mode);
void FS_Close(FileHandle_t f);
void func_00222500(FileHandle_t f, char *fmt, ...);
int func_00228C48(sfx_t *sfx, char *name);
int func_002CE270(void);

extern char D_00395D18[];       /* "-nosound" */
extern volatile dma_t *shm;
extern int D_00334D2C;          /* snd_ambient */
extern int D_00334D3C;          /* sound_started */
extern int D_00399844;          /* total_channels */
extern sfx_t *D_00465874;       /* sfx globais */
extern sfx_t *D_00465878;       /* sfx do jogo */
extern sfx_t *D_0046587C;       /* sfx da fase */
extern int D_00465880;          /* num_sfx da fase */
extern int D_00465884;          /* espaço livre */
extern int D_00465888;          /* num_sfx do jogo */
extern int D_0046588C;          /* num_sfx globais */
extern int D_00465890;          /* num_sfx total */

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00395BF0);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00395BF8);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00395C08);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00395C10);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00395C18);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00395C20);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00395C30);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00395C38);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00395C48);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00395C50);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00395C58);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00395C60);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00395C68);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00395C70);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00395C80);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00395C90);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00395C98);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00395CA8);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00395CB0);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00395CC0);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00395CC8);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00395CD8);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00395CF0);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00395D00);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00395D08);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00395D18);

void func_002CB508(char *name, void *data)
{
    sfx_t *sfx;

    if (COM_CheckParm(D_00395D18))
        return;

    sfx = &D_00465874[D_0046588C];
    D_0046588C++;
    D_00465890++;
    strcpy(sfx->name, name);
    sfx->cache.data = data;
    sfx->unk_4C = 0;
    sfx->servercount = 0;
}

void func_002CB5A0(void)
{
    D_00465878 = &D_00465874[D_0046588C];
}

void func_002CB5C8(char *name, void *data)
{
    sfx_t *sfx;

    if (COM_CheckParm(D_00395D18))
        return;

    sfx = &D_00465878[D_00465888];
    D_00465888++;
    D_00465890++;
    strcpy(sfx->name, name);
    sfx->cache.data = data;
    sfx->servercount = 1;
    sfx->unk_4C = 0;
}

void func_002CB660(void)
{
    int i;

    for (i = 0; i < D_00465880; i++)
    {
        if (Cache_Check(&D_0046587C[i].cache))
            Cache_Free(&D_0046587C[i].cache);
    }

    memset(D_00465878, 0, D_00465888 * sizeof(sfx_t));
    memset(D_0046587C, 0, D_00465880 * sizeof(sfx_t));

    D_00465890 = D_00465890 - D_00465880 - D_00465888;
    D_00465888 = 0;
    D_00465880 = 0;
    D_0046587C = NULL;
}

void func_002CB768(void)
{
    D_0046587C = &D_00465878[D_00465888];
    D_00465884 = MAX_SFX - (D_0046588C + D_00465888);
}

#ifdef NON_MATCHING
void func_002CB7B0(void)
{
    FileHandle_t f;
    int i;

    if (COM_CheckParm(D_00395D18))
        return;

    f = FS_Open("Sound Report.txt", "w");
    func_00222500(f, "Sounds Loaded: [%d]\r\n", D_00465890);
    func_00222500(f, "       Global: [%d]\r\n", D_0046588C);
    func_00222500(f, "         Game: [%d]\r\n", D_00465888);
    func_00222500(f, "        Level: [%d]\r\n", D_00465880);

    func_00222500(f, "\r\n\tGlobal Sounds\r\n");
    for (i = 0; i < D_0046588C; i++)
        func_00222500(f, "[%s]\r\n", D_00465874[i].name);

    func_00222500(f, "\r\n\tGame Sounds\r\n");
    for (i = 0; i < D_00465888; i++)
        func_00222500(f, "[%s]\r\n", D_00465878[i].name);

    func_00222500(f, "\r\n\tLevel Sounds\r\n");
    for (i = 0; i < D_00465880; i++)
        func_00222500(f, "[%s]\r\n", D_0046587C[i].name);

    FS_Close(f);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", func_002CB7B0);
#endif

sfx_t *func_002CB9A0(char *name)
{
    int i;
    sfx_t *sfx;

    sfx = NULL;
    for (i = 0; i < D_0046588C; i++)
    {
        if (func_00228C48(&D_00465874[i], name) == 1)
        {
            sfx = &D_00465874[i];
            break;
        }
    }

    if (!sfx)
    {
        for (i = 0; i < D_00465888; i++)
        {
            if (func_00228C48(&D_00465878[i], name) == 1)
            {
                sfx = &D_00465878[i];
                break;
            }
        }
    }
    return sfx;
}

void func_002CBA88(void)
{
    D_00334D2C = 0;
}

void func_002CBA98(void)
{
    D_00334D2C = 1;
}

void func_002CBAA8(void)
{
    if (!D_00334D3C || !shm)
    {
        Con_Printf("sound system not started\n");
        return;
    }

    Con_Printf("%5d stereo\n", shm->channels - 1);
    Con_Printf("%5d samples\n", shm->samples);
    Con_Printf("%5d samplepos\n", shm->samplepos);
    Con_Printf("%5d samplebits\n", shm->samplebits);
    Con_Printf("%5d submission_chunk\n", shm->submission_chunk);
    Con_Printf("%5d speed\n", shm->speed);
    Con_Printf("0x%x dma buffer\n", shm->buffer);
    Con_Printf("%5d total_channels\n", D_00399844);
}

void func_002CBBB0(void)
{
    func_002CE270();
    D_00334D3C = 1;
}

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", S_Init);

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", func_002CBE10);

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", func_002CBE40);

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", func_002CBE80);

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", S_PrecacheSound);

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", func_002CC198);

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", func_002CC1E8);

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", func_002CC3B8);

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", func_002CC510);

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", func_002CC7A8);

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", func_002CCB10);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00395F80);

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", S_StartDynamicSound);

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", S_StartStaticSound);

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", func_002CD280);

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", func_002CD340);

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", func_002CD3F8);

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", func_002CD450);

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", func_002CD4D0);

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", S_Update);

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", func_002CD788);

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", func_002CD8C8);

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", func_002CD9F8);

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", func_002CDA48);

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", func_002CDB18);

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", func_002CDC20);

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00396088);

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", func_002CDD38);

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", func_002CDDD8);

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", S_BeginPrecaching);

INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", S_EndPrecaching);
