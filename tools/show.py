"""Imprime o asm (sem comentários de endereço) de funções pelo nome ou endereço."""
import sys, os, re, glob
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
names = [n if not re.fullmatch(r'[0-9A-Fa-f]{6,8}', n) else 'func_%08X' % int(n, 16) for n in sys.argv[1:]]
files = sorted(glob.glob(os.path.join(ROOT, 'asm/**/*.s'), recursive=True))
want = set(names)
for path in files:
    if '/asm/data/' in path: continue
    out = None
    for line in open(path):
        m = re.match(r'^glabel (\S+)', line)
        if m and m.group(1) in want:
            out = []
            want.discard(m.group(1))
        if out is not None:
            line = re.sub(r'/\* [0-9A-F]+ [0-9A-F]{8} [0-9A-F]{8} \*/', '', line)
            if line.strip() and not line.startswith('.align') and not line.startswith('nonmatching'):
                out.append(line.rstrip())
            if line.startswith('endlabel'):
                print('\n'.join(out)); out = None
    if not want: break
