"""Ajusta a saída do ee-gcc 2.95 para que o gas moderno gere o mesmo código que o ee-as original.

- move rd, rs        -> daddu rd, rs, $0   (o ee-as usa daddu; o gas moderno usaria or)
- .rdata             -> .section .rodata
- break 7            -> break 0, 7          (o ee-as põe o código no campo baixo)
- em modo reorder, o ee-as não move mflo/mfhi para o delay slot de um desvio;
  nesses casos o desvio é emitido em noreorder com um nop explícito.
"""
import sys, re

BRANCH = re.compile(r'^\s+(jal|jalr|j|b|beq|bne|beqz|bnez|blez|bgtz|bltz|bgez|bc1t|bc1f)\s')
lines = open(sys.argv[1]).read().split('\n')
out = []
reorder = True
prev_real = None   # índice em out da última instrução real (modo reorder)
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
    if reorder and BRANCH.match(line) and prev_real is not None and re.match(r'^\s+mf(lo|hi)\s', out[prev_real]):
        out.append('\t.set\tnoreorder')
        out.append(line)
        out.append('\tnop')
        out.append('\t.set\treorder')
        prev_real = None
        continue
    if s and not s.startswith(('.', '#', '$L')) and not s.endswith(':'):
        prev_real = len(out)
    elif s.endswith(':'):
        prev_real = None
    out.append(line)
open(sys.argv[2], 'w').write('\n'.join(out))
