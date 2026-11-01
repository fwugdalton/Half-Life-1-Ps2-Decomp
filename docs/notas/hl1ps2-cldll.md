---
name: hl1ps2-cldll
description: HUD do cliente (cl_dll do HLSDK, versão do PS2 com HUD por jogador) em src/cl_dll: layout do CHud, vtable do CHudBase, nomes e arquivos feitos
metadata:
  type: project
---
Depois das dlls ([[hl1ps2-dlls-done-13]]), o bloco vram 0x100000–0x122138 tem _start, o Launcher da Gearbox e o HUD do cliente (cl_dll). Os arquivos do HUD vão do fim para o começo: ButtonHud, ISlaveHud, train, text_message, statusbar, status_icons, scoreboard, saytext, motd, message, menu, health, geiger, flashlight…

Infra (commit 8e42a92):
- include/cl_dll/hud.h: CHud por jogador `gHUD[clientId]` (0x20F4 bytes, gHUD = 0x308988); cor do HUD em 0x58/0x5C/0x60 (no lugar do UnpackRGB), m_iFontHeight 0x64, m_rghSprites 0x68, m_rgrcRects 0x6C, m_Train em 0x200, m_scrinfo em 0x1ED0 (iWidth 0x1ED4, iHeight 0x1ED8).
- gEngfuncs (cl_enginefunc_t do HLSDK) = 0x30CB70; HookUserMsg(name, pfn, clientId) em 0x48.
- CHudBase: m_pos 0, m_type 8, m_iFlags 0xC, vptr 0x10; SEM destrutor virtual; vtable {0,0}, Init(int clientId), VidInit(int), Draw(float flTime, int clientId, int iDrawFlags), Think, Reset, InitHUDData (as três últimas são inline: cada arquivo deixa cópias weak de 8 bytes no fim do .text; a primeira cópia (0x114E48/0x115D08/0x114840) leva o nome no symbol_addrs).
- DECLARE_MESSAGE: `__MsgFunc_X(int clientId, name, size, buf)` chama gHUD[clientId].m_X.MsgFunc_X(clientId, ...). Funções: AddHudElem 0x116D80, LoadSprite 0x116E20, BEGIN_READ 0x11C4E0, READ_BYTE 0x11C550.
- Makefile: src/cl_dll/%.o com -ffast-math e `-R .bss --set-section-alignment .data=8`. Script /tmp/claude-0/ccmp.sh (= fcmp.sh para src/cl_dll).
- Vtable referenciado pelo asm (construtor do CHud): pôr `_vt$9CHudTrain = 0x3395D8;` no symbol_addrs.

Feitos: train.cpp (0x11FC08–0x11FE90, .rodata 0x3395B8–0x339618; Draw sai cedo se !(iDrawFlags & 0x24); y = altura − SPR_Height − fonte − 24; x = largura/3 + SPR_Width/4 + 8).
- islave_hud.cpp (ef8aac2, 8/9): CHudISlave (nome inventado), membro m_ISlave em gHUD+0x1E24; .rodata 0x339618–0x3396B8 (o .rodata de cada arquivo termina com 8 bytes zerados depois do vtable). CHud: m_iPs2Flags 0x48 (0x40 = jogador vortigaunt), m_iPs2Health 0xCC; GetSpriteIndex 0x116738, GetSprite/GetSpriteRect inline. Truques: `y = ...; y -= 2;` (não `... - 2`) acerta os registradores; `r = 255; b = 0; g = 0;`. Draw NON_MATCHING (o alvo repete o (int)flCut três vezes). INLINE_INCLUDE_ASM com pasta asm/nonmatchings/cl_dll/<arquivo>.
- button_hud.cpp (9/9): CHudButton (nome inventado), m_Button em gHUD+0x1E58 (vai até 0x1ED0); texto 0x1207C8–0x122148 (as cópias weak 0x122138/40 são dele, não do aflock), .rodata 0x3396C0–0x3399B0 alinhada a 16 (jtbl; regra no Makefile). CHud::m_flTime float 0x20; READ_SHORT 0x11C598; FindKey estático usa Key_FindBinding (teclas do PS2 0xD2–0xE9). Truques: `m_flTime = gHUD[...].m_flTime + READ_SHORT() * 0.01f` direto; ordem dos stores sai invertida/içada (testar ordens); contador separado (j) no laço que zera keys[13].
- text_message.cpp (13/13, texto 0x11F5B8–0x11FC08, .rodata 0x339568): HLSDK com clientId; PS2: strcpy/sprintf, sem gViewPort, CenterPrint(msg, clientId) (gEngfuncs+0x84), ConsolePrint +0x80, TextMessageGet +0x70, SayTextPrint(msg,128,clientId) 0x11CB78 (m_SayText em 0x1CD8), READ_STRING 0x11C708; LocaliseTextString só aceita 'A'..'z' e termina com dst_buffer[buffer_size-1] (sem len); Buffered usa buffer na pilha. Precisa de -fmove-all-movables. Chamadas por ponteiro do gEngfuncs: `psz = ConvertCRtoNL(psz); ConsolePrint(psz);` (senão o gcc carrega o ponteiro antes). .bss (word_buf, szBuf) via extern "C" D_003A3270/D_003A3370. Este arquivo tem as cópias weak de VidInit/Draw/Think/Reset/InitHUDData (0x11FBE0–0x11FC00).
