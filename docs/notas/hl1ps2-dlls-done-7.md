---
name: hl1ps2-dlls-done-7
description: continuação da lista de .cpp das dlls feitos (gamerules em diante) e próximo arquivo
metadata:
  type: project
---

Continuação de [[hl1ps2-dlls-done-6]]. Formato: arquivo (início .text, commit, funções que casam).

- gamerules (0x16F258, 20e74b9, 28/28): .data só o g_pGameRules (0x31298C, 4 bytes: OBJCOPY_EXTRA .data=4 no Makefile), .rodata 0x350218 (vtable 0x350A30). gamerules.h ganhou DLLS_GAMERULES_CPP: GR_PURE (virtuais __pure_virtual na vtable do PS2) e GR_BODY (corpos inline do HLSDK; o gcc emite no fim, na ordem da vtable, e as strings ""/"Half-Life" saem no começo). Classe local do coop só com GetGameDescription "HL Coopplay". RefreshSkillData usa skill 2 se IsMultiplayer; plrDmgHornet = 7 antes do sk_suitcharger. InstallGameRules: coop > 0 → func_00152E20(operator new(0x5C)); sem g_teamplay. Ps2Virt63(index,a2,a3) chama func_001D0E60 no jogador.

Próximo: game.cpp (GameDLLInit = func_0016D4E8 gigante; .text termina em 0x16F258; .data termina em 0x31298C; .rodata termina em 0x350218, "exec skill.cfg" em 0x350208).
