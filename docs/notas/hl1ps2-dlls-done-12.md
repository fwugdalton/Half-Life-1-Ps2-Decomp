---
name: hl1ps2-dlls-done-12
description: continuação da lista de .cpp das dlls feitos (bloater, bigmomma) e próximo arquivo (barney.cpp)
metadata:
  type: project
---

Continuação de [[hl1ps2-dlls-done-11]]. Formato: arquivo (endereço do texto, commit, funções que casam).

- bloater (0x13BE98, b5110dc, 17/17): sem .data, .rodata 0x33F788–0x33FBE8 (o bmodels.o ganhou `--set-section-alignment .rodata=16`). HLSDK puro com floater.dol; os 4 sons vazios são funções de 8 bytes.

- bigmomma (0x138728, 7913a6f, 55/55): .data 0x30E478, .rodata 0x33E7F0–0x33F781. Mudanças do PS2: BIG_MAXCHILDREN 7 (era 20); Touch do morteiro com RANDOM_LONG(90,110); sqrt→sqrtf; VecCheckSplatToss sem os locais vecScale/vecTemp (pilha 32 bytes menor); o 2º trace do VecCheckSplatToss e o do Touch são UTIL_TraceLineNoEnts. Registro func_0013B8D0: "027" CBMortar::Animate (entre o MortarSpray/Touch e as cópias de GetClassPtr).
- ATENÇÃO: o cmpfunc não acusa alvo de jal diferente (UTIL_TraceLine vs UTIL_TraceLineNoEnts tem o mesmo código de chamada); só o build completo pega. Quando o ROM falhar com poucos bytes, ver o jal com objdump no .elf e `od -An -tx4 -j OFF -N4 SLUS_200.66.rom` (não há xxd).

Próximo: barney.cpp (monster_barney/monster_barney_dead em 0x33E7C0/0x33E7D0). .text termina em 0x138728 (static init 0x1386B0, _GLOBAL_$I 0x138708), .rodata termina em 0x33E7F0, .data termina em 0x30E478.
