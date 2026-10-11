#include "common.h"
#include "engine/mathlib.h"
#include "engine/cvar.h"
#include "engine/cmd.h"
#include "engine/client.h"
#include "engine/model.h"

extern cvar_t cl_bobcycle, cl_bob, cl_bobup, v_centerspeed;
extern float host_frametime;
extern int pm_onground[];
extern float v_bobtime[2];
extern float v_bob[2];
int atoi(const char *);
float sinf(float);
#define fabs(x) ((x) < 0 ? -(x) : (x))
float sqrtf(float);
int func_002AD110(int a0, int a1, int a2, int a3, int a4, int a5);
double atof(const char *);
float powf(float, float);
#define max(a, b) ((a) > (b) ? (a) : (b))
void func_002B05F8(void);

#include "engine/render.h"

typedef struct
{
    byte *buffer;               /* 0x00 */
    byte pad4[0x18 - 0x4];
    unsigned int rowbytes;      /* 0x18 */
    byte pad1C[0x24 - 0x1C];
    float aspect;               /* 0x24 */
} viddef_t;

extern refdef_t r_refdef;
extern model_t *sv_worldmodel;
extern cvar_t cl_forwardspeed, v_centermove;
extern cvar_t v_gamma, v_brightness, v_lightgamma, v_texgamma;
extern cvar_t v_iyaw_cycle, v_iroll_cycle, v_ipitch_cycle, v_iyaw_level, v_iroll_level, v_ipitch_level;
extern qboolean noclip_anglehack;
extern float v_idlescale;
extern float v_dmg_roll, v_dmg_pitch, v_dmg_time;
extern cvar_t cl_rollangle, cl_rollspeed, v_kicktime;
extern cl_entity_t *cl_entities[2];
float PM_CalcRoll(vec3_t angles, vec3_t velocity, float rollangle, float rollspeed);
void V_AddIdle(int clientId);
#define CONTENTS_WATER -3
#define VectorAdd(a, b, c) {(c)[0] = (a)[0] + (b)[0]; (c)[1] = (a)[1] + (b)[1]; (c)[2] = (a)[2] + (b)[2];}

extern refdef_t oldrefdef[2];
extern viddef_t r_viddef;
extern float oldz[2];
extern qboolean cam_thirdperson;
extern vec3_t cam_ofs;
extern cvar_t chase_active, scr_viewsize;
extern vec3_t pm_right;
extern qboolean con_forcedup;
int PM_WaterEntity(vec3_t p, int clientId);
int PM_PointContents(vec3_t p, int clientId);
void Chase_Update(void);
void R_PushDlights(void);
void R_RenderView(int clientId);
void V_CalcIntermissionRefdef(int clientId);
void V_CalcShake(int clientId);
void V_ApplyShake(float *origin, float *angles, float factor, int clientId);
void V_CalcViewRoll(int clientId);
void V_CalcGunAngle(int clientId);
void V_DriftPitch(int clientId);
float V_CalcBob(int clientId);
#define min(a, b) ((a) < (b) ? (a) : (b))

typedef struct
{
    float time;                 /* 0x00 */
    float duration;             /* 0x04 */
    float amplitude;            /* 0x08 */
    float frequency;            /* 0x0C */
    float nextShake;            /* 0x10 */
    vec3_t offset;              /* 0x14 */
    float angle;                /* 0x20 */
    vec3_t appliedOffset;       /* 0x24 */
    float appliedAngle;         /* 0x30 */
} screen_shake_t; /* 0x34 */

typedef struct
{
    unsigned short amplitude;
    unsigned short duration;
    unsigned short frequency;
} ScreenShake;

typedef struct
{
    unsigned short duration;
    unsigned short holdTime;
    short fadeFlags;
    byte r, g, b, a;
} ScreenFade;

extern screen_shake_t gVShake[2];
extern void (*D_00399448)(int);
extern int D_0039943C;
extern cvar_t lcd_x, lcd_yaw, v_dark, crosshair, scr_ofsx, scr_ofsy, scr_ofsz, cl_waterdist;
extern cvar_t v_kickroll, v_kickpitch, v_lambert, v_direct;
float RandomFloat(float flLow, float flHigh);
void func_0021C288(int clientId, int a1, unsigned char a2, float a3);
extern float D_00399440, D_00399444, D_0039944C, D_00399460;
extern float cshift_color[4];
extern byte texgammatable[256];
extern byte ramps[3][256];
extern int lightgammatable[1024];
extern int lineargammatable[1024];
extern int screengammatable[1024];
extern float oldgammavalue, oldlightgamma, oldtexgamma, oldbrightness;
extern int D_0053D1D4;
extern char D_00390518[], D_00390528[];
extern char D_00390340[], D_00390358[], D_00390368[], D_00390378[]; /* "0", "scr_ofsx", "scr_ofsy", "scr_ofsz" */

