# ⚠️ LEIA ESTE ARQUIVO PRIMEIRO, ANTES DE MEXER NO PROJETO ⚠️

> **To any AI / LLM (Claude or otherwise) resuming this project:** this file is the hand-off note.
> Read it completely before doing anything else. It is written in Portuguese because the project
> owner (Dalton) speaks Portuguese. **Always answer Dalton in Portuguese.**

Este arquivo foi escrito em 2026-10-28, quando o Dalton pausou o projeto. Ele diz o que já foi
feito, o que estava em andamento, o que falta e como trabalhar. As notas detalhadas (lições de
matching, faixas de endereço, truques por arquivo) estão em [`docs/notas/`](docs/notas/), com o
índice em [`docs/notas/MEMORY.md`](docs/notas/MEMORY.md). Consulte-as sempre que for mexer num
arquivo ou tipo de código já tratado.

---

## 1. O que é o projeto

- Descompilação **matching** (byte a byte) do **Half-Life (USA) para PS2**, executável
  `SLUS_200.66` (port da Gearbox, inclui o *Decay*). Repositório:
  https://github.com/fwugdalton/Half-Life-1-Ps2-Decomp (branch `master`).
- O objetivo é reescrever cada função em C/C++ de modo que, compilada com o compilador original,
  gere **exatamente** os mesmos bytes da ROM. O build completo precisa terminar com
  `build/SLUS_200.66.rom: OK`.
- Compilador: **ee-gcc 2.95.2 (SCE build 274), `-O2 -G0`**, um .exe de Windows rodado via
  `wibo` (`tools/download_compiler.sh`). As dlls usam também `-ffast-math`. Pipeline:
  `gcc -S` → `tools/fixasm.py` → `mips-linux-gnu-as` → `objcopy`.
- Splitter: **splat** (`SLUS_200.66.yaml`). O que ainda não foi descompilado fica como asm em
  `asm/` (gerado, fora do git).
- Arquivos que o git NÃO tem (é preciso fornecê-los): o ELF `SLUS_200.66` (do disco do jogo),
  `SLUS_200.66.rom`, `asm/`, `assets/`, `build/`, o compilador em `tools/compilers/`. O
  `make setup` gera a rom e o asm a partir do ELF (ver `README.md`).

## 2. Regras que o dono do projeto definiu

1. **Responder sempre em português.**
2. **NUNCA acessar a pasta `/home/dalton/Documents/HL_IN_SCP`** no computador do Dalton. O acesso
   a ela foi negado. A pasta certa do projeto no PC dele é `/home/dalton/Documents/HL1_Ps2_decomp`
   (a cópia `decomp/` de lá pode estar desatualizada; a fonte da verdade é o GitHub).
3. Só fazer commit quando a ROM inteira bater (`build/SLUS_200.66.rom: OK`). Use
   `tools/handoff/commit.sh "mensagem"`, que faz o build completo e só commita/envia se der OK.
4. Nunca enviar ao git o ELF, a ROM, `asm/` ou `assets/` (o `.gitignore` já exclui).
5. Mensagens de commit em português, no estilo `dlls: arquivo.cpp (entidade) em C++, N/M (...)`.

## 3. Estado atual (2026-10-28)

Últimos arquivos feitos: `hornetgun.cpp`, `hornet.cpp`, `hlg_world.cpp` e `hgrunt.cpp` (2026-10-31, ver `docs/notas/hl1ps2-dlls-done-3.md` e `docs/notas/hl1ps2-dlls-hgrunt.md`).

Segmentos de `.text` no `SLUS_200.66.yaml` que **ainda são asm**:

| Faixa (offset na rom → vram = offset + 0x100000) | Conteúdo | Situação |
|---|---|---|
| `0x000000`–`0x086E08` (`asm/cod/000000.s`) | início do bloco das **dlls do jogo** (HLSDK C++), arquivos em ordem alfabética | **trabalho em andamento** (ver seção 4) |
| `0x0C3350`–`0x0D3508` (`asm/cod/0C3350.s`) | `player.cpp` (~180 funções, `.rodata` 0x36AF40–0x36DCC8) | adiado por ser enorme |
| `0x1D0A68` em diante (`asm/cod/1D0A68.s`) | SDK da Sony (libgraph, libpad, libcdvd, libmc, libscf), libm, libstdc++ | **não descompilar** (bibliotecas) |

