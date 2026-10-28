---
name: hl1ps2-dlls-teamplay
description: teamplay_gamerules.cpp, talkmonster.cpp e subs.cpp (faixas, flags, diferenças do PS2) e a vtable de 68 entradas do CGameRules
metadata:
  type: project
---

teamplay_gamerules.cpp (CHalfLifeTeamplay) feito em 2026-10-20, commit 61bbc90, 22 de 22 funções casam.
- Faixas: texto 0x1F2B38–0x1F3E38, .rodata 0x378268–0x378600 (vtable em 0x3783D0), .data 0x323D4C (só num_teams, 4 bytes).
- Makefile: `-fmove-all-movables` (CFLAGS) para o InitHUD casar (lui do gmsgTeamInfo içado do laço) e OBJCOPY_EXTRA com `.data=4`.
- Static do .bss: team_names = D_003A3E20, team_scores = D_003A4020 (extern "C" + #define).
- Cvars do game.cpp (ps2cvar_t, passo de 0x18): fraglimit 0x30FEC8, timelimit 0x30FEE0, friendlyfire 0x30FEF8, teamlist 0x30FFB8, teamoverride 0x30FFD0, defaultteam 0x30FFE8.
- Diferenças do PS2: sem g_VoiceGameMgr, sem TeamNames, sem ScoreInfo no ChangePlayerTeam, RecountTeams sem bResendInfo, LogPrintf "\"%s<%i>\" changed to team %s", Think só confere timelimit/fraglimit.
- gamerules.h agora tem as 68 entradas reais (vptr em 0x10; entrada k no offset 8k). Renomeados: Ps2Virt09 → IsTeamplay, Ps2Virt18 → FPlayerCanTakeDamage, a antiga DeathNotice do slot 31 → Ps2Virt31 (o turret chama esta). CHalfLifeMultiplay declara só os overrides cujos endereços caem em 0x1AB000–0x1AD000; 67 ChangeLevel e 68 GoToIntermission são virtuais novas dele.
- player.h: m_szTeamName[16] em 0x610.

talkmonster.cpp (CTalkMonster) feito em 2026-10-20, commit 3776dfe: 43 de 44 casam, CanFollow NON_MATCHING (ordem dos blocos do return FALSE).
- Faixas: texto 0x1EFEF8–0x1F2B38, .data 0x3239A0–0x323D4C (g_talkWaitTime, m_SaveData, schedules), .rodata 0x377A90–0x378268 (vtable 0x377E48).
- Diferenças do PS2: bits_MEMORY_PROVOKED = 1; spawnflag 0x10000 (SF_MONSTER_PS2_NOFOLLOW) bloqueia Touch/StartFollowing e entra no DeclineFollowing junto com PREDISASTER; IdleRespond sem GetVoicePitch; tlIdleResponse espera 5 s (não 0.5).
- Truques: SetConditions/ClearConditions/HasConditions redefinidos inline no meio do arquivo (antes do Touch; StartTask/RunTask ainda chamam fora de linha); constantes float com f (1.0f, 0.75f, 2.0f, (float)TALKRANGE_MIN); fabs → __builtin_fabsf.
- Para gerar o .s de um INCLUDE_ASM novo: o símbolo tem que estar no symbol_addrs antes do make split (compilar uma vez com #define NON_MATCHING, rodar gensyms2, tirar o define).

subs.cpp feito em 2026-10-20, commit f50a610, tudo casa. Texto 0x1EE890–0x1EFEF8, .data 0x323850–0x3239A0, .rodata 0x376C20–0x377A86.
- PS2: LinearMoveDone é a versão antiga (sem o teste de erro 0.03125); há CBaseCoopStart (info_player_coop, KeyValue "index" → pev->team). Registro de think "272"–"274" em func_001EFB20.
- CPointEntity::ObjectCaps voltou a ser inline dentro da classe em cbase.h (a ordem das cópias inline no fim do TU segue a ordem de definição).
- talkmonster.o precisa de `.rodata=16` no OBJCOPY_EXTRA (o .rodata do subs termina em 0x377A86).

stats.cpp feito em 2026-10-20, tudo casa de primeira. Texto 0x1EE388–0x1EE890, .data 0x323830 (gStats), .rodata 0x376B60. UpdateStatsFile é vazia no PS2. player.h ganhou m_rgpPlayerItems[6] em 0x4A0.
