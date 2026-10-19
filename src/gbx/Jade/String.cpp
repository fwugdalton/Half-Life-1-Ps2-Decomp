/* Gbx/Jade: funções de string e árvore de memoização */

#include "common.h"
#include <stdarg.h>

/* nó da árvore de strings memoizadas */
typedef struct MemoNode {
    char *str;
    int id;
    struct MemoNode *parent;
    struct MemoNode *lesser;
    struct MemoNode *greater;
} MemoNode;

extern "C" {

void *func_00225BD8(int size, const char *tag);
void Mem_Free(void *p);
char *func_00228D30(const char *s);
int func_00228E00(const char *a, const char *b);
const char *func_00229108(const char *s);
void func_00229268(MemoNode *n);
void func_002292B8(void);
void func_002292F8(void);
void *func_002E3A78(int size);
void func_002E3AA0(void *p);
char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, int n);
char *strcat(char *dst, const char *src);
char *func_002E61B8(char *dst, const char *src, int n); /* strncat */
unsigned int strlen(const char *s);
char *func_002E6080(const char *s, int c);
int strcmp(const char *a, const char *b);
int strncmp(const char *a, const char *b, int n);
int strncasecmp(const char *a, const char *b, int n);
int strcasecmp(const char *a, const char *b);
char *func_002E5B60(const char *s, int c);
char *func_002E66E0(const char *s, int c);
char *strstr(const char *s, const char *sub);
int vsprintf(char *buf, const char *fmt, va_list ap);
int atoi(const char *s);
double atof(const char *s);

extern const unsigned char D_00397D81[]; /* _ctype_ + 1 */
extern MemoNode *D_003281D8;        /* raiz da árvore */
extern int D_003281DC;              /* próximo id */
extern unsigned char D_003281E0;    /* heap do sistema iniciado */

const char *func_00228BB0(const char *s) {
    return func_00229108(s);
}

void func_00228BD0(void) {
    func_00229268(D_003281D8);
    D_003281D8 = 0;
}

int func_00228C00(const char *a, const char *b) {
    int r;

    r = 1;
    if (a != b) {
        if (*a != *b) {
            r = 0;
        } else {
            do {
                a++;
                b++;
                if (*a != *b) {
                    r = 0;
                    break;
                }
            } while (*a);
        }
    }
    return r;
}

static inline int ToLower(char c) {
    int r = c;
    if (D_00397D81[c] & 1) {
        r = c + 0x20;
    }
    return r;
}

int func_00228C48(const char *a, const char *b) {
    int r;
    int ca;
    int cb;

    r = 1;
    if (ToLower(*a) != ToLower(*b)) {
        r = 0;
    } else {
        do {
            a++;
            b++;
            if (ToLower(*a) != ToLower(*b)) {
                r = 0;
                break;
            }
        } while (*a || *b);
    }
    return r;
}

char *func_00228CF0(char *dst, const char *src) {
    return strcpy(dst, src);
}

char *func_00228D10(char *dst, const char *src, int n) {
    return strncpy(dst, src, n);
}

char *func_00228D30(const char *s) {
    char *r;

    r = (char *)func_00225BD8(strlen(s) + 1, "strdupx");
    strcpy(r, s);
    return r;
}

char *func_00228D80(char *dst, const char *src) {
    return strcat(dst, src);
}

char *func_00228DA0(char *dst, const char *src, int n) {
    return func_002E61B8(dst, src, n);
}

int func_00228DC0(const char *s) {
    return strlen(s);
}

char *func_00228DE0(const char *s, int c) {
    return func_002E6080(s, c);
}

int func_00228E00(const char *a, const char *b) {
    return strcmp(a, b);
}

int func_00228E20(const char *a, const char *b, int n) {
    return strncmp(a, b, n);
}

int func_00228E40(const char *a, const char *b, int n) {
    return strncasecmp(a, b, n);
}

int func_00228E60(const char *a, const char *b) {
    return strcasecmp(a, b);
}

char *func_00228E80(const char *s, int c) {
    return func_002E5B60(s, c);
}

char *func_00228EA0(const char *s, int c) {
    return func_002E66E0(s, c);
}

char *func_00228EC0(const char *s, const char *sub) {
    return strstr(s, sub);
}

void func_00228EE0(char *buf, const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
}

char *func_00228F28(const char *a, const char *b) {
    char *r;
    int len;

    len = strlen(a);
    len += strlen(b);
    r = (char *)func_00225BD8(len + 1, "make_concat_string");
    strcpy(r, a);
    strcat(r, b);
    return r;
}

char *func_00228FA0(int n, char *buf, int radix) {
    char tmp[128];
    char *p;
    unsigned long v;
    int neg;
    int d;
    int i;

    if (n < 0) {
        v = n;
        if (radix == 10) {
            v = -n;
        }
        neg = 1;
    } else {
        v = n;
        neg = 0;
    }
    p = tmp;
    do {
        d = v % radix;
        v /= radix;
        *p = d < 10 ? d + '0' : d + 'a' - 10;
        p++;
    } while (v);
    if (radix == 10 && neg) {
        *p++ = '-';
    }
    p--;
    i = 0;
    while (p >= tmp) {
        buf[i++] = *p--;
    }
    buf[i] = 0;
    return buf;
}

int func_002290C0(const char *s) {
    return atoi(s);
}

float func_002290E0(const char *s) {
    return atof(s);
}

const char *func_00229108(const char *s) {
    MemoNode *n;
    MemoNode *cur;
    const char *r;
    int c;

    if (D_003281D8 == 0) {
        n = (MemoNode *)func_00225BD8(sizeof(MemoNode), "AddStringToMemoizationTree (gRoot)");
        r = n->str = func_00228D30(s);
        n->id = D_003281DC++;
        n->greater = 0;
        n->lesser = 0;
        n->parent = 0;
        D_003281D8 = n;
    } else {
        cur = D_003281D8;
        for (;;) {
            c = func_00228E00(cur->str, s);
            if (c < 0) {
                if (cur->lesser == 0) {
                    n = (MemoNode *)func_00225BD8(sizeof(MemoNode), "AddStringToMemoizationTree (lesser)");
                    n->str = func_00228D30(s);
                    n->id = D_003281DC++;
                    n->greater = 0;
                    n->lesser = 0;
                    n->parent = cur;
                    cur->lesser = n;
                    r = n->str;
                    break;
                }
                cur = cur->lesser;
            } else if (c > 0) {
                if (cur->greater == 0) {
                    n = (MemoNode *)func_00225BD8(sizeof(MemoNode), "AddStringToMemoizationTree (greater)");
                    n->str = func_00228D30(s);
                    n->id = D_003281DC++;
                    n->greater = 0;
                    n->lesser = 0;
                    n->parent = cur;
                    cur->greater = n;
                    r = n->str;
                    break;
                }
                cur = cur->greater;
            } else {
                r = cur->str;
                break;
            }
        }
    }
    return r;
}

void func_00229268(MemoNode *n) {
    if (n) {
        func_00229268(n->lesser);
        n->lesser = 0;
        func_00229268(n->greater);
        n->greater = 0;
        Mem_Free(n->str);
        Mem_Free(n);
    }
}

void func_002292B8(void) {
    if (D_003281E0) {
        func_002292F8();
    }
    D_003281E0 = 1;
}

void func_002292F8(void) {
    if (D_003281E0) {
        D_003281E0 = 0;
    }
}

void *func_00229310(int size) {
    if (!D_003281E0) {
        func_002292B8();
    }
    return func_002E3A78(size);
}

void func_00229350(void *p) {
    if (D_003281E0) {
        func_002E3AA0(p);
    }
}

}
