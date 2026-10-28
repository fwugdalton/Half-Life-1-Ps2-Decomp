---
name: hl1ps2-dlls-schedule
description: dlls/schedule.cpp do HL1 PS2 (faixas, diferenças do PS2, inline de HasConditions e alinhamento da rodata)
metadata:
  type: project
---

schedule.cpp feito em 2026-10-20 (commit f0adab4), 13/13 funções casando.
- Faixas: texto 0x1DB108–0x1DD878, sem .data, .rodata 0x370FE0–0x3715A0 (0x5B4 + `__asm__(".section .rodata\n.align 4\n.section .text")` no fim, porque a rodata termina numa jtbl).
- `inline` de SetConditions/ClearConditions/HasConditions no topo do arquivo: as funções pequenas inlinam; RunTask/StartTask chamam HasConditions fora de linha porque vem do TaskComplete (inline do header, compilado antes).
- PS2: campo novo CBaseMonster::m_flPathCheckTime (0x29C, em basemonster.h). TASK_WALK/RUN_TO_TARGET existe também no RunTask (repete a cada 3 s se m_pCine e o MoveToTarget falhou); GET_PATH_TO_ENEMY e _LKP recusam nova busca por 3 s após falha ("Not enough time since last check").
- TASK_DIE: se multiplayer ou Classify() entre CLASS_ALIEN_MILITARY e CLASS_INSECT (exclusivo), chama SUB_StartFadeOutLater (0x14A148, fade em 5 s) em vez do fluxo normal.
- FRefreshRoute(1) no RunTask; fabsf/1.0f no GetSchedule; slError = 0x30F4B0.
- CNode do PS2 tem 0x60 bytes (m_sHintActivity 0x52, m_flHintYaw 0x58); WorldGraph.m_pNodes em 0x31524C.
Ver [[hl1ps2-dlls]] e [[hl1ps2-dlls-shotgun]].

## satchel.cpp (100%, de primeira)
- Texto 0x1D9B68–0x1DB108, .data 0x318100 (só o m_SaveData), .rodata 0x370718–0x370FE0. Versão antiga do HLSDK: PrimaryAttack com switch (0 joga, 1 detona, 2 nada), SecondaryAttack joga, Deploy sem DefaultDeploy, WeaponIdle chama GetAutoaimVector(AUTOAIM_10DEGREES) antes.
- AddDuplicate usa g_pGameRules->Ps2Virt07() (vtable 0x38). Registro func_001DAEC0: 248 DestroyItem, 249 SatchelThink, 250 SatchelSlide (func_002397F0 = toque), 251 DetonateUse.
- HLSDK atual em raw.githubusercontent.com/ValveSoftware/halflife/master/dlls/ (a rede funciona; baixar para /tmp/claude-0/hlsdk).

rpg.cpp feito em 2026-10-20 (commit 2e1507e). Texto 0x1D75B8–0x1D9B68, .data 0x3180A0, .rodata 0x36F978 (vtables: CRpgAmmo 0x36FC50, CRpgRocket 0x36FE38, CRpg 0x370250, CLaserSpot 0x370528). CRpg: m_pSpot 0x8C, m_fSpotActive 0x90, m_fSpotVisible 0x94 (PS2), m_cActiveRockets 0x98, m_flNextEmptySound 0x9C (PS2; PrimaryAttack sem munição toca PlayEmptySound a cada 1.5 s). CRpgRocket: m_iTrail 0x2A8, m_flIgniteTime 0x2AC, m_pLauncher 0x2B0 (ponteiro, SDK antigo). Diferenças do PS2: o foguete tem cópia própria do CGrenade::Explode (sem puxar da parede) e do Smoke (SetThink SUB_Remove + 10 s); FollowThink desconta m_cActiveRockets depois do Detonate; Spawn do CRpg faz MAKE_STRING("weapon_rpg") antes do Precache; IsMultiplayer aqui é Ps2Virt07 (slot 0x38). Explode e Smoke NON_MATCHING (o alvo carrega o pfnWriteByte depois do trunc do (dmg-50)*0.6, mesmo problema do slave_collar; -fshort-double também não resolve). Truque: funções dummy em `.rpg_discard` emitem o ponteiro para Smoke, "spark_shower" e o ponteiro para SUB_Remove no lugar certo; os rótulos D_0036FB10/FB18/FB28 foram para o linker_script_extra.ld. Detonate do CGrenade = 0x1785E8, ExplodeTouch = 0x1786E8.

rosenberg.cpp e roach.cpp: ver [[hl1ps2-dlls-rosenberg]].
