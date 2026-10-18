/* Gbx/GameSave.cpp -- saves de jogo no memory card */

#include "common.h"

extern "C" {
#include "engine/cvar.h"

cvar_t D_00328270 = { "Slot1Available", "0" };
cvar_t D_00328288 = { "Slot2Available", "0" };
int D_0032829C = 0;
int D_003282A0[2] = { 0, 0 };
char D_003282A8[0x64] = { 0 };
int D_0032830C = 0;
}

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022E860);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022E8D0);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022E930);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022E958);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022E9A8);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387490);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_003874A0);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022EA30);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022EAE0);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022EBA0);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_003874C0);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_003874C8);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022EC50);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022ECC0);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022EDE8);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022F070);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022F0E8);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387540);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387548);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387558);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387560);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387568);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022F110);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387590);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387598);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_003875A0);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_003875B0);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022F360);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022F4B0);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022F598);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022F600);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022F630);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022F740);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022F820);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022F8D0);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022F9D8);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_0022FF50);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_002300D8);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_00230178);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387600);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387608);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387610);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387618);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387620);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387630);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_00230410);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_002304C8);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_002305B8);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_00230618);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_00230678);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_00230710);

INCLUDE_ASM("asm/nonmatchings/gbx/GameSave", func_002307A8);

INCLUDE_RODATA("asm/nonmatchings/gbx/GameSave", D_00387648);
