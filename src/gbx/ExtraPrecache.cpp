/* Gbx: ExtraPrecache.epc (modelos extras e grupos de sequência a carregar por mapa) */

#include "common.h"

extern "C" {
#include "engine/cmd.h"
#include "engine/cvar.h"
#include "engine/client.h"

/* um mapa do .epc: nome e onde começa a lista dele */
struct EpcEntry {
    char *name;
    int offset;
};

int func_00235D28(const char *name, byte **data, int *size);
int func_00228C48(const char *a, const char *b);
int func_00228EE0(char *dst, const char *fmt, ...);
int PF_precache_model_I(char *s);
void R_StudioLoadSeqGroup(void *pmodel, int seqgroup);
void func_00236350(void);

extern int D_00328A24;
extern byte *D_003A80FC;        /* .epc inteiro */
extern int D_003A8100;          /* posição de leitura */
extern int D_003A8104;
extern char **D_003A8108;       /* nomes dos modelos */
extern int D_003A810C;
extern EpcEntry *D_003A8110;
extern cvar_t D_0055A350[2];    /* modelo de cada jogador */
extern void *D_004C2FA0[];      /* modelos precacheados do cliente */

void func_00235F30(void) {
    int size;
    int len;
    int idx;
    int i;

    if (D_00328A24 == 1) {
        func_00236350();
    }
    D_003A8100 = 0;
    func_00235D28("ExtraPrecache.epc", &D_003A80FC, &size);
    D_003A8104 = *(int *)(D_003A80FC + D_003A8100);
    D_003A8100 += 4;
    D_003A8108 = (char **)Mem_DbgMalloc(D_003A8104 * 4, "Gbx/ExtraPrecache.cpp", 0x49);
    for (i = 0; i < D_003A8104; i++) {
        len = *(int *)(D_003A80FC + D_003A8100);
        D_003A8100 += 4;
        D_003A8108[i] = (char *)D_003A80FC + D_003A8100;
        D_003A8100 += len;
    }
    D_003A810C = *(int *)(D_003A80FC + D_003A8100);
    D_003A8100 += 4;
    D_003A8110 = (EpcEntry *)Mem_DbgMalloc(D_003A810C * 8, "Gbx/ExtraPrecache.cpp", 0x58);
    for (i = 0; i < D_003A810C; i++) {
        idx = *(int *)(D_003A80FC + D_003A8100);
        D_003A8100 += 4;
        D_003A8110[i].name = D_003A8108[idx];
        D_003A8110[i].offset = D_003A8100;
        D_003A8100 += 4 + *(int *)(D_003A80FC + D_003A8100) * 12;
    }
    D_00328A24 = 1;
}

#ifdef NON_MATCHING
/* fluxo igual; o original junta as três chamadas de PF_precache_model_I num só ponto (cross-jump) */
void func_00236108(const char *map) {
    char path[0xD0];
    int count;
    int mask;
    int pos;
    int type;
    int model;
    int idx;
    int found;
    int i;
    int j;
    int k;

    for (i = 0; i < D_003A810C; i++) {
        if (func_00228C48(D_003A8110[i].name, map) == 1) {
            pos = D_003A8110[i].offset;
            count = *(int *)(D_003A80FC + pos);
            pos += 4;
            for (j = 0; j < count; j++) {
                idx = -1;
                found = 0;
                type = *(int *)(D_003A80FC + pos);
                pos += 4;
                model = *(int *)(D_003A80FC + pos);
                pos += 4;
                mask = *(int *)(D_003A80FC + pos);
                pos += 4;
                if (type == 0) {
                    found = 1;
                    idx = PF_precache_model_I(D_003A8108[model]);
                } else {
                    func_00228EE0(path, "models/player/%s/%s.dol", D_0055A350[0].string, D_0055A350[0].string);
                    if (func_00228C48(path, D_003A8108[model]) == 1) {
                        found = 1;
                        idx = PF_precache_model_I(D_003A8108[model]);
                    } else if (cl_numlocal >= 2) {
                        func_00228EE0(path, "models/player/%s/%s.dol", D_0055A350[1].string, D_0055A350[1].string);
                        if (func_00228C48(path, D_003A8108[model]) == 1) {
                            found = 1;
                            idx = PF_precache_model_I(D_003A8108[model]);
                        }
                    }
                }
                if (found == 1) {
                    for (k = 1; k < 31; k++) {
                        if ((mask & (1 << k)) == (1 << k)) {
                            R_StudioLoadSeqGroup(D_004C2FA0[idx], k);
                        }
                    }
                }
            }
            return;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/ExtraPrecache", func_00236108);
#endif

void func_00236350(void) {
    Mem_Free(D_003A8108);
    D_003A8108 = 0;
    Mem_Free(D_003A8110);
    D_003A8110 = 0;
    D_00328A24 = 0;
}
}
