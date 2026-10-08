"""Lista funções num intervalo de endereços com tamanho e strings referenciadas."""
import sys; sys.path.insert(0, __import__('os').path.dirname(__file__)); import asmdb
funcs, data = asmdb.load()
lo, hi = int(sys.argv[1], 16), int(sys.argv[2], 16)
for f in sorted(funcs.values(), key=lambda f: f['addr']):
    if lo <= f['addr'] < hi:
        strs = [data[r]['str'] for r in sorted(f['refs']) if data.get(r, {}).get('str')]
        print('%08x %5x %-28s %s' % (f['addr'], f['end'] - f['addr'], f['name'], ' | '.join(repr(s)[:50] for s in strs[:4])))
