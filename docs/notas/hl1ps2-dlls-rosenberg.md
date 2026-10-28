---
name: hl1ps2-dlls-rosenberg
description: rosenberg.cpp e roach.cpp das dlls do HL1 PS2 (faixas, diferenças do PS2, alinhamento da rodata)
metadata:
  type: project
---
rosenberg.cpp (monster_rosenberg, Decay) feito em 2026-10-20 (commit 7038458), 100%. Texto 0x1D5950–0x1D75B8, .data 0x317D88, .rodata 0x36F080 (vtable 0x36F540). Saiu de `sed` do wheelchair.cpp (CWheelchair→CRosenberg, tlWc/slWc→tlRo/slRo, KL_→RO_, registro "243" em func_001D73C8) mais: HandleAnimEvent do SDK antigo (pev->body = body % 4 + 4 / body % 4), Spawn com health sem *2 e cabeça aleatória (body -1 → RANDOM_LONG(0,3), cabeça 2 → skin 1), Precache/PainSound que pulam ro_pain1/2 no multiplayer (pain1→pain3, pain2→pain4), e `break;` no fim do bloco HEAR_SOUND do estado ALERT/IDLE do GetSchedule (com som ouvido nunca chega ao IsFollowing).

roach.cpp (monster_cockroach) 2026-10-20, commit 82b7925, 100%: texto 0x1D4818, .rodata 0x36EBB0, sem .data. HLSDK quase igual (UTIL_SetSize max (1,1,1), bits_MF_TO_LOCATION = 1<<6, inline Set/Clear/HasConditions no topo, tirar o `Vector vecApex` sem uso do Move, 0.1f). A .rodata do rosenberg precisa de alinhamento 16 (OBJCOPY_EXTRA no Makefile); o __asm__ .align no fim do roach não serve porque o "monster_cockroach" do init estático sai depois.

Ver [[hl1ps2-dlls-schedule]] e [[hl1ps2-dlls]].

rat.cpp (12b2d46, 100%): texto 0x1D4570–0x1D4818, .rodata 0x36E750–0x36EBB0, sem .data. HLSDK com "models/bigrat.dol" e `pev->takedamage = DAMAGE_YES;` após MonsterInit(). roach.o precisou de `--set-section-alignment .rodata=16` no Makefile (rodata do rat acaba em 0x36EBAC).

python.cpp (b2cd24e): texto 0x1D37D8–0x1D4570, .rodata 0x36E128–0x36E750 (começa com "classname","targetname","target","Half-Life" que vêm de inlines dos headers), sem .data. Versão antiga do HLSDK (m_flSoundDelay 0x8C, m_fInZoom 0x90, gpGlobals->time, sem eventos), Ps2Virt07 no lugar de IsMultiplayer, rumble func_002071C8(pl,1,230,95), m_ps2InfiniteAmmo. effects do PS2 é (int)(float)effects | EF_MUZZLEFLASH, depois do SetAnimation. PrimaryAttack NON_MATCHING (só o lui de g_engfuncs sobe; sched1). A string só do NON_MATCHING ("!HEV_AMO0") vai sozinha no .s do splat; NÃO precisa de função dummy (ela duplicaria a string).

playermonster.cpp (7a3c789, 100%): texto 0x1D3508–0x1D37D8, .rodata 0x36DCC8. HLSDK puro com "models/player.dol". A cópia weak do __12CBaseMonster que aparece no .text dos arquivos de monstro é emitida pelo próprio g++ (não precisa INCLUDE_ASM). Lembrete: `make split` sozinho precisa do venv (source ../.venv/bin/activate) e apaga asm/ antes.

plats.cpp: Use e Spawn do CFuncTrackTrain NON_MATCHING (escalonamento). Se a função-chave da classe (1º virtual não-inline declarado) vira INCLUDE_ASM, a vtable some do .rodata: reordenei a declaração em trains.h (Precache antes de Spawn), a ordem dos slots não muda. O dummy em .plats_discard guarda o PMF do Find. fixasm: li 0x80000000 antes de branch não vai pro delay slot. Funções do CPathTrack (pathcorner) resolvidas com tools/resolveundef.py. Mudar OBJCOPY_EXTRA no Makefile não refaz o .o: apague o .o antes do full2.sh.
