/* Gbx: descompactação de arquivos do CD enquanto o IOP lê (dois buffers de 32 KB) */

#include "common.h"
#include "zlib/zlib.h"

/* ee_sema_t */
typedef struct {
    int count;
    int max_count;
    int init_count;
    int wait_threads;
    int attr;
    int option;
} SemaParam;

/* argumentos da leitura por RPC */
typedef struct {
    int lba;
    int sectors;
    unsigned char *buf;
    unsigned char *buf2;
} StreamArgs;

/* argumento das callbacks do IOP */
typedef struct {
    int pad[3];
    int size;
} StreamDone;

int func_002EF660(SemaParam *p);
int func_002EF670(int sema);
int func_002EF690(int sema);
int func_002EF6A0(int sema);
void func_002EF8E0(int a);
int func_002F26A0(void *cd, int fno, int mode, void *send, int ssize, void *recv, int rsize, void *end, void *endparam);
int func_002F2890(void *cd);
void *func_00225D48(int size, int align, const char *file, int line);
void func_00225DB8(void *buf);
void *Mem_DbgMalloc(int size, const char *file, int line);
void Mem_Free(void *ptr);
void *Hunk_AllocName(int size, const char *name);
int func_00237128(const char *name, int *size, int *lba, int *sectors);
void func_00238FC8(int a, void (*f)(StreamDone *), int b);
void func_00238F50(void);
void func_00239030(void);
void func_00239170(void);
void *func_00238418(int lba, int sectors, unsigned int mode, int size, int *loaded, void *buf);
void func_00238770(StreamDone *d);
void func_002387A0(StreamDone *d);

extern char D_0032EF40[];       /* cliente RPC do IOP */
extern StreamArgs D_003A9240;
extern unsigned char *D_003A9250;
extern int D_003A9254;
extern int D_003A9258;
extern int D_003A9260;
extern int D_003A9268;

void func_00238328(void) {
    func_00238F50();
    func_00239030();
    func_00239170();
}

void func_00238358(void) {
    func_00238FC8(0, func_00238770, 0);
    func_00238FC8(1, func_002387A0, 0);
}

void *func_00238398(const char *name, int *loaded, int mode, void *buf) {
    int size;
    int lba;
    int sectors;
    void *ret;

    ret = 0;
    if (func_00237128(name, &size, &lba, &sectors) == 1) {
        ret = func_00238418(lba, sectors, mode, size, loaded, buf);
    }
    return ret;
}

#ifdef NON_MATCHING /* "second" e "mode" trocam s4 e s5 */
void *func_00238418(int lba, int sectors, unsigned int mode, int size, int *loaded, void *buf) {
    SemaParam p;
    z_stream *z;
    void *out;
    int second;
    int left;
    int err;

    out = 0;
    p.max_count = 1;
    p.init_count = 0;
    p.option = 0;
    func_002EF8E0(0);
    D_003A9254 = func_002EF660(&p);
    D_003A9258 = func_002EF660(&p);
    D_003A9250 = func_00225D48(0x10000, 0x40, "Gbx/StreamZip.c", 0x68);
    D_003A9240.lba = lba;
    D_003A9240.sectors = sectors;
    D_003A9240.buf = D_003A9250;
    D_003A9240.buf2 = D_003A9250 + 0x8000;
    D_003A9260 = 0;
    while (func_002F2890(D_0032EF40) != 0) {
    }
    func_002F26A0(D_0032EF40, 100, 0, &D_003A9240, 0x10, 0, 0, 0, 0);
    z = Mem_DbgMalloc(sizeof(z_stream), "Gbx/StreamZip.c", 0x75);
    z->zalloc = 0;
    z->zfree = 0;
    if (inflateInit(z) == Z_OK) {
        func_002EF6A0(D_003A9254);
        second = 0;
        *loaded = *(int *)D_003A9250;
        switch (mode) {
        case 0:
            out = func_00225D48(*loaded, 0x40, "Gbx/StreamZip.c", 0x8D);
            break;
        case 1:
            out = Hunk_AllocName(*loaded, "ZipFile");
            break;
        case 2:
            out = buf;
            break;
        }
        z->avail_in = D_003A9260 - 4;
        z->next_in = D_003A9250 + 4;
        z->avail_out = *loaded;
        z->next_out = out;
        left = size;
        do {
            if (left < z->avail_in) {
                z->avail_in = left;
            }
            left -= z->avail_in;
            err = inflate(z, Z_SYNC_FLUSH);
            if (left > 0 && err == Z_OK) {
                if (!second) {
                    while (func_002F2890(D_0032EF40) != 0) {
                    }
                    func_002F26A0(D_0032EF40, 0x65, 0, 0, 0, 0, 0, 0, 0);
                    second = 1;
                    func_002EF6A0(D_003A9258);
                    z->avail_in = D_003A9268;
                    z->next_in = D_003A9250 + 0x8000;
                } else {
                    while (func_002F2890(D_0032EF40) != 0) {
                    }
                    func_002F26A0(D_0032EF40, 0x66, 0, 0, 0, 0, 0, 0, 0);
                    second = 0;
                    func_002EF6A0(D_003A9254);
                    z->avail_in = D_003A9260;
                    z->next_in = D_003A9250;
                }
            }
        } while (err != Z_STREAM_END && left > 0);
    }
    inflateEnd(z);
    Mem_Free(z);
    func_00225DB8(D_003A9250);
    func_002EF670(D_003A9254);
    func_002EF670(D_003A9258);
    return out;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/StreamZip", func_00238418);
#endif

void func_00238770(StreamDone *d) {
    D_003A9260 = d->size;
    func_002EF690(D_003A9254);
}

void func_002387A0(StreamDone *d) {
    D_003A9268 = d->size;
    func_002EF690(D_003A9258);
}
