# Half-Life (USA) PS2 — descompilação

Alvo: `SLUS_200.66` (Half-Life, PS2, NTSC-U), port da Gearbox Software, build de 19/out/2001.

- sha1 do ELF: `1a457f522b6bbf35f44b19b974b37538916eaf95`
- sha1 do segmento carregável (`SLUS_200.66.rom`): `f93030a659d13cbf8cc6b911a4d98a2a66fb4b29`

## Fatos sobre o binário

- ELF sem símbolos (stripped), um único PT_LOAD em `0x00100000`, entry `0x00100008`, `$gp = 0x003A0F70`.
- SDK Sony 2.1 (`PsIIlibgraph2100`, `PsIIlibcdvd 2130`), então o compilador provável é o `ee-gcc 2.9-ee-991111` (C++ com mangling do GCC 2.x e `.gcc_except_table`).
- As strings de assert revelam a árvore de código (`Gbx/*.cpp`, `Launcher/*.cpp`, `Gbx/Jade/*`), além de zlib 1.1.3 embutida.
- Há microcódigo VU (`.vutext`, overlays `.DVP.overlay*`), que por enquanto é tratado como binário.

## Requisitos

- `binutils-mips-linux-gnu` (`mips-linux-gnu-as`, `ld` e `objcopy`)
- Python 3 com `splat64[mips]` (`pip install -r requirements.txt`)
- `make`

## Uso

```sh
cp /caminho/do/disco/SLUS_200.66 .
make setup        # gera SLUS_200.66.rom e roda o splat (asm/, assets/, SLUS_200.66.ld)
make -j$(nproc)   # monta, linka e confere o sha1
```

O `make` termina com `build/SLUS_200.66.rom: OK` quando o rebuild é idêntico ao original.
