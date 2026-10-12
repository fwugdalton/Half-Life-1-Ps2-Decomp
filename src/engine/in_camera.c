#include "common.h"
#include "engine/cvar.h"
#include "engine/cmd.h"
#include "engine/client.h"
#include "engine/mathlib.h"
#include "engine/server.h"
#include "engine/render.h"
#include "engine/cl_entity.h"

typedef struct kbutton_s
{
    int down[2];
    int state;
} kbutton_t;

void func_0024C500(kbutton_t *b); /* KeyDown */
void func_0024C5B0(kbutton_t *b); /* KeyUp */

extern cvar_t cam_command, cam_snapto, cam_idealyaw, cam_idealpitch, cam_idealdist, cam_contain;
extern cvar_t c_maxpitch, c_minpitch, c_maxyaw, c_minyaw, c_maxdistance, c_mindistance;
extern kbutton_t cam_pitchup, cam_pitchdown, cam_yawleft, cam_yawright, cam_in, cam_out;
extern int iMouseInUse;
extern qboolean cam_thirdperson;
extern vec3_t cam_ofs;
extern int D_00399100;          /* cam_old_mouse_x */
extern int D_00399104;          /* cam_distancemove */
extern int D_00399108;          /* cam_old_mouse_y */
extern int D_00399110;          /* cam_mousemove */
extern int D_00399118[2];       /* posição do mouse */
extern float D_00331844;        /* sensitivity.value */
extern char D_00388E58[];       /* "cam_command" */
int abs(int);
void *memset(void *, int, unsigned int);

typedef struct
{
    vec3_t boxmins;             /* 0x00 */
    vec3_t boxmaxs;             /* 0x0C */
    const float *mins;          /* 0x18 */
    const float *maxs;          /* 0x1C */
    vec3_t mins2;               /* 0x20 */
    vec3_t maxs2;               /* 0x2C */
    const float *start;         /* 0x38 */
    const float *end;           /* 0x3C */
    trace_t trace;              /* 0x40 */
    short type;                 /* 0x78 */
    short ignoretrans;          /* 0x7A */
    edict_t *passedict;         /* 0x7C */
    qboolean monsterClipBrush;  /* 0x80 */
} moveclip_t;

trace_t SV_ClipMoveToEntity(edict_t *ent, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end);
float func_0024DA78(kbutton_t *key); /* CL_KeyState */
extern cl_entity_t *cl_entities[2];
extern refdef_t r_refdef;
extern int D_0039970C;          /* centro da janela em x */
extern int D_003996D8;          /* centro da janela em y */
float func_002456F8(float cur, float goal, float maxspeed); /* MoveToward */
void func_002463B8(void);       /* CAM_ToThirdPerson */
void func_00246430(void);       /* CAM_ToFirstPerson */
void func_00246768(void);       /* CAM_StartMouseMove */
void func_002467F0(void);       /* CAM_EndMouseMove */
void func_00246808(void);       /* CAM_StartDistance */
void func_002468A0(void);       /* CAM_EndDistance */

/* MoveToward */
float func_002456F8(float cur, float goal, float maxspeed)
{
    if (cur != goal)
    {
        if (abs((int)(cur - goal)) > 180.0f)
        {
            if (cur < goal)
                cur += 360.0f;
            else
                cur -= 360.0f;
        }

        if (cur < goal)
        {
            if (cur < goal - 1.0f)
                cur += (goal - cur) * 0.25f;
            else
                cur = goal;
        }
        else
        {
            if (cur > goal + 1.0f)
                cur -= (cur - goal) * 0.25f;
            else
                cur = goal;
        }
    }

    if (cur < 0)
        cur += 360.0f;
    else if (cur >= 360)
        cur -= 360;

    return cur;
}

