---
name: hl1ps2-dlls-done-12
description: continuação da lista de .cpp das dlls feitos (bloater em diante) e próximo arquivo (bigmomma.cpp)
metadata:
  type: project
---

Continuação de [[hl1ps2-dlls-done-11]]. Formato: arquivo (endereço do texto, commit, funções que casam).

- bloater (0x13BE98, b5110dc, 17/17): sem .data, .rodata 0x33F788–0x33FBE8 (o bmodels.o ganhou `--set-section-alignment .rodata=16`). HLSDK puro com floater.dol; os 4 sons vazios são funções de 8 bytes.

Próximo: bigmomma.cpp (info_bigmomma/monster_bigmomma em 0x33F760/0x33F770). .text termina em 0x13BE98 (static init 0x13BAE8, inlines 0x13BB48–0x13BDA0, _GLOBAL_$I 0x13BE78), .rodata termina em 0x33F788, .data termina em 0x30E620.
