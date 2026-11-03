---
name: hl1ps2-dlls-aiplayer
description: dlls/aiplayer.cpp (IA do parceiro do Decay, baseada no HPB bot): faixas, dados, NON_MATCHING e truques que casaram
metadata:
  type: project
  modified: 2026-11-03T12:12:04.011Z
---
dlls/aiplayer.cpp feito em 2026-11-03: 39 funções, 34 casam. Métodos Ps2AI* do CBasePlayer (declarados no player.h).
- Faixas: texto 0x126F80–0x12CF18 (rom 0x026F80), .data 0x30D8D0 (g_AIAmmo[15] e depois g_AIWeapons[17] em 0x30D988, ai_weapon_t de 0x44), .rodata 0x33B2E0–0x33B630. Dados e rodata escritos em C casaram de primeira (dados definidos logo depois das declarações, antes das funções).
- NON_MATCHING: FindItem (ordem de 2 constantes içadas), FireWeapon (switch), ChangeWeapon (registradores), Dodge (agendamento de args), TakeDamage (o alvo reusa o registrador de m_iAIState&0x400 como zero no FNullEnt). O splat migrou a rodata usada só por elas; strings compartilhadas com funções em C (D_0033B490 func_ladder, D_0033B5D0/5E0 GINA_STOP/COL_STOP) e os campos de g_AIWeapons (D_0030D98C/9A0/9C8) vão no linker_script_extra.ld.
- EHANDLE: a ROM chama __opP (operator CBaseEntity*) em vários lugares onde `m_h->` daria __rf e `m_h != NULL` daria __opi. O cmpfunc não vê, só o cmp da ROM: escrever `((CBaseEntity *)m_hEnemy)->` e `(CBaseEntity *)m_hAIUser != NULL`.
- vtable de CBaseEntity: entrada de 8 bytes começando em 0x08 (Classify 0x48, MyMonsterPointer 0x90, IsAlive 0x118, IsPlayer 0x140). PlaySentence do CBasePlayer em 0x2D8.
- Registro func_0012CEE8: func_00239810("007", &CBasePlayer::Ps2AIUse).
- Strings de fala: GINA_/COL_ + ATTACK/STOP/PAIN/DIE (m_iTeamNum==1 é a Gina), só com coopslave.value == 0, RANDOM_FLOAT(2.8f,3.2f), volume 1, atenuação 2.
- O enum ignore_glass agora está no util.h (tirado de h_battery, healthkit, monsters, combat, h_ai, hgrunt).
- Truques: `Vector v = expr` na declaração monta no lugar; constantes locais (`float flSide = 8;`) forçam registradores salvos; `if (x & bit) return TRUE; return FALSE;` em vez de `return x != 0`.
Ver [[hl1ps2-dlls-player]] e [[hl1ps2-dlls-decay]].
