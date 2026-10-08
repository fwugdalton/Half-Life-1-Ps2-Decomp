# Half-Life (USA) PS2 — descompilação

Alvo: `SLUS_200.66` (Half-Life, PS2, NTSC-U), port da Gearbox Software, build de 19/out/2001.

- sha1 do ELF: `1a457f522b6bbf35f44b19b974b37538916eaf95`
- sha1 do segmento carregável (`SLUS_200.66.rom`): `f93030a659d13cbf8cc6b911a4d98a2a66fb4b29`

## Fatos sobre o binário

- ELF sem símbolos (stripped), um único PT_LOAD em `0x00100000`, entry `0x00100008`, `$gp = 0x003A0F70`.
- SDK Sony 2.1 (`PsIIlibgraph2100`, `PsIIlibcdvd 2130`). O compilador confirmado é o **ee-gcc 2.95.2 (SCE, build 274) com `-O2 -G0`**: o `engine/Cmd.c` casa byte a byte com ele.
- As strings de assert revelam a árvore de código (`Gbx/*.cpp`, `Launcher/*.cpp`, `Gbx/Jade/*`), além de zlib 1.1.3 embutida.
- Há microcódigo VU (`.vutext`, overlays `.DVP.overlay*`), que por enquanto é tratado como binário.

## Requisitos

- `binutils-mips-linux-gnu` (`mips-linux-gnu-as`, `ld` e `objcopy`, versão 2.33 ou mais nova)
- Python 3 com `splat64[mips]` (`pip install -r requirements.txt`)
- `make` e `curl`
- O compilador: `tools/download_compiler.sh` baixa o ee-gcc 2.95.2 (um .exe de Windows) e o [wibo](https://github.com/decompals/wibo), que roda esse .exe no Linux.

## Uso

```sh
cp /caminho/do/disco/SLUS_200.66 .
make setup        # gera SLUS_200.66.rom e roda o splat (asm/, assets/, SLUS_200.66.ld)
make -j$(nproc)   # monta, linka e confere o sha1
```

O `make` termina com `build/SLUS_200.66.rom: OK` quando o rebuild é idêntico ao original.

## Mapa do executável

| Endereço | Módulo |
|---|---|
| `0x100000` | `Launcher/` (menus/GUI da Gearbox) |
| `0x116xxx` | `cl_dll/` (cliente do jogo, HUD) |
| `0x18Cxxx` | `dlls/` (servidor do jogo: entidades, monstros, IA) |
| `0x21Dxxx` | `Gbx/` (camada da Gearbox: CD, memory card, PAKs, zlib) |
| `0x247xxx` | `engine/` (motor GoldSrc) |
| `0x2D6xxx` | bibliotecas da SDK Sony e libc |
| `0x2F5980` | microcódigo VU (`.vutext`) |

## Como descompilar um arquivo

1. Ache os limites do arquivo (as strings de assert e as funções `_GLOBAL_$I$` ajudam) e crie um segmento `c` no `SLUS_200.66.yaml`.
2. Rode `make split`. O splat gera `src/<arquivo>.c` com um `INCLUDE_ASM` por função.
3. Troque cada `INCLUDE_ASM` por C. Use `python3 tools/cmpfunc.py build/src/<arquivo>.o` para ver quais funções casam e `python3 tools/difffunc.py build/src/<arquivo>.o <função>` para ver as diferenças instrução por instrução.
4. Quando o arquivo inteiro estiver em C, migre o `.rodata`/`.data` dele (segmentos `.rodata`/`.data` no yaml), usando literais de string de verdade.

`tools/region.py <início> <fim>` lista as funções de um intervalo com as strings que cada uma usa, e `tools/asmdb.py` é o banco de dados de funções e referências que os outros scripts usam.

## Progresso

- `engine/Cmd.c`: 100% (código, `.rodata` e `.data`)
