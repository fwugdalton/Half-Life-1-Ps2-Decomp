#include "common.h"
#include "engine/cvar.h"
#include "engine/mathlib.h"
#include "engine/sound.h"
#include "engine/cl_entity.h"
#include "engine/client.h"
#include "engine/model.h"
#include "engine/cmd.h"

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

#define BOUNCE_SHELL        0x20
#define BOUNCE_SHOTSHELL    0x80

#define SHARD_VOLUME        12.0f

#define kRenderFxNoDissipation 14

extern int D_003FE298[2];
extern cl_entity_t *cl_entities[2];
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
    void (*hitcallback)();      /* 0x24 */
    void (*callback)();         /* 0x28 */
    short clientIndex;          /* 0x2C */
    vec3_t tentOffset;          /* 0x30 */
    cl_entity_t entity;         /* 0x3C */
} TEMPENTITY;

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
extern int cl_shellchrome;

int func_00254880(char *name);
void func_0025AEA8(int arg);
TEMPENTITY *func_0025B0F8(vec3_t org, struct model_s *model, int clientId);
TEMPENTITY *func_0025B048(vec3_t org, struct model_s *model, int clientId);
int ModelFrameCount(struct model_s *model);
void *func_002B1420(vec3_t org, vec3_t vel, float life, int clientId);
sfx_t *S_PrecacheSound(char *name);
void S_StartDynamicSound(int entnum, int entchannel, sfx_t *sfx, vec3_t origin, float fvol, float attenuation, int flags, int pitch, int clientId);
int RandomLong(int lLow, int lHigh);
float RandomFloat(float flLow, float flHigh);

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

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_00256F20);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_002571D0);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_00257398);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_00257438);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_002574B0);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_002577D8);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_002578C0);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_002579E0);

INCLUDE_RODATA("asm/nonmatchings/engine/cl_tent", D_0038AD88);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_00257E28);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_00257F88);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_00258008);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_00258070);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_00258108);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_00258410);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_002585C8);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_00258778);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_0025AEA8);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_0025AFA0);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_0025B048);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_0025B0F8);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_0025B1E0);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_0025B588);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_0025B740);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", CL_FxBlend);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_0025C428);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_0025C5A8);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", R_GetSpriteFrame);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", R_GetSpriteAxes);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", R_SpriteColor);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", R_GetAttachmentPoint);

INCLUDE_ASM("asm/nonmatchings/engine/cl_tent", func_0025CA10);
