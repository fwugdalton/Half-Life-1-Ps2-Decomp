---
name: hl1ps2-dlls-done-8
description: continuação da lista de .cpp das dlls feitos (explode em diante) e próximo arquivo
metadata:
  type: project
---
Continuação de [[hl1ps2-dlls-done-7]] (ordem alfabética reversa).

- explode (0x163630, 2aee8cb, 17/18): .data 0x30FA88 (m_SaveData), .rodata 0x34A588 (vtables CEnvExplosion 0x34A648, CShower 0x34AA50). PS2: depois do decal, o Use vibra os jogadores 1 e 2 (CBaseEntity::Instance(INDEXENT(i)), IsPlayer, `(pev->origin - p->pev->origin).Length() < 800`, func_002071C8(p, 1, (BYTE)(m_iMagnitude * 1.7f), 250)). Registro func_00164370: 105 Smoke. Use NON_MATCHING (trunc.w.s da vibração); D_0034A610 (ptr do Smoke) e D_0034A618 ("spark_shower") no linker_script_extra.ld.

Próximo: egon.cpp (.text termina em 0x163630, _GLOBAL_ em 0x163610; .data termina em 0x30FA88; .rodata termina em 0x34A588, "ammo_egonclip" em 0x34A578).
