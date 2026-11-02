---
name: hl1ps2-launcher-vars
description: classes de variável do GUI do Launcher (Integer, Float, Numeric, base Variable): faixas, layout, truques
metadata:
  type: project
---
Continua [[hl1ps2-launcher-base]] (String e Manager estão lá).
- Hierarquia: CGUIVariable ("Variable", D_003029E0; vtable 0x3374D0 com 6 slots: dtor, IsType, ParseAttribute, SetString, Copy, V5) → CGUIVariableNumeric ("Numeric", D_003029E4; vtable 0x337548; ctor 0x106C50, dtor 0x106C98, IsType 0x106D40, ParseAttribute 0x106D98, V5 0x106F40; slots 6..9 puros: Add, AddAnim, Reset, Restore) → Integer/Float. O g++ 2.95 põe 8 bytes zero no fim de cada vtable.
- Numeric: 0x68 WRAP, 0x6C RUNNING, 0x70 LOOP, 0x74 LERP_TIME (unsigned), 0x78 início (unsigned long = 8 bytes no EE). Nome da variável em 0 é o nome da cvar.
- GUIVariableInteger.cpp (texto 0x1074C8, .rodata 0x337640, vtable 0x337678, .data 0x3029EC "Integer") 14/14 em 2026-11-02: 0x80 inicial, 0x84 valor, 0x88 máx, 0x8C mín. GetValue anima min→max e lê a cvar; SetValue prende e grava a cvar. Truques: `unsigned int now = relógio()` com relógio/tempo passado devolvendo unsigned long dá os dsll32/dsra32+dsrl32; `if (loop == 1) início = now; else {...}`; `m_iValue = m_iInitialValue = min`.
- GUIVariableFloat.cpp (texto 0x106F90, .rodata 0x3375A8, vtable 0x3375E0, .data 0x3029E8 "Float") 14/14: como a Integer com floats, 0x90 = usa a cvar (zera se ela some), WRAP no SetValue (if/else completos, não ternário). GetValue com `unsigned long` direto (lwu + __floatdisf). Aliases func_00107000 (GetValue) e func_00107398 (SetValue), usados pelo BindHelper e outros (KEEP no rename_syms).
- Próximo: Numeric (0x106C50–0x106F90, .rodata 0x337510, vtable 0x337548, .data 0x3029E4) e a base (0x106B58, .rodata 0x3374B8, .data 0x3029E0).
