# storeorder.py FUNC 'assinatura' : troca a ordem dos dois stores de cada bloco separado por 'p += 2;' e fica com a que mais reduz a diferença (busca gulosa). Ajustar p= no topo.
import subprocess,sys,re
fn=sys.argv[1]; sig=sys.argv[2]; p=sys.argv[3] if len(sys.argv)>3 else 'src/gbx/GsEnv.c'
src=open(p).read()
start=src.index(sig); body_start=src.index('    p[',start); end=src.index('\n}\n',start)
pre=src[:body_start]; body=src[body_start:end]; post=src[end:]
chunks=body.split('    p += 2;\n')
def swap(c):
    l=c.rstrip('\n').split('\n'); return '\n'.join([l[1],l[0]])+'\n'
def score(cs):
    open(p,'w').write(pre+'    p += 2;\n'.join(cs)+post)
    out=subprocess.run(['bash','tools/handoff/scan.sh',p],capture_output=True,text=True).stdout
    l=[x for x in out.splitlines() if ' %s '%fn in x][0]
    if ' OK ' in l: return 0
    return int(re.search(r'\((\d+) instr',l).group(1))
cur=list(chunks); best=score(cur); print('start',best)
improved=True
while improved and best:
  improved=False
  for i in range(len(cur)):
    if len(cur[i].rstrip('\n').split('\n'))!=2: continue
    t=list(cur); t[i]=swap(t[i]); s=score(t)
    if s<best: best=s; cur=t; improved=True; print(i,best); sys.stdout.flush()
    if best==0: break
open(p,'w').write(pre+'    p += 2;\n'.join(cur)+post)
print('final',best)
