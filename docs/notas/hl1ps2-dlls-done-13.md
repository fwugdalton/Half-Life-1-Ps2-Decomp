---
name: hl1ps2-dlls-done-13
description: continuação da lista de .cpp das dlls feitos (alienflyer em diante), regra nova do fixasm e próximo arquivo (airtank.cpp)
metadata:
  type: project
---

Continua [[hl1ps2-dlls-done-12]].

- alienflyer (0x12D4A8, 8e90131, 27/27, nada NON_MATCHING): só do Decay, sem fonte; escrito a partir do osprey.cpp ([[hl1ps2-dlls-osprey]]). .data 0x30DE20 (SaveData, 0x110), .rodata 0x33BB08–0x33C24A. CAlienFlyer : CBaseMonster, 0x388 bytes, vtable 0x33BE30; campos 0x2A0–0x30C iguais ao osprey; 0x310 m_iSoundState … 0x338 EHANDLE m_hGrunt[3], 0x350 m_iNumTracks, 0x354 m_iCurTrack, 0x358 string_t m_iszTrack[10] (KeyValue "aftrack_N"), 0x380 m_fChangedTrack, 0x384 m_fStopAttack. Classify = CLASS_ALIEN_MONSTER (7), BloodColor amarelo. Registro func_0012FAA8: "010" FlyThink … "014" CommandUse.
  - AttackThink: `if (!m_fStopAttack) {…} else {…}` (o else vai para o fim com bnel); `TraceResult tr; Vector vecSrc;` declarados antes e vecSrc atribuído (cópia via temporário); os beams usam SetScrollRate/SetBrightness (não SetWidth): pev->animtime 0x114, renderamt 0x12C.
  - DyingThink: os dois breakmodel com BREAK_FLESH, o 2º com 15 pedaços.
- fixasm (mesmo commit): o gas punha um nop entre `c.cond.s` e um sqrt.s/cvt.w.s convertido em .word; agora os dois vão dentro de `.set noreorder`. Depois de mudar o fixasm, apagar todos os .o (o Makefile não depende dele).
- `_GLOBAL_$I$monster_x` precisa estar no symbol_addrs (a lista de ctors da .data referencia func_ do endereço).

Próximo: airtank.cpp (item_airtank). .text termina em 0x12D4A8 (_GLOBAL_$I 0x12D488, static init 0x12D448); .rodata provavelmente 0x33B630 ("classname", "CAirtank", "models/w_oxygen.dol", "doors/aliendoor3.wav", "player/pl_swim2.wav", "008"/"009") até 0x33BB08; antes vêm GINA_STOP/COL_DIE (outro arquivo).
