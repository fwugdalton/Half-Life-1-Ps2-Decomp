---
name: hl1ps2-dlls-done-8
description: continuação da lista de .cpp das dlls feitos (explode em diante) e próximo arquivo
metadata:
  type: project
---
Continuação de [[hl1ps2-dlls-done-7]] (ordem alfabética reversa).

- explode (0x163630, 2aee8cb, 17/18): .data 0x30FA88 (m_SaveData), .rodata 0x34A588 (vtables CEnvExplosion 0x34A648, CShower 0x34AA50). PS2: depois do decal, o Use vibra os jogadores 1 e 2 (CBaseEntity::Instance(INDEXENT(i)), IsPlayer, `(pev->origin - p->pev->origin).Length() < 800`, func_002071C8(p, 1, (BYTE)(m_iMagnitude * 1.7f), 250)). Registro func_00164370: 105 Smoke. Use NON_MATCHING (trunc.w.s da vibração); D_0034A610 (ptr do Smoke) e D_0034A618 ("spark_shower") no linker_script_extra.ld.

- egon (0x161BB8, f2f7987, 30/30): .data 0x30F9F0 (g_fireAnims1/2, m_SaveData 0x30FA08), .rodata 0x349EA0 (vtables CEgonAmmo 0x34A0A8, CEgon 0x34A290). SDK antigo do servidor (CBeam/CSprite); "IsMultiplayer" é o Ps2Virt07 (slot 0x38). PS2: rumble func_002071C8(p, 1, RANDOM_LONG(40,175), RANDOM_LONG(5,15)) no FIRE_CHARGE, UseAmmo com m_ps2InfiniteAmmo, SecondaryAttack só chama WeaponIdle, Attack embaixo d'água testa só m_pBeam, UpdateEffect chama RelinkBeam (0x15B130) antes do SetStartPos do m_pBeam e depois do m_pNoise. Truques: `float timedist = 0;` (a ROM zera f20 antes do switch); aqui `m_flNextPrimaryAttack = m_flNextSecondaryAttack = x` (ordem do SDK) casou. effects.h ganhou SetWidth e RelinkBeam; Expand__7CSpriteff 0x15DDA8 no symbol_addrs.

- effects (0x15A6C0, 27983de, 207/212): ver [[hl1ps2-dlls-effects]].

Próximo: doors.cpp (.text termina em 0x15A6C0, _GLOBAL_ em 0x15A6A0; .data termina em 0x30F810; .rodata termina em 0x347560, "momentary_door" em 0x347548).
