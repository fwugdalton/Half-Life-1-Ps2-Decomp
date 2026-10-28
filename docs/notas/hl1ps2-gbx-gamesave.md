---
name: hl1ps2-gbx-gamesave
description: Gbx/GameSave.cpp (structs de save, lições de matching) e o próximo arquivo a partir de 0x2307D8
metadata:
  type: project
---

GameSave.cpp 0x22E860..0x2307D8, rodata 0x387428..0x387660, .data 0x328270..0x328310 (cvars Slot1/Slot2Available no topo). Commit 4c18e84 (2026-10-18).
- Estado global D_003A76D8 (GameSaveState): card, status, numSaves, unk0C (capacidade), saves (GameSaveEntry 0x11C: used, size, group, unk0C[0x20] nome do arquivo, dir[0x44] em 0x2C, hdr 0x5C em 0x70, name[0x50] em 0xCC), unk14 (nº de grupos), unk18 (capacidade), groups (GameSaveGroup 0x30: loaded, unk04, dir[0x20], unk28, refs), unk20 (espaço livre).
- GameSaveHeader 0x5C: unk00 (99999 = vazio), unk04[4] ('c'), chapter, unk0C[0x50].
- func_002307A8(slot) mapeia slot 1→porta 0; quem chama recebe `slot` em a0 e repassa (o asm não mexe em a0 antes do jal).
- Realloc com arquivo/linha: func_002260D0(ptr, size, "Gbx/GameSave.cpp", line).

Lições:
- Laço "salta para o teste" (b teste; i++; teste:) casou com `for (...) { if (x) { ret = 1; break; } ret = 0; }` e também com `for { if (f()==1) { found = 1; break; } }`.
- `max = 8 - strlen(x); max = (int)powf(10, max);` (mesma variável) casou; com duas variáveis não.
- try.py agora aceita função ainda em INCLUDE_ASM (troca a linha) e só casa definições no início da linha.
- -fmove-all-movables piorou F9D8 e não ajudou F4B0; GameSave fica sem flag.

Próximo: a partir de 0x2307D8 (cod/1307D8 no yaml): MapToChapter.txt (rodata 0x387660), formatação de hora/data, depois funções 0x230F80..0x2311D0 (objeto de 0x20 bytes usado na gravação), SPUMemory (~0x231FD0, string 0x387790), HLPak, GlobalPak, ExtraPrecache, CDRead, MethodPointerTable, Texture.
