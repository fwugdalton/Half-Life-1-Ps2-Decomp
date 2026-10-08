"""Mostra o asm de funções de asm/nonmatchings limpo, com as strings referenciadas anotadas.

Uso: showfn.py dir_ou_arquivo.s [...]
"""
import sys, os, re, glob
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
d = open(os.path.join(ROOT, 'SLUS_200.66'), 'rb').read()
o = lambda v: v - 0x100000 + 0x1000
syms = {}
for line in open(os.path.join(ROOT, 'symbol_addrs.txt')):
    m = re.match(r'\s*(\S+)\s*=\s*(0x[0-9A-Fa-f]+)', line)
    if m: syms[m.group(1)] = int(m.group(2), 16)
def strat(a):
    s = d[o(a):o(a) + 70].split(b'\0')[0]
    if len(s) >= 1 and all(32 <= c < 127 or c in (9, 10, 13) for c in s):
        return repr(s.decode())
    return None
files = []
for a in sys.argv[1:]:
    files += sorted(glob.glob(a + '/*.s')) if os.path.isdir(a) else [a]
for f in files:
    for l in open(f):
        l = l.rstrip()
        if l.startswith('.align') or not l or l.startswith('nonmatching') or l.startswith('endlabel') or l.startswith('.section') or l.startswith('.size'):
            continue
        l = re.sub(r'/\* [0-9A-F]+ [0-9A-F]+ [0-9A-F]+ \*/ ', '', l)
        m = re.search(r'%lo\((D_([0-9A-F]{8}))\)', l)
        if m:
            a = int(m.group(2), 16)
            if 0x380000 <= a < 0x399000:
                s = strat(a)
                if s: l += '   # ' + s
                else:
                    import struct
                    l += '   # f=%g' % struct.unpack_from('<f', d, o(a))[0]
        print(l)
