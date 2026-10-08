"""Mostra o disassembly lado a lado (compilado x original) de uma função de um .o."""
import sys, os, subprocess, struct, re
sys.path.insert(0, os.path.dirname(__file__)); import cmpfunc as c
o, name = sys.argv[1], sys.argv[2]
funcs, text, relocs = c.obj_funcs(o)
sa = c.symaddrs()
off, size = [(f[1], f[2]) for f in funcs if f[0] == name][0]
addr = sa[name] if name in sa else int(name[5:], 16)
osize = max(size, int(sys.argv[3], 16) if len(sys.argv) > 3 else size)
def dis(buf, base):
    open('/tmp/_d.bin', 'wb').write(buf)
    out = subprocess.check_output([c.CROSS + 'objdump', '-D', '-b', 'binary', '-m', 'mips:5900', '-EL', '--adjust-vma=%#x' % base, '-M', 'no-aliases', '/tmp/_d.bin'], text=True)
    return [l.split('\t', 2)[-1] for l in out.splitlines() if re.match(r'^\s+[0-9a-f]+:\t', l)]
a = dis(text[off:off + size], 0); b = dis(c.ROM[addr - c.VRAM:addr - c.VRAM + osize], 0)
for i in range(max(len(a), len(b))):
    x = a[i] if i < len(a) else ''; y = b[i] if i < len(b) else ''
    r = relocs.get(off + i * 4)
    print('%4x %s %-40s %s' % (i * 4, ' ' if (x == y or r) else '*', x, y))
