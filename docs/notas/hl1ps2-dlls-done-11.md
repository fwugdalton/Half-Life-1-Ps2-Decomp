---
name: hl1ps2-dlls-done-11
description: continuação da lista de .cpp das dlls feitos (buttons em diante), mudanças do PS2 no buttons.cpp e próximo arquivo (bullsquid.cpp)
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

Próximo: bullsquid.cpp. .text termina em 0x1412F0 (static init em 0x141288/0x1412D0), .rodata termina em 0x341810, .data termina em 0x30EA68.
