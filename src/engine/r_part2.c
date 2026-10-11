#include "common.h"
#include "engine/cmd.h"
#include "engine/cvar.h"
#include "engine/client.h"
#include "engine/render.h"
#include "engine/model.h"
#include "engine/glquake.h"
#include "engine/cl_entity.h"

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
    int type;                   /* 0x04 */
    int flags;                  /* 0x08 */
    vec3_t source;              /* 0x0C */
    vec3_t target;              /* 0x18 */
    vec3_t delta;               /* 0x24 */
    float t;                    /* 0x30 */
    float freq;                 /* 0x34 */
    float die;                  /* 0x38 */
    float width;                /* 0x3C */
    float amplitude;            /* 0x40 */
    float r, g, b;              /* 0x44 */
    float brightness;           /* 0x50 */
    float speed;                /* 0x54 */
    float frameRate;            /* 0x58 */
    float frame;                /* 0x5C */
    int segments;               /* 0x60 */
    int startEntity;            /* 0x64 */
    int endEntity;              /* 0x68 */
    int modelIndex;             /* 0x6C */
    int frameCount;             /* 0x70 */
    struct model_s *pFollowModel; /* 0x74 */
    struct particle_s *particles; /* 0x78 */
} BEAM; /* 0x7C */

int ModelFrameCount(struct model_s *model);
int CL_FxBlend(cl_entity_t *ent);
void func_002B75F0(BEAM *pbeam, float frametime, int clientId);

extern int D_00399658[2];               /* cl_numbeamentities */
extern cl_entity_t *D_0050EF88[2][64];  /* cl_beamentities */

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
extern vec3_t vpn, vup, vright;
extern unsigned short *host_basepal;

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
extern int D_00333940[8];       /* ramp3 */
extern int D_00333920[8];       /* ramp2 */
extern int D_00333960[9];       /* gSparkRamp */
extern int D_00399540;          /* particletexture */
extern int D_0033102C;
extern int D_00331044;
extern cvar_t sv_gravity;

void glEnable(int cap);
void glDisable(int cap);
void glTexEnvf(int target, int pname, float param);
void glBlendFunc(int sfactor, int dfactor);
void func_0023AF68(void);

enum
{
    kRenderNormal,
    kRenderTransColor,
    kRenderTransTexture,
    kRenderGlow,
    kRenderTransAlpha,
    kRenderTransAdd
};

extern color24 D_00333988[];    /* gTracerColors */
extern float D_00334178[];      /* tamanho do tracer por tipo */
extern cvar_t D_00331B88;       /* tracerred */
extern cvar_t D_00331BA0;       /* tracergreen */
extern cvar_t D_00331BB8;       /* tracerblue */
extern cvar_t D_00331BD0;       /* traceralpha */
extern model_t *D_003991E4;     /* sprite do tracer */

int tri_SpriteTexture(model_t *pSpriteModel, int frame);
void tri_RenderMode(int mode);
void tri_Color4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void tri_Brightness(float brightness);
void func_002A0EE0(int);        /* tri_CullFace */
int ScreenTransform(vec3_t point, vec3_t screen);
void glBegin(int mode);
void glEnd(void);
void glTexCoord2f(float s, float t);
void func_0023C218(float x, float y, float z); /* glVertex3f */
void func_002B3EE8(int clientId);
void func_002B7CA8(int clientId);
void func_002B0D58(particle_t **ppparticles, int clientId);
extern int D_00414280;          /* tracercount */

#define PART_BUFVERTS 0x4A

extern qvec_t D_00414300[2][PART_BUFVERTS]; /* vértices */
extern qvec_t D_00414C80[2][PART_BUFVERTS]; /* cores */
extern qvec_t D_00415600[2][PART_BUFVERTS]; /* st */
extern qvec_t *D_00334168;      /* ponteiro de vértice (uncached) */
extern qvec_t *D_0033416C;      /* ponteiro de cor (uncached) */
extern int D_00334164;          /* buffer atual */
extern int D_00334170;          /* número de vértices */
extern int D_00399534;
extern int D_00399598;
extern qvec_t *D_00331030;
extern qvec_t *D_00331034;
extern qvec_t *D_00331038;
extern int D_0033103C;

