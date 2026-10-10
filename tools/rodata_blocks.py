"""Gera os blocos __asm__ com a .rodata que o splat não migrou para as funções de um arquivo C.

Uso: rodata_blocks.py src/arquivo.c rodata_original.s
(a disassembly original de sv_main fica em tools/rodata_cache/engine_sv_main.rodata.s)
Rodar depois de `make split` sempre que uma função passar para C ou voltar para asm.

Quando um segmento `.rodata` de um arquivo C ainda tem funções em INCLUDE_ASM, o splat
só migra para o .s de cada função os dados usados por uma única função. O resto
(strings compartilhadas, strings usadas só pela .data) some e o link quebra.

Este script pega a disassembly original do trecho de rodata (gerada com o segmento
como `rodata`, sem ponto, por um `make split` temporário) e insere no .c blocos
__asm__ com os rótulos não migrados, na ordem dos endereços: cada bloco vai antes da
primeira função (em C ou INCLUDE_ASM) cuja rodata migrada vem depois dele.

Rótulos compartilhados que só funções em C usam são emitidos pelo próprio C e ficam de
fora (a menos que o C os declare `extern`, aí vão no bloco). Os que o C emite mas alguma função em asm ainda usa ganham símbolo absoluto em
linker_script_extra.ld (entre marcadores com o nome do arquivo).
Blocos e símbolos antigos são removidos antes, então dá para rodar de novo a cada mudança.
"""
import sys, re, os, glob

src, orig = sys.argv[1], sys.argv[2]
base = os.path.splitext(os.path.relpath(src, 'src'))[0]
asmdir = 'asm/nonmatchings/' + base
BEGIN, END = '/* rodata não migrada: início */\n', '/* rodata não migrada: fim */\n'
LD = 'linker_script_extra.ld'

# blocos da rodata original: [nome, endereço, linhas]
blocks = []
cur = None
for line in open(orig):
    m = re.match(r'nonmatching (\S+)', line)
    if m:
        cur = [m.group(1), None, []]
        blocks.append(cur)
        continue
    if cur is None:
        continue
    m = re.search(r'/\* [0-9A-F]+ ([0-9A-F]{8})', line)
    if m and cur[1] is None:
        cur[1] = int(m.group(1), 16)
    cur[2].append(line)
addr = {b[0]: b[1] for b in blocks}

# rótulos migrados e referências de cada função. O splat só gera .s para funções em
# INCLUDE_ASM, então guardamos um cache (tools/rodata_cache/<arquivo>.json) com o que já foi visto.
import json
cache_path = os.path.join('tools', 'rodata_cache', base.replace('/', '_') + '.json')
cache = json.load(open(cache_path)) if os.path.exists(cache_path) else {}
# funções renomeadas em symbol_addrs.txt depois de entrarem no cache
names = {}
for line in open('symbol_addrs.txt'):
    m = re.match(r'\s*(\w+)\s*=\s*0x([0-9A-Fa-f]+);', line)
    if m: names['func_%08X' % int(m.group(2), 16)] = m.group(1)
cache = {names.get(k, k): v for k, v in cache.items()}
for f in glob.glob(asmdir + '/*.s'):
    t = open(f).read()
    fn = os.path.basename(f)[:-2]
    cache[fn] = {'migrated': re.findall(r'^dlabel (\S+)', t, re.M),
                 'refs': sorted(set(re.findall(r'%lo\((\w+)\)', t)))}
os.makedirs(os.path.dirname(cache_path), exist_ok=True)
json.dump(cache, open(cache_path, 'w'), indent=0, sort_keys=True)
migrated = {fn: v['migrated'] for fn, v in cache.items()}
refs = {fn: set(v['refs']) for fn, v in cache.items()}
mig_all = {l for v in migrated.values() for l in v}

s = open(src).read()
s = re.sub(r'/\* rodata não migrada: início \*/\n.*?/\* rodata não migrada: fim \*/\n', '', s, flags=re.S)

