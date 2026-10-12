#include "common.h"
#include "engine/cvar.h"
#include "engine/mathlib.h"
#include "engine/sound.h"
#include "engine/cl_entity.h"
#include "engine/client.h"
#include "engine/model.h"
#include "engine/cmd.h"
#include "engine/pmove.h"
#include "engine/server.h"

#define FTENT_SINEWAVE      0x00000001
#define FTENT_GRAVITY       0x00000002
#define FTENT_ROTATE        0x00000004
#define FTENT_SLOWGRAVITY   0x00000008
#define FTENT_SMOKETRAIL    0x00000010
#define FTENT_COLLIDEWORLD  0x00000020
#define FTENT_FLICKER       0x00000040
#define FTENT_FADEOUT       0x00000080
#define FTENT_SPRCYCLE      0x00000800
#define FTENT_NOMODEL_HACK  0x00020000  /* PS2: peça de BreakModel contada em D_003FE298 */
#define FTENT_SPRANIMATE    0x00000100
#define FTENT_SPIRAL 0x400
#define FTENT_COLLIDEALL    0x00001000
#define FTENT_PERSIST       0x00002000
#define FTENT_COLLIDEKILL   0x00004000
#define FTENT_PLYRATTACHMENT 0x00008000
#define FTENT_SPRANIMATELOOP 0x00010000

#define mod_sprite 1
#define mod_studio 3

#define BREAK_TYPEMASK      0x4F
#define BREAK_GLASS         0x01
#define BREAK_SMOKE         0x10
#define BREAK_TRANS         0x20

#define TEFIRE_FLAG_ALLFLOAT  0x01
#define TEFIRE_FLAG_SOMEFLOAT 0x02
#define TEFIRE_FLAG_LOOP      0x04
#define TEFIRE_FLAG_ALPHA     0x08
#define TEFIRE_FLAG_PLANAR    0x10

#define BOUNCE_GLASS        0x01
#define BOUNCE_METAL        0x02
#define BOUNCE_FLESH        0x04
#define BOUNCE_WOOD         0x08
#define BOUNCE_SHRAP        0x10
#define BOUNCE_SHELL        0x20
#define BOUNCE_CONCRETE     0x40
#define BOUNCE_SHOTSHELL    0x80

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#define SHARD_VOLUME        12.0f

enum
{
    kRenderFxNone, kRenderFxPulseSlow, kRenderFxPulseFast, kRenderFxPulseSlowWide, kRenderFxPulseFastWide,
    kRenderFxFadeSlow, kRenderFxFadeFast, kRenderFxSolidSlow, kRenderFxSolidFast, kRenderFxStrobeSlow,
    kRenderFxStrobeFast, kRenderFxStrobeFaster, kRenderFxFlickerSlow, kRenderFxFlickerFast, kRenderFxNoDissipation,
    kRenderFxDistort, kRenderFxHologram, kRenderFxDeadPlayer, kRenderFxExplode, kRenderFxGlowShell,
    kRenderFxClampMinScale, kRenderFxLightMultiplier
};

extern int D_003FE298[2];
extern cl_entity_t *cl_entities[2];
extern char D_0038AD88[];           /* "No modelindex for funnel!!\n" */
extern char D_0038ACF8[];           /* "No model %d!\n" (ainda usada por funções em asm) */           /* peças de BreakModel vivas por jogador */

enum { kRenderNormal, kRenderTransColor, kRenderTransTexture, kRenderGlow, kRenderTransAlpha, kRenderTransAdd };

/* TEMPENTITY do PS2 (entity embutida em 0x3C) */
typedef struct tempent_s
{
    int flags;                  /* 0x00 */
    float die;                  /* 0x04 */
    float frameMax;             /* 0x08 */
    float x;                    /* 0x0C */
    float y;                    /* 0x10 */
    float z;                    /* 0x14 */
    float fadeSpeed;            /* 0x18 */
    float bounceFactor;         /* 0x1C */
    int hitSound;               /* 0x20 */
    struct tempent_s *next;     /* 0x24 */
    int priority;               /* 0x28 */
    short clientIndex;          /* 0x2C */
    vec3_t tentOffset;          /* 0x30 */
    cl_entity_t entity;         /* 0x3C */
} TEMPENTITY;

#define MAX_TEMP_ENTITIES 350
#define TENTPRIORITY_LOW 0
#define TENTPRIORITY_HIGH 1

extern TEMPENTITY D_003ACCF8[2][MAX_TEMP_ENTITIES]; /* gTempEnts */
extern TEMPENTITY *D_003FE288[2];   /* tempents livres por jogador */
extern TEMPENTITY *D_003FE290[2];   /* tempents ativas por jogador */

/* entidades temporárias do cliente (engine/cl_tent.c): efeitos de TE_*, tracers, sprites e modelos quebrando */

extern cvar_t D_00331B40;           /* tracerspeed */
extern cvar_t D_00331B58;           /* traceroffset */
extern cvar_t D_00331B70;           /* tracerlength */
extern cvar_t D_00331B88;           /* tracerred */
extern cvar_t D_00331BA0;           /* tracergreen */
extern cvar_t D_00331BB8;           /* tracerblue */
extern cvar_t D_00331BD0;           /* traceralpha */

extern sfx_t *D_003991DC;
extern sfx_t *D_00399230;
extern sfx_t *D_0039925C;
extern sfx_t *D_0039927C;
extern sfx_t *D_0039922C;
extern sfx_t *D_00399214;
extern sfx_t *D_00399238;
extern sfx_t *D_003991E8;
extern sfx_t *D_00399240;
extern sfx_t *D_003991EC;
extern sfx_t *D_00399218;
extern sfx_t *D_00399260;
extern sfx_t *D_00399210;
extern sfx_t *D_00399254;
extern sfx_t *D_00399248;
extern sfx_t *D_003991E0;
extern sfx_t *D_00399278;
extern sfx_t *D_0039923C;
extern sfx_t *D_003991C8;
extern sfx_t *D_0039921C;
extern sfx_t *D_003991F0;
extern sfx_t *D_003991D8;
extern sfx_t *D_00399258;
extern sfx_t *D_003991F4;
extern sfx_t *D_00399244;
extern sfx_t *D_003991D0;
extern sfx_t *D_0039926C;
extern sfx_t *D_003991D4;
extern sfx_t *D_003991FC;
extern sfx_t *D_00399250;
extern sfx_t *D_00399264;
extern sfx_t *D_00399208;
extern sfx_t *D_00399228;
extern sfx_t *D_00399274;
extern sfx_t *D_00399270;
extern sfx_t *D_00399224;
extern sfx_t *D_003991F8;
extern sfx_t *D_00399280;

extern int D_003991E4;
extern int D_00399200;
extern int D_003991CC;
extern int D_0039920C;
extern int D_00475C80[3];
extern int D_00399268;
extern unsigned short *host_basepal;
extern cvar_t r_decals;
extern cvar_t sv_gravity;
extern vec3_t r_origin;
extern vec3_t vpn, vup, vright;
extern vec3_t modelorg;
extern cl_entity_t *currententity;

typedef struct
{
    int type;                   /* 0x00 */
    void *frameptr;             /* 0x04 */
} mspriteframedesc_t;

typedef struct
{
    byte pad0[7];
    byte numframes;             /* 0x07 */
    mspriteframedesc_t frames[1]; /* 0x08 */
} msprite_t;
extern int D_00331BFC;              /* gTempEntFrame */

typedef struct
{
    vec3_t origin;      /* 0x00 */
    float radius;       /* 0x0C */
    color24 color;      /* 0x10 */
    float die;          /* 0x14 */
    float decay;        /* 0x18 */
    float minlight;     /* 0x1C */
    int key;            /* 0x20 */
    qboolean dark;      /* 0x24 */
} dlight_t;
extern int cl_numvisedicts[2];
extern cl_entity_t *cl_visedicts[2];

typedef struct client_textmessage_s
{
    int effect;
    byte r1, g1, b1, a1;
    byte r2, g2, b2, a2;
    float x;
    float y;
    float fadein;
    float fadeout;
    float holdtime;
    float fxtime;
    const char *pName;
    const char *pMessage;
} client_textmessage_t; /* 0x2C */

extern client_textmessage_t gNetworkTextMessage[];
extern char *D_00332A08[];              /* gNetworkMessageNames */
extern char D_004E2D98[4][512];         /* gNetworkTextMessageBuffer */
extern int cl_shellchrome;

int func_00254880(char *name);
void func_0025AEA8(int arg);
void func_002B1C00(vec3_t pos, int count, int velocityMin, int velocityMax, int clientId);
void func_002577D8(vec3_t pos, struct model_s *pmodel, float duration, float scale, int clientId);
TEMPENTITY *func_0025B0F8(vec3_t org, struct model_s *model, int clientId);
TEMPENTITY *func_0025B048(vec3_t org, struct model_s *model, int clientId);
int ModelFrameCount(struct model_s *model);
void *func_002B1420(vec3_t org, vec3_t vel, float life, int clientId);
sfx_t *S_PrecacheSound(char *name);
void S_StartDynamicSound(int entnum, int entchannel, sfx_t *sfx, vec3_t origin, float fvol, float attenuation, int flags, int pitch, int clientId);
int RandomLong(int lLow, int lHigh);
float RandomFloat(float flLow, float flHigh);
void *memset(void *, int, unsigned int);
int abs(int);
dlight_t *func_002510D0(int key);
void func_002B2F08(vec3_t start, vec3_t end, int type, int clientId);
qboolean SV_RecursiveHullCheck(hull_t *hull, int num, float p1f, float p2f, vec3_t p1, vec3_t p2, trace_t *trace);
float sinf(float);
float cosf(float);
struct mnode_s *PVSNode(struct mnode_s *node, vec_t *emins, vec_t *emaxs, int clientId);
void func_00254CE8(vec3_t pos, int clientId);
void func_002B15D0(vec3_t pos, int clientId);
int Draw_DecalIndex(int id);
void R_DecalShoot(int textureIndex, int entity, int modelIndex, vec3_t position, int flags, int clientId);
void AppendTEntity(cl_entity_t *pEnt, int clientId);
int MSG_ReadByte(void);
int MSG_ReadShort(void);
int func_00243460(void);
char *MSG_ReadString(void);
char *strcpy(char *, const char *);
void func_00251DC0(const char *pMessage, int a1);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_tent", D_0038A760);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_tent", D_0038A768);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_tent", D_0038A778);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_tent", D_0038A780);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_tent", D_0038A790);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_tent", D_0038A798);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_tent", D_0038A7A8);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_tent", D_0038A7B8);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_tent", D_0038A7C8);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_tent", D_0038A7D0);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_tent", D_0038A7E0);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_tent", D_0038A7E8);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_tent", D_0038A7F8);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_tent", D_0038A800);

