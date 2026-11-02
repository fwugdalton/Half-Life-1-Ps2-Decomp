---
name: hl1ps2-launcher-actions
description: ações GUI_ACTION_* do Launcher feitas antes de 0x10AD98 (faixas, truques) e próxima ação a fazer
metadata:
  type: project
---
Continua [[hl1ps2-launcher-bindhelper]] (que lista BindHelper, IsDeviceConnected, SystemSaveManualUse, SystemIdentifyState, GameSave). Mapa offset arquivo = vram - 0xFF000.
- Script /tmp/claude-0/integ.py Nome texto rodata vtable sym=addr...: põe yaml (cpp e .rodata), alias func_ no linker_script_extra.ld e símbolos (vtable incluída). Depois tools/handoff/fullbuild.sh.
- GUIActionSetPauseMenu.cpp (texto 0x10AC08, .rodata 0x337E28) 100% em 2026-11-02: func_002365C0/E8/236610 do PauseMenu; `if (!eq(screen,"$param2")) f(screen); else f(0);` (a ordem do if/else decide beq/bne).
- GUIActionSetFrontEnd.cpp (texto 0x10AA78, .rodata 0x337DC0) 100%: cópia do SetPauseMenu com func_00100358/100398/1003C0 (menu principal).
- Próximo: ação com ctor em 0x10A4D8 (cvars em .data 0x302A18/0x302A30, flag D_00302A44; depois 0x10A330, 0x10A220, 0x10A010, 0x109C30, 0x109B08, 0x109910, 0x109728, 0x109568, 0x109498, 0x1093E0, 0x109240, 0x1090A0, 0x108F40).