/* CAM_Think */
void func_00245848(void)
{
    vec3_t ext, pnt, camForward, camRight, camUp;
    moveclip_t clip;
    float dist;
    vec3_t camAngles;
    int i;
    cl_entity_t *ent = NULL;

    switch ((int)cam_command.value)
    {
    case 1:
        func_002463B8();
        break;
    case 2:
        func_00246430();
        break;
    case 0:
    default:
        break;
    }

    if (!cam_thirdperson)
        return;

    if (cam_contain.value)
    {
        ent = &cl_entities[0][cl[0].viewentity];
        ext[0] = ext[1] = ext[2] = 0.0f;
    }

    camAngles[PITCH] = cam_idealpitch.value;
    camAngles[YAW] = cam_idealyaw.value;
    dist = cam_idealdist.value;

    if (D_00399110)
    {
        if (!D_00399104)
        {
            if (D_00399118[0] > D_0039970C)
            {
                if (camAngles[YAW] < c_maxyaw.value)
                    camAngles[YAW] += 0.25f * (D_00399118[0] - D_0039970C);
                if (camAngles[YAW] > c_maxyaw.value)
                    camAngles[YAW] = c_maxyaw.value;
            }
            else if (D_00399118[0] < D_0039970C)
            {
                if (camAngles[YAW] > c_minyaw.value)
                    camAngles[YAW] -= 0.25f * (D_0039970C - D_00399118[0]);
                if (camAngles[YAW] < c_minyaw.value)
                    camAngles[YAW] = c_minyaw.value;
            }

            if (D_00399118[1] > D_003996D8)
            {
                if (camAngles[PITCH] < c_maxpitch.value)
                    camAngles[PITCH] += 0.25f * (D_00399118[1] - D_003996D8);
                if (camAngles[PITCH] > c_maxpitch.value)
                    camAngles[PITCH] = c_maxpitch.value;
            }
            else if (D_00399118[1] < D_003996D8)
            {
                if (camAngles[PITCH] > c_minpitch.value)
                    camAngles[PITCH] -= 0.25f * (D_003996D8 - D_00399118[1]);
                if (camAngles[PITCH] < c_minpitch.value)
                    camAngles[PITCH] = c_minpitch.value;
            }

            D_00399100 = D_00399118[0] * D_00331844;
            D_00399108 = D_00399118[1] * D_00331844;
        }
    }

    if (func_0024DA78(&cam_pitchup))
        camAngles[PITCH] += 2.5f;
    else if (func_0024DA78(&cam_pitchdown))
        camAngles[PITCH] -= 2.5f;

    if (func_0024DA78(&cam_yawleft))
        camAngles[YAW] -= 2.5f;
    else if (func_0024DA78(&cam_yawright))
        camAngles[YAW] += 2.5f;

    if (func_0024DA78(&cam_in))
    {
        dist -= 1.0f;
        if (dist < 30.0f)
        {
            camAngles[PITCH] = 0;
            camAngles[YAW] = 0;
            dist = 30.0f;
        }
    }
    else if (func_0024DA78(&cam_out))
        dist += 1.0f;

    if (D_00399104)
    {
        if (D_00399118[1] > D_003996D8)
        {
            if (dist < c_maxdistance.value)
                dist += 0.5f * (D_00399118[1] - D_003996D8);
            if (dist > c_maxdistance.value)
                dist = c_maxdistance.value;
        }
        else if (D_00399118[1] < D_003996D8)
        {
            if (dist > c_mindistance.value)
                dist -= 0.5f * (D_003996D8 - D_00399118[1]);
            if (dist < c_mindistance.value)
                dist = c_mindistance.value;
        }
        D_00399100 = D_00399118[0] * D_00331844;
        D_00399108 = D_00399118[1] * D_00331844;
    }

    if (cam_contain.value)
    {
        VectorCopy(ent->origin, pnt);
        AngleVectors(camAngles, camForward, camRight, camUp);
        for (i = 0; i < 3; i++)
            pnt[i] += -dist * camForward[i];

        memset(&clip, 0, sizeof(moveclip_t));
        clip.trace = SV_ClipMoveToEntity(sv.edicts, r_refdef.vieworg, ext, ext, pnt);
        if (clip.trace.fraction == 1.0f)
        {
            cam_idealpitch.value = camAngles[PITCH];
            cam_idealyaw.value = camAngles[YAW];
            cam_idealdist.value = dist;
        }
    }
    else
    {
        cam_idealpitch.value = camAngles[PITCH];
        cam_idealyaw.value = camAngles[YAW];
        cam_idealdist.value = dist;
    }

    VectorCopy(cam_ofs, camAngles);

    if (cam_snapto.value)
    {
        camAngles[YAW] = cam_idealyaw.value + cl[0].viewangles[YAW];
        camAngles[PITCH] = cam_idealpitch.value + cl[0].viewangles[PITCH];
        camAngles[2] = cam_idealdist.value;
    }
    else
    {
        if (camAngles[YAW] - cl[0].viewangles[YAW] != cam_idealyaw.value)
            camAngles[YAW] = func_002456F8(camAngles[YAW], cam_idealyaw.value + cl[0].viewangles[YAW], 2.5f);

        if (camAngles[PITCH] - cl[0].viewangles[PITCH] != cam_idealpitch.value)
            camAngles[PITCH] = func_002456F8(camAngles[PITCH], cam_idealpitch.value + cl[0].viewangles[PITCH], 2.5f);

        if (abs(camAngles[2] - cam_idealdist.value) < 2.0f)
            camAngles[2] = cam_idealdist.value;
        else
            camAngles[2] += (cam_idealdist.value - camAngles[2]) * 0.25f;
    }

    if (cam_contain.value)
    {
        dist = camAngles[ROLL];
        camAngles[ROLL] = 0;

        VectorCopy(ent->origin, pnt);
        AngleVectors(camAngles, camForward, camRight, camUp);
        for (i = 0; i < 3; i++)
            pnt[i] += -dist * camForward[i];

        memset(&clip, 0, sizeof(moveclip_t));
        ext[0] = ext[1] = ext[2] = 0.0f;
        clip.trace = SV_ClipMoveToEntity(sv.edicts, r_refdef.vieworg, ext, ext, pnt);
        if (clip.trace.fraction != 1.0f)
            return;
    }
    cam_ofs[0] = camAngles[0];
    cam_ofs[1] = camAngles[1];
    cam_ofs[2] = dist;
}

