# romchk.py obj.o Func... : compara a função do .o (resolvendo relocações com os endereços do
# build/SLUS_200.66.elf e do .map) com os bytes da ROM original. Imprime nº de palavras diferentes.
import sys, re, subprocess, struct
obj = sys.argv[1]; funcs = [a for a in sys.argv[2:] if not a.startswith('-')]
verbose = '-v' in sys.argv
ROM = open('SLUS_200.66.rom', 'rb').read()
syms = {}
for l in subprocess.run(['mips-linux-gnu-nm', 'build/SLUS_200.66.elf'], capture_output=True, text=True).stdout.splitlines():
    p = l.split()
    if len(p) == 3: syms.setdefault(p[2], int(p[0], 16))
secs = {}
mp = open('build/SLUS_200.66.map').read()
for m in re.finditer(r'^ (\.\w+)\s+0x([0-9a-f]+)\s+0x([0-9a-f]+) %s$' % re.escape(obj), mp, re.M):
    if int(m.group(3), 16): secs.setdefault(m.group(1), int(m.group(2), 16))
out = subprocess.run(['mips-linux-gnu-objdump', '-dr', '-z', obj], capture_output=True, text=True).stdout
for fn in funcs:
    m = re.search(r'<%s>:\n(.*?)(?=\n\n|\Z)' % re.escape(fn), out, re.S)
    if not m: print(fn, 'não achado'); continue
    insns = []; rel = {}
    for line in m.group(1).splitlines():
        a = re.match(r'\s*([0-9a-f]+):\s+([0-9a-f]{8})\s', line)
        if a: insns.append((int(a.group(1), 16), int(a.group(2), 16))); continue
        r = re.match(r'\s*([0-9a-f]+): (R_MIPS_\w+)\s+(\S+)', line)
        if r:
            s = r.group(3); add = 0
            mm = re.match(r'(.*?)([+-]0x[0-9a-f]+)$', s)
            if mm: s, add = mm.group(1), int(mm.group(2), 16)
            rel[int(r.group(1), 16)] = (r.group(2), s, add)
    m2 = re.match(r'func_([0-9A-F]{8})$', fn)
    base = int(m2.group(1), 16) if m2 else syms[fn]
    off0 = insns[0][0]
    bad = 0
    # pares hi/lo: addend do hi vem do lo seguinte com o mesmo símbolo
    lst = list(rel.items())
    for i, (o, w) in enumerate(insns):
        addr = base + (o - off0)
        ow = struct.unpack('<I', ROM[addr - 0x100000: addr - 0x100000 + 4])[0]
        exp = w
        if o in rel:
            t, s, add = rel[o]
            S = secs.get(s) if s.startswith('.') else syms.get(s)
            if S is None: S = 0
            if t == 'R_MIPS_LO16':
                v = (S + add + ((w & 0xffff) ^ 0x8000) - 0x8000) & 0xffff
                exp = (w & 0xffff0000) | v
            elif t == 'R_MIPS_HI16':
                lo = 0
                for o2, (t2, s2, a2) in sorted(rel.items()):
                    if o2 > o and t2 == 'R_MIPS_LO16' and s2 == s:
                        w2 = [x for y, x in insns if y == o2]
                        if w2: lo = ((w2[0] & 0xffff) ^ 0x8000) - 0x8000
                        break
                full = S + add + ((w & 0xffff) << 16) + lo
                exp = (w & 0xffff0000) | (((full + 0x8000) >> 16) & 0xffff)
            elif t == 'R_MIPS_26':
                tgt = S + add + ((w & 0x3ffffff) << 2)
                exp = (w & 0xfc000000) | ((tgt >> 2) & 0x3ffffff)
            elif t == 'R_MIPS_GPREL16':
                exp = ow  # não verificado
        if exp != ow:
            bad += 1
            if verbose: print('  %x: nosso %08x orig %08x %s' % (addr, exp, ow, rel.get(o, '')))
    print(fn, bad)
