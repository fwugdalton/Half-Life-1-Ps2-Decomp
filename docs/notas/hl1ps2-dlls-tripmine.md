---
name: hl1ps2-dlls-tripmine
description: tripmine.cpp (faixas, mudanças do PS2, NON_MATCHING com string do .rodata em asm) e lições
metadata:
  type: project
  modified: 2026-10-20T10:13:07.351Z
---
dlls/tripmine.cpp feito em 2026-10-20 (commit 10b085b), 30/31 funções casam.
- Faixas: texto 0x200ED8–0x202B90 (rom 0x100ED8), .data 0x3241F8 (0x90), .rodata 0x37E8D0 (0x928, sem padding).
- Versão antiga do HLSDK (gpGlobals->time, RANDOM_FLOAT, sem eventos, Holster(void)); WeaponIdle chama m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES) no começo; tempos `90.0f / 30.0f` (o -ffast-math gera 3.0000002).
- PS2: takedamage = DAMAGE_AIM; Touch novo na mina (jogador pisando detona, depois CBaseEntity::Touch inline); Spawn da arma usa g_pGameRules->Ps2Virt08(); PrimaryAttack chama `bool TripmineNearby(Vector)` (por valor, retorna bool: o `xori v0,1` denuncia o bool) e o rumble func_002071C8(m_pPlayer, 1, 195, 28).
- Registro func_00202840: 313 SUB_Remove, 314 DestroyItem, 315 BeamBreakThink, 316 DelayDeathThink, 317 PowerupThink (ordem não é a do arquivo).
- TripmineNearby NON_MATCHING (bnel/bne, ponteiro em a0). Como a função asm usa a string "monster_tripmine", sem NON_MATCHING a string é definida por `__asm__(".section .rodata ... D_0037EA90: .asciz ...")` logo antes do INCLUDE_ASM, e Precache/Create/LINK (escrito à mão com CEntityLink) usam o símbolo via macro TRIPMINE_CLASSNAME; senão o gcc emite cópias no fim do .rodata.
- Headers: CGrenade em weapons.h (Explode virtual slot 128, BounceSound 129, BloodColor fora de linha), CBeam::SetScrollRate, UTIL_FindEntityByClassname em util.h.
Ver [[hl1ps2-dlls]].
