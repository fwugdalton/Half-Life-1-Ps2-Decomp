"""Ajusta a saída do ee-gcc 2.95 para que o gas moderno gere o mesmo código que o ee-as original.

- move rd, rs        -> daddu rd, rs, $0   (o ee-as usa daddu; o gas moderno usaria or)
- .rdata             -> .section .rodata
- break 7            -> break 0, 7          (o ee-as põe o código no campo baixo)
- cvt.w.s fd, fs     -> .word (o gas não aceita essa instrução com -march=r5900)
- sqrt.s fd, fs      -> .word com o operando no campo ft (codificação do R5900)
- em modo reorder, o ee-as não move mflo/mfhi para o delay slot de um desvio;
  nesses casos o desvio é emitido em noreorder com um nop explícito.
- também não preenche o delay slot quando a penúltima instrução é sdl/sdr/swl/swr
  (visto em strcpy inline seguido de jr $31), nem com macros uld/usd/ulw/usw,
  nem quando a última instrução lê um registrador de FPU carregado pela penúltima.
- depois de mtc1 (inclusive o li.s expandido), o ee-as põe um nop se a instrução
  seguinte lê o mesmo par de registradores de FPU ($f2 e $f3 contam como o mesmo),
  mesmo com um rótulo no meio (aí o nop fica antes do rótulo); se a instrução depois
  do rótulo não lê o registrador, não há nop (o mtc1 vira .word para o gas não pôr um).
- cvt.w.s (.word) logo depois de mfc1 vai em noreorder: o gas poria um nop entre os dois.
- o ee-as também não preenche o delay slot de um desvio quando a instrução anterior
  precisou do nop do mtc1 (visto em conversão unsigned->float seguida de b).
- nem quando a penúltima é um cvt.w.s/sqrt.s (.word) cujo resultado a última lê.
- nem com um acesso volatile (entre "#.set volatile" e "#.set novolatile"; o ee-gcc
  original emitia a diretiva de verdade e o ee-as a respeitava).
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
    w = re.match(r'^\s+\.word\s+0x([0-9A-Fa-f]{8})\s*$', line)
    if w:
        # cvt.w.s e sqrt.s já convertidos em .word também leem registrador
        w = int(w.group(1), 16)
        if w & 0xFFE0003F == 0x46000024:
            return {(w >> 11) & 31}
        if w & 0xFFE0003F == 0x46000004:
            return {(w >> 16) & 31}
        return set()
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

def fp_load_use(load, use):
    """l.s/lwc1 seguido de instrução que lê o mesmo registrador de FPU."""
    m = re.match(r'^\s+(l\.s|lwc1)\s+\$f(\d+),', load)
    return bool(m) and (int(m.group(2)) & ~1) in set(r & ~1 for r in fp_reads(use))

def word_fp_use(word, use):
    """cvt.w.s/sqrt.s escritos como .word seguidos de instrução que lê o fd:
    o ee-as conhece a dependência e não move a instrução para o delay slot."""
    w = re.match(r'^\s+\.word\s+0x([0-9A-Fa-f]{8})', word)
    if not w:
        return False
    w = int(w.group(1), 16)
    if w & 0xFFE0003F not in (0x46000024, 0x46000004):
        return False
    return ((w >> 6) & 31) & ~1 in set(r & ~1 for r in fp_reads(use))

def raw_mtc1(line):
    """li.s/mtc1 escritos à mão, com o mtc1 como .word."""
    import struct
    m = re.match(r'^(\s+)li\.s\s+\$f(\d+),\s*(\S+)\s*$', line)
    if m:
        ind, f, val = m.group(1), int(m.group(2)), float(m.group(3))
        bits = struct.unpack('<I', struct.pack('<f', val))[0]
        res = ['%slui\t$1,0x%x' % (ind, bits >> 16)]
        if bits & 0xFFFF:
            res.append('%sori\t$1,$1,0x%x' % (ind, bits & 0xFFFF))
        res.append('%s.word\t0x%08X\t# mtc1 $1,$f%d' % (ind, 0x44810000 | (f << 11), f))
        return res
    m = re.match(r'^(\s+)mtc1\s+\$(\d+),\s*\$f(\d+)\s*$', line)
    if m:
        ind, r, f = m.group(1), int(m.group(2)), int(m.group(3))
        return ['%s.word\t0x%08X\t# mtc1 $%d,$f%d' % (ind, 0x44800000 | (r << 16) | (f << 11), r, f)]
    return [line]

pending_fpr = None
label_at = None
pending_idx = None
before = 0
after_nop = set()   # índices em out de instruções que receberam o nop do mtc1
volatile = False
vol_idx = set()     # índices em out de acessos volatile
for line in lines:
    s = line.strip()
    if s in ('#.set\tvolatile', '#.set volatile'):
        volatile = True
    elif s in ('#.set\tnovolatile', '#.set novolatile'):
        volatile = False
    if s == '.set\tnoreorder' or s == '.set noreorder':
        reorder = False
    elif s == '.set\treorder' or s == '.set reorder':
        reorder = True
    m = re.match(r'^(\s+)move\s+(\$\w+),\s*(\$\w+)\s*$', line)
    if m:
        line = '%sdaddu\t%s,%s,$0' % m.groups()
    m = re.match(r'^(\s+)la\s+(\$\w+),\s*([\w$.]+)\s*$', line)
    if m:
        # o gas moderno expande la com daddiu; o ee-as com lui/addiu
        ind, r, sym = m.groups()
        pending_fpr = None
        label_at = None
        prev_prev = prev_real
        prev_real = len(out)
        out.append('%slui\t%s,%%hi(%s)' % (ind, r, sym))
        line = '%saddiu\t%s,%s,%%lo(%s)' % (ind, r, r, sym)
    if re.match(r'^\s+\.rdata\s*$', line):
        line = '\t.section .rodata'
    m = re.match(r'^(\s+)break\s+(\d+)\s*$', line)
    if m:
        line = '%sbreak\t0,%s' % m.groups()
    md = re.match(r'^(\s+)li\.d\s+(\$\d+),\s*(\S+)\s*$', line)
    if md:
        # li.d num registrador inteiro: o ee-as carrega o double como dli (ori + dsll32)
        import struct
        ind, rd, val = md.groups()
        v = struct.unpack('<Q', struct.pack('<d', float(val)))[0]
        for sh in range(49):
            if v & ~(0xFFFF << sh) == 0:
                break
        else:
            # valor sem forma curta: o ee-as põe o double na .rodata e carrega com lui/ld via $at
            lit_n = globals().get('lit_n', 0) + 1
            globals()['lit_n'] = lit_n
            lab = '$LIT%d' % lit_n
            out.append('\t.pushsection .rodata')
            out.append('\t.align\t3')
            out.append('%s:' % lab)
            out.append('\t.dword\t0x%016X' % v)
            out.append('\t.popsection')
            out.append('\t.set\tnoat')
            out.append('%slui\t$1,%%hi(%s)' % (ind, lab))
            out.append('%sld\t%s,%%lo(%s)($1)' % (ind, rd, lab))
            line = '\t.set\tat'
            sh = None
        if sh is None:
            pass
        else:
          out.append('%sori\t%s,$0,0x%X' % (ind, rd, v >> sh))
          if sh >= 32:
            line = '%sdsll32\t%s,%s,%d' % (ind, rd, rd, sh - 32)
          elif sh:
            line = '%sdsll\t%s,%s,%d' % (ind, rd, rd, sh)
          else:
            line = out.pop()
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
    m3 = re.match(r'^(\s+)(uld|usd)\s+(\$\w+),\s*%lo\(([^)]+)\)\((\$\w+)\)\s*$', line)
    if m3:
        # o ee-as expande uld/usd com %lo() direto em ldl/ldr (sdl/sdr), sem passar por $at
        ind, op, rt, sym, base = m3.groups()
        l, r = ('ldl', 'ldr') if op == 'uld' else ('sdl', 'sdr')
        out.append('%s%s\t%s,%%lo(%s+7)(%s)' % (ind, l, rt, sym, base))
        line = '%s%s\t%s,%%lo(%s)(%s)\t# %s' % (ind, r, rt, sym, base, op)
    if reorder and BRANCH.match(line) and prev_real is not None and (
            re.match(r'^\s+(mf(lo|hi)|u(ld|sd|lw|sw|lh|lhu|sh))\s', out[prev_real]) or
            re.search(r'# u(ld|sd)$', out[prev_real]) or
            (prev_prev is not None and re.match(r'^\s+s[dw][lr]\s', out[prev_prev])) or
            (prev_prev is not None and fp_load_use(out[prev_prev], out[prev_real])) or
            (prev_prev is not None and word_fp_use(out[prev_prev], out[prev_real])) or
            prev_real in after_nop or prev_real in vol_idx):
        out.append('\t.set\tnoreorder')
        out.append(line)
        out.append('\tnop')
        out.append('\t.set\treorder')
        prev_real = prev_prev = None
        continue
    if s and not s.startswith(('.', '#', '$L')) and not s.endswith(':'):
        if reorder and pending_fpr is not None and any((r & ~1) == pending_fpr for r in fp_reads(line)):
            if label_at is not None:
                # o gas moderno poria o nop do mtc1 antes do .p2align (o ee-as põe depois):
                # o mtc1 vira .word para o gas não ver o atraso, e o nop vai junto do rótulo
                n = len(out)
                out[pending_idx:pending_idx + 1] = raw_mtc1(out[pending_idx])
                out.insert(label_at + len(out) - n, '\tnop')
            else:
                out.append('\tnop')
            after_nop.add(len(out))
        elif reorder and pending_fpr is not None and label_at is not None:
            # mtc1 seguido de rótulo cuja instrução não lê o registrador: o ee-as não põe nop,
            # mas o gas moderno poria (não sabe o que vem depois do rótulo)
            # (um li.s/mtc1 logo antes também vira .word, senão o gas põe o nop antes do .word)
            idxs = [pending_idx]
            while idxs[0] > 0 and FPWRITE.match(out[idxs[0] - 1]):
                idxs.insert(0, idxs[0] - 1)
            for i in reversed(idxs):
                out[i:i + 1] = raw_mtc1(out[i])
        pending_fpr = None
        label_at = None
        m = FPWRITE.match(line)
        if m and reorder:
            r = re.findall(r'\$f(\d+)', line)
            if r:
                pending_fpr = int(r[0]) & ~1
                pending_idx = len(out)
        prev_prev = prev_real
        prev_real = len(out)
        if volatile:
            vol_idx.add(len(out))
    elif s.endswith(':'):
        if reorder and pending_fpr is not None and label_at is None:
            # se a instrução depois do rótulo ler o registrador, o nop vai antes do rótulo
            label_at = len(out)
            before = len(out)
        else:
            pending_fpr = None
        prev_real = prev_prev = None
    out.append(line)
open(sys.argv[2], 'w').write('\n'.join(out))
