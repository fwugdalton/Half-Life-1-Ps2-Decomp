#!/bin/sh
# chkall.sh src/engine/X.c : romchk em todas as funções em C (fora de NON_MATCHING)
src=$1; obj=build/${src%.c}.o
fns=$(python3 - "$src" <<'PY'
import re,sys
s=open(sys.argv[1]).read()
# remove blocos NON_MATCHING
s=re.sub(r'#ifdef NON_MATCHING.*?#endif','',s,flags=re.S)
print(' '.join(re.findall(r'^[A-Za-z_][\w \*]*?\b(\w+)\([^;]*\)\n\{',s,re.M)))
PY
)
python3 tools/romchk.py $obj $fns | grep -v " 0$"
