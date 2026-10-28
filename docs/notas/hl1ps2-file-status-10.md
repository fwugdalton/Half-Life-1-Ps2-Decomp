---
name: hl1ps2-file-status-10
description: Estado de build.c e common.c do decomp HL1 PS2 (faixas, flags, NON_MATCHING) e próximo arquivo (Gbx C++ antes de 0x241578)
metadata:
  type: project
---
Continua [[hl1ps2-file-status-9]]. Lições em [[hl1ps2-matching-lessons-10]]. Flags em [[hl1ps2-compiler-flags]].

- build.c (texto 0x1455D8, rodata 0x288DE0): build_number casa (externs D_00331450 date, D_00331458 mon[12], D_00331488 mond[12], D_00331494 b; `(int)((y - 1901) * 365.25f)`).
- common.c (texto 0x141578..0x1455D8, rodata 0x288AA8; Makefile: `-ffast-math -fmove-all-movables`, 2026-10-12, commit eb7fff8): tudo em C. Só func_00244D88 (Info_FindLargestKey) é NON_MATCHING (troca de registrador s3/s4 entre largestKeySize e %hi do buffer estático).
  - Ordem: Info_Serverinfo, links, Q_str*, swaps, MSG_Write*/Read*, bit-ops (bf_write D_003AC410, bf_read D_003AC420), SZ_*, COM_Parse etc., COM_InitArgv, va/vstr, Info_ValueForKey 244978, Info_RemoveKey 244AC8, Info_RemovePrefixedKeys, Info_IsKeyImportant 244CF0, Info_FindLargestKey 244D88, Info_SetValueForStarKey 244EE0, Info_SetValueForKey, Info_Print, WON auth 245310 (D_003997CC/D_003997B0 ponteiros de função; soma em cls[0].connect_time), COM_EntsForPlayerSlots 245408, nome sanitizado 245420 (D_003AC318[9], "Noname"), PM_CalcRoll.
  - Strings compartilhadas usadas por extern: D_00388B40 "%s", D_00388B48 "", D_00388C40 "\\", D_00388C68 "name".
- snd_win_gbx.c (2026-10-12, commit 5e4bc33; Makefile `-fmove-all-movables`): tudo em C. 2CE938 (troca de buffer de stream) casa; 2CE380 (SNDDMA_Submit: reamostra 22050→48000 com passo 0.17687f) e 2CF080 (aloca blocos/streams) NON_MATCHING. Tipos locais sndstream_t (0xDC, D_00465898[2], D_00465A50 = atual) e sndhdr_t (0x10). D_00334EAC/D_00334EAE são dois u16 separados (D_00334EAE no linker_script_extra.ld).
- r_part2.c (commit a3c1453): 2B82F0 = R_DrawSegs do PS2 (laços separados por SHADEIN/escala/SINENOISE, cada um chama 2B8C18) e 2B8C18 = emite um segmento direto nos buffers D_003341A4/A8/AC (qvec_t, troca em 72 vértices). Ambos NON_MATCHING; o arquivo não usa -ffast-math, então o sqrt.s de 2B8C18 sai com `__asm__("sqrt.s")`.
- 0x277B58..0x277EB8 virou src/engine/unk_277B58.c (wrapper de Host_WriteConfiguration + mapa de bits dos botões por tipo de controle 0x23).
- Próximo: antes de 0x241578 é código C++ do Gbx (Texture.cpp ~0x2403F8); decidir se segue para trás nele ou pula para outro bloco de C do engine.
