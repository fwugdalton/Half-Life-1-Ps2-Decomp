"""Envolve funções em '#ifdef NON_MATCHING <func> #else INCLUDE_ASM(...) #endif' (o inverso de unwrap.py).

Uso: wrap.py src/arquivo.c Funcao [Funcao ...]
Depois rode `make split` (com o venv) para o splat gerar asm/nonmatchings/.../Funcao.s.
"""
import sys, re, os
src = sys.argv[1]
s = open(src).read()
asmdir = 'asm/nonmatchings/' + os.path.splitext(os.path.relpath(src, 'src'))[0]
for fn in sys.argv[2:]:
    # [^;{]* impede casar um protótipo (linha com ';') antes da definição
    m = re.search(r'^[^\n;]*\b%s\([^;{]*?\)\n\{.*?^\}\n' % re.escape(fn), s, re.S | re.M)
    if not m:
        sys.exit('não achei ' + fn)
    s = s[:m.start()] + '#ifdef NON_MATCHING\n' + m.group(0) + '#else\nINCLUDE_ASM("%s", %s);\n#endif\n' % (asmdir, fn) + s[m.end():]
open(src, 'w').write(s)
