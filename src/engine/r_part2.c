#include "common.h"
#include "engine/cmd.h"
#include "engine/cvar.h"
#include "engine/client.h"
#include "engine/render.h"
#include "engine/model.h"
#include "engine/glquake.h"

typedef enum
{
    pt_static,
    pt_grav,
    pt_slowgrav,
    pt_fire,
    pt_explode,
    pt_explode2,
    pt_blob,
    pt_blob2,
    pt_vox_slowgrav,
    pt_vox_grav,
    pt_clientcustom
} ptype_t;

typedef struct particle_s
{
    vec3_t org;                 /* 0x00 */
    short color;                /* 0x0C */
    short packedColor;          /* 0x0E */
    struct particle_s *next;    /* 0x10 */
    vec3_t vel;                 /* 0x14 */
    float ramp;                 /* 0x20 */
    float die;                  /* 0x24 */
    ptype_t type;               /* 0x28 */
} particle_t; /* 0x2C */

/* BEAM do HL (0x7C), lista ligada por next */
typedef struct beam_s
{
    struct beam_s *next;        /* 0x00 */
    byte pad4[0x7C - 0x4];
} BEAM; /* 0x7C */

#define MAX_BEAMS 128

extern BEAM *D_00414288[2];     /* cl_beams (por jogador) */
extern BEAM *D_00414290[2];     /* gBeamHead */
extern BEAM *D_00414298[2];     /* gpFreeBeams */

extern int D_00399650;          /* r_numparticles */
extern particle_t *D_00399664;  /* particles */
extern particle_t *D_0039966C;  /* free_particles */
extern particle_t *D_00399660;  /* active_particles */
extern int D_003339AC;          /* r_drawparticles */
extern vec3_t r_origin;

int COM_CheckParm(char *parm);
int Q_atoi(char *str);
void *Hunk_AllocName(int size, char *name);
float RandomFloat(float low, float high);
int RandomLong(int low, int high);
void func_002B3598(void);
int func_00243370(void);        /* MSG_ReadChar */
float MSG_ReadCoord(void);
int MSG_ReadByte(void);
void func_002B17F8(vec3_t org, vec3_t dir, int color, int count, int clientId);
void func_002B1C00(vec3_t pos, int count, int velocityMin, int velocityMax, int clientId);

#define NUMVERTEXNORMALS 162

extern vec3_t D_0050F390[NUMVERTEXNORMALS]; /* avelocities */
extern vec3_t D_003339B0[NUMVERTEXNORMALS]; /* r_avertexnormals */
extern float D_00334148;        /* beamlength */
extern particle_t *D_00399668;
extern int D_00333900[8];       /* ramp1 */
extern cvar_t D_00331B70;        /* tracerlength */


void func_002B15D0(vec3_t pos, int clientId)
{
    int i, count;
    float dist;
    vec3_t dir;
    particle_t *p;

    VectorSubtract(pos, r_origin, dir);
    dist = Length(dir);
    if (dist > 1000)
        dist = 1000;
    count = (1000 - dist) * 0.01f;
    if (!count)
        count = 1;

    func_002B1C00(pos, 2, -200, 200, clientId);

    count *= 4;
    for (i = 0; i < count; i++)
    {
        if (!D_0039966C)
            return;
        p = D_0039966C;
        D_0039966C = p->next;
        p->next = D_00399660;
        D_00399660 = p;

        VectorCopy(pos, p->org);
        dir[0] = RandomFloat(-1, 1);
        dir[1] = RandomFloat(-1, 1);
        dir[2] = RandomFloat(-1, 1);
        VectorCopy(dir, p->vel);
        VectorScale(p->vel, RandomFloat(50, 100), p->vel);
        p->color = 0;
        p->packedColor = 0;
        p->type = pt_grav;
        p->die = cl[clientId].time + 0.5f;
    }
}