void func_002548B8(int arg)
{
    if (!arg)
    {
        Cvar_RegisterVariable(&D_00331B40);
        Cvar_RegisterVariable(&D_00331B58);
        Cvar_RegisterVariable(&D_00331B70);
        Cvar_RegisterVariable(&D_00331B88);
        Cvar_RegisterVariable(&D_00331BA0);
        Cvar_RegisterVariable(&D_00331BB8);
        Cvar_RegisterVariable(&D_00331BD0);
        D_003991DC = S_PrecacheSound("weapons/ric1.wav");
        D_00399230 = S_PrecacheSound("weapons/ric2.wav");
        D_0039925C = S_PrecacheSound("weapons/ric3.wav");
        D_0039927C = S_PrecacheSound("weapons/ric4.wav");
        D_0039922C = S_PrecacheSound("weapons/ric5.wav");
        D_00399214 = S_PrecacheSound("weapons/explode3.wav");
        D_00399238 = S_PrecacheSound("weapons/explode4.wav");
        D_003991E8 = S_PrecacheSound("weapons/explode5.wav");
        D_00399240 = S_PrecacheSound("player/pl_shell1.wav");
        D_003991EC = S_PrecacheSound("player/pl_shell2.wav");
        D_00399218 = S_PrecacheSound("player/pl_shell3.wav");
        D_00399260 = S_PrecacheSound("weapons/sshell1.wav");
        D_00399210 = S_PrecacheSound("weapons/sshell2.wav");
        D_00399254 = S_PrecacheSound("weapons/sshell3.wav");
        D_00399248 = S_PrecacheSound("debris/wood1.wav");
        D_003991E0 = S_PrecacheSound("debris/wood2.wav");
        D_00399278 = S_PrecacheSound("debris/wood3.wav");
        D_0039923C = S_PrecacheSound("debris/metal1.wav");
        D_003991C8 = S_PrecacheSound("debris/metal2.wav");
        D_0039921C = S_PrecacheSound("debris/metal3.wav");
        D_003991F0 = S_PrecacheSound("debris/glass1.wav");
        D_003991D8 = S_PrecacheSound("debris/glass2.wav");
        D_00399258 = S_PrecacheSound("debris/glass3.wav");
        D_003991F4 = S_PrecacheSound("debris/concrete1.wav");
        D_00399244 = S_PrecacheSound("debris/concrete2.wav");
        D_003991D0 = S_PrecacheSound("debris/concrete3.wav");
        D_0039926C = S_PrecacheSound("debris/flesh1.wav");
        D_003991D4 = S_PrecacheSound("debris/flesh2.wav");
        D_003991FC = S_PrecacheSound("debris/flesh3.wav");
        D_00399250 = S_PrecacheSound("debris/flesh5.wav");
        D_00399264 = S_PrecacheSound("debris/flesh6.wav");
        D_00399208 = S_PrecacheSound("debris/flesh7.wav");
        D_00399228 = S_PrecacheSound("player/geiger1.wav");
        D_00399274 = S_PrecacheSound("player/geiger2.wav");
        D_00399270 = S_PrecacheSound("player/geiger3.wav");
        D_00399224 = S_PrecacheSound("player/geiger4.wav");
        D_003991F8 = S_PrecacheSound("player/geiger5.wav");
        D_00399280 = S_PrecacheSound("player/geiger6.wav");
        D_003991E4 = func_00254880("sprites/dot.spz");
        D_00399200 = func_00254880("sprites/lgtning.spz");
        D_003991CC = func_00254880("sprites/white.spz");
        D_0039920C = func_00254880("sprites/animglow01.spz");
        D_00475C80[0] = func_00254880("sprites/muzzleflash1.spz");
        D_00475C80[1] = func_00254880("sprites/muzzleflash2.spz");
        D_00475C80[2] = func_00254880("sprites/muzzleflash3.spz");
        D_00399268 = func_00254880("sprites/richo1.spz");
        cl_shellchrome = func_00254880("sprites/shellchr.spz");
    }
    func_0025AEA8(arg);
}

void func_00254CE8(vec3_t pos, int clientId)
{
    switch (RandomLong(0, 4))
    {
    case 0:
        S_StartDynamicSound(-1, 0, D_003991DC, pos, 1.0, 1.0, 0, 100, clientId);
        break;
    case 1:
        S_StartDynamicSound(-1, 0, D_00399230, pos, 1.0, 1.0, 0, 100, clientId);
        break;
    case 2:
        S_StartDynamicSound(-1, 0, D_0039925C, pos, 1.0, 1.0, 0, 100, clientId);
        break;
    case 3:
        S_StartDynamicSound(-1, 0, D_0039927C, pos, 1.0, 1.0, 0, 100, clientId);
        break;
    case 4:
        S_StartDynamicSound(-1, 0, D_0039922C, pos, 1.0, 1.0, 0, 100, clientId);
        break;
    }
}

void func_00254DF0(vec3_t start, vec3_t end, int clientId)
{
    vec3_t temp, vel;
    float len;

    if (D_00331B40.value <= 0)
        D_00331B40.value = 3;

    VectorSubtract(end, start, temp);
    len = Length(temp);
    VectorScale(temp, 1 / len, temp);
    VectorScale(temp, D_00331B58.value + RandomLong(-10, 9), vel);
    VectorAdd(start, vel, start);
    VectorScale(temp, D_00331B40.value, vel);
    func_002B1420(start, vel, len / D_00331B40.value, clientId);
}

