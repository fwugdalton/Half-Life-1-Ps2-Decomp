"""Ajusta a saída do ee-gcc 2.95 para que o gas moderno gere o mesmo código que o ee-as original.

- move rd, rs        -> daddu rd, rs, $0   (o ee-as usa daddu; o gas moderno usaria or)
- .rdata             -> .section .rodata
- break 7            -> break 0, 7          (o ee-as põe o código no campo baixo)
- cvt.w.s fd, fs     -> .word (o gas não aceita essa instrução com -march=r5900)
- em modo reorder, o ee-as não move mflo/mfhi para o delay slot de um desvio;
  nesses casos o desvio é emitido em noreorder com um nop explícito.
- também não preenche o delay slot quando a penúltima instrução é sdl/sdr/swl/swr
  (visto em strcpy inline seguido de jr $31).
"""
import sys, re

BRANCH = re.compile(r'^\s+(jal|jalr|j|b|beq|bne|beqz|bnez|blez|bgtz|bltz|bgez|bc1t|bc1f)\s')
lines = open(sys.argv[1]).read().split('\n')
out = []
reorder = True
prev_real = None   # índice em out da última instrução real (modo reorder)
prev_prev = None   # índice da penúltima
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
    if m:
        line = '%s.word\t0x%08X' % (m.group(1), 0x46000024 | (int(m.group(3)) << 11) | (int(m.group(2)) << 6))
    if reorder and BRANCH.match(line) and prev_real is not None and (
            re.match(r'^\s+mf(lo|hi)\s', out[prev_real]) or
            (prev_prev is not None and re.match(r'^\s+s[dw][lr]\s', out[prev_prev]))):
        out.append('\t.set\tnoreorder')
        out.append(line)
        out.append('\tnop')
        out.append('\t.set\treorder')
        prev_real = prev_prev = None
        continue
    if s and not s.startswith(('.', '#', '$L')) and not s.endswith(':'):
        prev_prev = prev_real
        prev_real = len(out)
    elif s.endswith(':'):
        prev_real = prev_prev = None
    out.append(line)
open(sys.argv[2], 'w').write('\n'.join(out))
