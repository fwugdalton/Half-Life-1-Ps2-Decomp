---
name: hl1ps2-gbx-save-sound
description: SaveInfo, SaveIconTitle, Mouse, GuiSound e SPUMemory (Gbx) — faixas, NON_MATCHING e truques que casaram
metadata:
  type: project
  modified: 2026-10-18T15:27:09.848Z
---

Arquivos Gbx depois de GameSave ([[hl1ps2-gbx-gamesave]]), 2026-10-18:
- SaveInfo.cpp 0x2307D8..0x230F80 (rodata 0x387660). NON_MATCHING 230CE0 e 230E38. A tabela de saltos fica alinhada a 16: `__asm__(".section .rodata\n.balign 16\n.section .text");` antes do INCLUDE_ASM.
- SaveIconTitle.cpp 0x230F80..0x2312D0 (rodata 0x387738). 231070 NON_MATCHING.
- Mouse.cpp 0x2312D0..0x231908. Classes em include/gbx/KeyStateManager.h. 2312D0 e 231338 NON_MATCHING. Os dados de 0x328340.. ficam em asm (extern).
- GuiSound.cpp 0x231908..0x231FD0 (rodata 0x387770), 12/12.
- SPUMemory.cpp 0x231FD0..0x232740 (rodata 0x387790..0x3879D0), 13/15. 232210 e 232540 são NON_MATCHING. Banco D_003A7F98: size, allocated de 64 bits em 8, first, start=0x15040, end, lock. A lista de livres fica em D_00328988.

Truques que casaram:
- sb de 0xFFFFFFFF (lui/ori): usar uma variável local `unsigned int all = 0xFFFFFFFF; p->flags = all;`. Uma constante direta vira li 255.
- Arredondar para 64: `size % 0x40` casou; `size & 0x3F` com variável não casou.
- `return (int)x64;` deixa um nop depois do jr. O ee-as não põe dsll/dsra/dsrl com deslocamento >= 32 (macro) no delay slot. O tools/fixasm.py agora trata isso.
- Uma string "" curta em strcpy vira lbu: tirar o INCLUDE_RODATA dela.
- Permutar ordem de comandos rápido, sem tocar build/: `$S/cc.sh` e `$S/declperm.py` no scratchpad. Eles se perdem ao fim da sessão; recriar se precisar.

Depois do zlib (2026-10-18):
- usbkb.c 0x234B58 e usbmouse.c 0x234D18 (10/10). É o cliente SIF RPC; o atraso é `unsigned int i = 0x10000; while (i--);`.
- HLPak.cpp 0x234ED8..0x235688 (20/21; 235300 NON_MATCHING).
- GlobalPak.cpp ..0x235A50, GamePak.cpp ..0x235E48 e SystemPak.cpp ..0x235F30, todos casando. GlobalPak e GamePak usam -fmove-all-movables.
- Um switch com `case 1: break;` explícito, que faz o mesmo que o default, gera a árvore ==2/<3/==3.
- ExtraPrecache.cpp 0x235F30 (2/3). PauseMenu.cpp 0x2363A0 (7/7). PauseState.cpp 0x236858 (11/12). BinRead.c 0x236B48 (3/3). DiscFail.cpp 0x236BF8..0x236EF0 (3/4).
- Cvar em .data (asm) que aponta para strings da rodata do arquivo: emitir as strings num `__asm__` com .globl D_X no topo do .cpp e usar `extern const char D_X[]` no código.
- CDRead.cpp 0x236EF0..0x2382E8 (rodata 0x3886C0..0x388708), 28/31; 236EF0, 237A80 e 237CE0 são NON_MATCHING. A rodata é "" , "\\", "Gbx/CDRead.cpp" (D_003886D0, via INCLUDE_RODATA depois de 236FD8), "HLADDON.GBX;1" e a tabela do switch.
- Truques do CDRead: um ponteiro `p = &path[i]` e `name = p + 1` acrescenta o registrador salvo que faltava; `off = 0;` antes do CD_Read (fora do for) casou; o limite do laço guardado numa variável `end` evita recarregar; `case 1:` antes de `case 0:` num switch interno muda a ordem dos blocos; `ret = 3; state = 6;` depois do if deixou o gcc compartilhar a cauda.
- Depois do CDRead: ver [[hl1ps2-gbx-cd-iop]].
