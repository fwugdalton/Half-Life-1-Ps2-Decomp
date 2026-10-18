/* Gbx: PAKS\SYSTEM.PAK, carregado inteiro na memória */

#include "common.h"

extern "C" {
#include "engine/decals.h"

void Pak_Init(pakfile_t *pak);
void Pak_Load(pakfile_t *pak, const char *name);
int func_00235280(pakfile_t *pak, const char *name);

extern int D_00328A20;
extern pakfile_t D_003A80E0;

void func_00235E48(void) {
    Pak_Init(&D_003A80E0);
    D_003A80E0.inMemory = 1;
    Pak_Load(&D_003A80E0, "PAKS\\SYSTEM.PAK;1");
    D_00328A20 = 1;
}

void func_00235EA0(const char *name, char **data, int *size) {
    int i;

    i = func_00235280(&D_003A80E0, name);
    *data = (char *)D_003A80E0.data + D_003A80E0.lumps[i].filepos;
    *size = D_003A80E0.lumps[i].size;
}

int func_00235F18(void) {
    return D_00328A20 == 1;
}
}
