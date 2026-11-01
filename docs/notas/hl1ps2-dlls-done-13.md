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

- airtank (0x12CF18, 5d1315b, 14/14): .data 0x30DE10, .rodata 0x33B630–0x33BB05. Igual ao HLSDK com .dol e o registro func_0012D328 ("008" TankThink, "009" TankTouch, com friend).

- aievent (0x126C00, fc79732, 9/9): info_aievent do PS2, `CAIEvent : CCineAI` (o vtable usa as versões do CCineAI; scripted.h ganhou `public:` no CCineAI). Chaves eventtype 0x2D4, anglerange 0x2D8, animcount 0x2DC (contador 0x2E0), probability 0x2E4, usetarget 0x2E8 (ALLOC_STRING); Spawn só põe pev->classname; SequenceDone volta cedo se tipo 2 e ++cont < animcount. .rodata 0x33AE10–0x33B2E0, sem .data. Registro "006" SUB_Remove.
- IA do parceiro (métodos Ps2AI* de CBasePlayer, .text 0x126F80–0x12CF18, .rodata 0x33B2E0–0x33B630, provavelmente .data até 0x30DE10): ADIADA como o player.cpp (precisa do layout 0x6CC–0x748 do CBasePlayer, que mexe nos Vector de preenchimento do player.h). Fica como asm cod/026F80 e rodata cod/23B2E0 no yaml.

Próximo: agrunt.cpp (.text termina em 0x126C00; _GLOBAL_$I do arquivo anterior em 0x124480, então começa por volta de 0x1244A0; .rodata termina em 0x33AE10).
