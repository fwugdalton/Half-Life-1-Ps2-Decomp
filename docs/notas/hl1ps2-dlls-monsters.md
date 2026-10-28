---
name: hl1ps2-dlls-monsters
description: dlls/monsters.cpp (CBaseMonster): faixas, -fmove-all-movables, diferenças do PS2 (info_aievent, triangulate) e truques
metadata:
  type: project
  modified: 2026-10-25T15:38:58.096Z
---
monsters.cpp feito em 2026-10-25 (commit 1710ffc): .text 0x1A1828–0x1A8D90, .data 0x314970–0x314F68, .rodata 0x363E10–0x364BA0. Com `-fmove-all-movables` no Makefile, 119/121 casam; RouteSimplify e FindCover ficaram NON_MATCHING (o laço deles na ROM não iça constantes nem com a flag, sem a flag eles casam mas Listen/BuildNearestRoute/FindHintNode/Ps2Unk127 quebram).

Diferenças do PS2 e truques:
- `#define DLLS_MONSTERS_CPP` antes dos includes torna Set/Clear/Has/HasAllConditions inline na classe (basemonster.h); em outros arquivos ficam declarações simples. Cópias fora de linha dos inlines do CBaseMonster saem no fim do .text (0x1A8A40–0x1A8D88) em ordem de declaração.
- cvar triangulate em 0x30FE98 (`extern "C" ps2cvar_t D_0030FE98`, value em +0xC): BuildRoute/Move só triangulam se `!Classify() || CLASS_PLAYER_ALLY || CLASS_HUMAN_PASSIVE || D_0030FE98.value > 0`.
- BuildNodeRoute novo (0x1A41F8, nós antes de triangular); FRefreshRoute(int) escolhe entre ele e BuildRoute; FGetNodeRoute(Vector, int iSrc=0, int iDest=0).
- IRelationship: tabela iEnemy 15x15 na .data 0x314BC0. BestVisibleEnemy pula jogadores se spawnflags & 0x2000. DropItem cria sem dono. FindCover limita altura ±64 e 3 tentativas.
- Ps2Unk127 (virtual extra): procura info_aievent num raio de 96; estado (int +0x2D4) em 3..4 aciona alvo (+0x2E8) e guarda em m_hAIEventTarget (0x294); senão zera byte +0x2EC e m_hAIEventPs2 (0x28C). Casou com `if (s<5) { if (s>=3) {...return;} }` seguido do reset único (cross-jump).
- Pilha a mais = locais Vector sem uso (vecBestOnLeft/Right em FindLateralCover tinham de sair).
- Nomes novos no symbol_addrs: LookupActivityHeaviest 0x12FFE0, SetSequenceBox 0x130E70, GetEyePosition__FPvPf 0x1312F8, DispatchBlocked 0x145340, activity_map 0x30DF80.
- Registro func_001A89C8: "188" CallMonsterThink, "189" CorpseFallThink, "190" MonsterInitThink, "191" MonsterUse.
