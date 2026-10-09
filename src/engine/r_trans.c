#include "common.h"
#include "engine/mathlib.h"
#include "engine/model.h"

INCLUDE_ASM("asm/nonmatchings/engine/r_trans", func_00277EB8);

/* strings/constantes compartilhadas por funções ainda em asm (o splat não as leva para os .s) */
__asm__(".section .rodata\n"
        ".align 3\n.globl D_0038E458\nD_0038E458: .asciz \"AddTentity: Too many objects\"\n"
        ".align 3\n.globl D_0038E478\nD_0038E478: .asciz \"Non-sprite set to glow!\\n\"\n"
        ".align 3\n.globl D_0038E498\nD_0038E498: .word 0xFF, 0xFF, 0xFF, 0xFF\n"
        ".section .text\n");

INCLUDE_ASM("asm/nonmatchings/engine/r_trans", func_00277F50);

INCLUDE_ASM("asm/nonmatchings/engine/r_trans", func_00277FB0);

INCLUDE_ASM("asm/nonmatchings/engine/r_trans", func_00278198);

INCLUDE_ASM("asm/nonmatchings/engine/r_trans", func_002781C8);

INCLUDE_ASM("asm/nonmatchings/engine/r_trans", func_00278380);

INCLUDE_ASM("asm/nonmatchings/engine/r_trans", func_00278458);

INCLUDE_ASM("asm/nonmatchings/engine/r_trans", func_00278650);

INCLUDE_ASM("asm/nonmatchings/engine/r_trans", func_002787A8);

INCLUDE_ASM("asm/nonmatchings/engine/r_trans", func_00278988);

INCLUDE_ASM("asm/nonmatchings/engine/r_trans", func_00278AC8);

INCLUDE_ASM("asm/nonmatchings/engine/r_trans", func_00278C78);

INCLUDE_ASM("asm/nonmatchings/engine/r_trans", func_00278D20);

INCLUDE_ASM("asm/nonmatchings/engine/r_trans", func_00278D78);

INCLUDE_ASM("asm/nonmatchings/engine/r_trans", func_00278F28);

INCLUDE_ASM("asm/nonmatchings/engine/r_trans", func_002794C0);

