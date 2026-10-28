---
name: hl1ps2-gbx-jade
description: bloco cod/1253C8 separado (GbxMain.c, Gbx/Jade Buffer/Membank/SmallMembank/String/HashTable): faixas, estado e truques
metadata:
  type: project
  modified: 2026-10-20T01:00:00.000Z
---

Depois do Texture.cpp ([[hl1ps2-gbx-gl]]), 2026-10-19:
- Texture.cpp terminado; TexConv.c (0x241500, ppacb à mão) separado com .text alinhado a 256.
- O bloco cod/1253C8 virou: GbxMain.c 0x2253C8 (pilha de estados/teclas, 15/15), Jade/Buffer.cpp 0x2257D0 (rodata 0x3861C0, 14/14), Jade/Membank.cpp 0x225A18 (rodata 0x3861D8, 36 casando + 15 NON_MATCHING), Jade/SmallMembank.cpp 0x227498 (rodata 0x3866F0), Jade/String.cpp 0x228BB0 (0x386B28), Jade/HashTable.cpp 0x229378 (0x386BC0..0x386C60).
- scratchpad/gensrc.py gera o .c/.cpp com INCLUDE_ASM/INCLUDE_RODATA na ordem certa a partir do dir do asm (o splat põe a rodata usada por uma só função dentro do .s dela).
- operator new/delete do gcc 2.95 = __builtin_new/__builtin_delete: definir `void *operator new(unsigned int)` fora do extern "C".
- Ao marcar NON_MATCHING funções que usam strings compartilhadas (arquivo D_ separado), pôr INCLUDE_RODATA antes da 1ª usuária e usar `extern const char D_X[]` nas funções em C.
- ctype do newlib: `if (tbl[c] & 4) return true; return false;` casou (o `!= 0` vira srl). tolower: `int r = c; if (...) r = c + 0x20; return r;`.
- Retornos de status (0x81030001 etc.): `if (x == 0) r = A; else if (!flag) r = B; else {...}` casou; o `r` pré-atribuído o gcc sobe para antes do desvio.
- n*size: escrever `total = n * size` em variável nova (senão a ordem do mult inverte).
- scratchpad/nm.py agora só casa definições que começam na coluna 0.
- Bloco Jade terminado (2026-10-19): SmallMembank.cpp 29 casando + 9 NON_MATCHING (cópia do Membank com trava D_003281D4: `if (D_003281D4 == 1) func_002262D8();`), String.cpp 29/29, HashTable.cpp 6 + 3 NON_MATCHING.
- tolower inline: `static inline int ToLower(char c) { int r = c; if (D_00397D81[c] & 1) r = c + 0x20; return r; }` e comparar `ToLower(*a) != ToLower(*b)` direto (sem variáveis) casou.
- Árvore de memoização (func_00229108): um só `return r` no fim, com laço `for(;;)` e `break` em cada caso, casou.
- itoa: `*p = d < 10 ? ... ; p++;` (não `*p++ =`).
- Classe C++ com virtuais (HashTable): escrevi em C com struct + vptr no fim (gcc 2.95 põe vptr após os membros) e chamada `((T(*)(void*,..))t->vptr->e.pfn)((char*)t + t->vptr->e.delta, ...)`; entrada da vtable = {short delta; short index; void *pfn}, a 1ª é RTTI. A vtable fica como INCLUDE_RODATA. func_002DE510 parece __pure_virtual.
- Laços de busca "sem rotação" (teste no topo, bnel para trás): `for (...) if (achou) break;` + `if (n) ... else ...` depois do laço chegou perto; com `return` dentro o gcc rota o laço.
