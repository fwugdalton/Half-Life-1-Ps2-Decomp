"""Troca um bloco '#ifdef NON_MATCHING <func> #else INCLUDE_ASM(...) #endif' pela função dada (ou pela própria do bloco).

Uso: unwrap.py src/arquivo.c Funcao [variante.c]
"""
import sys, re
src, fn = sys.argv[1], sys.argv[2]
s = open(src).read()
m = re.search(r'#ifdef NON_MATCHING\n((?:(?!#ifdef NON_MATCHING\n).)*?)#else\nINCLUDE_ASM\("[^"]*", %s\);\n#endif\n' % re.escape(fn), s, re.S)
body = open(sys.argv[3]).read() if len(sys.argv) > 3 else m.group(1)
open(src, 'w').write(s[:m.start()] + body + s[m.end():])
