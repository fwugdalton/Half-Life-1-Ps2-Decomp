---
name: hl1ps2-file-status-8
description: Estado do cl_ents.c do decomp HL1 PS2 (funções, tipos novos, NON_MATCHING) e próximo arquivo (cl_draw?)
metadata:
  type: project
---
Continua [[hl1ps2-file-status-7]]. Lições em [[hl1ps2-matching-lessons-9]].

- cl_ents.c (2026-10-12, texto 0x148148, rodata 0x389280; commits f2fefca..1170cd0): todo em C. Casam: delta/header/log, ParseDelta(Custom), 248F40, 249070, 249868 (debug da entidade), 249B50 (estado→cl_entity), 249CE8 CL_Particle, 249D98 CL_PlayerFlashlight, 24ABF8 (histórico de previsão), 24C060 CL_MoveAiments, 24C120 CL_EmitEntities. NON_MATCHING: 2490B0 CL_ParsePacketEntities (clientId vai para s7, original deixa na pilha), 24A160 CL_LinkPacketEntities (só o início do laço: original salta para o teste de cl_numvisedicts no fim), 24AD40 CL_ParsePlayerinfo (clamp `if (x > 100)` do byte unk_EA vira movn), 24B920 CL_LinkPlayers (regalloc), 24BD88 CL_SetSolidEntities (ordem de içamento das constantes 1/3/0xFFF0/0x58).
- Tipos: player_state_t completo em client.h (0xF8: command 0x0C, predorigin 0x34, deltaorigin 0x40, movetype..pmflags 0x70-0x88, render 0x9C-0xA8, sequence/animtime 0xAC/0xB0, controller 0xBC, blending 0xC0, basevelocity 0xD0, friction 0xDC). client_state_t: lerpfrac 0x2C0, velocity 0x300, flashlights[3] 0x15B8. cl_entity_t: unk4C, syncbase 0x64, unk70, unk18C; unk194 = effects. cl_packet_entities_t.entities é entity_state_t*. player_info_t: unk_E8 short + unk_EA byte.
- Globais: D_003AC498 int[2][166] (bits de entidades vistas), D_00399198/D_003991A0 old visedicts, D_00473C58 buffers de visedicts, máscaras PF_ D_00399354..D_003993E4 (iguais ao sv_main.c).
- Próximo: ver [[hl1ps2-file-status-9]] (chase, cl_draw, cdll_int).