void V_StartPitchDrift(int clientId);
void BuildGammaTable(float g);


int func_0028BF00(int a0, int a1, int a2, int a3, int a4, int a5)
{
    if (!a3 && !a4 && !a5)
        return 0;
    return func_002AD110(a0, a1, a2, a3, a4, a5);
}

#ifdef NON_MATCHING
float V_CalcBob(int clientId)
{
    float cycle;

    if (cl[clientId].paused)
        return 0;

    if (pm_onground[clientId] == -1 || cl[clientId].time == cl[clientId].oldtime)
        return v_bob[clientId];

    v_bobtime[clientId] += host_frametime;
    cycle = v_bobtime[clientId] - (int)(v_bobtime[clientId] / cl_bobcycle.value) * cl_bobcycle.value;
    cycle /= cl_bobcycle.value;

    if (cycle < cl_bobup.value)
        cycle = M_PI * cycle / cl_bobup.value;
    else
        cycle = M_PI + M_PI * (cycle - cl_bobup.value) / (1.0f - cl_bobup.value);

    v_bob[clientId] = sqrtf(cl[clientId].simvel[0] * cl[clientId].simvel[0] + cl[clientId].simvel[1] * cl[clientId].simvel[1]) * cl_bob.value;
    v_bob[clientId] = v_bob[clientId] * 0.3f + v_bob[clientId] * 0.7f * sinf(cycle);

    if (v_bob[clientId] > 4)
        v_bob[clientId] = 4;
    else if (v_bob[clientId] < -7)
        v_bob[clientId] = -7;

    return v_bob[clientId];
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/view", V_CalcBob);
#endif

void V_StartPitchDrift(int clientId)
{
    if (cl[clientId].laststop == cl[clientId].time)
        return;

    if (cl[clientId].nodrift || !cl[clientId].pitchvel)
    {
        cl[clientId].pitchvel = v_centerspeed.value;
        cl[clientId].nodrift = false;
        cl[clientId].driftmove = 0;
    }
}

void V_StartPitchDrift_f(void)
{
    int clientId = atoi(Cmd_Argv(1));

    if (cl[clientId].laststop == cl[clientId].time)
        return;

    if (cl[clientId].nodrift || !cl[clientId].pitchvel)
    {
        cl[clientId].pitchvel = v_centerspeed.value;
        cl[clientId].nodrift = false;
        cl[clientId].driftmove = 0;
    }
}

void V_StopPitchDrift(int clientId)
{
    cl[clientId].laststop = cl[clientId].time;
    cl[clientId].nodrift = true;
    cl[clientId].pitchvel = 0;
}

void V_DriftPitch(int clientId)
{
    float delta, move;

    if (noclip_anglehack || !cl[clientId].onground || cl[clientId].paused)
    {
        cl[clientId].driftmove = 0;
        cl[clientId].pitchvel = 0;
        return;
    }

    if (cl[clientId].nodrift)
    {
        if (fabs(cl[clientId].forwardmove) < cl_forwardspeed.value)
            cl[clientId].driftmove = 0;
        else
            cl[clientId].driftmove += host_frametime;

        if (cl[clientId].driftmove > v_centermove.value)
            V_StartPitchDrift(clientId);
        return;
    }

    delta = cl[clientId].idealpitch - cl[clientId].viewangles[PITCH];

    if (!delta)
    {
        cl[clientId].pitchvel = 0;
        return;
    }

    move = host_frametime * cl[clientId].pitchvel;
    cl[clientId].pitchvel += host_frametime * v_centerspeed.value;

    if (delta > 0)
    {
        if (move > delta)
        {
            cl[clientId].pitchvel = 0;
            move = delta;
        }
        cl[clientId].viewangles[PITCH] += move;
    }
    else if (delta < 0)
    {
        if (move > -delta)
        {
            cl[clientId].pitchvel = 0;
            move = -delta;
        }
        cl[clientId].viewangles[PITCH] -= move;
    }
}

void BuildGammaTable(float g)
{
    int i, inf;
    float g1, g3;

    g = 1.0f / g;
    g1 = v_texgamma.value * g;

    if (v_brightness.value <= 0.0f)
        g3 = 0.125f;
    else if (v_brightness.value > 1.0f)
        g3 = 0.05f;
    else
        g3 = 0.125f - (v_brightness.value * v_brightness.value) * 0.075f;

    for (i = 0; i < 256; i++)
    {
        inf = 255 * powf(i / 255.0f, g1);
        if (inf < 0)
            inf = 0;
        if (inf > 255)
            inf = 255;
        texgammatable[i] = inf;
    }

    for (i = 0; i < 1024; i++)
    {
        float f;

        f = powf(i / 1023.0f, v_lightgamma.value);

        if (v_brightness.value > 1.0f)
            f = f * v_brightness.value;

        if (f <= g3)
            f = (f / g3) * 0.125f;
        else
            f = 0.125f + ((f - g3) / (1.0f - g3)) * 0.875f;

        inf = 1023 * powf(f, g);
        if (inf < 0)
            inf = 0;
        if (inf > 1023)
            inf = 1023;
        lightgammatable[i] = inf;
    }

    for (i = 0; i < 1024; i++)
    {
        lineargammatable[i] = 1023 * powf(i / 1023.0f, v_gamma.value);
        screengammatable[i] = 1023 * powf(i / 1023.0f, 1.0f / v_gamma.value);
    }
}

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390340);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390348);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390350);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390358);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390368);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390378);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390388);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390390);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_003903A0);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_003903A8);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_003903B8);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_003903C0);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_003903D0);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_003903D8);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_003903E0);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_003903E8);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_003903F8);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390400);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390410);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390420);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390428);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390438);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390448);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390450);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390460);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390470);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390478);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390488);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390490);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_003904A0);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_003904A8);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_003904B8);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_003904C8);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_003904D0);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_003904E0);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_003904E8);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_003904F8);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390500);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390510);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390518);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390520);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390528);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390538);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390540);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390550);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390560);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390568);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390578);

