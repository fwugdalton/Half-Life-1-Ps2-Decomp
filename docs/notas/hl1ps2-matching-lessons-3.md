---
name: hl1ps2-matching-lessons-3
description: Lições de matching do ee-gcc 2.95 vindas do cl_parse.c do HL1 PS2 (switch, delay slot, variável local constante, ordem de stores)
metadata:
  type: project
---
- Ordem dos `case` no fonte = ordem dos blocos no binário; `default` pode vir primeiro. Ler os alvos da jtbl por endereço e reordenar os cases (CL_PrintResource, CL_ParseResourceList, CL_ParseServerMessage).
- Load incondicional no delay slot de um `bne` (não bnel) = atribuição feita ANTES do if no fonte (DispatchUserMsg: `iSize = msg->iSize; if (!found) {...}`).
- Constante 1 rematerializada num registrador próprio a cada volta do laço em vez de reaproveitar o valor comparado: variável local `world = 1` atribuída no topo do laço (CL_ResourceBreakdown).
- `sfx = (sfx_t *)name;` ANTES do strcpy/strcat fez o ponteiro ficar no registrador salvo (CL_ParseStartSoundPacket).
- Ponteiro local `p = &arr[i]` no laço muda a ordem dos incrementos; casou CL_ParseNewUserMsg.
- Pares de stores (pNext/pPrev) saem na ordem inversa da esperada: testar as duas.
- `if (a || b) return false;` reaproveita o endereço base; dois ifs separados não (CL_RequestMissingResources).
- Argumento de chamada vindo de variável própria (`gamedir = MSG_ReadString(); if (!clientId) f(gamedir);`) mudou o preenchimento do delay slot (CL_ParseServerInfo).
- Ternário direto no argumento do printf casou CL_DumpMessageLoad_f; `x = K; if (c) x = y;` não.
- Variáveis globais vizinhas acessadas com lui/lo separados são variáveis distintas, não um array (cmdlast do CL_ParseServerMessage).
- Repetir a expressão `cl.frames[i & mask]` em vez de ponteiro local aproximou o svc 53.
- Mesmo registrador para `valid` e para o contador do laço: reutilizar a MESMA variável (SV_Rcon).
- `lbu/sb` de 1 byte vindo de string "" compartilhada: `memcpy(dst, D_x, 1)` (Host_Map).
- Bloco de erro com `goto` que aparece logo após o `return 0` inicial: `if (0) { label: ...; goto done; }` ali no topo (SaveGameSlot).
- Padrão `a1 = i + 1` em cada ramo e `i = a1` no fim do laço: usar variável `next` explícita (`for (...; i = next)`).
- Ordem de stores no fim de função: testar permutações das linhas com trybody (SaveInit casou na 13ª).
- `flags = 0; if (!f()) flags |= C;` gera `lui C` antes da chamada + `movn` (LoadAdjacentEntities); `x = f() ? 0 : C` dá movz.
- Variáveis distintas para usos distintos (blocksize vs size) mudaram a alocação e casaram Host_Nextdl_f.
- `switch (id) { case 0: ...; case 1: ...; }` dá `beqz` + `beq id,1`; if/else-if inverte (Host_SetInfo_f, Host_Pause_f).
- Ao virar literal uma string que o splat partiu em dois labels (".word" + resto), apagar o INCLUDE_RODATA do pedaço final (D_00392E30).

Anterior: [[hl1ps2-matching-lessons-2]].
- Laço que no original faz `addiu aX, base, off` ANTES do teste de contagem: guardar a base numa variável antes do laço (`frames = psprite->frames; for (...) frames[i]...`). Gl_model sprites.
- Argumentos de ponteiro (palette/pixels) calculados antes de a0/a1 no original: guardá-los em locais antes da chamada anterior, na ordem inversa dos registradores (`palette = ...; pixels = ...;`).
- `& 3` vs `% 4` em diferença de ponteiros muda o registrador; `ptr >= base + ofs` sem variável `start` deixa o gcc reusar o registrador do argumento.
