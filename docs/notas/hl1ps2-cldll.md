---
name: hl1ps2-cldll
description: HUD do cliente (cl_dll do HLSDK, versão do PS2 com HUD por jogador) em src/cl_dll: layout do CHud, vtable do CHudBase, nomes e arquivos feitos
metadata:
  type: project
---
Depois das dlls ([[hl1ps2-dlls-done-13]]), o bloco vram 0x100000–0x122138 tem _start, o Launcher da Gearbox e o HUD do cliente (cl_dll). Os arquivos do HUD vão do fim para o começo: ButtonHud (0x1207B8?–0x122138, só PS2), ISlaveHud (0x11FE90–?, só PS2), train, text_message, statusbar, status_icons, scoreboard, saytext, motd, message, menu, health, geiger, flashlight…

Infra (commit 8e42a92):
- include/cl_dll/hud.h: CHud por jogador `gHUD[clientId]` (0x20F4 bytes, gHUD = 0x308988); cor do HUD em 0x58/0x5C/0x60 (no lugar do UnpackRGB), m_iFontHeight 0x64, m_rghSprites 0x68, m_rgrcRects 0x6C, m_Train em 0x200, m_scrinfo em 0x1ED0 (iWidth 0x1ED4, iHeight 0x1ED8).
- gEngfuncs (cl_enginefunc_t do HLSDK) = 0x30CB70; HookUserMsg(name, pfn, clientId) em 0x48.
- CHudBase: m_pos 0, m_type 8, m_iFlags 0xC, vptr 0x10; SEM destrutor virtual; vtable {0,0}, Init(int clientId), VidInit(int), Draw(float flTime, int clientId, int iDrawFlags), Think, Reset, InitHUDData (as três últimas são inline: cada arquivo deixa cópias weak de 8 bytes no fim do .text; a primeira cópia (0x114E48/0x115D08/0x114840) leva o nome no symbol_addrs).
- DECLARE_MESSAGE: `__MsgFunc_X(int clientId, name, size, buf)` chama gHUD[clientId].m_X.MsgFunc_X(clientId, ...). Funções: AddHudElem 0x116D80, LoadSprite 0x116E20, BEGIN_READ 0x11C4E0, READ_BYTE 0x11C550.
- Makefile: src/cl_dll/%.o com -ffast-math e `-R .bss --set-section-alignment .data=8`. Script /tmp/claude-0/ccmp.sh (= fcmp.sh para src/cl_dll).
- Vtable referenciado pelo asm (construtor do CHud): pôr `_vt$9CHudTrain = 0x3395D8;` no symbol_addrs.

Feitos: train.cpp (0x11FC08–0x11FE90, .rodata 0x3395B8–0x339618; Draw sai cedo se !(iDrawFlags & 0x24); y = altura − SPR_Height − fonte − 24; x = largura/3 + SPR_Width/4 + 8).
