"""Substitui a definição de uma função no .c pela do arquivo dado.

Uso: applybody.py src/arquivo.c Funcao variante.c
"""
import sys, re
src, fn, v = sys.argv[1:4]
s = open(src).read()
m = re.search(r'^[^\n]*\b%s\([^)]*\)\n\{.*?^\}\n' % re.escape(fn), s, re.S | re.M)
open(src, 'w').write(s[:m.start()] + open(v).read() + s[m.end():])