# funções na ordem do arquivo: (posição de inserção, nome, está em C?)
funcs = []
for m in re.finditer(r'^INCLUDE_ASM\("[^"]*", (\w+)\);\n|^[A-Za-z_][^\n;=()]*?\b(\w+)\([^;{]*\)\n\{', s, re.M):
    start = m.start()
    if m.group(1):
        fn, inc = m.group(1), True
        k = s.rfind('#ifdef NON_MATCHING\n', 0, start)
        if s[:start].endswith('#else\n') and k >= 0 and '#endif' not in s[k:start]:
            # função envolvida: a definição em C já foi registrada; troca para asm
            funcs = [f for f in funcs if f[1] != fn]
            start = k
    else:
        fn, inc = m.group(2), False
        k = s.rfind('#ifdef NON_MATCHING\n', 0, start)
        if k >= 0 and s.count('\n', k, start) == 1:
            start = k
    funcs.append((start, fn, not inc))
in_c = {fn for _, fn, c in funcs if c}
in_asm = {fn for _, fn, c in funcs if not c}

def users(label):
    return {fn for fn, r in refs.items() if label in r}

rest, absolute = [], []
for b in blocks:
    if b[0] in mig_all:
        continue
    u = users(b[0])
    if u & in_c and not re.search(r'^extern[^;\n]*\b%s\b' % b[0], s, re.M):
        # (rótulo declarado extern no C não é emitido pelo C: vai no bloco)
        # o C já emite; se alguma função em asm ainda usa, precisa de símbolo absoluto
        if u - in_c:
            absolute.append(b)
    else:
        rest.append(b)

def body(bl):
    # alinha o início do bloco conforme o endereço do primeiro rótulo (o .align original
    # fica no fim do rótulo anterior, que pode ter sido emitido pelo C)
    a = bl[0][1]
    out = ['.align %d\n' % (3 if a % 8 == 0 else 2 if a % 4 == 0 else 1 if a % 2 == 0 else 0)]
    for name, a, lines in bl:
        for l in lines:
            if l.startswith('dlabel'):
                out.append('.globl %s\n%s:\n' % (name, name))
            elif l.startswith('enddlabel') or not l.strip() or re.match(r'\s*/\*.*\*/\s*$', l):
                continue
            else:
                out.append(re.sub(r'/\*.*?\*/\s*', '', l).strip() + '\n')
    txt = ''.join(out).replace('\\', '\\\\').replace('"', '\\"')
    return BEGIN + '__asm__(".section .rodata\\n"\n' + ''.join('        "%s\\n"\n' % l for l in txt.splitlines()) + '        ".section .text\\n");\n' + END

def first_addr(fn):
    a = [addr[l] for l in migrated.get(fn, []) if l in addr]
    return min(a) if a else None

out = []
for start, fn, c in funcs:
    fa = first_addr(fn)
    if fa is None:
        continue
    take = [b for b in rest if b[1] < fa]
    if take:
        out.append((start, body(take)))
        rest = [b for b in rest if b[1] >= fa]
for start, txt in reversed(out):
    s = s[:start] + txt + s[start:]
if rest:
    s = s.rstrip('\n') + '\n\n' + body(rest)
open(src, 'w').write(s)

# símbolos absolutos
ld = open(LD).read()
mb, me = '/* %s: início */\n' % base, '/* %s: fim */\n' % base
ld = re.sub(re.escape(mb) + r'.*?' + re.escape(me), '', ld, flags=re.S)
real = absolute
if real:
    ld = ld.rstrip('\n') + '\n' + mb + ''.join('%s = 0x%08X;\n' % (b[0], b[1]) for b in real) + me
open(LD, 'w').write(ld)
print('blocos: %d, símbolos absolutos: %d' % (len(out) + (1 if rest else 0), len(real)))