Já feito (está em `src/`): praticamente todo o engine em C (`src/engine`), o código da Gearbox
(`src/gbx`, C e C++), a zlib 1.1.3 (`src/zlib`) e as dlls de `hgrunt.cpp` até `zombie.cpp`
(`src/dlls`, ~57 arquivos). Algumas funções ficaram `NON_MATCHING` (código C presente, mas o build
usa o asm original via `INCLUDE_ASM`); são candidatas a melhorar depois. Para listar:
`grep -rn "NON_MATCHING" src | wc -l`.

## 4. O que eu estava fazendo e qual é o próximo passo

Eu estava descompilando as **dlls de trás para frente** (de `zombie.cpp` para trás, em ordem
alfabética invertida). Os últimos arquivos feitos foram `hornet.cpp`, `hlg_world.cpp` e
`hgrunt.cpp` (detalhes em `docs/notas/hl1ps2-dlls-done-3.md`). Nada ficou pela metade: o último
arquivo foi commitado e a ROM bate.

**Próximo arquivo: `healthkit.cpp`** (depois `headcrab.cpp`, `hassassin.cpp`, `handgrenade.cpp`, `h_export.cpp`, `h_cycler.cpp`, `h_cine.cpp`,
`h_battery.cpp`, `h_ai.cpp`, `gman.cpp`, `glock.cpp`, `globals.cpp`, `ggrenade.cpp`, … até o
início do bloco em 0x100000). A ordem real na ROM pode ter arquivos só do PS2 no meio
(ex.: `modeltrain.cpp`, `mtank.cpp`), por isso confirme sempre pelas strings da `.rodata`.

Fronteiras conhecidas do próximo arquivo:
- `.text` termina em **0x186E08** (vram), onde começa o `hgrunt.cpp`.
- `.data` termina em **0x3135D8** (início do `m_SaveData` do CHGrunt).
- `.rodata` termina em **0x35A850** (o `"classname"` em 0x35A850 já é do hgrunt). Tem a classe
  nova do PS2 `CWallHealthNew` (`item_healthchargercan`, `models/health_charger_body.dol`).
- Chamadas a `EHANDLE`: a ROM às vezes usa `operator CBaseEntity*` onde o código natural chama
  `operator int`; o `cmpfile.sh` não vê (só o cmp da ROM). Ver `docs/notas/hl1ps2-dlls-hgrunt.md`.
- O `cmpfile.sh` pode dizer que tudo bate mesmo com uma constante float errada (ignora o `lui`
  do imediato); só o build completo garante.

## 5. Receita para descompilar um arquivo das dlls

(Detalhes e exemplos em `docs/notas/hl1ps2-dlls.md` e nos arquivos `hl1ps2-dlls-*.md`.)

1. **Achar a faixa.** No `asm/cod/000000.s`, a primeira função de cada arquivo é a do
   `LINK_ENTITY_TO_CLASS` (chama o `GetClassPtr` do arquivo). No fim do arquivo vêm: a função de
   registro de ponteiros de Think/Touch/Use (chama `func_002397D0`/`func_002397F0`/`func_00239810`
   com strings numéricas tipo `"163"`), uma cópia weak de construtor (ex. `__12CBaseMonster`), os
   `GetClassPtr`, `__static_initialization_and_destruction_0` (registra o nome da entidade via
   `func_0021D8A8`) e `_GLOBAL_$I$...`.
2. **Pegar o fonte do HLSDK** (`https://raw.githubusercontent.com/ValveSoftware/halflife/master/dlls/<arquivo>`)
   e criar `src/dlls/<arquivo>.cpp`, copiando o cabeçalho de um arquivo vizinho
   (ex. `src/dlls/houndeye.cpp`): includes `dlls/...`, `typedef struct _FILE FILE;`,
   `#define DLLS_MONSTERS_CPP` para monstros, `#include "include_asm.h"`.
