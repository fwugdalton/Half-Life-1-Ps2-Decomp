---
name: hl1ps2-dlls-done-7
description: continuação da lista de .cpp das dlls feitos (gamerules em diante) e próximo arquivo
metadata:
  type: project
---

Continuação de [[hl1ps2-dlls-done-6]]. Formato: arquivo (início .text, commit, funções que casam).

- gamerules (0x16F258, 20e74b9, 28/28): .data só o g_pGameRules (0x31298C, 4 bytes: OBJCOPY_EXTRA .data=4 no Makefile), .rodata 0x350218 (vtable 0x350A30). gamerules.h ganhou DLLS_GAMERULES_CPP: GR_PURE (virtuais __pure_virtual na vtable do PS2) e GR_BODY (corpos inline do HLSDK; o gcc emite no fim, na ordem da vtable, e as strings ""/"Half-Life" saem no começo). Classe local do coop só com GetGameDescription "HL Coopplay". RefreshSkillData usa skill 2 se IsMultiplayer; plrDmgHornet = 7 antes do sk_suitcharger. InstallGameRules: coop > 0 → func_00152E20(operator new(0x5C)); sem g_teamplay. Ps2Virt63(index,a2,a3) chama func_001D0E60 no jogador.

- game (0x16D4E8, 10a0a09, 0/1 + dados): os 462 cvars (.data 0x30FE38–0x312984, um por 0x18) e as strings (.rodata 0x34D738) casam; gerados por script a partir da ROM (nome da variável = nome do cvar sem "mp_"). Não incluir cbase.h (ele traz uma string "" que a ROM não tem). g_psv_gravity/g_psv_aim (0x312984/88) definidos antes do INCLUDE_ASM (o .s volta para .text e o que vem depois cai na seção errada). GameDLLInit NON_MATCHING: o gcc espalha o &g_engfuncs por s0-s3 e não iça os g_psv_* (flags não ajudam). Todos os D_ dos cvars citados no asm/C foram para o linker_script_extra.ld.

Próximo: func_tank.cpp (.text termina em 0x16D4E8, static init 0x16D4C8; .data termina em 0x30FE38; .rodata termina em 0x34D738, "func_tankcontrols" antes do "classname").