#ifdef NON_MATCHING
void func_00254F40(cl_entity_t *pent, int modelIndex, int density, int clientId)
{
    TEMPENTITY *pTemp;
    struct model_s *model;
    int i, width, depth, count, frameCount;
    float maxHeight, speed, xspeed, yspeed, zspeed;
    vec3_t origin;

    if (!pent->model || !modelIndex)
        return;

    model = cl[clientId].model_precache[modelIndex];
    if (!model)
        return;

    count = density + 1;

    maxHeight = pent->model->maxs[2] - pent->model->mins[2];
    width = pent->model->maxs[0] - pent->model->mins[0];
    depth = pent->model->maxs[1] - pent->model->mins[1];
    speed = pent->rendercolor.r * 256.0f + pent->rendercolor.g;
    if (pent->rendercolor.b)
        speed = -speed;

    xspeed = speed * cosf(pent->angles[1] * M_PI / 180);
    yspeed = speed * sinf(pent->angles[1] * M_PI / 180);
    frameCount = ModelFrameCount(model);

    for (i = 0; i < count; i++)
    {
        origin[0] = pent->model->mins[0] + RandomLong(0, width - 1);
        origin[1] = pent->model->mins[1] + RandomLong(0, depth - 1);
        origin[2] = pent->model->mins[2];
        pTemp = func_0025B048(origin, model, clientId);
        if (!pTemp)
            return;

        pTemp->flags |= FTENT_SINEWAVE;
        pTemp->x = origin[0];
        pTemp->y = origin[1];
        pTemp->entity.baseline.origin[0] = xspeed;
        pTemp->entity.baseline.origin[1] = yspeed;
        zspeed = RandomLong(80, 140);
        pTemp->entity.baseline.origin[2] = zspeed;
        pTemp->die = cl[clientId].time + maxHeight / zspeed - 0.1f;
        pTemp->entity.frame = RandomLong(0, frameCount - 1);
        pTemp->entity.scale = 1.0f / RandomFloat(2, 5);
        pTemp->entity.rendermode = kRenderTransAlpha;
        pTemp->entity.renderamt = 255;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_00254F40);
#endif

#ifdef NON_MATCHING
void func_002551E8(vec3_t mins, vec3_t maxs, float height, int modelIndex, int count, float speed, int clientId)
{
    TEMPENTITY *pTemp;
    struct model_s *model;
    int i, frameCount;
    float angle, zspeed;
    vec3_t origin;

    if (!modelIndex)
        return;

    model = cl[clientId].model_precache[modelIndex];
    if (!model)
        return;

    frameCount = ModelFrameCount(model);

    for (i = 0; i < count; i++)
    {
        origin[0] = RandomLong(mins[0], maxs[0]);
        origin[1] = RandomLong(mins[1], maxs[1]);
        origin[2] = RandomLong(mins[2], maxs[2]);
        pTemp = func_0025B048(origin, model, clientId);
        if (!pTemp)
            return;

        pTemp->flags |= FTENT_SINEWAVE;
        pTemp->x = origin[0];
        pTemp->y = origin[1];
        angle = RandomLong(-3, 3);
        pTemp->entity.baseline.origin[0] = speed * cosf(angle);
        pTemp->entity.baseline.origin[1] = speed * sinf(angle);
        zspeed = RandomLong(80, 140);
        pTemp->entity.baseline.origin[2] = zspeed;
        pTemp->die = cl[clientId].time + (height - (origin[2] - mins[2])) / zspeed - 0.1f;
        pTemp->entity.frame = RandomLong(0, frameCount - 1);
        pTemp->entity.scale = 1.0f / RandomFloat(2, 5);
        pTemp->entity.rendermode = kRenderTransAlpha;
        pTemp->entity.renderamt = 255;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_002551E8);
#endif

#ifdef NON_MATCHING
void func_00255460(vec3_t start, vec3_t end, float height, int modelIndex, int count, float speed, int clientId)
{
    TEMPENTITY *pTemp;
    struct model_s *model;
    int i, frameCount;
    float dist, angle, zspeed;
    vec3_t origin;

    if (!modelIndex)
        return;

    model = cl[clientId].model_precache[modelIndex];
    if (!model)
        return;

    frameCount = ModelFrameCount(model);

    for (i = 0; i < count; i++)
    {
        dist = RandomFloat(0, 1.0f);
        origin[0] = start[0] + dist * (end[0] - start[0]);
        origin[1] = start[1] + dist * (end[1] - start[1]);
        origin[2] = start[2] + dist * (end[2] - start[2]);
        pTemp = func_0025B048(origin, model, clientId);
        if (!pTemp)
            return;

        pTemp->flags |= FTENT_SINEWAVE;
        pTemp->x = origin[0];
        pTemp->y = origin[1];
        angle = RandomLong(-3, 3);
        pTemp->entity.baseline.origin[0] = speed * cosf(angle);
        pTemp->entity.baseline.origin[1] = speed * sinf(angle);
        zspeed = RandomLong(80, 140);
        pTemp->entity.baseline.origin[2] = zspeed;
        pTemp->die = cl[clientId].time + (height - (origin[2] - start[2])) / zspeed - 0.1f;
        pTemp->entity.frame = RandomLong(0, frameCount - 1);
        pTemp->entity.scale = 1.0f / RandomFloat(2, 5);
        pTemp->entity.rendermode = kRenderTransAlpha;
        pTemp->entity.renderamt = 255;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_00255460);
#endif

#ifdef NON_MATCHING
void func_002556A8(vec3_t origin, vec3_t velocity, int modelIndex, int life, int owner, int clientId)
{
    TEMPENTITY *pTemp;
    struct model_s *model;
    int frameCount;
    vec3_t angles;

    model = cl[clientId].model_precache[modelIndex];
    if (!model)
        return;

    pTemp = func_0025B0F8(origin, model, clientId);
    if (!pTemp)
    {
        Con_Printf("Couldn't allocate temp ent in R_Projectile!\n");
        return;
    }

    if (owner < 0 || owner > cl[clientId].maxclients)
    {
        Con_Printf("Bad ent in R_Projectile!\n");
        return;
    }

    pTemp->flags |= FTENT_COLLIDEALL | FTENT_PERSIST | FTENT_COLLIDEKILL;
    VectorCopy(velocity, pTemp->entity.baseline.origin);
    pTemp->clientIndex = owner;

    if (pTemp->entity.model->type == mod_sprite)
    {
        frameCount = ModelFrameCount(model);
        pTemp->flags |= FTENT_SPRANIMATE;
        pTemp->frameMax = frameCount;
        if (frameCount < 10)
        {
            pTemp->entity.framerate = 10;
            pTemp->flags |= FTENT_SPRANIMATELOOP;
        }
        else
        {
            pTemp->entity.framerate = frameCount / life;
        }
    }
    else
    {
        pTemp->frameMax = 0;
        VectorNormalize(velocity);
        VectorAngles(velocity, angles);
        VectorCopy(angles, pTemp->entity.angles);
    }

    pTemp->entity.rendermode = kRenderNormal;
    pTemp->entity.renderfx = 0;
    pTemp->entity.renderamt = pTemp->entity.baseline.renderamt = 255;
    pTemp->entity.rendercolor.r = pTemp->entity.rendercolor.g = pTemp->entity.rendercolor.b = 255;
    pTemp->entity.scale = 1;
    pTemp->entity.frame = 0;
    pTemp->entity.body = 0;
    pTemp->die = cl[clientId].time + life;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_002556A8);
#endif

#ifdef NON_MATCHING
void func_002558C0(int type, vec3_t start, vec3_t end, int modelIndex, int count, float life, float size, float amp, int renderamt, float speed, int clientId)
{
    TEMPENTITY *pTemp;
    struct model_s *model;
    int i, frameCount;
    vec3_t delta, pos, dir;

    model = cl[clientId].model_precache[modelIndex];
    if (!model)
        return;

    frameCount = ModelFrameCount(model);
    VectorSubtract(end, start, delta);
    VectorCopy(delta, dir);
    VectorNormalize(dir);
    amp /= 256.0f;

    for (i = 0; i < count; i++)
    {
        VectorMA(start, i / (count - 1.0f), delta, pos);
        pTemp = func_0025B048(pos, model, clientId);
        if (!pTemp)
            return;

        pTemp->flags |= FTENT_COLLIDEWORLD | FTENT_SPRCYCLE | FTENT_FADEOUT | FTENT_SLOWGRAVITY;
        VectorScale(dir, speed, pTemp->entity.baseline.origin);
        pTemp->entity.baseline.origin[0] += RandomLong(-127, 128) * amp;
        pTemp->entity.baseline.origin[1] += RandomLong(-127, 128) * amp;
        pTemp->entity.baseline.origin[2] += RandomLong(-127, 128) * amp;
        pTemp->entity.scale = size;
        pTemp->entity.renderamt = renderamt;
        pTemp->entity.rendermode = kRenderTransAdd;
        pTemp->entity.renderfx = kRenderFxNoDissipation;
        pTemp->die = cl[clientId].time + life + RandomFloat(0, 4);
        pTemp->entity.frame = RandomLong(0, frameCount - 1);
        pTemp->entity.baseline.renderamt = renderamt;
        pTemp->frameMax = frameCount;
        pTemp->entity.rendercolor.r = 255;
        pTemp->entity.rendercolor.g = 255;
        pTemp->entity.rendercolor.b = 255;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_002558C0);
#endif

void func_00255B70(vec3_t pos, float speed, float life, int count, int modelIndex, int clientId)
{
    TEMPENTITY *pTemp;
    struct model_s *model;
    int i, frameCount;

    if (!modelIndex)
        return;

    model = cl[clientId].model_precache[modelIndex];
    if (!model)
        return;

    frameCount = ModelFrameCount(model);

    for (i = 0; i < count; i++)
    {
        pTemp = func_0025B048(pos, model, clientId);
        if (!pTemp)
            return;

        pTemp->entity.body = RandomLong(0, frameCount - 1);

        if (RandomLong(0, 255) < 10)
            pTemp->flags |= FTENT_SLOWGRAVITY;
        else
            pTemp->flags |= FTENT_GRAVITY;

        if (RandomLong(0, 255) < 220)
        {
            pTemp->flags |= FTENT_ROTATE;
            pTemp->entity.baseline.angles[0] = RandomFloat(-256, -255);
            pTemp->entity.baseline.angles[1] = RandomFloat(-256, -255);
            pTemp->entity.baseline.angles[2] = RandomFloat(-256, -255);
        }

        if (RandomLong(0, 255) < 100)
            pTemp->flags |= FTENT_SMOKETRAIL;

        pTemp->flags |= FTENT_FLICKER | FTENT_COLLIDEWORLD;
        pTemp->entity.rendermode = kRenderNormal;
        pTemp->entity.unk194 = i & 31;
        pTemp->entity.baseline.origin[0] = RandomFloat(-1, 1);
        pTemp->entity.baseline.origin[1] = RandomFloat(-1, 1);
        pTemp->entity.baseline.origin[2] = RandomFloat(-1, 1);
        VectorNormalize(pTemp->entity.baseline.origin);
        VectorScale(pTemp->entity.baseline.origin, speed, pTemp->entity.baseline.origin);
        pTemp->die = cl[clientId].time + life;
    }
}

#ifdef NON_MATCHING
void func_00255DB8(vec3_t pos, vec3_t size, vec3_t dir, float random, float life, int count, int modelIndex, char flags, int clientId)
{
    TEMPENTITY *pTemp;
    struct model_s *model;
    int i, frameCount;
    char type;
    vec3_t vecSpot;

    if (!modelIndex)
        return;

    type = flags & BREAK_TYPEMASK;
    model = cl[clientId].model_precache[modelIndex];
    if (!model)
        return;

    frameCount = ModelFrameCount(model);

    if (count == 0)
        count = (size[0] * size[1] + size[1] * size[2] + size[2] * size[0]) / (3 * SHARD_VOLUME * SHARD_VOLUME);

    if (cl_numlocal < 2)
    {
        if (count > 25)
            count = 25;
        if (D_003FE298[clientId] + count >= 50)
            count = 5;
    }
    else
    {
        if (count > 10)
            count = 10;
        if (D_003FE298[clientId] + count >= 25)
            count = 2;
    }

    for (i = 0; i < count; i++)
    {
        vecSpot[0] = pos[0] + RandomFloat(-0.5f, 0.5f) * size[0];
        vecSpot[1] = pos[1] + RandomFloat(-0.5f, 0.5f) * size[1];
        vecSpot[2] = pos[2] + RandomFloat(-0.5f, 0.5f) * size[2];
        pTemp = func_0025B048(vecSpot, model, clientId);
        if (!pTemp)
            return;

        pTemp->hitSound = type;

        if (model->type == mod_sprite)
            pTemp->entity.frame = RandomLong(0, frameCount - 1);
        else if (model->type == mod_studio)
            pTemp->entity.body = RandomLong(0, frameCount - 1);

        pTemp->flags |= FTENT_COLLIDEWORLD | FTENT_FADEOUT | FTENT_SLOWGRAVITY;

        if (RandomLong(0, 255) < 200)
        {
            pTemp->flags |= FTENT_ROTATE;
            pTemp->entity.baseline.angles[0] = RandomFloat(-256, 255);
            pTemp->entity.baseline.angles[1] = RandomFloat(-256, 255);
            pTemp->entity.baseline.angles[2] = RandomFloat(-256, 255);
        }

        if (RandomLong(0, 255) < 100 && (flags & BREAK_SMOKE))
            pTemp->flags |= FTENT_SMOKETRAIL;

        if (type == BREAK_GLASS || (flags & BREAK_TRANS))
        {
            pTemp->entity.rendermode = kRenderTransTexture;
            pTemp->entity.renderamt = pTemp->entity.baseline.renderamt = 128;
        }
        else
        {
            pTemp->entity.rendermode = kRenderNormal;
            pTemp->entity.renderamt = pTemp->entity.baseline.renderamt = 255;
        }

        pTemp->entity.baseline.origin[0] = dir[0] + RandomFloat(-random, random);
        pTemp->entity.baseline.origin[1] = dir[1] + RandomFloat(-random, random);
        pTemp->entity.baseline.origin[2] = dir[2] + RandomFloat(0, random);
        pTemp->die = cl[clientId].time + life + RandomFloat(0, 1);
        pTemp->flags |= FTENT_NOMODEL_HACK;
        D_003FE298[clientId]++;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_00255DB8);
#endif

TEMPENTITY *func_002561F0(vec3_t pos, vec3_t dir, vec3_t angles, float life, int modelIndex, int soundtype, int clientId)
{
    TEMPENTITY *pTemp;
    struct model_s *model;
    int frameCount;

    if (!modelIndex)
        return NULL;

    model = cl[clientId].model_precache[modelIndex];
    if (!model)
    {
        Con_Printf(D_0038ACF8, modelIndex);
        return NULL;
    }

    frameCount = ModelFrameCount(model);

    pTemp = func_0025B048(pos, model, clientId);
    if (!pTemp)
        return NULL;

    VectorCopy(angles, pTemp->entity.angles);
    pTemp->frameMax = frameCount;
    pTemp->flags |= FTENT_COLLIDEWORLD | FTENT_GRAVITY;

    switch (soundtype)
    {
    case 0:
        pTemp->hitSound = 0;
        break;
    case 1:
        pTemp->hitSound = BOUNCE_SHELL;
        pTemp->entity.baseline.angles[0] = RandomFloat(-512, 511);
        pTemp->entity.baseline.angles[1] = RandomFloat(-256, 255);
        pTemp->entity.baseline.angles[2] = RandomFloat(-256, 255);
        pTemp->flags |= FTENT_ROTATE;
        break;
    case 2:
        pTemp->hitSound = BOUNCE_SHOTSHELL;
        pTemp->entity.baseline.angles[0] = RandomFloat(-512, 511);
        pTemp->entity.baseline.angles[1] = RandomFloat(-256, 255);
        pTemp->entity.baseline.angles[2] = RandomFloat(-256, 255);
        pTemp->flags |= FTENT_ROTATE | FTENT_SLOWGRAVITY;
        break;
    }

    VectorCopy(dir, pTemp->entity.baseline.origin);
    pTemp->die = cl[clientId].time + life;

    if (model->type == mod_sprite)
        pTemp->entity.frame = RandomLong(0, frameCount - 1);
    else
        pTemp->entity.body = RandomLong(0, frameCount - 1);

    return pTemp;
}

#ifdef NON_MATCHING
TEMPENTITY *func_00256478(vec3_t pos, vec3_t dir, float scale, int modelIndex, int rendermode, int renderfx, float a, float life, int flags, int clientId)
{
    TEMPENTITY *pTemp;
    struct model_s *model;
    int frameCount;

    if (!modelIndex)
        return NULL;

    model = cl[clientId].model_precache[modelIndex];
    if (!model)
    {
        Con_Printf(D_0038ACF8, modelIndex);
        return NULL;
    }

    frameCount = ModelFrameCount(model);

    pTemp = func_0025B048(pos, model, clientId);
    if (!pTemp)
        return NULL;

    pTemp->frameMax = frameCount;
    pTemp->entity.framerate = 10;
    pTemp->entity.rendermode = rendermode;
    pTemp->entity.renderfx = renderfx;
    pTemp->entity.scale = scale;
    pTemp->entity.rendercolor.r = 255;
    pTemp->entity.rendercolor.g = 255;
    pTemp->entity.rendercolor.b = 255;
    pTemp->entity.renderamt = pTemp->entity.baseline.renderamt = a * 255;
    pTemp->flags |= flags;
    VectorCopy(dir, pTemp->entity.baseline.origin);
    VectorCopy(pos, pTemp->entity.origin);

    if (life)
        pTemp->die = cl[clientId].time + life;
    else
        pTemp->die = cl[clientId].time + frameCount * 0.1f + 1.0f;

    pTemp->entity.frame = 0;
    return pTemp;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_00256478);
#endif

#ifdef NON_MATCHING
void func_00256668(vec3_t pos, vec3_t dir, int modelIndex, int count, int speed, int iRand, int clientId)
{
    TEMPENTITY *pTemp;
    struct model_s *model;
    float noise, znoise;
    int i, frameCount;

    noise = (float)iRand / 100;
    znoise = noise * 1.5f;
    if (znoise > 1)
        znoise = 1;

    model = cl[clientId].model_precache[modelIndex];
    if (!model)
    {
        Con_Printf(D_0038ACF8, modelIndex);
        return;
    }

    frameCount = ModelFrameCount(model) - 1;

    for (i = 0; i < count; i++)
    {
        pTemp = func_0025B048(pos, model, clientId);
        if (!pTemp)
            return;

        pTemp->entity.rendermode = kRenderTransAlpha;
        pTemp->entity.baseline.renderamt = pTemp->entity.renderamt = 255;
        pTemp->entity.renderfx = kRenderFxNoDissipation;
        pTemp->entity.scale = 0.5f;
        pTemp->flags |= FTENT_FADEOUT | FTENT_SLOWGRAVITY;
        pTemp->fadeSpeed = 2.0f;

        pTemp->entity.baseline.origin[0] = dir[0] + RandomFloat(-noise, noise);
        pTemp->entity.baseline.origin[1] = dir[1] + RandomFloat(-noise, noise);
        pTemp->entity.baseline.origin[2] = dir[2] + RandomFloat(0, znoise);
        VectorScale(pTemp->entity.baseline.origin, RandomFloat(speed * 0.8f, speed * 1.2f), pTemp->entity.baseline.origin);
        VectorCopy(pos, pTemp->entity.origin);
        pTemp->die = cl[clientId].time + 0.35f;
        pTemp->entity.frame = RandomLong(0, frameCount);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_00256668);
#endif

#ifdef NON_MATCHING
void func_002568F8(vec3_t pos, vec3_t dir, int modelIndex, int count, int speed, int iRand, int renderMode, int clientId)
{
    TEMPENTITY *pTemp;
    struct model_s *model;
    float noise, znoise;
    int i, frameCount;

    noise = (float)iRand / 100;
    znoise = noise * 1.5f;
    if (znoise > 1)
        znoise = 1;

    model = cl[clientId].model_precache[modelIndex];
    if (!model)
    {
        Con_Printf(D_0038ACF8, modelIndex);
        return;
    }

    frameCount = ModelFrameCount(model) - 1;

    for (i = 0; i < count; i++)
    {
        pTemp = func_0025B048(pos, model, clientId);
        if (!pTemp)
            return;

        pTemp->entity.rendermode = renderMode;
        pTemp->entity.renderfx = kRenderFxNoDissipation;
        pTemp->entity.scale = 0.5f;
        pTemp->entity.baseline.renderamt = pTemp->entity.renderamt = 255;
        pTemp->flags |= FTENT_COLLIDEWORLD | FTENT_SLOWGRAVITY;

        if (frameCount > 1)
        {
            pTemp->flags |= FTENT_SPRANIMATE;
            pTemp->entity.framerate = 10.0f;
            pTemp->die = cl[clientId].time + frameCount * 0.1f;
        }
        else
        {
            pTemp->die = cl[clientId].time + 0.35f;
        }

        pTemp->entity.baseline.origin[0] = dir[0] + RandomFloat(-noise, noise);
        pTemp->entity.baseline.origin[1] = dir[1] + RandomFloat(-noise, noise);
        pTemp->entity.baseline.origin[2] = dir[2] + RandomFloat(0, znoise);
        VectorScale(pTemp->entity.baseline.origin, RandomFloat(speed * 0.8f, speed * 1.2f), pTemp->entity.baseline.origin);
        VectorCopy(pos, pTemp->entity.origin);
        pTemp->frameMax = frameCount;
        pTemp->entity.frame = 0;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_002568F8);
#endif

INCLUDE_RODATA("asm/nonmatchings/engine/cl_tent", D_0038ACF8);

void func_00256BC8(int client, int modelIndex, int count, int size, int clientId)
{
    TEMPENTITY *pTemp;
    struct model_s *model;
    cl_entity_t *pEnt;
    float scale;
    int i, frameCount;
    vec3_t origin, dir;

    scale = 1.0f - size / 100.0f;

    if (client <= 0 || client > cl[clientId].maxclients)
    {
        Con_Printf("Bad ent in R_PlayerSprites!\n");
        return;
    }

    pEnt = &cl_entities[clientId][client];

    model = cl[clientId].model_precache[modelIndex];
    if (!model)
    {
        Con_Printf(D_0038ACF8, modelIndex);
        return;
    }

    frameCount = ModelFrameCount(model) - 1;

    for (i = 0; i < count; i++)
    {
        origin[0] = pEnt->origin[0] + RandomFloat(-10, 10);
        origin[1] = pEnt->origin[1] + RandomFloat(-10, 10);
        origin[2] = pEnt->origin[2] + RandomFloat(-20, 36);
        pTemp = func_0025B048(origin, model, clientId);
        if (!pTemp)
            return;

        VectorSubtract(pTemp->entity.origin, pEnt->origin, pTemp->tentOffset);

        if (i == 0)
        {
            VectorSubtract(origin, pEnt->origin, dir);
            VectorNormalize(dir);
            VectorScale(dir, 60, dir);
            pTemp->entity.baseline.origin[0] = dir[0];
            pTemp->entity.baseline.origin[1] = dir[1];
            pTemp->entity.baseline.origin[2] = RandomFloat(20, 60);
        }
        else
        {
            pTemp->flags |= FTENT_PLYRATTACHMENT;
            pTemp->clientIndex = client;
        }

        pTemp->entity.rendermode = kRenderNormal;
        pTemp->entity.baseline.renderamt = pTemp->entity.renderamt = 255;
        pTemp->entity.renderfx = kRenderFxNoDissipation;
        pTemp->entity.scale = RandomFloat(scale, 1.0f);

        if (frameCount > 1)
        {
            pTemp->flags |= FTENT_SPRANIMATE;
            pTemp->entity.framerate = 20.0f;
            pTemp->die = cl[clientId].time + frameCount * 0.05f;
        }
        else
        {
            pTemp->die = cl[clientId].time + 0.35f;
        }

        pTemp->frameMax = frameCount;
        pTemp->entity.frame = 0;
    }
}

#ifdef NON_MATCHING
void func_00256F20(vec3_t org, int radius, int modelIndex, int count, int flags, float life, int clientId)
{
    TEMPENTITY *pTemp;
    struct model_s *model;
    float lifetime;
    int i, frameCount;
    vec3_t pos;

    model = cl[clientId].model_precache[modelIndex];
    if (!model)
    {
        Con_Printf(D_0038ACF8, modelIndex);
        return;
    }

    frameCount = ModelFrameCount(model) - 1;

    for (i = 0; i < count; i++)
    {
        pos[0] = org[0] + RandomFloat(-radius, radius);
        pos[1] = org[1] + RandomFloat(-radius, radius);
        if (flags & TEFIRE_FLAG_PLANAR)
            pos[2] = org[2];
        else
            pos[2] = org[2] + RandomFloat(-radius, radius);

        pTemp = func_0025B048(pos, model, clientId);
        if (!pTemp)
            return;

        if (flags & TEFIRE_FLAG_ALPHA)
        {
            pTemp->entity.renderfx = kRenderFxNoDissipation;
            pTemp->entity.rendermode = kRenderTransAlpha;
            pTemp->entity.renderamt = pTemp->entity.baseline.renderamt = 128;
        }
        else
        {
            pTemp->entity.rendermode = kRenderNormal;
            pTemp->entity.renderfx = kRenderFxNoDissipation;
            pTemp->entity.renderamt = pTemp->entity.baseline.renderamt = 255;
        }

        pTemp->entity.scale = RandomFloat(0.75f, 1.25f);
        lifetime = life + RandomFloat(-0.25f, 0.5f);
        pTemp->die = cl[clientId].time + lifetime;

        if (frameCount > 1)
        {
            pTemp->flags |= FTENT_SPRANIMATE;
            if (flags & TEFIRE_FLAG_LOOP)
                pTemp->entity.framerate = 15;
            else
                pTemp->entity.framerate = frameCount / lifetime;
        }

        pTemp->frameMax = frameCount;
        pTemp->entity.frame = 0;
        VectorClear(pTemp->entity.baseline.origin);

        if ((flags & TEFIRE_FLAG_ALLFLOAT) || ((flags & TEFIRE_FLAG_SOMEFLOAT) && RandomLong(0, 1)))
            pTemp->entity.baseline.origin[2] = RandomFloat(10, 30);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_00256F20);
#endif

void func_002571D0(int client, int modelIndex, float zoffset, float life, int clientId)
{
    TEMPENTITY *pTemp;
    struct model_s *model;
    cl_entity_t *pClient;
    vec3_t position;

    if (client < 0 || client > cl[clientId].maxclients)
    {
        Con_Printf("Bad client in AttachTentToPlayer()!\n");
        return;
    }

    pClient = &cl_entities[clientId][client];

    model = cl[clientId].model_precache[modelIndex];
    if (!model)
    {
        Con_Printf(D_0038ACF8, modelIndex);
        return;
    }

    VectorCopy(pClient->origin, position);
    position[2] += zoffset;

    pTemp = func_0025B0F8(position, model, clientId);
    if (!pTemp)
    {
        Con_Printf("No temp ent.\n");
        return;
    }

    pTemp->entity.rendermode = kRenderNormal;
    pTemp->entity.baseline.renderamt = pTemp->entity.renderamt = 255;
    pTemp->entity.renderfx = kRenderFxNoDissipation;
    pTemp->entity.scale = 1;
    pTemp->clientIndex = client;
    pTemp->tentOffset[0] = 0;
    pTemp->tentOffset[1] = 0;
    pTemp->tentOffset[2] = zoffset;
    pTemp->die = cl[clientId].time + life;
    pTemp->flags |= FTENT_PLYRATTACHMENT | FTENT_PERSIST;

    if (pTemp->entity.model->type == mod_sprite)
    {
        pTemp->frameMax = ModelFrameCount(model);
        pTemp->flags |= FTENT_SPRANIMATE | FTENT_SPRANIMATELOOP;
        pTemp->entity.framerate = 10;
    }
    else
    {
        pTemp->frameMax = 0;
    }

    pTemp->entity.frame = 0;
}

void func_00257398(int client, int clientId)
{
    TEMPENTITY *pTemp;

    if (client < 0 || client > cl[clientId].maxclients)
    {
        Con_Printf("Bad client in KillAttachedTents()!\n");
        return;
    }

    for (pTemp = D_003FE290[clientId]; pTemp; pTemp = pTemp->next)
    {
        if (pTemp->flags & FTENT_PLYRATTACHMENT)
        {
            if (pTemp->clientIndex == client)
                pTemp->die = cl[clientId].time;
        }
    }
}

void func_00257438(vec3_t pos, int count, int velocityMin, int velocityMax, int clientId)
{
    func_002B1C00(pos, count, velocityMin, velocityMax, clientId);
    func_002577D8(pos, (struct model_s *)D_00399268, 0.1f, RandomFloat(0.5f, 1.0f), clientId);
}

void func_002574B0(vec3_t org, int modelIndex, int reverse, int clientId)
{
    TEMPENTITY *pTemp;
    struct model_s *model;
    int i, j, frameCount;
    float vel, dist;
    vec3_t dir, dest;

    if (!modelIndex)
    {
        Con_Printf(D_0038AD88);
        return;
    }

    model = cl[clientId].model_precache[modelIndex];
    if (!model)
    {
        Con_Printf(D_0038ACF8, modelIndex);
        return;
    }

    frameCount = ModelFrameCount(model);

    for (i = -256; i <= 256; i += 64)
    {
        for (j = -256; j <= 256; j += 64)
        {
            pTemp = func_0025B048(org, model, clientId);
            if (!pTemp)
                return;

            if (reverse)
            {
                VectorCopy(org, pTemp->entity.origin);
                dest[0] = org[0] + i;
                dest[1] = org[1] + j;
                dest[2] = org[2] + RandomFloat(100, 800);
                VectorSubtract(dest, pTemp->entity.origin, dir);
                vel = dest[2] / 8;
            }
            else
            {
                pTemp->entity.origin[0] = org[0] + i;
                pTemp->entity.origin[1] = org[1] + j;
                pTemp->entity.origin[2] = org[2] + RandomFloat(100, 800);
                VectorSubtract(org, pTemp->entity.origin, dir);
                vel = pTemp->entity.origin[2] / 8;
            }

            pTemp->frameMax = frameCount;
            pTemp->entity.baseline.renderamt = pTemp->entity.renderamt = 200;
            pTemp->entity.framerate = 10;
            pTemp->entity.rendermode = kRenderTransAdd;
            pTemp->entity.renderfx = kRenderFxNoDissipation;
            pTemp->entity.scale = RandomFloat(0.1f, 0.4f);
            pTemp->flags = FTENT_ROTATE | FTENT_FADEOUT;

            dist = VectorNormalize(dir);
            if (vel < 64)
                vel = 64;
            vel += RandomFloat(64, 128);
            VectorScale(dir, vel, pTemp->entity.baseline.origin);
            pTemp->fadeSpeed = 2.0f;
            pTemp->die = cl[clientId].time + (dist / vel - 0.5f);
        }
    }
}

void func_002577D8(vec3_t pos, struct model_s *pmodel, float duration, float scale, int clientId)
{
    TEMPENTITY *pTemp;

    pTemp = func_0025B048(pos, pmodel, clientId);
    if (!pTemp)
        return;

    pTemp->entity.rendermode = kRenderGlow;
    pTemp->entity.renderfx = kRenderFxNoDissipation;
    pTemp->flags = FTENT_FADEOUT;
    pTemp->entity.baseline.renderamt = pTemp->entity.renderamt = 200;
    pTemp->entity.scale = scale;
    VectorClear(pTemp->entity.baseline.origin);
    VectorCopy(pos, pTemp->entity.origin);
    pTemp->fadeSpeed = 8;
    pTemp->die = cl[clientId].time;
    pTemp->entity.frame = 0;
    pTemp->entity.angles[2] = 45 * RandomLong(0, 7);
}

void func_002578C0(vec3_t pos, int clientId)
{
    TEMPENTITY *pTemp;
    int frameCount;

    if (cl[clientId].time == cl[clientId].oldtime)
        return;

    frameCount = ModelFrameCount((struct model_s *)D_0039920C);
    pTemp = func_0025B048(pos, (struct model_s *)D_0039920C, clientId);
    if (!pTemp)
        return;

    pTemp->entity.rendermode = kRenderGlow;
    pTemp->frameMax = frameCount;
    pTemp->entity.renderamt = 255;
    pTemp->entity.renderfx = kRenderFxNoDissipation;
    pTemp->entity.scale = 1.0f;
    VectorCopy(pos, pTemp->entity.origin);
    pTemp->die = cl[clientId].time + 0.01f;
    pTemp->entity.frame = RandomLong(0, frameCount - 1);
}

#ifdef NON_MATCHING
void func_002579E0(vec3_t pos, int color, int model2Index, int modelIndex, float scale, int clientId)
{
    TEMPENTITY *pTemp;
    struct model_s *model;
    int frameCount;
    int i, count;
    color24 c1, c2;
    unsigned short *pal;

    color += RandomLong(1, 3);
    pal = &host_basepal[color * 4];
    c1.r = pal[2];
    c1.g = pal[1];
    c1.b = pal[0];
    pal = &host_basepal[(color - 1) * 4];
    c2.r = pal[2];
    c2.g = pal[1];
    c2.b = pal[0];

    if (modelIndex)
    {
        model = cl[clientId].model_precache[modelIndex];
        if (model)
        {
            frameCount = ModelFrameCount(model);
            count = RandomLong(1, 8) + RandomLong(1, 8) + scale;
            for (i = 0; i < count; i++)
            {
                pTemp = func_0025B048(pos, model, clientId);
                if (!pTemp)
                    continue;

                pTemp->entity.renderfx = 20;
                pTemp->entity.rendermode = kRenderNormal;
                pTemp->entity.baseline.renderamt = pTemp->entity.renderamt = 250;
                pTemp->entity.scale = RandomFloat(scale * (1.0f / 15), scale * 0.04f);
                pTemp->flags = FTENT_ROTATE | FTENT_SLOWGRAVITY | FTENT_COLLIDEWORLD;
                pTemp->entity.rendercolor = c2;
                pTemp->entity.baseline.origin[0] = RandomFloat(-96, 95);
                pTemp->entity.baseline.origin[1] = RandomFloat(-96, 95);
                pTemp->entity.baseline.origin[2] = RandomFloat(-32, 95);
                pTemp->entity.baseline.angles[0] = RandomFloat(-256, -255);
                pTemp->entity.baseline.angles[1] = RandomFloat(-256, -255);
                pTemp->entity.baseline.angles[2] = RandomFloat(-256, -255);
                pTemp->entity.framerate = 0;
                pTemp->die = cl[clientId].time + RandomFloat(1, 2);
                pTemp->entity.frame = RandomLong(1, frameCount - 1);
                if (pTemp->entity.frame > 8)
                    pTemp->entity.frame = frameCount - 1;
                pTemp->entity.angles[2] = RandomLong(0, 360);
                pTemp->bounceFactor = 0;
            }
        }
    }

    if (model2Index)
    {
        model = cl[clientId].model_precache[model2Index];
        if (model)
        {
            frameCount = ModelFrameCount(model);
            pTemp = func_0025B0F8(pos, model, clientId);
            if (!pTemp)
                return;

            pTemp->entity.rendermode = kRenderNormal;
            pTemp->entity.renderfx = 20;
            pTemp->entity.renderamt = pTemp->entity.baseline.renderamt = 250;
            pTemp->entity.scale = RandomFloat(scale * 0.04f, scale * (1.0f / 35));
            pTemp->flags = FTENT_SPRANIMATE;
            pTemp->entity.rendercolor = c1;
            VectorClear(pTemp->entity.baseline.origin);
            pTemp->entity.framerate = frameCount * 4;
            pTemp->die = cl[clientId].time + frameCount / pTemp->entity.framerate;
            pTemp->entity.frame = 0;
            pTemp->frameMax = frameCount;
            pTemp->entity.angles[2] = RandomLong(0, 360);
            pTemp->bounceFactor = 0;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_002579E0);
#endif


INCLUDE_RODATA("asm/nonmatchings/engine/cl_tent", D_0038AD88);

TEMPENTITY *func_00257E28(vec3_t pos, int spriteIndex, float framerate, int clientId)
{
    TEMPENTITY *pTemp;
    struct model_s *pSprite;
    int frameCount;

    if (cl[clientId].time == cl[clientId].oldtime)
        return NULL;

    pSprite = cl[clientId].model_precache[spriteIndex];
    if (!spriteIndex || !pSprite || pSprite->type != mod_sprite)
    {
        Con_DPrintf("No Sprite %d!\n", spriteIndex);
        return NULL;
    }

    frameCount = ModelFrameCount(pSprite);
    pTemp = func_0025B048(pos, pSprite, clientId);
    if (!pTemp)
        return NULL;

    pTemp->entity.scale = 1.0f;
    pTemp->frameMax = frameCount;
    pTemp->flags |= FTENT_SPRANIMATE;
    if (framerate == 0)
        framerate = 10;

    pTemp->entity.framerate = framerate;
    pTemp->die = cl[clientId].time + frameCount / framerate;
    pTemp->entity.frame = 0;
    VectorCopy(pos, pTemp->entity.origin);
    return pTemp;
}

void func_00257F88(TEMPENTITY *pTemp, float scale)
{
    int iColor;

    if (!pTemp)
        return;

    pTemp->entity.rendermode = kRenderTransAlpha;
    pTemp->entity.renderamt = 255;
    pTemp->entity.renderfx = 0;
    pTemp->entity.baseline.origin[2] = 30;
    iColor = RandomLong(20, 35);
    pTemp->entity.rendercolor.r = iColor;
    pTemp->entity.rendercolor.g = iColor;
    pTemp->entity.rendercolor.b = iColor;
    pTemp->entity.origin[2] += 20;
    pTemp->entity.scale = scale;
}

void func_00258008(TEMPENTITY *pTemp, float scale, int flags)
{
    if (!pTemp)
        return;

    if (flags & 1)
    {
        pTemp->entity.rendermode = kRenderNormal;
        pTemp->entity.renderamt = 255;
    }
    else
    {
        pTemp->entity.rendermode = kRenderTransAdd;
        pTemp->entity.renderamt = 180;
    }

    pTemp->entity.scale = scale;
    pTemp->entity.origin[2] += 10;
    pTemp->entity.rendercolor.r = 0;
    pTemp->entity.rendercolor.g = 0;
    pTemp->entity.rendercolor.b = 0;
    pTemp->entity.baseline.origin[2] = 8;
    pTemp->entity.renderfx = 0;
}

void func_00258070(TEMPENTITY *pTemp, float scale, int clientId)
{
    if (!pTemp)
        return;

    pTemp->entity.rendermode = kRenderTransAlpha;
    pTemp->entity.renderamt = 255;
    pTemp->entity.scale = scale;
    pTemp->entity.renderfx = 0;
    pTemp->entity.rendercolor.r = 0;
    pTemp->entity.rendercolor.g = 0;
    pTemp->entity.rendercolor.b = 0;
    pTemp->die = cl[clientId].time + 0.01f;
    pTemp->entity.angles[2] = RandomLong(0, 359);
    pTemp->entity.frame = 0;
}

void func_00258108(vec3_t start, vec3_t dir, float *spread, int count, int decalNumber, int clientId)
{
    int i;
    float x, y;
    vec3_t vecDir, vecEnd, vecRight, vecUp, vecSpread;
    vec3_t up, right, angles;
    pmtrace_t tr;

    VectorAngles(dir, angles);
    AngleVectors(angles, NULL, right, up);

    for (i = 0; i < count; i++)
    {
        do
        {
            x = RandomFloat(-0.5f, 0.5f) + RandomFloat(-0.5f, 0.5f);
            y = RandomFloat(-0.5f, 0.5f) + RandomFloat(-0.5f, 0.5f);
        } while (x * x + y * y > 1);

        VectorScale(right, x * spread[0], vecRight);
        VectorScale(up, y * spread[1], vecUp);
        VectorAdd(vecRight, vecUp, vecSpread);
        VectorAdd(dir, vecSpread, vecDir);
        VectorScale(vecDir, 4096, vecEnd);
        VectorAdd(start, vecEnd, vecEnd);

        pmove[clientId].usehull = 2;
        tr = PM_PlayerTrace(start, vecEnd, 1, clientId);
        if (tr.fraction != 1.0f)
        {
            if (i & 2)
                func_00254CE8(tr.endpos, clientId);
            func_002B15D0(tr.endpos, clientId);
            if (r_decals.value)
                R_DecalShoot(Draw_DecalIndex(decalNumber), pmove[clientId].physents[tr.ent].info, 0, tr.endpos, 0, clientId);
        }
    }
}

void func_00258410(vec3_t pos, int scale, int clientId)
{
    TEMPENTITY *pTemp;
    float fscale;
    int index, frameCount;

    fscale = scale * 0.1f * 0.1f;
    index = scale % 10;
    if (fscale == 0)
        fscale = 0.5f;

    frameCount = ModelFrameCount((struct model_s *)D_00475C80[index]);
    pTemp = func_0025B048(pos, (struct model_s *)D_00475C80[index], clientId);
    if (!pTemp)
        return;

    pTemp->entity.rendermode = kRenderTransAdd;
    pTemp->entity.renderfx = 0;
    pTemp->entity.renderamt = 255;
    pTemp->entity.scale = fscale;
    pTemp->entity.rendercolor.b = pTemp->entity.rendercolor.g = pTemp->entity.rendercolor.r = 255;
    VectorCopy(pos, pTemp->entity.origin);
    pTemp->die = cl[clientId].time + 0.01f;
    pTemp->entity.frame = RandomLong(0, frameCount - 1);
    pTemp->frameMax = frameCount;
    VectorCopy(vec3_origin, pTemp->entity.angles);
    if (!index)
        pTemp->entity.angles[2] = RandomLong(0, 20);
    else
        pTemp->entity.angles[2] = RandomLong(0, 359);
    AppendTEntity(&pTemp->entity, clientId);
}

void func_002585C8(void)
{
    int channel;
    client_textmessage_t *pNetMessage;

    channel = MSG_ReadByte() % 4;
    pNetMessage = &gNetworkTextMessage[channel];
    pNetMessage->x = MSG_ReadShort() * (1.0f / 8192);
    pNetMessage->y = MSG_ReadShort() * (1.0f / 8192);
    pNetMessage->effect = MSG_ReadByte();
    pNetMessage->r1 = MSG_ReadByte();
    pNetMessage->g1 = MSG_ReadByte();
    pNetMessage->b1 = MSG_ReadByte();
    pNetMessage->a1 = MSG_ReadByte();
    pNetMessage->r2 = MSG_ReadByte();
    pNetMessage->g2 = MSG_ReadByte();
    pNetMessage->b2 = MSG_ReadByte();
    pNetMessage->a2 = MSG_ReadByte();
    pNetMessage->fadein = func_00243460() * (1.0f / 256);
    pNetMessage->fadeout = func_00243460() * (1.0f / 256);
    pNetMessage->holdtime = func_00243460() * (1.0f / 256);
    if (pNetMessage->effect == 2)
        pNetMessage->fxtime = func_00243460() * (1.0f / 256);

    pNetMessage->pName = D_00332A08[channel];
    pNetMessage->pMessage = D_004E2D98[channel];
    strcpy(D_004E2D98[channel], MSG_ReadString());
    func_00251DC0(pNetMessage->pName, 0);
}

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_00258778);

void func_0025AEA8(int clientId)
{
    int i;

    memset(D_003ACCF8[clientId], 0, sizeof(D_003ACCF8[clientId]));

    for (i = 0; i < MAX_TEMP_ENTITIES - 1; i++)
        D_003ACCF8[clientId][i].next = &D_003ACCF8[clientId][i + 1];

    D_003ACCF8[clientId][MAX_TEMP_ENTITIES - 1].next = NULL;
    D_003FE288[clientId] = D_003ACCF8[clientId];
    D_003FE290[clientId] = NULL;
    D_003FE298[clientId] = 0;
}

void func_0025AFA0(TEMPENTITY *pTemp, struct model_s *model, int clientId)
{
    memset(&pTemp->entity, 0, sizeof(cl_entity_t));
    pTemp->flags = 0;
    pTemp->entity.colormap = 0;
    pTemp->die = cl[clientId].time + 0.75f;
    pTemp->entity.model = model;
    pTemp->entity.rendermode = kRenderNormal;
    pTemp->entity.renderfx = 0;
    pTemp->fadeSpeed = 0.5f;
    pTemp->hitSound = 0;
    pTemp->clientIndex = -1;
    pTemp->bounceFactor = 1;
}

TEMPENTITY *func_0025B048(vec3_t org, struct model_s *model, int clientId)
{
    TEMPENTITY *pTemp;

    if (!D_003FE288[clientId] || !model)
    {
        Con_DPrintf("Client [%d], Overflow %d temporary ents!\n", clientId, MAX_TEMP_ENTITIES);
        return NULL;
    }

    pTemp = D_003FE288[clientId];
    D_003FE288[clientId] = pTemp->next;

    func_0025AFA0(pTemp, model, clientId);

    pTemp->priority = TENTPRIORITY_LOW;
    VectorCopy(org, pTemp->entity.origin);

    pTemp->next = D_003FE290[clientId];
    D_003FE290[clientId] = pTemp;

    return pTemp;
}

TEMPENTITY *func_0025B0F8(vec3_t org, struct model_s *model, int clientId)
{
    TEMPENTITY *pTemp, *p;

    if (!model)
    {
        Con_DPrintf("temporary ent model invalid\n");
        return NULL;
    }

    if (!D_003FE288[clientId])
    {
        pTemp = NULL;
        for (p = D_003FE290[clientId]; p; p = p->next)
        {
            if (p->priority == TENTPRIORITY_LOW)
            {
                pTemp = p;
                break;
            }
        }

        if (!pTemp)
        {
            Con_DPrintf("Couldn't alloc a high priority TENT!\n");
            return NULL;
        }
    }
    else
    {
        pTemp = D_003FE288[clientId];
        D_003FE288[clientId] = pTemp->next;
        pTemp->next = D_003FE290[clientId];
        D_003FE290[clientId] = pTemp;
    }

    func_0025AFA0(pTemp, model, clientId);
    pTemp->priority = TENTPRIORITY_HIGH;
    VectorCopy(org, pTemp->entity.origin);
    return pTemp;
}

void func_0025B1E0(TEMPENTITY *pTemp, float damp, int clientId)
{
    float fvol;
    sfx_t *sounds[6];
    int isshellcasing = 0;
    int count;
    int zvel;
    int pitch;

    memset(sounds, 0, sizeof(sounds));

    switch (pTemp->hitSound)
    {
    case BOUNCE_GLASS:
        count = 3;
        sounds[0] = D_003991F0;
        sounds[1] = D_003991D8;
        sounds[2] = D_00399258;
        break;
    case BOUNCE_METAL:
        count = 3;
        sounds[0] = D_0039923C;
        sounds[1] = D_003991C8;
        sounds[2] = D_0039921C;
        break;
    case BOUNCE_FLESH:
        count = 6;
        sounds[0] = D_0039926C;
        sounds[1] = D_003991D4;
        sounds[2] = D_003991FC;
        sounds[3] = D_00399250;
        sounds[4] = D_00399264;
        sounds[5] = D_00399208;
        break;
    case BOUNCE_WOOD:
        count = 3;
        sounds[0] = D_00399248;
        sounds[1] = D_003991E0;
        sounds[2] = D_00399278;
        break;
    case BOUNCE_SHRAP:
        count = 5;
        sounds[0] = D_003991DC;
        sounds[1] = D_00399230;
        sounds[2] = D_0039925C;
        sounds[3] = D_0039927C;
        sounds[4] = D_0039922C;
        break;
    case BOUNCE_SHOTSHELL:
        count = 3;
        sounds[0] = D_00399260;
        sounds[1] = D_00399210;
        sounds[2] = D_00399254;
        isshellcasing = 1;
        fvol = 0.5f;
        goto play;
    case BOUNCE_SHELL:
        count = 3;
        sounds[0] = D_00399240;
        sounds[1] = D_003991EC;
        sounds[2] = D_00399218;
        isshellcasing = 1;
        break;
    case BOUNCE_CONCRETE:
        count = 3;
        sounds[0] = D_003991F4;
        sounds[1] = D_00399244;
        sounds[2] = D_003991D0;
        break;
    default:
        return;
    }

    fvol = 0.8f;
play:
    zvel = abs((int)pTemp->entity.baseline.origin[2]);

    if (isshellcasing)
    {
        if (zvel < 200 && RandomLong(0, 3))
            return;
    }
    else
    {
        if (RandomLong(0, 5))
            return;
    }

    if (count <= 0)
        return;

    if (damp > 0)
    {
        if (isshellcasing)
            fvol *= min(1.0f, (float)zvel / 350);
        else
            fvol *= min(1.0f, (float)zvel / 450);

        if (RandomLong(0, 3) || isshellcasing)
            pitch = 100;
        else
            pitch = RandomLong(90, 124);

        S_StartDynamicSound(-1, 0, sounds[RandomLong(0, count - 1)], pTemp->entity.origin, fvol, 1.0f, 0, pitch, clientId);
    }
}

int func_0025B588(cl_entity_t *ent, int clientId)
{
    int i;
    vec3_t mins, maxs;

    if (ent->model && cl_numvisedicts[clientId] < 512)
    {
        for (i = 0; i < 3; i++)
        {
            mins[i] = ent->origin[i] + ent->model->mins[i];
            maxs[i] = ent->origin[i] + ent->model->maxs[i];
        }

        if (PVSNode(cl[clientId].worldmodel->nodes, mins, maxs, clientId))
        {
            ent->index = -1;
            cl_visedicts[clientId][cl_numvisedicts[clientId]++] = *ent;
            return 1;
        }
    }

    return 0;
}

#ifdef NON_MATCHING
void func_0025B740(int clientId)
{
    TEMPENTITY *pTemp, *pnext, *pprev;
    float freq, gravity, gravitySlow, life, fastFreq, frametime;
    int active, i;
    vec3_t traceNormal;
    pmtrace_t trace;
    float traceFraction, damp, proj;
    dlight_t *dl;
    hull_t *hull;

    if (!D_003FE290)
        return;

    if (!cl[clientId].worldmodel)
        return;

    frametime = cl[clientId].time - cl[clientId].oldtime;
    D_00331BFC = (D_00331BFC + 1) & 31;
    pTemp = D_003FE290[clientId];

    if (frametime == 0)
    {
        while (pTemp)
        {
            func_0025B588(&pTemp->entity, clientId);
            pTemp = pTemp->next;
        }
        return;
    }

    pprev = NULL;
    fastFreq = cl[clientId].time * 5.5f;
    gravity = -frametime * sv_gravity.value;
    gravitySlow = gravity * 0.5f;

    for (; pTemp; pTemp = pnext)
    {
        active = 1;
        life = pTemp->die - cl[clientId].time;
        pnext = pTemp->next;

        if (life < 0)
        {
            if (pTemp->flags & FTENT_FADEOUT)
            {
                if (pTemp->entity.rendermode == kRenderNormal)
                    pTemp->entity.rendermode = kRenderTransTexture;
                pTemp->entity.renderamt = pTemp->entity.baseline.renderamt * (1 + life * pTemp->fadeSpeed);
                if (pTemp->entity.renderamt <= 0)
                    active = 0;
            }
            else
            {
                active = 0;
            }
        }

        if (!active)
        {
            if (pTemp->flags & FTENT_NOMODEL_HACK)
                D_003FE298[clientId]--;

            pTemp->next = D_003FE288[clientId];
            D_003FE288[clientId] = pTemp;

            if (!pprev)
                D_003FE290[clientId] = pnext;
            else
                pprev->next = pnext;
            continue;
        }

        pprev = pTemp;
        VectorCopy(pTemp->entity.origin, pTemp->entity.prevorigin);

        if (pTemp->flags & FTENT_PLYRATTACHMENT)
        {
            cl_entity_t *pClient = &cl_entities[clientId][pTemp->clientIndex];

            VectorAdd(pClient->origin, pTemp->tentOffset, pTemp->entity.origin);
        }
        else if (pTemp->flags & FTENT_SINEWAVE)
        {
            pTemp->x += pTemp->entity.baseline.origin[0] * frametime;
            pTemp->y += pTemp->entity.baseline.origin[1] * frametime;

            pTemp->entity.origin[0] = pTemp->x + sinf(pTemp->entity.baseline.origin[2] + cl[clientId].time * pTemp->entity.prevframe) * 10 * pTemp->entity.scale;
            pTemp->entity.origin[1] = pTemp->y + sinf(pTemp->entity.baseline.origin[2] + fastFreq + 0.7f) * 8 * pTemp->entity.scale;
            pTemp->entity.origin[2] += pTemp->entity.baseline.origin[2] * frametime;
        }
        else if (pTemp->flags & FTENT_SPIRAL)
        {
            float s, c;

            s = sinf(pTemp->entity.baseline.origin[2] + fastFreq);
            c = cosf(pTemp->entity.baseline.origin[2] + fastFreq);

            pTemp->entity.origin[0] += pTemp->entity.baseline.origin[0] * frametime + sinf(cl[clientId].time * 20 + (int)pTemp) * 8;
            pTemp->entity.origin[1] += pTemp->entity.baseline.origin[1] * frametime + sinf(cl[clientId].time * 30 + (int)pTemp) * 4;
            pTemp->entity.origin[2] += pTemp->entity.baseline.origin[2] * frametime;
        }
        else
        {
            for (i = 0; i < 3; i++)
                pTemp->entity.origin[i] += pTemp->entity.baseline.origin[i] * frametime;
        }

        if (pTemp->flags & FTENT_SPRANIMATE)
        {
            pTemp->entity.frame += frametime * pTemp->entity.framerate;
            if (pTemp->entity.frame >= pTemp->frameMax)
            {
                pTemp->entity.frame = pTemp->entity.frame - (int)pTemp->entity.frame;

                if (!(pTemp->flags & FTENT_SPRANIMATELOOP))
                {
                    pTemp->die = cl[clientId].time;
                    continue;
                }
            }
        }
        else if (pTemp->flags & FTENT_SPRCYCLE)
        {
            pTemp->entity.frame += frametime * 10;
            if (pTemp->entity.frame >= pTemp->frameMax)
                pTemp->entity.frame = pTemp->entity.frame - (int)pTemp->entity.frame;
        }

        if (pTemp->flags & FTENT_ROTATE)
        {
            pTemp->entity.angles[0] += pTemp->entity.baseline.angles[0] * frametime;
            pTemp->entity.angles[1] += pTemp->entity.baseline.angles[1] * frametime;
            pTemp->entity.angles[2] += pTemp->entity.baseline.angles[2] * frametime;
        }

        if (pTemp->flags & (FTENT_COLLIDEALL | FTENT_COLLIDEWORLD))
        {
            traceFraction = 1;

            if (pTemp->flags & FTENT_COLLIDEALL)
            {
                pmove[clientId].usehull = 2;
                trace = PM_PlayerTrace(pTemp->entity.prevorigin, pTemp->entity.origin, 2, clientId);

                if (trace.fraction != 1)
                {
                    if (!trace.ent || pmove[clientId].physents[trace.ent].info != pTemp->clientIndex)
                    {
                        traceFraction = trace.fraction;
                        VectorCopy(trace.plane.normal, traceNormal);
                    }
                }
            }
            else if (pTemp->flags & FTENT_COLLIDEWORLD)
            {
                memset(&trace, 0, sizeof(trace_t));
                hull = &cl[clientId].worldmodel->hulls[0];
                trace.fraction = 1;
                trace.allsolid = 1;
                SV_RecursiveHullCheck(hull, hull->firstclipnode, 0, 1, pTemp->entity.prevorigin, pTemp->entity.origin, (trace_t *)&trace);

                if (trace.fraction != 1)
                {
                    traceFraction = trace.fraction;
                    VectorCopy(trace.plane.normal, traceNormal);
                }
            }

            if (traceFraction != 1)
            {
                VectorMA(pTemp->entity.prevorigin, traceFraction * frametime, pTemp->entity.baseline.origin, pTemp->entity.origin);

                damp = pTemp->bounceFactor;
                if (pTemp->flags & (FTENT_GRAVITY | FTENT_SLOWGRAVITY))
                {
                    damp *= 0.5f;
                    if (traceNormal[2] > 0.9f)
                    {
                        if (pTemp->entity.baseline.origin[2] <= 0 && pTemp->entity.baseline.origin[2] >= gravity * 3)
                        {
                            damp = 0;
                            pTemp->flags &= ~(FTENT_ROTATE | FTENT_GRAVITY | FTENT_SLOWGRAVITY | FTENT_COLLIDEWORLD | FTENT_SMOKETRAIL);
                            pTemp->entity.angles[0] = 0;
                            pTemp->entity.angles[2] = 0;
                        }
                    }
                }

                if (pTemp->hitSound)
                    func_0025B1E0(pTemp, damp, clientId);

                if (pTemp->flags & FTENT_COLLIDEKILL)
                {
                    pTemp->flags &= ~FTENT_FADEOUT;
                    pTemp->die = cl[clientId].time;
                }
                else
                {
                    if (damp != 0)
                    {
                        proj = DotProduct(pTemp->entity.baseline.origin, traceNormal);
                        VectorMA(pTemp->entity.baseline.origin, -proj * 2, traceNormal, pTemp->entity.baseline.origin);
                        pTemp->entity.angles[1] = -pTemp->entity.angles[1];
                    }

                    if (damp != 1)
                    {
                        VectorScale(pTemp->entity.baseline.origin, damp, pTemp->entity.baseline.origin);
                        VectorScale(pTemp->entity.angles, 0.9f, pTemp->entity.angles);
                    }
                }
            }
        }

        if ((pTemp->flags & FTENT_FLICKER) && D_00331BFC == pTemp->entity.unk194)
        {
            dl = func_002510D0(0);
            VectorCopy(pTemp->entity.origin, dl->origin);
            dl->radius = 60;
            dl->color.r = 255;
            dl->color.g = 120;
            dl->color.b = 0;
            dl->die = cl[clientId].time + 0.01f;
        }

        if (pTemp->flags & FTENT_SMOKETRAIL)
            func_002B2F08(pTemp->entity.prevorigin, pTemp->entity.origin, 1, clientId);

        if (pTemp->flags & FTENT_GRAVITY)
            pTemp->entity.baseline.origin[2] += gravity;
        else if (pTemp->flags & FTENT_SLOWGRAVITY)
            pTemp->entity.baseline.origin[2] += gravitySlow;

        if (!func_0025B588(&pTemp->entity, clientId))
        {
            if (!(pTemp->flags & FTENT_PERSIST))
            {
                pTemp->die = cl[clientId].time;
                pTemp->flags &= ~FTENT_FADEOUT;
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_0025B740);
#endif

#ifdef NON_MATCHING
int CL_FxBlend(cl_entity_t *e)
{
    int blend;
    float offset, dist;
    vec3_t tmp;

    offset = e->index * 363.0f;

    switch (e->renderfx)
    {
    case kRenderFxPulseSlowWide:
        blend = e->renderamt + sinf(cl[0].time * 2 + offset) * 64;
        break;
    case kRenderFxPulseFastWide:
        blend = e->renderamt + sinf(cl[0].time * 8 + offset) * 64;
        break;
    case kRenderFxPulseSlow:
        blend = e->renderamt + sinf(cl[0].time * 2 + offset) * 16;
        break;
    case kRenderFxPulseFast:
        blend = e->renderamt + sinf(cl[0].time * 8 + offset) * 16;
        break;
    case kRenderFxFadeSlow:
        if (e->renderamt > 0)
            e->renderamt -= 1;
        else
            e->renderamt = 0;
        blend = e->renderamt;
        break;
    case kRenderFxFadeFast:
        if (e->renderamt > 3)
            e->renderamt -= 4;
        else
            e->renderamt = 0;
        blend = e->renderamt;
        break;
    case kRenderFxSolidSlow:
        if (e->renderamt < 255)
            e->renderamt += 1;
        else
            e->renderamt = 255;
        blend = e->renderamt;
        break;
    case kRenderFxSolidFast:
        if (e->renderamt < 252)
            e->renderamt += 4;
        else
            e->renderamt = 255;
        blend = e->renderamt;
        break;
    case kRenderFxStrobeSlow:
        blend = 20 * (int)sinf(cl[0].time * 4 + offset);
        if (blend < 0)
            blend = 0;
        else
            blend = e->renderamt;
        break;
    case kRenderFxStrobeFast:
        blend = 20 * (int)sinf(cl[0].time * 16 + offset);
        if (blend < 0)
            blend = 0;
        else
            blend = e->renderamt;
        break;
    case kRenderFxStrobeFaster:
        blend = 20 * (int)sinf(cl[0].time * 36 + offset);
        if (blend < 0)
            blend = 0;
        else
            blend = e->renderamt;
        break;
    case kRenderFxFlickerSlow:
        blend = (int)(sinf(cl[0].time * 2) + sinf(cl[0].time * 17 + offset)) * 20;
        if (blend < 0)
            blend = 0;
        else
            blend = e->renderamt;
        break;
    case kRenderFxFlickerFast:
        blend = (int)(sinf(cl[0].time * 16) + sinf(cl[0].time * 23 + offset)) * 20;
        if (blend < 0)
            blend = 0;
        else
            blend = e->renderamt;
        break;
    case kRenderFxHologram:
    case kRenderFxDistort:
        VectorCopy(e->origin, tmp);
        VectorSubtract(tmp, r_origin, tmp);
        dist = DotProduct(tmp, vpn);

        if (e->renderfx == kRenderFxDistort)
            dist = 1;

        if (dist <= 0)
        {
            blend = 0;
        }
        else
        {
            e->renderamt = 180;
            if (dist <= 100)
                blend = e->renderamt;
            else
                blend = (1.0f - (dist - 100) * (1.0f / 400)) * e->renderamt;
            blend += RandomLong(-32, 31);
        }
        break;
    case kRenderFxNone:
    case kRenderFxNoDissipation:
    case kRenderFxExplode:
    case kRenderFxGlowShell:
    case kRenderFxClampMinScale:
    default:
        blend = e->renderamt;
        break;
    }

    if (blend > 255)
        blend = 255;
    else if (blend < 0)
        blend = 0;

    return blend;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", CL_FxBlend);
#endif

void func_0025C428(cl_entity_t *ent, float transform[3][4])
{
    float scale;
    int axis;
    float offset;

    switch (ent->renderfx)
    {
    case kRenderFxDistort:
    case kRenderFxHologram:
        if (!RandomLong(0, 49))
        {
            axis = RandomLong(0, 1);
            if (axis == 1)
                axis = 2;
            VectorScale(transform[axis], RandomFloat(1.0f, 1.484f), transform[axis]);
        }
        else if (!RandomLong(0, 49))
        {
            axis = RandomLong(0, 1);
            if (axis == 1)
                axis = 2;
            offset = RandomFloat(-10.0f, 10.0f);
            transform[RandomLong(0, 2)][3] += offset;
        }
        break;
    case kRenderFxExplode:
        scale = 1.0f + (cl[0].time - ent->animtime) * 10.0f;
        if (scale > 2)
            scale = 2;
        transform[0][1] *= scale;
        transform[1][1] *= scale;
        transform[2][1] *= scale;
        break;
    }
}

int func_0025C5A8(int x)
{
    if (x < 9) { if (x >= 5) return 1; } return 0;
}

mspriteframedesc_t *R_GetSpriteFrame(msprite_t *pSprite, int frame)
{
    mspriteframedesc_t *pframes;

    if (frame >= pSprite->numframes || frame < 0)
    {
        Con_Printf("Sprite: no such frame %d\n", frame);
        frame = 0;
    }

    pframes = pSprite->frames;
    return &pframes[frame];
}

void R_GetSpriteAxes(cl_entity_t *pEntity, int type, vec3_t forward, vec3_t right, vec3_t up)
{
    int i;
    float dot, angle, sr, cr;
    vec3_t tvec;

    if (pEntity->angles[2] != 0 && type == 0)
        type = 3;

    switch (type)
    {
    case 4:
        tvec[0] = -modelorg[0];
        tvec[1] = -modelorg[1];
        tvec[2] = -modelorg[2];
        VectorNormalize(tvec);
        dot = tvec[2];
        if (dot > 0.999848f || dot < -0.999848f)
            return;
        up[0] = 0;
        up[1] = 0;
        up[2] = 1;
        right[0] = tvec[1];
        right[1] = -tvec[0];
        right[2] = 0;
        VectorNormalize(right);
        forward[0] = -right[1];
        forward[1] = right[0];
        forward[2] = 0;
        break;
    case 0:
        for (i = 0; i < 3; i++)
        {
            up[i] = vup[i];
            right[i] = vright[i];
            forward[i] = vpn[i];
        }
        break;
    case 1:
        dot = vpn[2];
        if (dot > 0.999848f || dot < -0.999848f)
            return;
        up[0] = 0;
        up[1] = 0;
        up[2] = 1;
        right[0] = vpn[1];
        right[1] = -vpn[0];
        right[2] = 0;
        VectorNormalize(right);
        forward[0] = -right[1];
        forward[1] = right[0];
        forward[2] = 0;
        break;
    case 2:
        AngleVectors(pEntity->angles, forward, right, up);
        break;
    case 3:
        angle = currententity->angles[2] * (M_PI / 180);
        sr = sinf(angle);
        cr = cosf(angle);
        for (i = 0; i < 3; i++)
        {
            forward[i] = vpn[i];
            right[i] = vright[i] * cr + vup[i] * sr;
            up[i] = vright[i] * -sr + vup[i] * cr;
        }
        break;
    default:
        Sys_Error("R_DrawSprite: Bad sprite type %d", type);
        break;
    }
}

void R_SpriteColor(int *pColor, cl_entity_t *pEntity, int alpha)
{
    int a;

    if (pEntity->rendermode == kRenderTransAdd || pEntity->rendermode == kRenderGlow)
        a = alpha;
    else
        a = 256;

    if (!pEntity->rendercolor.r && !pEntity->rendercolor.g && !pEntity->rendercolor.b)
    {
        pColor[0] = pColor[1] = pColor[2] = (255 * a) >> 8;
    }
    else
    {
        pColor[0] = (pEntity->rendercolor.r * a) >> 8;
        pColor[1] = (pEntity->rendercolor.g * a) >> 8;
        pColor[2] = (pEntity->rendercolor.b * a) >> 8;
    }
}

float *R_GetAttachmentPoint(int entity, int attachment, int clientId)
{
    cl_entity_t *pEnt = &cl_entities[clientId][entity];

    if (attachment)
        return pEnt->attachment[attachment - 1];
    else
        return pEnt->origin;
}

#ifdef NON_MATCHING
void func_0025CA10(vec3_t org, int modelIndex, int reverse, int density, vec3_t angles, int clientId)
{
    TEMPENTITY *pTemp;
    struct model_s *model;
    int i, j, frameCount, size, step;
    float vel, dist;
    vec3_t dir, dest2, dest;
    vec3_t forward, right, up;
    vec3_t vf, vr, vu, vfr, vfru;

    AngleVectors(angles, forward, right, up);

    size = density * 2;
    if (density != 255)
        step = density * 4;
    else
        step = 127;
    if (density == 1)
        step = size * 10;
    if (step <= 0)
        step = 1;

    if (!modelIndex)
    {
        Con_Printf(D_0038AD88);
        return;
    }

    model = cl[clientId].model_precache[modelIndex];
    if (!model)
    {
        Con_Printf(D_0038ACF8, modelIndex);
        return;
    }

    frameCount = ModelFrameCount(model);

    for (i = -size; i <= size; i += step)
    {
        for (j = -size; j <= size; j += step)
        {
            pTemp = func_0025B048(dest, model, clientId);
            if (!pTemp)
                return;

            if (reverse)
            {
                dest[0] = org[0] + RandomFloat(-60, 60);
                dest[1] = org[1] + RandomFloat(-60, 60);
                dest[2] = org[2] + RandomFloat(-500, 500);
                VectorCopy(org, pTemp->entity.origin);
                dest2[0] = dest[0] + i + RandomFloat(-500, 500);
                dest2[1] = dest[1] + j + RandomFloat(-500, 0);
                dest2[2] = dest[2] + RandomFloat(0, 500);
                VectorScale(forward, RandomFloat(-500, 500), vf);
                VectorScale(right, RandomFloat(-500, 500), vr);
                VectorScale(up, RandomFloat(-500, 500), vu);
                VectorAdd(vf, vr, vfr);
                VectorAdd(vfr, vu, vfru);
                VectorAdd(vfru, dest, dest2);
                VectorSubtract(dest2, pTemp->entity.origin, dir);
                vel = dest2[2] / 8;
            }
            else
            {
                pTemp->entity.origin[0] = org[0] + i;
                pTemp->entity.origin[1] = org[1] + j;
                pTemp->entity.origin[2] = org[2] + RandomFloat(-400, 400);
                VectorSubtract(org, pTemp->entity.origin, dir);
                vel = pTemp->entity.origin[2] / 8;
            }

            pTemp->frameMax = frameCount;
            pTemp->entity.framerate = 10;
            pTemp->entity.rendermode = kRenderTransAdd;
            pTemp->entity.renderfx = kRenderFxNoDissipation;
            pTemp->entity.renderamt = pTemp->entity.baseline.renderamt = 200;
            pTemp->entity.framerate = RandomFloat(0.1f, 0.4f);
            pTemp->flags = FTENT_ROTATE | FTENT_FADEOUT;

            dist = VectorNormalize(dir);
            if (vel < 64)
                vel = 64;
            vel += RandomFloat(64, 178);
            VectorScale(dir, vel, pTemp->entity.baseline.origin);
            pTemp->fadeSpeed = 2.0f;
            pTemp->die = cl[clientId].time + (dist / vel - 0.5f);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_0025CA10);
#endif
