---
name: hl1ps2-matching-lessons-10
description: Lições do common.c (checagem redundante do QW, strlen unsigned, rótulos e put.py, sed &)
metadata:
  type: feedback
---
Continua [[hl1ps2-matching-lessons-9]].

- Info_* do QW: o laço do valor `while (*s != '\\' && *s) { if (!*s) return ""; *o++ = *s++; }` precisa do `if (!*s)` redundante para casar (sem ele o agendamento do laço muda).
- `strlen(x) > 63` que vira sltiu: o original tratava strlen como unsigned; com o protótipo `int strlen` do arquivo usar `(unsigned int)strlen(x)`.
- abs.s: usar `__builtin_fabsf(x)` (fabs sem protótipo dá aviso e código errado).
- Laço que guarda uma constante (20) em registrador s* antes do laço = arquivo com -fmove-all-movables; testar a flag no arquivo inteiro com tools/chkall.sh (common.c casou tudo com ela).
- put.py quebra blocos em "\n\n(?=\S)": rótulos na coluna 0 (`skipwhite:`) partem a função; indentar rótulos ou trocar a região à mão.
- sed: `&` na substituição é o texto casado (corrompe `&=`); para editar código use python.

- Global (ponteiro) que NÃO é recarregado depois de um store por ponteiro = o store era em campo de struct/array (MEM_IN_STRUCT; heurística "escalar fixo vs struct variável" do gcc 2.95). Ex.: `hdr[c][0] = x` recarregava; `hdr[c]->addr = x` (struct) casou. `*p = x` simples força recarga.
- `n*0x810` virando sll 11 + sll 4 + addu: escrever `(n << 11) + (n << 4)`; com `*` sai mult.
- Contador de laço em t0 em vez de s*: o original usava outra variável para o segundo laço (não reutilizar `i`).
- Dois blocos iguais de código no asm (ex.: half++ duplicado) = o C tinha o código repetido em if/else, o gcc não duplica sozinho.

**Why:** cada um custou várias tentativas no common.c.
**How to apply:** tentar esses ajustes primeiro em funções Info_*/strlen/fabs e em laços com constantes içadas.
