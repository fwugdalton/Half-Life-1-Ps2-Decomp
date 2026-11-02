---
name: hl1ps2-launcher-base
description: Launcher antes das ações (GUIActionTable/CGUIAction base em diante): faixas, truques e próximo arquivo
metadata:
  type: project
---
Continua [[hl1ps2-launcher-actions]]. Ferramentas: /tmp/claude-0/integ.py, showfn.sh, ro.py, vb.py (fim com $'\n}\n' para não pegar o } de um bloco interno).
- GUIActionTable.cpp (texto 0x108890–0x108BD8, .rodata 0x3377B0 com a vtable _vt$10CGUIAction) 100% em 2026-11-02: CGUIActionTable (0x14: 0 init, 4 máx, 8 nº, 0xC CGUIAction**, 0x10 CGUI*; ctor/dtor/Init/Add(realloc +10)/Find/GetGUI/Contains) e CGUIAction (ctor, dtor com Mem_Free do nome, IsNamed, Equals). Init/Execute do CGUIAction viraram `= 0` no GUI.h (vtable com __pure_virtual). GUI.cpp chama por func_00108890/1088A0/108960/108980 (aliases). Ordem dos stores do Init achada por tentativa.
- O Makefile não segue os headers: depois de mexer no GUI.h, `touch src/launcher/*.cpp src/gbx/*.cpp` e make para conferir.
- Próximo: tokenizador (0x108008–0x108890: func_00108008…func_00108810) e antes dele variáveis (0x107000–0x107F00), desenho e telas.
