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

#define NUMVERTEXNORMALS 162

extern vec3_t D_0050F390[NUMVERTEXNORMALS]; /* avelocities */
extern vec3_t D_003339B0[NUMVERTEXNORMALS]; /* r_avertexnormals */
extern float D_00334148;        /* beamlength */
extern particle_t *D_00399668;
extern int D_00333900[8];       /* ramp1 */

void func_002B0600(void)
{
    int i;

    for (i = 0; i < 2; i++)
        D_00414288[i] = Hunk_AllocName(MAX_BEAMS * sizeof(BEAM), "BeamList");
}

void func_002B0660(int clientId)
{
    int i;

    D_00414298[clientId] = D_00414288[clientId];
    D_00414290[clientId] = NULL;
    memset(D_00414288[clientId], 0, MAX_BEAMS * sizeof(BEAM));
    for (i = 0; i < MAX_BEAMS; i++)
        D_00414288[clientId][i].next = &D_00414288[clientId][i + 1];
    D_00414288[clientId][MAX_BEAMS - 1].next = NULL;
}

void func_002B0708(void)
{
    D_003339AC = !D_003339AC;
    Con_Printf("drawparticles [%s]\n", !D_003339AC ? "OFF" : "ON");
}

void func_002B0758(void)
{
    int i;

    i = COM_CheckParm("-particles");
    if (i)
    {
        D_00399650 = Q_atoi(com_argv[i + 1]);
        if (D_00399650 < 512)
            D_00399650 = 512;
    }
    else
    {
        D_00399650 = 2048;
    }
    D_00399664 = Hunk_AllocName(D_00399650 * sizeof(particle_t), "particles");
    func_002B0600();
    func_002B3598();
    Cmd_AddCommand("drawparticles", func_002B0708);
}

void func_002B0810(cl_entity_t *ent, int clientId)
{
    int i, j, k;
    float fi, fj, fk;
    particle_t *p;
    float vel;
    vec3_t dir;
    vec3_t org;

    VectorCopy(ent->origin, org);
    for (i = -16; i < 16; i += 8)
        for (j = -16; j < 16; j += 8)
            for (k = 0; k < 32; k += 8)
            {
                if (!D_0039966C)
                    return;
                p = D_0039966C;
                fk = k;
                fj = j;
                D_0039966C = p->next;
                fi = i;
                p->next = D_00399660;
                D_00399660 = p;

                p->die = cl[clientId].time + RandomFloat(0.2f, 0.34f);
                p->color = RandomLong(150, 155);
                p->packedColor = 0;
                p->type = pt_slowgrav;

                dir[0] = fj * 8;
                dir[1] = fi * 8;
                dir[2] = fk * 8;

                p->org[0] = org[0] + fi + RandomLong(0, 3);
                p->org[1] = org[1] + fj + RandomLong(0, 3);
                p->org[2] = org[2] + fk + RandomLong(0, 3);

                VectorNormalize(dir);
                vel = RandomFloat(50, 113);
                VectorScale(dir, vel, p->vel);
            }
}

