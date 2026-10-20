"""Descobre o endereço original dos símbolos indefinidos de um .o já casado.

Uso: resolveundef.py obj.o secao=endereco_vram ...  (ex.: .text=0x2162C0 .rodata=0x384A98 .data=0x325018)
Lê as relocações R_MIPS_26/HI16/LO16/32 contra símbolos indefinidos e tira o valor da ROM.
Imprime linhas no formato do symbol_addrs.txt."""
import sys, os, struct, subprocess, re
sys.path.insert(0, os.path.dirname(__file__)); import cmpfunc as c
o = sys.argv[1]
base = {k: int(v, 16) for k, v in (a.split('=') for a in sys.argv[2:])}
und = set(l.split()[-1] for l in subprocess.check_output([c.CROSS + 'nm', '-u', o], text=True).splitlines())
out = subprocess.check_output([c.CROSS + 'readelf', '-rW', o], text=True)
word = lambda va: struct.unpack('<I', c.ROM[va - c.VRAM:va - c.VRAM + 4])[0]
secdata = {}
def oword(sec, off):
    # a parte do endereço que já vem no .o (o addend do REL)
    if sec not in secdata:
        secdata[sec] = subprocess.check_output([c.CROSS + 'objcopy', '-O', 'binary', '-j', sec, o, '/dev/stdout'])
    return struct.unpack('<I', secdata[sec][off:off + 4])[0]
s16 = lambda x: struct.unpack('<h', struct.pack('<H', x & 0xFFFF))[0]
res, sec, hi, ohi = {}, None, {}, {}
for l in out.splitlines():
    m = re.match(r"Relocation section '\.rel(\S+)'", l)
    if m: sec = m.group(1); continue
    p = l.split()
    if len(p) < 5 or not re.match(r'^[0-9a-f]{8}$', p[0]) or sec not in base: continue
    off, typ, name = int(p[0], 16), p[2], p[-1]
    if name not in und: continue
    w = word(base[sec] + off)
    if typ == 'R_MIPS_26': v = ((base[sec] + off) & 0xF0000000) | ((w & 0x3FFFFFF) << 2)
    elif typ == 'R_MIPS_HI16': hi[name] = (w & 0xFFFF) << 16; ohi[name] = (oword(sec, off) & 0xFFFF) << 16; continue
    elif typ == 'R_MIPS_LO16':
        if name not in hi: continue
        v = (hi[name] + s16(w) - (ohi[name] + s16(oword(sec, off)))) & 0xFFFFFFFF
    elif typ == 'R_MIPS_32': v = (w - oword(sec, off)) & 0xFFFFFFFF
    else: continue
    if name in res and res[name] != v: print('// CONFLITO %s %#x %#x' % (name, res[name], v))
    res.setdefault(name, v)
for n in sorted(und):
    if n in res: print('%s = 0x%08X;' % (n, res[n]))
    else: print('// sem endereço: %s' % n)
