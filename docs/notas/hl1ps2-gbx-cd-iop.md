---
name: hl1ps2-gbx-cd-iop
description: Arquivos Gbx de 0x2382E8 a 0x239C80 (StreamZip, AsyncRead, Iop, CdAudio, MethodPointerTable), lições e próximo arquivo
metadata:
  type: project
---

Continuação de [[hl1ps2-gbx-save-sound]] (CDRead.cpp vai até 0x238328 e agora inclui func_002382E8), 2026-10-18:
- StreamZip.c 0x238328..0x2387D0 (rodata 0x388708), 6/7. 238418 NON_MATCHING (mode/second trocam s4/s5). Usa -fmove-all-movables (Makefile). `switch` com teste ==1, ==0, ==2 sem slti: o mode é `unsigned int`.
- AsyncRead.c 0x2387D0..0x238F50, 4/9: 8 pedidos de 0x1C em D_003A9290. Os laços do alvo às vezes não são girados nem reduzidos; não achei a forma. Laço com índice sai `slt` de ponteiro; laço com ponteiro sai `sltu`.
- Iop.c 0x238F50..0x239318 (rodata "" em 0x388728), 7/10. DI() com `__asm__ volatile("di")`, "sync.p" e "mfc0 %0, $12" compila certo (o nop depois do mfc0 sai sozinho). func_00238FC8 = sceSifAddCmdHandler(cmd, handler, arg).
- CdAudio.c 0x239318..0x2397D0 (rodata 0x388730), 8/8: comando "cd" (play/loop/playfile/stop/pause/gbxpause/resume), RPC 400..403.
- MethodPointerTable.cpp 0x2397D0..0x239C80 (rodata 0x3887F0..0x388838), 14/15. A vtable D_00388810 fica em INCLUDE_RODATA depois da função que usa a string, com `.balign 8` antes. 239870 chama os 69 inits dos arquivos do jogo.
- Ordem de stores: o gcc costuma inverter; testar com declperm (scratchpad/declperm2.py aceita .c e CC_SH=sz.sh).
- Próximo: cod/139C80 (cvars de cheats em 239C80, rodata 0x388838 "GravityCheat"...), depois Texture.cpp (rodata 0x388A08) e o GL do PS2 (glBegin etc.).
