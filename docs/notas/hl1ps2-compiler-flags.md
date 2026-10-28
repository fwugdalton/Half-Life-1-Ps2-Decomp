---
name: hl1ps2-compiler-flags
description: Flags de compilação por arquivo do decomp HL1 PS2 (-ffast-math, -fmove-all-movables) e como descobrir qual usar
metadata:
  type: project
---
- Base: `-O2 -G0`. Arquivos de matemática/física usam também `-ffast-math` (linha `$(BUILD_DIR)/src/engine/X.o: CFLAGS += ...` no Makefile).
- **-fmove-all-movables por arquivo** (descoberto 2026-10-11): quando o original iça constantes (2, 5, 0x8420, 80...) para registradores s* antes de um laço e o meu código não, o arquivo foi compilado com `-fmove-all-movables`. Já ligado no Makefile para Host, Host_cmd, Gl_model, cmodel, sv_move, zone, gl_warp, gl_rlight, r_part, r_studio e common (common.c: 2026-10-12) (r_studio confirmado em 2026-10-12: o endereço pbone+0x40 e o -1 içados no laço de R_StudioCalcBonePosition). Em Console, sv_main, world, pr_cmds e sv_phys a flag quebra funções que casam, então lá não usar. **max.s/min.s no asm = arquivo com -ffast-math** (r_studio também usa -ffast-math, 2026-10-12; com a flag, `if (x > 2) return 2;` dentro de um if precisa de retorno compartilhado no fim). Testar compilando o .o com `make build/src/engine/X.o CFLAGS="-O2 -G0 -Iinclude -Isrc -DNON_MATCHING -ffast-math -fmove-all-movables"` (CFLAGS na linha de comando anula os += do Makefile, por isso repetir -ffast-math) e rodar cmpfunc. Mudar o Makefile não refaz o .o: dar `touch` no .c antes de testar uma flag (senão todas as flags parecem iguais).
- Ao tirar um INCLUDE_ASM que carregava strings compartilhadas, conferir se outra função ainda usa o rótulo D_ via extern (Host.c: STR_S e STR_ADR_PORT) e se a .rodata do arquivo tem entrada própria no yaml (cmodel não tinha; adicionei 0x28B538).

Ver [[hl1ps2-matching-lessons-3]] e [[hl1ps2-file-status-2]].
- cl_tent.c (2026-10-12): só `-ffast-math` (M_PI/180 vira 0x3C8EFA36 só com fast-math). Laços que içam TODAS as constantes de uso único (fizz, bubbletrail, sprite_spray) só casam com -fmove-all-movables, mas outras funções do mesmo arquivo içam só parte (heurística normal do loop.c: içar se threshold*savings*life >= insns do laço; threshold do EE ~55-66). Ver números com `ee-gcc ... -dL` (arquivo .loop: "life N ... not desirable"). Não achei a forma de C que dá life 2; ficaram NON_MATCHING.
