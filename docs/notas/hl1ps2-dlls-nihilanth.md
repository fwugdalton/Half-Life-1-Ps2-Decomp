---
name: hl1ps2-dlls-nihilanth
description: dlls/nihilanth.cpp (faixas, mudanças do PS2, NON_MATCHING) e o padrão de div com break depois que o gcc não reproduz
metadata:
  type: project
---
nihilanth.cpp feito em 2026-10-25 (commit 72647e9). Texto 0x1ACFE0–0x1B36A0, .data 0x314FC8, .rodata 0x3663F0–0x367348. 53 de 59 funções casam.

Mudanças do PS2:
- Attachments deslocados em -2: GetAttachment 2→0 e 3→1 (DyingThink usa iAttachment-1 com 3 fixo no TE_ELIGHT), entindex()+0x3000/0x4000 viram 0x1000/0x2000, SetAttachment(edict(), 4) no NextActivity.
- DyingThink: o switch tem a ordem case 1 left hand, 2 right hand, 3 eyes, 4 head.
- Flight: sem flSpeed e flDist, mas a linha do flDir continua (as Vector temporárias ficam na pilha). fabs vira fabsf (abs.s).
- TargetSphere: pSphere = NULL, `if (m_hSphere[i])` e `if (!pSphere->m_hEnemy)` (operator int). GetAttachment(0).
- CommandUse: `if ((CBaseEntity *)m_hEnemy == NULL) UTIL_PlayerByIndex(1);` com o resultado descartado (o cast escolhe o operator CBaseEntity* que o ROM chama), depois `if (pTouch && m_hEnemy) pTouch->Touch(m_hEnemy)`.
- HoverThink: RANDOM_LONG(0,99) sem if (os efeitos estão comentados).
- Registro func_001B3290: "194".."206", sem o "202".

NON_MATCHING: ShootBalls (ordem das constantes içadas), MakeFriend (escalonamento), TraceAttack (escalonamento dos args do UTIL_BloodStream) e HoverThink, ZapThink e TeleportThink (div/% com o beql+break DEPOIS do div e a constante carregada junto; o nosso gcc agenda o trap antes. Não é o macro rem do gas nem -fno-schedule-insns; mesmo problema do CALC_RANGE do nodes).

Dica: o cmpfunc não pega troca de alvo de jal entre __opi__7EHANDLE e __opP11CBaseEntity__7EHANDLE; o CONFLITO do resolveundef pega.

Próximo: multiplay_gamerules.cpp (texto termina em 0x1ACFE0, .rodata termina em 0x3663F0, .data termina em 0x314FC8). Ver [[hl1ps2-dlls]] e [[hl1ps2-dlls-nodes]].
