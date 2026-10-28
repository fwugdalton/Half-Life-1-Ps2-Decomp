#!/bin/bash
# Build completo: renomeia símbolos, refaz o split (splat), regenera a rodata do sv_main,
# recompila tudo e compara com a ROM original. Precisa imprimir "build/SLUS_200.66.rom: OK".
# Uso: tools/handoff/fullbuild.sh   (rodar da raiz do repositório)
cd "$(dirname "$0")/../.." || exit 1
# o splat precisa de um venv com splat64 (requirements.txt); ajuste o caminho se necessário
[ -f ../.venv/bin/activate ] && source ../.venv/bin/activate
python3 tools/rename_syms.py
make split >/dev/null 2>&1 || { echo SPLIT FAIL; exit 1; }
python3 tools/rodata_blocks.py src/engine/sv_main.c tools/rodata_cache/engine_sv_main.rodata.s
touch src/engine/*.c
make 2>&1 | grep -E "error|Error|undefined|OK|FAIL|differ" | head -20
cmp -l SLUS_200.66.rom build/SLUS_200.66.rom | wc -l
