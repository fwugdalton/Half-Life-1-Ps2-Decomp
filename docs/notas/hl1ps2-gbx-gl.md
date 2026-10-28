---
name: hl1ps2-gbx-gl
description: Cheats.c, gl.c e Texture.cpp do Gbx (faixas, rodata, vu0words.py, cmpfunc rígido) e lições do gl
metadata:
  type: project
  modified: 2026-10-19T21:00:00.000Z
---

Depois do [[hl1ps2-gbx-cd-iop]] (2026-10-19):
- Cheats.c 0x239C80..0x239D88, 4/4. A string "1.0" em 0x388938 é dele (cvar), fica na cod/288838.
- GsEnv.c 0x239D88..0x23AA00 (ambientes do GS com bitfields da libgraph). gl.c 0x23AA00..0x23EFD0: o .text dele é alinhado a 256 por SECTALIGN_C no Makefile (o fim do GsEnv é preenchido com nops). Tipos em include/gbx/gl.h. Rodata do gl 0x388940..0x388A08 (alinhada a 16, tabelas de salto com `.align 4`). Texture.cpp 0x23EFD0..0x241578 (rodata 0x388A08), em `extern "C" {}`.
- tools/vu0words.py roda no `make split`: troca instruções VU0 macro por `.word` (o gas não aceita a sintaxe do splat).
- tools/cmpfunc.py agora confere o endereço de %hi/%lo quando o símbolo é D_/func_/jtbl_ ou está em symbol_addrs. Antes, stores em globais trocadas davam "OK" falso. tools/romcheck.py compara a rom com os bytes do asm.
- Lições do gl: ordem dos cases = ordem dos corpos; `case 0xC11: break;` no fim. Um store final sem delay slot preenchido (`sw; jr; nop`) casou com a global `volatile`. Ordem de stores: permutar com try.py (script $S/gl/perm.py).
- Mais lições: cópias com lq/sq intercalados e blocos do VU0 eram asm inline (macros QCOPY/MCOPY/MIDENTITY/MMUL no gl.h; o gas aceita os mnemônicos do VU0, menos o campo do vsqrt: usar .word). Valores calculados nos dois ramos de um if com cauda igual (o gcc junta as caudas): escrever o código duplicado nos dois ramos. Ordem de stores de muitas globais: tentar a ordem dos endereços. try.py agora para com "XX ERRO DE BUILD" se a compilação falhar (antes mostrava OK falso com o .o velho).
- Switch de 3 cases com a cadeia beq ==1; nop; beq ==0; beq ==2 (sem slti): `switch ((unsigned int)mode)` casou (ECF8, EE68).
- gl.c terminado (2026-10-19): só AA18 e AB00 (escritas à mão) ficam em asm; 16 funções NON_MATCHING (pacotes do VIF D8B8..E830, glInit AB28, FPS ADB8, C880, CE98...). O falso func_0023C7B0 era o caso 0x2804 do glTexParameterf (C600): o C600 vai até 0x23C7C8.
- `sync.l; nop` antes de um jal: usar a versão com `.set noreorder/.set reorder` para o gas não mover o nop (C880, CD48).
- Muitos stores em globais (CBC8): ordem dos endereços casou de primeira.
- NUNCA usar `git checkout arquivo` com trabalho não commitado (perdi trabalho uma vez; refiz com os arquivos do scratchpad).
- Texture.cpp terminado (2026-10-19): tudo em C; NON_MATCHING 0500, FAB0, 07F0, EFD0, FC60, F420 (alocador de VRAM), FE10 (pacote TEX0/TEX1). A rotina 0x241500 (ppacb, escrita à mão) foi para TexConv.c (.text alinhado a 256 no Makefile) para manter os nops de preenchimento.
- Texture: stores em globais casaram na ordem dos endereços (0D60: D_003A9D70, D74, D78[0..5]); 09F8 casou com `p = start;` entre os stores. Comparação de cache com `(short)global != c[20]` gera lh.
