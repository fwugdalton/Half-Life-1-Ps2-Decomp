#include "common.h"
#include "engine/mathlib.h"
#include "engine/model.h"
#include "engine/server.h"
#include "engine/cvar.h"
#include "engine/progdefs.h"
#include "engine/eiface.h"
#include "engine/pmove.h"

#define FL_ONGROUND (1 << 9)
#define MAX_FORWARD 6
#define ON_EPSILON 0.1f

extern edict_t *sv_player;
extern byte *g_playertouch;
extern cvar_t sv_idealpitchscale;
extern float host_frametime;
void *memset(void *, int, unsigned int);
trace_t SV_Move(const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int type, edict_t *passedict, qboolean monsterClipBrush);

void func_00289058(void)
{
    float angleval, sinval, cosval;
    trace_t tr;
    vec3_t top, bottom;
    float z[MAX_FORWARD];
    int i, j;
    int step, dir, steps;

    if (!(sv_player->v.flags & FL_ONGROUND))
        return;

    angleval = sv_player->v.angles[YAW] * M_PI * 2 / 360;
    sinval = sinf(angleval);
    cosval = cosf(angleval);

    for (i = 0; i < MAX_FORWARD; i++)
    {
        top[0] = sv_player->v.origin[0] + cosval * (i + 3) * 12;
        top[1] = sv_player->v.origin[1] + sinval * (i + 3) * 12;
        top[2] = sv_player->v.origin[2] + sv_player->v.view_ofs[2];

        bottom[0] = top[0];
        bottom[1] = top[1];
        bottom[2] = top[2] - 160;

        tr = SV_Move(top, vec3_origin, vec3_origin, bottom, 1, sv_player, 0);
        if (tr.allsolid)
            return;

        if (tr.fraction == 1)
            return;

        z[i] = top[2] + tr.fraction * (bottom[2] - top[2]);
    }

    dir = 0;
    steps = 0;
    for (j = 1; j < i; j++)
    {
        step = z[j] - z[j - 1];
        if (step > -ON_EPSILON && step < ON_EPSILON)
            continue;

        if (dir && (step - dir > ON_EPSILON || step - dir < -ON_EPSILON))
            return;

        steps++;
        dir = step;
    }

    if (!dir)
    {
        sv_player->v.idealpitch = 0;
        return;
    }

    if (steps < 2)
        return;

    sv_player->v.idealpitch = -dir * sv_idealpitchscale.value;
}

void func_00289320(float frametime)
{
    float len;

    len = VectorNormalize(sv_player->v.punchangle);
    len -= (10.0f + len * 0.5f) * frametime;
    if (len < 0)
        len = 0;
    VectorScale(sv_player->v.punchangle, len, sv_player->v.punchangle);
}

void SV_PreRunCmd(void)
{
    memset(g_playertouch, 0, (sv.max_edicts + 7) / 8);
}

INCLUDE_RODATA("asm/nonmatchings/engine/sv_user", D_0038FEF0);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_user", D_0038FEF8);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_user", D_0038FF10);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_user", D_0038FF18);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_user", D_0038FF28);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_user", D_0038FF30);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_user", D_0038FF40);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_user", D_0038FF48);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_user", D_0038FF58);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_user", D_0038FF60);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_user", D_0038FF70);

INCLUDE_RODATA("asm/nonmatchings/engine/sv_user", D_0038FF78);

INCLUDE_ASM("asm/nonmatchings/engine/sv_user", func_002893F8);

INCLUDE_ASM("asm/nonmatchings/engine/sv_user", func_00289818);

INCLUDE_ASM("asm/nonmatchings/engine/sv_user", SV_RunCmd);

INCLUDE_ASM("asm/nonmatchings/engine/sv_user", func_0028A820);

INCLUDE_ASM("asm/nonmatchings/engine/sv_user", SV_ExecuteClientMessage);

INCLUDE_ASM("asm/nonmatchings/engine/sv_user", func_0028B188);

INCLUDE_ASM("asm/nonmatchings/engine/sv_user", func_0028B200);

INCLUDE_ASM("asm/nonmatchings/engine/sv_user", func_0028B2A0);

INCLUDE_ASM("asm/nonmatchings/engine/sv_user", func_0028B308);
