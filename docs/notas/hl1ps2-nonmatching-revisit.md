---
name: hl1ps2-nonmatching-revisit
description: como revisitar funções NON_MATCHING em lote (scan com -DNON_MATCHING, fila do permuter) e o que já casou assim
metadata:
  type: project
  modified: 2026-11-03T14:28:30.230Z
---
Desde 2026-11-03 o código do jogo está todo em C/C++ (sobram em asm só o crt0 e as bibliotecas da Sony, que não se descompilam). O trabalho agora é reduzir as ~320 NON_MATCHING.
- Levantamento: compilar cada arquivo com -DNON_MATCHING (pegar o comando do `make -n -W src/x.c build/src/x.o`) e rodar tools/cmpfunc.py com pares nome=endereço tirados dos .s de asm/nonmatchings (linha seguinte ao glabel). Filtrar só nomes que aparecem em INCLUDE_ASM depois de `#else` (o cmpfunc também lista funções já casadas).
- permuter (só C, não C++): tools/permuter_setup.py src/x.c Func dir; rodar 4 em paralelo (4 núcleos) com timeout 600 s e --stop-on-zero. Em 42 funções de até 12 instruções de diferença, 14 chegaram a 0. Aceitar só mudanças naturais; recusar volatile, `&ptr` e índices long long.
- Lição: func_00222500 (FS_FPrintf) devolve int. Declarar como void mudava a alocação de registradores depois da chamada (Cache_Print, Hunk_PrintDetails, Cache_Print_Sounds_And_Totals, func_002CB7B0, Mod_Report_f passaram a casar).
- Lição: o DI() do Iop.c casa como bloco `{ ... }`, sem o `do { } while (0)` por fora.
- Lição: V_UpdatePalette casou com `frac = cshift_color[3]; a = frac * 255; ... frac = 1 - frac;`.
- Lição: func_002C9C98 casou com `size * 2048 + (x % 2048 - 2048)`, e R_BlendLightmaps com `tex = lightmap_textures;` lido antes do base.
- Cuidado: `pkill -f permuter.py` mata o próprio shell se a linha de comando contiver o texto.
Commits 2e732cf e 9214e38. Ver [[hl1ps2-dlls-aiplayer]].
