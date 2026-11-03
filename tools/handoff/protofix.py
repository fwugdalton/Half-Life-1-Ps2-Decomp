# protofix.py: acha declarações "void f(...)" de funções definidas com outro tipo de retorno, troca e mantém só se alguma NON_MATCHING passar a casar (rodar da raiz)
import re,os,collections,subprocess,sys
pat=re.compile(r'^((?:extern\s+"C"\s+)?(?:static\s+)?)((?:unsigned |signed |const )?[A-Za-z_][A-Za-z0-9_]*\s*\**)(\s*)\b([A-Za-z_][A-Za-z0-9_]{3,})(\s*\([^;{]*\))\s*([;{]?)',re.M)
defs={}; decls=collections.defaultdict(list)
files=[]
for root,_,fs in os.walk('src'):
  for f in fs:
    if f.endswith(('.c','.cpp')): files.append(os.path.join(root,f))
for p in files:
  s=open(p,errors='replace').read()
  for m in pat.finditer(s):
    t=m.group(2).strip()
    if t in('return','else','new','delete','case','goto','typedef','struct','class','operator','define'):continue
    rest=s[m.end(5):m.end(5)+5]
    if m.group(6)=='{' or rest.lstrip().startswith('{'): defs[m.group(4)]=t
    elif m.group(6)==';': decls[p].append((m.group(0),m.group(4),t))
def scan(p):
  r=subprocess.run(['bash','tools/handoff/scan.sh',p],capture_output=True,text=True)
  out=r.stdout
  if 'Traceback' in r.stdout+r.stderr or ' OK ' not in out: return None
  return set(l.split()[2] for l in out.splitlines() if ' XX ' in l)
for p,ds in decls.items():
  bad=[(full,fn,t) for full,fn,t in ds if t=='void' and fn in defs and defs[fn]!='void']
  if not bad: continue
  src=open(p).read()
  if '#ifdef NON_MATCHING' not in src: continue
  before=scan(p)
  new=src
  for full,fn,t in bad:
    new=new.replace(full, full.replace('void',defs[fn],1),1)
  open(p,'w').write(new)
  after=scan(p)
  if before is None: open(p,'w').write(src); continue
  if after is not None and after < before:
    print('KEEP',p,'fixed:',sorted(before-after),[fn for _,fn,_ in bad]); sys.stdout.flush()
  else:
    open(p,'w').write(src)
    if after is None: print('compile fail',p)
    elif after!=before: print('revert',p,'new XX:',sorted(after-before))
