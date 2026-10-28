# Envolve uma função do .cpp em #ifdef NON_MATCHING ... #else INCLUDE_ASM(...) #endif.
# Uso: wrapnm.py src/dlls/x.cpp "texto exato do início da função" NomeMangled asm/nonmatchings/dlls/x [código extra para o #else]
import sys,re
p,hdr,mangled,asmdir=sys.argv[1:5]
dummy=sys.argv[5] if len(sys.argv)>5 else ''
s=open(p).read()
i=s.index(hdr)
# start of preceding comment block? just put #ifdef right before header line
j=s.index('\n}\n',i)+3
body=s[i:j]
rep='#ifdef NON_MATCHING\n'+body+'#else\n'+(dummy+'\n' if dummy else '')+'INCLUDE_ASM("%s", %s);\n#endif\n'%(asmdir,mangled)
s=s[:i]+rep+s[j:]
open(p,'w').write(s)
