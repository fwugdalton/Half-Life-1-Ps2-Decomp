# score.py t_Arquivo func asm/nonmatchings/.../func.s [-v] : diferença por alinhamento (difflib) entre a ROM
# e o .o que o tools/handoff/scan.sh deixou em $TMPDIR/nmscan/t_Arquivo. Melhor que a contagem do cmpfunc
# quando uma instrução a mais desloca todo o resto. -v mostra os trechos diferentes.
import sys,re,subprocess,difflib,os
t,fn,asm=sys.argv[1:4]; verbose='-v' in sys.argv
R=['zero','at','v0','v1','a0','a1','a2','a3','t0','t1','t2','t3','t4','t5','t6','t7','s0','s1','s2','s3','s4','s5','s6','s7','t8','t9','k0','k1','gp','sp','s8','ra']
def num(x):
    try: return str(int(x,0))
    except: return x
def norm(op,args):
    args=[a.strip() for a in args.split(',')] if args else []
    out=[]
    for a in args:
        a=re.sub(r'\$(\d+)',lambda m:R[int(m.group(1))],a)
        a=a.replace('$fp','s8').replace('fp','s8')
        if '%' in a or '.L' in a or '<' in a or 'func_' in a or 'D_' in a: a='X'
        m=re.match(r'^(-?(?:0x)?[0-9a-fA-F]+)\((\w+)\)$',a)
        if m: a=num(m.group(1))+'('+m.group(2)+')'
        elif re.match(r'^-?(0x)?[0-9a-fA-F]+$',a) and not re.match(r'^[a-z]\d$',a): a=num(a) if (a.startswith('0x') or a.startswith('-') or a.isdigit()) else a
        out.append(a)
    if op in ('daddu','move','addu','or') and len(out)==3 and out[2]=='zero': op='move'; out=out[:2]
    if op=='move' and len(out)==3: out=out[:2]
    if op=='bnez' or op=='beqz' or op.startswith('b') or op in('j','jal'): out=[o for o in out if o!='X'] + ['L']
    if op=='li': op='addiu'; out=[out[0],'zero',out[1]]
    if op=='lui' and len(out)==2: out[1]='X'
    if op=='bnezl': op='bnel'; out=[out[0],'zero','L']
    if op=='beqzl': op='beql'; out=[out[0],'zero','L']
    if op=='bnez': op='bne'; out=[out[0],'zero','L']
    if op=='beqz': op='beq'; out=[out[0],'zero','L']
    if op=='b': op='beq'; out=['zero','zero','L']
    return op+' '+','.join(out)
rom=[]
for l in open(asm):
    m=re.search(r'\*/\s+([a-z0-9.]+)\s*(.*)$',l)
    if m and not l.strip().startswith('.'): rom.append(norm(m.group(1),m.group(2)))
d=subprocess.run(['mips-linux-gnu-objdump','-d','--no-show-raw-insn',(os.path.join(os.environ.get('TMPDIR','/tmp'),'nmscan',t,'a.o') if not t.startswith('/') else t)],capture_output=True,text=True).stdout
blk=d[d.index('<%s>:'%fn)+1:].split('\n')[1:]
ours=[]
for l in blk:
    if not l.strip() or l.endswith('>:'): break
    p=l.split('\t')
    if len(p)<2: continue
    op=p[1].strip(); args=re.sub(r'\s*<[^>]*>','',p[2]) if len(p)>2 else ''
    ours.append(norm(op,args))
while ours and ours[-1]=='nop ': ours.pop()
while rom and rom[-1]=='nop ': rom.pop()
sm=difflib.SequenceMatcher(None,rom,ours,autojunk=False)
diff=0
for tag,i1,i2,j1,j2 in sm.get_opcodes():
    if tag!='equal':
        diff+=max(i2-i1,j2-j1)
        if verbose:
            print('---',tag,i1)
            for k in range(max(i2-i1,j2-j1)):
                a=rom[i1+k] if i1+k<i2 else ''; b=ours[j1+k] if j1+k<j2 else ''
                print('%-40s| %s'%(a,b))
print('SCORE',diff,'rom',len(rom),'ours',len(ours))