/* CAM_PitchUpDown */
void func_00246238(void)
{
    func_0024C500(&cam_pitchup);
}

/* CAM_PitchUpUp */
void func_00246258(void)
{
    func_0024C5B0(&cam_pitchup);
}

/* CAM_PitchDownDown */
void func_00246278(void)
{
    func_0024C500(&cam_pitchdown);
}

/* CAM_PitchDownUp */
void func_00246298(void)
{
    func_0024C5B0(&cam_pitchdown);
}

/* CAM_YawLeftDown */
void func_002462B8(void)
{
    func_0024C500(&cam_yawleft);
}

/* CAM_YawLeftUp */
void func_002462D8(void)
{
    func_0024C5B0(&cam_yawleft);
}

/* CAM_YawRightDown */
void func_002462F8(void)
{
    func_0024C500(&cam_yawright);
}

/* CAM_YawRightUp */
void func_00246318(void)
{
    func_0024C5B0(&cam_yawright);
}

/* CAM_InDown */
void func_00246338(void)
{
    func_0024C500(&cam_in);
}

/* CAM_InUp */
void func_00246358(void)
{
    func_0024C5B0(&cam_in);
}

/* CAM_OutDown */
void func_00246378(void)
{
    func_0024C500(&cam_out);
}

/* CAM_OutUp */
void func_00246398(void)
{
    func_0024C5B0(&cam_out);
}

/* CAM_ToThirdPerson */
void func_002463B8(void)
{
    if (cl[0].maxclients > 1)
        return;
    if (!cam_thirdperson)
    {
        cam_thirdperson = 1;
        cam_ofs[0] = cl[0].viewangles[0];
        cam_ofs[1] = cl[0].viewangles[1];
        cam_ofs[2] = 30.0f;
    }
    Cvar_SetValue(D_00388E58, 0);
}

/* CAM_ToFirstPerson */
void func_00246430(void)
{
    cam_thirdperson = 0;
    Cvar_SetValue(D_00388E58, 0);
}

/* CAM_ToggleSnapto */
void func_00246460(void)
{
    cam_snapto.value = !cam_snapto.value;
}

INCLUDE_RODATA("asm/nonmatchings/engine/in_camera", D_00388E58);

INCLUDE_RODATA("asm/nonmatchings/engine/in_camera", D_00388E68);

INCLUDE_RODATA("asm/nonmatchings/engine/in_camera", D_00388E78);

INCLUDE_RODATA("asm/nonmatchings/engine/in_camera", D_00388E80);

INCLUDE_RODATA("asm/nonmatchings/engine/in_camera", D_00388E90);

INCLUDE_RODATA("asm/nonmatchings/engine/in_camera", D_00388EA0);

INCLUDE_RODATA("asm/nonmatchings/engine/in_camera", D_00388EA8);

INCLUDE_RODATA("asm/nonmatchings/engine/in_camera", D_00388EB8);

INCLUDE_RODATA("asm/nonmatchings/engine/in_camera", D_00388EC8);

INCLUDE_RODATA("asm/nonmatchings/engine/in_camera", D_00388ED0);

INCLUDE_RODATA("asm/nonmatchings/engine/in_camera", D_00388EE0);

INCLUDE_RODATA("asm/nonmatchings/engine/in_camera", D_00388EE8);

INCLUDE_RODATA("asm/nonmatchings/engine/in_camera", D_00388EF8);

INCLUDE_RODATA("asm/nonmatchings/engine/in_camera", D_00388F00);

INCLUDE_RODATA("asm/nonmatchings/engine/in_camera", D_00388F10);

INCLUDE_RODATA("asm/nonmatchings/engine/in_camera", D_00388F18);

