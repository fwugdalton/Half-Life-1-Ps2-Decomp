# datacmp.py obj.o .section ROMADDR : compara a seção do .o com a ROM (palavras com relocação mostradas à parte)
import sys,subprocess,struct,re
import os as _os; sys.path.insert(0,_os.path.dirname(_os.path.dirname(_os.path.abspath(__file__))));import cmpfunc as c
obj,sec,addr=sys.argv[1],sys.argv[2],int(sys.argv[3],16)
showrel='-r' in sys.argv
import tempfile,os
_t=tempfile.mktemp(suffix='.bin'); subprocess.run(['mips-linux-gnu-objcopy','-O','binary','-j',sec,obj,_t],check=True); data=open(_t,'rb').read(); os.unlink(_t)
rel={}
out=subprocess.run(['mips-linux-gnu-objdump','-r','-j',sec,obj],capture_output=True,text=True).stdout
for l in out.splitlines():
    m=re.match(r'([0-9a-f]{8}) (R_MIPS_\w+)\s+(\S+)',l)
    if m: rel[int(m.group(1),16)]=m.group(3)
nd=0
for i in range(0,len(data),4):
    w=struct.unpack('<I',data[i:i+4].ljust(4,b'\0'))[0]
    r=struct.unpack('<I',c.ROM[addr+i-c.VRAM:addr+i-c.VRAM+4])[0]
    if i in rel:
        if showrel: print('%06x %08x R %s (+%x)'%(addr+i,r,rel[i],w))
    elif w!=r:
        nd+=1; print('%06x obj %08x rom %08x'%(addr+i,w,r))
print('tamanho %x, diferentes %d'%(len(data),nd))
