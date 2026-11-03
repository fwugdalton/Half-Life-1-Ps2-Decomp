# flipchain.py arquivo : em cada função NON_MATCHING, tenta inverter as cadeias "X = Y = expr;" (uma por vez e todas) e mantém o que melhora o SCORE
import sys,re,subprocess,os
p=sys.argv[1]
env=dict(os.environ,TMPDIR='/tmp/claude-0')
t='t_'+os.path.splitext(os.path.basename(p))[0]
def locate(s,fn):
    import re as _r
    m=_r.search(r'#ifdef NON_MATCHING[^\n]*\n(?:/\*[^\n]*\*/\n)?[^\n]*? %s\('%fn,s)
    return s.find(' %s('%fn,m.start()) if m else -1
def score(fn):
    asm=subprocess.run('grep -rl "glabel %s$" asm/nonmatchings'%fn,shell=True,capture_output=True,text=True).stdout.split()[0]
    subprocess.run(['bash','tools/handoff/scan.sh',p],capture_output=True,text=True,env=env)
    o=subprocess.run(['python3','tools/handoff/score.py',t,fn,asm],capture_output=True,text=True,env=env).stdout
    m=re.search(r'SCORE (\d+)',o); return int(m.group(1)) if m else 99999
src=open(p).read()
fns=re.findall(r'#ifdef NON_MATCHING[^\n]*\n(?:/\*[^\n]*\*/\n)?[^\n]*?(\w+)\(',src)
pat=re.compile(r'^(\s*)([^=;\n]+?) = ([^=;\n]+?) = ([^;\n]+);$',re.M)
for fn in fns:
    s=open(p).read()
    i=locate(s,fn)
    if i<0: continue
    j=s.find('#else',i); body=s[i:j]
    ms=list(pat.finditer(body))
    if not ms: continue
    base=score(fn); best=base; print(fn,'base',base,len(ms)); sys.stdout.flush()
    for k,m in enumerate(ms):
        s=open(p).read(); i=locate(s,fn); j=s.find('#else',i); body=s[i:j]
        mm=list(pat.finditer(body))[k]
        nb=body[:mm.start()]+'%s%s = %s = %s;'%(mm.group(1),mm.group(3),mm.group(2),mm.group(4))+body[mm.end():]
        open(p,'w').write(s[:i]+nb+s[j:])
        sc=score(fn)
        if sc<best: best=sc; print('  flip',k,'->',sc); sys.stdout.flush()
        else: open(p,'w').write(s)
    print(fn,'final',best); sys.stdout.flush()
