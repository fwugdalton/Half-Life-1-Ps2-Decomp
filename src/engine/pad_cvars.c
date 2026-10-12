#include "common.h"
#include "engine/cvar.h"

/* cvars de controle por jogador (m_pitch_0, m_yaw_1, ...): copia os modelos e renomeia com o índice */

INCLUDE_RODATA("asm/nonmatchings/engine/pad_cvars", D_003963E8);

INCLUDE_RODATA("asm/nonmatchings/engine/pad_cvars", D_003963F0);

INCLUDE_RODATA("asm/nonmatchings/engine/pad_cvars", D_003963F8);

INCLUDE_RODATA("asm/nonmatchings/engine/pad_cvars", D_00396400);

INCLUDE_RODATA("asm/nonmatchings/engine/pad_cvars", D_00396408);

INCLUDE_RODATA("asm/nonmatchings/engine/pad_cvars", D_00396410);

INCLUDE_RODATA("asm/nonmatchings/engine/pad_cvars", D_00396418);

INCLUDE_RODATA("asm/nonmatchings/engine/pad_cvars", D_00396420);

INCLUDE_RODATA("asm/nonmatchings/engine/pad_cvars", D_00396428);

INCLUDE_RODATA("asm/nonmatchings/engine/pad_cvars", D_00396438);

INCLUDE_RODATA("asm/nonmatchings/engine/pad_cvars", D_00396440);

INCLUDE_RODATA("asm/nonmatchings/engine/pad_cvars", D_00396448);

INCLUDE_RODATA("asm/nonmatchings/engine/pad_cvars", D_00396450);

INCLUDE_RODATA("asm/nonmatchings/engine/pad_cvars", D_00396460);

INCLUDE_RODATA("asm/nonmatchings/engine/pad_cvars", D_00396470);

INCLUDE_RODATA("asm/nonmatchings/engine/pad_cvars", D_00396478);

INCLUDE_RODATA("asm/nonmatchings/engine/pad_cvars", D_00396488);

INCLUDE_RODATA("asm/nonmatchings/engine/pad_cvars", D_00396490);

INCLUDE_RODATA("asm/nonmatchings/engine/pad_cvars", D_003964A8);

INCLUDE_RODATA("asm/nonmatchings/engine/pad_cvars", D_003964B0);

extern cvar_t D_00334EB0;           /* name */
extern cvar_t D_00334EC8;
extern cvar_t D_00334EE0;           /* m_pitch */
extern cvar_t D_00334EF8;           /* m_yaw */
extern cvar_t D_00334F10;           /* m_forward */
extern cvar_t D_00334F28;           /* m_side */
extern cvar_t D_00334F40;           /* sensitivity */
extern cvar_t D_00334F58;
extern cvar_t D_00334F70;
extern cvar_t D_00334F88;
extern cvar_t D_00334FA0;

extern cvar_t D_0055A328[2];
extern cvar_t D_00556030[2];
extern cvar_t D_0055A2D8[2];
extern cvar_t D_0056A220[2];
extern cvar_t D_0056A1F8[2];
extern cvar_t D_00556008[2];
extern cvar_t D_0055A350[2];
extern cvar_t D_0055A3A0[2];
extern cvar_t D_0055A300[2];
extern cvar_t D_0055A378[2];
extern cvar_t D_00556058[2];

char *func_00228D30(char *in);
int sprintf(char *, const char *, ...);

void func_002D0520(void)
{
    char buf[112];
    int i;

    for (i = 0; i < 2; i++)
    {
        D_0055A328[i] = D_00334EE0;
        sprintf(buf, "m_pitch_%d", i);
        D_0055A328[i].name = func_00228D30(buf);
        D_0055A328[i].flags |= i << 16;

        D_00556030[i] = D_00334EF8;
        sprintf(buf, "m_yaw_%d", i);
        D_00556030[i].name = func_00228D30(buf);
        D_00556030[i].flags |= i << 16;

        D_0055A2D8[i] = D_00334F10;
        sprintf(buf, "m_forward_%d", i);
        D_0055A2D8[i].name = func_00228D30(buf);
        D_0055A2D8[i].flags |= i << 16;

        D_0056A220[i] = D_00334F28;
        sprintf(buf, "m_side_%d", i);
        D_0056A220[i].name = func_00228D30(buf);
        D_0056A220[i].flags |= i << 16;

        D_0056A1F8[i] = D_00334F40;
        sprintf(buf, "sensitivity_%d", i);
        D_0056A1F8[i].name = func_00228D30(buf);
        D_0056A1F8[i].flags |= i << 16;

        D_00556008[i] = D_00334EB0;
        sprintf(buf, "name_%d", i);
        D_00556008[i].name = func_00228D30(buf);
        D_00556008[i].flags |= i << 16;

        sprintf(buf, "%s_%d", D_00334EC8.name, i);
        D_0055A350[i] = D_00334EC8;
        D_0055A350[i].name = func_00228D30(buf);
        D_0055A350[i].flags |= i << 16;

        sprintf(buf, "%s_%d", D_00334F58.name, i);
        D_0055A3A0[i] = D_00334F58;
        D_0055A3A0[i].name = func_00228D30(buf);

        sprintf(buf, "%s_%d", D_00334F70.name, i);
        D_0055A300[i] = D_00334F70;
        D_0055A300[i].name = func_00228D30(buf);

        sprintf(buf, "%s_%d", D_00334F88.name, i);
        D_0055A378[i] = D_00334F88;
        D_0055A378[i].name = func_00228D30(buf);

        sprintf(buf, "%s_%d", D_00334FA0.name, i);
        D_00556058[i] = D_00334FA0;
        D_00556058[i].name = func_00228D30(buf);
    }
}
