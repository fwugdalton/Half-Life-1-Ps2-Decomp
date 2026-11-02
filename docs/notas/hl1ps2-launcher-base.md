---
name: hl1ps2-launcher-base
description: Launcher antes das ações (GUIActionTable/CGUIAction base em diante): faixas, truques e próximo arquivo
metadata:
  type: project
---
Continua [[hl1ps2-launcher-actions]]. Ferramentas: /tmp/claude-0/integ.py, showfn.sh, ro.py, vb.py (fim com $'\n}\n' para não pegar o } de um bloco interno).
- GUIActionTable.cpp (texto 0x108890–0x108BD8, .rodata 0x3377B0 com a vtable _vt$10CGUIAction) 100% em 2026-11-02: CGUIActionTable (0x14: 0 init, 4 máx, 8 nº, 0xC CGUIAction**, 0x10 CGUI*; ctor/dtor/Init/Add(realloc +10)/Find/GetGUI/Contains) e CGUIAction (ctor, dtor com Mem_Free do nome, IsNamed, Equals). Init/Execute do CGUIAction viraram `= 0` no GUI.h (vtable com __pure_virtual). GUI.cpp chama por func_00108890/1088A0/108960/108980 (aliases). Ordem dos stores do Init achada por tentativa.
- O Makefile não segue os headers: depois de mexer no GUI.h, `touch src/launcher/*.cpp src/gbx/*.cpp` e make para conferir.
- GUIParser.cpp (texto 0x108008, .rodata 0x337788, .data 0x3029F8 "COMPRESSED") em 2026-11-02, 12/15 batem: CGUIParser 0x14 (0 init, 4 GUIDefineStack* (10 níveis de 11 defines {name[100],value[100]}), 8 texto, 0xC pos, 0x10 GUIStream* {z_stream; pos 0x48; buf[0x800] 0x4C}); texto com prefixo "COMPRESSED" é lido com inflate (zlib 1.1.3). Token em D_0039A7E0 (.bss). NON_MATCHING: NextToken, Open, Advance (só registradores/ordem de 2 stores); o splat põe a string .rodata deles no .s, mas o Open usa "Launcher/GUIParser.cpp" do Init: D_00337788 no linker_script_extra.ld. Aliases func_00108008/018/070/120/148 (usados pelo GUI.cpp). Classe sem vtable (tirar a linha _vt$ que o integ.py põe). Splat: `source ../.venv/bin/activate` antes do make split.
- Próximo: variáveis (0x107000–0x108008) e antes delas desenho e telas.
