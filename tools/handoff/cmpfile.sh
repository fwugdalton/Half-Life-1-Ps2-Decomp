#!/bin/sh
# Recompila um .cpp das dlls e compara cada função com a ROM.
# Uso: tools/handoff/cmpfile.sh houndeye pares.txt
#   pares.txt: uma linha "nome_mangled=0xENDERECO" por função (ordem do nm = ordem dos glabel na ROM;
#   gere com: paste -d= <(nm -n ... | nomes) <(glabels do asm com func_ trocado por 0x)).
# Imprime só as funções que NÃO casam ("XX nome endereço tamanho (N instruções diferentes)").
cd "$(dirname "$0")/../.." || exit 1
rm -f build/src/dlls/$1.o
make build/src/dlls/$1.o 2>&1 | grep -E " error|rror:|: parse|undeclared|no matching|lacks a cast" | head -12
python3 tools/cmpfunc.py build/src/dlls/$1.o $(cat "$2") 2>&1 | grep -v "^OK"
