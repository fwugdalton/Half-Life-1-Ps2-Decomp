---
name: hl1ps2-launcher-gui
description: src/launcher/GUI.cpp (CGUI do Launcher da Gearbox, sem fonte): faixas, layout, NON_MATCHING, truques e próximos arquivos do Launcher
metadata:
  type: project
---
Feito em 2026-11-01 (depois de [[hl1ps2-cldll-7]]). Launcher = vram 0x100000–0x110680, ainda asm cod/000000 (~360 funções).
- GUI.cpp: texto 0x10EC68–0x110680 (34 funções, 32 batem), .rodata 0x338970 ("BrightnessValue"…" \\\"\t\n\r"), .data 0x302A58 (D_00302A58 e D_00302A5C = 1). Flag -fmove-all-movables (sem ela o SetFocus e o ParseScreens não batem).
- CGUI (nomes inventados): 0 m_bInitialized, 4/8 ints com get/set, 0xC tela atual, 0x10 máx, 0x14 nº telas, 0x18 CGUIScreen**, 0x20 início do fade-in, 0x24 tempo do fade-out, 0x28 início do fade-out, 0x2C fading, 0x30 próxima tela, 0x34 alpha, 0x38 lista de ações, 0x3C tokenizador, 0x40 desenho (0x15E8), 0x44 variáveis, 0x48 pakfile_t. Dtor grava "gamma %f" da BrightnessValue.
- Classes de fora ficam como funções extern "C" func_XXXX (tela 0x105830–0x105DF8, desenho 0x1066C0, variáveis 0x107000–0x107F00 com vptr em 0x64, tokenizador 0x108008–0x108148, ações 0x108890–0x108BD8 com vptr em 0x8, construtores das ações 0x108F40…0x10EA58). Os .cpp (PauseMenu, DiscFail, GameSave) ainda chamam func_0010EC68 etc.: aliases no linker_script_extra.ld. O rename_syms troca nos .c (GbxMain.c virou Get04__4CGUI).
- func_00229B88/229C38 (tempo) são unsigned long (64 bits); Q_memcpy precisa retornar void* para o LoadFile casar.
- NON_MATCHING: ProcessDefines (s1/s2 trocados entre dois grupos) e FindToken estática (beq em vez de beql).
- Truques: separar variáveis por uso (q do laço de contagem, r do laço de troca, next) tira cópias para s0; reaproveitar o ponteiro de varredura c como destino; `rest = len - (end - *pText)` num temporário; `const char *name = ...; v->SetString(name)` (carregar a vtable depois); `if (Exists == 1) {...}` em vez de retorno cedo; ok = 0 / if (strchr) ok = 1 dá o movn.
- Ferramentas: /tmp/claude-0/variant.py FUNC início fim arquivo_de_variantes (separadas por ====) e try.sh (compila com -DNON_MATCHING).
- Próximo: arquivos das ações antes de 0x10EC68 (Vibrate 0x10EA58, DisplayMode 0x10E8F8, MemorySave 0x10E6A8, Volume 0x10E540, cada uma com ctor/dtor/Init/Execute e vtable na .rodata).