void func_002B17F8(vec3_t org, vec3_t dir, int color, int count, int clientId)
{
    int i, j;
    particle_t *p;

    for (i = 0; i < count; i++)
    {
        if (!D_0039966C)
            return;
        p = D_0039966C;
        D_0039966C = p->next;
        p->next = D_00399660;
        D_00399660 = p;

        if (count == 1024)
        {
            p->die = cl[clientId].time + 5;
            p->color = D_00333900[0];
            p->packedColor = 0;
            p->ramp = RandomLong(0, 3);
            if (i & 1)
            {
                p->type = pt_explode;
                for (j = 0; j < 3; j++)
                {
                    p->org[j] = org[j] + RandomFloat(-16, 16);
                    p->vel[j] = RandomFloat(-256, 256);
                }
            }
            else
            {
                p->type = pt_explode2;
                for (j = 0; j < 3; j++)
                {
                    p->org[j] = org[j] + RandomFloat(-16, 16);
                    p->vel[j] = RandomFloat(-256, 256);
                }
            }
        }
        else
        {
            p->die = cl[clientId].time + RandomFloat(0, 0.4f);
            p->color = (color & ~7) + RandomLong(0, 7);
            p->packedColor = 0;
            p->type = pt_slowgrav;
            for (j = 0; j < 3; j++)
            {
                p->org[j] = org[j] + RandomFloat(-8, 8);
                p->vel[j] = dir[j] * 15;
            }
        }
    }
}

void func_002B1AB8(vec3_t org, int clientId)
{
    int i, j;
    particle_t *p;

    for (i = 0; i < 15; i++)
    {
        if (!D_0039966C)
            return;
        p = D_0039966C;
        D_0039966C = p->next;
        p->next = D_00399660;
        D_00399660 = p;

        for (j = 0; j < 3; j++)
        {
            p->org[j] = org[j];
            p->vel[j] = RandomFloat(-32, 32);
        }
        p->vel[2] = RandomFloat(80, 143);
        p->die = cl[clientId].time + 2;
        p->type = pt_blob2;
        p->color = 254;
        p->packedColor = 0;
        p->ramp = 0;
    }
}


void func_002B1C00(vec3_t pos, int count, int velocityMin, int velocityMax, int clientId)
{
    int i, j;
    particle_t *p;

    for (i = 0; i < count; i++)
    {
        if (!D_0039966C)
            return;
        p = D_0039966C;
        D_0039966C = p->next;
        p->next = D_00399668;
        D_00399668 = p;

        p->die = cl[clientId].time + RandomFloat(0.1f, 0.5f);
        p->ramp = 0.5f;
        p->color = 5;
        p->packedColor = 255;
        p->type = pt_grav;

        for (j = 0; j < 3; j++)
        {
            p->org[j] = pos[j];
            p->vel[j] = RandomFloat(velocityMin, velocityMax);
        }
    }
}

void func_002B1D78(vec3_t pos, vec3_t dir, int color, int count, float speed, int velocityMin, int velocityMax, int clientId)
{
    int i, j;
    particle_t *p;
    vec3_t initialVelocity;

    VectorScale(dir, speed, initialVelocity);
    for (i = 0; i < count; i++)
    {
        if (!D_0039966C)
            return;
        p = D_0039966C;
        D_0039966C = p->next;
        p->next = D_00399668;
        D_00399668 = p;

        p->die = cl[clientId].time + RandomFloat(0.1f, 0.5f);
        p->color = color;
        p->packedColor = 255;
        p->type = pt_grav;
        p->ramp = 1;

        for (j = 0; j < 3; j++)
        {
            p->org[j] = pos[j];
            p->vel[j] = initialVelocity[j] + RandomFloat(velocityMin, velocityMax);
        }
    }
}

void func_002B1F20(vec3_t pos, int size, int color, int clientId, float life)
{
    int i, j, k;
    particle_t *p;
    vec3_t dir, dest;
    float dist;
    int min = -size;

    for (i = -16; i < 16; i++)
        for (j = -16; j < 16; j++)
            for (k = 0; k < 1; k++)
            {
                if (!D_0039966C)
                    return;
                p = D_0039966C;
                D_0039966C = p->next;
                p->next = D_00399660;
                D_00399660 = p;

                p->color = color + RandomLong(0, 10);
                p->packedColor = 0;
                p->type = pt_static;
                VectorCopy(pos, p->org);

                dest[0] = pos[0] + RandomFloat(min, size);
                dest[1] = pos[1] + RandomFloat(min, size);
                dest[2] = pos[2] + RandomFloat(min, size);

                VectorSubtract(dest, p->org, dir);
                dist = VectorNormalize(dir) / life;
                p->die = cl[clientId].time + life + RandomFloat(-0.5f, 0.5f);
                VectorScale(dir, dist, p->vel);
            }
}

