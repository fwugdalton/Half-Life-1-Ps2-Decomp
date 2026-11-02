---
name: hl1ps2-launcher-bindhelper
description: src/launcher/GUIActionBindHelper.cpp (tela de controles do Launcher): faixas, structs, flag, NON_MATCHING e truques
metadata:
  type: project
---
Feito em 2026-11-01 (depois de [[hl1ps2-launcher-gui]]). 21 funções, 16 batem.
- Faixas: texto 0x10C2E8–0x10E540, .rodata 0x3384D8 (0x2C8, vtable no fim em 0x338778), .data 0x302A48 (primeira vez = 1, nº/capacidade/ponteiro da lista de nomes de teclas). Flag -fmove-all-movables (UpdateBind e Execute só casam com ela).
- CGUIActionBindHelper (0x10 bytes): m_iMode em 0xC (1 Dualshock 1, 2 Dualshock 2, 4 teclado, 8 mouse). Comandos do Execute: KEY_REP, PRESET, CONTROLLER1/2_MODE, KEYBOARD_MODE, MOUSE_MODE, UPDATE_BIND, UNBIND_ACTION, BIND_TITS, FIND_PRESET, IN_KEY_RANGE_VALID, TRANSLATE_GLYPHS, APPLY_PRESET, UPDATE_CUSTOM_BINDS, SAVE_SETTINGS.
- BindTable 0x14E0 (nome 0x34, count 0x34, 33 BindEntry de 0xA0 a partir de 0x38, first/last 0x14D8/0x14DC) em 0x39CFF0 DS1, 0x39E4D0 teclado, 0x39F9B0 mouse, 0x3A0E90 DS2; D_003A2370 = tabela atual; D_003A2378 = int[4][8] presets dos sticks. BindEntry: cmd[0x28], <nome>Graphic[0x32] em 0x28, <nome>Number[0x32] em 0x5A, preset[4] em 0x8C, value em 0x9C. KeyInfo {key, nome strdup, mask}.
- kbutton_t dos sticks (in_leftStickSideMove etc., [2] por jogador) e Key_SetBinding/Key_ClearBinding/Key_FindBinding do engine.
- NON_MATCHING: SaveSettings (só ordem de 2 instruções antes do laço), FindPreset, UpdateCustomBinds, TranslateGlyphs (o original não reduz o i*160; com a flag o nosso reduz), FindStickPreset. O "= " só existe no TranslateGlyphs: definido com __asm__(".section .rodata ...") antes dele; D_00338670 e D_00338678 no linker_script_extra.ld.
- Truques: `p = 0; ... if (v->IsType(f) == 1) p = v;` escrito à mão (inline deu movn ao contrário) e a mesma variável v para os dois Find; ordem dos stores: o gcc inverte (escrever `state` antes de `down[0]`; presets antes de value); first/last intercalados por tabela; `r = key; if (faixa) r = key - 100;` no TranslateKey; método inline na classe é emitido fora de linha no fim do .text (não usar). Script /tmp/claude-0/vb.py FUNC início fim variantes.
- GUIActionIsDeviceConnected.cpp (texto 0x10BFF8, .rodata 0x338430) feito 100% em 2026-11-02: lê D_00334FB8 DS1, FC0 teclado, FC4 mouse, FBC DS2; Init com m_bInitialized antes do strdup.
- Próximo: a ação em 0x10B8D8 (e as outras da fábrica até 0x108F40).
