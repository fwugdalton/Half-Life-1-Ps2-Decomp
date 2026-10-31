---
name: hl1ps2-dlls-done-12
description: continuação da lista de .cpp das dlls feitos (bloater, bigmomma, barney) e próximo arquivo (barnacle.cpp)
metadata:
  type: project
---

Continuação de [[hl1ps2-dlls-done-11]]. Formato: arquivo (endereço do texto, commit, funções que casam).

- bloater (0x13BE98, b5110dc, 17/17): sem .data, .rodata 0x33F788–0x33FBE8 (o bmodels.o ganhou `--set-section-alignment .rodata=16`). HLSDK puro com floater.dol; os 4 sons vazios são funções de 8 bytes.

- bigmomma (0x138728, 7913a6f, 55/55): .data 0x30E478, .rodata 0x33E7F0–0x33F781. Mudanças do PS2: BIG_MAXCHILDREN 7 (era 20); Touch do morteiro com RANDOM_LONG(90,110); sqrt→sqrtf; VecCheckSplatToss sem os locais vecScale/vecTemp (pilha 32 bytes menor); o 2º trace do VecCheckSplatToss e o do Touch são UTIL_TraceLineNoEnts. Registro func_0013B8D0: "027" CBMortar::Animate (entre o MortarSpray/Touch e as cópias de GetClassPtr).
- ATENÇÃO: o cmpfunc não acusa alvo de jal diferente (UTIL_TraceLine vs UTIL_TraceLineNoEnts tem o mesmo código de chamada); só o build completo pega. Quando o ROM falhar com poucos bytes, ver o jal com objdump no .elf e `od -An -tx4 -j OFF -N4 SLUS_200.66.rom` (não há xxd).

- barney (0x136CC0, 8252aee, 37/37): .data 0x30E340, .rodata 0x33DC48–0x33E7E4 (bigmomma.o ganhou .rodata=16). Precisa de DLLS_MONSTERS_CPP (HasConditions inline). Mudanças do PS2: arma por SetBodygroup(1, x) em vez de pev->body (saque e coldre usam ambos 1; Killed testa GetBodygroup(1) < 2 e põe 2); pev->effects |= EF_MUZZLEFLASH; Spawn só faz SetUse(FollowerUse) se !(spawnflags & 0x10000), senão SetUse(NULL); CheckRangeAttack1 usa UTIL_TraceLineNoEnts. Registro func_00138510: use "026" FollowerUse.

Próximo: barnacle.cpp ("monster_barnacle" em 0x33DC30). .text termina em 0x136CC0 (static init 0x136C68, _GLOBAL_$I 0x136CA0), .rodata termina em 0x33DC48, .data termina em 0x30E340.
