---
name: hl1ps2-dlls-controller
description: controller.cpp (alien controller) do PS2: diferenças do HLSDK, NON_MATCHING do RunAI e truques (fabsf misto, int no m_iBallCurrent, dpsub)
metadata:
  type: project
---
controller.cpp feito em 2026-10-31 (commit 26d644d): texto 0x14D630–0x1517C8, .data 0x30ECC0, .rodata 0x344040 (vtables ZapBall 0x344578, HeadBall 0x344980, CController 0x344D88). 52/53 casam; RunAI é NON_MATCHING (INCLUDE_ASM de asm/nonmatchings/dlls/controller/; só agendamento e ordem dos endereços guardados na pilha). Ver [[hl1ps2-dlls-done-9]].

Diferenças do PS2:
- arrays pAttackSoundsMP {attack1, attack3} e pIdleSoundsMP {idle5}; Idle/AttackSound e Precache escolhem pelo IsMultiplayer; AlertSound e o precache dos alert só fora do multiplayer.
- toda mensagem TE_ELIGHT (HandleAnimEvent, RunAI, 1ª do HuntThink) fica dentro de `if (!g_pGameRules->IsMultiplayer())`; o TE_BEAMENTPOINT do HuntThink não.
- attachments um a menos: GetAttachment(0) no anim event, GetAttachment(1) no RunTask, SetAttachment(i+2)/GetAttachment(i+1)/0x1000*(i+2) no RunAI.
- GetSchedule só `return CSquadMonster::GetSchedule();` (sem o Intersect de teste).
- CControllerHeadBall sem m_iTrail (m_flNextAttack logo após CBaseMonster); HuntThink não grava m_flNextAttack e usa UTIL_TraceLineNoEnts (0x207F18).

Truques:
- constantes: trocar todos os literais por float (0.5f etc.), senão aparecem chamadas de double; sqrt -> sqrtf (extern "C").
- LookupFloat: `if (fabsf(x) > fabs(y) && fabsf(x) > fabsf(z))` e `else if (fabsf(y) > fabsf(z))` (o 1º compara em double, o fabs double negando com dpsub = 0x2DF490, adicionado ao symbol_addrs).
- RunTask casou com `inline BOOL CBaseMonster::HasConditions` definida no .cpp antes dele.
- RunAI: `m_iBallCurrent[i] += (int)((m_iBall[i] - m_iBallCurrent[i]) * t)` (soma inteira) dá a lógica certa mas não o agendamento.
- POWERUP_FULL/HALF saem mesclados pelo gcc sozinho.
- D_00344320 (rodata citado pelo asm do RunAI) foi para o linker_script_extra.ld.
