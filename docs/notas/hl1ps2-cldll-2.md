---
name: hl1ps2-cldll-2
description: continuação do HUD do cliente (cl_dll): statusbar.cpp em diante, layout novo do CHud e lições
metadata:
  type: project
---
Continua [[hl1ps2-cldll]]. Fonte do HLSDK do cl_dll baixada em /tmp/claude-0/hlsdk_cl (raw.githubusercontent.com/ValveSoftware/halflife/master/cl_dll/).

- statusbar.cpp (10/11, texto 0x11ED50–0x11F5B8, .rodata 0x3394E0–0x339568, vtable 0x339528): CHudStatusBar em gHUD+0x1A88 (0x238 bytes). PS2: 2 linhas, sem m_pflNameColors/GetClientColor, Init registra hud_centerid só se clientId == 0 (pfnRegisterVariable 0x38), Y_START = `ScreenHeight >= 480 ? H-69 : H-59`, x = 5, y = Y_START - TextHeight*i, MsgFunc_StatusText testa `m_szStatusText[0] == 0` (endereço, bug) para ligar/desligar HUD_ACTIVE, limites `>= MAX`. GetPlayerInfo(idx, &info, 0) = gEngfuncs+0x5C; lista de jogadores dentro do CHud: struct em 0x378 com 0x14 bytes e depois hud_player_info_t[ ] de 0x18 bytes (0x38C); só com o struct aninhado o gcc separa 0x378 + 0x14 como no alvo.
  - Precisa de -fmove-all-movables (o Draw tira gEngfuncs e 0.0f do laço) e de DrawConsoleString/GetConsoleStringSize como funções inline (cl_util.h), não macros.
  - ParseStatusString NON_MATCHING: com a flag o gcc tira &m_iStatusValues do laço de fora (106 instr.); sem a flag faltam 17. Corpo C no #else porque o asm já traz "******" e "%d".
- gEngfuncs (cl_enginefunc_t do PS2): 0x38 RegisterVariable, 0x3C GetCvarFloat, 0x48 HookUserMsg(nome, pfn, clientId), 0x5C GetPlayerInfo, 0x70 TextMessageGet, 0x78 DrawConsoleString, 0x7C DrawConsoleStringLen, 0x80 ConsolePrint, 0x84 CenterPrint(msg, clientId).
- status_icons.cpp (10/10 de primeira, texto 0x11E870–0x11ED50, .rodata 0x339490): CHudStatusIcons em gHUD+0x1D50; PS2: Draw sai se !(iDrawFlags & 0x24), x = 13, sem IsSpectateOnly; EnableIcon(..., clientId) sem o som da granada; stricmp = strcasecmp.
- scoreboard.cpp (271069a, 17/19, texto 0x11CF98–0x11E870, .rodata 0x339398, vtable 0x339450): CHudScoreboard em gHUD+0x378 (0x1600 bytes) com as listas dentro: m_PlayerInfoList[65] 0x14, m_PlayerExtraInfo[65] 0x62C (frags, deaths, damage, shots, hits, teamname), m_TeamInfo[65] 0xCC8, 0x15EC NumTeams…0x15FC ShowscoresHeld; m_fLastKillTime é int. hud_player_info_t: 0x8 = iClient (dono local), packetloss 0xD. Comandos com clientId: DECLARE_COMMAND faz atoi(pfnCmd_Argv(1)) (0x54). CHud: 0x50 m_Teamplay, 0x20EC m_iIntermission, 0x20F0 m_HUD_number_0; DrawHudString 0x11A2A0, DrawHudNumberString 0x11A3B0, DrawHudStringReverse 0x11A460, DrawHudNumber 0x11A570.
  - Precisa de -fmove-all-movables (TeamInfo). No TeamInfo o `j` do for de dentro vaza (usar outra variável k no max/strncpy; o players++ usa o j de fora).
  - Draw e DrawPlayers NON_MATCHING (a ROM converte 154.0f em runtime e repete cvt). As strings SCORE_* usadas pelas duas não migram para o asm: definidas com __asm__(".section .rodata …") antes do Draw dentro de #ifndef NON_MATCHING ("%d%%" já vem no asm do DrawPlayers).
- Próximo: saytext.cpp (antes de 0x11CF98; .rodata 0x339318 "SayText", vtable 0x339358: Init 0x11C8A0, VidInit 0x11C980, Draw 0x11CA00, InitHUDData 0x11C958). Antes dele motd (0x3392B0) e o arquivo do GameTitle (0x339230).
