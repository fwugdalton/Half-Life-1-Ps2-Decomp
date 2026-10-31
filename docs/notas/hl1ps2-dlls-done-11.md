---
name: hl1ps2-dlls-done-11
description: continuação da lista de .cpp das dlls feitos (buttons, bullsquid), mudanças do PS2 e próximo arquivo (bmodels.cpp)
metadata:
  type: project
---

Continuação de [[hl1ps2-dlls-done-10]]. Formato: arquivo (endereço do texto, commit, funções que casam).

- buttons (0x1412F0, 6808d15, 79/79): .data 0x30EA68–0x30EBE8, .rodata 0x341810–0x342FF0. HLSDK com CGlobalState, CMultiSource (MS_MAX_TARGETS 32) e CBaseButton locais; todos os literais float com sufixo f (sem isso Off/UpdateSelfReturn crescem). Mudanças do PS2:
  - CMultiSource::Use: spawnflags&2 põe m_rgTriggered=1 em vez de inverter.
  - CBaseButton::KeyValue usa atoi nos campos BYTE; Spawn zera componentes do movedir com fabsf(x)<0.001f.
  - CMomentaryRotButton: m_start/m_end Vector, campos novos m_fInactive (0xF8) e m_iszEndLockTarget (0xFC, keyvalue "endlocktarget"). Spawn termina com `m_lastUsed = 0; m_fInactive = FALSE;` (nessa ordem). Use e Return saem cedo se m_fInactive. UpdateSelf: ao chegar no fim com spawnflags&0x10000, FireTargets(endlocktarget, this, this, USE_TOGGLE, 0) e m_fInactive = TRUE.
  - Classe nova CButtonToggleIndicator (button_toggle_indicator): Use acha pev->target e inverte o bit 0x200 do spawnflags dele.
  - Registro de funções extern "C" func_001446D8 (move-done, think, touch, use).
  - _GLOBAL_$I$_10CEnvGlobal$m_SaveData = 0x144E78 precisa estar no symbol_addrs (o .data em asm cita).

- bullsquid (0x13E8C0, f426bd3, 41/41): .data 0x30E6F0–0x30EA68, .rodata 0x340C40–0x341810 (começa com "classname"/"targetname"/"target" das inlines). Aqui os LINK_ENTITY_TO_CLASS ficam no meio (logo após cada classe), não no fim. Mudanças do PS2: campo novo m_flAttackTime (0x2AC, salvo); TE_SPRITE_SPRAY vale 110; Touch do cuspe usa RANDOM_LONG(90,110) e UTIL_TraceLineNoEnts; .spz/.dol; ChaseEnemy sem COND_CAN_RANGE_ATTACK1; schedules novos SquidSniff (fareja virando 30/60/30) e IdleStand (WAIT 1, conds do slIdleStand + CAN_RANGE1/MELEE1/MELEE2), tipos 49/50; GetSchedule no combate termina com: time < m_flNextSpitTime → IdleStand; time - m_flAttackTime > 5 → guarda o tempo e Chase; senão Sniff. fabs→fabsf, sufixo f em todos os floats, m_maxFrame sem (float). Registro func_001410E0: "038" Animate, "039" SUB_Remove.

Próximo: bmodels.cpp. .text termina em 0x13E8C0 (static init 0x13E7C8, inlines de 8 bytes e _GLOBAL_$I em 0x13E8A0), .rodata termina em 0x340C40, .data termina em 0x30E6F0.
