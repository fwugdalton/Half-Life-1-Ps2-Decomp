# Testa variantes de um trecho de código.
# Uso: vtry.py src/dlls/x.cpp script_que_compila_e_compara.sh NomeDaFuncao variantes.txt
# variantes.txt: o trecho original, depois cada variante, separados por linhas '===='.
# Mostra quantas instruções diferem em cada variante e restaura o arquivo no fim.
import sys,subprocess
p,sh,fn,vf=sys.argv[1:5]
src=open(p).read()
parts=open(vf).read().split('====\n')
old=parts[0].rstrip('\n')
assert src.count(old)==1, 'old not unique'
for i,v in enumerate(parts[1:]):
    open(p,'w').write(src.replace(old,v.rstrip('\n')))
    out=subprocess.run(['sh',sh],capture_output=True,text=True).stdout
    m=[l for l in out.splitlines() if fn in l]
    print(i, m[0] if m else 'OK')
open(p,'w').write(src)
