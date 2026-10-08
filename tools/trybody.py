"""Testa variantes de uma função: substitui o corpo da função no .c, compila só o .o e compara.

Uso: trybody.py src/arquivo.c Funcao variante1.c [variante2.c ...]
Cada variante contém a definição completa da função. O arquivo original é restaurado no fim.
"""
import sys, os, subprocess, re
sys.path.insert(0, os.path.dirname(__file__)); import cmpfunc as c
src, fn = sys.argv[1], sys.argv[2]
orig = open(src).read()
m = re.search(r'^[^\n]*\b%s\([^)]*\)\n\{.*?^\}\n' % re.escape(fn), orig, re.S | re.M)
obj = 'build/' + src[:-2] + '.o'
try:
    for v in sys.argv[3:]:
        open(src, 'w').write(orig[:m.start()] + open(v).read() + orig[m.end():])
        r = subprocess.run(['make', '-s', obj], capture_output=True, text=True)
        if r.returncode: print(v, 'ERRO', r.stderr[-500:]); continue
        funcs, text, relocs = c.obj_funcs(obj)
        sa = c.symaddrs()
        for name, off, size in funcs:
            if name == fn:
                ok, diffs = c.compare(name, off, size, text, relocs, sa[name])
                print('%-30s %s %d bytes, %d diferenças' % (v, 'OK' if ok else 'XX', size, diffs))
finally:
    open(src, 'w').write(orig)
