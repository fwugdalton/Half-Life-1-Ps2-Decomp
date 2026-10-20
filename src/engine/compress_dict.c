/* carga de arquivos .HL1 comprimidos com dicionários (PAKS\DICTS.PAK); nome original desconhecido */

#include "common.h"
#include "engine/decals.h"

void Pak_Open(pakfile_t *pak, const char *name);
void Pak_Close(pakfile_t *pak, const char *name);
int func_00235300(const char *a, paklump_t *lump);
int CD_SectorCount(int size);
void CD_Read(int lba, int sectors, void *buf);
void func_0022B078(const char *name, char *data, int size);
void func_0022B248(const char *name);
int func_0022B6D0(const char *name);
int func_00221470(const char *name, void *stream);
void func_00221768(void *stream);
void func_0021DA50(void *in, void *dict, const char *out);
void func_0021F180(const char *in, void *dict, const char *out);
int sprintf(char *buf, const char *fmt, ...);

int func_00217768(const char *name);
void func_00217890(const char *name);
void func_002178B8(char *s);

extern const char D_00385090[];     /* "PAKS\\DICTS.PAK;1" */
extern const char D_003850A8[];     /* "%s%s.HL1" */
extern const char D_003850B8[];     /* "%s.hl1" */
extern const char D_003850C0[];     /* "%s%s.HL4" */
extern pakfile_t D_003A4AB8;        /* PAKS\DICTS.PAK */
extern char *D_00331AFC;            /* buffer de leitura */
extern int D_00331B04;              /* arquivo do dicionário carregado */

INCLUDE_RODATA("asm/nonmatchings/engine/compress_dict", D_00385090);

INCLUDE_RODATA("asm/nonmatchings/engine/compress_dict", D_003850A8);

INCLUDE_RODATA("asm/nonmatchings/engine/compress_dict", D_003850B8);

INCLUDE_RODATA("asm/nonmatchings/engine/compress_dict", D_003850C0);

void func_00217510(void) {
    Pak_Open(&D_003A4AB8, D_00385090);
}

void func_00217538(void) {
    Pak_Close(&D_003A4AB8, D_00385090);
}

int func_00217560(const char *dir, const char *name) {
    char path[256];
    char dictName[16];
    char outPath[256];
    char dict[0xE0];
    char in[0xE0];
    int r;

    sprintf(path, D_003850A8, dir, name);
    sprintf(dictName, D_003850B8, name);
    sprintf(outPath, D_003850C0, dir, name);
    func_002178B8(path);
    func_002178B8(outPath);
    if (func_00217768(dictName) == 1) {
        r = 1;
        if (func_00221470(dictName, dict) == r) {
            if (func_00221470(path, in) == r) {
                func_0021DA50(in, dict, outPath);
                func_00221768(in);
            }
            func_00221768(dict);
        }
        func_00217890(dictName);
    } else {
        r = 0;
    }
    return r;
}

void func_00217678(const char *dir, const char *name) {
    char path[256];
    char dictName[16];
    char outPath[256];
    char dict[0xE0];
    int r;

    sprintf(path, D_003850A8, dir, name);
    sprintf(dictName, D_003850B8, name);
    sprintf(outPath, D_003850C0, dir, name);
    func_002178B8(path);
    func_002178B8(outPath);
    r = func_0022B6D0(outPath);
    if (r == 1) {
        func_00217768(dictName);
        if (func_00221470(dictName, dict) == r) {
            func_0021F180(outPath, dict, path);
            func_00221768(dict);
        }
        func_00217890(dictName);
    }
}

int func_00217768(const char *name) {
    int found;
    int i;
    int sectors;
    char *buf;

    found = 0;
    for (i = 0; i < D_003A4AB8.count; i++) {
        if (func_00235300(name, &D_003A4AB8.lumps[i]) == 1) {
            found = 1;
            sectors = CD_SectorCount(D_003A4AB8.lumps[i].size);
            buf = D_00331AFC;
            D_00331B04 = 1;
            CD_Read(D_003A4AB8.sector + D_003A4AB8.lumps[i].filepos / 2048, sectors, buf);
            func_0022B078(name, buf, sectors << 11);
            break;
        }
    }
    return found;
}

void func_00217890(const char *name) {
    func_0022B248(name);
    D_00331B04 = 0;
}

void func_002178B8(char *s) {
    while (*s) {
        if (*s == '\\') {
            *s = '/';
        }
        s++;
    }
}
