#include "common.h"
#include "engine/cmd.h"
#include "engine/sound.h"
#include "engine/cvar.h"
#include "engine/client.h"
#include "engine/server.h"
#include "engine/model.h"
#include "engine/cl_entity.h"
#include "engine/mathlib.h"

/* snd_dma.c do HL (versão PS2: três listas de sfx, global/jogo/fase) */


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
extern cvar_t D_00334D40;        /* volume */
extern cvar_t D_00334D58;
extern cvar_t D_00334D70;
extern cvar_t hisound;
extern cvar_t loadas8bit;
extern cvar_t D_00334E00;
extern cvar_t D_00334E18;
extern cvar_t D_00334E30;
extern cvar_t D_00334E48;
extern cvar_t D_00334E60;
extern cvar_t D_00334E78;
extern int D_00334D30;          /* snd_initialized */
extern sfx_t *D_00465870;       /* known_sfx */
extern cvar_t D_00334DB8;       /* nosound */
extern cvar_t D_00334DD0;       /* precache */
extern int D_0039984C;          /* paintedtime */
extern cvar_t D_00334D88;       /* stereo? (value em 0x334D94) */
extern cl_entity_t *cl_entities[2];
extern vec3_t D_00551760[2];    /* listener_origin por jogador */
extern vec3_t D_00551778;       /* listener_right */
extern vec3_t D_005517A0;       /* listener_origin */
void func_002CA320(channel_t *ch);
typedef struct
{
    int left;
    int right;
} portable_samplepair_t;
extern portable_samplepair_t D_00542D70[]; /* paintbuffer */
extern char *rgpszrawsentence[];
extern int D_0039982C;          /* cszrawsentences */
extern int D_00334E98;          /* hash do S_Say */
extern int D_00557634;          /* fakedma/precaching */
unsigned long func_00229B88(void);
void func_002C99B0(int count, int, int, int); /* SX_RoomFX */
char *func_002416A8(char *s, char c); /* Q_strrchr */
float Q_atof(char *str);
extern int D_00334E90;          /* hash do play */
extern int D_00334E94;          /* hash do playvol */
extern int D_00334E8C;          /* buffers */
extern int D_0046586C;          /* oldsamplepos */
extern int D_00399848;          /* soundtime */
int func_002E0F18(float f);
void func_002C5A30(void);
void func_0021C520(void);
void func_002C7FF8(int endtime, int clientId);
void func_002CE380(void);
void func_002CDA48(int clientId);
extern int D_00334D28;          /* snd_blocked */
extern vec3_t D_00553DC8;       /* listener_forward */
extern vec3_t D_00555DE8;       /* listener_up */
extern vec3_t D_00553DB0;       /* forward do jogador 0 */
extern vec3_t D_00551788;       /* right do jogador 0 */
extern vec3_t D_00551718;       /* up do jogador 0 */
void func_002CD450(void);
void func_002CECE0(void);
mleaf_t *Mod_PointInLeaf(vec_t *p, model_t *model);
extern float host_frametime;
extern sfx_t *D_00553DD8[NUM_AMBIENTS]; /* ambient_sfx */
extern char D_00395F80[];       /* "Warning: pitch shift ignored on stream sound %s\n" */
extern float D_00334D38;        /* sound_nominal_clip_dist */
void func_002CA7E8(channel_t *ch, char *name, int clientId);
void func_002CA700(channel_t *ch, int pitch);
void func_002CB290(channel_t *ch, sfxcache_t *sc);
void func_002C9E48(int entnum, int entchannel, int clientId);
int RandomLong(int lLow, int lHigh);

char *strstr(const char *, const char *);
sfxcache_t *S_LoadSound(sfx_t *s, channel_t *ch);
void func_002C7660(channel_t *ch, int clientId);

int strcasecmp(const char *, const char *);
void func_002CA018(void);
void func_002C8218(void);
void func_002C84C0(void);
void func_002C9C10(void);
void func_002CD340(int, int);
void func_002CECC0(void);
void func_00231D68(float);
void func_00231CB8(float);
void func_002CDB18(void);
void func_002CDC20(void);
void func_002CDDD8(void);
void func_002CD3F8(void);
void func_002CB7B0(void);
void func_002CBAA8(void);

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

