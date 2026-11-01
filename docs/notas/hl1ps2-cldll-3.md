---
name: hl1ps2-cldll-3
description: cl_dll do PS2 de saytext.cpp para trás (parsemsg, motd...): faixas, truques e próximo arquivo
metadata:
  type: project
---
Continua [[hl1ps2-cldll-2]].

- saytext.cpp (7a9729a, 11/11, texto 0x11C858–0x11CF98, .rodata 0x339318, vtable 0x339358, .data 0x30D460–0x30D470): CHudSayText em gHUD+0x1CD8 sem campos; buffer g_szLineBuffer[6][256] global no .bss (D_003A2C30, um só para os 2 jogadores), sem cores/nomes/gViewPort. .data: flSayTextTime = 5 (lido do cvar hud_saytext_time só quando chega a linha 0), flScrollTime, Y_START, line_height (todos `static ... = 0` explícitos ficam no .data). Draw sai se !(flags & 0x24), y = Y_START + 24. SayTextPrint é void; strncpy com max(iBufSize - 1, 255); PlaySound = gEngfuncs+0x60 (nome, vol, clientId). Truque: `if (ScreenHeight < 480) Y_START = H-35; else Y_START = H-45;` e depois `Y_START = Y_START - line_height*6` (ternário não casa). memmove = func_002E46A4.
- parsemsg.cpp (b9e7150, 11/11, texto 0x11C4E0–0x11C858): igual ao HLSDK; gpBuf/giSize/giRead/giBadRead em 0x3A2C20..2C (.bss), string do READ_STRING em 0x3A2420. READ_COORD etc. com constantes float (1.0f/8).
- motd.cpp (4e526a3, 7/8, texto 0x11C010–0x11C4E0, .rodata 0x3392B0, vtable 0x3392D8): CHudMOTD em gHUD+0x1978 (m_szMOTD[241] 0x14, m_flActiveTill 0x108, m_iLines 0x10C). PS2: cvar só no jogador 0; Draw roda se (flags & 4) ou flags != 0, m_flActiveTill vira contagem regressiva (desconta min(max(dt,0),1) com a hora anterior em D_003A2418); MsgFunc usa `MOTD_DISPLAY_TIME = (cvar >= 10) ? (int)cvar : 10;` (o ternário casa o trunc+mfc1+sw; if/else sai swc1) e m_flActiveTill = MOTD_DISPLAY_TIME (D_003990B4). Precisa de -fmove-all-movables. Draw NON_MATCHING (o gcc tira gHUD+0x1ED4 do laço).
- Próximo: o arquivo antes de 0x11C010 (.rodata 0x339230 "GameTitle", title_half/title_life/GAMETITLE, vtable 0x339274: Init 0x11AE20, VidInit 0x11AED0, Draw 0x11B968, Reset 0x11AF38, InitHUDData base). É o message.cpp (CHudMessage) do HLSDK.
