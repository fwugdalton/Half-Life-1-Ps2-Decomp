---
name: hl1ps2-matching-lessons-2
description: Mais lições de matching do ee-gcc 2.95 no decomp do HL1 PS2 (pilha, laços, retorno de struct, ordem de stores)
metadata:
  type: project
---
- Pilha: agregados (structs/arrays) ganham slot na declaração; escalares com & ganham slot depois (ADDRESSOF), em ordem de declaração. Struct declarada num bloco interno vai depois dos escalares (SV_WritePlayersToClient).
- Constante 0xFFFFFFFF em registrador salvo (lui/ori no prólogo) = variável local `unsigned int x = 0xFFFFFFFF` (SV_ConnectClient).
- Layout [teste A→blocoA][teste B→fim][b blocoB][blocoA][blocoB] vem de `if (A || B) { if (A) {...} else {...} }`.
- Strings mortas (código `if (0)`) ainda são emitidas na .rodata na ordem do fonte; string compartilhada usada primeiro pela função nova → as funções seguintes usam o literal (gcc reaproveita o rótulo), não extern.
- ee-gcc 2.95: `while (1)` e `for (;;)` geram laços diferentes (for(;;) roda o otimizador de laço e iça constantes/lui; while(1) não). Se o original iça só parte, nenhum dos dois casa direto (Hunk_Print).
- Laço que libera lista: `if (list) { p = list; do { tmp = p; ...; p = p->next; Mem_Free(tmp); } while (p); }` casou Hunk_PrintDetails.
- `x = K; if (y > x) x = y;` (comparando com a variável recém-gravada) gera li+slt; `if (y > K)` gera slti.
- Retorno cedo `if (frac == 0) return clip.trace;` muda onde v0 é setado em funções que retornam struct (SV_Move).
- `fabs`/`max` como macro ternária geram abs.s/max.s com -ffast-math; `(flags & FL_X) > 0` gera and+sltu.
- Atribuição encadeada `a = b = x` grava b primeiro; ordem de stores rotacionada: testar permutações (tools/trybody.py com variantes geradas).
- `sv.models[...]` dentro de laço: o original recalcula sv+0x198E8 a cada volta e o meu iça; ficou NON_MATCHING (SV_TouchLinks, SV_LinkContents).

Anterior: [[hl1ps2-matching-lessons]].
