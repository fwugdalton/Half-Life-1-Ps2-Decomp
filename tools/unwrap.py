"""Remove o #ifdef NON_MATCHING / #else INCLUDE_ASM / #endif em volta de uma função que passou a casar.

Uso: unwrap.py arquivo.c Func [Func...]
"""
import re, sys
path = sys.argv[1]
src = open(path).read()
for name in sys.argv[2:]:
    pat = re.compile(r'#ifdef NON_MATCHING\n((?:(?!#ifdef NON_MATCHING).)*?)#else\nINCLUDE_ASM\("[^"]*", ' + re.escape(name) + r'\);\n#endif\n', re.S)
    src, n = pat.subn(lambda m: m.group(1), src)
    print(name, 'ok' if n == 1 else 'NÃO ENCONTRADO (%d)' % n)
open(path, 'w').write(src)
