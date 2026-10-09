"""Ajusta a saída do ee-gcc 2.95 para que o gas moderno gere o mesmo código que o ee-as original.

- move rd, rs        -> daddu rd, rs, $0   (o ee-as usa daddu; o gas moderno usaria or)
- .rdata             -> .section .rodata
- break 7            -> break 0, 7          (o ee-as põe o código no campo baixo)
- cvt.w.s fd, fs     -> .word (o gas não aceita essa instrução com -march=r5900)
- sqrt.s fd, fs      -> .word com o operando no campo ft (codificação do R5900)
- em modo reorder, o ee-as não move mflo/mfhi para o delay slot de um desvio;
  nesses casos o desvio é emitido em noreorder com um nop explícito.
- também não preenche o delay slot quando a penúltima instrução é sdl/sdr/swl/swr
  (visto em strcpy inline seguido de jr $31), nem com macros uld/usd/ulw/usw.
- depois de mtc1 (inclusive o li.s expandido), o ee-as põe um nop se a instrução
  seguinte lê o mesmo par de registradores de FPU ($f2 e $f3 contam como o mesmo).
- cvt.w.s (.word) logo depois de mfc1 vai em noreorder: o gas poria um nop entre os dois.
"""
import sys, re

BRANCH = re.compile(r'^\s+(jal|jalr|j|b|beq|bne|beqz|bnez|blez|bgtz|bltz|bgez|bc1t|bc1f)\s')
lines = open(sys.argv[1]).read().split('\n')
out = []
reorder = True
prev_real = None   # índice em out da última instrução real (modo reorder)
prev_prev = None   # índice da penúltima
FPWRITE = re.compile(r'^\s+(li\.s|mtc1)\s')

def fp_reads(line):
    """Registradores de FPU lidos pela instrução (só os pares importam)."""
    m = re.match(r'^\s+([\w.]+)\s+(.*)$', line)
    if not m:
        return set()
    op, args = m.group(1), m.group(2)
    regs = [int(r) for r in re.findall(r'\$f(\d+)', args)]
    if not regs:
        return set()
    if op in ('s.s', 'swc1', 's.d', 'sdc1', 'mfc1', 'cfc1') or op.startswith('c.'):
        return set(regs)
    if op in ('l.s', 'lwc1', 'l.d', 'ldc1', 'li.s', 'li.d', 'mtc1', 'ctc1'):
        return set()
    if op.endswith(('.s', '.d', '.w')) or op.startswith(('madd', 'msub')):
        return set(regs[1:])
    return set()

pending_fpr = None
for line in lines:
    s = line.strip()
    if s == '.set\tnoreorder' or s == '.set noreorder':
        reorder = False
    elif s == '.set\treorder' or s == '.set reorder':
        reorder = True
    m = re.match(r'^(\s+)move\s+(\$\w+),\s*(\$\w+)\s*$', line)
    if m:
        line = '%sdaddu\t%s,%s,$0' % m.groups()
    if re.match(r'^\s+\.rdata\s*$', line):
        line = '\t.section .rodata'
    m = re.match(r'^(\s+)break\s+(\d+)\s*$', line)
    if m:
        line = '%sbreak\t0,%s' % m.groups()
    m = re.match(r'^(\s+)cvt\.w\.s\s+\$f(\d+),\s*\$f(\d+)\s*$', line)
    m2 = re.match(r'^(\s+)sqrt\.s\s+\$f(\d+),\s*\$f(\d+)\s*$', line)
    if m2:
        # no R5900 o operando do sqrt.s vai no campo ft, não no fs
        line = '%s.word\t0x%08X' % (m2.group(1), 0x46000004 | (int(m2.group(3)) << 16) | (int(m2.group(2)) << 6))
    if m:
        line = '%s.word\t0x%08X' % (m.group(1), 0x46000024 | (int(m.group(3)) << 11) | (int(m.group(2)) << 6))
        if reorder and prev_real == len(out) - 1 and re.match(r'^\s+mfc1\s', out[prev_real]):
            # o gas põe um nop entre mfc1 e um .word que ele não conhece; o ee-as não
            out.insert(prev_real, '\t.set\tnoreorder')
            out.append(line)
            out.append('\t.set\treorder')
            continue
    if reorder and BRANCH.match(line) and prev_real is not None and (
            re.match(r'^\s+(mf(lo|hi)|u(ld|sd|lw|sw|lh|lhu|sh))\s', out[prev_real]) or
            (prev_prev is not None and re.match(r'^\s+s[dw][lr]\s', out[prev_prev]))):
        out.append('\t.set\tnoreorder')
        out.append(line)
        out.append('\tnop')
        out.append('\t.set\treorder')
        prev_real = prev_prev = None
        continue
    if s and not s.startswith(('.', '#', '$L')) and not s.endswith(':'):
        if reorder and pending_fpr is not None and any((r & ~1) == pending_fpr for r in fp_reads(line)):
            out.append('\tnop')
        pending_fpr = None
        m = FPWRITE.match(line)
        if m and reorder:
            r = re.findall(r'\$f(\d+)', line)
            if r:
                pending_fpr = int(r[0]) & ~1
        prev_prev = prev_real
        prev_real = len(out)
    elif s.endswith(':'):
        prev_real = prev_prev = None
        pending_fpr = None
    out.append(line)
open(sys.argv[2], 'w').write('\n'.join(out))
