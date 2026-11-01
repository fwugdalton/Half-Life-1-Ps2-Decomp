---
name: hl1ps2-cldll-7
description: ammo.cpp do cl_dll (faixas, CHudAmmo/WEAPON do PS2, NON_MATCHING, truques) e próximo arquivo (GUI antes de 0x110680)
metadata:
  type: project
---
Feito em 2026-11-01 (continuação de [[hl1ps2-cldll-6]]):
- ammo.cpp: texto 0x110680–0x1148B0 (70 funções, 66 batem), .rodata 0x338A20 (vtable CHudAmmo 0x338C90), .data 0x302A60 (gWR[2], gpActiveSel 0x308888, gpLastSel, giBucketHeight/Width, giABHeight/Width, ghsprBuckets 0x3088A8; zerados vão para .data).
- NON_MATCHING: MsgFunc_CurWeapon, UserCmd_PrevSlot, DrawReloadBar, DrawWList (alocação de registradores/cross-jump).
- PS2: mensagens `(int clientId, const char*, int, void*)`; comandos UserCmd_X(int clientId) com atoi(Cmd_Argv(1)); SelectSlot(clientId, slot, fAdvance, dir); Close faz ClientCmd("pause %d"); sem SlotInput (inline). DrawReloadBar e DrawBar/DrawAmmoBar(...,clientId) são do PS2; func_00113D08 fica extern "C" (as dlls usam).
- hud.h: pfnSetCrosshair(hspr, rc, r, g, b, clientId) em 0x34; pfnServerCmd(char*, clientId) em 0x4C; CHud 0x44 m_iKeyBits, 0x20E4 m_iWeaponBits, 0x20E8 m_fPlayerDead. WEAPON (ammohistory.h, `struct WEAPON_s`): crosshair 0xF8, autoaimold 0x10C, autoaim 0x120, zoomed 0x134/0x148, scrosshair 0x15C (sprites .spz). Símbolo renomeado: HasAmmo__15WeaponsResourceP8WEAPON_s.
- Truques: `if (ScreenWidth < 512) iRes = 320; else iRes = 640;` com `int clientId = 0;` local; GetSpriteList `!strcmp(...) && (p->iRes == iRes)`; NextSlot `int pos=0, slot=0; if (sel) { pos=0; slot=sel->iSlot+1; }`; Draw: calcular cx/cy antes do SPR_Set dentro de `if (iDrawFlags & 0x24)`.
- Pegadinha: compilar à mão com -DNON_MATCHING deixa o build/.../ammo.o mais novo que o .cpp e o make não refaz: apagar o .o antes do build completo. Os .s de nonmatchings só existem depois do yaml + make split.
- Próximo: o código antes de 0x110680 (GUI da Gearbox, strings Launcher/GUI.cpp, por volta de 0x1103xx).
