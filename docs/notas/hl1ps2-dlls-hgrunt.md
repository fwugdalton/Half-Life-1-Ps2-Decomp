---
name: hl1ps2-dlls-hgrunt
description: hgrunt.cpp do PS2 (granadas pegas do chão, GetIdealState, spawnflags novos) e truques de matching
metadata:
  type: project
---
dlls/hgrunt.cpp, commit 86a0c81 (ver [[hl1ps2-dlls-done-3]]).

Mudanças do PS2:
- CHGrunt ganhou EHANDLE m_pGrenadePickUp (0x310) e float m_flCoverTime (0x318), ambos no m_SaveData. CGrenade::m_iPs2Unk2A4 marca a granada como já pega.
- Tarefas 94 FLEE_SOUND (corre 256 para longe do PBestSound, depois para os lados com v_right ±192/256), 95 FIND_GRENADE (UTIL_FindEntityByString "classname" "grenade" a < 192, MoveToLocation se >= 48), 96 CHECK_GRENADE (ACT_SPECIAL_ATTACK2 = "kick_grenade" se falta < 1 s, senão "throwback_grenade"; RunTask completa com m_fSequenceFinished).
- Schedules 54 PS2_RELOAD, 55 RUN_AWAY (falha -> 56), 56 THROWBACK (falha -> COWER). TakeCoverFromBestSound falha para 55 (no SDK COWER). Lista de schedules termina com RunAway, Ps2Reload, ThrowBack.
- Eventos 12 (CAUGHT_ENEMY cai no DropPickedUp) e 13 (EF_NODRAW na granada pega). GREN_TOSS usa CGrenade::ThrowPickedUp se tem granada pega.
- GetIdealState novo (0x18B130): parado ouvindo som, vira, traça (atravessando func_wall com flag 4) e entra em combate se acha monstro (spawnflag 0x2000 ignora o jogador).
- Spawnflags: 0x1000 escolhe o modelo (if/else com a mesma string, também no Precache), 0x8000 agressivo (sem cobertura), 0x40000 ELOF vira WaitInCover. m_flCoverTime: só procura cobertura se passaram 3 s.
- GetSchedule: m_hAIEventTarget com pev->deadflag != DEAD_DEAD -> SCHED_IDLE_STAND. CHGruntRepel::RepelUse usa UTIL_TraceLineNoEnts. Registro func_0018B940: "149" SUB_Remove, "150" RepelUse.

Truques:
- EHANDLE: `if (h)` e `h != NULL` chamam __opi; a ROM às vezes chama __opP (0x145B18): escrever `(CBaseEntity *)h`. O cmpfile não vê; só o cmp da ROM (byte 0xC6 vs 0xDE no jal).
- Vetores declarados juntos no topo e depois atribuídos (`vecDir = ...`) dão o temporário + cópia que a ROM tem; `Vector v = expr` grava direto e muda a pilha.
- `bool fMoved` gera o sltu de MoveToLocation; `||` virou flag: `if (len < 48) ; else if (!MoveToLocation(...)) { TaskFail(); break; }` casou.
- Um só `CBaseEntity *pHit` para os dois blocos casou a alocação de registradores.
- fptoui (0x2E0F18) declarado no linker_script_extra.ld: no symbol_addrs o rename_syms renomeia o func_002E0F18 nos .c do engine.
