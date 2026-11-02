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
- GUIActionAddOnDisc.cpp (nome real pela string de alocação; texto 0x10A4D8, .rodata 0x337CA0, .data 0x302A18 com cvars AddOnSwapState/AddOnErrorMessage e flag de registro) 100%: máquina de estados Run (switch duplo), Load lê do CD (func_00237258/CD_Read) e Parse "SDJ x caminho" troca o caminho (func_00236FD8). Truques: ordem dos cases no fonte = ordem no asm (case 10 antes de 8/9); `next = 1` depois do Initialize() mudou a ordem lui/mtc1; tokenizador local `char tok[0x14]` com func_00108008/108018(tok,2). .data entra no yaml depois do cod/201B00.
- GUIActionCopyAttribute.cpp (texto 0x10A330, .rodata 0x337C40) 100%: chama o 6º virtual do elemento (CGUIWidget::CopyAttribute, slot 0x30, adicionado no GUI.h); `char *name = tok; strcpy(attr, name)` em variável separada tirou o s3 a mais.
- GUIActionPrintf.cpp (texto 0x10A220, .rodata 0x337BF8) 100%: só lê o parâmetro (printf tirado).
- GUIActionVcrVariable.cpp (texto 0x10A010, .rodata 0x337B90) 100%: PAUSE/PLAY/REWIND em variável do tipo D_003029E4 (func_00106D28/106D30/106CE8). Mesmo truque do token em variável antes do strcpy.
- GUIActionModifyViewport.cpp (texto 0x109C30, .rodata 0x337B40, .data 0x302A04 = 4 flags de primeira vez; valores em .bss D_0039CFE0..EC como extern) 4/5: encolhe a área de desenho (vid, D_0032EF68/6C). Truques: `x = W / 10` antes de `flag = 1` põe o beql+break antes do div (como no ROM); w/h numa variável e uma chamada só. NON_MATCHING: SetViewport (registradores dos lui). AddOnDisc.o ganhou .data=8 no Makefile (4 bytes de enchimento em 0x302A14).
- GUIActionSetGame.cpp (texto 0x109B08, .rodata 0x337AF0) 100%: func_002354D8(tok) do HLPak.
- GUIActionAddVariable.cpp (texto 0x109910, .rodata 0x337AA0) 100%: CGUIVariable ganhou V4, V5, Add (slot 0x38, tipo D_003029F0) e AddAnim (slot 0x40, tipo D_003029E4) no GUI.h.
- GUIActionSetVariableVariable.cpp (texto 0x109728, .rodata 0x337A40) 100%: CGUIVariable::Copy (slot 0x28, era V4); senão Cvar_VariableString. Chamada virtual com argumento que é outra chamada: guardar o argumento numa variável antes (senão a vtable é lida antes).
- GUIActionSetVariable.cpp (texto 0x109568, .rodata 0x3379E0) 100%: "NULL" vira "".
- GUIActionRefresh.cpp (texto 0x109498, .rodata 0x3379A0) 100%: gui->Set08(1); gui->Set04(1).
- GUIActionExit.cpp (texto 0x1093E0, .rodata 0x337968) 100%: gui->Set04(1).
- GUIActionAppString.cpp (texto 0x109240, .rodata 0x337908) 100%: junta $param0.. e manda para Cbuf_AddText. `if (...) {...; more = 1;} else more = 0;` (atribuir 0 antes deixa a variável num sN).
- GUIActionSetFocus.cpp (texto 0x1090A0, .rodata 0x3378C0) 100%: ctor zera m_bInitialized; `focus` com if/else nos dois níveis (sem valor inicial).
- GUIActionPlaySound.cpp (texto 0x108F40, .rodata 0x337870) 100%: func_00231A98(name, vol, vol) (o Gbx só usa o nome). Fim das ações da fábrica (2026-11-02).
- Próximo: o que vem antes de 0x108F40 no cod/000000 (CGUIAction base 0x108890–0x108BD8, tokenizador 0x108008–0x108148, variáveis 0x107000–0x107F00...).
