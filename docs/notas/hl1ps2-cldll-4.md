---
name: hl1ps2-cldll-4
description: cl_dll do PS2 de menu.cpp para trás (hud_update, hud_redraw, hud_msg): faixas, truques e próximo arquivo (hud.cpp)
metadata:
  type: project
---
Continua [[hl1ps2-cldll-3]]. A ordem dos arquivos do cl_dll na ROM é alfabética (health, hud, hud_msg, hud_redraw, hud_update, menu, message...).

- menu.cpp (45a74c7, 9/9, texto 0x11AA70–0x11AD90, .rodata 0x3391C8, vtable 0x3391E8, .data 0x30D060–0x30D460): CHudMenu em gHUD+0x1CEC (0x14 m_fMenuDisplayed, 0x18 m_bitsValidSlots, 0x1C m_flShutoffTime, 0x20 m_fWaitingForMore). Draw do PS2 vazio mas com frame de pilha (casou com `char szBuf[16];` sem uso). Sem KB_ConvertString. g_szMenuString/g_szPrelocalisedMenuString vão no .data porque são `= ""`. strncat = func_002E61B8. gEngfuncs 0x4C ServerCmd, 0x50 ClientCmd.
- hud_update.cpp (5d64827, 1/1, 0x11A9A8–0x11AA70): UpdateClientData(cdata, clientId) com client_data_t do PS2 (origin 0x0, viewangles 0x14, iWeaponBits 0x2C, 0x30 -> CHud+0x20E4, fov 0x34, idlescale 0x38 = concussão, mouse_sensitivity 0x3C só se != 0); chama gHUD[clientId].Think e devolve iWeaponBits.
- hud_redraw.cpp (5d64827, 8/8, 0x119F70–0x11A9A8, .rodata 0x339198, .data 0x30CF80 grgLogoFrame[56]): Think(clientId) com `m_iFOV = max(CVAR_GET_FLOAT("default_fov"), 90)` só se m_iFOV == 0, e só casou com CVAR_GET_FLOAT como função inline do cl_util.h (com a macro os registros s0/s2 trocam). Redraw(flTime, intermission, clientId, iDrawFlags), esconde com m_iPs2Flags & 4, logo "sprites/%d_logo.spz". ScaleColors fora de linha. DrawHudString/Reverse usam gHUD[clientId].m_scrinfo e TextMessageDrawChar (agora inline no hud.h). DrawHudNumber com DHN_4DIGITS = 8 (milhar; a centena só olha 4, a dezena olha 8|4|2). GetNumWidth devolve 4 com o flag 8.
- hud_msg.cpp (5d64827, 6/6, 0x119CF0–0x119F70, .rodata 0x339188): ResetHUD, InitHUD (void), GameMode, Damage (casou igual ao HLSDK com 0.5f), Concuss (EnableIcon com gHUD[clientId].m_iColorR/G/B e clientId), MsgFunc_HudColor novo (3 READ_BYTE em m_iColorR/G/B). Sem Logo.
- CHud: 0x0 m_pHudList, 0x4 m_hsprLogo, 0x8 m_iLogo, 0x18 m_flMouseSensitivity, 0x1C m_iConcussionEffect, 0x24 m_fOldTime, 0x28 m_flTimeDelta, 0x2C m_vecOrigin, 0x38 m_vecAngles, 0x44 m_iWeaponBits, 0x4C m_iFOV.
- Próximo: hud.cpp (texto antes de 0x119CF0; os 2 stubs de 8 bytes em 0x119CE0/0x119CE8 são as cópias weak do fim dele). Tem os __MsgFunc_* do CHud, CHud::Init/VidInit, AddHudElem, GetSpriteIndex.
