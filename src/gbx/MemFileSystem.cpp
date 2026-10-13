/* Gbx/MemFileSystem.cpp -- sistema de arquivos em memória (até 50 arquivos) */

#include "common.h"
#include <stdarg.h>

extern "C" {
#include "zlib/zlib.h"

void *Mem_DbgMalloc(int size, const char *file, int line);
void Mem_Free(void *ptr);
void Mem_PushTag(void);
void Mem_SetTag(int tag);
void Mem_PopTag(void);
void *Q_memcpy(void *dst, const void *src, int count);
float Sys_FloatTime(void);
int vsprintf(char *buf, const char *fmt, va_list ap);
unsigned int strlen(const char *s);
char *strcpy(char *dst, const char *src);
void CD_Read(int lsn, int sectors, void *buf);
void *FS_Open(const char *name, const char *mode);
void FS_Close(void *f);
int func_002224D8(const void *buf, int size, int count, void *f);
void *func_00225BD8(int size, const char *name);
void *func_00225FB0(void *ptr, int size, const char *name);
void *func_002260D0(void *ptr, int size, const char *file, int line);
char *func_00228CF0(char *dst, const char *src);
int func_00228DC0(const char *s);
int func_00228E40(const void *a, const void *b, int n);
int func_00228E60(const char *a, const char *b);
int func_0022CC20(int a, void *buf, int size, int *read);
int func_0022D6F8(int a, int b, int size, void *data);
void func_00237128(const char *path, int *size, int *lsn, int *sectors);
}

struct MemFile {
    int inUse;
    char name[0x100];
    int closed;
    int ownsData;
    char *data;
    int pos;
    int capacity;
    int size;
    float time;
    int mode;
    int type;
};

struct MemFind {
    int free;
    char pattern[0x100];
    int index;
};

extern "C" {
extern MemFile *D_003A6580;
extern MemFind D_003A6588[10];

void func_0022AFD0(const char *name, int size);
MemFile *func_0022B118(const char *name, const char *mode);
void func_0022B238(MemFile *f);
void func_0022B248(const char *name);
void func_0022B2B0(const char *name, const char *newName);
int func_0022B6D0(const char *name);
int func_0022B810(int handle, char *name);
void func_0022B8A0(int handle);
void func_0022BC60(const char *name);
int func_0022BD98(const void *buf, int size, int count, MemFile *f);
int func_0022BE50(void *buf, int size, int count, MemFile *f);
char *func_0022BF38(MemFile *f);
void func_0022BF48(MemFile *f, int n);
void func_0022BF68(MemFile *f, int n);
MemFile *func_0022BFD8(const char *name);
int func_0022C078(int start, const char *pattern);
int func_0022C118(const char *pattern, const char *str);
int func_0022C210(const char *str, int *mode, int *type);
void func_0022C3C8(z_stream *zs, char **buf, int *size);
}

extern "C" void func_0022AE48(void)
{
    int i;

    D_003A6580 = (MemFile *)Mem_DbgMalloc(50 * sizeof(MemFile), "Gbx/MemFileSystem.cpp", 0x7A);
    for (i = 0; i < 50; i++) {
        D_003A6580[i].inUse = 0;
    }
    for (i = 0; i < 10; i++) {
        D_003A6588[i].free = 1;
    }
}

extern "C" void func_0022AEC0(void)
{
    int i;

    for (i = 0; i < 50; i++) {
        if (D_003A6580[i].inUse == 1) {
            Mem_Free(D_003A6580[i].data);
        }
    }
    Mem_Free(D_003A6580);
}

/* copia um arquivo do CD para a memória */
extern "C" void func_0022AF38(const char *path, const char *name)
{
    int size;
    int lsn;
    int sectors;
    MemFile *f;

    func_00237128(path, &size, &lsn, &sectors);
    func_0022AFD0(name, sectors << 11);
    f = func_0022B118(name, "wb");
    CD_Read(lsn, sectors, func_0022BF38(f));
    func_0022BF48(f, size);
    func_0022B238(f);
}

