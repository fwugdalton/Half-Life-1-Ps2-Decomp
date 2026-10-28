---
name: hl1ps2-dlls-multiplay
description: multiplay_gamerules.cpp do HL1 PS2 (CHalfLifeMultiplay): faixas, diferenças do PS2, NON_MATCHING e truques
metadata:
  type: project
---
multiplay_gamerules.cpp feito em 2026-10-25, commit 0c77b85. Texto 0x1AB410–0x1ACFE0, sem .data, .rodata 0x365E78–0x3663E8 (vtable 0x3661B8; começa com "classname"... dos inlines). Base do HLSDK antigo (ChangeLevel com sscanf, sem mapcycle_t).

Diferenças do PS2:
- IsMultiplayer devolve gpGlobals->coop (0x18), não deathmatch.
- ClientCommand novo (0x1ABEC0): "coopswitch" alterna m_ps2ShowScores (0x778) e manda "+showscores %d\n"/"-showscores %d\n" (buffer char[16]); "slowmo" com cheat_slowmo.
- ScoreInfo com 5 shorts: frags, m_iDeaths, m_ps2Stat5F8, m_ps2ShotsFired, m_ps2ShotsHit.
- FlWeaponTryRespawn limita a 5 "monster_tripmine" (pEntity e count declarados no topo da função).
- DeathNotice vazio, mas o corpo antigo ficou depois de um `return;` (as strings ficam no .rodata). Sem NON_MATCHING essas strings saem no .s do PlayerKilled que o splat gerou, por isso o corpo morto fica sob #ifdef NON_MATCHING. Não precisa de função dummy: o splat migra as strings das funções INCLUDE_ASM.
- ChangeLevel termina chamando o slot 0x1C do g_engfuncs com 1 argumento (pfnPs2ChangeLevelMP em extdll.h), não o CHANGE_LEVEL.

NON_MATCHING: GetNextBestWeapon (v0/v1), InitHUD e PlayerKilled (pfnWriteShort carregado antes da conversão float→int, o mesmo problema do coopplay), ChangeLevel (alocação de registradores no laço do isspace).

Headers: player.h ganhou m_afButtonLast/Pressed/Released (0x2C0/0x2C4/0x2C8), m_flNextDecalTime 0x60C, IsOnLadder 0x1C5890, HasNamedPlayerItem 0x1D0BE0, SwitchWeapon 0x1D0C88. gmsgMOTD = 0x317CEC. A .rodata do nihilanth.o passou a precisar de alinhamento 16 no Makefile.

Ver [[hl1ps2-dlls]] e [[hl1ps2-dlls-nihilanth]].

mp5.cpp feito em 2026-10-25, commit a64b5af, 100%: texto 0x1A9D68–0x1AB410, sem .data, .rodata 0x365368–0x365E78. HLSDK antigo (m_flNextAnimTime 0x8C, m_iShell 0x90, SecondaryAmmoIndex fora de linha). PS2: rumble (215,28) no tiro e (255,125) na granada, `waterlevel == 3 || m_iClip <= 0` juntos, m_ps2InfiniteAmmo antes do `m_rgAmmo[sec]--`, EMIT_SOUND (não _DYN) no glauncher. O ShootContact casou com `Vector vecSrc, vecThrow;` declarados logo antes e montados em atribuições (vecThrow = fwd*800; vecSrc = fwd*16; vecSrc = vecSrc + view_ofs; vecSrc = vecSrc + origin). ShootContact = 0x179190; player.h ganhou m_flStopExtraSoundTime 0x2B4.
