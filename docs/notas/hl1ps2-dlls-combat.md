---
name: hl1ps2-dlls-combat
description: combat.cpp do PS2 (gibs, dano, FVisible): diferenças do HLSDK, 5 NON_MATCHING e truques (heurística de laço, dummies de rodata)
metadata:
  type: project
---
combat.cpp feito em 2026-10-31 (commit d1a8404): texto 0x1487C0–0x14D630, sem .data, .rodata 0x343C60 (vtable CGib 0x343E58). CGib definida no próprio .cpp (não está em include/dlls/monsters.h). 38/43 casam; NON_MATCHING (INCLUDE_ASM): SpawnStickyGibs, SpawnRandomGibs, FireBullets, TraceBleed, MakeDamageBloodDecal. Ver [[hl1ps2-dlls-done-9]].

Diferenças do PS2:
- HasHumanGibs aceita também a classe 14; GibMonster sem o TraceResult; CallGibMonster usa effects |= EF_NODRAW.
- SpawnRandomGibs(pev, cGibs, human, fBig): com fBig põe SetTouch(BounceGibTouch) e scale 3 (o gargantua chama com 1; alias func_00149170 no linker_script_extra).
- novos SUB_StartFadeOutLater (fade começa em 5 s) e SUB_FadeOutFast (passo 21, 0x14A278; declarado no cbase.h).
- Bounce/StickyGibTouch: gib some (UTIL_Remove) se pOther->IsPlayer(); Sticky testa pOther != NULL antes do "worldspawn".
- TakeDamage: script só ignora dano sem spawnflag 0x4000; com health <= 0 e deadflag DEAD_NO chama g_pGameRules->Ps2Virt31(this, attacker, inflictor); FCheckAITrigger() em MONSTERSTATE_SCRIPT nos dois caminhos.
- FVisible (os dois): se o trace bate num func_wall com spawnflag 4, traça de novo a partir do vecEndPos ignorando-o.
- RadiusDamage usa UTIL_TraceLineNoEnts (0x207F18). FireBullets do PS2 é bem maior (strncmp, contador global, mensagens extras): não escrito.
- registro func_0014D4C0: 055..061 + "401" (SUB_FadeOutFast).

Truques:
- NÃO usar -fmove-all-movables: RadiusDamage só casa sem; a ROM tira do laço só constantes usadas mais de uma vez (heurística normal), mas no TraceBleed/MakeDamageBloodDecal tira também o 1.0 (não consegui reproduzir).
- inline HasConditions/SetConditions definidas no .cpp (casou TakeDamage e Killed).
- tracerCount (static local) = `extern "C" int D_003A3774` com #define.
- gamerules.h DEPOIS do cbase.h aqui ("" do TeamID antes de "Half-Life").
- constantes de ponteiro para membro das funções em asm: dummies em .combat_discard depois do INCLUDE_ASM; D_ citados pelos .s vão para o linker_script_extra.
- controller.o precisa de .rodata alinhado a 16 (Makefile), senão o fim do combat fica 8 bytes curto.
- um byte errado no ROM após cmpfile OK = alvo de jal diferente (UTIL_TraceLine vs NoEnts): cmpfile ignora relocações.
