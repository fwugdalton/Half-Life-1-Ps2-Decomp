---
name: hl1ps2-file-status-3
description: Estado do decomp HL1 PS2 de gl_rsurf.c (0x2A0450) em diante: gl_rsurf, gl_rmain, gl_rlight e o próximo arquivo a separar
metadata:
  type: project
---
- gl_rsurf.c (0x2A0450–0x2A8358, -ffast-math -fmove-all-movables) todo em C (2026-10-11). NON_MATCHING: R_AddDynamicLights, func_002A76F8, func_002A7238/6BE0/6CA0 (envio VU), func_002A7AF8 (desenho VL), AllocBlock, R_BlendLightmaps, R_RenderBrushPolyChain, R_DecalNode.
- gl_rmain.c (texto 0x2A8358–0x2AB0E8, .rodata 0x2944F8–0x294650, sem flags extras) todo em C (2026-10-11, commit b15a511). NON_MATCHING: R_PreDrawViewModel (1 instr., CSE de &cl[i]), R_DrawEntitiesOnList (endereços de cl[] içados do laço), R_DrawSpriteModel (troca f23/f24). Viewports por jogador: D_0032EF78.. (tela cheia, ordem y,x,w,h), D_0032EF90/EFA0 e EFD0/EFE0 (split), EFB0/EFC0 e EFF0/F000 (R_SetupGL); D_00399564 = modo widescreen (16:9).
- gl_rlight.c (0x2AB0E8–0x2ABA40, -fmove-all-movables) 100% (R_AnimateLight, R_MarkLights, R_PushDlights, RecursiveLightPoint, R_LightVec, R_LightPoint). cl[0].worldmodel aparece como D_0055746C no asm.
- Funções GL do bloco asm 000000.s nomeadas: glMatrixMode 0x23B1B8, glPushMatrix, glPopMatrix, glLoadIdentity, glFrustum, glOrtho, glViewport 0x23B7B8, glDepthRange, glDepthFunc, glVertex2f, glTranslatef, glRotatef, glGetFloatv, glColor4ubv, glColor4ub 0x23C0E0; tanf 0x2D7700, atanf 0x2D7178.
- cl_entity_t ganhou renderfx 0x18, movetype 0x1C, sequence 0x24, colormap 0x30, skin 0x34, aiment 0x44, framerate 0x5C, animtime 0x68, prevanimtime 0x74, prevorigin 0x9C, prevangles 0xA8. client_state_t: light_level 0x38C. frame_t: playerstate[3] (0xF8 cada) em 0x34.
- gl_screen.c (0x2ABA40–0x2ACFA8, .rodata 0x294650–0x294798, -ffast-math) todo em C. NON_MATCHING: SCR_CalcRefdef (só o delay slot do último sw), func_002AC1B0, SCR_DrawConnectMessage.
- gl_draw.c (0x2ACFA8–0x2AFE50, .rodata 0x294798–0x294A08, sem flags) todo em C, nomes Draw_*/GL_*. NON_MATCHING: Draw_ColoredCharacter, Draw_Character, Draw_GetAlphaFontChar, Draw_ScaledCharacter (registradores float), Draw_SubPicColored, GL_LoadDecalTexture (= GL_LoadTexture; spill de nummips+1), GL_BuildDecal. Header novo include/engine/glquake.h (qpic_t psi, gltexture_t 0x48, defines GL_*). D_00394938 precisa de `.align 3` antes do INCLUDE_RODATA.
- Daqui em diante: ver [[hl1ps2-file-status-4]] (gl_rmisc, r_part, r_part2).

Ver [[hl1ps2-file-status-2]] e [[hl1ps2-matching-lessons-4]].
