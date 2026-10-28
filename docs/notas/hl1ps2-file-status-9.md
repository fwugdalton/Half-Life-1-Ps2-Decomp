---
name: hl1ps2-file-status-9
description: Estado de in_camera.c, chase.c, cl_draw.c e cdll_int.c do decomp HL1 PS2 (faixas, tipos, NON_MATCHING) e próximo arquivo (in_camera)
metadata:
  type: project
---
Continua [[hl1ps2-file-status-8]]. Lições em [[hl1ps2-matching-lessons-9]].

- chase.c (texto 0x147660, rodata 0x2891C0): tudo casa. Chase_Init/Reset/TraceLine/Update. symbol_addrs: chase_back 0x331640, chase_up 0x331658, chase_right 0x331670 (0x18 cada), chase_active 0x331688 (size 0x14; 0x18 engolia D_0033169C). D_00331694 virou chase_active.value em gl_rmain/cl_ents.
- cl_draw.c (texto 0x1478F8, rodata 0x289218): tudo em C. SPR_* (0x247A80..0x248128) e crosshair. SPR_Load NON_MATCHING. "engine/cl_draw.c" é D_00389218 extern + INCLUDE_RODATA antes do SPR_Load.
- cdll_int.c (texto 0x1468C0, rodata 0x289060, -ffast-math no Makefile; 2026-10-12, commit 6330a45): 23 de 24 casam (hud* do cl_enginefuncs, ClientDLL_Init/Shutdown/HudVidInit/HudInit/HudRedraw/UpdateClientData/Reset, SPR_GetList). NON_MATCHING: 246B10 hudGetPlayerInfo (o lb de name[0] usa ent*0x3C8 em vez de (ent-1)*0x3C8).
- Tipos locais no cdll_int.c: cldll_func_t D_0046E4F8 (0x18: Init, HudInit 4, HudVidInit 8, Redraw 0xC (time, intermission, clientId, unk), UpdateClientData 0x10 (cdata, time, clientId), Reset 0x14 (clientId)), client_data_t 0x40, client_sprite_t 0x98, hud_player_info_t, SCREENINFO 0x214. D_003332E0 float[2] = fov por cliente; D_00331844 = sensitivity.value.
- Funções do DLL do cliente ficam em 0x115D18.. (asm/cod); o ponteiro do Reset recebe clientId (descoberto lendo func_00115F78).
- in_camera.c (texto 0x1456F8, rodata 0x288E58; commit ca7044d): MoveToward 2456F8 (3º arg maxspeed não usado), CAM_Think 245848 (igual ao SDK, casou de primeira), Key Down/Up, ToThirdPerson/FirstPerson/ToggleSnapto, CAM_Init, ClearStates. NON_MATCHING: 246768 StartMouseMove e 246808 StartDistance (o else final deixa o delay slot vazio; com iMouseInUse volatile casam, mas aí os End* quebram). symbol_addrs: cvars cam_command..c_mindistance 0x331498..0x3315A0 (0x18 cada), iMouseInUse 0x3315B4, kbuttons cam_pitchup..cam_out 0x3AC438..0x3AC488. D_0039970C/D_003996D8 = centro da janela; D_00399118 = posição do mouse.
- Próximo: build.c (build_number 0x2455D8; meses em rodata 0x388DD8..0x388E57) e o que vem antes (PM_CalcRoll 0x245500, Info_* ~0x245170).
