---
name: hl1ps2-launcher-main
description: começo do Launcher (GUIImage.cpp e Launcher/HalfLife.cpp 0x100138–0x101450, feitos; crt0 antes): faixas, globais e truques
metadata:
  type: project
---
Continua [[hl1ps2-launcher-elements]].
- GUIImage.cpp (nome inventado, texto 0x101450–0x1014C0, 2/2 em 2026-11-03): func_00101450(cabeçalho .psi, GUIImage*) e func_001014B8 vazio, extern "C" (o GUIRenderer.cpp os declara assim).
- Launcher/HalfLife.cpp (string de alocação): texto 0x100140–0x101450 (main 0x100140, laço do front-end 0x100448/0x100608, init do motor 0x1007C0, laço do jogo 0x100990, tabela de funções do launcher 0x100B48 → D_00301E18 (0x8C), cópia das funções do motor 0x101078 → globais 0x399000–0x3990B4 (bss, início do .bss que o crt0 zera) e D_00301ED8/EE4/EE8), .rodata 0x336C80–0x336D28, .data 0x301CC0–0x301F70 (CGUI* 0x301CC0, caminhos 0x301CC8/D00/D38, struct de vídeo 0x301DC8 (0x50), "Sean Mode" 0x301EB0, tabela scantokey 0x301EF0).
- 0x100000–0x100140 é crt0 (asm do SDK, fica em asm); .data 0x301B00–0x301CC0 são listas de ctors/dtors (ficam em asm).
- HalfLife.cpp feito em 2026-11-03 (49/53, -fmove-all-movables). NON_MATCHING: 0x1007C0 (ordem de load/store do rect), 0x100990 (registro do ponteiro D_0039902C), 0x100B48 e 0x101078 (ordem dos stores; o scheduler reordena). O yaml começa o .cpp em 0x100138 (função vazia); 0x100000–0x100138 fica crt0.
- Truques: não chamar a função de main (gcc põe jal __main): func_00100140. Dois "" no .rodata (0x336CC8 e 0x336CD8): o segundo é "\0" (char[2], não deduplica). Globais inicializadas (até = 0) vão para o .data na ordem; D_00301DA4 = 0x2000000. D_00399008/0C no linker_script_extra.ld. fixasm expande `la sym+off`.
- Depois do Launcher só sobra asm: cod/026F80 (IA do parceiro), cod/0C3350 (player.cpp) e cod/1D0A68 (libc/libm/SDK do PS2, 767 funções).
