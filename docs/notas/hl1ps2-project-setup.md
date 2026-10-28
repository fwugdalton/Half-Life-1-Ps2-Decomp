---
name: hl1ps2-project-setup
description: Onde fica o projeto de decomp do Half-Life PS2, como ele builda e o que já se sabe do binário
metadata:
  type: project
  modified: 2026-10-08T21:19:32.593Z
---

Dalton (escreve em português; responder em português) quer descompilar o Half-Life (USA) PS2, SLUS_200.66.

- Pasta no dispositivo: `/home/dalton/Documents/HL1_Ps2_decomp` (a pasta `HL_IN_SCP` NÃO é a certa). Lá estão `Half-Life (USA).bin/.cue`, o disco extraído em `disc/` e o projeto git em `decomp/`.
- O sandbox do dispositivo NÃO tem rede nem binutils MIPS. Splat 0.50.0 e `mips-linux-gnu-*` rodam no container da nuvem (python 3.13 precisa de venv). Cópia do projeto já splitado, com asm: `/mnt/project-files/hl1ps2/hl1ps2.tar.gz`.
- `device_commit_files` falhou (404) a partir de /mnt/user-data/outputs; os arquivos foram escritos via heredoc no device_bash.
- O rebuild byte a byte do segmento carregável (`.rom`, sha1 f93030a6…) foi verificado em 2026-10-08. Correções feitas: VMA fixa de .sbss em 0x399000 e .bss em +0x900, além de truncar a rom em 0x298F84. As duas ficam no `make split`.
- Binário: build da Gearbox de 19/10/2001, stripped, SDK 2.1, provável ee-gcc 2.9-ee-991111, C++, ~8,5k funções, strings de assert com `Gbx/*.cpp` e `Launcher/*.cpp`, zlib 1.1.3. Os PAKs são `u32 tamanho + zlib`.

**Why:** evita redescobrir a pasta e o ambiente. **How to apply:** os próximos passos são dividir o .text por arquivo e montar o compilador para matching de C/C++. Ver [[hl1ps2-next-steps]].
