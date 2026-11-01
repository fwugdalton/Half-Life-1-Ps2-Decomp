---
name: hl1ps2-cldll-6
description: crouch, cdll_int, cl_hud (hud.cpp) e battery do cl_dll (gHUD[2] em .data, layout do CHud, D_ de campos no linker_script_extra) e próximo arquivo
metadata:
  type: project
---
Feitos em 2026-11-01 (continuação de [[hl1ps2-cldll-5]]):
- crouch.cpp (320cf05, 8/8, texto 0x116E70–0x117300, .rodata 0x338E90, vtable 0x338EC0): CHudCrouch só do PS2 em gHUD+0x25C (mensagem CrouchState; sprites crouch_on/walk/run; m_iState 1 corre, 2 agacha). Nome do arquivo é palpite.
- cdll_int.cpp (2d7eed0, 9/9, texto 0x115D18–0x116188, .data 0x308988): `CHud gHUD[2];` e `cl_enginefunc_t gEngfuncs;` (vão para .data sozinhos). Initialize, HUD_VidInit/Init/Redraw/UpdateClientData/Reset(clientId) e o init estático (loop que grava as vtables de cada membro do CHud). Precisou de CHud() : m_pHudList(0), m_iSpriteCount(0) {} e ~CHud() (mangling `_$_4CHud`).
- Com gHUD/gEngfuncs num .o, o asm que cita campos (D_003089FC etc.) dá undefined: listei todos no linker_script_extra.ld (loop: build, pegar os D_0030[89ABC]..., anexar).
- cl_hud.cpp (fff32c2, 15/15, texto 0x116188–0x116E70, .rodata 0x338DB0): é o hud.cpp (string "cl_dll/cl_hud.cpp" do Mem_DbgMalloc, linha 318). Init: cor 255/160/0, hooks Logo, ResetHUD, GameMode, InitHUD, SetFOV, Concuss, HudColor; cvars só com clientId 0; Mem_Free da lista; Init dos elementos na ordem Ammo, Health, Geiger, Train, Battery, Flash, Crouch, Message, Scoreboard, MOTD, StatusBar, DeathNotice, AmmoSecondary, TextMessage, StatusIcons, ISlave, Button, SayText, Menu. VidInit igual ao HLSDK (sprites/hud.txt, "sprites/%s.spz", m_iRes 320/640). ~CHud só faz os 3 delete[] (__builtin_vec_delete = 0x2E2138). LoadSprite usa gHUD[0].
- CHud: 0xC m_pSpriteList, 0x10 m_iSpriteCount, 0x14 m_iSpriteCountAllRes, 0x54 m_iRes, 0x70 m_rgszSpriteNames, 0x74 m_Ammo (0x44), 0x1D0 m_Battery, 0x21C m_Flash, 0x25C m_Crouch, 0x1CC0 m_DeathNotice, 0x1D10 m_AmmoSecondary (0x2C). Vtables: CHudAmmo 0x338C90, CHudAmmoSecondary 0x338CF0, CHudBattery 0x338D70.
- battery.cpp (f0b5461, 8/8, texto 0x115730–0x115D18, .rodata 0x338D48): Draw com flags & 0x14, sai com m_iPs2Flags & 8, título do jogo, m_iPs2Unk20E4 >= 0; `rc.top += (int)(...)` (sem o (int) soma em float); x = W/2+8 se cl_numlocal >= 2 && !D_00399564, senão W/4+8; y -24; sem o +0.2f do número.
- Próximo: ammohistory.cpp (texto ~0x114A28–0x115730, .rodata 0x338D30 "hud_drawhistory_time"), depois ammo_secondary (0x114940, vtable 0x338CF0) e ammo.cpp (Init 0x111580).
