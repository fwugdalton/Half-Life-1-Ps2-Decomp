---
name: hl1ps2-gbx-memorycard
description: Gbx/MemoryCard.cpp (wrappers do libmc) e os truques de C++ que fizeram as 28 funções casarem
metadata:
  type: project
---

MemoryCard.cpp (commit f34b2cb, 2026-10-13), 0x22C448..0x22D600, tudo extern "C" func_XXXX, 28/28 casando.
- MCPort {port, slot} devolvido em v0 64 bits por func_0022D4A8(card). MCInfo {present, type, format, free} (16 bytes, retorno por ponteiro oculto) em D_003A7040[2]. MCFind (0x118) x5 em D_003A7048: free, pattern[0x100], MCPort em 0x104, MCDirEntry* (sceMcTblGetDir, 0x40 bytes, name em 0x20) em 0x10C, count, index. Sync do libmc = func_002D57F8(0, &cmd, &result).

Truques:
- Passar o porto: `MCPort p; p = func_0022D4A8(card); f(p.port, p.slot, ...)` (declaração e atribuição separadas) gera a cópia extra 0x10→0x0 com ldl/sdl. Struct por valor ou `MCPort p = ...` não casam.
- Árvore de switch: casos com o mesmo código viram faixa; para comparar um a um, repetir o corpo em cada case (`case -4: return 0xB; case -5: return 0xB;`).
- Caso 0 sem teste (slti <2 já leva ao valor): `case 0: default:` juntos.
- Caso explícito que devolve o mesmo que o default (case 0: return 3) muda o pivô da árvore.
- Função que reusa o resultado em variável `ret` com um único retorno no fim: escrever if/else aninhado com `ret = ...`.
- Acessos repetidos `D_003A7048[h].campo` (sem ponteiro local) casam com os endereços de campo içados pelo gcc.

Arquivos seguintes (2026-10-18): SaveUtil.cpp 0x22D600..0x22D9B8 (rodata 0x3871B0, commit 7d33913, 8/8) e SystemSave.cpp 0x22D9B8..0x22DFA8 (rodata 0x387218..0x387388, commit 154e1f3; D9B8, DB88, DCC0 NON_MATCHING). Nomes inventados (originais desconhecidos). .data de 0x328250 em diante continua em cod/228250 (extern no C++).
- String compartilhada cuja primeira referência é uma função INCLUDE_ASM: depois de `make split` o splat tira a rodata do .s; emitir a string num `__asm__(".section .rodata ... .globl D_X ... .section .text")` antes da função e usar `extern const char D_X[]` no C; strings referidas só pelo asm vão como absolutas no linker_script_extra.ld.
- Função que recebe 2 args (ex.: func_0022E118(a, b)): conferir no asm do callee antes de supor aridade; um registrador a1 "estranho" no caller costuma ser argumento.
- SystemSaveInitialize.cpp 0x22DFA8..0x22E2E8 (commit 7057f91, E118 NON_MATCHING) e MemCardSlot.cpp 0x22E2E8..0x22E860 (commit 3b348f7, 11/11): os dois usam -fmove-all-movables (constantes 2/7/0xC7FF içadas no laço de 5 tentativas). Laço casou com ifs aninhados (não `continue`); ordem dos cases no fonte = ordem dos blocos no asm; `ret = 4; done = 1;` vs `done = 1; ret = 4;` decide se o gcc funde os blocos.
- GameSave.cpp 0x22E860..0x2307D8 pronto (commit 4c18e84, 2026-10-18): 33 funções em C, 26 casam, 7 NON_MATCHING (F110, F360, F4B0, F8D0, F9D8, FF50, 230178). Detalhes em [[hl1ps2-gbx-gamesave]].
