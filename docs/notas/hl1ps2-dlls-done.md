---
name: hl1ps2-dlls-done
description: lista dos arquivos .cpp das dlls já feitos (endereço do texto, commit) em ordem de trás pra frente
metadata:
  type: project
  modified: 2026-10-25T15:38:48.208Z
---
Dlls (HLSDK, gcc 2.95 C++) no bloco asm cod/000000, ordem alfabética, feitas de trás pra frente. Formato: arquivo (início do .text, commit) e nota/lição.

- zombie (0x1162C0, eb3add1), xen (0x1144E8, 19259cc), wheelchair (0x1129E8, 6309519); entre wheelchair e xen há rodata solta 0x283518–0x283540 (asm).
- weapons (0x10F3C8, 6708908) [[hl1ps2-dlls-weapons]]; weapon_islave (0x10C3A0, be01c86) e warpball (0x10B330, df33958) [[hl1ps2-dlls-islave]].
- util (0x106730, ae3ba7e) [[hl1ps2-dlls-util]]; turret (0x102B90, 0dca00d) [[hl1ps2-dlls-turret]]; tripmine (0x100ED8, 10b085b) [[hl1ps2-dlls-tripmine]]; triggers (0xF8190, 86168db) [[hl1ps2-dlls-triggers]].
- tentacle_decay (0xF5F68, b2b55cc), tentacle (0xF3E38, 59c2dce) [[hl1ps2-dlls-tentacle]].
- teamplay_gamerules (0xF2B38, 61bbc90), talkmonster (0xEFEF8, 3776dfe), subs (0xEE890, f50a610) [[hl1ps2-dlls-teamplay]]; stats (0xEE388).
- squeakgrenade (0xEC6A0, 0347efb), squadmonster (0xEB048), spectator (0xEAE30), soundent (0xEA608) [[hl1ps2-dlls-squeak]].
- sound (0xE6BC0, fec65af), slave_collar (0xE56E8, 40d4032), coopplay_gamerules (0xE4FC0, 66797f2), skill (0xE4F38, 5de3c2d) [[hl1ps2-dlls-sound]]; singleplay_gamerules (0xE48B0, 2eb12d7).
- shotgun (0xE2CF0, fe4e8bd), scripted (0xE0420, 12acb7a), scientist (d65b840) [[hl1ps2-dlls-shotgun]].
- schedule (f0adab4), satchel (98b788e), rpg (0x1D75B8, 2e1507e) [[hl1ps2-dlls-schedule]].
- rosenberg (0x1D5950, 7038458), roach (0x1D4818, 82b7925), rat (0x1D4570, 12b2d46), python (0x1D37D8, b2cd24e), plats (0x1BD438, 84c29c1, trains.h) [[hl1ps2-dlls-rosenberg]]; playermonster (0x1D3508, 7a3c789); plane (0x1BD368, b6afaed, plane.h).
- pathcorner (0x1BC3D0, feb6eae), osprey (0x1B97E8, 03470db) [[hl1ps2-dlls-osprey]]; nodes (0x1B36A0, 6981ac9, 11 NON_MATCHING) [[hl1ps2-dlls-nodes]]; nihilanth (0x1ACFE0, 72647e9, 6 NON_MATCHING) [[hl1ps2-dlls-nihilanth]].
- multiplay_gamerules (0x1AB410, 0c77b85, 4 NON_MATCHING) e mp5 (0x1A9D68, a64b5af) [[hl1ps2-dlls-multiplay]].
- mortar (0x1A9208, 12df6ae, HLSDK puro, dmg 200, registro "192"/"193" em func_001A9B50).
- monsterstate (0x1A8D90, b153f34, RunAI testa pev->flags & SF_MONSTER_GAG; ClearConditions inline).
- monsters (0x1A1828, 1710ffc, 2 NON_MATCHING: RouteSimplify, FindCover) [[hl1ps2-dlls-monsters]].
- monstermaker (0x1A0E80, f7f8099, 100%): m_iszMakerTarget (0x2A4), m_iMonsterSpawnFlags (0x2C0); `int DispatchSpawn` consertou troca v0/v1.
- modeltrain (0x19F5F8, 8a66d19, 100%): monster_modeltrain, só do PS2; TurnThink casou com `goto`; fmod = 0x2D78A0.
- mtank (0x19D2C0, 97c4a10, 100%): monster_tank/monster_tank_shell, só do PS2; gTankSpread via `asm("D_003A3B88")` + fixasm.
- maprules (0x19B2A0, 75137fa, 83/88): .data 0x3146B0, .rodata 0x360EE8; include gamerules.h ANTES de cbase.h (senão o "" do TeamID sai antes do "Half-Life"). 5 NON_MATCHING por conversão float->int (ver [[hl1ps2-dlls-maprules]]).
- Continua em [[hl1ps2-dlls-done-2]] (lights em diante).
- player.cpp (0x1C3350) adiado por ser enorme.
