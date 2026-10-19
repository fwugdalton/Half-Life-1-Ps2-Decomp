/* Gbx/Jade: alocador de memória (membank) com tags */

#include "common.h"

extern "C" {

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", __builtin_new);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", __builtin_delete);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00225A58);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00225AD0);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00225B40);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00225BD8);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", Mem_DbgMalloc);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00225C78);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00225CE0);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00225D48);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00225DB8);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00225DD8);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00225E20);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", Mem_DbgCalloc);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00225EC0);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00225FB0);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_002260D0);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_003861D8);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", Mem_Free);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00226228);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", Mem_SetTag);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", Mem_PushTag);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", Mem_PopTag);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_002262D8);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_002262E0);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00226300);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_00386258);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00226318);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00226410);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00226418);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_002264A0);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00226520);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00226588);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00226600);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_002266D0);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00226788);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_002268C0);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00226930);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00226988);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00226A18);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00226A90);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00226AF0);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00226B60);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00226D00);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00226E68);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_003862C0);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_003862D0);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00226E98);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_00386400);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00226FC0);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_00386470);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_00386480);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_00386490);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_003864A0);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_003864B0);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_00386508);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_00386510);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_00386518);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_00386520);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_00386528);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_00386588);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_003865B8);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_00386620);

INCLUDE_RODATA("asm/nonmatchings/gbx/Jade/Membank", D_00386690);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00227300);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00227408);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00227450);

INCLUDE_ASM("asm/nonmatchings/gbx/Jade/Membank", func_00227490);

}
