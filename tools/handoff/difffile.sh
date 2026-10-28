#!/bin/sh
# Diff lado a lado de uma função: coluna da esquerda = nosso .o, direita = ROM; "*" marca diferença.
# Uso: tools/handoff/difffile.sh houndeye NomeMangled 0xENDERECO TAMANHO
cd "$(dirname "$0")/../.." || exit 1
python3 tools/difffunc.py build/src/dlls/$1.o $2=$3 $4