INCLUDE_RODATA("asm/nonmatchings/engine/view", D_00390580);

void V_Brightness_f(void)
{
    int i;

    if (Cmd_Argc() == 2)
    {
        float f = atof(Cmd_Argv(1));
        Cvar_SetValue(D_00390528, f);
        BuildGammaTable(v_gamma.value);
        if (sv_worldmodel)
        {
            int n = sv_worldmodel->numsurfaces;
            msurface_t *surf = sv_worldmodel->surfaces;

            for (i = 0; i < n; i++, surf++)
                surf->cached_dlight = -1;
        }
    }

    Con_Printf("brightness is now %f\n", v_brightness.value);
}

void V_Gamma_f(void)
{
    int i;

    if (Cmd_Argc() == 2)
    {
        float f = atof(Cmd_Argv(1));
        Cvar_SetValue(D_00390518, f);
        BuildGammaTable(v_gamma.value);
        if (sv_worldmodel)
        {
            int n = sv_worldmodel->numsurfaces;
            msurface_t *surf = sv_worldmodel->surfaces;

            for (i = 0; i < n; i++, surf++)
                surf->cached_dlight = -1;
        }
    }

    Con_Printf("gamma is now %f\n", v_gamma.value);
}

qboolean V_CheckGamma(void)
{
    if (v_gamma.value == oldgammavalue && v_lightgamma.value == oldlightgamma
        && v_texgamma.value == oldtexgamma && v_brightness.value == oldbrightness)
        return false;

    BuildGammaTable(v_gamma.value);
    oldgammavalue = v_gamma.value;
    oldlightgamma = v_lightgamma.value;
    oldtexgamma = v_texgamma.value;
    oldbrightness = v_brightness.value;
    func_002B05F8();
    D_0053D1D4 = 1;
    return true;
}

void func_0028C9F0(void)
{
    D_00399444 = max(D_00399444 - host_frametime, 0);
    D_00399460 = max(D_00399460 - host_frametime, 0);
    D_00399440 = max(D_00399440 - host_frametime, 0);
    D_0039944C = max(D_0039944C - host_frametime, 0);
}

