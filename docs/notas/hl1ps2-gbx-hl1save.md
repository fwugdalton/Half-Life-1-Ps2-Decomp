---
name: hl1ps2-gbx-hl1save
description: bloco ex-cod/11BC78 (Gbx/HL1Input, HL1Compress, HL1Decompress, HL1Read, engine/fs_stub): faixas, estado, NON_MATCHING e truques
metadata:
  type: project
  modified: 2026-10-20T00:59:14.508Z
---

Feito em 2026-10-20 (depois de [[hl1ps2-pad-compress]]):
- Faixas: HL1Input.cpp 0x11BC78, HL1Compress.cpp 0x11D8A8 (rodata 0x2859B0), HL1Decompress.cpp 0x11F180 (0x2859F8), HL1Read.cpp 0x121470 (0x285B70), engine/fs_stub.c 0x1224B8 (9/9).
- Estado: HL1Input quase todo (NON_MATCHING 21C520, 21BFC0; INCLUDE_ASM nos init estáticos 21D7A8/21D868/21D888). HL1Compress 16/17 (21E880, o codificador de 0x900 bytes, ainda INCLUDE_ASM). HL1Decompress todo em C (NON_MATCHING 21F180, 20748, 20B10). HL1Read 8/9 (NON_MATCHING 21DF0) e precisa de -fmove-all-movables no Makefile.
- Structs do save (0xD8 bytes, magic 'VALV' 0x564C4156): SaveData com header SAVE_HEADER em 0x34, SAVECONNECTION (0x50: mapName[32], landmarkName[32], int @0x40, float[3] @0x44), SAVELIGHTSTYLE (0x44), SaveRecord (16 bytes: short token, char *classname @4, u8 count @8, SaveEntry *entries @0xC), SaveEntry (12 bytes: short id, short len, void *data, int owned).
- Funções: Q_memcpy 0x225820, func_00225860 = memset, func_0022BE50 = fread, func_0022BD98 = fwrite, func_0022B118 = fopen, func_0022B238 = fclose, func_00228E00 = strcmp.

Truques:
- **Declarar Q_memcpy e memset como `void *` (não `void`) consertou v0/v1 trocados** no teste de entrada de laços logo depois das chamadas (mesma lição do ponteiro de função em [[hl1ps2-compiler-flags]]).
- Pilha em C++: os escalares com & ficam na ordem de declaração (`short len; short token; int fields;` = sp+0/2/4). Para pôr um escalar antes de um struct local, declarar o struct num bloco interno (ex.: `SaveEntry e` dentro do while).
- Switch pequeno (até 4 cases, árvore de beq): o ÚLTIMO case do fonte não é fundido no memcpy comum e fica no fim do código. Usar isso para escolher a ordem.
- `records[i].campo` direto, sem ponteiro local, casou (o gcc iça &records[i] para s0 no laço interno). Em outras funções o ponteiro local casou melhor; testar os dois.
- Retorno único: `if (f != 0) {...} else r = 0; return r;` e `if (magic != X) r = 0; else {...; r = 1;}`.
- Tabelas de salto compiladas são alinhadas a 16. Converter uma função com jtbl e deixar a seguinte em INCLUDE_ASM pode quebrar a ROM: o .s seguinte tem .align 3. Converter as duas.
- O teste do laço externo de 20B10 foi fundido por crossjump com o do fim. Não achei a forma que evita isso, então ficou NON_MATCHING.