void func_0023D520(void);
int func_0023D5F0(void);
void func_0023D618(int);
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

#ifdef NON_MATCHING
void func_002B2938(vec3_t org, vec3_t dir, int pcolor, int speed, int clientId)
{
    particle_t *p;
    int i, j;
    float arc;
    int accel;
    float num;
    vec3_t vel;

    VectorNormalize(dir);

    for (arc = 0.075f, i = 0; i < speed / 5; i++)
    {
        if (!D_0039966C)
            return;
        p = D_0039966C;
        D_0039966C = p->next;
        p->next = D_00399660;
        D_00399660 = p;

        p->die = cl[clientId].time + 3;
        p->color = pcolor + RandomLong(0, 9);
        p->packedColor = 0;
        p->type = pt_vox_slowgrav;

        VectorCopy(org, p->org);
        VectorCopy(dir, vel);
        vel[2] -= arc;
        arc -= 0.005f;

        num = RandomFloat(0, 1);
        accel = speed * num;
        num *= 1.7f;

        VectorScale(vel, num, vel);
        VectorScale(vel, accel, p->vel);

        for (j = 0; j < 2; j++)
        {
            if (!D_0039966C)
                return;
            p = D_0039966C;
            D_0039966C = p->next;
            p->next = D_00399660;
            D_00399660 = p;

            p->die = cl[clientId].time + 3;
            p->color = pcolor + RandomLong(0, 9);
            p->packedColor = 0;
            p->type = pt_vox_slowgrav;

            p->org[0] = org[0] + RandomFloat(-1, 1);
            p->org[1] = org[1] + RandomFloat(-1, 1);
            p->org[2] = org[2] + RandomFloat(-1, 1);

            VectorCopy(dir, vel);
            vel[2] -= arc;

            VectorScale(vel, num, vel);
            VectorScale(vel, accel, p->vel);
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B2938);
#endif

#ifdef NON_MATCHING
void func_002B2C70(vec3_t org, vec3_t dir, int pcolor, int speed, int clientId)
{
    vec3_t vec, pos;
    int pspeed = speed * 3;
    int i, j;
    particle_t *p;

    VectorNormalize(dir);

    for (i = 0; i < speed / 2; i++)
    {
        pos[0] = org[0] + RandomFloat(-3, 3);
        pos[1] = org[1] + RandomFloat(-3, 3);
        pos[2] = org[2] + RandomFloat(-3, 3);
        vec[0] = dir[0] + RandomFloat(-0.06f, 0.06f);
        vec[1] = dir[1] + RandomFloat(-0.06f, 0.06f);
        vec[2] = dir[2] + RandomFloat(-0.06f, 0.06f);

        for (j = 0; j < 8; j++)
        {
            if (!D_0039966C)
                return;
            p = D_0039966C;
            D_0039966C = p->next;
            p->next = D_00399660;
            D_00399660 = p;

            p->die = cl[clientId].time + 1.5f;
            p->color = pcolor + RandomLong(0, 9);
            p->packedColor = 0;
            p->type = pt_vox_grav;

            p->org[0] = pos[0] + RandomFloat(-1, 1);
            p->org[1] = pos[1] + RandomFloat(-1, 1);
            p->org[2] = pos[2] + RandomFloat(-1, 1);

            VectorScale(vec, pspeed, p->vel);
        }
        pspeed -= speed;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B2C70);
#endif

void func_002B2F08(vec3_t start, vec3_t end, int type, int clientId)
{
    vec3_t vec, right, up;
    float len;
    int j;
    particle_t *p;
    int dec;
    float s, c, x, y;

    VectorSubtract(end, start, vec);
    len = VectorNormalize(vec);
    if (type == 7)
    {
        dec = 1;
        VectorMatrix(vec, right, up);
    }
    else if (type < 128)
    {
        dec = 3;
    }
    else
    {
        dec = 1;
        type -= 128;
    }

    VectorScale(vec, dec, vec);

    while (len > 0)
    {
        len -= dec;

        if (!D_0039966C)
            return;
        p = D_0039966C;
        D_0039966C = p->next;
        p->next = D_00399660;
        D_00399660 = p;

        VectorCopy(vec3_origin, p->vel);
        p->die = cl[clientId].time + 2;

        switch (type)
        {
        case 0:
            p->ramp = RandomLong(0, 3);
            p->color = D_00333940[(int)p->ramp];
            p->type = pt_fire;
            for (j = 0; j < 3; j++)
                p->org[j] = start[j] + RandomFloat(-3, 3);
            break;

        case 1:
            p->ramp = RandomLong(2, 5);
            p->color = D_00333940[(int)p->ramp];
            p->type = pt_fire;
            for (j = 0; j < 3; j++)
                p->org[j] = start[j] + RandomFloat(-3, 3);
            break;

        case 2:
            p->type = pt_grav;
            p->color = RandomLong(67, 70);
            for (j = 0; j < 3; j++)
                p->org[j] = start[j] + RandomFloat(-3, 3);
            break;

        case 3:
        case 5:
            p->die = cl[clientId].time + 0.5f;
            p->type = pt_static;
            if (type == 3)
                p->color = 52 + ((D_00414280 & 4) << 1);
            else
                p->color = 230 + ((D_00414280 & 4) << 1);

            D_00414280++;

            VectorCopy(start, p->org);
            if (D_00414280 & 1)
            {
                p->vel[0] = 30 * vec[1];
                p->vel[1] = 30 * -vec[0];
            }
            else
            {
                p->vel[0] = 30 * -vec[1];
                p->vel[1] = 30 * vec[0];
            }
            break;

        case 4:
            p->type = pt_grav;
            p->color = RandomLong(67, 70);
            for (j = 0; j < 3; j++)
                p->org[j] = start[j] + RandomFloat(-3, 3);
            len -= 3;
            break;

        case 6:
            p->ramp = RandomLong(0, 3);
            p->color = D_00333940[(int)p->ramp];
            p->type = pt_fire;
            VectorCopy(start, p->org);
            break;

        case 7:
            x = RandomLong(0, 0xFFFF);
            y = RandomLong(8, 16);
            s = sinf(x) * y;
            c = cosf(x) * y;
            p->org[0] = start[0] + s * right[0] + c * up[0];
            p->org[1] = start[1] + s * right[1] + c * up[1];
            p->org[2] = start[2] + s * right[2] + c * up[2];
            VectorSubtract(start, p->org, p->vel);
            VectorScale(p->vel, 2, p->vel);
            VectorMA(p->vel, RandomFloat(96, 111), vec, p->vel);
            p->die = cl[clientId].time + 2;
            p->type = pt_explode2;
            p->ramp = RandomLong(0, 3);
            p->color = D_00333940[(int)p->ramp];
            break;
        }

        p->packedColor = 0;
        VectorAdd(start, vec, start);
    }
}

void func_002B3598(void)
{
    int i, j;
    qvec_t *st;

    for (i = 0; i < 2; i++)
    {
        st = D_00415600[i];
        for (j = 0; j < PART_BUFVERTS; j += 3)
        {
            st->v[0] = 0;
            st->v[1] = 0;
            st->v[2] = 1.0f;
            st->v[3] = 0;
            st++;
            st->v[0] = 1.0f;
            st->v[1] = 0;
            st->v[2] = 1.0f;
            st->v[3] = 0;
            st++;
            st->v[0] = 0;
            st->v[1] = 1.0f;
            st->v[2] = 1.0f;
            st->v[3] = 0;
            st++;
        }
    }
}

void func_002B3628(void)
{
    if (D_00334170 > 0)
    {
        D_00399534++;
        D_00399598 += D_00334170;
        D_00334170 = 0;
        func_0023D520();
        D_00334164 = !D_00334164;
    }
}

#ifdef NON_MATCHING
void func_002B3680(void)
{
    int done = 0;
    int n;

    if (D_00334170 > 0 && (n = D_00334170 + 3) < PART_BUFVERTS)
    {
        D_00331038 = &D_00415600[D_00334164][D_00334170];
        D_00334168 = UNCACHED(&D_00414300[D_00334164][D_00334170]);
        D_0033416C = UNCACHED(&D_00414C80[D_00334164][D_00334170]);
        D_0033103C = 3;
        D_00331030 = &D_00414300[D_00334164][D_00334170];
        D_00331034 = &D_00414C80[D_00334164][D_00334170];
        D_00334170 = n;
        if (func_0023D5F0() == 1)
        {
            done = 1;
            func_0023D618(0);
        }
    }

    if (!done)
    {
        func_002B3628();
        D_00334168 = UNCACHED(&D_00414300[D_00334164][0]);
        D_00331038 = &D_00415600[D_00334164][0];
        D_0033416C = UNCACHED(&D_00414C80[D_00334164][0]);
        D_00334170 = 3;
        D_00331030 = &D_00414300[D_00334164][0];
        D_00331034 = &D_00414C80[D_00334164][0];
        D_0033103C = 3;
        func_0023D618(0);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B3680);
#endif

void func_002B37F8(particle_t *p, vec3_t up, vec3_t right)
{
    unsigned short *pal;
    float r, g, b;
    float scale;

    func_002B3680();

    pal = host_basepal + p->color * 4;
    b = pal[2] * (1.0f / 255);
    g = pal[1] * (1.0f / 255);
    r = pal[0] * (1.0f / 255);

    scale = (p->org[0] - r_origin[0]) * vpn[0] + (p->org[1] - r_origin[1]) * vpn[1] + (p->org[2] - r_origin[2]) * vpn[2];

    D_0033416C->v[0] = b;
    D_0033416C->v[1] = g;
    D_0033416C->v[2] = r;
    D_0033416C->v[3] = 1.0f;
    D_0033416C++;
    D_0033416C->v[0] = b;
    D_0033416C->v[1] = g;
    D_0033416C->v[2] = r;
    D_0033416C->v[3] = 1.0f;
    D_0033416C++;
    D_0033416C->v[0] = b;
    D_0033416C->v[1] = g;
    D_0033416C->v[2] = r;
    D_0033416C->v[3] = 1.0f;
    D_0033416C++;

    scale = 1 + scale * 0.004f;

    D_00334168->v[0] = p->org[0];
    D_00334168->v[1] = p->org[1];
    D_00334168->v[2] = p->org[2];
    D_00334168->v[3] = 1.0f;
    D_00334168++;
    D_00334168->v[0] = p->org[0] + up[0] * scale;
    D_00334168->v[1] = p->org[1] + up[1] * scale;
    D_00334168->v[2] = p->org[2] + up[2] * scale;
    D_00334168->v[3] = 1.0f;
    D_00334168++;
    D_00334168->v[0] = p->org[0] + right[0] * scale;
    D_00334168->v[1] = p->org[1] + right[1] * scale;
    D_00334168->v[2] = p->org[2] + right[2] * scale;
    D_00334168->v[3] = 1.0f;
    D_00334168++;
}

void func_002B3A10(int clientId)
{
    particle_t *p;
    float frametime, grav, time1, time2, time3, dvel;
    vec3_t up, right;
    int i;
    int first = 1;

    if (!D_003339AC)
        return;

    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    VectorScale(vup, 1.5f, up);
    VectorScale(vright, 1.5f, right);

    frametime = cl[clientId].time - cl[clientId].oldtime;
    time3 = frametime * 15;
    time2 = frametime * 10;
    time1 = frametime * 5;
    grav = frametime * sv_gravity.value * 0.05f;
    dvel = 4 * frametime;

    func_002B0D58(&D_00399660, clientId);

    D_0033102C = 1;
    D_00331044 = 5;

    for (p = D_00399660; p; p = p->next)
    {
        if (p->type != pt_blob)
        {
            if (first)
            {
                first = 0;
                GL_Bind(D_00399540);
                func_0023AF68();
            }
            func_002B37F8(p, up, right);
        }

        p->org[0] += p->vel[0] * frametime;
        p->org[1] += p->vel[1] * frametime;
        p->org[2] += p->vel[2] * frametime;

        switch (p->type)
        {
        case pt_static:
            break;

        case pt_fire:
            p->ramp += time1;
            if (p->ramp >= 6)
                p->die = -1;
            else
            {
                p->color = D_00333940[(int)p->ramp];
                p->packedColor = 0;
            }
            p->vel[2] += grav;
            break;

        case pt_explode:
            p->ramp += time2;
            if (p->ramp >= 8)
                p->die = -1;
            else
            {
                p->color = D_00333900[(int)p->ramp];
                p->packedColor = 0;
            }
            for (i = 0; i < 3; i++)
                p->vel[i] += p->vel[i] * dvel;
            p->vel[2] -= grav;
            break;

        case pt_explode2:
            p->ramp += time3;
            if (p->ramp >= 8)
                p->die = -1;
            else
            {
                p->color = D_00333920[(int)p->ramp];
                p->packedColor = 0;
            }
            for (i = 0; i < 3; i++)
                p->vel[i] -= p->vel[i] * frametime;
            p->vel[2] -= grav;
            break;

        case pt_blob:
        case pt_blob2:
            p->ramp += time2;
            if (p->ramp >= 9)
                p->ramp = 0;
            p->color = D_00333960[(int)p->ramp];
            p->packedColor = 0;
            p->vel[0] -= p->vel[0] * (frametime * 0.5f);
            p->vel[1] -= p->vel[1] * (frametime * 0.5f);
            p->vel[2] -= grav * 5;
            if (RandomLong(0, 3))
                p->type = pt_blob;
            else
                p->type = pt_blob2;
            break;

        case pt_grav:
            p->vel[2] -= grav * 20;
            break;

        case pt_slowgrav:
            p->vel[2] = grav;
            break;

        case pt_vox_grav:
            p->vel[2] -= grav * 8;
            break;

        case pt_vox_slowgrav:
            p->vel[2] -= grav * 4;
            break;
        }
    }

    func_002B3628();
    D_0033102C = 0;
    func_002B3EE8(clientId);
    func_002B7CA8(clientId);

    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
}

#ifdef NON_MATCHING
void func_002B3EE8(int clientId)
{
    vec3_t up, right;
    vec3_t start, end, screen, screenLast, tmp, normal;
    particle_t *p;
    float frametime, scale, dist, gravity;
    float atten, dist1, dist2, delta, frac, size;
    color24 *pColor;
    int draw;

    if (!D_00399668)
        return;

    D_00333988[4].r = D_00331B88.value * D_00331BD0.value * 255;
    D_00333988[4].g = D_00331BA0.value * D_00331BD0.value * 255;
    D_00333988[4].b = D_00331BB8.value * D_00331BD0.value * 255;

    func_002B0D58(&D_00399668, clientId);

    frametime = cl[clientId].time - cl[clientId].oldtime;
    VectorScale(vup, 1.5f, up);
    VectorScale(vright, 1.5f, right);

    if (!tri_SpriteTexture(D_003991E4, 0))
        return;

    scale = 1.0f - frametime * 0.9f;
    dist = DotProduct(r_origin, vpn);
    gravity = (cl[clientId].time - cl[clientId].oldtime) * sv_gravity.value;
    if (scale < 0)
        scale = 0;

    tri_RenderMode(kRenderTransAdd);
    func_002A0EE0(1);

    for (p = D_00399668; p; p = p->next)
    {
        pColor = &D_00333988[p->color];
        draw = 1;

        if (p->die - 0.1f > cl[clientId].time)
            atten = p->ramp * 0.1f;
        else
            atten = (p->die - cl[clientId].time) * p->ramp;

        end[0] = p->org[0] + p->vel[0] * atten;
        end[1] = p->org[1] + p->vel[1] * atten;
        end[2] = p->org[2] + p->vel[2] * atten;
        VectorCopy(p->org, start);

        if (ScreenTransform(start, screen) || ScreenTransform(end, screenLast))
        {
            dist1 = DotProduct(start, vpn) - dist;
            dist2 = DotProduct(end, vpn) - dist;

            if (dist1 <= 0 && dist2 <= 0)
            {
                draw = 0;
            }
            else
            {
                delta = dist2 - dist1;
                if (delta < 0.01f)
                {
                    draw = 0;
                }
                else
                {
                    frac = dist1 / delta;
                    if (dist1 <= 0)
                    {
                        start[0] = start[0] + frac * (end[0] - start[0]);
                        start[1] = start[1] + frac * (end[1] - start[1]);
                        start[2] = start[2] + frac * (end[2] - start[2]);
                    }
                    else
                    {
                        end[0] = start[0] + frac * (end[0] - start[0]);
                        end[1] = start[1] + frac * (end[1] - start[1]);
                        end[2] = start[2] + frac * (end[2] - start[2]);
                    }
                    ScreenTransform(start, screen);
                    ScreenTransform(end, screenLast);
                }
            }
        }

        if (draw)
        {
            ScreenTransform(start, screen);
            ScreenTransform(end, screenLast);

            VectorSubtract(screenLast, screen, tmp);
            tmp[2] = 0;
            size = D_00334178[p->type];
            VectorNormalize(tmp);

            VectorScale(vup, tmp[0] * size, normal);
            VectorMA(normal, -tmp[1] * size, vright, normal);

            glBegin(6);
            D_00399534++;
            D_00399598 += 4;
            tri_Color4ub(pColor->r, pColor->g, pColor->b, p->packedColor);

            tri_Brightness(0);
            glTexCoord2f(0, 0);
            func_0023C218(start[0] + normal[0], start[1] + normal[1], start[2] + normal[2]);

            tri_Brightness(1);
            glTexCoord2f(0, 1);
            func_0023C218(end[0] + normal[0], end[1] + normal[1], end[2] + normal[2]);

            tri_Brightness(1);
            glTexCoord2f(1, 1);
            func_0023C218(end[0] - normal[0], end[1] - normal[1], end[2] - normal[2]);

            tri_Brightness(0);
            glTexCoord2f(1, 0);
            func_0023C218(start[0] - normal[0], start[1] - normal[1], start[2] - normal[2]);

            glEnd();
        }

        p->org[0] += p->vel[0] * frametime;
        p->org[1] += p->vel[1] * frametime;
        p->org[2] += p->vel[2] * frametime;

        if (p->type == pt_grav)
        {
            p->vel[0] *= scale;
            p->vel[1] *= scale;
            p->vel[2] -= gravity;
            p->packedColor = (p->die - cl[clientId].time) * 2 * 255;
            if (p->packedColor > 255)
                p->packedColor = 255;
        }
        else if (p->type == pt_slowgrav)
        {
            p->vel[2] = gravity * 0.05f;
        }
    }

    func_002A0EE0(0);
    tri_RenderMode(kRenderNormal);
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B3EE8);
#endif

BEAM *func_002B4638(int clientId)
{
    BEAM *pBeam;

    pBeam = D_00414298[clientId];
    if (!pBeam)
        return NULL;
    D_00414298[clientId] = pBeam->next;
    pBeam->next = D_00414290[clientId];
    D_00414290[clientId] = pBeam;
    return pBeam;
}

void func_002B4770(BEAM *pbeam, float *start, float *end, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int clientId);

BEAM *func_002B4680(float *start, float *end, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int clientId)
{
    BEAM *pBeam;

    pBeam = func_002B4638(clientId);
    if (!pBeam)
        return NULL;
    pBeam->die = cl[clientId].time;
    if (modelIndex < 0)
        return NULL;
    func_002B4770(pBeam, start, end, modelIndex, life, width, amplitude, brightness, speed, clientId);
    return pBeam;
}

void func_002B4770(BEAM *pbeam, float *start, float *end, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int clientId)
{
    struct model_s *sprite = cl[clientId].model_precache[modelIndex];

    if (!sprite)
        return;

    pbeam->type = 0;
    pbeam->frame = 0;
    pbeam->frameRate = 0;
    pbeam->modelIndex = modelIndex;
    pbeam->frameCount = ModelFrameCount(sprite);
    VectorCopy(start, pbeam->source);
    VectorCopy(end, pbeam->target);
    VectorSubtract(end, start, pbeam->delta);
    pbeam->freq = cl[clientId].time * speed;
    pbeam->die = cl[clientId].time + life;
    pbeam->width = width;
    pbeam->brightness = brightness;
    pbeam->speed = speed;
    pbeam->amplitude = amplitude;

    if (amplitude >= 0.50f)
        pbeam->segments = Length(pbeam->delta) * 0.25f + 3;
    else
        pbeam->segments = Length(pbeam->delta) * 0.075f + 3;
    pbeam->flags = 0;
    pbeam->pFollowModel = NULL;
}

void func_002B4920(BEAM *pbeam, float r, float g, float b, float framerate, int startFrame)
{
    pbeam->frame = (float)startFrame;
    pbeam->frameRate = framerate;
    pbeam->r = r;
    pbeam->g = g;
    pbeam->b = b;
}

void func_002B4948(int clientId, float frametime)
{
    int i;
    BEAM beam;
    cl_entity_t *pbeam;
    int beamType;

    for (i = 0; i < D_00399658[clientId]; i++)
    {
        pbeam = D_0050EF88[clientId][i];
        beamType = pbeam->rendermode & 0x0F;

        /* movetype (0x1C) guarda o modelindex do feixe */
        func_002B4770(&beam, pbeam->origin, pbeam->angles, pbeam->movetype, 0, pbeam->scale,
                      pbeam->body * 0.01f, CL_FxBlend(pbeam) * (1.0f / 255.0f), pbeam->animtime, clientId);
        func_002B4920(&beam, pbeam->rendercolor.r * (1.0f / 255.0f), pbeam->rendercolor.g * (1.0f / 255.0f),
                      pbeam->rendercolor.b * (1.0f / 255.0f), 0, pbeam->frame);

        switch (beamType)
        {
        case 0:
            break;
        case 2:
            beam.type = 0;
            beam.flags = 3;
            beam.startEntity = pbeam->sequence;
            beam.endEntity = pbeam->skin;
            break;
        case 1:
            beam.type = 0;
            beam.flags = 2;
            beam.startEntity = 0;
            beam.endEntity = pbeam->skin;
            break;
        }

        if (pbeam->rendermode & 0x10)
            beam.flags |= 0x10;
        if (pbeam->rendermode & 0x20)
            beam.flags |= 0x20;
        if (pbeam->rendermode & 0x40)
            beam.flags |= 0x40;
        if (pbeam->rendermode & 0x80)
            beam.flags |= 0x80;

        beam.pFollowModel = NULL;
        func_002B75F0(&beam, frametime, clientId);
    }
}

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

/* jump tables do gcc são alinhadas em 16 */
__asm__(".section .rodata\n.align 4\n.section .text");
INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B75F0);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B7CA8);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B7E98);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B82F0);

INCLUDE_ASM("asm/nonmatchings/engine/r_part2", func_002B8C18);
