# compara a rom construída com os bytes dos comentários do asm (asm/nonmatchings/<dir>)
import sys, re, glob
rom = open('build/SLUS_200.66.rom', 'rb').read()
for f in sorted(glob.glob(sys.argv[1] + '/*.s')):
    bad = []
    for l in open(f):
        m = re.match(r'\s*/\* ([0-9A-F]+) ([0-9A-F]{8}) ([0-9A-F]{8}) \*/', l)
        if m:
            off = int(m.group(1), 16)
            if rom[off:off + 4] != bytes.fromhex(m.group(3)):
                bad.append(m.group(2))
    if bad:
        print(f.split('/')[-1], len(bad), bad[:4])