void func_0028CA58(void)
{
    cshift_color[3] = cshift_color[2] = cshift_color[1] = cshift_color[0] = 0;
}

#ifdef NON_MATCHING
void V_UpdatePalette(void)
{
    int i, ir, ig, ib;
    float a, r, g, b, frac;

    func_0028C9F0();

    if (!V_CheckGamma())
        return;

    func_0028CA58();

    a = cshift_color[3] * 255.0f;
    r = cshift_color[0] * a;
    g = cshift_color[1] * a;
    b = cshift_color[2] * a;
    frac = 1.0f - cshift_color[3];

    for (i = 0; i < 256; i++)
    {
        ir = i * frac + r;
        ig = i * frac + g;
        ib = i * frac + b;
        if (ib > 255)
            ib = 255;
        if (ir > 255)
            ir = 255;
        if (ig > 255)
            ig = 255;
        ramps[0][i] = texgammatable[ir];
        ramps[1][i] = texgammatable[ig];
        ramps[2][i] = texgammatable[ib];
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/view", V_UpdatePalette);
#endif

float angledelta(float a)
{
    a = anglemod(a);
    if (a > 180)
        a -= 360;
    return a;
}

void V_CalcGunAngle(int clientId)
{
    float s;

    cl[clientId].viewent.angles[1] = r_refdef.viewangles[1];
    cl[clientId].viewent.angles[0] = -r_refdef.viewangles[0];
    cl[clientId].viewent.angles[2] -= v_idlescale * sinf(cl[clientId].time * v_iroll_cycle.value) * v_iroll_level.value;
    s = v_idlescale * sinf(cl[clientId].time * v_ipitch_cycle.value) * 0.5f;
    cl[clientId].viewent.angles[0] -= s * v_ipitch_level.value;
    cl[clientId].viewent.angles[1] -= v_idlescale * sinf(cl[clientId].time * v_iyaw_cycle.value) * v_iyaw_level.value;
}

void V_BoundOffsets(int clientId)
{
    if (r_refdef.vieworg[0] < cl[clientId].simorg[0] - 14)
        r_refdef.vieworg[0] = cl[clientId].simorg[0] - 14;
    else if (r_refdef.vieworg[0] > cl[clientId].simorg[0] + 14)
        r_refdef.vieworg[0] = cl[clientId].simorg[0] + 14;

    if (r_refdef.vieworg[1] < cl[clientId].simorg[1] - 14)
        r_refdef.vieworg[1] = cl[clientId].simorg[1] - 14;
    else if (r_refdef.vieworg[1] > cl[clientId].simorg[1] + 14)
        r_refdef.vieworg[1] = cl[clientId].simorg[1] + 14;

    if (r_refdef.vieworg[2] < cl[clientId].simorg[2] - 22)
        r_refdef.vieworg[2] = cl[clientId].simorg[2] - 22;
    else if (r_refdef.vieworg[2] > cl[clientId].simorg[2] + 30)
        r_refdef.vieworg[2] = cl[clientId].simorg[2] + 30;
}

void V_AddIdle(int clientId)
{
    r_refdef.viewangles[ROLL] += v_idlescale * sinf(cl[clientId].time * v_iroll_cycle.value) * v_iroll_level.value;
    r_refdef.viewangles[PITCH] += v_idlescale * sinf(cl[clientId].time * v_ipitch_cycle.value) * v_ipitch_level.value;
    r_refdef.viewangles[YAW] += v_idlescale * sinf(cl[clientId].time * v_iyaw_cycle.value) * v_iyaw_level.value;
}

void V_CalcViewRoll(int clientId)
{
    float side;
    cl_entity_t *viewentity;

    viewentity = &cl_entities[clientId][cl[clientId].viewentity];
    side = PM_CalcRoll(viewentity->angles, cl[clientId].simvel, cl_rollangle.value, cl_rollspeed.value);

    r_refdef.viewangles[ROLL] += side;

    if (v_dmg_time > 0)
    {
        r_refdef.viewangles[ROLL] += v_dmg_time / v_kicktime.value * v_dmg_roll;
        r_refdef.viewangles[PITCH] += v_dmg_time / v_kicktime.value * v_dmg_pitch;
        v_dmg_time -= host_frametime;
    }

    if (cl[clientId].stats[STAT_HEALTH] <= 0 && cl[clientId].viewheight != 0)
        r_refdef.viewangles[ROLL] = 80;
}

#ifdef NON_MATCHING
void V_CalcIntermissionRefdef(int clientId)
{
    float old;

    VectorCopy(cl[clientId].simorg, r_refdef.vieworg);
    VectorCopy(cl[clientId].simangles, r_refdef.viewangles);
    cl[clientId].viewent.model = NULL;

    old = v_idlescale;
    v_idlescale = 1;
    V_AddIdle(clientId);
    v_idlescale = old;
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/view", V_CalcIntermissionRefdef);
#endif

#ifdef NON_MATCHING
void V_CalcRefdef(int clientId)
{
    cl_entity_t *ent, *view;
    int i;
    vec3_t forward, right, up;
    vec3_t angles;
    vec3_t camAngles, camForward, camRight, camUp;
    float bob;
    float waterOffset;

    V_DriftPitch(clientId);

    view = &cl[clientId].viewent;
    waterOffset = 0;

    bob = V_CalcBob(clientId);

    VectorCopy(cl[clientId].simorg, r_refdef.vieworg);
    r_refdef.vieworg[2] += cl[clientId].viewheight + bob;
    VectorCopy(cl[clientId].viewangles, r_refdef.viewangles);

    V_CalcShake(clientId);
    V_ApplyShake(r_refdef.vieworg, r_refdef.viewangles, 1.0f, clientId);

    r_refdef.vieworg[0] += 1.0f / 32;
    r_refdef.vieworg[1] += 1.0f / 32;
    r_refdef.vieworg[2] += 1.0f / 32;

    if (cl[clientId].waterlevel >= 2)
    {
        int contents, waterDist, waterEntity;
        vec3_t point;

        waterDist = cl_waterdist.value;
        waterEntity = PM_WaterEntity(cl[clientId].simorg, clientId);

        if (waterEntity >= 0 && waterEntity < cl[clientId].max_edicts)
        {
            cl_entity_t *pwater = &cl_entities[clientId][waterEntity];

            if (pwater->model)
                waterDist += pwater->scale * 16;
        }

        VectorCopy(r_refdef.vieworg, point);

        if (cl[clientId].waterlevel == 2)
        {
            point[2] -= waterDist;
            for (i = 0; i < waterDist; i++)
            {
                contents = PM_PointContents(point, clientId);
                if (contents > CONTENTS_WATER)
                    break;
                point[2] += 1;
            }
            waterOffset = (point[2] + waterDist) - r_refdef.vieworg[2];
        }
        else
        {
            point[2] += waterDist;
            for (i = 0; i < waterDist; i++)
            {
                contents = PM_PointContents(point, clientId);
                if (contents <= CONTENTS_WATER)
                    break;
                point[2] -= 1;
            }
            waterOffset = (point[2] - waterDist) - r_refdef.vieworg[2];
        }
    }

    r_refdef.vieworg[2] += waterOffset;

    V_CalcViewRoll(clientId);
    V_AddIdle(clientId);

    angles[0] = cl[clientId].viewangles[0];
    angles[1] = cl[clientId].viewangles[1];
    angles[2] = cl[clientId].simangles[2];
    AngleVectors(angles, forward, right, up);

    for (i = 0; i < 3; i++)
        r_refdef.vieworg[i] += scr_ofsx.value * forward[i] + scr_ofsy.value * right[i] + scr_ofsz.value * up[i];

    if (cam_thirdperson)
    {
        camAngles[0] = cam_ofs[0];
        camAngles[1] = cam_ofs[1];
        camAngles[2] = 0;
        AngleVectors(camAngles, camForward, camRight, camUp);

        for (i = 0; i < 3; i++)
            r_refdef.vieworg[i] += -cam_ofs[2] * camForward[i];
    }

    VectorCopy(cl[clientId].viewangles, view->angles);
    V_CalcGunAngle(clientId);

    VectorCopy(cl[clientId].simorg, view->origin);
    view->origin[2] += cl[clientId].viewheight + waterOffset;

    V_ApplyShake(view->origin, view->angles, 0.9f, clientId);

    for (i = 0; i < 3; i++)
        view->origin[i] += forward[i] * bob * 0.4f;
    view->origin[2] += bob;

    view->angles[YAW] -= bob * 0.5f;
    view->angles[ROLL] -= bob;
    view->angles[PITCH] -= bob * 0.3f;

    view->origin[2] -= 1;

    if (scr_viewsize.value == 110)
        view->origin[2] += 1;
    else if (scr_viewsize.value == 100)
        view->origin[2] += 2;
    else if (scr_viewsize.value == 90)
        view->origin[2] += 1;
    else if (scr_viewsize.value == 80)
        view->origin[2] += 0.5f;

    view->model = cl[clientId].model_precache[cl[clientId].stats[STAT_WEAPON]];
    view->frame = 0;
    view->unk_30 = 0;
    view->index = cl[clientId].playernum + 1;

    VectorAdd(r_refdef.viewangles, cl[clientId].punchangle, r_refdef.viewangles);

    if (cl[clientId].onground && cl[clientId].simorg[2] - oldz[clientId] > 0)
    {
        float steptime;

        steptime = cl[clientId].time - cl[clientId].oldtime;
        if (steptime < 0)
            steptime = 0;

        oldz[clientId] += steptime * 80;
        if (oldz[clientId] > cl[clientId].simorg[2])
            oldz[clientId] = cl[clientId].simorg[2];
        if (cl[clientId].simorg[2] - oldz[clientId] > 12)
            oldz[clientId] = cl[clientId].simorg[2] - 12;

        r_refdef.vieworg[2] += oldz[clientId] - cl[clientId].simorg[2];
        view->origin[2] += oldz[clientId] - cl[clientId].simorg[2];
    }
    else
    {
        oldz[clientId] = cl[clientId].simorg[2];
    }

    if (cam_thirdperson)
        VectorCopy(camAngles, r_refdef.viewangles);

    if (chase_active.value)
        Chase_Update();

    if (cl[clientId].viewentity > cl[clientId].maxclients)
    {
        ent = &cl_entities[clientId][cl[clientId].viewentity];
        VectorCopy(ent->origin, r_refdef.vieworg);
        VectorCopy(ent->angles, r_refdef.viewangles);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/view", V_CalcRefdef);
#endif

void V_RenderView(int clientId)
{
    int i;

    if (con_forcedup)
        return;

    if (cls[clientId].state != ca_active)
        return;

    if (cl[clientId].maxclients > 1)
    {
        Cvar_Set(D_00390358, D_00390340);
        Cvar_Set(D_00390368, D_00390340);
        Cvar_Set(D_00390378, D_00390340);
    }

    if (cl[clientId].intermission)
    {
        V_CalcIntermissionRefdef(clientId);
    }
    else if (!cl[clientId].unk_37C)
    {
        V_CalcRefdef(clientId);
        oldrefdef[clientId] = r_refdef;
    }
    else
    {
        r_refdef = oldrefdef[clientId];
    }

    R_PushDlights();

    if (lcd_x.value)
    {
        r_viddef.rowbytes <<= 1;
        r_viddef.aspect *= 0.5f;

        r_refdef.viewangles[YAW] -= lcd_yaw.value;
        for (i = 0; i < 3; i++)
            r_refdef.vieworg[i] -= pm_right[i] * lcd_x.value;
        R_RenderView(clientId);

        r_viddef.buffer += r_viddef.rowbytes >> 1;

        R_PushDlights();

        r_refdef.viewangles[YAW] += lcd_yaw.value * 2;
        for (i = 0; i < 3; i++)
            r_refdef.vieworg[i] += 2 * pm_right[i] * lcd_x.value;
        R_RenderView(clientId);

        r_viddef.buffer -= r_viddef.rowbytes >> 1;

        r_refdef.vrect.height <<= 1;

        r_viddef.rowbytes >>= 1;
        r_viddef.aspect *= 2;
    }
    else
    {
        R_RenderView(clientId);
    }
}

void V_CalcShake(int clientId)
{
    int i, j;
    float fraction, freq;
    float frametime;
    float sum;

    if (cl[clientId].time > gVShake[clientId].time || gVShake[clientId].duration <= 0
        || gVShake[clientId].amplitude <= 0 || gVShake[clientId].frequency <= 0)
    {
        if (gVShake[clientId].time != 0)
        {
            gVShake[clientId].time = 0;
            VectorCopy(vec3_origin, gVShake[clientId].appliedOffset);
            gVShake[clientId].appliedAngle = 0;
        }
        return;
    }

    frametime = cl[clientId].time - cl[clientId].oldtime;

    if (gVShake[clientId].nextShake < cl[clientId].time)
    {
        gVShake[clientId].nextShake = cl[clientId].time + gVShake[clientId].duration / gVShake[clientId].frequency;

        for (i = 0; i < 3; i++)
            gVShake[clientId].offset[i] = RandomFloat(-gVShake[clientId].amplitude, gVShake[clientId].amplitude);

        gVShake[clientId].angle = RandomFloat(-gVShake[clientId].amplitude * 0.25f, gVShake[clientId].amplitude * 0.25f);

        sum = 0;
        for (j = 0; j < 3; j++)
            sum += fabs(gVShake[clientId].offset[j]);

        func_0021C288(clientId, 0, (int)(sum / (gVShake[clientId].amplitude * 3) * 128), 0.05f);
    }

    fraction = (cl[clientId].time - gVShake[clientId].time) / gVShake[clientId].duration;

    if (fraction)
        freq = gVShake[clientId].frequency / fraction * gVShake[clientId].frequency;
    else
        freq = 0;

    fraction *= fraction;
    fraction = fraction * sinf(cl[clientId].time * freq);

    for (i = 0; i < 3; i++)
        gVShake[clientId].appliedOffset[i] = gVShake[clientId].offset[i] * fraction;

    gVShake[clientId].appliedAngle = gVShake[clientId].angle * fraction;

    gVShake[clientId].amplitude -= gVShake[clientId].amplitude * (frametime / (gVShake[clientId].duration * gVShake[clientId].frequency));
}

void V_ApplyShake(float *origin, float *angles, float factor, int clientId)
{
    if (origin)
        VectorMA(origin, factor, gVShake[clientId].appliedOffset, origin);

    if (angles)
        angles[ROLL] += gVShake[clientId].appliedAngle * factor;
}

int V_ScreenShake(int clientId, const char *pszName, int iSize, void *pbuf)
{
    ScreenShake *pShake = (ScreenShake *)pbuf;
    float amplitude;

    gVShake[clientId].duration = (float)pShake->duration * (1.0f / (float)(1 << 12));
    gVShake[clientId].time = cl[clientId].time + gVShake[clientId].duration;

    amplitude = (float)pShake->amplitude * (1.0f / (float)(1 << 12));
    if (gVShake[clientId].amplitude < amplitude)
        gVShake[clientId].amplitude = amplitude;

    gVShake[clientId].frequency = (float)pShake->frequency * (1.0f / (float)(1 << 8));
    gVShake[clientId].nextShake = 0;

    return 1;
}

int V_ScreenFade(int clientId, const char *pszName, int iSize, void *pbuf)
{
    ScreenFade *pFade = (ScreenFade *)pbuf;

    cl[clientId].sf.fadeEnd = (float)pFade->duration * (1.0f / (float)(1 << 12));
    cl[clientId].sf.fadeReset = (float)pFade->holdTime * (1.0f / (float)(1 << 12));
    cl[clientId].sf.fader = pFade->r;
    cl[clientId].sf.fadeg = pFade->g;
    cl[clientId].sf.fadeb = pFade->b;
    cl[clientId].sf.fadealpha = pFade->a;
    cl[clientId].sf.fadeFlags = pFade->fadeFlags;
    cl[clientId].sf.fadeSpeed = 0;

    if (pFade->duration > 0)
    {
        if (pFade->fadeFlags & FFADE_OUT)
        {
            if (cl[clientId].sf.fadeEnd)
                cl[clientId].sf.fadeSpeed = -(float)cl[clientId].sf.fadealpha / cl[clientId].sf.fadeEnd;

            cl[clientId].sf.fadeEnd += cl[clientId].time;
            cl[clientId].sf.fadeReset += cl[clientId].sf.fadeEnd;
        }
        else
        {
            if (cl[clientId].sf.fadeEnd)
                cl[clientId].sf.fadeSpeed = (float)cl[clientId].sf.fadealpha / cl[clientId].sf.fadeEnd;

            cl[clientId].sf.fadeReset += cl[clientId].time;
            cl[clientId].sf.fadeEnd += cl[clientId].sf.fadeReset;
        }
    }

    return 1;
}

int V_FadeAlpha(int clientId)
{
    int iFadeAlpha;

    if (cl[clientId].sf.fadeFlags & FFADE_STAYOUT)
        cl[clientId].sf.fadeEnd = cl[clientId].time + 0.1f;

    if (cl[clientId].time > cl[clientId].sf.fadeReset && cl[clientId].time > cl[clientId].sf.fadeEnd)
    {
        if (D_00399448)
        {
            D_00399448(D_0039943C);
            D_00399448 = NULL;
            D_0039943C = 0;
        }
        return 0;
    }

    if (cl[clientId].sf.fadeFlags & FFADE_OUT)
        iFadeAlpha = cl[clientId].sf.fadealpha + cl[clientId].sf.fadeSpeed * (cl[clientId].sf.fadeEnd - cl[clientId].time);
    else
        iFadeAlpha = cl[clientId].sf.fadeSpeed * (cl[clientId].sf.fadeEnd - cl[clientId].time);

    if (iFadeAlpha > cl[clientId].sf.fadealpha)
        iFadeAlpha = cl[clientId].sf.fadealpha;
    else if (iFadeAlpha < 0)
        iFadeAlpha = 0;

    return iFadeAlpha;
}

void V_Init(void)
{
    Cmd_AddCommand("centerview", V_StartPitchDrift_f);
    Cmd_AddCommand("brightness", V_Brightness_f);
    Cmd_AddCommand("gamma", V_Gamma_f);

    Cvar_RegisterVariable(&lcd_x);
    Cvar_RegisterVariable(&lcd_yaw);
    Cvar_RegisterVariable(&v_centermove);
    Cvar_RegisterVariable(&v_centerspeed);
    Cvar_RegisterVariable(&v_dark);
    Cvar_RegisterVariable(&crosshair);
    Cvar_RegisterVariable(&scr_ofsx);
    Cvar_RegisterVariable(&scr_ofsy);
    Cvar_RegisterVariable(&scr_ofsz);
    Cvar_RegisterVariable(&cl_rollspeed);
    Cvar_RegisterVariable(&cl_rollangle);
    Cvar_RegisterVariable(&cl_bob);
    Cvar_RegisterVariable(&cl_bobcycle);
    Cvar_RegisterVariable(&cl_bobup);
    Cvar_RegisterVariable(&cl_waterdist);
    Cvar_RegisterVariable(&v_kicktime);
    Cvar_RegisterVariable(&v_kickroll);
    Cvar_RegisterVariable(&v_kickpitch);

    BuildGammaTable(2.5f);

    Cvar_RegisterVariable(&v_gamma);
    Cvar_RegisterVariable(&v_lightgamma);
    Cvar_RegisterVariable(&v_texgamma);
    Cvar_RegisterVariable(&v_brightness);
    Cvar_RegisterVariable(&v_lambert);
    Cvar_RegisterVariable(&v_direct);

    D_00399448 = NULL;
    D_0039943C = 0;
}

/* zera o tremor e, com v_dark, começa a fase com a tela escurecendo */
void V_ResetScreenEffects(int clientId)
{
    memset(&gVShake[clientId], 0, sizeof(screen_shake_t));

    if (v_dark.value)
    {
        cl[clientId].sf.fadeEnd = 5;
        cl[clientId].sf.fadeReset = 5;
        cl[clientId].sf.fadealpha = 255;
        cl[clientId].sf.fadeb = 0;
        cl[clientId].sf.fadeg = 0;
        cl[clientId].sf.fader = 0;
        cl[clientId].sf.fadeFlags = 0;

        if (cl[clientId].sf.fadeEnd)
            cl[clientId].sf.fadeSpeed = (float)cl[clientId].sf.fadealpha / cl[clientId].sf.fadeEnd;

        cl[clientId].sf.fadeReset += cl[clientId].time;
        cl[clientId].sf.fadeEnd += cl[clientId].sf.fadeReset;

        if (cl_numlocal < 2 || clientId == 1)
            v_dark.value = 0;
    }
    else
    {
        cl[clientId].sf.fadeSpeed = 0;
        cl[clientId].sf.fadeEnd = 0;
        cl[clientId].sf.fadeReset = 0;
        cl[clientId].sf.fadealpha = 0;
        cl[clientId].sf.fadeb = 0;
        cl[clientId].sf.fadeg = 0;
        cl[clientId].sf.fader = 0;
        cl[clientId].sf.fadeFlags = 0;
    }
}
