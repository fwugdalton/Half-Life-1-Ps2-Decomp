---
name: hl1ps2-launcher-screen
description: GUIWidget.cpp e GUIScreen.cpp do Launcher (telas e base dos elementos): faixas, layout, truques e próximo arquivo
metadata:
  type: project
---
Continua [[hl1ps2-launcher-vars]].
- GUIWidget.cpp (texto 0x1056C8–0x105898, .rodata = só a vtable 0x3373A0) 6/6 em 2026-11-02: CGUIWidget (vptr em 0x34; slots dtor, Parse (int, puro), V2 (desenho), Activate, Deactivate, CopyAttribute, todos puros): 0 nome, 4 ?, 8 camada (Get8), 0xC/0x10 dono+objeto de teclas (HandleKey 0x102AB8), 0x14/0x18 dono+objeto, 0x1C, 0x20..0x2C 4 objetos com vptr em 8 (CGUIWidgetPart no GUI.h), 0x30 CGUI*. Ctor zera os 4 com `for (i = 3; i >= 0; i--)`.
- GUIScreen.cpp (texto 0x105898–0x106038, .rodata 0x3373E0) 14/14: CGUIScreen 0x44 (0 CGUI*, 4 lido, 8 nome, 0xC tem fundo, 0x10 textura, 0x14 TEXTURE_RECT[4], 0x24/0x28 tam. (o original lê a altura com GetWidth!), 0x2C/0x30 fades em ms, 0x34 nº, 0x38 máx, 0x3C foco, 0x40 lista). GUI_GENERIC = new 0xA0 com ctor 0x104910 (CGUIGeneric). Draw desenha 100 camadas.
- Truques: Parse com `t = Next(); while (t && strcmp(t,"END") != 1) {...; t = Next();}` e `if (!lido) {...} else r = 0;`; slot Parse do widget devolve int (acerta o registro do vptr); GetFocus = inline GetWidget(m_iFocus) (o parâmetro do inline fica em a1); SetFocus com break; IsFocus `w == lista[foco]`.
- Aliases func_00105830…105DF8 (GUI.cpp) no linker_script_extra.ld e no KEEP do rename_syms.
- Próximo: CGUIGeneric (ctor 0x104910, vtable 0x337360, strings até 0x337360: GL_REPLACE, GL_ALPHA...) e antes dele.
