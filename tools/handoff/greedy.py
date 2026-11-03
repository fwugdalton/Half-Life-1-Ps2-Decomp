# greedy.py arquivo funcao_asm [rounds] : hill climbing com trocas de linhas vizinhas e de operandos comutativos
# no bloco #ifdef NON_MATCHING da função; mantém o que baixa o SCORE (score.py). Para C e C++.
import sys,re,subprocess,os
p,fn=sys.argv[1:3]; rounds=int(sys.argv[3]) if len(sys.argv)>3 else 2
env=dict(os.environ,TMPDIR='/tmp/claude-0')
t='t_'+os.path.splitext(os.path.basename(p))[0]
asm=subprocess.run('grep -rl "glabel %s$" asm/nonmatchings'%fn,shell=True,capture_output=True,text=True).stdout.split()[0]
def block(s):
    k=s.index('INCLUDE_ASM'); k=s.find(', %s)'%fn)
    e=s.rfind('#else',0,k); b=s.rfind('#ifdef NON_MATCHING',0,e); b=s.index('\n',b)+1
    return b,e
def score(s):
    open(p,'w').write(s)
    o=subprocess.run(['bash','tools/handoff/scan.sh',p],capture_output=True,text=True,env=env).stdout
    if ' %s '%fn in o and [l for l in o.splitlines() if ' %s '%fn in l and ' OK ' in l]: return 0
    o=subprocess.run(['python3','tools/handoff/score.py',t,fn,asm],capture_output=True,text=True,env=env).stdout
    m=re.search(r'SCORE (\d+) rom (\d+) ours (\d+)',o)
    return int(m.group(1))+abs(int(m.group(2))-int(m.group(3))) if m else 99999
orig=open(p).read()
best=score(orig); cur=orig; print('base',best); sys.stdout.flush()
simple=re.compile(r'^\s+[^{}\n]*;\s*$')
ctrl=re.compile(r'^\s*(if|else|for|while|do|return|break|continue|case|default|switch|goto)\b')
def cands(s):
    b,e=block(s); body=s[b:e]; L=body.split('\n'); out=[]
    for i in range(len(L)-1):
        a,c=L[i],L[i+1]
        if simple.match(a) and simple.match(c) and not ctrl.match(a) and not ctrl.match(c) and len(a)-len(a.lstrip())==len(c)-len(c.lstrip()):
            M=L[:]; M[i],M[i+1]=c,a; out.append(('swap%d'%i,s[:b]+'\n'.join(M)+s[e:]))
    tok=r'[A-Za-z_][\w\.\->\[\]]*'
    for m in re.finditer(r'(?<![\w.\]>-])(%s) (\+|\*|==|!=|&|\||\^) (%s)(?![\w\.\[(])'%(tok,tok),body):
        if m.group(1)==m.group(3): continue
        nb=body[:m.start()]+m.group(3)+' '+m.group(2)+' '+m.group(1)+body[m.end():]
        out.append(('comm@%d'%m.start(),s[:b]+nb+s[e:]))
    return out
try:
  for r in range(rounds):
    improved=False; i=0
    while True:
        C=cands(cur)
        if i>=len(C): break
        name,s=C[i]; i+=1
        sc=score(s)
        if sc<best:
            best=sc; cur=s; improved=True; print(' ',name,'->',sc); sys.stdout.flush()
            if best==0: break
    if best==0 or not improved: break
finally:
    open(p,'w').write(cur)
print(fn,'final',best)
