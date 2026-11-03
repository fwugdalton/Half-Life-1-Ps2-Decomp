#!/bin/bash
# tools/handoff/runq.sh lista.txt worker (PERMUTER=caminho do decomp-permuter, PERMDIR=saída)  : cada linha "arquivo funcao"; o worker N pega as linhas com (i % 4)==N
cd "$(dirname "$0")/../.." || exit 1
i=0
while read f fn; do
  if [ $((i % 4)) -eq $2 ]; then
    d=${PERMDIR:-/tmp/perm}/$fn
    [ -d $d ] || python3 tools/permuter_setup.py $f $fn $d >/dev/null 2>&1
    (cd ${PERMUTER:-../decomp-permuter} && timeout 600 python3 permuter.py $d --stop-on-zero > $d.log 2>&1)
    ls -d $d/output-0-* >/dev/null 2>&1 && echo "ZERO $f $fn" || echo "fim $fn"
  fi
  i=$((i+1))
done < $1