void S_Init(int arg)
{
    Con_DPrintf("\nSound Initialization\n");
    func_002CA018();

    if (COM_CheckParm(D_00395D18))
        return;

    Cmd_AddCommand("play", func_002CDB18);
    Cmd_AddCommand("playvol", func_002CDC20);
    Cmd_AddCommand("speak", func_002CDDD8);
    Cmd_AddCommand("stopsound", func_002CD3F8);
    Cmd_AddCommand("soundreport", func_002CB7B0);
    Cmd_AddCommand("soundinfo", func_002CBAA8);

    Cvar_RegisterVariable(&D_00334D58);
    Cvar_RegisterVariable(&D_00334D70);
    Cvar_RegisterVariable(&D_00334D40);
    Cvar_RegisterVariable(&D_00334D88);
    Cvar_RegisterVariable(&D_00334DB8);
    Cvar_RegisterVariable(&hisound);
    Cvar_RegisterVariable(&D_00334DD0);
    Cvar_RegisterVariable(&loadas8bit);
    Cvar_RegisterVariable(&D_00334E00);
    Cvar_RegisterVariable(&D_00334E18);
    Cvar_RegisterVariable(&D_00334E30);
    Cvar_RegisterVariable(&D_00334E48);
    Cvar_RegisterVariable(&D_00334E60);
    Cvar_RegisterVariable(&D_00334E78);

    D_00334D30 = 1;

    func_002CBBB0();
    func_002C8218();

    D_00465870 = Mem_DbgMalloc(MAX_SFX * sizeof(sfx_t), "engine/snd_dma_gbx.c", 340);
    memset(D_00465870, 0, MAX_SFX * sizeof(sfx_t));
    D_00465874 = D_00465870;
    D_00465878 = NULL;
    D_0046587C = NULL;
    D_00465890 = 0;
    D_0046588C = 0;
    D_00465888 = 0;
    D_00465880 = 0;

    Con_DPrintf("Sound sampling rate: %i\n", shm->speed);

    func_002CD340(1, arg);
    func_002C84C0();
    func_002C9C10();
    func_00231D68(D_00334D40.value);
}

void func_002CBE10(void)
{
    func_00231CB8(D_00334D58.value);
    func_00231D68(D_00334D40.value);
}

void func_002CBE40(void)
{
    if (!D_00334D3C)
        return;

    if (shm)
        shm->gamealive = 0;

    shm = NULL;
    D_00334D3C = 0;
    func_002CECC0();
}

sfx_t *func_002CBE80(char *name, int *pfInCache)
{
    int i;
    sfx_t *sfx;

    sfx = NULL;

    for (i = 0; i < D_0046588C; i++)
    {
        if (!strcasecmp(D_00465874[i].name, name))
        {
            if (pfInCache)
                *pfInCache = 1;
            return &D_00465874[i];
        }
    }

    for (i = 0; i < D_00465888; i++)
    {
        if (!strcasecmp(D_00465878[i].name, name))
        {
            if (pfInCache)
                *pfInCache = 1;
            return &D_00465878[i];
        }
    }

    for (i = 0; i < D_00465880; i++)
    {
        if (!strcasecmp(D_0046587C[i].name, name))
        {
            if (pfInCache)
            {
                if (!Cache_Check(&D_0046587C[i].cache))
                    *pfInCache = 0;
                else
                    *pfInCache = 1;
            }
            D_0046587C[i].unk_4C = cl[0].servercount;
            return &D_0046587C[i];
        }

        if (D_0046587C[i].unk_4C != cl[0].servercount)
            sfx = &D_0046587C[i];
    }

    if (!sfx)
    {
        if (D_00465880 < D_00465884)
        {
            sfx = &D_0046587C[D_00465880];
            memset(sfx, 0, sizeof(sfx_t));
            D_00465880++;
            D_00465890++;
        }
    }
    else
    {
        if (Cache_Check(&sfx->cache))
            Cache_Free(&sfx->cache);
    }

    if (sfx)
    {
        strcpy(sfx->name, name);
        sfx->servercount = 2;
        sfx->unk_4C = cl[0].servercount;
        if (pfInCache)
            *pfInCache = 0;
    }
    return sfx;
}

