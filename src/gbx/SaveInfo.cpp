/* Gbx: cabeçalho dos saves (data, hora, capítulo) e estado do memory card */

#include "common.h"

extern "C" {
#include "engine/progdefs.h"

/* relógio do CD (sceCdCLOCK), campos em BCD */
struct CdClock {
    unsigned char stat;
    unsigned char second;
    unsigned char minute;
    unsigned char hour;
    unsigned char pad;
    unsigned char day;
    unsigned char month;
    unsigned char year;
};

/* cabeçalho de 0x5C bytes no início de cada save */
struct GameSaveHeader {
    int year;
    char month;
    char day;
    char hour;
    char minute;
    int chapter;
    char map[0x50];
};

struct GameSaveEntry {
    int used;
    int size;
    int group;
    char unk0C[0x20];
    char dir[0x44];
    GameSaveHeader hdr;
    char name[0x50];
};

struct GameSaveState {
    int card;
    int status;
    int numSaves;
    int unk0C;
    GameSaveEntry *saves;
};

/* tokenizador do motor (0x20 bytes) */
struct Tokenizer {
    char pad[0x20];
};

int func_002D47D8(CdClock *clk);
void func_002D7110(CdClock *clk);
int func_002D6B10(void);
int func_002D6A10(void);
int func_00235D28(const char *name, char **buf, int *len);
void func_00108008(Tokenizer *t);
void func_00108018(Tokenizer *t, int flags);
void func_00108070(Tokenizer *t);
void func_00108120(Tokenizer *t, char *buf, int len);
char *func_00108148(Tokenizer *t, int a, int b);
int func_00228C48(const char *a, const char *b);
void func_00228CF0(char *dst, const char *src);
int func_00228EE0(char *dst, const char *fmt, ...);
int func_002290C0(const char *s);
int sprintf(char *dst, const char *fmt, ...);
void qsort(void *base, int n, int size, int (*cmp)(const void *, const void *));
void func_0022C510(int card);
int func_0022C598(int card);
int func_0022C668(int card);
int func_0022C680(int card);
int func_0022CF08(int card, const char *pattern, int *handle, char *name);
void func_0022D260(int handle);
int func_0022D688(int card);
int func_00230C00(GameSaveEntry *a, GameSaveEntry *b);
int func_00230E38(int card);
int func_00230F48(int a, int b);

extern char *pr_strings;
extern unsigned int D_00328310;
extern char D_00328250[16];
extern int D_003A771C;

#define BCD(x) (((x) >> 4) * 10 + ((x) & 0xF))

void func_002307D8(GameSaveHeader *h) {
    CdClock clk;
    Tokenizer tok;
    char *buf;
    int len;
    char *mapname;
    char *tk;

    if (func_002D47D8(&clk) == 1) {
        func_002D7110(&clk);
        h->year = BCD(clk.year) + 2000;
        h->month = BCD(clk.month);
        h->day = BCD(clk.day);
        h->hour = BCD(clk.hour);
        h->minute = BCD(clk.minute);
    } else {
        h->year = 0;
        h->month = 0;
        h->day = 0;
        h->hour = 0;
        h->minute = 0;
    }
    mapname = pr_strings + gGlobalVariables.mapname;
    if (func_00235D28("MapToChapter.txt", &buf, &len) == 1) {
        func_00108008(&tok);
        func_00108070(&tok);
        func_00108120(&tok, buf, -1);
        tk = func_00108148(&tok, 0, 0);
        while (tk != 0) {
            if (func_00228C48(mapname, tk) == 1) {
                func_00228CF0(h->map, func_00108148(&tok, 0, 0));
                tk = func_00108148(&tok, 0, 0);
                h->chapter = func_002290C0(tk);
                break;
            }
            func_00108148(&tok, 0, 0);
            func_00108148(&tok, 0, 0);
            tk = func_00108148(&tok, 0, 0);
        }
        if (tk == 0) {
            func_00228CF0(h->map, mapname);
        }
        func_00108018(&tok, 2);
    } else {
        func_00228CF0(h->map, mapname);
    }
}

void func_002309F0(GameSaveHeader *h, char *out) {
    char timebuf[0x20];
    char datebuf[0x20];
    char ampm[0x10];
    char hour;

    switch (func_002D6B10()) {
    case 0:
        func_00228EE0(timebuf, "%02d:%02d", h->hour, h->minute);
        break;
    case 1:
        if (h->hour >= 12) {
            if (h->hour < 13) {
                hour = 12;
            } else {
                hour = h->hour - 12;
            }
            func_00228CF0(ampm, "pm");
        } else {
            hour = 12;
            if (h->hour != 0) {
                hour = h->hour;
            }
            func_00228CF0(ampm, "am");
        }
        func_00228EE0(timebuf, "%2d:%02d %s", hour, h->minute, ampm);
        break;
    default:
        sprintf(timebuf, "Unknown time");
        break;
    }
    switch (func_002D6A10()) {
    case 0:
        sprintf(datebuf, "%d/%02d/%02d", h->year, h->month, h->day);
        break;
    case 1:
        sprintf(datebuf, "%02d/%02d/%d", h->month, h->day, h->year);
        break;
    case 2:
        sprintf(datebuf, "%02d/%02d/%d", h->day, h->month, h->year);
        break;
    default:
        sprintf(datebuf, "Unknown date");
        break;
    }
    func_00228EE0(out, "%s %s - %s", datebuf, timebuf, h->map);
}

void func_00230BD0(GameSaveState *s) {
    qsort(s->saves, s->numSaves, sizeof(GameSaveEntry), (int (*)(const void *, const void *))func_00230C00);
}

int func_00230C00(GameSaveEntry *a, GameSaveEntry *b) {
    int y, mo, d, h, mi;

    if (a->hdr.year > b->hdr.year) {
        y = 1;
    } else if (a->hdr.year < b->hdr.year) {
        y = -1;
    } else {
        y = 0;
    }
    if (a->hdr.month > b->hdr.month) {
        mo = 1;
    } else if (a->hdr.month < b->hdr.month) {
        mo = -1;
    } else {
        mo = 0;
    }
    if (a->hdr.day > b->hdr.day) {
        d = 1;
    } else if (a->hdr.day < b->hdr.day) {
        d = -1;
    } else {
        d = 0;
    }
    if (a->hdr.hour > b->hdr.hour) {
        h = 1;
    } else if (a->hdr.hour < b->hdr.hour) {
        h = -1;
    } else {
        h = 0;
    }
    if (a->hdr.minute > b->hdr.minute) {
        mi = 1;
    } else if (a->hdr.minute < b->hdr.minute) {
        mi = -1;
    } else {
        mi = 0;
    }
    return -(y * 10000 + mo * 1000 + d * 100 + h * 10 + mi);
}

void func_00230CD8(void) {
}

#ifdef NON_MATCHING
/* o original guarda o resultado em v1 até o fim e trata o default de outro jeito */
int func_00230CE0(int state) {
    int ret;
    int r;

    if (D_00328310 % 10 == 0) {
        if (state < 7) {
            if (state < 4) {
                ret = 0;
            } else if (func_0022C598(0) == 1 || func_0022C598(1) == 1) {
                ret = 1;
            } else {
                ret = 0;
            }
        } else {
            ret = 0;
        }
    } else {
        ret = 0;
    }
    D_00328310++;
    if (ret != 0) {
        return ret;
    }
    switch (state) {
    case 0:
        return 0;
    case 1:
        return 2;
    case 2:
        r = func_00230E38(0);
        D_003A771C = r;
        if (r == 4 || r == 1) {
            return 3;
        }
        return 8;
    case 3:
        r = func_00230E38(1);
        if (r == 1 || r == 4) {
            return func_00230F48(D_003A771C, r);
        }
        return 8;
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        return state;
    }
    return 0;
}
#else
/* a tabela de saltos fica em 0x387700 (alinhada a 16) */
__asm__(".section .rodata\n.balign 16\n.section .text");
INCLUDE_ASM("asm/nonmatchings/gbx/SaveInfo", func_00230CE0);
#endif

#ifdef NON_MATCHING
/* só a alocação de s3/s4 (contador e constante 3) troca */
int func_00230E38(int card) {
    char pattern[0x50];
    char name[0x20];
    int handle;
    int ret;
    int i;

    for (i = 4; i >= 0; i--) {
        func_0022C510(card);
        if (func_0022D688(card) != 1) {
            ret = 1;
        } else if (func_0022C668(card) != 0) {
            ret = 2;
        } else {
            func_00228EE0(pattern, "%svalve*", D_00328250);
            if (func_0022CF08(card, pattern, &handle, name) == 0 && handle != -1) {
                func_0022D260(handle);
                ret = 3;
            } else {
                ret = 4;
                if (func_0022C680(card) > 0x64000) {
                    ret = 3;
                }
            }
        }
    }
    return ret;
}
#else
INCLUDE_ASM("asm/nonmatchings/gbx/SaveInfo", func_00230E38);
#endif

int func_00230F48(int a, int b) {
    int r;

    switch (a) {
    case 4:
        r = 5;
        break;
    case 1:
        r = 4;
        if (b == 4) r = 6;
        break;
    default:
        r = 4;
        break;
    }
    return r;
}
}
