---
name: hl1ps2-file-status-2
description: Estado dos arquivos .c do decomp HL1 PS2 de view.c (0x28BF00) em diante, incluindo cl_parse.c e o próximo arquivo a separar
metadata:
  type: project
---
- view.c (0x28BF00–0x28E6D8, .rodata 0x390340–0x3905E8) todo em C; NON_MATCHING: V_CalcBob, V_UpdatePalette, V_CalcIntermissionRefdef, V_CalcRefdef. Nomes de cvar ficam via INCLUDE_RODATA + `extern char D_...[]` (strings também usadas pelo .data em asm).
- wad.c (0x28E838–0x28EB80) 100%.
- world.c (0x28EB80–0x292150, -ffast-math) todo em C; NON_MATCHING: SV_TouchLinks, SV_LinkContents, SV_ClipToWorldbrush.
- zone.c (0x292150–0x294268) todo em C; NON_MATCHING: Hunk_Print, Cache_Print, Hunk_PrintDetails, Cache_Print_Sounds_And_Totals.
- ui.c (0x294268–0x294518) 100%.
- cl_parse.c (texto 0x294518–0x2979A0, .rodata 0x391120–0x391EE8) todo em C (2026-10-10, commit 8018f92). Só CL_ParseServerMessage é NON_MATCHING (tamanho igual, diferenças de escalonamento/registrador no somatório de bytes por comando). Os svc_* do PS2 estão definidos no topo do arquivo (svc_choke 44, svc_resourcelist 45... até 53).
- Layouts novos: entity_state_t em include/engine/entity_state.h (compartilhado por server.h e cl_entity.h; baseline em cl_entity_t+0xF8); frame_t (0x330) e muitos campos de client_state_t em client.h (stats[32] em 0x238, frames 0x3AC, levelname 0x13B8...); incomingtransfer/soundfade/latency em client_static_t (cmd.h).
- Funções vazias do mesmo arquivo chamadas com argumento: declarar com o parâmetro (`void f(int clientId) {}`), o corpo gera o mesmo código.
- Nomes novos: MSG_ReadShort 0x243400, MSG_ReadFloat 0x243528, S_PrecacheSound 0x2CC0E8, SCR_CenterPrint 0x2ABA40, COM_ExplainDisconnection 0x241D40, cvars r_decals/mp_decals/gl_zmax/cl_skyname/gl_wateramp/cl_downloadinterval.
- Host_cmd.c (texto 0x2979A0–0x29F030, .rodata 0x391EE8–0x393490) todo em C (2026-10-10, commit a1386ff). NON_MATCHING: Host_Load, LoadGamestate, CreateEntityTransitionList (`lui 0x8000; and; bne` vs bltz), Host_FullInfo_f (constantes 0x8420/0x8000 içadas pro laço no original). client_t ganhou download/downloadsize/downloadcount/downloadCRC (0x853C..). 0x393230/0x393330/0x393430 marcados `can_be_referenced:False` (falsos ponteiros "029"/"039"/"049" no .data do cod).
- Gl_model.c (texto 0x29F030–0x2A0450, .rodata 0x393490–0x393660, -ffast-math) todo em C (2026-10-10, commit 00c6172). NON_MATCHING: Mod_Report_f, Mod_FindName (permuter não zerou, melhor 115), Mod_SetupSubmodels (constante 80 içada). Header .bs2 = bs2header_t (ofs 0x04..0x3C, contagens 0x84..0xB8) / bs2lumps_t {version; ofs[16]; len[16]}; sprite PS2 = gl_sprite_t (flags@6, numframes@7, frames[]{texnum, frame*}).
- gl_rsurf.c em diante: ver [[hl1ps2-file-status-3]].
- Pendências antigas: 0x277B58–0x277EB8 em asm; cmodel 7/9, Console 20/21, decals 12/13, Host 42/47, keys 18/19.

Ver [[hl1ps2-file-status]] e [[hl1ps2-matching-lessons-2]].
