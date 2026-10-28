---
name: hl1ps2-dlls-weapons
description: lições do weapons.cpp (dlls HL1 PS2): headers player.h/weapons.h, g_vecZero_dll, registro com void, gpGlobals local
metadata:
  type: project
---
weapons.cpp (commit 6708908, 2026-10-20) casa inteiro. Lições:
- include/dlls/player.h (CBasePlayer com padding até os campos conhecidos: m_pActiveItem 0x4B8, m_rgAmmo 0x4C4, m_ps2InfiniteAmmo 0x770...) e weapons.h (HLSDK antigo: sem GetWeaponPtr, Holster(void), iItemSlot é o slot 76). gamerules.h reescrito com slots PS2 (Ps2Virt07/08, Ps2Virt64(int) devolve CBaseEntity*).
- g_vecZero das dlls (0x312C28) colide com o do motor (0x3328F8): util.h faz `#define g_vecZero g_vecZero_dll`.
- funções de registro (func_002397D0/F0/810) devem ser declaradas `void`: com `int` o gcc usa v1 em vez de v0 no lui das constantes PMF.
- DefaultReload: `globalvars_t *pGlobals = gpGlobals;` depois do SendWeaponAnim manteve o ponteiro em registrador salvo através do ENTINDEX.
- PackWeapon: no PS2 é `effects |= EF_NODRAW`.
- asm do motor referenciava D_00324C60 (gMultiDamage+8): definido em linker_script_extra.ld.
- globais não inicializados (g_sModelIndex*, gMultiDamage) ficam no .data, no fim do arquivo.
- rodata alvo tinha 8 bytes de zeros a mais no fim: segmento `[0x282BC8, rodata, cod/282BC8]`.
Ver [[hl1ps2-dlls]].
