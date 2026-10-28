---
name: hl1ps2-file-status-7
description: Estado dos arquivos do cliente (cl_tent, cl_cam, cl_pred, Cl_main) do decomp HL1 PS2 e próximo (cl_input)
metadata:
  type: project
---
Continua [[hl1ps2-file-status-6]]. Lições em [[hl1ps2-matching-lessons-8]].

- cl_tent.c (2026-10-12, commits e972604, 285102e): todo em C. NON_MATCHING: 2579E0, 256F20, 25B740, CL_FxBlend, 25CA10, CL_ParseTEnt 258778 (só regalloc: usa s7 e o original s0-s6), e as que dependem de içamento de constantes (254F40, 2551E8, 255460, 2556A8, 2558C0, 255DB8, 256478, 256668, 2568F8). Funções em #ifdef NON_MATCHING precisam de protótipo separado no topo, senão o gcc trata como sem protótipo (li.d em GPR). Próximo: arquivo antes do cl_tent (spectator ~0x252A40, Cl_main CL_Init 0x251E78, cl_input 0x24E700).
- cl_cam.c (2026-10-12, commits f459b8b, 246e0db; texto 0x152868, rodata 0x28A6A8, -ffast-math): câmera de espectador do QW (vectoangles, Cam_Lock/Unlock, TryFlyby, InitFlyby, Cam_Track, Cam_FinishMove) com modos 2/3 do PS2. Tudo casa menos 2537C0 (interpolação de ângulos, só regalloc de float). Globais: D_003991C0 autocam, D_003991B0 spec_track, D_003ACCE8 locked, D_003ACCF0 oldbuttons, D_003ACCD0 desired_position, D_003991A8 cam_lastviewtime, D_00331B28 cl_hightrack. cl.paused (0x3B4) renomeado para cl.spectator.
- cl_pred.c (texto 0x1524F8, rodata 0x28A690 = cvar cl_dumpents; commit c82cdf8): CL_PredictMove e CL_InitPrediction casam. 0x252818 (zera locked/spec_track/autocam) foi para o cl_cam.c.
- Cl_main.c separado (texto 0x14EFB8 a 0x1524F8, rodata 0x289A80; commit f44cccd), 41 funções em asm. 0x24EFB8 calcula o soundfade (cls+0x7F00). Antes: cl_input.c (CL_InitInput 0x24E700). Falsos ponteiros 0x352E30 ("0.5") e 0x353530 ("255") marcados can_be_referenced:False.
- Cl_main.c (2026-10-12, commits 57040a5..9ffb9e6): todo em C. NON_MATCHING: 24EFB8 soundfade (regalloc float; divide por soundFadeOutTime nas duas fases, bug do original), CL_ReadPackets 24FB48 (cls+4 içado), CL_SendCmd 251710 (0x330 em s8). frame_t ganhou usercmd_t cmd e delta_sequence; client_state_t.cmd (0x214) substitui forwardmove; cls.challenge 0x73BC e unk_73C0 (tipo de auth: 1=WON, 2=cdkey). Próximo: cl_input.c (CL_InitInput 0x24E700).
- cl_input.c (2026-10-12, texto 0x14C500, rodata 0x2895E0, -ffast-math pelo sqrt.s do CL_BaseMove; commits 28ddd7b..c2ea7e5): todo em C e tudo casa. kbutton_t[2] por jogador (Cmd_Argv(1) = clientId); nomes dos botões em comentários nos externs. IN_CANCEL/LEFT/RIGHT/RELOAD/ALT1/SCORE em pmove.h. cl_ents.c em [[hl1ps2-file-status-8]].