sfx_t *S_PrecacheSound(char *name)
{
    sfx_t *sfx;

    if (!D_00334D3C || D_00334DB8.value)
        return NULL;

    if (name[0] == '*' || name[0] == '!')
        return func_002CBE80(name, NULL);

    sfx = func_002CBE80(name, NULL);
    if (!D_00334DD0.value)
        return sfx;

    S_LoadSound(sfx, NULL);
    return sfx;
}

int func_002CC198(sfx_t *sfx)
{
    int i;

    for (i = NUM_AMBIENTS; i < D_00399844; i++)
    {
        if (channels[i].sfx == sfx)
            return 1;
    }
    return 0;
}

#ifdef NON_MATCHING
channel_t *func_002CC1E8(int entnum, int entchannel, sfx_t *sfx, int clientId)
{
    int ch_idx;
    int first_to_die;
    int life_left;
    channel_t *target_chan;

    if (entchannel == CHAN_STREAM && func_002CC198(sfx))
        return NULL;

    first_to_die = -1;
    life_left = 0x7FFFFFFF;

    for (ch_idx = NUM_AMBIENTS; ch_idx < MAX_DYNAMIC_CHANNELS; ch_idx++)
    {
        if (channels[ch_idx].entchannel == CHAN_STREAM && D_00553DE8[ch_idx].buffer)
        {
            if (entchannel == CHAN_VOICE)
                return NULL;
            continue;
        }

        if (entchannel != CHAN_AUTO && channels[ch_idx].entnum == entnum &&
            (channels[ch_idx].entchannel == entchannel || entchannel == -1))
        {
            first_to_die = ch_idx;
            break;
        }

        if (channels[ch_idx].entnum == cl[clientId].viewentity && entnum != cl[clientId].viewentity &&
            channels[ch_idx].sfx)
            continue;

        if (channels[ch_idx].end - D_0039984C < life_left)
        {
            if (channels[ch_idx].unk_40 == 1)
                continue;
            life_left = channels[ch_idx].end - D_0039984C;
            first_to_die = ch_idx;
        }
    }

    if (first_to_die == -1)
        return NULL;

    target_chan = &channels[first_to_die];
    if (target_chan->sfx)
    {
        func_002C7660(target_chan, clientId);
        target_chan->sfx = NULL;
    }
    return target_chan;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", func_002CC1E8);
#endif

#ifdef NON_MATCHING
channel_t *func_002CC3B8(int entnum, int unused, sfx_t *sfx)
{
    int i;
    int found;
    channel_t *ch;

    strstr(sfx->name, "c0a0_tr_gmorn.wav");

    if (sfx->name[0] == '*' && func_002CC198(sfx))
        return NULL;

    found = 0;

    for (i = MAX_DYNAMIC_CHANNELS; i < D_00399844; i++)
    {
        if (channels[i].sfx == sfx && channels[i].entnum == entnum)
        {
            found = 1;
            break;
        }
    }

    if (found)
        return NULL;

    for (i = MAX_DYNAMIC_CHANNELS; i < D_00399844; i++)
    {
        if (!channels[i].sfx)
            break;
    }

    if (i < D_00399844)
        return &channels[i];

    if (D_00399844 != MAX_CHANNELS)
    {
        ch = &channels[D_00399844];
        D_00399844++;
        return ch;
    }

    Con_DPrintf("total_channels == MAX_CHANNELS\n");
    return NULL;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", func_002CC3B8);
#endif

#ifdef NON_MATCHING
void func_002CC510(channel_t *ch, int clientId)
{
    vec_t dot;
    vec_t dist;
    vec_t lscale, rscale, scale;
    vec3_t source_vec;
    cl_entity_t *pent;

    if (ch->entnum == cl[clientId].viewentity)
    {
        ch->rightvol = ch->leftvol = ch->master_vol;
        func_002CA320(ch);
        return;
    }

    if (ch->entnum > 0 && ch->entnum < sv.num_edicts)
    {
        pent = &cl_entities[clientId][ch->entnum];
        if (pent && pent->model)
        {
            VectorCopy(pent->origin, ch->origin);
            if (pent->model->type == mod_brush)
            {
                ch->origin[0] = ch->origin[0] + (pent->model->mins[0] + pent->model->maxs[0]) * 0.5f;
                ch->origin[1] = ch->origin[1] + (pent->model->mins[1] + pent->model->maxs[1]) * 0.5f;
                ch->origin[2] = ch->origin[2] + (pent->model->mins[2] + pent->model->maxs[2]) * 0.5f;
            }
        }
    }

    VectorSubtract(ch->origin, D_005517A0, source_vec);
    dist = VectorNormalize(source_vec) * ch->dist_mult;
    dot = DotProduct(D_00551778, source_vec);

    if (shm->channels == 1)
    {
        rscale = 1.0f;
        lscale = 1.0f;
    }
    else
    {
        rscale = 1.0f + dot;
        lscale = 1.0f - dot;
    }

    if (D_00334D88.value == 1.0f)
    {
        scale = (1.0f - dist) * rscale;
        ch->rightvol = (int)(ch->master_vol * scale);
        scale = (1.0f - dist) * lscale;
        ch->leftvol = (int)(ch->master_vol * scale);
    }
    else
    {
        lscale = (1.0f - dist) * lscale;
        rscale = (1.0f - dist) * rscale;
        scale = (lscale + rscale) * 0.5f;
        ch->rightvol = ch->leftvol = (int)(ch->master_vol * scale);
    }

    func_002CA320(ch);

    if (ch->rightvol < 0)
        ch->rightvol = 0;
    if (ch->leftvol < 0)
        ch->leftvol = 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", func_002CC510);
#endif

#ifdef NON_MATCHING
void func_002CC7A8(channel_t *ch)
{
    vec_t dot;
    vec_t dist;
    vec_t lscale, rscale;
    vec_t left, right, left2, right2;
    vec3_t source_vec;
    cl_entity_t *pent;

    if (ch->entnum == cl[0].viewentity || ch->entnum == cl[1].viewentity)
    {
        ch->rightvol = ch->leftvol = ch->master_vol;
        func_002CA320(ch);
        return;
    }

    if (ch->entnum > 0 && ch->entnum < sv.num_edicts)
    {
        pent = &cl_entities[0][ch->entnum];
        if (pent && pent->model)
        {
            VectorCopy(pent->origin, ch->origin);
            if (pent->model->type == mod_brush)
            {
                ch->origin[0] = ch->origin[0] + (pent->model->mins[0] + pent->model->maxs[0]) * 0.5f;
                ch->origin[1] = ch->origin[1] + (pent->model->mins[1] + pent->model->maxs[1]) * 0.5f;
                ch->origin[2] = ch->origin[2] + (pent->model->mins[2] + pent->model->maxs[2]) * 0.5f;
            }
        }
    }

    VectorSubtract(ch->origin, D_00551760[0], source_vec);
    dist = VectorNormalize(source_vec) * ch->dist_mult;
    dot = DotProduct(D_00551778, source_vec);

    if (shm->channels == 1)
    {
        rscale = 1.0f;
        lscale = 1.0f;
    }
    else
    {
        lscale = 1.0f - dot;
        rscale = 1.0f + dot;
    }

    left = (1.0f - dist) * lscale;
    right = (1.0f - dist) * rscale;

    VectorSubtract(ch->origin, D_00551760[1], source_vec);
    dist = VectorNormalize(source_vec) * ch->dist_mult;
    dot = DotProduct(D_00551778, source_vec);

    if (shm->channels == 1)
    {
        rscale = 1.0f;
        lscale = 1.0f;
    }
    else
    {
        lscale = 1.0f - dot;
        rscale = 1.0f + dot;
    }

    left2 = (1.0f - dist) * lscale;
    right2 = (1.0f - dist) * rscale;

    left = left2 > left ? left2 : left;
    right = right2 > right ? right2 : right;

    if (D_00334D88.value == 0.0f)
    {
        ch->rightvol = ch->leftvol = (int)(ch->master_vol * ((left + right) * 0.5f));
    }
    else
    {
        ch->leftvol = (int)(ch->master_vol * left);
        ch->rightvol = (int)(ch->master_vol * right);
    }

    func_002CA320(ch);

    if (ch->rightvol < 0)
        ch->rightvol = 0;
    if (ch->leftvol < 0)
        ch->leftvol = 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", func_002CC7A8);
#endif

int func_002CCB10(int entnum, int entchannel, sfx_t *sfx, int vol, int pitch, int flags, int clientId)
{
    int ch_idx;

    if (sfx->name[0] == '!')
    {
        for (ch_idx = NUM_AMBIENTS; ch_idx < D_00399844; ch_idx++)
        {
            if (channels[ch_idx].entnum == entnum && channels[ch_idx].entchannel == entchannel
                && channels[ch_idx].sfx && channels[ch_idx].isentence >= 0)
            {
                if (flags & 0x80)
                    channels[ch_idx].pitch = pitch;
                if (flags & 0x40)
                    channels[ch_idx].master_vol = vol;
                if (flags & 0x20)
                    func_002C7660(&channels[ch_idx], clientId);
                return 1;
            }
        }
        return 0;
    }

    for (ch_idx = NUM_AMBIENTS; ch_idx < D_00399844; ch_idx++)
    {
        if (channels[ch_idx].entnum == entnum && channels[ch_idx].entchannel == entchannel
            && channels[ch_idx].sfx == sfx)
        {
            if (flags & 0x80)
                channels[ch_idx].pitch = pitch;
            if (flags & 0x40)
                channels[ch_idx].master_vol = vol;
            if (flags & 0x20)
                func_002C7660(&channels[ch_idx], clientId);
            return 1;
        }
    }
    return 0;
}

INCLUDE_RODATA("asm/nonmatchings/engine/snd_dma", D_00395F80);

void S_StartDynamicSound(int entnum, int entchannel, sfx_t *sfx, vec3_t origin, float fvol, float attenuation, int flags, int pitch, int clientId)
{
    channel_t *target_chan, *check;
    sfxcache_t *sc;
    int vol;
    int ch_idx;
    int skip;
    int fsentence;
    sfx_t *s;
    char name[64];

    if (!D_00334D3C)
        return;
    if (!sfx)
        return;
    if (D_00334DB8.value != 0.0f)
        return;

    if (sfx->name[0] == '*')
    {
        s = func_002CB9A0(sfx->name + 1);
        if (!s)
            entchannel = CHAN_STREAM;
        else
            sfx = s;
    }

    if (entchannel == CHAN_STREAM && pitch != 100)
    {
        Con_DPrintf(D_00395F80, sfx);
        pitch = 100;
    }

    vol = (int)(fvol * 255.0f);
    if (vol > 255)
    {
        Con_DPrintf("S_StartDynamicSound: %s volume > 255", sfx);
        vol = 255;
    }

    if (flags & 0xE0)
    {
        if (func_002CCB10(entnum, entchannel, sfx, vol, pitch, flags, clientId))
            return;
        if (flags & 0x20)
            return;
    }

    if (pitch == 0)
    {
        Con_DPrintf("Warning: S_StartDynamicSound Ignored, called with pitch 0");
        return;
    }

    target_chan = func_002CC1E8(entnum, entchannel, sfx, clientId);
    if (!target_chan)
        return;

    fsentence = sfx->name[0] == '!';

    memset(target_chan, 0, sizeof(*target_chan));
    VectorCopy(origin, target_chan->origin);
    target_chan->dist_mult = attenuation / D_00334D38;
    target_chan->master_vol = vol;
    target_chan->isentence = -1;
    target_chan->entnum = entnum;
    target_chan->entchannel = entchannel;
    target_chan->pitch = pitch;

    if (cl_numlocal >= 2)
        func_002CC7A8(target_chan);
    else
        func_002CC510(target_chan, clientId);

    if (!target_chan->leftvol && !target_chan->rightvol && entchannel != CHAN_STREAM)
    {
        target_chan->sfx = NULL;
        return;
    }

    if (fsentence)
    {
        Q_strcpy(name, sfx->name + 1);
        func_002CA7E8(target_chan, name, clientId);
        target_chan->unk_42 = 0;
        return;
    }

    sc = S_LoadSound(sfx, target_chan);
    target_chan->sfx = sfx;
    if (!sc)
    {
        target_chan->sfx = NULL;
        return;
    }

    target_chan->pos = 0;
    target_chan->end = D_0039984C + sc->length;

    if (pitch != 100)
        func_002CA700(target_chan, pitch);
    func_002CB290(target_chan, sc);
    func_002C9E48(entnum, entchannel, clientId);

    check = &channels[NUM_AMBIENTS];
    for (ch_idx = NUM_AMBIENTS; ch_idx < NUM_AMBIENTS + 8; ch_idx++, check++)
    {
        if (check == target_chan)
            continue;
        if (check->sfx == sfx && !check->pos)
        {
            skip = RandomLong(0, (int)(shm->speed * 0.1f));
            if (skip >= target_chan->end)
                skip = target_chan->end - 1;
            target_chan->pos += skip;
            target_chan->end -= skip;
            break;
        }
    }
}

#ifdef NON_MATCHING
void S_StartStaticSound(int entnum, int entchannel, sfx_t *sfx, vec3_t origin, float fvol, float attenuation, int flags, int pitch, int clientId)
{
    channel_t *ss;
    sfxcache_t *sc;
    int vol;
    char name[64];

    if (!sfx)
        return;

    if (sfx->name[0] == '*')
        entchannel = CHAN_STREAM;

    if (entchannel == CHAN_STREAM && pitch != 100)
    {
        Con_DPrintf(D_00395F80, sfx);
        pitch = 100;
    }

    vol = (int)(fvol * 255.0f);
    if (vol > 255)
    {
        Con_DPrintf("S_StartStaticSound: %s volume > 255", sfx);
        vol = 255;
    }

    if (flags & 0xE0)
    {
        if (func_002CCB10(entnum, entchannel, sfx, vol, pitch, flags, clientId))
            return;
        if (flags & 0x20)
            return;
    }

    if (pitch == 0)
    {
        Con_DPrintf("Warning: S_StartStaticSound Ignored, called with pitch 0");
        return;
    }

    ss = func_002CC3B8(entnum, entchannel, sfx);
    if (!ss)
        return;

    if (sfx->name[0] == '!')
    {
        Q_strcpy(name, sfx->name + 1);
        func_002CA7E8(ss, name, clientId);
        ss->unk_42 = 1;
        VectorCopy(origin, ss->origin);
        ss->entchannel = entchannel;
        ss->master_vol = vol;
        ss->dist_mult = attenuation / D_00334D38;
        ss->pitch = pitch;
        ss->entnum = entnum;
        ss->pos = 0;
        return;
    }

    sc = S_LoadSound(sfx, ss);
    ss->sfx = sfx;
    ss->isentence = -1;
    if (!sc)
    {
        ss->sfx = NULL;
        return;
    }

    VectorCopy(origin, ss->origin);
    ss->master_vol = vol;
    ss->dist_mult = attenuation / D_00334D38;
    ss->pos = 0;
    ss->end = D_0039984C + sc->length;
    ss->entnum = entnum;
    ss->entchannel = entchannel;
    ss->pitch = pitch;

    if (pitch != 100)
        func_002CA700(ss, pitch);
    func_002CB290(ss, sc);

    if (cl_numlocal >= 2)
        func_002CC7A8(ss);
    else
        func_002CC510(ss, clientId);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", S_StartStaticSound);
#endif

void func_002CD280(int entnum, int entchannel, int clientId)
{
    int i;

    for (i = NUM_AMBIENTS; i < D_00399844; i++)
    {
        if (channels[i].entnum == entnum && channels[i].entchannel == entchannel)
            func_002C7660(&channels[i], clientId);
    }
}

void func_002CD340(int clear, int clientId)
{
    int i;

    if (!D_00334D3C)
        return;

    D_00399844 = NUM_AMBIENTS + 8;

    for (i = 0; i < MAX_CHANNELS; i++)
    {
        if (channels[i].sfx)
            func_002C7660(&channels[i], clientId);
    }

    memset(channels, 0, sizeof(channels));
    func_002C9C10();

    if (clear)
        func_002CD450();
}

void func_002CD3F8(void)
{
    int i;

    for (i = 0; i < cl_numlocal; i++)
        func_002CD340(1, i);
}

void func_002CD450(void)
{
    int clear;

    if (!D_00334D3C || !shm || !shm->buffer)
        return;

    if (shm->samplebits == 8)
        clear = 0x80;
    else
        clear = 0;

    memset(shm->buffer, clear, shm->samples * shm->samplebits / 8);
    func_002CECE0();
}

void func_002CD4D0(int clientId)
{
    mleaf_t *l;
    float vol;
    int ambient;
    channel_t *chan;

    if (!D_00334D2C)
        return;

    if (!cl[clientId].worldmodel)
        return;

    l = Mod_PointInLeaf(D_005517A0, cl[clientId].worldmodel);
    if (!l || !D_00334E18.value)
    {
        for (ambient = 0; ambient < NUM_AMBIENTS; ambient++)
            channels[ambient].sfx = NULL;
        return;
    }

    for (ambient = 0; ambient < NUM_AMBIENTS; ambient++)
    {
        chan = &channels[ambient];
        chan->sfx = D_00553DD8[ambient];

        vol = 0;

        if (chan->master_vol < vol)
        {
            chan->master_vol += host_frametime * D_00334E30.value;
            if (chan->master_vol > vol)
                chan->master_vol = vol;
        }
        else if (chan->master_vol > vol)
        {
            chan->master_vol -= host_frametime * D_00334E30.value;
            if (chan->master_vol < vol)
                chan->master_vol = vol;
        }

        chan->leftvol = chan->rightvol = chan->master_vol;
    }
}

void S_Update(vec3_t origin, vec3_t forward, vec3_t right, vec3_t up, int clientId)
{
    int i;
    channel_t *ch;

    if (clientId == 1)
        return;
    if (!D_00334D3C)
        return;
    if (D_00334D28 > 0)
        return;

    VectorCopy(origin, D_005517A0);
    VectorCopy(forward, D_00553DC8);
    VectorCopy(right, D_00551778);
    VectorCopy(up, D_00555DE8);

    func_002CD4D0(clientId);

    ch = &channels[NUM_AMBIENTS];
    for (i = NUM_AMBIENTS; i < D_00399844; i++, ch++)
    {
        if (!ch->sfx)
            continue;
        func_002CC510(ch, clientId);
    }

    func_002CDA48(clientId);
}

void func_002CD788(void)
{
    int i;
    channel_t *ch;

    if (!D_00334D3C)
        return;
    if (D_00334D28 > 0)
        return;

    VectorCopy(D_00551760[0], D_005517A0);
    VectorCopy(D_00553DB0, D_00553DC8);
    VectorCopy(D_00551788, D_00551778);
    VectorCopy(D_00551718, D_00555DE8);

    func_002CD4D0(0);

    ch = &channels[NUM_AMBIENTS];
    for (i = NUM_AMBIENTS; i < D_00399844; i++, ch++)
    {
        if (ch->sfx)
            func_002CC7A8(ch);
    }

    func_002CDA48(0);
}

void func_002CD8C8(void)
{
    int samplepos;
    int fullsamples;
    int i;

    fullsamples = shm->samples / shm->channels;

    samplepos = func_002CE338();
    if (samplepos < D_0046586C)
    {
        D_00334E8C++;

        if (D_0039984C > 0x40000000)
        {
            D_00334E8C = 0;
            D_0039984C = fullsamples;
            for (i = 0; i < cl_numlocal; i++)
                func_002CD340(1, i);
        }
    }
    D_0046586C = samplepos;

    D_00399848 = D_00334E8C * fullsamples + samplepos / shm->channels;
}

void func_002CD9F8(int clientId)
{
    func_002C5A30();
    func_0021C520();

    if (D_00334E48.value)
        return;

    func_002CDA48(clientId);
}

#ifdef NON_MATCHING
void func_002CDA48(int clientId)
{
    int endtime;
    int samps;
    int cid;

    cid = 0;
    if (clientId != 1)
        cid = clientId;

    if (!D_00334D3C || D_00334D28 > 0)
        return;

    func_002CD8C8();

    endtime = func_002E0F18(D_00399848 + D_00334E78.value * shm->dmaspeed);

    samps = shm->samples >> (shm->channels - 1);
    if (endtime - D_00399848 > samps)
        endtime = D_00399848 + samps;

    func_002C7FF8((unsigned int)endtime >> 1, cid);

    func_002CE380();
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/snd_dma", func_002CDA48);
#endif

void func_002CDB18(void)
{
    int i;
    char name[256];
    sfx_t *sfx;

    i = 1;
    while (i < Cmd_Argc())
    {
        if (!func_002416A8(Cmd_Argv(i), '.'))
        {
            Q_strcpy(name, Cmd_Argv(i));
            Q_strcat(name, ".wav");
        }
        else
            Q_strcpy(name, Cmd_Argv(i));
        sfx = S_PrecacheSound(name);
        S_StartDynamicSound(D_00334E90++, 0, sfx, D_005517A0, 1.0f, 1.0f, 0, 100, 0);
        i++;
    }
}

void func_002CDC20(void)
{
    int i;
    float vol;
    char name[256];
    sfx_t *sfx;

    i = 1;
    while (i < Cmd_Argc())
    {
        if (!func_002416A8(Cmd_Argv(i), '.'))
        {
            Q_strcpy(name, Cmd_Argv(i));
            Q_strcat(name, ".wav");
        }
        else
            Q_strcpy(name, Cmd_Argv(i));
        sfx = S_PrecacheSound(name);
        vol = Q_atof(Cmd_Argv(i + 1));
        S_StartDynamicSound(D_00334E94++, 0, sfx, D_005517A0, vol, 1.0f, 0, 100, 0);
        i += 2;
    }
}


void func_002CDD38(char *sound)
{
    sfx_t *sfx;

    if (D_00334DB8.value)
        return;
    if (!D_00334D3C)
        return;

    sfx = S_PrecacheSound(sound);
    if (!sfx)
    {
        Con_Printf("S_LocalSound: can't cache %s\n", sound);
        return;
    }
    S_StartDynamicSound(cl[0].viewentity, -1, sfx, vec3_origin, 1.0f, 1.0f, 0, 100, 0);
}

void func_002CDDD8(void)
{
    sfx_t *sfx;
    char sound[256];
    int i;

    if (D_00334DB8.value)
        return;
    if (!D_00334D3C)
        return;

    Q_strcpy(sound, Cmd_Argv(1));

    if (!Q_strcmp(sound, "dsp"))
    {
        unsigned int start;

        for (i = 0; i < 512; i++)
        {
            D_00542D70[i].left = RandomLong(0, 2999);
            D_00542D70[i].right = RandomLong(0, 2999);
        }

        Con_Printf("Start profiling 10,000 calls to DSP\n");
        start = func_00229B88();
        for (i = 0; i < 10000; i++)
            func_002C99B0(512, 1, 1, 0);
        Con_Printf("%d milliseconds \n", func_00229B88() - start);
        return;
    }

    if (!Q_strcmp(sound, "paint"))
    {
        unsigned int start;
        int ptime;

        ptime = D_0039984C;
        Con_Printf("Start profiling S_PaintChannels\n");
        sfx = S_PrecacheSound("ambience/labdrone1.wav");
        S_StartDynamicSound(D_00334E98++, 0, sfx, D_005517A0, 1.0f, 1.0f, 0, 100, 0);
        start = func_00229B88();
        func_002C7FF8(D_0039984C + 512 * 10000, 0);
        Con_Printf("%d milliseconds \n", func_00229B88() - start);
        D_0039984C = ptime;
        return;
    }

    if (sound[0] != '!')
    {
        Q_strcpy(sound, "xxtestxx ");
        Q_strcat(sound, Cmd_Argv(1));
        rgpszrawsentence[D_0039982C] = sound;
        sound[8] = 0;
        D_0039982C++;

        sfx = S_PrecacheSound("!xxtestxx");
        if (!sfx)
        {
            Con_Printf("S_Say: can't cache %s\n", sound);
            return;
        }
        S_StartDynamicSound(cl[0].viewentity, -1, sfx, vec3_origin, 1.0f, 1.0f, 0, 100, 0);

        D_0039982C--;
        rgpszrawsentence[D_0039982C] = NULL;
    }
    else
    {
        sfx = func_002CBE80(sound, NULL);
        if (!sfx)
        {
            Con_Printf("S_Say: can't find sentence name %s\n", sound);
            return;
        }
        S_StartDynamicSound(cl[0].viewentity, -1, sfx, vec3_origin, 1.0f, 1.0f, 0, 100, 0);
    }
}

void S_BeginPrecaching(void)
{
    D_00557634 = 1;
}

void S_EndPrecaching(void)
{
    D_00557634 = 0;
}
