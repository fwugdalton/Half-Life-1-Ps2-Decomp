# unwrap2.py arquivo Func : tira o #ifdef NON_MATCHING (com ou sem comentário) de uma função e o comentário /* NON_MATCHING: */ da linha anterior
import sys,re
p,fn=sys.argv[1:3]
s=open(p).read()
m=re.search(r'#else\n((?:[^\n]*\n)??)INCLUDE_ASM\("[^"]+", *%s\);\n#endif\n'%re.escape(fn),s)
if not m: print('nao achei INCLUDE_ASM',fn); sys.exit(1)
i=s.rfind('#ifdef NON_MATCHING',0,m.start())
body=s[s.index('\n',i)+1:m.start()]
pre=s[:i]
# comentário NON_MATCHING na linha anterior
k=pre.rstrip('\n').rfind('\n')+1
if pre[k:].startswith('/* NON_MATCHING') : pre=pre[:k]
s=pre+body+s[m.end():]
open(p,'w').write(s); print('ok',fn)