INCLUDE_RODATA("asm/nonmatchings/engine/in_camera", D_00388F28);

INCLUDE_RODATA("asm/nonmatchings/engine/in_camera", D_00388F30);

INCLUDE_RODATA("asm/nonmatchings/engine/in_camera", D_00388F40);

INCLUDE_RODATA("asm/nonmatchings/engine/in_camera", D_00388F48);

/* CAM_Init */
void func_00246490(void)
{
    Cmd_AddCommand("+campitchup", func_00246238);
    Cmd_AddCommand("-campitchup", func_00246258);
    Cmd_AddCommand("+campitchdown", func_00246278);
    Cmd_AddCommand("-campitchdown", func_00246298);
    Cmd_AddCommand("+camyawleft", func_002462B8);
    Cmd_AddCommand("-camyawleft", func_002462D8);
    Cmd_AddCommand("+camyawright", func_002462F8);
    Cmd_AddCommand("-camyawright", func_00246318);
    Cmd_AddCommand("+camin", func_00246338);
    Cmd_AddCommand("-camin", func_00246358);
    Cmd_AddCommand("+camout", func_00246378);
    Cmd_AddCommand("-camout", func_00246398);
    Cmd_AddCommand("thirdperson", func_002463B8);
    Cmd_AddCommand("firstperson", func_00246430);
    Cmd_AddCommand("+cammousemove", func_00246768);
    Cmd_AddCommand("-cammousemove", func_002467F0);
    Cmd_AddCommand("+camdistance", func_00246808);
    Cmd_AddCommand("-camdistance", func_002468A0);
    Cmd_AddCommand("snapto", func_00246460);
    Cvar_RegisterVariable(&cam_command);
    Cvar_RegisterVariable(&cam_snapto);
    Cvar_RegisterVariable(&cam_idealyaw);
    Cvar_RegisterVariable(&cam_idealpitch);
    Cvar_RegisterVariable(&cam_idealdist);
    Cvar_RegisterVariable(&cam_contain);
    Cvar_RegisterVariable(&c_maxpitch);
    Cvar_RegisterVariable(&c_minpitch);
    Cvar_RegisterVariable(&c_maxyaw);
    Cvar_RegisterVariable(&c_minyaw);
    Cvar_RegisterVariable(&c_maxdistance);
    Cvar_RegisterVariable(&c_mindistance);
}

/* CAM_ClearStates */
/* CAM_ClearStates */
void func_002466B0(void)
{
    cam_pitchup.state = 0;
    cam_pitchdown.state = 0;
    cam_yawleft.state = 0;
    cam_yawright.state = 0;
    cam_in.state = 0;
    cam_out.state = 0;
    cam_thirdperson = 0;
    cam_command.value = 0;
    D_00399110 = 0;
    cam_snapto.value = 0;
    D_00399104 = 0;
    cam_ofs[0] = 0;
    cam_ofs[1] = 0;
    cam_ofs[2] = 30.0f;
    cam_idealpitch.value = cl[0].viewangles[0];
    cam_idealyaw.value = cl[0].viewangles[1];
    cam_idealdist.value = 30.0f;
}

/* CAM_StartMouseMove */
#ifdef NON_MATCHING
void func_00246768(void)
{
    float flSensitivity;

    if (cam_thirdperson)
    {
        if (!D_00399110)
        {
            D_00399110 = 1;
            iMouseInUse = 1;
            flSensitivity = D_00331844;
            D_00399100 = D_00399118[0] * flSensitivity;
            D_00399108 = D_00399118[1] * flSensitivity;
        }
    }
    else
    {
        D_00399110 = 0;
        iMouseInUse = 0;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/in_camera", func_00246768);
#endif

/* CAM_EndMouseMove */
void func_002467F0(void)
{
    D_00399110 = 0;
    iMouseInUse = 0;
}

/* CAM_StartDistance */
#ifdef NON_MATCHING
void func_00246808(void)
{
    float flSensitivity;

    if (cam_thirdperson)
    {
        if (!D_00399104)
        {
            D_00399104 = 1;
            D_00399110 = 1;
            iMouseInUse = 1;
            flSensitivity = D_00331844;
            D_00399100 = D_00399118[0] * flSensitivity;
            D_00399108 = D_00399118[1] * flSensitivity;
        }
    }
    else
    {
        D_00399104 = 0;
        D_00399110 = 0;
        iMouseInUse = 0;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/engine/in_camera", func_00246808);
#endif

/* CAM_EndDistance */
void func_002468A0(void)
{
    D_00399104 = 0;
    D_00399110 = 0;
    iMouseInUse = 0;
}
