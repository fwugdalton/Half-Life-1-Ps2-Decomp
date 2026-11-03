---
name: hl1ps2-dlls-player
description: player.cpp do PS2 feito (faixas, flag, NON_MATCHING, .data/.rodata e truques da integração)
metadata:
  type: project
  modified: 2026-11-03T07:34:49.479Z
---
Feito em 2026-11-03 (depois do Launcher, [[hl1ps2-launcher-main]]). ROM OK.
- Texto 0x1C3350–0x1D3508, .rodata 0x36AF40–0x36DCC8, .data 0x317958–0x317D88. yaml `[0x0C3350, cpp, dlls/player]`.
- Flag: compila SEM -fmove-all-movables (160/164 casam). NON_MATCHING (INCLUDE_ASM): TakeDamage (nunca casa), PlayerUse, AddPlayerItem, UpdateClientData (casam só com a flag). Com a flag falhariam Ps2FindUseable, AutoaimDeflection, Ps2AutoaimDeflectionOld, DropPlayerItem.
- Nomes do PS2 vindos da tabela de save: m_iTeamNum 0x620, m_hTargetedEnemy 0x77C, m_fInfiniteAmmo 0x770, m_flDefaultGravity 0x774 (renomeados em todos os .cpp).
- .data: globais SEM inicializador (gEvilImpulse101, g_pevLastInflictor, gmsgGeigerRange, g_pLastSpawn) vão para o FIM do .data na ordem de declaração; `= 0`/`= NULL` ficam no lugar. gEvilImpulse101 é definido aqui.
- .rodata: player.cpp inclui effects.h depois do gamerules.h (2 ponteiros p/ membro antes de "Half-Life"... depois dele). LinkUserMessages virou macro (as strings saem depois das do Precache; inline definida antes punha antes). O gcc junta strings iguais no mesmo arquivo: o CDeadHEV usa "models/player.dol".
- Classes do PS2: CTargetSpot (targeting_spot, Create(iType) com cor por jogador, SetTarget com MOVETYPE_FOLLOW e ShrinkThink) e CUseableGraphic (useable_graphic, Create(pTarget,iType), escala pelo tamanho; ajustes para item_eyescanner (v_right * -2.3), healthcharger/recharge, func_tankcontrols). ObjectCaps dos dois: `(CBaseEntity::ObjectCaps() & ~FCAP_ACROSS_TRANSITION) | FCAP_DONT_SAVE` (gera nop no delay slot); o do CSprayCan é `return FCAP_DONT_SAVE`.
- Registro de thinks "236"–"242" em func_001D2BF0. DeathMessage não existe no player.h do PS2.
- Outros arquivos agora usam os nomes reais (gmsgHudColor, gmsgButtonHud, SendAmmoUpdate, FlashlightIsOn/TurnOn/TurnOff, AmmoInventory, EntSelectSpawnPoint, Ps2TrackTarget, g_pLastSpawn, gInitHUD, sqrt no cl_cam.c).
- Métodos da IA do parceiro (arquivo adiado 0x126F80–0x12CF18) estão no symbol_addrs: Ps2AIKilled 0x12CCD0, Ps2AISwitchWeapon 0x12CDE0, PlaySentence 0x12CE30, ISoundMask 0x1270D8.
