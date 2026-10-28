---
name: hl1ps2-dlls-shotgun
description: shotgun.cpp e scripted.cpp feitos (faixas, diferenças do PS2, NON_MATCHING, protótipo func_00113D08)
metadata:
  type: project
---

shotgun.cpp (weapon_shotgun CShotgun 0x9C, ammo_buckshot CShotgunAmmo 0x30) feito em 2026-10-20, commit fe4e8bd, tudo casa.
- Faixas: texto 0x1E2CF0–0x1E48B0, .data 0x3185E8 (m_SaveData, 3 campos), .rodata 0x373A70–0x374118 (vtables 0x373C48 ammo, 0x373E30 arma).
- Versão antiga do SDK (sem eventos): Spawn faz `pev->classname = MAKE_STRING("weapon_shotgun")`, modelos .dol, CShotgun tem campos próprios m_fInReload 0x8C, m_flNextReload 0x90, m_iShell 0x94, m_flPumpTime 0x98. IsDeathmatch = g_pGameRules->Ps2Virt08().
- PS2: rumble func_002071C8(m_pPlayer, 1, 180, 95) no primário e (255, 125) no secundário; o secundário ejeta 2 cápsulas (segunda com `vecShellVelocity = ...` reatribuído); punchangle.x -= 5 / -= 10 no fim; testes de munição com `m_pPlayer->m_ps2InfiniteAmmo` (0x770) antes do m_rgAmmo.
- func_00113D08 é `(float flTime, float flDelay, int iPlayer)` (EABI separa int e float; a ordem dos parâmetros muda a ordem de avaliação e o delay slot). weapons.cpp continua casando com ele.
- player.h: m_iExtraSoundTypes 0x2AC, m_iWeaponFlash 0x2B0, SetSuitUpdate (0x1C9C30), gmsgWeapPickup = 0x317CF4.
- Reload usa `globalvars_t *pGlobals = gpGlobals;` no bloco do m_fInReload == 1 (igual ao DefaultReload).

scripted.cpp feito em 2026-10-20, commit 12acb7a, 56 de 57 funções casam. Texto 0x1E0420–0x1E2CF0, .data 0x318488 (m_SaveData de CCineMonster e CScriptedSentence), .rodata 0x3727B0–0x373A70 (vtables CFurniture 0x372BD8, CScriptedSentence 0x372FE0, CCineAI 0x3731C8, CCineMonster 0x3735F8). include/dlls/scripted.h novo (SequenceDone virtual, slot 131).
- PS2: m_saved_effects = effects & ~0x300 e o CineCleanup devolve `saved | (effects & 0x300)`; ACT_RESET só sem o flag 0x400 do scripted_sequence; SetConditions inline (m_afConditions |=). CCineMonster::Die solta o m_pGoalEnt do alvo (versão antiga). CScriptedSentence é o antigo (AcceptableSpeaker/FindEntity/StartSentence com CBaseMonster*, FindThink solta m_pGoalEnt antes do UTIL_Remove).
- DelayStart NON_MATCHING (só agendamento no topo do laço); "targetname" D_003727C0 e "scripted_sequence" D_00372A48 absolutos no linker_script_extra.ld.
- Endereços novos: SequencePrecache 0x131400, SetState 0x1A8D90, SUB_StartFadeOut 0x14A0A8, GetBonePosition 0x130738, FCheckAITrigger 0x1A7690. EF_NOINTERP no util.h.
- Cuidado: `grep -v "__12CBaseMonster="` também apaga CineCleanup__12CBaseMonster=; e __static_initialization_and_destruction_0 não pode ir pro symbol_addrs (duplicado).

## scientist.cpp (commit d65b840, 100%)
- Texto 0x1DD878–0x1E0420, .data 0x318110, .rodata 0x3715A0. Feito copiando o wheelchair.cpp (que é o mesmo CScientist) e trocando o que difere.
- PS2: cabeça por SetBodygroup(1, body ou RANDOM_LONG(0,3)) e skin=1 se GetBodygroup(1)==2; seringa por SetBodygroup(2,1/0); campo extra m_coverTime (0x31C) usado no GetSchedule (cover só a cada 3s); CoverRadius 512; SetUse só olha spawnflags&0x10000; voicePitch por faixas de pev->body (0–24/100–124 → 105, etc.); SetAnswerQuestion usa RANDOM_FLOAT(6,8).
- Os arrays tl*/sl* do wheelchair foram renomeados para tlWc*/slWc* (código e symbol_addrs) para não colidir com os nomes HLSDK do scientist.
