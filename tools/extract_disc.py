import sys,os,struct
src,out=sys.argv[1],sys.argv[2]
f=open(src,'rb')
def sec(lba,n=1):
    b=bytearray()
    for i in range(n):
        f.seek((lba+i)*2352+24); b+=f.read(2048)
    return bytes(b)
pvd=sec(16)
root=pvd[156:156+34]
def walk(rec,path):
    lba=struct.unpack('<I',rec[2:6])[0]; size=struct.unpack('<I',rec[10:14])[0]
    data=sec(lba,(size+2047)//2048)
    i=0
    while i<size:
        l=data[i]
        if l==0: i=(i//2048+1)*2048; continue
        r=data[i:i+l]; i+=l
        nl=r[32]; name=r[33:33+nl]
        if name in (b'\x00',b'\x01'): continue
        name=name.decode().split(';')[0]
        flags=r[25]; p=os.path.join(path,name)
        if flags&2:
            os.makedirs(p,exist_ok=True); walk(r,p)
        else:
            flba=struct.unpack('<I',r[2:6])[0]; fs=struct.unpack('<I',r[10:14])[0]
            with open(p,'wb') as o:
                rem=fs; l2=flba
                while rem>0:
                    n=min(rem,2048*512)
                    o.write(sec(l2,(n+2047)//2048)[:n]); rem-=n; l2+=(n+2047)//2048
            print(fs,p)
os.makedirs(out,exist_ok=True); walk(root,out)
