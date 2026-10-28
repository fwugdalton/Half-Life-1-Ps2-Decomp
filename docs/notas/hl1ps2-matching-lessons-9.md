---
name: hl1ps2-matching-lessons-9
description: lições do cl_ents.c (memcpy 4/2 inline, lw sobre dois shorts, cadeia de stores, valor reutilizado em registrador)
metadata:
  type: project
---
Anterior: [[hl1ps2-matching-lessons-8]].
- lw/sw de 4 bytes e lhu/sh de 2 bytes copiando byte arrays (controller/blending) = `memcpy(dst, src, 4)` / `memcpy(dst, src, 2)` (o gcc inline com alinhamento conhecido).
- lw em 0x34 de entity_state_t (dois shorts skin/solid) comparado com 0xFFF0 = `*(int *)&state->skin == 0xFFF0` no original.
- `pent = &arr[n]; n++;` em vez de `&arr[n++]` mudou os registradores temporários (casou 24BD88).
- Store-to-load: `VectorSubtract(state->origin, ...)` logo após `VectorCopy(origin, state->origin)` reaproveita os registradores; com o array local o gcc relê da pilha.
- Um `if (a || b) { if (a) A; else B; } else C;` gera os testes seguidos e os blocos A, B, C depois (padrão de modelindex/weaponmodel do CL_ParsePlayerinfo).
- Valor lido uma vez para dois campos (animtime/prevanimtime) = `x->a = x->b = expr;`; constante float hoisted antes de laço = variável local (`frac = cl.lerpfrac`).
- $S/opd.py obj func: diff só de opcodes (normaliza registradores, relocs e alvos de desvio); bom para achar diferenças estruturais em funções grandes.
- Chamada por ponteiro cuja regalloc evita a0..a2 = a função recebe mais argumentos repassados (ex.: Redraw recebia clientId e um 4º arg); conferir os registradores preservados antes do jalr.
- Dois `switch` com case 0/1/default (hudServerCmd "cmd "/"cmd2 ") geram `beqz` + `bne 1` com rótulo compartilhado; `if/else` não casa.
