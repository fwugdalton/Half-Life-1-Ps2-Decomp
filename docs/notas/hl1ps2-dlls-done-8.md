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

- doors (0x157ED0, c7ad726, 35/35): .data 0x30F790 (m_SaveData CBaseDoor, CMomentaryDoor 0x30F800), .rodata 0x346A70 (vtables CMomentaryDoor 0x346F80, CRotDoor 0x347168, CBaseDoor 0x347350). HLSDK + mudanças do PS2: KeyValue com atoi (só WaveHeight usa atof*(1.0/8.0) em double); Use com flag 0x10000 (USE_ON só abre em TS_AT_BOTTOM, USE_OFF só em TS_AT_TOP, sem m_hActivator); DoorTouch com targetname: se !IsMultiplayer() || pev->flags & 0x20000, retorna quando há m_sMaster; DoorGoUp/GoDown tocam o som sem checar o estado; Blocked sem STOP_SOUND no laço; CMomentaryDoor sem DoorMoveDone, Use sem clamp de 0 e sem teste do m_vecFinalDest, speed = Length()*10. ButtonSound__Fi = 0x1426A8. Registro func_0015A3E8 "077"–"081" (MOVEPTR = func_00239830). A .rodata do effects.o passou a precisar de alinhamento 16 no Makefile (doors acaba em 0x347557).

Próximo: defaultai.cpp (.text 0x157AE0–0x157ED0: ScheduleFromName, ScheduleInList, GetScheduleOfType; o _GLOBAL_ do crowbar está em 0x157AC0; .data termina em 0x30F790; .rodata termina em 0x346A70).
