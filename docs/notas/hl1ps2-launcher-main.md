---
name: hl1ps2-launcher-main
description: começo do Launcher (GUIImage.cpp e Launcher/HalfLife.cpp 0x100140–0x101450, crt0 antes): faixas, globais e plano
metadata:
  type: project
---
Continua [[hl1ps2-launcher-elements]].
- GUIImage.cpp (nome inventado, texto 0x101450–0x1014C0, 2/2 em 2026-11-03): func_00101450(cabeçalho .psi, GUIImage*) e func_001014B8 vazio, extern "C" (o GUIRenderer.cpp os declara assim).
- Launcher/HalfLife.cpp (string de alocação): texto 0x100140–0x101450 (main 0x100140, laço do front-end 0x100448/0x100608, init do motor 0x1007C0, laço do jogo 0x100990, tabela de funções do launcher 0x100B48 → D_00301E18 (0x8C), cópia das funções do motor 0x101078 → globais 0x399000–0x3990B4 (bss, início do .bss que o crt0 zera) e D_00301ED8/EE4/EE8), .rodata 0x336C80–0x336D28, .data 0x301CC0–0x301F70 (CGUI* 0x301CC0, caminhos 0x301CC8/D00/D38, struct de vídeo 0x301DC8 (0x50), "Sean Mode" 0x301EB0, tabela scantokey 0x301EF0).
- 0x100000–0x100140 é crt0 (asm do SDK, fica em asm); .data 0x301B00–0x301CC0 são listas de ctors/dtors (ficam em asm).