#ifdef NON_MATCHING
void func_002B2128(vec3_t org, int clientId)
{
    int i, j, k;
    particle_t *p;
    float vel;
    vec3_t dir;

    for (i = -16; i < 16; i++)
        for (j = -16; j < 16; j++)
            for (k = 0; k < 1; k++)
            {
                if (!D_0039966C)
                    return;
                p = D_0039966C;
                D_0039966C = p->next;
                p->next = D_00399660;
                D_00399660 = p;

                p->die = cl[clientId].time + RandomFloat(2, 2.62f);
                p->color = RandomLong(224, 231);
                p->packedColor = 0;
                p->type = pt_slowgrav;

                dir[0] = j * 8.0f + RandomFloat(0, 7);
                dir[1] = i * 8.0f + RandomFloat(0, 7);
                dir[2] = 256;

                p->org[0] = org[0] + dir[0];
                p->org[1] = org[1] + dir[1];
                p->org[2] = org[2] + RandomFloat(0, 63);

                VectorNormalize(dir);
                vel = RandomFloat(50, 113);
                VectorScale(dir, vel, p->vel);
            }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B2128);
#endif

void func_002B2330(vec3_t org, int reverse, int clientId)
{
    int i, j;
    particle_t *p;
    float vel;
    vec3_t dir, dest;
    float flDist;

    for (i = -256; i <= 256; i += 32)
    {
        for (j = -256; j <= 256; j += 32)
        {
            if (!D_0039966C)
                return;
            p = D_0039966C;
            D_0039966C = p->next;
            p->next = D_00399660;
            D_00399660 = p;

            if (reverse)
            {
                VectorCopy(org, p->org);
                dest[0] = org[0] + i;
                dest[1] = org[1] + j;
                dest[2] = org[2] + RandomFloat(100, 800);
                VectorSubtract(dest, p->org, dir);
                vel = dest[2] / 8;
            }
            else
            {
                p->org[0] = org[0] + i;
                p->org[1] = org[1] + j;
                p->org[2] = org[2] + RandomFloat(100, 800);
                VectorSubtract(org, p->org, dir);
                vel = p->org[2] / 8;
            }

            p->color = 244;
            p->packedColor = 0;
            p->type = pt_static;

            flDist = VectorNormalize(dir);
            if (vel < 64)
                vel = 64;
            vel += RandomFloat(64, 128);
            VectorScale(dir, vel, p->vel);
            p->die = cl[clientId].time + (flDist / vel);
        }
    }
}

#ifdef NON_MATCHING
void func_002B2598(vec3_t org, int clientId)
{
    int i, j, k;
    particle_t *p;
    float vel;
    vec3_t dir;

    for (i = -16; i < 16; i += 4)
        for (j = -16; j < 16; j += 4)
            for (k = -24; k < 32; k += 4)
            {
                if (!D_0039966C)
                    return;
                p = D_0039966C;
                D_0039966C = p->next;
                p->next = D_00399660;
                D_00399660 = p;

                p->die = cl[clientId].time + RandomFloat(0.2f, 0.34f);
                p->color = RandomLong(7, 14);
                p->packedColor = 0;
                p->type = pt_slowgrav;

                dir[0] = (float)j * 8;
                dir[1] = (float)i * 8;
                dir[2] = (float)k * 8;

                p->org[0] = org[0] + i + RandomFloat(0, 3);
                p->org[1] = org[1] + j + RandomFloat(0, 3);
                p->org[2] = org[2] + k + RandomFloat(0, 3);

                VectorNormalize(dir);
                vel = RandomFloat(50, 113);
                VectorScale(dir, vel, p->vel);
            }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B2598);
#endif


void func_002B27B8(vec3_t start, vec3_t end, int clientId)
{
    vec3_t vec;
    float len;
    particle_t *p;

    VectorSubtract(end, start, vec);
    len = VectorNormalize(vec);
    VectorScale(vec, 5, vec);

    while (len > 0)
    {
        len -= 5;

        if (!D_0039966C)
            return;
        p = D_0039966C;
        D_0039966C = p->next;
        p->next = D_00399660;
        D_00399660 = p;

        VectorCopy(vec3_origin, p->vel);
        p->die = cl[clientId].time + 30;
        p->color = 75;
        p->packedColor = 0;
        p->type = pt_static;
        VectorCopy(start, p->org);
        VectorAdd(start, vec, start);
    }
}

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B2938);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B2C70);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B2F08);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B3598);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B3628);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B3680);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B37F8);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B3A10);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B3EE8);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B4638);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B4680);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B4770);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B4920);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B4948);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B4BB0);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B4CE8);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B4E08);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B4F58);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B5028);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B50F0);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B5228);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B53A0);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B5458);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B5590);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B5648);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", ScreenTransform);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B57C8);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B5DC8);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B61D8);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B6460);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B6770);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B6CF0);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B72D8);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B7318);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B74D0);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B75F0);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B7CA8);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B7E98);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B82F0);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B8C18);
