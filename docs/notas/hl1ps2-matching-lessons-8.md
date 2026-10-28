---
name: hl1ps2-matching-lessons-8
description: lições do cl_tent.c (ordem de locais na pilha, constantes 0.01, unsigned→float, return 0 único, permute.py)
metadata:
  type: project
---
Anterior: [[hl1ps2-matching-lessons-7]].
- Locais na pilha recebem endereços crescentes na ordem de declaração (vec3_t pos @0x10, tmp @0x20, ...). Floats soltos que caem em 0x20/0x24 = elementos de um array declarado, não variáveis separadas.
- Ordem de stores: cadeia `b = g = r = 255` grava na ordem r, g, b (casou 258410).
- `scale * 0.1f * 0.1f` dá a constante 0x3C23D70B; `/100.0f` dá 0x3C23D70A (0.01f).
- Padrão bltz/srl/andi/or/cvt/add.s = conversão unsigned int → float: `(unsigned int)MSG_ReadByte() * 0.1f`.
- Um único label `return 0` compartilhado = `if (cond) { ...; return 1; } return 0;`. `if (x < 9) { if (x >= 5) return 1; } return 0;` casou 25C5A8.
- Variável com `daddu sN,zero,zero` logo no prólogo, usada só em alguns ramos de um switch = declarada com `= 0` no topo (startEnt/endEnt do CL_ParseTEnt).
- Uma store bruta que some depois de `*= k` (spread[1]) mas a outra fica (spread[0]): a primeira foi antes de uma chamada, a segunda o gcc eliminou.
- tools/permute.py: rodar com timeout longo do Bash, nunca com `timeout` (SIGTERM deixa o arquivo permutado). Não rodar $S/ck.sh enquanto o permute roda (ele apaga o .o). Não usar `pkill -f` com padrão que case com o próprio shell.
- Içamento de constantes de laço: no cl_tent nem -fmove-all-movables nem a ausência serve para o arquivo todo; deixar só -ffast-math e marcar NON_MATCHING as afetadas. Ver [[hl1ps2-compiler-flags]].
- tools/fixasm.py agora expande `li.d $gpr, X` sem forma curta como o ee-as: constante double na .rodata (`.pushsection`) + `lui $at; ld`. Ex.: `atan2(...) * 180 / M_PI` com -ffast-math vira uma constante (M_PI é float no mathlib.h).
- Switch que retorna 1 para casos 1/2/3: escrever um `return 1;` por case (com `case 1: case 2: case 3:` juntos o gcc gera teste de faixa).
- Cl_main (2026-10-12): `if (x != 1) {A} else {B}` quando o asm faz `beq x,1 -> B` e A cai direto. -1 feito com lui 0xffff/ori = variável unsigned. Valor em sN que vira o dividendo: `n = a - 6; n /= 6;`. Ponteiro de função cujo argumento sai com sll/sra 16 = parâmetro short; andi 0xFFFF no retorno = tipo int convertido depois. Ternário `!f() ? s + 11 : s` gera movn. char data[] com 0xFF = escrever -1.
- $S/rmro.py apaga INCLUDE_RODATA de strings usadas só via extern em C (e por .s de data): conferir ROM e restaurar se o link falhar.
