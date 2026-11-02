"""Troca func_XXXXXXXX / D_XXXXXXXX nos .c e .h pelos nomes que já estão em symbol_addrs.txt.

Uso: rename_syms.py   (roda em src/ e include/)
"""
import re, os, glob
names = {}
# endereços que os headers/fontes ainda chamam pelo nome func_ (aliases no linker_script_extra.ld)
KEEP = {0x001066C0, 0x001066D0, 0x00106758, 0x00106038, 0x00106048, 0x001060D0, 0x001063D8, 0x00106BA0, 0x00106CC0, 0x00106CE8, 0x00106D28, 0x00106D30, 0x00107000, 0x00107398, 0x00107A40, 0x00107C10, 0x00107C20, 0x00107CE0, 0x00107CF8, 0x00107EE0, 0x00107F00, 0x00106F90, 0x001079E0, 0x00108008, 0x00108018, 0x00108070, 0x00108120, 0x00108148}
for line in open('symbol_addrs.txt'):
    m = re.match(r'\s*(\w+)\s*=\s*0x([0-9A-Fa-f]+);', line)
    if m and not re.match(r'(func|D)_[0-9A-F]{8}$', m.group(1)):
        a = int(m.group(2), 16)
        if a in KEEP:
            continue
        names['func_%08X' % a] = m.group(1)
        names['D_%08X' % a] = m.group(1)
pat = re.compile(r'\b(?:func|D)_[0-9A-F]{8}\b')
for f in glob.glob('src/**/*.c', recursive=True) + glob.glob('include/**/*.h', recursive=True):
    s = open(f).read()
    t = pat.sub(lambda m: names.get(m.group(0), m.group(0)), s)
    if t != s:
        open(f, 'w').write(t)
        print('renomeado em', f)
