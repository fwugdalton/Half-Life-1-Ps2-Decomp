"""Substitui linhas INCLUDE_ASM pelo código C das funções num arquivo de trechos.

Uso: putfunc.py src/arquivo.c trechos.c
O trecho de cada função vai no lugar do INCLUDE_ASM dela (pelo nome ou pelo func_XXXXXXXX
do endereço em symbol_addrs.txt). Texto antes da primeira função do trecho (externs etc.)
vai junto com a primeira. Também renomeia INCLUDE_ASM func_XXXXXXXX que já têm nome.
"""
import sys, re, os
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
src, frag = sys.argv[1], sys.argv[2] if len(sys.argv) > 2 else None
names = {}
for line in open(os.path.join(ROOT, 'symbol_addrs.txt')):
    m = re.match(r'\s*(\w+)\s*=\s*0x([0-9A-Fa-f]+);', line)
    if m: names['func_%08X' % int(m.group(2), 16)] = m.group(1)
s = open(src).read()
# renomeia INCLUDE_ASM antigos
s = re.sub(r'(INCLUDE_ASM\("[^"]*", )(func_[0-9A-F]{8})\);', lambda m: m.group(1) + names.get(m.group(2), m.group(2)) + ');', s)
if frag:
    code = open(frag).read()
    defs = list(re.finditer(r'^[A-Za-z_][^\n;=]*?\b(\w+)\([^;{]*\)\n\{.*?^\}\n', code, re.S | re.M))
    prev = 0
    for d in defs:
        fn = d.group(1)
        chunk = code[prev:d.end()]
        prev = d.end()
        pat = re.compile(r'^INCLUDE_ASM\("[^"]*", %s\);\n' % re.escape(fn), re.M)
        if not pat.search(s):
            sys.exit('sem INCLUDE_ASM para ' + fn)
        s = pat.sub(lambda m: chunk.lstrip('\n'), s, count=1)
open(src, 'w').write(s)
