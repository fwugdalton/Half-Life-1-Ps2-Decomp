/* Gbx: arquivos .PAK do CD (diretório de lumps de 64 bytes) e largura de texto */

#include "common.h"

extern "C" {
#include "engine/decals.h"
#include "engine/cmd.h"

/* cabeçalho de um .PAK (dpackheader_t) */
struct PakHeader {
    char id[4];
    int dirofs;
    unsigned int dirlen;
};

/* um caractere da fonte (4 bytes) */
struct FontChar {
    short x;
    short width;
};

struct Font {
    char pad[0xC];
    int height;
    FontChar chars[1];
};

void *func_00225D48(int size, int align, const char *file, int line);
void func_00225DB8(void *buf);
int func_00237128(const char *name, int *size, int *lba, int *sectors);
void *func_00238418(int lba, int sectors, int mode, int size, int *loaded, void *buf);
void CD_Read(int lba, int sectors, void *buf);
int CD_SectorCount(int size);
void Pak_Open(pakfile_t *pak, const char *name);
void func_002351E0(pakfile_t *pak);
int func_00235280(pakfile_t *pak, const char *name);
int func_00235300(const char *a, paklump_t *lump);
void func_002353C0(pakfile_t *pak, const char *name);

int func_00228C48(const char *a, const char *b);
void func_00228CF0(char *dst, const char *src);
int func_00228E60(const char *a, const char *b);
void func_00114E58(void);
void func_0022EC50(void);
void func_00235A50(void);
void func_00235AA0(const char *game);
void func_00235B08(void);
void func_00235BA0(void);
void func_00235F30(void);
void func_00237440(void);
void func_0028BD90(void);
void func_002BEF98(void);
void func_002CA018(void);
void func_002CF6F8(const char *game);
void func_002CF7B8(void);
void func_00235500(void);
void func_00235610(void);
void func_00235668(void);

extern float D_00328A00;
extern float D_00328A04;
extern const char _ctype_[];
extern char D_00328A08[8];      /* jogo carregado */
extern char D_00328A10[8];      /* jogo pedido */

int func_00234ED8(Font *f, int c) {
    return (int)(f->chars[c].width * D_00328A00 + 1.4f);
}

int func_00234F20(Font *f) {
    return (int)(f->height * D_00328A04 + 0.9f);
}

int func_00234F58(Font *f, const char *s) {
    int w;
    const char *p;

    if (s != 0) {
        w = 0;
        for (p = s; *p != 0; p++) {
            w += func_00234ED8(f, *p);
        }
    } else {
        w = 0;
    }
    return w;
}

void Pak_Init(pakfile_t *pak) {
    pak->count = 0;
    pak->lumps = 0;
    pak->sector = 0;
    pak->unk_0C = 0;
    pak->unk_10 = 0;
    pak->data = 0;
    pak->inMemory = 0;
}

void Pak_Load(pakfile_t *pak, const char *name) {
    PakHeader *h;

    func_002353C0(pak, name);
    h = (PakHeader *)pak->data;
    pak->lumps = (paklump_t *)((char *)h + h->dirofs);
    pak->count = h->dirlen >> 6;
}

void func_00235038(pakfile_t *pak, byte *data, int size) {
    pak->unk_10 = size;
    pak->inMemory = 3;
    pak->data = data;
    pak->lumps = (paklump_t *)(data + ((PakHeader *)data)->dirofs);
    pak->count = ((PakHeader *)data)->dirlen >> 6;
}

void Pak_Open(pakfile_t *pak, const char *name) {
    PakHeader *h;
    int lba;
    int n;

    h = (PakHeader *)func_00225D48(0x800, 0x40, "Gbx/HLPak.cpp", 0x85);
    func_00237128(name, &pak->unk_10, &pak->sector, &pak->unk_0C);
    CD_Read(pak->sector, 1, h);
    lba = pak->sector + h->dirofs / 2048;
    n = CD_SectorCount(h->dirlen);
    pak->count = h->dirlen >> 6;
    pak->lumps = (paklump_t *)func_00225D48(n << 11, 0x40, "Gbx/HLPak.cpp", 0x96);
    CD_Read(lba, n, pak->lumps);
    func_00225DB8(h);
}

void Pak_Close(pakfile_t *pak, const char *name) {
    func_002351E0(pak);
    Pak_Open(pak, name);
}

void func_00235198(pakfile_t *pak) {
    if (pak->inMemory == 1) {
        if (pak->data != 0) {
            func_00225DB8(pak->data);
        }
        pak->data = 0;
    }
}

void func_002351E0(pakfile_t *pak) {
    if (pak->lumps != 0) {
        func_00225DB8(pak->lumps);
    }
    pak->lumps = 0;
}

void func_00235218(pakfile_t *pak, const char *name, char **data, int *size) {
    int i;

    i = func_00235280(pak, name);
    *data = (char *)pak->data + pak->lumps[i].filepos;
    *size = pak->lumps[i].size;
}

int func_00235280(pakfile_t *pak, const char *name) {
    int i;
    int ret;

    ret = -1;
    for (i = 0; i < pak->count; i++) {
        if (func_00235300(name, &pak->lumps[i]) == 1) {
            ret = i;
            break;
        }
    }
    return ret;
}

#ifdef NON_MATCHING
/* fluxo igual; registradores (o original usa um temporário a mais) e um movn no lugar de movz */
int func_00235300(const char *a, paklump_t *lump) {
    const char *b;
    int c1;
    int c2;

    b = lump->name;
    do {
        if (!(*a == '/' && *b == '\\')) {
            c1 = *a;
            c2 = *b;
            if ((_ctype_ + 1)[c1] & 1) {
                c1 += 0x20;
            }
            if ((_ctype_ + 1)[c2] & 1) {
                c2 += 0x20;
            }
            if (c1 != c2) {
                return 0;
            }
            if (*a == 0) {
                return 1;
            }
        }
    } while (*a++ != 0 && *b++ != 0);
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/HLPak", func_00235300);
#endif

void func_002353A0(int *n) {
    int i;

    for (i = 0; i < *n; i++) {
    }
}

void func_002353C0(pakfile_t *pak, const char *name) {
    int size;

    func_00237128(name, &size, &pak->sector, &pak->unk_0C);
    switch (pak->inMemory) {
    case 1:
        pak->data = (byte *)func_00238418(pak->sector, pak->unk_0C, 0, size, &pak->unk_10, 0);
        break;
    case 2:
        pak->data = (byte *)func_00238418(pak->sector, pak->unk_0C, 1, size, &pak->unk_10, 0);
        break;
    case 3:
        pak->data = (byte *)func_00238418(pak->sector, pak->unk_0C, 2, size, &pak->unk_10, pak->data);
        break;
    }
}

void func_00235490(void) {
    func_00228CF0(D_00328A08, "");
    func_00228CF0(D_00328A10, "valve");
    func_00235A50();
}

void func_002354D8(const char *game) {
    func_00228CF0(D_00328A10, game);
}

/* troca de jogo (valve ou um mod): recarrega os paks e o que depende deles */
void func_00235500(void) {
    if (func_00228C48(D_00328A08, D_00328A10) != 0) {
        return;
    }
    func_00237440();
    func_002BEF98();
    func_002CF7B8();
    if (func_00228E60(D_00328A10, "VALVE") != 0) {
        func_002CF6F8(D_00328A10);
    }
    func_00114E58();
    func_002CA018();
    func_0028BD90();
    func_00235B08();
    func_00235AA0(D_00328A10);
    func_00235BA0();
    func_00235F30();
    func_0022EC50();
    func_00228CF0(D_00328A08, D_00328A10);
}

void func_002355D0(void) {
    Cmd_AddCommand("setgame", func_00235610);
    Cmd_AddCommand("syncgame", func_00235668);
}

void func_00235610(void) {
    if (Cmd_Argc() == 2) {
        func_002354D8(Cmd_Argv(1));
    } else {
        Con_Printf("Loaded game: [%s], Current game [%s]\n", D_00328A08, D_00328A10);
    }
}

void func_00235668(void) {
    func_00235500();
}
}
