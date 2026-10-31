---
name: hl1ps2-dlls-done-6
description: continuação da lista de .cpp das dlls feitos (item_generic em diante)
metadata:
  type: project
---

Continuação de [[hl1ps2-dlls-done-5]]. Formato: arquivo (início .text, commit, funções que casam).

- item_generic (0x176970, 400870e, 12/12): entidade só do PS2, arquivo src/dlls/item_generic.cpp. CGenericItem : CBaseAnimating (0x54: m_lastTime 0x4C, m_iszSequenceName 0x50), sem save próprio, sem .data, .rodata 0x352420 (vtable 0x3524C8). KeyValue sequencename (vazio ou "0" vira 0)/skin ((int)atof)/body (atoi); Spawn com SOLID_NOT, MOVETYPE_NONE, effects e frame 0, StartItem se tem sequência ou spawnflags & 1; StartItem faz DROP_TO_FLOOR com ALERT(at_error,"Item %s fell out of level...") + UTIL_Remove; AnimateThink em loop; Use remove em 0.1s. Registro "117"–"119" em func_00176DF8. A função solta no fim (0x176F18, rendermode/rendercolor/renderamt/renderfx) é um membro não virtual declarado na classe e definido como `inline` depois de tudo: o gcc 2.95 emite mesmo sem uso, depois do static init. Casou de primeira.

Próximo: gauss.cpp (.text termina em 0x176970, _GLOBAL_ em 0x176950; .data termina em 0x312B88; .rodata termina em 0x352420).
