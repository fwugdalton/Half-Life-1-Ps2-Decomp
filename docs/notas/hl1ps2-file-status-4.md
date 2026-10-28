---
name: hl1ps2-file-status-4
description: Estado do decomp HL1 PS2 de gl_rmisc.c (0x2AFE50) a r_part2.c e o próximo arquivo (r_studio)
metadata:
  type: project
  modified: 2026-10-12T00:10:14.266Z
---
Continua [[hl1ps2-file-status-3]]. Lições em [[hl1ps2-matching-lessons-5]].

- gl_rmisc.c (0x2AFE50–0x2B0600) 100% (GL_Info_f, R_InitParticleTexture, R_NewMap, R_TimeRefresh_f, D_FlushCaches).
- r_part.c (0x2B0600–0x2B15D0, rodata 0x294E30, -fmove-all-movables) todo em C; NON_MATCHING só R_EntityParticles (0x2B0A48).
- r_part2.c (0x2B15D0–0x2B9100, rodata 0x294EE0–0x294FA0, sem flags) quase todo em C (2026-10-12). Partículas, tracers, feixes (BEAM 0x7C completo, listas D_00414290 ativo / D_00414298 livre por jogador), R_DrawSegs/Torus/Disk/Cylinder/Ring/BeamFollow, ScreenTransform, R_BeamDraw (jtbl do gcc + `.align 4` no fim do arquivo).
  - NON_MATCHING: R_LavaSplash 2128, 2598, R_BloodStream 2938, R_Blood 2C70, R_PartBeginTri 3680, R_DrawTracers 3EE8, R_BeamKill 5228, R_DrawSegs 57C8, R_DrawDisk 61D8, R_DrawRing 6CF0, R_DrawBeams 7CA8, 7E98 (funil de partículas com AngleVectors).
  - Faltam em asm: 0x2B82F0 (0x924, desenho de feixe com D_003994E0..EC e func_0023AF68) e 0x2B8C18.
- Fatos: cl[c].worldmodel 0x13EC; cl_entities é `cl_entity_t *cl_entities[2]`; PVSNode(node, mins, maxs, clientId); fmod double = 0x2D78A0; rgNoise D_0050F188[129]; cvar D_00331BE8 (faíscas de feixe); 0x1C da entidade de feixe = modelindex.
- Próximo: r_studio separado; ver [[hl1ps2-file-status-5]].