#ifdef NON_MATCHING
void func_002B0A48(cl_entity_t *ent, int clientId)
{
    int i;
    particle_t *p;
    float angle;
    float sp, sy, cp, cy;
    vec3_t forward;
    float dist;

    dist = 64;
    if (!D_0050F390[0][0])
    {
        for (i = 0; i < NUMVERTEXNORMALS * 3; i++)
            D_0050F390[0][i] = RandomFloat(0, 2.55f);
    }

    for (i = 0; i < NUMVERTEXNORMALS; i++)
    {
        angle = cl[clientId].time * D_0050F390[i][0];
        sy = sinf(angle);
        cy = cosf(angle);
        angle = cl[clientId].time * D_0050F390[i][1];
        sp = sinf(angle);
        cp = cosf(angle);
        angle = cl[clientId].time * D_0050F390[i][2];
        sinf(angle);
        cosf(angle);

        forward[0] = cp * cy;
        forward[1] = cp * sy;
        forward[2] = -sp;

        if (!D_0039966C)
            return;
        p = D_0039966C;
        D_0039966C = p->next;
        p->next = D_00399660;
        D_00399660 = p;

        p->die = cl[clientId].time + 0.01f;
        p->color = 0x6F;
        p->packedColor = 0;
        p->type = pt_explode;

        p->org[0] = ent->origin[0] + D_003339B0[i][0] * dist + forward[0] * D_00334148;
        p->org[1] = ent->origin[1] + D_003339B0[i][1] * dist + forward[1] * D_00334148;
        p->org[2] = ent->origin[2] + D_003339B0[i][2] * dist + forward[2] * D_00334148;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B0A48);
#endif

void func_002B0CD0(int clientId)
{
    int i;

    D_0039966C = &D_00399664[0];
    D_00399660 = NULL;
    D_00399668 = NULL;

    for (i = 0; i < D_00399650; i++)
        D_00399664[i].next = &D_00399664[i + 1];
    D_00399664[D_00399650 - 1].next = NULL;

    func_002B0660(clientId);
}

void func_002B0D58(particle_t **ppparticles, int clientId)
{
    particle_t *p, *kill;

    for (;;)
    {
        kill = *ppparticles;
        if (kill && kill->die < cl[clientId].time)
        {
            *ppparticles = kill->next;
            kill->next = D_0039966C;
            D_0039966C = kill;
            continue;
        }
        break;
    }

    for (p = *ppparticles; p; p = p->next)
    {
        for (;;)
        {
            kill = p->next;
            if (kill && kill->die < cl[clientId].time)
            {
                p->next = kill->next;
                kill->next = D_0039966C;
                D_0039966C = kill;
                continue;
            }
            break;
        }
    }
}

void func_002B0E20(void) {
}

void func_002B0E28(int clientId)
{
    vec3_t org, dir;
    int i, count, msgcount, color;

    for (i = 0; i < 3; i++)
        org[i] = MSG_ReadCoord();
    for (i = 0; i < 3; i++)
        dir[i] = func_00243370() * (1.0f / 16);
    msgcount = MSG_ReadByte();
    color = MSG_ReadByte();
    if (msgcount == 255)
        count = 1024;
    else
        count = msgcount;

    func_002B17F8(org, dir, color, count, clientId);
}

void func_002B0EF8(vec3_t org, int clientId)
{
    int i, j;
    particle_t *p;

    for (i = 0; i < 1024; i++)
    {
        if (!D_0039966C)
            return;
        p = D_0039966C;
        D_0039966C = p->next;
        p->next = D_00399660;
        D_00399660 = p;

        p->die = cl[clientId].time + 5;
        p->color = D_00333900[0];
        p->packedColor = 0;
        p->ramp = RandomLong(0, 3);
        if (i & 1)
            p->type = pt_explode;
        else
            p->type = pt_explode2;

        do
        {
            for (j = 0; j < 3; j++)
                p->vel[j] = RandomFloat(-512, 512);
        } while (DotProduct(p->vel, p->vel) > 512 * 512);

        for (j = 0; j < 3; j++)
            p->org[j] = org[j] + p->vel[j] / 4;
    }
}

void func_002B10C8(vec3_t org, int colorStart, int colorLength, int clientId)
{
    int i, j;
    particle_t *p;
    int colorMod = 0;

    for (i = 0; i < 512; i++)
    {
        if (!D_0039966C)
            return;
        p = D_0039966C;
        D_0039966C = p->next;
        p->next = D_00399660;
        D_00399660 = p;

        p->die = cl[clientId].time + 0.3f;
        p->color = colorStart + (colorMod % colorLength);
        p->packedColor = 0;
        colorMod++;
        p->type = pt_blob;

        for (j = 0; j < 3; j++)
        {
            p->org[j] = org[j] + RandomFloat(-16, 16);
            p->vel[j] = RandomFloat(-256, 256);
        }
    }
}

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B1238);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B1420);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B14D8);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B15D0);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B17F8);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B1AB8);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B1C00);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B1D78);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B1F20);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B2128);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B2330);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B2598);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B27B8);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B2938);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B2C70);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B2F08);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B3598);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B3628);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B3680);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B37F8);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B3A10);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B3EE8);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B4638);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B4680);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B4770);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B4920);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B4948);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B4BB0);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B4CE8);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B4E08);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B4F58);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B5028);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B50F0);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B5228);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B53A0);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B5458);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B5590);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B5648);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", ScreenTransform);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B57C8);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B5DC8);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B61D8);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B6460);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B6770);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B6CF0);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B72D8);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B7318);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B74D0);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B75F0);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B7CA8);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B7E98);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B82F0);

INCLUDE_ASM("asm/nonmatchings/engine/r_part", func_002B8C18);
