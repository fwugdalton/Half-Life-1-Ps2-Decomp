# rorange.py lo hi : faixa de rodata (0x236C80+0x100000..) referenciada pelas funções do 000000.s em [lo,hi)
import re,sys
lo,hi=int(sys.argv[1],16),int(sys.argv[2],16)
R0,R1=0x336C80,0x38B158
refs={}
cur=None;a=0
for l in open('asm/cod/000000.s'):
    m=re.match(r'glabel (\w+)',l)
    if m: cur=m.group(1); first=True; continue
    m2=re.match(r'\s*/\* [0-9A-F]+ (00[0-9A-F]{6})',l)
    if m2 and first: a=int(m2.group(1),16); first=False
    for d in re.findall(r'%lo\((?:D|jtbl)_00([0-9A-F]{6})\)',l):
        v=int(d,16)
        if R0<=v<R1: refs.setdefault(a,set()).add(v)
ins=[v for k,s in refs.items() if lo<=k<hi for v in s]
before=[v for k,s in refs.items() if k<lo for v in s]
after=[v for k,s in refs.items() if k>=hi for v in s]
print('dentro: %X..%X'%(min(ins),max(ins)))
print('antes max: %X'%max(before), ' antes>min dentro:', sorted('%X'%v for v in before if v>=min(ins))[:10])
print('depois min:', '%X'%min(after) if after else None)
