"""Troca func_XXXXXXXX / D_XXXXXXXX nos .c e .h pelos nomes que já estão em symbol_addrs.txt.

Uso: rename_syms.py   (roda em src/ e include/)
"""
import re, os, glob
names = {}
for line in open('symbol_addrs.txt'):
    m = re.match(r'\s*(\w+)\s*=\s*0x([0-9A-Fa-f]+);', line)
    if m and not re.match(r'(func|D)_[0-9A-F]{8}$', m.group(1)):
        a = int(m.group(2), 16)
        names['func_%08X' % a] = m.group(1)
        names['D_%08X' % a] = m.group(1)
pat = re.compile(r'\b(?:func|D)_[0-9A-F]{8}\b')
for f in glob.glob('src/**/*.c', recursive=True) + glob.glob('include/**/*.h', recursive=True):
    s = open(f).read()
    t = pat.sub(lambda m: names.get(m.group(0), m.group(0)), s)
    if t != s:
        open(f, 'w').write(t)
        print('renomeado em', f)
