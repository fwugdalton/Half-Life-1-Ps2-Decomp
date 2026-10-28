---
name: hl1ps2-gbx-cpp
description: Como compilar e casar o C++ da Gearbox (Gbx/*.cpp) no decomp HL1 PS2; estado do KeyStateManager.cpp
metadata:
  type: project
---

Regra .cpp no Makefile (commit ba5f389, 2026-10-13): o mesmo ee-gcc.exe compila src/**/*.cpp (chama cc1plus), mesmo pipeline fixasm/as/objcopy. No yaml o segmento é `cpp` (splat gera stub .cpp com INCLUDE_ASM).

Pegadinhas do C++ (gcc 2.95):
- Nomes mangled no symbol_addrs: ctor `__15KeyStateManager`, dtor `_$_15KeyStateManager` (o $ funciona no gas), métodos `Init__15KeyStateManagerUi`. Funções de nome desconhecido: `extern "C"` para manter func_XXXX.
- `bool` ocupa 4 bytes (sw); campo gravado com sb é `unsigned char`.
- __builtin_new = 0x225A18, __builtin_delete = 0x225A38. Dtor sem vtable: `andi a1,1` + __builtin_delete. Dtor de derivada sem membros: só chama o dtor da base repassando a1.
- Depois de `make split` com nomes novos, dar `touch src/gbx/*.cpp` (o .o não depende do asm em nonmatchings e fica velho).
- `__FILE__` sai "src/gbx/X.cpp": escrever a string "Gbx/X.cpp" literal.
- z_stream tem 0x48 bytes (uLong = 64 bits no EE): total_out em 0x18, zalloc/zfree em 0x28/0x2C.
- Laços `for (i = 0; i < N; i++) a[i].x = c;` saem invertidos (ponteiro descendo do fim): escrever o laço para frente.
- O ee-gcc põe no delay slot do jal uma instrução que lê $v0 (ex.: `jal Find; lw v0,0x118(v0)`); escrever o C normal (`return Find(n)->size;`) já casa.
- scratchpad/try.py aceita "Classe::Metodo" e `--sym=prefixo_mangled`.

KeyStateManager.cpp (0x229CF0..0x22AB50, rodata 0x386EA8..0x386EC8; commit c69c003): KeyStateManager (Init/Shutdown/SetData/Press/Update/Dispatch/AnyDown), MouseStateManager derivada (mouse USB, 5 botões), teclado USB (func_0022A280 cria D_003281F8 = new KeyStateManager(0xEB teclas), 22A4C0 mapa HID→tecla, 22A2E0 leitura). Callback D_00399084(data, down). NON_MATCHING: Release__17MouseStateManagerUi (delay slot) e func_0022A2E0 (constante 1 vs endereço içado). Dados D_003281F8/D_00328200 ainda na .data em asm.

Logger.cpp (0x22AB50..0x22AE48, rodata 0x386EC8, .data 0x328208..0x328218; commit f942dbc): 4 funções extern "C", func_0022ACE0 NON_MATCHING (ordem a1/a2). Dados do KeyStateManager (0x3281F8..0x328208) definidos no .cpp.

MemFileSystem.cpp (0x22AE48..0x22C448, rodata 0x386F30..0x3870F0 alinhada em 16; commit bad610f): 50 MemFile de 0x128 em D_003A6580, 10 buscas MemFind de 0x108 em D_003A6588; tudo extern "C" func_XXXX. NON_MATCHING: AFD0, B078, B118, B310, B590, C118, C210. Strings compartilhadas com INCLUDE_ASM no linker_script_extra.ld.

MemoryCard.cpp (0x22C448..0x22D600, commit f34b2cb): 28/28 casando, ver [[hl1ps2-gbx-memorycard]].

Próximo (cod/12D600): SystemSaveInitialize, GameSave, SPUMemory; depois HLPak/GlobalPak/ExtraPrecache/CDRead/MethodPointerTable/Texture. Ver [[hl1ps2-gbx-kernel]].
