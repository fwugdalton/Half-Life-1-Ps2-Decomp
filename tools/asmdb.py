"""Banco de dados simples a partir da saída do splat: funções, referências e strings."""
import re, os, pickle, sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CACHE = os.path.join(ROOT, 'build', 'asmdb.pkl')

re_glabel = re.compile(r'^glabel (\S+)')
re_ins = re.compile(r'^\s+/\* \w+ ([0-9A-F]{8}) ([0-9A-F]{8}) \*/\s+(\S+)\s*(.*)$')
re_sym = re.compile(r'%(?:hi|lo|gp_rel)\(([A-Za-z_][\w.$]*)\)')
re_dlabel = re.compile(r'^dlabel (\S+)')
re_data = re.compile(r'^\s+/\* \w+ ([0-9A-F]{8})(?: [0-9A-F]+)? \*/\s+\.(\w+)\s+(.*)$')

def parse():
    funcs = {}   # name -> dict(addr, size, refs:set, calls:set)
    cur = None
    with open(os.path.join(ROOT, 'asm/cod/000000.s')) as f:
        for line in f:
            m = re_glabel.match(line)
            if m:
                cur = {'name': m.group(1), 'addr': None, 'end': None, 'refs': set(), 'calls': set()}
                funcs[cur['name']] = cur
                continue
            if cur is None:
                continue
            m = re_ins.match(line)
            if m:
                a = int(m.group(1), 16)
                if cur['addr'] is None: cur['addr'] = a
                cur['end'] = a + 4
                op, args = m.group(3), m.group(4)
                for s in re_sym.findall(args): cur['refs'].add(s)
                if op == 'jal': cur['calls'].add(args.strip())
    data = {}    # name -> dict(addr, kind, items)
    for fn in ('asm/data/cod/236C80.rodata.s', 'asm/data/cod/201B00.data.s', 'asm/data/cod/298F80.sdata.s'):
        cur = None
        with open(os.path.join(ROOT, fn)) as f:
            for line in f:
                m = re_dlabel.match(line)
                if m:
                    cur = {'name': m.group(1), 'addr': None, 'items': [], 'file': fn}
                    data[cur['name']] = cur
                    continue
                if cur is None: continue
                m = re_data.match(line)
                if m:
                    if cur['addr'] is None: cur['addr'] = int(m.group(1), 16)
                    cur['items'].append((int(m.group(1), 16), m.group(2), m.group(3)))
    for d in data.values():
        if d['items'] and d['items'][0][1] == 'asciz':
            d['str'] = eval(d['items'][0][2]) if d['items'][0][2].startswith('"') else None
    return funcs, data

def load():
    if os.path.exists(CACHE):
        return pickle.load(open(CACHE, 'rb'))
    db = parse()
    os.makedirs(os.path.dirname(CACHE), exist_ok=True)
    pickle.dump(db, open(CACHE, 'wb'))
    return db

if __name__ == '__main__':
    funcs, data = load()
    print(len(funcs), 'funcs', len(data), 'data syms', sum(1 for d in data.values() if d.get('str')), 'strings')
