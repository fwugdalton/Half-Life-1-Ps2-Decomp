---
name: hl1ps2-dlls-tentacle
description: tentacle_decay.cpp (CTentacleDecay, faixas, mudanças do PS2, lições) e dicas para o tentacle.cpp antes dele
metadata:
  type: project
---
dlls/tentacle_decay.cpp feito em 2026-10-20 (commit b2b55cc), 31/31 funções casam, sem NON_MATCHING.
- Faixas: texto 0x1F5F68–0x1F8190 (rom 0xF5F68), .data 0x323E88 (0x138), .rodata 0x3794B0 (0xA58). Statics g_fFlySound/g_fSquirmSound = D_003990C0/C4 (.bss, extern "C").
- É cópia do CTentacle do HLSDK (classe própria, deriva de CBaseMonster, tamanho 0x2F0). PS2: takedamage DAMAGE_YES, modelo .dol, campo novo 0x2EC (vira para o m_pCine em script e chama LookupSequence(m_iszPlay)), DieThink manda TE_BLOODSTREAM, BleedThink novo (evento 1003: SetBodygroup(1,1), SetThink(BleedThink), SetTouch(NULL)), precache das listas por m_iTapSound.
- Registro func_001F7ED0: 280 BleedThink, 281 DieThink, 282 Cycle, 283 Start (Think), 284 HitTouch, 285 CommandUse.
- Lições: switch(RANDOM_LONG(0,1)) com `case 0: ...; default:` (não `case 1`) gera o bnez do alvo; cosf/sinf com `((float)M_PI / 180.0f)`; `if (pev->angles.y == pev->ideal_yaw)` (ordem dos operandos muda os lwc1); BloodStream com v_right copiado num Vector local; Test: `entvars_t *p = pev; p->framerate = 0; StudioFrameAdvance(p->framerate);` (sem recarregar pev).
- fixasm.py: quando mtc1 + rótulo + leitura põe o nop no rótulo, um li.s/mtc1 logo antes também vira .word (senão o gas põe nop entre os dois).
- tentacle.cpp feito em 2026-10-20 (commit 59c2dce), 34/34: texto 0x1F3E38–0x1F5F68, .data 0x323D50, .rodata 0x378600 (0xEA4 + 0xC de padding; por isso o tentacle_decay.o tem `--set-section-alignment .rodata=16` no Makefile). Statics D_003990B8/BC. Diferenças do decay: MOVETYPE_NONE no Spawn, script do HLSDK, `char *sound = NULL;` e `case 1:` nos switch de som, CTentacleMaw (models/maw.dol). Registro func_001F5C18: 275 DieThink, 276 Cycle, 277 Start, 278 HitTouch, 279 CommandUse.
Ver [[hl1ps2-dlls]].
