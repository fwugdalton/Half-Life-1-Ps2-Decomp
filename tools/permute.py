# permute.py src Func "linhaA|linhaB|..." [max] [-a]: testa permutações das linhas contíguas (use @@ como separador se as linhas têm |)
import sys, itertools, subprocess, random
src, fn, spec = sys.argv[1], sys.argv[2], sys.argv[3]
mx = int(sys.argv[4]) if len(sys.argv) > 4 and sys.argv[4] != '-a' else 200
lines = spec.split('@@') if '@@' in spec else spec.split('|')
s = open(src).read()
import re as _re
ind = '    '
for _m in _re.finditer(r'\n([ \t]*)' + _re.escape(lines[0]) + r'\n', s):
    if ''.join(_m.group(1) + '%s\n' % l for l in lines) in s:
        ind = _m.group(1)
        break
blk = ''.join(ind + '%s\n' % l for l in lines)
assert blk in s, 'bloco não achado'
obj = 'build/' + src[:-2] + '.o'
perms = list(itertools.permutations(lines))
random.seed(1); random.shuffle(perms)
best = None
try:
    for p in perms[:mx]:
        nb = ''.join(ind + '%s\n' % l for l in p)
        open(src, 'w').write(s.replace(blk, nb))
        subprocess.run(['rm', '-f', obj]); r = subprocess.run(['make', obj], capture_output=True)
        if r.returncode: continue
        d = int(subprocess.run(['python3', 'tools/romchk.py', obj, fn], capture_output=True, text=True).stdout.split()[-1])
        d += subprocess.run(['python3', 'tools/difffunc.py', obj, fn], capture_output=True, text=True).stdout.count(' * ')
        if best is None or d < best[0]:
            best = (d, p); print(d, ' | '.join(p), flush=True)
        if d == 0: break
finally:
    if best and (best[0] == 0 or '-a' in sys.argv):
        open(src, 'w').write(s.replace(blk, ''.join(ind + '%s\n' % l for l in best[1])))
        print('aplicado')
    else:
        open(src, 'w').write(s)
    subprocess.run(['make', obj], capture_output=True)
