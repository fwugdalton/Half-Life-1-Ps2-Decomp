#!/usr/bin/env python3
"""Troca instruções de macro do VU0 (vmulax.xyzw ACC, ... $vf8x) por .word.

O gas do binutils não aceita a sintaxe que o splat gera para elas, então
depois do split cada linha `/* off vram BYTES */  vXXX ...` vira `.word`."""
import glob
import re

pat = re.compile(r'^(\s*/\* \w+ \w+ ([0-9A-F]{8}) \*/\s+)(v[a-z0-9.]+)\s.*$')
for path in glob.glob('asm/**/*.s', recursive=True):
    lines = open(path).read().split('\n')
    changed = False
    for i, l in enumerate(lines):
        m = pat.match(l)
        if m:
            b = bytes.fromhex(m.group(2))
            word = int.from_bytes(b, 'little')
            lines[i] = '%s.word 0x%08X /* %s */' % (m.group(1), word, l[m.end(1):].strip())
            changed = True
    if changed:
        open(path, 'w').write('\n'.join(lines))
