#!/bin/bash
# tools/handoff/scan.sh src/arq.c : compila com -DNON_MATCHING e mostra quantas instruções diferem em cada função NON_MATCHING
cd "$(dirname "$0")/../.." || exit 1
f=$1; b=${f%.*}; o=build/${b}.o
cmd=$(make -n -W $f $o 2>/dev/null | grep "ee-gcc" | head -1)
[ -z "$cmd" ] && { echo "sem cmd $f"; exit 1; }
T=${TMPDIR:-/tmp}/nmscan/t_$(basename $b); mkdir -p $T
cmd=$(echo "$cmd" | sed "s| -S -o [^ ]*| -DNON_MATCHING -S -o $T/a.s0|")
eval "$cmd" 2>&1 | grep -i "error" | head -3
python3 tools/fixasm.py $T/a.s0 $T/a.s || exit 1
mips-linux-gnu-as -EL -march=r5900 -mabi=eabi -G0 -no-pad-sections -I include -I . -o $T/a.o $T/a.s 2>&1 | grep -v "used .at\|Assembler messages" | head -3
pairs=""
for n in $(grep -o 'INCLUDE_ASM("[^"]*", *[A-Za-z0-9_$]*' $f | sed 's/.*, *//'); do
  s=$(grep -rl "glabel $n\$" asm/nonmatchings 2>/dev/null | head -1)
  [ -z "$s" ] && continue
  a=$(grep -A1 "glabel $n\$" $s | tail -1 | grep -o "^ */\* [0-9A-F]* [0-9A-F]*" | awk '{print $3}')
  [ -n "$a" ] && pairs="$pairs $n=0x$a"
done
python3 tools/cmpfunc.py $T/a.o $pairs 2>&1 | grep -v "^OK" | sed "s|^|$f: |"
python3 tools/cmpfunc.py $T/a.o $pairs 2>&1 | grep "^OK" | sed "s|^|$f: |"
