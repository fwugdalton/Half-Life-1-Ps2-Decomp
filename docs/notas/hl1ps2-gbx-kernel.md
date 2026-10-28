---
name: hl1ps2-gbx-kernel
description: Gbx/Kernel.c em C (faixas, 14/15 casando) e lições de matching dele; próximos blocos Gbx
metadata:
  type: project
  modified: 2026-10-13T18:23:52.858Z
---

Gbx/Kernel.c (commit 691770c, 2026-10-13): src/gbx/Kernel.c, text 0x229720..0x229CF0, rodata 0x386C60..0x386EA8, data 0x3281E8..0x3281F8 (dois unsigned long = 0).
- Funções: boot do IOP + IRX (229720), abrir/fechar/ler/seek de arquivos host0: (KFILE {fd,size,name}), timer pelo COP0 Count (mfc0 via __asm__ volatile), espera ativa.
- func_00229CB0 (laço do SifLoadModule) NON_MATCHING: só troca s0/s1; a versão com goto retry chega mais perto.
- __udivdi3 = 0x2DE518 adicionado ao symbol_addrs.

Lições:
- Para retorno em registro salvo que reaproveita o do parâmetro: `if (x == 1) { r = 0; call(f); } else { r = 1; } return r;` casou; outras ordens não.
- `r = open(); if (r >= 0) { fd = r; ... } else fd = -1;` faz o bltz usar v0 com a cópia no delay slot.
- `return (b < a) ? b + max - a : b - a;` com `unsigned long max = 0xFFFFFFFF;` em variável evita o gcc dobrar a constante.
- q * 0x48000: escrever `(q << 18) + (q << 15)` explícito.
- String "" alinhada em 8 no fim da rodata: definir `const char D_X[8] = "";` (INCLUDE_RODATA alinha só em 4).
- Ferramenta de rascunho: scratchpad/try.py arquivo.c função variante.c [-k] troca o corpo e roda cmpfunc.

Próximo: Gbx C++ (KeyStateManager.cpp em 0x229CF0, classe hash em 0x229378) precisa de regra .cpp no Makefile; StreamZip.c (string na rodata 0x388708, função 0x238418). Ver [[hl1ps2-zlib-inflate]].
