---
name: hl1ps2-dlls-sound
description: dlls/sound.cpp feito (faixas, diferenças do PS2, ctype do newlib, truques de float e init)
metadata:
  type: project
---

sound.cpp feito em 2026-10-20, commit fec65af, todas as funções casam.
- Faixas: texto 0x1E6BC0–0x1EA608, .data 0x3187C0–0x323770 (rgdpvpreset, savedata, fSentencesInit, gcallsentences, fTextureTypeInit, gcTextures, gszallsentencenames 0x3192D0, rgsentenceg 0x31F2D0, grgszTextureName 0x321B70, grgchTextureType 0x323570), .rodata 0x374A90–0x375778.
- PS2: EMIT_SOUND_DYN começa com `if (ISlave_EmitSound(...)) return;` (0x20C6C8, weapon_islave.cpp). func_001E82E0 (fSentencesInit = FALSE) é chamada pelo snd_mix_gbx.c do motor. env_sound Spawn usa RANDOM_FLOAT(0.0, 0.5) (versão antiga).
- Truques: f em TODOS os literais float (perl no arquivo inteiro resolveu quase tudo); `unsigned char ipick = 0;` no USENTENCEG_Pick; TEXTURETYPE_Init com `int fileSize;` sem init e `filePos = 0;` depois do return; SpeakerThink com `char *szSoundFile = NULL;`.
- ctype do newlib em D_00397D81 (macros no topo do arquivo); stricmp→strcasecmp.
- Registro func_001EA230: "262"/"263" SUB_Remove (Ambient/Speaker), "264" RampThink, "265" SpeakerThink, "266"/"267" ToggleUse (func_00239810).
- player.h: m_pentSndLast 0x2CC, m_flSndRoomtype 0x2D0, m_flSndRange 0x2D4. util.h: SVC_ROOMTYPE 37, EMIT_SOUND_DYN2, strcat/strncasecmp, UTIL_EmitAmbientSound.

slave_collar.cpp (item_slave_collar, CISlaveCollar : CBaseToggle, tamanho 0x10C, do Decay) feito em 2026-10-20, commit 40d4032. Texto 0x1E56E8–0x1E6BC0, .data 0x318780 (m_SaveData), .rodata 0x374748 (vtable 0x374890). É o ArmBeam/ZapBeam do islave.cpp num modelo parado; m_pBeam[12] em 0xD0 (ZapBeam usa m_pBeam[12 - side]), dano do zap é gSkillData.slaveDmgClaw (0x74). Truques: `Vector vecSrc, vecAngles;` declarados no topo com vecSrc atribuído; `vecAim = v_forward; vecAim = vecAim + ...` em duas linhas. StartCharge NON_MATCHING (só agendamento do WRITE_BYTE(20 / framerate), igual ao StartZap do weapon_islave); a string "debris/zap4.wav" sai por __asm__ como D_003747D8 (macro ZAP4_WAV). CBeam::PointsInit = 0x15AF28. Antes dele: gamerules de coop ("HL Coopplay", GetGameDescription em 0x1E56D8) e skill.cpp.

coopplay_gamerules.cpp (CHalfLifeCoopplay : CHalfLifeMultiplay, "HL Coopplay") feito em 2026-10-20, commit 66797f2. Texto 0x1E4FC0–0x1E56E8, sem .data, .rodata 0x374498 (vtable 0x374510). Mistura teamplay (ShouldAutoAim, IPointsForKill), singleplay (FlPlayerFallDamage) e respawns presos ao cvar mp_coopweprespawn (D_00310048; mp_weaponstay é D_0030FF28, os dois no linker_script_extra.ld). Slot 69 novo (IPointsForMonsterKill, `return pKilled != NULL`). Ps2Virt31 NON_MATCHING: o alvo carrega o pfnWriteShort do WRITE_SHORT(frags) depois da conversão float→int (mesmo problema do StartCharge do slave_collar); não resolvido com cast, variável local nem flags. player.h ganhou m_flFallVelocity 0x2D8.
- tools/cmpfunc.py e resolveundef.py: objcopy -O binary para /dev/stdout às vezes sai vazio; agora usam arquivo temporário (o datacmp.py do scratchpad também).

skill.cpp feito em 2026-10-20 (commit 5de3c2d): GetSkillCvar 0x1E4F38, .data gSkillData 0x318618–0x318780 (0x168), .rodata 0x374438. Layout real do skilldata_t (visto no CGameRules::RefreshSkillData 0x16F488): os 8 bytes extras são plrDmgISlave/plrDmgISlaveZap (0x78/0x7C) logo depois do slaveDmgZap (0x74), não depois do bigmomma; flyerHealth/flyerDamage/focusHealth no fim. weapon_islave e slave_collar foram renomeados para os campos certos. O asm acessa os campos como D_0031861C...; todos estão no linker_script_extra.ld.

singleplay_gamerules.cpp (CHalfLifeRules) feito em 2026-10-20, commit 2eb12d7, casou de primeira. Texto 0x1E48B0–0x1E4F38, .rodata 0x374118 (vtable 0x374218), sem .data. PS2: PlayerSpawn só tem os cheats (cheat_infammo D_003325F0 enche a munição e liga CBasePlayer::m_ps2InfiniteAmmo; cheat_slavemode D_00332650 dá weapon_islave); cheat_slowmo D_003325D8 no Think/ClientCommand ("slowmo", "slavemode", "coopswitch"). CGameRules ganhou m_flSlowmoEnd 0x4, m_flNextSlowmo 0x8, m_fInfiniteAmmo 0xC. g_iSkillLevel = D_00312C44. DeadPlayerWeapons 9, DeadPlayerAmmo 12.
