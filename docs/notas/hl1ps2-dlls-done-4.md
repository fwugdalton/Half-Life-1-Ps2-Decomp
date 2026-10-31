---
name: hl1ps2-dlls-done-4
description: continuação da lista de .cpp das dlls feitos (hassassin em diante), com faixas e diferenças do PS2
metadata:
  type: project
---
Continuação de [[hl1ps2-dlls-done-3]] (formato: arquivo (texto, commit, funções casadas)).

- hassassin (0x1811D8, f8248ff, 24/24): .data 0x313010 (m_SaveData com 10 campos; termina com o static iStep em 0x313380), .rodata 0x358A70. PS2: campo novo float m_flCoverTime (+0x2D8, salvo como FIELD_FLOAT, depois do m_iShell) que limita a 3 s as fugas TAKE_COVER_FROM_BEST_SOUND (idle/alert e combate) e TAKE_COVER_FROM_ENEMY (inimigo novo); CheckMeleeAttack1 e CheckRangeAttack1 usam UTIL_TraceLineNoEnts (0x207F18) em vez de TraceHull/TraceLine; CheckRangeAttack2 põe m_flNextGrenadeCheck = time + 6 quando o VecCheckThrow dá zero; sqrtf no pulo. HasConditions inline definido no topo do arquivo (o TaskComplete do header continua chamando a cópia fora de linha). Pegadinha: o cmpfile.sh ignora o alvo do jal; só o cmp da ROM pegou o UTIL_TraceLine errado.
- handgrenade (0x180810, ebb0c20, 14/14): sem .data, .rodata 0x3586C8 (a .rodata do hassassin passou a precisar de OBJCOPY_EXTRA .rodata=16 no Makefile). Versão antiga do SDK (gpGlobals->time, Holster(void), sem eventos). PS2: rumble func_002071C8 no pino (1,80,15) e no arremesso (1,255,125), munição infinita (m_ps2InfiniteAmmo 0x770), flVel = (90-x)*4 com teto 500, vecThrow soma só metade da velocity; WeaponIdle começa com GetAutoaimVector(AUTOAIM_10DEGREES) descartado (como comando solto, não variável, senão a pilha cresce); m_flReleaseThrow==0 sem testar m_flStartThrow. Registro "141" DestroyItem em func_001810C0.
