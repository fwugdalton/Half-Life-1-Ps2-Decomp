"""Prepara um diretório para o decomp-permuter a partir de uma função NON_MATCHING.

Uso: permuter_setup.py src/arquivo.c Funcao [dir_saida]
Gera base.c (pré-processado, com -DNON_MATCHING), target.s/target.o (asm original),
compile.sh e settings.toml. Depois: python3 decomp-permuter/permuter.py <dir> -j8
"""
import sys, os, subprocess, re
ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
src, fn = sys.argv[1], sys.argv[2]
out = os.path.abspath(sys.argv[3] if len(sys.argv) > 3 else os.path.join(ROOT, 'nonmatchings', fn))
os.makedirs(out, exist_ok=True)
wibo = os.path.join(ROOT, 'tools/compilers/wibo')
gcc = os.path.join(ROOT, 'tools/compilers/ee-gcc2.95.2-274/bin/ee-gcc.exe')
# pré-processa com o cpp do próprio ee-gcc
pre = subprocess.check_output([wibo, gcc, '-E', '-P', '-DNON_MATCHING', '-DPERMUTER', '-I' + os.path.join(ROOT, 'include'),
                               '-I' + os.path.join(ROOT, 'src'), os.path.join(ROOT, src)], text=True, cwd=ROOT)
open(os.path.join(out, 'base.c'), 'w').write(pre)
asm = [p for p in subprocess.check_output(['find', 'asm/nonmatchings', '-name', fn + '.s'], cwd=ROOT, text=True).split()][0]
open(os.path.join(out, 'target.s'), 'w').write(
    '.include "%s/include/macro.inc"\n.set noat\n.set noreorder\n.section .text\n' % ROOT + open(os.path.join(ROOT, asm)).read())
subprocess.check_call(['mips-linux-gnu-as', '-EL', '-march=r5900', '-mabi=eabi', '-G0', '-o', os.path.join(out, 'target.o'), os.path.join(out, 'target.s')])
# flags extras do arquivo no Makefile (ex.: -ffast-math)
obj = '$(BUILD_DIR)/' + os.path.splitext(src)[0] + '.o'
extra = ' '.join(m.group(1).strip() for m in re.finditer(r'^%s: CFLAGS \+= (.*)$' % re.escape(obj), open(os.path.join(ROOT, 'Makefile')).read(), re.M))
open(os.path.join(out, 'compile.sh'), 'w').write('''#!/bin/sh
# uso: compile.sh entrada.c -o saida.o
set -e
T=$(mktemp -d)
"%s" "%s" -O2 -G0 %s -S -o "$T/a.s" "$1"
python3 "%s/tools/fixasm.py" "$T/a.s" "$T/b.s"
mips-linux-gnu-as -EL -march=r5900 -mabi=eabi -G0 -o "$3" "$T/b.s"
rm -rf "$T"
''' % (wibo, gcc, extra, ROOT))
os.chmod(os.path.join(out, 'compile.sh'), 0o755)
open(os.path.join(out, 'settings.toml'), 'w').write('func_name = "%s"\ncompiler_type = "gcc"\n' % fn)
print(out)
