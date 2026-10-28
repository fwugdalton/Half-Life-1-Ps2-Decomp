---
name: hl1ps2-dlls-nodes
description: dlls/nodes.cpp (faixas, mudanças do PS2 no CNode/CGraph, NON_MATCHING, truque das strings compartilhadas) e símbolos achados
metadata:
  type: project
---
nodes.cpp feito em 2026-10-25 (commit 6981ac9). Texto 0x1B36A0–0x1B97E8, .data 0x315240 (WorldGraph; Primes em 0x317310), .rodata 0x367348–0x368350. Header novo include/dlls/nodes.h.

Mudanças do PS2:
- CNode tem m_iRegion (0x54) e um BYTE em 0x5C (copiado no SortNodes); CNodeEnt tem m_iRegion (KeyValue "region" com else-if); CNodeEnt::Spawn copia m_iRegion.
- CGraph ganhou ShowNodePos e SameRegion; FindShortestPath checa região inline.
- FSaveGraph retorna FALSE e CheckNODFile TRUE. BuildNodeGraph: fopen é NULL, então só sobra ALERT + free (sem if) e o buffer tem 816 bytes (ajustado ao quadro).
- CTestHull::Spawn faz ALERT(at_error, "Node graph out of date!\n") e usa Vector(-16,-16,0)/(16,16,72); DropDelay chama UTIL_ClientPrintAll(HUD_PRINTCENTER, ...).
- calloc vira Mem_DbgCalloc com "dlls/nodes.cpp" e as linhas do ROM vêm de diretivas #line.

NON_MATCHING (11): LinkEntForLink, FindShortestPath, CheckNode, FindNearestNode, RejectInlineLinks, FLoadGraph, FSetGraphPointers, BuildRegionTables, ComputeStaticRoutingTables, TestRoutingTables e DrawThink. Causas: CALC_RANGE do ROM converte e divide em outra ordem e com break em cada div; alocação de registradores; e i++ do DrawThink em outra posição.

Truques:
- O gcc 2.95 deduplica strings iguais dentro do arquivo. As strings que o LinkEntForLink divide com o HandleLinkEnt ("func_door", "func_door_rotating") são emitidas por uma função dummy em .nodes_discard antes do INCLUDE_ASM.
- O splat migrou "target" errado; resolvi com `D_00367368 = 0x00367368; // force_not_migration:True` no symbol_addrs.
- Depois do split, apague o .o: o Makefile não depende dos .s em nonmatchings.

Símbolos: VecBModelOrigin 0x13C240, __builtin_vec_new 0x2E21B0, memcmp 0x2E4560, gTouchDisabled 0x30EC6C, g_pBodyQueueHead 0x314030 e UTIL_ClientPrintAll.

Próximo: nihilanth.cpp (texto termina em 0x1B36A0, _GLOBAL_ em 0x1B3680; rodata termina em 0x367348). Ver [[hl1ps2-dlls]] e [[hl1ps2-dlls-osprey]].
