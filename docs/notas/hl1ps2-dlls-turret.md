---
name: hl1ps2-dlls-turret
description: dlls/turret.cpp (torretas e sentry): faixas, diferenças do PS2 e truques que casaram
metadata:
  type: project
---
turret.cpp feito em 2026-10-20 (commit 0dca00d), 52/52 funções casam. Texto 0x202B90–0x206730, .data 0x324288, .rodata 0x37F1F8 (+8 bytes de padding em cod/2807C8 antes do util). Antes dele vem tripmine.cpp (texto termina em 0x202B90, .data em 0x324288, .rodata em 0x37F1F8; o registro de nomes 0x202A30 e o _GLOBAL_$I 0x202B70 são os últimos).

Diferenças do PS2 vs HLSDK:
- Precache e mortes: tu_die3 só se !g_pGameRules->IsMultiplayer() (senão tu_die2); sem tu_search.wav.
- CSentry tem `int m_iShell` (0x310): Precache guarda PRECACHE_MODEL("models/shell.dol"); Shoot ejeta cápsula se !(spawnflags & 0x400) e põe m_eyeBrightness = 255; Spawn cria o m_pEyeGlow (escala 0.5) no GetAttachment(1).
- TakeDamage da sentry chama g_pGameRules->DeathNotice(this, pevAttacker, pevInflictor) (slot 31, era Ps2Virt31).
- TurretDeath/SentryDeath disparam FireTargets(m_iszTriggerTarget) se m_iTriggerCondition == AITRIGGER_DEATH; SentryDeath no multiplayer faz TE_EXPLOSION e UTIL_Remove.
- fim do arquivo: registro de ponteiros de método ids "318".."332".
Truques: trocar todos os literais por float (regex), __builtin_fabsf, Vector locais usados no fim declarados no começo da função (ordem na pilha).
Ver [[hl1ps2-dlls]].
