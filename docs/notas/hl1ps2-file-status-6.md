---
name: hl1ps2-file-status-6
description: Estado do som (snd_dma.c, snd_mix_gbx.c) no decomp HL1 PS2 e próximo arquivo (common/filesystem 0x2CF408)
metadata:
  type: project
---
Continua [[hl1ps2-file-status-5]] (faixas de texto/rodata dos arquivos de som estão lá). Lições em [[hl1ps2-matching-lessons-7]].

- snd_dma.c (2026-10-12, commit d382914): todo em C. NON_MATCHING: func_002CB7B0, PickChannel CC1E8, PickStaticChannel CC3B8, SND_Spatialize CC510, CC7A8, S_StartStaticSound (só v0/v1), S_Update_ CDA48 (agendamento do prólogo). Próximo: snd_mix_gbx.c (0x2C72C8).
- snd_mix_gbx.c (2026-10-12, commit c1125c0): todo em C. NON_MATCHING (11): CheckWavEnd 7788, MixChannelsToPaintbuffer 7A20, PaintChannels 7FF8, SX_Init 84C0, 8F28, 9980, RoomFX 99B0, 9C98, VOX_ParseString CA050, VOX_MixChannelPitch CAB28 (só o lui da string), VOX_TrimStartEndTimes CB290 (regalloc). channel_t.unk_42/unk_43 são char (lb). Próximo: common/filesystem em 0x2CF408 (asm/cod/1CF408).
- HlFileIO.c (texto 0x1CF408, rodata 0x296220; commits d788553, 19714c6): todo em C, só func_002CF448 (strcasecmp que ignora / e \) NON_MATCHING. Tipos pack_t/searchpath_t/packfile_t em include/engine/hlfileio.h.
- HlCdFileIO.c (texto 0x1CFF88, rodata 0x296340; commit 19714c6): todo em C e casa (COM_LoadFile, func_002CFF88 busca em pak, func_002D0368 COM_LoadPackFile, func_002D04E8). Lição: `if (f()==1) {...} else pack = NULL; return pack;` deu o `bne; daddu s0,0` no delay slot.
- Próximo: asm/cod/1D0520 (cvars de input m_pitch etc., rodata 0x2963E8).
- pad_cvars.c (texto 0x1D0520, rodata 0x2963E8; commit 1c6fe1e): func_002D0520 casa. De 0x1D0A68 em diante é SDK Sony (libgraph, libpad, libcdvd, libmc, libscf), libm e libstdc++: não descompilar.
- Falta o bloco asm/cod/000000 (0x000000-0x1548B8, ~6300 funções: código cliente/GL), ordem alfabética por grupo. Indo para TRÁS a partir do Cmd.c: cl_tent.c separado (texto 0x1548B8, rodata 0x28A760, commit 51969c5). Antes dele: cl_spectator? (ptrack, ~0x252A40), Cl_main.c (CL_Init 0x251E78), cl_input (0x24E700). Script $S/rorange.py lo hi acha a faixa de rodata de um intervalo.
- Strings de cvar na .rodata vêm VALOR antes do NOME ("6000","tracerspeed"); o gcc funde literais iguais no mesmo arquivo, então dois "0" seguidos de nomes = arquivos diferentes.
- Arquivos do cliente (cl_tent, cl_cam, cl_pred, Cl_main) em [[hl1ps2-file-status-7]].
