# unwrap.py src Func : remove o #ifdef NON_MATCHING ... #else INCLUDE_ASM #endif de uma função
import sys, re
src, fn = sys.argv[1], sys.argv[2]
s = open(src).read()
pat = re.compile(r'#ifdef NON_MATCHING\n((?:(?!#ifdef NON_MATCHING).)*?\b%s\([^;]*?\n\{.*?\n\}\n)#else\nINCLUDE_ASM\("[^"]+", %s\);\n#endif\n' % (fn, fn), re.S)
s2, n = pat.subn(lambda m: m.group(1), s)
print(n)
open(src, 'w').write(s2)
