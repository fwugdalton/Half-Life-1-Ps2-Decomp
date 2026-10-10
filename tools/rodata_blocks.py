"""Gera os blocos __asm__ com a .rodata que o splat não migrou para as funções de um arquivo C.

Uso: rodata_blocks.py src/arquivo.c rodata_original.s

Quando um segmento `.rodata` de um arquivo C ainda tem funções em INCLUDE_ASM, o splat
só migra para o .s de cada função os dados usados por uma única função. O resto
(strings compartilhadas, strings usadas só pela .data) some e o link quebra.
Este script pega a disassembly original do trecho de rodata (gerada com o segmento
como `rodata`, sem ponto) e insere no .c, antes de cada INCLUDE_ASM, um bloco
__asm__ com os rótulos não migrados que vêm antes da rodata migrada daquela função,
mantendo a ordem dos endereços. Rótulos depois da última função vão no fim do arquivo.
Blocos antigos (entre os marcadores) são removidos antes, então dá para rodar de novo.
Funções já em C também contam: as strings delas aparecem como rótulos `D_` que não são
mais referenciados, então o script as ignora se o rótulo estiver em `skip` (linker_script_extra).
"""
import sys, re, os, glob

src, orig = sys.argv[1], sys.argv[2]
asmdir = 'asm/nonmatchings/' + os.path.splitext(os.path.relpath(src, 'src'))[0]
BEGIN, END = '/* rodata não migrada: início */\n', '/* rodata não migrada: fim */\n'

# blocos da rodata original: nome -> (endereço, linhas)
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

# rótulos migrados por função
migrated = {}
for f in glob.glob(asmdir + '/*.s'):
    labs = re.findall(r'^dlabel (\S+)', open(f).read(), re.M)
    migrated[os.path.basename(f)[:-2]] = labs
mig_all = {l for v in migrated.values() for l in v}
addr = {b[0]: b[1] for b in blocks}

s = open(src).read()
s = re.sub(r'/\* rodata não migrada: início \*/\n.*?/\* rodata não migrada: fim \*/\n', '', s, flags=re.S)

# rótulos que o C já emite (strings de funções decompiladas): não emitir de novo
skip = set(sys.argv[3:])
rest = [b for b in blocks if b[0] not in mig_all and b[0] not in skip]

def body(bl):
    out = []
    for name, a, lines in bl:
        for l in lines:
            if l.startswith('dlabel'):
                out.append('.globl %s\n%s:\n' % (name, name))
            elif l.startswith('enddlabel') or l.strip() == '' or l.lstrip().startswith('/*') and l.rstrip().endswith('*/') and not re.search(r'\*/\s*\.', l):
                continue
            else:
                out.append(re.sub(r'/\*.*?\*/\s*', '', l).strip() + '\n')
    txt = ''.join(out)
    txt = txt.replace('\\', '\\\\').replace('"', '\\"')
    return BEGIN + '__asm__(".section .rodata\\n"\n' + ''.join('        "%s\\n"\n' % l for l in txt.splitlines()) + '        ".section .text\\n");\n' + END

def first_addr(fn):
    a = [addr[l] for l in migrated.get(fn, []) if l in addr]
    return min(a) if a else None

out = []
pos = 0
for m in re.finditer(r'^(?:#ifdef NON_MATCHING\n)?[^\n]*?\b(\w+)\([^;{]*\)\n\{|^INCLUDE_ASM\("[^"]*", (\w+)\);\n', s, re.M):
    fn = m.group(1) or m.group(2)
    if m.group(2) is None:
        continue
    fa = first_addr(fn)
    if fa is None:
        continue
    take = [b for b in rest if b[1] < fa]
    if take:
        # o bloco entra antes do #ifdef NON_MATCHING da função, se houver
        start = m.start()
        k = s.rfind('#ifdef NON_MATCHING\n', 0, start)
        if s[:start].endswith('#else\n') and k >= 0 and '#endif' not in s[k:start]:
            start = k
        out.append((start, body(take)))
        rest = [b for b in rest if b[1] >= fa]
for start, txt in reversed(out):
    s = s[:start] + txt + s[start:]
if rest:
    s = s.rstrip('\n') + '\n\n' + body(rest)
open(src, 'w').write(s)
print('blocos:', len(out) + (1 if rest else 0))
