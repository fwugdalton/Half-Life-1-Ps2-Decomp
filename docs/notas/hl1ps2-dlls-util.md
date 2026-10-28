---
name: hl1ps2-dlls-util
description: dlls/util.cpp (UTIL_*, CSave/CRestore): faixas, flag -fmove-all-movables, diferenças do PS2 e slots do CGameRules
metadata:
  type: project
---
util.cpp feito em 2026-10-20 (commit ae3ba7e): texto 0x206730–0x20B330, .data 0x3243F8 (gEntvarsDescription 86 entradas + gSizes), .rodata 0x3807D0. Só EntityFromIndex é NON_MATCHING (um addu com operandos trocados). Precisa de `-fmove-all-movables` (Makefile) por causa do UTIL_ScreenShake.

Diferenças do PS2 vs HLSDK:
- func_002071C8 = vibração (MESSAGE_BEGIN MSG_ONE gmsgRumble 0x325084, WRITE_CHAR, WRITE_CHAR, WRITE_SHORT).
- UTIL_TraceLineNoEnts (6 e 5 args, 0x207EE0/0x207F18) usam pfnTraceLineNoEnts; antes se chamava UTIL_TraceLine2.
- WriteFunction: Q_memcpy do ponteiro de método e BufferField(pname, 4, func_00239AB0(pfn)) (id em texto "337"). ReadField: FIELD_FUNCTION = func_00239AD8(texto) que retorna long long; leituras desalinhadas com Q_memcpy; FIELD_TIME via func_00236B70; strcpy sem limite no StringToVector; StripToken sem nLen.
- UTIL_TeamsMatch chama o slot 9 do CGameRules (Ps2Virt09) e UTIL_BloodDrips o slot 7 (Ps2Virt07), não IsTeamplay/IsMultiplayer.
- Constantes float: 1.0f, 0.5f, 2.0f senão sai chamada de double.
- buffers static viram extern D_003A42xx (dlls não têm .bss).
Ver [[hl1ps2-dlls]] e [[hl1ps2-dlls-islave]].
