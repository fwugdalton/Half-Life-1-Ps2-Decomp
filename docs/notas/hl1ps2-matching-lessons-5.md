---
name: hl1ps2-matching-lessons-5
description: lições do r_part2.c (feixes): trybody com protótipo/NON_MATCHING, tb2.py, variável de bloco, ordem de argumentos HL
metadata:
  type: feedback
  modified: 2026-10-12T00:10:03.916Z
---
Lições do r_part2.c (beams, 2026-10-12). Continua [[hl1ps2-matching-lessons-4]].

- tools/trybody.py acha a PRIMEIRA ocorrência de `Func(`: se houver protótipo antes, ele troca o lugar errado (erro "previous declaration"). E numa função já embrulhada em NON_MATCHING ele dá "OK 0" falso, porque compila o INCLUDE_ASM. Usar /tmp/claude-0/tb2.py (procura a definição `...Func(...)\n{` e roda `make build/...o` + difffunc). Se o /tmp sumir, recriar: regex `\n[^\n;]*\bFunc\([^;{]*\)\n\{`, troca até `\n}\n`, compila, conta ` * `.
- Ponteiro de retorno usado só num bloco (VectorCopy(R_GetAttachmentPoint(...), dst)): declarar `float *attachment = ...;` DENTRO do bloco if; no topo da função ele vai para outro registrador (R_BeamDraw: 309 → 29 diffs).
- VectorCopy é macro: com chamada dentro, chama 3 vezes. Sempre usar temporário.
- Ordem de carga dos argumentos na chamada segue a ordem dos parâmetros (avaliação da direita p/ esquerda): usar a assinatura original do HL (R_DrawSegs(source, delta, width, scale, freq, speed, segments, flags)), mesmo que no EABI os regs sejam os mesmos.
- Índice reaproveitado depois de uma chamada (`cl_entities[c][idx].model` relido): guardar `idx = BEAMENT_ENTITY(x)` numa variável.
- `fraction = i * div * 2 * M_PI` numa só expressão (R_DrawCylinder) vs `fraction = i*div; ... fraction*2*M_PI` mudam o registrador.
- `frametime` separado vs reaproveitar `fraction` para `cl.time - cl.oldtime` no fim (R_DrawBeamFollow casou reaproveitando a variável).
- `D_00399534++; D_00399598 += 2;` aparecem com stores em ordem inversa à do fonte; testar as duas.
- Divisão inteira por 8 com o resultado reaproveitado: o original guarda o valor ajustado (seg+7) e refaz o sra.
- switch com `case 0: break;` vazio muda a árvore de decisão (R_BeamDrawList).
- Laço com teste no topo e endereços içados (R_DrawBeams): nem while, for(;;), while(1) nem goto reproduziram; ficou NON_MATCHING.
