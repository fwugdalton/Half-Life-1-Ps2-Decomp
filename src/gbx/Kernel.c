/* Gbx/Kernel.c -- inicialização do IOP e E/S de arquivos host0: */

#include "common.h"

typedef struct {
    int fd;
    int size;
    char *name;
} KFILE;

int sprintf(char *buf, const char *fmt, ...);
unsigned int strlen(const char *s);
char *strcpy(char *dst, const char *src);
void *Mem_DbgMalloc(int size, const char *file, int line);
void Mem_Free(void *ptr);
void Mem_PushTag(void);
void Mem_SetTag(int tag);
void Mem_PopTag(void);

int func_00239D60(void);
void func_002D3D18(int);
void func_002F1E40(int);
void func_002D4658(int);
int func_002F4750(const char *);
int func_002F4708(void);
void func_002F2FD8(void);
void func_0022AEC0(void);
void func_002EF490(void);
int func_002F3010(const char *, int);
int func_002F32C8(int, int, int);
void func_002F31D0(int);
int func_002F34B0(int, void *, int);
int func_002F4270(const char *, int, const char *);

void func_00229CB0(const char *path);
int func_00229AF8(KFILE *f, int off, int whence);

extern unsigned int D_003A6450;
unsigned long D_003281E8 = 0;
unsigned long D_003281F0 = 0;

int func_00229720(void)
{
    int x;

    x = func_00239D60();
    func_002D3D18(0);
    func_002F1E40(0);
    func_002D4658(x);
    while (!func_002F4750("cdrom0:\\IOPIMAGE\\IOPRP214.IMG;1"));
    while (!func_002F4708());
    func_002F1E40(0);
    func_002D3D18(0);
    func_002D4658(x);
    func_002F2FD8();
    func_00229CB0("cdrom0:\\IRX\\SIO2MAN.IRX;1");
    func_00229CB0("cdrom0:\\IRX\\PADMAN.IRX;1");
    func_00229CB0("cdrom0:\\IRX\\USBD.IRX;1");
    func_00229CB0("cdrom0:\\IRX\\USBKEYBD.IRX;1");
    func_00229CB0("cdrom0:\\IRX\\USBMOUSE.IRX;1");
    func_00229CB0("cdrom0:\\IRX\\LIBSD.IRX;1");
    func_00229CB0("cdrom0:\\IRX\\SDRDRV.IRX;1");
    func_00229CB0("cdrom0:\\IRX\\MCMAN.IRX;1");
    func_00229CB0("cdrom0:\\IRX\\MCSERV.IRX;1");
    func_00229CB0("cdrom0:\\IRX\\GBXCD.IRX;1");
    return 1;
}

int func_00229828(void)
{
    func_0022AEC0();
    return 1;
}

void func_00229848(void)
{
    func_002EF490();
}

KFILE *func_00229868(const char *name, const char *mode)
{
    char buf[0x800];
    char *p;
    int flags;
    int append;
    int ok;
    int fd;
    int size;
    int r;
    KFILE *f;

    ok = 0;
    append = 0;
    sprintf(buf, "host0:%s", name);
    for (p = buf; *p != 0; p++) {
        if (*p == '\\') {
            *p = '/';
        }
    }
    flags = 0x200;
    for (; *mode != 0; mode++) {
        switch (*mode) {
        case 'a':
        case 'A':
            flags |= 0x102;
            append = 1;
            break;
        case 'w':
        case 'W':
            flags |= 0x403;
            break;
        case 'r':
        case 'R':
            flags &= ~0x200;
            flags |= 1;
            break;
        }
    }
    r = func_002F3010(buf, flags);
    if (r >= 0) {
        fd = r;
        size = func_002F32C8(fd, 0, 2);
        ok = 1;
        if (!append) {
            func_002F32C8(fd, 0, 0);
        }
    } else {
        fd = -1;
        size = 0;
    }
    if (ok != 1) {
        return 0;
    }
    Mem_PushTag();
    Mem_SetTag(2);
    f = Mem_DbgMalloc(sizeof(KFILE), "Gbx/Kernel.c", 0x130);
    f->name = Mem_DbgMalloc(strlen(name) + 1, "Gbx/Kernel.c", 0x131);
    strcpy(f->name, name);
    f->fd = fd;
    f->size = size;
    Mem_PopTag();
    return f;
}

void func_00229A30(KFILE *f)
{
    func_002F31D0(f->fd);
    Mem_Free(f->name);
    f->name = 0;
    Mem_Free(f);
}

int func_00229A70(void *buf, int size, int count, KFILE *f)
{
    return func_002F34B0(f->fd, buf, count * size);
}

int func_00229A98(KFILE *f)
{
    char c;
    int eof;

    if (func_00229A70(&c, 1, 1, f) == 1) {
        eof = 0;
        func_00229AF8(f, -1, 1);
    } else {
        eof = 1;
    }
    return eof;
}

int func_00229AF8(KFILE *f, int off, int whence)
{
    int w;

    switch (whence) {
    case 0:
        w = 0;
        break;
    case 1:
        w = 1;
        break;
    case 2:
        w = 2;
        break;
    default:
        w = 0;
        break;
    }
    return func_002F32C8(f->fd, off, w);
}

int func_00229B48(KFILE *f)
{
    return func_002F32C8(f->fd, 0, 1);
}

int func_00229B70(KFILE *f)
{
    return f->size;
}

void func_00229B78(void)
{
    unsigned int now;

    __asm__ volatile("mfc0 %0, $9" : "=r"(now));
    D_003A6450 = now;
}

unsigned long func_00229B88(void)
{
    unsigned int now;
    unsigned int last;
    unsigned long delta;
    unsigned long q;

    __asm__ volatile("mfc0 %0, $9" : "=r"(now));
    last = D_003A6450;
    if (now < last) {
        delta = (unsigned long)~last + (unsigned long)now;
    } else {
        delta = now - last;
    }
    D_003A6450 = now;
    D_003281F0 += delta;
    q = D_003281F0 / 0x48000;
    D_003281F0 -= (q << 18) + (q << 15);
    D_003281E8 += q;
    return D_003281E8;
}

unsigned long func_00229C38(unsigned long a, unsigned long b)
{
    unsigned long max = 0xFFFFFFFF;

    return (b < a) ? b + max - a : b - a;
}

void func_00229C60(unsigned long n)
{
    unsigned long start;

    start = func_00229B88();
    while (func_00229C38(start, func_00229B88()) < n);
}

#ifdef NON_MATCHING
/* só troca s0/s1 entre path e "" */
void func_00229CB0(const char *path)
{
retry:
    if (func_002F4270(path, 0, "") < 0) {
        goto retry;
    }
}
#else
const char D_00386EA0[8] = "";

INCLUDE_ASM("asm/nonmatchings/gbx/Kernel", func_00229CB0);
#endif
