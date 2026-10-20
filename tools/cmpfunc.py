"""Compara as funções de um .o compilado com os bytes originais (rom).

Uso: cmpfunc.py arquivo.o [nome=endereco ...]
Sem pares nome=endereco, usa symbol_addrs.txt para achar o endereço de cada função.
Campos de relocação (jal/hi/lo) são mascarados: só opcode e registradores são comparados.
"""
import sys, os, re, subprocess, struct
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CROSS = os.environ.get('CROSS', 'mips-linux-gnu-')
ROM = open(os.path.join(ROOT, 'SLUS_200.66.rom'), 'rb').read()
VRAM = 0x100000

def symaddrs():
    d = {}
    for line in open(os.path.join(ROOT, 'symbol_addrs.txt')):
        m = re.match(r'\s*(\S+)\s*=\s*(0x[0-9A-Fa-f]+)', line)
        if m: d[m.group(1)] = int(m.group(2), 16)
    return d

def obj_funcs(o):
    out = subprocess.check_output([CROSS + 'readelf', '-sW', o], text=True)
    funcs = []
    for line in out.splitlines():
        p = line.split()
        if len(p) >= 8 and p[3] == 'FUNC' and p[6] != 'UND':
            funcs.append((p[7], int(p[1], 16), int(p[2])))
    # num arquivo temporário: com /dev/stdout num pipe o objcopy às vezes não escreve nada
    import tempfile
    with tempfile.NamedTemporaryFile(suffix='.bin') as tmp:
        subprocess.run([CROSS + 'objcopy', '-O', 'binary', '-j', '.text', o, tmp.name], check=True)
        text = open(tmp.name, 'rb').read()
    rel = subprocess.check_output([CROSS + 'objdump', '-r', '-j', '.text', o], text=True)
    relocs = {int(m.group(1), 16): m.group(2) for m in re.finditer(r'^([0-9a-f]{8}) (R_MIPS_\w+)', rel, re.M)}
    global RELSYM
    RELSYM = {int(m.group(1), 16): m.group(3) for m in re.finditer(r'^([0-9a-f]{8}) (R_MIPS_\w+)\s+(\S+)', rel, re.M)}
    return funcs, text, relocs

RELSYM = {}
SA = {}

def symaddr(name):
    if name in SA: return SA[name]
    m = re.fullmatch(r'(?:D|func|jtbl)_([0-9A-F]{8})', name)
    return int(m.group(1), 16) if m else None

def relcheck(text, off, relocs, a, b, r):
    # confere o endereço completo quando o símbolo tem endereço conhecido (%hi/%lo)
    sym = RELSYM.get(off)
    addr = symaddr(sym) if sym else None
    if addr is None or r not in ('R_MIPS_HI16', 'R_MIPS_LO16'): return True
    if r == 'R_MIPS_LO16':
        full = addr + ((a & 0xFFFF) ^ 0x8000) - 0x8000
        return (full & 0xFFFF) == (b & 0xFFFF)
    for j in range(off + 4, off + 64, 4):
        if relocs.get(j) == 'R_MIPS_LO16' and RELSYM.get(j) == sym:
            lo = struct.unpack('<I', text[j:j + 4])[0]
            full = addr + ((lo & 0xFFFF) ^ 0x8000) - 0x8000
            return ((full + 0x8000) >> 16) & 0xFFFF == (b & 0xFFFF)
    return True

def mask(w, r):
    if r is None: return w
    if r == 'R_MIPS_26': return w & 0xFC000000
    return w & 0xFFFF0000

def compare(name, off, size, text, relocs, addr):
    ok = True; diffs = 0
    for i in range(0, size, 4):
        a = struct.unpack('<I', text[off + i:off + i + 4])[0]
        b = struct.unpack('<I', ROM[addr - VRAM + i:addr - VRAM + i + 4])[0]
        r = relocs.get(off + i)
        if mask(a, r) != mask(b, r) or (r is None and a != b) or not relcheck(text, off + i, relocs, a, b, r):
            diffs += 1; ok = False
    # tamanho: a próxima palavra do original deve começar outra função (não checado aqui)
    return ok, diffs

if __name__ == '__main__':
    o = sys.argv[1]
    pairs = dict((k, int(v, 16)) for k, v in (a.split('=') for a in sys.argv[2:]))
    sa = symaddrs(); sa.update(pairs); SA.update(sa)
    funcs, text, relocs = obj_funcs(o)
    allok = True
    for name, off, size in funcs:
        m = re.fullmatch(r'func_([0-9A-F]{8})', name)
        if m and name not in sa: sa[name] = int(m.group(1), 16)
        if name not in sa:
            print('?? %-32s sem endereço' % name); continue
        ok, diffs = compare(name, off, size, text, relocs, sa[name])
        allok &= ok
        print('%s %-32s %08x %4d bytes%s' % ('OK' if ok else 'XX', name, sa[name], size, '' if ok else '  (%d instruções diferentes)' % diffs))
    sys.exit(0 if allok else 1)
