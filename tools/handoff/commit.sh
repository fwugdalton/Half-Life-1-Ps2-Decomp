#!/bin/bash
# Uso: tools/handoff/commit.sh "mensagem"
# Faz o build completo e só commita/envia se a ROM bater byte a byte.
cd "$(dirname "$0")/../.." || exit 1
out=$(timeout 900 tools/handoff/fullbuild.sh 2>&1)
echo "$out" | tail -2
echo "$out" | grep -q "SLUS_200.66.rom: OK" || { echo "BUILD FALHOU, sem commit"; exit 1; }
git add -A src include tools docs symbol_addrs.txt Makefile SLUS_200.66.yaml linker_script_extra.ld
git commit -qm "$1" || exit 1
git push -q origin master
git log --oneline -1
