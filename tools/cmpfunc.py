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
    text = subprocess.run([CROSS + 'objcopy', '-O', 'binary', '-j', '.text', o, '/dev/stdout'], capture_output=True).stdout
    rel = subprocess.check_output([CROSS + 'objdump', '-r', '-j', '.text', o], text=True)
    relocs = {int(m.group(1), 16): m.group(2) for m in re.finditer(r'^([0-9a-f]{8}) (R_MIPS_\w+)', rel, re.M)}
    return funcs, text, relocs

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
        if mask(a, r) != mask(b, r) or (r is None and a != b):
            diffs += 1; ok = False
    # tamanho: a próxima palavra do original deve começar outra função (não checado aqui)
    return ok, diffs

if __name__ == '__main__':
    o = sys.argv[1]
    pairs = dict((k, int(v, 16)) for k, v in (a.split('=') for a in sys.argv[2:]))
    sa = symaddrs(); sa.update(pairs)
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