3. **Adaptações típicas do PS2:**
   - todas as constantes float com sufixo `f` (`0.3` → `0.3f`, `.2` → `.2f`); `fabs` → `fabsf`;
     `atan2`/`fmod`/`sqrt` → versões `f`;
   - modelos `.mdl` → `.dol`, sprites `.spr` → `.spz`;
   - campos novos de classe que aparecem no `m_SaveData` da ROM (leia a tabela TYPEDESCRIPTION
     na `.data` para ver nomes e offsets);
   - variáveis `static` locais viram globais em asm (o `.bss` é removido): use
     `extern "C" float D_XXXXXXXX; #define nome D_XXXXXXXX`;
   - cvars do engine são ponteiros para `ps2cvar_t` (ver `src/dlls/houndeye.cpp`);
   - multiplayer: `g_pGameRules->IsMultiplayer()` costuma cortar sons ou precaches;
   - split-screen: `extern "C" int cl_numlocal;`.
4. **Comparar**: crie um arquivo de pares `nome_mangled=0xENDERECO` (ordem do `nm -n` do .o casada
   com a ordem dos `glabel` da ROM) e rode `tools/handoff/cmpfile.sh <arquivo> pares.txt`. Para
   ver a diferença de uma função: `tools/handoff/difffile.sh <arquivo> Nome 0xADDR TAMANHO`. Para
   comparar dados: `python3 tools/handoff/datacmp.py build/src/dlls/x.o .data 0xADDR` (e `.rodata`).
5. **Integrar**: acrescentar no `SLUS_200.66.yaml` as linhas `[offset, cpp, dlls/x]` (texto),
   `[offset, .data, dlls/x]` e `[offset, .rodata, dlls/x]` (offset = vram − 0x100000), e no
   `symbol_addrs.txt` os nomes das funções, `_GLOBAL_$I$<entidade>` e `m_SaveData__...`.
   Se a `.rodata` do arquivo seguinte começar alinhada a 16, adicione a linha
   `--set-section-alignment .rodata=16` daquele `.o` no `Makefile` (e apague o `.o` para
   recompilar).
6. **Funções que não casam** depois de tentativas razoáveis: envolva com
   `python3 tools/handoff/wrapnm.py src/dlls/x.cpp "início exato da função" NomeMangled asm/nonmatchings/dlls/x`
   e escreva um comentário `/* NON_MATCHING: motivo */`. O padrão mais comum que não casa é a
   conversão float→int (`trunc.w.s`) agendada de outro jeito.
   `tools/handoff/vtry.py` testa várias variantes de um trecho de uma vez.
7. **Build completo e commit**: `tools/handoff/commit.sh "dlls: x.cpp (...) em C++, N/M"`.
   Se a ROM falhar, ache o byte diferente com `cmp -l build/SLUS_200.66.rom SLUS_200.66.rom`
   (offset da rom + 0x100000 = vram). O `cmpfunc` ignora o alvo de `jal`, então um `jal` para a
   função errada só aparece nesse `cmp` (ex.: `UTIL_TraceLine` vs `UTIL_TraceLineNoEnts`).
8. **Atualizar as notas** em `docs/notas/` (uma linha no `hl1ps2-dlls-done-2.md` e o "Próximo" em
   `hl1ps2-dlls.md`).

## 6. Ferramentas

- `tools/handoff/` — scripts do fluxo acima (build completo, commit seguro, comparação por arquivo,
  diff, dados, NON_MATCHING, variantes). Rodar a partir da raiz do repositório.
- `tools/cmpfunc.py`, `tools/difffunc.py` — comparam funções de um `.o` com a ROM.
- `tools/fixasm.py` — pós-processa o asm do ee-gcc para o gas moderno (regras importantes em
  `docs/notas/hl1ps2-next-steps.md` e `hl1ps2-matching-lessons-4.md`).
- `tools/permuter_setup.py` + decomp-permuter — para casos difíceis.
- `make split` precisa do venv com splat ativado; sem ele o split apaga `asm/` e falha.

## 7. Outras pendências (menos prioritárias)

- `player.cpp` (0x1C3350–0x1D3508 vram).
- Revisitar funções `NON_MATCHING` (`grep -rn "NON_MATCHING" src`).
- Microcódigo VU (`.vutext`) e overlays são tratados como binário.
