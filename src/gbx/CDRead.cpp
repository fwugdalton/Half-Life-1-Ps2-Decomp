/* Gbx: leitura do CD (ISO9660) pelo IOP e árvore de diretórios em memória */

#include "common.h"

extern "C" {
#include "engine/cmd.h"

/* um arquivo do CD (0x1C bytes) */
struct CdFile {
    char name[0x10];
    int lba;
    int sectors;
    int size;
};

/* um diretório do CD (0x24 bytes) */
struct CdDir {
    char name[0xC];
    int lba;
    int sectors;
    int numFiles;
    CdFile *files;
    int numDirs;
    CdDir *dirs;
};

/* ee_sema_t */
struct SemaParam {
    int count;
    int max_count;
    int init_count;
    int wait_threads;
    int attr;
    int option;
};

/* argumentos da leitura por RPC */
struct CdReadArgs {
    int lba;
    int sectors;
    void *buf;
};

void Mem_PushTag(void);
void Mem_SetTag(int tag);
void Mem_PopTag(void);
int func_002EF660(SemaParam *p);
int func_002EF690(int sema);
int func_002EF6A0(int sema);
void func_002EF8E0(int a);
int func_002F26A0(void *cd, int fno, int mode, void *send, int ssize, void *recv, int rsize, void *end, void *endparam);
int func_002F2890(void *cd);
int func_00228C00(const char *a, const char *b);
int func_00228C48(const char *a, const char *b);
void func_00228CF0(char *dst, const char *src);
int func_00228DC0(const char *s);
int func_00228E40(const char *a, const char *b, int n);
void func_00225860(void *dst, int c, int n);
void *func_00225D48(int size, int align, const char *file, int line);
unsigned long func_00229B88(void);
void func_00238FC8(int a, void (*f)(void), int b);
int func_00239D40(void);
void func_00239D50(int a);
int func_00239D60(void);
int func_002D3938(void);
int func_002D3A98(int a);
void func_002D3D18(int a);
int func_002D4000(int a);
int func_002D42B0(void);
int func_002D4348(void);
void func_002D4658(int a);
int func_002D4AD0(int a, int b);
void CD_Read(int lba, int sectors, void *buf);
int CD_SectorCount(int size);
void func_00237590(CdDir *d);
CdDir *func_00237628(CdDir *d, const char *name, int len);
CdDir *func_002376D0(CdDir *d, int lba);
CdDir *func_00237730(CdDir *d, int lba);
CdFile *func_002377D8(CdDir *d, const char *name, int len);
void func_00237880(CdDir *d, int depth);
int func_00237950(CdDir *d, int *size, int *lba, int *sectors);
int func_00237A80(CdFile *f);
void func_00237B40(int *lba, int *sectors);
void func_00237BB8(int lba, int sectors, int *numFiles, int *numDirs);
void func_00237CE0(CdDir *d);
int func_00237528(int size);
int func_00238020(unsigned char *buf, int off);
void func_00238070(void);

extern char D_0032EF40[];       /* cliente RPC do IOP */
extern const char D_003886D0[];
extern int D_00328B20;
extern int D_00328B24;
extern CdReadArgs D_003A89C0;
extern int D_003A89D0;
extern unsigned char D_003A8A00[0x800];
extern int D_003A9200;
extern CdDir D_003A9208;        /* raiz */
extern CdDir *D_003A922C;       /* diretório atual */

INCLUDE_RODATA("asm/nonmatchings/gbx/CDRead", D_003886C0);

#ifdef NON_MATCHING /* a ordem de dois sw (D_003A922C e p.option) e lui v0/v1 diferem */
void func_00236EF0(void) {
    SemaParam p;

    D_003A89D0 = 0;
    func_00228CF0(D_003A9208.name, "");
    D_003A9208.files = 0;
    D_003A9208.numDirs = 0;
    D_003A9208.dirs = 0;
    p.max_count = 1;
    p.init_count = 0;
    D_003A922C = &D_003A9208;
    p.option = 0;
    func_002EF8E0(0);
    D_003A9200 = func_002EF660(&p);
    func_00238FC8(5, func_00238070, 0);
    D_00328B20 = 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/CDRead", func_00236EF0);
#endif

void func_00236F90(void) {
    func_00237590(&D_003A9208);
    D_003A922C = &D_003A9208;
    func_00237B40(&D_003A9208.lba, &D_003A9208.sectors);
    func_00237CE0(&D_003A9208);
}

void func_00236FD8(const char *path) {
    CdDir *d;
    const char *name;
    const char *p;
    int ok;
    int len;
    int start;
    int i;

    if (func_00228C48(D_003A9208.name, path) == 1 || func_00228C00("\\", path) == 1) {
        d = &D_003A9208;
        ok = 1;
    } else {
        d = &D_003A9208;
        ok = 1;
        len = func_00228DC0(path);
        name = path;
        start = 0;
        for (i = 0; i < len; i++) {
            p = &path[i];
            if (*p == '\\' || i == len - 1) {
                d = func_00237628(d, name, i - start);
                name = p + 1;
                start = i + 1;
                if (d == 0) {
                    ok = 0;
                    break;
                }
            }
        }
    }
    if (ok) {
        D_003A922C = d;
    }
}

INCLUDE_RODATA("asm/nonmatchings/gbx/CDRead", D_003886D0);

void func_002370F0(int lba) {
    CdDir *d;

    d = func_00237730(&D_003A9208, lba);
    if (d != 0) {
        D_003A922C = d;
    }
}

int func_00237128(const char *path, int *size, int *lba, int *sectors) {
    CdDir *d;
    CdFile *f;
    const char *name;
    int ok;
    int len;
    int start;
    int i;
    const char *p;

    ok = 1;
    *size = 0;
    *lba = 0;
    *sectors = 0;
    d = D_003A922C;
    len = func_00228DC0(path);
    name = path;
    start = 0;
    for (i = 0; i < len; i++) {
        p = &path[i];
        if (*p == '\\') {
            d = func_00237628(d, name, i - start);
            name = p + 1;
            start = i + 1;
            if (d == 0) {
                ok = 0;
                break;
            }
        }
    }
    if (ok == 1) {
        f = func_002377D8(d, name, i - start);
        if (f != 0) {
            *size = f->size;
            *lba = f->lba;
            *sectors = f->sectors;
        } else {
            ok = 0;
        }
    }
    return ok;
}

int func_00237258(int *size, int *lba, int *sectors) {
    *size = 0;
    *lba = 0;
    *sectors = 0;
    return func_00237950(&D_003A9208, size, lba, sectors);
}

void CD_Read(int lba, int sectors, void *buf) {
    D_003A89C0.lba = lba;
    D_003A89C0.sectors = sectors;
    D_003A89C0.buf = buf;
    while (func_002F2890(D_0032EF40) != 0) {
    }
    func_002F26A0(D_0032EF40, 0x12C, 1, &D_003A89C0, 0x10, 0, 0, 0, 0);
    func_002EF8E0(0);
    func_002EF6A0(D_003A9200);
}

int func_00237330(const char *path, void **buf) {
    int size;
    int lba;
    int sectors;

    *buf = 0;
    if (func_00237128(path, &size, &lba, &sectors) == 1) {
        *buf = func_00225D48(sectors << 11, 0x40, D_003886D0, 0x151);
        CD_Read(lba, sectors, *buf);
    }
    return size;
}

void func_002373A8(void) {
    func_00237880(&D_003A9208, 0);
}

void func_002373D0(void) {
    while (func_002F2890(D_0032EF40) != 0) {
    }
    func_002F26A0(D_0032EF40, 0x258, 0, 0, 0, 0, 0, 0, 0);
    while (func_002F2890(D_0032EF40) != 0) {
    }
}

void func_00237440(void) {
    while (func_002F2890(D_0032EF40) != 0) {
    }
    func_002F26A0(D_0032EF40, 0x259, 0, 0, 0, 0, 0, 0, 0);
    while (func_002F2890(D_0032EF40) != 0) {
    }
}

int CD_SectorCount(int size) {
    int n;

    n = size / 2048;
    if (size & 0x7FF) {
        n++;
    }
    return n;
}

void func_002374D8(char *s) {
    for (; *s != 0; s++) {
        if ((unsigned char)*s >= 'a' && (unsigned char)*s <= 'z') {
            *s &= ~0x20;
        }
        if (*s == '/') {
            *s = '\\';
        }
    }
}

int func_00237528(int size) {
    int n;

    switch (func_00239D40()) {
    case 0:
        n = size / 2048;
        break;
    case 1:
        n = CD_SectorCount(size);
        break;
    default:
        n = size;
        break;
    }
    return n;
}

void func_00237590(CdDir *d) {
    int i;

    if (d->dirs != 0) {
        for (i = 0; i < d->numDirs; i++) {
            func_00237590(&d->dirs[i]);
        }
        Mem_Free(d->dirs);
        d->dirs = 0;
    }
    d->numDirs = 0;
    if (d->files != 0) {
        Mem_Free(d->files);
        d->files = 0;
    }
    d->numFiles = 0;
}

CdDir *func_00237628(CdDir *d, const char *name, int len) {
    CdDir *r;
    int i;

    r = 0;
    for (i = 0; i < d->numDirs; i++) {
        if (func_00228E40(d->dirs[i].name, name, len) == 0) {
            r = &d->dirs[i];
            break;
        }
    }
    return r;
}

CdDir *func_002376D0(CdDir *d, int lba) {
    CdDir *r;
    int i;

    r = 0;
    for (i = 0; i < d->numDirs; i++) {
        if (d->dirs[i].lba == lba) {
            r = &d->dirs[i];
            break;
        }
    }
    return r;
}

CdDir *func_00237730(CdDir *d, int lba) {
    CdDir *r;
    int i;

    if (d->lba == lba) {
        r = d;
    } else {
        r = func_002376D0(d, lba);
        if (r == 0) {
            for (i = 0; i < d->numDirs; i++) {
                r = func_00237730(&d->dirs[i], lba);
                if (r != 0) {
                    break;
                }
            }
        }
    }
    return r;
}

CdFile *func_002377D8(CdDir *d, const char *name, int len) {
    CdFile *r;
    int i;

    r = 0;
    for (i = 0; i < d->numFiles; i++) {
        if (func_00228E40(d->files[i].name, name, len) == 0) {
            r = &d->files[i];
            break;
        }
    }
    return r;
}

/* listagem da árvore (os printf foram compilados fora) */
void func_00237880(CdDir *d, int depth) {
    int i;
    int j;

    for (i = 0; i < depth * 4; i++) {
    }
    for (i = 0; i < d->numFiles; i++) {
        for (j = 0; j < depth * 4; j++) {
        }
    }
    for (i = 0; i < d->numDirs; i++) {
        func_00237880(&d->dirs[i], depth + 1);
    }
}

int func_00237950(CdDir *d, int *size, int *lba, int *sectors) {
    int found;
    int i;

    found = 0;
    for (i = 0; i < d->numFiles; i++) {
        if (func_00237A80(&d->files[i]) == 1) {
            found = 1;
            *size = d->files[i].size;
            *lba = d->files[i].lba;
            *sectors = d->files[i].sectors;
            break;
        }
    }
    if (found == 0) {
        for (i = 0; i < d->numDirs; i++) {
            found = func_00237950(&d->dirs[i], size, lba, sectors);
            if (found == 1) {
                break;
            }
        }
    }
    return found;
}

/* HLADDON.GBX de um disco de expansão começa com "SDJ\r\n" */
#ifdef NON_MATCHING /* alocação de registradores e ordem dos blocos de saída diferem */
int func_00237A80(CdFile *f) {
    int r;
    int ret;

    r = func_00228C48(f->name, "HLADDON.GBX;1");
    if (r == 1) {
        CD_Read(f->lba, 1, D_003A8A00);
        ret = 0;
        if (D_003A8A00[0] == 'S' && D_003A8A00[1] == 'D' && D_003A8A00[2] == 'J' && D_003A8A00[3] == '\r') {
            if (D_003A8A00[4] == '\n') {
                ret = r;
            }
        }
        return ret;
    }
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/CDRead", func_00237A80);
#endif

/* descritor primário (setor 16): registro do diretório raiz */
void func_00237B40(int *lba, int *sectors) {
    CD_Read(16, 1, D_003A8A00);
    *lba = func_00238020(D_003A8A00, 0x9E);
    *sectors = func_00238020(D_003A8A00, 0xA6);
    *sectors = func_00237528(*sectors);
}

void func_00237BB8(int lba, int sectors, int *numFiles, int *numDirs) {
    int files;
    int dirs;
    int off;
    int i;

    files = 0;
    dirs = 0;
    for (i = 0; i < sectors; i++) {
        off = 0;
        CD_Read(lba + i, 1, D_003A8A00);
        for (; D_003A8A00[off] != 0; off += D_003A8A00[off]) {
            if (D_003A8A00[off + 0x20] == 1 && D_003A8A00[off + 0x21] < 2) {
                continue;
            }
            if (D_003A8A00[off + 0x19] & 2) {
                dirs++;
            } else {
                files++;
            }
        }
    }
    *numFiles = files;
    *numDirs = dirs;
}

#ifdef NON_MATCHING /* o contador do último laço usa s4/s0 em vez de s0/s1 */
void func_00237CE0(CdDir *d) {
    int nf;
    int nd;
    int off;
    int lba;
    int size;
    int i;
    int j;
    int k;
    int end;

    nf = 0;
    nd = 0;
    func_00237BB8(d->lba, d->sectors, &d->numFiles, &d->numDirs);
    Mem_PushTag();
    Mem_SetTag(2);
    d->files = (CdFile *)Mem_DbgMalloc(d->numFiles * sizeof(CdFile), "Gbx/CDRead.cpp", 0x2DB);
    d->dirs = (CdDir *)Mem_DbgMalloc(d->numDirs * sizeof(CdDir), "Gbx/CDRead.cpp", 0x2DC);
    Mem_PopTag();
    func_00225860(d->files, 0, d->numFiles * sizeof(CdFile));
    func_00225860(d->dirs, 0, d->numDirs * sizeof(CdDir));
    for (i = 0; i < d->sectors; i++) {
        off = 0;
        CD_Read(d->lba + i, 1, D_003A8A00);
        for (; D_003A8A00[off] != 0; off += D_003A8A00[off]) {
            if (D_003A8A00[off + 0x20] == 1 && D_003A8A00[off + 0x21] < 2) {
                continue;
            }
            lba = func_00238020(D_003A8A00, off + 2);
            size = func_00238020(D_003A8A00, off + 10);
            if (D_003A8A00[off + 0x19] & 2) {
                end = off + 0x20 + D_003A8A00[off + 0x20];
                j = 0;
                for (k = off + 0x21; k <= end; k++) {
                    d->dirs[nd].name[j] = D_003A8A00[k];
                    j++;
                }
                d->dirs[nd].name[j] = 0;
                d->dirs[nd].lba = lba;
                d->dirs[nd].sectors = func_00237528(size);
                nd++;
            } else {
                end = off + 0x20 + D_003A8A00[off + 0x20];
                j = 0;
                for (k = off + 0x21; k <= end; k++) {
                    d->files[nf].name[j] = D_003A8A00[k];
                    j++;
                }
                d->files[nf].name[j] = 0;
                d->files[nf].lba = lba;
                d->files[nf].size = size;
                d->files[nf].sectors = CD_SectorCount(size);
                nf++;
            }
        }
    }
    for (i = 0; i < d->numDirs; i++) {
        func_00237CE0(&d->dirs[i]);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/CDRead", func_00237CE0);
#endif

int func_00238020(unsigned char *buf, int off) {
    buf += off;
    return buf[0] + (buf[1] << 8) + (buf[2] << 16) + (buf[3] << 24);
}

short func_00238050(unsigned char *buf, int off) {
    buf += off;
    return buf[0] + (buf[1] << 8);
}

void func_00238070(void) {
    func_002EF690(D_003A9200);
}

void func_00238098(void) {
    D_00328B24 = 1;
}

void func_002380A8(void) {
    D_00328B24 = 0;
}

/* máquina de estados da bandeja do CD (troca de disco) */
int func_002380B8(void) {
    unsigned long t;
    int state;
    int ret;
    int r;
    int x;

    switch (D_00328B24) {
    case 1:
        r = func_002D4348();
        if (r != 1) {
            ret = 0;
            if (func_002D3938() == 0) {
                state = 1;
                if (r == 0) {
                    state = 3;
                }
            } else {
                state = 2;
            }
        } else {
            ret = 1;
            state = 5;
        }
        break;
    case 2:
        state = 2;
        ret = 0;
        if (func_002D3A98(1) == 0) {
            state = 3;
        }
        break;
    case 3:
        state = 4;
        ret = 0;
        if (func_002D4AD0(0, 0) == 0) {
            state = 1;
        }
        break;
    case 4:
        switch (func_002D4348()) {
        case 1:
            ret = 1;
            t = func_00229B88() + 0x21;
            state = 5;
            while (func_00229B88() < t) {
            }
            break;
        case 0:
            ret = 0;
            state = 4;
            break;
        default:
            ret = 0;
            state = 1;
            break;
        }
        break;
    case 5:
        if (func_002D4000(1) == 2) {
            switch (func_002D42B0()) {
            case 0x12:
                if (func_00239D40() != 0) {
                    func_00239D50(0);
                    x = func_00239D60();
                    func_002D3D18(0);
                    func_002D4658(x);
                }
                ret = 3;
                state = 6;
                break;
            case 0x14:
                if (func_00239D40() != 1) {
                    func_00239D50(1);
                    x = func_00239D60();
                    func_002D3D18(0);
                    func_002D4658(x);
                }
                ret = 3;
                state = 6;
                break;
            default:
                ret = 5;
                state = 3;
                break;
            }
        } else {
            ret = 1;
            if (func_002D4348() != 1) {
                ret = 2;
            }
            state = 5;
        }
        break;
    case 6:
        ret = 3;
        state = 6;
        break;
    default:
        ret = 6;
        state = 0;
        break;
    }
    D_00328B24 = state;
    return ret;
}

/* espera o disco ficar pronto */
void func_002382E8(void) {
    func_00238098();
    while (func_002380B8() != 3) {
    }
    func_002380A8();
}
}
