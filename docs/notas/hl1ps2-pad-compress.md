---
name: hl1ps2-pad-compress
description: zlib/compress.c, engine/compress_dict.c e engine/pad_ps2.c (ex-cod/117420): faixas, structs do libpad e truques que casaram
metadata:
  type: project
---

Feito em 2026-10-20 (depois de [[hl1ps2-gbx-jade]]):
- cod/117420 virou zlib/compress.c (0x117420, compress2/compress, rodata "1.1.3" 0x285088), engine/compress_dict.c (0x117510, rodata 0x285090: arquivos .HL1/.HL4 com dicionários de PAKS\DICTS.PAK, 7/7) e engine/pad_ps2.c (0x1178F8..0x119B20, rodata 0x2850D0, 53/53). Os nomes dos dois últimos foram inventados.
- pad_ps2.c usa o libpad (versão 2130) em 0x2D1xxx: 2D1088 scePadInit, 2D1268 End, 2D12E8 PortOpen, 2D14D0 PortClose, 2D1638 Read, 2D16B8 GetState, 2D1858 InfoAct, 2D1A98 InfoMode, 2D1BD0 SetMainMode, 2D1C88 SetActDirect, 2D1D40 SetActAlign. Os nomes vêm do uso e não foram confirmados por símbolos. Struct Pad de 0xB80 bytes x2 em D_003A4B00: PadState (0x54 bytes, botões em float) em 0x20, histórico de 30 em 0x80, buffer de DMA em 0xA80.
- func_002E4560 = memcmp. func_00229B88 = tick atual (unsigned long), func_00229C38 = diferença.

Truques:
- `r = 0` antes do if é içado pelo gcc para antes das chamadas (fica num registrador s próprio). Para o 0 sair no delay slot do desvio, escrever `r = 0` em cada `else` (if/else if/else com r em todos os ramos). Isso casou em 2 funções.
- Laço sobre os 2 controles: `for (i = 0; i < 2; i++) D[i].x` sai com `slt`; com ponteiro sai `sltu`.
- switch sem slti (==1, ==0, ==2) => o campo testado é `unsigned int` (confirmado de novo).
- Retorno bool de float: `if (sum == 0.0f) return 0; return 1;` (e não `return sum != 0`). Soma longa de floats: `sum = a; sum += b; ...` em linhas separadas.
- Vários `return` antecipados viram blocos próprios; o alvo costuma ter uma variável `ok` e um único return, com o caso "comum" no if (`if (n == 2) {...} else {...}`).
- HLPak func_00235300 casou com o mesmo tolower inline do String.cpp ([[hl1ps2-gbx-jade]]).