#ifdef NON_MATCHING
/* o original não iça a constante 1 do laço */
extern "C" void func_0022AFD0(const char *name, int size)
{
    int i;
    MemFile *f;

    for (i = 0; i < 50; i++) {
        f = &D_003A6580[i];
        if (f->inUse == 0) {
            f->inUse = 1;
            func_00228CF0(f->name, name);
            f->ownsData = 1;
            f->closed = 1;
            f->data = (char *)func_00225BD8(size, f->name);
            f->capacity = size;
            f->size = 0;
            f->time = Sys_FloatTime();
            return;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/MemFileSystem", func_0022AFD0);
#endif

#ifdef NON_MATCHING
/* o original não iça a constante 1 do laço */
extern "C" void func_0022B078(const char *name, char *data, int size)
{
    int i;
    MemFile *f;

    for (i = 0; i < 50; i++) {
        f = &D_003A6580[i];
        if (f->inUse == 0) {
            f->inUse = 1;
            func_00228CF0(f->name, name);
            f->closed = 1;
            f->data = data;
            f->size = size;
            f->ownsData = 0;
            f->capacity = size;
            f->time = Sys_FloatTime();
            return;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/MemFileSystem", func_0022B078);
#endif

#ifdef NON_MATCHING
/* o valor de retorno passa por a0 no original */
extern "C" MemFile *func_0022B118(const char *name, const char *modeStr)
{
    int mode;
    int type;
    MemFile *f;

    if (func_0022C210(modeStr, &mode, &type) != 1) {
        return 0;
    }
    switch (mode) {
    case 1:
    case 4:
        f = func_0022BFD8(name);
        if (f == 0) {
            return 0;
        }
        if (f->closed != 1) {
            return 0;
        }
        f->closed = 0;
        f->pos = 0;
        break;
    case 2:
    case 5:
        f = func_0022BFD8(name);
        if (f == 0) {
            func_0022BC60(name);
            f = func_0022BFD8(name);
            f->closed = 0;
            f->pos = 0;
        } else {
            func_0022BC60(name);
            if (f->closed != 1) {
                return 0;
            }
            f->closed = 0;
            f->pos = 0;
            f->size = 0;
        }
        break;
    case 3:
    case 6:
    default:
        return 0;
    }
    f->mode = mode;
    f->type = type;
    return f;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/MemFileSystem", func_0022B118);
#endif

extern "C" void func_0022B238(MemFile *f)
{
    f->closed = 1;
}

extern "C" void func_0022B248(const char *name)
{
    MemFile *f;

    f = func_0022BFD8(name);
    if (f != 0 && f->closed == 1) {
        f->inUse = 0;
        strcpy(f->name, "Deleted");
        if (f->ownsData == 1) {
            Mem_Free(f->data);
        }
    }
}

extern "C" void func_0022B2B0(const char *name, const char *newName)
{
    MemFile *f;

    f = func_0022BFD8(name);
    if (f != 0 && f->closed == 1 && !func_0022B6D0(newName)) {
        func_00228CF0(f->name, newName);
    }
}

#ifdef NON_MATCHING
/* alocação de registradores diferente (ok/initOk) */
/* comprime um arquivo da memória (int tamanho original + zlib) */
extern "C" int func_0022B310(const char *name, int level)
{
    int ok = 0;
    char *out;
    int outSize;
    int origSize;
    MemFile *src;
    MemFile *tmp;
    MemFile *f;
    z_stream *zs;
    int initOk;
    int err;
    int again;

    if (func_0022B6D0(name) == 1) {
    func_0022B2B0(name, "MemFSCompress.tmp");
    src = func_0022BFD8("MemFSCompress.tmp");
    tmp = func_0022B118("MemFSCompress.tmp", "rb");
    if (tmp != 0 && src != 0) {
    Mem_PushTag();
    Mem_SetTag(2);
    outSize = 0;
    out = 0;
    zs = (z_stream *)Mem_DbgMalloc(sizeof(z_stream), "Gbx/MemFileSystem.cpp", 0x1F5);
    zs->zalloc = 0;
    zs->zfree = 0;
    initOk = 0;
    if (deflateInit(zs, level) == Z_OK) {
        initOk = 1;
        func_0022C3C8(zs, &out, &outSize);
    }
    if (initOk == 1) {
        zs->avail_in = src->size;
        zs->next_in = (Bytef *)src->data;
        do {
            err = deflate(zs, Z_NO_FLUSH);
            if (zs->avail_out == 0) {
                func_0022C3C8(zs, &out, &outSize);
                again = 1;
            } else {
                again = 0;
            }
        } while (again);
        if (err == Z_OK) {
            do {
                err = deflate(zs, Z_FINISH);
                if (zs->avail_out == 0) {
                    func_0022C3C8(zs, &out, &outSize);
                    again = 1;
                } else {
                    again = 0;
                }
            } while (again);
            if (err == Z_STREAM_END) {
                origSize = src->size;
                ok = 1;
                Mem_PushTag();
                Mem_SetTag(1);
                func_0022AFD0(name, zs->total_out + 4);
                f = func_0022B118(name, "wb");
                func_0022BD98(&origSize, 4, 1, f);
                func_0022BD98(out, 1, zs->total_out, f);
                func_0022B238(f);
                Mem_PopTag();
                func_0022B238(tmp);
                func_0022B248("MemFSCompress.tmp");
                Mem_Free(out);
            }
        }
    }
    deflateEnd(zs);
    Mem_Free(zs);
    Mem_PopTag();
    }
    }
    return ok;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/MemFileSystem", func_0022B310);
INCLUDE_RODATA("asm/nonmatchings/gbx/MemFileSystem", D_00386F88);
INCLUDE_RODATA("asm/nonmatchings/gbx/MemFileSystem", D_00386F90);
#endif

#ifdef NON_MATCHING
/* só a ordem de carga de dst->data */
/* descomprime src para um novo arquivo dst */
extern "C" int func_0022B590(const char *srcName, const char *dstName)
{
    int ok = 0;
    int size;
    MemFile *src;
    MemFile *h;
    MemFile *dst;
    z_stream *zs;

    src = func_0022BFD8(srcName);
    h = func_0022B118(srcName, "rb");
    if (h != 0 && src != 0) {
    zs = (z_stream *)Mem_DbgMalloc(sizeof(z_stream), "Gbx/MemFileSystem.cpp", 0x27C);
    zs->zalloc = 0;
    zs->zfree = 0;
    if (inflateInit(zs) == Z_OK) {
        func_0022BE50(&size, 4, 1, h);
        ok = 1;
        func_0022AFD0(dstName, size);
        dst = func_0022BFD8(dstName);
        zs->avail_in = src->size - src->pos;
        zs->next_in = (Bytef *)(src->data + src->pos);
        zs->next_out = (Bytef *)dst->data;
        zs->avail_out = size;
        inflate(zs, Z_SYNC_FLUSH);
        dst->size = size;
    }
    inflateEnd(zs);
    Mem_Free(zs);
    func_0022B238(h);
    }
    return ok;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/MemFileSystem", func_0022B590);
#endif

extern "C" int func_0022B6D0(const char *name)
{
    return func_0022BFD8(name) != 0;
}

extern "C" int func_0022B6F0(const char *pattern, char *name)
{
    int index;
    int handle;
    int found;
    int i;

    index = func_0022C078(0, pattern);
    if (index < 50) {
        found = 0;
        for (i = 0; i < 10; i++) {
            if (D_003A6588[i].free == 1) {
                found = 1;
                break;
            }
        }
        if (found == 1) {
            handle = i;
            D_003A6588[handle].free = 0;
            func_00228CF0(D_003A6588[handle].pattern, pattern);
            D_003A6588[handle].index = index;
            func_00228CF0(name, D_003A6580[index].name);
        } else {
            handle = -1;
        }
    } else {
        handle = -1;
    }
    return handle;
}

extern "C" int func_0022B810(int handle, char *name)
{
    int index;

    index = func_0022C078(D_003A6588[handle].index + 1, D_003A6588[handle].pattern);
    D_003A6588[handle].index = index;
    if (index >= 50) {
        return 0;
    }
    func_00228CF0(name, D_003A6580[index].name);
    return 1;
}

extern "C" void func_0022B8A0(int handle)
{
    D_003A6588[handle].free = 1;
}

extern "C" int func_0022B8C0(const char *name)
{
    return func_0022BFD8(name)->size;
}

extern "C" float func_0022B8E0(const char *name)
{
    return func_0022BFD8(name)->time;
}

extern "C" void func_0022B900(void)
{
    int i;

    for (i = 0; i < 50; i++) {
    }
}

/* grava um arquivo da memória em host0: */
extern "C" void func_0022B918(const char *name, const char *path)
{
    MemFile *f;
    void *fp;

    f = func_0022BFD8(name);
    if (f != 0) {
        fp = FS_Open(path, "wb");
        func_002224D8(f->data, 1, f->size, fp);
        FS_Close(fp);
    }
}

extern "C" int func_0022B980(const char *name, int a, int b)
{
    MemFile *f;

    f = func_0022BFD8(name);
    if (f == 0) {
        return 0;
    }
    return func_0022D6F8(b, a, f->size, f->data);
}

extern "C" int func_0022B9D8(MemFile *f, int a, int size)
{
    int ok = 0;
    int grow;
    int read;

    if (f->pos + size > f->capacity) {
        grow = f->pos + size - f->capacity;
        f->data = (char *)func_00225FB0(f->data, f->capacity + grow, f->name);
        f->capacity += grow;
    }
    if (func_0022CC20(a, func_0022BF38(f), size, &read) == 0) {
        func_0022BF48(f, read);
        ok = (size == read);
    }
    return ok;
}

/* realoca os arquivos que casam com o padrão, do heap temporário para o permanente */
extern "C" void func_0022BAA0(const char *pattern)
{
    char name[0x100];
    int handle;
    MemFile *f;
    int size;
    char *tmp;

    handle = func_0022B6F0(pattern, name);
    if (handle == -1) {
        return;
    }
    do {
        f = func_0022BFD8(name);
        if (f->closed == 1) {
            size = f->size;
            Mem_PushTag();
            Mem_SetTag(2);
            tmp = (char *)Mem_DbgMalloc(size, "Gbx/MemFileSystem.cpp", 0x39C);
            Mem_PopTag();
            Q_memcpy(tmp, f->data, size);
            Mem_Free(f->data);
            f->data = 0;
            Mem_PushTag();
            Mem_SetTag(1);
            f->data = (char *)Mem_DbgMalloc(size, "Gbx/MemFileSystem.cpp", 0x3A5);
            Mem_PopTag();
            f->capacity = size;
            Q_memcpy(f->data, tmp, size);
            Mem_Free(tmp);
        }
    } while (func_0022B810(handle, name) == 1);
    func_0022B8A0(handle);
}

extern "C" void func_0022BBC0(const char *name)
{
    MemFile *src;
    MemFile *dst;

    src = func_0022BFD8(name);
    func_0022B2B0(name, "ShortToLong.tmp");
    Mem_PushTag();
    Mem_SetTag(1);
    func_0022AFD0(name, src->size);
    Mem_PopTag();
    dst = func_0022BFD8(name);
    Q_memcpy(dst->data, src->data, src->size);
    dst->size = src->size;
    func_0022B248("ShortToLong.tmp");
}

extern "C" void func_0022BC60(const char *name)
{
    MemFile *f;

    f = func_0022BFD8(name);
    if (f == 0) {
        func_0022AFD0(name, 0x400);
    } else {
        f->time = Sys_FloatTime();
    }
}

extern "C" int func_0022BCB8(const char *a, const char *b)
{
    MemFile *fa;
    MemFile *fb;
    int same;

    fa = func_0022BFD8(a);
    fb = func_0022BFD8(b);
    same = 0;
    if (fa != 0 && fb != 0 && fa->size == fb->size) {
        same = func_00228E40(fa->data, fb->data, fa->size) == 0;
    }
    return same;
}

extern "C" int FS_FPrintf(MemFile *f, const char *fmt, ...)
{
    char buf[0x800];
    va_list ap;

    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    return func_0022BD98(buf, 1, strlen(buf), f);
}

extern "C" int func_0022BD98(const void *buf, int size, int count, MemFile *f)
{
    int n;
    int grow;

    n = size * count;
    if (f->pos + n > f->capacity) {
        grow = (n >= 0x400) ? n : 0x400;
        f->data = (char *)func_00225FB0(f->data, f->capacity + grow, f->name);
        f->capacity += grow;
    }
    Q_memcpy(f->data + f->pos, buf, n);
    f->size += n;
    f->pos += n;
    return n;
}

extern "C" int func_0022BE50(void *buf, int size, int count, MemFile *f)
{
    int n;

    n = size * count;
    if (f->pos + n > f->size) {
        n = f->size - f->pos;
    }
    Q_memcpy(buf, f->data + f->pos, n);
    f->pos += n;
    return n;
}

extern "C" int func_0022BEB8(MemFile *f, int offset, int whence)
{
    int pos;

    switch (whence) {
    case 0:
        pos = offset;
        break;
    case 1:
        pos = f->pos + offset;
        break;
    case 2:
        pos = f->size - offset;
        break;
    default:
        pos = 0;
        break;
    }
    if (pos >= 0 && pos <= f->size) {
        f->pos = pos;
        return 0;
    }
    return 1;
}

extern "C" int func_0022BF28(MemFile *f)
{
    return f->pos;
}

extern "C" int func_0022BF30(MemFile *f)
{
    return f->size;
}

extern "C" char *func_0022BF38(MemFile *f)
{
    return f->data + f->pos;
}

extern "C" void func_0022BF48(MemFile *f, int n)
{
    f->size += n;
    f->pos += n;
}

extern "C" void func_0022BF68(MemFile *f, int n)
{
    f->pos += n;
}

extern "C" void func_0022BF78(MemFile *f, MemFile *dst, int n)
{
    func_0022BD98(func_0022BF38(f), n, 1, dst);
    func_0022BF68(f, n);
}

extern "C" MemFile *func_0022BFD8(const char *name)
{
    MemFile *f = 0;
    int i;

    for (i = 0; i < 50; i++) {
        if (D_003A6580[i].inUse == 1 && func_00228E60(D_003A6580[i].name, name) == 0) {
            f = &D_003A6580[i];
            break;
        }
    }
    return f;
}

extern "C" int func_0022C078(int start, const char *pattern)
{
    int i;

    for (i = start; i < 50; i++) {
        if (D_003A6580[i].inUse == 1 && func_0022C118(pattern, D_003A6580[i].name) == 1) {
            break;
        }
    }
    return i;
}

#ifdef NON_MATCHING
/* no original str fica em a1 e só vai para s0 no caso '*' */
/* casa nomes com curingas '*' e '?' */
extern "C" int func_0022C118(const char *pattern, const char *str)
{
    for (;;) {
        switch (*pattern) {
        case '\0':
            return *str == '\0';
        case '*':
            if (pattern[1] == '\0') {
                return 1;
            }
            for (; *str != '\0'; str++) {
                if (func_0022C118(pattern + 1, str)) {
                    return 1;
                }
            }
            return func_0022C118(pattern + 1, str);
        case '?':
            if (*str == '\0') {
                return 0;
            }
            break;
        default:
            if (*str != *pattern) {
                return 0;
            }
            break;
        }
        pattern++;
        str++;
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/MemFileSystem", func_0022C118);
#endif

#ifdef NON_MATCHING
/* só a ordem de i = 0 e *type = 0 */
/* interpreta o modo do fopen: r/w/a (+) e b/t */
extern "C" int func_0022C210(const char *str, int *mode, int *type)
{
    int len;
    int plus;
    int err;
    int i;

    len = func_00228DC0(str);
    plus = 0;
    if (len < 4) {
        *mode = 0;
        *type = 0;
        err = 0;
        for (i = 0; i < len; i++) {
            switch (str[i]) {
            case 'r':
            case 'R':
                if (*mode != 0) {
                    err = 1;
                } else {
                    *mode = 1;
                }
                break;
            case 'w':
            case 'W':
                if (*mode != 0) {
                    err = 1;
                } else {
                    *mode = 2;
                }
                break;
            case 'a':
            case 'A':
                if (*mode != 0) {
                    err = 1;
                } else {
                    *mode = 3;
                }
                break;
            case '+':
                if (plus != 0) {
                    err = 1;
                } else {
                    plus = 1;
                }
                break;
            case 'b':
            case 'B':
                if (*type != 0) {
                    err = 1;
                } else {
                    *type = 2;
                }
                break;
            case 't':
            case 'T':
                if (*type != 0) {
                    err = 1;
                } else {
                    *type = 1;
                }
                break;
            default:
                err = 1;
                break;
            }
        }
        if (*mode == 0) {
            err = 1;
        }
        if (*type == 0) {
            *type = 1;
        }
        if (plus == 1) {
            switch (*mode) {
            case 1:
                *mode = 4;
                break;
            case 2:
                *mode = 5;
                break;
            case 3:
                *mode = 6;
                break;
            }
        }
    } else {
        err = 1;
    }
    return err ^ 1;
}
#else
/* a tabela de salto do gcc vem alinhada em 16 */
__asm__(".section .rodata\n.balign 16\n.section .text");
INCLUDE_ASM("asm/nonmatchings/gbx/MemFileSystem", func_0022C210);
#endif

/* aumenta o buffer de saída do deflate em 1 KB */
extern "C" void func_0022C3C8(z_stream *zs, char **buf, int *size)
{
    *buf = (char *)func_002260D0(*buf, *size + 0x400, "Gbx/MemFileSystem.cpp", 0x5E3);
    *size += 0x400;
    zs->next_out = (Bytef *)(*buf + zs->total_out);
    zs->avail_out = *size - zs->total_out;
}
